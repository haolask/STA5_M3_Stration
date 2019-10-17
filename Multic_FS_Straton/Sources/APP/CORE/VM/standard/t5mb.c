/*****************************************************************************
T5MB.c :     MODBUS core engine

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_MODBUSIOS

/****************************************************************************/
/* byte masks */

static T5_WORD _T5_MASK8[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

/****************************************************************************/
/* static services */

static T5PTR_MBCLIENT _T5MB_FindClient (T5_PTR pMB, T5_WORD wID);
static T5PTR_MBSLAVE _T5MB_FindSlave (T5PTR_MBCLIENT pClient, T5_WORD wSlaveNo);
static T5PTR_MBREQ _T5MB_FindReq (T5_PTR pMB, T5_WORD wID);
static void _T5MB_DispatchStatus (T5PTR_MBCLIENT pClient);
static T5_BOOL _T5MB_ActivateNextRequest (T5PTR_MBCLIENT pClient,
                                          T5_DWORD dwTimeStamp);
static T5_BOOL _T5MB_ExchangeInput (T5PTR_MBVAR pVar);
static void _T5MB_ExchangeVSI (T5PTR_MBVAR pVar);
static void _T5MB_ExchangeOutput (T5PTR_MBVAR pVar);
static void _T5MB_ExchangeOutput32 (T5PTR_MBVAR pVar);
static void _T5MB_ResetCmdVar (T5PTR_MBVAR pVar);
static void _T5MB_OnSlaveExchange (T5PTR_MBCLIENT pClient, T5PTR_MBREQ pReq);
static void _T5MB_ResetSlave (T5PTR_MBSLAVE pSlave);
static void _T5MB_ExchangeSlave (T5PTR_MBVAR pVar);
static T5_BOOL _T5MB_ExchangeInString (T5PTR_MBVAR pVar);
static void _T5MB_ExchangeOutString (T5PTR_MBVAR pVar);
static void _T5MB_ExchangeRAPSS (T5PTR_MBVAR pVar);

/*****************************************************************************
T5MB_GetMemorySizing
Returs the full size required for MODBUS data
Parameters:
    pCode (IN) static definition of MODBUS exchanges
Return: number of bytes to be allocated
*****************************************************************************/

T5_BOOL _T5MB_IsSrvID (T5_WORD wID, T5_PTWORD pIDs)
{
    T5_WORD i;

    for (i=0; i<32; i++)
    {
        if (pIDs[i] == wID)
            return TRUE;
    }
    return FALSE;
}

T5_DWORD T5MB_GetMemorySizing (T5_PTR pCode)
{
    T5_PTBYTE pScan;
    T5_DWORD dwSize, dwAllocSize;
    T5PTR_MBDEF pDef;
    T5PTR_MBDEFREQ pReq;
    T5_WORD wSrvID[32], wNbSrv;
    T5PTR_MBDEFNODE pNode;
    
    if (pCode == NULL)
        return sizeof (T5STR_MBHEAD);

    dwAllocSize = ((T5PTR_MBDEFHEAD)pCode)->dwAllocSize;
    pScan = (T5_PTBYTE)pCode;
    pScan += sizeof (T5STR_MBDEFHEAD);
    dwSize = sizeof (T5STR_MBHEAD);
    T5_MEMSET (wSrvID, 0xff, sizeof (wSrvID));
    wNbSrv = 0;
    while (*(T5_PTWORD)pScan != 0)
    {
        pDef = (T5PTR_MBDEF)pScan;
        switch (pDef->wDefID)
        {
        case T5MBDEF_NODE :
            pNode = (T5PTR_MBDEFNODE)pScan;
            if ((pNode->wFlags & T5MBDEF_SERVER) != 0 && wNbSrv < 32)
                wSrvID[wNbSrv++] = pNode->wID;
            dwSize += sizeof (T5STR_MBCLIENT);
            break;
        case T5MBDEF_REQ :
            dwSize += sizeof (T5STR_MBREQ);
            pReq = (T5PTR_MBDEFREQ)pScan;
            dwSize += pReq->wDataSize;

#ifdef T5DEF_MBSREADQ
            if (_T5MB_IsSrvID (pReq->wNodeID, wSrvID)
                && (pReq->wFunc == T5MBF_CB || pReq->wFunc == T5MBF_HW))
                dwSize += pReq->wDataSize;
#endif /*T5DEF_MBSREADQ*/

            break;
        case T5MBDEF_SLAVE :
            dwSize += sizeof (T5STR_MBSLAVE);
            break;
        case T5MBDEF_VAR :
        case T5MBDEF_VARCT :
        case T5MBDEF_SVAR :
        case T5MBDEF_SVARCT :
            dwSize += sizeof (T5STR_MBVAR);
            break;
        default : break;
        }
        pScan += *(T5_PTWORD)pScan;
    }
    dwSize = T5_ALIGNED (dwSize);
    return (dwSize > dwAllocSize) ? dwSize : dwAllocSize;
}

/*****************************************************************************
_T5MB_FindClient
Find a client descriptor by its ID
Parameters:
    pMB (IN) pointer to MODBUS data
    wID (IN) wished client ID
Return: pointer to found client descriptor or NULL if not found
*****************************************************************************/

static T5PTR_MBCLIENT _T5MB_FindClient (T5_PTR pMB, T5_WORD wID)
{
    T5PTR_MBCLIENT pClient, pFound;
    T5PTR_MBHEAD pHeader;
    T5_WORD i;

    pFound = NULL;
    pHeader = (T5PTR_MBHEAD)pMB;
    pClient = pHeader->pClient;
    for (i=0; pFound==NULL && i<pHeader->wNbClient; i++, pClient++)
        if (pClient->wID == wID)
            pFound = pClient;
    return pFound;
}

/*****************************************************************************
_T5MB_FindReq
Find a request descriptor by its ID
Parameters:
    pMB (IN) pointer to MODBUS data
    wID (IN) wished request ID
Return: pointer to found request descriptor or NULL if not found
*****************************************************************************/

static T5PTR_MBREQ _T5MB_FindReq (T5_PTR pMB, T5_WORD wID)
{
    T5PTR_MBREQ pReq, pFound;
    T5PTR_MBHEAD pHeader;
    T5_WORD i;

    pFound = NULL;
    pHeader = (T5PTR_MBHEAD)pMB;
    pReq = pHeader->pReq;
    for (i=0; pFound==NULL && i<pHeader->wNbReq; i++, pReq++)
        if (pReq->pDef->wID == wID)
            pFound = pReq;
    return pFound;
}

/*****************************************************************************
_T5MB_FindSlave
Find a slave object
Parameters:
    pClient (IN) channel object
    wSlaveNo (IN) serached slave number
Return: pointer to found slave or NULL if not found
*****************************************************************************/

static T5PTR_MBSLAVE _T5MB_FindSlave (T5PTR_MBCLIENT pClient, T5_WORD wSlaveNo)
{
    T5PTR_MBSLAVE pSlave;
    T5_WORD i;

    pSlave = pClient->pSlave;
    if (pSlave == NULL)
        return NULL;

    for (i=0; i<pClient->wNbSlave; i++, pSlave++)
    {
        if (pSlave->wSlaveNo == wSlaveNo)
            return pSlave;
    }
    return NULL;
}

/*****************************************************************************
T5MB_Build
Construct MODBUS data
Parameters:
    pCode (IN) static definition of MODBUS exchanges
    pMB (IN) pointer to MODBUS data
    dwFullSize (IN) full raw size of MODBUS data buffer
Return: OK or error
*****************************************************************************/

static T5_RET _T5MB_BuildEx (T5_PTR pCode, T5_PTR pMB,
                             T5_DWORD dwFullSize, T5_BOOL bChange)
{
    T5_PTBYTE pScan, pData;
    T5PTR_MBDEF pDef;
    T5PTR_MBHEAD pHeader;
    T5PTR_MBCLIENT pClient;
    T5PTR_MBREQ pReq;
    T5PTR_MBSLAVE pSlave;
    T5PTR_MBVAR pVar;
    T5_WORD i;
    T5_UNUSED(bChange);

    if (pMB == NULL)
        return T5RET_OK;

    if (pCode == NULL)
    {
        pHeader = (T5PTR_MBHEAD)pMB;
        pHeader->dwAllocSize = dwFullSize;
        T5_MEMSET (pMB, 0, dwFullSize);
        return T5RET_OK;
    }

    T5_MEMSET (pMB, 0, dwFullSize);
    /* header */
    pScan = (T5_PTBYTE)pCode;
    pData = (T5_PTBYTE)pMB;
    pHeader = (T5PTR_MBHEAD)pMB;
    pHeader->dwAllocSize = dwFullSize;
    pScan += sizeof (T5STR_MBDEFHEAD);
    pData += sizeof (T5STR_MBHEAD);
    /* records */
    while (*(T5_PTWORD)pScan != 0)
    {
        pDef = (T5PTR_MBDEF)pScan;
        switch (pDef->wDefID)
        {
        case T5MBDEF_NODE :
            pClient = (T5PTR_MBCLIENT)pData;
            if (pHeader->wNbClient == 0)
                pHeader->pClient = pClient;
            pHeader->wNbClient += 1;
            pClient->pDef = (T5PTR_MBDEFNODE)pScan;
            pClient->wID = pClient->pDef->wID;
            pClient->wFlags = pClient->pDef->wFlags;
            if (pHeader->pServer == NULL && (pClient->pDef->wFlags & T5MBDEF_SERVER) != 0)
                pHeader->pServer = pClient;
            if (pClient->pDef->wFlags & T5MBDEF_SERIAL)
                pClient->wDriver = T5MB_RS232;
            else
                pClient->wDriver = T5MB_TCP;
            pData += sizeof (T5STR_MBCLIENT);
            break;
        case T5MBDEF_SLAVE :
            pSlave = (T5PTR_MBSLAVE)pData;
            if (pHeader->wNbSlave == 0)
                pHeader->pSlave = pSlave;
            pHeader->wNbSlave += 1;
            pSlave->wSlaveNo = ((T5PTR_MBDEFSLAVE)pScan)-> wSlaveNo;
            pClient = _T5MB_FindClient (pMB, ((T5PTR_MBDEFSLAVE)pScan)->wNodeID);
            if (pClient != NULL)
            {
                if (pClient->wNbSlave == 0)
                    pClient->pSlave = pSlave;
                pClient->wNbSlave += 1;
            }
            pData += sizeof (T5STR_MBSLAVE);
            break;
        case T5MBDEF_REQ :
            pReq = (T5PTR_MBREQ)pData;
            pClient = _T5MB_FindClient (pMB, ((T5PTR_MBDEFREQ)pScan)->wNodeID);
            if (pClient != NULL)
            {
                if (pHeader->wNbReq == 0)
                    pHeader->pReq = pReq;
                pHeader->wNbReq += 1;
                pReq->pDef = (T5PTR_MBDEFREQ)pScan;
                pReq->pSlave = _T5MB_FindSlave (pClient, pReq->pDef->wSlave);
                pReq->bServer = ((pClient->pDef->wFlags & T5MBDEF_SERVER) != 0);
                pReq->bChange = (pReq->bServer) ? FALSE : TRUE;
                if (pReq->pDef->wMode == T5MBMODE_PERIODIC
                    || pReq->pDef->wMode == T5MBMODE_ONCHANGE)
                    pReq->bActive = TRUE;
                if (pClient->wNbReq == 0)
                    pClient->pReq = pReq;
                pClient->wNbReq += 1;
            }
            pData += sizeof (T5STR_MBREQ);
            break;
        case T5MBDEF_VAR :
        case T5MBDEF_VARCT :
        case T5MBDEF_SVAR :
        case T5MBDEF_SVARCT :
            pVar = (T5PTR_MBVAR)pData;
            pReq = _T5MB_FindReq (pMB, ((T5PTR_MBDEFVAR)pScan)->wReqID);
            if (pReq != NULL)
            {
                if (pHeader->wNbVar == 0)
                    pHeader->pVar = pVar;
                pHeader->wNbVar += 1;
                pVar->pDef = (T5PTR_MBDEFVAR)pScan;
                pVar->pReq = pReq;
            }
            pData += sizeof (T5STR_MBVAR);
            break;
        default : break;
        }
        pScan += *(T5_PTWORD)pScan;
    }
    /* request data buffers */
    pHeader->pReqData = pData;
    pReq = pHeader->pReq;
    for (i=0; i<pHeader->wNbReq; i++, pReq++)
    {
        pReq->pData = pData;
        pData += pReq->pDef->wDataSize;
#ifdef T5DEF_MBSREADQ
        if (T5MB_ISREQSH(pReq)) /* update flag for server Write */
            pData += pReq->pDef->wDataSize;
#endif /*T5DEF_MBSREADQ*/
    }
    return T5RET_OK;
}

T5_RET T5MB_Build (T5_PTR pCode, T5_PTR pMB, T5_DWORD dwFullSize)
{
    return _T5MB_BuildEx (pCode, pMB, dwFullSize, FALSE);
}

/*****************************************************************************
T5MB_Link
Link MODBUS data to VM database
Parameters:
    pMB (IN) pointer to MODBUS data
    pDB (IN) pointer to VM database
Return: OK or error
*****************************************************************************/

T5_RET T5MB_Link (T5_PTR pMB, T5PTR_DB pDB)
{
    T5PTR_MBHEAD pHeader;
    T5PTR_MBVAR pVar;
    T5_WORD i;
    T5_PTBYTE pLockBase;
    T5_PTBYTE pD8;
#ifdef T5DEF_CTSEG
    T5_DWORD dwOffset;
#endif /*T5DEF_CTSEG*/
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pD16;
#endif /*T5DEF_DATA16SUPPORTED*/
    T5_PTDWORD pD32;
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 pD64;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE *pString;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_XV
    T5_PTBYTE *pXV;
#endif /*T5DEF_XV*/

    if (pDB == NULL || pMB == NULL)
        return T5RET_OK;

    pLockBase = T5GET_DBLOCK(pDB);
    pHeader = (T5PTR_MBHEAD)pMB;
    pVar = pHeader->pVar;
    if (pVar == NULL)
        return T5RET_OK;

    for (i=0; i<pHeader->wNbVar; i++, pVar++)
    {
        switch (pVar->pReq->pDef->wFunc)
        {
        case T5MBF_RIB :
        case T5MBF_RCB :
        case T5MBF_W1B :
        case T5MBF_WNB :
            pVar->wOffset = pVar->pDef->wOffset / 8;
            pVar->wMask = _T5_MASK8[pVar->pDef->wOffset % 8];
            pVar->bBit = TRUE;
            break;
        default :
            pVar->wOffset = pVar->pDef->wOffset * 2;
            pVar->wMask = pVar->pDef->wMask;
            pVar->bBit = FALSE;
            break;
        }
        pVar->wOpe = pVar->pDef->wOpe;
        pVar->pData = NULL;
#ifndef T5DEF_MB_NOLOCK
        pVar->pLock = pLockBase + pVar->pDef->wT5Offset;
#endif /*T5DEF_MB_NOLOCK*/

        if (pVar->pDef->wDefID == T5MBDEF_VARCT
            || pVar->pDef->wDefID == T5MBDEF_SVARCT)
        {
            pVar->bLockMask = 0; /* no mask supported */
#ifdef T5DEF_CTSEG
            pVar->pData = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
            /* take care of possible wrong alignment! */
            T5_MEMCPY (&dwOffset, &(pVar->pDef->wCTOffset1), sizeof (T5_DWORD));
            pVar->pData += dwOffset;
#endif /*T5DEF_CTSEG*/
        }
        else if (pVar->pDef->wT5Type & T5C_EXTERN)
        {
            pVar->bLockMask = T5LOCK_XV;
#ifdef T5DEF_XV
            pXV = T5GET_DBXV(pDB);
            pVar->pData = (T5_PTBYTE)(pXV[pVar->pDef->wT5Offset]);
#endif /*T5DEF_XV*/
        }
        else
        {
            switch (pVar->pDef->wT5Type)
            {
            case T5C_BOOL :
            case T5C_SINT :
            case T5C_USINT :
                pVar->bLockMask = T5LOCK_DATA8;
                pD8 = T5GET_DBDATA8(pDB);
                pVar->pData = (T5_PTBYTE)(pD8 + pVar->pDef->wT5Offset);
                break;
            case T5C_INT :
            case T5C_UINT :
                pVar->bLockMask = T5LOCK_DATA16;
#ifdef T5DEF_DATA16SUPPORTED
                pD16 = T5GET_DBDATA16(pDB);
                pVar->pData = (T5_PTBYTE)(pD16 + pVar->pDef->wT5Offset);
#endif /*T5DEF_DATA16SUPPORTED*/
                break;
            case T5C_DINT :
            case T5C_REAL :
            case T5C_UDINT :
                pVar->bLockMask = T5LOCK_DATA32;
                pD32 = T5GET_DBDATA32(pDB);
                pVar->pData = (T5_PTBYTE)(pD32 + pVar->pDef->wT5Offset);
                break;
            case T5C_TIME :
                pVar->bLockMask = T5LOCK_TIME;
                pD32 = T5GET_DBTIME(pDB);
                pVar->pData = (T5_PTBYTE)(pD32 + pVar->pDef->wT5Offset);
                break;
            case T5C_LINT :
            case T5C_ULINT :
            case T5C_LREAL :
                pVar->bLockMask = T5LOCK_DATA64;
#ifdef T5DEF_DATA64SUPPORTED
                pD64 = T5GET_DBDATA64(pDB);
                pVar->pData = (T5_PTBYTE)(pD64 + pVar->pDef->wT5Offset);
#endif /*T5DEF_DATA64SUPPORTED*/
                break;
            case T5C_STRING :
                pVar->bLockMask = T5LOCK_STRING;
#ifdef T5DEF_STRINGSUPPORTED
                pString = T5GET_DBSTRING(pDB);
                pVar->pData = (T5_PTBYTE)(pString[pVar->pDef->wT5Offset]);
#endif /*T5DEF_STRINGSUPPORTED*/
                break;
            default : break;
            }
        }
#ifdef T5DEF_VARMAP
#ifndef T5DEF_MB_NOVSI
        pVar->pVSI = T5Map_GetVSIByData (pDB, pVar->pData);
#endif /*T5DEF_MB_NOVSI*/
#endif /*T5DEF_VARMAP*/
    }
    return T5RET_OK;
}

/*****************************************************************************
T5MB_CanBuildHOT
Check if MODBUS data can be constructed on hot-restart data
Parameters:
    pCode (IN) static definition of MODBUS exchanges
    pMB (IN) pointer to existing MODBUS database
Return: OK or error
*****************************************************************************/

T5_RET T5MB_CanBuildHOT (T5_PTR pCode, T5_PTR pMB)
{
    T5_DWORD dwSize;

    if (pMB == NULL)
        return T5RET_ERROR;

    dwSize = *(T5_DWORD *)pMB;

    if (T5MB_GetMemorySizing (pCode) > dwSize)
        return T5RET_ERROR;

    return T5RET_OK;
}

/*****************************************************************************
T5MB_BuildHOT
Construct MODBUS on hot-restart data
Parameters:
    pCode (IN) static definition of MODBUS exchanges
    pMB (IN) pointer to existing MODBUS database
Return: OK or error
*****************************************************************************/

T5_RET T5MB_BuildHOT (T5_PTR pCode, T5_PTR pMB)
{
    T5_DWORD dwSize;

    if (pMB == NULL)
        return T5RET_ERROR;

    dwSize = *(T5_DWORD *)pMB;
    return _T5MB_BuildEx (pCode, pMB, dwSize, TRUE);
}

/*****************************************************************************
_T5MB_DispatchStatus
Dispatch client status and error code to all dependent requests
Parameters:
    pClient (IN) pointer to teh client descriptor
*****************************************************************************/

static void _T5MB_DispatchStatus (T5PTR_MBCLIENT pClient)
{
    T5PTR_MBREQ pReq;
    T5_WORD i;
    T5PTR_MBSLAVE pSlave;
    T5_DWORD dwDate, dwTime;

    pReq = pClient->pReq;
    for (i=0; i<pClient->wNbReq; i++, pReq++)
    {
        pReq->wStatus = pClient->frame.wStatus;
        pReq->wError = pClient->frame.wError;
        if (pClient->frame.wError)
        {
            pReq->bChange = FALSE;
            pReq->bShot = 0xff;
        }
    }

    if (pClient->frame.wError == 0)
        return;
    pSlave = pClient->pSlave;
    if (pSlave == NULL)
        return;

#ifdef T5DEF_RTCLOCKSTAMP
        dwDate = T5RtClk_GetCurDateStamp ();
        dwTime = T5RtClk_GetCurTimeStamp ();
#else /*T5DEF_RTCLOCKSTAMP*/
        dwDate = 0L;
        dwTime = 0L;
#endif /*T5DEF_RTCLOCKSTAMP*/
    for (i=0; i<pClient->wNbSlave; i++, pSlave++)
    {
        pSlave->wLastError = pClient->frame.wError;
        pSlave->dwDateLastError = dwDate;
        pSlave->dwTimeLastError = dwTime;
    }
}

/*****************************************************************************
T5MB_Open
Open all MODBUS ports
Parameters:
    pMB (IN) pointer to existing MODBUS database
Return: OK or error
*****************************************************************************/

T5_RET T5MB_Open (T5_PTR pMB)
{
    T5_WORD iClient;
    T5PTR_MBHEAD pHeader;
    T5PTR_MBCLIENT pClient;

    if (pMB == NULL)
        return T5RET_OK;
    pHeader = (T5PTR_MBHEAD)pMB;
    pClient = pHeader->pClient;
    if (pClient == NULL)
        return T5RET_OK;
    for (iClient=0; iClient<pHeader->wNbClient; iClient++, pClient++)
    {
        if ((pClient->pDef->wFlags & T5MBDEF_SERVER) == 0)
        {
            T5MBComm_Open (pClient);
            _T5MB_DispatchStatus (pClient);
        }
    }

    T5MBSrv_SetDevIdent ((T5_PTCHAR)"\0", (T5_PTCHAR)"\0", (T5_PTCHAR)"\0", NULL, 0);

    return T5RET_OK;
}

/*****************************************************************************
T5MB_Close
Close all MODBUS ports
Parameters:
    pMB (IN) pointer to existing MODBUS database
*****************************************************************************/

void T5MB_Close (T5_PTR pMB)
{
    T5_WORD iClient;
    T5PTR_MBHEAD pHeader;
    T5PTR_MBCLIENT pClient;

    if (pMB == NULL)
        return;
    pHeader = (T5PTR_MBHEAD)pMB;
    pClient = pHeader->pClient;
    if (pClient == NULL)
        return;
    for (iClient=0; iClient<pHeader->wNbClient; iClient++, pClient++)
    {
        if ((pClient->wFlags & T5MBDEF_SERVER) == 0)
            T5MBComm_Close (pClient);
    }
    T5MBSrv_SetDevIdent ((T5_PTCHAR)"\0", (T5_PTCHAR)"\0", (T5_PTCHAR)"\0", NULL, 0);
}

/*****************************************************************************
T5MB_Activate
Operate exchanges on all MODBUS ports
Parameters:
    pMB (IN) pointer to existing MODBUS database
    dwTimeStamp (IN) current time stamp
*****************************************************************************/

#ifdef T5DEF_MBMASTER_SMHOOK
extern T5_BOOL T5MBHook_ProcessOemReq (T5PTR_MBCLIENT pClient);
#endif /*T5DEF_MBMASTER_SMHOOK*/

void T5MB_Activate (T5_PTR pMB, T5_DWORD dwTimeStamp)
{
    T5_WORD iClient;
    T5PTR_MBHEAD pHeader;
    T5PTR_MBCLIENT pClient;
    T5_DWORD dwReqDelay;

    if (pMB == NULL)
        return;
    pHeader = (T5PTR_MBHEAD)pMB;
    pClient = pHeader->pClient;
    if (pClient == NULL)
        return;

    for (iClient=0; iClient<pHeader->wNbClient; iClient++, pClient++)
    {
        dwReqDelay = (T5_DWORD)(pClient->pDef->wReqDelay) & 0xffffL;
        if ((pClient->pDef->wFlags & T5MBDEF_SERVER) == 0)
        {
            switch (pClient->frame.wStatus)
            {
            case T5MB_CONNECTING :
            case T5MB_RECONNECT :
                T5MBComm_CheckPendingConnect (pClient);
                if (pClient->frame.wStatus == T5MB_READY)
                {
                    _T5MB_DispatchStatus (pClient);
                    /* continue and activate... */
                }
                else
                {
                    break;
                }
            case T5MB_READY :
                if (!_T5MB_ActivateNextRequest (pClient, dwTimeStamp))
                    break;
                T5MBFram_Build (&(pClient->frame), pClient->wDriver,
                                pClient->pReq[pClient->wReq].pDef->wSlave,
                                &(pClient->pReq[pClient->wReq]));
                pClient->frame.wStatus = T5MB_SENDING;
                /*T5_PRINTF ("SEND FRAME (%u)", dwTimeStamp);*/
                pClient->pReq[pClient->wReq].wStatus = T5MB_SENDING;
                /* continue: sending */
            case T5MB_SENDING :
                T5MBComm_Send (pClient);
                if (pClient->frame.wStatus == T5MB_ERROR)
                    _T5MB_DispatchStatus (pClient);
                else if (pClient->frame.wStatus == T5MB_SENT)
                {
                    if (pClient->wDriver == T5MB_RS232
                        && pClient->frame.buffer[0] == 0) /* broadcast */
                    {
                        pClient->pReq[pClient->wReq].wTry = 0;
                        pClient->pReq[pClient->wReq].wStatus = T5MB_READY;
                        pClient->pReq[pClient->wReq].wNbOK += 1;
                        pClient->pReq[pClient->wReq].bShot = 0xff;
                        pClient->pReq[pClient->wReq].bChange = FALSE;
                        _T5MB_OnSlaveExchange (pClient, &(pClient->pReq[pClient->wReq]));
#ifdef T5DEF_RTCLOCKSTAMP
                        pClient->pReq[pClient->wReq].dwDt = T5RtClk_GetCurDateStamp ();
                        pClient->pReq[pClient->wReq].dwTm = T5RtClk_GetCurTimeStamp ();
#endif /*T5DEF_RTCLOCKSTAMP*/

                        pClient->wReq += 1;
                        if (pClient->wReq < pClient->wNbReq)
                            pClient->pReq[pClient->wReq].wTry = 0;
                        else
                            pClient->pReq[0].wTry = 0;
                        pClient->frame.wStatus = T5MB_READY;
                        pClient->frame.wError = 0;

                    }
                    else
                    {
                        pClient->frame.wLenCur = 0;
                        pClient->frame.wSize = 0;
                        pClient->frame.dwDate = dwTimeStamp;
                        pClient->frame.wStatus = T5MB_RECEIVING;
                        pClient->frame.wError = 0;
                    }
                }
                break;
            case T5MB_RECEIVING :
                T5MBComm_Receive (pClient, dwTimeStamp);
                if (pClient->frame.wStatus == T5MB_ERROR)
                    _T5MB_DispatchStatus (pClient);
                else if (pClient->frame.wStatus == T5MB_RECEIVED)
                {
                    if (pClient->frame.wError
                        && pClient->pReq[pClient->wReq].wTry < 
                                (pClient->pReq[pClient->wReq].pDef->wRetry - 1))
                    {
                        if (pClient->pReq[pClient->wReq].bShot)
                            pClient->pReq[pClient->wReq].bActive = TRUE;
                        pClient->pReq[pClient->wReq].wTry += 1;
                        pClient->frame.wStatus = T5MB_READY;
                        pClient->frame.wError = 0;
                    }
                    else
                    {
                        T5MBFram_Extract (&(pClient->frame), pClient->wDriver,
                                       pClient->pReq[pClient->wReq].pDef->wSlave,
                                          &(pClient->pReq[pClient->wReq]));
                        pClient->pReq[pClient->wReq].wTry = 0;
                        pClient->pReq[pClient->wReq].bShot = 0xff;
                        pClient->pReq[pClient->wReq].bChange = FALSE;
                        _T5MB_OnSlaveExchange (pClient, &(pClient->pReq[pClient->wReq]));

                        pClient->wReq += 1;
                        if (pClient->wReq < pClient->wNbReq)
                            pClient->pReq[pClient->wReq].wTry = 0;
                        else
                            pClient->pReq[0].wTry = 0;
                        pClient->frame.wError = 0;
#ifndef T5DEF_MBMASTER_SMHOOK
                        if (dwReqDelay == 0L)
                            pClient->frame.wStatus = T5MB_READY;
                        else
#endif /*T5DEF_MBMASTER_SMHOOK*/
                        {
                            pClient->dwReqDelay = dwTimeStamp;
                            pClient->frame.wStatus = T5MB_REQDELAY;
                        }
                    }
                }
                break;
            case T5MB_REQDELAY :
#ifdef T5DEF_MBMASTER_SMHOOK
                if (!T5MBHook_ProcessOemReq (pClient))
#endif /*T5DEF_MBMASTER_SMHOOK*/
                {
                    if (T5_BOUNDTIME(dwTimeStamp - pClient->dwReqDelay) >= dwReqDelay)
                        pClient->frame.wStatus = T5MB_READY;
                }
                break;
            case T5MB_ERROR :
                if ((pClient->pDef->wFlags & T5MBDEF_RECONNECT) != 0)
                {
                    if (pClient->pDef->wCnxDelay <= 1)
                    {
                        pClient->frame.wStatus = T5MBComm_Open (pClient);
                        if (pClient->frame.wStatus == T5MB_CONNECTING)
                            pClient->frame.wStatus = T5MB_RECONNECT;
                    }
                    else
                    {
                        pClient->dwCnxDelay = dwTimeStamp;
                        pClient->frame.wStatus = T5MB_CNXDELAY;
                    }
                }
                _T5MB_DispatchStatus (pClient);
                break;
            case T5MB_CNXDELAY :
                if (T5_BOUNDTIME(dwTimeStamp - pClient->dwCnxDelay)
                        >= ((T5_DWORD)(pClient->pDef->wCnxDelay)))
                {
                    pClient->frame.wStatus = T5MBComm_Open (pClient);
                    if (pClient->frame.wStatus == T5MB_CONNECTING)
                    {
                        pClient->frame.wStatus = T5MB_RECONNECT;
                        _T5MB_DispatchStatus (pClient);
                    }
                    else
                        pClient->dwCnxDelay = dwTimeStamp;
                }
                break;
            default :
                break;
            }
        }
    }
}
/*****************************************************************************
_T5MB_ActivateNextRequest
Get the next request to be activated
Parameters:
    pClient (IN) pointer to the parent communication port
    dwTimeStamp (IN) current time stamp
Return: TRUE if new request activated (FALSE = standby)
*****************************************************************************/

static T5_BOOL _T5MB_ActivateNextRequest (T5PTR_MBCLIENT pClient,
                                          T5_DWORD dwTimeStamp)
{
    T5PTR_MBREQ pReq;
    T5_WORD wFirstReq;
    T5_DWORD dwPeriod;

    if (pClient->wNbReq == 0)
        return FALSE;

    if (pClient->wReq >= pClient->wNbReq)
        pClient->wReq = 0;
    wFirstReq = pClient->wReq;

    while (1)
    {
        pReq = &(pClient->pReq[pClient->wReq]);

        if ((pReq->wError != 0 || pReq->wTry > 0)
            && pReq->pDef->wMode == T5MBMODE_ONCHANGE)
        {
            if (!T5MB_ISREQ_V2(pReq->pDef))
                pReq->bActive = TRUE;
            else
            {
                dwPeriod = (T5_DWORD)(pReq->pDef->wErrPeriod);
                if (T5_BOUNDTIME(dwTimeStamp - pReq->dwDate) >= dwPeriod)
                {
                    /*T5_PRINTF ("Ract - spent=%u - per=%u", T5_BOUNDTIME(dwTimeStamp - pReq->dwDate), dwPeriod);*/
                    pReq->dwDate = dwTimeStamp;
                    pReq->bActive = TRUE;
                }
            }
        }

        dwPeriod = (T5_DWORD)(pReq->pDef->wPeriod);
        if (pReq->wError && T5MB_ISREQ_V2(pReq->pDef))
            dwPeriod = (T5_DWORD)(pReq->pDef->wErrPeriod);

        if (pReq->bActive == TRUE
            && (pReq->pDef->wMode != T5MBMODE_PERIODIC
                || pReq->dwDate == 0
                || T5_BOUNDTIME(dwTimeStamp - pReq->dwDate) >= dwPeriod))
        {
            pReq->dwDate = dwTimeStamp;
            if (pReq->pDef->wMode != T5MBMODE_PERIODIC)
                pReq->bActive = FALSE;
            return TRUE;
        }
        else
        {
            pClient->wReq += 1;
            if (pClient->wReq >= pClient->wNbReq)
                pClient->wReq = 0;
            if (pClient->wReq == wFirstReq)
                return FALSE;
        }
    }
}

/*****************************************************************************
_T5MB_ResetServerchanges
Reset "changed" marks in the server
Parameters:
    pMB (IN) pointer to existing MODBUS database
    pDB (IN) pointer to VM database
*****************************************************************************/

static void _T5MB_ResetServerchanges (T5_PTR pMB, T5PTR_DB pDB)
{
#ifdef T5DEF_MBSREADQ
    T5PTR_MBHEAD pHeader;
    T5PTR_MBREQ pReq;
    T5_PTBYTE pData;
    T5_WORD i;
    T5_UNUSED(pDB);

    if (pMB == NULL)
        return;

    pHeader = (T5PTR_MBHEAD)pMB;
    pReq = pHeader->pReq;
    for (i=0; i<pHeader->wNbReq; i++, pReq++)
    {
        if (T5MB_ISREQSH(pReq))
        {
            pData = pReq->pData;
            pData += pReq->pDef->wDataSize;
            T5_MEMSET(pData, 0, pReq->pDef->wDataSize);
        }
    }
#endif /*T5DEF_MBSREADQ*/
}

/*****************************************************************************
T5MB_Exchange
Exchanges MODBUS data with VM database
Parameters:
    pMB (IN) pointer to existing MODBUS database
    pDB (IN) pointer to VM database
*****************************************************************************/

void T5MB_Exchange (T5_PTR pMB, T5PTR_DB pDB)
{
    T5PTR_MBHEAD pHeader;
    T5PTR_MBVAR pVar;
    T5_WORD i;
    T5_BOOL bDone;

    if (pMB == NULL)
        return;
    pHeader = (T5PTR_MBHEAD)pMB;
    pVar = pHeader->pVar;
    if (pVar == NULL)
        return;
    for (i=0; i<pHeader->wNbVar; i++, pVar++)
    {
        bDone = FALSE;
        if (pVar->pReq != NULL && pVar->pData != NULL)
        {
            switch (pVar->wOpe)
            {
            case T5MBVAR_SLVERROR :
            case T5MBVAR_SLVDTERROR :
            case T5MBVAR_SLVTMERROR :
            case T5MBVAR_SLVDTRESET :
            case T5MBVAR_SLVTMRESET :
            case T5MBVAR_SLVNBTRANS :
            case T5MBVAR_SLVNBFAIL :
            case T5MBVAR_SLVRESET :
                if (!pVar->pReq->bServer)
                    _T5MB_ExchangeSlave (pVar);
                break;
            case T5MBVAR_STATUS :
            case T5MBVAR_STATACK :
            case T5MBVAR_TRIAL :
            case T5MBVAR_NBOK :
            case T5MBVAR_NBFAIL :
            case T5MBVAR_GOING :
                _T5MB_ExchangeInput (pVar);
                break;
            case T5MBVAR_INPUT :
                if (pVar->pReq->wStatus == T5MB_READY)
                    bDone = _T5MB_ExchangeInput (pVar);
                if (!pVar->pReq->bServer)
                    bDone = TRUE;
#ifndef T5DEF_MB_NOVSI
                if (bDone && pVar->pVSI != NULL)
                    _T5MB_ExchangeVSI (pVar);
#endif /*T5DEF_MB_NOVSI*/
                break;
            case T5MBVAR_OUTPUT :
                if (pVar->pReq->wStatus == T5MB_READY
                    || pVar->pReq->wStatus == T5MB_ERROR
                    || pVar->pReq->bChange)
                {
                    if ((pVar->pDef->wNbWrd & 0xf) == 2)
                        _T5MB_ExchangeOutput32 (pVar);
                    else
                        _T5MB_ExchangeOutput (pVar);
                }
                break;
            case T5MBVAR_INSTRING :
                if (pVar->pReq->wStatus == T5MB_READY)
                    bDone = _T5MB_ExchangeInString (pVar);
                if (!pVar->pReq->bServer)
                    bDone = TRUE;
#ifndef T5DEF_MB_NOVSI
                if (bDone && pVar->pVSI != NULL)
                    _T5MB_ExchangeVSI (pVar);
#endif /*T5DEF_MB_NOVSI*/
                break;
            case T5MBVAR_OUTSTRING :
                if (pVar->pReq->wStatus == T5MB_READY
                    || pVar->pReq->wStatus == T5MB_ERROR)
                {
                    _T5MB_ExchangeOutString (pVar);
                }
                break;
            case T5MBVAR_COMMAND :
            case T5MBVAR_SHOT :
            case T5MBVAR_RSTCNT :
                if (pVar->pReq->wStatus == T5MB_READY
                    || pVar->pReq->wStatus == T5MB_ERROR)
                    _T5MB_ExchangeOutput (pVar);
                break;
            default : break;
            }
        }
    }
    _T5MB_ResetServerchanges (pMB, pDB);
}

void T5MB_ExchangeServerOnly (T5_PTR pMB, T5PTR_DB pDB)
{
    T5PTR_MBHEAD pHeader;
    T5PTR_MBVAR pVar;
    T5_WORD i;
    T5_BOOL bDone;

    if (pMB == NULL)
        return;
    pHeader = (T5PTR_MBHEAD)pMB;
    pVar = pHeader->pVar;
    if (pVar == NULL)
        return;
    for (i=0; i<pHeader->wNbVar; i++, pVar++)
    {
        bDone = FALSE;
        if (pVar->pReq != NULL && pVar->pData != NULL
            && pVar->pReq->bServer)
        {
            switch (pVar->wOpe)
            {
            case T5MBVAR_STATUS :
            case T5MBVAR_STATACK :
            case T5MBVAR_TRIAL :
            case T5MBVAR_NBOK :
            case T5MBVAR_NBFAIL :
            case T5MBVAR_GOING :
                _T5MB_ExchangeInput (pVar);
                break;
            case T5MBVAR_INPUT :
                if (pVar->pReq->wStatus == T5MB_READY)
                    bDone = _T5MB_ExchangeInput (pVar);
#ifndef T5DEF_MB_NOVSI
                if (bDone && pVar->pVSI != NULL)
                    _T5MB_ExchangeVSI (pVar);
#endif /*T5DEF_MB_NOVSI*/
                break;
            case T5MBVAR_OUTPUT :
                if (pVar->pReq->wStatus == T5MB_READY)
                {
                    if ((pVar->pDef->wNbWrd & 0xf) == 2)
                        _T5MB_ExchangeOutput32 (pVar);
                    else
                        _T5MB_ExchangeOutput (pVar);
                }
                break;
            case T5MBVAR_INSTRING :
                if (pVar->pReq->wStatus == T5MB_READY)
                    bDone = _T5MB_ExchangeInString (pVar);
#ifndef T5DEF_MB_NOVSI
                if (bDone && pVar->pVSI != NULL)
                    _T5MB_ExchangeVSI (pVar);
#endif /*T5DEF_MB_NOVSI*/
                break;
            case T5MBVAR_OUTSTRING :
                if (pVar->pReq->wStatus == T5MB_READY)
                {
                    _T5MB_ExchangeOutString (pVar);
                }
                break;
            case T5MBVAR_COMMAND :
            case T5MBVAR_SHOT :
            case T5MBVAR_RSTCNT :
                if (pVar->pReq->wStatus == T5MB_READY
                    || pVar->pReq->wStatus == T5MB_ERROR)
                    _T5MB_ExchangeOutput (pVar);
                break;
            default : break;
            }
        }
    }
    _T5MB_ResetServerchanges (pMB, pDB);
}

void T5MB_ExchangeClientOnly (T5_PTR pMB, T5PTR_DB pDB)
{
    T5PTR_MBHEAD pHeader;
    T5PTR_MBVAR pVar;
    T5_WORD i;
    T5_UNUSED(pDB);

    if (pMB == NULL)
        return;
    pHeader = (T5PTR_MBHEAD)pMB;
    pVar = pHeader->pVar;
    if (pVar == NULL)
        return;
    for (i=0; i<pHeader->wNbVar; i++, pVar++)
    {
        if (pVar->pReq != NULL && pVar->pData != NULL
            && !pVar->pReq->bServer)
        {
            switch (pVar->wOpe)
            {
            case T5MBVAR_SLVERROR :
            case T5MBVAR_SLVDTERROR :
            case T5MBVAR_SLVTMERROR :
            case T5MBVAR_SLVDTRESET :
            case T5MBVAR_SLVTMRESET :
            case T5MBVAR_SLVNBTRANS :
            case T5MBVAR_SLVNBFAIL :
            case T5MBVAR_SLVRESET :
                _T5MB_ExchangeSlave (pVar);
                break;
            case T5MBVAR_STATUS :
            case T5MBVAR_STATACK :
            case T5MBVAR_TRIAL :
            case T5MBVAR_NBOK :
            case T5MBVAR_NBFAIL :
            case T5MBVAR_GOING :
                _T5MB_ExchangeInput (pVar);
                break;
            case T5MBVAR_INPUT :
                if (pVar->pReq->wStatus == T5MB_READY)
                    _T5MB_ExchangeInput (pVar);
#ifndef T5DEF_MB_NOVSI
                if (pVar->pVSI != NULL)
                    _T5MB_ExchangeVSI (pVar);
#endif /*T5DEF_MB_NOVSI*/
                break;
            case T5MBVAR_OUTPUT :
                if (pVar->pReq->wStatus == T5MB_READY)
                {
                    if ((pVar->pDef->wNbWrd & 0xf) == 2)
                        _T5MB_ExchangeOutput32 (pVar);
                    else
                        _T5MB_ExchangeOutput (pVar);
                }
                break;

            case T5MBVAR_INSTRING :
                if (pVar->pReq->wStatus == T5MB_READY)
                    _T5MB_ExchangeInString (pVar);
#ifndef T5DEF_MB_NOVSI
                if (pVar->pVSI != NULL)
                    _T5MB_ExchangeVSI (pVar);
#endif /*T5DEF_MB_NOVSI*/
                break;
            case T5MBVAR_OUTSTRING :
                if (pVar->pReq->wStatus == T5MB_READY)
                {
                    _T5MB_ExchangeOutString (pVar);
                }
                break;

            case T5MBVAR_COMMAND :
            case T5MBVAR_SHOT :
            case T5MBVAR_RSTCNT :
                if (pVar->pReq->wStatus == T5MB_READY
                    || pVar->pReq->wStatus == T5MB_ERROR)
                    _T5MB_ExchangeOutput (pVar);
                break;
            default : break;
            }
        }
    }
}

/*****************************************************************************
_T5MB_ExchangeInput
Exchange input or status MODBUS variable with VM database
Parameters:
    pVar (IN) pointer to MODBUS variable descriptor
*****************************************************************************/

static T5_BOOL _T5MB_ExchangeInput (T5PTR_MBVAR pVar)
{
    T5_WORD wData;
    T5_DWORD dwData;
    T5_PTBYTE pData, pdw;

#ifndef T5DEF_MB_NOLOCK
    if ((*(pVar->pLock) & pVar->bLockMask) != 0)
        return FALSE;
#endif /*T5DEF_MB_NOLOCK*/

    if (pVar->pReq->pDef->wFunc == T5MBF_RAPSS)
    {
        if (pVar->wOffset > 0)
            _T5MB_ExchangeRAPSS (pVar);
        return FALSE;
    }

#ifdef T5DEF_MBSREADQ
    if (T5MB_ISREQSH(pVar->pReq)) /* check update flag for server Write */
    {
        pData = pVar->pReq->pData + pVar->wOffset;
        pData += pVar->pReq->pDef->wDataSize;
        if (pVar->bBit)
        {
            if (((T5_WORD)(*pData) & pVar->wMask) == 0)
            {
                _T5MB_ExchangeOutput (pVar);
                return FALSE;
            }
        }
        else
        {
            if (*pData == 0 && ((pVar->pDef->wNbWrd & 0xf) != 2 || pData[2] == 0))
            {
                if ((pVar->pDef->wNbWrd & 0xf) == 2)
                    _T5MB_ExchangeOutput32 (pVar);
                else
                    _T5MB_ExchangeOutput (pVar);
                return FALSE;
            }
        }
    }
#endif /*T5DEF_MBSREADQ*/

    if (pVar->pReq->bChange)
        return FALSE;

    pData = pVar->pReq->pData + pVar->wOffset;

    if (pVar->wOpe == T5MBVAR_STATUS)
    {
        wData = pVar->pReq->wError;
    }
    else if (pVar->wOpe == T5MBVAR_STATACK)
    {
        if (pVar->pReq->wError != 0)
            wData = pVar->pReq->wError;
        else
            return TRUE;
    }
    else if (pVar->wOpe == T5MBVAR_TRIAL)
    {
        wData = pVar->pReq->wTry;
    }
    else if (pVar->wOpe == T5MBVAR_NBOK)
    {
        wData = pVar->pReq->wNbOK;
    }
    else if (pVar->wOpe == T5MBVAR_NBFAIL)
    {
        wData = pVar->pReq->wNbFail;
    }
    else if (pVar->wOpe == T5MBVAR_GOING)
    {
        wData = (pVar->pReq->wStatus == T5MB_SENDING
            || pVar->pReq->wStatus == T5MB_RECEIVING) ? 1 : 0;
    }
    else
    {
        if (pVar->bBit)
            wData = (T5_WORD)(*pData);
        else
            wData = *(T5_PTWORD)pData;
        wData &= pVar->wMask;
    }

    if (pVar->pDef->wNbWrd == 2)
    {
        T5_COPYWORDSTOLONG(&dwData, pData);
    }
    else if (pVar->pDef->wNbWrd == 0x8002)
    {
        pdw = (T5_PTBYTE)(&dwData);
#ifdef T5DEF_BIGENDIAN
        pdw[0] = pData[2];
        pdw[1] = pData[3];
        pdw[2] = pData[0];
        pdw[3] = pData[1];
#else /*T5DEF_BIGENDIAN*/
        pdw[0] = pData[0];
        pdw[1] = pData[1];
        pdw[2] = pData[2];
        pdw[3] = pData[3];
#endif /*T5DEF_BIGENDIAN*/
    }
    else
        dwData = 0L;

#ifdef T5DEF_REALSUPPORTED
    if (pVar->pDef->wDefID >= T5MBDEF_SVAR)
    {
        T5_REAL rMul, rAdd, rData;

        T5_MEMCPY (&rMul, &(pVar->pDef->wScaleMul1), sizeof (T5_REAL));
        T5_MEMCPY (&rAdd, &(pVar->pDef->wScaleAdd1), sizeof (T5_REAL));

        if ((pVar->pDef->wNbWrd & 0xf) != 2)
            rData = (T5_REAL)wData;
        else if (pVar->pDef->wT5Type != T5C_REAL && pVar->pDef->wT5Type != T5C_LREAL)
            rData = (T5_REAL)dwData;
        else
        {
#ifdef T5DEF_STRICTALIASING
            T5_MEMCPY (&rData, &dwData, sizeof (rData));
#else /*T5DEF_STRICTALIASING*/
            rData = *(T5_PTREAL)(&dwData);
#endif /*T5DEF_STRICTALIASING*/
        }

        if (rMul != 0.0)
            rData = (rData - rAdd) / rMul;
        
        T5VMCnv_ConvBin (pVar->pData, &rData, (T5_BYTE)(pVar->pDef->wT5Type), T5C_REAL);
        return TRUE;
    }
#endif /*T5DEF_REALSUPPORTED*/

    switch (pVar->pDef->wT5Type)
    {
    case T5C_BOOL :
        *(T5_PTCHAR)(pVar->pData) = (T5_CHAR)(wData != 0);
        break;
    case T5C_SINT :
    case T5C_USINT :
        *(T5_PTCHAR)(pVar->pData) = (T5_CHAR)wData;
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
    case T5C_UINT :
        *(T5_PTSHORT)(pVar->pData) = (T5_SHORT)wData;
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_TIME :
    case T5C_UDINT :
        if ((pVar->pDef->wNbWrd & 0xf) == 2)
            *(T5_PTDWORD)(pVar->pData) = dwData;
        else
            *(T5_PTDWORD)(pVar->pData) = (T5_DWORD)wData;
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        if ((pVar->pDef->wNbWrd & 0xf) == 2)
            T5_MEMCPY (pVar->pData, &dwData, sizeof (T5_REAL));
        else
            *(T5_PTREAL)(pVar->pData) = (T5_REAL)wData;
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        if ((pVar->pDef->wNbWrd & 0xf) == 2)
            *(T5_PTLINT)(pVar->pData) = (T5_LINT)dwData;
        else
            *(T5_PTLINT)(pVar->pData) = (T5_LINT)wData;
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        if ((pVar->pDef->wNbWrd & 0xf) == 2)
            *(T5_PTLWORD)(pVar->pData) = (T5_LWORD)dwData;
        else
            *(T5_PTLWORD)(pVar->pData) = (T5_LWORD)wData;
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        if ((pVar->pDef->wNbWrd & 0xf) == 2)
        {
#ifdef T5DEF_STRICTALIASING
            T5_REAL rm;
            T5_LREAL lrm;
            T5_MEMCPY (&rm, &dwData, sizeof (rm));
            lrm = (T5_LREAL)rm;
            T5_MEMCPY (pVar->pData, &lrm, sizeof (T5_LREAL));
#else /*T5DEF_STRICTALIASING*/
            *(T5_PTLREAL)(pVar->pData) = (T5_LREAL)(*(T5_PTREAL)(&dwData));
#endif /*T5DEF_STRICTALIASING*/
        }
        else
            *(T5_PTLREAL)(pVar->pData) = (T5_LREAL)wData;
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default : break;
    }
    return TRUE;
}

/*****************************************************************************
_T5MB_ExchangeVSI
Update VSI for a received variable
Parameters:
    pVar (IN) pointer to MODBUS variable descriptor
*****************************************************************************/

static void _T5MB_ExchangeVSI (T5PTR_MBVAR pVar)
{
#ifdef T5DEF_VARMAP
#ifndef T5DEF_MB_NOVSI
    if (pVar->pReq->pDef->wFunc == T5MBF_RAPSS)
        return;
    if (pVar->pVSI != NULL)
    {
        if (pVar->pReq->wError == 0)
            pVar->pVSI->dwStatLo &= ~T5VSM_L_I_BIT;
        else
            pVar->pVSI->dwStatLo |= T5VSM_L_I_BIT;
        pVar->pVSI->dwDate = pVar->pReq->dwDt;
        pVar->pVSI->dwTime = pVar->pReq->dwTm;
    }
#endif /*T5DEF_MB_NOVSI*/
#endif /*T5DEF_VARMAP*/
}

/*****************************************************************************
_T5MB_ExchangeOutput
Exchange output MODBUS variable with VM database
Process activation of OnChange requests
Parameters:
    pVar (IN) pointer to MODBUS variable descriptor
*****************************************************************************/

static void _T5MB_ExchangeOutput (T5PTR_MBVAR pVar)
{
    T5_WORD wData, wOut;
    T5_BYTE bOut;
    T5_PTBYTE pByteData;
    T5_PTWORD pReqData;
    T5_BOOL bChange;

    switch (pVar->pDef->wT5Type)
    {
    case T5C_BOOL :
        wData = (*(T5_PTCHAR)(pVar->pData) != 0) ? 1 : 0;
        break;
    case T5C_SINT :
#ifdef T5DEF_MB_SINTTOINT
        wData = (T5_SHORT)(*(T5_PTCHAR)(pVar->pData));
        break;
#endif /*T5DEF_MB_SINTTOINT*/
    case T5C_USINT :
        wData = (T5_WORD)(*(T5_PTCHAR)(pVar->pData)) & 0x00ff;
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
    case T5C_UINT :
        wData = *(T5_PTWORD)(pVar->pData);
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_TIME :
    case T5C_UDINT :
        wData = (T5_WORD)(*(T5_PTDWORD)(pVar->pData));
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        wData = (T5_WORD)(*(T5_PTREAL)(pVar->pData));
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        wData = (T5_WORD)(*(T5_PTLINT)(pVar->pData));
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        wData = (T5_WORD)(*(T5_PTLWORD)(pVar->pData));
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        wData = (T5_WORD)(*(T5_PTLREAL)(pVar->pData));
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default :
        wData = 0;
        break;
    }
    if (pVar->wOpe == T5MBVAR_COMMAND)
    {
        if (wData)
            pVar->pReq->bActive = TRUE;
        return;
    }
    else if (pVar->wOpe == T5MBVAR_RSTCNT)
    {
        if (wData)
        {
            pVar->pReq->wNbFail = 0;
            pVar->pReq->wNbOK = 0;
            _T5MB_ResetCmdVar (pVar);
        }
        return;
    }
    else if (pVar->wOpe == T5MBVAR_SHOT)
    {
        if (wData  && !(pVar->pReq->bShot))
        {
            pVar->pReq->bActive = TRUE;
            pVar->pReq->bShot = TRUE;
        }
        else if (pVar->pReq->bShot == 0xff)
        {
            pVar->pReq->bShot = FALSE;
            _T5MB_ResetCmdVar (pVar);
        }
        return;
    }

    pByteData = pVar->pReq->pData + pVar->wOffset;
    pReqData = (T5_PTWORD)pByteData;
    if (pVar->bBit)
    {
        bOut = *pByteData;
        if (wData)
            bOut |= (T5_BYTE)(pVar->wMask);
        else
            bOut &= ~(T5_BYTE)(pVar->wMask);
        bChange = (*pByteData != bOut);
        *pByteData = bOut;
    }
    else
    {

#ifdef T5DEF_REALSUPPORTED
        if (pVar->pDef->wDefID >= T5MBDEF_SVAR)
        {
            T5_REAL rMul, rAdd, rData;

            T5VMCnv_ConvBin (&rData, pVar->pData, T5C_REAL, (T5_BYTE)(pVar->pDef->wT5Type));
            T5_MEMCPY (&rMul, &(pVar->pDef->wScaleMul1), sizeof (T5_REAL));
            T5_MEMCPY (&rAdd, &(pVar->pDef->wScaleAdd1), sizeof (T5_REAL));
            wData = (T5_WORD)((rData * rMul) + rAdd);
        }
#endif /*T5DEF_REALSUPPORTED*/

        wOut = *pReqData;
        wOut &= ~pVar->wMask;

        /* for sending bits on words using masks */
        if (wData != 0 && pVar->wMask != 0xffff
            && pVar->pDef->wT5Type == T5C_BOOL)
        {
            wData = 0xffff;
        }

        wOut |= (wData & pVar->wMask);
        bChange = (*pReqData != wOut);
        *pReqData = wOut;
    }
    if (bChange && pVar->pReq->pDef->wMode == T5MBMODE_ONCHANGE)
        pVar->pReq->bActive = TRUE;
}

/* specific *****************************************************************/

static void _T5MB_ExchangeRAPSS (T5PTR_MBVAR pVar)
{
#ifdef T5DEF_LINTSUPPORTED
    T5_LINT li;

    li = 0;
    T5_MEMCPY (&li, pVar->pReq->pData + 2 + (pVar->wOffset - 2) * 3, 6); /* intel only */
    T5VMCnv_ConvBin (pVar->pData, &li, (T5_BYTE)(pVar->pDef->wT5Type), T5C_LINT);
#endif /*T5DEF_LINTSUPPORTED*/
}

/*****************************************************************************
_T5MB_ExchangeOutput32
Exchange output MODBUS variable with VM database - work on 2 consecutive bytes
Process activation of OnChange requests
Parameters:
    pVar (IN) pointer to MODBUS variable descriptor
*****************************************************************************/

static void _T5MB_ExchangeOutput32 (T5PTR_MBVAR pVar)
{
    T5_DWORD dwData, dwOut;
    T5_BYTE bOut;
    T5_PTBYTE pByteData, pdw;
    T5_BOOL bChange;

    switch (pVar->pDef->wT5Type)
    {
    case T5C_BOOL :
        dwData = (*(T5_PTCHAR)(pVar->pData) != 0) ? 1L : 0L;
        break;
    case T5C_SINT :
    case T5C_USINT :
        dwData = (T5_DWORD)(*(T5_PTCHAR)(pVar->pData)) & 0x00ff;
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
    case T5C_UINT :
        dwData = (T5_DWORD)(*(T5_PTWORD)(pVar->pData));
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_TIME :
    case T5C_UDINT :
        dwData = *(T5_PTDWORD)(pVar->pData);
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        T5_MEMCPY (&dwData, pVar->pData, sizeof (T5_REAL));
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        dwData = (T5_DWORD)(*(T5_PTLINT)(pVar->pData));
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        dwData = (T5_DWORD)(*(T5_PTLWORD)(pVar->pData));
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        {
#ifdef T5DEF_STRICTALIASING
            T5_REAL rm = (T5_REAL)(*(T5_PTLREAL)(pVar->pData));
            T5_MEMCPY (&dwData, &rm, sizeof (dwData));
#else /*T5DEF_STRICTALIASING*/
            *(T5_PTREAL)(&dwData) = (T5_REAL)(*(T5_PTLREAL)(pVar->pData));
#endif /*T5DEF_STRICTALIASING*/
        }
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default :
        dwData = 0L;
        break;
    }

    pByteData = pVar->pReq->pData + pVar->wOffset;
    if (pVar->bBit)
    {
        bOut = *pByteData;
        if (dwData)
            bOut |= (T5_BYTE)(pVar->wMask);
        else
            bOut &= ~(T5_BYTE)(pVar->wMask);
        bChange = (*pByteData != bOut);
        *pByteData = bOut;
    }
    else
    {
#ifdef T5DEF_REALSUPPORTED
        if (pVar->pDef->wDefID >= T5MBDEF_SVAR)
        {
            T5_REAL rMul, rAdd, rData;

            T5_MEMCPY (&rMul, &(pVar->pDef->wScaleMul1), sizeof (T5_REAL));
            T5_MEMCPY (&rAdd, &(pVar->pDef->wScaleAdd1), sizeof (T5_REAL));
            if (pVar->pDef->wT5Type == T5C_REAL || pVar->pDef->wT5Type == T5C_LREAL)
            {
#ifdef T5DEF_STRICTALIASING
                T5_MEMCPY (&rData, &dwData, sizeof (T5_REAL));
                rData = (rData * rMul) + rAdd;
                T5_MEMCPY (&dwData, &rData, sizeof (dwData));
#else /*T5DEF_STRICTALIASING*/
                T5_MEMCPY (&rData, &dwData, sizeof (T5_REAL));
                *(T5_PTREAL)(&dwData) = (rData * rMul) + rAdd;
#endif /*T5DEF_STRICTALIASING*/
            }
            else
            {
                T5VMCnv_ConvBin (&rData, pVar->pData, T5C_REAL, (T5_BYTE)(pVar->pDef->wT5Type));
                dwData = (T5_DWORD)((rData * rMul) + rAdd);
            }
        }
#endif /*T5DEF_REALSUPPORTED*/

        T5_COPYWORDSTOLONG(&dwOut, pByteData);
        bChange = (dwOut != dwData);

        if (pVar->pDef->wNbWrd == 0x8002)
        {
            pdw = (T5_PTBYTE)(&dwData);
#ifdef T5DEF_BIGENDIAN
            pByteData[0] = pdw[2];
            pByteData[1] = pdw[3];
            pByteData[2] = pdw[0];
            pByteData[3] = pdw[1];
#else /*T5DEF_BIGENDIAN*/
            pByteData[0] = pdw[0];
            pByteData[1] = pdw[1];
            pByteData[2] = pdw[2];
            pByteData[3] = pdw[3];
#endif /*T5DEF_BIGENDIAN*/
        }
        else
        {
            T5_COPYWORDSTOLONG(pByteData, &dwData);
        }
    }
    if (bChange && pVar->pReq->pDef->wMode == T5MBMODE_ONCHANGE)
        pVar->pReq->bActive = TRUE;
}

/*****************************************************************************
_T5MB_ResetCmdVar
Reset a "command" variable
Parameters:
    pVar (IN) pointer to MODBUS variable descriptor
*****************************************************************************/

static void _T5MB_ResetCmdVar (T5PTR_MBVAR pVar)
{
    switch (pVar->pDef->wT5Type)
    {
    case T5C_BOOL :
    case T5C_SINT :
    case T5C_USINT :
        *(T5_PTCHAR)(pVar->pData) = 0;
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
    case T5C_UINT :
        *(T5_PTWORD)(pVar->pData) = 0;
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_TIME :
    case T5C_UDINT :
    case T5C_REAL :
        *(T5_PTDWORD)(pVar->pData) = 0L;
        break;
#ifdef T5DEF_DATA64SUPPORTED
    case T5C_LINT :
    case T5C_ULINT :
        *(T5_PTDATA64)(pVar->pData) = 0;
        break;
#endif /*T5DEF_DATA64SUPPORTED*/
    default : break;
    }
}

/*****************************************************************************
_T5MB_OnSlaveExchange
Manage slave status
Parameters:
    pClient (IN) channel object
    pReq (IN) request object
*****************************************************************************/

static void _T5MB_OnSlaveExchange (T5PTR_MBCLIENT pClient, T5PTR_MBREQ pReq)
{
    T5PTR_MBSLAVE pSlave;
    T5_UNUSED(pClient);

    pSlave = pReq->pSlave;
    if (pSlave == NULL)
        return;

    pSlave->dwNbTrans += 1L;

    if (pReq->wError)
    {
        pSlave->dwNbFail += 1L;
        pSlave->wLastError = pReq->wError;
#ifdef T5DEF_RTCLOCKSTAMP
        pSlave->dwDateLastError = T5RtClk_GetCurDateStamp ();
        pSlave->dwTimeLastError = T5RtClk_GetCurTimeStamp ();
#endif /*T5DEF_RTCLOCKSTAMP*/
    }
}

/*****************************************************************************
_T5MB_ResetSlave
Reset slave status
Parameters:
    pSlave (IN) slave object
*****************************************************************************/

static void _T5MB_ResetSlave (T5PTR_MBSLAVE pSlave)
{
    if (pSlave == NULL)
        return;

    pSlave->dwDateLastError = 0;
    pSlave->dwTimeLastError = 0;
    pSlave->dwNbTrans = 0;
    pSlave->dwNbFail = 0;
    pSlave->wLastError = 0;

#ifdef T5DEF_RTCLOCKSTAMP
    pSlave->dwDateLastReset = T5RtClk_GetCurDateStamp ();
    pSlave->dwTimeLastReset = T5RtClk_GetCurTimeStamp ();
#endif /*T5DEF_RTCLOCKSTAMP*/
}

/*****************************************************************************
_T5MB_ExchangeSlave
Exchange slave status variable
Parameters:
    pVar (IN) status variable
*****************************************************************************/

static void _T5MB_ExchangeSlave (T5PTR_MBVAR pVar)
{
    T5PTR_MBSLAVE pSlave;
    T5_BYTE bType, bZero;

    pSlave = pVar->pReq->pSlave;
    if (pSlave == NULL || pVar->pData == NULL)
        return;

#ifndef T5DEF_MB_NOLOCK
    if (pVar->wOpe != T5MBVAR_SLVRESET)
    {
        if ((*(pVar->pLock) & pVar->bLockMask) != 0)
        return;
    }
#endif /*T5DEF_MB_NOLOCK*/

    bType = (T5_BYTE)(pVar->pDef->wT5Type);
    switch (pVar->wOpe)
    {
    case T5MBVAR_SLVERROR :
        T5VMCnv_ConvBin (pVar->pData, &(pSlave->wLastError), bType, T5C_UINT);
        break;
    case T5MBVAR_SLVDTERROR :
        T5VMCnv_ConvBin (pVar->pData, &(pSlave->dwDateLastError), bType, T5C_UDINT);
        break;
    case T5MBVAR_SLVTMERROR :
        T5VMCnv_ConvBin (pVar->pData, &(pSlave->dwTimeLastError), bType, T5C_UDINT);
        break;
    case T5MBVAR_SLVDTRESET :
        T5VMCnv_ConvBin (pVar->pData, &(pSlave->dwDateLastReset), bType, T5C_UDINT);
        break;
    case T5MBVAR_SLVTMRESET :
        T5VMCnv_ConvBin (pVar->pData, &(pSlave->dwTimeLastReset), bType, T5C_UDINT);
        break;
    case T5MBVAR_SLVNBTRANS :
        T5VMCnv_ConvBin (pVar->pData, &(pSlave->dwNbTrans), bType, T5C_UDINT);
        break;
    case T5MBVAR_SLVNBFAIL :
        T5VMCnv_ConvBin (pVar->pData, &(pSlave->dwNbFail), bType, T5C_UDINT);
        break;
    case T5MBVAR_SLVRESET :
        T5VMCnv_ConvBin (&bZero, pVar->pData, T5C_USINT, bType);
        if (bZero)
        {
            _T5MB_ResetSlave (pSlave);
            bZero = 0;
            T5VMCnv_ConvBin (pVar->pData, &bZero, bType, T5C_USINT);
        }
        break;
    default : break;
    }
}

/*****************************************************************************
_T5MB_ExchangeInString
Exchange string input variable
Parameters:
    pVar (IN) status variable
*****************************************************************************/

static T5_BOOL _T5MB_ExchangeInString (T5PTR_MBVAR pVar)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_WORD i, wValue[128];
    T5_PTWORD pFrame;
    T5_BYTE bLen;

#ifndef T5DEF_MB_NOLOCK
    if ((*(pVar->pLock) & pVar->bLockMask) != 0)
        return FALSE;
#endif /*T5DEF_MB_NOLOCK*/

#ifdef T5DEF_MBSREADQ
    if (T5MB_ISREQSH(pVar->pReq)) /* check update flag for server Write */
    {
        T5_PTBYTE pData = pVar->pReq->pData + pVar->wOffset;
        pData += pVar->pReq->pDef->wDataSize;
        if (*pData == 0)
        {
            _T5MB_ExchangeOutString (pVar);
            return FALSE;
        }
    }
#endif /*T5DEF_MBSREADQ*/

    if (pVar->pReq->bChange)
        return FALSE;
    pFrame = (T5_PTWORD)(pVar->pReq->pData + pVar->wOffset);

    T5_MEMSET (wValue, 0, sizeof (wValue));
    for (i=0; i<pVar->pDef->wNbWrd && i<127; i++)
    {
        T5_COPYFRAMEWORD (&(wValue[i]), pFrame+i);
    }
    bLen = (T5_BYTE)T5_STRLEN((T5_PTCHAR)wValue);
    if (bLen > pVar->pData[0])
        bLen = pVar->pData[0];
    T5_MEMCPY (pVar->pData + 2, wValue, bLen);
    pVar->pData[2+bLen] = '\0';
    pVar->pData[1] = bLen;
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

/*****************************************************************************
_T5MB_ExchangeOutString
Exchange string output variable
Parameters:
    pVar (IN) status variable
*****************************************************************************/

static void _T5MB_ExchangeOutString (T5PTR_MBVAR pVar)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_BOOL bChange;
    T5_WORD i, w, wValue[128];
    T5_PTWORD pFrame;

    pFrame = (T5_PTWORD)(pVar->pReq->pData + pVar->wOffset);
    bChange = FALSE;

    T5_MEMSET (wValue, 0, sizeof (wValue));
    T5_MEMCPY (wValue, pVar->pData + 2, pVar->pData[1]);

    for (i=0; i<pVar->pDef->wNbWrd && i<127; i++)
    {
        T5_COPYFRAMEWORD (&w, &(wValue[i]));
        if (w != pFrame[i])
        {
            pFrame[i] = w;
            bChange = TRUE;
        }
    }

    if (bChange && pVar->pReq->pDef->wMode == T5MBMODE_ONCHANGE)
        pVar->pReq->bActive = TRUE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

/****************************************************************************/

#ifndef T5DEF_MBSLAVENO
#define T5DEF_MBSLAVENO     ((T5_BYTE)(pServer->pDef->wPort))
#endif /*T5DEF_MBSLAVENO*/

/*****************************************************************************
T5MB_FindServer
Search for a server
Parameters:
    pMB (IN) MODBUS data
    szID (IN) Server identifier
Return: pointer to the server object or NULL if not found
*****************************************************************************/

T5_PTR T5MB_FindServer (T5_PTR pMB, T5_PTCHAR szID)
{
    T5PTR_MBHEAD pHeader;
    T5_WORD i;

    if (pMB == NULL)
        return NULL;
    pHeader = (T5PTR_MBHEAD)pMB;

    if (szID == NULL || *szID == '\0')
        return (T5_PTR)(pHeader->pServer);

    for (i=0; i<pHeader->wNbClient; i++)
    {
        if ((pHeader->pClient[i].wFlags & T5MBDEF_SERVER) != 0
            && T5_STRCMP (pHeader->pClient[i].pDef->szAddr, szID) == 0)
        {
            return (T5_PTR)(&(pHeader->pClient[i]));
        }
    }

    return NULL;
}

/*****************************************************************************
T5MB_Serve
Serve a MODBUS slave request
Parameters:
    pMB (IN) MODBUS data
    pIn (IN) received frame
    pOut (OUT) answer buffer
    bTCP (IN) false = RTU / true = TCP
Return: answer length
*****************************************************************************/

T5_WORD T5MB_Serve (T5_PTR pMB, T5_PTBYTE pIn, T5_PTBYTE pOut, T5_BOOL bTCP)
{
    return T5MB_ServeEx (pMB, pIn, pOut, bTCP, 0);
}

T5_WORD T5MB_ServeEx (T5_PTR pMB, T5_PTBYTE pIn, T5_PTBYTE pOut,
                      T5_BOOL bTCP, T5_WORD wSlave)
{
    return T5MB_ServeExID (pMB, pIn, pOut, bTCP, wSlave, NULL);
}

T5_WORD T5MB_ServeExID (T5_PTR pMB, T5_PTBYTE pIn, T5_PTBYTE pOut,
                        T5_BOOL bTCP, T5_WORD wSlave, T5_PTCHAR szID)
{
    T5PTR_MBCLIENT pServer;
    T5_WORD wLen;
    T5_PTBYTE pFrame, pAnswer;

    if (pMB == NULL)
        return 0;

    pServer = (T5PTR_MBCLIENT)T5MB_FindServer (pMB, szID);
    if (pServer == NULL)
        return 0;

    /* get MODBUS frame including slave number */
    if (bTCP)
    {
        pFrame = pIn + 6;
        pAnswer = pOut + 6;
    }
    else
    {
        pFrame = pIn;
        pAnswer = pOut;
    }
    /* check number */
    if (wSlave == 0)
        wSlave = T5DEF_MBSLAVENO;
    if (*pFrame != 0 && *pFrame != wSlave)
        return 0;
    /* process request and get answer */
    wLen = T5MBSrv_Serve (pServer, pFrame, pAnswer);
    /* broadcast: don't answer */
    if (!bTCP && *pFrame == 0)
        return 0;
    /* manage MODBUS-TCP MBAP */
    if (bTCP)
    {
        pOut[0] = pIn[0];
        pOut[1] = pIn[1];
        pOut[2] = pOut[3] = 0;
        T5_COPYFRAMEWORD (pOut+4, &wLen);
        wLen += 6;
    }
    return wLen;
}

/* Server on UDP ************************************************************/

#ifdef T5DEF_UDP

/*****************************************************************************
T5MBSrv_UdpOpen
Open a UDP server
Parameters:
    wPort (IN) port number
    pSocket (OUT) UDP socket
Return: OK or error
*****************************************************************************/

T5_RET T5MBSrv_UdpOpen (T5_WORD wPort, T5_PTSOCKET pSocket)
{
    return T5Socket_UdpCreate (wPort, pSocket);
}

/*****************************************************************************
T5MBSrv_UdpClose
Close a UDP server
Parameters:
    socket (IN) UDP socket
*****************************************************************************/

void T5MBSrv_UdpClose (T5_SOCKET sock)
{
    if (sock != T5_INVSOCKET)
        T5Socket_CloseSocket (sock);
}

/*****************************************************************************
T5MBSrv_UdpActivate
UDP state machine
Parameters:
    pDB (IN) pointer to the VMDB
    sock (IN) UDP socket
    bRTU (IN) true = RTU / false = Open MODBUS
    wSlave (IN) slave number
    szID (IN) server identifier
*****************************************************************************/

void T5MBSrv_UdpActivate (T5PTR_DB pDB, T5_SOCKET sock, T5_BOOL bRTU,
                          T5_WORD wSlave, T5_PTCHAR szID)
{
    T5_BYTE bIn[300], bOut[300];
    T5_WORD wNbRec, wNbSnd;
    T5_UDPADDR sAdd;
    T5_WORD crc;

    if (sock == T5_INVSOCKET)
        return;

    wNbRec = T5Socket_UdpRecvFrom (sock, bIn, sizeof (bIn), &sAdd);
    if (wNbRec == 0)
        return;

    wNbSnd = T5MB_ServeExID (T5GET_MBC(pDB), bIn, bOut, (T5_BOOL)(!bRTU), wSlave, szID);
    if (wNbSnd == 0)
        return;

    if (bRTU)
    {
        crc = T5Tools_Crc16 (bOut, wNbSnd);
        bOut[wNbSnd++] = (T5_BYTE)(crc & 0xff);
        bOut[wNbSnd++] = (T5_BYTE)((crc>>8) & 0xff);
    }
    T5Socket_UdpSendTo (sock, &sAdd, bOut, wNbSnd);
}

#endif /*T5DEF_UDP*/

/****************************************************************************/

#endif /*T5DEF_MODBUSIOS*/

/* eof **********************************************************************/
