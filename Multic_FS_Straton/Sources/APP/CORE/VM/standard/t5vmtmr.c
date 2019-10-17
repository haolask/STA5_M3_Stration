/*****************************************************************************
T5VMtmr.c :  manage active timers

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_ACTIMESUPPORTED

/*****************************************************************************
T5VMTmr_StartTimer
Starts an active timer - generates a log error in case of memory overflow
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the timer
return: TRUE if OK
*****************************************************************************/

T5_BOOL T5VMTmr_StartTimer (T5PTR_DB pDB, T5_PTDWORD pdwTime)
{
    T5_BOOL bRet, bFound;
    T5PTR_DBSTATUS pStatus;
    T5PTR_DB pAcdir;
    T5PTR_DBACTIME pActime;
    T5_WORD i;

    pStatus = T5GET_DBSTATUS(pDB);
    pAcdir = &(pDB[T5TB_ACTIME]);
    pActime = T5GET_DBACTIME(pDB);
    /* search if already in list */
    bFound = FALSE;
    for (i=0; !bFound && i<pStatus->wNbActime; i++)
        if (pActime[i].pdwTime == pdwTime) bFound = TRUE;
    if (bFound)
        bRet = TRUE;
    else if (pStatus->wNbActime >= (pAcdir->wNbItemAlloc-1))
        bRet = FALSE;
    else
    {
        pActime[pStatus->wNbActime].pdwTime = pdwTime;
        pActime[pStatus->wNbActime].dwLastUpdate = pStatus->dwTimStamp;
        pStatus->wNbActime += 1;
        bRet = TRUE;
    }
    return bRet;
}

/*****************************************************************************
T5VMTmr_StopTimer
Stops an active timer
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the timer
*****************************************************************************/

void T5VMTmr_StopTimer (T5PTR_DB pDB, T5_PTDWORD pdwTime)
{
    T5_BOOL bFound;
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBACTIME pActime;
    T5_WORD i;

    pStatus = T5GET_DBSTATUS(pDB);
    pActime = T5GET_DBACTIME(pDB);
    /* search if already in list */
    bFound = FALSE;
    for (i=0; i<pStatus->wNbActime; i++)
    {
        if (pActime[i].pdwTime == pdwTime) bFound = TRUE;
        if (bFound) T5_MEMCPY (&(pActime[i]), &(pActime[i+1]),
                    sizeof (T5STR_DBACTIME));
    }
    if (bFound)
    {
        pStatus->wNbActime -= 1;
        T5_MEMSET (&(pActime[i]), 0, sizeof (T5STR_DBACTIME));
    }
}

/*****************************************************************************
T5VMTmr_UpdateTimers
Update alltimers
Parameters:
    pDB (IN) pointer to the data base
*****************************************************************************/

void T5VMTmr_UpdateTimers (T5PTR_DB pDB)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBACTIME pActime;
    T5_WORD i;

    pStatus = T5GET_DBSTATUS(pDB);
    pActime = T5GET_DBACTIME(pDB);
    /* search if already in list */
    for (i=0; i<pStatus->wNbActime; i++, pActime++)
    {
        if (pActime->pdwTime != NULL
            && pActime->dwLastUpdate != pStatus->dwTimStamp)
        {
            *(pActime->pdwTime) = 
                ((T5_24HMS + *(pActime->pdwTime) + 
                    pStatus->dwTimStamp - pActime->dwLastUpdate)
                    % T5_24HMS);
            pActime->dwLastUpdate = pStatus->dwTimStamp;
        }
    }
}

#endif /*T5DEF_ACTIMESUPPORTED*/

/*****************************************************************************
T5VMTmr_HotRestartTimers
Update all active timers during a hat restart
Parameters:
    pDB (IN) pointer to the data base
*****************************************************************************/

void T5VMTmr_HotRestartTimers (T5PTR_DB pDB)
{
    T5PTR_DBSTATUS pStatus;
#ifdef T5DEF_SFC
    T5_WORD i;
    static T5PTR_DBSTEP pStep;
#endif /*T5DEF_SFC*/

    pStatus = T5GET_DBSTATUS(pDB);
    pStatus->wNbActime = 0;
    /* update active steps */
#ifdef T5DEF_SFC
    pStep = T5GET_DBSTEP(pDB);
    for (i=0; i<pDB[T5TB_STEP].wNbItemAlloc; i++)
    {
        if (pStep[i].wStatus == T5STEP_N)
            pStep[i].dwLastUpdate = pStatus->dwTimStamp;
    }
#endif /*T5DEF_SFC*/
}

/* eof **********************************************************************/
