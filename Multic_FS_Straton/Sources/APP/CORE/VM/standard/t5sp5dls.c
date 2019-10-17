/*****************************************************************************
T5sp5dls.c : SP5 Safe protocol - data link layer - SLAVE

DO NOT ALTER THIS !

(c) COPALP 2007
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_SP5

/****************************************************************************/

static T5_BOOL _T5SP5Dls_CheckFrame (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader);

/****************************************************************************/

T5_BOOL T5SP5Dls_Open (T5PTR_SP5DL pDL, T5_WORD wFlags, T5_WORD wUnit,
                       T5_PTCHAR szConfig, T5_PTBYTE pDataSnd, T5_PTBYTE pDataRcv)
{
    T5_MEMSET (pDL, 0, sizeof (T5STR_SP5DL));
    pDL->wFlags = (wFlags | SP5DL_SLAVE) & ~SP5DL_MASTER;
    pDL->wUnit = wUnit;
    pDL->pDataSnd = pDataSnd;
    pDL->pDataRcv = pDataRcv;
    pDL->bState = SP5DLS_LISTEN;
    pDL->bResetRcv = TRUE;
    pDL->bResetSnd = TRUE;
    return T5SP5Frm_Open (&(pDL->port), szConfig);
}

void T5SP5Dls_Close (T5PTR_SP5DL pDL)
{
    T5SP5Frm_Close (&(pDL->port));
}

void T5SP5Dls_DontSend (T5PTR_SP5DL pDL)
{
    if (pDL->bState == SP5DLS_WAITANS)
        pDL->bState = SP5DLS_LISTEN;
}

T5_BOOL T5SP5Dls_Send (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader)
{
    if (pDL->bState != SP5DLS_WAITANS)
        return FALSE;

    T5_MEMCPY (&(pDL->hSend), pHeader, sizeof (T5STR_SP5H));
    T5_MEMCPY (&(pDL->hWork), pHeader, sizeof (T5STR_SP5H));
    pDL->hSend.fn = pDL->hWork.fn = pDL->bFN;
    pDL->bState = SP5DLS_ANSWER;
    return TRUE;
}

T5_BOOL T5SP5Dls_Listen (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader)
{
    switch (pDL->bState)
    {
    case SP5DLS_LISTEN :
        pDL->bResetRcv = T5SP5Frm_Receive (&(pDL->port), &(pDL->hWork),
                                           pDL->pDataRcv, pDL->bResetRcv);
        if (pDL->bResetRcv && _T5SP5Dls_CheckFrame (pDL, pHeader))
        {
            pDL->bState = SP5DLS_WAITANS;
            return TRUE;
        }
        break;
    case SP5DLS_ANSWER :
    case SP5DLS_SERVICE :
        pDL->bResetSnd = T5SP5Frm_Send (&(pDL->port), &(pDL->hWork),
                                        pDL->pDataSnd, pDL->bResetSnd);
        if (pDL->bResetSnd)
            pDL->bState = SP5DLS_LISTEN;
        break;
    case SP5DLS_WAITANS :
    default : break;
    }
    return FALSE;
}

static T5_BOOL _T5SP5Dls_CheckFrame (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader)
{
    T5_WORD wUnit;

    /* flush frames for others */
    T5_COPYFRAMEWORD (&wUnit, &(pDL->hWork.un));
    if ((SP5_ISCAST(wUnit) && (pDL->wFlags & SP5DL_CANCAST) == 0)
        || wUnit != pDL->wUnit
        || (pDL->hWork.fk != SP5FK_Q1 && pDL->hWork.fk != SP5FK_QS
            && pDL->hWork.fk != SP5FK_N))
    {
        pDL->nFlush += 1;

#ifdef T5DEF_SERIALINFO
        T5Serial_Info (&(pDL->port.port), T5SERINFO_UNEXPRCV);
#endif /*T5DEF_SERIALINFO*/

        return FALSE;
    }

    /* check frame */
    if (!T5SP5Frm_CheckCRC (&(pDL->port), &(pDL->hWork), pDL->pDataRcv))
    {
        pDL->nError += 1;
        pDL->hWork.fk = SP5FK_FE;
        pDL->hWork.ds[0] = pDL->hWork.ds[1] = 0;
        pDL->bState = SP5DLS_SERVICE;
        return FALSE;
    }

    /* repeat */
    if (pDL->bFN != 0 && pDL->bFN == pDL->hWork.fn)
    {
        pDL->nRepeat += 1;
        T5_MEMCPY (&(pDL->hWork), &(pDL->hSend), sizeof (T5STR_SP5H));
        pDL->bState = SP5DLS_SERVICE;
        return FALSE;
    }

#ifdef T5DEF_SERIALINFO
        T5Serial_Info (&(pDL->port.port), T5SERINFO_RCV);
#endif /*T5DEF_SERIALINFO*/

    /* new valid frame received */
    pDL->nOK += 1;
    pDL->bFN = pDL->hWork.fn;
    T5_MEMCPY (pHeader, &(pDL->hWork), sizeof (T5STR_SP5H));
    return TRUE;
}

/****************************************************************************/

#endif /*T5DEF_SP5*/

/* eof **********************************************************************/
