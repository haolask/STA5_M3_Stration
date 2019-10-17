/*****************************************************************************
T5VMapi.h :  VM apis

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#ifndef _T5VMAPI_H_INCLUDED_
#define _T5VMAPI_H_INCLUDED_

/****************************************************************************/
/* return values */

#define T5RET_OK                0
#define T5RET_ERROR             1
#define T5RET_BADCODE           2    /* invalid app code - bad header */
#define T5RET_BADCODETARGET     3    /* invalid code - bad target id */
#define T5RET_BADCODEVERSION    4    /* invalid code - bad version */
#define T5RET_EXECSIZE          5    /* exec buffer overflow */
#define T5RET_BADREQUEST        6    /* unknown comm  request */
#define T5RET_SERVEREAD         7    /* cannot process read request */
#define T5RET_UNKNOWNTIC        8    /* unknocn tic code */
#define T5RET_DATA8OVERFLOW     9    /* too many DATA8 used */
#define T5RET_DATA16OVERFLOW    10   /* too many DATA16 used */
#define T5RET_DATA32OVERFLOW    11   /* too many DATA32 used */
#define T5RET_DIVZERO           12   /* division by 0 in TIC (32bit) */
#define T5RET_TIMEOVERFLOW      13   /* too many TIME variables */
#define T5RET_CANTACTIME        14   /* active timers not supported */
#define T5RET_SAFETICLOOP       15   /* awful loop in TIC (safe mode) */
#define T5RET_OUTOFTIC          16   /* TIC code ointer out of range */
#define T5RET_UNKNOWNSTF        17   /* not supported std function */
#define T5RET_LDIVZERO          18   /* division by 0 in TIC (64bit) */
#define T5RET_DATA64OVERFLOW    19   /* too many DATA64 used */
#define T5RET_STRINGOVERFLOW    20   /* too many strings */
#define T5RET_SDIVZERO          21   /* division by zero in TIC (8 bit) */
#define T5RET_WDIVZERO          22   /* division by zero in TIC (16 bit) */
#define T5RET_HOTRESOURCES      23   /* HOT restart: system resources used */
#define T5RET_HOTMAP            24   /* invalid var map in hot restart */
#define T5RET_HOTACTIME         25   /* change of active timer map */
#define T5RET_HOTSTAMP          26   /* bad build stamp for hot restart */
#define T5RET_HOTSFC            27   /* SFC changed - no hot restart! */
#define T5RET_HOTRETAIN         28   /* retain def has changed - no hot */
#define T5RET_HOTFORMAT         29   /* bad DB format - no hot restart! */
#define T5RET_CYCLEOVERFLOW     30   /* cycle time overflow */
#define T5RET_RETAINSTORING     31   /* cant store retain variables */
#define T5RET_ARRAYBOUND        32   /* array index out of bounds */
#define T5RET_RECURSE           33   /* recursive call of sub-program */
#define T5RET_XVOVERFLOW        34   /* too many external variables used */
#define T5RET_NOBSAMPLING       35   /* sampling trace not supported */
#define T5RET_TASKOVERFLOW      36   /* too many tasks */
#define T5RET_HOTCALLSTACK      37   /* call stack overflow (hot) */
#define T5RET_STACKOVER         38   /* stack overlow */
#define T5RET_HOTASI            39   /* cannot hot restart ASi */
#define T5RET_TOXC              40   /* OXC limit reached */
#define T5RET_WOXC              41   /* OXC info */
#define T5RET_HOTUNSUP          42   /* On Line Change not supported */
#define T5RET_HOTCODE           43   /* On Line Change: bad code */
#define T5RET_HOTAPPNAME        44   /* On Line Change: not the same app */
#define T5RET_HOTIOS            45   /* On Line Change: IO drivers */
#define T5RET_HOTMAPCT          46   /* On Line Change: CT seg changed */
#define T5RET_HOTBUSDRV         47   /* On Line Change: bus drivers */
#define T5RET_HOTVARMAP         48   /* On Line Change: var map / profiles */
#define T5RET_HOTTASKS          49   /* On Line Change: tasks */
#define T5RET_HOTMB             50   /* On Line Change: MODBUS */
#define T5RET_HOTCANBUS         51   /* On Line Change: CAN bus */
#define T5RET_HOTEA             52   /* On Line Change: event production */
#define T5RET_HOTALLOC8         53   /* On Line Change: change D8 alloc */
#define T5RET_HOTALLOC16        54   /* On Line Change: change D16 alloc */
#define T5RET_HOTALLOC32        55   /* On Line Change: change D32 alloc */
#define T5RET_HOTALLOC64        56   /* On Line Change: change D64 alloc */
#define T5RET_HOTALLOCT         57   /* On Line Change: change TMR alloc */
#define T5RET_HOTALLOCS         58   /* On Line Change: change STR alloc */
#define T5RET_HOTALLOCFBI       59   /* On Line Change: change FBI alloc */
#define T5RET_HOTALLOCCFC       60   /* On Line Change: change CFC alloc */
#define T5RET_HOTALLOCXV        61   /* On Line Change: change XV alloc */
#define T5RET_HOTFIXED8         153  /* On Line Change: change D8 fixed */
#define T5RET_HOTFIXED16        154  /* On Line Change: change D16 fixed */
#define T5RET_HOTFIXED32        155  /* On Line Change: change D32 fixed */
#define T5RET_HOTFIXED64        156  /* On Line Change: change D64 fixed */
#define T5RET_HOTFIXEDT         157  /* On Line Change: change TMR fixed */
#define T5RET_HOTFIXEDS         158  /* On Line Change: change STR fixed */
#define T5RET_HOTFIXEDFBI       159  /* On Line Change: change FBI fixed */
#define T5RET_HOTFIXEDCFC       160  /* On Line Change: change CFC fixed */
#define T5RET_HOTFIXEDXV        161  /* On Line Change: change XV fixed */
#define T5RET_HOTSTRBUF         62   /* On Line Change: STR buff size */
#define T5RET_HOTSTRLEN         63   /* On Line Change: STR length */
#define T5RET_HOTFBIBUF         64   /* On Line Change: FBI buffer */
#define T5RET_HOTFBI            65   /* On Line Change: FBI changed */
#define T5RET_HOTPRGSTYLE       66   /* On Line Change: Program syle */
#define T5RET_HOTPRGNB          67   /* On Line Change: nb programs */
#define T5RET_VIEWPOINT         68   /* View point trace message */
#define T5RET_NOAPP             69   /* no VMDB available */
#define T5RET_BADCRC            70   /* CRC mismatch */
#define T5RET_HOTHEAP           71   /* cannot relocate heap */
#define T5RET_HOTSTEPPING       72   /* On Line Change: step or breakpoint */
#define T5RET_PRGWAITING        73   /* On Line Change: program waiting */
#define T5RET_LOADVMDB          74   /* not enough RAM for VMDB */
#define T5RET_LOADCODE          75   /* cannot load application code */
#define T5RET_HOTCMPVERS        76   /* bad compiler version for on line change */
#define T5RET_HOTTMPLOCKED      77   /* some temp variables locked during on line change */

/****************************************************************************/
/* hooks */

#ifndef T5HOOK_SETPRGSUPPORTED
#define T5HOOK_SETPRG(wp) /* nothing */
#endif /*T5HOOK_SETPRGSUPPORTED*/


/****************************************************************************/
/* high level calls */

extern void   T5Def_Cycle (T5PTR_DB pDB, T5_DWORD dwTimeStamp,
                           T5_PTR pEvData);
extern T5_RET T5Def_ColdStart (T5_PTR pCode, T5PTR_DB pDB,
                               T5_PTR pRetData, T5_PTR pEvData,
                               T5_BOOL bWarm, T5_BOOL bCC,
                               T5_PTCHAR szAppName);
extern T5_RET T5Def_HotStart (T5_PTR pCode, T5PTR_DB pDB,
                              T5_PTR pRetData, T5_PTR pEvData,
                              T5_BOOL bCC, T5_PTCHAR szAppName,
                              T5_DWORD dwTimeStamp);
extern T5_RET T5Def_Change (T5_PTR pCode, T5PTR_DB pDB, T5_PTR pEvData,
                            T5_PTCHAR szAppName, T5_DWORD dwTimeStamp);
extern T5_RET T5Def_UndoChange (T5_PTR pCode, T5PTR_DB pDB, T5_PTR pEvData,
                                T5_PTCHAR szAppName, T5_DWORD dwTimeStamp);

extern void   T5Def_Stop (T5PTR_DB pDB, T5_PTR pEvData, T5_PTR pRetData,
                          T5_PTCHAR szAppName);
extern void   T5Def_ServeVMRequest (T5PTR_CS pCSData, T5PTR_DB pDB, T5_WORD wCaller);
extern void   T5Def_ServeVMRequestEx (T5PTR_CS pCSData, T5PTR_DB pDB, T5_WORD wCaller, T5_WORD wEvPort);
extern void   T5Def_ServeModbusRequest (T5PTR_CS pCSData, T5PTR_DB pDB,
                                        T5_WORD wReq, T5_WORD wCaller);
extern T5_RET T5Def_ServeSyncRequest (T5PTR_CS pCSData, T5PTR_DB pDB, T5_WORD wCaller);

/****************************************************************************/
/* VM main calls */

extern T5_RET     T5VM_GetDBSize (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET     T5VM_Build (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET     T5VM_BuildEx (T5_PTR pCode, T5PTR_DB pDB,
                                T5_BOOL bIOs, T5_BOOL bBusDrv);
extern T5_RET     T5VM_BuildHOT (T5_PTR pCode, T5PTR_DB pDB,
                                 T5_DWORD dwTimeStamp);
extern T5_RET     T5VM_UndoHOTChange (T5_PTR pCode, T5PTR_DB pDB,
                                      T5_DWORD dwTimeStamp);

extern T5_RET     T5VM_CanHOTStart (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET     T5VM_Cycle (T5PTR_DB pDB, T5_DWORD dwTimeStamp);
extern T5_RET     T5VM_FullIOExchange (T5PTR_DB pDB);
extern T5_RET     T5VM_FullIOExchangeEx (T5PTR_DB pDB, T5_DWORD dwTimeStamp);
extern T5_PTDWORD T5VM_GetTriggerPtr (T5PTR_DB pDB);
extern void       T5VM_ShutDown (T5PTR_DB pDB, T5_PTR pArgs);

extern T5_WORD    T5VM_Serve (T5PTR_DB pDB, T5_PTBYTE pIn, T5_PTBYTE pOut);
extern T5_WORD    T5VM_ServeEx (T5PTR_DB pDB, T5_PTBYTE pIn, T5_PTBYTE pOut, T5_WORD wEvsPort);
extern T5_WORD    T5VM_ReadObjects (T5PTR_DB pDB,
                                    T5_PTBYTE pAsk, T5_PTBYTE pReply);
extern T5_RET     T5VM_Execute (T5PTR_DB pDB, T5_PTBYTE pTic, T5_WORD wSize);

#ifdef T5DEF_EA

extern T5_WORD    T5VM_SrvSubscribe (T5PTR_DB pDB, T5_PTBYTE pAsk,
                                     T5_PTBYTE pReply);
extern T5_WORD    T5VM_SrvUnsubscribe (T5PTR_DB pDB, T5_PTBYTE pAsk);

#ifdef T5DEF_CTSEG
extern T5_WORD    T5VM_SrvSubscribeL (T5PTR_DB pDB, T5_PTBYTE pAsk,
                                      T5_PTBYTE pReply);
extern T5_WORD    T5VM_SrvUnsubscribeL (T5PTR_DB pDB, T5_PTBYTE pAsk);
#endif /*T5DEF_CTSEG*/

#endif /*T5DEF_EA*/

extern T5_WORD    T5VM_SrvPollTable (T5PTR_DB pDB, T5_PTBYTE pAsk,
                                     T5_PTBYTE pReply);


extern T5_DWORD   T5VM_GetTotalDBSize (T5PTR_DB pDB);
extern void       T5VM_BuildDBMainLinks (T5PTR_DB pDB);

extern void       T5VM_SetPauseMode (T5PTR_DB pDB, T5_BOOL bPause);

extern void       T5VM_PrepareRetains (T5PTR_DB pDB, T5_PTR pArgs);
extern T5_RET     T5VM_LoadRetains (T5PTR_DB pDB, T5_PTR pArgs);
extern T5_BOOL    T5VM_CanLoadRetains (T5_PTR pCode, T5_PTR pArgs);

extern T5_DWORD   T5VM_OpenDynaLinks (void);
extern void       T5VM_CloseDynaLinks (T5_DWORD dwData);
extern T5_DWORD   T5VM_RegisterCycleTime (T5PTR_DB pDB, T5_DWORD dwDuration);

extern T5_WORD    T5VM_GetNbTask (T5PTR_DB pDB);
extern T5_PTCHAR  T5VM_GetTaskName (T5PTR_DB pDB, T5_WORD wTask);
extern T5_RET     T5VM_PerformTask (T5PTR_DB pDB, T5_WORD wTask,
                                  T5_DWORD dwTimeStamp,
                                  T5_BOOL bUpdateAcTimers);
extern T5_RET     T5VM_FindTask (T5PTR_DB pDB, T5_PTCHAR sTaskName,
                                 T5_PTWORD pwTaskNo);

extern T5_RET     T5VM_OpenModbus (T5PTR_DB pDB);
extern void       T5VM_CloseModbus (T5PTR_DB pDB);
extern void       T5VM_ActivateModbus (T5PTR_DB pDB, T5_DWORD dwTimeStamp);
extern void       T5VM_ExchangeModbusData (T5PTR_DB pDB);
extern void       T5VM_ExchangeModbusDataServer (T5PTR_DB pDB);
extern void       T5VM_ExchangeModbusDataClient (T5PTR_DB pDB);
extern T5_WORD    T5VM_ServeModbus (T5PTR_DB pDB, T5_WORD wRequest,
                                    T5_PTBYTE pIn, T5_PTBYTE pOut);

extern T5_RET     T5VM_OpenExtData (T5PTR_DB pDB);
extern void       T5VM_CloseExtData (T5PTR_DB pDB);
extern void       T5VM_UpdateExtData (T5PTR_DB pDB, T5_DWORD dwTimeStamp);

#ifdef T5DEF_REG

extern T5_PTR     T5VM_GetReg (T5PTR_DB pDB);
extern T5_PTR     T5VM_SetReg (T5PTR_DB pDB, T5_PTR pReg);

#endif /*T5DEF_REG*/

#ifdef T5DEF_IT

extern void       T5VM_SignalEvent (T5PTR_DB pDB, T5_WORD wEvent);
extern void       T5VM_CallEventHandler (T5PTR_DB pDB, T5_WORD wEvent);

#endif /*T5DEF_IT*/


/****************************************************************************/
/* hot restart */

#ifdef T5DEF_HOTRESTART
extern T5_BOOL T5VMHot_PrcVarMap (T5_PTR pCode, T5PTR_DB pDB,
                                  T5_BOOL bResetNew, T5_RET *pRet);
extern T5_BOOL T5VMHot_UndoVarMap (T5_PTR pCode, T5PTR_DB pDB);
#endif /*T5DEF_HOTRESTART*/

/****************************************************************************/
/* access to the app code */

extern T5_DWORD T5VMCode_GetPassword (T5_PTR pCode);
extern T5_DWORD T5VMCode_GetFullBuildStamp (T5_PTR pCode);
extern T5_DWORD T5VMCode_GetSfcCrc (T5_PTR pCode);
extern T5_RET  T5VMCode_CheckCode (T5_PTR pCode);
extern T5_WORD T5VMCode_GetNbProgAlloc (T5_PTR pCode);
extern T5_WORD T5VMCode_GetNbProg (T5_PTR pCode);
extern T5_WORD T5VMCode_GetNbProgWithStack (T5_PTR pCode);
extern T5_WORD T5VMCode_GetCallStackSize (T5_PTR pCode);
extern T5_WORD T5VMCode_GetCallStackAllocSize (T5_PTR pCode);
extern T5_RET  T5VMCode_InitiateStatus (T5_PTR pCode, T5PTR_DBSTATUS pStatus);
extern T5_RET  T5VMCode_UpdateStatus (T5_PTR pCode, T5PTR_DBSTATUS pStatus);
extern T5_BOOL T5VMCode_CanUpdateStatus (T5_PTR pCode,
                                         T5PTR_DBSTATUS pStatus);
extern T5_RET  T5VMCode_SetProgramPointers (T5_PTR pCode, T5PTR_DB pDB);
extern T5_WORD T5VMCode_GetNbLockFlags (T5_PTR pCode);
extern T5_WORD T5VMCode_GetNbData8 (T5_PTR pCode);
extern T5_RET  T5VMCode_GetData8Sizing (T5_PTR pCode, T5PTR_DB pDef);
extern T5_RET  T5VMCode_ApplyData8Init (T5_PTR pCode, T5PTR_DB pDef,
                                        T5_BOOL bFixedAlso);
extern T5_WORD T5VMCode_GetNbData16 (T5_PTR pCode);
extern T5_RET  T5VMCode_GetData16Sizing (T5_PTR pCode, T5PTR_DB pDef);
extern T5_RET  T5VMCode_ApplyData16Init (T5_PTR pCode, T5PTR_DB pDef,
                                         T5_BOOL bFixedAlso);
extern T5_WORD T5VMCode_GetNbData32 (T5_PTR pCode);
extern T5_RET  T5VMCode_GetData32Sizing (T5_PTR pCode, T5PTR_DB pDef);
extern T5_RET  T5VMCode_ApplyData32Init (T5_PTR pCode, T5PTR_DB pDef,
                                         T5_BOOL bFixedAlso);
extern T5_WORD T5VMCode_GetNbData64 (T5_PTR pCode);
extern T5_RET  T5VMCode_GetData64Sizing (T5_PTR pCode, T5PTR_DB pDef);
extern T5_RET  T5VMCode_ApplyData64Init (T5_PTR pCode, T5PTR_DB pDef,
                                         T5_BOOL bFixedAlso);
extern T5_WORD T5VMCode_GetNbActiveTime (T5_PTR pCode);
extern T5_WORD T5VMCode_GetNbTime (T5_PTR pCode);
extern T5_RET  T5VMCode_GetTimeSizing (T5_PTR pCode, T5PTR_DB pDef);
extern T5_RET  T5VMCode_ApplyTimeInit (T5_PTR pCode, T5PTR_DB pDef,
                                       T5_BOOL bFixedAlso);
extern T5_WORD T5VMCode_GetNbFBInstance (T5_PTR pCode);
extern T5_PTR  T5VMCode_GetInstanceList (T5_PTR pCode);
extern T5_PTR  T5VMCode_GetClassList (T5_PTR pCode, T5_PTWORD pwNbClass);
extern T5_DWORD T5VMCode_GetWishedFBIRawSize (T5_PTR pCode);

extern T5_WORD T5VMCode_GetNbCustomFunc (T5_PTR pCode);
extern T5_PTR  T5VMCode_GetCustomFuncList (T5_PTR pCode, T5_PTWORD pwNbClass);

extern T5_WORD T5VMCode_GetNbString (T5_PTR pCode, T5_PTDWORD pdwRawSize);
extern T5_RET  T5VMCode_GetStringSizing (T5_PTR pCode, T5PTR_DB pDef);
extern T5_RET  T5VMCode_SetStringPointers (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET  T5VMCode_ApplyStringInit (T5_PTR pCode, T5PTR_DB pDef,
                                         T5_BOOL bFixedAlso);

extern T5_WORD T5VMCode_GetNbStep (T5_PTR pCode);
extern T5_WORD T5VMCode_GetNbTrans (T5_PTR pCode);
extern T5_PTBYTE T5VMCode_GetVarTypeList (T5_PTR pCode);

extern T5_PTR  T5VMCode_GetIODef (T5_PTR pCode);
extern T5_PTR  T5VMCode_GetPublishedList (T5_PTR pCode);
extern T5_PTCHAR T5VMCode_GetAppName (T5_PTR pCode);

extern T5_WORD T5VMCode_GetNbXV (T5_PTR pCode);
extern T5_RET  T5VMCode_GetXVSizing (T5_PTR pCode, T5PTR_DB pDef);
extern T5_RET  T5VMCode_SetXVPointers (T5_PTR pCode, T5PTR_DB pDB);

extern T5_RET  T5VMCode_BuildTaskList (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET  T5VMCode_CanHOTBuildTaskList (T5_PTR pCode, T5PTR_DB pDB);

extern T5_PTR  T5VMCode_MBClientConfig (T5_PTR pCode);
extern T5_PTR  T5VMCode_AsiConfig (T5_PTR pCode);
extern T5_PTR  T5VMCode_StaticEvents (T5_PTR pCode);
extern T5_BOOL T5VMCode_GetEAX (T5_PTR pCode, T5_PTR *ppCnx, T5_PTR *ppVar);

extern T5_PTR  T5VMCode_GetBusDrv (T5_PTR pCode);
extern T5_PTR T5VMCode_GetBusIOList (T5_PTR pCode);

extern T5_BOOL T5VMCode_GetNbIOs (T5_PTR pCode, T5_PTDWORD pdwNb);
extern T5_BOOL T5VMCode_IsCCPure (T5_PTR pCode);

extern T5_DWORD T5VMCode_GetHeapSize (T5_PTR pCode);
extern T5_RET   T5VMCode_AllocHeap (T5_PTR pCode, T5PTR_DB pDB, T5_BOOL bHot);

extern T5_PTWORD T5VMCode_GetITs (T5_PTR pCode);
extern T5_WORD T5VMCode_GetITCount (T5_PTR pCode);
extern T5_BOOL T5VMCode_IsCodeDebug (T5_PTR pCode);

extern T5_PTCHAR T5VMCode_GetInstanceNames (T5_PTR pCode);

#ifdef T5DEF_CTSEG

extern T5_PTR    T5VMCode_GetCTSegDef (T5_PTR pCode);
extern T5_PTCHAR T5VMCode_GetCTTypeDef (T5_PTR pCode);
extern T5_DWORD  T5VMCode_GetCTSegAllocSize (T5_PTR pCode);
extern T5_DWORD  T5VMCode_GetCTSegUsedSize (T5_PTR pCode);
extern T5_RET    T5VMCode_ApplyCTSegInit (T5_PTR pCode, T5PTR_DB pDB,
                                          T5_DWORD dwStartOffset);

#endif /*T5DEF_CTSEG*/

#ifdef T5DEF_RECIPES

T5PTR_CODERCPL T5VMCode_GetRecipes (T5_PTR pCode);

#endif /*T5DEF_RECIPES*/

#ifdef T5DEF_VLST

T5PTR_CODEVLSTL T5VMCode_GetVLsts (T5_PTR pCode);

#endif /*T5DEF_VLST*/

#ifdef T5DEF_STRINGTABLE

T5_DWORD T5VMCode_GetDefStringTable (T5_PTR pCode);
T5_PTR   T5VMCode_GetStringTable (T5_PTR pCode, T5_DWORD dwIndex);

#endif /*T5DEF_STRINGTABLE*/

#ifdef T5DEF_SIGNAL

T5_PTR   T5VMCode_GetSignal (T5_PTR pCode, T5_DWORD dwIndex);

#endif /*T5DEF_SIGNAL*/

#ifdef T5DEF_CANBUS

extern T5PTR_CANDEF T5VMCode_GetCanDef (T5_PTR pCode);

#endif /*T5DEF_CANBUS*/

/****************************************************************************/
/* function block handlers */

extern T5_RET   T5VMFbl_Link (T5_PTR pCode, T5PTR_DB pDB);
extern T5_WORD  T5VMFbl_GetNbClass (T5_PTR pCode);
extern T5_DWORD T5VMFbl_GetClassDataSize (T5_PTR pCode);
extern T5_DWORD T5VMFbl_GetInstanceDataSize (T5_PTR pCode);
extern T5_DWORD T5VMFbl_GetInstanceDataSizeExact (T5_PTR pCode);
extern T5_RET   T5VMFbl_InitInstances (T5_PTR pCode, T5PTR_DB pDB,
                                       T5_BOOL bInitInstance);
extern void     T5VMFbl_ShutDown (T5PTR_DB pDB);
extern T5_RET   T5VMFbl_InitCustomFunctions (T5_PTR pCode, T5PTR_DB pDB);
extern void     T5VMFbl_InitNewInstances (T5PTR_DB pDB, T5_WORD wFirst);
extern void     T5VMFbl_KeepAlive (T5PTR_DB pDB);

extern T5_WORD  T5VMFbl_GetInstanceCount (T5PTR_DB pDB, T5_PTCHAR szClass);

/****************************************************************************/
/* log messages and unresolved */

extern void      T5VMLog_Push (T5PTR_DB pDB, T5_WORD wCode,
                               T5_WORD wArg, T5_DWORD dwData);
extern T5_BOOL   T5VMLog_Pop (T5PTR_DB pDB, T5_PTWORD pwCode,
                              T5_PTWORD pwArg, T5_PTDWORD pdwData);

extern void      T5VMLog_PushSys (T5_WORD wCode);
extern T5_BOOL   T5VMLog_PopSys (T5_PTWORD pwCode);

extern void      T5VMLog_RegisterUnresolved (T5PTR_DB pDB, T5_PTCHAR szName);
extern T5_PTCHAR T5VMLog_GetUnresolved (T5PTR_DB pDB, T5_WORD wIndex);

#ifdef T5DEF_SMARTLOCK

extern void      T5VMLog_RegisterLockVar (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_DWORD dwCTOffset);
extern void      T5VMLog_UnRegisterLockVar (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_DWORD dwCTOffset);
extern T5_BOOL   T5VMLog_SetLockVar (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_DWORD dwCTOffset,
                                     T5_PTR pValue, T5_BOOL bReApply);
extern void      T5VMLog_UnRegisterAllLockVars (T5PTR_DB pDB);
extern T5_DWORD  T5VMLog_GetNbLocked (T5PTR_DB pDB);
extern T5_WORD   T5VMLog_GetLockedVariable (T5PTR_DB pDB, T5_WORD wIndex, T5_PTR pDest, T5_WORD wMaxLen);
extern T5_BOOL   T5VMLog_IsCTLocked (T5PTR_DB pDB, T5_WORD wType, T5_WORD wSrc, T5_WORD wRef);
extern T5_BOOL   T5VMLog_IsCTLockedRaw (T5PTR_DB pDB, T5_WORD wType, T5_WORD wSrc, T5_DWORD dwOffset);
extern T5_BOOL   T5VMLog_IsCTLockedOff (T5PTR_DB pDB, T5_DWORD dwOffset);
extern T5_BOOL   T5VMLog_HasTempVarLocked (T5PTR_DB pDB);

#else /*T5DEF_SMARTLOCK*/

#define T5VMLog_RegisterLockVar(pDB,wType,wIndex,dwCTOffset) /*nothing*/
#define T5VMLog_UnRegisterLockVar(pDB,wType,wIndex,dwCTOffset,bReApply) /*nothing*/
#define T5VMLog_SetLockVar(pDB,wType,wIndex,dwCTOffset,pValue) /*nothing*/
#define T5VMLog_UnRegisterAllLockVars(pDB) /*nothing*/
#define T5VMLog_GetNbLocked(pDB) 0L
#define T5VMLog_GetLockedVariable(pDB,wIndex,pDest,wMaxLen) FALSE
#define T5VMLog_IsCTLocked(pDB,wType,wSrc,wRef) FALSE
#define T5VMLog_IsCTLockedRaw(pDB,wType,wSrc,dwOffset) FALSE
#define T5VMLog_IsCTLockedOff(pDB,dwOffset) FALSE

#endif /*T5DEF_SMARTLOCK*/

/****************************************************************************/
/* programs */

extern void    T5VMPrg_Execute (T5PTR_DB pDB);
extern void    T5VMPrg_ActivateAllPrograms (T5PTR_DB pDB);
extern void    T5VMPrg_ActivateNewPrograms (T5PTR_DB pDB, T5_PTR pCode);
extern T5_BOOL T5VMPrg_Call (T5PTR_DB pDB, T5_WORD wIndex);
extern void    T5VMPrg_Task (T5PTR_DB pDB, T5PTR_DBTASK pTask, T5_BOOL bMain);
extern void    T5VMPrg_ProcessExecBuffer (T5PTR_DB pDB);
extern void    T5VMPrg_SetBreakpoint (T5PTR_DB pDB, T5_WORD wProg,
                                      T5_WORD wPos, T5_WORD wStyle);
extern T5_BOOL T5VMPrg_IsBreakpoint (T5PTR_DB pDB, T5_WORD wProg,
                                     T5_WORD wPos, T5_WORD wID);

extern void    T5VMPrg_SaveCallStack (T5PTR_DB pDB, T5_WORD wProg);
extern void    T5VMPrg_RestoreCallStack (T5PTR_DB pDB, T5_WORD wProg);
extern T5_BOOL T5VMPrg_HasBreapointOrStep (T5PTR_DB pDB);

#ifdef T5DEF_IT
extern void    T5VMPrg_BeginEvent (T5PTR_DB pDB, T5_WORD wPrgNo);
extern void    T5VMPrg_EndEvent (T5PTR_DB pDB, T5_WORD wPrgNo);
extern T5_BYTE T5VMPrg_GetCurrentEvent (T5PTR_DB pDB);
extern T5_WORD T5VMPrg_GetHighestEventHandler (T5PTR_DB pDB);
extern void    T5VMPrg_CallEventHandler (T5PTR_DB pDB, T5_WORD wEvent);
#endif /*T5DEF_IT*/

/****************************************************************************/
/* TIC interpreter */

#ifdef T5DEF_NOPCSWITCH
extern void    T5VMTic_Init (T5PTR_DB p_pDB);
extern void    T5VMTic_Exit (T5PTR_DB p_pDB);
extern T5_BOOL T5VMTic_Run (T5PTR_DB p_pDB, T5PTR_DBSTATUS p_pStatus,
                            T5_PTWORD p_pCode, T5_DWORD p_dwSize,
                            T5_WORD p_wPrgNo);
#else /*T5DEF_NOPCSWITCH*/
extern void    T5VMTic_Init (T5PTR_DB pDB);
extern void    T5VMTic_Exit (T5PTR_DB pDB);
extern T5_BOOL T5VMTic_Run (T5PTR_DB pDB, T5PTR_DBSTATUS pStatus,
                            T5_PTWORD pCode, T5_DWORD dwSize, T5_WORD wPrgNo);
#endif /*T5DEF_NOPCSWITCH*/

#ifdef T5DEF_CTSEG

extern void T5VMTic_GetCTSegItem (T5PTR_DB pDB, T5_WORD wType,
                                  T5_WORD wDst, T5_WORD wRef);
extern void T5VMTic_SetCTSegItem (T5PTR_DB pDB, T5_WORD wType,
                                  T5_WORD wSrc, T5_WORD wRef);
extern void T5VMTic_CTMemcpy (T5PTR_DB pDB, T5_WORD wDst,
                              T5_WORD wSrc, T5_WORD wSize);

#endif /*T5DEF_CTSEG*/

#ifdef T5DEF_SFC

extern void T5VMTic_SetSfcBreakS (T5PTR_DB pDB, T5_WORD wIndex, T5_WORD wStyle);
extern void T5VMTic_SetSfcBreakT (T5PTR_DB pDB, T5_WORD wIndex, T5_WORD wStyle);

#endif /*T5DEF_SFC*/

extern void T5VMTic_ExeCtrl (T5PTR_DB pDB, T5_WORD wCommand);
extern void T5VMTic_LockVar (T5PTR_DB pDB, T5_WORD wIndex, T5_BYTE bMask, T5_DWORD dwCTOffset);
extern void T5VMTic_UnlockVar (T5PTR_DB pDB, T5_WORD wIndex, T5_BYTE bMask, T5_DWORD dwCTOffset);
extern void T5VMTic_UnlockAllVars (T5PTR_DB pDB);

/****************************************************************************/
/* Compiled code execution */

#ifdef T5DEF_CCAPP

extern void    T5CC_Init (T5PTR_DB pDB);
extern void    T5CC_Exit (T5PTR_DB pDB);
extern T5_BOOL T5CC_IsPossible (T5PTR_DB pDB);
extern T5_BOOL T5CC_Call (T5PTR_DB pDB, T5_WORD wPrgNo, T5_PTBOOL pbDone);

extern T5_BOOL T5CC_PrepareHotChange (T5PTR_DB pDB, T5_PTR pNewCode);
extern T5_BOOL T5CC_CancelHotChange (void);
extern T5_BOOL T5CC_AcceptHotChange (void);

#endif /*T5DEF_CCAPP*/

/****************************************************************************/
/* type conversion functions */

extern void T5VMCnv_Convert (T5PTR_DB pDB, T5_PTWORD pCode);
extern void T5VMCnv_ConvBin (T5_PTR pDst, T5_PTR pSrc,
                             T5_BYTE bDstType, T5_BYTE bSrcType);
extern void T5VMCnv_NumToSZ (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex,
                             T5_PTCHAR sz);
extern void T5VMCnv_NumToSZPtr (T5_PTR pData, T5_WORD wType, T5_PTCHAR sz);
extern void T5VMCnv_SZToNum (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex,
                             T5_PTCHAR sz);
extern void T5VMCnv_SZToNumPtr (T5_PTR pData, T5_WORD wType, T5_PTCHAR sz);

/****************************************************************************/
/* active timers */

extern T5_BOOL T5VMTmr_StartTimer (T5PTR_DB pDB, T5_PTDWORD pdwTime);
extern void    T5VMTmr_StopTimer (T5PTR_DB pDB, T5_PTDWORD pdwTime);
extern void    T5VMTmr_UpdateTimers (T5PTR_DB pDB);
extern void    T5VMTmr_HotRestartTimers (T5PTR_DB pDB);

/****************************************************************************/
/* strings */

extern T5_BYTE  T5VMStr_ICmp (T5_PTBYTE sz1, T5_PTBYTE sz2);
extern T5_BYTE  T5VMStr_NICmp (T5_PTBYTE sz1, T5_PTBYTE sz2, T5_WORD n);

extern T5_RET   T5VMStr_AscForce (T5_PTBYTE pDst,
                                  T5_PTBYTE pbValue, T5_BYTE bStrLen);
extern T5_RET   T5VMStr_AscForceC (T5PTR_DB pDB,
                                   T5_WORD wIndex, T5_PTCHAR pbValue);
extern T5_RET   T5VMStr_Force (T5PTR_DB pDB, T5_WORD wIndex,
                               T5_PTBYTE pbValue);
extern T5_RET   T5VMStr_DBForce (T5PTR_DB pDB, T5_WORD wIndex,
                                 T5_PTWORD pwValue);
extern T5_RET   T5VMStr_DBForcePtr (T5_PTBYTE pDst, T5_PTWORD pwValue);
extern T5_RET   T5VMStr_Copy (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc);
extern T5_RET   T5VMStr_CopyPtr (T5_PTBYTE pDst, T5_PTBYTE pSrc);
extern T5_RET   T5VMStr_Cat (T5PTR_DB pDB, T5_WORD wDst,
                             T5_WORD wS1, T5_WORD wS2);
extern T5_SHORT T5VMStr_Compare (T5PTR_DB pDB, T5_WORD wS1, T5_WORD wS2);
extern T5_RET   T5VMStr_ItoA (T5PTR_DB pDB, T5_WORD wDst, T5_LONG lValue);
extern T5_RET   T5VMStr_UtoA (T5PTR_DB pDB, T5_WORD wDst, T5_DWORD dwValue);
extern void     T5VMStr_UtoSZ (T5_PTCHAR sz, T5_DWORD dwValue);
extern T5_RET   T5VMStr_BootoA (T5PTR_DB pDB, T5_WORD wDst, T5_BOOL bValue);
extern T5_RET   T5VMStr_TmrtoA (T5PTR_DB pDB, T5_WORD wDst, T5_DWORD dwValue);
extern T5_LONG  T5VMStr_AtoI (T5PTR_DB pDB, T5_WORD wSrc);
extern T5_DWORD T5VMStr_AtoUL (T5PTR_DB pDB, T5_WORD wSrc);
extern T5_BOOL  T5VMStr_IsEmpty (T5PTR_DB pDB, T5_WORD wSrc);
extern T5_RET   T5VMStr_FtoA (T5PTR_DB pDB, T5_WORD wDst, T5_REAL lValue);
extern T5_REAL  T5VMStr_AtoF (T5PTR_DB pDB, T5_WORD wSrc);

#ifdef T5DEF_LREALSUPPORTED
extern T5_RET   T5VMStr_DtoA (T5PTR_DB pDB, T5_WORD wDst, T5_LREAL lValue);
extern T5_LREAL T5VMStr_AtoD (T5PTR_DB pDB, T5_WORD wSrc);
#endif /*T5DEF_LREALSUPPORTED*/

extern T5_RET   T5VMStr_Empty (T5PTR_DB pDB, T5_WORD wSrc);
extern T5_LONG  T5VMStr_Ascii (T5PTR_DB pDB, T5_WORD wSrc, T5_LONG lPos);
extern T5_RET   T5VMStr_Char (T5PTR_DB pDB, T5_WORD wDst, T5_LONG lValue);
extern T5_LONG  T5VMStr_GetLength (T5PTR_DB pDB, T5_WORD wSrc);
extern T5_BYTE  T5VMStr_GetMaxLength (T5PTR_DB pDB, T5_WORD wSrc);
extern T5_RET   T5VMStr_Left (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc,
                              T5_LONG lNb);
extern T5_RET   T5VMStr_Right (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc,
                               T5_LONG lNb);
extern T5_RET   T5VMStr_Mid (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc,
                             T5_LONG lPos, T5_LONG lNb);
extern T5_RET   T5VMStr_Delete (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc,
                                T5_LONG lPos, T5_LONG lNb);
extern T5_RET   T5VMStr_Insert (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc,
                                T5_WORD wInsert, T5_LONG lPos);
extern T5_LONG  T5VMStr_Find (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wPat);
extern T5_PTCHAR T5VMStr_GetText (T5PTR_DB pDB, T5_WORD wSrc);
extern T5_DWORD T5VMStr_AtoH (T5PTR_DB pDB, T5_WORD wSrc);
extern void     T5VMStr_HtoA (T5PTR_DB pDB, T5_WORD wDst, T5_DWORD dwSrc);
extern T5_WORD  T5VMStr_Crc16 (T5PTR_DB pDB, T5_WORD wSrc);

#ifdef T5DEF_LINTSUPPORTED

extern T5_LINT T5VMStr_AtoLINT (T5PTR_DB pDB, T5_WORD wSrc);
extern void T5VMStr_LINTtoA (T5PTR_DB pDB, T5_WORD wDst, T5_LINT lSrc);
extern void T5VMStr_LINTtoSZ (T5_PTCHAR sz, T5_LINT lValue);
extern T5_LINT T5VMStr_SZtoLINT (T5_PTCHAR sz);

#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_SINTSUPPORTED
extern T5_DWORD T5VMStr_ToD8 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wDst,
                              T5_WORD wSizeof);
extern T5_DWORD T5VMStr_FromD8 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wSizeof,
                                T5_WORD wDst, T5_DWORD dwCount);
#endif /*T5DEF_SINTSUPPORTED*/

#ifdef T5DEF_DATA16SUPPORTED

extern T5_DWORD T5VMStr_ToD16 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wDst,
                               T5_WORD wSizeof);
extern T5_DWORD T5VMStr_FromD16 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wSizeof,
                               T5_WORD wDst, T5_DWORD dwCount);
#endif /*T5DEF_DATA16SUPPORTED*/

extern T5_DWORD T5VMStr_ToD32 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wDst,
                               T5_WORD wSizeof);
extern T5_DWORD T5VMStr_FromD32 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wSizeof,
                                 T5_WORD wDst, T5_DWORD dwCount);

extern void T5VMStr_DateFmt (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wFmt,
                             T5_DWORD dwDate, T5_DWORD dwTime);

extern T5_BOOL T5VMStr_NumToStr (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wType,
                           T5_WORD wIndex, T5_LONG lWidth, T5_LONG lNbDigit);

/****************************************************************************/
/* standard functions */

extern T5_BOOL     T5VMStf_Execute (T5PTR_DB pDB, T5_PTWORD pArgs);

/****************************************************************************/
/* basic DB calls */

extern T5_PTR T5Tools_GetCTParam (T5PTR_DB pDB, T5_WORD wOffset);
extern T5_PTR T5Tools_GetAnyParam (T5PTR_DB pDB, T5_WORD wTicType,
                       T5_WORD wIndex, T5_PTBOOL pbLock, T5_PTWORD pwSizeof);
extern T5_PTR T5Tools_GetAnyArray (T5PTR_DB pDB,
                                   T5_WORD wTicType, T5_WORD wIndex);
extern T5_PTBYTE T5Tools_GetD8Array (T5PTR_DB pDB, T5_WORD wOffset);

#ifdef T5DEF_DATA16SUPPORTED
extern T5_PTWORD T5Tools_GetD16Array (T5PTR_DB pDB, T5_WORD wOffset);
#endif /*T5DEF_DATA16SUPPORTED*/

extern T5_PTDWORD T5Tools_GetD32Array (T5PTR_DB pDB, T5_WORD wOffset);
extern T5_PTDWORD T5Tools_GetTimeArray (T5PTR_DB pDB, T5_WORD wOffset);

#ifdef T5DEF_DATA64SUPPORTED
extern T5_PTDATA64 T5Tools_GetD64Array (T5PTR_DB pDB, T5_WORD wOffset);
#endif /*T5DEF_DATA64SUPPORTED*/

#ifdef T5DEF_STRINGSUPPORTED
extern T5_PTR T5Tools_GetStringArray (T5PTR_DB pDB, T5_WORD wOffset,
                                      T5_PTBOOL pbRaw);
#endif /*T5DEF_STRINGSUPPORTED*/

extern T5_WORD T5Tools_Crc16 (T5_PTBYTE pBuffer, T5_WORD wLen);

/****************************************************************************/
/* standard function blocks */

extern T5_DWORD T5stfb_R_TRIG (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_F_TRIG (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_RS (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_SR (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_CTU (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_CTD (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_CTUD (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_CMP (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_SEMA (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_TON (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_TOF (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_TP (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_BLINK (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_STACKINT (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_SERIO (T5_WORD wCommand, T5PTR_DB pBase,
                              T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_SERIO_B (T5_WORD wCommand, T5PTR_DB pBase,
                              T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_TOFR (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_TPR (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_PLS (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_UNPACK8 (T5_WORD wCommand, T5PTR_DB pBase,
                                T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_FLIPFLOP (T5_WORD wCommand, T5PTR_DB pBase,
                                 T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_BLINKA (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_DTEVERY (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_FIFO (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_LIFO (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_ALARM_M (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_ALARM_A (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_TMU (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_TMD (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_TMUSEC (T5_WORD wCommand, T5PTR_DB pBase,
                               T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_ISFCSTEP (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_ISFCINITSTEP (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

#ifdef T5DEF_REALSUPPORTED
extern T5_DWORD T5stfb_AVERAGE (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_HYSTER (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_LIM_ALRM (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_INTEGRAL (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_DERIVATE (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_SIG_GEN (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_RAMP (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_CURVELIN (T5_WORD wCommand, T5PTR_DB pBase,
                                 T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_SURFLIN (T5_WORD wCommand, T5PTR_DB pBase,
                                T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

#define T5HLIB_FBLIST_REAL \
            T5HLIB_FB("AVERAGE", T5stfb_AVERAGE) \
            T5HLIB_FB("HYSTER", T5stfb_HYSTER) \
            T5HLIB_FB("LIM_ALRM", T5stfb_LIM_ALRM) \
            T5HLIB_FB("INTEGRAL", T5stfb_INTEGRAL) \
            T5HLIB_FB("DERIVATE", T5stfb_DERIVATE) \
            T5HLIB_FB("SIG_GEN", T5stfb_SIG_GEN) \
            T5HLIB_FB("RAMP", T5stfb_RAMP)\
            T5HLIB_FB("CURVELIN", T5stfb_CURVELIN)\
            T5HLIB_FB("SURFLIN", T5stfb_SURFLIN)

#else /*T5DEF_REALSUPPORTED*/
#define T5HLIB_FBLIST_REAL  /* nothing */
#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_LREALSUPPORTED
extern T5_DWORD T5stfb_AVERAGEL (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_HYSTERACC (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

#define T5HLIB_FBLIST_LREAL \
            T5HLIB_FB("AVERAGEL", T5stfb_AVERAGEL) \
            T5HLIB_FB("HYSTERACC", T5stfb_HYSTERACC)

#else /*T5DEF_LREALSUPPORTED*/
#define T5HLIB_FBLIST_LREAL  /* nothing */
#endif /*T5DEF_LREALSUPPORTED*/

#ifdef T5DEF_RTCLOCKSTAMP

extern T5_DWORD T5stfb_DTAT (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_DTCURDATETIME (T5_WORD wCommand, T5PTR_DB pBase,
                                      T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

#define T5HLIB_FBLIST_DTAT \
            T5HLIB_FB("DTAT", T5stfb_DTAT) \
            T5HLIB_FB("DTCURDATETIME", T5stfb_DTCURDATETIME)

#else /*T5DEF_RTCLOCKSTAMP*/
#define T5HLIB_FBLIST_DTAT  /* nothing */
#endif /*T5DEF_RTCLOCKSTAMP*/

#ifdef T5DEF_VLST
#ifdef T5DEF_FILESUPPORTED

extern T5_DWORD T5stfb_LOGFILECSV (T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

#define T5HLIB_FBLIST_LOGFILE \
            T5HLIB_FB("LOGFILECSV", T5stfb_LOGFILECSV)

#endif /*T5DEF_FILESUPPORTED*/
#endif /*T5DEF_VLST*/

#ifndef T5HLIB_FBLIST_LOGFILE
#define T5HLIB_FBLIST_LOGFILE /* nothing */
#endif /*T5HLIB_FBLIST_LOGFILE*/

#ifdef T5DEF_UDP

extern T5_DWORD T5stfb_MBSLAVEUDP (T5_WORD wCommand, T5PTR_DB pBase,
                                   T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_MBSLAVEUDPEX (T5_WORD wCommand, T5PTR_DB pBase,
                                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

#define T5HLIB_FBLIST_UDP \
            T5HLIB_FB("MBSLAVEUDP", T5stfb_MBSLAVEUDP) \
            T5HLIB_FB("MBSLAVEUDPEX", T5stfb_MBSLAVEUDPEX)

#else /*T5DEF_UDP*/
#define T5HLIB_FBLIST_UDP  /* nothing */
#endif /*T5DEF_UDP*/

#ifdef T5DEF_SERIAL

extern T5_DWORD T5stfb_MBSLAVERTU (T5_WORD wCommand, T5PTR_DB pBase,
                                   T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_MBSLAVERTUEX (T5_WORD wCommand, T5PTR_DB pBase,
                                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD T5stfb_MBSLAVERTUEXD (T5_WORD wCommand, T5PTR_DB pBase,
    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

#define T5HLIB_FBLIST_RTU \
            T5HLIB_FB("MBSLAVERTU", T5stfb_MBSLAVERTU) \
            T5HLIB_FB("MBSLAVERTUEX", T5stfb_MBSLAVERTUEX) \
            T5HLIB_FB("MBSLAVERTUEXD", T5stfb_MBSLAVERTUEXD)

#else /*T5DEF_SERIAL*/
#define T5HLIB_FBLIST_RTU  /* nothing */
#endif /*T5DEF_SERIAL*/

#ifdef T5DEF_SIGNAL
#ifdef T5DEF_REALSUPPORTED

extern T5_DWORD T5stfb_SIGPLAY (T5_WORD wCommand, T5PTR_DB pBase,
                                T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
extern T5_PTR T5stfb_GetSignal (T5PTR_DB pDB, T5_DWORD dwID,
                                 T5_PTDWORD pdwDuration);
extern T5_REAL T5stfb_GetSigVal (T5_PTR pSignal, T5_DWORD dwTime);

#define T5HLIB_FBLIST_SIGPLAY \
            T5HLIB_FB("SIGPLAY", T5stfb_SIGPLAY)

#endif /*T5DEF_REALSUPPORTED*/
#endif /*T5DEF_SIGNAL*/

#ifndef T5HLIB_FBLIST_SIGPLAY
#define T5HLIB_FBLIST_SIGPLAY /* nothing */
#endif /*T5HLIB_FBLIST_SIGPLAY*/

#ifdef T5DEF_CANBUS
extern T5_DWORD T5stfb_CANRCVMSG (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass,
                                  T5_PTR pInst, T5_PTWORD pArgs);
#define T5HLIB_FBLIST_CAN \
            T5HLIB_FB("CANRCVMSG", T5stfb_CANRCVMSG)
#else /*T5DEF_CANBUS*/
#define T5HLIB_FBLIST_CAN /* nothing */
#endif /*T5DEF_CANBUS*/

#ifdef T5DEF_DATA16SUPPORTED

#ifdef  T5DEF_TCPCLIENT
extern T5_DWORD MBMASTERTCP (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass,
                             T5_PTR pInst, T5_PTWORD pArgs);
#endif  /*T5DEF_TCPCLIENT*/
#ifdef  T5DEF_SERIAL
extern T5_DWORD MBMASTERRTU (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass,
                             T5_PTR pInst, T5_PTWORD pArgs);
#endif  /*T5DEF_SERIAL*/

#ifdef  T5DEF_TCPCLIENT
#ifdef      T5DEF_SERIAL
#define T5HLIB_FBLIST_MBM \
            T5HLIB_FB("MBMASTERTCP", MBMASTERTCP) \
            T5HLIB_FB("MBMASTERRTU", MBMASTERRTU)
#else       /*T5DEF_SERIAL*/
#define T5HLIB_FBLIST_MBM \
            T5HLIB_FB("MBMASTERTCP", MBMASTERTCP)
#endif      /*T5DEF_SERIAL*/
#else   /*T5DEF_TCPCLIENT*/
#ifdef      T5DEF_SERIAL
#define T5HLIB_FBLIST_MBM \
            T5HLIB_FB("MBMASTERRTU", MBMASTERRTU)
#else       /*T5DEF_SERIAL*/
#define T5HLIB_FBLIST_MBM /*nothing*/
#endif      /*T5DEF_SERIAL*/
#endif  /*T5DEF_TCPCLIENT*/

#else /*T5DEF_DATA16SUPPORTED*/

#define T5HLIB_FBLIST_MBM /*nothing*/

#endif /*T5DEF_DATA16SUPPORTED*/

#ifdef T5DEF_MBSTCP

extern T5_DWORD MBSLAVETCP (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass,
                            T5_PTR pInst, T5_PTWORD pArgs);
extern T5_DWORD MBSLAVETCPEX (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass,
                              T5_PTR pInst, T5_PTWORD pArgs);

#define T5HLIB_FBLIST_MBSTCP \
            T5HLIB_FB("MBSLAVETCP", MBSLAVETCP) \
            T5HLIB_FB("MBSLAVETCPEX", MBSLAVETCPEX)

#else /* T5DEF_MBSTCP */

#define T5HLIB_FBLIST_MBSTCP /*nothing*/

#endif /*T5DEF_MBSTCP*/

#ifndef T5DEF_NOIECCOUNTER

#define T5HLIB_FBLIST_IECCOUNTERS \
            T5HLIB_FB("CTU", T5stfb_CTU) \
            T5HLIB_FB("CTD", T5stfb_CTD) \
            T5HLIB_FB("CTUD", T5stfb_CTUD)

#else /*T5DEF_NOIECCOUNTER*/

#define T5HLIB_FBLIST_IECCOUNTERS /*nothing*/

#endif /*T5DEF_NOIECCOUNTER*/

#define T5HLIBLIST_STD \
        T5HLIB_BEGIN \
            T5HLIB_FB("R_TRIG", T5stfb_R_TRIG) \
            T5HLIB_FB("F_TRIG", T5stfb_F_TRIG) \
            T5HLIB_FB("RS", T5stfb_RS) \
            T5HLIB_FB("SR", T5stfb_SR) \
            T5HLIB_FBLIST_IECCOUNTERS \
            T5HLIB_FB("CMP", T5stfb_CMP) \
            T5HLIB_FB("SEMA", T5stfb_SEMA) \
            T5HLIB_FB("TON", T5stfb_TON) \
            T5HLIB_FB("TOF", T5stfb_TOF) \
            T5HLIB_FB("TP", T5stfb_TP) \
            T5HLIB_FB("BLINK", T5stfb_BLINK) \
            T5HLIB_FB("STACKINT", T5stfb_STACKINT) \
            T5HLIB_FB("SERIO", T5stfb_SERIO) \
            T5HLIB_FB("SERIO_B", T5stfb_SERIO_B) \
            T5HLIB_FBLIST_REAL \
            T5HLIB_FB("UNPACK8", T5stfb_UNPACK8) \
            T5HLIB_FB("TOFR", T5stfb_TOFR) \
            T5HLIB_FB("TPR", T5stfb_TPR) \
            T5HLIB_FB("PLS", T5stfb_PLS) \
            T5HLIB_FB("FLIPFLOP", T5stfb_FLIPFLOP) \
            T5HLIB_FB("BLINKA", T5stfb_BLINKA) \
            T5HLIB_FBLIST_DTAT \
            T5HLIB_FB("DTEVERY", T5stfb_DTEVERY) \
            T5HLIB_FB("FIFO", T5stfb_FIFO) \
            T5HLIB_FB("LIFO", T5stfb_LIFO) \
            T5HLIB_FB("ALARM_M", T5stfb_ALARM_M) \
            T5HLIB_FB("ALARM_A", T5stfb_ALARM_A) \
            T5HLIB_FBLIST_LOGFILE \
            T5HLIB_FB("TMU", T5stfb_TMU) \
            T5HLIB_FB("TMUSEC", T5stfb_TMUSEC) \
            T5HLIB_FB("TMD", T5stfb_TMD) \
            T5HLIB_FBLIST_UDP \
            T5HLIB_FBLIST_RTU \
            T5HLIB_FBLIST_SIGPLAY \
            T5HLIB_FB("ISFCSTEP", T5stfb_ISFCSTEP) \
            T5HLIB_FB("ISFCINITSTEP", T5stfb_ISFCINITSTEP) \
            T5HLIB_FBLIST_LREAL \
            T5HLIB_FBLIST_CAN \
            T5HLIB_FBLIST_MBM \
            T5HLIB_FBLIST_MBSTCP \
        T5HLIB_END

/****************************************************************************/
/* File access */

#ifdef T5DEF_FILESUPPORTED

extern T5_LONG T5File_Open (T5_PTCHAR szPathname, T5_BOOL bWrite);
extern T5_LONG T5File_OpenAppend (T5_PTCHAR szPathname);
extern void    T5File_Close (T5_LONG hFile);
extern T5_BOOL T5File_Eof (T5_LONG hFile);
extern T5_BOOL T5File_Read (T5_LONG hFile, T5_PTR pData, T5_LONG dwSize);
extern T5_BOOL T5File_Write (T5_LONG hFile, T5_PTR pData, T5_LONG dwSize);
extern T5_BYTE T5File_ReadLine (T5_LONG hFile, T5_PTCHAR pString,
                                T5_BYTE bMaxLen);
extern T5_BOOL T5File_WriteLine (T5_LONG hFile, T5_PTCHAR pString);

#ifdef T5DEF_FILEMGT

extern T5_BOOL  T5File_Exist (T5_PTCHAR pPath);
extern T5_DWORD T5File_GetSize (T5_PTCHAR pPath);
extern T5_BOOL  T5File_Copy (T5_PTCHAR pSrc, T5_PTCHAR pDst);
extern T5_BOOL  T5File_Delete (T5_PTCHAR pPath);
extern T5_BOOL  T5File_Rename (T5_PTCHAR pPath, T5_PTCHAR pNewName);

#endif /*T5DEF_FILEMGT*/

#ifdef T5DEF_F_ISREADY
extern T5_BOOL  T5File_IsReady (T5_LONG hFile);
#endif /*T5DEF_F_ISREADY*/

#ifdef T5DEF_FILEV2
extern T5_DWORD T5File_Fread (T5_LONG hFile, T5_DWORD dwRecSize, T5_DWORD nb, T5_PTR pData);
extern T5_DWORD T5File_Fwrite (T5_LONG hFile, T5_DWORD dwRecSize, T5_DWORD nb, T5_PTR pData);
extern T5_BOOL T5File_Fflush (T5_LONG hFile);
extern T5_BOOL T5File_Fseek (T5_LONG hFile, T5_DWORD dwOrigin, T5_LONG lPos);
#endif /*T5DEF_FILEV2*/

#endif /*T5DEF_FILESUPPORTED*/

/****************************************************************************/
/* System resources */

extern T5PTR_DBSYSRSC T5VMRsc_GetFreeItem (T5PTR_DB pDB);
extern T5PTR_DBSYSRSC T5VMRsc_FindData (T5PTR_DB pDB, T5_DWORD dwObject);
extern T5PTR_DBSYSRSC T5VMRsc_FindArray (T5PTR_DB pDB, T5_LONG lID);
extern T5PTR_DBSYSRSC T5VMRsc_FindSocket (T5PTR_DB pDB, T5_DWORD dwID);
extern void T5VMRsc_FreeAll (T5PTR_DB pDB);
extern T5_LONG T5VMRsc_CreateArray (T5PTR_DB pDB, T5_LONG lId, T5_LONG lSize,
                                    T5_WORD wSizeOf);
extern T5_BOOL T5VMRsc_ReadArray (T5PTR_DB pDB, T5_LONG lId, T5_LONG lIndex,
                                  T5_WORD wSizeOf, T5_PTR pData);
extern T5_LONG T5VMRsc_WriteArray (T5PTR_DB pDB, T5_LONG lId, T5_LONG lIndex,
                                   T5_WORD wSizeOf, T5_PTR pData);
extern T5_BOOL T5VMRsc_Exist (T5PTR_DB pDB);

/****************************************************************************/
/* real time clock - calendar */

#ifdef T5DEF_RTCLOCKSUPPORTED
extern T5_BYTE T5RtClk_GetString (T5_LONG lType, T5_PTCHAR pString);
#endif /*T5DEF_RTCLOCKSUPPORTED*/

#ifdef T5DEF_RTCLOCKSTAMP
extern T5_DWORD T5RtClk_GetCurDateStamp (void);
extern T5_DWORD T5RtClk_GetCurTimeStamp (void);
#endif /*T5DEF_RTCLOCKSTAMP*/

#ifdef T5DEF_RTCLOCKSTAMPENH

extern void T5RtClk_GetCurDateTimeStamp (T5_BOOL bLocal, T5_PTDWORD pdwDate,
                                         T5_PTDWORD pdwTime, T5_PTBOOL pbDST);

#define T5RTCLK_GETDATETIME(l,d,t,dst) \
    T5RtClk_GetCurDateTimeStamp (l, &(d), &(t), &(dst))

#else /*T5DEF_RTCLOCKSTAMPENH*/

#define T5RTCLK_GETDATETIME(l,d,t,dst) \
    { \
        t = T5RtClk_GetCurTimeStamp (); \
        d = T5RtClk_GetCurDateStamp (); \
        dst = FALSE; \
    }

#endif /*T5DEF_RTCLOCKSTAMPENH*/

/****************************************************************************/
/* I/Os */

extern T5_DWORD T5Ios_GetMemorySizing (T5_PTR pDef);
extern T5_RET   T5Ios_Open (T5_PTR pDef, T5PTR_DB pDB, T5_PTR pMem);
extern T5_RET   T5Ios_Close (T5_PTR pDef, T5PTR_DB pDB, T5_PTR pMem);
extern T5_RET   T5Ios_Exchange (T5_PTR pDef, T5PTR_DB pDB, T5_PTR pMem);
extern T5_RET   T5Ios_CanHotRestart (T5_PTR pDef, T5PTR_DB pDB, T5_PTR pMem);
extern T5_RET   T5Ios_HotRestart (T5_PTR pDef, T5PTR_DB pDB, T5_PTR pMem);

#ifdef T5DEF_DLLIOS

extern T5_DWORD T5IOdll_OpenDynaLinks (void);
extern void     T5IOdll_CloseDynaLinks (void);
extern T5_DWORD T5IOdll_Call (T5_DWORD dwCommand, T5_PTR pDef,
                              T5PTR_DB pDB, T5_PTR pMem);

#endif /*T5DEF_DLLIOS*/

#define T5DLLIO_MEMSIZING   1
#define T5DLLIO_OPEN        2
#define T5DLLIO_CANHOT      3
#define T5DLLIO_HOT         4
#define T5DLLIO_CLOSE       5
#define T5DLLIO_EXCHANGE    6

/****************************************************************************/
/* RETAIN - warm restart */

#ifdef T5DEF_RETAIN

extern void    T5VMWarm_PrepareData (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET  T5VMWarm_CanUpdatePointers (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET  T5VMWarm_UpdatePointers (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET  T5VMWarm_Load (T5PTR_DB pDB, T5_PTR pArgs);
extern void    T5VMWarm_ShutDown (T5PTR_DB pDB, T5_PTR pArgs);
extern void    T5VMWarm_CycleExchange (T5PTR_DB pDB);
extern T5_BOOL T5VMWarm_CanLoad (T5_PTR pCode, T5_PTR pArgs);
extern T5_BOOL T5VMWarm_CanStore (T5PTR_DB pDB, T5_PTR pArgs);

#ifdef T5DEF_FILESUPPORTED

extern T5_BOOL T5VMWarm_SaveToFile (T5PTR_DB pDB, T5_PTCHAR szFile);
extern T5_BOOL T5VMWarm_LoadFromFile (T5PTR_DB pDB, T5_PTCHAR szFile);
extern T5_BOOL T5VMWarm_CheckFile (T5_DWORD dwCrcExpected, T5_PTCHAR szFile);

#endif /*T5DEF_FILESUPPORTED*/

extern T5_BOOL T5Retain_CanLoad (T5_DWORD dwCrc, T5_PTR pArgs);
extern T5_RET  T5Retain_Load (T5PTR_DBRETAIN pRetain, T5_PTR pArgs);
extern T5_BOOL T5Retain_CanStore (T5PTR_DBRETAIN pRetain, T5_PTR pArgs);
extern void    T5Retain_ShutDown (T5PTR_DBRETAIN pRetain, T5_PTR pArgs);
extern void    T5Retain_CycleExchange (T5PTR_DBRETAIN pRetain);

#endif /*T5DEF_RETAIN*/

/****************************************************************************/
/* custom blocks enumeration */

extern T5_BOOL   T5Blocks_GetFirstID (T5_PTDWORD pdwID);
extern T5_BOOL   T5Blocks_GetNextID (T5_PTDWORD pwdID);
extern T5_PTCHAR T5Blocks_GetName (T5_DWORD dwID);
extern T5HND_FB  T5Blocks_GetProc (T5_DWORD dwID);

#ifdef T5DEF_T5BLOCKS_FIND
extern T5HND_FB  T5Blocks_Find (T5_PTCHAR szName);
#endif /*T5DEF_T5BLOCKS_FIND*/

#ifdef T5DEF_DLLBLOCKS

extern T5_DWORD  T5Bdll_OpenDynaLinks (void);
extern void      T5Bdll_CloseDynaLinks (void);
extern T5_BOOL   T5Bdll_StartEnumerate (void);
extern void      T5Bdll_StopEnumerate (void);
extern T5_BOOL   T5Bdll_GetFirstID (T5_PTDWORD pdwID);
extern T5_BOOL   T5Bdll_GetNextID (T5_PTDWORD pdwID);
extern T5_PTCHAR T5Bdll_GetName (T5_DWORD dwID);
extern T5HND_FB  T5Bdll_GetProc (T5_DWORD dwID);

#endif /*T5DEF_DLLBLOCKS*/

/****************************************************************************/
/* Sockets */

#ifdef T5DEF_ETHERNET

extern T5_RET    T5Socket_Initialize (T5HND_CS pfCallback);
extern void      T5Socket_Terminate (void);
extern T5_RET    T5Socket_CreateListeningSocket (T5_WORD wPort,
                                        T5_WORD wMaxCnx, T5_PTSOCKET pSocket,
                                        T5HND_CS pfCallback);
extern void      T5Socket_CloseSocket (T5_SOCKET sock);
extern T5_SOCKET T5Socket_Accept (T5_SOCKET sockListen, T5HND_CS pfCallback);
extern T5_WORD   T5Socket_Send (T5_SOCKET sock, T5_WORD wSize, T5_PTR pData,
                                T5_PTBOOL pbFail);
extern T5_WORD   T5Socket_Receive (T5_SOCKET sock, T5_WORD wSize,
                                   T5_PTR pData, T5_PTBOOL pbFail);

#ifdef T5DEF_TCPCLIENT

extern T5_RET T5Socket_CreateConnectedSocket (T5_PTCHAR szAddr, T5_WORD wPort,
                                              T5_PTSOCKET pSocket,
                                              T5_PTBOOL pbWait,
                                              T5HND_CS pfCallback);

extern T5_RET T5Socket_CheckPendingConnect (T5_SOCKET sock, T5_PTBOOL pbFail);

#ifdef T5DEF_SOCKETV2

extern T5_RET T5Socket_CreateListeningSocketOn (T5_WORD wPort, T5_WORD wMaxCnx,
                                                T5_PTCHAR szAddr, T5_PTSOCKET pSocket,
                                                T5HND_CS pfCallback);

#endif /*T5DEF_SOCKETV2*/

#endif /*T5DEF_TCPCLIENT*/

#ifdef T5DEF_UDP

extern T5_RET  T5Socket_UdpCreate (T5_WORD wPort, T5_PTSOCKET pSocket);
extern T5_RET  T5Socket_UdpSendTo (T5_SOCKET sock, T5_PTR pAddr,
                                   T5_PTR pData, T5_WORD wSize);
extern T5_WORD T5Socket_UdpRecvFrom (T5_SOCKET sock, T5_PTR pData, T5_WORD wMax,
                                     T5_PTR pAddr);
extern void    T5Socket_UdpAddrMake (T5_PTR pAddr, T5_PTCHAR szAddr,
                                     T5_WORD wPort);


#ifdef T5DEF_SOCKETV2

extern T5_RET T5Socket_AcceptBroadcast (T5_SOCKET sock);
extern T5_RET T5Socket_UdpCreateOn (T5_WORD wPort, T5_PTCHAR szAddr,
                                    T5_PTSOCKET pSocket);

#endif /*T5DEF_SOCKETV2*/

#endif /*T5DEF_UDP*/

#endif /*T5DEF_ETHERNET*/

/****************************************************************************/
/* serial communication */

#ifdef T5DEF_SERIAL

extern void    T5Serial_Initialize (T5_PTSERIAL pSerial);
extern T5_BOOL T5Serial_IsValid (T5_PTSERIAL pSerial);
extern T5_BOOL T5Serial_Open (T5_PTSERIAL pSerial, T5_PTCHAR szConfig);
extern void    T5Serial_Close (T5_PTSERIAL pSerial);
extern T5_WORD T5Serial_Send (T5_PTSERIAL pSerial, T5_WORD wSize,
                              T5_PTR pData);
extern T5_WORD T5Serial_Receive (T5_PTSERIAL pSerial, T5_WORD wSize,
                                 T5_PTR pData);

#ifdef T5DEF_SERIALSENDTIME
extern T5_DWORD T5Serial_SendTime (T5_PTSERIAL pSerial);
#else /*T5DEF_SERIALSENDTIME*/
#define T5Serial_SendTime(s) 0L
#endif /*T5DEF_SERIALSENDTIME*/

#ifdef T5DEF_SERIALINFO
extern void    T5Serial_Info (T5_PTSERIAL pSerial, T5_WORD wInfo);
#endif /*T5DEF_SERIALINFO*/

extern T5_BOOL T5SerialAsc_Open (T5_PTSERIAL pSerial, T5_PTCHAR szConfig,
                                 T5_PTDWORD pdwStatus);
extern void    T5SerialAsc_Close (T5_PTSERIAL pSerial, T5_PTDWORD pdwStatus);
extern T5_WORD T5SerialAsc_Send (T5_PTSERIAL pSerial, T5_WORD wSize,
                                 T5_PTR pData, T5_PTDWORD pdwStatus);
extern T5_WORD T5SerialAsc_Receive (T5_PTSERIAL pSerial, T5_WORD wSize,
                                    T5_PTR pData, T5_PTDWORD pdwStatus);
extern void    T5SerialAsc_Flush (T5_PTSERIAL pSerial, T5_PTDWORD pdwStatus);

#endif /*T5DEF_SERIAL*/

/****************************************************************************/
/* Communication server - socket implementation */

#ifdef T5DEF_ETHERNET

extern void T5CSSock_Accept (T5PTR_CS pData);
extern void T5CSSock_CloseAllSockets (T5PTR_CS pData);
extern void T5CSSock_ActivateAllSockets (T5PTR_CS pData);

extern void T5CSSock_CheckSilentSockets (T5PTR_CS pData,
                                         T5_DWORD dwTimeStamp);
extern void T5CSSock_CheckSilentSocketsEx (T5PTR_CS pData,
                                           T5_DWORD dwTimeStamp,
                                           T5_DWORD dwTimeout
                                           );

#endif /*T5DEF_ETHERNET*/

/****************************************************************************/
/* Communication server - serial implementation */

#ifdef T5DEF_SERIAL

extern void T5CSSer_Open (T5PTR_CS pData, T5_PTCHAR szConfig);
extern void T5CSSer_Close (T5PTR_CS pData);
extern void T5CSSer_Activate (T5PTR_CS pData);

#endif /*T5DEF_SERIAL*/

/****************************************************************************/
/* Communication server - multitasking */

#ifdef T5DEF_CSM5

extern void T5CS_OpenM5 (T5PTR_CS pData, T5PTR_CSSET pSettings);
extern void T5CS_ActivateM5 (T5PTR_CS pData, T5_DWORD dwTimeStamp);
extern void T5CSM5_Activate (T5PTR_CSPORT pPort, T5_DWORD dwTimeStamp);

#endif /*T5DEF_CSM5*/

/****************************************************************************/
/* Communication services */

extern void    T5CS_Open (T5PTR_CS pData, T5PTR_CSSET pSettings,
                          T5HND_CS pfCallback);
extern void    T5CS_Close (T5PTR_CS pData);
extern void    T5CS_Activate (T5PTR_CS pData);
extern void    T5CS_ActivateStamp (T5PTR_CS pData, T5_DWORD dwTimeStamp);

extern T5_WORD T5CS_GetTaskNo (T5PTR_CS pData, T5_WORD wCaller);

extern T5_WORD T5CS_GetQuestion (T5PTR_CS pData, T5_PTWORD pwCaller);
extern void    T5CS_ReleaseQuestion (T5PTR_CS pData, T5_WORD wCaller);
extern void    T5CS_SendRC (T5PTR_CS pData, T5_WORD wCaller, T5_WORD wRC);

extern T5_PTR  T5CS_GetVMRequest (T5PTR_CS pData, T5_WORD wCaller);
extern T5_PTR  T5CS_GetVMAnswerBuffer (T5PTR_CS pData, T5_WORD wCaller);
extern void    T5CS_SendVMAnswer (T5PTR_CS pData, T5_WORD wCaller,
                                  T5_WORD wLen);

extern T5_BOOL T5CS_GetLoadInfo (T5PTR_CS pData, T5_WORD wCaller,
                                 T5PTR_CSLOAD pLoad);
extern T5_BOOL T5CS_GetStartInfo (T5PTR_CS pData, T5_WORD wCaller,
                                  T5PTR_CSSTART pStart);

extern T5_PTR  T5CS_GetRequestFrame (T5PTR_CS pData, T5_WORD wCaller);
extern T5_PTR  T5CS_GetAnswerFrameBuffer (T5PTR_CS pData, T5_WORD wCaller);
extern void    T5CS_SendAnswerFrame (T5PTR_CS pData, T5_WORD wCaller,
                                     T5_WORD wLen);

extern void T5CS_CheckSilentSockets (T5PTR_CS pData, T5_DWORD dwTimeStamp,
                                     T5_DWORD dwTimeout);
extern T5_DWORD T5CS_GetNbHostTCP (T5PTR_CS pData);

#ifdef T5DEF_SP5

extern void    T5CS_SetSP5Password (T5PTR_CS pData, T5_PTCHAR szPassword);

#endif /*T5DEF_SP5*/

/****************************************************************************/
/* protocol passwords */

#ifndef T5_DOWNLOADPASSWORD

#define T5_GETDOWNLOADPASSWORD()    0L
#define T5_SETDOWNLOADPASSWORD(o,n) TRUE

#endif /*T5_DOWNLOADPASSWORD*/

/****************************************************************************/
/* Memory manager */

extern void   T5MM_Open (T5PTR_MM pMM, T5_PTR pConf);
extern void   T5MM_Close (T5PTR_MM pMM);

extern T5_RET T5MM_LoadCode (T5PTR_MM pMM, T5_PTCHAR szAppName);
extern T5_RET T5MM_FreeCode (T5PTR_MM pMM);
extern T5_PTR T5MM_LinkCode (T5PTR_MM pMM);
extern void   T5MM_UnlinkCode (T5PTR_MM pMM);

extern T5_RET T5MM_WriteCodeRequest (T5PTR_MM pMM, T5_WORD wRequest,
                                     T5PTR_CSLOAD pLoad);
extern T5_RET T5MM_WriteChangeRequest (T5PTR_MM pMM, T5_WORD wRequest,
                                       T5PTR_CSLOAD pLoad);
extern T5_RET T5MM_WriteSyncRequest (T5PTR_MM pMM, T5_WORD wRequest,
                                     T5PTR_CSLOAD pLoad);

extern T5_RET T5MM_AllocRawVMDB (T5PTR_MM pMM, T5_DWORD dwRawSize);
extern T5_RET T5MM_FreeVMDB (T5PTR_MM pMM);
extern T5_PTR T5MM_LinkVMDB (T5PTR_MM pMM);
extern void   T5MM_UnlinkVMDB (T5PTR_MM pMM);
extern T5_RET T5MM_LoadVMDB (T5PTR_MM pMM, T5_PTCHAR szAppName);
extern T5_RET T5MM_SaveVMDB (T5PTR_MM pMM, T5_PTCHAR szAppName);

#ifdef T5DEF_ONLINECHANGE

extern T5_RET T5MM_LoadChange (T5PTR_MM pMM, T5_PTCHAR szAppName);
extern T5_RET T5MM_FreeChange (T5PTR_MM pMM);
extern T5_PTR T5MM_LinkChange (T5PTR_MM pMM);
extern void   T5MM_UnlinkChange (T5PTR_MM pMM);

extern T5_RET T5MM_RegisterChange (T5PTR_MM pMM);
extern T5_RET T5MM_SaveChange (T5PTR_MM pMM, T5_PTCHAR szAppName);
extern T5_RET T5MM_SaveChangeA (T5PTR_MM pMM, T5_PTCHAR szAppName);

#endif /*T5DEF_ONLINECHANGE*/

/****************************************************************************/
/* System memory */

extern T5_RET T5Memory_Alloc (T5PTR_MMB mmb, T5_DWORD dwSize, T5_PTR pConf);
extern T5_RET T5Memory_Load (T5PTR_MMB mmb, T5_PTCHAR szAppName,
                             T5_PTR pConf);
extern T5_RET T5Memory_Free (T5PTR_MMB mmb, T5_PTR pConf);
extern T5_RET T5Memory_Link (T5PTR_MMB mmb, T5_PTR pConf);
extern T5_RET T5Memory_Unlink (T5PTR_MMB mmb, T5_PTR pConf);

extern T5_RET T5Memory_StartWrite (T5PTR_MMB mmb, T5PTR_CSLOAD pLoad,
                                   T5_PTR pConf);
extern T5_RET T5Memory_Write (T5PTR_MMB mmb, T5PTR_CSLOAD pLoad,
                              T5_PTR pConf);
extern T5_RET T5Memory_EndWrite (T5PTR_MMB mmb, T5PTR_CSLOAD pLoad,
                                 T5_PTR pConf);
extern T5_RET T5Memory_Save (T5PTR_MMB mmb, T5_PTCHAR szAppName,
                             T5_PTR pConf);

/****************************************************************************/
/* Embedded variable map */

#ifdef T5DEF_VARMAP

extern T5_RET      T5VMMap_GetSizing (T5PTR_DB pDB, T5_PTR pCode);
extern T5_RET      T5VMMap_Build (T5PTR_DB pDB, T5_PTR pCode);
extern T5_RET      T5VMMap_CanHotRestart (T5PTR_DB pDB, T5_PTR pCode);
extern T5_RET      T5VMMap_HotRestart (T5PTR_DB pDB, T5_PTR pCode);

extern T5PTR_DBMAP T5Map_GetFirst (T5PTR_DB pDB, T5_PTWORD pwPos);
extern T5PTR_DBMAP T5Map_GetNext (T5PTR_DB pDB, T5_PTWORD pwPos);
extern T5PTR_DBMAP T5Map_GetByIndex (T5PTR_DB pDB, T5_WORD wType,
                                     T5_WORD wIndex);
extern T5PTR_DBMAP T5Map_GetByNumTag (T5PTR_DB pDB, T5_WORD wTag);
extern T5PTR_DBMAP T5Map_GetBySymbol (T5PTR_DB pDB, T5_PTCHAR szSymbol);
extern T5_PTCHAR   T5Map_GetSymbol (T5PTR_DBMAP pItem);
extern T5_WORD     T5Map_GetIndex (T5PTR_DBMAP pItem);
extern T5_WORD     T5Map_GetType (T5PTR_DBMAP pItem);
extern T5_WORD     T5Map_GetDim (T5PTR_DBMAP pItem);
extern T5_WORD     T5Map_GetStringLenth (T5PTR_DBMAP pItem);
extern T5_WORD     T5Map_GetNumTag (T5PTR_DBMAP pItem);
extern T5_PTR      T5Map_GetProps (T5PTR_DBMAP pItem);
extern T5_WORD     T5Map_GetPropsSize (T5PTR_DBMAP pItem);
extern T5_PTCHAR   T5Map_GetProfileName (T5PTR_DBMAP pItem);
extern T5_PTCHAR   T5Map_GetTypeName (T5PTR_DBMAP pItem, T5PTR_DB pDB);
extern T5_BOOL     T5Map_IsLocked (T5PTR_DBMAP pItem);

extern T5PTR_DBMAP T5Map_GetByData (T5PTR_DB pDB, T5_PTR pData);
extern T5PTR_DBMAPEX T5Map_GetVSIByData (T5PTR_DB pDB, T5_PTR pData);

extern T5PTR_DBMAPEX T5Map_GetVSI (T5PTR_DBMAP pItem);
extern T5_BOOL     T5Map_GetVSIBit (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_DWORD dwBit);
extern T5_BOOL     T5Map_SetVSIBit (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_DWORD dwBit, T5_BOOL bValue);
extern T5_DWORD    T5Map_GetVSIDate (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex);
extern T5_BOOL     T5Map_SetVSIDate (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_DWORD dwValue);
extern T5_DWORD    T5Map_GetVSITime (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex);
extern T5_BOOL     T5Map_SetVSITime (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_DWORD dwValue);
extern T5_BOOL     T5Map_StampVSI (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex);
extern T5_BOOL     T5Map_StampVSIGMT (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex);
extern T5_BOOL     T5Map_ResetSetVSIBits (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex);
extern T5_BOOL     T5Map_CopyVSI (T5PTR_DB pDB, T5_WORD wType, T5_WORD wDst, T5_WORD wSrc,
                                  T5_BOOL bBits, T5_BOOL bStamp);

extern T5_LONG     T5Map_GetLongValue (T5PTR_DBMAP pItem);
extern T5_DWORD    T5Map_GetDWordValue (T5PTR_DBMAP pItem);
#ifdef T5DEF_LINTSUPPORTED
extern T5_LINT     T5Map_GetLIntValue (T5PTR_DBMAP pItem);
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_REALSUPPORTED
extern T5_REAL     T5Map_GetRealValue (T5PTR_DBMAP pItem);
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
extern T5_LREAL    T5Map_GetLRealValue (T5PTR_DBMAP pItem);
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
extern T5_PTBYTE   T5Map_GetStringValue (T5PTR_DBMAP pItem, T5_PTBYTE pbLen);
#endif /*T5DEF_STRINGSUPPORTED*/

extern T5_RET      T5Map_SetLongValue (T5PTR_DBMAP pItem, T5_LONG lValue);
extern T5_RET      T5Map_SetDWordValue (T5PTR_DBMAP pItem, T5_DWORD dwValue);
#ifdef T5DEF_LINTSUPPORTED
extern T5_RET      T5Map_SetLIntValue (T5PTR_DBMAP pItem, T5_LINT lValue);
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_REALSUPPORTED
extern T5_RET      T5Map_SetRealValue (T5PTR_DBMAP pItem, T5_REAL rValue);
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
extern T5_RET      T5Map_SetLRealValue (T5PTR_DBMAP pItem, T5_LREAL lValue);
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
extern T5_RET      T5Map_SetStringValue (T5PTR_DBMAP pItem, T5_PTBYTE pbValue,
                                         T5_BYTE bLen);
#endif /*T5DEF_STRINGSUPPORTED*/

extern T5_RET T5Map_SetXVAddress (T5PTR_DB pDB, T5PTR_DBMAP pItem,
                                  T5_PTR pData);

extern T5_WORD T5Map_GetDescLength (T5PTR_DB pDB, T5_WORD wItem);
extern T5_WORD T5Map_GetDesc (T5PTR_DB pDB, T5_WORD wItem, T5_PTBYTE pFrame);

extern T5_PTBYTE T5Map_GetTypeInfo (T5PTR_DBMAP pItem, T5PTR_DB pDB);
extern T5_BOOL T5Map_CheckPointer (T5PTR_DB pDB, T5PTR_DBMAP pMapPtr);

extern T5_PTR T5VmSyb_ParseSingle (T5PTR_DB pDB, T5_PTCHAR szSyb, T5_PTBYTE pbType);
extern T5_PTR T5VmSyb_Parse (T5PTR_DB pDB, T5_PTCHAR szSyb, T5_PTBYTE pbType, T5_PTCHAR *pszType);


extern T5_PTR T5Map_IsArrayOfSingleVar (T5PTR_DB pDB, T5PTR_DBMAP pVar,
                                        T5_PTWORD pwCount, T5_PTWORD pwType);

typedef void T5HDNSPEC (*T5HND_CTSYBENUM)(T5_PTCHAR szName, T5_BYTE bTicType, T5_DWORD dwData);

extern T5_BOOL T5VmSyb_EnumCTMembers (T5PTR_DB pDB, T5PTR_DBMAP pMap,
                                      T5_PTCHAR szBuffer, T5_WORD wBufSize,
                                      T5HND_CTSYBENUM pfCallback, T5_DWORD dwData);

#endif /*T5DEF_VARMAP*/

/****************************************************************************/
/* cloning */

extern T5_DWORD T5VMClone_GetSize (T5PTR_DB pExternalBase);
extern T5_RET T5VMClone_Build (T5PTR_DB pExternalBase, T5PTR_DB pClone,
                               T5_PTR pCode);

/****************************************************************************/
/* sampling trace */

#ifdef T5DEF_BSAMPLING

extern T5_RET T5VMBsmp_ResetContent (T5PTR_DB pDB, T5_WORD wIndex);
extern T5_RET T5VMBsmp_Set (T5PTR_DB pDB, T5_WORD wIndex, T5_PTWORD pCode);
extern void T5VMBsmp_Activate (T5PTR_DB pDB, T5_WORD wIndex);
extern void T5VMBsmp_Ctl (T5PTR_DB pDB, T5_WORD wIndex, T5_WORD wCtl);

extern T5_WORD T5VMBsmp_GetSettingsSize (T5PTR_DB pDB, T5_WORD wIndex);
extern T5_WORD T5VMBsmp_GetSettings (T5PTR_DB pDB, T5_WORD wIndex,
                                     T5_PTR pFrame);
extern T5_WORD T5VMBsmp_GetDataSize (T5PTR_DB pDB, T5_WORD wIndex);
extern T5_WORD T5VMBsmp_GetData (T5PTR_DB pDB, T5_WORD wIndex,
                                 T5_PTR pFrame);

#endif /*T5DEF_BSAMPLING*/

/****************************************************************************/
/* IOX - DB access from I/O drivers */

extern void     T5IOX_WriteChanBool (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                     T5_WORD wChan, T5_BOOL bValue);
extern T5_BOOL  T5IOX_ReadChanBool (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                    T5_WORD wChan);

#ifdef T5DEF_SINTSUPPORTED

extern void     T5IOX_WriteChanSint (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                     T5_WORD wChan, T5_CHAR cValue);
extern T5_CHAR  T5IOX_ReadChanSint (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                    T5_WORD wChan);

#endif /*T5DEF_SINTSUPPORTED*/

#ifdef T5DEF_DATA16SUPPORTED

extern void     T5IOX_WriteChanInt (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                    T5_WORD wChan, T5_SHORT iValue);
extern T5_SHORT T5IOX_ReadChanInt (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                   T5_WORD wChan);

#endif /*T5DEF_DATA16SUPPORTED*/

extern void     T5IOX_WriteChanDint (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                     T5_WORD wChan, T5_LONG lValue);
extern T5_LONG  T5IOX_ReadChanDint (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                    T5_WORD wChan);

#ifdef T5DEF_REALSUPPORTED

extern void     T5IOX_WriteChanReal (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                     T5_WORD wChan, T5_REAL rValue);
extern T5_REAL  T5IOX_ReadChanReal (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                    T5_WORD wChan);

#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_LINTSUPPORTED

extern void     T5IOX_WriteChanLint (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                     T5_WORD wChan, T5_LINT lValue);
extern T5_LINT  T5IOX_ReadChanLint (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                    T5_WORD wChan);

#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_LREALSUPPORTED

extern void     T5IOX_WriteChanLReal (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                      T5_WORD wChan, T5_LREAL lValue);
extern T5_LREAL T5IOX_ReadChanLReal (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                     T5_WORD wChan);

#endif /*T5DEF_LREALSUPPORTED*/

#ifdef T5DEF_STRINGSUPPORTED

extern void     T5IOX_WriteChanBinString (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                          T5_WORD wChan, T5_PTBYTE sValue);
extern void     T5IOX_WriteChanCString (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                        T5_WORD wChan, T5_PTCHAR sValue);
extern T5_PTBYTE T5IOX_ReadChanBinString (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                          T5_WORD wChan);
extern T5_PTCHAR T5IOX_ReadChanCString (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                                        T5_WORD wChan);

#endif /*T5DEF_STRINGSUPPORTED*/

/****************************************************************************/
/* MODBUS */

#ifdef T5DEF_MODBUSIOS

extern T5_DWORD T5MB_GetMemorySizing (T5_PTR pCode);
extern T5_RET   T5MB_Build (T5_PTR pCode, T5_PTR pMB, T5_DWORD dwFullSize);
extern T5_RET   T5MB_CanBuildHOT (T5_PTR pCode, T5_PTR pMB);
extern T5_RET   T5MB_BuildHOT (T5_PTR pCode, T5_PTR pMB);
extern T5_RET   T5MB_Link (T5_PTR pMB, T5PTR_DB pDB);
extern T5_RET   T5MB_Open (T5_PTR pMB);
extern void     T5MB_Close (T5_PTR pMB);
extern void     T5MB_Activate (T5_PTR pMB, T5_DWORD dwTimeStamp);
extern void     T5MB_Exchange (T5_PTR pMB, T5PTR_DB pDB);
extern void     T5MB_ExchangeServerOnly (T5_PTR pMB, T5PTR_DB pDB);
extern void     T5MB_ExchangeClientOnly (T5_PTR pMB, T5PTR_DB pDB);

extern T5_WORD  T5MB_Serve (T5_PTR pMB, T5_PTBYTE pIn, T5_PTBYTE pOut,
                            T5_BOOL bTCP);
extern T5_PTR   T5MB_FindServer (T5_PTR pMB, T5_PTCHAR szID);
extern T5_WORD  T5MB_ServeEx (T5_PTR pMB, T5_PTBYTE pIn, T5_PTBYTE pOut,
                              T5_BOOL bTCP, T5_WORD wSlave);
extern T5_WORD  T5MB_ServeExID (T5_PTR pMB, T5_PTBYTE pIn, T5_PTBYTE pOut,
                                T5_BOOL bTCP, T5_WORD wSlave, T5_PTCHAR szID);

extern T5_WORD  T5MBComm_Open (T5PTR_MBCLIENT pClient);
extern void     T5MBComm_Close (T5PTR_MBCLIENT pClient);
extern void     T5MBComm_CheckPendingConnect (T5PTR_MBCLIENT pClient);
extern void     T5MBComm_Send (T5PTR_MBCLIENT pClient);
extern void     T5MBComm_Receive (T5PTR_MBCLIENT pClient,
                                  T5_DWORD dwTimeStamp);

extern void     T5MBFram_Build (T5PTR_MBFRAME pFrame, T5_WORD wDriver,
                                T5_WORD wSlave, T5PTR_MBREQ pReq);
extern void     T5MBFram_Extract (T5PTR_MBFRAME pFrame, T5_WORD wDriver,
                                  T5_WORD wSlave, T5PTR_MBREQ pReq);

extern T5_WORD  T5MBSrv_Serve (T5PTR_MBCLIENT pServer,
                               T5_PTBYTE pIn, T5_PTBYTE pOut);
extern T5_BOOL T5MBSrv_SetDevIdent (T5_PTCHAR szVendor, T5_PTCHAR szProduct,
                                    T5_PTCHAR szRevision, T5_PTWORD pwExt,
                                    T5_WORD wExtCount);

#ifdef T5DEF_UDP

extern T5_RET T5MBSrv_UdpOpen (T5_WORD wPort, T5_PTSOCKET pSocket);
extern void   T5MBSrv_UdpClose (T5_SOCKET sock);
extern void   T5MBSrv_UdpActivate (T5PTR_DB pDB, T5_SOCKET sock,
                                   T5_BOOL bRTU, T5_WORD wSlave, T5_PTCHAR szID);

#endif /*T5DEF_UDP*/

#endif /*T5DEF_MODBUSIOS*/

/****************************************************************************/
/* ASi */

#ifdef T5DEF_ASI

extern T5_DWORD T5VMAsi_GetSizing (T5PTR_ASIDEF pDef);
extern T5_RET   T5VMAsi_Open (T5PTR_ASIDEF pDef, T5PTR_ASI pASI,
                              T5PTR_DB pDB);
extern T5_RET   T5VMAsi_CanHotStart (T5PTR_ASIDEF pDef, T5PTR_ASI pASI);
extern T5_RET   T5VMAsi_HotStart (T5PTR_ASIDEF pDef, T5PTR_ASI pASI,
                                  T5PTR_DB pDB);
extern void     T5VMAsi_Close (T5PTR_ASI pASI);
extern void     T5VMAsi_Exchange (T5PTR_ASI pASI);
extern void     T5VMAsi_GetMasterProfiles (T5PTR_ASI pASI, T5_WORD wMaster,
                                           T5_PTBYTE pDest);

extern T5_BOOL  T5Asi_IsHWOpen (void);
extern T5_RET   T5Asi_Init (T5PTR_ASI pASI);
extern void     T5Asi_Exit (T5PTR_ASI pASI);
extern void     T5Asi_ApplyConfig (T5PTR_ASI pASI);
extern void     T5Asi_ReadConfig (T5PTR_ASI pASI);
extern void     T5Asi_Exchange (T5PTR_ASI pASI);

extern T5_BYTE  T5Asi_ReadPP (T5PTR_ASI pASI, T5_WORD wMaster, T5_WORD wSlave);
extern T5_BOOL  T5Asi_WritePP (T5PTR_ASI pASI, T5_WORD wMaster, T5_WORD wSlave,
                               T5_BYTE bValue);
extern T5_BOOL  T5Asi_SendParameter (T5PTR_ASI pASI, T5_WORD wMaster,
                                     T5_WORD wSlave, T5_BYTE bValue);
extern T5_BYTE  T5Asi_ReadPI (T5PTR_ASI pASI, T5_WORD wMaster, T5_WORD wSlave);
extern T5_BOOL  T5Asi_StorePI (T5PTR_ASI pASI, T5_WORD wMaster);

#endif /*T5DEF_ASI*/

/****************************************************************************/
/* event server */

#ifdef T5DEF_EVENTSERVER

extern void     T5Evsq_Init (T5PTR_EVQUEUE pQueue,
                             T5_DWORD dwSize, T5_DWORD dwPass, T5_PTR pData);
extern void     T5Evsq_FlushReadPointer (T5PTR_EVQUEUE pQueue);
extern void     T5Evsq_FlushReadData (T5PTR_EVQUEUE pQueue);
extern void     T5Evsq_FlushAllEvents (T5PTR_EVQUEUE pQueue);
extern T5_WORD  T5Evs_GetNbClient (T5_PTR pData);

extern void     T5EvsFrm_PutIdle (T5PTR_EVS pEvs);
extern void     T5EvsFrm_PutHeader (T5PTR_EVS pEvs);
extern T5_BOOL  T5EvsFrm_PutEvent (T5PTR_EVS pEvs, T5PTR_EVENT pEvent,
                                   T5_PTBYTE pData);
extern T5_BOOL  T5EvsFrm_GetAck (T5PTR_EVS pEvs);
extern T5_BOOL  T5EvsFrm_PutEventList (T5PTR_EVS pEvs);

extern T5_DWORD T5Evs1_GetMemorySize (T5PTR_EVSETTINGS pSettings);
extern T5_RET   T5Evs1_Open (T5_PTR pData, T5PTR_EVSETTINGS pSettings);
extern void     T5Evs1_Close (T5_PTR pData);
extern void     T5Evs1_Activate (T5_PTR pData, T5_DWORD dwTimeStamp);
extern T5_RET   T5Evs1_SetCallback (T5_PTR pData, T5HND_EVSCB pCallback);
extern T5_RET   T5Evs1_RemoveCallback (T5_PTR pData, T5HND_EVSCB pCallback);
extern T5_RET   T5Evs1_EventDT (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                                T5_BYTE bLen, T5_PTR pArg,
                                T5_DWORD dwDate, T5_DWORD dwTime);
extern T5_RET   T5Evs1_Event (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                              T5_BYTE bLen, T5_PTR pArg);
extern T5_RET   T5Evs1_TextEvent (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                                  T5_PTCHAR szText);
extern T5_RET   T5Evs1_Trace (T5_PTR pData, T5_PTCHAR pText);
extern T5_RET   T5Evs1_OnAppStart (T5_PTR pData, T5_PTCHAR szAppName);
extern T5_RET   T5Evs1_OnAppStop (T5_PTR pData, T5_PTCHAR szAppName);
extern T5_RET   T5Evs1_OnAppChange (T5_PTR pData, T5_PTCHAR szAppName);
extern void     T5Evs1_FlushEAEvents (T5_PTR pData);

#ifdef T5MAX_EVCLIENT

extern T5_DWORD T5Evs_GetMemorySize (T5PTR_EVSETTINGS pSettings);
extern T5_RET   T5Evs_Open (T5_PTR pData, T5PTR_EVSETTINGS pSettings);
extern void     T5Evs_Close (T5_PTR pData);
extern void     T5Evs_Activate (T5_PTR pData, T5_DWORD dwTimeStamp);
extern T5_RET   T5Evs_SetCallback (T5_PTR pData, T5HND_EVSCB pCallback);
extern T5_RET   T5Evs_RemoveCallback (T5_PTR pData, T5HND_EVSCB pCallback);
extern T5_RET   T5Evs_Event (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                             T5_BYTE bLen, T5_PTR pArg);
extern T5_RET   T5Evs_TextEvent (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                                 T5_PTCHAR szText);
extern T5_RET   T5Evs_Trace (T5_PTR pData, T5_PTCHAR pText);
extern T5_RET   T5Evs_OnAppStart (T5_PTR pData, T5_PTCHAR szAppName);
extern T5_RET   T5Evs_OnAppStop (T5_PTR pData, T5_PTCHAR szAppName);
extern T5_RET   T5Evs_OnAppChange (T5_PTR pData, T5_PTCHAR szAppName);
extern T5_RET   T5Evs_EventTo (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                               T5_BYTE bLen, T5_PTR pArg, T5_WORD wServer,
                               T5_PTBOOL pbNoServer,
                               T5_DWORD dwDate, T5_DWORD dwTime);
extern T5_BOOL  T5Evs_HasNewConections (T5_PTR pData);
extern void     T5Evs_FlushEAEvents (T5_PTR pData);
extern T5_WORD  T5Evs_GetCnxPrefs (T5_PTR pData, T5_WORD wServer);
extern void     T5Evs_GetCnxVsiPrefs (T5_PTR pData, T5_PTBOOL pFlags);

#else /*T5MAX_EVCLIENT*/

#define T5Evs_GetMemorySize     T5Evs1_GetMemorySize
#define T5Evs_Open              T5Evs1_Open
#define T5Evs_Close             T5Evs1_Close
#define T5Evs_Activate          T5Evs1_Activate
#define T5Evs_SetCallback       T5Evs1_SetCallback
#define T5Evs_RemoveCallback    T5Evs1_RemoveCallback
#define T5Evs_Event             T5Evs1_Event
#define T5Evs_TextEvent         T5Evs1_TextEvent
#define T5Evs_Trace             T5Evs1_Trace
#define T5Evs_OnAppStart        T5Evs1_OnAppStart
#define T5Evs_OnAppStop         T5Evs1_OnAppStop
#define T5Evs_OnAppChange       T5Evs1_OnAppChange
#define T5Evs_FlushEAEvents     T5Evs1_FlushEAEvents

#endif /*T5MAX_EVCLIENT*/

#ifdef T5DEF_EVSHOOK

extern T5_BOOL T5EvsHook_BeforeQueuing (void);
extern void    T5EvsHook_AfterQueuing (void);

#define T5EVSHOOK_BEFORE    T5EvsHook_BeforeQueuing ()
#define T5EVSHOOK_AFTER     T5EvsHook_AfterQueuing ()

#else /*T5DEF_EVSHOOK*/

#define T5EVSHOOK_BEFORE    TRUE
#define T5EVSHOOK_AFTER     

#endif /*T5DEF_EVSHOOK*/

#endif /*T5DEF_EVENTSERVER*/

/****************************************************************************/
/* value change events */

#ifdef T5DEF_EA

extern T5_WORD T5EA_RegisterChangeEvent (T5PTR_DB pDB, T5_WORD hCnx,
                                         T5_BYTE bTicType, T5_WORD wOffset,
                                         T5_WORD wID, T5_PTR pHP, T5_PTR pHN,
                                         T5_DWORD dwCTOffset);
extern T5_RET  T5EA_UnregisterChangeEvent (T5PTR_DB pDB, T5_WORD hCnx,
                                           T5_BYTE bTicType, T5_WORD wOffset,
                                           T5_DWORD dwCTOffset);
extern T5_RET  T5EA_UnregisterAllBroadcastEvents (T5PTR_DB pDB);
extern T5_RET  T5EA_RegisterStaticEvents (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET  T5EA_Open (T5_PTR pCode, T5PTR_DB pDB, T5_PTR pEvss);
extern void    T5EA_Close (T5PTR_DB pDB, T5_PTR pEvss);
extern void    T5EA_ManageChangeEvents (T5PTR_DB pDB, T5_PTR pEvss);
extern void    T5EA_ManageBroadcastEvents (T5PTR_DB pDB, T5_PTR pEvss);
extern void    T5EA_ManagePrivateEvents (T5PTR_DB pDB, T5_PTR pEvss);
extern void    T5EA_ForceGeneralUpdate (T5PTR_DB pDB);

extern T5_WORD  T5EAX_GetNbCnx (T5_PTR pCode);
extern T5_WORD  T5EAX_GetNbVar (T5_PTR pCode);
extern T5_RET   T5EAX_Build (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET   T5EAX_CanBuildHOT (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET   T5EAX_BuildHOT (T5_PTR pCode, T5PTR_DB pDB);
extern T5_RET   T5EAX_Link (T5PTR_DB pDB);
extern T5_RET   T5EAX_Open (T5PTR_DB pDB);
extern void     T5EAX_Close (T5PTR_DB pDB);
extern void     T5EAX_Activate (T5PTR_DB pDB, T5_DWORD dwTimeStamp);

#endif /*T5DEF_EA*/

/****************************************************************************/
/* file transfer */

#ifdef T5DEF_FT

extern void T5CSFT_Serve (T5PTR_CS pData, T5_WORD wCaller);

extern T5_DWORD T5FT_OpenWrite (T5_DWORD dwSize, T5_PTCHAR szPath);
extern T5_DWORD T5FT_OpenRead (T5_PTCHAR szPath, T5_PTDWORD pdwSize);
extern T5_BOOL  T5FT_Write (T5_DWORD dwHandle, T5_WORD wLen, T5_PTBYTE pData);
extern T5_WORD  T5FT_Read (T5_DWORD dwHandle, T5_WORD wLen, T5_PTBYTE pData);
extern void     T5FT_Close (T5_DWORD dwHandle);
extern T5_BOOL  T5FT_Delete (T5_PTCHAR szPath);
extern T5_DWORD T5FT_OpenDir (void);
extern T5_BOOL  T5FT_GetDirEntry (T5_DWORD dwHandle, T5_PTBYTE pData);
extern void     T5FT_CloseDir (T5_DWORD dwHandle);

#endif /*T5DEF_FT*/

/****************************************************************************/
/* bus drivers */

#ifdef T5DEF_BUSDRV

extern void     T5BusDrv_OpenLinks (void);
extern void     T5BusDrv_CloseLinks (void);
extern T5_RET   T5BusDrv_Build (T5PTR_DB pDB, T5_PTR pCode);
extern T5_RET   T5BusDrv_CanBuildHot (T5PTR_DB pDB, T5_PTR pCode);
extern T5_RET   T5BusDrv_BuildHot (T5PTR_DB pDB, T5_PTR pCode);
extern void     T5BusDrv_Close (T5PTR_DB pDB);
extern void     T5BusDrv_Exchange (T5PTR_DB pDB);

#ifdef T5DEF_REDBUSDRVEXT

extern void     T5BusDrv_OpenPassive (T5_PTR pCode);
extern void     T5BusDrv_ClosePassive (void);
extern void     T5BusDrv_ExchangePassive (void);

#endif /*T5DEF_REDBUSDRVEXT*/

#endif /*T5DEF_BUSDRV*/

#ifdef T5DEF_DDKC

extern void     T5DDKC_OpenLinks (void);
extern void     T5DDKC_CloseLinks (void);
extern T5_DWORD T5DDKC_GetSize (T5_PTR pCode);
extern T5_RET   T5DDKC_Build (T5PTR_DB pDB, T5_PTR pCode);
extern T5_RET   T5DDKC_CanBuildHot (T5PTR_DB pDB, T5_PTR pCode);
extern T5_RET   T5DDKC_BuildHot (T5PTR_DB pDB, T5_PTR pCode);
extern void     T5DDKC_Close (T5PTR_DB pDB);
extern void     T5DDKC_Exchange (T5PTR_DB pDB, T5_DWORD dwTimeStamp);

#endif /*T5DEF_DDKC*/

extern T5_BOOL  T5Param_Get (T5PTR_DB pDB, T5_PTCHAR szFile,
                             T5_PTCHAR szSection, T5_PTCHAR szEntry,
                             T5_PTCHAR szBuffer, T5_WORD wBufSize);

/* CAN bus ******************************************************************/

#ifdef T5DEF_CANBUS

extern void     T5VMCan_GetDBSize (T5PTR_DB pDB, T5_PTR pCode);
extern T5_RET   T5VMCan_Build (T5PTR_DB pDB, T5_PTR pCode);
extern T5_RET   T5VMCan_CanBuildHot (T5PTR_DB pDB, T5_PTR pCode);
extern T5_RET   T5VMCan_BuildHot (T5PTR_DB pDB, T5_PTR pCode);
extern void     T5VMCan_Close (T5PTR_DB pDB);
extern void     T5VMCan_Exchange (T5PTR_DB pDB, T5_DWORD dwTimeStamp);

extern void     T5Can_PreOpen (void);
extern void     T5Can_PostClose (void);
extern T5_RET   T5Can_Open (T5PTR_DBCANPORT pPort, T5_PTCHAR szSettings);
extern void     T5Can_Close (T5PTR_DBCANPORT pPort);
extern void     T5Can_OnPreExchange (T5PTR_DBCANPORT pPort);
extern void     T5Can_OnPostExchange (T5PTR_DBCANPORT pPort);
extern T5_BOOL  T5Can_GetBusOffFlag (T5PTR_DBCANPORT pPort);
extern T5_DWORD T5Can_GetErrors (T5PTR_DBCANPORT pPort);
extern void     T5CAN_OnSpcOut (T5PTR_DBCANPORT pPort, T5PTR_DBCANMSG pMsg);

extern T5PTR_DBCANMSG T5VMCan_PeekNextSndMsg (T5PTR_DBCANPORT pPort);
extern T5PTR_DBCANMSG T5VMCan_PopNextSndMsg (T5PTR_DBCANPORT pPort);
extern T5PTR_DBCANMSG T5VMCan_FindRcvMsg (T5PTR_DBCANPORT pPort, T5_DWORD dwID);
extern void T5VMCan_MarkRcvMsg (T5PTR_DBCANPORT pPort, T5PTR_DBCANMSG pMsg);
extern T5PTR_DBCANMSG T5VMCan_GetSndMsgList (T5PTR_DBCANPORT pPort, T5_PTWORD pwNb);
extern T5PTR_DBCANMSG T5VMCan_GetRcvMsgList (T5PTR_DBCANPORT pPort, T5_PTWORD pwNb);

extern T5_BOOL  T5VMCan_SendAppMsg (T5PTR_DB pDB, T5_PTCHAR szPort, T5_DWORD dwID,
                                    T5_LONG lLen, T5_PTBYTE pData, T5_BOOL bRTR);

extern T5_BOOL  T5VMCan_RcvAppMsg (T5PTR_DB pDB, T5_PTCHAR szPort,
                                   T5_PTDWORD pdwID, T5_PTBYTE pbLen,
                                   T5_PTBYTE pData);

#ifdef T5DEF_CANRAW

extern T5_DWORD T5CANRaw_Open (T5_WORD wRate, T5_BOOL b2B, T5_PTCHAR szSettings);
extern void T5CANRaw_Close (T5_DWORD dwPort);
extern T5_BOOL T5CANRaw_PopRcv (T5_DWORD dwPort, T5_PTDWORD pdwID,
                                T5_PTBYTE pbLen, T5_PTBYTE data);
extern T5_BOOL T5CANRaw_Send (T5_DWORD dwPort, T5_DWORD dwID, T5_BYTE bLen,
                              T5_PTBYTE data, T5_BOOL b2B, T5_BOOL bRTR);

#endif /*T5DEF_CANRAW*/

#ifdef T5DEF_CANJF

extern void T5CANJF_Begin (void);
extern void T5CANJF_Repeat (T5_PTCHAR szSource, T5_PTCHAR szDest);
extern void T5CANJF_Bridge (T5_PTCHAR szSource, T5_PTCHAR szDest,
                            T5_DWORD dwID, T5_BOOL bSAMask, T5_BOOL bPrioMask);
extern void T5CANJF_Route (T5_PTCHAR szSource, T5_PTCHAR szDest,
                           T5_DWORD dwID, T5_BYTE bSubstSA, T5_BOOL bSAMask, T5_BOOL bPrioMask);
extern void T5CANJF_End (void);

#endif /*T5DEF_CANJF*/

#endif /*T5DEF_CANBUS*/

/* XS text buffers **********************************************************/

extern T5_DWORD T5XS_Alloc (T5PTR_DB pDB, T5_DWORD dwSize, T5_PTCHAR szText);
extern T5_BOOL T5XS_Free (T5PTR_DB pDB, T5_DWORD hXS);
extern T5_DWORD T5XS_GetLength (T5PTR_DB pDB, T5_DWORD hXS);
extern T5_DWORD T5XS_Get (T5PTR_DB pDB, T5_DWORD hXS, T5_PTCHAR pDst,
                          T5_DWORD dwBufSize);
extern T5_BOOL T5XS_SetAllocSize (T5PTR_DB pDB, T5_DWORD hXS, T5_DWORD dwWished);
extern T5_BOOL T5XS_Set (T5PTR_DB pDB, T5_DWORD hXS, T5_PTR pSrc, T5_DWORD dwLen);
extern T5_BOOL T5XS_Copy (T5PTR_DB pDB, T5_DWORD hDst, T5_DWORD hSrc);
extern T5_BOOL T5XS_Rewind (T5PTR_DB pDB, T5_DWORD hXS);
extern T5_BOOL T5XS_GetLine (T5PTR_DB pDB, T5_DWORD hXS, T5_PTCHAR pDst,
                             T5_DWORD dwBufSize);
extern T5_BOOL T5XS_Append (T5PTR_DB pDB, T5_DWORD hXS, T5_PTCHAR pSrc);
extern T5_BOOL T5XS_AppendXS (T5PTR_DB pDB, T5_DWORD hDst, T5_DWORD hSrc);
extern T5_BOOL T5XS_SetFile (T5PTR_DB pDB, T5_DWORD hXS, T5_PTCHAR szPath);
extern T5_BOOL T5XS_SaveToFile (T5PTR_DB pDB, T5_DWORD hXS, T5_PTCHAR szPath);
extern T5_BOOL T5XS_AnsiToUtf8 (T5PTR_DB pDB, T5_DWORD hXS);
extern T5_BOOL T5XS_Utf8ToAnsi (T5PTR_DB pDB, T5_DWORD hXS);
extern T5_DWORD T5XS_GetTextHandle (T5PTR_DB pDB, T5_DWORD hXS);

/* error reports */

#define XSERR_BADMANAGER    1   /* invalid instance of TXBManager */
#define XSERR_MANOPEN       2   /* manager already open */
#define XSERR_MANNOTOPEN    3   /* manager not open */
#define XSERR_BADHANDLE     4   /* invalid handle */
#define XSERR_TRUNCATE      5   /* string truncated */
#define XSERR_READFILE      6   /* cant read file */
#define XSERR_WRITEFILE     7   /* cant write file */
#define XSERR_BADTYPE       8   /* unsupported data type */
#define XSERR_OVERFLOW      9   /* too many buffers allocated */
#define XSERR_NOTSUP        10  /* operation not supported */

/* XML blocks ***************************************************************/

extern T5_DWORD T5XML_Alloc (T5PTR_DB pDB, T5_PTCHAR szName);
extern T5_BOOL T5XML_Free (T5PTR_DB pDB, T5_DWORD hXml);
extern T5_BOOL T5XML_GetTagName (T5PTR_DB pDB, T5_DWORD hXml,
                                 T5_PTCHAR pBuffer, T5_DWORD dwBufSize);
extern T5_BOOL T5XML_SetTagContents (T5PTR_DB pDB, T5_DWORD hXml,
                                     T5_PTCHAR pBuffer);
extern T5_BOOL T5XML_GetTagContents (T5PTR_DB pDB, T5_DWORD hXml,
                                     T5_PTCHAR pBuffer, T5_DWORD dwBufSize);
extern T5_BOOL T5XML_SetTagContentsXS (T5PTR_DB pDB, T5_DWORD hXml,
                                       T5_DWORD hXS);
extern T5_BOOL T5XML_AppendContents (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR pBuffer);
extern T5_DWORD T5XML_GetTagContentsXS (T5PTR_DB pDB, T5_DWORD hXml);
extern T5_DWORD T5XML_AddChild (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR szName);
extern T5_DWORD T5XML_FirstChild (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR szName);
extern T5_DWORD T5XML_NextChild (T5PTR_DB pDB, T5_DWORD hXml, T5_DWORD hChild,
                                 T5_PTCHAR szName);
extern T5_BOOL T5XML_GetTagAttrib (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR szAttrib,
                                   T5_PTCHAR pBuffer, T5_DWORD dwBufSize);
extern T5_BOOL T5XML_SetTagAttrib (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR szAttrib,
                                   T5_PTCHAR pBuffer);
extern T5_BOOL T5XML_WriteToFile (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR szFile);
extern T5_BOOL T5XML_WriteToXS (T5PTR_DB pDB, T5_DWORD hXml, T5_DWORD hXS);
extern T5_DWORD T5XML_ParseFile (T5PTR_DB pDB, T5_PTCHAR szFile);
extern T5_DWORD T5XML_ParseXS (T5PTR_DB pDB, T5_DWORD hXS);
extern T5_DWORD T5XML_GetParent (T5PTR_DB pDB, T5_DWORD hXml);

#define XMLERR_BADMANAGER   1   /* invalid instance of XMLManager */
#define XMLERR_MANOPEN      2   /* manager already open */
#define XMLERR_MANNOTOPEN   3   /* manager not open */
#define XMLERR_BADHANDLE    4   /* invalid handle */
#define XMLERR_OVERFLOW     5   /* too many buffers allocated */
#define XMLERR_BADTXB       6   /* invalid TXB handle */
#define XMLERR_XMLSYNTAX    7   /* invalid XML file (syntax error) */
#define XMLERR_READFILE     8   /* cant read file */
#define XMLERR_WRITEFILE    9   /* cant write file */

/* TXB blocks ***************************************************************/

#define _T5TXBDECLAREHANDLE(f) \
    T5_DWORD f (T5_WORD wCommand, T5PTR_DB pBase, \
    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

_T5TXBDECLAREHANDLE(TXBMANAGER)
_T5TXBDECLAREHANDLE(TXBAPPEND)
_T5TXBDECLAREHANDLE(TXBAPPENDLINE)
_T5TXBDECLAREHANDLE(TXBAPPENDTXB)
_T5TXBDECLAREHANDLE(TXBAPPENDEOL)
_T5TXBDECLAREHANDLE(TXBCLEAR)
_T5TXBDECLAREHANDLE(TXBCOPY)
_T5TXBDECLAREHANDLE(TXBFREE)
_T5TXBDECLAREHANDLE(TXBGETDATA)
_T5TXBDECLAREHANDLE(TXBLASTERROR)
_T5TXBDECLAREHANDLE(TXBGETLENGTH)
_T5TXBDECLAREHANDLE(TXBGETLINE)
_T5TXBDECLAREHANDLE(TXBGETSTRING)
_T5TXBDECLAREHANDLE(TXBNEW)
_T5TXBDECLAREHANDLE(TXBNEWSTRING)
_T5TXBDECLAREHANDLE(TXBREADFILE)
_T5TXBDECLAREHANDLE(TXBREWIND)
_T5TXBDECLAREHANDLE(TXBSETDATA)
_T5TXBDECLAREHANDLE(TXBSETSTRING)
_T5TXBDECLAREHANDLE(TXBWRITEFILE)
_T5TXBDECLAREHANDLE(TXBANSITOUTF8)
_T5TXBDECLAREHANDLE(TXBUTF8TOANSI)
_T5TXBDECLAREHANDLE(TXBSENDTCP)
_T5TXBDECLAREHANDLE(TXBRCVTCP)

#define T5TXB_FBLIST_STATIC \
    { "TXBMANAGER", TXBMANAGER }, \
    { "TXBAPPEND", TXBAPPEND }, \
    { "TXBAPPENDLINE", TXBAPPENDLINE }, \
    { "TXBAPPENDTXB", TXBAPPENDTXB }, \
    { "TXBAPPENDEOL", TXBAPPENDEOL }, \
    { "TXBCLEAR", TXBCLEAR }, \
    { "TXBCOPY", TXBCOPY }, \
    { "TXBFREE", TXBFREE }, \
    { "TXBGETDATA", TXBGETDATA }, \
    { "TXBLASTERROR", TXBLASTERROR }, \
    { "TXBGETLENGTH", TXBGETLENGTH }, \
    { "TXBGETLINE", TXBGETLINE }, \
    { "TXBGETSTRING", TXBGETSTRING }, \
    { "TXBNEW", TXBNEW }, \
    { "TXBNEWSTRING", TXBNEWSTRING }, \
    { "TXBREADFILE", TXBREADFILE }, \
    { "TXBREWIND", TXBREWIND }, \
    { "TXBSETDATA", TXBSETDATA }, \
    { "TXBSETSTRING", TXBSETSTRING }, \
    { "TXBWRITEFILE", TXBWRITEFILE }, \
    { "TXBANSITOUTF8", TXBANSITOUTF8 }, \
    { "TXBUTF8TOANSI", TXBUTF8TOANSI }, \
    { "TXBSENDTCP", TXBSENDTCP }, \
    { "TXBRCVTCP", TXBRCVTCP },

/* XML blocks ***************************************************************/

#define _T5XMLDECLAREHANDLE(f) \
    T5_DWORD f (T5_WORD wCommand, T5PTR_DB pBase, \
    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

_T5XMLDECLAREHANDLE(XMLMANAGER)
_T5XMLDECLAREHANDLE(XMLADDCHILD)
_T5XMLDECLAREHANDLE(XMLCHECKTAGNAME)
_T5XMLDECLAREHANDLE(XMLFIRSTCHILD)
_T5XMLDECLAREHANDLE(XMLFREEDOC)
_T5XMLDECLAREHANDLE(XMLLASTERROR)
_T5XMLDECLAREHANDLE(XMLGETSYBVALUE)
_T5XMLDECLAREHANDLE(XMLGETTAGATTRIB)
_T5XMLDECLAREHANDLE(XMLGETTAGCONT)
_T5XMLDECLAREHANDLE(XMLGETTAGCONTT)
_T5XMLDECLAREHANDLE(XMLGETTAGNAME)
_T5XMLDECLAREHANDLE(XMLNEWDOC)
_T5XMLDECLAREHANDLE(XMLNEXTCHILD)
_T5XMLDECLAREHANDLE(XMLPARSEDOCFILE)
_T5XMLDECLAREHANDLE(XMLPARSEDOCTXB)
_T5XMLDECLAREHANDLE(XMLSETSYBVALUE)
_T5XMLDECLAREHANDLE(XMLSETTAGATTRIB)
_T5XMLDECLAREHANDLE(XMLSETTAGCONT)
_T5XMLDECLAREHANDLE(XMLSETTAGCONTT)
_T5XMLDECLAREHANDLE(XMLWRITEDOCFILE)
_T5XMLDECLAREHANDLE(XMLWRITEDOCTXB)
_T5XMLDECLAREHANDLE(XMLGETPARENT)
_T5XMLDECLAREHANDLE(HMINEWMENU)
_T5XMLDECLAREHANDLE(HMINEWMENUITEM)
_T5XMLDECLAREHANDLE(HMIDELMENU)
_T5XMLDECLAREHANDLE(HMIRENAMEMENU)
_T5XMLDECLAREHANDLE(HMIMENU)

#define T5XML_FBLIST_STATIC \
    { "XMLMANAGER", XMLMANAGER }, \
    { "XMLADDCHILD", XMLADDCHILD }, \
    { "XMLCHECKTAGNAME", XMLCHECKTAGNAME }, \
    { "XMLFIRSTCHILD", XMLFIRSTCHILD }, \
    { "XMLFREEDOC", XMLFREEDOC }, \
    { "XMLLASTERROR", XMLLASTERROR }, \
    { "XMLGETSYBVALUE", XMLGETSYBVALUE }, \
    { "XMLGETTAGATTRIB", XMLGETTAGATTRIB }, \
    { "XMLGETTAGCONT", XMLGETTAGCONT }, \
    { "XMLGETTAGCONTT", XMLGETTAGCONTT }, \
    { "XMLGETTAGNAME", XMLGETTAGNAME }, \
    { "XMLNEWDOC", XMLNEWDOC }, \
    { "XMLNEXTCHILD", XMLNEXTCHILD }, \
    { "XMLPARSEDOCFILE", XMLPARSEDOCFILE }, \
    { "XMLPARSEDOCTXB", XMLPARSEDOCTXB }, \
    { "XMLSETSYBVALUE", XMLSETSYBVALUE }, \
    { "XMLSETTAGATTRIB", XMLSETTAGATTRIB }, \
    { "XMLSETTAGCONT", XMLSETTAGCONT }, \
    { "XMLSETTAGCONTT", XMLSETTAGCONTT }, \
    { "XMLWRITEDOCFILE", XMLWRITEDOCFILE }, \
    { "XMLWRITEDOCTXB", XMLWRITEDOCTXB }, \
    { "XMLGETPARENT", XMLGETPARENT }, \
    { "HMINEWMENU", HMINEWMENU }, \
    { "HMINEWMENUITEM", HMINEWMENUITEM }, \
    { "HMIDELMENU", HMIDELMENU }, \
    { "HMIRENAMEMENU", HMIRENAMEMENU }, \
    { "HMIMENU", HMIMENU },

/* Collections **************************************************************/

#ifdef T5DEF_BARRAY
#include "t5barray.h"
#endif /*T5DEF_BARRAY*/

/* UNICODE OEM functions ****************************************************/

#ifdef T5DEF_UNICODE

extern T5_PTCHAR T5Unicod_AllocUtf8Conversion (T5_PTCHAR szAnsi);
extern T5_PTCHAR T5Unicod_AllocAnsiConversion (T5_PTCHAR szUtf8);
extern void T5Unicod_FreeConversion (T5_PTCHAR szBuffer);

#endif /*T5DEF_UNICODE*/

/****************************************************************************/

#endif /*_T5VMAPI_H_INCLUDED_*/

/* eof **********************************************************************/

