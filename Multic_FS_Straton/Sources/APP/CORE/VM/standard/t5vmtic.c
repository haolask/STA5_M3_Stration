/*****************************************************************************
T5VMtic.c :  pcode interpreter

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/
/* access to outputs */

#ifdef T5DEF_DONTFORCELOCKEDVARS

#define ISLOCK8(i)              ((pLock[i] & T5LOCK_DATA8) != 0)
#define ISLOCK16(i)             ((pLock[i] & T5LOCK_DATA16) != 0)
#define ISLOCK32(i)             ((pLock[i] & T5LOCK_DATA32) != 0)
#define ISLOCKTIME(i)           ((pLock[i] & T5LOCK_TIME) != 0)
#define ISLOCK64(i)             ((pLock[i] & T5LOCK_DATA64) != 0)
#define ISLOCKSTRING(i)         ((pLock[i] & T5LOCK_STRING) != 0)
#define ISLOCKXV(i)             ((pLock[i] & T5LOCK_XV) != 0)

#define IF_ISD8LOCKED(i)        if (ISLOCK8(i))
#define IF_ISD16LOCKED(i)       if (ISLOCK16(i))
#define IF_ISD32LOCKED(i)       if (ISLOCK32(i))
#define IF_ISTIMELOCKED(i)      if (ISLOCKTIME(i))
#define IF_ISD64LOCKED(i)       if (ISLOCK64(i))
#define IF_ISSTRINGLOCKED(i)    if (ISLOCKSTRING(i))
#define IF_ISXVLOCKED(i)        if (ISLOCKXV(i))

#else /*T5DEF_DONTFORCELOCKEDOUTPUTS*/

#define ISLOCK8(i)              FALSE
#define ISLOCK16(i)             FALSE
#define ISLOCK32(i)             FALSE
#define ISLOCKTIME(i)           FALSE
#define ISLOCK64(i)             FALSE
#define ISLOCKSTRING(i)         FALSE
#define ISLOCKXV(i)             FALSE

#define IF_ISD8LOCKED(i)        if (0)
#define IF_ISD16LOCKED(i)       if (0)
#define IF_ISD32LOCKED(i)       if (0)
#define IF_ISTIMELOCKED(i)      if (0)
#define IF_ISD64LOCKED(i)       if (0)
#define IF_ISSTRINGLOCKED(i)    if (0)
#define IF_ISXVLOCKED(i)        if (0)

#endif /*T5DEF_DONTFORCELOCKEDOUTPUTS*/

/****************************************************************************/
/* smart lock table */

#ifdef T5DEF_SMARTLOCK

static void _SmLock8 (T5PTR_DB pDB, T5_WORD wIndex, T5_BYTE bValue);
static void _SmLock16 (T5PTR_DB pDB, T5_WORD wIndex, T5_WORD wValue);
static void _SmLock32 (T5PTR_DB pDB, T5_WORD wIndex, T5_DWORD dwValue);
static void _SmLock64 (T5PTR_DB pDB, T5_WORD wIndex, T5_DATA64 lValue);
static void _SmLockTime (T5PTR_DB pDB, T5_WORD wIndex, T5_DWORD dwValue);
static void _SmLockString (T5PTR_DB pDB, T5_WORD wIndex, T5_WORD wSrcIndex);

#define _SmLock8xa(p,i,s)       /*not supported*/
#define _SmLock16xa(p,i,s)      /*not supported*/
#define _SmLock32xa(p,i,s)      /*not supported*/
#define _SmLock64xa(p,i,s)      /*not supported*/
#define _SmLockTimexa(p,i,s)    /*not supported*/
#define _SmLockStringxa(p,i,s)  /*not supported*/

#else /*T5DEF_SMARTLOCK*/

#define _SmLock8(p,i,s)         /*nothing*/
#define _SmLock16(p,i,s)        /*nothing*/
#define _SmLock32(p,i,s)        /*nothing*/
#define _SmLock64(p,i,s)        /*nothing*/
#define _SmLockTime(p,i,s)      /*nothing*/
#define _SmLockString(p,i,s)    /*nothing*/

#define _SmLock8xa(p,i,s)       /*nothing*/
#define _SmLock16xa(p,i,s)      /*nothing*/
#define _SmLock32xa(p,i,s)      /*nothing*/
#define _SmLock64xa(p,i,s)      /*nothing*/
#define _SmLockTimexa(p,i,s)    /*nothing*/
#define _SmLockStringxa(p,i,s)  /*nothing*/

#endif /*T5DEF_SMARTLOCK*/

/****************************************************************************/
/* access to 8 bit data */

#define GET_D8(i)           (pData8[i])
#define SET_D8(i,b)         { pData8[i] = b; }

#define GET_SINT(i)         (pSint[i])
#define SET_SINT(i,b)       { pSint[i] = b; }

#define GET_UD8(i)          ((T5_BYTE)(pSint[i]))
#define SET_UD8(i,b)        { pSint[i] = (T5_CHAR)(b); }

/****************************************************************************/
/* access to 16 bit data */

#define GET_D16(i)          (pData16[i])
#define SET_D16(i,w)        { pData16[i] = w; }

#define GET_UD16(i)         ((T5_WORD)(pData16[i]))
#define SET_UD16(i,w)       { pData16[i] = (T5_SHORT)(w); }

/****************************************************************************/
/* access to 32 bit data */

#define GET_DINT(i)         (pDint[i])
#define SET_DINT(i,d)       { pDint[i] = d; }

#define GET_UD32(i)         ((T5_DWORD)(pDint[i]))
#define SET_UD32(i,d)       { pDint[i] = (T5_LONG)(d); }

#ifdef T5DEF_REALSUPPORTED
#define GET_REAL(i)         (pReal[i])
#define SET_REAL(i,r)       { pReal[i] = r; }
#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/
/* access to TIME data */

#define GET_TIME(i)         (pTime[i])
#define SET_TIME(i,d)       { pTime[i] = d; }

/****************************************************************************/
/* access to 64 bit data */

#ifdef T5DEF_LREALSUPPORTED
#define GET_LREAL(i)        (pLReal[i])
#define SET_LREAL(i,r)      { pLReal[i] = r; }
#endif /*T5DEF_LREALSUPPORTED*/

#ifdef T5DEF_LINTSUPPORTED
#define GET_LINT(i)         (pLInt[i])
#define SET_LINT(i,r)       { pLInt[i] = r; }
#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_DATA64SUPPORTED
#define GET_DATA64(i)       (p64[i])
#define SET_DATA64(i,d)     { p64[i] = d; }
#endif /*T5DEF_DATA64SUPPORTED*/

#ifdef T5DEF_ULINTSUPPORTED
#define GET_UD64(i)         ((T5_LWORD)(pLInt[i]))
#define SET_UD64(i,d)       { pLInt[i] = (T5_LINT)(d); }
#endif /*T5DEF_ULINTSUPPORTED*/

/****************************************************************************/

#ifndef T5_BM
#define T5_BM       /*nothing*/
#endif /*T5_BM*/

#ifndef T5_EM
#define T5_EM       0L
#endif /*T5_EM*/

#ifndef T5_BMLOOP
#define T5_BMLOOP   1000000L
#endif /*T5_BMLOOP*/

/****************************************************************************/

#ifndef T5DEF_NOPCSWITCH

/****************************************************************************/
/* data for log messages: program index */

#define ERRDATA  ((((T5_DWORD)wPrgNo) & 0xffff)\
                  |((((T5_DWORD)pCode-(T5_DWORD)pStartCode)<<16)& 0xffff0000))

/*****************************************************************************
T5VMTic_Init
Initialize private data
parameters:
    pDB (IN) pointer to the database
*****************************************************************************/

void T5VMTic_Init (T5PTR_DB pDB)
{
#ifdef T5DEF_CCAPP
    T5CC_Init (pDB);
#endif /*T5DEF_CCAPP*/
}

/*****************************************************************************
T5VMTic_Exit
Release private data
parameters:
    pDB (IN) pointer to the database
*****************************************************************************/

void T5VMTic_Exit (T5PTR_DB pDB)
{
#ifdef T5DEF_CCAPP
    T5CC_Exit (pDB);
#endif /*T5DEF_CCAPP*/
}

/*****************************************************************************
T5VMTic_Run
execute a TIC sequence
parameters:
    pDB (IN) pointer to the database
    pStatus (IN) pointer to the application status
    pCode (IN) pointer to the TIC sequence
    dwSize (IN) size of the TIC sequence in bytes
    wPrgNo (IN) index of the program - 0 for the exec buffer
returns: TRUE if ok / FALSE if error encountered
*****************************************************************************/

T5_BOOL T5VMTic_Run (T5PTR_DB pDB, T5PTR_DBSTATUS pStatus, T5_PTWORD pCode,
                     T5_DWORD dwSize, T5_WORD wPrgNo)
{
    T5_PTBYTE pStartCode;
#ifdef T5DEF_TICSAFE
    T5_PTBYTE pEndCode;
#endif /*T5DEF_TICSAFE*/
    T5_WORD wIndex;
    T5_DWORD dwVal;
    T5PTR_DBFBINST pI;
    T5PTR_DBFCLASS pF;
    T5_BOOL bSkip;
    T5_PTWORD pPrgStatus;
    T5PTR_DBPROG pProg;
#ifdef T5DEF_TICSAFE
    T5_DWORD dwCount=0L;
#endif /*T5DEF_TICSAFE*/
    T5_PTBYTE pLock;
    T5_PTBYTE pData8;
#ifdef T5DEF_SINTSUPPORTED
    T5_PTCHAR pSint;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTSHORT pData16;
#endif /*T5DEF_DATA16SUPPORTED*/
    T5_PTLONG pDint;
#ifdef T5DEF_REALSUPPORTED
    T5_PTREAL pReal;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    T5_PTLREAL pLReal;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    T5_PTLINT pLInt;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 p64;
#endif /*T5DEF_DATA64SUPPORTED*/
    T5_PTDWORD pTime;
    T5PTR_DBFBINST pInst;
    T5PTR_DBFCLASS pFC;
#ifdef T5DEF_SFC
    T5PTR_DBSTEP pStep;
    T5PTR_DBTRANS pTrans;
#endif /*T5DEF_SFC*/
#ifdef T5DEF_XV
    T5_PTBYTE *pXV, *pXS;
#endif /*T5DEF_XV*/
    T5PTR_DBCALLSTACK pStack;
    T5_DWORD dwCTOffset;
#ifdef T5DEF_CTSEG
    T5_PTBYTE pCTSeg;
#endif /*T5DEF_CTSEG*/
#ifdef T5DEF_CCAPP
    T5_BOOL bCCRet, bCCDone;
#endif /*T5DEF_CCAPP*/
    T5_PTWORD pSaveCode;
    T5_DWORD dwMeas;
#ifdef T5DEF_IT
    T5_PTBOOL pbSignalEvent;
#endif /*T5DEF_IT*/

    dwMeas = 0;
    pSaveCode = pCode;
    /* may be compiled code */
#ifdef T5DEF_CCAPP
    if (wPrgNo && (pStatus->dwFlagsEx & T5FLAGEX_CCACT) != 0)
    {
        bCCRet = T5CC_Call (pDB, wPrgNo, &bCCDone);
        if (bCCDone)
            return bCCRet;
    }
#endif /*T5DEF_CCAPP*/

    /* prepare fast access pointers */
    pLock = T5GET_DBLOCK(pDB);
    pData8 = T5GET_DBDATA8(pDB);
    pStack = T5GET_DBCALLSTACK(pDB);
#ifdef T5DEF_SINTSUPPORTED
    pSint = (T5_PTCHAR)T5GET_DBDATA8(pDB);
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
    pData16 = (T5_PTSHORT)T5GET_DBDATA16(pDB);
#endif /*T5DEF_DATA16SUPPORTED*/
    pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
#ifdef T5DEF_REALSUPPORTED
    pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    pLReal = (T5_PTLREAL)T5GET_DBDATA64(pDB);
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    pLInt = (T5_PTLINT)T5GET_DBDATA64(pDB);
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
    p64 = (T5_PTDATA64)T5GET_DBDATA64(pDB);
#endif /*T5DEF_DATA64SUPPORTED*/
    pTime = T5GET_DBTIME(pDB);
    pInst = T5GET_DBFBINST(pDB);
    pFC = T5GET_DBFCLASS(pDB);
#ifdef T5DEF_SFC
    pStep = T5GET_DBSTEP(pDB);
    pTrans = T5GET_DBTRANS(pDB);
#endif /*T5DEF_SFC*/
#ifdef T5DEF_XV
    pXV = T5GET_DBXV(pDB);
#endif /*T5DEF_XV*/
    dwCTOffset = 0xffffffffL;
    pProg = T5GET_DBPROG(pDB);
#ifdef T5DEF_CTSEG
    pCTSeg = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
#endif /*T5DEF_CTSEG*/
#ifdef T5DEF_IT
    pbSignalEvent = &(pStatus->bSignalEvent);
#endif /*T5DEF_IT*/

    if (wPrgNo) /* dont do that for exec buffer! */
    {
#ifdef T5DEF_IT
        T5VMPrg_BeginEvent (pDB, wPrgNo);
#endif /*T5DEF_IT*/
        T5VMPrg_RestoreCallStack (pDB, wPrgNo);
        if (pDB[T5TB_CALLSTACK].wNbItemUsed != 0)
        {
            wPrgNo = pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg;
            pCode = (T5_PTWORD)(pProg[wPrgNo].pCode);
        }
        else
            pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg = wPrgNo;
        T5HOOK_SETPRG (wPrgNo);

        pPrgStatus = &(pProg[wPrgNo].wFlags);

        pStartCode = (T5_PTBYTE)pCode;
#ifdef T5DEF_TICSAFE
        pEndCode = pStartCode + pProg[wPrgNo].dwSize;
#endif /*T5DEF_TICSAFE*/

        pCode += pProg[wPrgNo].dwPos;
        pStatus->wCurPrg = pStatus->wCurPos = 0;
        pProg[wPrgNo].dwPos = 0;
    }
    else
    {
        pStartCode = (T5_PTBYTE)pCode;
#ifdef T5DEF_TICSAFE
        pEndCode = (T5_PTBYTE)pCode + dwSize;
#endif /*T5DEF_TICSAFE*/
        pPrgStatus = NULL;
    }

    while (1)
    {
#ifdef T5DEF_TICSAFE
        if (dwCount++ > T5DEF_TICSAFEMAXINST)
        {
            T5VMLog_Push (pDB, T5RET_SAFETICLOOP, 0, ERRDATA);
            pStatus->wFlags &= ~T5FLAG_RUNMASK;
            pStatus->wFlags |= T5FLAG_ERROR;
            T5HOOK_SETPRG (0);
            return FALSE;
        }
        if ((T5_PTBYTE)pCode < pStartCode || (T5_PTBYTE)pCode >= pEndCode)
        {
            T5TRACEDW("Out of range pCode", pCode);
            T5TRACEDW("   pStartCode", pStartCode);
            T5TRACEDW("   pEndCode", pEndCode);
            T5VMLog_Push (pDB, T5RET_OUTOFTIC, 0, ERRDATA);
            pStatus->wFlags &= ~T5FLAG_RUNMASK;
            pStatus->wFlags |= T5FLAG_ERROR;
            T5HOOK_SETPRG (0);
            return FALSE;
        }
#endif /*T5DEF_TICSAFE*/

#ifdef T5DEFHOOK_OEM_TICSAFE
        if (!T5HOOK_OEM_TICSAFE (pStatus->wFlags))
        {          
            return FALSE;            
        }
#endif /* T5DEFHOOK_OEM_TICSAFE */

#ifdef T5DEF_IT
        if (*pbSignalEvent)
        {
            *pbSignalEvent = FALSE;
            wIndex = T5VMPrg_GetHighestEventHandler (pDB);
            if (wIndex != 0 && wIndex != wPrgNo)
            {
                if (pDB[T5TB_CALLSTACK].wNbItemUsed >= pStatus->wMaxCallStack)
                {
                    T5VMLog_Push (pDB, T5RET_STACKOVER, wIndex, ERRDATA);
                    pStatus->wFlags &= ~T5FLAG_RUNMASK;
                    pStatus->wFlags |= T5FLAG_ERROR;
                    T5HOOK_SETPRG (0);
                    return FALSE;
                }
                pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg = wPrgNo;
                pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wPos
                 = ((T5_WORD)(pCode - (T5_PTWORD)(pProg[wPrgNo].pCode))) * 2;
                pDB[T5TB_CALLSTACK].wNbItemUsed += 1;
                pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg = wIndex;
                wPrgNo = wIndex;
                T5HOOK_SETPRG (wPrgNo);
                pCode = (T5_PTWORD)(pProg[wPrgNo].pCode);
                pStartCode = (T5_PTBYTE)pCode;
#ifdef T5DEF_TICSAFE
                pEndCode = pStartCode + pProg[wPrgNo].dwSize;
#endif /*T5DEF_TICSAFE*/
                T5VMPrg_BeginEvent (pDB, wPrgNo);
            }
        }
#endif /*T5DEF_IT*/

        switch (*pCode++)
        {
        case T5C_BM :
            pSaveCode = pCode;
            dwMeas = T5_BMLOOP;
            T5_BM;
            break;
        case T5C_EM :
            if (--dwMeas)
                pCode = pSaveCode;
            else
            {
                dwMeas = T5_EM;
                SET_DINT (*pCode++, dwMeas);
                dwMeas = 0;
            }
            break;

        case T5C_NOP :
            break;

        /* break the cycle **************************************************/
        case T5C_RET : /* BRANCH: return */
        case T5C_ENDSEQ : /* BRANCH: end of sequence */
#ifdef T5DEF_IT
                T5VMPrg_EndEvent (pDB, wPrgNo);
#endif /*T5DEF_IT*/
            if (wPrgNo != 0 && pDB[T5TB_CALLSTACK].wNbItemUsed != 0)
            {
                wPrgNo = pStack[pDB[T5TB_CALLSTACK].wNbItemUsed-1].wProg;
                T5HOOK_SETPRG (wPrgNo);
                pCode = (T5_PTWORD)(pProg[wPrgNo].pCode);
                pStartCode = (T5_PTBYTE)pCode;
#ifdef T5DEF_TICSAFE
                pEndCode = pStartCode + pProg[wPrgNo].dwSize;
#endif /*T5DEF_TICSAFE*/
                pCode += (pStack[pDB[T5TB_CALLSTACK].wNbItemUsed-1].wPos) / 2;
                pDB[T5TB_CALLSTACK].wNbItemUsed -= 1;
                pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg = wPrgNo;
                break;
            }
            else
            {
                T5HOOK_SETPRG (0);
                return TRUE;
            }

        case T5C_SCHEDPRG : /* MISC: program scheduling: period, offset */
            if (pCode[0] > 1 && pDB[T5TB_CALLSTACK].wNbItemUsed == 0)
            {
                bSkip = (pProg[wPrgNo].wCount != pCode[1]);
                pProg[wPrgNo].wCount = (pProg[wPrgNo].wCount + 1) % pCode[0];
                if (bSkip)
                {
#ifdef T5DEF_IT
                    T5VMPrg_EndEvent (pDB, wPrgNo);
#endif /*T5DEF_IT*/
                    T5HOOK_SETPRG (0);
                    return TRUE;
                }
            }
            pCode += 2;
            break;

        case T5C_SETWAITMARK :
            pProg[wPrgNo].dwPos = (pCode - 1 - (T5_PTWORD)(pProg[wPrgNo].pCode));
            break;
        case T5C_WAIT : /* break program witing for a condition */
            if (!GET_D8(*pCode))
            {
                T5VMPrg_SaveCallStack (pDB, wPrgNo);
                T5HOOK_SETPRG (0);
                return TRUE;
            }
            else
            {
                pProg[wPrgNo].dwPos = 0;
                pCode += 1;
            }
            break;
        
        /* program call ****************************************************/

        case T5C_CALLPRG : /* CALL: sub program */
            if (pDB[T5TB_CALLSTACK].wNbItemUsed >= pStatus->wMaxCallStack)
            {
                T5VMLog_Push (pDB, T5RET_STACKOVER, *pCode, ERRDATA);
                pStatus->wFlags &= ~T5FLAG_RUNMASK;
                pStatus->wFlags |= T5FLAG_ERROR;
                T5HOOK_SETPRG (0);
                return FALSE;
            }
            pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg = wPrgNo;
            pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wPos
             = ((T5_WORD)(pCode - (T5_PTWORD)(pProg[wPrgNo].pCode)) + 1) * 2;
            pDB[T5TB_CALLSTACK].wNbItemUsed += 1;
            pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg = *pCode;
            wPrgNo = *pCode;
            T5HOOK_SETPRG (wPrgNo);
            pCode = (T5_PTWORD)(pProg[wPrgNo].pCode);
            pStartCode = (T5_PTBYTE)pCode;
#ifdef T5DEF_TICSAFE
            pEndCode = pStartCode + pProg[wPrgNo].dwSize;
#endif /*T5DEF_TICSAFE*/
#ifdef T5DEF_IT
            T5VMPrg_BeginEvent (pDB, wPrgNo);
#endif /*T5DEF_IT*/
            break;

        /* cycle triggering control *****************************************/

        case T5C_STEPMARK :
            dwVal = (T5_DWORD)(pCode - (T5_PTWORD)(pProg[wPrgNo].pCode));

            wIndex = *pCode++; /* argument: step ID */

            if ((pStatus->wFlags & T5FLAG_PROGSTEP) != 0
                || T5VMPrg_IsBreakpoint (pDB, wPrgNo,
                                         (T5_WORD)((dwVal-1) * 2), wIndex))
            {
                pStatus->wFlags &= ~T5FLAG_RUN;
                pStatus->wFlags |= T5FLAG_ONECYCLE;
                pStatus->wFlags |= T5FLAG_PROGSTEP;
                /* avoid consecutive step marks */
                while (*pCode == T5C_STEPMARK)
                    pCode += 2;
                /* mark next step position and exit */
                pProg[wPrgNo].dwPos = dwVal + 1;
                pStatus->wCurPrg = wPrgNo;
                pStatus->wCurPos = (T5_WORD)(dwVal - 1) * 2;
                T5HOOK_SETPRG (0);
                return FALSE;
            }
            break;

        case T5C_PAUSE : /* MISC: pause VM execution */
            pStatus->wFlags &= ~T5FLAG_RUN;
            pStatus->wFlags &= ~T5FLAG_PROGSTEP;
            break;

        case T5C_RESUME : /* MISC: resume VM execution */
            pStatus->wFlags |= T5FLAG_RUN;
            pStatus->wFlags &= ~T5FLAG_ERROR;
            pStatus->wFlags &= ~T5FLAG_PROGSTEP;
            break;

        case T5C_CYCLESTEP : /* MISC: execute one cycle */
            pStatus->wFlags |= T5FLAG_ONECYCLE;
            pStatus->wFlags &= ~T5FLAG_PROGSTEP;
            break;

        case T5C_STEPFWD : /* execute next step in code */
            if ((pStatus->wFlags & T5FLAG_RUN) == 0)
            {
                pStatus->wFlags |= T5FLAG_ONECYCLE;
                pStatus->wFlags |= T5FLAG_PROGSTEP;
            }
            break;
        
        case T5C_SETCYCLETIME : /* MISC: set cycle time (triggering): PF - Pf */
            pStatus->dwTimTrigger = (T5_DWORD)(*pCode++) << 16 & 0xffff0000L;
            pStatus->dwTimTrigger |= ((T5_DWORD)(*pCode++) & 0xffffL);
            break;

        case T5C_SETBKP : /* set breakpoint - prog - pos - style */
            T5VMPrg_SetBreakpoint (pDB, pCode[0], pCode[1], pCode[2]);
            pCode += 3;
            break;

        /* jumps ************************************************************/

        case T5C_JUMP : /* BRANCH: jump - no condition */
            pCode = (T5_PTWORD)((T5_DWORD)pCode + (T5_SHORT)(*pCode));
            pCode++;
            break;
        case T5C_JUMPIF : /* BRANCH: jump if a */
            if (GET_D8(*pCode))
                pCode = (T5_PTWORD)((T5_DWORD)pCode + (T5_SHORT)(pCode[1]));
            pCode += 2;
            break;
        case T5C_JUMPIFNOT : /* BRANCH: jump if not a */
            if (!GET_D8(*pCode))
                pCode = (T5_PTWORD)((T5_DWORD)pCode + (T5_SHORT)(pCode[1]));
            pCode += 2;
            break;

        /* jumps ************************************************************/

        case T5C_JA : /* BRANCH: abs jump - no condition */
            T5TRACEDW ("JA jump", *pCode);
            pCode = (T5_PTWORD)(pStartCode + *pCode);
            break;
        case T5C_JAC : /* BRANCH: abs jump if a */
            T5TRACEDW ("JAC jump", pCode[1]);
            if (GET_D8(*pCode))
                pCode = (T5_PTWORD)(pStartCode + pCode[1]);
            else
                pCode += 2;
            break;
        case T5C_JACN : /* BRANCH: abs jump if not a */
            T5TRACEDW ("JACN jump", pCode[1]);
            if (!GET_D8(*pCode))
                pCode = (T5_PTWORD)(pStartCode + pCode[1]);
            else
                pCode += 2;
            break;

        /* boolean / data8 operations ***************************************/

        case T5C_O_NOTBOOL : /* <ifnolock> BOOL: a := not b */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_NOTBOOL : /* BOOL: a := not b */
            SET_D8(*pCode, !GET_D8(pCode[1]));
            pCode += 2;
            break;

        case T5C_O_EQBOOL : /* <ifnolock> BOOL: a := b == c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_EQBOOL : /* BOOL: a := b == c */
            SET_D8(*pCode, (GET_D8(pCode[1]) == GET_D8(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_NEBOOL : /* <ifnolock> BOOL: a := b != c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_NEBOOL : /* BOOL: a := b != c */
            SET_D8(*pCode, (GET_D8(pCode[1]) != GET_D8(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_ORBOOL : /* <ifnolock> BOOL: a := b OR c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ORBOOL : /* BOOL: a := b OR c */
            SET_D8(*pCode, (GET_D8(pCode[1]) | GET_D8(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_ANDBOOL : /* <ifnolock> BOOL: a := b AND c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ANDBOOL : /* BOOL: a := b AND c */
            SET_D8(*pCode, (GET_D8(pCode[1]) & GET_D8(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_XORBOOL : /* <ifnolock> BOOL: a := b XOR c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_XORBOOL : /* BOOL: a := b XOR c */
            SET_D8(*pCode, (GET_D8(pCode[1]) ^ GET_D8(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_COPYBOOL : /* <ifnolock> BOOL/SINT: a := b */
            IF_ISD8LOCKED(*pCode)
            {
                _SmLock8 (pDB, *pCode, GET_SINT(pCode[1]));
                pCode += 2;
                break;
            }
        case T5C_COPYBOOL : /* BOOL/SINT: a := b */
            SET_D8(*pCode, GET_D8(pCode[1]));
            pCode += 2;
            break;

        case T5C_O_SETBOOL : /* <ifnolock> BOOL: a := TRUE */
            IF_ISD8LOCKED(*pCode)
            {
                pCode ++;
                break;
            }
        case T5C_SETBOOL : /* BOOL: a := TRUE */
            _SmLock8 (pDB, *pCode, 1);
            SET_D8(*pCode, 1);
            pCode ++;
            break;

        case T5C_O_RESETBOOL : /* <ifnolock> BOOL: a := FALSE */
            IF_ISD8LOCKED(*pCode)
            {
                pCode ++;
                break;
            }
        case T5C_RESETBOOL : /* BOOL: a := FALSE */
            _SmLock8 (pDB, *pCode, 0);
            SET_D8(*pCode, 0);
            pCode ++;
            break;

        case T5C_O_AGETBOOL : /* <ifnolock> BOOL: a := b [ c ] */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARGETBOOL : /* BOOL: a := b [ c ] */
            SET_D8(*pCode, GET_D8(pCode[1] + GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_APUTBOOL : /* <ifnolock> BOOL: a [ b ] := c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARPUTBOOL : /* BOOL: a [ b ] := c */
            SET_D8((*pCode + GET_DINT(pCode[1])), GET_D8(pCode[2]));
            pCode += 3;
            break;

        case T5C_LOCKBOOL : /* BOOL: lock data8 */
        case T5C_LOCKSINT : /* SINT: lock data8 */
            T5VMTic_LockVar (pDB, *pCode, T5LOCK_DATA8, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_RegisterLockVar (pDB, T5C_SINT, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_RegisterLockVar (pDB, T5C_SINT, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;

        case T5C_UNLOCKBOOL : /* BOOL: unlock data8 */
        case T5C_UNLOCKSINT : /* SINT: unlock data8 */
            T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_DATA8, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_UnRegisterLockVar (pDB, T5C_SINT, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_UnRegisterLockVar (pDB, T5C_SINT, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;

        case T5C_O_REDGE : /* <ifnolock> BOOL: REGDE: a := b & !c; c := b */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_REDGE : /* BOOL: REDGE: a := b & !c; c := b */
            SET_D8(*pCode, GET_D8(pCode[1]) && !GET_D8(pCode[2]) && !T5STAT_CHGCYC(pStatus));
            SET_D8(pCode[2], GET_D8(pCode[1]));
            pCode += 3;
            break;
        case T5C_O_FEDGE : /* <ifnolock> BOOL: FEDGE: a := !b & c; c := b */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_FEDGE : /* BOOL: FEDGE: a := !b & c; c := b */
            SET_D8(*pCode, !GET_D8(pCode[1]) && GET_D8(pCode[2]) && !T5STAT_CHGCYC(pStatus));
            SET_D8(pCode[2], GET_D8(pCode[1]));
            pCode += 3;
            break;

        /* SINT operations **************************************************/

#ifdef T5DEF_SINTSUPPORTED

        case T5C_O_NEGSINT : /* <ifnolock> SINT: a := - b */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_NEGSINT : /* SINT: a := - b */
            SET_SINT(*pCode, -GET_SINT(pCode[1]));
            pCode += 2;
            break;

        case T5C_O_MULSINT : /* <ifnolock> SINT: a := b * c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_MULSINT : /* SINT: a := b * c */
            SET_SINT(*pCode, GET_SINT(pCode[1]) * GET_SINT(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_DIVSINT : /* <ifnolock> SINT: a := b / c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_DIVSINT : /* SINT: a := b / c */
            if (!GET_SINT(pCode[2]))
            {
                T5_DIAGHOOK (T5_DIAGHOOK_DIVZERO, 0);
                T5VMLog_Push (pDB, T5RET_SDIVZERO, pCode[2], ERRDATA);
                SET_SINT(*pCode, T5_MAXSINT);
            }
            else
                SET_SINT(*pCode, (GET_SINT(pCode[1]) / GET_SINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_ADDSINT : /* <ifnolock> SINT: a := b + c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ADDSINT : /* SINT: a := b + c */
            SET_SINT(*pCode, GET_SINT(pCode[1]) + GET_SINT(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_SUBSINT : /* <ifnolock> SINT: a := b - c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_SUBSINT : /* SINT: a := b - c */
            SET_SINT(*pCode, GET_SINT(pCode[1]) - GET_SINT(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_GTSINT : /* <ifnolock> SINT: a := b > c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTSINT : /* SINT: a := b > c */
            SET_D8(*pCode, GET_SINT(pCode[1]) > GET_SINT(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_GESINT : /* <ifnolock> SINT: a := b >= c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GESINT : /* SINT: a := b >= c */
            SET_D8(*pCode, GET_SINT(pCode[1]) >= GET_SINT(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_EQSINT : /* <ifnolock> SINT: a := b == c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_EQSINT : /* SINT: a := b == c */
            SET_D8(*pCode, GET_SINT(pCode[1]) == GET_SINT(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_NESINT : /* <ifnolock> SINT: a := b != c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_NESINT : /* SINT: a := b != c */
            SET_D8(*pCode, GET_SINT(pCode[1]) != GET_SINT(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_COPYSINT : /* <ifnolock> SINT: a := b */
            IF_ISD8LOCKED(*pCode) /*SMARTLOCK*/
            {
                _SmLock8 (pDB, *pCode, GET_SINT(pCode[1]));
                pCode += 2;
                break;
            }
        case T5C_COPYSINT : /* SINT: a := b */
            SET_SINT(*pCode, GET_SINT(pCode[1]));
            pCode += 2;
            break;

        case T5C_O_ARGETSINT : /* <ifnolock> SINT: a := b [ c ] */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARGETSINT : /* SINT: a := b [ c ] */
            SET_SINT(*pCode, GET_SINT(pCode[1] + GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_ARPUTSINT : /* <ifnolock> SINT: a [ b ] := c */
            IF_ISD8LOCKED((*pCode + GET_DINT(pCode[1]))) /*SMARTLOCK*/
            {
                _SmLock8 (pDB, (T5_WORD)(*pCode + GET_DINT(pCode[1])), GET_SINT(pCode[2]));
                pCode += 3;
                break;
            }
        case T5C_ARPUTSINT : /* SINT: a [ b ] := c */
            SET_SINT((*pCode + GET_DINT(pCode[1])), GET_SINT(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_SETSINT : /* <ifnolock> SINT: force with 8 bit constant value */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_SETSINT : /* SINT: force with 8 bit constant value */
            if (*pCode != 0)
            {
                _SmLock8 (pDB, *pCode, (T5_BYTE)(pCode[1] & 0xff));
                SET_SINT(*pCode, (T5_CHAR)(pCode[1] & 0xff));
            }
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                pCTSeg[dwCTOffset] = (T5_BYTE)(pCode[1] & 0xff);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
            pCode += 2;
            break;

#ifdef T5DEF_DOTBITSUPPORTED

        case T5C_O_GETBIT8 : /* <ifnolock> SINT: a := b.c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GETBIT8 : /* SINT: a := b.c */
            SET_D8(*pCode, (GET_SINT(pCode[1]) & GET_SINT(pCode[2])) != 0);
            pCode += 3;
            break;
        case T5C_O_SETBIT8 : /* <ifnolock> SINT: a.b := c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_SETBIT8 : /* SINT: a.b := c */
            if (GET_D8(pCode[2]))
            {
                SET_SINT(*pCode, (GET_SINT(*pCode) | GET_SINT(pCode[1])));
            }
            else
            {
                SET_SINT(*pCode, (GET_SINT(*pCode) & ~GET_SINT(pCode[1])));
            }
            pCode += 3;
            break;

#endif /*T5DEF_DOTBITSUPPORTED*/

#endif /*T5DEF_SINTSUPPORTED*/

        /* USINT operations *************************************************/

#ifdef T5DEF_USINTSUPPORTED

        case T5C_O_MULUSINT : /* <ifnolock> USINT: a := b * c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_MULUSINT : /* USINT: a := b * c */
            SET_UD8(*pCode, GET_UD8(pCode[1]) * GET_UD8(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_DIVUSINT : /* <ifnolock> USINT: a := b / c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_DIVUSINT : /* USINT: a := b / c */
            if (!GET_UD8(pCode[2]))
            {
                T5_DIAGHOOK (T5_DIAGHOOK_DIVZERO, 0);
                T5VMLog_Push (pDB, T5RET_SDIVZERO, pCode[2], ERRDATA);
                SET_UD8(*pCode, T5_MAXSINT);
            }
            else
                SET_UD8(*pCode, (GET_UD8(pCode[1]) / GET_UD8(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_GTUSINT : /* <ifnolock> USINT: a := b > c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTUSINT : /* USINT: a := b > c */
            SET_D8(*pCode, GET_UD8(pCode[1]) > GET_UD8(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_GEUSINT : /* <ifnolock> USINT: a := b >= c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GEUSINT : /* USINT: a := b >= c */
            SET_D8(*pCode, GET_UD8(pCode[1]) >= GET_UD8(pCode[2]));
            pCode += 3;
            break;

#endif /*T5DEF_USINTSUPPORTED*/

        /* DATA16 operations ************************************************/

#ifdef T5DEF_DATA16SUPPORTED

        case T5C_O_COPYINT : /* <ifnolock> INT: a := b */
            IF_ISD16LOCKED(*pCode) /*SMARTLOCK*/
            {
                _SmLock16 (pDB, *pCode, GET_D16(pCode[1]));
                pCode += 2;
                break;
            }
        case T5C_COPYINT : /* INT: a := b */
            SET_D16(*pCode, GET_D16(pCode[1]));
            pCode += 2;
            break;

        case T5C_O_NEGINT : /* <ifnolock> INT: a := - b */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_NEGINT : /* INT: a := - b */
            SET_D16(*pCode, -GET_D16(pCode[1]));
            pCode += 2;
            break;

        case T5C_O_MULINT : /* <ifnolock> INT: a := b * c */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_MULINT : /* INT: a := b * c */
            SET_D16(*pCode, GET_D16(pCode[1]) * GET_D16(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_DIVINT : /* <ifnolock> INT: a := b / c */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_DIVINT : /* INT: a := b / c */
            if (!GET_D16(pCode[2]))
            {
                T5_DIAGHOOK (T5_DIAGHOOK_DIVZERO, 0);
                T5VMLog_Push (pDB, T5RET_WDIVZERO, pCode[2], ERRDATA);
                SET_D16(*pCode, T5_MAXINT);
            }
            else
                SET_D16(*pCode, (GET_D16(pCode[1]) / GET_D16(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_ADDINT : /* <ifnolock> INT: a := b + c */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ADDINT : /* INT: a := b + c */
            SET_D16(*pCode, GET_D16(pCode[1]) + GET_D16(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_SUBINT : /* <ifnolock> INT: a := b - c */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_SUBINT : /* INT: a := b - c */
            SET_D16(*pCode, GET_D16(pCode[1]) - GET_D16(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_GTINT : /* <ifnolock> INT: a := b > c */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTINT : /* INT: a := b > c */
            SET_D8(*pCode, GET_D16(pCode[1]) > GET_D16(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_GEINT : /* <ifnolock> INT: a := b >= c */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GEINT : /* INT: a := b >= c */
            SET_D8(*pCode, GET_D16(pCode[1]) >= GET_D16(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_EQINT : /* <ifnolock> INT: a := b == c */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_EQINT : /* INT: a := b == c */
            SET_D8(*pCode, GET_D16(pCode[1]) == GET_D16(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_NEINT : /* <ifnolock> INT: a := b != c */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_NEINT : /* INT: a := b != c */
            SET_D8(*pCode, GET_D16(pCode[1]) != GET_D16(pCode[2]));
            pCode += 3;
            break;


        case T5C_O_ARGETINT : /* <ifnolock> INT: a := b [ c ] */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARGETINT : /* INT: a := b [ c ] */
            SET_D16(*pCode, GET_D16(pCode[1] + GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_ARPUTINT : /* <ifnolock> INT: a [ b ] := c */
            IF_ISD16LOCKED((*pCode + GET_DINT(pCode[1]))) /*SMARTLOCK*/
            {
                _SmLock16 (pDB, (T5_WORD)(*pCode + GET_DINT(pCode[1])), GET_D16(pCode[2]));
                pCode += 3;
                break;
            }
        case T5C_ARPUTINT : /* INT: a [ b ] := c */
            SET_D16((*pCode + GET_DINT(pCode[1])), GET_D16(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_SETINT : /* <ifnolock> INT: force with 16 bit constant value */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_SETINT : /* <ifnolock> INT: force with 16 bit constant value */
            if (*pCode != 0)
            {
                _SmLock16 (pDB, *pCode, (T5_WORD)(pCode[1]));
                SET_D16(*pCode, (T5_SHORT)(pCode[1]));
            }
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                *(T5_PTWORD)(pCTSeg+dwCTOffset) = pCode[1];
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
            pCode += 2;
            break;

        case T5C_LOCKINT : /* INT: lock 16 bit variable */
            T5VMTic_LockVar (pDB, *pCode, T5LOCK_DATA16, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_RegisterLockVar (pDB, T5C_INT, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_RegisterLockVar (pDB, T5C_INT, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;

        case T5C_UNLOCKINT : /* INT: unlock 16 bit variable */
            T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_DATA16, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_UnRegisterLockVar (pDB, T5C_INT, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_UnRegisterLockVar (pDB, T5C_INT, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;

#ifdef T5DEF_DOTBITSUPPORTED

        case T5C_O_GETBIT16 : /* <ifnolock> INT: a := b.c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GETBIT16 : /* INT: a := b.c */
            SET_D8(*pCode, (GET_D16(pCode[1]) & GET_D16(pCode[2])) != 0);
            pCode += 3;
            break;
        case T5C_O_SETBIT16 : /* <ifnolock> INT: a.b := c */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_SETBIT16 : /* INT: a.b := c */
            if (GET_D8(pCode[2]))
            {
                SET_D16(*pCode, (GET_D16(*pCode) | GET_D16(pCode[1])));
            }
            else
            {
                SET_D16(*pCode, (GET_D16(*pCode) & ~GET_D16(pCode[1])));
            }
            pCode += 3;
            break;

#endif /*T5DEF_DOTBITSUPPORTED*/

#endif /*T5DEF_DATA16SUPPORTED*/

        /* UINT operations **************************************************/

#ifdef T5DEF_UINTSUPPORTED

        case T5C_O_MULUINT : /* <ifnolock> UINT: a := b * c */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_MULUINT : /* UINT: a := b * c */
            SET_UD16(*pCode, GET_UD16(pCode[1]) * GET_UD16(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_DIBUINT : /* <ifnolock> UINT: a := b / c */
            IF_ISD16LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_DIVUINT : /* UINT: a := b / c */
            if (!GET_UD16(pCode[2]))
            {
                T5_DIAGHOOK (T5_DIAGHOOK_DIVZERO, 0);
                T5VMLog_Push (pDB, T5RET_WDIVZERO, pCode[2], ERRDATA);
                SET_UD16(*pCode, T5_MAXINT);
            }
            else
                SET_UD16(*pCode, (GET_UD16(pCode[1]) / GET_UD16(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_GTUINT : /* <ifnolock> UINT: a := b > c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTUINT : /* UINT: a := b > c */
            SET_D8(*pCode, GET_UD16(pCode[1]) > GET_UD16(pCode[2]));
            pCode += 3;
            break;

        case T5C_O_GEUINT : /* <ifnolock> UINT: a := b >= c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GEUINT : /* UINT: a := b >= c */
            SET_D8(*pCode, GET_UD16(pCode[1]) >= GET_UD16(pCode[2]));
            pCode += 3;
            break;

#endif /*T5DEF_UINTSUPPORTED*/

        /* DINT operations **************************************************/

        case T5C_O_NEGDINT : /* <ifnolock> DINT: a := - b */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_NEGDINT : /* DINT: a := - b */
            SET_DINT(*pCode, -(GET_DINT(pCode[1])));
            pCode += 2;
            break;

        case T5C_O_MULDINT : /* <ifnolock> DINT: a := b * c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_MULDINT : /* DINT: a := b * c */
            SET_DINT(*pCode, (GET_DINT(pCode[1]) * GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_DIVDINT : /* <ifnolock> DINT: a := b / c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_DIVDINT : /* DINT: a := b / c */
            if (!GET_DINT(pCode[2]))
            {
                T5_DIAGHOOK (T5_DIAGHOOK_DIVZERO, 0);
                T5VMLog_Push (pDB, T5RET_DIVZERO, pCode[2], ERRDATA);
                SET_DINT(*pCode, T5_MAXDINT);
            }
            else
                SET_DINT(*pCode, (GET_DINT(pCode[1]) / GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_ADDDINT : /* <ifnolock> DINT: a := b + c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ADDDINT : /* DINT: a := b + c */
            SET_DINT(*pCode, (GET_DINT(pCode[1]) + GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_SUBDINT : /* <ifnolock> DINT: a := b - c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_SUBDINT : /* DINT: a := b - c */
            SET_DINT(*pCode, (GET_DINT(pCode[1]) - GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_GTDINT : /* <ifnolock> DINT: a := b > c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTDINT : /* DINT: a := b > c */
            SET_D8(*pCode, (GET_DINT(pCode[1]) > GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_GEDINT : /* <ifnolock> DINT: a := b >= c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GEDINT : /* DINT: a := b >= c */
            SET_D8(*pCode, (GET_DINT(pCode[1]) >= GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_EQDINT : /* <ifnolock> DINT: a := b == c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_EQDINT : /* DINT: a := b == c */
            SET_D8(*pCode, (GET_DINT(pCode[1]) == GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_NEDINT : /* <ifnolock> DINT: a := b != c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_NEDINT : /* DINT: a := b != c */
            SET_D8(*pCode, (GET_DINT(pCode[1]) != GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_COPYDINT : /* <ifnolock> DINT/REAL: a := b */
            IF_ISD32LOCKED(*pCode) /*SMARTLOCK*/
            {
                _SmLock32 (pDB, *pCode, (GET_DINT(pCode[1])));
                pCode += 2;
                break;
            }
        case T5C_COPYDINT : /* DINT/REAL: a := b */
            SET_DINT(*pCode, (GET_DINT(pCode[1])));
            pCode += 2;
            break;

        case T5C_O_SETDINT : /* <ifnolock> DINT/REAL: force with 32 bit constant value */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_SETDINT : /* DINT/REAL: force with 32 bit constant value */
            if (*pCode != 0)
            {
                T5_COPYWORDSTOLONG((T5_PTBYTE)(pDint+*pCode), (T5_PTBYTE)(pCode+1));
                _SmLock32 (pDB, *pCode, GET_DINT(*pCode));
            }
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5_COPYWORDSTOLONG(pCTSeg+dwCTOffset, (T5_PTBYTE)(pCode+1));
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
            pCode += 3;
            break;

        case T5C_O_AGETDINT : /* <ifnolock> DINT: a := b [ c ] */ 
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARGETDINT : /* DINT: a := b [ c ] */ 
            SET_DINT(*pCode, GET_DINT(pCode[1] + GET_DINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_APUTDINT : /* <ifnolock> DINT: a [ b ] := c */
            IF_ISD32LOCKED((*pCode + GET_DINT(pCode[1]))) /*SMARTLOCK*/
            {
                _SmLock32 (pDB, (T5_WORD)(*pCode + GET_DINT(pCode[1])), GET_DINT(pCode[2]));
                pCode += 3;
                break;
            }
        case T5C_ARPUTDINT : /* DINT: a [ b ] := c */
            SET_DINT((*pCode + GET_DINT(pCode[1])), GET_DINT(pCode[2]));
            pCode += 3;
            break;

        case T5C_LOCKDINT : /* DINT/REAL: lock 32 bit variable */
            T5VMTic_LockVar (pDB, *pCode, T5LOCK_DATA32, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_RegisterLockVar (pDB, T5C_DINT, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_RegisterLockVar (pDB, T5C_DINT, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;

        case T5C_UNLOCKDINT : /* DINT/REAL: unlock 32 bit variable */
            T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_DATA32, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_UnRegisterLockVar (pDB, T5C_DINT, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_UnRegisterLockVar (pDB, T5C_DINT, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;

#ifdef T5DEF_DOTBITSUPPORTED

        case T5C_O_GETBIT32 : /* <ifnolock> DINT: a := b.c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GETBIT32 : /* DINT: a := b.c */
            SET_D8(*pCode, (GET_DINT(pCode[1]) & GET_DINT(pCode[2])) != 0);
            pCode += 3;
            break;
        case T5C_O_SETBIT32 : /* <ifnolock> DINT: a.b := c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_SETBIT32 : /* DINT: a.b := c */
            if (GET_D8(pCode[2]))
            {
                SET_DINT(*pCode, (GET_DINT(*pCode) | GET_DINT(pCode[1])));
            }
            else
            {
                SET_DINT(*pCode, (GET_DINT(*pCode) & ~GET_DINT(pCode[1])));
            }
            pCode += 3;
            break;

#endif /*T5DEF_DOTBITSUPPORTED*/
            
        /* UDINT operations *************************************************/

#ifdef T5DEF_UDINTSUPPORTED

        case T5C_0_MULUDINT : /* <ifnolock> UDINT: a := b * c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_MULUDINT : /* UDINT: a := b * c */
            SET_UD32(*pCode, GET_UD32(pCode[1]) * GET_UD32(pCode[2]));
            pCode += 3;
            break;

        case T5C_0_DIVUDINT : /* <ifnolock> UDINT: a := b / c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_DIVUDINT : /* UDINT: a := b / c */
            if (!GET_UD32(pCode[2]))
            {
                T5_DIAGHOOK (T5_DIAGHOOK_DIVZERO, 0);
                T5VMLog_Push (pDB, T5RET_DIVZERO, pCode[2], ERRDATA);
                SET_UD32(*pCode, T5_MAXDINT);
            }
            else
                SET_UD32(*pCode, (GET_UD32(pCode[1]) / GET_UD32(pCode[2])));
            pCode += 3;
            break;

        case T5C_0_GTUDINT : /* <ifnolock> UDINT: a := b > c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTUDINT : /* UDINT: a := b > c */
            SET_D8(*pCode, GET_UD32(pCode[1]) > GET_UD32(pCode[2]));
            pCode += 3;
            break;

        case T5C_0_GEUDINT : /* <ifnolock> UDINT: a := b >= c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GEUDINT : /* UDINT: a := b >= c */
            SET_D8(*pCode, GET_UD32(pCode[1]) >= GET_UD32(pCode[2]));
            pCode += 3;
            break;

#endif /*T5DEF_UDINTSUPPORTED*/

        /* REAL operations **************************************************/

#ifdef T5DEF_REALSUPPORTED

        case T5C_O_NEGREAL : /* <ifnolock> REAL: a := - b */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_NEGREAL : /* REAL: a := - b */
            SET_REAL(*pCode, -(GET_REAL(pCode[1])));
            pCode += 2;
            break;

        case T5C_O_MULREAL : /* <ifnolock> REAL: a := b * c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_MULREAL : /* REAL: a := b * c */
            SET_REAL(*pCode, (GET_REAL(pCode[1]) * GET_REAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_DIVREAL : /* <ifnolock> REAL: a := b / c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_DIVREAL : /* REAL: a := b / c */
            if (!GET_REAL(pCode[2]))
            {
                T5_DIAGHOOK (T5_DIAGHOOK_DIVZERO, 0);
                T5VMLog_Push (pDB, T5RET_DIVZERO, pCode[2], ERRDATA);
                SET_REAL(*pCode, T5_MAXREAL);
            }
            else
                SET_REAL(*pCode, (GET_REAL(pCode[1]) / GET_REAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_ADDREAL : /* <ifnolock> REAL: a := b + c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ADDREAL : /* REAL: a := b + c */
            SET_REAL(*pCode, (GET_REAL(pCode[1]) + GET_REAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_SUBREAL : /* <ifnolock> REAL: a := b - c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_SUBREAL : /* REAL: a := b - c */
            SET_REAL(*pCode, (GET_REAL(pCode[1]) - GET_REAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_GTREAL : /* <ifnolock> REAL: a := b > c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTREAL : /* REAL: a := b > c */
            SET_D8(*pCode, (GET_REAL(pCode[1]) > GET_REAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_GEREAL : /* <ifnolock> REAL: a := b >= c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GEREAL : /* REAL: a := b >= c */
            SET_D8(*pCode, (GET_REAL(pCode[1]) >= GET_REAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_EQREAL : /* <ifnolock> REAL: a := b == c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_EQREAL : /* REAL: a := b == c */
            SET_D8(*pCode, (GET_REAL(pCode[1]) == GET_REAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_NEREAL : /* <ifnolock> REAL: a := b != c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_NEREAL : /* REAL: a := b != c */
            SET_D8(*pCode, (GET_REAL(pCode[1]) != GET_REAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_COPYREAL : /* <ifnolock> REAL: a := b */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_COPYREAL : /* REAL: a := b */
            SET_REAL(*pCode, GET_REAL(pCode[1]));
            pCode += 2;
            break;


        case T5C_O_AGETREAL : /* <ifnolock> REAL: a := b [ c ] */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARGETREAL : /* REAL: a := b [ c ] */
            SET_REAL(*pCode, GET_REAL(pCode[1] + GET_DINT(pCode[2])));
            pCode += 3;
            break;
        case T5C_O_APUTREAL : /* <ifnolock> REAL: a [ b ] := c */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARPUTREAL : /* REAL: a [ b ] := c */
            SET_REAL((*pCode + GET_DINT(pCode[1])), GET_REAL(pCode[2]));
            pCode += 3;
            break;

#endif /*T5DEF_REALSUPPORTED*/

        /* DATA64 operations *********************************************/

#ifdef T5DEF_DATA64SUPPORTED

        case T5C_O_SETLINT : /* <ifnolock> LINT/LREAL: force 64 bit value */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 5;
                break;
            }
        case T5C_SETLINT : /* LINT/LREAL: force 64 bit value */
            if (*pCode != 0)
            {
                T5_COPYWORDSTO64((T5_PTBYTE)(p64+*pCode), (T5_PTBYTE)(pCode+1));
                _SmLock64 (pDB, *pCode, GET_DATA64(*pCode));
            }
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5_COPYWORDSTO64(pCTSeg+dwCTOffset, (T5_PTBYTE)(pCode+1));
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
            pCode += 5;
            break;
        case T5C_LOCKLINT : /* LINT/LREAL: lock 64 bit variable */
            T5VMTic_LockVar (pDB, *pCode, T5LOCK_DATA64, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_RegisterLockVar (pDB, T5C_LINT, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_RegisterLockVar (pDB, T5C_LINT, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;
        case T5C_UNLOCKLINT : /* LINT/LREAL: unlock 64 bit variable */
            T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_DATA64, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_UnRegisterLockVar (pDB, T5C_LINT, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_UnRegisterLockVar (pDB, T5C_LINT, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;

#endif /*T5DEF_DATA64SUPPORTED*/

        /* LINT operations **************************************************/

#ifdef T5DEF_LINTSUPPORTED

        case T5C_O_NEGLINT : /* <ifnolock> LINT: a := - b */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_NEGLINT : /* LINT: a := - b */
            SET_LINT(*pCode, -(GET_LINT(pCode[1])));
            pCode += 2;
            break;

        case T5C_O_MULLINT : /* <ifnolock> LINT: a := b * c */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_MULLINT : /* LINT: a := b * c */
            SET_LINT(*pCode, (GET_LINT(pCode[1]) * GET_LINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_DIVLINT : /* <ifnolock> LINT: a := b / c */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_DIVLINT : /* LINT: a := b / c */
            if (!GET_LINT(pCode[2]))
            {
                T5_DIAGHOOK (T5_DIAGHOOK_DIVZERO, 0);
                T5VMLog_Push (pDB, T5RET_LDIVZERO, pCode[2], ERRDATA);
                SET_LINT(*pCode, T5_MAXLINT);
            }
            else
                SET_LINT(*pCode, (GET_LINT(pCode[1]) / GET_LINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_ADDLINT : /* <ifnolock> LINT: a := b + c */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ADDLINT : /* LINT: a := b + c */
            SET_LINT(*pCode, (GET_LINT(pCode[1]) + GET_LINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_SUBLINT : /* <ifnolock> LINT: a := b - c */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_SUBLINT : /* LINT: a := b - c */
            SET_LINT(*pCode, (GET_LINT(pCode[1]) - GET_LINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_GTLINT : /* <ifnolock> LINT: a := b > c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTLINT : /* LINT: a := b > c */
            SET_D8(*pCode, (GET_LINT(pCode[1]) > GET_LINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_GELINT : /* <ifnolock> LINT: a := b >= c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GELINT : /* LINT: a := b >= c */
            SET_D8(*pCode, (GET_LINT(pCode[1]) >= GET_LINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_EQLINT : /* <ifnolock> LINT: a := b == c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_EQLINT : /* LINT: a := b == c */
            SET_D8(*pCode, (GET_LINT(pCode[1]) == GET_LINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_NELINT : /* <ifnolock> LINT: a := b != c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_NELINT : /* LINT: a := b != c */
            SET_D8(*pCode, (GET_LINT(pCode[1]) != GET_LINT(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_COPYLINT : /* <ifnolock> LINT: a := b */
            IF_ISD64LOCKED(*pCode) /*SMARTLOCK*/
            {
                _SmLock64 (pDB, *pCode, GET_DATA64(pCode[1]));
                pCode += 2;
                break;
            }
        case T5C_COPYLINT : /* LINT: a := b */
            SET_LINT(*pCode, GET_LINT(pCode[1]));
            pCode += 2;
            break;


        case T5C_O_ARGETLINT : /* <ifnolock> LINT: a := b [ c ] */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARGETLINT : /* LINT: a := b [ c ] */
            SET_LINT(*pCode, GET_LINT(pCode[1] + GET_DINT(pCode[2])));
            pCode += 3;
            break;
        case T5C_O_ARPUTLINT : /* <ifnolock> LINT: a [ b ] := c */
            IF_ISD64LOCKED((*pCode + GET_DINT(pCode[1]))) /*SMARTLOCK*/
            {
                _SmLock64 (pDB, (T5_WORD)(*pCode + GET_DINT(pCode[1])), GET_DATA64(pCode[2]));
                pCode += 3;
                break;
            }
        case T5C_ARPUTLINT : /* LINT: a [ b ] := c */
            SET_LINT((*pCode + GET_DINT(pCode[1])), GET_LINT(pCode[2]));
            pCode += 3;
            break;

#endif /*T5DEF_LINTSUPPORTED*/

        /* ULINT operations *************************************************/

#ifdef T5DEF_ULINTSUPPORTED

        case T5C_0_MULULINT : /* <ifnolock> ULINT: a := b * c */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_MULULINT : /* ULINT: a := b * c */
            SET_UD64(*pCode, GET_UD64(pCode[1]) * GET_UD64(pCode[2]));
            pCode += 3;
            break;

        case T5C_0_DIVULINT : /* <ifnolock> ULINT: a := b / c */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_DIVULINT : /* ULINT: a := b / c */
            if (!GET_UD64(pCode[2]))
            {
                T5_DIAGHOOK (T5_DIAGHOOK_DIVZERO, 0);
                T5VMLog_Push (pDB, T5RET_DIVZERO, pCode[2], ERRDATA);
                SET_UD64(*pCode, T5_MAXULINT);
            }
            else
                SET_UD64(*pCode, (GET_UD64(pCode[1]) / GET_UD64(pCode[2])));
            pCode += 3;
            break;

        case T5C_0_GTULINT : /* <ifnolock> ULINT: a := b > c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTULINT : /* ULINT: a := b > c */
            SET_D8(*pCode, GET_UD64(pCode[1]) > GET_UD64(pCode[2]));
            pCode += 3;
            break;

        case T5C_0_GEULINT : /* <ifnolock> ULINT: a := b >= c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GEULINT : /* ULINT: a := b >= c */
            SET_D8(*pCode, GET_UD64(pCode[1]) >= GET_UD64(pCode[2]));
            pCode += 3;
            break;

#endif /*T5DEF_ULINTSUPPORTED*/

        /* LREAL operations *************************************************/

#ifdef T5DEF_LREALSUPPORTED

        case T5C_O_NEGLREAL : /* <ifnolock> LREAL: a := - b */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_NEGLREAL : /* LREAL: a := - b */
            SET_LREAL(*pCode, -(GET_LREAL(pCode[1])));
            pCode += 2;
            break;

        case T5C_O_MULLREAL : /* <ifnolock> LREAL: a := b * c */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_MULLREAL : /* LREAL: a := b * c */
            SET_LREAL(*pCode, (GET_LREAL(pCode[1]) * GET_LREAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_DIVLREAL : /* <ifnolock> LREAL: a := b / c */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_DIVLREAL : /* LREAL: a := b / c */
            if (!GET_LREAL(pCode[2]))
            {
                T5_DIAGHOOK (T5_DIAGHOOK_DIVZERO, 0);
                T5VMLog_Push (pDB, T5RET_LDIVZERO, pCode[2], ERRDATA);
                SET_LREAL(*pCode, T5_MAXLREAL);
            }
            else
                SET_LREAL(*pCode, (GET_LREAL(pCode[1]) / GET_LREAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_ADDLREAL : /* <ifnolock> LREAL: a := b + c */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ADDLREAL : /* LREAL: a := b + c */
            SET_LREAL(*pCode, (GET_LREAL(pCode[1]) + GET_LREAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_SUBLREAL : /* <ifnolock> LREAL: a := b - c */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_SUBLREAL : /* LREAL: a := b - c */
            SET_LREAL(*pCode, (GET_LREAL(pCode[1]) - GET_LREAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_GELREAL : /* <ifnolock> LREAL: a := b > c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTLREAL : /* LREAL: a := b > c */
            SET_D8(*pCode, (GET_LREAL(pCode[1]) > GET_LREAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_GTLREAL : /* <ifnolock> LREAL: a := b >= c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GELREAL : /* LREAL: a := b >= c */
            SET_D8(*pCode, (GET_LREAL(pCode[1]) >= GET_LREAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_EQLREAL : /* <ifnolock> LREAL: a := b == c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_EQLREAL : /* LREAL: a := b == c */
            SET_D8(*pCode, (GET_LREAL(pCode[1]) == GET_LREAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_NELREAL : /* <ifnolock> LREAL: a := b != c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_NELREAL : /* LREAL: a := b != c */
            SET_D8(*pCode, (GET_LREAL(pCode[1]) != GET_LREAL(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_COPYLREAL : /* <ifnolock> LREAL: a := b */
            IF_ISD64LOCKED(*pCode)
            {
                _SmLock64 (pDB, *pCode, GET_DATA64(pCode[1]));
                pCode += 2;
                break;
            }
        case T5C_COPYLREAL : /* LREAL: a := b */
            SET_LREAL(*pCode, GET_LREAL(pCode[1]));
            pCode += 2;
            break;


        case T5C_O_ARGETLREAL : /* <ifnolock> LREAL: a := b [ c ] */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARGETLREAL : /* LREAL: a := b [ c ] */
            SET_LREAL(*pCode, GET_LREAL(pCode[1] + GET_DINT(pCode[2])));
            pCode += 3;
            break;
        case T5C_O_ARPUTLREAL : /* <ifnolock> LREAL: a [ b ] := c */
            IF_ISD64LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARPUTLREAL : /* LREAL: a [ b ] := c */
            SET_LREAL((*pCode + GET_DINT(pCode[1])), GET_LREAL(pCode[2]));
            pCode += 3;
            break;

#endif /*T5DEF_LREALSUPPORTED*/

        /* TIME operations **************************************************/

        case T5C_O_ADDTIME : /* <ifnolock> TIME: a := b + c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ADDTIME : /* TIME: a := b + c */
            SET_TIME(*pCode, (GET_TIME(pCode[1]) + GET_TIME(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_SUBTIME : /* <ifnolock> TIME: a := b - c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_SUBTIME : /* TIME: a := b - c */
            if (GET_TIME(pCode[1]) < GET_TIME(pCode[2]))
            {
                SET_TIME(*pCode, 0L);
            }
            else
            {
                SET_TIME(*pCode, (GET_TIME(pCode[1]) - GET_TIME(pCode[2])));
            }
            pCode += 3;
            break;

        case T5C_O_GTTIME : /* <ifnolock> TIME: a := b > c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTTIME : /* TIME: a := b > c */
            SET_D8(*pCode, (GET_TIME(pCode[1]) > GET_TIME(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_GETIME : /* <ifnolock> TIME: a := b >= c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GETIME : /* TIME: a := b >= c */
            SET_D8(*pCode, (GET_TIME(pCode[1]) >= GET_TIME(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_EQTIME : /* <ifnolock> TIME: a := b == c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_EQTIME : /* TIME: a := b == c */
            SET_D8(*pCode, (GET_TIME(pCode[1]) == GET_TIME(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_NETIME : /* <ifnolock> TIME: a := b != c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_NETIME : /* TIME: a := b != c */
            SET_D8(*pCode, (GET_TIME(pCode[1]) != GET_TIME(pCode[2])));
            pCode += 3;
            break;

        case T5C_O_COPYTIME : /* <ifnolock> TIME: a := b */
            IF_ISTIMELOCKED(*pCode) /*SMARTLOCK*/
            {
                _SmLockTime (pDB, *pCode, (GET_TIME(pCode[1])));
                pCode += 2;
                break;
            }
        case T5C_COPYTIME : /* TIME: a := b */
            SET_TIME(*pCode, (GET_TIME(pCode[1])));
            pCode += 2;
            break;

        case T5C_O_SETTIME : /* <ifnolock> TIME: force timer */
            IF_ISTIMELOCKED(*pCode)
            {
                pCode += 1;
                break;
            }
        case T5C_SETTIME : /* TIME: force timer */
            wIndex = *pCode;
            dwVal = (T5_DWORD)(pCode[1]) << 16 & 0xffff0000L;
            dwVal |= ((T5_DWORD)(pCode[2]) & 0xffffL);
            if (wIndex)
            {
                ((T5_PTDWORD)pTime)[wIndex] = dwVal;
                _SmLockTime (pDB, *pCode, GET_TIME(wIndex));
            }
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                *(T5_PTDWORD)(pCTSeg+dwCTOffset) = dwVal;
            }
#endif /*T5DEF_CTSEG*/
            pCode += 3;
            break;

        case T5C_O_STARTTIME : /* <ifnolock> TIME: tstart a */
            IF_ISTIMELOCKED(*pCode)
            {
                pCode++;
                break;
            }
        case T5C_STARTTIME : /* TIME: tstart a */
#ifdef T5DEF_ACTIMESUPPORTED
            T5VMTmr_StartTimer (pDB, &(pTime[*pCode]));
#else /*T5DEF_ACTIMESUPPORTED*/
            T5VMLog_Push (pDB, T5RET_CANTACTIME, *pCode, ERRDATA);
            pStatus->wFlags &= ~T5FLAG_RUNMASK;
            pStatus->wFlags |= T5FLAG_ERROR;
            T5HOOK_SETPRG (0);
            return FALSE;
#endif /*T5DEF_ACTIMESUPPORTED*/
            pCode++;
            break;

        case T5C_O_STOPTIME : /* <ifnolock> TIME: tstop a */
            IF_ISTIMELOCKED(*pCode)
            {
                pCode++;
                break;
            }
        case T5C_STOPTIME : /* TIME: tstop a */
#ifdef T5DEF_ACTIMESUPPORTED
            T5VMTmr_StopTimer (pDB, &(pTime[*pCode]));
#else /*T5DEF_ACTIMESUPPORTED*/
            T5VMLog_Push (pDB, T5RET_CANTACTIME, *pCode, ERRDATA);
            pStatus->wFlags &= ~T5FLAG_RUNMASK;
            pStatus->wFlags |= T5FLAG_ERROR;
            T5HOOK_SETPRG (0);
            return FALSE;
#endif /*T5DEF_ACTIMESUPPORTED*/
            pCode++;
            break;

        case T5C_LOCKTIME : /* TIME: lock time */
            T5VMTic_LockVar (pDB, *pCode, T5LOCK_TIME, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_RegisterLockVar (pDB, T5C_TIME, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_RegisterLockVar (pDB, T5C_TIME, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;

        case T5C_UNLOCKTIME : /* TIME: unlock time */
            T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_TIME, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_UnRegisterLockVar (pDB, T5C_TIME, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_UnRegisterLockVar (pDB, T5C_TIME, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;

        case T5C_O_AGETTIME : /* <ifnolock> TIME: a := b [ c ] */
            IF_ISTIMELOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARGETTIME : /* TIME: a := b [ c ] */
            SET_TIME(*pCode, GET_TIME(pCode[1] + GET_DINT(pCode[2])));
            pCode += 3;
            break;
        case T5C_O_APUTTIME : /* <ifnolock> TIME: a [ b ] := c */
            IF_ISTIMELOCKED((*pCode + GET_DINT(pCode[1]))) /*SMARTLOCK*/
            {
                _SmLockTime (pDB, (T5_WORD)(*pCode + GET_DINT(pCode[1])), GET_TIME(pCode[2]));
                pCode += 3;
                break;
            }
        case T5C_ARPUTTIME : /* TIME: a [ b ] := c */
            SET_TIME((*pCode + GET_DINT(pCode[1])), GET_TIME(pCode[2]));
            pCode += 3;
            break;

        /* standard functions **********************************************/

        case T5C_STDFC : /* CALL: std function: no + nbp + out + inputs */
            if (!T5VMStf_Execute (pDB, pCode))
            {
                pStatus->wFlags &= ~T5FLAG_RUNMASK;
                pStatus->wFlags |= T5FLAG_ERROR;
                T5HOOK_SETPRG (0);
                return FALSE;
            }
            pCode += (pCode[1] + 2);
            break;

        /* cast ************************************************************/

        case T5C_O_CAST : /* <ifnolock> CALL: cast: a <- b c=dsttype d=dsttype; */
#ifdef T5DEF_DONTFORCELOCKEDVARS
            bSkip = FALSE;
            switch (pCode[2])
            {
            case T5C_BOOL   : 
            case T5C_SINT   :
            case T5C_USINT  : bSkip = ISLOCK8(*pCode); break;
            case T5C_INT    :
            case T5C_UINT   : bSkip = ISLOCK16(*pCode); break;
            case T5C_DINT   : 
            case T5C_UDINT  : 
            case T5C_REAL   : bSkip = ISLOCK32(*pCode); break;
            case T5C_TIME   : bSkip = ISLOCKTIME(*pCode); break;
            case T5C_LINT   :
            case T5C_LREAL  : bSkip = ISLOCK64(*pCode); break;
            case T5C_STRING : bSkip = ISLOCKSTRING(*pCode); break;
            default         : break;
            }
            if (bSkip)
            {
                 pCode += 4;
                 break;
            }
#endif /*T5DEF_DONTFORCELOCKEDVARS*/
        case T5C_CAST : /* CALL: cast: a <- b c=dsttype d=dsttype; */
            T5VMCnv_Convert (pDB, pCode);
            pCode += 4;
            break;

        /* function blocks *************************************************/

        case T5C_FBCALL : /* CALL: FB: inst + nbp + inputs + outputs */
		    pI = pInst + *pCode;
			pI->pfHandler (T5FBCMD_ACTIVATE, pDB, pI->pClassData,
							pI->pData, pCode+2);
            pCode += (pCode[1] + 2);
            break;

        /* custom functions ************************************************/

        case T5C_CUSFC : /* CALL: C Function: Fno + nbp + inputs + outputs + outp. type */
		    pF = pFC + *pCode;
			pF->pfHandler (T5FBCMD_ACTIVATE, pDB,
                           T5_PTNULL, T5_PTNULL, pCode+2);
            pCode += (pCode[1] + 3);
            break;

        /* strings *********************************************************/

#ifdef T5DEF_STRINGSUPPORTED

        case T5C_O_COPYSTRING : /* <ifnolock> STRING: a := b */
            IF_ISSTRINGLOCKED(*pCode) /*SMARTLOCK*/
            {
                _SmLockString (pDB, pCode[0], pCode[1]);
                pCode += 2;
                break;
            }
        case T5C_COPYSTRING : /* STRING: a := b */
            T5VMStr_Copy (pDB, pCode[0], pCode[1]);
            pCode += 2;
            break;
        case T5C_O_ADDSTRING : /* <ifnolock> STRING: a := b + c */
            IF_ISSTRINGLOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ADDSTRING : /* STRING: a := b + c */
            T5VMStr_Cat (pDB, pCode[0], pCode[1], pCode[2]);
            pCode += 3;
            break;

        case T5C_O_GTSTRING : /* <ifnolock> STRING: a := b > c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GTSTRING : /* STRING: a := b > c */
            SET_D8(*pCode, T5VMStr_Compare (pDB, pCode[1], pCode[2]) > 0);
            pCode += 3;
            break;
        case T5C_O_GESTRING : /* <ifnolock> STRING: a := b >= c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_GESTRING : /* STRING: a := b >= c */
            SET_D8(*pCode, T5VMStr_Compare (pDB, pCode[1], pCode[2]) >= 0);
            pCode += 3;
            break;
        case T5C_O_EQSTRING : /* <ifnolock> STRING: a := b == c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_EQSTRING : /* STRING: a := b == c */
            SET_D8(*pCode, T5VMStr_Compare (pDB, pCode[1], pCode[2]) == 0);
            pCode += 3;
            break;
        case T5C_O_NESTRING : /* <ifnolock> STRING: a := b != c */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_NESTRING : /* STRING: a := b != c */
            SET_D8(*pCode, T5VMStr_Compare (pDB, pCode[1], pCode[2]) != 0);
            pCode += 3;
            break;

        case T5C_O_AGETSTRING : /* <ifnolock> STRING: a := b [ c ] */
            IF_ISSTRINGLOCKED(*pCode)
            {
                pCode += 3;
                break;
            }
        case T5C_ARGETSTRING : /* STRING: a := b [ c ] */
            T5VMStr_Copy (pDB, pCode[0],
                          (T5_WORD)(pCode[1]+GET_DINT(pCode[2])));
            pCode += 3;
            break;
        case T5C_O_APUTSTRING : /* <ifnolock> STRING: a [ b ] := c */
            IF_ISSTRINGLOCKED((T5_WORD)(pCode[0]+GET_DINT(pCode[1]))) /*SMARTLOCK*/
            {
                _SmLockString (pDB, (T5_WORD)(pCode[0]+GET_DINT(pCode[1])), pCode[2]);
                pCode += 3;
                break;
            }
        case T5C_ARPUTSTRING : /* STRING: a [ b ] := c */
            T5VMStr_Copy (pDB, (T5_WORD)(pCode[0]+GET_DINT(pCode[1])),
                          pCode[2]);
            pCode += 3;
            break;

        case T5C_LOCKSTRING : /* STRING: lock a */
            T5VMTic_LockVar (pDB, *pCode, T5LOCK_STRING, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_RegisterLockVar (pDB, T5C_STRING, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_RegisterLockVar (pDB, T5C_STRING, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;
        case T5C_UNLOCKSTRING : /* STRING: unlock a */
            T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_STRING, dwCTOffset);
#ifdef T5DEF_SMARTLOCK
            if (*pCode)
                T5VMLog_UnRegisterLockVar (pDB, T5C_STRING, *pCode, 0L);
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMLog_UnRegisterLockVar (pDB, T5C_STRING, 0, dwCTOffset);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_SMARTLOCK*/
            pCode ++;
            break;
        case T5C_O_SETSTRING : /* <ifnolock> STRING: for with constant */
            IF_ISD8LOCKED(*pCode)
            {
                pCode += (pCode[1] + 2);  /* index + len + 1 word per char */
                break;
            }
        case T5C_SETSTRING : /* STRING: for with constant */
            if (*pCode != 0)
            {
                T5VMStr_DBForce (pDB, *pCode, pCode + 1);
                _SmLockString (pDB, *pCode, *pCode);
            }
#ifdef T5DEF_CTSEG
            else if (dwCTOffset != 0xffffffffL)
            {
                T5VMStr_DBForcePtr (pCTSeg+dwCTOffset, pCode + 1);
                dwCTOffset = 0xffffffffL;
            }
#endif /*T5DEF_CTSEG*/
            pCode += (pCode[1] + 2);  /* index + len + 1 word per char */
            break;

#endif /*T5DEF_STRINGSUPPORTED*/

        /* SFC steps and transitions ***************************************/

#ifdef T5DEF_SFC

        case T5C_TRSASSIGN : /* SFC: transition = boolean */
            pTrans[*pCode].wValid = GET_D8(pCode[1]);
            pCode += 2;
            break;

        case T5C_TRANSSOURCE1 : /* SFC: eval source transition - simple */
            if (*pPrgStatus == T5FLAG_PRGTOSTART)
            {
                T5TRACEDW("Trans Source - Prg", wPrgNo);
                T5TRACEDW("   Step = ", pCode[1]);
                *pPrgStatus = T5FLAG_PRGACTIVE;
                pStep[pCode[1]].wStatus = T5STEP_P1;
                pStep[pCode[1]].dwAcTime = 0L;
            }
            pCode += 2;
            break;

        case T5C_TRANSSOURCE : /* SFC: eval source transition - divs */
            if (*pPrgStatus == T5FLAG_PRGTOSTART)
            {
                *pPrgStatus = T5FLAG_PRGACTIVE;
                for (wIndex=0; wIndex<pCode[1]; wIndex++)
                    pStep[pCode[2+wIndex]].wStatus = T5STEP_P1;
            }
            pCode += (2 + pCode[1]);
            break;

        case T5C_TRANSEVAL1 : /* SFC: eval transition - simple */
            if (pStep[pCode[2]].wStatus != T5STEP_N
                || (*pPrgStatus & T5FLAG_PRGTOSTOP) != 0)
            {
                pTrans[pCode[1]].wValid = 0;
                pCode = (T5_PTWORD)((T5_DWORD)pCode + (T5_SHORT)(pCode[0]));
                pCode += 1;
            }
            else
            {
                pTrans[pCode[1]].wValid = 1;
                pCode += 3;
            }


            break;

        case T5C_TRANSEVAL : /* SFC: eval source transition - div */
            pTrans[pCode[1]].wValid = ((*pPrgStatus & T5FLAG_PRGTOSTOP) == 0);
            wIndex = 0;
            while (pTrans[pCode[1]].wValid != 0 && wIndex < pCode[2])
            {
                if (pStep[pCode[3+wIndex]].wStatus != T5STEP_N)
                    pTrans[pCode[1]].wValid = 0;
                wIndex++;
            }
            if (pTrans[pCode[1]].wValid == 0)
            {
                pCode = (T5_PTWORD)((T5_DWORD)pCode + (T5_SHORT)(pCode[0]));
                pCode += 1;
            }
            else
                pCode += (3 + pCode[2]);
            break;

        case T5C_TRANSCROSS1 : /* SFC: cross source transition - simple */
            if (pTrans[*pCode].wValid)
            {
                pStep[pCode[1]].wStatus = T5STEP_P0;
                pStep[pCode[2]].wStatus = T5STEP_P1;
                if (pTrans[*pCode].wBreak)
                {
                    pStatus->wFlags &= ~T5FLAG_RUN;
                    pStatus->wFlags &= ~T5FLAG_PROGSTEP;
                    T5HOOK_SETPRG (0);
                    return TRUE;
                }
            }
            pCode += 3;
            break;

        case T5C_TRANSCROSSP0 : /* SFC: cross source transition - div - P0 */
            if (pTrans[*pCode].wValid)
            {
                for (wIndex=0; wIndex<pCode[1]; wIndex++)
                    pStep[pCode[wIndex+2]].wStatus = T5STEP_P0;
            }
            pCode += (2 + pCode[1]);
            break;
        case T5C_TRANSCROSSP1 : /* SFC: cross source transition - div - P1 */
            if (pTrans[*pCode].wValid)
            {
                for (wIndex=0; wIndex<pCode[1]; wIndex++)
                    pStep[pCode[wIndex+2]].wStatus = T5STEP_P1;
                if (pTrans[*pCode].wBreak)
                {
                    pStatus->wFlags &= ~T5FLAG_RUN;
                    pStatus->wFlags &= ~T5FLAG_PROGSTEP;
                    T5HOOK_SETPRG (0);
                    return TRUE;
                }
            }
            pCode += (2 + pCode[1]);
            break;

        case T5C_STEP : /* SFC: execute step actions */
            if (pStep[*pCode].wStatus == 0)
            {
                pCode = (T5_PTWORD)((T5_DWORD)pCode + (T5_SHORT)(pCode[3]));
                pCode += 4;
            }
            else if (pStep[*pCode].wStatus == T5STEP_P0
               || ((*pPrgStatus & T5FLAG_PRGTOSTOP) != 0
                  && (pStep[*pCode].wStatus == T5STEP_N)))
            {
                pStep[*pCode].wStatus = 0; /* inactive */
                pCode = (T5_PTWORD)((T5_DWORD)pCode + (T5_SHORT)(pCode[2]));
                pCode += 3;
            }
            else if (pStep[*pCode].wStatus == T5STEP_P1)
            {
                pStep[*pCode].dwAcTime = 0L;
                pStep[*pCode].dwLastUpdate = pStatus->dwTimStamp;
                pStep[*pCode].wStatus = T5STEP_N;
                pCode = (T5_PTWORD)((T5_DWORD)pCode + (T5_SHORT)(pCode[1]));
                pCode += 2;
            }
            else /* update time and continue with action N */
            {
                if (pStep[*pCode].dwLastUpdate != pStatus->dwTimStamp)
                {
                    pStep[*pCode].dwAcTime = 
                        ((T5_24HMS + pStep[*pCode].dwAcTime + 
                            pStatus->dwTimStamp - pStep[*pCode].dwLastUpdate)
                            % T5_24HMS);
                    pStep[*pCode].dwLastUpdate = pStatus->dwTimStamp;
                }
                pCode += 4;
            }
            break;

        case T5C_STEPFLAG : /* <ifnolock> SFC: get step time: timedst, step */
            if (!ISLOCKTIME(*pCode))
            {
                if (pStep[pCode[1]].wStatus == T5STEP_N)
                {
                    pStep[pCode[1]].dwAcTime = 
                        ((T5_24HMS + pStep[pCode[1]].dwAcTime + 
                            pStatus->dwTimStamp - pStep[pCode[1]].dwLastUpdate)
                            % T5_24HMS);
                    pStep[pCode[1]].dwLastUpdate = pStatus->dwTimStamp;
                }
                SET_TIME(*pCode, pStep[pCode[1]].dwAcTime);
            }
            pCode += 2;
            break;

        case T5C_STEPTIME : /* SFC: get step time: booldst, step */
            if (!ISLOCK8(*pCode))
                SET_D8(*pCode, (pStep[pCode[1]].wStatus != 0));
            pCode += 2;
            break;

        case T5C_SELFPRGCONTROL : /* SFC: child program self control */
            if ((*pPrgStatus & T5FLAG_PRGTOSTOP) != 0)
            {
                for (wIndex=0; wIndex<*pCode; wIndex++)
                {
                    if ((pProg[pCode[wIndex+1]].wFlags & T5FLAG_PRGACTIVE) != 0)
                        pProg[pCode[wIndex+1]].wFlags |= T5FLAG_PRGTOSTOP;
                }
                *pPrgStatus = 0;
            }
            T5HOOK_SETPRG (0);
            return TRUE; /* end of SFC program code */

        case T5C_SFCBKS : /* SFC: set step bkp - index - style */
            T5VMTic_SetSfcBreakS (pDB, *pCode, pCode[1]);
            pCode += 2;
            break;

        case T5C_SFCBKT : /* SFC: set trans bkp - index - style */
            T5VMTic_SetSfcBreakT (pDB, *pCode, pCode[1]);
            pCode += 2;
            break;

#endif /*T5DEF_SFC*/

        /* program control *************************************************/

        case T5C_POUSTART : /* MISC: start a program */        
            T5TRACEDW("GStart", *pCode);
            if (pProg[*pCode].wFlags == 0)
            {
                if (pProg[*pCode].wStyle == T5PRGSTYLE_DEFAULT)
                {
                    pProg[*pCode].wFlags = T5FLAG_PRGACTIVE;
                }
                else
                {
                    pProg[*pCode].wFlags = T5FLAG_PRGTOSTART;
                }
            }
            pCode++;
            break;
        case T5C_POUSTOP : /* MISC: stop a program */
            T5TRACEDW("GKill", *pCode);
            if ((pProg[*pCode].wFlags & T5FLAG_PRGACTIVE) != 0)
            {
                pProg[*pCode].wFlags &= ~T5FLAG_PRGSUSPENDED;
                if (pProg[*pCode].wStyle == T5PRGSTYLE_DEFAULT)
                    pProg[*pCode].wFlags = 0;
                else pProg[*pCode].wFlags |= T5FLAG_PRGTOSTOP;
            }
            pCode++;
            break;
        case T5C_POUSUSPEND : /* MISC: suspend a program */
            if (pProg[*pCode].wFlags == T5FLAG_PRGACTIVE)
                pProg[*pCode].wFlags |= T5FLAG_PRGSUSPENDED;
            pCode++;
            break;
        case T5C_POURESUME : /* MISC: restart a program */
            if ((pProg[*pCode].wFlags & T5FLAG_PRGACTIVE) != 0
                && (pProg[*pCode].wFlags & T5FLAG_PRGSUSPENDED) != 0)
                pProg[*pCode].wFlags &= ~T5FLAG_PRGSUSPENDED;
            pCode++;
            break;

        case T5C_O_POUSTATUS : /* <ifnolock> MISC: get program status */
            IF_ISD32LOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_POUSTATUS : /* MISC: get program status */
            if (pProg[pCode[1]].wFlags == T5FLAG_PRGACTIVE)
            {
                SET_DINT(*pCode, 1L);
            }
            else if ((pProg[pCode[1]].wFlags & T5FLAG_PRGSUSPENDED) != 0)
            {
                SET_DINT(*pCode, 2L);
            }
            else
            {
                SET_DINT(*pCode, 0L);
            }
            pCode += 2;
            break;

        /* external variables **********************************************/

#ifdef T5DEF_XV

        case T5C_O_XVGET8 : /* <ifnolock> BOOL/SINT: get external var: xv, index */
            IF_ISD8LOCKED(pCode[1])
            {
                pCode += 2;
                break;
            }
        case T5C_XVGET8 : /* BOOL/SINT: get external var: xv, index */
            SET_D8(pCode[1], *(pXV[*pCode]));
            pCode += 2;
            break;

        case T5C_O_XVSET8 : /* <ifnolock> BOOL/SINT: set external var: xv, index */
            IF_ISXVLOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_XVSET8 : /* BOOL/SINT: set external var: xv, index */
            *(pXV[*pCode]) = GET_D8(pCode[1]);
            pCode += 2;
            break;

        case T5C_O_XAGET8 : /* <ifnolock> BOOL/SINT: get external array: xv, index, array index */
            IF_ISD8LOCKED(pCode[1])
            {
                pCode += 3;
                break;
            }
        case T5C_XAGET8 : /* BOOL/SINT: get external array: xv, index, array index */
            SET_D8(pCode[1], *(pXV[*pCode + GET_DINT(pCode[2])]));
            pCode += 3;
            break;

        case T5C_O_XASET8 : /* <ifnolock> BOOL/SINT: set external array: xv, index, array index */
            IF_ISXVLOCKED(*pCode + GET_DINT(pCode[2])) /*SMARTLOCK*/
            {
                _SmLock8xa (pDB, (T5_WORD)(*pCode + GET_DINT(pCode[2])), GET_D8(pCode[1]));
                pCode += 3;
                break;
            }
        case T5C_XASET8 : /* BOOL/SINT: set external array: xv, index, array index */
            *(pXV[*pCode + GET_DINT(pCode[2])]) = GET_D8(pCode[1]);
            pCode += 3;
            break;

        case T5C_XVF8 : /* BOOL/SINT: force external var with 8 bit constant value */
            *(pXV[*pCode]) = (T5_BYTE)(pCode[1] & 0xff);
            pCode += 2;
            break;

#ifdef T5DEF_DATA16SUPPORTED
        case T5C_O_XVGET16 : /* <ifnolock> INT: get external var: xv, index */
            IF_ISD16LOCKED(pCode[1])
            {
                pCode += 2;
                break;
            }
        case T5C_XVGET16 : /* INT: get external var: xv, index */
            SET_D16(pCode[1], *(T5_PTSHORT)(pXV[*pCode]));
            pCode += 2;
            break;

        case T5C_O_XVSET16 : /* <ifnolock> INT: set external var: xv, index */
            IF_ISXVLOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_XVSET16 : /* INT: set external var: xv, index */
            *(T5_PTSHORT)(pXV[*pCode]) = GET_D16(pCode[1]);
            pCode += 2;
            break;

        case T5C_O_XAGET16 : /* <ifnolock> INT: get external array: xv, index, array index */
            IF_ISD16LOCKED(pCode[1])
            {
                pCode += 3;
                break;
            }
        case T5C_XAGET16 : /* INT: get external array: xv, index, array index */
            SET_D16(pCode[1], *(T5_PTSHORT)(pXV[*pCode + GET_DINT(pCode[2])]));
            pCode += 3;
            break;

        case T5C_O_XASET16 : /* <ifnolock> INT: set external array: <ifnolock> xv, index, array index */
            IF_ISXVLOCKED(*pCode + GET_DINT(pCode[2])) /*SMARTLOCK*/
            {
                _SmLock16xa (pDB, (T5_WORD)(*pCode + GET_DINT(pCode[2])), GET_D16(pCode[1]));
                pCode += 3;
                break;
            }
        case T5C_XASET16 : /* INT: set external array: xv, index, array index */
            *(T5_PTSHORT)(pXV[*pCode + GET_DINT(pCode[2])]) = GET_D16(pCode[1]);
            pCode += 3;
            break;

        case T5C_XVF16 : /* INT: force external var with 16 bit constant value */
            *(T5_PTSHORT)(pXV[*pCode]) = (T5_SHORT)(pCode[1]);
            pCode += 2;
            break;

#endif /*T5DEF_DATA16SUPPORTED*/

        case T5C_O_XVGET32 : /* <ifnolock> DINT/REAL: get external var: xv, index */
            IF_ISD32LOCKED(pCode[1])
            {
                pCode += 2;
                break;
            }
        case T5C_XVGET32 : /* DINT/REAL: get external var: xv, index */
            SET_DINT(pCode[1], *(T5_PTLONG)(pXV[*pCode]));
            pCode += 2;
            break;

        case T5C_O_XVSET32 : /* <ifnolock> DINT/REAL: set external var: xv, index */
            IF_ISXVLOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_XVSET32 : /* DINT/REAL: set external var: xv, index */
            *(T5_PTLONG)(pXV[*pCode]) = GET_DINT(pCode[1]);
            pCode += 2;
            break;

        case T5C_O_XAGET32 : /* <ifnolock> DINT/REAL: get external array: xv, index, array index */
            IF_ISD32LOCKED(pCode[1])
            {
                pCode += 3;
                break;
            }
        case T5C_XAGET32 : /* DINT/REAL: get external array: xv, index, array index */
            SET_DINT(pCode[1], *(T5_PTLONG)(pXV[*pCode + GET_DINT(pCode[2])]));
            pCode += 3;
            break;

        case T5C_O_XASET32 : /* <ifnolock> DINT/REAL: set external array: xv, index, array index */
            IF_ISXVLOCKED(*pCode + GET_DINT(pCode[2])) /*SMARTLOCK*/
            {
                _SmLock32xa (pDB, (T5_WORD)(*pCode + GET_DINT(pCode[2])), GET_DINT(pCode[1]));
                pCode += 3;
                break;
            }
        case T5C_XASET32 : /* DINT/REAL: set external array: xv, index, array index */
            *(T5_PTLONG)(pXV[*pCode + GET_DINT(pCode[2])]) = GET_DINT(pCode[1]);
            pCode += 3;
            break;

        case T5C_XVF32 : /* DINT/REAL: force external var with 32 bit constant value */
        case T5C_XVFTIME : /* TIME: force external var with 32 bit constant value */
            T5_COPYWORDSTOLONG(pXV[*pCode], (T5_PTBYTE)(pCode+1));
            pCode += 3;
            break;

#ifdef T5DEF_DATA64SUPPORTED
        case T5C_O_XVGET64 : /* <ifnolock> LINT/LREAL: get external var: xv, index */
            IF_ISD64LOCKED(pCode[1])
            {
                pCode += 2;
                break;
            }
        case T5C_XVGET64 : /* LINT/LREAL: get external var: xv, index */
            SET_DATA64(pCode[1], *(T5_PTDATA64)(pXV[*pCode]));
            pCode += 2;
            break;

        case T5C_O_XVSET64 : /* <ifnolock> LINT/LREAL: set external var: xv, index */
            IF_ISXVLOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_XVSET64 : /* LINT/LREAL: set external var: xv, index */
            *(T5_PTDATA64)(pXV[*pCode]) = GET_DATA64(pCode[1]);
            pCode += 2;
            break;

        case T5C_O_XAGET64 : /* <ifnolock> LINT/LREAL: get external array: xv, index, array index */
            IF_ISD64LOCKED(pCode[1])
            {
                pCode += 3;
                break;
            }
        case T5C_XAGET64 : /* LINT/LREAL: get external array: xv, index, array index */
            SET_DATA64(pCode[1], *(T5_PTDATA64)(pXV[*pCode + GET_DINT(pCode[2])]));
            pCode += 3;
            break;

        case T5C_O_XASET64 : /* <ifnolock> LINT/LREAL: set external array: xv, index, array index */
            IF_ISXVLOCKED(*pCode + GET_DINT(pCode[2])) /*SMARTLOCK*/
            {
                _SmLock64xa (pDB, (T5_WORD)(*pCode + GET_DINT(pCode[2])), GET_DATA64(pCode[1]));
                pCode += 3;
                break;
            }
        case T5C_XASET64 : /* LINT/LREAL: set external array: xv, index, array index */
            *(T5_PTDATA64)(pXV[*pCode + GET_DINT(pCode[2])]) = GET_DATA64(pCode[1]);
            pCode += 3;
            break;

        case T5C_XVF64 : /* LINT/LREAL: force external var with 64 bit constant value */
            T5_COPYWORDSTO64(pXV[*pCode], (T5_PTBYTE)(pCode+1));
            pCode += 5;
            break;
#endif /*T5DEF_DATA64SUPPORTED*/

        case T5C_O_XVGETTIME : /* <ifnolock> TIME: get external var: xv, index */
            IF_ISTIMELOCKED(pCode[1])
            {
                pCode += 2;
                break;
            }
        case T5C_XVGETTIME : /* TIME: get external var: xv, index */
            SET_TIME(pCode[1], *(T5_PTDWORD)(pXV[*pCode]));
            pCode += 2;
            break;

        case T5C_O_XVSETTIME : /* <ifnolock> TIME: set external var: xv, index */
            IF_ISXVLOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_XVSETTIME : /* TIME: set external var: xv, index */
            *(T5_PTDWORD)(pXV[*pCode]) = GET_TIME(pCode[1]);
            pCode += 2;
            break;

        case T5C_O_XAGETTIME : /* <ifnolock> TIME: get external array: xv, index, array index */
            IF_ISTIMELOCKED(pCode[1])
            {
                pCode += 3;
                break;
            }
        case T5C_XAGETTIME : /* TIME: get external array: xv, index, array index */
            SET_TIME(pCode[1], *(T5_PTDWORD)(pXV[*pCode + GET_DINT(pCode[2])]));
            pCode += 3;
            break;

        case T5C_O_XASETTIME : /* <ifnolock> TIME: set external array: xv, index, array index */
            IF_ISXVLOCKED(*pCode + GET_DINT(pCode[2])) /*SMARTLOCK*/
            {
                _SmLockTimexa (pDB, (T5_WORD)(*pCode + GET_DINT(pCode[2])), GET_TIME(pCode[1]));
                pCode += 3;
                break;
            }
        case T5C_XASETTIME : /* TIME: set external array: xv, index, array index */
            *(T5_PTDWORD)(pXV[*pCode + GET_DINT(pCode[2])]) = GET_TIME(pCode[1]);
            pCode += 3;
            break;

#ifdef T5DEF_STRINGSUPPORTED
        case T5C_O_XVGETSTRING : /* <ifnolock> STRING: get external var: xv, index */
            IF_ISSTRINGLOCKED(pCode[1])
            {
                pCode += 2;
                break;
            }
        case T5C_XVGETSTRING : /* STRING: get external var: xv, index */
            pXS = T5GET_DBSTRING(pDB);
            T5VMStr_CopyPtr (pXS[pCode[1]], pXV[*pCode]);
            pCode += 2;
            break;

        case T5C_O_XVSETSTRING : /* <ifnolock> STRING: set external var: xv, index */
            IF_ISXVLOCKED(*pCode)
            {
                pCode += 2;
                break;
            }
        case T5C_XVSETSTRING : /* STRING: set external var: xv, index */
            pXS = T5GET_DBSTRING(pDB);
            T5VMStr_CopyPtr (pXV[*pCode], pXS[pCode[1]]);
            pCode += 2;
            break;

        case T5C_O_XAGETSTRING : /* <ifnolock> STRING: get external array: xv, index, array index */
            IF_ISSTRINGLOCKED(pCode[1])
            {
                pCode += 3;
                break;
            }
        case T5C_XAGETSTRING : /* STRING: get external array: xv, index, array index */
            pXS = T5GET_DBSTRING(pDB);
            T5VMStr_CopyPtr (pXS[pCode[1]], pXV[*pCode + GET_DINT(pCode[2])]);
            pCode += 3;
            break;

        case T5C_O_XASETSTRING : /* <ifnolock> STRING: set external array: xv, index, array index */
            IF_ISXVLOCKED(*pCode + GET_DINT(pCode[2])) /*SMARTLOCK*/
            {
                _SmLockStringxa (pDB, (T5_WORD)(*pCode + GET_DINT(pCode[2])), pCode[1]);
                pCode += 3;
                break;
            }
        case T5C_XASETSTRING : /* STRING: set external array: xv, index, array index */
            pXS = T5GET_DBSTRING(pDB);
            T5VMStr_CopyPtr (pXV[*pCode + GET_DINT(pCode[2])], pXS[pCode[1]]);
            pCode += 3;
            break;

        case T5C_XVFSTRING : /* STRING: force external var with constant value */
            T5VMStr_DBForcePtr (pXV[*pCode], pCode + 1);
            pCode += (pCode[1] + 2);  /* index + len + 1 word per char */
            break;
#endif /*T5DEF_STRINGSUPPORTED*/

        case T5C_XVLOCK : /* MISC: lock external variable */
            /* SMARTLOCK: not supported */
            pLock[*pCode] |= T5LOCK_XV;
            pCode ++;
            break;
        case T5C_XVUNLOCK : /* MISC: unlock external variable */
            /* SMARTLOCK: not supported */
            pLock[*pCode] &= ~T5LOCK_XV;
            pCode ++;
            break;

#endif /*T5DEF_XV*/

        /*******************************************************************/
        /* sampling trace */

        case T5C_BSAMPLING : /* BSMP: define sampling trace */
                             /* index, flags, periodH, periodL, on, off, */
                             /* ondelay, offdelay, nbvars, vars */
#ifdef T5DEF_BSAMPLING
            T5VMBsmp_Set (pDB, *pCode, pCode+1);
#else /*T5DEF_BSAMPLING*/
            T5VMLog_Push (pDB, T5RET_NOBSAMPLING, 0, 0L);
#endif /*T5DEF_BSAMPLING*/

            /* ignore and continue if sampling not supported */
            pCode += (9 + pCode[8]);
            break;

        case T5C_BSMPCTL : /* BSMP: start or stop sampling trace */
                           /* index, bRun */
#ifdef T5DEF_BSAMPLING
            T5VMBsmp_Ctl (pDB, pCode[0], pCode[1]);
#else /*T5DEF_BSAMPLING*/
            T5VMLog_Push (pDB, T5RET_NOBSAMPLING, 0, 0L);
#endif /*T5DEF_BSAMPLING*/
            pCode += 2;
            break;

        /*******************************************************************/
        /* CTSeg operations */

#ifdef T5DEF_CTSEG

        case T5C_CTSEG_NOP : /* CTSeg FB: no operation */
            break;
        case T5C_O_GETCTSEG : /* CTSeg: get type - dst - reference */
        case T5C_GETCTSEG : /* CTSeg: get type - dst - reference */
            T5VMTic_GetCTSegItem (pDB, pCode[0], pCode[1], pCode[2]);
            pCode += 3;
            break;
        case T5C_O_SETCTSEG : /* CTSeg: put: type - src - reference */
        case T5C_SETCTSEG : /* CTSeg: put: type - src - reference */
#ifdef T5DEF_SMARTLOCK
            if ((pStatus->dwFlagsEx & T5FLAGEX_CTSM) != 0
                && T5VMLog_IsCTLocked (pDB, pCode[0], pCode[1], pCode[2]))
            {
                pCode += 3;
            }
            else
#endif /*T5DEF_SMARTLOCK*/
            {
                T5VMTic_SetCTSegItem (pDB, pCode[0], pCode[1], pCode[2]);
                pCode += 3;
            }
            break;
        case T5C_SETCTSEGREF : /* CTSeg: setref for write: hi - low */
            T5_COPYWORDSTOLONG((T5_PTBYTE)(&dwCTOffset), (T5_PTBYTE)pCode);
            pCode += 2;
            break;
        case T5C_CTSEG_FBCALL : /* CTSeg FB: ref + nbp + inputs + outputs */
            wIndex = *(T5_PTWORD)(pCTSeg + GET_DINT(*pCode));
            pI = pInst + wIndex;
            if (pI == NULL || pI->pfHandler == NULL)
            {
                T5TRACEDW("Invalid FB index encoutered: %u", wIndex);
                T5_PRINTF("Invalid FB index encoutered: %u", wIndex);
                pStatus->wFlags &= ~T5FLAG_RUNMASK;
                pStatus->wFlags |= T5FLAG_ERROR;
                T5HOOK_SETPRG (0);
                return FALSE;
            }
			pI->pfHandler (T5FBCMD_ACTIVATE, pDB, pI->pClassData,
							pI->pData, pCode+2);
            pCode += (pCode[1] + 2);
            break;

        case T5C_CTSEG_FCARG : /* check CTseg arg func: iFC */
		    pF = pFC + *pCode;
			if (pF == NULL || pF->pfHandler == NULL
                || !pF->pfHandler (T5FBCMD_ACCEPTCT, pDB,
                                   T5_PTNULL, T5_PTNULL, T5_PTNULL))
            {
                T5_PRINTF("Argument type not supported by function");
                T5VMLog_Push (pDB, T5RET_UNKNOWNTIC, 0, ERRDATA);
                pStatus->wFlags &= ~T5FLAG_RUNMASK;
                pStatus->wFlags |= T5FLAG_ERROR;
                T5HOOK_SETPRG (0);
                return FALSE;
            }
            pCode++;
            break;
        case T5C_CTSEG_FBARG : /* check CTSeg arg FB: ref */
            wIndex = *(T5_PTWORD)(pCTSeg + GET_DINT(*pCode));
            pI = pInst + wIndex;
            if (pI == NULL || pI->pfHandler == NULL
                || !pI->pfHandler (T5FBCMD_ACCEPTCT, pDB,
                                   T5_PTNULL, T5_PTNULL, T5_PTNULL))
            {
                T5_PRINTF("Argument type not supported by function block");
                T5VMLog_Push (pDB, T5RET_UNKNOWNTIC, 0, ERRDATA);
                pStatus->wFlags &= ~T5FLAG_RUNMASK;
                pStatus->wFlags |= T5FLAG_ERROR;
                T5HOOK_SETPRG (0);
                return FALSE;
            }
            pCode++;
            break;

        case T5C_CTSEG_TSTART : /* start CTSeg timer: ref */
#ifdef T5DEF_ACTIMESUPPORTED
            T5VMTmr_StartTimer (pDB, (T5_PTDWORD)(pCTSeg + GET_DINT(*pCode)));
            pCode++;
#else /*T5DEF_ACTIMESUPPORTED*/
            T5VMLog_Push (pDB, T5RET_CANTACTIME, *pCode, ERRDATA);
            pStatus->wFlags &= ~T5FLAG_RUNMASK;
            pStatus->wFlags |= T5FLAG_ERROR;
            T5HOOK_SETPRG (0);
            return FALSE;
#endif /*T5DEF_ACTIMESUPPORTED*/
            break;
        case T5C_CTSEG_TSTOP : /* stop CTSeg timer: ref */
#ifdef T5DEF_ACTIMESUPPORTED
            T5VMTmr_StopTimer (pDB, (T5_PTDWORD)(pCTSeg + GET_DINT(*pCode)));
            pCode++;
#else /*T5DEF_ACTIMESUPPORTED*/
            T5VMLog_Push (pDB, T5RET_CANTACTIME, *pCode, ERRDATA);
            pStatus->wFlags &= ~T5FLAG_RUNMASK;
            pStatus->wFlags |= T5FLAG_ERROR;
            T5HOOK_SETPRG (0);
            return FALSE;
#endif /*T5DEF_ACTIMESUPPORTED*/
            break;

        case T5C_CTMEMCPY : /* dst - srv - size */
            T5VMTic_CTMemcpy (pDB, pCode[0], pCode[1], pCode[2]);
            pCode += 3;
            break;

#endif /*T5DEF_CTSEG*/

        /* advanced control ************************************************/

        case T5C_EXECTL : /* exec control: command */
            T5VMTic_ExeCtrl (pDB, *pCode);
            pCode++;
            break;

        /* switch table ****************************************************/

        case T5C_SWITCH : /* switch table: i, #nb, abs-jumps */
            wIndex = (T5_WORD)(GET_DINT(*pCode) & 0xffff);
            if (wIndex >= pCode[1])
                pCode += (2 + pCode[1]);
            else
                pCode = (T5_PTWORD)(pStartCode + pCode[2+wIndex]);
            break;

        case T5C_JAEQ : /* jump if EQ: abs-jumps, dint1, dint2 */
            if (GET_DINT(pCode[1]) == GET_DINT(pCode[2]))
                pCode = (T5_PTWORD)(pStartCode + *pCode);
            else
                pCode += 3;
            break;
            
        /* simplified SFC control ******************************************/

#ifdef T5DEF_SFC

        case T5C_JNFPSTAT : /* abs-jmp if not prg stat: flag, dest */
            if ((*pPrgStatus & *pCode) == 0)
                pCode = (T5_PTWORD)(pStartCode + pCode[1]);
            else
                pCode += 2;
            break;

        case T5C_SETPSTAT : /* set prg stat: status */
            *pPrgStatus = *pCode;
            pCode++;
            break;

        case T5C_SFCSETSX : /* set .X: step, value */
            pStep[*pCode].wStatus = pCode[1];
            if (pCode[1] == T5STEP_N) /* reset time on =N */
            {
                pStep[*pCode].dwAcTime = 0L;
                pStep[*pCode].dwLastUpdate = pStatus->dwTimStamp;
            }
            pCode += 2;
            break;

        case T5C_SFCSETST : /* set .T: step, 0=reset | 1=update */
            if (pCode[1]) /* update */
            {
                pStep[*pCode].dwAcTime = 
                    ((T5_24HMS + pStep[*pCode].dwAcTime + 
                        pStatus->dwTimStamp - pStep[*pCode].dwLastUpdate)
                        % T5_24HMS);
                pStep[*pCode].dwLastUpdate = pStatus->dwTimStamp;
            }
            else /* reset */
            {
                pStep[*pCode].dwAcTime = 0L;
                pStep[*pCode].dwLastUpdate = pStatus->dwTimStamp;
            }
            pCode += 2;
            break;

        case T5C_SFCCHECKTBK : /* check trans bkpt: trans */
            if (pTrans[*pCode].wBreak)
            {
                pStatus->wFlags &= ~T5FLAG_RUN;
                pStatus->wFlags &= ~T5FLAG_PROGSTEP;
                T5HOOK_SETPRG (0);
                return TRUE;
            }
            pCode++;
            break;

        case T5C_JNTRANS : /* jump if not trans: trans, asb-dest */
            if (!pTrans[*pCode].wValid)
                pCode = (T5_PTWORD)(pStartCode + pCode[1]);
            else
                pCode += 2;
            break;

        case T5C_SFCTSR : /* SFC test and reset: ASvar, asb-dest */
            if ((*pPrgStatus & T5FLAG_PRGTOSTOP) != 0)
            {
                SET_DINT(*pCode, 0L);
                pCode = (T5_PTWORD)(pStartCode + pCode[1]);
            }
            else
                pCode += 2;
            break;

#endif /*T5DEF_SFC*/

        /* pointers ********************************************************/

        case T5C_ADR : /* dst, type, src, index */
            dwVal = (T5_DWORD)T5Tools_GetAnyParam (
                        pDB, pCode[1],
                        (T5_WORD)(pCode[2] + (T5_WORD)GET_DINT(pCode[3])),
                        NULL, NULL);
            SET_DINT(*pCode, dwVal);
            pCode += 4;
            break;

#ifdef T5DEF_CTSEG

        case T5C_ADRCT : /* dst, ref */
            dwVal = (T5_DWORD)(pDB[T5TB_CTSEG].pData);
            dwVal += GET_DINT(pCode[1]);
            SET_DINT(*pCode, dwVal);
            pCode += 2;
            break;

#endif /*T5DEF_CTSEG*/

        /* maintenance *****************************************************/

        case T5C_UNLOCKALL :
            T5VMTic_UnlockAllVars (pDB);
            break;

#ifdef T5DEF_VARMAP
        case T5C_SETVSISTAMP : /* type, index, dtHI, dtLO, tmHi, tmLo   */
            {
                T5_DWORD dwDate, dwTime;
                T5_COPYWORDSTOLONG ((T5_PTBYTE)(&dwDate), (T5_PTBYTE)(pCode+2));
                T5_COPYWORDSTOLONG ((T5_PTBYTE)(&dwTime), (T5_PTBYTE)(pCode+4));
                T5Map_SetVSIDate (pDB, pCode[0], pCode[1], dwDate);
                T5Map_SetVSITime (pDB, pCode[0], pCode[1], dwTime);
            }
            pCode += 6;
            break;

        case T5C_RESETVSI : /* type, index */
            T5Map_SetVSIDate (pDB, pCode[0], pCode[1], 0L);
            T5Map_SetVSITime (pDB, pCode[0], pCode[1], 0L);
            T5Map_ResetSetVSIBits (pDB, pCode[0], pCode[1]);
            pCode += 2;
            break;


#endif /*T5DEF_VARMAP*/

        /* error: break the cycle ******************************************/

        case T5C_CHECKBOUND : /* MISC: Check array index: index, min, max */
            if (GET_DINT(*pCode) < ((T5_LONG)(pCode[1]) & 0xffffL)
                || GET_DINT(*pCode) > ((T5_LONG)(pCode[2]) & 0xffffL))
            {
                T5_DIAGHOOK (T5_DIAGHOOK_BADINDEX, 0);
                T5TRACEDW("Index out of bounds", GET_DINT(*pCode));
                T5VMLog_Push (pDB, T5RET_ARRAYBOUND, *pCode, ERRDATA);
                pStatus->wFlags &= ~T5FLAG_RUNMASK;
                pStatus->wFlags |= T5FLAG_ERROR;
                T5HOOK_SETPRG (0);
                return FALSE;
            }
            else
                pCode += 3;
            break;

        default :
            T5TRACEDW("TIC: error", *(pCode-1));
            T5VMLog_Push (pDB, T5RET_UNKNOWNTIC, *(pCode-1), ERRDATA);
            if (wPrgNo) /* dont do that for exec buffer! */
            {
                pStatus->wFlags &= ~T5FLAG_RUNMASK;
                pStatus->wFlags |= T5FLAG_ERROR;
            }
            T5HOOK_SETPRG (0);
            return FALSE;
        }
    }
}

/****************************************************************************/

#endif /*T5DEF_NOPCSWITCH*/

/****************************************************************************/
/* CTSeg operations */

#ifdef T5DEF_CTSEG

void T5VMTic_GetCTSegItem (T5PTR_DB pDB, T5_WORD wType,
                           T5_WORD wDst, T5_WORD wRef)
{
    T5_PTDWORD pDint, pTime;
    T5_PTBYTE pData8;
    T5_PTBYTE pCTSeg;
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pData16;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 pData64;
#endif /*T5DEF_DATA64SUPPORTED*/
    T5_PTBYTE pLock;

    pLock = T5GET_DBLOCK(pDB);
    /* get address in CT segment */
    pDint = T5GET_DBDATA32(pDB);
    pCTSeg = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    pCTSeg += pDint[wRef];
    /* copy */
    switch (wType)
    {
    case T5C_BOOL :
        if (!ISLOCK8(wDst))
        {
            pData8 = T5GET_DBDATA8(pDB);
            pData8[wDst] = *pCTSeg;
        }
        break;
    case T5C_INT :
#ifdef T5DEF_DATA16SUPPORTED
        if (!ISLOCK16(wDst))
        {
            pData16 = T5GET_DBDATA16(pDB);
            pData16[wDst] = *(T5_PTWORD)pCTSeg;
        }
#endif /*T5DEF_DATA16SUPPORTED*/
        break;
    case T5C_DINT :
        if (!ISLOCK32(wDst))
            pDint[wDst] = *(T5_PTDWORD)pCTSeg;
        break;
    case T5C_LINT :
#ifdef T5DEF_DATA64SUPPORTED
        if (!ISLOCK64(wDst))
        {
            pData64 = T5GET_DBDATA64(pDB);
            pData64[wDst] = *(T5_PTDATA64)pCTSeg;
        }
#endif /*T5DEF_DATA64SUPPORTED*/
        break;
    case T5C_TIME :
        if (!ISLOCKTIME(wDst))
        {
            pTime = T5GET_DBTIME(pDB);
            pTime[wDst] = *(T5_PTDWORD)pCTSeg;
        }
        break;
    case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
        if (!ISLOCKSTRING(wDst))
            T5VMStr_Force (pDB, wDst, pCTSeg + 1);
#endif /*T5DEF_STRINGSUPPORTED*/
        break;
    default : break;
    }
}

void T5VMTic_SetCTSegItem (T5PTR_DB pDB, T5_WORD wType,
                           T5_WORD wSrc, T5_WORD wRef)
{
    T5_PTDWORD pDint, pTime;
    T5_PTBYTE pData8;
    T5_PTBYTE pCTSeg;
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pData16;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 pData64;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE *pStr;
    T5_PTBYTE pString;
#endif /*T5DEF_STRINGSUPPORTED*/

    /* get address in CT segment */
    pDint = T5GET_DBDATA32(pDB);
    pCTSeg = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);

    pCTSeg += pDint[wRef];
    /* copy */
    switch (wType)
    {
    case T5C_BOOL :
        pData8 = T5GET_DBDATA8(pDB);
        *pCTSeg = pData8[wSrc];
        break;
    case T5C_INT :
#ifdef T5DEF_DATA16SUPPORTED
        pData16 = T5GET_DBDATA16(pDB);
        *(T5_PTWORD)pCTSeg = pData16[wSrc];
#endif /*T5DEF_DATA16SUPPORTED*/
        break;
    case T5C_DINT :
        *(T5_PTDWORD)pCTSeg = pDint[wSrc];
        break;
    case T5C_LINT :
#ifdef T5DEF_DATA64SUPPORTED
        pData64 = T5GET_DBDATA64(pDB);
        *(T5_PTDATA64)pCTSeg = pData64[wSrc];
#endif /*T5DEF_DATA64SUPPORTED*/
        break;
    case T5C_TIME :
        pTime = T5GET_DBTIME(pDB);
        *(T5_PTDWORD)pCTSeg = pTime[wSrc];
        break;
    case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
        pStr = T5GET_DBSTRING(pDB);
        pString = pStr[wSrc];
        T5VMStr_CopyPtr (pCTSeg, pString);
#endif /*T5DEF_STRINGSUPPORTED*/
        break;
    default : break;
    }
}

void T5VMTic_CTMemcpy (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc, T5_WORD wSize)
{
    T5_PTDWORD pDint;
    T5_PTBYTE pCTSeg;

    /* get address in CT segment */
    pDint = T5GET_DBDATA32(pDB);
    pCTSeg = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    /* raw copy */
    T5_MEMCPY (pCTSeg + pDint[wDst], pCTSeg + pDint[wSrc], pDint[wSize]);
}

#endif /*T5DEF_CTSEG*/

/****************************************************************************/
/* SFC breakpoints */

#ifdef T5DEF_SFC

void T5VMTic_SetSfcBreakS (T5PTR_DB pDB, T5_WORD wIndex, T5_WORD wStyle)
{
    T5PTR_DBSTEP pStep;
    T5_WORD i;

    pStep = T5GET_DBSTEP(pDB);
    if (wIndex == 0)
    {
        for (i=1; i<pDB[T5TB_STEP].wNbItemUsed; i++)
            pStep[i].wBreak = wStyle;
    }
    else if (wIndex < pDB[T5TB_STEP].wNbItemUsed)
    {
        pStep[wIndex].wBreak = wStyle;
    }
}

void T5VMTic_SetSfcBreakT (T5PTR_DB pDB, T5_WORD wIndex, T5_WORD wStyle)
{
    T5PTR_DBTRANS pTrans;
    T5_WORD i;

    pTrans = T5GET_DBTRANS(pDB);
    if (wIndex == 0)
    {
        for (i=1; i<pDB[T5TB_TRANS].wNbItemAlloc; i++)
            pTrans[i].wBreak = wStyle;
    }
    else if (wIndex < pDB[T5TB_TRANS].wNbItemAlloc)
    {
        pTrans[wIndex].wBreak = wStyle;
    }
}

#endif /*T5DEF_SFC*/

/****************************************************************************/
/* advanced control */

void T5VMTic_ExeCtrl (T5PTR_DB pDB, T5_WORD wCommand)
{
#ifdef T5DEF_CCAPP
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    switch (wCommand)
    {
    case T5CCTL_CC :
        if ((pStatus->dwFlagsEx & T5FLAGEX_CCAVL) != 0
            && (pStatus->dwFlagsEx & T5FLAGEX_CCACT) == 0)
        {
            pStatus->dwFlagsEx |= T5FLAGEX_CCACT;
        }
        break;
    case T5CCTL_PC :
        if ((pStatus->dwFlagsEx & T5FLAGEX_CCAVL) != 0
            && (pStatus->dwFlagsEx & T5FLAGEX_CCACT) != 0)
        {
            pStatus->dwFlagsEx &= ~T5FLAGEX_CCACT;
        }
        break;
    default :
        break;
    }
#endif /*T5DEF_CCAPP*/
}

/****************************************************************************/
/* variable locking */

void T5VMTic_LockVar (T5PTR_DB pDB, T5_WORD wIndex, T5_BYTE bMask, T5_DWORD dwCTOffset)
{
#ifdef T5DEF_DONTFORCELOCKEDVARS
    T5PTR_DBSTATUS pStatus;
    T5_PTBYTE pLock;

    pStatus = T5GET_DBSTATUS(pDB);
    if (wIndex != 0)
    {
        pLock = T5GET_DBLOCK(pDB);
        if ((pLock[wIndex] & bMask) != 0)
            return;
        pLock[wIndex] |= bMask;
    }
#ifdef T5DEF_SMARTLOCK
    if (dwCTOffset != 0xffffffffL && T5VMLog_IsCTLockedOff (pDB, dwCTOffset))
        return;
#endif /*T5DEF_SMARTLOCK*/
    pStatus->dwNbLock += 1L;
    pStatus->wFlags |= T5FLAG_VLOCK;
#endif /*T5DEF_DONTFORCELOCKEDVARS*/
}

void T5VMTic_UnlockVar (T5PTR_DB pDB, T5_WORD wIndex, T5_BYTE bMask, T5_DWORD dwCTOffset)
{
#ifdef T5DEF_DONTFORCELOCKEDVARS
    T5PTR_DBSTATUS pStatus;
    T5_PTBYTE pLock;

    pStatus = T5GET_DBSTATUS(pDB);
    if (wIndex != 0)
    {
        pLock = T5GET_DBLOCK(pDB);
        if ((pLock[wIndex] & bMask) == 0)
            return;
        pLock[wIndex] &= ~bMask;
    }
#ifdef T5DEF_SMARTLOCK
    if (dwCTOffset != 0xffffffffL && !T5VMLog_IsCTLockedOff (pDB, dwCTOffset))
        return;
#endif /*T5DEF_SMARTLOCK*/
    if (pStatus->dwNbLock)
    {
        pStatus->dwNbLock -= 1L;
        if (pStatus->dwNbLock == 0)
            pStatus->wFlags &= ~T5FLAG_VLOCK;
    }
#endif /*T5DEF_DONTFORCELOCKEDVARS*/
}

void T5VMTic_UnlockAllVars (T5PTR_DB pDB)
{
#ifdef T5DEF_DONTFORCELOCKEDVARS
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    T5_MEMSET(pDB[T5TB_LOCK].pData, 0, pDB[T5TB_LOCK].wNbItemAlloc);
    pStatus->dwNbLock =0;
    pStatus->wFlags &= ~T5FLAG_VLOCK;
#ifdef T5DEF_SMARTLOCK
    T5VMLog_UnRegisterAllLockVars (pDB);

#endif /*T5DEF_SMARTLOCK*/
#endif /*T5DEF_DONTFORCELOCKEDVARS*/
}

/****************************************************************************/
/* smart lock table */

#ifdef T5DEF_SMARTLOCK

static void _SmLock8 (T5PTR_DB pDB, T5_WORD wIndex, T5_BYTE bValue)
{
    T5VMLog_SetLockVar (pDB, T5C_SINT, wIndex, 0L, &bValue, FALSE);
}

static void _SmLock16 (T5PTR_DB pDB, T5_WORD wIndex, T5_WORD wValue)
{
    T5VMLog_SetLockVar (pDB, T5C_INT, wIndex, 0L, &wValue, FALSE);
}

static void _SmLock32 (T5PTR_DB pDB, T5_WORD wIndex, T5_DWORD dwValue)
{
    T5VMLog_SetLockVar (pDB, T5C_DINT, wIndex, 0L, &dwValue, FALSE);
}

static void _SmLock64 (T5PTR_DB pDB, T5_WORD wIndex, T5_DATA64 lValue)
{
    T5VMLog_SetLockVar (pDB, T5C_LINT, wIndex, 0L, &lValue, FALSE);
}

static void _SmLockTime (T5PTR_DB pDB, T5_WORD wIndex, T5_DWORD dwValue)
{
    T5VMLog_SetLockVar (pDB, T5C_TIME, wIndex, 0L, &dwValue, FALSE);
}

static void _SmLockString (T5PTR_DB pDB, T5_WORD wIndex, T5_WORD wSrcIndex)
{
    T5_PTBYTE *pStr;

    pStr = T5GET_DBSTRING(pDB);
    T5VMLog_SetLockVar (pDB, T5C_STRING, wIndex, 0L, pStr[wSrcIndex], FALSE);
}

#endif /*T5DEF_SMARTLOCK*/

/* eof **********************************************************************/
