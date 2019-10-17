/*****************************************************************************
T5sp5tlm.c : SP5 Safe protocol - transport layer - MASTER

DO NOT ALTER THIS !

(c) COPALP 2007
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_SP5

/****************************************************************************/

T5_BOOL T5SP5Tlm_Open (T5PTR_SP5TL pTL, T5_WORD wFlags, T5_PTCHAR szConfig,
                       T5_DWORD dwTimeout, T5_WORD wNbTrial)
{
    T5_MEMSET (pTL, 0, sizeof (T5STR_SP5TL));
    pTL->wFlags = wFlags;
    pTL->bSlice = ((wFlags & SP5TL_SLICE) != 0);
    pTL->bState = SP5TLM_READY;
    return T5SP5Dlm_Open (&(pTL->port), wFlags, szConfig, dwTimeout, wNbTrial,
                          pTL->dataSnd, pTL->dataRcv);
}

void T5SP5Tlm_Close (T5PTR_SP5TL pTL)
{
    T5SP5Dlm_Close (&(pTL->port));
}

T5_BOOL T5SP5Tlm_Send (T5PTR_SP5TL pTL, T5_WORD wUnit, T5_WORD wLen,
                       T5_PTBYTE pData, T5_DWORD dwTimeStamp)
{
    T5STR_SP5H header;
    T5_BOOL bRet;

    if (wLen == 0 || wLen > SP5MAX_FRAME)
        return FALSE;
    if (pTL->bState != SP5TLM_READY)
        return FALSE;

    /* app level message numbering */
    pTL->bMsg += 1;
    if (pTL->bMsg == 0)
        pTL->bMsg = 1;

    /* prepare header */
    T5_MEMSET (&header, 0, sizeof (header));
    T5_COPYFRAMEWORD (header.un, &wUnit);
    header.fk = (pTL->bSlice) ? SP5FK_QS : SP5FK_Q1;
    header.mn = pTL->bMsg;

    if (!pTL->bSlice || wLen < SP5_PAKSIZE)
    {
        /* send single frame */
        T5_COPYFRAMEWORD (header.ds, &wLen);
        if (pData != NULL)
            T5_MEMCPY (pTL->dataSnd, pData, wLen);
        else
            T5_MEMCPY (pTL->dataSnd, pTL->dataApp, wLen);
        pTL->wSize = 0;
        pTL->bPak = 0;
        bRet = T5SP5Dlm_Send (&(pTL->port), &header, dwTimeStamp);
        if (bRet)
            pTL->bState = SP5TLM_WAIT;
    }
    else
    {
        /* slice: send first packet */
        if (pData != NULL)
            T5_MEMCPY (pTL->dataApp, pData, wLen);
        pTL->wSize = wLen;
        wLen = SP5_PAKSIZE;
        T5_MEMCPY (pTL->dataSnd, pTL->dataApp, wLen);
        T5_COPYFRAMEWORD (header.ds, &wLen);
        pTL->bPak = 1;
        header.pn = 1;
        bRet = T5SP5Dlm_Send (&(pTL->port), &header, dwTimeStamp);
        if (bRet)
            pTL->bState = SP5TLM_SENDING;
    }

    /* sent fail */
    if (!bRet)
    {
        pTL->wSize = 0;
        pTL->bPak = 0;
    }
    return bRet;
}

void T5SP5Tlm_AbortRequest (T5PTR_SP5TL pTL)
{
    T5SP5Dlm_AbortRequest (&(pTL->port));
}

T5_WORD T5SP5Tlm_GetAnswer (T5PTR_SP5TL pTL, T5_WORD wMaxLen, T5_PTBYTE pData,
                            T5_PTWORD pwUnit, T5_DWORD dwTimeStamp,
                            T5_PTBOOL pbOK)
{
    T5STR_SP5H header;
    T5_WORD wSize, wOffset;

    /* anything happens? - check frame*/
    if (!T5SP5Dlm_GetAnswer (&(pTL->port), &header, dwTimeStamp, pbOK))
        return 0;
    if (pwUnit != NULL)
        T5_COPYFRAMEWORD (pwUnit, header.un);
    if (*pbOK && header.fk == SP5FK_SL)
        *pbOK = FALSE;
    if (*pbOK == FALSE)
    {
        pTL->bState = SP5TLM_READY;
        return 0;
    }

    /* answer here */
    T5_COPYFRAMEWORD (&wSize, header.ds);
    switch (pTL->bState)
    {
    case SP5TLM_SENDING :
        /* slice for question */
        header.fk = (pTL->bSlice) ? SP5FK_QS : SP5FK_Q1;
        header.mn = pTL->bMsg;
        wOffset = ((T5_WORD)pTL->bPak) * SP5_PAKSIZE;
        wSize = pTL->wSize - wOffset;
        pTL->bPak += 1;
        header.pn = pTL->bPak;
        if (wSize > SP5_PAKSIZE)
        {
            /* send one more packet */
            wSize = SP5_PAKSIZE;
            T5_COPYFRAMEWORD (header.ds, &wSize);
            T5_MEMCPY (pTL->dataSnd, pTL->dataApp + wOffset, wSize);
        }
        else
        {
            /* send last packet */
            T5_COPYFRAMEWORD (header.ds, &wSize);
            T5_MEMCPY (pTL->dataSnd, pTL->dataApp + wOffset, wSize);
            header.pn |= SP5PAK_LAST;
            pTL->bPak = 0;
            pTL->wSize = 0;
            pTL->bState = SP5TLM_WAIT;
        }
        if (!T5SP5Dlm_Send (&(pTL->port), &header, dwTimeStamp))
        {
            pTL->bPak = 0;
            pTL->wSize = 0;
            pTL->bState = SP5TLM_READY;
            *pbOK = FALSE;
        }
        break;
    case SP5TLM_WAIT :
        /* begin a sliced answer? */
        if (pTL->bPak == 0 && header.pn == 1)
        {
            pTL->bPak = 1;
            pTL->wSize = 0;
        }
        /* single packet answer */
        if (header.pn == 0)
        {
            if (wSize > wMaxLen)
                wSize = wMaxLen;
            if (wSize)
                T5_MEMCPY (pData, pTL->dataRcv, wSize);
            pTL->bState = SP5TLM_READY;
            return wSize;
        }
        /* check answer slicing */
        else if ((wSize + pTL->wSize) > SP5MAX_FRAME
            || (header.pn & ~SP5PAK_LAST) != pTL->bPak)
        {
            pTL->bPak = 0;
            pTL->wSize = 0;
            pTL->bState = SP5TLM_READY;
            *pbOK = FALSE;
        }
        /* sliced answer */
        else
        {
            T5_MEMCPY (pTL->dataApp + pTL->wSize, pTL->dataRcv, wSize);
            pTL->wSize += wSize;
            wSize = pTL->wSize;
            if (header.pn & SP5PAK_LAST)
            {
                /* last packet: answer complete */
                if (wSize > wMaxLen)
                    wSize = wMaxLen;
                if (wSize)
                    T5_MEMCPY (pData, pTL->dataApp, wSize);
                pTL->bPak = 0;
                pTL->wSize = 0;
                pTL->bState = SP5TLM_READY;
                return wSize;
            }
            else
            {
                /* ask for next packet */
                pTL->bPak += 1;
                header.fk = SP5FK_N;
                header.ds[0] = header.ds[1] = 0;
                header.mn = pTL->bMsg;
                header.pn = pTL->bPak;
                if (!T5SP5Dlm_Send (&(pTL->port), &header, dwTimeStamp))
                {
                    pTL->bPak = 0;
                    pTL->wSize = 0;
                    pTL->bState = SP5TLM_READY;
                    *pbOK = FALSE;
                }
            }
        }
        break;
    default : break;
    }
    return 0;
}

/****************************************************************************/

#endif /*T5DEF_SP5*/

/* eof **********************************************************************/
