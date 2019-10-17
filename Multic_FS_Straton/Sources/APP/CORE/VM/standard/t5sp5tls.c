/*****************************************************************************
T5sp5tls.c : SP5 Safe protocol - transport layer - SLAVE

DO NOT ALTER THIS !

(c) COPALP 2007
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_SP5

/****************************************************************************/

T5_BOOL T5SP5Tls_Open (T5PTR_SP5TL pTL, T5_WORD wFlags,
                       T5_WORD wUnit, T5_PTCHAR szConfig)
{
    T5_MEMSET (pTL, 0, sizeof (T5STR_SP5TL));
    pTL->bState = SP5TLS_LISTEN;
    return T5SP5Dls_Open (&(pTL->port), wFlags, wUnit, szConfig,
                          pTL->dataSnd, pTL->dataRcv);
}

void T5SP5Tls_Close (T5PTR_SP5TL pTL)
{
    T5SP5Dls_Close (&(pTL->port));
}

T5_WORD T5SP5Tls_Listen (T5PTR_SP5TL pTL, T5_WORD wMaxLen, T5_PTBYTE pData,
                         T5_PTWORD pwUnit)
{
    T5STR_SP5H header;
    T5_WORD wSize, wOffset;

    /* listen for packet */
    if (!T5SP5Dls_Listen (&(pTL->port), &header))
        return 0;

    /* ask for next packet of an answer */
    if (header.fk == SP5FK_N)
    {
        wOffset = (((T5_WORD)header.pn) - 1) * SP5_PAKSIZE;
        if (header.pn == 0 || header.mn != pTL->bMsg
            || wOffset >= pTL->wSize
            || header.pn != (pTL->bPak + 1))
        {
            header.fk = SP5FK_SL;
            header.ds[0] = header.ds[1] = 0;
            T5SP5Dls_Send (&(pTL->port), &header);
            pTL->bPak = 0;
        }
        else
        {
            wSize = (pTL->wSize) - wOffset;
            if (wSize <= SP5_PAKSIZE)
            {
                header.pn |= SP5PAK_LAST;
                pTL->bPak = 0;
                pTL->wSize = 0;
            }
            else
            {
                wSize = SP5_PAKSIZE;
                pTL->bPak += 1;
                header.pn = pTL->bPak;
            }
            header.fk = SP5FK_A;
            T5_COPYFRAMEWORD (header.ds, &wSize);
            T5_MEMCPY (pTL->dataSnd, pTL->dataApp + wOffset, wSize);
            T5SP5Dls_Send (&(pTL->port), &header);
        }
        return 0;
    }

    /* single packet */
    T5_COPYFRAMEWORD (&wSize, header.ds);
    if (header.pn == 0)
    {
        if (wSize == 0)
            T5SP5Dls_DontSend (&(pTL->port));
        else
            pTL->bState = SP5TLS_WAITANS;
        pTL->bSlice = (header.fk == SP5FK_QS);
        pTL->bPak = 0;
        pTL->wSize = 0;
        pTL->bMsg = header.mn;
        if (wSize > wMaxLen)
            wSize = wMaxLen;
        T5_MEMCPY (pData, pTL->dataRcv, wSize);
        if (pwUnit != NULL)
            T5_COPYFRAMEWORD (pwUnit, header.un);
        return wSize;
    }

    /* packet numbering error or overflow */
    if ((header.pn & ~SP5PAK_LAST) != (pTL->bPak + 1)
        || (pTL->wSize + wSize) > SP5MAX_FRAME)
    {
        header.fk = SP5FK_SL;
        header.ds[0] = header.ds[1] = 0;
        T5SP5Dls_Send (&(pTL->port), &header);
        pTL->bPak = 0;
        pTL->wSize = 0;
        return 0;
    }

    /* append packet to app buffer */
    T5_MEMCPY (pTL->dataApp + pTL->wSize, pTL->dataRcv, wSize);
    pTL->wSize += wSize;

    /* if not last send acknoledge */
    if ((header.pn & SP5PAK_LAST) == 0)
    {
        header.fk = SP5FK_A;
        header.ds[0] = header.ds[1] = 0;
        T5SP5Dls_Send (&(pTL->port), &header);
        pTL->bPak += 1;
        return 0;
    }

    /* last packet received */
    wSize = pTL->wSize;
    if (wSize == 0)
        T5SP5Dls_DontSend (&(pTL->port));
    else
        pTL->bState = SP5TLS_WAITANS;
    pTL->bSlice = (header.fk == SP5FK_QS);
    pTL->bPak = 0;
    pTL->wSize = 0;
    pTL->bMsg = header.mn;
    if (wSize > wMaxLen)
        wSize = wMaxLen;
    T5_MEMCPY (pData, pTL->dataApp, wSize);
    if (pwUnit != NULL)
        T5_COPYFRAMEWORD (pwUnit, header.un);
    return wSize;
}

T5_BOOL T5SP5Tls_Send (T5PTR_SP5TL pTL, T5_WORD wLen, T5_PTBYTE pData)
{
    T5STR_SP5H header;

    if (wLen > SP5MAX_FRAME)
        return FALSE;

    pTL->bState = SP5TLS_LISTEN;

    T5_MEMSET (&header, 0, sizeof (header));
    T5_COPYFRAMEWORD (header.un, &(pTL->port.wUnit));
    header.fk = SP5FK_A;
    header.mn = pTL->bMsg;

    /* no slicing: send all */
    if (!pTL->bSlice || wLen < SP5_PAKSIZE)
    {
        T5_COPYFRAMEWORD (header.ds, &wLen);
        if (pData != NULL)
            T5_MEMCPY (pTL->dataSnd, pData, wLen);
        else
            T5_MEMCPY (pTL->dataSnd, pTL->dataApp, wLen);
        pTL->bPak = 0;
        return T5SP5Dls_Send (&(pTL->port), &header);
    }

    /* remember app data */
    if (pData != NULL)
        T5_MEMCPY (pTL->dataApp, pData, wLen);
    pTL->wSize = wLen;

    /* send first packet */
    wLen = SP5_PAKSIZE;
    T5_COPYFRAMEWORD (header.ds, &wLen);
    T5_MEMCPY (pTL->dataSnd, pTL->dataApp, wLen);
    header.pn = pTL->bPak = 1;
    return T5SP5Dls_Send (&(pTL->port), &header);
}

void T5SP5Tls_DontSend (T5PTR_SP5TL pTL)
{
    pTL->bState = SP5TLS_LISTEN;
    T5SP5Dls_DontSend (&(pTL->port));
}

/****************************************************************************/

#endif /*T5DEF_SP5*/

/* eof **********************************************************************/
