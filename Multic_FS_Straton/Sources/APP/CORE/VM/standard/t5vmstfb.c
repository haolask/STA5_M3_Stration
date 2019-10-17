/*****************************************************************************
T5VMstfb.c : standard function block classes

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#include "t5vm.h"
#include "monitor.h"

#include "std_type.h"  //leizhu add for lin test 180709
/* note:
FBLs always make write access to private internal variables
reserved just for them. These variables do not need lock check
*/

/****************************************************************************/

#ifdef T5DEF_VLST
#ifdef T5DEF_FILESUPPORTED

static T5_BOOL _LogCSV (T5PTR_DB pDB, T5_DWORD hFile, T5_DWORD dwList,
                        T5_PTDWORD pdwErr);

#endif /*T5DEF_FILESUPPORTED*/
#endif /*T5DEF_VLST*/

/****************************************************************************/
/* access to outputs */

#ifdef T5DEF_DONTFORCELOCKEDVARS

#define ISLOCK8(i)      ((pLock[i] & T5LOCK_DATA8) != 0)
#define ISLOCK16(i)     ((pLock[i] & T5LOCK_DATA16) != 0)
#define ISLOCK32(i)     ((pLock[i] & T5LOCK_DATA32) != 0)
#define ISLOCK64(i)     ((pLock[i] & T5LOCK_DATA64) != 0)
#define ISLOCKTIME(i)   ((pLock[i] & T5LOCK_TIME) != 0)
#define ISLOCKSTRING(i) ((pLock[i] & T5LOCK_STRING) != 0)

#else /*T5DEF_DONTFORCELOCKEDOUTPUTS*/

#define ISLOCK8(i)      FALSE
#define ISLOCK16(i)     FALSE
#define ISLOCK32(i)     FALSE
#define ISLOCK64(i)     FALSE
#define ISLOCKTIME(i)   FALSE
#define ISLOCKSTRING(i) FALSE

#endif /*T5DEF_DONTFORCELOCKEDOUTPUTS*/

/****************************************************************************/
/* access to 8 bit data */

#define GET_D8IN(i)         (pData8[pArgs[i]])
#define SET_D8OUT(i,b)      {if (!ISLOCK8(pArgs[i])) pData8[pArgs[i]] = b;}

/****************************************************************************/
/* access to 16 bit data */

#define GET_D16IN(i)        (pData16[pArgs[i]])
#define SET_D16OUT(i,w)     {if (!ISLOCK16(pArgs[i])) pData16[pArgs[i]] = w;}

/****************************************************************************/
/* access to 32 bit data */

#define GET_DINTIN(i)       (pDint[pArgs[i]])
#define SET_DINTOUT(i,d)    {if (!ISLOCK32(pArgs[i])) pDint[pArgs[i]] = d;}

#ifdef T5DEF_REALSUPPORTED
#define GET_REALIN(i)       (pReal[pArgs[i]])
#define SET_REALOUT(i,r)    {if (!ISLOCK32(pArgs[i])) pReal[pArgs[i]] = r;}
#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/
/* access to 64 bit data */

#ifdef T5DEF_LREALSUPPORTED
#define GET_LREALIN(i)      (pLReal[pArgs[i]])
#define SET_LREALOUT(i,r)   {if (!ISLOCK64(pArgs[i])) pLReal[pArgs[i]] = r;}
#endif /*T5DEF_LREALSUPPORTED*/

/****************************************************************************/
/* access to TIME data */

#define GET_TIMEIN(i)       (pTime[pArgs[i]])
#define SET_TIMEOUT(i,d)    {if (!ISLOCKTIME(pArgs[i])) pTime[pArgs[i]] = d;}

/****************************************************************************/
/* triggers */

#define _P_IN   GET_D8IN(0)
#define _P_Q    1

T5_DWORD T5stfb_R_TRIG (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pMem;
    T5_PTBYTE pData8, pLock;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        /* any BOOL parameter is a byte containing 0 or 1 */
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pMem = (T5_PTBYTE)pInst;
        SET_D8OUT(_P_Q, _P_IN & !(*pMem));
        *pMem = _P_IN;
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_INITINSTANCE :
        pStatus = T5GET_DBSTATUS (pBase);
        if ((pStatus->dwFlagsEx & T5FLAGEX_RTRIGNO0) != 0)
            *(T5_PTBYTE)pInst = 1;
        return 0L;
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_R_TRIG;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_BYTE);
    default :
        return 0L;
    }
}

T5_DWORD T5stfb_F_TRIG (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pMem;
    T5_PTBYTE pData8, pLock;
    T5_UNUSED(pClass);

	if (!wCommand)
	{
        /* any BOOL parameter is a byte containing 0 or 1 */
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pMem = (T5_PTBYTE)pInst;
        SET_D8OUT(_P_Q, ((T5_BYTE)(!_P_IN)) & (*pMem));
        *pMem = _P_IN;
        return 0L;
	}
	switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_F_TRIG;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_BYTE);
    default :
        return 0L;
    }
}

#undef _P_IN
#undef _P_Q

/****************************************************************************/
/* bistable */

#define _P_S    GET_D8IN(0)
#define _P_R    GET_D8IN(1)
#define _P_Q    2

T5_DWORD T5stfb_RS (T5_WORD wCommand, T5PTR_DB pBase,
                    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pMem, pData8, pLock;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
		pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pMem = (T5_PTBYTE)pInst;
        if (_P_R) *pMem = FALSE;
        else if (_P_S) *pMem = TRUE;
        SET_D8OUT(_P_Q, *pMem);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_RS;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_BYTE);
	default :
        return 0L;
    }
}

T5_DWORD T5stfb_SR (T5_WORD wCommand, T5PTR_DB pBase,
                    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pMem;
    T5_PTBYTE pData8, pLock;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
		pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pMem = (T5_PTBYTE)pInst;
        if (_P_S) *pMem = TRUE;
        else if (_P_R) *pMem = FALSE;
        SET_D8OUT(_P_Q, *pMem);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_SR;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_BYTE);
    default :
        return 0L;
    }
}

#undef _P_S
#undef _P_R
#undef _P_Q

/****************************************************************************/
/* counter UP */

#define _P_CU       GET_D8IN(0)
#define _P_RESET    GET_D8IN(1)
#define _P_PV       GET_DINTIN(2)
#define _P_Q        3
#define _P_CV       4

T5_DWORD T5stfb_CTU (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint, pMem;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pMem = (T5_PTLONG)pInst;
        if (_P_RESET) *pMem = 0;
        else if (_P_CU && *pMem < _P_PV) *pMem += 1;
        SET_D8OUT(_P_Q, (*pMem >= _P_PV));
        SET_DINTOUT(_P_CV, *pMem);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_CTU;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_DWORD);
    default :
        return 0L;
    }
}

#undef _P_CU
#undef _P_RESET
#undef _P_PV
#undef _P_Q
#undef _P_CV

/****************************************************************************/
/* counter DOWN */

#define _P_CD       GET_D8IN(0)
#define _P_LOAD     GET_D8IN(1)
#define _P_PV       GET_DINTIN(2)
#define _P_Q        3
#define _P_CV       4

T5_DWORD T5stfb_CTD (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint, pMem;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pMem = (T5_PTLONG)pInst;
        if (_P_LOAD) *pMem = _P_PV;
        else if (_P_CD && *pMem > 0) *pMem -= 1;
        SET_D8OUT(_P_Q, (*pMem <= 0));
        SET_DINTOUT(_P_CV, *pMem);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_CTD;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_DWORD);
    default :
        return 0L;
    }
}

#undef _P_CD
#undef _P_LOAD
#undef _P_PV
#undef _P_Q
#undef _P_CV

/****************************************************************************/
/* counter UP/DOWN */

#define _P_CU       GET_D8IN(0)
#define _P_CD       GET_D8IN(1)
#define _P_RESET    GET_D8IN(2)
#define _P_LOAD     GET_D8IN(3)
#define _P_PV       GET_DINTIN(4)
#define _P_QU       5
#define _P_QD       6
#define _P_CV       7

T5_DWORD T5stfb_CTUD (T5_WORD wCommand, T5PTR_DB pBase,
                      T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint, pMem;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pMem = (T5_PTLONG)pInst;
        if (_P_RESET) *pMem = 0;
        else if (_P_LOAD) *pMem = _P_PV;
        else if (_P_CU && *pMem < _P_PV) *pMem += 1;
        else if (_P_CD && !_P_CU &&  *pMem > 0) *pMem -= 1;
        SET_D8OUT(_P_QU, (*pMem >= _P_PV));
        SET_D8OUT(_P_QD, (*pMem <= 0));
        SET_DINTOUT(_P_CV, *pMem);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_CTUD;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_DWORD);
    default :
        return 0L;
    }
}

#undef _P_CU
#undef _P_CD
#undef _P_RESET
#undef _P_LOAD
#undef _P_PV
#undef _P_QU
#undef _P_QD
#undef _P_CV

/****************************************************************************/
/* comparison */

#define _P_IN1      GET_DINTIN(0)
#define _P_IN2      GET_DINTIN(1)
#define _P_LT       2
#define _P_EQ       3
#define _P_GT       4

T5_DWORD T5stfb_CMP (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint;
    T5_UNUSED(pClass);
    T5_UNUSED(pInst);

    if (!wCommand)
	{
		pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
	    SET_D8OUT(_P_LT, (_P_IN1 < _P_IN2));
	    SET_D8OUT(_P_EQ, (_P_IN1 == _P_IN2));
	    SET_D8OUT(_P_GT, (_P_IN1 > _P_IN2));
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_CMP;
    case T5FBCMD_SIZEOFINSTANCE :
        return 4L;
    default :
        return 0L;
    }
}

#undef _P_IN1
#undef _P_IN2
#undef _P_LT
#undef _P_EQ
#undef _P_GT

/****************************************************************************/
/* semaphore */

#define _P_CLAIM    GET_D8IN(0)
#define _P_RELEASE  GET_D8IN(1)
#define _P_BUSY     2

T5_DWORD T5stfb_SEMA (T5_WORD wCommand, T5PTR_DB pBase,
                      T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pMem, pLock;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pMem = (T5_PTBYTE)pInst;
		SET_D8OUT(_P_BUSY, *pMem);
		if (_P_CLAIM) *pMem = TRUE;
		else if (_P_RELEASE)
		{
			SET_D8OUT(_P_BUSY, FALSE);
			*pMem = FALSE;
		}
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_SEMA;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_BYTE);
    default :
        return 0L;
    }
}

#undef _P_CLAIM
#undef _P_RELEASE
#undef _P_BUSY

/****************************************************************************/
/* timers */

#define _P_IN   GET_D8IN(0)
#define _P_PT   GET_TIMEIN(1)
#define _P_Q    2
#define _P_ET   3

typedef struct 
{
   T5_DWORD tElapsed;
   T5_DWORD tLastCall;
   T5_BOOL  bTrigger;
   T5_BOOL  bFinished;
   T5_BOOL  bCounting;
   T5_BOOL  res1;
} _str_FBtimer;

T5_DWORD T5stfb_TON (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    _str_FBtimer *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBtimer *)pInst;
        if (!_P_IN)
        {
            pMem->tElapsed = 0L;
            pMem->bFinished = pMem->bTrigger = FALSE;
        }
        else
        {
            if (!pMem->bTrigger)
            {
                pMem->tLastCall = pStatus->dwTimStamp;
                pMem->bTrigger = TRUE;
            }
            if (!pMem->bFinished)
            {
                pMem->tElapsed += T5_BOUNDTIME(pStatus->dwTimStamp
                                                - pMem->tLastCall);
                pMem->tLastCall = pStatus->dwTimStamp;
                if (pMem->tElapsed >= _P_PT) 
                {
                    pMem->tElapsed = _P_PT;
                    pMem->bFinished = TRUE;
                }
            }
        }
        SET_D8OUT(_P_Q, pMem->bFinished);
        SET_TIMEOUT(_P_ET, pMem->tElapsed);
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_TON;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBtimer);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBtimer *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    default :
        return 0L;
    }
}

T5_DWORD T5stfb_TOF (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    _str_FBtimer *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBtimer *)pInst;
        if (_P_IN)
        {
            pMem->tElapsed = 0L;
            pMem->bFinished = pMem->bTrigger = TRUE;
        }
        else
        {
            if (pMem->bTrigger)
            {
                pMem->tLastCall = pStatus->dwTimStamp;
                pMem->bTrigger = FALSE;
            }
            if (pMem->bFinished)
            {
                pMem->tElapsed += T5_BOUNDTIME(pStatus->dwTimStamp
                                                - pMem->tLastCall);
                pMem->tLastCall = pStatus->dwTimStamp;
                if (pMem->tElapsed >= _P_PT) 
                {
                    pMem->tElapsed = _P_PT;
                    pMem->bFinished = FALSE;
                }
            }
        }
        SET_D8OUT(_P_Q, pMem->bFinished);
        SET_TIMEOUT(_P_ET, pMem->tElapsed);
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_TOF;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBtimer);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBtimer *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    default :
        return 0L;
    }
}

T5_DWORD T5stfb_TP (T5_WORD wCommand, T5PTR_DB pBase,
                    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    _str_FBtimer *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBtimer *)pInst;
        if (_P_IN && !pMem->bTrigger)
        {
            pMem->tLastCall = pStatus->dwTimStamp;
            pMem->bTrigger = pMem->bCounting = TRUE;
        }
        if (pMem->bCounting)
        {
            pMem->tElapsed += T5_BOUNDTIME(pStatus->dwTimStamp
                                                - pMem->tLastCall);
            pMem->tLastCall = pStatus->dwTimStamp;
            if (pMem->tElapsed >= _P_PT) 
            {
                pMem->tElapsed = _P_PT;
                pMem->bCounting = FALSE;
            }
        }
        if (!pMem->bCounting && !_P_IN)
        {
            pMem->tElapsed = 0L;
            pMem->bTrigger = FALSE;
        }
        SET_D8OUT(_P_Q, pMem->bCounting);
        SET_TIMEOUT(_P_ET, pMem->tElapsed);
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_TP;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBtimer);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBtimer *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_IN
#undef _P_PT
#undef _P_Q
#undef _P_ET

/****************************************************************************/
/* blink signal */

#define _P_RUN      GET_D8IN(0)
#define _P_CYCLE    GET_TIMEIN(1)
#define _P_Q        2

T5_DWORD T5stfb_BLINK (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    _str_FBtimer *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBtimer *)pInst;
        if (!_P_RUN)
        {
            pMem->tElapsed = 0L;
            pMem->bFinished = pMem->bTrigger = FALSE;
        }
        else
        {
            if (!pMem->bTrigger)
            {
                pMem->tLastCall = pStatus->dwTimStamp;
                pMem->bTrigger = TRUE;
            }
            pMem->tElapsed += T5_BOUNDTIME(pStatus->dwTimStamp
                                            - pMem->tLastCall);
            pMem->tLastCall = pStatus->dwTimStamp;
            if (pMem->tElapsed >= (_P_CYCLE / 2L)) 
            {
                pMem->tElapsed = 0L;
                pMem->bFinished = !pMem->bFinished;
            }
        }
        SET_D8OUT(_P_Q, pMem->bFinished);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_BLINK;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBtimer);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBtimer *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    case T5FBCMD_INITINSTANCE :
    default :
        return 0L;
    }
}

#undef _P_RUN
#undef _P_CYCLE
#undef _P_Q

/****************************************************************************/
/* stack of DINT */

#define _P_PUSH     GET_D8IN(0)
#define _P_POP      GET_D8IN(1)
#define _P_R1       GET_D8IN(2)
#define _P_IN       GET_DINTIN(3)
#define _P_N        GET_DINTIN(4)
#define _P_EMPTY    5
#define _P_OFLO     6
#define _P_OUT      7

#define _MAX_STACK  128

typedef struct
{
    T5_LONG aiStack[_MAX_STACK];
    T5_LONG iTop;
    T5_LONG iSize;
    T5_BOOL bPushTrigger;
    T5_BOOL bPopTrigger;
} _str_FBstackint;

T5_DWORD T5stfb_STACKINT (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint;
    _str_FBstackint *pMem;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pMem = (_str_FBstackint *)pInst;
        if (_P_R1)
        {
            if (_P_N < 0L) pMem->iSize = 0L;
            else if (_P_N > _MAX_STACK) pMem->iSize = _MAX_STACK;
            else pMem->iSize = _P_N;
            pMem->iTop = 0;
            SET_D8OUT(_P_OFLO, FALSE);
        }
        else if (_P_POP && !pMem->bPopTrigger)
        {
            if (pMem->iTop) pMem->iTop -= 1L;
            SET_D8OUT(_P_OFLO, FALSE);
        }
        else if (_P_PUSH && !pMem->bPushTrigger)
        {
            if (pMem->iTop >= pMem->iSize)
            {
                SET_D8OUT(_P_OFLO, TRUE);
            }
            else
            {
                pMem->aiStack[(pMem->iTop)++] = _P_IN;
                SET_D8OUT(_P_OFLO, FALSE);
            }
        }
        pMem->bPushTrigger = _P_PUSH;
        pMem->bPopTrigger = _P_POP;
        SET_D8OUT(_P_EMPTY, (pMem->iTop == 0L));
        if (pMem->iTop == 0L || pMem->iTop > pMem->iSize)
        {
            SET_DINTOUT(_P_OUT, 0L);
        }
        else
        {
            SET_DINTOUT(_P_OUT, pMem->aiStack[pMem->iTop-1L]);
        }
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_STACKINT;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBstackint);
    case T5FBCMD_INITINSTANCE :
        pMem = (_str_FBstackint *)pInst;
        pMem->iSize = _MAX_STACK;
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_PUSH
#undef _P_POP
#undef _P_R1
#undef _P_IN
#undef _P_N
#undef _P_EMPTY
#undef _P_OFLO
#undef _P_OUT
#undef _MAX_STACK

/****************************************************************************/
/* average */

#if 0//def T5DEF_REALSUPPORTED

#define _P_RUN      GET_D8IN(0)
#define _P_XIN      GET_REALIN(1)
#define _P_N        GET_DINTIN(2)
#define _P_XOUT     3

#define _MAX_STACK  128

typedef struct
{
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL rSum;
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL rSum;
#endif /*T5DEF_LREALSUPPORTED*/
    T5_REAL rResult;
    T5_REAL arPoint[_MAX_STACK];
    T5_LONG iNext;
    T5_LONG iSize;
    T5_LONG iNbPoint;
} _str_FBaverage;

T5_DWORD T5stfb_AVERAGE (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint;
    T5_PTREAL pReal;
    _str_FBaverage *pMem;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pReal = (T5_PTREAL)T5GET_DBDATA32(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pMem = (_str_FBaverage *)pInst;
        
        if (!_P_RUN)
        {
            if (_P_N < 0L) pMem->iSize = 0L;
            else if (_P_N > _MAX_STACK) pMem->iSize = _MAX_STACK;
            else pMem->iSize = _P_N;
            pMem->iNbPoint = 0L;
            pMem->rResult = T5_ZEROREAL;
            pMem->iNext = 0L;
#ifdef T5DEF_LREALSUPPORTED
            pMem->rSum = T5_ZEROLREAL;
#else /*T5DEF_LREALSUPPORTED*/
            pMem->rSum = T5_ZEROREAL;
#endif /*T5DEF_LREALSUPPORTED*/
        }
        else if (pMem->iSize == 0)
        {
            pMem->rResult = T5_ZEROREAL;
        }
        else
        {
            if (pMem->iNbPoint >= pMem->iSize)
            {
#ifdef T5DEF_LREALSUPPORTED
                pMem->rSum -= (T5_LREAL)(pMem->arPoint[pMem->iNext]);
#else /*T5DEF_LREALSUPPORTED*/
                pMem->rSum -= pMem->arPoint[pMem->iNext];
#endif /*T5DEF_LREALSUPPORTED*/
            }
            pMem->arPoint[pMem->iNext] = _P_XIN;

#ifdef T5DEF_LREALSUPPORTED
            pMem->rSum += (T5_LREAL)_P_XIN;
#else /*T5DEF_LREALSUPPORTED*/
            pMem->rSum += _P_XIN;
#endif /*T5DEF_LREALSUPPORTED*/

            pMem->iNext += 1L;
            if (pMem->iNbPoint < pMem->iSize)
                pMem->iNbPoint += 1L;
            if (pMem->iNext >= pMem->iSize)
                pMem->iNext = 0L;

#ifdef T5DEF_LREALSUPPORTED
            pMem->rResult = (T5_REAL)(pMem->rSum / (T5_LREAL)(pMem->iNbPoint));
#else /*T5DEF_LREALSUPPORTED*/
            pMem->rResult = pMem->rSum / (T5_REAL)(pMem->iNbPoint);
#endif /*T5DEF_LREALSUPPORTED*/
        }        
        SET_REALOUT(_P_XOUT, pMem->rResult);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_AVERAGE;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBaverage);
    case T5FBCMD_INITINSTANCE :
        pMem = (_str_FBaverage *)pInst;
        pMem->iSize = _MAX_STACK;
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_RUN
#undef _P_XIN
#undef _P_N
#undef _P_XOUT
#undef _MAX_STACK

#endif /*T5DEF_REALSUPPORTED*/

#define _P_XIN /*_P_DID*/   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_XOUT /*_P_Q*/   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[1]))

/* handler */
//leizhu add for lin test 180709
T5_DWORD T5stfb_AVERAGE (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      _P_XOUT=DstWMonitor_eApplication_Function.s32MLin_eGetStatus_Exe( _P_XIN);
      return 0L;

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}
#undef _P_XIN
#undef _P_XOUT
/* undefine argument list */
/****************************************************************************/
/* averageL */

#ifdef T5DEF_LREALSUPPORTED

#define _P_RUN      GET_D8IN(0)
#define _P_XIN      GET_LREALIN(1)
#define _P_N        GET_DINTIN(2)
#define _P_XOUT     3

#define _MAX_STACK  128

typedef struct
{
    T5_LREAL rSum;
    T5_LREAL rResult;
    T5_LREAL arPoint[_MAX_STACK];
    T5_LONG  iNext;
    T5_LONG  iSize;
    T5_LONG  iNbPoint;
} _str_FBaverageL;

T5_DWORD T5stfb_AVERAGEL (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint;
    T5_PTLREAL pLReal;
    _str_FBaverageL *pMem;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pLReal = (T5_PTLREAL)T5GET_DBDATA64(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pMem = (_str_FBaverageL *)pInst;
        
        if (!_P_RUN)
        {
            if (_P_N < 0L) pMem->iSize = 0L;
            else if (_P_N > _MAX_STACK) pMem->iSize = _MAX_STACK;
            else pMem->iSize = _P_N;
            pMem->iNbPoint = 0L;
            pMem->rResult = T5_ZEROLREAL;
            pMem->iNext = 0L;
            pMem->rSum = T5_ZEROLREAL;
        }
        else if (pMem->iSize == 0)
        {
            pMem->rResult = T5_ZEROLREAL;
        }
        else
        {
            if (pMem->iNbPoint >= pMem->iSize)
            {
                pMem->rSum -= pMem->arPoint[pMem->iNext];
            }
            pMem->arPoint[pMem->iNext] = _P_XIN;
            pMem->rSum += _P_XIN;
            pMem->iNext += 1L;
            if (pMem->iNbPoint < pMem->iSize)
                pMem->iNbPoint += 1L;
            if (pMem->iNext >= pMem->iSize)
                pMem->iNext = 0L;

            pMem->rResult = (pMem->rSum / (T5_LREAL)(pMem->iNbPoint));
        }        
        SET_LREALOUT(_P_XOUT, pMem->rResult);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_AVERAGEL;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBaverageL);
    case T5FBCMD_INITINSTANCE :
        pMem = (_str_FBaverageL *)pInst;
        pMem->iSize = _MAX_STACK;
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_RUN
#undef _P_XIN
#undef _P_N
#undef _P_XOUT
#undef _MAX_STACK

#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/
/* hysteresis */

#ifdef T5DEF_REALSUPPORTED

#define _P_XIN1     GET_REALIN(0)
#define _P_XIN2     GET_REALIN(1)
#define _P_EPS      GET_REALIN(2)
#define _P_Q        3

T5_DWORD T5stfb_HYSTER (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTREAL pReal;
    T5_PTBYTE pMem;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pReal = (T5_PTREAL)T5GET_DBDATA32(pBase);
        pMem = (T5_PTBYTE)pInst;
        if (*pMem && _P_XIN1 < (_P_XIN2 - _P_EPS))
            *pMem = FALSE;
        else if (!(*pMem) && _P_XIN1 > (_P_XIN2 + _P_EPS))
            *pMem = TRUE;
        SET_D8OUT(_P_Q, *pMem);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_HYSTER;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_BYTE);
    default :
        return 0L;
    }
}

#undef _P_XIN1
#undef _P_XIN2
#undef _P_EPS
#undef _P_Q

#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/
/* accumulated hysteresis */

#ifdef T5DEF_LREALSUPPORTED

#define _P_XIN      GET_REALIN(0)
#define _P_EPS      GET_REALIN(1)
#define _P_FACTOR   GET_REALIN(2)
#define _P_XOUT     3

typedef struct
{
    T5_LREAL lrAcc;
    T5_REAL  rLast;
    T5_DWORD tLast;
    T5_BOOL  bInit;
    T5_BOOL  res1;
    T5_BOOL  res2;
    T5_BOOL  res3;
} _str_FBlim_hysteracc;

T5_DWORD T5stfb_HYSTERACC (T5_WORD wCommand, T5PTR_DB pBase,
                           T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock;
    T5_PTREAL pReal;
    T5_REAL rOut, rFactor;
    T5_LREAL lrDiff;
    T5PTR_DBSTATUS pStatus;
    _str_FBlim_hysteracc *pMem;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pReal = (T5_PTREAL)T5GET_DBDATA32(pBase);
        pMem = (_str_FBlim_hysteracc *)pInst;
        if (!(pMem->bInit))
        {
            pMem->lrAcc = 0;
            pMem->rLast = _P_XIN;
            rOut = _P_XIN;
            pMem->bInit = TRUE;
        }
        else
        {
            lrDiff = fabs ((T5_LREAL)(_P_XIN - pMem->rLast)); 
            if (lrDiff > (T5_LREAL)_P_EPS)
            {
                pMem->lrAcc = 0;
                pMem->rLast = _P_XIN;
                rOut = _P_XIN;
            }
            else
            {
                pMem->lrAcc += ((lrDiff * (T5_LREAL)T5_BOUNDTIME(pStatus->dwTimStamp - pMem->tLast)) / 1000);
                rFactor = _P_FACTOR;
                if (rFactor < 2)
                    rFactor = 2;
                if (pMem->lrAcc > (T5_LREAL)(_P_EPS * _P_FACTOR))
                {
                    pMem->lrAcc = 0;
                    pMem->rLast = _P_XIN;
                    rOut = _P_XIN;
                }
                else
                    rOut = pMem->rLast;
            }
        }
        pMem->tLast = pStatus->dwTimStamp;
        SET_REALOUT(_P_XOUT, rOut);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_HYSTERACC;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBlim_hysteracc);
    default :
        return 0L;
    }
}

#undef _P_XIN
#undef _P_EPS
#undef _P_FACTOR
#undef _P_XOUT

#endif /*T5DEF_LREALSUPPORTED*/

/****************************************************************************/
/* alarm on limits */

#ifdef T5DEF_REALSUPPORTED

#define _P_H    GET_REALIN(0)
#define _P_X    GET_REALIN(1)
#define _P_L    GET_REALIN(2)
#define _P_EPS  GET_REALIN(3)
#define _P_QH   4
#define _P_Q    5
#define _P_QL   6

typedef struct
{
    T5_BYTE bHigh;
    T5_BYTE bLow;
} _str_FBlim_alrm;

T5_DWORD T5stfb_LIM_ALRM (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTREAL pReal;
    _str_FBlim_alrm *pMem;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pReal = (T5_PTREAL)T5GET_DBDATA32(pBase);
        pMem = (_str_FBlim_alrm *)pInst;
        if (pMem->bHigh && _P_X < (_P_H - _P_EPS))
            pMem->bHigh = FALSE;
        else if (!(pMem->bHigh) && _P_X > _P_H)
            pMem->bHigh = TRUE;
        if (pMem->bLow && _P_X > (_P_L + _P_EPS))
            pMem->bLow = FALSE;
        else if (!(pMem->bLow) && _P_X < _P_L)
            pMem->bLow = TRUE;
        SET_D8OUT(_P_QH, pMem->bHigh);
        SET_D8OUT(_P_Q, (pMem->bHigh || pMem->bLow));
        SET_D8OUT(_P_QL, pMem->bLow);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_LIM_ALRM;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBlim_alrm);
    default :
        return 0L;
    }
}

#undef _P_H
#undef _P_X
#undef _P_L
#undef _P_EPS
#undef _P_QH
#undef _P_Q
#undef _P_QL

#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/
/* integral */

#ifdef T5DEF_REALSUPPORTED

#define _P_RUN      GET_D8IN(0)
#define _P_R1       GET_D8IN(1)
#define _P_XIN      GET_REALIN(2)
#define _P_X0       GET_REALIN(3)
#define _P_CYCLE    GET_TIMEIN(4)
#define _P_Q        5
#define _P_XOUT     6

typedef struct
{
    T5_DWORD tLastCall;
    T5_REAL  rSum;
} _str_FBintegral;

T5_DWORD T5stfb_INTEGRAL (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    T5_DWORD dwTime;
    T5_PTREAL pReal;
    _str_FBintegral *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pReal = (T5_PTREAL)T5GET_DBDATA32(pBase);
        pMem = (_str_FBintegral *)pInst;
        if (pMem->tLastCall == 0)
            pMem->tLastCall = pStatus->dwTimStamp;

        if (_P_R1)
        {
            pMem->rSum = _P_X0;
            pMem->tLastCall = pStatus->dwTimStamp;
        }
        else if (_P_RUN)
        {
            dwTime = T5_BOUNDTIME(pStatus->dwTimStamp - pMem->tLastCall);
            if (dwTime > _P_CYCLE)
            {
                pMem->tLastCall = pStatus->dwTimStamp;
                pMem->rSum += (_P_XIN * (T5_REAL)dwTime);
            }
        }
        else
            pMem->tLastCall = pStatus->dwTimStamp;

        SET_D8OUT(_P_Q, !_P_R1);
        SET_REALOUT(_P_XOUT, pMem->rSum);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_INTEGRAL;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBintegral);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBintegral *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_RUN
#undef _P_R1
#undef _P_XIN
#undef _P_X0
#undef _P_CYCLE
#undef _P_Q
#undef _P_XOUT

#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/
/* derivate */

#ifdef T5DEF_REALSUPPORTED

#define _P_RUN      GET_D8IN(0)
#define _P_XIN      GET_REALIN(1)
#define _P_CYCLE    GET_TIMEIN(2)
#define _P_XOUT     3

typedef struct
{
    T5_DWORD tLastCall;
    T5_REAL  rSample[3];
    T5_REAL  rResult;
} _str_FBderivate;

T5_DWORD T5stfb_DERIVATE (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    T5_DWORD dwTime;
    T5_PTREAL pReal;
    _str_FBderivate *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pReal = (T5_PTREAL)T5GET_DBDATA32(pBase);
        pMem = (_str_FBderivate *)pInst;
        if (pMem->tLastCall == 0xffffffffL)
        {
            pMem->rSample[0] = pMem->rSample[1] = pMem->rSample[2] = _P_XIN;
            pMem->tLastCall = pStatus->dwTimStamp;
        }
        if (_P_RUN)
        {
            dwTime = T5_BOUNDTIME(pStatus->dwTimStamp - pMem->tLastCall);
            if (dwTime != 0 && dwTime > _P_CYCLE)
            {
                pMem->rResult = ((_P_XIN - pMem->rSample[2]) * (T5_REAL)3
                                 + pMem->rSample[0] - pMem->rSample[1])
                                 / ((T5_REAL)dwTime * 10);
                pMem->tLastCall = pStatus->dwTimStamp;
                pMem->rSample[2] = pMem->rSample[1];
                pMem->rSample[1] = pMem->rSample[0];
                pMem->rSample[0] = _P_XIN;
            }
        } 
        SET_REALOUT(_P_XOUT, pMem->rResult);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_DERIVATE;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBderivate);
    case T5FBCMD_INITINSTANCE :
        pMem = (_str_FBderivate *)pInst;
        pMem->tLastCall = 0xffffffffL;
        return 0L;
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBderivate *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_RUN
#undef _P_XIN
#undef _P_CYCLE
#undef _P_XOUT

#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/
/* signals */

#ifdef T5DEF_REALSUPPORTED

#define _P_RUN      GET_D8IN(0)
#define _P_PERIOD   GET_TIMEIN(1)
#define _P_MAX      GET_DINTIN(2)
#define _P_PULSE    3
#define _P_UP       4
#define _P_END      5
#define _P_SIN      6

#define _ANGLE(d,m)     ((T5_REAL)(d) / (T5_REAL)(m) * (T5_REAL)6.2826)
#define _SAFEANGLE(d,m) ((m) ? _ANGLE(d,m) : T5_ZEROREAL)

typedef struct
{
    T5_DWORD tLastCall;
    T5_DWORD tElapsed;
    T5_LONG  dwCounter;
    T5_BOOL  bPhase2;
    T5_BOOL  bTrigger;
} _str_FBsiggen;

T5_DWORD T5stfb_SIG_GEN (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint;
    T5_PTDWORD pTime;
    T5_PTREAL pReal;
    _str_FBsiggen *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pTime = T5GET_DBTIME(pBase);
        pReal = (T5_PTREAL)T5GET_DBDATA32(pBase);
        pMem = (_str_FBsiggen *)pInst;

        if (!_P_RUN)
        {
            pMem->tElapsed = 0L;
            pMem->dwCounter = 0L;
            pMem->bPhase2 = pMem->bTrigger = FALSE;      
        }
        else
        {
            if (!(pMem->bTrigger))
            {
                pMem->tLastCall = pStatus->dwTimStamp;
                pMem->bTrigger = TRUE;
            }
            pMem->tElapsed += T5_BOUNDTIME(pStatus->dwTimStamp
                                           - pMem->tLastCall);
            pMem->tLastCall = pStatus->dwTimStamp;
            if (pMem->tElapsed >= _P_PERIOD)
            {
                pMem->tElapsed = 0L;
                pMem->bPhase2 = !pMem->bPhase2;
                if (pMem->dwCounter < _P_MAX) pMem->dwCounter += 1L;
                else pMem->dwCounter = 0L;
            }
        }
        SET_D8OUT(_P_PULSE, pMem->bPhase2);
        SET_DINTOUT(_P_UP, pMem->dwCounter);
        SET_D8OUT(_P_END, (pMem->dwCounter == 0));
#ifdef T5DEF_TRIGOSUPPORTED
        SET_REALOUT(_P_SIN, (T5_REAL)sin (_SAFEANGLE(pMem->dwCounter,_P_MAX)));
#else /*T5DEF_TRIGOSUPPORTED*/
        SET_REALOUT(_P_SIN, T5_ZEROREAL);
#endif /*T5DEF_TRIGOSUPPORTED*/
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_SIG_GEN;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBsiggen);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBsiggen *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_RUN
#undef _P_PERIOD
#undef _P_MAX
#undef _P_PULSE
#undef _P_UP
#undef _P_END
#undef _P_SIN

#undef _ANGLE
#undef _SAFEANGLE

#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/
/* Serial In/Out */

#define _P_RUN      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_SND      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_CONF     (*(T5GET_DBSTRING(pBase)+pArgs[2]))
#define _P_DATASND  (*(T5GET_DBSTRING(pBase)+pArgs[3]))
#define _P_OPEN     (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[4]))
#define _P_RCV      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[5]))
#define _P_ERR      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[6]))
#define _P_DATARCV  (*(T5GET_DBSTRING(pBase)+pArgs[7]))

T5_DWORD T5stfb_SERIO (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
#ifdef T5DEF_SERIAL

    T5_PTSERIAL pS;
    T5_UNUSED(pClass);

    pS = (T5_PTSERIAL)pInst;
    if (!wCommand)
    {
        if (pS == NULL)
            return 0L;

        /* open / close */
        if (_P_RUN && !T5Serial_IsValid (pS))
        {
#ifdef T5DEF_SERIALINFO
            T5Serial_Info (pS, T5SERINFO_P_SERIO);
#endif /*T5DEF_SERIALINFO*/
            T5Serial_Open (pS, (T5_PTCHAR)(_P_CONF + 2));
#ifdef T5DEF_SERIALINFO
            if (T5Serial_IsValid (pS))
                T5Serial_Info (pS, T5SERINFO_SERIO);
#endif /*T5DEF_SERIALINFO*/

        }
        else if (!_P_RUN && T5Serial_IsValid (pS))
        {
            T5Serial_Close (pS);
        }
        _P_OPEN = T5Serial_IsValid (pS);
        _P_ERR = 0;
        _P_RCV = 0;
        _P_DATARCV[1] = 0;
        _P_DATARCV[2] = 0;
        if (T5Serial_IsValid (pS))
        {
            /* receive */
            _P_DATARCV[1] = (T5_BYTE)T5Serial_Receive (pS,
                                     (T5_WORD)(_P_DATARCV[0]), _P_DATARCV+2);
            if (_P_DATARCV[1])
            {
                _P_DATARCV[2+_P_DATARCV[1]] = 0;
                _P_RCV = 1;
            }
            /* send */
            if (_P_SND && _P_DATASND[1] != 0)
            {
                if (!T5Serial_Send (pS, (T5_WORD)(_P_DATASND[1]),
                                    _P_DATASND+2))
                    _P_ERR = 1;
            }
        }
        return 0L;
    }
    
    switch (wCommand)
    {
    case T5FBCMD_INITINSTANCE :
        T5Serial_Initialize (pS);
        return 0L;
    case T5FBCMD_EXITINSTANCE :
        if (T5Serial_IsValid (pS))
        {
            T5Serial_Close (pS);
        }
        return 0L;
    case T5FBCMD_HOTRESTART :
        return 0L;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_SERIAL);
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_SERIO;
    default :
        return 0L;
    }

#else /*T5DEF_SERIAL*/
    if (!wCommand)
    {
        _P_OPEN = 0;
        _P_ERR = 1;
        _P_RCV = 0;
        _P_DATARCV[1] = 0;
        _P_DATARCV[2] = 0;
        return 0L;
    }
    
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_SERIO;
    default :
        return 0L;
    }
#endif /*T5DEF_SERIAL*/
}

/* Undefine argument list */

#undef _P_RUN
#undef _P_SND
#undef _P_CONF
#undef _P_DATASND
#undef _P_OPEN
#undef _P_RCV
#undef _P_ERR
#undef _P_DATARCV

/****************************************************************************/
/* SERIO_B - Function block */

#define _P_RUN          (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_CONF         (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_DATASND      ((T5_PTBYTE)T5Tools_GetD8Array(pBase,pArgs[2]))
#define _P_DATASND_SIZE pArgs[3]
#define _P_IXSND        (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[4]))
#define _P_MAXSND       (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[5]))
#define _P_DATARCV      ((T5_PTBYTE)T5Tools_GetD8Array(pBase,pArgs[6]))
#define _P_DATARCV_SIZE pArgs[7]
#define _P_IXRCV        (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[8]))
#define _P_MAXRCV       (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[9]))
#define _P_OPEN         (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[10]))
#define _P_NSND         (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[11]))
#define _P_NRCV         (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[12]))

T5_DWORD T5stfb_SERIO_B (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass,
                         T5_PTR pInst, T5_PTWORD pArgs)
{
#ifdef T5DEF_SERIAL
    T5_PTSERIAL pS;
    pS = (T5_PTSERIAL)pInst;
#endif /*T5DEF_SERIAL*/
    T5_UNUSED(pClass);

    switch (wCommand)
    {
    case T5FBCMD_ACTIVATE :
#ifdef T5DEF_SERIAL
        /* open / close */
        if (_P_RUN && !T5Serial_IsValid (pS))
        {
#ifdef T5DEF_SERIALINFO
            T5Serial_Info (pS, T5SERINFO_P_SERIO);
#endif /*T5DEF_SERIALINFO*/
            T5Serial_Open (pS, (T5_PTCHAR)(_P_CONF + 2));
#ifdef T5DEF_SERIALINFO
            if (T5Serial_IsValid (pS))
                T5Serial_Info (pS, T5SERINFO_SERIO);
#endif /*T5DEF_SERIALINFO*/

        }
        else if (!_P_RUN && T5Serial_IsValid (pS))
        {
            T5Serial_Close (pS);
        }
        _P_OPEN = T5Serial_IsValid (pS);
        _P_NSND = 0;
        _P_NRCV = 0;
        if (T5Serial_IsValid (pS))
        {
            if (_P_MAXSND != 0 && _P_IXSND < (T5_DWORD)_P_DATASND_SIZE
                && (_P_IXSND + _P_MAXSND) <= (T5_DWORD)_P_DATASND_SIZE)
            {
                _P_NSND = (T5_DWORD)T5Serial_Send (pS, (T5_WORD)_P_MAXSND, _P_DATASND+_P_IXSND);
            }
            if (_P_MAXRCV != 0 && _P_IXRCV < (T5_DWORD)_P_DATARCV_SIZE
                && (_P_IXRCV + _P_MAXRCV) <= (T5_DWORD)_P_DATARCV_SIZE)
            {
                _P_NRCV = (T5_DWORD)T5Serial_Receive (pS, (T5_WORD)_P_MAXRCV, _P_DATARCV+_P_IXRCV);
            }
        }
#else /*T5DEF_SERIAL*/
        _P_OPEN = FALSE;
        _P_NSND = 0;
        _P_NRCV = 0;
#endif /*T5DEF_SERIAL*/
        return 0L;
    case T5FBCMD_INITINSTANCE :
#ifdef T5DEF_SERIAL
        T5Serial_Initialize (pS);
#endif /*T5DEF_SERIAL*/
        return 0L;
    case T5FBCMD_EXITINSTANCE :
#ifdef T5DEF_SERIAL
        if (T5Serial_IsValid (pS))
            T5Serial_Close (pS);
#endif /*T5DEF_SERIAL*/
        return 0L;
    case T5FBCMD_SIZEOFINSTANCE :
#ifdef T5DEF_SERIAL
        return (T5_DWORD)sizeof(T5_SERIAL);
#endif /*T5DEF_SERIAL*/
    case T5FBCMD_HOTRESTART :
        return 0L;
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_SERIO_B;
    case T5FBCMD_ACCEPTCT :
        return 1L;
    default :
        return 0L;
    }
}

#undef _P_RUN
#undef _P_CONF
#undef _P_DATASND
#undef _P_DATASND_SIZE
#undef _P_IXSND
#undef _P_MAXSND
#undef _P_DATARCV
#undef _P_DATARCV_SIZE
#undef _P_IXRCV
#undef _P_MAXRCV
#undef _P_OPEN
#undef _P_NSND
#undef _P_NRCV

/****************************************************************************/
/* UNPACK8 */

T5_DWORD T5stfb_UNPACK8 (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_BYTE i, b, mask;
    T5_PTBYTE pData8;
    T5_UNUSED(pClass);
    T5_UNUSED(pInst);

    switch (wCommand)
    {
    case T5FBCMD_ACTIVATE :
        pData8 = T5GET_DBDATA8(pBase);
        b = pData8[pArgs[0]]; /* input */
        mask = 0x01;
        for (i=1; i<=8; i++)
        {
            if (b & mask)
                pData8[pArgs[i]] = TRUE;
            else
                pData8[pArgs[i]] = FALSE;
            mask <<= 1;
        }
        return 0L;
    case T5FBCMD_SIZEOFINSTANCE :
        return 0L;
    case T5FBCMD_ACCEPTCT :
        return 1L;
    case T5FBCMD_GETCLASSID :
        return T5FBL_UNPACK8;
    default :
        return 0L;
    }
}

/****************************************************************************/
/* timers with RESET */

#define _P_IN   GET_D8IN(0)
#define _P_PT   GET_TIMEIN(1)
#define _P_RST  GET_D8IN(2)
#define _P_Q    3
#define _P_ET   4

T5_DWORD T5stfb_TOFR (T5_WORD wCommand, T5PTR_DB pBase,
                      T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    _str_FBtimer *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBtimer *)pInst;
        if (_P_RST)
        {
            T5_MEMSET (pMem, 0, sizeof (_str_FBtimer));
        }
        else if (_P_IN)
        {
            pMem->tElapsed = 0L;
            pMem->bFinished = pMem->bTrigger = TRUE;
        }
        else
        {
            if (pMem->bTrigger)
            {
                pMem->tLastCall = pStatus->dwTimStamp;
                pMem->bTrigger = FALSE;
            }
            if (pMem->bFinished)
            {
                pMem->tElapsed += T5_BOUNDTIME(pStatus->dwTimStamp
                                                - pMem->tLastCall);
                pMem->tLastCall = pStatus->dwTimStamp;
                if (pMem->tElapsed >= _P_PT) 
                {
                    pMem->tElapsed = _P_PT;
                    pMem->bFinished = FALSE;
                }
            }
        }
        SET_D8OUT(_P_Q, pMem->bFinished);
        SET_TIMEOUT(_P_ET, pMem->tElapsed);
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBtimer);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBtimer *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    case T5FBCMD_GETCLASSID :
        return T5FBL_TOFR;
    default :
        return 0L;
    }
}

T5_DWORD T5stfb_TPR (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    _str_FBtimer *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBtimer *)pInst;
        if (_P_RST)
        {
            T5_MEMSET (pMem, 0, sizeof (_str_FBtimer));
        }
        else
        {
            if (_P_IN && !pMem->bTrigger)
            {
                pMem->tLastCall = pStatus->dwTimStamp;
                pMem->bTrigger = pMem->bCounting = TRUE;
            }
            if (pMem->bCounting)
            {
                pMem->tElapsed += T5_BOUNDTIME(pStatus->dwTimStamp
                                                    - pMem->tLastCall);
                pMem->tLastCall = pStatus->dwTimStamp;
                if (pMem->tElapsed >= _P_PT) 
                {
                    pMem->tElapsed = _P_PT;
                    pMem->bCounting = FALSE;
                }
            }
            if (!pMem->bCounting && !_P_IN)
            {
                pMem->tElapsed = 0L;
                pMem->bTrigger = FALSE;
            }
        }
        SET_D8OUT(_P_Q, pMem->bCounting);
        SET_TIMEOUT(_P_ET, pMem->tElapsed);
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBtimer);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBtimer *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    case T5FBCMD_GETCLASSID :
        return T5FBL_TPR;
    default :
        return 0L;
    }
}

#undef _P_IN
#undef _P_PT
#undef _P_RST
#undef _P_Q
#undef _P_ET

/****************************************************************************/
/* pulse signal */

#define _P_RUN      GET_D8IN(0)
#define _P_CYCLE    GET_TIMEIN(1)
#define _P_Q        2

T5_DWORD T5stfb_PLS (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    _str_FBtimer *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBtimer *)pInst;
        if (!_P_RUN)
        {
            pMem->tElapsed = 0L;
            pMem->bFinished = pMem->bTrigger = FALSE;
        }
        else if (pMem->bFinished)
        {
            pMem->bFinished = FALSE;
        }
        else
        {
            if (!pMem->bTrigger)
            {
                pMem->tLastCall = pStatus->dwTimStamp;
                pMem->bTrigger = TRUE;
            }
            pMem->tElapsed += T5_BOUNDTIME(pStatus->dwTimStamp
                                            - pMem->tLastCall);
            pMem->tLastCall = pStatus->dwTimStamp;
            if (pMem->tElapsed >= _P_CYCLE) 
            {
                pMem->tElapsed = 0L;
                pMem->bFinished = TRUE;
            }
        }
        SET_D8OUT(_P_Q, pMem->bFinished);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBtimer);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBtimer *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    case T5FBCMD_GETCLASSID :
        return T5FBL_PLS;
    default :
        return 0L;
    }
}

#undef _P_RUN
#undef _P_CYCLE
#undef _P_Q

/****************************************************************************/
/* flipflop */

#define _P_IN   GET_D8IN(0)
#define _P_RST  GET_D8IN(1)
#define _P_Q    2

typedef struct
{
    T5_BYTE bPrev;
    T5_BYTE bOut;
} _str_FBflipflop;

T5_DWORD T5stfb_FLIPFLOP (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    _str_FBflipflop *pMem;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pMem = (_str_FBflipflop *)pInst;
		pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        if (_P_RST)
        {
            pMem->bOut = FALSE;
            pMem->bPrev = FALSE;
        }
        else
        {
            if (_P_IN && !(pMem->bPrev))
            {
                pMem->bOut = !(pMem->bOut);
            }
            pMem->bPrev = _P_IN;
        }
        SET_D8OUT(_P_Q, pMem->bOut);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBflipflop);
    case T5FBCMD_GETCLASSID :
        return T5FBL_FLIPFLOP;
	default :
        return 0L;
    }
}

#undef _P_IN
#undef _P_RST
#undef _P_Q

/****************************************************************************/
/* RAMP */

#ifdef T5DEF_REALSUPPORTED

#define _P_IN   GET_REALIN(0)
#define _P_ASC  GET_REALIN(1)
#define _P_DSC  GET_REALIN(2)
#define _P_TM   GET_TIMEIN(3)
#define _P_RST  GET_D8IN(4)
#define _P_OUT  5

typedef struct
{
    T5_DWORD tLastCall;
    T5_REAL  rLast;
    T5_REAL  rUp;
    T5_REAL  rDn;
    T5_REAL  rTM;
} _str_FBramp;

T5_DWORD T5stfb_RAMP (T5_WORD wCommand, T5PTR_DB pBase,
                      T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    _str_FBramp *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    T5_PTREAL pReal;
    T5_DWORD tElapsed;
    T5_REAL rElapsed, rLim;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pMem = (_str_FBramp *)pInst;
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pReal = (T5_PTREAL)T5GET_DBDATA32(pBase);
        pTime = T5GET_DBTIME(pBase);
        tElapsed = T5_BOUNDTIME(pStatus->dwTimStamp - pMem->tLastCall);
        rElapsed = (T5_REAL)tElapsed;

        if (_P_RST || pMem->tLastCall == 0L)
        {
            pMem->rUp = _P_ASC;
            pMem->rDn = _P_DSC;
            pMem->rLast = _P_IN;
            pMem->rTM = (T5_REAL)(_P_TM);
        }
        else if (pMem->rTM < 0.0)
        {
            pMem->rLast = _P_IN;
        }
        else if (_P_IN > pMem->rLast && tElapsed > 0L && pMem->rUp > 0.0)
        {
            rLim = pMem->rLast + (pMem->rUp * rElapsed) / pMem->rTM;
            if (rLim > _P_IN)
                pMem->rLast = _P_IN;
            else
                pMem->rLast = rLim;
        }
        else if (_P_IN < pMem->rLast && tElapsed > 0L && pMem->rDn > 0.0)
        {
            rLim = pMem->rLast - (pMem->rDn * rElapsed) / pMem->rTM;
            if (rLim < _P_IN)
                pMem->rLast = _P_IN;
            else
                pMem->rLast = rLim;
        }

        pMem->tLastCall = pStatus->dwTimStamp;
        SET_REALOUT (_P_OUT, pMem->rLast);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBramp);
    case T5FBCMD_GETCLASSID :
        return T5FBL_RAMP;
	default :
        return 0L;
    }
}

#undef _P_IN
#undef _P_ASC
#undef _P_DSC
#undef _P_TM
#undef _P_RST
#undef _P_OUT

#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/
/* asymetric blink signal */

#define _P_RUN      GET_D8IN(0)
#define _P_TM0      GET_TIMEIN(1)
#define _P_TM1      GET_TIMEIN(2)
#define _P_Q        3

T5_DWORD T5stfb_BLINKA (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    _str_FBtimer *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBtimer *)pInst;
        if (!_P_RUN)
        {
            pMem->tElapsed = 0L;
            pMem->bFinished = pMem->bTrigger = FALSE;
        }
        else
        {
            if (!pMem->bTrigger)
            {
                pMem->tLastCall = pStatus->dwTimStamp;
                pMem->bTrigger = TRUE;
            }
            pMem->tElapsed += T5_BOUNDTIME(pStatus->dwTimStamp
                                            - pMem->tLastCall);
            pMem->tLastCall = pStatus->dwTimStamp;
            if (!pMem->bFinished && pMem->tElapsed >= _P_TM0) 
            {
                pMem->tElapsed = 0L;
                pMem->bFinished = TRUE;
            }
            else if (pMem->bFinished && pMem->tElapsed >= _P_TM1) 
            {
                pMem->tElapsed = 0L;
                pMem->bFinished = FALSE;
            }
        }
        SET_D8OUT(_P_Q, pMem->bFinished);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBtimer);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBtimer *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    case T5FBCMD_GETCLASSID :
        return T5FBL_BLINKA;
    default :
        return 0L;
    }
}

#undef _P_RUN
#undef _P_CYCLE
#undef _P_Q

/****************************************************************************/
/* AT */

#ifdef T5DEF_RTCLOCKSUPPORTED
#ifdef T5DEF_RTCLOCKSTAMP

#define _P_YEAR     GET_DINTIN(0)
#define _P_MONTH    GET_DINTIN(1)
#define _P_DAY      GET_DINTIN(2)
#define _P_TMOFDAY  GET_TIMEIN(3)
#define _P_RST      GET_D8IN(4)
#define _P_AT       5
#define _P_PAST     6

typedef struct
{
    T5_DWORD dwStamp;
    T5_DWORD dwTime;
    T5_BOOL  bAt;
    T5_BOOL  bPast;
    T5_BOOL  bInitDone;
    T5_BOOL  res1;
} _str_FBdtat;

T5_DWORD T5stfb_DTAT (T5_WORD wCommand, T5PTR_DB pBase,
                      T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    _str_FBdtat *pMem;
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime, pDint;
    T5_DWORD dwDate;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pMem = (_str_FBdtat *)pInst;
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTDWORD)T5GET_DBDATA32(pBase);
        pTime = T5GET_DBTIME(pBase);
        if (!pMem->bInitDone || _P_RST)
        {
            pMem->dwStamp = ((_P_YEAR << 16) & 0xffff0000L)
                          + ((_P_MONTH << 8) & 0x0000ff00L)
                          + (_P_DAY & 0x000000FFL);
            pMem->dwTime = _P_TMOFDAY;
            pMem->bInitDone = TRUE;
            pMem->bAt = FALSE;
            pMem->bPast = FALSE;
        }
        else if (pMem->bPast)
            pMem->bAt = FALSE;
        else
        {
            dwDate = T5RtClk_GetCurDateStamp ();
            if ((pMem->dwStamp & 0xffff0000L) == 0L)
            {
                dwDate &= 0x0000ffffL;
                if ((pMem->dwStamp & 0x0000ff00L) == 0L)
                {
                    dwDate &= 0xffff00ffL;
                    if ((pMem->dwStamp & 0x000000ffL) == 0L)
                        dwDate &= 0xffffff00L;
                }
            }
            if (pMem->dwStamp <= dwDate
                && pMem->dwTime <= T5RtClk_GetCurTimeStamp ())
            {
                pMem->bAt = TRUE;
                pMem->bPast = TRUE;
            }
        }
        SET_D8OUT(_P_AT, pMem->bAt);
        SET_D8OUT(_P_PAST, pMem->bPast);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBdtat);
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_DTAT;
	default :
        return 0L;
    }
}

#undef _P_YEAR
#undef _P_MONTH
#undef _P_DAY
#undef _P_TMOFDAY
#undef _P_RST
#undef _P_AT
#undef _P_PAST

#endif /*T5DEF_RTCLOCKSTAMP*/
#endif /*T5DEF_RTCLOCKSUPPORTED*/

/****************************************************************************/
/* EVERY */

#define _P_RUN      GET_D8IN(0)
#define _P_DAYS     GET_DINTIN(1)
#define _P_TM       GET_TIMEIN(2)
#define _P_Q        3

typedef struct 
{
   T5_DWORD dwDays;
   T5_DWORD tElapsed;
   T5_DWORD tLastCall;
   T5_BOOL  bTrigger;
   T5_BOOL  res1;
   T5_BOOL  res2;
   T5_BOOL  res3;
} _str_FBdtevery;

T5_DWORD T5stfb_DTEVERY (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime, pDint;
    _str_FBdtevery *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{

        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pDint = (T5_PTDWORD)T5GET_DBDATA32(pBase);
        pMem = (_str_FBdtevery *)pInst;
        /* inactive */
        if (!_P_RUN)
        {
            pMem->tLastCall = 0L;
            pMem->tElapsed = 0L;
            pMem->dwDays = 0L;
            pMem->bTrigger = FALSE;
        }
        /* first active scan */
        else if (pMem->tLastCall == 0L)
        {
            pMem->tLastCall = pStatus->dwTimStamp;
            pMem->bTrigger = TRUE;
        }
        /* pulse scan */
        else if (pMem->bTrigger)
        {
            pMem->bTrigger = FALSE;
        }
        /* counting time */
        else
        {
            pMem->tElapsed += T5_BOUNDTIME(pStatus->dwTimStamp
                                           - pMem->tLastCall);
            while (pMem->tElapsed > T5_24HMS)
            {
                pMem->dwDays += 1L;
                pMem->tElapsed -= T5_24HMS;
            }
            if (pMem->dwDays >= _P_DAYS && pMem->tElapsed >= _P_TM)
            {
                pMem->tElapsed = 0L;
                pMem->dwDays = 0L;
                pMem->bTrigger = TRUE;
            }
            pMem->tLastCall = pStatus->dwTimStamp;
        }
        SET_D8OUT(_P_Q, pMem->bTrigger);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBdtevery);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBdtevery *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_DTEVERY;
    default :
        return 0L;
    }
}

#undef _P_RUN
#undef _P_DAYS
#undef _P_TM
#undef _P_Q

/****************************************************************************/
/* FIFO */

#define _P_ANYTYPE  pArgs[0]
#define _P_PUSH     (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_POP      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))
#define _P_RST      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))
#define _P_IN       pArgs[4] /*index*/
#define _P_OUT      pArgs[5] /*index*/
#define _P_BUF      ((T5_PTBYTE)(T5Tools_GetAnyArray (pBase, pArgs[0], pArgs[6])))
#define _P_BUF_SIZE pArgs[7]
#define _P_EMPTY    (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[8]))
#define _P_OFLO     (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[9]))
#define _P_COUNT    (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[10]))
#define _P_PREAD    (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[11]))
#define _P_PWRITE   (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[12]))

typedef struct 
{
   T5_DWORD  pRead;
   T5_DWORD  pWrite;
   T5_DWORD  dwCount;
   T5_DWORD  dwSize;
   T5_PTBYTE pBuf;
   T5_BOOL   bPushPrev;
   T5_BOOL   bPopPrev;
   T5_BOOL   res1;
   T5_BOOL   res2;
} _str_FBfifo;

T5_DWORD T5stfb_FIFO (T5_WORD wCommand, T5PTR_DB pBase,
                      T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    _str_FBfifo *pData;
    T5_PTR pIn, pOut;
    T5_WORD wSizeof;
    T5_UNUSED(pClass);

    switch (wCommand)
    {
    case T5FBCMD_ACTIVATE :
        _P_OFLO = FALSE;
        pData = (_str_FBfifo *)pInst;
        pIn = T5Tools_GetAnyParam (pBase, _P_ANYTYPE, _P_IN, NULL, &wSizeof);
        pOut = T5Tools_GetAnyParam (pBase, _P_ANYTYPE, _P_OUT, NULL, NULL);
        /* exceptions */
        if (_P_ANYTYPE == T5C_STRING)
        {
            ((T5_PTBYTE)pOut)[1] = ((T5_PTBYTE)pOut)[2] = '\0';
            _P_EMPTY = TRUE;
            _P_COUNT = _P_PREAD = _P_PWRITE = 0L;
            return 0L;
        }
        if (pData->pBuf == NULL)
        {
            pData->pBuf = _P_BUF;
            pData->dwSize = (T5_DWORD)_P_BUF_SIZE;
        }
        else if (pData->pBuf != _P_BUF) /* cannot change the array! */
        {
            _P_EMPTY = TRUE;
            _P_COUNT = _P_PREAD = _P_PWRITE = 0L;
            return 0L;
        }
        /* normal mode */
        if (_P_RST)
        {
            pData->pRead = pData->pWrite = 0L;
            pData->dwCount = 0L;
        }
        else if (_P_POP && !pData->bPopPrev && pData->dwCount > 0L)
        {
            if (pData->pRead >= (pData->dwSize - 1L))
                pData->pRead = 0;
            else
                pData->pRead += 1L;
            pData->dwCount -= 1L;
        }
        else if (_P_PUSH && !pData->bPushPrev)
        {
            if (pData->dwCount == pData->dwSize)
                _P_OFLO = TRUE;
            else
            {
                T5_MEMCPY (pData->pBuf + (pData->pWrite * wSizeof),
                           pIn, wSizeof);
                if (pData->pWrite >= (pData->dwSize - 1L))
                    pData->pWrite = 0;
                else
                    pData->pWrite += 1L;
                pData->dwCount += 1L;
            }
        }
        /* triggers */
        pData->bPushPrev = _P_PUSH;
        pData->bPopPrev = _P_POP;
        /* outputs */
        if (pData->dwCount == 0L)
            T5_MEMSET (pOut, 0, wSizeof);
        else
            T5_MEMCPY (pOut, pData->pBuf + (pData->pRead * wSizeof), wSizeof);
        _P_EMPTY = (pData->dwCount == 0L);
        _P_COUNT = pData->dwCount;
        _P_PREAD = pData->pRead;
        _P_PWRITE = pData->pWrite;
        return 0L;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBfifo);
    case T5FBCMD_ACCEPTCT :
        return 1L;
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_FIFO;
    default :
        return 0L;
    }
}

T5_DWORD T5stfb_LIFO (T5_WORD wCommand, T5PTR_DB pBase,
                      T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    _str_FBfifo *pData;
    T5_PTR pIn, pOut;
    T5_WORD wSizeof;
    T5_UNUSED(pClass);

    switch (wCommand)
    {
    case T5FBCMD_ACTIVATE :
        _P_OFLO = FALSE;
        pData = (_str_FBfifo *)pInst;
        pIn = T5Tools_GetAnyParam (pBase, _P_ANYTYPE, _P_IN, NULL, &wSizeof);
        pOut = T5Tools_GetAnyParam (pBase, _P_ANYTYPE, _P_OUT, NULL, NULL);
        /* exceptions */
        if (_P_ANYTYPE == T5C_STRING)
        {
            ((T5_PTBYTE)pOut)[1] = ((T5_PTBYTE)pOut)[2] = '\0';
            _P_EMPTY = TRUE;
            _P_COUNT = _P_PREAD = _P_PWRITE = 0L;
            return 0L;
        }
        if (pData->pBuf == NULL)
        {
            pData->pBuf = _P_BUF;
            pData->dwSize = (T5_DWORD)_P_BUF_SIZE;
        }
        else if (pData->pBuf != _P_BUF) /* cannot change the array! */
        {
            _P_EMPTY = TRUE;
            _P_COUNT = _P_PREAD = _P_PWRITE = 0L;
            return 0L;
        }
        /* normal mode */
        if (_P_RST)
        {
            pData->pRead = pData->pWrite = 0L;
            pData->dwCount = 0L;
        }
        else if (_P_POP && !pData->bPopPrev && pData->dwCount > 0L)
        {
            if (pData->pRead)
                pData->pRead -= 1L;
            pData->pWrite -= 1L;
            pData->dwCount -= 1L;
        }
        else if (_P_PUSH && !pData->bPushPrev)
        {
            if (pData->dwCount == pData->dwSize)
                _P_OFLO = TRUE;
            else
            {
                T5_MEMCPY (pData->pBuf + (pData->pWrite * wSizeof),
                           pIn, wSizeof);
                if (pData->dwCount)
                    pData->pRead += 1L;
                pData->pWrite += 1L;
                pData->dwCount += 1L;
            }
        }
        /* triggers */
        pData->bPushPrev = _P_PUSH;
        pData->bPopPrev = _P_POP;
        /* outputs */
        if (pData->dwCount == 0L)
            T5_MEMSET (pOut, 0, wSizeof);
        else
            T5_MEMCPY (pOut, pData->pBuf + (pData->pRead * wSizeof), wSizeof);
        _P_EMPTY = (pData->dwCount == 0L);
        _P_COUNT = pData->dwCount;
        _P_PREAD = pData->pRead;
        _P_PWRITE = pData->pWrite;
        return 0L;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBfifo);
    case T5FBCMD_ACCEPTCT :
        return 1L;
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_LIFO;
    default :
        return 0L;
    }
}

#undef _P_ANYTYPE
#undef _P_PUSH
#undef _P_POP
#undef _P_RST
#undef _P_IN
#undef _P_OUT
#undef _P_BUF
#undef _P_BUF_SIZE
#undef _P_EMPTY
#undef _P_OFLO
#undef _P_COUNT
#undef _P_PREAD
#undef _P_PWRITE

/****************************************************************************/
/* ALARM A/M */

#define _P_IN       GET_D8IN(0)
#define _P_ACK      GET_D8IN(1)
#define _P_RST      GET_D8IN(2)
#define _P_Q        3
#define _P_QACK     4

#define _S_OFF      0
#define _S_ON       1
#define _S_ACK      2

T5_DWORD T5stfb_ALARM_M (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pMem;
    T5_PTBYTE pData8, pLock;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pMem = (T5_PTBYTE)pInst;
        switch (*pMem)
        {
        case _S_ON :
            if (_P_ACK)
                *pMem = _S_ACK;
            break;
        case _S_ACK :
            if (!_P_IN && _P_RST)
                *pMem = _S_OFF;
            break;
        default : /* _S_OFF */
            if (_P_IN)
                *pMem = _S_ON;
            break;
        }
        SET_D8OUT(_P_Q, (*pMem == _S_ON));
        SET_D8OUT(_P_QACK, (*pMem == _S_ACK));
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_ALARM_M;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_BYTE);
    default :
        return 0L;
    }
}

#undef _P_Q
#undef _P_QACK
#define _P_Q        2
#define _P_QACK     3

T5_DWORD T5stfb_ALARM_A (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pMem;
    T5_PTBYTE pData8, pLock;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pMem = (T5_PTBYTE)pInst;
        switch (*pMem)
        {
        case _S_ON :
            if (_P_ACK)
            {
                if (_P_IN)
                    *pMem = _S_ACK;
                else
                    *pMem = _S_OFF;
            }
            break;
        case _S_ACK :
            if (!_P_IN)
                *pMem = _S_OFF;
            break;
        default : /* _S_OFF */
            if (_P_IN)
                *pMem = _S_ON;
            break;
        }
        SET_D8OUT(_P_Q, (*pMem == _S_ON));
        SET_D8OUT(_P_QACK, (*pMem == _S_ACK));
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_ALARM_A;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_BYTE);
    default :
        return 0L;
    }
}

#undef _S_OFF
#undef _S_ON
#undef _S_ACK

#undef _P_IN
#undef _P_ACK
#undef _P_RST
#undef _P_Q
#undef _P_QACK

/****************************************************************************/
/* LOGFILECSV */

#ifdef T5DEF_VLST
#ifdef T5DEF_FILESUPPORTED

#define _P_LOG      GET_D8IN(0)
#define _P_RST      GET_D8IN(1)
#define _P_LIST     GET_DINTIN(2)
#define _P_PATH     (T5_PTCHAR)((pString[pArgs[3]])+2)
#define _P_Q        4
#define _P_ERR      5

typedef struct 
{
   T5_BOOL bLogPrev;
   T5_BOOL bRstPrev;
} _str_FBlogfilecsv;

T5_DWORD T5stfb_LOGFILECSV (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    _str_FBlogfilecsv *pMem;
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pDint;
    T5_PTBYTE *pString;
    T5_DWORD dwErr, hFile;
    T5_BOOL bOK;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = T5GET_DBDATA32(pBase);
        pString = T5GET_DBSTRING(pBase);
        pMem = (_str_FBlogfilecsv *)pInst;
        dwErr = 0;
        bOK = FALSE;

        if (_P_RST && !pMem->bRstPrev)
        {
            hFile = T5File_Open (_P_PATH, TRUE);
            if (!hFile)
                dwErr = 1;
            else
            {
                T5File_Close (hFile);
                bOK = TRUE;
            }
        }
        else if (_P_LOG && !pMem->bLogPrev)
        {
            hFile = T5File_OpenAppend (_P_PATH);
            if (!hFile)
                dwErr = 2;
            else
            {
                bOK = _LogCSV (pBase, hFile, _P_LIST, &dwErr);
                T5File_Close (hFile);
            }
        }
        pMem->bLogPrev = _P_LOG;
        pMem->bRstPrev = _P_RST;

        SET_D8OUT (_P_Q, bOK);
        SET_DINTOUT (_P_ERR, dwErr);
        return 0L;
	}
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_LOGFILECSV;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBlogfilecsv);
    default :
        return 0L;
    }
}

#undef _P_LOG
#undef _P_RST
#undef _P_LIST
#undef _P_PATH
#undef _P_Q
#undef _P_ERR

static void _CscFilterDec (T5_PTCHAR sz, T5_CHAR szSep)
{
    while (*sz)
    {
        if (*sz == '.')
            *sz = szSep;
        sz++;
    }
}

static T5_BOOL _LogCSV (T5PTR_DB pDB, T5_DWORD hFile, T5_DWORD dwList,
                        T5_PTDWORD pdwErr)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVLSTL pVLstL;
    T5PTR_CODEVLST pVLst;
    T5PTR_CODEVLSTV pVarDef;
    T5_PTDWORD pOffset;
    T5_WORD i;
    T5_PTBYTE pData;
    T5_CHAR szData[64], szSep[2];

    pPrivate = T5GET_DBPRIVATE (pDB);
    pStatus = T5GET_DBSTATUS(pDB);
    pHeader = (T5PTR_CODEHEADER)(pPrivate->pCode);
    pVLstL = T5VMCode_GetVLsts (pPrivate->pCode);
    if (pVLstL == NULL)
    {
        *pdwErr = 3;
        return FALSE;
    }
    if (dwList >= (T5_DWORD)pVLstL->wCount)
    {
        *pdwErr = 4;
        return FALSE;
    }

    sprintf ((char *)szSep, "%c", pStatus->cCsvSep);
    pOffset = &(pVLstL->dwVLst1);
    pOffset += dwList;
    pVLst = (T5PTR_CODEVLST)T5_OFF(pHeader, *pOffset);
    pVarDef = (T5PTR_CODEVLSTV)(pVLst+1);

    *pdwErr = 5; /* if any write fault */
    for (i=0; i<pVLst->wNbVar; i++, pVarDef++)
    {
        if (i > 0 && !T5File_Write (hFile, szSep, 1L))
            return FALSE;
        if (!T5File_Write (hFile, (T5_PTR)"\"", 1L))
            return FALSE;

        if (pVarDef->dwCTOffset != 0)
        {
            pData = (T5_PTBYTE)pDB[T5TB_CTSEG].pData;
            pData += pVarDef->dwCTOffset;
        }
        else
        {
            pData = (T5_PTBYTE)T5Tools_GetAnyParam (pDB, pVarDef->wType,
                                        pVarDef->wOffset, NULL, NULL);
        }
        if (pData == NULL)
            return FALSE;

        switch (pVarDef->wType)
        {
        case T5C_BOOL :
            if (*(T5_PTBYTE)pData)
                T5_STRCPY (szData, "1");
            else
                T5_STRCPY (szData, "0");
            break;
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT :
            T5_LTOA(szData, (T5_LONG)(*(T5_PTCHAR)pData));
            break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT :
            T5_LTOA(szData, (T5_LONG)(*(T5_PTBYTE)pData));
            break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT :
            T5_LTOA(szData, (T5_LONG)(*(T5_PTSHORT)pData));
            break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT :
            T5_LTOA(szData, (T5_LONG)(*(T5_PTWORD)pData));
            break;
#endif /*T5DEF_UINTSUPPORTED*/
        case T5C_DINT :
            T5_LTOA(szData, *(T5_PTLONG)pData);
            break;
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT :
            T5VMStr_UtoSZ (szData, *(T5_PTDWORD)pData);
            break;
#endif /*T5DEF_UDINTSUPPORTED*/
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL :
            T5_RTOA (szData, *(T5_PTREAL)pData);
            _CscFilterDec (szData, pStatus->cCsvDec);
            break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME :
            T5VMStr_UtoSZ (szData, *(T5_PTDWORD)pData);
            break;
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT :
            T5VMStr_LINTtoSZ (szData, *(T5_PTLINT)pData);
            break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT :
            T5VMStr_LINTtoSZ (szData, *(T5_PTLINT)pData); /* todo */
            break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL :
            T5_RTOA (szData, *(T5_PTLREAL)pData);
            _CscFilterDec (szData, pStatus->cCsvDec);
            break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING :
            if (pData[1] != 0 && pData[2] != 0
                && !T5File_Write (hFile, pData+2, (T5_DWORD)T5_STRLEN(pData + 2)))
                return FALSE;
            *szData = '\0';
            break;
#endif /*T5DEF_STRINGSUPPORTED*/
        default : return FALSE;
        }

        if (*szData && !T5File_Write (hFile, szData, (T5_DWORD)T5_STRLEN(szData)))
            return FALSE;
        if (!T5File_Write (hFile, (T5_PTR)"\"", 1L))
            return FALSE;
    }
    if (!T5File_Write (hFile, (T5_PTR)T5_FEOLSTRING, (T5_DWORD)T5_STRLEN(T5_FEOLSTRING)))
        return FALSE;

    *pdwErr = 0;
    return TRUE;
}

#endif /*T5DEF_FILESUPPORTED*/
#endif /*T5DEF_VLST*/

/****************************************************************************/
/* TMU / TMD */

#define _P_IN   GET_D8IN(0)
#define _P_RST  GET_D8IN(1)
#define _P_PT   GET_TIMEIN(2)
#define _P_Q    3
#define _P_ET   4

T5_DWORD T5stfb_TMU (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    _str_FBtimer *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBtimer *)pInst;

        if (_P_RST)
        {
            T5_MEMSET (pMem, 0, sizeof (_str_FBtimer));
        }
        else if (_P_IN && !(pMem->bFinished))
        {
            if (pMem->tLastCall == 0L)
                pMem->tLastCall = pStatus->dwTimStamp;
            pMem->tElapsed += T5_BOUNDTIME(pStatus->dwTimStamp
                                            - pMem->tLastCall);
            if (pMem->tElapsed >= _P_PT) 
            {
                pMem->tElapsed = _P_PT;
                pMem->bFinished = TRUE;
            }
        }
        pMem->tLastCall = pStatus->dwTimStamp;

        SET_D8OUT (_P_Q, pMem->bFinished);
        SET_TIMEOUT (_P_ET, pMem->tElapsed);
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_TMU;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBtimer);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBtimer *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    default :
        return 0L;
    }
}

T5_DWORD T5stfb_TMD (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pTime;
    T5_DWORD dwElapsed;
    _str_FBtimer *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBtimer *)pInst;

        if (_P_RST)
        {
            T5_MEMSET (pMem, 0, sizeof (_str_FBtimer));
            pMem->tElapsed = _P_PT;
        }
        else if (_P_IN && !(pMem->bFinished))
        {
            if (pMem->tLastCall == 0L)
                pMem->tLastCall = pStatus->dwTimStamp;
            dwElapsed = T5_BOUNDTIME(pStatus->dwTimStamp - pMem->tLastCall);
            if (pMem->tElapsed > dwElapsed)
            {
                pMem->tElapsed -= dwElapsed;
            }
            else
            {
                pMem->tElapsed = 0;
                pMem->bFinished = TRUE;
            }
        }
        pMem->tLastCall = pStatus->dwTimStamp;

        SET_D8OUT (_P_Q, pMem->bFinished);
        SET_TIMEOUT (_P_ET, pMem->tElapsed);
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_TMD;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBtimer);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBtimer *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_IN
#undef _P_RST
#undef _P_PT
#undef _P_Q
#undef _P_ET

/****************************************************************************/
/* TMUsec */

#define _P_IN   GET_D8IN(0)
#define _P_RST  GET_D8IN(1)
#define _P_PT   GET_DINTIN(2)
#define _P_Q    3
#define _P_ET   4

typedef struct 
{
   T5_DWORD dwElapsed;
   T5_DWORD dwLastCall;
   T5_BOOL  bTrigger;
   T5_BOOL  bFinished;
   T5_BOOL  bCounting;
   T5_BOOL  res1;
} _str_FBtumsec;

T5_DWORD T5stfb_TMUSEC (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pDint;
    _str_FBtumsec *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTDWORD)T5GET_DBDATA32(pBase);
        pMem = (_str_FBtumsec *)pInst;

        if (_P_RST)
        {
            T5_MEMSET (pMem, 0, sizeof (_str_FBtumsec));
        }
        else if (_P_IN && !(pMem->bFinished))
        {
            if (pMem->dwLastCall == 0L)
                pMem->dwLastCall = pStatus->dwElapsed;
            pMem->dwElapsed += (pStatus->dwElapsed - pMem->dwLastCall);
            if (pMem->dwElapsed >= _P_PT) 
            {
                pMem->dwElapsed = _P_PT;
                pMem->bFinished = TRUE;
            }
        }
        pMem->dwLastCall = pStatus->dwElapsed;

        SET_D8OUT (_P_Q, pMem->bFinished);
        SET_DINTOUT (_P_ET, pMem->dwElapsed);
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_TMUSEC;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBtumsec);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBtumsec *)pInst;
        pMem->dwLastCall = pStatus->dwElapsed;
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_IN
#undef _P_RST
#undef _P_PT
#undef _P_Q
#undef _P_ET

/****************************************************************************/
/* MBSlaveUDP */

#ifdef T5DEF_UDP

#define _P_IN   GET_D8IN(0)
#define _P_PORT GET_DINTIN(1)
#define _P_SLV  GET_DINTIN(2)
#define _P_RTU  GET_D8IN(3)
#define _P_ID   ((bEX)?(pString[pArgs[4]]+2):NULL)
#define _P_Q    ((bEX)?5:4)

T5_DWORD T5stfb_MBSLAVEUDP_hnd (T5_WORD wCommand, T5PTR_DB pBase,
                                T5_PTR pClass, T5_PTR pInst,
                                T5_PTWORD pArgs, T5_BOOL bEX)
{
    T5_PTSOCKET pSocket;
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint;
    T5_PTCHAR *pString;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pString = (T5_PTCHAR *)T5GET_DBSTRING(pBase);
        pSocket = (T5_PTSOCKET)pInst;
#ifdef T5DEF_MODBUSIOS
        /* open - close */
        if (_P_IN && *pSocket == T5_INVSOCKET && T5MB_FindServer (T5GET_MBC(pBase), _P_ID))
        {
            T5MBSrv_UdpOpen ((T5_WORD)_P_PORT, pSocket);
        }
        else if (!_P_IN && *pSocket != T5_INVSOCKET)
        {
            T5MBSrv_UdpClose (*pSocket);
            *pSocket = T5_INVSOCKET;
        }
        /* activate */
        if (*pSocket != T5_INVSOCKET)
        {
            T5MBSrv_UdpActivate (pBase, *pSocket, _P_RTU, (T5_WORD)_P_SLV, _P_ID);
        }
        SET_D8OUT (_P_Q, (T5_BOOL)(*pSocket != T5_INVSOCKET));
#else /*T5DEF_MODBUSIOS*/
        SET_D8OUT (_P_Q, FALSE);
#endif /*T5DEF_MODBUSIOS*/
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)((bEX)?T5FBL_MBSUDPEX:T5FBL_MBSUDP);
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(T5_SOCKET);
    case T5FBCMD_INITINSTANCE :
        *(T5_PTSOCKET)pInst = T5_INVSOCKET;
        return 0L;
    case T5FBCMD_HOTRESTART :
        pSocket = (T5_PTSOCKET)pInst;
        if (*pSocket != T5_INVSOCKET)
            T5Socket_CloseSocket (*pSocket);
        *pSocket = T5_INVSOCKET;
        return 0L;
    case T5FBCMD_EXITINSTANCE :
        pSocket = (T5_PTSOCKET)pInst;
        if (*pSocket != T5_INVSOCKET)
            T5Socket_CloseSocket (*pSocket);
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_IN
#undef _P_PORT
#undef _P_SLV
#undef _P_RTU
#undef _P_ID
#undef _P_Q

T5_DWORD T5stfb_MBSLAVEUDP (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    return T5stfb_MBSLAVEUDP_hnd (wCommand, pBase, pClass, pInst, pArgs, FALSE);
}

T5_DWORD T5stfb_MBSLAVEUDPEX (T5_WORD wCommand, T5PTR_DB pBase,
                              T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    return T5stfb_MBSLAVEUDP_hnd (wCommand, pBase, pClass, pInst, pArgs, TRUE);
}

#endif /*T5DEF_UDP*/

/****************************************************************************/
/* MBSlaveRTU */

#ifndef T5DEF_MBSLAVERTU_CUSTOM

#ifdef T5DEF_SERIAL

#define ISFC(b)  ( b == T5MBF_RIB || b == T5MBF_RCB || b == T5MBF_RIW ||\
b == T5MBF_RHW || b == T5MBF_W1B || b == T5MBF_W1W || b == T5MBF_WNB ||\
b == T5MBF_WNW || b == T5MBF_REX || b == T5MBF_RID || b == T5MBF_RWR ||\
b == T5MBF_DIAG || b == T5MBF_IDENT)

#define _P_IN   GET_D8IN(0)
#define _P_PORT (pString[pArgs[1]] + 2)
#define _P_SLV  GET_DINTIN(2)
#define _P_ID   ((bEX)?(pString[pArgs[3]]+2):NULL)
#define _P_Q    ((bEX)?4:3)
#define _P_NREC 5
#define _P_NX   6
#define _P_NERR 7
#define _P_NOK  8
#define _P_NBR  9
#define _P_NBAD 10


typedef struct 
{
    T5_BYTE   bIn[256];
    T5_BYTE   bOut[256];
    T5_WORD   wInSize;
    T5_WORD   wInSizeTx;
    T5_WORD   wInSizeRx;
    T5_WORD   wInRec;
    T5_WORD   wOutSize;
    T5_WORD   wOutSnt;
    T5_DWORD  dwNREC;
    T5_DWORD  dwNX;
    T5_DWORD  dwNERR;
    T5_DWORD  dwNOK;
    T5_DWORD  dwNBR;
    T5_DWORD  nbBAD;
    T5_SERIAL serial;
    T5_BOOL   bOpen;
} _str_FBMBSLAVERTU;

T5_DWORD T5stfb_MBSLAVERTU_hnd (T5_WORD wCommand, T5PTR_DB pBase,
                                T5_PTR pClass, T5_PTR pInst,
                                T5_PTWORD pArgs, T5_BOOL bEX, T5_DWORD dwID)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint;
    T5_PTCHAR *pString;
    _str_FBMBSLAVERTU *pMem;
    T5_BOOL bComplete;
#ifdef T5DEF_MODBUSIOS
    T5_PTBYTE pFrame;
    T5_WORD wCrc;
    T5_WORD isync ;
#endif /*T5DEF_MODBUSIOS*/
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pString = (T5_PTCHAR *)T5GET_DBSTRING(pBase);
        pMem = (_str_FBMBSLAVERTU *)pInst;

#ifdef T5DEF_MODBUSIOS
        /* open - close */
        if (_P_IN && !(pMem->bOpen) && T5MB_FindServer (T5GET_MBC(pBase), _P_ID))
        {
            pMem->dwNREC = pMem->dwNX = pMem->dwNERR = pMem->dwNOK = pMem->dwNBR = 0;
            T5_MEMSET (pMem, 0, sizeof (_str_FBMBSLAVERTU));
            T5Serial_Initialize (&(pMem->serial));
#ifdef T5DEF_SERIALINFO
            T5Serial_Info (&(pMem->serial), T5SERINFO_P_MBS);
#endif /*T5DEF_SERIALINFO*/
            pMem->bOpen = T5Serial_Open (&(pMem->serial), _P_PORT);
#ifdef T5DEF_SERIALINFO
            if (pMem->bOpen)
                T5Serial_Info (&(pMem->serial), T5SERINFO_MBS);
#endif /*T5DEF_SERIALINFO*/
        }
        else if (!_P_IN && pMem->bOpen)
        {
            T5Serial_Close (&(pMem->serial));
            T5Serial_Initialize (&(pMem->serial));
            pMem->bOpen = FALSE;
        }
        /* activate */
        if (pMem->bOpen)
        {
            if (pMem->wOutSize) /* data to send */
            {
                pFrame = pMem->bOut;
                pMem->wOutSnt += T5Serial_Send (
                    &(pMem->serial),
                    (T5_WORD)(pMem->wOutSize - pMem->wOutSnt),
                    (T5_PTR)(pFrame + pMem->wOutSnt));
                if (pMem->wOutSnt == pMem->wOutSize)
                {
                    pMem->wOutSnt = 0;
                    pMem->wOutSize = 0;

#ifdef T5DEF_SERIALINFO
                    T5Serial_Info (&(pMem->serial), T5SERINFO_SND);
#endif /*T5DEF_SERIALINFO*/
                }
            }
            else /* listen */
            {
                pFrame = pMem->bIn;
                /* pump characters */
                pMem->wInSize += T5Serial_Receive (&(pMem->serial), 256 - pMem->wInSize, pFrame + pMem->wInSize);
                pMem->wInRec = 0;
                bComplete = FALSE;
                while (!bComplete && pMem->wInRec < pMem->wInSize)
                {
                    switch (pMem->wInRec)
                    {
                    case 0 : /* slave number */
                        pMem->wInRec += 1;
                        break;
                    case 1 : /* function code */
                        pMem->wInRec += 1;
                        switch (pFrame[1])
                        {
                        case T5MBF_RIB : /* read bit inputs */
                        case T5MBF_RCB : /* read coils */
                        case T5MBF_RIW : /* read input registers */
                        case T5MBF_RHW : /* read holding registers */
                            pMem->wInSizeTx = 8;
                            pMem->wInSizeRx = 3; /* wait */
                            break;
                        case T5MBF_W1B : /* write 1 coil */
                        case T5MBF_W1W : /* write 1 register */
                        case T5MBF_DIAG : /* diagnostic */
                            pMem->wInSizeTx = 8;
                            pMem->wInSizeRx = 8;
                            break;
                        case T5MBF_WNB : /* write N coils */
                        case T5MBF_WNW : /* write N registers */
                            pMem->wInSizeTx = 7; /* wait */
                            pMem->wInSizeRx = 8;
                            break;
                        case T5MBF_IDENT :
                            pMem->wInSizeTx = 7;
                            pMem->wInSizeRx = 10; /* unknown list!*/
                            break;
                        case T5MBF_RID :
                            pMem->wInSizeTx = 4;
                            pMem->wInSizeRx = 3; /* wait */
                            break;
                        default :
                            if (pFrame[1] & 0x80) /* error answer */
                            {
                                pMem->wInSizeTx = 0;
                                pMem->wInSizeRx = 5;
                            }
                            else /* bad */
                            {
                                T5_MEMMOVE (pFrame, pFrame + 1, (pMem->wInSize - 1));
                                pMem->wInSize -= 1;
                                pMem->wInRec = 1;
                            }
                            break;
                        }
                        break;
                    case 2 : /* read N byte count? */
                        pMem->wInRec += 1;
                        if (pFrame[1] == T5MBF_RIB || pFrame[1] == T5MBF_RCB
                            || pFrame[1] == T5MBF_RIW || pFrame[1] == T5MBF_RHW || pFrame[1] == T5MBF_RID)
                        {
                            pMem->wInSizeRx = 5 + ((T5_WORD)(pFrame[2]) & 0x00ff);
                            if (pMem->wInSizeRx > 255)
                                pMem->wInSizeRx = 0; /* not an answer */
                        }
                        break;
                    default :
                        pMem->wInRec += 1;
                        /* is it an question ? */
                        if (pMem->wInSizeTx > 0 && pMem->wInRec == pMem->wInSizeTx)
                        {
                            /* is it a Write N header ? */
                            if (pMem->wInSizeTx == 7
                                && (pFrame[1] == T5MBF_WNB || pFrame[1] == T5MBF_WNW))
                            {
                                /* get byte count for write N */
                                pMem->wInSizeTx = 9 + ((T5_WORD)(pFrame[6]) & 0x00ff);
                                if (pMem->wInSizeTx > 255)
                                    pMem->wInSizeTx = 0; /* not a question */
                            }
                            /* or a complete question ? */
                            else
                            {
                                /* check CRC */
                                T5_COPYFRAMEWORD(&wCrc, &(pMem->bIn[pMem->wInSizeTx-2]));
                                wCrc = ((wCrc << 8) & 0xff00) | ((wCrc >> 8) & 0x00ff);
                                if (wCrc != T5Tools_Crc16 (pMem->bIn, (T5_WORD)(pMem->wInSizeTx-2)))
                                {
                                    pMem->nbBAD += 1L;
                                }
                                else
                                {
                                    /* frame complete and OK */
                                    pMem->dwNX += 1L;
                                    if (*pFrame == 0)
                                        pMem->dwNBR += 1L;
                                    if (*pFrame == (T5_BYTE)_P_SLV)
                                        pMem->dwNREC += 1L;
                                    if (*pFrame == (T5_BYTE)_P_SLV || *pFrame == 0)
                                    {
                                        pMem->wOutSize = T5MB_ServeExID (
                                            T5GET_MBC(pBase),
                                            pMem->bIn, pMem->bOut, FALSE,
                                            (T5_WORD)_P_SLV, _P_ID);
                                        if ((pMem->bOut[1] & 0x80) != 0)
                                            pMem->dwNERR += 1L;
                                        else if (*pFrame != 0)
                                            pMem->dwNOK += 1L;
                                        if (*pFrame != 0) /* answer if not broadcast */
                                        {
                                            wCrc = T5Tools_Crc16 (pMem->bOut, pMem->wOutSize);
                                            wCrc = ((wCrc << 8) & 0xff00) | ((wCrc >> 8) & 0x00ff);
                                            T5_COPYFRAMEWORD(&(pMem->bOut[pMem->wOutSize]), &wCrc);
                                            pMem->wOutSize += 2;
                                            pMem->wOutSnt = 0;

#ifdef T5DEF_SERIALINFO
                                            T5Serial_Info (&(pMem->serial), T5SERINFO_RCV);
#endif /*T5DEF_SERIALINFO*/
                                            /* try to send now */
                                            pFrame = pMem->bOut;
                                            pMem->wOutSnt += T5Serial_Send (
                                                &(pMem->serial),
                                                (T5_WORD)(pMem->wOutSize - pMem->wOutSnt),
                                                (T5_PTR)(pFrame + pMem->wOutSnt));
                                            if (pMem->wOutSnt == pMem->wOutSize)
                                            {
                                                pMem->wOutSnt = 0;
                                                pMem->wOutSize = 0;
#ifdef T5DEF_SERIALINFO
                                                T5Serial_Info (&(pMem->serial), T5SERINFO_SND);
#endif /*T5DEF_SERIALINFO*/
                                            }
                                        }
                                        else
                                            pMem->wOutSize = 0; /* broadcast */
                                        bComplete = TRUE;
                                    }
                                    T5_MEMMOVE (pFrame, pFrame + pMem->wInRec, (pMem->wInSize - pMem->wInRec));
                                    pMem->wInSize -= pMem->wInRec;
                                    pMem->wInRec = pMem->wInSizeRx = 0;
                                }
                                pMem->wInSizeTx = 0;
                            }
                        }
                        /* is it an anwser ? */
                        if (pMem->wInSizeRx > 0 && pMem->wInRec == pMem->wInSizeRx)
                        {
                            T5_COPYFRAMEWORD(&wCrc, &(pMem->bIn[pMem->wInSizeRx-2]));
                            wCrc = ((wCrc << 8) & 0xff00) | ((wCrc >> 8) & 0x00ff);
                            if (wCrc == T5Tools_Crc16 (pMem->bIn,
                                                       (T5_WORD)(pMem->wInSizeRx-2)))
                            {
                                /* an answer by someone else */
                                T5_MEMMOVE (pFrame, pFrame + pMem->wInRec, (pMem->wInSize - pMem->wInRec));
                                pMem->wInSize -= pMem->wInRec;
                                pMem->wInRec = pMem->wInSizeTx = 0;
                            }
                            pMem->wInSizeRx = 0;
                        }
                        /* rejected for both question and answer -> resynchro */
                        if (pMem->wInRec > pMem->wInSizeRx && pMem->wInRec > pMem->wInSizeTx)
                        {
                            isync = 1 ;

                            while (isync < pMem->wInSize)
                            {   /* resynchro : Search slave number */
                                if (pFrame[isync] == (T5_BYTE)_P_SLV || pFrame[isync] == 0)
                                {
                                    /* if we have a caracter after the slave number, check it */
                                    if (isync+1 < pMem->wInSize)
                                    {
                                        if (ISFC(pFrame[isync+1]))
                                            break ; /* The couple SLV/FC seems OK : hight probability it's the header of the frame */
                                        else isync+=2 ;
                                    }
                                    else break ;    /* The SLV is OK : perhaps it's the header of the frame */
                                }
                                 else isync++ ;
                            }

                            T5_MEMMOVE (pFrame, pFrame + isync, (pMem->wInSize - isync));
                            pMem->wInSize -= isync;
                            pMem->wInSizeRx = pMem->wInSizeTx = 0;
                            pMem->wInRec = 0;
                        }
                        break;
                    }
                }
            }
        }
        SET_D8OUT (_P_Q, pMem->bOpen);
        if (dwID == T5FBL_MBSRTUEXD)
        {
            SET_DINTOUT (_P_NREC, pMem->dwNREC);
            SET_DINTOUT (_P_NX, pMem->dwNX);
            SET_DINTOUT (_P_NERR, pMem->dwNERR);
            SET_DINTOUT (_P_NOK, pMem->dwNOK);
            SET_DINTOUT (_P_NBR, pMem->dwNBR);
            if (*(pArgs - 1) >= 11)
            {
                SET_DINTOUT (_P_NBAD, pMem->nbBAD);
            }
        }
#else /*T5DEF_MODBUSIOS*/
        SET_D8OUT (_P_Q, FALSE);
#endif /*T5DEF_MODBUSIOS*/
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return dwID;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBMBSLAVERTU);
    case T5FBCMD_INITINSTANCE :
        pMem = (_str_FBMBSLAVERTU *)pInst;
        T5_MEMSET (pMem, 0, sizeof (_str_FBMBSLAVERTU));
        T5Serial_Initialize (&(pMem->serial));
        return 0L;
    case T5FBCMD_HOTRESTART :
        pMem = (_str_FBMBSLAVERTU *)pInst;
        pMem->bOpen = T5Serial_IsValid (&(pMem->serial));
        return 0L;
    case T5FBCMD_EXITINSTANCE :
        pMem = (_str_FBMBSLAVERTU *)pInst;
        if (T5Serial_IsValid (&(pMem->serial)))
            T5Serial_Close (&(pMem->serial));
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_IN
#undef _P_PORT
#undef _P_SLV
#undef _P_ID
#undef _P_Q

T5_DWORD T5stfb_MBSLAVERTU (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    return T5stfb_MBSLAVERTU_hnd (wCommand, pBase, pClass, pInst, pArgs, FALSE, T5FBL_MBSRTU);
}

T5_DWORD T5stfb_MBSLAVERTUEX (T5_WORD wCommand, T5PTR_DB pBase,
                              T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    return T5stfb_MBSLAVERTU_hnd (wCommand, pBase, pClass, pInst, pArgs, TRUE, T5FBL_MBSRTUEX);
}

T5_DWORD T5stfb_MBSLAVERTUEXD (T5_WORD wCommand, T5PTR_DB pBase,
    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    return T5stfb_MBSLAVERTU_hnd (wCommand, pBase, pClass, pInst, pArgs, TRUE, T5FBL_MBSRTUEXD);
}

#endif /*T5DEF_SERIAL*/

#endif /*T5DEF_MBSLAVERTU_CUSTOM*/

/* signals ******************************************************************/

#ifdef T5DEF_SIGNAL
#ifdef T5DEF_REALSUPPORTED

T5_PTR T5stfb_GetSignal (T5PTR_DB pDB, T5_DWORD dwID,
                                 T5_PTDWORD pdwDuration)
{
    T5PTR_DBPRIVATE pPrivate;
    T5_PTDWORD pdwSignal;

    pPrivate = T5GET_DBPRIVATE (pDB);
    pdwSignal = (T5_PTDWORD)T5VMCode_GetSignal (pPrivate->pCode, dwID);
    if (pdwSignal == NULL || *pdwSignal < 2L)
    {
        *pdwDuration = 0L;
        return NULL;
    }

    *pdwDuration = pdwSignal[(*pdwSignal * 2) - 1];
    return pdwSignal;
}

T5_REAL T5stfb_GetSigVal (T5_PTR pSignal, T5_DWORD dwTime)
{
    T5_PTDWORD pTime;
    T5_DWORD nb, iMin, tDiff;
    T5_REAL rDiff;

    pTime = (T5_PTDWORD)pSignal;
    /* check at least one point */
    nb = *pTime;
    if (nb == 0)
        return 0L;

    /* return if one point or if before 1rst stamp */
    pTime++;
    if (nb == 1 || dwTime <= *pTime)
        return *(T5_PTREAL)(pTime+1);

    /* now nb is the number of DWORD */
    nb *= 2;
    iMin = 0;
    /* search for interval */
    while (iMin < (nb-2) && dwTime >= pTime[iMin+2])
        iMin += 2;
    if (iMin >= nb)
        iMin = nb - 2;

    /* exact time found */
    if (dwTime == pTime[iMin])
        return *(T5_PTREAL)(pTime+iMin+1);

    /* skip similar times */
    while (iMin < (nb-2) && pTime[iMin] == pTime[iMin+2])
        iMin += 2;
    if (iMin >= nb)
        iMin = nb - 2;

    /* not found - return last value */
    if (iMin == (nb - 2))
        return *(T5_PTREAL)(pTime+iMin+1);

    /* check time difference */
    tDiff = pTime[iMin+2] - pTime[iMin];
    if ((T5_LONG)tDiff <= 0)
        return *(T5_PTREAL)(pTime+iMin+1);

    /* scale */
    rDiff = *(T5_PTREAL)(pTime+iMin+3)
          - *(T5_PTREAL)(pTime+iMin+1);
    rDiff *= (dwTime - pTime[iMin]);
    rDiff /= (T5_REAL)tDiff;
    rDiff += *(T5_PTREAL)(pTime+iMin+1);
    return rDiff;
}

#define _P_IN   GET_D8IN(0)
#define _P_ID   GET_DINTIN(1)
#define _P_RST  GET_D8IN(2)
#define _P_TM   GET_TIMEIN(3)
#define _P_Q    4
#define _P_OUT  5
#define _P_ET   6

typedef struct 
{
   T5_PTR   pSignal;
   T5_REAL  rOut;
   T5_DWORD dwID;
   T5_DWORD tElapsed;
   T5_DWORD tLastCall;
   T5_DWORD tLastSample;
   T5_DWORD tMax;
   T5_BOOL  bFinished;
   T5_BOOL  res1;
} _str_FBSIGPLAY;

T5_DWORD T5stfb_SIGPLAY (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint;
    T5_PTDWORD pTime;
    T5_PTREAL pReal;
    _str_FBSIGPLAY *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_DWORD tSample;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pReal = (T5_PTREAL)T5GET_DBDATA32(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBSIGPLAY *)pInst;

        if (!_P_RST && pMem->dwID != (T5_DWORD)_P_ID)
        {
            pMem->pSignal = T5stfb_GetSignal (pBase, (T5_DWORD)_P_ID, &(pMem->tMax));
            pMem->dwID = (T5_DWORD)_P_ID;
        }

        if (_P_RST)
        {
            T5_MEMSET (pMem, 0, sizeof (_str_FBSIGPLAY));
        }
        else if (_P_IN && !(pMem->bFinished) && pMem->pSignal != NULL)
        {
            tSample = T5_BOUNDTIME(pStatus->dwTimStamp
                                   - pMem->tLastCall);
            if (tSample)
            {
                pMem->tElapsed += tSample;
                if (pMem->tElapsed >= pMem->tMax && pMem->tLastSample != 0) 
                {
                    pMem->tElapsed = pMem->tMax;
                    pMem->bFinished = TRUE;
                    pMem->tLastCall = pStatus->dwTimStamp;
                    pMem->rOut = T5stfb_GetSigVal (pMem->pSignal, pMem->tElapsed);
                    pMem->tLastSample = pStatus->dwTimStamp;
                }
                if (pMem->tLastSample == 0
                    || T5_BOUNDTIME(pStatus->dwTimStamp
                                    - pMem->tLastSample) >= _P_TM)
                {
                    pMem->rOut = T5stfb_GetSigVal (pMem->pSignal, pMem->tElapsed);
                    pMem->tLastSample = pStatus->dwTimStamp;
                }
            }
        }

        pMem->tLastCall = pStatus->dwTimStamp;

        SET_D8OUT (_P_Q, pMem->bFinished);
        SET_REALOUT (_P_OUT, pMem->rOut);
        SET_TIMEOUT (_P_ET, pMem->tElapsed);
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_SIGPLAY;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBSIGPLAY);
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        pMem = (_str_FBSIGPLAY *)pInst;
        pMem->tLastCall = pStatus->dwTimStamp;
        pMem->tLastSample = 0L;
        pMem->pSignal = T5stfb_GetSignal (pBase, pMem->dwID, &(pMem->tMax));
        return 0L;
    default :
        return 0L;
    }
}

#undef _P_IN
#undef _P_ID
#undef _P_RST
#undef _P_TM
#undef _P_Q
#undef _P_OUT
#undef _P_ET

#endif /*T5DEF_REALSUPPORTED*/
#endif /*T5DEF_SIGNAL*/

/* ISFC *********************************************************************/

#define _P_IN   GET_DINTIN(0)
#define _P_X    1
#define _P_T    2
#define _P_P1   3
#define _P_N    4
#define _P_P0   5

typedef struct 
{
    T5_DWORD tElapsed;
    T5_DWORD tLastCall;
    T5_BYTE  bState;
    T5_BYTE res1;
    T5_BYTE res2;
    T5_BYTE res3;
} _str_FBISFCSTEP;

#define _T5ISFC_INA 0
#define _T5ISFC_P1  1
#define _T5ISFC_N   2
#define _T5ISFC_P0  3

static T5_DWORD _T5stfb_ISFC_Step (T5_WORD wCommand, T5PTR_DB pBase,
                                   T5_PTR pClass, T5_PTR pInst,
                                   T5_PTWORD pArgs, T5_BOOL bInit)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint;
    T5_PTDWORD pTime;
    T5_DWORD tSample;
    _str_FBISFCSTEP *pMem;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pStatus = T5GET_DBSTATUS(pBase);
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pTime = T5GET_DBTIME(pBase);
        pMem = (_str_FBISFCSTEP *)pInst;

        switch (_P_IN)
        {
        case 2 :
            break;
        case 1 :
            /*T5_PRINTF ("ACTIVATE %u", pMem);*/
            if (pMem->bState == _T5ISFC_INA || pMem->bState == _T5ISFC_P0)
            {
                pMem->bState = _T5ISFC_P1;
                pMem->tElapsed = 0L;
            }
            else
            {
                pMem->bState = _T5ISFC_N;
                if (pMem->tLastCall != 0L)
                {
                    tSample = T5_BOUNDTIME (pStatus->dwTimStamp - pMem->tLastCall);
                    if ((pMem->tElapsed + tSample) > T5_24HMS)
                        pMem->tElapsed = T5_24HMS;
                    else
                        pMem->tElapsed += tSample;
                }
            }
            pMem->tLastCall = pStatus->dwTimStamp;
            break;
        case 0 :
            /*T5_PRINTF ("DEACTIVATE %u", pMem);*/
            if (pMem->bState == _T5ISFC_N)
                pMem->bState = _T5ISFC_P0;
            else
                pMem->bState = _T5ISFC_INA;
            pMem->tLastCall = pStatus->dwTimStamp;
            break;
        default :
            if (pMem->bState == _T5ISFC_P1)
                pMem->bState = _T5ISFC_N;
            else if (pMem->bState == _T5ISFC_P0)
                pMem->bState = _T5ISFC_INA;
            if (pMem->bState == _T5ISFC_N && pMem->tLastCall != 0L)
            {
                tSample = T5_BOUNDTIME (pStatus->dwTimStamp - pMem->tLastCall);
                if ((pMem->tElapsed + tSample) > T5_24HMS)
                    pMem->tElapsed = T5_24HMS;
                else
                    pMem->tElapsed += tSample;
            }
            pMem->tLastCall = pStatus->dwTimStamp;
            break;
        }

        SET_D8OUT (_P_X, (pMem->bState == _T5ISFC_N));
        SET_D8OUT (_P_P1, (pMem->bState == _T5ISFC_P1));
        SET_D8OUT (_P_N, (pMem->bState == _T5ISFC_N || pMem->bState == _T5ISFC_P1));
        SET_D8OUT (_P_P0, (pMem->bState == _T5ISFC_P0));
        SET_TIMEOUT (_P_T, pMem->tElapsed);
        return 0L;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        if (bInit)
            return (T5_DWORD)T5FBL_ISFCINITSTEP;
        else
            return (T5_DWORD)T5FBL_ISFCSTEP;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FBISFCSTEP);
    case T5FBCMD_INITINSTANCE :
        pMem = (_str_FBISFCSTEP *)pInst;
        if (bInit)
            pMem->bState = _T5ISFC_P1;
        return 0L;
    case T5FBCMD_HOTRESTART :
        pMem = (_str_FBISFCSTEP *)pInst;
        pStatus = T5GET_DBSTATUS(pBase);
        pMem->tLastCall = pStatus->dwTimStamp;
        return 0L;
    default :
        return 0L;
    }
}

T5_DWORD T5stfb_ISFCSTEP (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    return _T5stfb_ISFC_Step (wCommand, pBase, pClass, pInst, pArgs, FALSE);
}

T5_DWORD T5stfb_ISFCINITSTEP (T5_WORD wCommand, T5PTR_DB pBase,
                              T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    return _T5stfb_ISFC_Step (wCommand, pBase, pClass, pInst, pArgs, TRUE);
}

#undef _P_IN
#undef _P_X
#undef _P_T
#undef _P_P1
#undef _P_N
#undef _P_P0

/* Linearization - curve ****************************************************/

#define T5LINERR_INTERNAL   -1
#define T5LINERR_BADDIMS    1
#define T5LINERR_BADXASIS   2
#define T5LINERR_BADYASIS   3
#define T5LINERR_XOVER      4
#define T5LINERR_YOVER      5

#ifdef T5DEF_REALSUPPORTED

#define _P_X        GET_REALIN(0)
#define _P_XAXIS    (pArgs[1])
#define _P_XAXIS_N  (pArgs[2])
#define _P_YVAL     (pArgs[3])
#define _P_YVAL_N   (pArgs[4])
#define _P_Y        5
#define _P_OK       6
#define _P_ERR      7

T5_DWORD T5stfb_CURVELIN (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint;
    T5_PTREAL pReal, pXaxis, pYval;
    T5_WORD i;
    T5_REAL x, y;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pReal = (T5_PTREAL)T5GET_DBDATA32(pBase);
        pXaxis = (T5_PTREAL)T5Tools_GetD32Array(pBase, _P_XAXIS);
        pYval = (T5_PTREAL)T5Tools_GetD32Array(pBase, _P_YVAL);
        SET_REALOUT(_P_Y, 0);
        SET_D8OUT(_P_OK, FALSE);
        SET_DINTOUT(_P_ERR, T5LINERR_INTERNAL);
        /* check array dimentsions */
        if (_P_XAXIS_N < 2 || _P_XAXIS_N != _P_YVAL_N)
        {
            SET_DINTOUT(_P_ERR, T5LINERR_BADDIMS);
            return 0;
        }
        /* check X axis */
        for (i=1; i<_P_XAXIS_N; i++)
        {
            if (pXaxis[i] <= pXaxis[i-1])
            {
                SET_DINTOUT(_P_ERR, T5LINERR_BADXASIS);
                return 0;
            }
        }
        /* search for point */
        x = _P_X;
        /* X too small */
        if (x < pXaxis[0])
        {
            SET_REALOUT(_P_Y, pYval[0]);
            SET_DINTOUT(_P_ERR, T5LINERR_XOVER);
            return 0;
        }
        /* search for point */
        for (i=1; i<_P_XAXIS_N; i++)
        {
            if (x <= pXaxis[i] && x >= pXaxis[i-1])
            {
                y = pYval[i-1]
                  + (pYval[i] - pYval[i-1])
                  / (pXaxis[i] - pXaxis[i-1])
                  * (x - pXaxis[i-1]);
                SET_REALOUT(_P_Y, y);
                SET_D8OUT(_P_OK, TRUE);
                SET_DINTOUT(_P_ERR, 0);
                return 0;
            }
        }
        /* X too big */
        SET_REALOUT(_P_Y, pYval[_P_XAXIS_N-1]);
        SET_DINTOUT(_P_ERR, T5LINERR_YOVER);
        return 0;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_CURVELIN;
    case T5FBCMD_ACCEPTCT :
        return 1L;
    default :
        return 0L;
    }
}

#undef _P_X
#undef _P_XAXIS
#undef _P_XAXIS_N
#undef _P_YVAL
#undef _P_YVAL_N
#undef _P_Y
#undef _P_OK
#undef _P_ERR

#endif /*T5DEF_REALSUPPORTED*/

/* Linearization - surface **************************************************/

#ifdef T5DEF_REALSUPPORTED

#define _P_X        GET_REALIN(0)
#define _P_Y        GET_REALIN(1)
#define _P_XAXIS    (pArgs[2])
#define _P_XAXIS_N  (pArgs[3])
#define _P_YAXIS    (pArgs[4])
#define _P_YAXIS_N  (pArgs[5])
#define _P_ZVAL     (pArgs[6])
#define _P_ZVAL_N   (pArgs[7])
#define _P_Z        8
#define _P_OK       9
#define _P_ERR      10

#define _Z_(ix,iy)  pZval[(ix)*_P_YAXIS_N+(iy)]

T5_DWORD T5stfb_SURFLIN (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8, pLock;
    T5_PTLONG pDint;
    T5_PTREAL pReal, pXaxis, pYaxis, pZval;
    T5_REAL x, y, z, zLeft, zRight;
    T5_WORD i, iX, iY;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pData8 = T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pReal = (T5_PTREAL)T5GET_DBDATA32(pBase);
        pXaxis = (T5_PTREAL)T5Tools_GetD32Array(pBase, _P_XAXIS);
        pYaxis = (T5_PTREAL)T5Tools_GetD32Array(pBase, _P_YAXIS);
        pZval = (T5_PTREAL)T5Tools_GetD32Array(pBase, _P_ZVAL);
        SET_REALOUT(_P_Z, 0);
        SET_D8OUT(_P_OK, FALSE);
        SET_DINTOUT(_P_ERR, T5LINERR_INTERNAL);
        x = _P_X;
        y = _P_Y;
        /* check array dimentsions */
        if (_P_XAXIS_N < 2 || _P_YAXIS_N < 2
            || _P_ZVAL_N != (_P_XAXIS_N * _P_YAXIS_N))
        {
            SET_DINTOUT(_P_ERR, T5LINERR_BADDIMS);
            return 0;
        }
        /* check X axis */
        for (i=1; i<_P_XAXIS_N; i++)
        {
            if (pXaxis[i] <= pXaxis[i-1])
            {
                SET_DINTOUT(_P_ERR, T5LINERR_BADXASIS);
                return 0;
            }
        }
        /* check Y axis */
        for (i=1; i<_P_YAXIS_N; i++)
        {
            if (pYaxis[i] <= pYaxis[i-1])
            {
                SET_DINTOUT(_P_ERR, T5LINERR_BADYASIS);
                return 0;
            }
        }
        /* check bounds and report possible error */
        if (x < pXaxis[0])
            x = pXaxis[0];
        else if (x > pXaxis[_P_XAXIS_N-1])
            x = pXaxis[_P_XAXIS_N-1];
        if (y < pYaxis[0])
            y = pYaxis[0];
        else if (y > pYaxis[_P_YAXIS_N-1])
            y = pYaxis[_P_YAXIS_N-1];
        if (x != _P_X)
        {
            SET_DINTOUT(_P_ERR, T5LINERR_XOVER);
        }
        else if (y != _P_Y)
        {
            SET_DINTOUT(_P_ERR, T5LINERR_YOVER);
        }
        else
        {
            SET_D8OUT(_P_OK, FALSE);
            SET_DINTOUT(_P_ERR, 0);
        }
        /* find cell */
        for (iX=0, i=1; i<_P_XAXIS_N; i++)
        {
            if (x >= pXaxis[i-1] && x <= pXaxis[i])
            {
                iX = i - 1;
                break;;
            }
        }
        for (iY=0, i=1; i<_P_YAXIS_N; i++)
        {
            if (y >= pYaxis[i-1] && y <= pYaxis[i])
            {
                iY = i - 1;
                break;;
            }
        }
        /* y-interpolation left */
        zLeft = _Z_ (iX, iY)
                + (_Z_ (iX, iY+1) - _Z_ (iX, iY))
                / (pYaxis[iY+1] - pYaxis[iY])
                * (y - pYaxis[iY]);
        /* y-interpolation right */
        zRight = _Z_ (iX+1, iY)
                 + (_Z_ (iX+1, iY+1) - _Z_ (iX+1, iY))
                 / (pYaxis[iY+1] - pYaxis[iY])
                 * (y - pYaxis[iY]);
        /* x-interpolation */
        z = zLeft
            + (zRight - zLeft)
            / (pXaxis[iX+1] - pXaxis[iX])
            * (x - pXaxis[iX]);
        SET_REALOUT(_P_Z, z);
        return 0;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_SURFLIN;
    case T5FBCMD_ACCEPTCT :
        return 1L;
    default :
        return 0L;
    }
}

#undef _P_X
#undef _P_Y
#undef _P_XAXIS
#undef _P_XAXIS_N
#undef _P_YAXIS
#undef _P_YAXIS_N
#undef _P_ZVAL
#undef _P_ZVAL_N
#undef _P_Z
#undef _P_OK
#undef _P_ERR

#undef _Z_

#endif /*T5DEF_REALSUPPORTED*/

/* current date and time - single call **************************************/

#ifdef T5DEF_RTCLOCKSTAMP

#define _P_LOCAL    GET_D8IN(0)
#define _P_YEAR     1
#define _P_MONTH    2
#define _P_DAY      3
#define _P_HOUR     4
#define _P_MIN      5
#define _P_SEC      6
#define _P_MSEC     7
#define _P_TMOFDAY  8
#define _P_DST      9

T5_DWORD T5stfb_DTCURDATETIME (T5_WORD wCommand, T5PTR_DB pBase,
                               T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_PTBOOL pData8, pLock;
    T5_PTDWORD pTime;
    T5_PTLONG pDint;
    T5_DWORD dwDate, dwTime;
    T5_BOOL bDST;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    if (!wCommand)
	{
        pDint = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pTime = T5GET_DBTIME(pBase);
        pData8 = (T5_PTBOOL)T5GET_DBDATA8(pBase);
        pLock = T5GET_DBLOCK(pBase);

        bDST = FALSE;
        T5RTCLK_GETDATETIME(_P_LOCAL, dwDate, dwTime, bDST);

        SET_DINTOUT (_P_YEAR, (dwDate >> 16) & 0xffffL);
        SET_DINTOUT (_P_MONTH, (dwDate >> 8) & 0xffL);
        SET_DINTOUT (_P_DAY, dwDate & 0xffL);
        SET_DINTOUT (_P_HOUR, dwTime / 3600000L);
        SET_DINTOUT (_P_MIN, (dwTime / 60000L) % 60L);
        SET_DINTOUT (_P_SEC, (dwTime / 1000L) % 60L);
        SET_DINTOUT (_P_MSEC, dwTime % 1000L);
        SET_TIMEOUT (_P_TMOFDAY, dwTime);
        SET_D8OUT (_P_DST, bDST);
        return 0;
    }
    switch (wCommand)
    {
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_DTCURDATETIME;
    case T5FBCMD_ACCEPTCT :
        return 1L;
    default :
        return 0L;
    }
}

#undef _P_LOCAL
#undef _P_YEAR
#undef _P_MONTH
#undef _P_DAY
#undef _P_HOUR
#undef _P_MIN
#undef _P_SEC
#undef _P_MSEC
#undef _P_TMOFDAY
#undef _P_DST

#endif /*T5DEF_RTCLOCKSTAMP*/

/* eof **********************************************************************/
