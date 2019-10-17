/*****************************************************************************
T5VM.c :     main entry points of the virtual machine

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/

#ifndef T5EAX_NBVAR_XALLOC
#define T5EAX_NBVAR_XALLOC 0
#endif /*T5EAX_NBVAR_XALLOC*/

#ifndef T5DEF_DDKCEXTRAMEM
#define T5DEF_DDKCEXTRAMEM 0
#endif /*T5DEF_DDKCEXTRAMEM*/

/*****************************************************************************
T5VM_GetDBSize
Calculate the size to be allocated for each DB table
(get sizing information from the application code)
fills members 'wSizeOf' and 'wNbItemAlloc' for each table
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN/OUT) pointer to a DB header.
Return: ok or error
*****************************************************************************/

T5_RET T5VM_GetDBSize (T5_PTR pCode, T5PTR_DB pDB)
{
    T5_DWORD dwRawSize;
    T5_WORD wRet;

    wRet = T5RET_OK;
    /* first check if this code is correct */
    if (T5VMCode_CheckCode (pCode) != T5RET_OK)
        return T5RET_ERROR;
    /* status */
    pDB[T5TB_STATUS].wSizeOf = sizeof (T5STR_DBSTATUS);
    pDB[T5TB_STATUS].wNbItemAlloc = 1;
    /* stack of log messages */
    pDB[T5TB_LOG].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_LOG].dwRawSize = T5DBLOG_RAWSIZE + T5MAX_SMARTLOCK;
    /* TIC command from extarnal comm */
    pDB[T5TB_EXEC].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_EXEC].wNbItemAlloc = T5MAX_EXECBUFFER;
    /* VM private data */
    pDB[T5TB_PRIVATE].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_PRIVATE].dwRawSize = sizeof (T5STR_DBPRIVATE) + T5VMCode_GetHeapSize (pCode);
    /* programs */
    pDB[T5TB_PROG].wSizeOf = sizeof (T5STR_DBPROG);
    pDB[T5TB_PROG].wNbItemAlloc = T5VMCode_GetNbProgAlloc (pCode);
    /* lock flags */
    pDB[T5TB_LOCK].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_LOCK].wNbItemAlloc = T5VMCode_GetNbLockFlags (pCode);
    /* 8 bit data */
    pDB[T5TB_DATA8].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_DATA8].wNbItemAlloc = T5VMCode_GetNbData8 (pCode);
    /* 16 bit data */
    pDB[T5TB_DATA16].wSizeOf = sizeof (T5_WORD);
    pDB[T5TB_DATA16].wNbItemAlloc = T5VMCode_GetNbData16 (pCode);
    /* 32 bit data */
    pDB[T5TB_DATA32].wSizeOf = sizeof (T5_DWORD);
    pDB[T5TB_DATA32].wNbItemAlloc = T5VMCode_GetNbData32 (pCode);
    /* 64 bit data */
#ifdef T5DEF_LREALSUPPORTED
    pDB[T5TB_DATA64].wSizeOf = sizeof (T5_DATA64);
#else /*T5DEF_LREALSUPPORTED*/
    pDB[T5TB_DATA64].wSizeOf = 0;
#endif /*T5DEF_LREALSUPPORTED*/
    pDB[T5TB_DATA64].wNbItemAlloc = T5VMCode_GetNbData64 (pCode);
    /* timers */
    pDB[T5TB_TIME].wSizeOf = sizeof (T5_DWORD);
    pDB[T5TB_TIME].wNbItemAlloc = T5VMCode_GetNbTime (pCode);
    /* list of active timers */
    pDB[T5TB_ACTIME].wSizeOf = sizeof (T5STR_DBACTIME);
    pDB[T5TB_ACTIME].wNbItemAlloc = T5VMCode_GetNbActiveTime (pCode);
    /* FB classes */
    pDB[T5TB_FBCLASS].wSizeOf = sizeof (T5STR_DBFBCLASS);
    pDB[T5TB_FBCLASS].wNbItemAlloc = T5VMFbl_GetNbClass (pCode);
    /* FB classes - data */
    pDB[T5TB_FBCDATA].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_FBCDATA].dwRawSize = T5VMFbl_GetClassDataSize (pCode);
    /* FB instances */
    pDB[T5TB_FBINST].wSizeOf = sizeof (T5STR_DBFBINST);
    pDB[T5TB_FBINST].wNbItemAlloc = T5VMCode_GetNbFBInstance (pCode);
    /* FB instances - data */
    pDB[T5TB_FBIDATA].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_FBIDATA].dwRawSize = T5VMFbl_GetInstanceDataSize (pCode);
    /* custom function classes */
    pDB[T5TB_FCLASS].wSizeOf = sizeof (T5STR_DBFCLASS);
    pDB[T5TB_FCLASS].wNbItemAlloc = T5VMCode_GetNbCustomFunc (pCode);
    /* string variables */
    pDB[T5TB_STRING].wSizeOf = sizeof (T5_PTBYTE *);
    pDB[T5TB_STRING].wNbItemAlloc = T5VMCode_GetNbString (pCode, &dwRawSize);
    /* string buffers */
    pDB[T5TB_STRBUF].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_STRBUF].dwRawSize = dwRawSize;
    /* system resources */
    pDB[T5TB_SYSRSC].wSizeOf = sizeof (T5STR_DBSYSRSC);
    pDB[T5TB_SYSRSC].wNbItemAlloc = T5MAX_SYSRSC;
    /* SFC steps */
    pDB[T5TB_STEP].wSizeOf = sizeof (T5STR_DBSTEP);
    pDB[T5TB_STEP].wNbItemAlloc = T5VMCode_GetNbStep (pCode);
    /* SFC transitions */
    pDB[T5TB_TRANS].wSizeOf = sizeof (T5STR_DBTRANS);
    pDB[T5TB_TRANS].wNbItemAlloc = T5VMCode_GetNbTrans (pCode);
    /* stack of unresolved resources */
    pDB[T5TB_UNRES].wSizeOf = sizeof (T5STR_DBUNRES);
    pDB[T5TB_UNRES].wNbItemAlloc = T5MAX_UNRES;
    /* IOs */
    pDB[T5TB_IODATA].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_IODATA].dwRawSize = T5Ios_GetMemorySizing (
                                                   T5VMCode_GetIODef (pCode));
    /* embedded var map */
#ifdef T5DEF_VARMAP
    if ((wRet = T5VMMap_GetSizing (pDB, pCode)) != T5RET_OK)
        return wRet;
#endif /*T5DEF_VARMAP*/

    /* external variables */
#ifdef T5DEF_XV
    pDB[T5TB_XV].wSizeOf = sizeof (T5_PTBYTE);
    pDB[T5TB_XV].wNbItemAlloc = T5VMCode_GetNbXV (pCode);
#endif /*T5DEF_XV*/

    /* sampling trace */
#ifdef T5DEF_BSAMPLING
    pDB[T5TB_BSAMPLING].wSizeOf = sizeof (T5STR_DBBSAMPLING);
    pDB[T5TB_BSAMPLING].wNbItemAlloc = 1;
#endif /*T5DEF_BSAMPLING*/

    /* tasks */
    pDB[T5TB_TASK].wSizeOf = sizeof (T5STR_DBTASK);
    pDB[T5TB_TASK].wNbItemAlloc = T5MAX_TASK;

    /* call stack */
    pDB[T5TB_CALLSTACK].wSizeOf = sizeof (T5STR_DBCALLSTACK);
    pDB[T5TB_CALLSTACK].wNbItemAlloc = T5VMCode_GetCallStackAllocSize (pCode);

    /* MODBUS */
#ifdef T5DEF_MODBUSIOS
    pDB[T5TB_MBC].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_MBC].dwRawSize = T5MB_GetMemorySizing (
                                    T5VMCode_MBClientConfig (pCode));
#endif /*T5DEF_MODBUSIOS*/

    /* ASi I/Os */
#ifdef T5DEF_ASI
    pDB[T5TB_ASI].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_ASI].dwRawSize = T5VMAsi_GetSizing (
                                    (T5PTR_ASIDEF)T5VMCode_AsiConfig (pCode));
#endif /*T5DEF_ASI*/

    /* CAN bus */
#ifdef T5DEF_CANBUS
    T5VMCan_GetDBSize (pDB, pCode);
#endif /*T5DEF_CANBUS*/

    /* EA Change events and external variables */
#ifdef T5DEF_EA
    pDB[T5TB_EACHANGE].wSizeOf = sizeof (T5STR_EACHANGE);
    pDB[T5TB_EACHANGE].wNbItemAlloc = T5MAX_EACHANGE;

    pDB[T5TB_EACNX].wSizeOf = sizeof (T5STR_EACNX);
    pDB[T5TB_EACNX].wNbItemAlloc = T5EAX_GetNbCnx (pCode);
    pDB[T5TB_EAEXT].wSizeOf = sizeof (T5STR_EAEXT);
    pDB[T5TB_EAEXT].wNbItemAlloc = T5EAX_GetNbVar (pCode) + T5EAX_NBVAR_XALLOC;
#endif /*T5DEF_EA*/

#ifdef T5DEF_CTSEG
    pDB[T5TB_CTSEG].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_CTSEG].dwRawSize = T5VMCode_GetCTSegAllocSize (pCode);
#endif /*T5DEF_CTSEG*/

    /* bus drivers in C */
#ifdef T5DEF_DDKC
    pDB[T5TB_DDKC].wSizeOf = sizeof (T5_BYTE);
    pDB[T5TB_DDKC].dwRawSize = T5DDKC_GetSize (pCode) + T5DEF_DDKCEXTRAMEM;
#endif /*T5DEF_DDKC*/

    return T5RET_OK;
}

/*****************************************************************************
T5VM_Build
Builds the data base. All memory is allocated.
Completes members of the header for On Line Change sizeong info
Initiates all the tables
(get all information from the application code)
fills members 'wSizeOf' and 'wNbItemAlloc' for each table
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer the DB
Return: ok or error
*****************************************************************************/

T5_RET T5VM_Build (T5_PTR pCode, T5PTR_DB pDB)
{
    return T5VM_BuildEx (pCode, pDB, TRUE, TRUE);
}

T5_RET T5VM_BuildEx (T5_PTR pCode, T5PTR_DB pDB,
                     T5_BOOL bIOs, T5_BOOL bBusDrv)
{
    T5_RET wRet;
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5_BOOL bDDKC, bDDKCPP;
    T5_UNUSED(bBusDrv);

    if (T5VMCode_CheckCode (pCode) != T5RET_OK)
        return T5RET_ERROR;

    bDDKC = bDDKCPP = FALSE;
    /* system resources: fixed and used always set to max */
    pDB[T5TB_SYSRSC].wNbItemFixed = pDB[T5TB_SYSRSC].wNbItemUsed
                                  = pDB[T5TB_SYSRSC].wNbItemAlloc;
    /* initiate status */
    pStatus = T5GET_DBSTATUS(pDB);
    wRet = T5VMCode_InitiateStatus (pCode, pStatus);
    pStatus->wMaxCallStack = T5VMCode_GetCallStackSize (pCode);
    pStatus->cCsvSep = ';';
    pStatus->cCsvDec = '.';
    pStatus->dwElapsed = pStatus->dwElapsedMS = 0L;

    if (wRet == T5RET_OK)
    {
        /* VM private data: keep pointer to code */
        pPrivate = T5GET_DBPRIVATE(pDB);
        pPrivate->pCode = pCode;
        pPrivate->pfStf = T5VMStf_Execute;
        pPrivate->dwFullBuild = T5VMCode_GetFullBuildStamp (pCode);
        pPrivate->dwSfcCrc = T5VMCode_GetSfcCrc (pCode);
        pPrivate->pTypeMasks = T5VMCode_GetVarTypeList (pCode); 
        pPrivate->dwVers = K5DBVERS;
        pPrivate->dwPassword = T5VMCode_GetPassword (pCode);
#ifdef T5DEF_CTSEG
        pPrivate->dwCTUsedSize = T5VMCode_GetCTSegUsedSize (pCode);
#endif /*T5DEF_CTSEG*/
#ifdef T5DEF_STRINGTABLE
        pPrivate->dwStrTable = T5VMCode_GetDefStringTable (pCode);
        pPrivate->pStrTable = T5VMCode_GetStringTable (pCode, pPrivate->dwStrTable);
#endif /*T5DEF_STRINGTABLE*/
        /* programs */
        wRet = T5VMCode_SetProgramPointers (pCode, pDB);
        if (wRet == T5RET_OK)
            wRet = T5VMCode_AllocHeap (pCode, pDB, FALSE);

    }
    /* variables */
    if (wRet == T5RET_OK)
        wRet = T5VMCode_GetData8Sizing (pCode, &(pDB[T5TB_DATA8]));
    if (wRet == T5RET_OK)
        wRet = T5VMCode_GetData16Sizing (pCode, &(pDB[T5TB_DATA16]));
    if (wRet == T5RET_OK)
        wRet = T5VMCode_GetData32Sizing (pCode, &(pDB[T5TB_DATA32]));
    if (wRet == T5RET_OK)
        wRet = T5VMCode_GetData64Sizing (pCode, &(pDB[T5TB_DATA64]));
    if (wRet == T5RET_OK)
        wRet = T5VMCode_GetTimeSizing (pCode, &(pDB[T5TB_TIME]));
    if (wRet == T5RET_OK)
        wRet = T5VMCode_GetStringSizing (pCode, &(pDB[T5TB_STRING]));
    /* string buffers */
    if (wRet == T5RET_OK)
        wRet = T5VMCode_SetStringPointers (pCode, pDB);
    if (wRet == T5RET_OK)
    {
        /* program status */
        T5VMPrg_ActivateAllPrograms (pDB);
        /* init value */
        T5VMCode_ApplyData8Init (pCode, &(pDB[T5TB_DATA8]), TRUE);
        T5VMCode_ApplyData16Init (pCode, &(pDB[T5TB_DATA16]), TRUE);
        T5VMCode_ApplyData32Init (pCode, &(pDB[T5TB_DATA32]), TRUE);
        T5VMCode_ApplyData64Init (pCode, &(pDB[T5TB_DATA64]), TRUE);
        T5VMCode_ApplyTimeInit (pCode, &(pDB[T5TB_TIME]), TRUE);
        T5VMCode_ApplyStringInit (pCode, pDB, TRUE);
    }

    /* CT segment */
#ifdef T5DEF_CTSEG
    if (wRet == T5RET_OK)
        wRet = T5VMCode_ApplyCTSegInit (pCode, pDB, 0L); 
#endif /*T5DEF_CTSEG*/

    /* embedded var map */
#ifdef T5DEF_VARMAP
    if (wRet == T5RET_OK)
        wRet = T5VMMap_Build (pDB, pCode);
#endif /*T5DEF_VARMAP*/

    /* FB classes and instances */
    if (wRet == T5RET_OK)
        wRet = T5VMFbl_Link (pCode, pDB);
    if (wRet == T5RET_OK)
        wRet = T5VMFbl_InitInstances (pCode, pDB, TRUE);
   
    /* Custom function classes */
    if (wRet == T5RET_OK)
        wRet = T5VMFbl_InitCustomFunctions (pCode, pDB);
    /* IOs */
    if ((wRet == T5RET_OK) && bIOs)
	{
        wRet = T5Ios_Open (T5VMCode_GetIODef (pCode),
                             pDB, T5GET_DBIODATA(pDB));
    }
    /* bus drivers */
#ifdef T5DEF_BUSDRV
    if ((wRet == T5RET_OK) && bBusDrv)
	{
        wRet = T5BusDrv_Build (pDB, pCode);
        if (wRet == T5RET_OK) bDDKCPP = TRUE;
    }
#endif /*T5DEF_BUSDRV*/
#ifdef T5DEF_DDKC
    if ((wRet == T5RET_OK) && bBusDrv)
	{
        wRet = T5DDKC_Build (pDB, pCode);
        if (wRet == T5RET_OK) bDDKC = TRUE;
    }
#endif /*T5DEF_DDKC*/

    /* RETAIN */
#ifdef T5DEF_RETAIN
    if (wRet == T5RET_OK)
        T5VMWarm_PrepareData (pCode, pDB);
#endif /*T5DEF_RETAIN*/

    /* perform private initializations */
    if (wRet == T5RET_OK)
	{
		T5VMTic_Init (pDB);
	}

    /* external variables */
#ifdef T5DEF_XV
    if (wRet == T5RET_OK)
        wRet = T5VMCode_GetXVSizing (pCode, &(pDB[T5TB_XV]));
    if (wRet == T5RET_OK)
        wRet = T5VMCode_SetXVPointers (pCode, pDB);
#endif /*T5DEF_XV*/

#ifdef T5DEF_BSAMPLING
    /* sampling trace: fixed and used always set to max */
    if (wRet == T5RET_OK)
        pDB[T5TB_BSAMPLING].wNbItemFixed = pDB[T5TB_BSAMPLING].wNbItemUsed
                                         = pDB[T5TB_BSAMPLING].wNbItemAlloc;
#endif /*T5DEF_BSAMPLING*/

    /* tasks */
    if (wRet == T5RET_OK)
        wRet = T5VMCode_BuildTaskList (pCode, pDB);

    /* call stack */
    if (wRet == T5RET_OK)
        pDB[T5TB_CALLSTACK].wNbItemFixed = pDB[T5TB_CALLSTACK].wNbItemUsed = 0;

    /* MODBUS */
#ifdef T5DEF_MODBUSIOS
    if (wRet == T5RET_OK)
        wRet = T5MB_Build (T5VMCode_MBClientConfig (pCode),
                           pDB[T5TB_MBC].pData, pDB[T5TB_MBC].dwRawSize);
    if (wRet == T5RET_OK)
        wRet = T5MB_Link (pDB[T5TB_MBC].pData, pDB);
#endif /*T5DEF_MODBUSIOS*/

    /* ASi I/Os */
#ifdef T5DEF_ASI
    if (wRet == T5RET_OK)
        wRet = T5VMAsi_Open ((T5PTR_ASIDEF)T5VMCode_AsiConfig (pCode),
                             (T5PTR_ASI)(pDB[T5TB_ASI].pData), pDB);
#endif /*T5DEF_ASI*/

#ifdef T5DEF_CANBUS
    if (wRet == T5RET_OK)
        wRet = T5VMCan_Build (pDB, pCode);
#endif /*T5DEF_CANBUS*/

    /* EA Change events and external variables */
#ifdef T5DEF_EA
    pDB[T5TB_EACHANGE].wNbItemUsed = 0;

    if (wRet == T5RET_OK)
        wRet = T5EAX_Build (pCode, pDB);
    if (wRet == T5RET_OK)
        wRet = T5EAX_Link (pDB);
#endif /*T5DEF_EA*/

#ifdef T5DEF_BUSDRV
    if (wRet != T5RET_OK && bDDKCPP == TRUE)
        T5BusDrv_Close (pDB);
#endif /*T5DEF_BUSDRV*/
#ifdef T5DEF_DDKC
    if (wRet != T5RET_OK && bDDKC == TRUE)
        T5DDKC_Close (pDB);
#endif /*T5DEF_DDKC*/

    return wRet;
}

/*****************************************************************************
T5VM_Cycle
Execute all the programs for one full cycle:
Parameters:
    pDB (IN) pointer the DB
    dwTimeStamp (IN) new cycle time stamp
Return: ok or error
*****************************************************************************/

T5_RET T5VM_Cycle (T5PTR_DB pDB, T5_DWORD dwTimeStamp)
{
    T5PTR_DB pTask;
    T5PTR_DBSTATUS pStatus;
    T5_WORD wTask;
    T5_BOOL bFirst;
    T5PTR_DBPRIVATE pPrivate;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_ERROR) != 0)
        return T5RET_ERROR;
    if ((pStatus->wFlags & T5FLAG_HNDMISS) != 0)
    {
        pStatus->wFlags &= ~T5FLAG_RUN;
        pStatus->wFlags |= T5FLAG_ERROR;
        return T5RET_ERROR;
    }

    bFirst = (pStatus->dwTimStamp == 0L);
    if (!bFirst)
    {
        pStatus->dwElapsedMS += T5_BOUNDTIME(dwTimeStamp - pStatus->dwTimStamp);
        if (pStatus->dwElapsedMS >= 1000L)
        {
            pStatus->dwElapsed += (pStatus->dwElapsedMS / 1000L);
            pStatus->dwElapsedMS %= 1000L;
        }
    }

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (bFirst)
    {
        pStatus->dwID = *(T5_PTDWORD)(pPrivate->pCode);
        pStatus->dwID >>= 4;
        pStatus->dwID &= 0x0fffffffL;
        pStatus->dwFirstStamp = dwTimeStamp;
#ifndef T5DEF_SIMUL
        if (
#ifndef T5OXC_STRICT
            pStatus->dwID != 0L && 
#endif /*T5OXC_STRICT*/
            pStatus->dwID != T5_OXC)
        {
            T5VMLog_Push (pDB, T5RET_WOXC, 0, 0L);
        }
#endif /*T5DEF_SIMUL*/
    }
#ifndef T5DEF_SIMUL
    else if (
#ifndef T5OXC_STRICT
        pStatus->dwID != 0L &&
#endif /*T5OXC_STRICT*/
        pStatus->dwID != T5_OXC
        && T5_BOUNDTIME(dwTimeStamp - pStatus->dwFirstStamp) > T5_TOXC
        && (pStatus->wFlags & T5FLAG_RUN) != 0)
    {
        T5VMLog_Push (pDB, T5RET_TOXC, 0, 0L);
        pStatus->wFlags &= ~T5FLAG_RUN;
        pStatus->wFlags |= T5FLAG_ERROR;
        return FALSE;
    }
#endif /*T5DEF_SIMUL*/

    pStatus->dwTimStamp = dwTimeStamp;

#ifdef T5DEF_ACTIMESUPPORTED
    T5VMTmr_UpdateTimers (pDB);
#endif /*T5DEF_ACTIMESUPPORTED*/

    pTask = &(pDB[T5TB_TASK]);

    if (bFirst && T5VM_FindTask (pDB, (T5_PTCHAR)"@@B", &wTask) == T5RET_OK)
        T5VM_PerformTask (pDB, wTask, dwTimeStamp, FALSE);

    if (pTask->wNbItemUsed == 0)
        T5VMPrg_Execute (pDB);
    else
    {
        T5VMPrg_Task (pDB, (T5PTR_DBTASK)(pTask->pData), TRUE);
        T5VMPrg_ProcessExecBuffer (pDB);
    }

    /* compact VM heap */
    if (pPrivate->pHeap != NULL)
        T5Heap_Compact (pPrivate->pHeap, 32);

    return T5RET_OK;
}

/*****************************************************************************
T5VM_PerformTask
Execute all the programs of the specified task
Parameters:
    pDB (IN) pointer the DB
    wTask (IN) index of the task
    dwTimeStamp (IN) new cycle time stamp if not 0
    bUpdateAcTimers (IN) option: update all running active timers
Return: ok or error
*****************************************************************************/

T5_RET T5VM_PerformTask (T5PTR_DB pDB, T5_WORD wTask,
                         T5_DWORD dwTimeStamp, T5_BOOL bUpdateAcTimers)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBTASK pTask;

    if (wTask > pDB[T5TB_TASK].wNbItemUsed)
        return T5RET_ERROR;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_ERROR) != 0)
        return T5RET_ERROR;
    if ((pStatus->wFlags & T5FLAG_HNDMISS) != 0)
    {
        pStatus->wFlags &= ~T5FLAG_RUN;
        pStatus->wFlags |= T5FLAG_ERROR;
        return T5RET_ERROR;
    }

    if (dwTimeStamp)
        pStatus->dwTimStamp = dwTimeStamp;

#ifdef T5DEF_ACTIMESUPPORTED
    if (bUpdateAcTimers)
        T5VMTmr_UpdateTimers (pDB);
#endif /*T5DEF_ACTIMESUPPORTED*/


    pTask = T5GET_DBTASK(pDB);
    T5VMPrg_Task (pDB, &(pTask[wTask]), FALSE);

    return T5RET_OK;
}

/*****************************************************************************
T5VM_FullIOExchange
Exhange all outputs then all inputs
Parameters:
    pDB (IN) pointer the DB
Return: ok or error
*****************************************************************************/

T5_RET T5VM_FullIOExchangeEx (T5PTR_DB pDB, T5_DWORD dwTimeStamp)
{
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_ERROR) != 0)
        return T5RET_ERROR;

    if ((pStatus->dwFlagsEx & T5FLAGEX_NOSIO) != 0
        && (pStatus->wFlags & T5FLAG_PROGSTEP) != 0)
        return T5RET_OK;

#ifdef T5DEF_CANBUS
    T5VMCan_Exchange (pDB, dwTimeStamp);
#endif /*T5DEF_CANBUS*/
#ifdef T5DEF_DDKC
    T5DDKC_Exchange (pDB, dwTimeStamp);
#endif /*T5DEF_DDKC*/

    return T5VM_FullIOExchange (pDB);
}

T5_RET T5VM_FullIOExchange (T5PTR_DB pDB)
{
    T5PTR_DBSTATUS pStatus;
    T5_WORD wRet;
    T5PTR_DBPRIVATE pPrivate;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_ERROR) != 0)
        return T5RET_ERROR;

    if ((pStatus->dwFlagsEx & T5FLAGEX_NOSIO) != 0
        && (pStatus->wFlags & T5FLAG_PROGSTEP) != 0)
        return T5RET_OK;

    if ((pStatus->wFlags & T5FLAG_HNDMISS) != 0)
        return T5RET_ERROR;

#ifdef T5DEF_ASI
    T5VMAsi_Exchange (T5GET_ASI(pDB));
#endif /*T5DEF_ASI*/

#ifdef T5DEF_BUSDRV
    T5BusDrv_Exchange (pDB);
#endif /*T5DEF_BUSDRV*/

    pPrivate = T5GET_DBPRIVATE(pDB);
    wRet = T5Ios_Exchange (T5VMCode_GetIODef (pPrivate->pCode),
                           pDB, T5GET_DBIODATA(pDB));

    /* keep alive FBs */
    T5VMFbl_KeepAlive (pDB);

#ifdef T5DEF_RETAIN
    T5VMWarm_CycleExchange (pDB);
#endif /*T5DEF_RETAIN*/

    return wRet;
}

/*****************************************************************************
T5VM_GetTriggerPtr
returns the pointer to the cycle trigger period
Parameters:
    pDB (IN) pointer the DB
Return: pointer to DWORD time (in microseconds)
*****************************************************************************/

T5_PTDWORD T5VM_GetTriggerPtr (T5PTR_DB pDB)
{
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    return &(pStatus->dwTimTrigger);
}

/*****************************************************************************
T5VM_ShutDown
closes the application
Parameters:
    pDB (IN) pointer the DB
*****************************************************************************/

void T5VM_ShutDown (T5PTR_DB pDB, T5_PTR pArgs)
{
    T5PTR_DBPRIVATE pPrivate;

#ifndef T5DEF_NOENDTASK
    T5PTR_DBSTATUS pStatus;
    T5_WORD wTask;

    pStatus = T5GET_DBSTATUS(pDB);
    if (T5VM_FindTask (pDB, (T5_PTCHAR)"@@E", &wTask) == T5RET_OK)
    {
        T5VM_PerformTask (pDB, wTask, pStatus->dwTimStamp, FALSE);
        T5VM_FullIOExchangeEx (pDB, pStatus->dwTimStamp);
    }
#endif /*T5DEF_NOENDTASK*/

    T5VMTic_Exit (pDB);

#ifdef T5DEF_RETAIN
    T5VMWarm_ShutDown (pDB, pArgs);
#endif /*T5DEF_RETAIN*/
    T5VMFbl_ShutDown (pDB);

#ifdef T5DEF_ASI
    T5VMAsi_Close (T5GET_ASI(pDB));
#endif /*T5DEF_ASI*/

#ifdef T5DEF_CANBUS
    T5VMCan_Close (pDB);
#endif /*T5DEF_CANBUS*/

#ifdef T5DEF_BUSDRV
    T5BusDrv_Close (pDB);
#endif /*T5DEF_BUSDRV*/
#ifdef T5DEF_DDKC
    T5DDKC_Close (pDB);
#endif /*T5DEF_DDKC*/
    
    pPrivate = T5GET_DBPRIVATE(pDB);
    T5Ios_Close (T5VMCode_GetIODef (pPrivate->pCode),
                 pDB, T5GET_DBIODATA(pDB));
    T5VMRsc_FreeAll (pDB);

    if (pPrivate->pHeap != NULL)
    {
        T5Heap_FreeAll (pPrivate->pHeap);
        T5Heap_DeleteHeap (pPrivate->pHeap);
    }
}

/*****************************************************************************
T5VM_GetTotalDBSize
returns the full size of the database (assumed as 1 block)
Parameters:
    pDB (IN) pointer the DB header
Return: full size in bytes
*****************************************************************************/

T5_DWORD T5VM_GetTotalDBSize (T5PTR_DB pDB)
{
    T5_DWORD dwSize, dwTab;
    T5_WORD i;

    dwSize = T5MAX_TABLE * sizeof (T5STR_DBTABLE);
	for (i=0; i<T5MAX_TABLE; i++)
	{
		dwTab = pDB[i].dwRawSize;
		if (dwTab == 0)
            dwTab = (T5_DWORD)(pDB[i].wNbItemAlloc) * (T5_DWORD)(pDB[i].wSizeOf);
		dwTab = T5_ALIGNED(dwTab);
		dwSize += dwTab;
	}
    return dwSize;
}

/*****************************************************************************
T5VM_BuildDBMainLinks
rebuild main links from the DB header to the DB tables
Parameters:
    pDB (IN/OUT) pointer the DB header
*****************************************************************************/

void T5VM_BuildDBMainLinks (T5PTR_DB pDB)
{
    T5_DWORD dwTab;
    T5_WORD i;
    T5_PTCHAR pData;

    pData = (T5_PTCHAR)pDB;
    pData += (T5MAX_TABLE * sizeof (T5STR_DBTABLE));

	for (i=0; i<T5MAX_TABLE; i++)
	{
		dwTab = pDB[i].dwRawSize;
		if (dwTab == 0)
            dwTab = (T5_DWORD)(pDB[i].wNbItemAlloc) * (T5_DWORD)(pDB[i].wSizeOf);
		dwTab = T5_ALIGNED(dwTab);
        pDB[i].pData = (T5_PTR)pData;
        pData += dwTab;
	}
}

/*****************************************************************************
T5VM_CanHOTStart
Check if for hot restart can be performed
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer the DB
Return: ok or error
*****************************************************************************/

T5_RET T5VM_CanHOTStart (T5_PTR pCode, T5PTR_DB pDB)
{
#ifndef T5DEF_HOTRESTART
    return T5RET_HOTUNSUP;
#else /*T5DEF_HOTRESTART*/

    T5PTR_DBSTATUS pStatus;
    T5PTR_DBPRIVATE pPrivate;
    T5_RET wRet;
    T5_DWORD dwFull;

    if (T5VMCode_CheckCode (pCode) != T5RET_OK)
        return T5RET_HOTCODE;

    pStatus = T5GET_DBSTATUS(pDB);
    pPrivate = T5GET_DBPRIVATE(pDB);

    /* check DB format */
    if (pPrivate->dwVers != K5DBVERS)
    {
        T5VMLog_Push (pDB, T5RET_HOTFORMAT, 0, 0);
        return T5RET_HOTFORMAT;
    }

    /* check compiler version */
    if (((T5PTR_CODEHEADER)pCode)->dwCmpVersion != pStatus->dwCmpVersion)
    {
        T5VMLog_Push (pDB, T5RET_HOTCMPVERS, 0, 0);
        return T5RET_HOTCMPVERS;
    }

    /* check app name - update versionning info */
    if (!T5VMCode_CanUpdateStatus (pCode, pStatus))
    {
        T5VMLog_Push (pDB, T5RET_ERROR, 0, 0);
        return T5RET_HOTAPPNAME;
    }
    /* check date stamp of last full build */
    dwFull = T5VMCode_GetFullBuildStamp (pCode);
    if (!dwFull || pPrivate->dwFullBuild != dwFull)
    {
        T5VMLog_Push (pDB, T5RET_HOTSTAMP, 0, 0);
        return T5RET_HOTSTAMP;
    }
    /* check call stack and breakpoints */
    if (T5VMPrg_HasBreapointOrStep (pDB))
    {
        if ((pStatus->wFlags & (T5FLAG_BKP|T5FLAG_PROGSTEP)) != 0)
        {
            T5VMLog_Push (pDB, T5RET_HOTSTEPPING, 0, 0);
            return T5RET_HOTSTEPPING;
        }
        T5VMLog_Push (pDB, T5RET_PRGWAITING, 0, 0);
        return T5RET_PRGWAITING;
    }
    /* check locked variables */
#ifdef T5DEF_SMARTLOCK
    if (T5VMLog_HasTempVarLocked (pDB))
    {
        T5VMLog_Push (pDB, T5RET_HOTTMPLOCKED, 0, 0);
        return T5RET_HOTTMPLOCKED;
    }
#endif /*T5DEF_SMARTLOCK*/

    /* check SFC charts */
    if (pPrivate->dwSfcCrc != T5VMCode_GetSfcCrc (pCode))
    {
        T5VMLog_Push (pDB, T5RET_HOTSFC, 0, 0);
        return T5RET_HOTSFC;
    }
    /* check and update data map */
    if (!T5VMHot_PrcVarMap (pCode, pDB, FALSE, &wRet))
    {
        T5VMLog_Push (pDB, wRet, 0, 0);
        return wRet;
    }
    /* system resources: error if at least one used */
    if (T5VMRsc_Exist (pDB))
    {
        T5VMLog_Push (pDB, T5RET_HOTRESOURCES, 0, 0);
        return T5RET_HOTRESOURCES;
    }
    /* nb active actimers */
    if (pDB[T5TB_ACTIME].wNbItemAlloc < T5VMCode_GetNbActiveTime (pCode))
    {
        T5VMLog_Push (pDB, T5RET_HOTACTIME, 0, 0);
        return T5RET_HOTACTIME;
    }

    /* IOs */
    wRet = T5Ios_CanHotRestart (T5VMCode_GetIODef (pCode),
                                pDB, T5GET_DBIODATA(pDB));
    if (wRet != T5RET_OK)
    {
        T5VMLog_Push (pDB, T5RET_HOTIOS, 0, 0);
        return T5RET_HOTIOS;
    }

    /* CT segment */
#ifdef T5DEF_CTSEG
    if (T5VMCode_GetCTSegUsedSize (pCode) > pDB[T5TB_CTSEG].dwRawSize)
    {
        T5VMLog_Push (pDB, T5RET_HOTMAPCT, 0, 0);
        return T5RET_HOTMAPCT;
    }
#endif /*T5DEF_CTSEG*/

    /* bus drivers */
#ifdef T5DEF_BUSDRV
    wRet = T5BusDrv_CanBuildHot (pDB, pCode);
    if (wRet != T5RET_OK)
    {
        T5VMLog_Push (pDB, T5RET_HOTBUSDRV, 0, 0);
        return T5RET_HOTBUSDRV;
    }
#endif /*T5DEF_BUSDRV*/
#ifdef T5DEF_DDKC
    wRet = T5DDKC_CanBuildHot (pDB, pCode);
    if (wRet != T5RET_OK)
    {
        T5VMLog_Push (pDB, T5RET_HOTBUSDRV, 0, 0);
        return T5RET_HOTBUSDRV;
    }
#endif /*T5DEF_DDKC*/

    /* retain */
#ifdef T5DEF_RETAIN
    wRet = T5VMWarm_CanUpdatePointers (pCode, pDB);
    if (wRet != T5RET_OK)
    {
        T5VMLog_Push (pDB, T5RET_HOTRETAIN, 0, 0);
        return T5RET_HOTRETAIN;
    }
#endif /*T5DEF_RETAIN*/
    /* embedded var map */
#ifdef T5DEF_VARMAP
    wRet = T5VMMap_CanHotRestart (pDB, pCode);
    if (wRet != T5RET_OK)
    {
        T5VMLog_Push (pDB, T5RET_HOTVARMAP, 0, 0);
        return T5RET_HOTVARMAP;
    }
#endif /*T5DEF_VARMAP*/

    /* task list */
    wRet = T5VMCode_CanHOTBuildTaskList (pCode, pDB);
    if (wRet != T5RET_OK)
    {
        T5VMLog_Push (pDB, T5RET_HOTTASKS, 0, 0);
        return T5RET_HOTTASKS;
    }

    /* call stack */
    if (T5VMCode_GetCallStackSize (pCode) > pDB[T5TB_CALLSTACK].wNbItemAlloc
        || T5VMCode_GetCallStackSize (pCode) > pStatus->wMaxCallStack)
    {
        T5VMLog_Push (pDB, T5RET_HOTCALLSTACK, 0, 0);
        return T5RET_HOTCALLSTACK;
    }

#ifdef T5DEF_MODBUSIOS
    wRet = T5MB_CanBuildHOT (T5VMCode_MBClientConfig (pCode), T5GET_MBC(pDB));
    if (wRet != T5RET_OK)
    {
        T5VMLog_Push (pDB, T5RET_HOTMB, 0, 0);
        return T5RET_HOTMB;
    }
#endif /*T5DEF_MODBUSIOS*/

#ifdef T5DEF_ASI
    wRet = T5VMAsi_CanHotStart ((T5PTR_ASIDEF)T5VMCode_AsiConfig (pCode), T5GET_ASI(pDB));
    if (wRet != T5RET_OK)
    {
        T5VMLog_Push (pDB, T5RET_HOTASI, 0, 0);
        return T5RET_HOTASI;
    }
#endif /*T5DEF_ASI*/

#ifdef T5DEF_CANBUS
    wRet = T5VMCan_CanBuildHot (pDB, pCode);
    if (wRet != T5RET_OK)
    {
        T5VMLog_Push (pDB, T5RET_HOTCANBUS, 0, 0);
        return T5RET_HOTCANBUS;
    }
#endif /*T5DEF_CANBUS*/

#ifdef T5DEF_EA
    wRet = T5EAX_CanBuildHOT (pCode, pDB);
    if (wRet != T5RET_OK)
    {
        T5VMLog_Push (pDB, T5RET_HOTEA, 0, 0);
        return T5RET_HOTEA;
    }
#endif /*T5DEF_EA*/

    return T5RET_OK;

#endif /*T5DEF_HOTRESTART*/
}

/*****************************************************************************
T5VM_BuildHOT
ReBuilds the data base for hot restart or on line change
keep data - rebuild links to code
(get all information from the application code)
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer the DB
Return: ok or error
*****************************************************************************/

T5_RET T5VM_BuildHOT (T5_PTR pCode, T5PTR_DB pDB, T5_DWORD dwTimeStamp)
{
#ifndef T5DEF_HOTRESTART
    return T5RET_ERROR;
#else /*T5DEF_HOTRESTART*/

    T5_RET wRet;
    T5PTR_DBPRIVATE pPrivate;
    T5_WORD wNbInstBefore;
    T5PTR_DBSTATUS pStatus;
    T5_DWORD dwFull;
    T5_WORD wf8, wf16, wf32, wf64, wfT, wfS;
    T5_WORD wnf8, wnf16, wnf32, wnf64, wnfT, wnfS;

    if (T5VMCode_CheckCode (pCode) != T5RET_OK)
        return T5RET_ERROR;

    pStatus = T5GET_DBSTATUS(pDB);
    pPrivate = T5GET_DBPRIVATE(pDB);

    pStatus->wCurPrg = 0;
    pStatus->wCurPos = 0;
    pStatus->wCurTag = 0;

    T5_MEMSET (pStatus->bkp, 0, sizeof (pStatus->bkp));
    pStatus->wFlags &= ~T5FLAG_BKP;

    pStatus->dwTimStamp = dwTimeStamp;
    wNbInstBefore = pDB[T5TB_FBINST].wNbItemUsed;

    /* check DB format */
    if (pPrivate->dwVers != K5DBVERS)
        return T5RET_HOTFORMAT;

    /* check compiler version */
    if (((T5PTR_CODEHEADER)pCode)->dwCmpVersion != pStatus->dwCmpVersion)
    {
        T5VMLog_Push (pDB, T5RET_HOTCMPVERS, 0, 0);
        return T5RET_HOTCMPVERS;
    }

    /* store old fixed limits */
    wf8 = pDB[T5TB_DATA8].wNbItemFixed;
    wf16 = pDB[T5TB_DATA16].wNbItemFixed;
    wf32 = pDB[T5TB_DATA32].wNbItemFixed;
    wf64 = pDB[T5TB_DATA64].wNbItemFixed;
    wfT = pDB[T5TB_TIME].wNbItemFixed;
    wfS = pDB[T5TB_STRING].wNbItemFixed;

    /* check app name - update versionning info */
    wRet = T5VMCode_UpdateStatus (pCode, pStatus);
    if (wRet != T5RET_OK) return wRet;

    /* check date stamp of last full build */
    dwFull = T5VMCode_GetFullBuildStamp (pCode);
    if (!dwFull || pPrivate->dwFullBuild != dwFull)
        return T5RET_HOTSTAMP;

    /* check call stack and breakpoints */
    if (T5VMPrg_HasBreapointOrStep (pDB))
    {
        if ((pStatus->wFlags & (T5FLAG_BKP|T5FLAG_PROGSTEP)) != 0)
            return T5RET_HOTSTEPPING;
        return T5RET_PRGWAITING;
    }

    /* check locked variables */
#ifdef T5DEF_SMARTLOCK
    if (T5VMLog_HasTempVarLocked (pDB))
    {
        T5VMLog_Push (pDB, T5RET_HOTTMPLOCKED, 0, 0);
        return T5RET_HOTTMPLOCKED;
    }
#endif /*T5DEF_SMARTLOCK*/

    /* check SFC charts */
    if (pPrivate->dwSfcCrc != T5VMCode_GetSfcCrc (pCode))
        return T5RET_HOTSFC;

    /* check and update data map */
    if (!T5VMHot_PrcVarMap (pCode, pDB, FALSE, &wRet))
        return wRet;

    /* system resources: error if at least one used */
    if (T5VMRsc_Exist (pDB))
        return T5RET_HOTRESOURCES;

    /* nb active actimers */
    if (pDB[T5TB_ACTIME].wNbItemAlloc < T5VMCode_GetNbActiveTime (pCode))
        return T5RET_HOTACTIME;

    /* VM private data: rebuild links and actuate password */
    pPrivate->pCode = pCode;
    pPrivate->pfStf = T5VMStf_Execute;
    pPrivate->pTypeMasks = T5VMCode_GetVarTypeList (pCode); 
    pPrivate->dwPassword = T5VMCode_GetPassword (pCode);
#ifdef T5DEF_STRINGTABLE
    pPrivate->pStrTable = T5VMCode_GetStringTable (pCode, pPrivate->dwStrTable);
#endif /*T5DEF_STRINGTABLE*/

    /* programs */
    wRet = T5VMCode_SetProgramPointers (pCode, pDB);
    if (wRet != T5RET_OK)
        return wRet;
    /* start new programs */
    T5VMPrg_ActivateNewPrograms (pDB, pCode);

    /* heap: re-allocated for hot restart only */
    if ((pStatus->dwFlagsEx & T5FLAGEX_CHGCYC) == 0L)
    {
        wRet = T5VMCode_AllocHeap (pCode, pDB, TRUE);
        if (wRet != T5RET_OK)
            return wRet;
    }

    /* call stack */
    if (T5VMCode_GetCallStackSize (pCode) > pDB[T5TB_CALLSTACK].wNbItemAlloc
        || T5VMCode_GetCallStackSize (pCode) > pStatus->wMaxCallStack)
        return T5RET_HOTCALLSTACK;

    /* string buffers */
    wRet = T5VMCode_SetStringPointers (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;

    /* embedded var map */
#ifdef T5DEF_VARMAP
    wRet = T5VMMap_HotRestart (pDB, pCode);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_VARMAP*/

    /* FB classes and instances - dont init instances */
#ifndef T5DEF_OLC_NORELINK
    wRet = T5VMFbl_Link (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /* T5DEF_OLC_NORELINK */

    wRet = T5VMFbl_InitInstances (pCode, pDB, FALSE);
    if (wRet != T5RET_OK) return wRet;

    T5VMFbl_InitNewInstances (pDB, wNbInstBefore);

    /* Custom function classes */
#ifndef T5DEF_OLC_NORELINK
    wRet = T5VMFbl_InitCustomFunctions (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /* T5DEF_OLC_NORELINK */

    /* update running timers */
    T5VMTmr_HotRestartTimers (pDB);

	/* perform private initializations */
    T5VMTic_Init (pDB);

    /* reset new allocated variables */
    T5VMHot_PrcVarMap (pCode, pDB, TRUE, &wRet);

    /* init value - only non fixed variables */
    wnf8 = pDB[T5TB_DATA8].wNbItemFixed;
    wnf16 = pDB[T5TB_DATA16].wNbItemFixed;
    wnf32 = pDB[T5TB_DATA32].wNbItemFixed;
    wnf64 = pDB[T5TB_DATA64].wNbItemFixed;
    wnfT = pDB[T5TB_TIME].wNbItemFixed;
    wnfS = pDB[T5TB_STRING].wNbItemFixed;

    pDB[T5TB_DATA8].wNbItemFixed = wf8;
    pDB[T5TB_DATA16].wNbItemFixed = wf16;
    pDB[T5TB_DATA32].wNbItemFixed = wf32;
    pDB[T5TB_DATA64].wNbItemFixed = wf64;
    pDB[T5TB_TIME].wNbItemFixed = wfT;
    pDB[T5TB_STRING].wNbItemFixed = wfS;

    T5VMCode_ApplyData8Init (pCode, &(pDB[T5TB_DATA8]), FALSE);
    T5VMCode_ApplyData16Init (pCode, &(pDB[T5TB_DATA16]), FALSE);
    T5VMCode_ApplyData32Init (pCode, &(pDB[T5TB_DATA32]), FALSE);
    T5VMCode_ApplyData64Init (pCode, &(pDB[T5TB_DATA64]), FALSE);
    T5VMCode_ApplyTimeInit (pCode, &(pDB[T5TB_TIME]), FALSE);
    T5VMCode_ApplyStringInit (pCode, pDB, FALSE);

    pDB[T5TB_DATA8].wNbItemFixed = wnf8;
    pDB[T5TB_DATA16].wNbItemFixed = wnf16;
    pDB[T5TB_DATA32].wNbItemFixed = wnf32;
    pDB[T5TB_DATA64].wNbItemFixed = wnf64;
    pDB[T5TB_TIME].wNbItemFixed = wnfT;
    pDB[T5TB_STRING].wNbItemFixed = wnfS;

    wRet = T5Ios_HotRestart (T5VMCode_GetIODef (pCode),
                             pDB, T5GET_DBIODATA(pDB));
    if (wRet != T5RET_OK) return wRet;

    /* CT segment */
#ifdef T5DEF_CTSEG
    T5VMCode_ApplyCTSegInit (pCode, pDB, pPrivate->dwCTUsedSize);
    pPrivate->dwCTUsedSize = T5VMCode_GetCTSegUsedSize (pCode);
#endif /*T5DEF_CTSEG*/

    /* retain */
#ifdef T5DEF_RETAIN
    wRet = T5VMWarm_UpdatePointers (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_RETAIN*/

    /* external variables */
#ifdef T5DEF_XV
    wRet = T5VMCode_SetXVPointers (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_XV*/

    /* tasks */
    wRet = T5VMCode_BuildTaskList (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;

#ifdef T5DEF_MODBUSIOS
    wRet = T5MB_BuildHOT (T5VMCode_MBClientConfig (pCode), T5GET_MBC(pDB));
    if (wRet != T5RET_OK) return wRet;
    wRet = T5MB_Link (pDB[T5TB_MBC].pData, pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_MODBUSIOS*/

#ifdef T5DEF_ASI
    wRet = T5VMAsi_HotStart ((T5PTR_ASIDEF)T5VMCode_AsiConfig (pCode),
                             T5GET_ASI(pDB), pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_ASI*/

#ifdef T5DEF_CANBUS
    wRet = T5VMCan_BuildHot (pDB, pCode);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_CANBUS*/

#ifndef T5DEF_BUILDHOT_SKIPEA
    /* EA Change events - RESET ON HOT START */
#ifdef T5DEF_EA
    pDB[T5TB_EACHANGE].wNbItemUsed = 0;
    T5_MEMSET (pDB[T5TB_EACHANGE].pData, 0,
           (pDB[T5TB_EACHANGE].wSizeOf * pDB[T5TB_EACHANGE].wNbItemAlloc));
#endif /*T5DEF_EA*/

    /* EA external variables */
#ifdef T5DEF_EA
    wRet = T5EAX_BuildHOT (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;
    wRet = T5EAX_Link (pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_EA*/
#endif /*T5DEF_BUILDHOT_SKIPEA*/

    /* bus drivers */
#ifdef T5DEF_BUSDRV
    wRet = T5BusDrv_BuildHot (pDB, pCode);
    if (wRet != T5RET_OK)
        return wRet;
#endif /*T5DEF_BUSDRV*/
#ifdef T5DEF_DDKC
    wRet = T5DDKC_BuildHot (pDB, pCode);
    if (wRet != T5RET_OK)
    {
#ifdef T5DEF_BUSDRV
        T5BusDrv_Close (pDB);
#endif /*T5DEF_BUSDRV*/
        return wRet;
    }
#endif /*T5DEF_DDKC*/

    return T5RET_OK;

#endif /*T5DEF_HOTRESTART*/
}

/*****************************************************************************
T5VM_UndoHOTChange
ReBuilds the data base for an undo of an on line change
Parameters:
    pCode (IN) pointer to the previous application code
    pDB (IN) pointer the DB
Return: ok or error
*****************************************************************************/

T5_RET T5VM_UndoHOTChange (T5_PTR pCode, T5PTR_DB pDB, T5_DWORD dwTimeStamp)
{
#ifndef T5DEF_HOTRESTART
    return T5RET_ERROR;
#else /*T5DEF_HOTRESTART*/

    T5_RET wRet;
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5_DWORD dwFull;

    pStatus = T5GET_DBSTATUS(pDB);
    pPrivate = T5GET_DBPRIVATE(pDB);

    if (T5VMCode_CheckCode (pCode) != T5RET_OK)
        return T5RET_ERROR;

    /* check call stack and breakpoints */
    if (T5VMPrg_HasBreapointOrStep (pDB))
    {
        if ((pStatus->wFlags & (T5FLAG_BKP|T5FLAG_PROGSTEP)) != 0)
            return T5RET_HOTSTEPPING;
        return T5RET_PRGWAITING;
    }

    pStatus->wCurPrg = 0;
    pStatus->wCurPos = 0;
    pStatus->wCurTag = 0;

    T5_MEMSET (pStatus->bkp, 0, sizeof (pStatus->bkp));
    pStatus->wFlags &= ~T5FLAG_BKP;

    pStatus->dwTimStamp = dwTimeStamp;

    /* check DB format */
    if (pPrivate->dwVers != K5DBVERS)
        return T5RET_HOTFORMAT;

    /* check app name - update versionning info */
    wRet = T5VMCode_UpdateStatus (pCode, pStatus);
    if (wRet != T5RET_OK) return wRet;

    /* check date stamp of last full build */
    dwFull = T5VMCode_GetFullBuildStamp (pCode);
    if (!dwFull || pPrivate->dwFullBuild != dwFull)
        return T5RET_HOTSTAMP;

    /* check SFC charts */
    if (pPrivate->dwSfcCrc != T5VMCode_GetSfcCrc (pCode))
        return T5RET_HOTSFC;

    /* check and update data map */
    if (!T5VMHot_UndoVarMap (pCode, pDB))
        return wRet;

    /* VM private data: rebuild links and actuate password */
    pPrivate->pCode = pCode;
    pPrivate->pfStf = T5VMStf_Execute;
    pPrivate->pTypeMasks = T5VMCode_GetVarTypeList (pCode); 
    pPrivate->dwPassword = T5VMCode_GetPassword (pCode);
#ifdef T5DEF_STRINGTABLE
    pPrivate->pStrTable = T5VMCode_GetStringTable (pCode, pPrivate->dwStrTable);
#endif /*T5DEF_STRINGTABLE*/

    /* programs */
    wRet = T5VMCode_SetProgramPointers (pCode, pDB);
    if (wRet != T5RET_OK)
        return wRet;
    T5VMPrg_ActivateNewPrograms (pDB, pCode);

    /* string buffers */
    wRet = T5VMCode_SetStringPointers (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;

    /* embedded var map */
#ifdef T5DEF_VARMAP
    wRet = T5VMMap_HotRestart (pDB, pCode);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_VARMAP*/

	/* perform private initializations */
    T5VMTic_Init (pDB);

    /* init value - only non fixed variables */
    T5VMCode_ApplyData8Init (pCode, &(pDB[T5TB_DATA8]), FALSE);
    T5VMCode_ApplyData16Init (pCode, &(pDB[T5TB_DATA16]), FALSE);
    T5VMCode_ApplyData32Init (pCode, &(pDB[T5TB_DATA32]), FALSE);
    T5VMCode_ApplyData64Init (pCode, &(pDB[T5TB_DATA64]), FALSE);
    T5VMCode_ApplyTimeInit (pCode, &(pDB[T5TB_TIME]), FALSE);
    T5VMCode_ApplyStringInit (pCode, pDB, FALSE);

    /* IOs */
    wRet = T5Ios_HotRestart (T5VMCode_GetIODef (pCode),
                             pDB, T5GET_DBIODATA(pDB));
    if (wRet != T5RET_OK) return wRet;

    /* CT segment */
#ifdef T5DEF_CTSEG
    pPrivate->dwCTUsedSize = T5VMCode_GetCTSegUsedSize (pCode);
#endif /*T5DEF_CTSEG*/

    /* retain */
#ifdef T5DEF_RETAIN
    wRet = T5VMWarm_UpdatePointers (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_RETAIN*/

    /* external variables */
#ifdef T5DEF_XV
    wRet = T5VMCode_SetXVPointers (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_XV*/

    /* tasks */
    wRet = T5VMCode_BuildTaskList (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;

#ifdef T5DEF_MODBUSIOS
    wRet = T5MB_BuildHOT (T5VMCode_MBClientConfig (pCode), T5GET_MBC(pDB));
    if (wRet != T5RET_OK) return wRet;
    wRet = T5MB_Link (pDB[T5TB_MBC].pData, pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_MODBUSIOS*/

#ifdef T5DEF_ASI
    wRet = T5VMAsi_HotStart ((T5PTR_ASIDEF)T5VMCode_AsiConfig (pCode),
                             T5GET_ASI(pDB), pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_ASI*/

#ifdef T5DEF_CANBUS
    wRet = T5VMCan_BuildHot (pDB, pCode);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_CANBUS*/

    /* EA Change events - RESET ON HOT START */
#ifdef T5DEF_EA
    pDB[T5TB_EACHANGE].wNbItemUsed = 0;
    T5_MEMSET (pDB[T5TB_EACHANGE].pData, 0,
           (pDB[T5TB_EACHANGE].wSizeOf * pDB[T5TB_EACHANGE].wNbItemAlloc));
#endif /*T5DEF_EA*/

    /* EA external variables */
#ifdef T5DEF_EA
    wRet = T5EAX_BuildHOT (pCode, pDB);
    if (wRet != T5RET_OK) return wRet;
    wRet = T5EAX_Link (pDB);
    if (wRet != T5RET_OK) return wRet;
#endif /*T5DEF_EA*/

    /* bus drivers */
#ifdef T5DEF_BUSDRV
    wRet = T5BusDrv_BuildHot (pDB, pCode);
    if (wRet != T5RET_OK)
        return wRet;
#endif /*T5DEF_BUSDRV*/
#ifdef T5DEF_DDKC
    wRet = T5DDKC_BuildHot (pDB, pCode);
    if (wRet != T5RET_OK)
    {
#ifdef T5DEF_BUSDRV
        T5BusDrv_Close (pDB);
#endif /*T5DEF_BUSDRV*/
        return wRet;
    }
#endif /*T5DEF_DDKC*/

    /* prevent from any further change */
    pPrivate->dwFullBuild = 0L;

    return T5RET_OK;

#endif /*T5DEF_HOTRESTART*/
}

/*****************************************************************************
K5VM_SetPauseMode
Force the single stepping mode
Parameters:
    pDB (IN) pointer the DB
    bPause (IN) new state of the single stepping flag
*****************************************************************************/

void T5VM_SetPauseMode (T5PTR_DB pDB, T5_BOOL bPause)
{
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if (bPause) pStatus->wFlags &= ~T5FLAG_RUN;
    else pStatus->wFlags |= T5FLAG_RUN;
}

/*****************************************************************************
T5VM_PrepareRetains
Prepare management RETAIN variables
Parameters:
    pDB (IN) pointer the DB
    pArgs (IN) configuration string (from main caller)
Return: OK or error
*****************************************************************************/

void T5VM_PrepareRetains (T5PTR_DB pDB, T5_PTR pArgs)
{
#ifdef T5DEF_RETAIN
    T5VMWarm_CanStore (pDB, pArgs);
#endif /*T5DEF_RETAIN*/
}

/*****************************************************************************
T5VM_LoadRetains
Load RETAIN variables
Parameters:
    pDB (IN) pointer the DB
    pArgs (IN) configuration string (from main caller)
Return: OK or error
*****************************************************************************/

T5_RET T5VM_LoadRetains (T5PTR_DB pDB, T5_PTR pArgs)
{
#ifdef T5DEF_RETAIN
    return T5VMWarm_Load (pDB, pArgs);
#else /*T5DEF_RETAIN*/
    return T5RET_ERROR;
#endif /*T5DEF_RETAIN*/
}

/*****************************************************************************
T5VM_CanLoadRetains
Check if RETAIN variables can be loaded (valid support available)
Parameters:
    pDB (IN) pointer the DB
    pArgs (IN) configuration string (from main caller)
Return: TRUE if valid RETAIN data available
*****************************************************************************/

T5_BOOL T5VM_CanLoadRetains (T5_PTR pCode, T5_PTR pArgs)
{
#ifdef T5DEF_RETAIN
    return T5VMWarm_CanLoad (pCode, pArgs);
#else /*T5DEF_RETAIN*/
    return FALSE;
#endif /*T5DEF_RETAIN*/
}

/*****************************************************************************
T5VM_OpenDynaLinks
Open links with DLLs
Return: OEM defined value to be passed to T5VM_CloseDynaLinks
*****************************************************************************/

T5_DWORD T5VM_OpenDynaLinks (void)
{
    T5_DWORD dwRet=0L;

#ifdef T5DEF_BUSDRV
    T5BusDrv_OpenLinks ();
#endif /*T5DEF_BUSDRV*/

#ifdef T5DEF_DDKC
    T5DDKC_OpenLinks ();
#endif /*T5DEF_DDKC*/

#ifdef T5DEF_DLLBLOCKS
    dwRet = T5Bdll_OpenDynaLinks ();
#endif /*T5DEF_DLLBLOCKS*/

#ifdef T5DEF_DLLIOS
    dwRet |= T5IOdll_OpenDynaLinks ();
#endif /*T5DEF_DLLIOS*/

    return dwRet;
}

/*****************************************************************************
T5VM_CloseDynaLinks
Close all links with DLLs
Parameters:
    dwData (IN) OEM defined data returned by T5VM_OpenDynaLinks
*****************************************************************************/

void T5VM_CloseDynaLinks (T5_DWORD dwData)
{
    T5_UNUSED(dwData);
#ifdef T5DEF_DLLBLOCKS
    T5Bdll_CloseDynaLinks ();
#endif /*T5DEF_DLLBLOCKS*/

#ifdef T5DEF_DLLIOS
    T5IOdll_CloseDynaLinks ();
#endif /*T5DEF_DLLIOS*/

#ifdef T5DEF_BUSDRV
    T5BusDrv_CloseLinks ();
#endif /*T5DEF_BUSDRV*/

#ifdef T5DEF_DDKC
    T5DDKC_CloseLinks ();
#endif /*T5DEF_DDKC*/
}

/*****************************************************************************
T5VM_RegisterCycleTime
Register the main cycle time in the data base
Parameters:
    pDB (IN) pointer the DB
    dwDuration (IN) duration of the las cycle in MICROSECONDS
Return: time to wait before next normal cycle (microseconds)
*****************************************************************************/

T5_DWORD T5VM_RegisterCycleTime (T5PTR_DB pDB, T5_DWORD dwDuration)
{
    T5PTR_DBSTATUS pStatus;
    T5_DWORD dwWait;

    pStatus = T5GET_DBSTATUS(pDB);

    pStatus->dwTimCycle = dwDuration;
    if (dwDuration > pStatus->dwTimMax)
        pStatus->dwTimMax = dwDuration;
    if (pStatus->dwTimTrigger != 0L && dwDuration > pStatus->dwTimTrigger)
    {
        pStatus->dwOverflow += 1L;
        T5VMLog_Push (pDB, T5RET_CYCLEOVERFLOW, 0, 0L);
    }

    if (pStatus->dwTimTrigger == 0L)
        dwWait = 0L;
    else
    {
        while (dwDuration >= pStatus->dwTimTrigger)
            dwDuration -= pStatus->dwTimTrigger;
        dwWait = pStatus->dwTimTrigger - dwDuration;
    }
    return dwWait;
}

/*****************************************************************************
T5VM_GetNbTask
Get the number of defined task
Parameters:
    pDB (IN) pointer the DB
Return: number of tasks (can be 0 if old style compiler)
*****************************************************************************/

T5_WORD T5VM_GetNbTask (T5PTR_DB pDB)
{
    return pDB[T5TB_TASK].wNbItemUsed;
}

/*****************************************************************************
T5VM_GetTaskName
Get the named of a defined task
Parameters:
    pDB (IN) pointer the DB
    wTask (IN) index of the wished task
Return: task name or NULL if error
*****************************************************************************/

T5_PTCHAR T5VM_GetTaskName (T5PTR_DB pDB, T5_WORD wTask)
{
    T5PTR_DBTASK pTask;

    if (wTask > pDB[T5TB_TASK].wNbItemUsed)
        return T5_PTNULL;

    pTask = T5GET_DBTASK(pDB);
    return pTask[wTask].pName;
}

/*****************************************************************************
T5VM_FindTask
Finds the index of a task specified by its name
--> Cannot be used to find the main cycle!
Parameters:
    pDB (IN) pointer the DB
    sTaskName (IN) task name - MUST BE UPPERCASE!
    pwTaskNo (OUT) index of the found task or 0 if not found
Return: OK if task found
*****************************************************************************/

T5_RET T5VM_FindTask (T5PTR_DB pDB, T5_PTCHAR sTaskName, T5_PTWORD pwTaskNo)
{
    T5_WORD i;
    T5PTR_DBTASK pTask;

    if (sTaskName == T5_PTNULL)
    {
        if (pwTaskNo != T5_PTNULL)
            *pwTaskNo = 0;
        return T5RET_ERROR;
    }

    pTask = T5GET_DBTASK(pDB);
    for (i=1; i<pDB[T5TB_TASK].wNbItemUsed; i++)
    {
        if (T5_STRCMP(pTask[i].pName, sTaskName) == 0)
        {
            if (pwTaskNo != T5_PTNULL)
                *pwTaskNo = i;
            return T5RET_OK;
        }
    }

    if (pwTaskNo != T5_PTNULL)
        *pwTaskNo = 0;
    return T5RET_ERROR;
}

/*****************************************************************************
T5VM_OpenModbus
Open all MODBUS communication ports
Parameters:
    pDB (IN) pointer the DB
Return: OK or error
*****************************************************************************/

T5_RET T5VM_OpenModbus (T5PTR_DB pDB)
{
#ifdef T5DEF_MODBUSIOS
    return T5MB_Open (T5GET_MBC(pDB));
#else /*T5DEF_MODBUSIOS*/
    return T5RET_ERROR;
#endif /*T5DEF_MODBUSIOS*/
}

/*****************************************************************************
T5VM_CloseModbus
Close all MODBUS communication ports
Parameters:
    pDB (IN) pointer the DB
*****************************************************************************/

void T5VM_CloseModbus (T5PTR_DB pDB)
{
#ifdef T5DEF_MODBUSIOS
    T5MB_Close (T5GET_MBC(pDB));
#endif /*T5DEF_MODBUSIOS*/
}

/*****************************************************************************
T5VM_ActivateModbus
Operate exchanges on all open MODBUS communication ports
Parameters:
    pDB (IN) pointer the DB
*****************************************************************************/

void T5VM_ActivateModbus (T5PTR_DB pDB, T5_DWORD dwTimeStamp)
{
#ifdef T5DEF_MODBUSIOS
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_ERROR) != 0)
        return;
    T5MB_Activate (T5GET_MBC(pDB), dwTimeStamp);
#endif /*T5DEF_MODBUSIOS*/
}

/*****************************************************************************
T5VM_ExchangeModbusData
Exchange MODBUS data with VM database
Parameters:
    pDB (IN) pointer the DB
*****************************************************************************/

void T5VM_ExchangeModbusData (T5PTR_DB pDB)
{
#ifdef T5DEF_MODBUSIOS
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_ERROR) != 0)
        return;
    if ((pStatus->dwFlagsEx & T5FLAGEX_NOSIO) != 0
        && (pStatus->wFlags & T5FLAG_PROGSTEP) != 0)
        return;
    if ((pStatus->wFlags & T5FLAG_HNDMISS) != 0)
        return;
    T5MB_Exchange (T5GET_MBC(pDB), pDB);
#endif /*T5DEF_MODBUSIOS*/
}

void T5VM_ExchangeModbusDataServer (T5PTR_DB pDB)
{
#ifdef T5DEF_MODBUSIOS
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_ERROR) != 0)
        return;
    if ((pStatus->dwFlagsEx & T5FLAGEX_NOSIO) != 0
        && (pStatus->wFlags & T5FLAG_PROGSTEP) != 0)
        return;
    if ((pStatus->wFlags & T5FLAG_HNDMISS) != 0)
        return;
    T5MB_ExchangeServerOnly (T5GET_MBC(pDB), pDB);
#endif /*T5DEF_MODBUSIOS*/
}

void T5VM_ExchangeModbusDataClient (T5PTR_DB pDB)
{
#ifdef T5DEF_MODBUSIOS
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_ERROR) != 0)
        return;
    if ((pStatus->dwFlagsEx & T5FLAGEX_NOSIO) != 0
        && (pStatus->wFlags & T5FLAG_PROGSTEP) != 0)
        return;
    if ((pStatus->wFlags & T5FLAG_HNDMISS) != 0)
        return;
    T5MB_ExchangeClientOnly (T5GET_MBC(pDB), pDB);
#endif /*T5DEF_MODBUSIOS*/
}

/*****************************************************************************
T5VM_ServeModbus
Serve a MODBUS request
Parameters:
    pDB (IN) pointer the DB - CAN BE NULL IF NO APP LOADED!!!
    wRequest (IN) protocol: T5CSRQ_MODBUSRTU or T5CSRQ_MODBUSTCP
    pIn (IN) input frame
    wOut (IN) output frame
Return: size of the full out frame
*****************************************************************************/

T5_WORD T5VM_ServeModbus (T5PTR_DB pDB, T5_WORD wRequest,
                          T5_PTBYTE pIn, T5_PTBYTE pOut)
{
#ifdef T5DEF_MODBUSIOS
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_ERROR) != 0)
        return T5RET_ERROR;

    return T5MB_Serve (T5GET_MBC(pDB), pIn, pOut,
                       (T5_BOOL)(wRequest == T5CSRQ_MODBUSTCP));
#else /*T5DEF_MODBUSIOS*/
    return 0;
#endif /*T5DEF_MODBUSIOS*/
}

/*****************************************************************************
T5VM_OpenExtData
Open all communication ports used for external data binding
Parameters:
    pDB (IN) pointer the DB
Return: OK or error
*****************************************************************************/

T5_RET T5VM_OpenExtData (T5PTR_DB pDB)
{
#ifdef T5DEF_EA
    return T5EAX_Open (pDB);
#else /*T5DEF_EA*/
    return T5RET_ERROR;
#endif /*T5DEF_EA*/
}

/*****************************************************************************
T5VM_CloseExtData
Close all communication ports used for external data binding
Parameters:
    pDB (IN) pointer the DB
*****************************************************************************/

void T5VM_CloseExtData (T5PTR_DB pDB)
{
#ifdef T5DEF_EA
    T5EAX_Close (pDB);
#endif /*T5DEF_EA*/
}

/*****************************************************************************
T5VM_UpdateExtData
Handle all communication ports used for external data binding
Parameters:
    pDB (IN) pointer the DB
    dwTimeStamp (IN) current time stamp in milliseconds (used for timeouts)
*****************************************************************************/

void T5VM_UpdateExtData (T5PTR_DB pDB, T5_DWORD dwTimeStamp)
{
#ifdef T5DEF_EA
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_ERROR) != 0)
        return;

    if ((pStatus->wFlags & T5FLAG_HNDMISS) != 0)
        return;
    T5EAX_Activate (pDB, dwTimeStamp);
#endif /*T5DEF_EA*/
}

#ifdef T5DEF_REG

/*****************************************************************************
T5VM_GetReg
get registry object attached to the VM
Parameters:
    pDB (IN/OUT) pointer to a DB header.
Return: pointer to the registry
*****************************************************************************/

T5_PTR T5VM_GetReg (T5PTR_DB pDB)
{
    T5PTR_DBPRIVATE pPrivate;

    pPrivate = T5GET_DBPRIVATE(pDB);
    return pPrivate->pReg;
}

/*****************************************************************************
T5VM_SetReg
attach a registry object to the VM
Parameters:
    pDB (IN/OUT) pointer to a DB header.
    pReg (IN) pointer to the new registry object to attach
Return: pointer to the previously attached registry
*****************************************************************************/

T5_PTR T5VM_SetReg (T5PTR_DB pDB, T5_PTR pReg)
{
    T5PTR_DBPRIVATE pPrivate;
    T5_PTR pOld;

    pPrivate = T5GET_DBPRIVATE(pDB);
    pOld = pPrivate->pReg;
    pPrivate->pReg = pReg;
    return pOld;
}

#endif /*T5DEF_REG*/

#ifdef T5DEF_IT

/*****************************************************************************
T5VM_SignalEvent
Soft ITs: signal an event -> forces an interruption during the cycle
Parameters:
    pDB (IN/OUT) pointer to a DB header.
    wEvent (IN) event ID
*****************************************************************************/

void T5VM_SignalEvent (T5PTR_DB pDB, T5_WORD wEvent)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTWORD pHandlers;

    if (wEvent >= T5MAX_EVENT)
        return;

    pPrivate = T5GET_DBPRIVATE(pDB);
    pStatus = T5GET_DBSTATUS(pDB);
    pHandlers = T5VMCode_GetITs (pPrivate->pCode);
    if (pHandlers == NULL || pHandlers[wEvent] == 0)
        return;

    if ((pStatus->wFlags & T5FLAG_RUNMASK) != T5FLAG_RUN
        /*|| pDB[T5TB_CALLSTACK].wNbItemUsed != 0*/)
        return;

    pStatus->bEvSig[wEvent] = TRUE;
    pStatus->bSignalEvent = TRUE;
}

/*****************************************************************************
T5VM_CallEventHandler
Soft ITs: Call an event handler outside the cycle
Parameters:
    pDB (IN/OUT) pointer to a DB header.
    wEvent (IN) event ID
*****************************************************************************/

void T5VM_CallEventHandler (T5PTR_DB pDB, T5_WORD wEvent)
{
    T5VMPrg_CallEventHandler (pDB, wEvent);
}

#endif /*T5DEF_IT*/

/* eof **********************************************************************/
