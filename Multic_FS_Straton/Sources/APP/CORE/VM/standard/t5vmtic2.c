/*****************************************************************************
T5VMtic2.c : pcode interpreter - alternate version without switch

DO NOT ALTER THIS !

NOTE: ULINT OPERATIONS NOT SUPPORTED

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_NOPCSWITCH

#define _MAX_TIC    600

/****************************************************************************/
/* array of functions */

/* could be - not used because of Keil compiler
typedef T5_BOOL (*T5HND_TIC)(void);
static T5HND_TIC _T5FPC[_MAX_TIC];
*/

static T5_BOOL (*_T5FPC[_MAX_TIC])();

/****************************************************************************/
/* static variables for calls */

static T5_BOOL bRetValue;
static T5PTR_DB pDB;
static T5PTR_DBSTATUS pStatus;
static T5_PTWORD pCode;
static T5_DWORD dwSize;
static T5_WORD wPrgNo;
static T5_PTBYTE pStartCode;
static T5_PTBYTE pEndCode;
static T5_WORD wIndex;
static T5_DWORD dwVal;
static T5PTR_DBFBINST pI;
static T5PTR_DBFCLASS pF;
static T5_BOOL bSkip;
static T5_PTWORD pPrgStatus;
static T5PTR_DBPROG pProg;
#ifdef T5DEF_TICSAFE
static T5_DWORD dwCount;
#endif /*T5DEF_TICSAFE*/
static T5_PTBYTE pLock;
static T5_PTBYTE pData8;
#ifdef T5DEF_SINTSUPPORTED
static T5_PTCHAR pSint;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
static T5_PTSHORT pData16;
#endif /*T5DEF_DATA16SUPPORTED*/
static T5_PTLONG pDint;
#ifdef T5DEF_REALSUPPORTED
static T5_PTREAL pReal;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
static T5_PTLREAL pLReal;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
static T5_PTLINT pLInt;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
static T5_PTDATA64 p64;
#endif /*T5DEF_DATA64SUPPORTED*/
static T5_PTDWORD pTime;
static T5PTR_DBFBINST pInst;
static T5PTR_DBFCLASS pFC;
#ifdef T5DEF_SFC
static T5PTR_DBSTEP pStep;
static T5PTR_DBTRANS pTrans;
#endif /*T5DEF_SFC*/
#ifdef T5DEF_XV
static T5_PTBYTE *pXV, *pXS;
#endif /*T5DEF_XV*/
static T5PTR_DBCALLSTACK pStack;
static T5_DWORD dwCTOffset;
#ifdef T5DEF_CTSEG
static T5_PTBYTE pCTSeg;
#endif /*T5DEF_CTSEG*/

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

#define IF_ISD8LOCKED(i)        /*nothing*/
#define IF_ISD16LOCKED(i)       /*nothing*/
#define IF_ISD32LOCKED(i)       /*nothing*/
#define IF_ISTIMELOCKED(i)      /*nothing*/
#define IF_ISD64LOCKED(i)       /*nothing*/
#define IF_ISSTRINGLOCKED(i)    /*nothing*/
#define IF_ISXVLOCKED(i)        /*nothing*/

#endif /*T5DEF_DONTFORCELOCKEDOUTPUTS*/

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

/****************************************************************************/
/* data for log messages: program index */

#define ERRDATA  ((((T5_DWORD)wPrgNo) & 0xffff)\
                  |((((T5_DWORD)pCode-(T5_DWORD)pStartCode)<<16)& 0xffff0000))

/****************************************************************************/
/* static services */

static void _T5VMTic_InitFTable (void);

/*****************************************************************************
T5VMTic_Init
Initialize private data
parameters:
    p_pDB (IN) pointer to the database
*****************************************************************************/

void T5VMTic_Init (T5PTR_DB p_pDB)
{
    _T5VMTic_InitFTable ();

    pDB = p_pDB;
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
}

/*****************************************************************************
T5VMTic_Exit
Release private data
parameters:
    p_pDB (IN) pointer to the database
*****************************************************************************/

void T5VMTic_Exit (T5PTR_DB p_pDB)
{
}

/****************************************************************************/
/* p-code instructions */

T5_BOOL _ERROR (void)
{
    T5TRACEDW("TIC: error", *(pCode-1));
    T5VMLog_Push (pDB, T5RET_UNKNOWNTIC, *(pCode-1), ERRDATA);
    pStatus->wFlags &= ~T5FLAG_RUNMASK;
    pStatus->wFlags |= T5FLAG_ERROR;
    bRetValue = FALSE;
    return FALSE;
}


T5_BOOL _FT5C_NOP (void)
{
    return TRUE;
}

T5_BOOL _FT5C_RET (void)
{
    if (wPrgNo != 0 && pDB[T5TB_CALLSTACK].wNbItemUsed != 0)
    {
        wPrgNo = pStack[pDB[T5TB_CALLSTACK].wNbItemUsed-1].wProg;
        pCode = pProg[wPrgNo].pCode;
        pStartCode = (T5_PTBYTE)pCode;
        pEndCode = pStartCode + pProg[wPrgNo].dwSize;
        pCode += (pStack[pDB[T5TB_CALLSTACK].wNbItemUsed-1].wPos) / 2;
        pDB[T5TB_CALLSTACK].wNbItemUsed -= 1;
        pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg = wPrgNo;
        return TRUE;
    }
    return FALSE;
}

T5_BOOL _FT5C_SCHEDPRG (void)
{
    if (pDB[T5TB_CALLSTACK].wNbItemUsed == 0
        && ((T5_WORD)(pStatus->dwCycleCount) % pCode[0]) != pCode[1])
        return FALSE;
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_SETWAITMARK (void)
{
    pProg[wPrgNo].dwPos = (pCode - 1 - (T5_PTWORD)(pProg[wPrgNo].pCode));
    return TRUE;
}

T5_BOOL _FT5C_WAIT (void)
{
    if (!GET_D8(*pCode))
        return FALSE;
    else
    {
        pProg[wPrgNo].dwPos = 0;
        pCode += 1;
    }
    return TRUE;
}

T5_BOOL _FT5C_CALLPRG (void)
{
    if (pDB[T5TB_CALLSTACK].wNbItemUsed >= pStatus->wMaxCallStack)
    {
        T5VMLog_Push (pDB, T5RET_STACKOVER, *pCode, ERRDATA);
        pStatus->wFlags &= ~T5FLAG_RUNMASK;
        pStatus->wFlags |= T5FLAG_ERROR;
        bRetValue = FALSE;
        return FALSE;
    }
    pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg = wPrgNo;
    pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wPos
     = ((T5_WORD)(pCode - (T5_PTWORD)(pProg[wPrgNo].pCode)) + 1) * 2;
    pDB[T5TB_CALLSTACK].wNbItemUsed += 1;
    pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg = *pCode;
    wPrgNo = *pCode;
    pCode = pProg[wPrgNo].pCode;
    pStartCode = (T5_PTBYTE)pCode;
    pEndCode = pStartCode + pProg[wPrgNo].dwSize;
    return TRUE;
}

T5_BOOL _FT5C_STEPMARK (void)
{
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
        bRetValue = FALSE;
        return FALSE;
    }
    return TRUE;
}

T5_BOOL _FT5C_PAUSE (void)
{
    pStatus->wFlags &= ~T5FLAG_RUN;
    pStatus->wFlags &= ~T5FLAG_PROGSTEP;
    return TRUE;
}

T5_BOOL _FT5C_RESUME (void)
{
    pStatus->wFlags |= T5FLAG_RUN;
    pStatus->wFlags &= ~T5FLAG_ERROR;
    pStatus->wFlags &= ~T5FLAG_PROGSTEP;
    return TRUE;
}

T5_BOOL _FT5C_CYCLESTEP (void)
{
    pStatus->wFlags |= T5FLAG_ONECYCLE;
    pStatus->wFlags &= ~T5FLAG_PROGSTEP;
    return TRUE;
}

T5_BOOL _FT5C_STEPFWD (void)
{
    if ((pStatus->wFlags & T5FLAG_RUN) == 0)
    {
        pStatus->wFlags |= T5FLAG_ONECYCLE;
        pStatus->wFlags |= T5FLAG_PROGSTEP;
    }
    return TRUE;
}
        
T5_BOOL _FT5C_SETCYCLETIME (void)
{
    pStatus->dwTimTrigger = (T5_DWORD)(*pCode++) << 16 & 0xffff0000L;
    pStatus->dwTimTrigger |= ((T5_DWORD)(*pCode++) & 0xffffL);
    return TRUE;
}

T5_BOOL _FT5C_SETBKP (void)
{
    T5VMPrg_SetBreakpoint (pDB, pCode[0], pCode[1], pCode[2]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_JMP (void)
{
    pCode = (T5_PTWORD)((T5_DWORD)pCode + (T5_SHORT)(*pCode));
    pCode++;
    return TRUE;
}

T5_BOOL _FT5C_JMPIF (void)
{
    if (GET_D8(*pCode))
        pCode = (T5_PTWORD)((T5_DWORD)pCode + (T5_SHORT)(pCode[1]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_JMPIFNOT (void)
{
    if (!GET_D8(*pCode))
        pCode = (T5_PTWORD)((T5_DWORD)pCode + (T5_SHORT)(pCode[1]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_JA (void)
{
    T5TRACEDW ("JA jump", *pCode);
    pCode = (T5_PTWORD)(pStartCode + *pCode);
    return TRUE;
}

T5_BOOL _FT5C_JAC (void)
{
    T5TRACEDW ("JAC jump", pCode[1]);
    if (GET_D8(*pCode))
        pCode = (T5_PTWORD)(pStartCode + pCode[1]);
    else
        pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_JACN (void)
{
    T5TRACEDW ("JACN jump", pCode[1]);
    if (!GET_D8(*pCode))
        pCode = (T5_PTWORD)(pStartCode + pCode[1]);
    else
        pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_NOTBOOL (void)
{
    SET_D8(*pCode, !GET_D8(pCode[1]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_NOTBOOL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_NOTBOOL ();
}

T5_BOOL _FT5C_EQBOOL (void)
{
    SET_D8(*pCode, (GET_D8(pCode[1]) == GET_D8(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_EQBOOL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_EQBOOL ();
}

T5_BOOL _FT5C_NEBOOL (void)
{
    SET_D8(*pCode, (GET_D8(pCode[1]) != GET_D8(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_NEBOOL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_NEBOOL ();
}

T5_BOOL _FT5C_ORBOOL (void)
{
    SET_D8(*pCode, (GET_D8(pCode[1]) | GET_D8(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ORBOOL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ORBOOL ();
}

T5_BOOL _FT5C_ANDBOOL (void)
{
    SET_D8(*pCode, (GET_D8(pCode[1]) & GET_D8(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ANDBOOL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ANDBOOL ();
}

T5_BOOL _FT5C_XORBOOL (void)
{
    SET_D8(*pCode, (GET_D8(pCode[1]) ^ GET_D8(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XORBOOL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XORBOOL ();
}

T5_BOOL _FT5C_COPYBOOL (void)
{
    SET_D8(*pCode, GET_D8(pCode[1]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_COPYBOOL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_COPYBOOL ();
}

T5_BOOL _FT5C_SETBOOL (void)
{
    SET_D8(*pCode, 1);
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_O_SETBOOL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode ++;
        return TRUE;
    }
    return _FT5C_SETBOOL ();
}

T5_BOOL _FT5C_RESETBOOL (void)
{
    SET_D8(*pCode, 0);
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_O_RESETBOOL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode ++;
        return TRUE;
    }
    return _FT5C_RESETBOOL ();
}

T5_BOOL _FT5C_ARGETBOOL (void)
{
    SET_D8(*pCode, GET_D8(pCode[1] + GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_AGETBOOL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARGETBOOL ();
}

T5_BOOL _FT5C_ARPUTBOOL (void)
{
    SET_D8((*pCode + GET_DINT(pCode[1])), GET_D8(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_APUTBOOL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARPUTBOOL ();
}

T5_BOOL _FT5C_LOCKBOOL (void)
{
    T5VMTic_LockVar (pDB, *pCode, T5LOCK_DATA8, 0L);
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_UNLOCKBOOL (void)
{
    T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_DATA8, 0L);
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_REDGE (void)
{
    SET_D8(*pCode, GET_D8(pCode[1]) & !GET_D8(pCode[2]));
    SET_D8(pCode[2], GET_D8(pCode[1]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_REDGE (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_REDGE ();
}

T5_BOOL _FT5C_FEDGE (void)
{
    SET_D8(*pCode, !GET_D8(pCode[1]) & GET_D8(pCode[2]));
    SET_D8(pCode[2], GET_D8(pCode[1]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_FEDGE (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_FEDGE ();
}

#ifdef T5DEF_SINTSUPPORTED

T5_BOOL _FT5C_NEGSINT (void)
{
    SET_SINT(*pCode, -GET_SINT(pCode[1]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_NEGSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_NEGSINT ();
}

T5_BOOL _FT5C_MULSINT (void)
{
    SET_SINT(*pCode, GET_SINT(pCode[1]) * GET_SINT(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_MULSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_MULSINT ();
}

T5_BOOL _FT5C_DIVSINT (void)
{
    if (!GET_SINT(pCode[2]))
    {
        T5VMLog_Push (pDB, T5RET_SDIVZERO, pCode[2], ERRDATA);
        SET_SINT(*pCode, T5_MAXSINT);
    }
    else
        SET_SINT(*pCode, (GET_SINT(pCode[1]) / GET_SINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_DIVSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_DIVSINT ();
}

T5_BOOL _FT5C_ADDSINT (void)
{
    SET_SINT(*pCode, GET_SINT(pCode[1]) + GET_SINT(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ADDSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ADDSINT ();
}

T5_BOOL _FT5C_SUBSINT (void)
{
    SET_SINT(*pCode, GET_SINT(pCode[1]) - GET_SINT(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SUBSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_SUBSINT ();
}

T5_BOOL _FT5C_GTSINT (void)
{
    SET_D8(*pCode, GET_SINT(pCode[1]) > GET_SINT(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GTSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GTSINT ();
}

T5_BOOL _FT5C_GESINT (void)
{
    SET_D8(*pCode, GET_SINT(pCode[1]) >= GET_SINT(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GESINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GESINT ();
}

T5_BOOL _FT5C_EQSINT (void)
{
    SET_D8(*pCode, GET_SINT(pCode[1]) == GET_SINT(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_EQSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_EQSINT ();
}

T5_BOOL _FT5C_NESINT (void)
{
    SET_D8(*pCode, GET_SINT(pCode[1]) != GET_SINT(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_NESINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_NESINT ();
}

T5_BOOL _FT5C_COPYSINT (void)
{
    SET_SINT(*pCode, GET_SINT(pCode[1]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_COPYSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_COPYSINT ();
}

T5_BOOL _FT5C_ARGETSINT (void)
{
    SET_SINT(*pCode, GET_SINT(pCode[1] + GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ARGETSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARGETSINT ();
}

T5_BOOL _FT5C_ARPUTSINT (void)
{
    SET_SINT((*pCode + GET_DINT(pCode[1])), GET_SINT(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ARPUTSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARPUTSINT ();
}

T5_BOOL _FT5C_SETSINT (void)
{
    if (*pCode != 0)
    {
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
    return TRUE;
}

T5_BOOL _FT5C_O_SETSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_SETSINT ();
}

#ifdef T5DEF_DOTBITSUPPORTED

T5_BOOL _FT5C_GETBIT8 (void)
{
    SET_D8(*pCode, (GET_SINT(pCode[1]) & GET_SINT(pCode[2])) != 0);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GETBIT8 (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GETBIT8 ();
}

T5_BOOL _FT5C_SETBIT8 (void)
{
    if (GET_D8(pCode[2]))
    {
        SET_SINT(*pCode, (GET_SINT(*pCode) | GET_SINT(pCode[1])));
    }
    else
    {
        SET_SINT(*pCode, (GET_SINT(*pCode) & ~GET_SINT(pCode[1])));
    }
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SETBIT8 (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_SETBIT8 ();
}

#endif /*T5DEF_DOTBITSUPPORTED*/

#endif /*T5DEF_SINTSUPPORTED*/

#ifdef T5DEF_USINTSUPPORTED

T5_BOOL _FT5C_MULUSINT (void)
{
    SET_UD8(*pCode, GET_UD8(pCode[1]) * GET_UD8(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_MULUSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_MULUSINT ();
}

T5_BOOL _FT5C_DIVUSINT (void)
{
    if (!GET_UD8(pCode[2]))
    {
        T5VMLog_Push (pDB, T5RET_SDIVZERO, pCode[2], ERRDATA);
        SET_UD8(*pCode, T5_MAXSINT);
    }
    else
        SET_UD8(*pCode, (GET_UD8(pCode[1]) / GET_UD8(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_DIVUSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_DIVUSINT ();
}

T5_BOOL _FT5C_GTUSINT (void)
{
    SET_D8(*pCode, GET_UD8(pCode[1]) > GET_UD8(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GTUSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GTUSINT ();
}

T5_BOOL _FT5C_GEUSINT (void)
{
    SET_D8(*pCode, GET_UD8(pCode[1]) >= GET_UD8(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GEUSINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GEUSINT ();
}

#endif /*T5DEF_USINTSUPPORTED*/

#ifdef T5DEF_DATA16SUPPORTED

T5_BOOL _FT5C_COPYINT (void)
{
    SET_D16(*pCode, GET_D16(pCode[1]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_COPYINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_COPYINT ();
}

T5_BOOL _FT5C_NEGINT (void)
{
    SET_D16(*pCode, -GET_D16(pCode[1]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_NEGINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_NEGINT ();
}

T5_BOOL _FT5C_MULINT (void)
{
    SET_D16(*pCode, GET_D16(pCode[1]) * GET_D16(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_MULINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_MULINT ();
}

T5_BOOL _FT5C_DIVINT (void)
{
    if (!GET_D16(pCode[2]))
    {
        T5VMLog_Push (pDB, T5RET_WDIVZERO, pCode[2], ERRDATA);
        SET_D16(*pCode, T5_MAXINT);
    }
    else
        SET_D16(*pCode, (GET_D16(pCode[1]) / GET_D16(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_DIVINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_DIVINT ();
}

T5_BOOL _FT5C_ADDINT (void)
{
    SET_D16(*pCode, GET_D16(pCode[1]) + GET_D16(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ADDINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ADDINT ();
}

T5_BOOL _FT5C_SUBINT (void)
{
    SET_D16(*pCode, GET_D16(pCode[1]) - GET_D16(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SUBINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_SUBINT ();
}

T5_BOOL _FT5C_GTINT (void)
{
    SET_D8(*pCode, GET_D16(pCode[1]) > GET_D16(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GTINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GTINT ();
}

T5_BOOL _FT5C_GEINT (void)
{
    SET_D8(*pCode, GET_D16(pCode[1]) >= GET_D16(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GEINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GEINT ();
}

T5_BOOL _FT5C_EQINT (void)
{
    SET_D8(*pCode, GET_D16(pCode[1]) == GET_D16(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_EQINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_EQINT ();
}

T5_BOOL _FT5C_NEINT (void)
{
    SET_D8(*pCode, GET_D16(pCode[1]) != GET_D16(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_NEINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_NEINT ();
}

T5_BOOL _FT5C_ARGETINT (void)
{
    SET_D16(*pCode, GET_D16(pCode[1] + GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ARGETINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARGETINT ();
}

T5_BOOL _FT5C_ARPUTINT (void)
{
    SET_D16((*pCode + GET_DINT(pCode[1])), GET_D16(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ARPUTINT (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARPUTINT ();
}

T5_BOOL _FT5C_SETINT (void)
{
    if (*pCode != 0)
    {
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
    return TRUE;
}

T5_BOOL _FT5C_O_SETINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_SETINT ();
}

T5_BOOL _FT5C_LOCKINT (void)
{
    T5VMTic_LockVar (pDB, *pCode, T5LOCK_DATA16, 0L);
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_UNLOCKINT (void)
{
    T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_DATA16, 0L);
    pCode ++;
    return TRUE;
}

#ifdef T5DEF_DOTBITSUPPORTED

T5_BOOL _FT5C_GETBIT16 (void)
{
    SET_D8(*pCode, (GET_D16(pCode[1]) & GET_D16(pCode[2])) != 0);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GETBIT16 (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GETBIT16 ();
}

T5_BOOL _FT5C_SETBIT16 (void)
{
    if (GET_D8(pCode[2]))
    {
        SET_D16(*pCode, (GET_D16(*pCode) | GET_D16(pCode[1])));
    }
    else
    {
        SET_D16(*pCode, (GET_D16(*pCode) & ~GET_D16(pCode[1])));
    }
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SETBIT16 (void)
{
    IF_ISD16LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_SETBIT16 ();
}

#endif /*T5DEF_DOTBITSUPPORTED*/

#endif /*T5DEF_DATA16SUPPORTED*/

#ifdef T5DEF_UINTSUPPORTED

T5_BOOL _FT5C_MULUINT (void)
{
    SET_UD16(*pCode, GET_UD16(pCode[1]) * GET_UD16(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_MULUINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_MULUINT ();
}

T5_BOOL _FT5C_DIVUINT (void)
{
    if (!GET_UD16(pCode[2]))
    {
        T5VMLog_Push (pDB, T5RET_WDIVZERO, pCode[2], ERRDATA);
        SET_UD16(*pCode, T5_MAXINT);
    }
    else
        SET_UD16(*pCode, (GET_UD16(pCode[1]) / GET_UD16(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_DIBUINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_DIVUINT ();
}

T5_BOOL _FT5C_GTUINT (void)
{
    SET_D8(*pCode, GET_UD16(pCode[1]) > GET_UD16(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GTUINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GTUINT ();
}

T5_BOOL _FT5C_GEUINT (void)
{
    SET_D8(*pCode, GET_UD16(pCode[1]) >= GET_UD16(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GEUINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GEUINT ();
}

#endif /*T5DEF_UINTSUPPORTED*/

T5_BOOL _FT5C_NEGDINT (void)
{
    SET_DINT(*pCode, -(GET_DINT(pCode[1])));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_NEGDINT (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_NEGDINT ();
}

T5_BOOL _FT5C_MULDINT (void)
{
    SET_DINT(*pCode, (GET_DINT(pCode[1]) * GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_MULDINT (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_MULDINT ();
}

T5_BOOL _FT5C_DIVDINT (void)
{
    if (!GET_DINT(pCode[2]))
    {
        T5VMLog_Push (pDB, T5RET_DIVZERO, pCode[2], ERRDATA);
        SET_DINT(*pCode, T5_MAXDINT);
    }
    else
        SET_DINT(*pCode, (GET_DINT(pCode[1]) / GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_DIVDINT (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_DIVDINT ();
}

T5_BOOL _FT5C_ADDDINT (void)
{
    SET_DINT(*pCode, (GET_DINT(pCode[1]) + GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ADDDINT (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ADDDINT ();
}

T5_BOOL _FT5C_SUBDINT (void)
{
    SET_DINT(*pCode, (GET_DINT(pCode[1]) - GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SUBDINT (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_SUBDINT ();
}

T5_BOOL _FT5C_GTDINT (void)
{
    SET_D8(*pCode, (GET_DINT(pCode[1]) > GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GTDINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GTDINT ();
}

T5_BOOL _FT5C_GEDINT (void)
{
    SET_D8(*pCode, (GET_DINT(pCode[1]) >= GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GEDINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GEDINT ();
}

T5_BOOL _FT5C_EQDINT (void)
{
    SET_D8(*pCode, (GET_DINT(pCode[1]) == GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_EQDINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_EQDINT ();
}

T5_BOOL _FT5C_NEDINT (void)
{
    SET_D8(*pCode, (GET_DINT(pCode[1]) != GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_NEDINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_NEDINT ();
}

T5_BOOL _FT5C_COPYDINT (void)
{
    SET_DINT(*pCode, (GET_DINT(pCode[1])));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_COPYDINT (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_COPYDINT ();
}

T5_BOOL _FT5C_SETDINT (void)
{
    if (*pCode != 0)
    {
        T5_COPYWORDSTOLONG((T5_PTBYTE)(pDint+*pCode), (T5_PTBYTE)(pCode+1));
    }
#ifdef T5DEF_CTSEG
    else if (dwCTOffset != 0xffffffffL)
    {
        T5_COPYWORDSTOLONG(pCTSeg+dwCTOffset, (T5_PTBYTE)(pCode+1));
        dwCTOffset = 0xffffffffL;
    }
#endif /*T5DEF_CTSEG*/
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SETDINT (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_SETDINT ();
}

T5_BOOL _FT5C_ARGETDINT (void)
{
    SET_DINT(*pCode, GET_DINT(pCode[1] + GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_AGETDINT (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARGETDINT ();
}

T5_BOOL _FT5C_ARPUTDINT (void)
{
    SET_DINT((*pCode + GET_DINT(pCode[1])), GET_DINT(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_APUTDINT (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARPUTDINT ();
}

T5_BOOL _FT5C_LOCKDINT (void)
{
    T5VMTic_LockVar (pDB, *pCode, T5LOCK_DATA32, 0L);
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_UNLOCKDINT (void)
{
    T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_DATA32, 0L);
    pCode ++;
    return TRUE;
}

#ifdef T5DEF_DOTBITSUPPORTED

T5_BOOL _FT5C_GETBIT32 (void)
{
    SET_D8(*pCode, (GET_DINT(pCode[1]) & GET_DINT(pCode[2])) != 0);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GETBIT32 (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GETBIT32 ();
}

T5_BOOL _FT5C_SETBIT32 (void)
{
    if (GET_D8(pCode[2]))
    {
        SET_DINT(*pCode, (GET_DINT(*pCode) | GET_DINT(pCode[1])));
    }
    else
    {
        SET_DINT(*pCode, (GET_DINT(*pCode) & ~GET_DINT(pCode[1])));
    }
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SETBIT32 (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_SETBIT32 ();
}

#endif /*T5DEF_DOTBITSUPPORTED*/
    
#ifdef T5DEF_UDINTSUPPORTED

T5_BOOL _FT5C_MULUDINT (void)
{
    SET_UD32(*pCode, GET_UD32(pCode[1]) * GET_UD32(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_0_MULUDINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_MULUDINT ();
}

T5_BOOL _FT5C_DIVUDINT (void)
{
    if (!GET_UD32(pCode[2]))
    {
        T5VMLog_Push (pDB, T5RET_DIVZERO, pCode[2], ERRDATA);
        SET_UD32(*pCode, T5_MAXDINT);
    }
    else
        SET_UD32(*pCode, (GET_UD32(pCode[1]) / GET_UD32(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_0_DIVUDINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_DIVUDINT ();
}

T5_BOOL _FT5C_GTUDINT (void)
{
    SET_D8(*pCode, GET_UD32(pCode[1]) > GET_UD32(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_0_GTUDINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GTUDINT ();
}

T5_BOOL _FT5C_GEUDINT (void)
{
    SET_D8(*pCode, GET_UD32(pCode[1]) >= GET_UD32(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_0_GEUDINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GEUDINT ();
}

#endif /*T5DEF_UDINTSUPPORTED*/

#ifdef T5DEF_REALSUPPORTED

T5_BOOL _FT5C_NEGREAL (void)
{
    SET_REAL(*pCode, -(GET_REAL(pCode[1])));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_NEGREAL (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_NEGREAL ();
}

T5_BOOL _FT5C_MULREAL (void)
{
    SET_REAL(*pCode, (GET_REAL(pCode[1]) * GET_REAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_MULREAL (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_MULREAL ();
}

T5_BOOL _FT5C_DIVREAL (void)
{
    if (!GET_REAL(pCode[2]))
    {
        T5VMLog_Push (pDB, T5RET_DIVZERO, pCode[2], ERRDATA);
        SET_REAL(*pCode, T5_MAXREAL);
    }
    else
        SET_REAL(*pCode, (GET_REAL(pCode[1]) / GET_REAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_DIVREAL (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_DIVREAL ();
}

T5_BOOL _FT5C_ADDREAL (void)
{
    SET_REAL(*pCode, (GET_REAL(pCode[1]) + GET_REAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ADDREAL (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ADDREAL ();
}

T5_BOOL _FT5C_SUBREAL (void)
{
    SET_REAL(*pCode, (GET_REAL(pCode[1]) - GET_REAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SUBREAL (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_SUBREAL ();
}

T5_BOOL _FT5C_GTREAL (void)
{
    SET_D8(*pCode, (GET_REAL(pCode[1]) > GET_REAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GTREAL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GTREAL ();
}

T5_BOOL _FT5C_GEREAL (void)
{
    SET_D8(*pCode, (GET_REAL(pCode[1]) >= GET_REAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GEREAL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GEREAL ();
}

T5_BOOL _FT5C_EQREAL (void)
{
    SET_D8(*pCode, (GET_REAL(pCode[1]) == GET_REAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_EQREAL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_EQREAL ();
}

T5_BOOL _FT5C_NEREAL (void)
{
    SET_D8(*pCode, (GET_REAL(pCode[1]) != GET_REAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_NEREAL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_NEREAL ();
}

T5_BOOL _FT5C_COPYREAL (void)
{
    SET_REAL(*pCode, GET_REAL(pCode[1]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_COPYREAL (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_COPYREAL ();
}

T5_BOOL _FT5C_ARGETREAL (void)
{
    SET_REAL(*pCode, GET_REAL(pCode[1] + GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_AGETREAL (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARGETREAL ();
}

T5_BOOL _FT5C_ARPUTREAL (void)
{
    SET_REAL((*pCode + GET_DINT(pCode[1])), GET_REAL(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_APUTREAL (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARPUTREAL ();
}

#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_DATA64SUPPORTED

T5_BOOL _FT5C_SETLINT (void)
{
    if (*pCode != 0)
    {
        T5_COPYWORDSTO64((T5_PTBYTE)(p64+*pCode), (T5_PTBYTE)(pCode+1));
    }
#ifdef T5DEF_CTSEG
    else if (dwCTOffset != 0xffffffffL)
    {
        T5_COPYWORDSTO64(pCTSeg+dwCTOffset, (T5_PTBYTE)(pCode+1));
        dwCTOffset = 0xffffffffL;
    }
#endif /*T5DEF_CTSEG*/
    pCode += 5;
    return TRUE;
}

T5_BOOL _FT5C_O_SETLINT (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 5;
        return TRUE;
    }
    return _FT5C_SETLINT ();
}

T5_BOOL _FT5C_LOCKLINT (void)
{
    T5VMTic_LockVar (pDB, *pCode, T5LOCK_DATA64, 0L);
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_UNLOCKLINT (void)
{
    T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_DATA64, 0L);
    pCode ++;
    return TRUE;
}

#endif /*T5DEF_DATA64SUPPORTED*/

#ifdef T5DEF_LINTSUPPORTED

T5_BOOL _FT5C_NEGLINT (void)
{
    SET_LINT(*pCode, -(GET_LINT(pCode[1])));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_NEGLINT (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_NEGLINT ();
}

T5_BOOL _FT5C_MULLINT (void)
{
    SET_LINT(*pCode, (GET_LINT(pCode[1]) * GET_LINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_MULLINT (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_MULLINT ();
}

T5_BOOL _FT5C_DIVLINT (void)
{
    if (!GET_LINT(pCode[2]))
    {
        T5VMLog_Push (pDB, T5RET_LDIVZERO, pCode[2], ERRDATA);
        SET_LINT(*pCode, T5_MAXLINT);
    }
    else
        SET_LINT(*pCode, (GET_LINT(pCode[1]) / GET_LINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_DIVLINT (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_DIVLINT ();
}

T5_BOOL _FT5C_ADDLINT (void)
{
    SET_LINT(*pCode, (GET_LINT(pCode[1]) + GET_LINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ADDLINT (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ADDLINT ();
}

T5_BOOL _FT5C_SUBLINT (void)
{
    SET_LINT(*pCode, (GET_LINT(pCode[1]) - GET_LINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SUBLINT (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_SUBLINT ();
}

T5_BOOL _FT5C_GTLINT (void)
{
    SET_D8(*pCode, (GET_LINT(pCode[1]) > GET_LINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GTLINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GTLINT ();
}

T5_BOOL _FT5C_GELINT (void)
{
    SET_D8(*pCode, (GET_LINT(pCode[1]) >= GET_LINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GELINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GELINT ();
}

T5_BOOL _FT5C_EQLINT (void)
{
    SET_D8(*pCode, (GET_LINT(pCode[1]) == GET_LINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_EQLINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_EQLINT ();
}

T5_BOOL _FT5C_NELINT (void)
{
    SET_D8(*pCode, (GET_LINT(pCode[1]) != GET_LINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_NELINT (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_NELINT ();
}

T5_BOOL _FT5C_COPYLINT (void)
{
    SET_LINT(*pCode, GET_LINT(pCode[1]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_COPYLINT (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_COPYLINT ();
}

T5_BOOL _FT5C_ARGETLINT (void)
{
    SET_LINT(*pCode, GET_LINT(pCode[1] + GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ARGETLINT (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARGETLINT ();
}

T5_BOOL _FT5C_ARPUTLINT (void)
{
    SET_LINT((*pCode + GET_DINT(pCode[1])), GET_LINT(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ARPUTLINT (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARPUTLINT ();
}

#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_LREALSUPPORTED

T5_BOOL _FT5C_NEGLREAL (void)
{
    SET_LREAL(*pCode, -(GET_LREAL(pCode[1])));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_NEGLREAL (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_NEGLREAL ();
}

T5_BOOL _FT5C_MULLREAL (void)
{
    SET_LREAL(*pCode, (GET_LREAL(pCode[1]) * GET_LREAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_MULLREAL (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_MULLREAL ();
}

T5_BOOL _FT5C_DIVLREAL (void)
{
    if (!GET_LREAL(pCode[2]))
    {
        T5VMLog_Push (pDB, T5RET_LDIVZERO, pCode[2], ERRDATA);
        SET_LREAL(*pCode, T5_MAXLREAL);
    }
    else
        SET_LREAL(*pCode, (GET_LREAL(pCode[1]) / GET_LREAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_DIVLREAL (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_DIVLREAL ();
}

T5_BOOL _FT5C_ADDLREAL (void)
{
    SET_LREAL(*pCode, (GET_LREAL(pCode[1]) + GET_LREAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ADDLREAL (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ADDLREAL ();
}

T5_BOOL _FT5C_SUBLREAL (void)
{
    SET_LREAL(*pCode, (GET_LREAL(pCode[1]) - GET_LREAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SUBLREAL (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_SUBLREAL ();
}

T5_BOOL _FT5C_GTLREAL (void)
{
    SET_D8(*pCode, (GET_LREAL(pCode[1]) > GET_LREAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GELREAL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GTLREAL ();
}

T5_BOOL _FT5C_GELREAL (void)
{
    SET_D8(*pCode, (GET_LREAL(pCode[1]) >= GET_LREAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GTLREAL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GELREAL ();
}

T5_BOOL _FT5C_EQLREAL (void)
{
    SET_D8(*pCode, (GET_LREAL(pCode[1]) == GET_LREAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_EQLREAL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_EQLREAL ();
}

T5_BOOL _FT5C_NELREAL (void)
{
    SET_D8(*pCode, (GET_LREAL(pCode[1]) != GET_LREAL(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_NELREAL (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_NELREAL ();
}

T5_BOOL _FT5C_COPYLREAL (void)
{
    SET_LREAL(*pCode, GET_LREAL(pCode[1]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_COPYLREAL (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_COPYLREAL ();
}

T5_BOOL _FT5C_ARGETLREAL (void)
{
    SET_LREAL(*pCode, GET_LREAL(pCode[1] + GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ARGETLREAL (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARGETLREAL ();
}

T5_BOOL _FT5C_ARPUTLREAL (void)
{
    SET_LREAL((*pCode + GET_DINT(pCode[1])), GET_LREAL(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ARPUTLREAL (void)
{
    IF_ISD64LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARPUTLREAL ();
}

#endif /*T5DEF_LREALSUPPORTED*/

T5_BOOL _FT5C_ADDTIME (void)
{
    SET_TIME(*pCode, (GET_TIME(pCode[1]) + GET_TIME(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ADDTIME (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ADDTIME ();
}

T5_BOOL _FT5C_SUBTIME (void)
{
    if (GET_TIME(pCode[1]) < GET_TIME(pCode[2]))
    {
        SET_TIME(*pCode, 0L);
    }
    else
    {
        SET_TIME(*pCode, (GET_TIME(pCode[1]) - GET_TIME(pCode[2])));
    }
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SUBTIME (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_SUBTIME ();
}

T5_BOOL _FT5C_GTTIME (void)
{
    SET_D8(*pCode, (GET_TIME(pCode[1]) > GET_TIME(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GTTIME (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GTTIME ();
}

T5_BOOL _FT5C_GETIME (void)
{
    SET_D8(*pCode, (GET_TIME(pCode[1]) >= GET_TIME(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GETIME (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GETIME ();
}

T5_BOOL _FT5C_EQTIME (void)
{
    SET_D8(*pCode, (GET_TIME(pCode[1]) == GET_TIME(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_EQTIME (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_EQTIME ();
}

T5_BOOL _FT5C_NETIME (void)
{
    SET_D8(*pCode, (GET_TIME(pCode[1]) != GET_TIME(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_NETIME (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_NETIME ();
}

T5_BOOL _FT5C_COPYTIME (void)
{
    SET_TIME(*pCode, (GET_TIME(pCode[1])));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_COPYTIME (void)
{
    IF_ISTIMELOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_COPYTIME ();
}

T5_BOOL _FT5C_SETTIME (void)
{
    wIndex = *pCode;
    dwVal = (T5_DWORD)(pCode[1]) << 16 & 0xffff0000L;
    dwVal |= ((T5_DWORD)(pCode[2]) & 0xffffL);
    if (wIndex)
    {
        ((T5_PTDWORD)pTime)[wIndex] = dwVal;
    }
#ifdef T5DEF_CTSEG
    else if (dwCTOffset != 0xffffffffL)
    {
        *(T5_PTDWORD)(pCTSeg+dwCTOffset) = dwVal;
    }
#endif /*T5DEF_CTSEG*/
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_SETTIME (void)
{
    IF_ISTIMELOCKED(*pCode)
    {
        pCode += 1;
        return TRUE;
    }
    return _FT5C_SETTIME ();
}

T5_BOOL _FT5C_STARTTIME (void)
{
#ifdef T5DEF_ACTIMESUPPORTED
    T5VMTmr_StartTimer (pDB, &(pTime[*pCode]));
#else /*T5DEF_ACTIMESUPPORTED*/
    T5VMLog_Push (pDB, T5RET_CANTACTIME, *pCode, ERRDATA);
    pStatus->wFlags &= ~T5FLAG_RUNMASK;
    pStatus->wFlags |= T5FLAG_ERROR;
    return FALSE;
#endif /*T5DEF_ACTIMESUPPORTED*/
    pCode++;
    return TRUE;
}

T5_BOOL _FT5C_O_STARTTIME (void)
{
    IF_ISTIMELOCKED(*pCode)
    {
        pCode++;
        return TRUE;
    }
    return _FT5C_STARTTIME ();
}

T5_BOOL _FT5C_STOPTIME (void)
{
#ifdef T5DEF_ACTIMESUPPORTED
    T5VMTmr_StopTimer (pDB, &(pTime[*pCode]));
#else /*T5DEF_ACTIMESUPPORTED*/
    T5VMLog_Push (pDB, T5RET_CANTACTIME, *pCode, ERRDATA);
    pStatus->wFlags &= ~T5FLAG_RUNMASK;
    pStatus->wFlags |= T5FLAG_ERROR;
    return FALSE;
#endif /*T5DEF_ACTIMESUPPORTED*/
    pCode++;
    return TRUE;
}

T5_BOOL _FT5C_O_STOPTIME (void)
{
    IF_ISTIMELOCKED(*pCode)
    {
        pCode++;
        return TRUE;
    }
    return _FT5C_STOPTIME ();
}

T5_BOOL _FT5C_LOCKTIME (void)
{
    T5VMTic_LockVar (pDB, *pCode, T5LOCK_TIME, 0L);
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_UNLOCKTIME (void)
{
    T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_TIME, 0L);
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_ARGETTIME (void)
{
    SET_TIME(*pCode, GET_TIME(pCode[1] + GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_AGETTIME (void)
{
    IF_ISTIMELOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARGETTIME ();
}

T5_BOOL _FT5C_ARPUTTIME (void)
{
    SET_TIME((*pCode + GET_DINT(pCode[1])), GET_TIME(pCode[2]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_APUTTIME (void)
{
    IF_ISTIMELOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARPUTTIME ();
}

T5_BOOL _FT5C_STDFC (void)
{
    if (!T5VMStf_Execute (pDB, pCode))
    {
        pStatus->wFlags &= ~T5FLAG_RUNMASK;
        pStatus->wFlags |= T5FLAG_ERROR;
        bRetValue = FALSE;
        return FALSE;
    }
    pCode += (pCode[1] + 2);
    return TRUE;
}

T5_BOOL _FT5C_CAST (void)
{
    T5VMCnv_Convert (pDB, pCode);
    pCode += 4;
    return TRUE;
}

T5_BOOL _FT5C_O_CAST (void)
{
#ifdef T5DEF_DONTFORCELOCKEDVARS
    bSkip = FALSE;
    switch (pCode[2])
    {
    case T5C_BOOL   : 
    case T5C_SINT  : bSkip = ISLOCK8(*pCode); break;
    case T5C_DINT   : 
    case T5C_REAL   : bSkip = ISLOCK32(*pCode); break;
    case T5C_TIME   : bSkip = ISLOCKTIME(*pCode); break;
    case T5C_LINT  :
    case T5C_LREAL : bSkip = ISLOCK64(*pCode); break;
    case T5C_STRING   : bSkip = ISLOCKSTRING(*pCode); break;
    case T5C_INT  : break;
    default        : break;
    }
    if (bSkip)
    {
         pCode += 4;
         return TRUE;
    }
#endif /*T5DEF_DONTFORCELOCKEDVARS*/
    return _FT5C_CAST ();
}

T5_BOOL _FT5C_FBCALL (void)
{
	pI = pInst + *pCode;
	pI->pfHandler (T5FBCMD_ACTIVATE, pDB, pI->pClassData,
					pI->pData, pCode+2);
    pCode += (pCode[1] + 2);
    return TRUE;
}

T5_BOOL _FT5C_CUSFC (void)
{
	pF = pFC + *pCode;
	pF->pfHandler (T5FBCMD_ACTIVATE, pDB,
                   T5_PTNULL, T5_PTNULL, pCode+2);
    pCode += (pCode[1] + 3);
    return TRUE;
}

#ifdef T5DEF_STRINGSUPPORTED

T5_BOOL _FT5C_COPYSTRING (void)
{
    T5VMStr_Copy (pDB, pCode[0], pCode[1]);
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_COPYSTRING (void)
{
    IF_ISSTRINGLOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_COPYSTRING ();
}

T5_BOOL _FT5C_ADDSTRING (void)
{
    T5VMStr_Cat (pDB, pCode[0], pCode[1], pCode[2]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_ADDSTRING (void)
{
    IF_ISSTRINGLOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ADDSTRING ();
}

T5_BOOL _FT5C_GTSTRING (void)
{
    SET_D8(*pCode, T5VMStr_Compare (pDB, pCode[1], pCode[2]) > 0);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GTSTRING (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GTSTRING ();
}

T5_BOOL _FT5C_GESTRING (void)
{
    SET_D8(*pCode, T5VMStr_Compare (pDB, pCode[1], pCode[2]) >= 0);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_GESTRING (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_GESTRING ();
}

T5_BOOL _FT5C_EQSTRING (void)
{
    SET_D8(*pCode, T5VMStr_Compare (pDB, pCode[1], pCode[2]) == 0);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_EQSTRING (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_EQSTRING ();
}

T5_BOOL _FT5C_NESTRING (void)
{
    SET_D8(*pCode, T5VMStr_Compare (pDB, pCode[1], pCode[2]) != 0);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_NESTRING (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_NESTRING ();
}

T5_BOOL _FT5C_ARGETSTRING (void)
{
    T5VMStr_Copy (pDB, pCode[0],
                  (T5_WORD)(pCode[1]+GET_DINT(pCode[2])));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_AGETSTRING (void)
{
    IF_ISSTRINGLOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARGETSTRING ();
}

T5_BOOL _FT5C_ARPUTSTRING (void)
{
    T5VMStr_Copy (pDB, (T5_WORD)(pCode[0]+GET_DINT(pCode[1])),
                  pCode[2]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_APUTSTRING (void)
{
    IF_ISSTRINGLOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_ARPUTSTRING ();
}

T5_BOOL _FT5C_LOCKSTRING (void)
{
    T5VMTic_LockVar (pDB, *pCode, T5LOCK_STRING, 0L);
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_UNLOCKSTRING (void)
{
    T5VMTic_UnlockVar (pDB, *pCode, T5LOCK_STRING, 0L);
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_SETSTRING (void)
{
    if (*pCode != 0)
    {
        T5VMStr_DBForce (pDB, *pCode, pCode + 1);
    }
#ifdef T5DEF_CTSEG
    else if (dwCTOffset != 0xffffffffL)
    {
        T5VMStr_DBForcePtr (pCTSeg+dwCTOffset, pCode + 1);
        dwCTOffset = 0xffffffffL;
    }
#endif /*T5DEF_CTSEG*/
    pCode += (pCode[1] + 2);  /* index + len + 1 word per char */
    return TRUE;
}

T5_BOOL _FT5C_O_SETSTRING (void)
{
    IF_ISD8LOCKED(*pCode)
    {
        pCode += (pCode[1] + 2);  /* index + len + 1 word per char */
        return TRUE;
    }
    return _FT5C_SETSTRING ();
}

#endif /*T5DEF_STRINGSUPPORTED*/

#ifdef T5DEF_SFC

T5_BOOL _FT5C_TRSASSIGN (void)
{
    pTrans[*pCode].wValid = GET_D8(pCode[1]);
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_TRANSSOURCE1 (void)
{
    if (*pPrgStatus == T5FLAG_PRGTOSTART)
    {
        T5TRACEDW("Trans Source - Prg", wPrgNo);
        T5TRACEDW("   Step = ", pCode[1]);
        *pPrgStatus = T5FLAG_PRGACTIVE;
        pStep[pCode[1]].wStatus = T5STEP_P1;
        pStep[pCode[1]].dwAcTime = 0L;
    }
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_TRANSSOURCE (void)
{
    if (*pPrgStatus == T5FLAG_PRGTOSTART)
    {
        *pPrgStatus = T5FLAG_PRGACTIVE;
        for (wIndex=0; wIndex<pCode[1]; wIndex++)
            pStep[pCode[2+wIndex]].wStatus = T5STEP_P1;
    }
    pCode += (2 + pCode[1]);
    return TRUE;
}

T5_BOOL _FT5C_TRANSEVAL1 (void)
{
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
    return TRUE;
}

T5_BOOL _FT5C_TRANSEVAL (void)
{
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
    return TRUE;
}

T5_BOOL _FT5C_TRANSCROSS1 (void)
{
    if (pTrans[*pCode].wValid)
    {
        pStep[pCode[1]].wStatus = T5STEP_P0;
        pStep[pCode[2]].wStatus = T5STEP_P1;
        if (pTrans[*pCode].wBreak)
        {
            pStatus->wFlags &= ~T5FLAG_RUN;
            pStatus->wFlags &= ~T5FLAG_PROGSTEP;
            return FALSE;
        }
    }
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_TRANSCROSSP0 (void)
{
    if (pTrans[*pCode].wValid)
    {
        for (wIndex=0; wIndex<pCode[1]; wIndex++)
            pStep[pCode[wIndex+2]].wStatus = T5STEP_P0;
    }
    pCode += (2 + pCode[1]);
    return TRUE;
}

T5_BOOL _FT5C_TRANSCROSSP1 (void)
{
    if (pTrans[*pCode].wValid)
    {
        for (wIndex=0; wIndex<pCode[1]; wIndex++)
            pStep[pCode[wIndex+2]].wStatus = T5STEP_P1;
        if (pTrans[*pCode].wBreak)
        {
            pStatus->wFlags &= ~T5FLAG_RUN;
            pStatus->wFlags &= ~T5FLAG_PROGSTEP;
            return FALSE;
        }
    }
    pCode += (2 + pCode[1]);
    return TRUE;
}

T5_BOOL _FT5C_STEP (void)
{
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
    return TRUE;
}

T5_BOOL _FT5C_STEPFLAG (void)
{
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
    return TRUE;
}

T5_BOOL _FT5C_STEPTIME (void)
{
    if (!ISLOCK8(*pCode))
        SET_D8(*pCode, (pStep[pCode[1]].wStatus != 0));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_SELFPRGCONTROL (void)
{
    if ((*pPrgStatus & T5FLAG_PRGTOSTOP) != 0)
    {
        for (wIndex=0; wIndex<*pCode; wIndex++)
        {
            if ((pProg[pCode[wIndex+1]].wFlags & T5FLAG_PRGACTIVE) != 0)
                pProg[pCode[wIndex+1]].wFlags |= T5FLAG_PRGTOSTOP;
        }
        *pPrgStatus = 0;
    }
    return FALSE; /* end of SFC program code */
}

T5_BOOL _FT5C_SFCBKS (void)
{
    T5VMTic_SetSfcBreakS (pDB, *pCode, pCode[1]);
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_SFCBKT (void)
{
    T5VMTic_SetSfcBreakT (pDB, *pCode, pCode[1]);
    pCode += 2;
    return TRUE;
}

#endif /*T5DEF_SFC*/

T5_BOOL _FT5C_POUSTART (void)
{
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
    return TRUE;
}

T5_BOOL _FT5C_POUSTOP (void)
{
    T5TRACEDW("GKill", *pCode);
    if ((pProg[*pCode].wFlags & T5FLAG_PRGACTIVE) != 0)
    {
        pProg[*pCode].wFlags &= ~T5FLAG_PRGSUSPENDED;
        if (pProg[*pCode].wStyle == T5PRGSTYLE_DEFAULT)
            pProg[*pCode].wFlags = 0;
        else pProg[*pCode].wFlags |= T5FLAG_PRGTOSTOP;
    }
    pCode++;
    return TRUE;
}

T5_BOOL _FT5C_POUSUSPEND (void)
{
    if (pProg[*pCode].wFlags == T5FLAG_PRGACTIVE)
        pProg[*pCode].wFlags |= T5FLAG_PRGSUSPENDED;
    pCode++;
    return TRUE;
}

T5_BOOL _FT5C_POURESUME (void)
{
    if ((pProg[*pCode].wFlags & T5FLAG_PRGACTIVE) != 0
        && (pProg[*pCode].wFlags & T5FLAG_PRGSUSPENDED) != 0)
        pProg[*pCode].wFlags &= ~T5FLAG_PRGSUSPENDED;
    pCode++;
    return TRUE;
}

T5_BOOL _FT5C_POUSTATUS (void)
{
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
    return TRUE;
}

T5_BOOL _FT5C_O_POUSTATUS (void)
{
    IF_ISD32LOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_POUSTATUS ();
}

#ifdef T5DEF_XV

T5_BOOL _FT5C_XVGET8 (void)
{
    SET_D8(pCode[1], *(pXV[*pCode]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVGET8 (void)
{
    IF_ISD8LOCKED(pCode[1])
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_XVGET8 ();
}

T5_BOOL _FT5C_XVSET8 (void)
{
    *(pXV[*pCode]) = GET_D8(pCode[1]);
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVSET8 (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_XVSET8 ();
}

T5_BOOL _FT5C_XAGET8 (void)
{
    SET_D8(pCode[1], *(pXV[*pCode + GET_DINT(pCode[2])]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XAGET8 (void)
{
    IF_ISD8LOCKED(pCode[1])
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XAGET8 ();
}

T5_BOOL _FT5C_XASET8 (void)
{
    *(pXV[*pCode + GET_DINT(pCode[2])]) = GET_D8(pCode[1]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XASET8 (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XASET8 ();
}

T5_BOOL _FT5C_XVF8 (void)
{
    *(pXV[*pCode]) = (T5_BYTE)(pCode[1] & 0xff);
    pCode += 2;
    return TRUE;
}

#ifdef T5DEF_DATA16SUPPORTED

T5_BOOL _FT5C_XVGET16 (void)
{
    SET_D16(pCode[1], *(T5_PTSHORT)(pXV[*pCode]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVGET16 (void)
{
    IF_ISD16LOCKED(pCode[1])
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_XVGET16 ();
}

T5_BOOL _FT5C_XVSET16 (void)
{
    *(T5_PTSHORT)(pXV[*pCode]) = GET_D16(pCode[1]);
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVSET16 (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    *(T5_PTSHORT)(pXV[*pCode]) = GET_D16(pCode[1]);
    pCode += 2;
    return _FT5C_XVSET16 ();
}

T5_BOOL _FT5C_XAGET16 (void)
{
    SET_D16(pCode[1], *(T5_PTSHORT)(pXV[*pCode + GET_DINT(pCode[2])]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XAGET16 (void)
{
    IF_ISD16LOCKED(pCode[1])
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XAGET16 ();
}

T5_BOOL _FT5C_XASET16 (void)
{
    *(T5_PTSHORT)(pXV[*pCode + GET_DINT(pCode[2])]) = GET_D16(pCode[1]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XASET16 (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XASET16 ();
}

T5_BOOL _FT5C_XVF16 (void)
{
    *(T5_PTSHORT)(pXV[*pCode]) = (T5_SHORT)(pCode[1]);
    pCode += 2;
    return TRUE;
}

#endif /*T5DEF_DATA16SUPPORTED*/

T5_BOOL _FT5C_XVGET32 (void)
{
    SET_DINT(pCode[1], *(T5_PTLONG)(pXV[*pCode]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVGET32 (void)
{
    IF_ISD32LOCKED(pCode[1])
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_XVGET32 ();
}

T5_BOOL _FT5C_XVSET32 (void)
{
    *(T5_PTLONG)(pXV[*pCode]) = GET_DINT(pCode[1]);
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVSET32 (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_XVSET32 ();
}

T5_BOOL _FT5C_XAGET32 (void)
{
    SET_DINT(pCode[1], *(T5_PTLONG)(pXV[*pCode + GET_DINT(pCode[2])]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XAGET32 (void)
{
    IF_ISD32LOCKED(pCode[1])
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XAGET32 ();
}

T5_BOOL _FT5C_XASET32 (void)
{
    *(T5_PTLONG)(pXV[*pCode + GET_DINT(pCode[2])]) = GET_DINT(pCode[1]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XASET32 (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XASET32 ();
}

T5_BOOL _FT5C_XVF32 (void)
{
    T5_COPYWORDSTOLONG(pXV[*pCode], (T5_PTBYTE)(pCode+1));
    pCode += 3;
    return TRUE;
}

#ifdef T5DEF_DATA64SUPPORTED

T5_BOOL _FT5C_XVGET64 (void)
{
    SET_DATA64(pCode[1], *(T5_PTDATA64)(pXV[*pCode]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVGET64 (void)
{
    IF_ISD64LOCKED(pCode[1])
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_XVGET64 ();
}

T5_BOOL _FT5C_XVSET64 (void)
{
    *(T5_PTDATA64)(pXV[*pCode]) = GET_DATA64(pCode[1]);
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVSET64 (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_XVSET64 ();
}

T5_BOOL _FT5C_XAGET64 (void)
{
    SET_DATA64(pCode[1], *(T5_PTDATA64)(pXV[*pCode + GET_DINT(pCode[2])]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XAGET64 (void)
{
    IF_ISD64LOCKED(pCode[1])
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XAGET64 ();
}

T5_BOOL _FT5C_XASET64 (void)
{
    *(T5_PTDATA64)(pXV[*pCode + GET_DINT(pCode[2])]) = GET_DATA64(pCode[1]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XASET64 (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XASET64 ();
}

T5_BOOL _FT5C_XVF64 (void)
{
    T5_COPYWORDSTO64(pXV[*pCode], (T5_PTBYTE)(pCode+1));
    pCode += 5;
    return TRUE;
}

#endif /*T5DEF_DATA64SUPPORTED*/

T5_BOOL _FT5C_XVGETTIME (void)
{
    SET_TIME(pCode[1], *(T5_PTDWORD)(pXV[*pCode]));
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVGETTIME (void)
{
    IF_ISTIMELOCKED(pCode[1])
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_XVGETTIME ();
}

T5_BOOL _FT5C_XVSETTIME (void)
{
    *(T5_PTDWORD)(pXV[*pCode]) = GET_TIME(pCode[1]);
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVSETTIME (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_XVSETTIME ();
}

T5_BOOL _FT5C_XAGETTIME (void)
{
    SET_TIME(pCode[1], *(T5_PTDWORD)(pXV[*pCode + GET_DINT(pCode[2])]));
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XAGETTIME (void)
{
    IF_ISTIMELOCKED(pCode[1])
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XAGETTIME ();
}

T5_BOOL _FT5C_XASETTIME (void)
{
    *(T5_PTDWORD)(pXV[*pCode + GET_DINT(pCode[2])]) = GET_TIME(pCode[1]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XASETTIME (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XASETTIME ();
}

#ifdef T5DEF_STRINGSUPPORTED

T5_BOOL _FT5C_XVGETSTRING (void)
{
    pXS = T5GET_DBSTRING(pDB);
    T5VMStr_CopyPtr (pXS[pCode[1]], pXV[*pCode]);
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVGETSTRING (void)
{
    IF_ISSTRINGLOCKED(pCode[1])
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_XVGETSTRING ();
}

T5_BOOL _FT5C_XVSETSTRING (void)
{
    pXS = T5GET_DBSTRING(pDB);
    T5VMStr_CopyPtr (pXV[*pCode], pXS[pCode[1]]);
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_O_XVSETSTRING (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 2;
        return TRUE;
    }
    return _FT5C_XVSETSTRING ();
}

T5_BOOL _FT5C_XAGETSTRING (void)
{
    pXS = T5GET_DBSTRING(pDB);
    T5VMStr_CopyPtr (pXS[pCode[1]], pXV[*pCode + GET_DINT(pCode[2])]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XAGETSTRING (void)
{
    IF_ISSTRINGLOCKED(pCode[1])
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XAGETSTRING ();
}

T5_BOOL _FT5C_XASETSTRING (void)
{
    pXS = T5GET_DBSTRING(pDB);
    T5VMStr_CopyPtr (pXV[*pCode + GET_DINT(pCode[2])], pXS[pCode[1]]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_O_XASETSTRING (void)
{
    IF_ISXVLOCKED(*pCode)
    {
        pCode += 3;
        return TRUE;
    }
    return _FT5C_XASETSTRING ();
}

T5_BOOL _FT5C_XVFSTRING (void)
{
    T5VMStr_DBForcePtr (pXV[*pCode], pCode + 1);
    pCode += (pCode[1] + 2);  /* index + len + 1 word per char */
    return TRUE;
}

#endif /*T5DEF_STRINGSUPPORTED*/

T5_BOOL _FT5C_XVLOCK (void)
{
    pLock[*pCode] |= T5LOCK_XV;
    pCode ++;
    return TRUE;
}

T5_BOOL _FT5C_XVUNLOCK (void)
{
    pLock[*pCode] &= ~T5LOCK_XV;
    pCode ++;
    return TRUE;
}

#endif /*T5DEF_XV*/

T5_BOOL _FT5C_BSAMPLING (void)
{
#ifdef T5DEF_BSAMPLING
    T5VMBsmp_Set (pDB, *pCode, pCode+1);
#else /*T5DEF_BSAMPLING*/
    T5VMLog_Push (pDB, T5RET_NOBSAMPLING, 0, 0L);
#endif /*T5DEF_BSAMPLING*/
    pCode += (9 + pCode[8]);
    return TRUE;
}

T5_BOOL _FT5C_BSMPCTL (void)
{
#ifdef T5DEF_BSAMPLING
    T5VMBsmp_Ctl (pDB, pCode[0], pCode[1]);
#else /*T5DEF_BSAMPLING*/
    T5VMLog_Push (pDB, T5RET_NOBSAMPLING, 0, 0L);
#endif /*T5DEF_BSAMPLING*/
    pCode += 2;
    return TRUE;
}

#ifdef T5DEF_CTSEG

T5_BOOL _FT5C_CTSEG_NOP (void)
{
    return TRUE;
}

T5_BOOL _FT5C_GETCTSEG (void)
{
    T5VMTic_GetCTSegItem (pDB, pCode[0], pCode[1], pCode[2]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_SETCTSEG (void)
{
    T5VMTic_SetCTSegItem (pDB, pCode[0], pCode[1], pCode[2]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FTT5C_CTMEMCPY (void)
{
    T5VMTic_CTMemcpy (pDB, pCode[0], pCode[1], pCode[2]);
    pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_SETCTSEGREF (void)
{
    T5_COPYWORDSTOLONG((T5_PTBYTE)(&dwCTOffset), (T5_PTBYTE)pCode);
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_CTSEG_FBCALL (void)
{
    wIndex = *(T5_PTWORD)(pCTSeg + GET_DINT(*pCode));
    pI = pInst + wIndex;
    if (pI == NULL || pI->pfHandler == NULL)
    {
        T5TRACEDW("Invalid FB index encoutered: %u", wIndex);
        T5_PRINTF("Invalid FB index encoutered: %u", wIndex);
        pStatus->wFlags &= ~T5FLAG_RUNMASK;
        pStatus->wFlags |= T5FLAG_ERROR;
        bRetValue = FALSE;
        return FALSE;
    }
	pI->pfHandler (T5FBCMD_ACTIVATE, pDB, pI->pClassData,
					pI->pData, pCode+2);
    pCode += (pCode[1] + 2);
    return TRUE;
}

T5_BOOL _FT5C_CTSEG_FCARG (void)
{
	pF = pFC + *pCode;
	if (pF == NULL || pF->pfHandler == NULL
        || !pF->pfHandler (T5FBCMD_ACCEPTCT, pDB,
                           T5_PTNULL, T5_PTNULL, T5_PTNULL))
    {
        T5_PRINTF("Argument type not supported by function");
        T5VMLog_Push (pDB, T5RET_UNKNOWNTIC, 0, ERRDATA);
        pStatus->wFlags &= ~T5FLAG_RUNMASK;
        pStatus->wFlags |= T5FLAG_ERROR;
        bRetValue = FALSE;
        return FALSE;
    }
    pCode++;
    return TRUE;
}

T5_BOOL _FT5C_CTSEG_FBARG (void)
{
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
        bRetValue = FALSE;
        return FALSE;
    }
    pCode++;
    return TRUE;
}

T5_BOOL _FT5C_CTSEG_TSTART (void)
{
#ifdef T5DEF_ACTIMESUPPORTED
    T5VMTmr_StartTimer (pDB, (T5_PTDWORD)(pCTSeg + GET_DINT(*pCode)));
    pCode++;
#else /*T5DEF_ACTIMESUPPORTED*/
    T5VMLog_Push (pDB, T5RET_CANTACTIME, *pCode, ERRDATA);
    pStatus->wFlags &= ~T5FLAG_RUNMASK;
    pStatus->wFlags |= T5FLAG_ERROR;
    bRetValue = FALSE;
    return FALSE;
#endif /*T5DEF_ACTIMESUPPORTED*/
    return TRUE;
}

T5_BOOL _FT5C_CTSEG_TSTOP (void)
{
#ifdef T5DEF_ACTIMESUPPORTED
    T5VMTmr_StopTimer (pDB, (T5_PTDWORD)(pCTSeg + GET_DINT(*pCode)));
    pCode++;
#else /*T5DEF_ACTIMESUPPORTED*/
    T5VMLog_Push (pDB, T5RET_CANTACTIME, *pCode, ERRDATA);
    pStatus->wFlags &= ~T5FLAG_RUNMASK;
    pStatus->wFlags |= T5FLAG_ERROR;
    bRetValue = FALSE;
    return FALSE;
#endif /*T5DEF_ACTIMESUPPORTED*/
    return TRUE;
}

#endif /*T5DEF_CTSEG*/

T5_BOOL _FT5C_EXECTL (void)
{
    T5VMTic_ExeCtrl (pDB, *pCode);
    pCode++;
    return TRUE;
}

T5_BOOL _FT5C_CHECKBOUND (void)
{
    if (GET_DINT(*pCode) < ((T5_LONG)(pCode[1]) & 0xffffL)
        || GET_DINT(*pCode) > ((T5_LONG)(pCode[2]) & 0xffffL))
    {
        T5TRACEDW("Index out of bounds", GET_DINT(*pCode));
        T5VMLog_Push (pDB, T5RET_ARRAYBOUND, *pCode, ERRDATA);
        pStatus->wFlags &= ~T5FLAG_RUNMASK;
        pStatus->wFlags |= T5FLAG_ERROR;
        bRetValue = FALSE;
        return FALSE;
    }
    else
        pCode += 3;
    return TRUE;
}

T5_BOOL _FT5C_SWITCH (void)
{
    wIndex = (T5_WORD)(GET_DINT(*pCode) & 0xffff);
    if (wIndex >= pCode[1])
        pCode += (2 + pCode[1]);
    else
        pCode = (T5_PTWORD)(pStartCode + pCode[2+wIndex]);
    return TRUE;
}

T5_BOOL _FT5C_JAEQ (void)
{
    if (GET_DINT(pCode[1]) == GET_DINT(pCode[2]))
        pCode = (T5_PTWORD)(pStartCode + *pCode);
    else
        pCode += 3;
    return TRUE;
}

#ifdef T5DEF_SFC

T5_BOOL _FT5C_JNFPSTAT (void)
{
    if ((*pPrgStatus & *pCode) == 0)
        pCode = (T5_PTWORD)(pStartCode + pCode[1]);
    else
        pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_SETPSTAT (void)
{
    *pPrgStatus = *pCode;
    pCode++;
    return TRUE;
}

T5_BOOL _FT5C_SFCSETSX (void)
{
    pStep[*pCode].wStatus = pCode[1];
    if (pCode[1] == T5STEP_N) /* reset time on =N */
    {
        pStep[*pCode].dwAcTime = 0L;
        pStep[*pCode].dwLastUpdate = pStatus->dwTimStamp;
    }
    pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_SFCSETST (void)
{
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
    return TRUE;
}

T5_BOOL _FT5C_SFCCHECKTBK (void)
{
    if (pTrans[*pCode].wBreak)
    {
        pStatus->wFlags &= ~T5FLAG_RUN;
        pStatus->wFlags &= ~T5FLAG_PROGSTEP;
        bRetValue = TRUE;
        return FALSE;
    }
    pCode++;
    return TRUE;
}

T5_BOOL _FT5C_JNTRANS (void)
{
    if (!pTrans[*pCode].wValid)
        pCode = (T5_PTWORD)(pStartCode + pCode[1]);
    else
        pCode += 2;
    return TRUE;
}

T5_BOOL _FT5C_SFCTSR (void)
{
    if ((*pPrgStatus & T5FLAG_PRGTOSTOP) != 0)
    {
        SET_DINT(*pCode, 0L);
        pCode = (T5_PTWORD)(pStartCode + pCode[1]);
    }
    else
        pCode += 2;
    return TRUE;
}

#endif /*T5DEF_SFC*/

T5_BOOL _FT5C_ADR (void)
{
    T5_DWORD dwVal;

    dwVal = (T5_DWORD)T5Tools_GetAnyParam (
                    pDB, pCode[1],
                    pCode[2] + (T5_WORD)GET_DINT(pCode[3]),
                    NULL, NULL);
    SET_DINT(*pCode, dwVal);
    pCode += 4;
    return TRUE;
}

#ifdef T5DEF_CTSEG

T5_BOOL _FT5C_ADRCT (void)
{
    T5_DWORD dwVal;

    dwVal = (T5_DWORD)(pDB[T5TB_CTSEG].pData);
    dwVal += GET_DINT(pCode[1]);
    SET_DINT(*pCode, dwVal);
    pCode += 3;
    return TRUE;
}

#endif /*T5DEF_CTSEG*/

T5_BOOL _FT5C_UNLOCKALL (void)
{
    T5VMTic_UnlockAllVars (pDB);
    return TRUE;
}

/*****************************************************************************
T5VMTic_Run
execute a TIC sequence
parameters:
    p_pDB (IN) pointer to the database
    p_pStatus (IN) pointer to the application status
    p_pCode (IN) pointer to the TIC sequence
    p_dwSize (IN) size of the TIC sequence in bytes
    p_wPrgNo (IN) index of the program - 0 for the exec buffer
returns: TRUE if ok / FALSE if error encountered
*****************************************************************************/

T5_BOOL T5VMTic_Run (T5PTR_DB p_pDB, T5PTR_DBSTATUS p_pStatus,
                     T5_PTWORD p_pCode, T5_DWORD p_dwSize, T5_WORD p_wPrgNo)
{
    T5_BOOL bLoop;

    pStatus = p_pStatus;
    pCode = p_pCode;
    dwSize = p_dwSize;
    wPrgNo = p_wPrgNo;
    dwCTOffset = 0xffffffffL;
#ifdef T5DEF_TICSAFE
    dwCount = 0L;
#endif /*T5DEF_TICSAFE*/
    bRetValue = TRUE;

    if (wPrgNo) /* dont do that for exec buffer! */
    {
        if (pDB[T5TB_CALLSTACK].wNbItemUsed != 0)
        {
            wPrgNo = pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg;
            pCode = pProg[wPrgNo].pCode;
        }
        else
            pStack[pDB[T5TB_CALLSTACK].wNbItemUsed].wProg = wPrgNo;
        pPrgStatus = &(pProg[wPrgNo].wFlags);

        pStartCode = (T5_PTBYTE)pCode;
        pEndCode = pStartCode + pProg[wPrgNo].dwSize;

        pCode += pProg[wPrgNo].dwPos;
        pStatus->wCurPrg = pStatus->wCurPos = 0;
        pProg[wPrgNo].dwPos = 0;
    }
    else
    {
        pStartCode = (T5_PTBYTE)pCode;
        pEndCode = (T5_PTBYTE)pCode + dwSize;
        pPrgStatus = NULL;
    }

    bLoop = TRUE;
    while (bLoop)
    {
#ifdef T5DEF_TICSAFE
        if (dwCount++ > T5DEF_TICSAFEMAXINST)
        {
            T5VMLog_Push (pDB, T5RET_SAFETICLOOP, 0, ERRDATA);
            pStatus->wFlags &= ~T5FLAG_RUNMASK;
            pStatus->wFlags |= T5FLAG_ERROR;
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
            return FALSE;
        }
#endif /*T5DEF_TICSAFE*/

        if (*pCode < _MAX_TIC)
            bLoop = _T5FPC[*pCode++] ();
        else
        {
            _ERROR ();
            bLoop = FALSE;
        }
    }
    return bRetValue;
}

/****************************************************************************/
/* define all functions */

static void _T5VMTic_InitFTable (void)
{
    T5_WORD i;

    for (i=0; i<_MAX_TIC; i++)
        _T5FPC[i] = _ERROR;

    _T5FPC[T5C_NOP] = _FT5C_NOP;
    _T5FPC[T5C_RET] = _T5FPC[T5C_ENDSEQ] = _FT5C_RET;
    _T5FPC[T5C_SCHEDPRG] = _FT5C_SCHEDPRG;
    _T5FPC[T5C_SETWAITMARK] = _FT5C_SETWAITMARK;
    _T5FPC[T5C_WAIT] = _FT5C_WAIT;
    _T5FPC[T5C_CALLPRG] = _FT5C_CALLPRG;
    _T5FPC[T5C_STEPMARK] = _FT5C_STEPMARK;
    _T5FPC[T5C_PAUSE] = _FT5C_PAUSE;
    _T5FPC[T5C_RESUME] = _FT5C_RESUME;
    _T5FPC[T5C_CYCLESTEP] = _FT5C_CYCLESTEP;
    _T5FPC[T5C_STEPFWD] = _FT5C_STEPFWD;
    _T5FPC[T5C_SETCYCLETIME] = _FT5C_SETCYCLETIME;
    _T5FPC[T5C_SETBKP] = _FT5C_SETBKP;
    _T5FPC[T5C_JUMP] = _FT5C_JMP;
    _T5FPC[T5C_JUMPIF] = _FT5C_JMPIF;
    _T5FPC[T5C_JUMPIFNOT] = _FT5C_JMPIFNOT;
    _T5FPC[T5C_JA] = _FT5C_JA;
    _T5FPC[T5C_JAC] = _FT5C_JAC;
    _T5FPC[T5C_JACN] = _FT5C_JACN;
    _T5FPC[T5C_O_NOTBOOL] = _FT5C_O_NOTBOOL;
    _T5FPC[T5C_NOTBOOL] = _FT5C_NOTBOOL;
    _T5FPC[T5C_O_EQBOOL] = _FT5C_O_EQBOOL;
    _T5FPC[T5C_EQBOOL] = _FT5C_EQBOOL;
    _T5FPC[T5C_O_NEBOOL] = _FT5C_O_NEBOOL;
    _T5FPC[T5C_NEBOOL] = _FT5C_NEBOOL;
    _T5FPC[T5C_O_ORBOOL] = _FT5C_O_ORBOOL;
    _T5FPC[T5C_ORBOOL] = _FT5C_ORBOOL;
    _T5FPC[T5C_O_ANDBOOL] = _FT5C_O_ANDBOOL;
    _T5FPC[T5C_ANDBOOL] = _FT5C_ANDBOOL;
    _T5FPC[T5C_O_XORBOOL] = _FT5C_O_XORBOOL;
    _T5FPC[T5C_XORBOOL] = _FT5C_XORBOOL;
    _T5FPC[T5C_O_COPYBOOL] = _FT5C_O_COPYBOOL;
    _T5FPC[T5C_COPYBOOL] = _FT5C_COPYBOOL;
    _T5FPC[T5C_O_SETBOOL] = _FT5C_O_SETBOOL;
    _T5FPC[T5C_SETBOOL] = _FT5C_SETBOOL;
    _T5FPC[T5C_O_RESETBOOL] = _FT5C_O_RESETBOOL;
    _T5FPC[T5C_RESETBOOL] = _FT5C_RESETBOOL;
    _T5FPC[T5C_O_AGETBOOL] = _FT5C_O_AGETBOOL;
    _T5FPC[T5C_ARGETBOOL] = _FT5C_ARGETBOOL;
    _T5FPC[T5C_O_APUTBOOL] = _FT5C_O_APUTBOOL;
    _T5FPC[T5C_ARPUTBOOL] = _FT5C_ARPUTBOOL;
    _T5FPC[T5C_LOCKBOOL] = _T5FPC[T5C_LOCKSINT] = _FT5C_LOCKBOOL;
    _T5FPC[T5C_UNLOCKBOOL] = _T5FPC[T5C_UNLOCKSINT] = _FT5C_UNLOCKBOOL;
    _T5FPC[T5C_O_REDGE] = _FT5C_O_REDGE;
    _T5FPC[T5C_REDGE] = _FT5C_REDGE;
    _T5FPC[T5C_O_FEDGE] = _FT5C_O_FEDGE;
    _T5FPC[T5C_FEDGE] = _FT5C_FEDGE;

#ifdef T5DEF_SINTSUPPORTED

    _T5FPC[T5C_O_NEGSINT] = _FT5C_O_NEGSINT;
    _T5FPC[T5C_NEGSINT] = _FT5C_NEGSINT;
    _T5FPC[T5C_O_MULSINT] = _FT5C_O_MULSINT;
    _T5FPC[T5C_MULSINT] = _FT5C_MULSINT;
    _T5FPC[T5C_O_DIVSINT] = _FT5C_O_DIVSINT;
    _T5FPC[T5C_DIVSINT] = _FT5C_DIVSINT;
    _T5FPC[T5C_O_ADDSINT] = _FT5C_O_ADDSINT;
    _T5FPC[T5C_ADDSINT] = _FT5C_ADDSINT;
    _T5FPC[T5C_O_SUBSINT] = _FT5C_O_SUBSINT;
    _T5FPC[T5C_SUBSINT] = _FT5C_SUBSINT;
    _T5FPC[T5C_O_GTSINT] = _FT5C_O_GTSINT;
    _T5FPC[T5C_GTSINT] = _FT5C_GTSINT;
    _T5FPC[T5C_O_GESINT] = _FT5C_O_GESINT;
    _T5FPC[T5C_GESINT] = _FT5C_GESINT;
    _T5FPC[T5C_O_EQSINT] = _FT5C_O_EQSINT;
    _T5FPC[T5C_EQSINT] = _FT5C_EQSINT;
    _T5FPC[T5C_O_NESINT] = _FT5C_O_NESINT;
    _T5FPC[T5C_NESINT] = _FT5C_NESINT;
    _T5FPC[T5C_O_COPYSINT] = _FT5C_O_COPYSINT;
    _T5FPC[T5C_COPYSINT] = _FT5C_COPYSINT;
    _T5FPC[T5C_O_ARGETSINT] = _FT5C_O_ARGETSINT;
    _T5FPC[T5C_ARGETSINT] = _FT5C_ARGETSINT;
    _T5FPC[T5C_O_ARPUTSINT] = _FT5C_O_ARPUTSINT;
    _T5FPC[T5C_ARPUTSINT] = _FT5C_ARPUTSINT;
    _T5FPC[T5C_O_SETSINT] = _FT5C_O_SETSINT;
    _T5FPC[T5C_SETSINT] = _FT5C_SETSINT;

#ifdef T5DEF_DOTBITSUPPORTED

    _T5FPC[T5C_O_GETBIT8] = _FT5C_O_GETBIT8;
    _T5FPC[T5C_GETBIT8] = _FT5C_GETBIT8;
    _T5FPC[T5C_O_SETBIT8] = _FT5C_O_SETBIT8;
    _T5FPC[T5C_SETBIT8] = _FT5C_SETBIT8;

#endif /*T5DEF_DOTBITSUPPORTED*/
#endif /*T5DEF_SINTSUPPORTED*/

#ifdef T5DEF_USINTSUPPORTED

    _T5FPC[T5C_O_MULUSINT] = _FT5C_O_MULUSINT;
    _T5FPC[T5C_MULUSINT] = _FT5C_MULUSINT;
    _T5FPC[T5C_O_DIVUSINT] = _FT5C_O_DIVUSINT;
    _T5FPC[T5C_DIVUSINT] = _FT5C_DIVUSINT;
    _T5FPC[T5C_O_GTUSINT] = _FT5C_O_GTUSINT;
    _T5FPC[T5C_GTUSINT] = _FT5C_GTUSINT;
    _T5FPC[T5C_O_GEUSINT] = _FT5C_O_GEUSINT;
    _T5FPC[T5C_GEUSINT] = _FT5C_GEUSINT;

#endif /*T5DEF_USINTSUPPORTED*/

#ifdef T5DEF_DATA16SUPPORTED

    _T5FPC[T5C_O_COPYINT] = _FT5C_O_COPYINT;
    _T5FPC[T5C_COPYINT] = _FT5C_COPYINT;
    _T5FPC[T5C_O_NEGINT] = _FT5C_O_NEGINT;
    _T5FPC[T5C_NEGINT] = _FT5C_NEGINT;
    _T5FPC[T5C_O_MULINT] = _FT5C_O_MULINT;
    _T5FPC[T5C_MULINT] = _FT5C_MULINT;
    _T5FPC[T5C_O_DIVINT] = _FT5C_O_DIVINT;
    _T5FPC[T5C_DIVINT] = _FT5C_DIVINT;
    _T5FPC[T5C_O_ADDINT] = _FT5C_O_ADDINT;
    _T5FPC[T5C_ADDINT] = _FT5C_ADDINT;
    _T5FPC[T5C_O_SUBINT] = _FT5C_O_SUBINT;
    _T5FPC[T5C_SUBINT] = _FT5C_SUBINT;
    _T5FPC[T5C_O_GTINT] = _FT5C_O_GTINT;
    _T5FPC[T5C_GTINT] = _FT5C_GTINT;
    _T5FPC[T5C_O_GEINT] = _FT5C_O_GEINT;
    _T5FPC[T5C_GEINT] = _FT5C_GEINT;
    _T5FPC[T5C_O_EQINT] = _FT5C_O_EQINT;
    _T5FPC[T5C_EQINT] = _FT5C_EQINT;
    _T5FPC[T5C_O_NEINT] = _FT5C_O_NEINT;
    _T5FPC[T5C_NEINT] = _FT5C_NEINT;
    _T5FPC[T5C_O_ARGETINT] = _FT5C_O_ARGETINT;
    _T5FPC[T5C_ARGETINT] = _FT5C_ARGETINT;
    _T5FPC[T5C_O_ARPUTINT] = _FT5C_O_ARPUTINT;
    _T5FPC[T5C_ARPUTINT] = _FT5C_ARPUTINT;
    _T5FPC[T5C_O_SETINT] = _FT5C_O_SETINT;
    _T5FPC[T5C_SETINT] = _FT5C_SETINT;
    _T5FPC[T5C_LOCKINT] = _FT5C_LOCKINT;
    _T5FPC[T5C_UNLOCKINT] = _FT5C_UNLOCKINT;

#ifdef T5DEF_DOTBITSUPPORTED

    _T5FPC[T5C_O_GETBIT16] = _FT5C_O_GETBIT16;
    _T5FPC[T5C_GETBIT16] = _FT5C_GETBIT16;
    _T5FPC[T5C_O_SETBIT16] = _FT5C_O_SETBIT16;
    _T5FPC[T5C_SETBIT16] = _FT5C_SETBIT16;

#endif /*T5DEF_DOTBITSUPPORTED*/
#endif /*T5DEF_DATA16SUPPORTED*/

#ifdef T5DEF_UINTSUPPORTED

    _T5FPC[T5C_O_MULUINT] = _FT5C_O_MULUINT;
    _T5FPC[T5C_MULUINT] = _FT5C_MULUINT;
    _T5FPC[T5C_O_DIBUINT] = _FT5C_O_DIBUINT;
    _T5FPC[T5C_DIVUINT] = _FT5C_DIVUINT;
    _T5FPC[T5C_O_GTUINT] = _FT5C_O_GTUINT;
    _T5FPC[T5C_GTUINT] = _FT5C_GTUINT;
    _T5FPC[T5C_O_GEUINT] = _FT5C_O_GEUINT;
    _T5FPC[T5C_GEUINT] = _FT5C_GEUINT;

#endif /*T5DEF_UINTSUPPORTED*/

    _T5FPC[T5C_O_NEGDINT] = _FT5C_O_NEGDINT;
    _T5FPC[T5C_NEGDINT] = _FT5C_NEGDINT;
    _T5FPC[T5C_O_MULDINT] = _FT5C_O_MULDINT;
    _T5FPC[T5C_MULDINT] = _FT5C_MULDINT;
    _T5FPC[T5C_O_DIVDINT] = _FT5C_O_DIVDINT;
    _T5FPC[T5C_DIVDINT] = _FT5C_DIVDINT;
    _T5FPC[T5C_O_ADDDINT] = _FT5C_O_ADDDINT;
    _T5FPC[T5C_ADDDINT] = _FT5C_ADDDINT;
    _T5FPC[T5C_O_SUBDINT] = _FT5C_O_SUBDINT;
    _T5FPC[T5C_SUBDINT] = _FT5C_SUBDINT;
    _T5FPC[T5C_O_GTDINT] = _FT5C_O_GTDINT;
    _T5FPC[T5C_GTDINT] = _FT5C_GTDINT;
    _T5FPC[T5C_O_GEDINT] = _FT5C_O_GEDINT;
    _T5FPC[T5C_GEDINT] = _FT5C_GEDINT;
    _T5FPC[T5C_O_EQDINT] = _FT5C_O_EQDINT;
    _T5FPC[T5C_EQDINT] = _FT5C_EQDINT;
    _T5FPC[T5C_O_NEDINT] = _FT5C_O_NEDINT;
    _T5FPC[T5C_NEDINT] = _FT5C_NEDINT;
    _T5FPC[T5C_O_COPYDINT] = _FT5C_O_COPYDINT;
    _T5FPC[T5C_COPYDINT] = _FT5C_COPYDINT;
    _T5FPC[T5C_O_SETDINT] = _FT5C_O_SETDINT;
    _T5FPC[T5C_SETDINT] = _FT5C_SETDINT;
    _T5FPC[T5C_O_AGETDINT] = _FT5C_O_AGETDINT;
    _T5FPC[T5C_ARGETDINT] = _FT5C_ARGETDINT;
    _T5FPC[T5C_O_APUTDINT] = _FT5C_O_APUTDINT;
    _T5FPC[T5C_ARPUTDINT] = _FT5C_ARPUTDINT;
    _T5FPC[T5C_LOCKDINT] = _FT5C_LOCKDINT;
    _T5FPC[T5C_UNLOCKDINT] = _FT5C_UNLOCKDINT;

#ifdef T5DEF_DOTBITSUPPORTED

    _T5FPC[T5C_O_GETBIT32] = _FT5C_O_GETBIT32;
    _T5FPC[T5C_GETBIT32] = _FT5C_GETBIT32;
    _T5FPC[T5C_O_SETBIT32] = _FT5C_O_SETBIT32;
    _T5FPC[T5C_SETBIT32] = _FT5C_SETBIT32;

#endif /*T5DEF_DOTBITSUPPORTED*/
            
#ifdef T5DEF_UDINTSUPPORTED

    _T5FPC[T5C_0_MULUDINT] = _FT5C_0_MULUDINT;
    _T5FPC[T5C_MULUDINT] = _FT5C_MULUDINT;
    _T5FPC[T5C_0_DIVUDINT] = _FT5C_0_DIVUDINT;
    _T5FPC[T5C_DIVUDINT] = _FT5C_DIVUDINT;
    _T5FPC[T5C_0_GTUDINT] = _FT5C_0_GTUDINT;
    _T5FPC[T5C_GTUDINT] = _FT5C_GTUDINT;
    _T5FPC[T5C_0_GEUDINT] = _FT5C_0_GEUDINT;
    _T5FPC[T5C_GEUDINT] = _FT5C_GEUDINT;

#endif /*T5DEF_UDINTSUPPORTED*/

#ifdef T5DEF_REALSUPPORTED

    _T5FPC[T5C_O_NEGREAL] = _FT5C_O_NEGREAL;
    _T5FPC[T5C_NEGREAL] = _FT5C_NEGREAL;
    _T5FPC[T5C_O_MULREAL] = _FT5C_O_MULREAL;
    _T5FPC[T5C_MULREAL] = _FT5C_MULREAL;
    _T5FPC[T5C_O_DIVREAL] = _FT5C_O_DIVREAL;
    _T5FPC[T5C_DIVREAL] = _FT5C_DIVREAL;
    _T5FPC[T5C_O_ADDREAL] = _FT5C_O_ADDREAL;
    _T5FPC[T5C_ADDREAL] = _FT5C_ADDREAL;
    _T5FPC[T5C_O_SUBREAL] = _FT5C_O_SUBREAL;
    _T5FPC[T5C_SUBREAL] = _FT5C_SUBREAL;
    _T5FPC[T5C_O_GTREAL] = _FT5C_O_GTREAL;
    _T5FPC[T5C_GTREAL] = _FT5C_GTREAL;
    _T5FPC[T5C_O_GEREAL] = _FT5C_O_GEREAL;
    _T5FPC[T5C_GEREAL] = _FT5C_GEREAL;
    _T5FPC[T5C_O_EQREAL] = _FT5C_O_EQREAL;
    _T5FPC[T5C_EQREAL] = _FT5C_EQREAL;
    _T5FPC[T5C_O_NEREAL] = _FT5C_O_NEREAL;
    _T5FPC[T5C_NEREAL] = _FT5C_NEREAL;
    _T5FPC[T5C_O_COPYREAL] = _FT5C_O_COPYREAL;
    _T5FPC[T5C_COPYREAL] = _FT5C_COPYREAL;
    _T5FPC[T5C_O_AGETREAL] = _FT5C_O_AGETREAL;
    _T5FPC[T5C_ARGETREAL] = _FT5C_ARGETREAL;
    _T5FPC[T5C_O_APUTREAL] = _FT5C_O_APUTREAL;
    _T5FPC[T5C_ARPUTREAL] = _FT5C_ARPUTREAL;

#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_DATA64SUPPORTED

    _T5FPC[T5C_O_SETLINT] = _FT5C_O_SETLINT;
    _T5FPC[T5C_SETLINT] = _FT5C_SETLINT;
    _T5FPC[T5C_LOCKLINT] = _FT5C_LOCKLINT;
    _T5FPC[T5C_UNLOCKLINT] = _FT5C_UNLOCKLINT;

#endif /*T5DEF_DATA64SUPPORTED*/

#ifdef T5DEF_LINTSUPPORTED

    _T5FPC[T5C_O_NEGLINT] = _FT5C_O_NEGLINT;
    _T5FPC[T5C_NEGLINT] = _FT5C_NEGLINT;
    _T5FPC[T5C_O_MULLINT] = _FT5C_O_MULLINT;
    _T5FPC[T5C_MULLINT] = _FT5C_MULLINT;
    _T5FPC[T5C_O_DIVLINT] = _FT5C_O_DIVLINT;
    _T5FPC[T5C_DIVLINT] = _FT5C_DIVLINT;
    _T5FPC[T5C_O_ADDLINT] = _FT5C_O_ADDLINT;
    _T5FPC[T5C_ADDLINT] = _FT5C_ADDLINT;
    _T5FPC[T5C_O_SUBLINT] = _FT5C_O_SUBLINT;
    _T5FPC[T5C_SUBLINT] = _FT5C_SUBLINT;
    _T5FPC[T5C_O_GTLINT] = _FT5C_O_GTLINT;
    _T5FPC[T5C_GTLINT] = _FT5C_GTLINT;
    _T5FPC[T5C_O_GELINT] = _FT5C_O_GELINT;
    _T5FPC[T5C_GELINT] = _FT5C_GELINT;
    _T5FPC[T5C_O_EQLINT] = _FT5C_O_EQLINT;
    _T5FPC[T5C_EQLINT] = _FT5C_EQLINT;
    _T5FPC[T5C_O_NELINT] = _FT5C_O_NELINT;
    _T5FPC[T5C_NELINT] = _FT5C_NELINT;
    _T5FPC[T5C_O_COPYLINT] = _FT5C_O_COPYLINT;
    _T5FPC[T5C_COPYLINT] = _FT5C_COPYLINT;
    _T5FPC[T5C_O_ARGETLINT] = _FT5C_O_ARGETLINT;
    _T5FPC[T5C_ARGETLINT] = _FT5C_ARGETLINT;
    _T5FPC[T5C_O_ARPUTLINT] = _FT5C_O_ARPUTLINT;
    _T5FPC[T5C_ARPUTLINT] = _FT5C_ARPUTLINT;

#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_LREALSUPPORTED

    _T5FPC[T5C_O_NEGLREAL] = _FT5C_O_NEGLREAL;
    _T5FPC[T5C_NEGLREAL] = _FT5C_NEGLREAL;
    _T5FPC[T5C_O_MULLREAL] = _FT5C_O_MULLREAL;
    _T5FPC[T5C_MULLREAL] = _FT5C_MULLREAL;
    _T5FPC[T5C_O_DIVLREAL] = _FT5C_O_DIVLREAL;
    _T5FPC[T5C_DIVLREAL] = _FT5C_DIVLREAL;
    _T5FPC[T5C_O_ADDLREAL] = _FT5C_O_ADDLREAL;
    _T5FPC[T5C_ADDLREAL] = _FT5C_ADDLREAL;
    _T5FPC[T5C_O_SUBLREAL] = _FT5C_O_SUBLREAL;
    _T5FPC[T5C_SUBLREAL] = _FT5C_SUBLREAL;
    _T5FPC[T5C_O_GELREAL] = _FT5C_O_GELREAL;
    _T5FPC[T5C_GTLREAL] = _FT5C_GTLREAL;
    _T5FPC[T5C_O_GTLREAL] = _FT5C_O_GTLREAL;
    _T5FPC[T5C_GELREAL] = _FT5C_GELREAL;
    _T5FPC[T5C_O_EQLREAL] = _FT5C_O_EQLREAL;
    _T5FPC[T5C_EQLREAL] = _FT5C_EQLREAL;
    _T5FPC[T5C_O_NELREAL] = _FT5C_O_NELREAL;
    _T5FPC[T5C_NELREAL] = _FT5C_NELREAL;
    _T5FPC[T5C_O_COPYLREAL] = _FT5C_O_COPYLREAL;
    _T5FPC[T5C_COPYLREAL] = _FT5C_COPYLREAL;
    _T5FPC[T5C_O_ARGETLREAL] = _FT5C_O_ARGETLREAL;
    _T5FPC[T5C_ARGETLREAL] = _FT5C_ARGETLREAL;
    _T5FPC[T5C_O_ARPUTLREAL] = _FT5C_O_ARPUTLREAL;
    _T5FPC[T5C_ARPUTLREAL] = _FT5C_ARPUTLREAL;

#endif /*T5DEF_LREALSUPPORTED*/

    _T5FPC[T5C_O_ADDTIME] = _FT5C_O_ADDTIME;
    _T5FPC[T5C_ADDTIME] = _FT5C_ADDTIME;
    _T5FPC[T5C_O_SUBTIME] = _FT5C_O_SUBTIME;
    _T5FPC[T5C_SUBTIME] = _FT5C_SUBTIME;
    _T5FPC[T5C_O_GTTIME] = _FT5C_O_GTTIME;
    _T5FPC[T5C_GTTIME] = _FT5C_GTTIME;
    _T5FPC[T5C_O_GETIME] = _FT5C_O_GETIME;
    _T5FPC[T5C_GETIME] = _FT5C_GETIME;
    _T5FPC[T5C_O_EQTIME] = _FT5C_O_EQTIME;
    _T5FPC[T5C_EQTIME] = _FT5C_EQTIME;
    _T5FPC[T5C_O_NETIME] = _FT5C_O_NETIME;
    _T5FPC[T5C_NETIME] = _FT5C_NETIME;
    _T5FPC[T5C_O_COPYTIME] = _FT5C_O_COPYTIME;
    _T5FPC[T5C_COPYTIME] = _FT5C_COPYTIME;
    _T5FPC[T5C_O_SETTIME] = _FT5C_O_SETTIME;
    _T5FPC[T5C_SETTIME] = _FT5C_SETTIME;
    _T5FPC[T5C_O_STARTTIME] = _FT5C_O_STARTTIME;
    _T5FPC[T5C_STARTTIME] = _FT5C_STARTTIME;
    _T5FPC[T5C_O_STOPTIME] = _FT5C_O_STOPTIME;
    _T5FPC[T5C_STOPTIME] = _FT5C_STOPTIME;
    _T5FPC[T5C_LOCKTIME] = _FT5C_LOCKTIME;
    _T5FPC[T5C_UNLOCKTIME] = _FT5C_UNLOCKTIME;
    _T5FPC[T5C_O_AGETTIME] = _FT5C_O_AGETTIME;
    _T5FPC[T5C_ARGETTIME] = _FT5C_ARGETTIME;
    _T5FPC[T5C_O_APUTTIME] = _FT5C_O_APUTTIME;
    _T5FPC[T5C_ARPUTTIME] = _FT5C_ARPUTTIME;
    _T5FPC[T5C_STDFC] = _FT5C_STDFC;
    _T5FPC[T5C_O_CAST] = _FT5C_O_CAST;
    _T5FPC[T5C_CAST] = _FT5C_CAST;
    _T5FPC[T5C_FBCALL] = _FT5C_FBCALL;
    _T5FPC[T5C_CUSFC] = _FT5C_CUSFC;

#ifdef T5DEF_STRINGSUPPORTED

    _T5FPC[T5C_O_COPYSTRING] = _FT5C_O_COPYSTRING;
    _T5FPC[T5C_COPYSTRING] = _FT5C_COPYSTRING;
    _T5FPC[T5C_O_ADDSTRING] = _FT5C_O_ADDSTRING;
    _T5FPC[T5C_ADDSTRING] = _FT5C_ADDSTRING;
    _T5FPC[T5C_O_GTSTRING] = _FT5C_O_GTSTRING;
    _T5FPC[T5C_GTSTRING] = _FT5C_GTSTRING;
    _T5FPC[T5C_O_GESTRING] = _FT5C_O_GESTRING;
    _T5FPC[T5C_GESTRING] = _FT5C_GESTRING;
    _T5FPC[T5C_O_EQSTRING] = _FT5C_O_EQSTRING;
    _T5FPC[T5C_EQSTRING] = _FT5C_EQSTRING;
    _T5FPC[T5C_O_NESTRING] = _FT5C_O_NESTRING;
    _T5FPC[T5C_NESTRING] = _FT5C_NESTRING;
    _T5FPC[T5C_O_AGETSTRING] = _FT5C_O_AGETSTRING;
    _T5FPC[T5C_ARGETSTRING] = _FT5C_ARGETSTRING;
    _T5FPC[T5C_O_APUTSTRING] = _FT5C_O_APUTSTRING;
    _T5FPC[T5C_ARPUTSTRING] = _FT5C_ARPUTSTRING;
    _T5FPC[T5C_LOCKSTRING] = _FT5C_LOCKSTRING;
    _T5FPC[T5C_UNLOCKSTRING] = _FT5C_UNLOCKSTRING;
    _T5FPC[T5C_O_SETSTRING] = _FT5C_O_SETSTRING;
    _T5FPC[T5C_SETSTRING] = _FT5C_SETSTRING;

#endif /*T5DEF_STRINGSUPPORTED*/

#ifdef T5DEF_SFC

    _T5FPC[T5C_TRSASSIGN] = _FT5C_TRSASSIGN;
    _T5FPC[T5C_TRANSSOURCE1] = _FT5C_TRANSSOURCE1;
    _T5FPC[T5C_TRANSSOURCE] = _FT5C_TRANSSOURCE;
    _T5FPC[T5C_TRANSEVAL1] = _FT5C_TRANSEVAL1;
    _T5FPC[T5C_TRANSEVAL] = _FT5C_TRANSEVAL;
    _T5FPC[T5C_TRANSCROSS1] = _FT5C_TRANSCROSS1;
    _T5FPC[T5C_TRANSCROSSP0] = _FT5C_TRANSCROSSP0;
    _T5FPC[T5C_TRANSCROSSP1] = _FT5C_TRANSCROSSP1;
    _T5FPC[T5C_STEP] = _FT5C_STEP;
    _T5FPC[T5C_STEPFLAG] = _FT5C_STEPFLAG;
    _T5FPC[T5C_STEPTIME] = _FT5C_STEPTIME;
    _T5FPC[T5C_SELFPRGCONTROL] = _FT5C_SELFPRGCONTROL;
    _T5FPC[T5C_SFCBKS] = _FT5C_SFCBKS;
    _T5FPC[T5C_SFCBKT] = _FT5C_SFCBKT;

#endif /*T5DEF_SFC*/

    _T5FPC[T5C_POUSTART] = _FT5C_POUSTART;
    _T5FPC[T5C_POUSTOP] = _FT5C_POUSTOP;
    _T5FPC[T5C_POUSUSPEND] = _FT5C_POUSUSPEND;
    _T5FPC[T5C_POURESUME] = _FT5C_POURESUME;
    _T5FPC[T5C_O_POUSTATUS] = _FT5C_O_POUSTATUS;
    _T5FPC[T5C_POUSTATUS] = _FT5C_POUSTATUS;

#ifdef T5DEF_XV

    _T5FPC[T5C_O_XVGET8] = _FT5C_O_XVGET8;
    _T5FPC[T5C_XVGET8] = _FT5C_XVGET8;
    _T5FPC[T5C_O_XVSET8] = _FT5C_O_XVSET8;
    _T5FPC[T5C_XVSET8] = _FT5C_XVSET8;
    _T5FPC[T5C_O_XAGET8] = _FT5C_O_XAGET8;
    _T5FPC[T5C_XAGET8] = _FT5C_XAGET8;
    _T5FPC[T5C_O_XASET8] = _FT5C_O_XASET8;
    _T5FPC[T5C_XASET8] = _FT5C_XASET8;
    _T5FPC[T5C_XVF8] = _FT5C_XVF8;

#ifdef T5DEF_DATA16SUPPORTED

    _T5FPC[T5C_O_XVGET16] = _FT5C_O_XVGET16;
    _T5FPC[T5C_XVGET16] = _FT5C_XVGET16;
    _T5FPC[T5C_O_XVSET16] = _FT5C_O_XVSET16;
    _T5FPC[T5C_XVSET16] = _FT5C_XVSET16;
    _T5FPC[T5C_O_XAGET16] = _FT5C_O_XAGET16;
    _T5FPC[T5C_XAGET16] = _FT5C_XAGET16;
    _T5FPC[T5C_O_XASET16] = _FT5C_O_XASET16;
    _T5FPC[T5C_XASET16] = _FT5C_XASET16;
    _T5FPC[T5C_XVF16] = _FT5C_XVF16;

#endif /*T5DEF_DATA16SUPPORTED*/

    _T5FPC[T5C_O_XVGET32] = _FT5C_O_XVGET32;
    _T5FPC[T5C_XVGET32] = _FT5C_XVGET32;
    _T5FPC[T5C_O_XVSET32] = _FT5C_O_XVSET32;
    _T5FPC[T5C_XVSET32] = _FT5C_XVSET32;
    _T5FPC[T5C_O_XAGET32] = _FT5C_O_XAGET32;
    _T5FPC[T5C_XAGET32] = _FT5C_XAGET32;
    _T5FPC[T5C_O_XASET32] = _FT5C_O_XASET32;
    _T5FPC[T5C_XASET32] = _FT5C_XASET32;
    _T5FPC[T5C_XVF32] = _T5FPC[T5C_XVFTIME] = _FT5C_XVF32;

#ifdef T5DEF_DATA64SUPPORTED

    _T5FPC[T5C_O_XVGET64] = _FT5C_O_XVGET64;
    _T5FPC[T5C_XVGET64] = _FT5C_XVGET64;
    _T5FPC[T5C_O_XVSET64] = _FT5C_O_XVSET64;
    _T5FPC[T5C_XVSET64] = _FT5C_XVSET64;
    _T5FPC[T5C_O_XAGET64] = _FT5C_O_XAGET64;
    _T5FPC[T5C_XAGET64] = _FT5C_XAGET64;
    _T5FPC[T5C_O_XASET64] = _FT5C_O_XASET64;
    _T5FPC[T5C_XASET64] = _FT5C_XASET64;
    _T5FPC[T5C_XVF64] = _FT5C_XVF64;

#endif /*T5DEF_DATA64SUPPORTED*/

    _T5FPC[T5C_O_XVGETTIME] = _FT5C_O_XVGETTIME;
    _T5FPC[T5C_XVGETTIME] = _FT5C_XVGETTIME;
    _T5FPC[T5C_O_XVSETTIME] = _FT5C_O_XVSETTIME;
    _T5FPC[T5C_XVSETTIME] = _FT5C_XVSETTIME;
    _T5FPC[T5C_O_XAGETTIME] = _FT5C_O_XAGETTIME;
    _T5FPC[T5C_XAGETTIME] = _FT5C_XAGETTIME;
    _T5FPC[T5C_O_XASETTIME] = _FT5C_O_XASETTIME;
    _T5FPC[T5C_XASETTIME] = _FT5C_XASETTIME;

#ifdef T5DEF_STRINGSUPPORTED

    _T5FPC[T5C_O_XVGETSTRING] = _FT5C_O_XVGETSTRING;
    _T5FPC[T5C_XVGETSTRING] = _FT5C_XVGETSTRING;
    _T5FPC[T5C_O_XVSETSTRING] = _FT5C_O_XVSETSTRING;
    _T5FPC[T5C_XVSETSTRING] = _FT5C_XVSETSTRING;
    _T5FPC[T5C_O_XAGETSTRING] = _FT5C_O_XAGETSTRING;
    _T5FPC[T5C_XAGETSTRING] = _FT5C_XAGETSTRING;
    _T5FPC[T5C_O_XASETSTRING] = _FT5C_O_XASETSTRING;
    _T5FPC[T5C_XASETSTRING] = _FT5C_XASETSTRING;
    _T5FPC[T5C_XVFSTRING] = _FT5C_XVFSTRING;

#endif /*T5DEF_STRINGSUPPORTED*/

    _T5FPC[T5C_XVLOCK] = _FT5C_XVLOCK;
    _T5FPC[T5C_XVUNLOCK] = _FT5C_XVUNLOCK;

#endif /*T5DEF_XV*/

    _T5FPC[T5C_BSAMPLING] = _FT5C_BSAMPLING;
    _T5FPC[T5C_BSMPCTL] = _FT5C_BSMPCTL;

#ifdef T5DEF_CTSEG

    _T5FPC[T5C_CTSEG_NOP] = _FT5C_CTSEG_NOP;
    _T5FPC[T5C_GETCTSEG] = _FT5C_GETCTSEG;
    _T5FPC[T5C_SETCTSEG] = _FT5C_SETCTSEG;
    _T5FPC[T5C_SETCTSEGREF] = _FT5C_SETCTSEGREF;
    _T5FPC[T5C_CTSEG_FBCALL] = _FT5C_CTSEG_FBCALL;
    _T5FPC[T5C_CTSEG_FCARG] = _FT5C_CTSEG_FCARG;
    _T5FPC[T5C_CTSEG_FBARG] = _FT5C_CTSEG_FBARG;
    _T5FPC[T5C_CTSEG_TSTART] = _FT5C_CTSEG_TSTART;
    _T5FPC[T5C_CTSEG_TSTOP] = _FT5C_CTSEG_TSTOP;
    _T5FPC[T5C_CTMEMCPY] = _FTT5C_CTMEMCPY;

#endif /*T5DEF_CTSEG*/

    _T5FPC[T5C_EXECTL] = _FT5C_EXECTL;
    _T5FPC[T5C_CHECKBOUND] = _FT5C_CHECKBOUND;
    _T5FPC[T5C_SWITCH] = _FT5C_SWITCH;
    _T5FPC[T5C_JAEQ] = _FT5C_JAEQ;

#ifdef T5DEF_SFC
    _T5FPC[T5C_JNFPSTAT] = _FT5C_JNFPSTAT;
    _T5FPC[T5C_SETPSTAT] = _FT5C_SETPSTAT;
    _T5FPC[T5C_SFCSETSX] = _FT5C_SFCSETSX;
    _T5FPC[T5C_SFCSETST] = _FT5C_SFCSETST;
    _T5FPC[T5C_SFCCHECKTBK] = _FT5C_SFCCHECKTBK;
    _T5FPC[T5C_JNTRANS] = _FT5C_JNTRANS;
    _T5FPC[T5C_SFCTSR] = _FT5C_SFCTSR;
#endif /*T5DEF_SFC*/

    _T5FPC[T5C_ADR] = _FT5C_ADR;

#ifdef T5DEF_CTSEG

    _T5FPC[T5C_ADRCT] = _FT5C_ADRCT;
    _T5FPC[T5C_UNLOCKALL] = _FT5C_UNLOCKALL;

#endif /*T5DEF_CTSEG*/

}

/****************************************************************************/

#endif /*T5DEF_NOPCSWITCH*/

/* eof **********************************************************************/
