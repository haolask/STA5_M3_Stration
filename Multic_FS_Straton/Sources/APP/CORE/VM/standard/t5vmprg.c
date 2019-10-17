/*****************************************************************************
T5VMprg.c :  execute programs

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/*****************************************************************************
T5VMPrg_ActivateAllPrograms
activates all programs of the cycle
Parameters:
    pDB (IN) pointer to the data base
*****************************************************************************/

void T5VMPrg_ActivateAllPrograms (T5PTR_DB pDB)
{
    T5PTR_DB pTable;
    T5PTR_DBPROG pProg;
    T5_WORD iPrg;

    pTable = &(pDB[T5TB_PROG]);
    pProg = (T5PTR_DBPROG)(pTable->pData);
    for (iPrg=1; iPrg<=pTable->wNbItemUsed; iPrg++)
    {
        switch (pProg[iPrg].wStyle)
        {
        case T5PRGSTYLE_CHILD  : pProg[iPrg].wFlags = 0; break;
        case T5PRGSTYLE_ONCALL : pProg[iPrg].wFlags = 0; break;
        case T5PRGSTYLE_SFC    : pProg[iPrg].wFlags = T5FLAG_PRGTOSTART; break;
        default                : pProg[iPrg].wFlags = T5FLAG_PRGACTIVE; break;
        }
    }
}

/*****************************************************************************
T5VMPrg_ActivateNewPrograms
activates all new programs of the cycle after an On Line Change
Parameters:
    pDB (IN) pointer to the data base
*****************************************************************************/

void T5VMPrg_ActivateNewPrograms (T5PTR_DB pDB, T5_PTR pCode)
{
    T5PTR_DB pTable;
    T5PTR_DBPROG pProg;
    T5_WORD iPrg;
    T5_UNUSED(pCode);

    pTable = &(pDB[T5TB_PROG]);
    pProg = (T5PTR_DBPROG)(pTable->pData);
    for (iPrg=1; iPrg<=pTable->wNbItemUsed; iPrg++)
    {
        switch (pProg[iPrg].wStyle)
        {
        case T5PRGSTYLE_CHILD  :
        case T5PRGSTYLE_SFC :
            break;
        case T5PRGSTYLE_ONCALL :
            pProg[iPrg].wFlags = 0;
            break;
        default :
            pProg[iPrg].wFlags = T5FLAG_PRGACTIVE;
            break;
        }
    }
}

/*****************************************************************************
T5VMPrg_Execute
execute all programs of the cycle including exec buffer
Parameters:
    pDB (IN) pointer to the data base
*****************************************************************************/

void T5VMPrg_Execute (T5PTR_DB pDB)
{
    T5PTR_DB pTable;
    T5PTR_DBPROG pProg;
    T5_WORD iPrg;
    T5_BOOL bOK;
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    /* programs of the cycle - if cycle requested */
    if ((pStatus->wFlags & (T5FLAG_RUN | T5FLAG_ONECYCLE)) != 0)
    {
        pTable = &(pDB[T5TB_PROG]);
        pProg = (T5PTR_DBPROG)(pTable->pData);
        bOK = TRUE;
        for (iPrg=1; bOK && iPrg<=pTable->wNbItemUsed; iPrg++)
        {

#ifdef T5DEFHOOK_PRGEXEC_BEFORE
            T5HOOK_PRGEXEC_BEFORE(pDB, iPrg)
#endif /*T5DEFHOOK_PRGEXEC_BEFORE*/

            if (bOK && ((pStatus->wFlags & (T5FLAG_RUN | T5FLAG_ONECYCLE)) == 0))
                bOK = FALSE;
            else if ((pProg[iPrg].wFlags
                      & (T5FLAG_PRGTOSTART|T5FLAG_PRGACTIVE))!=0
                && (pProg[iPrg].wFlags & T5FLAG_PRGSUSPENDED) == 0)
            {

#ifdef T5DEFHOOK_PRGEXEC_BEFORERUN
                T5HOOK_PRGEXEC_BEFORERUN(pDB, iPrg)
#endif /*T5DEFHOOK_PRGEXEC_BEFORERUN*/

                bOK = T5VMTic_Run (pDB, pStatus,
                                    (T5_PTWORD)(pProg[iPrg].pCode),
                                    pProg[iPrg].dwSize, iPrg);

#ifdef T5DEFHOOK_PRGEXEC_AFTER
                T5HOOK_PRGEXEC_AFTER(pDB, iPrg)
#endif /*T5DEFHOOK_PRGEXEC_AFTER*/

            }
        }
        pStatus->dwCycleCount += 1L;
    }
    pStatus->dwFlagsEx &= ~T5FLAGEX_CHGCYC;
    /* reset single cyle mode if present */
    pStatus->wFlags &= ~T5FLAG_ONECYCLE;
    /* exec buffer from comm */
    if (pStatus->wExecSize != 0)
    {
        /* actual size is one more TIC code (end of sequence) */
        T5VMTic_Run (pDB, pStatus, (T5_PTWORD)(pDB[T5TB_EXEC].pData),
                        pStatus->wExecSize + sizeof (T5_WORD), 0);
        pStatus->wExecSize = 0;
    }
}

/*****************************************************************************
T5VMPrg_Call
execute one program (called from another program)
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the program
Return : TRUE if OK
*****************************************************************************/

T5_BOOL T5VMPrg_Call (T5PTR_DB pDB, T5_WORD wIndex)
{
    T5PTR_DB pTable;
    T5PTR_DBPROG pProg;
    T5PTR_DBSTATUS pStatus;
    T5_BOOL bRet;

    pStatus = T5GET_DBSTATUS(pDB);
    pTable = &(pDB[T5TB_PROG]);
    pProg = (T5PTR_DBPROG)(pTable->pData);
    if (wIndex < 1 || wIndex > pTable->wNbItemUsed)
        return FALSE;

    if ((pProg[wIndex].wFlags & T5FLAG_PRGINEXEC) != 0)
    {
        T5VMLog_Push (pDB, T5RET_RECURSE, wIndex, 0L);
        return FALSE;
    }

    pProg[wIndex].wFlags |= T5FLAG_PRGINEXEC;

#ifdef T5DEFHOOK_PRGEXEC_BEFORE
    T5HOOK_PRGEXEC_BEFORE(pDB, iPrg)
#endif /*T5DEFHOOK_PRGEXEC_BEFORE*/

    if ((pStatus->wFlags & (T5FLAG_RUN | T5FLAG_ONECYCLE)) == 0)
        return FALSE;

#ifdef T5DEFHOOK_PRGEXEC_BEFORERUN
    T5HOOK_PRGEXEC_BEFORERUN(pDB, iPrg)
#endif /*T5DEFHOOK_PRGEXEC_BEFORERUN*/

    bRet = T5VMTic_Run (pDB, pStatus, (T5_PTWORD)(pProg[wIndex].pCode),
                        pProg[wIndex].dwSize, wIndex);

#ifdef T5DEFHOOK_PRGEXEC_AFTER
    T5HOOK_PRGEXEC_AFTER(pDB, iPrg)
#endif /*T5DEFHOOK_PRGEXEC_AFTER*/

    pProg[wIndex].wFlags &= ~T5FLAG_PRGINEXEC;
    return bRet;
}

/*****************************************************************************
T5VMPrg_Task
execute all programs of a task
Parameters:
    pDB (IN) pointer to the data base
    pTask (IN) descriptor of the task
    bMain (IN) TRUE if task is the main cycle (count cycles / handle cc mode)
*****************************************************************************/

void T5VMPrg_Task (T5PTR_DB pDB, T5PTR_DBTASK pTask, T5_BOOL bMain)
{
    T5PTR_DB pTable;
    T5PTR_DBPROG pProg;
    T5_WORD iPrg, iFirst;
    T5_PTWORD pPou;
    T5_BOOL bOK;
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBCALLSTACK pStack;

    pStatus = T5GET_DBSTATUS(pDB);
    pStack = T5GET_DBCALLSTACK(pDB);
    /* programs of the cycle - if cycle requested */
    if (!bMain
        || (pStatus->wFlags & (T5FLAG_RUN | T5FLAG_ONECYCLE)) != 0)
    {
        pTable = &(pDB[T5TB_PROG]);
        pProg = (T5PTR_DBPROG)(pTable->pData);
        bOK = TRUE;
        pPou = pTask->pPou;

        /* start at current position of stepping */
        if (pStatus->wCurPrg != 0)
        {
            if ((pStatus->dwFlagsEx & T5FLAGEX_CCACT) != 0)
            {
                pDB[T5TB_CALLSTACK].wNbItemUsed = 0;
                pProg[pStatus->wCurPrg].dwPos = 0;
                pStatus->wCurPrg = 0;
                pStatus->wCurPos = 0;
                pStatus->wCurTag = 0;
            }
            else
            {
                if (pDB[T5TB_CALLSTACK].wNbItemUsed != 0)
                    iFirst = pStack[0].wProg;
                else
                    iFirst = pStatus->wCurPrg;
                while ((iPrg = *pPou) != 0 && iPrg != iFirst)
                    pPou++;
                T5TRACEDW("Start exec at", iFirst);
            }
        }
        while (bOK && (iPrg = *pPou++) != 0)
        {

#ifdef T5DEFHOOK_PRGEXEC_BEFORE
            T5HOOK_PRGEXEC_BEFORE(pDB, iPrg)
#endif /*T5DEFHOOK_PRGEXEC_BEFORE*/

            /* reset call stack if not stepping */
            if (pStatus->wCurPrg == 0)
                pDB[T5TB_CALLSTACK].wNbItemUsed = 0;

            if (bOK && bMain
                && (pStatus->wFlags & (T5FLAG_RUN|T5FLAG_ONECYCLE)) == 0)
                bOK = FALSE;
            else if ((pProg[iPrg].wFlags
                      & (T5FLAG_PRGTOSTART|T5FLAG_PRGACTIVE))!=0
                && (pProg[iPrg].wFlags & T5FLAG_PRGSUSPENDED) == 0)
            {

#ifdef T5DEFHOOK_PRGEXEC_BEFORERUN
                T5HOOK_PRGEXEC_BEFORERUN(pDB, iPrg)
#endif /*T5DEFHOOK_PRGEXEC_BEFORERUN*/
                
                bOK = T5VMTic_Run (pDB, pStatus,
                                    (T5_PTWORD)(pProg[iPrg].pCode),
                                    pProg[iPrg].dwSize, iPrg);

#ifdef T5DEFHOOK_PRGEXEC_AFTER
                T5HOOK_PRGEXEC_AFTER(pDB, iPrg)
#endif /*T5DEFHOOK_PRGEXEC_AFTER*/

            }
        }

        if (bMain && pStatus->wCurPrg == 0)
            pStatus->dwCycleCount += 1L;
    }
    /* reset single cyle mode if present */
    if (bMain)
    {
        pStatus->dwFlagsEx &= ~T5FLAGEX_CHGCYC;
        pStatus->wFlags &= ~T5FLAG_ONECYCLE;
    }
}

/*****************************************************************************
T5VMPrg_ProcessExecBuffer
execute instructions received by the communication server
Parameters:
    pDB (IN) pointer to the data base
*****************************************************************************/

void T5VMPrg_ProcessExecBuffer (T5PTR_DB pDB)
{
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if (pStatus->wExecSize != 0)
    {
        /* actual size is one more TIC code (end of sequence) */
        T5VMTic_Run (pDB, pStatus, (T5_PTWORD)(pDB[T5TB_EXEC].pData),
                        pStatus->wExecSize + sizeof (T5_WORD), 0);
        pStatus->wExecSize = 0;
    }
}

/*****************************************************************************
T5VMPrg_SetBreakpoint
set or remove a breakpoint
Parameters:
    pDB (IN) pointer to the data base
    wProg (IN) index of the program where to break (0 for remove all)
    wPos (IN) position in the program
    wStyle (IN) breakpoint style
*****************************************************************************/

void T5VMPrg_SetBreakpoint (T5PTR_DB pDB,
                            T5_WORD wProg, T5_WORD wPos, T5_WORD wStyle)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_BKP pBkp;
    T5_WORD i, iFree, iFound;
    T5_BOOL bFound, bSet;

    pStatus = T5GET_DBSTATUS(pDB);

    if (wProg == 0 && wStyle == T5BKP_DELETED) /* remove all */
    {
        T5_MEMSET (pStatus->bkp, 0, sizeof (pStatus->bkp));
        pStatus->wFlags &= ~T5FLAG_BKP;
        return;
    }
    
    iFree = T5MAX_BKP;
    iFound = T5MAX_BKP;
    pBkp = pStatus->bkp;
    bSet=FALSE;
    for (i=0; iFound==T5MAX_BKP && i<T5MAX_BKP; i++)
    {
        if (pBkp->wStyle == T5BKP_DELETED)
        {
            if (iFree == T5MAX_BKP)
                iFree = i;
        }
        else if (pBkp->wProg == wProg && pBkp->wPos == wPos)
            iFound = i;
        pBkp++;
    }
    if (iFound == T5MAX_BKP)
        iFound = iFree;
    if (iFound < T5MAX_BKP)
    {
        pBkp = &(pStatus->bkp[iFound]);
        if (pBkp->wStyle == T5BKP_DELETED || wStyle != T5BKP_ONESHOT)
        {
            pBkp->wProg = (wStyle == T5BKP_DELETED) ? 0 : wProg;
            pBkp->wPos = (wStyle == T5BKP_DELETED) ? 0 : wPos;
            pBkp->wStyle = wStyle;
            bSet = TRUE;
        }
    }
    /* update main flag */
    if (bSet)
    {
        pBkp = pStatus->bkp;
        for (i=0, bFound=FALSE; !bFound && i<T5MAX_BKP; i++, pBkp++)
            bFound = (pBkp->wStyle != T5BKP_DELETED);
        if (bFound)
            pStatus->wFlags |= T5FLAG_BKP;
        else
            pStatus->wFlags &= ~T5FLAG_BKP;
    }
}

/*****************************************************************************
T5VMPrg_IsBreakpoint
test if a position has a breakpoint
Parameters:
    pDB (IN) pointer to the data base
    wProg (IN) index of the program
    wPos (IN) position in the program
    wID (IN) position logical ID
Return : TRUE if break requested
*****************************************************************************/

T5_BOOL T5VMPrg_IsBreakpoint (T5PTR_DB pDB, T5_WORD wProg, T5_WORD wPos,
                              T5_WORD wID)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_BKP pBkp;
    T5_WORD i;
    T5_BOOL bFound, bFound2, bDel;
    T5_DWORD dwData;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_BKP) == 0)
        return FALSE;

    bFound = bDel = FALSE;
    pBkp = pStatus->bkp;
    for (i=0; !bFound && i<T5MAX_BKP; i++)
    {
        if (pBkp->wStyle == T5BKP_ONESHOT)
        {
            if ((pBkp->wProg == wProg
                    && (pBkp->wPos == wPos || pBkp->wPos == 0))
                || (pBkp->wProg == 0xffff && pBkp->wPos == wID))
            {
                pBkp->wProg = 0;
                pBkp->wPos = 0;
                pBkp->wStyle = T5BKP_DELETED;
                bDel = TRUE;
                bFound = TRUE;
            }
        }
        else if (pBkp->wStyle == T5BKP_VIEW)
        {
            if ((pBkp->wProg == wProg && pBkp->wPos == wPos)
                || (pBkp->wProg == 0xffff && pBkp->wPos == wID))
            {
                dwData = ((((T5_DWORD)pBkp->wProg) & 0xffff)\
                      |((((T5_DWORD)pBkp->wPos)<<16)& 0xffff0000));
                T5VMLog_Push (pDB, T5RET_VIEWPOINT, 0, dwData);
            }
        }
        else if (pBkp->wStyle != T5BKP_DELETED)
        {
            if ((pBkp->wProg == wProg && pBkp->wPos == wPos)
                || (pBkp->wProg == 0xffff && pBkp->wPos == wID))
            {
                bFound = TRUE;
            }
        }
        pBkp++;
    }
    /* update main flag */
    if (bDel)
    {
        pBkp = pStatus->bkp;
        for (i=0, bFound2=FALSE; !bFound2 && i<T5MAX_BKP; i++, pBkp++)
            bFound2 = (pBkp->wStyle != T5BKP_DELETED);
        if (bFound2)
            pStatus->wFlags |= T5FLAG_BKP;
        else
            pStatus->wFlags &= ~T5FLAG_BKP;
    }
    return bFound;
}

/*****************************************************************************
T5VMPrg_SaveCallStack
save callstack state
Parameters:
    pDB (IN) pointer to the data base
    wProg (IN) index of the program
*****************************************************************************/

void T5VMPrg_SaveCallStack (T5PTR_DB pDB, T5_WORD wProg)
{
    T5PTR_DBPROG pProg;
    T5PTR_DBCALLSTACK pStack;
    T5_UNUSED(wProg);

    pStack = T5GET_DBCALLSTACK(pDB);
    pProg = T5GET_DBPROG(pDB);

    if (pDB[T5TB_CALLSTACK].wNbItemUsed == 0)
    {
        pProg->wStackDepth = 0;
        return;
    }

    pProg += pStack->wProg;
    if (pProg->wStackSave == 0)
    {
        pProg->wStackDepth = 0;
        return;
    }

    pProg->wStackDepth = pDB[T5TB_CALLSTACK].wNbItemUsed + 1;
    T5_MEMCPY (pStack + pProg->wStackSave, pStack,
               pProg->wStackDepth * pDB[T5TB_CALLSTACK].wSizeOf);
}

/*****************************************************************************
T5VMPrg_RestoreCallStack
restore callstack state
Parameters:
    pDB (IN) pointer to the data base
    wProg (IN) index of the program
*****************************************************************************/

void T5VMPrg_RestoreCallStack (T5PTR_DB pDB, T5_WORD wProg)
{
    T5PTR_DBPROG pProg;
    T5PTR_DBCALLSTACK pStack;

    pStack = T5GET_DBCALLSTACK(pDB);
    pProg = T5GET_DBPROG(pDB);
    pProg += wProg;

    if (pDB[T5TB_CALLSTACK].wNbItemUsed != 0
        || pProg->wStackSave == 0 || pProg->wStackDepth == 0)
        return;

    T5_MEMCPY (pStack, pStack + pProg->wStackSave,
               pProg->wStackDepth * pDB[T5TB_CALLSTACK].wSizeOf);
    pDB[T5TB_CALLSTACK].wNbItemUsed = pProg->wStackDepth - 1;

    pProg->wStackDepth = 0;
}

/*****************************************************************************
T5VMPrg_HasBreapointOrStep
Check if any breakpoint is installed or if the system is stepping
Parameters:
    pDB (IN) pointer to the data base
Return: TRUE if stepping or breakpoint(s) installed
*****************************************************************************/

T5_BOOL T5VMPrg_HasBreapointOrStep (T5PTR_DB pDB)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBPROG pProg;
    T5_WORD i;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & (T5FLAG_BKP|T5FLAG_PROGSTEP)) != 0)
        return TRUE;
    pProg = T5GET_DBPROG(pDB);
    for (i=0; i<pDB[T5TB_PROG].wNbItemUsed; i++, pProg++)
    {
        if (pProg->dwPos != 0L)
            return TRUE;
    }
    return FALSE;
}

#ifdef T5DEF_IT

/*****************************************************************************
T5VMPrg_BeginEvent
Initialize event handlers when starting a POU
Parameters:
    pDB (IN) pointer to the data base
    wPrgNo (IN) index of the program
*****************************************************************************/

void T5VMPrg_BeginEvent (T5PTR_DB pDB, T5_WORD wPrgNo)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTWORD pHandlers;
    T5_WORD iTask, iMax;

    pPrivate = T5GET_DBPRIVATE(pDB);
    pStatus = T5GET_DBSTATUS(pDB);
    pHandlers = T5VMCode_GetITs (pPrivate->pCode);
    if (pHandlers == NULL || pStatus->bEvStackPtr >= T5MAX_EVENT)
        return;

    iMax = T5VMCode_GetITCount (pPrivate->pCode);
    for (iTask=0; iTask<iMax; iTask++)
    {
        if (wPrgNo == pHandlers[iTask])
        {
            pStatus->bEvStack[pStatus->bEvStackPtr] = iTask;
            pStatus->bEvStackPtr += 1;
#ifdef T5HOOK_ONEVENT
            T5HOOK_ONEVENT_BEGIN (wPrgNo);
#endif /*T5HOOK_ONEVENT*/
        }
    }
}

/*****************************************************************************
T5VMPrg_EndEvent
Release event handlers after execution of a POU
Parameters:
    pDB (IN) pointer to the data base
    wPrgNo (IN) index of the program
*****************************************************************************/

void T5VMPrg_EndEvent (T5PTR_DB pDB, T5_WORD wPrgNo)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTWORD pHandlers;

    pPrivate = T5GET_DBPRIVATE(pDB);
    pStatus = T5GET_DBSTATUS(pDB);
    pHandlers = T5VMCode_GetITs (pPrivate->pCode);
    if (pHandlers == NULL || pStatus->bEvStackPtr == 0)
        return;

    if (pHandlers[pStatus->bEvStack[pStatus->bEvStackPtr-1]] == wPrgNo)
    {
        pStatus->bEvStackPtr -= 1;
        pStatus->bSignalEvent = TRUE;
#ifdef T5HOOK_ONEVENT
        T5HOOK_ONEVENT_END (wPrgNo);
#endif /*T5HOOK_ONEVENT*/
    }
}

/*****************************************************************************
T5VMPrg_GetCurrentEvent
Check if any event is required
Parameters:
    pDB (IN) pointer to the data base
Return: wished event of FF if nothing
*****************************************************************************/

T5_BYTE T5VMPrg_GetCurrentEvent (T5PTR_DB pDB)
{
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if (pStatus->bEvStackPtr == 0)
        return 0xff;
    return pStatus->bEvStack[pStatus->bEvStackPtr-1];
}

/*****************************************************************************
T5VMPrg_GetHighestEventHandler
Check highest requested event
Parameters:
    pDB (IN) pointer to the data base
Return: event ID
*****************************************************************************/

T5_WORD T5VMPrg_GetHighestEventHandler (T5PTR_DB pDB)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTWORD pHandlers;
    T5_SHORT i, iHighTask, iMax;

    pPrivate = T5GET_DBPRIVATE(pDB);
    pStatus = T5GET_DBSTATUS(pDB);
    pHandlers = T5VMCode_GetITs (pPrivate->pCode);
    if (pHandlers == NULL)
        return 0;

    if (pStatus->bEvStackPtr == 0)
        iHighTask = 0;
    else
        iHighTask = (T5_SHORT)(pStatus->bEvStack[pStatus->bEvStackPtr-1]) + 1;
    iMax = (T5_SHORT)T5VMCode_GetITCount (pPrivate->pCode);
    for (i=iMax-1; i>=iHighTask; i--)
    {
        if (pStatus->bEvSig[i] && pHandlers[i] != 0)
        {
            pStatus->bEvSig[i] = FALSE;
            return pHandlers[i];
        }
    }
    return 0;
}

/*****************************************************************************
T5VMPrg_CallEventHandler
Run an event handler
Parameters:
    pDB (IN) pointer to the data base
    wEvent (IN) event ID
*****************************************************************************/

void T5VMPrg_CallEventHandler (T5PTR_DB pDB, T5_WORD wEvent)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTWORD pHandlers;
    T5PTR_DBPROG pProg;
    T5_WORD wPrg;

    pPrivate = T5GET_DBPRIVATE(pDB);
    pStatus = T5GET_DBSTATUS(pDB);
    pHandlers = T5VMCode_GetITs (pPrivate->pCode);
    if (pHandlers == NULL || wEvent >= T5MAX_EVENT || pHandlers[wEvent] == 0)
        return;

    if ((pStatus->wFlags & T5FLAG_RUNMASK) != T5FLAG_RUN
        || pDB[T5TB_CALLSTACK].wNbItemUsed != 0)
        return;

    pProg = T5GET_DBPROG(pDB);
    wPrg = pHandlers[wEvent];
    T5VMTic_Run (pDB, pStatus, (T5_PTWORD)(pProg[wPrg].pCode), pProg[wPrg].dwSize, wPrg);
}

#endif /*T5DEF_IT*/

/* eof **********************************************************************/
