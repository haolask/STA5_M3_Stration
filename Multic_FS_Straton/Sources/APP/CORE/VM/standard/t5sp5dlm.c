/*****************************************************************************
T5sp5dlm.c : SP5 Safe protocol - data link layer - MASTER

DO NOT ALTER THIS !

(c) COPALP 2007
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_SP5

/****************************************************************************/

static T5_BOOL _T5SP5Dlm_Timeout (T5PTR_SP5DL pDL, T5_DWORD dwTimeStamp);
static T5_BOOL _T5SP5Dlm_ReSend (T5PTR_SP5DL pDL, T5_DWORD dwTimeStamp);
static T5_BOOL _T5SP5Dlm_CheckAnswer (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader,
                                      T5_DWORD dwTimeStamp, T5_PTBOOL pbOK);

/****************************************************************************/

T5_BOOL T5SP5Dlm_Open (T5PTR_SP5DL pDL, T5_WORD wFlags, T5_PTCHAR szConfig,
                       T5_DWORD dwTimeout, T5_WORD wNbTrial,
                       T5_PTBYTE pDataSnd, T5_PTBYTE pDataRcv)
{
    T5_MEMSET (pDL, 0, sizeof (T5STR_SP5DL));
    pDL->wFlags = (wFlags | SP5DL_MASTER) & ~SP5DL_SLAVE;
    pDL->pDataSnd = pDataSnd;
    pDL->pDataRcv = pDataRcv;
    pDL->bState = SP5DLM_READY;
    pDL->bResetRcv = TRUE;
    pDL->bResetSnd = TRUE;
    pDL->dwTimeout = dwTimeout;
    pDL->wNbTrial = wNbTrial;
    return T5SP5Frm_Open (&(pDL->port), szConfig);
}

void T5SP5Dlm_Close (T5PTR_SP5DL pDL)
{
    T5SP5Frm_Close (&(pDL->port));
}

T5_BOOL T5SP5Dlm_Send (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader, T5_DWORD dwTimeStamp)
{
    if (pDL->bState != SP5DLM_READY)
        return FALSE;

    if (pDL->bFN == 0)
        pDL->bFN = (T5_BYTE)dwTimeStamp;
    else
        pDL->bFN += 1;
    if (pDL->bFN == 0)
        pDL->bFN = 1;

    T5_MEMCPY (&(pDL->hSend), pHeader, sizeof (T5STR_SP5H));
    pDL->hSend.fn = pDL->bFN;

    T5_MEMCPY (&(pDL->hWork), &(pDL->hSend), sizeof (T5STR_SP5H));
    
    pDL->bResetSnd = TRUE;
    pDL->dwReqStart = dwTimeStamp;
    pDL->bState = SP5DLM_SENDING;
    pDL->wTrial = 0;
    return TRUE;
}

void T5SP5Dlm_AbortRequest (T5PTR_SP5DL pDL)
{
    pDL->bState = SP5DLM_READY;
    pDL->bResetRcv = TRUE;
    pDL->bResetSnd = TRUE;
}

T5_BOOL T5SP5Dlm_GetAnswer (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader,
                            T5_DWORD dwTimeStamp, T5_PTBOOL pbOK)
{
    switch (pDL->bState)
    {
    case SP5DLM_SENDING :
        pDL->bResetSnd = T5SP5Frm_Send (&(pDL->port), &(pDL->hWork),
                                        pDL->pDataSnd, pDL->bResetSnd);
        if (pDL->bResetSnd)
            pDL->bState = SP5DLM_WAIT;
        break;
    case SP5DLM_WAIT :
        pDL->bResetRcv = T5SP5Frm_Receive (&(pDL->port), &(pDL->hWork),
                                           pDL->pDataRcv, pDL->bResetRcv);
        if (pDL->bResetRcv)
            return _T5SP5Dlm_CheckAnswer (pDL, pHeader, dwTimeStamp, pbOK);
        else if (_T5SP5Dlm_Timeout (pDL, dwTimeStamp))
        {
            *pbOK = FALSE;
            return TRUE;
        }
        break;
    default : break;
    }
    *pbOK = TRUE;
    return FALSE;
}

static T5_BOOL _T5SP5Dlm_Timeout (T5PTR_SP5DL pDL, T5_DWORD dwTimeStamp)
{
    T5_BOOL bRet;

    if (T5_BOUNDTIME(dwTimeStamp - pDL->dwReqStart) < pDL->dwTimeout)
        return FALSE;

    bRet = !_T5SP5Dlm_ReSend (pDL, dwTimeStamp);
    if (bRet)
    {
        pDL->bState = SP5DLM_READY;
        pDL->nError += 1;
    }
    return bRet;
}

static T5_BOOL _T5SP5Dlm_ReSend (T5PTR_SP5DL pDL, T5_DWORD dwTimeStamp)
{
    pDL->wTrial += 1;
    if (pDL->wTrial >= pDL->wNbTrial)
        return FALSE;

    T5_MEMCPY (&(pDL->hWork), &(pDL->hSend), sizeof (T5STR_SP5H));
    pDL->bResetSnd = TRUE;
    pDL->dwReqStart = dwTimeStamp;
    pDL->bState = SP5DLM_SENDING;
    pDL->nRepeat += 1;
    return TRUE;
}

static T5_BOOL _T5SP5Dlm_CheckAnswer (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader,
                                      T5_DWORD dwTimeStamp, T5_PTBOOL pbOK)
{
    /* bad frame */
    if (!T5SP5Frm_CheckCRC (&(pDL->port), &(pDL->hWork), pDL->pDataRcv)
        || pDL->hWork.fk == SP5FK_FE)
    {
        *pbOK = _T5SP5Dlm_ReSend (pDL, dwTimeStamp);
        if (*pbOK)
        {
            pDL->bState = SP5DLM_READY;
            pDL->nError += 1;
        }
        return (*pbOK == FALSE);
    }

    /* unexpected frame */
    if (pDL->hWork.un[0] != pDL->hSend.un[0]
        || pDL->hWork.un[1] != pDL->hSend.un[1]
        || pDL->hWork.fn != pDL->hSend.fn)
    {
        pDL->nFlush += 1;
        *pbOK = TRUE;
        return FALSE;
    }

    /* OK */
    pDL->nOK += 1;
    T5_MEMCPY (pHeader, &(pDL->hWork), sizeof (T5STR_SP5H));
    pDL->bState = SP5DLM_READY;
    return TRUE;
}

/****************************************************************************/

#endif /*T5DEF_SP5*/

/* eof **********************************************************************/
