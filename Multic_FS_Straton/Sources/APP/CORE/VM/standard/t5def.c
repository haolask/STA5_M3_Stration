/*****************************************************************************
T5Def.c :    ready-to-use high level calls

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#include "t5vm.h"

/*****************************************************************************
T5Def_Cycle
Execute a complete cycle including all IO and data exchange
Parameters:
    pDB (IN) pointer to a DB header
    dwTimeStamp (IN) current time stamp in milliseconds
    pEvData (IN) pointer to an instance of an event server
*****************************************************************************/

void T5Def_Cycle (T5PTR_DB pDB, T5_DWORD dwTimeStamp, T5_PTR pEvData)
{
    if (pDB == NULL)
        return;

    T5_DIAGHOOK (T5_DIAGHOOK_CYCLE_BEGIN, 0);

    /* 1- get external variables from binding */
#ifdef T5DEF_EA
    T5VM_UpdateExtData (pDB, dwTimeStamp);
#endif /*T5DEF_EA*/
    T5_DIAGHOOK (T5_DIAGHOOK_CYCLE_BINDCONS, 0);

    /* 2- exchange IOs */
    T5VM_FullIOExchangeEx (pDB, dwTimeStamp);
    T5_DIAGHOOK (T5_DIAGHOOK_CYCLE_IOS, 0);

    /* 3- activate MODBUS master connections */
#ifdef T5DEF_MODBUSIOS
    T5VM_ActivateModbus (pDB, dwTimeStamp);
    T5VM_ExchangeModbusData (pDB);
#endif /*T5DEF_MODBUSIOS*/
    T5_DIAGHOOK (T5_DIAGHOOK_CYCLE_MODBUS, 0);

    /* 4- performs the cycle */
    T5VM_Cycle (pDB, dwTimeStamp);

    /* 5- digital sampling trace recording */
#ifdef T5DEF_BSAMPLING
    T5VMBsmp_Activate (pDB, 0);
#endif /*T5DEF_BSAMPLING*/
    T5_DIAGHOOK (T5_DIAGHOOK_CYCLE_PROGS, 0);

    /* 6- produce change events for public variables (binding) */
#ifdef T5DEF_EA
    if (pEvData != NULL)
    {
        T5EA_ManageChangeEvents (pDB, pEvData);
    }
#endif /*T5DEF_EA*/

    /* 7- publish variables (binding) */
#ifdef T5DEF_EVENTSERVER
    if (pEvData != NULL)
    {
        T5Evs_Activate (pEvData, dwTimeStamp);
    }
#endif /*T5DEF_EVENTSERVER*/
    T5_DIAGHOOK (T5_DIAGHOOK_CYCLE_BINDPROD, 0);
}

/*****************************************************************************
T5Def_ColdStart
Perform a typical cold or warm start
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer to a DB header
    pRetData (IN) OEM defined parameter for RETAIN interface
    bWarm (IN) TRUE if RETAIN must be loaded (warm start)
    bCC (IN) TRUE if application must start in cycle stepping mode
    szAppName (IN) pointer to the name of the application
Return: ok or error
*****************************************************************************/

T5_RET T5Def_ColdStart (T5_PTR pCode, T5PTR_DB pDB,
                        T5_PTR pRetData, T5_PTR pEvData,
                        T5_BOOL bWarm, T5_BOOL bCC, T5_PTCHAR szAppName)
{
    T5_RET wRet;

    if (pCode == NULL || pDB == NULL)
        return T5RET_ERROR;

    /* build the application */
    wRet = T5VM_Build (pCode, pDB);
    if (wRet != T5RET_OK)
        return wRet;

    /* set running mode */
    if (bCC)
    {
        T5VM_SetPauseMode (pDB, TRUE);
    }

    /* prepare and load RETAIN data */
#ifdef T5DEF_RETAIN
    T5VM_PrepareRetains (pDB, pRetData);
    if (bWarm)
    {
        T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS (pDB);
        T5VM_LoadRetains (pDB, pRetData);
        pStatus->dwFlagsEx |= T5FLAGEX_WARM;
    }
#endif /*T5DEF_RETAIN*/

    /* open MODBUS ports */
#ifdef T5DEF_MODBUSIOS
    wRet = T5VM_OpenModbus (pDB);
    if (wRet != T5RET_OK)
    {
        T5_PRINTF ("Cannot open MODBUS client connection");
    }
#endif /*T5DEF_MODBUSIOS*/

    /* links to external data */
#ifdef T5DEF_EA
    wRet = T5VM_OpenExtData (pDB);
    if (wRet != T5RET_OK)
    {
        T5_PRINTF ("Cannot open connections to external variables");
    }
#endif /*T5DEF_EA*/

    /* prepare event logger */
#ifdef T5DEF_EA
    if (pEvData != NULL)
    {
        wRet = T5EA_Open (pCode, pDB, pEvData);
        if (wRet != T5RET_OK)
        {
            T5_PRINTF ("Cannot produce all public variables");
        }
    }
#endif /*T5DEF_EA*/

    /* system event */
#ifdef T5DEF_EVENTSERVER
    if (pEvData != NULL)
    {
        T5Evs_OnAppStart (pEvData, szAppName);
    }
#endif /*T5DEF_EVENTSERVER*/

    return T5RET_OK;
}

/*****************************************************************************
T5Def_HotStart
Perform a typical hot restart
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer to a DB header
    pRetData (IN) OEM defined parameter for RETAIN interface
    pEvData (IN) pointer to an instance of an event server
    bCC (IN) TRUE if application must start in cycle stepping mode
    szAppName (IN) pointer to the name of the application
    dwTimeStamp (IN) current time stamp in milliseconds
Return: ok or error
*****************************************************************************/

T5_RET T5Def_HotStart (T5_PTR pCode, T5PTR_DB pDB,
                       T5_PTR pRetData, T5_PTR pEvData,
                       T5_BOOL bCC, T5_PTCHAR szAppName,
                       T5_DWORD dwTimeStamp)
{
    T5_RET wRet;

    if (pCode == NULL || pDB == NULL)
        return T5RET_ERROR;

    wRet = T5VM_CanHOTStart (pCode, pDB);
    if (wRet != T5RET_OK)
        return wRet;

    wRet = T5VM_BuildHOT (pCode, pDB, dwTimeStamp);
    if (wRet != T5RET_OK)
        return wRet;

    /* prepare and load RETAIN data */
#ifdef T5DEF_RETAIN
    T5VM_PrepareRetains (pDB, pRetData);
    if (bCC)
    {
        T5VM_SetPauseMode (pDB, TRUE);
    }
#endif /*T5DEF_RETAIN*/

    /* open MODBUS ports and links to external data */
#ifdef T5DEF_MODBUSIOS
    wRet = T5VM_OpenModbus (pDB);
    if (wRet != T5RET_OK)
    {
        T5_PRINTF ("Cannot open MODBUS client connection");
    }
#endif /*T5DEF_MODBUSIOS*/

    /* links to external data */
#ifdef T5DEF_EA
    wRet = T5VM_OpenExtData (pDB);
    if (wRet != T5RET_OK)
    {
        T5_PRINTF ("Cannot open connections to external variables");
    }
#endif /*T5DEF_EA*/

    /* prepare event logger */
#ifdef T5DEF_EA
    if (pEvData != NULL)
    {
        wRet = T5EA_Open (pCode, pDB, pEvData);
        if (wRet != T5RET_OK)
        {
            T5_PRINTF ("Cannot produce all public variables");
        }
    }
#endif /*T5DEF_EA*/

    /* system event */
#ifdef T5DEF_EVENTSERVER
    if (pEvData != NULL)
    {
        T5Evs_OnAppStart (pEvData, szAppName);
    }
#endif /*T5DEF_EVENTSERVER*/

    return T5RET_OK;
}

/*****************************************************************************
T5Def_Change
Perform a typical On Line Change
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer to a DB header
    pEvData (IN) pointer to an instance of an event server
    szAppName (IN) pointer to the name of the application
    dwTimeStamp (IN) current time stamp in milliseconds
Return: ok or error
*****************************************************************************/

T5_RET T5Def_Change (T5_PTR pCode, T5PTR_DB pDB, T5_PTR pEvData,
                     T5_PTCHAR szAppName, T5_DWORD dwTimeStamp)
{
    T5_RET wRet;
    T5PTR_DBSTATUS pStatus;

    if (pCode == NULL || pDB == NULL)
        return T5RET_ERROR;

    pStatus = T5GET_DBSTATUS(pDB);

    /* close all MODBUS ports before Change */
#ifdef T5DEF_MODBUSIOS
    T5VM_CloseModbus (pDB);
#endif /*T5DEF_MODBUSIOS*/

    /* close all external links*/
#ifdef T5DEF_EA
    T5VM_CloseExtData (pDB);
#endif /*T5DEF_EA*/

    /* perform On Line Change */
    pStatus->dwFlagsEx |= T5FLAGEX_CHGCYC;
    wRet = T5VM_BuildHOT (pCode, pDB, dwTimeStamp);
    if (wRet != T5RET_OK)
        return wRet;
    pStatus->dwFlagsEx |= T5FLAGEX_CHGCYC;

    /* re-open MODBUS ports */
#ifdef T5DEF_MODBUSIOS
    wRet = T5VM_OpenModbus (pDB);
    if (wRet != T5RET_OK)
    {
        T5_PRINTF ("Cannot open MODBUS client connection");
    }
#endif /*T5DEF_MODBUSIOS*/

    /* re-open links to external data */
#ifdef T5DEF_EA
    wRet = T5VM_OpenExtData (pDB);
    if (wRet != T5RET_OK)
    {
        T5_PRINTF ("Cannot open connections to external variables");
    }
#endif /*T5DEF_EA*/

    /* update produced events */
#ifdef T5DEF_EA
    if (pEvData != NULL)
    {
        wRet = T5EA_Open (pCode, pDB, pEvData);
        if (wRet != T5RET_OK)
        {
            T5_PRINTF ("Cannot produce all public variables");
        }
    }
#endif /*T5DEF_EA*/

    /* system event */
#ifdef T5DEF_EVENTSERVER
    if (pEvData != NULL)
    {
        T5Evs_OnAppChange (pEvData, szAppName);
    }
#endif /*T5DEF_EVENTSERVER*/

    return T5RET_OK;
}

/*****************************************************************************
T5Def_UndoChange
Undo an On Line Change
Parameters:
    pCode (IN) pointer to previous the application code
    pDB (IN) pointer to a DB header
    pEvData (IN) pointer to an instance of an event server
    szAppName (IN) pointer to the name of the application
    dwTimeStamp (IN) current time stamp in milliseconds
Return: ok or error
*****************************************************************************/

T5_RET T5Def_UndoChange (T5_PTR pCode, T5PTR_DB pDB, T5_PTR pEvData,
                         T5_PTCHAR szAppName, T5_DWORD dwTimeStamp)
{
    T5_RET wRet;
    T5PTR_DBSTATUS pStatus;

    if (pCode == NULL || pDB == NULL)
        return T5RET_ERROR;

    pStatus = T5GET_DBSTATUS(pDB);

    /* close all MODBUS ports before Change */
#ifdef T5DEF_MODBUSIOS
    T5VM_CloseModbus (pDB);
#endif /*T5DEF_MODBUSIOS*/

    /* close all external links*/
#ifdef T5DEF_EA
    T5VM_CloseExtData (pDB);
#endif /*T5DEF_EA*/

    /* perform On Line Change */
    pStatus->dwFlagsEx |= T5FLAGEX_CHGCYC;
    wRet = T5VM_UndoHOTChange (pCode, pDB, dwTimeStamp);
    if (wRet != T5RET_OK)
        return wRet;
    pStatus->dwFlagsEx |= T5FLAGEX_CHGCYC;

    /* re-open MODBUS ports */
#ifdef T5DEF_MODBUSIOS
    wRet = T5VM_OpenModbus (pDB);
    if (wRet != T5RET_OK)
    {
        T5_PRINTF ("Cannot open MODBUS client connection");
    }
#endif /*T5DEF_MODBUSIOS*/

    /* re-open links to external data */
#ifdef T5DEF_EA
    wRet = T5VM_OpenExtData (pDB);
    if (wRet != T5RET_OK)
    {
        T5_PRINTF ("Cannot open connections to external variables");
    }
#endif /*T5DEF_EA*/

    /* update produced events */
#ifdef T5DEF_EA
    if (pEvData != NULL)
    {
        wRet = T5EA_Open (pCode, pDB, pEvData);
        if (wRet != T5RET_OK)
        {
            T5_PRINTF ("Cannot produce all public variables");
        }
    }
#endif /*T5DEF_EA*/

    /* system event */
#ifdef T5DEF_EVENTSERVER
    if (pEvData != NULL)
    {
        T5Evs_OnAppChange (pEvData, szAppName);
    }
#endif /*T5DEF_EVENTSERVER*/

    return T5RET_OK;
}

/*****************************************************************************
T5Def_Stop
Perform a typical stop of a T5 VM application
Parameters:
    pDB (IN) pointer to a DB header
    pEvData (IN) pointer to an instance of an event server
    pRetData (IN) OEM defined parameter for RETAIN interface
    szAppName (IN) pointer to the name of the application
Return: ok or error
*****************************************************************************/

void T5Def_Stop (T5PTR_DB pDB, T5_PTR pEvData, T5_PTR pRetData,
                 T5_PTCHAR szAppName)
{
    /* system event */
#ifdef T5DEF_EVENTSERVER
    if (pEvData != NULL)
        T5Evs_OnAppStop (pEvData, szAppName);
#endif /*T5DEF_EVENTSERVER*/

    /* close event logger */
#ifdef T5DEF_EA
    if (pEvData != NULL)
        T5EA_Close (pDB, pEvData);
#endif /*T5DEF_EA*/

    /* close all MODBUS ports */
#ifdef T5DEF_MODBUSIOS
    T5VM_CloseModbus (pDB);
#endif /*T5DEF_MODBUSIOS*/

    /* close links to external data */
#ifdef T5DEF_EA
    T5VM_CloseExtData (pDB);
#endif /*T5DEF_EA*/

    /* shut down: IOs and RETAIN variables */
    T5VM_ShutDown (pDB, pRetData);
}

/*****************************************************************************
T5Def_ServeVMRequest
Serve a communication request for the VM
Parameters:
    pCsData (IN) pointer to the communication server data
    pDB (IN) pointer to a DB header
    wCaller (IN) indetifier of the calling client
*****************************************************************************/

void T5Def_ServeVMRequest (T5PTR_CS pCSData, T5PTR_DB pDB, T5_WORD wCaller)
{
    T5_WORD wLen;
    T5_PTBYTE pIn, pOut;

    pIn = (T5_PTBYTE)T5CS_GetVMRequest (pCSData, wCaller);
    pOut = (T5_PTBYTE)T5CS_GetVMAnswerBuffer (pCSData, wCaller);
    if (pIn == NULL || pOut == NULL)
    {
        T5CS_SendRC (pCSData, wCaller, T5RET_ERROR);
        return;
    }

    wLen = T5VM_Serve (pDB, pIn, pOut);
    T5CS_SendVMAnswer (pCSData, wCaller, wLen);
}

void T5Def_ServeVMRequestEx (T5PTR_CS pCSData, T5PTR_DB pDB, T5_WORD wCaller, T5_WORD wEvPort)
{
    T5_WORD wLen;
    T5_PTBYTE pIn, pOut;

    pIn = (T5_PTBYTE)T5CS_GetVMRequest (pCSData, wCaller);
    pOut = (T5_PTBYTE)T5CS_GetVMAnswerBuffer (pCSData, wCaller);
    if (pIn == NULL || pOut == NULL)
    {
        T5CS_SendRC (pCSData, wCaller, T5RET_ERROR);
        return;
    }

    wLen = T5VM_ServeEx (pDB, pIn, pOut, wEvPort);
    T5CS_SendVMAnswer (pCSData, wCaller, wLen);
}

/*****************************************************************************
T5Def_ServeModbusRequest
Serve a MODBUS slave request
Parameters:
    pCsData (IN) pointer to the communication server data
    pDB (IN) pointer to a DB header
    wReq (IN) type of request (RTU or TCP)
    wCaller (IN) indetifier of the calling client
*****************************************************************************/

void T5Def_ServeModbusRequest (T5PTR_CS pCSData, T5PTR_DB pDB,
                               T5_WORD wReq, T5_WORD wCaller)
{
#ifdef T5DEF_MODBUSIOS
    T5_WORD wLen;
    T5_PTBYTE pIn, pOut;

    if (pDB == NULL)
    {
        T5CS_SendRC (pCSData, wCaller, T5RET_ERROR);
        return;
    }
    pIn = (T5_PTBYTE)T5CS_GetRequestFrame (pCSData, wCaller);
    pOut = (T5_PTBYTE)T5CS_GetAnswerFrameBuffer (pCSData, wCaller);
    if (pIn == NULL || pOut == NULL)
    {
        T5CS_SendRC (pCSData, wCaller, T5RET_ERROR);
        return;
    }
    wLen = T5VM_ServeModbus (pDB, wReq, pIn, pOut);
    T5CS_SendAnswerFrame (pCSData, wCaller, wLen);
#else /*T5DEF_MODBUSIOS*/
    T5CS_SendRC (pCSData, wCaller, T5RET_ERROR);
#endif /*T5DEF_MODBUSIOS*/
}

/*****************************************************************************
T5Def_ServeSyncRequest
Serve a request for code synchronization
Parameters:
    pCsData (IN) pointer to the communication server data
    pDB (IN) pointer to a DB header
    wCaller (IN) indetifier of the calling client
Return: ok or error
*****************************************************************************/

T5_RET T5Def_ServeSyncRequest (T5PTR_CS pCSData, T5PTR_DB pDB, T5_WORD wCaller)
{
    T5_PTBYTE pIn;
    T5PTR_DBSTATUS pStatus;

    if (pCSData == NULL || pDB == NULL)
        return T5RET_ERROR;

    pIn = (T5_PTBYTE)T5CS_GetRequestFrame (pCSData, wCaller);
    if (pIn[4] != T5CSRQ_SYNCCODE)
        return T5RET_ERROR;

    pStatus = T5GET_DBSTATUS(pDB);
    T5_COPYFRAMEDWORD (&(pStatus->dwAppVersion), pIn + 5);
    T5_COPYFRAMEDWORD (&(pStatus->dwAppDateStamp), pIn + 9);
    T5_COPYFRAMEDWORD (&(pStatus->dwAppCodeCRC), pIn + 13);
    /* pIn + 17 - CRC DATA */
    return T5RET_OK;
}

/* eof **********************************************************************/
