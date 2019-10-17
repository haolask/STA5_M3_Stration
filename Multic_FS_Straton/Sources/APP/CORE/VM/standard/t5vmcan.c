/*****************************************************************************
T5VMcan.c :  CAN-bus driver main core

DO NOT ALTER THIS !

(c) COPA-DATA France 2006
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_CANBUS

/****************************************************************************/
/* static services */

static void _T5VMCan_BuildPorts (T5PTR_DB pDB, T5_PTR pCode, T5PTR_CANDEF pDef);
static void _T5VMCan_BuildMsgs (T5PTR_DB pDB, T5_PTR pCode, T5PTR_CANDEF pDef);
static void _T5VMCan_BuildVars (T5PTR_DB pDB, T5_PTR pCode, T5PTR_CANDEF pDef);
static T5_RET _T5VMCan_Open (T5PTR_DB pDB);
static void _T5VMCan_ExchangeInput (T5PTR_DBCANVAR pVar);
static void _T5VMCan_ExchangeOutput (T5PTR_DBCANVAR pVar);
static void _T5VMCan_ExchangeInputBS (T5PTR_DBCANVAR pVar);
static void _T5VMCan_ExchangeOutputBS (T5PTR_DBCANVAR pVar);
static void _T5VMCan_ExchangeInputSPN (T5PTR_DBCANVAR pVar);
static void _T5VMCan_ExchangeOutputSPN (T5PTR_DBCANVAR pVar);
static void _T5VMCan_SwapBytes (T5_BYTE bBigEndian, T5_PTBYTE pBuffer, T5_BYTE bSize);
static T5_BOOL _T5VMCan_ShouldSend (T5_BOOL bScan0, T5PTR_DBCANMSG pMsg,
                                    T5_DWORD dwTimeStamp);
static void _T5VMCan_Send (T5PTR_DBCANPORT pPort, T5PTR_DBCANMSG pMsg);
static void _T5VMCan_ExchangeSpecial (T5PTR_DBCANPORT pPortBase, T5PTR_DBCANVAR pVar);
static T5PTR_DBCANPORT _T5VMCan_FindPort (T5PTR_DB pDB, T5_PTCHAR szPort);
static void _T5VMCan_JForward (T5PTR_DB pDB);

/****************************************************************************/
/* callback for drivers */

T5_PTR T5VMCan_CB (T5_DWORD dwCommand, T5_PTR pPort, T5_PTR pMsg, T5_DWORD dwID)
{
    switch (dwCommand)
    {
    case T5CANHC_PEEK :
        return (T5_PTR)T5VMCan_PeekNextSndMsg ((T5PTR_DBCANPORT)pPort);
    case T5CANHC_POP :
        return (T5_PTR)T5VMCan_PopNextSndMsg ((T5PTR_DBCANPORT)pPort);
    case T5CANHC_FIND :
        return (T5_PTR)T5VMCan_FindRcvMsg ((T5PTR_DBCANPORT)pPort, dwID);
    case T5CANHC_MARK :
        T5VMCan_MarkRcvMsg ((T5PTR_DBCANPORT)pPort, (T5PTR_DBCANMSG)pMsg);
        return NULL;
    default :
        return NULL;
    }
}

/*****************************************************************************
T5VMCan_GetDBSize
Get sizing info for CAN driver memory
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
    pCode (IN) pointer to app code
*****************************************************************************/

void T5VMCan_GetDBSize (T5PTR_DB pDB, T5_PTR pCode)
{
    T5PTR_CANDEF pDef;

    pDef = T5VMCode_GetCanDef (pCode);

    pDB[T5TB_CANPORT].wSizeOf = sizeof (T5STR_DBCANPORT);
    pDB[T5TB_CANMSG].wSizeOf = sizeof (T5STR_DBCANMSG);
    pDB[T5TB_CANVAR].wSizeOf = sizeof (T5STR_DBCANVAR);

    if (pDef == NULL)
    {
        pDB[T5TB_CANPORT].wNbItemAlloc = 0;
        pDB[T5TB_CANMSG].wNbItemAlloc = 0;
        pDB[T5TB_CANVAR].wNbItemAlloc = 0;
    }
    else
    {
        pDB[T5TB_CANPORT].wNbItemAlloc = pDef->wNbPort;
        pDB[T5TB_CANMSG].wNbItemAlloc = pDef->wNbMsgIn + pDef->wNbMsgOut
                                      + 2 * pDef->wFifoSize * pDef->wNbPort;
        pDB[T5TB_CANVAR].wNbItemAlloc = pDef->wNbVar;
    }
}

/*****************************************************************************
T5VMCan_CanBuildHot
Check if hot start or on line change is possible
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
    pCode (IN) pointer to new app code
Return: OK if possible
*****************************************************************************/

T5_RET T5VMCan_CanBuildHot (T5PTR_DB pDB, T5_PTR pCode)
{
    T5PTR_CANDEF pDef;

    pDef = T5VMCode_GetCanDef (pCode);
    if (pDef == NULL)
        return T5RET_OK;

    if (pDef->wNbPort > pDB[T5TB_CANPORT].wNbItemAlloc)
        return T5RET_ERROR;
    if ((pDef->wNbMsgIn + pDef->wNbMsgOut) > pDB[T5TB_CANMSG].wNbItemAlloc)
        return T5RET_ERROR;
    if (pDef->wNbVar > pDB[T5TB_CANVAR].wNbItemAlloc)
        return T5RET_ERROR;

    return T5RET_OK;
}

/*****************************************************************************
T5VMCan_BuildHot
Performs a hot start or on line change
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
    pCode (IN) pointer to new app code
Return: OK or error
*****************************************************************************/

T5_RET T5VMCan_BuildHot (T5PTR_DB pDB, T5_PTR pCode)
{
    if (T5VMCan_CanBuildHot (pDB, pCode) != T5RET_OK)
        return T5RET_ERROR;

    if (T5VMCan_Build (pDB, pCode) != T5RET_OK)
        return T5RET_ERROR;

    return _T5VMCan_Open (pDB);
}

/*****************************************************************************
T5VMCan_BuildHot
Build CAN driver memory for a cold start and open all CAN ports
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
    pCode (IN) pointer to app code
Return: OK or error
*****************************************************************************/

T5_RET T5VMCan_Build (T5PTR_DB pDB, T5_PTR pCode)
{
    T5PTR_CANDEF pDef;

    pDef = T5VMCode_GetCanDef (pCode);

    if (pDef == NULL)
    {
        pDB[T5TB_CANPORT].wNbItemUsed = 0;
        pDB[T5TB_CANMSG].wNbItemUsed = 0;
        pDB[T5TB_CANVAR].wNbItemUsed = 0;
        return T5RET_OK;
    }

    _T5VMCan_BuildPorts (pDB, pCode, pDef);
    _T5VMCan_BuildMsgs (pDB, pCode, pDef);
    _T5VMCan_BuildVars (pDB, pCode, pDef);
    return _T5VMCan_Open (pDB);
}

/*****************************************************************************
_T5VMCan_BuildPorts
Build CAN driver memory for CAN ports
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
    pCode (IN) pointer to app code
    pDef (IN/OUT) pointer to CAN static definition
*****************************************************************************/

static void _T5VMCan_BuildPorts (T5PTR_DB pDB, T5_PTR pCode, T5PTR_CANDEF pDef)
{
    T5PTR_DBCANPORT pPort;
    T5PTR_DBCANMSG pMsgIn, pMsgOut, pFifo;
    T5PTR_CODEHEADER pHeader;
    T5PTR_CANPORTDEF pPortDef;
    T5_WORD i;

    pPort = T5GET_DBCANPORT(pDB);
    pDB[T5TB_CANPORT].wNbItemUsed = pDef->wNbPort;
    pHeader = (T5PTR_CODEHEADER)pCode;
    pPortDef = (T5PTR_CANPORTDEF)T5_OFF(pHeader, pDef->dwPortOffset);
    /* first all IN messages - then all OUT messages */
    pMsgIn = T5GET_DBCANMSG(pDB);
    pMsgOut = pMsgIn + pDef->wNbMsgIn;
    pFifo = pMsgOut + pDef->wNbMsgOut;
    for (i=0; i<pDef->wNbPort; i++, pPort++, pPortDef++)
    {
        T5_MEMSET (pPort, 0, sizeof (T5STR_DBCANPORT));
        pPort->pfCB = T5VMCan_CB;
        pPort->pDef = pPortDef;
        pPort->wFifoSize = pDef->wFifoSize;
        pPort->wFlags |= T5CAN_SCAN0;
        if (pPortDef->wType == T5CANPORT_2B)
            pPort->wFlags |= T5CAN_2B;
        pPort->wRate = pPortDef->wRate;
        pPort->szSettings = pPortDef->szSettings;
        pPort->pMsgIn = pMsgIn;
        pMsgIn += pPortDef->wNbMsgIn;
        pPort->pMsgOut = pMsgOut;
        pMsgOut += pPortDef->wNbMsgOut;
        pPort->pRcvFifo = pFifo;
        pFifo += pDef->wFifoSize;
        pPort->pSndFifo = pFifo;
        pFifo += pDef->wFifoSize;
    }
}

/*****************************************************************************
_T5VMCan_BuildMsgs
Build CAN driver memory for CAN messages
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
    pCode (IN) pointer to app code
    pDef (IN/OUT) pointer to CAN static definition
*****************************************************************************/

static void _T5VMCan_BuildMsgs (T5PTR_DB pDB, T5_PTR pCode, T5PTR_CANDEF pDef)
{
    T5PTR_DBCANMSG pMsg;
    T5PTR_CODEHEADER pHeader;
    T5PTR_CANMSGDEF pMsgDef;
    T5_PTBYTE pData;
    T5_WORD i;
    T5PTR_DBCANPORT pPort;

    pPort = T5GET_DBCANPORT(pDB);
    pDB[T5TB_CANMSG].wNbItemUsed = pDef->wNbMsgIn + pDef->wNbMsgOut;
    pHeader = (T5PTR_CODEHEADER)pCode;
    pMsg = T5GET_DBCANMSG(pDB);
    pMsgDef = (T5PTR_CANMSGDEF)T5_OFF(pHeader, pDef->dwInMsgOffset);
    /* first all IN messages */
    for (i=0; i<pDef->wNbMsgIn; i++, pMsg++)
    {
        T5_MEMSET (pMsg, 0, sizeof (T5STR_DBCANMSG));
#ifdef T5CAN_OEMALLOC
        pMsg->data = T5CAN_OemAlloc (pPort + pMsgDef->wPort, pMsgDef->dwID, pMsgDef->bLen);
#endif /*T5CAN_OEMALLOC*/
        pMsg->pDef = pMsgDef;
        pMsg->dwID = pMsgDef->dwID;
        pMsg->bLen = pMsgDef->bLen;
        if (pMsg->bLen > T5CAN_MAXDATA)
            pMsg->bLen = T5CAN_MAXDATA;
        if ((pMsgDef->bMode & T5CANMSG_HASDATA) == 0)
            pMsgDef++;
        else
        {
            pMsgDef++;
            pData = (T5_PTBYTE)pMsgDef;
            T5_MEMCPY (pMsg->data, pData, T5CAN_MAXDATA);
            pData += T5CAN_MAXDATA;
            pMsgDef = (T5PTR_CANMSGDEF)pData;
        }
    }
    /* then all OUT messages */
    pMsgDef = (T5PTR_CANMSGDEF)T5_OFF(pHeader, pDef->dwOutMsgOffset);
    for (i=0; i<pDef->wNbMsgOut; i++, pMsg++)
    {
        T5_MEMSET (pMsg, 0, sizeof (T5STR_DBCANMSG));
#ifdef T5CAN_OEMALLOC
        pMsg->data = T5CAN_OemAlloc (pPort + pMsgDef->wPort, pMsgDef->dwID, pMsgDef->bLen);
#endif /*T5CAN_OEMALLOC*/
        pMsg->pDef = pMsgDef;
        pMsg->dwID = pMsgDef->dwID;
        pMsg->bLen = pMsgDef->bLen;
        if (pMsg->bLen > T5CAN_MAXDATA)
            pMsg->bLen = T5CAN_MAXDATA;
        pMsg->wFlags |= T5CAN_OUT;
        if ((pMsgDef->bMode & T5CANMSG_RTR) != 0)
            pMsg->wFlags |= T5CAN_RTR;
        if ((pMsgDef->bMode & T5CANMSG_HASDATA) == 0)
            pMsgDef++;
        else
        {
            pMsgDef++;
            pData = (T5_PTBYTE)pMsgDef;
//----------------------------------------------------------
// mod C.Jargot email 05/04/2016
//            T5_MEMCPY (pMsg->data, pData, T5CAN_MAXDATA);
//            pData += T5CAN_MAXDATA;
            T5_MEMCPY (pMsg->data, pData, 8);
            pData += 8;
//----------------------------------------------------------
            pMsgDef = (T5PTR_CANMSGDEF)pData;
        }
    }
    /* finally application FIFOs */
    for (i=0; i<2*(pDef->wFifoSize); i++, pMsg++)
    {
        T5_MEMSET (pMsg, 0, sizeof (T5STR_DBCANMSG));
        pMsgDef++;
    }
}

/*****************************************************************************
_T5VMCan_BuildVars
Build CAN driver memory for CAN exchanged variables
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
    pCode (IN) pointer to app code
    pDef (IN/OUT) pointer to CAN static definition
*****************************************************************************/

static void _T5VMCan_BuildVars (T5PTR_DB pDB, T5_PTR pCode, T5PTR_CANDEF pDef)
{
    T5PTR_DBCANMSG pMsg;
    T5PTR_DBCANVAR pVar;
    T5PTR_CODEHEADER pHeader;
    T5PTR_CANVARDEF pVarDef;
    T5_WORD i;
    T5_PTBYTE pLock;
#ifdef T5DEF_REALSUPPORTED
    T5PTR_CANVARSCALE pScale;
#endif /*T5DEF_REALSUPPORTED*/

    pDB[T5TB_CANVAR].wNbItemUsed = pDef->wNbVar;
    pHeader = (T5PTR_CODEHEADER)pCode;
    pMsg = T5GET_DBCANMSG(pDB);
    pVar = T5GET_DBCANVAR(pDB);
    pVarDef = (T5PTR_CANVARDEF)T5_OFF(pHeader, pDef->dwVarOffset);
#ifdef T5DEF_REALSUPPORTED
    if (pDef->wVersion >= 2)
        pScale = (T5PTR_CANVARSCALE)T5_OFF(pHeader, pDef->dwScaleOffset);
    else
        pScale = NULL;
#endif /*T5DEF_REALSUPPORTED*/
    pLock = T5GET_DBLOCK(pDB);
    for (i=0; i<pDef->wNbVar; i++, pVar++, pVarDef++)
    {
        T5_MEMSET (pVar, 0, sizeof (T5STR_DBCANVAR));
        pVar->pDef = pVarDef;
#ifdef T5DEF_REALSUPPORTED
        if (pScale != NULL && (pScale->rFactor != 1.0 || pScale->rOffset != 0.0))
            pVar->pScale = pScale;
#endif /*T5DEF_REALSUPPORTED*/

        pVar->pMsg = pMsg + pVarDef->wMsg;
        if (pVarDef->bOutMsg)
            pVar->pMsg += pDef->wNbMsgIn;

        if (pVarDef->wIndex == 0) /* CT variable */
        {
            pVar->pData = (T5_PTBYTE)pDB[T5TB_CTSEG].pData;
            pVar->pData += pVarDef->dwCTOffset;
        }
        else
        {
            pVar->pData = (T5_PTBYTE)T5Tools_GetAnyParam (pDB, pVarDef->bType,
                                                pVarDef->wIndex, NULL, NULL);
            pVar->pLock = pLock + pVarDef->wIndex;
            switch (pVarDef->bType)
            {
            case T5C_BOOL :
            case T5C_SINT :
            case T5C_USINT :
                pVar->bLockMask = T5LOCK_DATA8;
                break;
            case T5C_INT :
            case T5C_UINT :
                pVar->bLockMask = T5LOCK_DATA16;
                break;
            case T5C_DINT :
            case T5C_UDINT :
            case T5C_REAL :
                pVar->bLockMask = T5LOCK_DATA32;
                break;
            case T5C_TIME :
                pVar->bLockMask = T5LOCK_TIME;
                break;
            case T5C_LINT :
            case T5C_ULINT :
            case T5C_LREAL :
                pVar->bLockMask = T5LOCK_DATA64;
                break;
            default : break;
            }
        }
#ifdef T5DEF_REALSUPPORTED
        if (pDef->wVersion >= 2)
            pScale++;
#endif /*T5DEF_REALSUPPORTED*/
    }
}

/*****************************************************************************
_T5VMCan_Open
Open all CAN ports
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
Return: OK or error
*****************************************************************************/

static T5_RET _T5VMCan_Open (T5PTR_DB pDB)
{
    T5PTR_DBCANPORT pPort;
    T5PTR_CANPORTDEF pPortDef;
    T5_WORD i;
    T5_RET wRet = T5RET_OK;

    T5Can_PreOpen ();
    pPort = T5GET_DBCANPORT(pDB);
    for (i=0; i<pDB[T5TB_CANPORT].wNbItemUsed; i++, pPort++)
    {
        pPortDef = (T5PTR_CANPORTDEF)(pPort->pDef);
        if (T5Can_Open (pPort, pPortDef->szSettings) != T5RET_OK)
            wRet = T5RET_ERROR;
    }

    _T5VMCan_JForward (pDB);

    return wRet;
}

/*****************************************************************************
T5VMCan_Close
Close all CAN ports
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
*****************************************************************************/

void T5VMCan_Close (T5PTR_DB pDB)
{
    T5PTR_DBCANPORT pPort;
    T5_WORD i;
#ifdef T5CAN_OEMALLOC
    T5PTR_DBCANMSG pMsg;
    T5PTR_CANMSGDEF pMsgDef;

    pPort = T5GET_DBCANPORT(pDB);
    pMsg = T5GET_DBCANMSG(pDB);
    for (i=0; i<pDB[T5TB_CANMSG].wNbItemAlloc; i++, pMsg++)
    {
        pMsgDef = (T5PTR_CANMSGDEF)(pMsg->pDef);
        T5CAN_OemFree (pPort + pMsgDef->wPort, pMsg->data);
    }
#endif /*T5CAN_OEMALLOC*/

    pPort = T5GET_DBCANPORT(pDB);
    for (i=0; i<pDB[T5TB_CANPORT].wNbItemUsed; i++, pPort++)
        T5Can_Close (pPort);
    T5Can_PostClose ();
}

/*****************************************************************************
T5VMCan_Exchange
Exchange data with CAN driver and activate CAN ports
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
    dwTimeStamp (IN) current time stamp
*****************************************************************************/

void T5VMCan_Exchange (T5PTR_DB pDB, T5_DWORD dwTimeStamp)
{
    T5PTR_DBCANPORT pPort;
    T5PTR_DBCANMSG pMsg;
    T5PTR_DBCANVAR pVar;
    T5PTR_CANVARDEF pVarDef;
    T5_WORD i;
    T5_BOOL bScan0;

    if (pDB[T5TB_CANPORT].wNbItemUsed == 0)
        return;

    pPort = T5GET_DBCANPORT(pDB);
    pMsg = T5GET_DBCANMSG(pDB);
    pVar = T5GET_DBCANVAR(pDB);
    bScan0 = (pPort[0].wFlags & T5CAN_SCAN0);

    /* OnPreExchange */
    for (i=0; i<pDB[T5TB_CANPORT].wNbItemUsed; i++)
        T5Can_OnPreExchange (&(pPort[i]));

    /* echange vars */
    for (i=0; i<pDB[T5TB_CANVAR].wNbItemUsed; i++)
    {
        pVarDef = (T5PTR_CANVARDEF)(pVar[i].pDef);
        switch (pVarDef->bOpe)
        {
        case T5CANOPE_IN :
            _T5VMCan_ExchangeInput (&(pVar[i]));
            break;
        case T5CANOPE_BSIN :
            _T5VMCan_ExchangeInputBS (&(pVar[i]));
            break;
        case T5CANOPE_SPNIN:
            _T5VMCan_ExchangeInputSPN (&(pVar[i]));
            break;
        case T5CANOPE_OUT:
            _T5VMCan_ExchangeOutput (&(pVar[i]));
            break;
        case T5CANOPE_BSOUT :
            _T5VMCan_ExchangeOutputBS (&(pVar[i]));
            break;
        case T5CANOPE_SPNOUT:
            _T5VMCan_ExchangeOutputSPN (&(pVar[i]));
            break;
        default:
            _T5VMCan_ExchangeSpecial (pPort, &(pVar[i]));
            break;
        }
    }

    /* activate sent messages */
    for (i=0; i<pDB[T5TB_CANMSG].wNbItemUsed; i++)
    {
        if ((pMsg[i].wFlags & T5CAN_OUT) != 0
            && (pMsg[i].wFlags & T5CAN_TOSEND) == 0
            && _T5VMCan_ShouldSend (bScan0, &(pMsg[i]), dwTimeStamp))
        {
            pMsg[i].wFlags |= T5CAN_TOSEND;
            pMsg[i].dwLastSnd = dwTimeStamp;
            _T5VMCan_Send (pPort, &(pMsg[i]));
        }
    }
    
    /* reset all "received" and "scan0" flags */
    for (i=0; i<pDB[T5TB_CANPORT].wNbItemUsed; i++)
    {
        pPort[i].wFlags &= ~T5CAN_RECVS;
        pPort->wFlags &= ~T5CAN_SCAN0;
    }
    for (i=0; i<pDB[T5TB_CANMSG].wNbItemUsed; i++)
        pMsg[i].wFlags &= ~T5CAN_RECEIVED;

    /* OnPostExchange */
    for (i=0; i<pDB[T5TB_CANPORT].wNbItemUsed; i++)
        T5Can_OnPostExchange (&(pPort[i]));
}

/*****************************************************************************
_T5VMCan_ExchangeInput
Exchange an input data with CAN driver
Parameters:
    pVar (IN) driver object for the variable
*****************************************************************************/

static void _T5VMCan_ExchangeInput (T5PTR_DBCANVAR pVar)
{
    T5_PTBYTE pBuffer;
    T5PTR_DBCANMSG pMsg;
    T5PTR_CANVARDEF pVarDef;
    T5_BYTE bBit;
    T5_NUMBUFFER bValue;
#ifdef T5DEF_REALSUPPORTED
    T5_REAL rValue;
    T5PTR_CANVARSCALE pScale;
#endif /*T5DEF_REALSUPPORTED*/

    pMsg = pVar->pMsg;
    pVarDef = (T5PTR_CANVARDEF)(pVar->pDef);

    if ((pVarDef->bOffset + pVarDef->bSize) > T5CAN_MAXDATA)
        return;

    /* not useful if nothing received */
    if ((pMsg->wFlags & T5CAN_RECEIVED) == 0)
        return;

    /* don't do it if variable is locked */
    if (pVar->pLock != NULL && (*(pVar->pLock) & pVar->bLockMask) != 0)
        return;

    /* get source data in CAN message */
    pBuffer = pMsg->data + pVarDef->bOffset;
    if (pVarDef->bMask != 0)
    {
        bBit = (*pBuffer & pVarDef->bMask) ? 1 : 0;
        pBuffer = &bBit;
    }
    else
    {
        T5_MEMCPY (&bValue, pBuffer, pVarDef->bSize);
        _T5VMCan_SwapBytes (pVarDef->bEndian, bValue.b, pVarDef->bSize);
        pBuffer = bValue.b;
    }

    /* copy and convert */
    T5VMCnv_ConvBin (pVar->pData, pBuffer, pVarDef->bType, pVarDef->bFormat);

    /* scaling */
#ifdef T5DEF_REALSUPPORTED
    if (pVar->pScale != NULL)
    {
        pScale = (T5PTR_CANVARSCALE)(pVar->pScale);
        T5VMCnv_ConvBin (&rValue, pVar->pData, T5C_REAL, pVarDef->bType);
        rValue = (rValue - pScale->rOffset) / pScale->rFactor;
        T5VMCnv_ConvBin (pVar->pData, &rValue, pVarDef->bType, T5C_REAL);
    }
#endif /*T5DEF_REALSUPPORTED*/
}

static void _T5VMCan_ExchangeInputBS (T5PTR_DBCANVAR pVar)
{
    T5_DWORD dwBuffer, dwMask;
    T5PTR_DBCANMSG pMsg;
    T5PTR_CANVARDEF pVarDef;
    T5_PTBYTE pMsgBuffer;
    T5_BYTE i, bMask;

#ifdef T5DEF_REALSUPPORTED
    T5_REAL rValue;
    T5PTR_CANVARSCALE pScale;
#endif /*T5DEF_REALSUPPORTED*/

    pMsg = pVar->pMsg;
    pVarDef = (T5PTR_CANVARDEF)(pVar->pDef);

    i = pVarDef->bSize / 8;
    if (pVarDef->bSize % 8)
        i += 1;
    if ((pVarDef->bOffset + i) > T5CAN_MAXDATA)
        return;

    /* not useful if nothing received */
    if ((pMsg->wFlags & T5CAN_RECEIVED) == 0)
        return;

    /* don't do it if variable is locked */
    if (pVar->pLock != NULL && (*(pVar->pLock) & pVar->bLockMask) != 0)
        return;

    /* get source data in CAN message */
    pMsgBuffer = pMsg->data + pVarDef->bOffset;
    bMask = pVarDef->bMask;
    dwBuffer = 0L;
    if (pVarDef->bEndian == 0) /* little endian */
    {
        dwMask = 1L;
        for (i=0; i<pVarDef->bSize; i++)
        {
            if (*pMsgBuffer & bMask)
                dwBuffer |= dwMask;
            dwMask <<= 1;
            bMask <<= 1;
            if (bMask == 0)
            {
                bMask = 1;
                pMsgBuffer++;
            }
        }
    }
    else /* big endian */
    {
        for (i=0; i<pVarDef->bSize; i++)
        {
            dwBuffer <<= 1;
            if (*pMsgBuffer & bMask)
                dwBuffer += 1L;
            bMask = (bMask >> 1) & 0x7f;
            if (bMask == 0)
            {
                bMask = 0x80;
                pMsgBuffer++;
            }
        }
    }

    /* copy and convert */
    T5VMCnv_ConvBin (pVar->pData, &dwBuffer, pVarDef->bType, pVarDef->bFormat);

    /* scaling */
#ifdef T5DEF_REALSUPPORTED
    if (pVar->pScale != NULL)
    {
        pScale = (T5PTR_CANVARSCALE)(pVar->pScale);
        T5VMCnv_ConvBin (&rValue, pVar->pData, T5C_REAL, pVarDef->bType);
        rValue = (rValue - pScale->rOffset) / pScale->rFactor;
        T5VMCnv_ConvBin (pVar->pData, &rValue, pVarDef->bType, T5C_REAL);
    }
#endif /*T5DEF_REALSUPPORTED*/
}

static void _T5VMCan_ExchangeInputSPN (T5PTR_DBCANVAR pVar)
{
    T5PTR_DBCANMSG pMsg;
    T5PTR_CANVARDEF pVarDef;
    T5_PTBYTE pMsgBuffer;
    T5_DWORD dwBuffer;
#ifdef T5DEF_REALSUPPORTED
    T5_REAL rValue;
    T5PTR_CANVARSCALE pScale;
#endif /*T5DEF_REALSUPPORTED*/

    pMsg = pVar->pMsg;
    pVarDef = (T5PTR_CANVARDEF)(pVar->pDef);

    /* not useful if nothing received */
    if ((pMsg->wFlags & T5CAN_RECEIVED) == 0)
        return;

    /* don't do it if variable is locked */
    if (pVar->pLock != NULL && (*(pVar->pLock) & pVar->bLockMask) != 0)
        return;

    /* get source data in CAN message */
    pMsgBuffer = pMsg->data + pVarDef->bOffset;
    dwBuffer = (T5_DWORD)(*pMsgBuffer);
    dwBuffer |= (((T5_DWORD)(pMsgBuffer[1])) << 8);
    dwBuffer |= (((T5_DWORD)(pMsgBuffer[2] >> 5)) << 16);

    /* copy and convert */
    T5VMCnv_ConvBin (pVar->pData, &dwBuffer, pVarDef->bType, T5C_UDINT);

    /* scaling */
#ifdef T5DEF_REALSUPPORTED
    if (pVar->pScale != NULL)
    {
        pScale = (T5PTR_CANVARSCALE)(pVar->pScale);
        T5VMCnv_ConvBin (&rValue, pVar->pData, T5C_REAL, pVarDef->bType);
        rValue = (rValue - pScale->rOffset) / pScale->rFactor;
        T5VMCnv_ConvBin (pVar->pData, &rValue, pVarDef->bType, T5C_REAL);
    }
#endif /*T5DEF_REALSUPPORTED*/
}

/*****************************************************************************
_T5VMCan_ExchangeOutput
Exchange an output data with CAN driver
Parameters:
    pVar (IN) driver object for the variable
*****************************************************************************/

static void _T5VMCan_ExchangeOutput (T5PTR_DBCANVAR pVar)
{
    T5_PTBYTE pBuffer;
    T5PTR_DBCANMSG pMsg;
    T5PTR_CANVARDEF pVarDef;
    T5_NUMBUFFER bValue;
#ifdef T5DEF_REALSUPPORTED
    T5_REAL rValue;
    T5PTR_CANVARSCALE pScale;
#endif /*T5DEF_REALSUPPORTED*/

    pMsg = pVar->pMsg;
    pVarDef = (T5PTR_CANVARDEF)(pVar->pDef);

    if ((pVarDef->bOffset + pVarDef->bSize) > T5CAN_MAXDATA)
        return;

    /* get source data in CAN message */
    pBuffer = pMsg->data + pVarDef->bOffset;

    /* scaling */
#ifdef T5DEF_REALSUPPORTED
    if (pVar->pScale != NULL)
    {
        pScale = (T5PTR_CANVARSCALE)(pVar->pScale);
        T5VMCnv_ConvBin (&rValue, pVar->pData, T5C_REAL, pVarDef->bType);
        rValue = (rValue * pScale->rFactor) + pScale->rOffset;
        T5VMCnv_ConvBin (bValue.b, &rValue, pVarDef->bFormat, T5C_REAL);
    }
    else
#endif /*T5DEF_REALSUPPORTED*/

    /* convert data value to local buffer */
    {
        T5VMCnv_ConvBin (bValue.b, pVar->pData, pVarDef->bFormat, pVarDef->bType);
    }

    /* endian */
    _T5VMCan_SwapBytes (pVarDef->bEndian, bValue.b, pVarDef->bSize);

    /* test for change */
    if (pVarDef->bMask != 0)
    {
        if (((bValue.b[0] != 0) ^ ((*pBuffer & pVarDef->bMask) != 0)) == 0)
            return;
    }
    else
    {
        if (T5_MEMCMP (pBuffer, &bValue, pVarDef->bSize) == 0)
            return;
    }

    /* copy and mark as changed */
    if (pVarDef->bMask != 0)
    {
        if (bValue.b[0] != 0)
            *pBuffer |= pVarDef->bMask;
        else
            *pBuffer &= ~(pVarDef->bMask);
    }
    else
        T5_MEMCPY (pBuffer, &bValue, pVarDef->bSize);
    pMsg->wFlags |= T5CAN_CHANGED;
}

static void _T5VMCan_ExchangeOutputBS (T5PTR_DBCANVAR pVar)
{
    T5_PTBYTE pMsgBuffer;
    T5PTR_DBCANMSG pMsg;
    T5PTR_CANVARDEF pVarDef;
#ifdef T5DEF_REALSUPPORTED
    T5_REAL rValue;
    T5PTR_CANVARSCALE pScale;
#endif /*T5DEF_REALSUPPORTED*/
    T5_DWORD dwValue, dwMask;
    T5_BYTE i, bMask;
    T5_BOOL bChange;

    pMsg = pVar->pMsg;
    pVarDef = (T5PTR_CANVARDEF)(pVar->pDef);

    i = pVarDef->bSize / 8;
    if (pVarDef->bSize % 8)
        i += 1;
    if ((pVarDef->bOffset + i) > T5CAN_MAXDATA)
        return;

    /* scaling */
#ifdef T5DEF_REALSUPPORTED
    if (pVar->pScale != NULL)
    {
        pScale = (T5PTR_CANVARSCALE)(pVar->pScale);
        T5VMCnv_ConvBin (&rValue, pVar->pData, T5C_REAL, pVarDef->bType);
        rValue = (rValue * pScale->rFactor) + pScale->rOffset;
        T5VMCnv_ConvBin (&dwValue, &rValue, pVarDef->bFormat, T5C_REAL);
    }
    else
#endif /*T5DEF_REALSUPPORTED*/

    /* convert data value to local buffer */
    {
        T5VMCnv_ConvBin (&dwValue, pVar->pData, pVarDef->bFormat, pVarDef->bType);
    }

    /* copy and check for change */
    bChange = FALSE;
    pMsgBuffer = pMsg->data + pVarDef->bOffset;
    bMask = pVarDef->bMask;
    if (pVarDef->bEndian == 0) /* little endian */
    {
        dwMask = 1L;
        for (i=0; i<pVarDef->bSize; i++)
        {
            if (((*pMsgBuffer & bMask) != 0) ^ ((dwValue & dwMask) != 0))
            {
                if ((dwValue & dwMask) != 0)
                    *pMsgBuffer |= bMask;
                else
                    *pMsgBuffer &= ~bMask;
                bChange = TRUE;
            }
            dwMask <<= 1;
            bMask <<= 1;
            if (bMask == 0)
            {
                bMask = 1;
                pMsgBuffer++;
            }
        }
    }
    else /* big endian */
    {
        dwMask = 1L << (pVarDef->bSize - 1);
        for (i=0; i<pVarDef->bSize; i++)
        {
            if (((*pMsgBuffer & bMask) != 0) ^ ((dwValue & dwMask) != 0))
            {
                if ((dwValue & dwMask) != 0)
                    *pMsgBuffer |= bMask;
                else
                    *pMsgBuffer &= ~bMask;
                bChange = TRUE;
            }
            dwMask = (dwMask >> 1) & 0x7fffffffL;
            bMask = (bMask >> 1) & 0x7f;
            if (bMask == 0)
            {
                bMask = 0x80;
                pMsgBuffer++;
            }
        }
    }

    /* mark as changed */
    if (bChange)
        pMsg->wFlags |= T5CAN_CHANGED;
}

static void _T5VMCan_ExchangeOutputSPN (T5PTR_DBCANVAR pVar)
{
    T5_PTBYTE pMsgBuffer;
    T5PTR_DBCANMSG pMsg;
    T5PTR_CANVARDEF pVarDef;
#ifdef T5DEF_REALSUPPORTED
    T5_REAL rValue;
    T5PTR_CANVARSCALE pScale;
#endif /*T5DEF_REALSUPPORTED*/
    T5_DWORD dwValue, dwBefore;
    T5_BOOL bChange;

    pMsg = pVar->pMsg;
    pVarDef = (T5PTR_CANVARDEF)(pVar->pDef);

    /* scaling */
#ifdef T5DEF_REALSUPPORTED
    if (pVar->pScale != NULL)
    {
        pScale = (T5PTR_CANVARSCALE)(pVar->pScale);
        T5VMCnv_ConvBin (&rValue, pVar->pData, T5C_REAL, pVarDef->bType);
        rValue = (rValue * pScale->rFactor) + pScale->rOffset;
        T5VMCnv_ConvBin (&dwValue, &rValue, pVarDef->bFormat, T5C_REAL);
    }
    else
#endif /*T5DEF_REALSUPPORTED*/

    /* convert data value to local buffer */
    {
        T5VMCnv_ConvBin (&dwValue, pVar->pData, T5C_UDINT, pVarDef->bType);
    }

    /* copy and check for change */
    pMsgBuffer = pMsg->data + pVarDef->bOffset;
    dwBefore = (T5_DWORD)(*pMsgBuffer);
    dwBefore |= (((T5_DWORD)(pMsgBuffer[1])) << 8);
    dwBefore |= (((T5_DWORD)(pMsgBuffer[2] >> 5)) << 16);
    bChange = (dwBefore != dwValue);

    pMsgBuffer[0] = (T5_BYTE)(dwValue & 0xffL);
    pMsgBuffer[1] = (T5_BYTE)((dwValue >> 8) & 0xffL);
    pMsgBuffer[2] &= 0x1F;
    pMsgBuffer[2] |= (T5_BYTE)((dwValue >> 11) & 0xe0);

    /* mark as changed */
    if (bChange)
        pMsg->wFlags |= T5CAN_CHANGED;
}

/*****************************************************************************
_T5VMCan_SwapBytes
Swap data bytes if different endian
Parameters:
    bBigEndian (IN) TRUE if big endian data in buffer
    pBuffer (IN) pointer to data
    bSize (IN) length of data
*****************************************************************************/

static void _T5VMCan_SwapBytes (T5_BYTE bBigEndian, T5_PTBYTE pBuffer, T5_BYTE bSize)
{
    T5_BYTE i, b;

#ifdef T5DEF_BIGENDIAN
    if (bBigEndian || bSize < 2) return;
#else /*T5DEF_BIGENDIAN*/
    if (!bBigEndian || bSize < 2) return;
#endif /*T5DEF_BIGENDIAN*/

    for (i=0; i<bSize/2; i++)
    {
        b = pBuffer[i];
        pBuffer[i] = pBuffer[bSize-i-1];
        pBuffer[bSize-i-1] = b;
    }
}

/*****************************************************************************
_T5VMCan_ShouldSend
Check if a message has to be sent
Parameters:
    bScan0 (IN) TRUE on first scan
    pMsg (IN) pointer to message object in driver memory
    dwTimeStamp (IN) current time stamp
Return: TRUE if message must be sent
*****************************************************************************/

static T5_BOOL _T5VMCan_ShouldSend (T5_BOOL bScan0, T5PTR_DBCANMSG pMsg,
                                    T5_DWORD dwTimeStamp)
{
    T5PTR_CANMSGDEF pMsgDef;
    T5_DWORD dwElapsed;
    T5_BOOL bSend;

    if (pMsg->dwLastSnd == 0)
        pMsg->dwLastSnd = dwTimeStamp;
    dwElapsed = T5_BOUNDTIME(dwTimeStamp - pMsg->dwLastSnd);

    pMsgDef = (T5PTR_CANMSGDEF)(pMsg->pDef);
    switch (T5CANMSG_GETMODE(pMsgDef->bMode))
    {
    case T5CANMSG_PERIO :
        bSend = bScan0 || (dwElapsed >= pMsgDef->dwTMin);
        break;
    case T5CANMSG_ONCHANGE :
        if (bScan0)
            bSend = TRUE;
        else if (pMsgDef->dwTMax != 0L && dwElapsed >= pMsgDef->dwTMax)
            bSend = TRUE;
        else if ((pMsg->wFlags & T5CAN_CHANGED) != 0
            && dwElapsed >= pMsgDef->dwTMin)
            bSend = TRUE;
        else
            bSend = FALSE;
        break;
    case T5CANMSG_SHOT :
        bSend = ((pMsg->wFlags & T5CAN_SHOT) != 0);
        pMsg->wFlags &= ~T5CAN_SHOT;
        break;
    default :
        bSend = FALSE;
        break;
    }
    return bSend;
}

/*****************************************************************************
_T5VMCan_Send
Put a message in the send queue
Parameters:
    pPort (IN) pointer to the FIRST port object in driver memory
    pMsg (IN) pointer to message object in driver memory
*****************************************************************************/

static void _T5VMCan_Send (T5PTR_DBCANPORT pPort, T5PTR_DBCANMSG pMsg)
{
    T5PTR_CANMSGDEF pMsgDef;

    pMsgDef = (T5PTR_CANMSGDEF)(pMsg->pDef);
    pPort += pMsgDef->wPort;

    if (pPort->pOutHead == NULL)
        pPort->pOutHead = pMsg;
    else
        pPort->pOutQueue->pNext = pMsg;
    pPort->pOutQueue = pMsg;
}

/*****************************************************************************
_T5VMCan_ExchangeSpecial
Exchange a diagnostic or control data with CAN driver
Parameters:
    pPortBase (IN) pointer to the FIRST port object in driver memory
    pVar (IN) driver object for the variable
*****************************************************************************/

static void _T5VMCan_ExchangeSpecial (T5PTR_DBCANPORT pPortBase, T5PTR_DBCANVAR pVar)
{
    T5PTR_DBCANMSG pMsg;
    T5PTR_CANMSGDEF pMsgDef;
    T5PTR_CANVARDEF pVarDef;
    T5PTR_DBCANPORT pPort;
    T5_DWORD dwValue;

    pMsg = pVar->pMsg;
    pMsgDef = (T5PTR_CANMSGDEF)(pMsg->pDef);
    pVarDef = (T5PTR_CANVARDEF)(pVar->pDef);
    pPort = pPortBase + pMsgDef->wPort;
    switch (pVarDef->bOpe)
    {
    case T5CANOPE_WARN :
        dwValue = T5Can_GetErrors (pPort);
        break;
    case T5CANOPE_BUSOFF :
        dwValue = (T5Can_GetBusOffFlag (pPort)) ? 1L : 0L;
        break;
    case T5CANOPE_SPSTAT :
        dwValue = pMsg->dwSpcIn;
        break;
    case T5CANOPE_SPCMD :
        T5VMCnv_ConvBin (&dwValue, pVar->pData, T5C_UDINT, pVarDef->bType);
        if (pMsg->dwSpcOut != dwValue) /* change of value */
        {
            pMsg->dwSpcOut = dwValue;
            T5CAN_OnSpcOut (pPort, pMsg);
        }
        break;
    case T5CANOPE_RCV :
        dwValue = ((pMsg->wFlags & T5CAN_RECEIVED) != 0) ? 1L : 0L;
        break;
    case T5CANOPE_RCVGLO :
        dwValue = ((pPort->wFlags & T5CAN_RECVS) != 0) ? 1L : 0L;
        break;
    case T5CANOPE_CMD :
        dwValue = 0L;
        T5VMCnv_ConvBin (&dwValue, pVar->pData, T5C_BOOL, pVarDef->bType);
        if (dwValue != 0L)
        {
            pMsg->wFlags |= T5CAN_SHOT;
            dwValue = 0L;
            T5VMCnv_ConvBin (pVar->pData, &dwValue, pVarDef->bType, T5C_BOOL);
        }
        break;
    case T5CANOPE_SETLEN :
        dwValue = 0;
        T5VMCnv_ConvBin (&dwValue, pVar->pData, T5C_UDINT, pVarDef->bType);
        if (dwValue <= T5CAN_MAXDATA && (pMsg->wFlags & T5CAN_RECEIVED) == 0)
        {
            pMsg->bLen = (T5_BYTE)dwValue;
            if (pMsg->bLen > T5CAN_MAXDATA)
                pMsg->bLen = T5CAN_MAXDATA;
        }
        break;
    default : break;
    }

    /* update variables - diagnostic only */
    switch (pVarDef->bOpe)
    {
    case T5CANOPE_WARN :
    case T5CANOPE_BUSOFF :
    case T5CANOPE_RCV :
    case T5CANOPE_RCVGLO :
    case T5CANOPE_SPSTAT :
        if (pVar->pLock == NULL || (*(pVar->pLock) & pVar->bLockMask) == 0)
            T5VMCnv_ConvBin (pVar->pData, &dwValue, pVarDef->bType, T5C_UDINT);
        break;
    default : break;
    }
}

/*****************************************************************************
T5VMCan_PeekNextSndMsg - FOR USE IN T5CAN LOWER LAYER
Get next message to send - leave it in queue
Parameters:
    pPort (IN) pointer to the port object in driver memory
Return value: pointer to the next message - NULL if empty queue
*****************************************************************************/

T5PTR_DBCANMSG T5VMCan_PeekNextSndMsg (T5PTR_DBCANPORT pPort)
{
    T5PTR_DBCANMSG pMsg;
#ifndef T5CAN_OEMALLOC
    T5_WORD i;
#endif /*T5CAN_OEMALLOC*/

    pMsg = pPort->pOutHead;
    if (pMsg != NULL)
        return pMsg;

#ifndef T5CAN_OEMALLOC
    pMsg = pPort->pSndFifo;
    for (i=0; i<pPort->wFifoSize; i++, pMsg++)
    {
        if ((pMsg->wFlags & T5CAN_TOSEND) != 0)
            return pMsg;
    }
#endif /*T5CAN_OEMALLOC*/

    return NULL;
}

/*****************************************************************************
T5VMCan_PopNextSndMsg - FOR USE IN T5CAN LOWER LAYER
Get next message to send and remove it from the send queue
Parameters:
    pPort (IN) pointer to the port object in driver memory
Return value: pointer to the next message - NULL if empty queue
*****************************************************************************/

T5PTR_DBCANMSG T5VMCan_PopNextSndMsg (T5PTR_DBCANPORT pPort)
{
    T5PTR_DBCANMSG pHead;
#ifndef T5CAN_OEMALLOC
    T5PTR_DBCANMSG pMsg;
    T5_WORD i;
#endif /*T5CAN_OEMALLOC*/

    pHead = pPort->pOutHead;
    if (pPort->pOutHead != NULL)
    {
        pPort->pOutHead = (T5PTR_DBCANMSG)(pHead->pNext);
        pHead->pNext = NULL;
        pHead->wFlags &= ~(T5CAN_TOSEND | T5CAN_CHANGED | T5CAN_SHOT);
        return pHead;
    }

#ifndef T5CAN_OEMALLOC
    pMsg = pPort->pSndFifo;
    for (i=0; i<pPort->wFifoSize; i++, pMsg++)
    {
        if ((pMsg->wFlags & T5CAN_TOSEND) != 0)
        {
            pMsg->wFlags &= ~T5CAN_TOSEND;
            return pMsg;
        }
    }
#endif /*T5CAN_OEMALLOC*/

    return NULL;
}

/*****************************************************************************
T5VMCan_FindRcvMsg - FOR USE IN T5CAN LOWER LAYER
Find a received message object in driver memory - binary search in sorted list
Parameters:
    pPort (IN) pointer to the port object in driver memory
    dwID (IN) CAN identifier of the wearched message
Return value: pointer to found message - NULL if not found
*****************************************************************************/

T5PTR_DBCANMSG T5VMCan_FindRcvMsg (T5PTR_DBCANPORT pPort, T5_DWORD dwID)
{
    T5PTR_CANPORTDEF pPortDef;
    T5PTR_DBCANMSG pIn;
    int i, iCenter, iLow, iHigh;

    pPortDef = (T5PTR_CANPORTDEF)(pPort->pDef);
    pIn = pPort->pMsgIn;

    i = iLow = 0;
    iHigh = (int)(pPortDef->wNbMsgIn);

    if (iHigh == 0)
        return NULL;

    while (iLow <= iHigh)
    { 
        iCenter = (iLow + iHigh) / 2;
        if (pIn[iCenter].dwID == dwID)
            return &(pIn[iCenter]);
        else if (dwID < pIn[iCenter].dwID)
            iHigh = iCenter - 1;
        else
            iLow = iCenter + 1;
    }

#ifndef T5CAN_OEMALLOC
    /* search in application FIFO */
    pIn = pPort->pRcvFifo;
    for (i=0; i<pPort->wFifoSize; i++, pIn++)
    {
        if ((pIn->wFlags & T5CAN_RECEIVED) == 0)
        {
            T5_MEMSET (pIn, 0, sizeof (T5STR_DBCANMSG));
            pIn->dwID = dwID;
            pIn->bLen = T5CAN_MAXDATA;
            pIn->wFlags |= (T5CAN_RECEIVED | T5CAN_APPFIFO);
            pIn->bCO |= 0x80;
            return pIn;
        }
    }
#endif /*T5CAN_OEMALLOC*/
    return NULL;
}

/*****************************************************************************
T5VMCan_MarkRcvMsg - FOR USE IN T5CAN LOWER LAYER
Mark a message object as "received"
Parameters:
    pPort (IN) pointer to the port object in driver memory
    pMsg (IN) pointer to the received message object
*****************************************************************************/

void T5VMCan_MarkRcvMsg (T5PTR_DBCANPORT pPort, T5PTR_DBCANMSG pMsg)
{
    T5PTR_CANPORTDEF pPortDef;

    pMsg->wFlags |= T5CAN_RECEIVED;
    pMsg->bCO |= 0x80;
    pPort->wFlags |= T5CAN_RECVS;

    /* for CAN open messages, mask toggle bit in node guarding answers */
    pPortDef = (T5PTR_CANPORTDEF)(pPort->pDef);
    if ((pPortDef->wFlags & T5CANPORT_CO) != 0 && pMsg->dwID >= 0x700 && pMsg->dwID <= 0x77F)
    {
        pMsg->data[0] &= 0x7f;
    }
}

/*****************************************************************************
T5VMCan_GetSndMsgList / T5VMCan_GetRcvMsgList
Get configured message lists - FOR USE IN T5CAN LOWER LAYER
Parameters:
    pPort (IN) pointer to the port object in driver memory
    pwNb (OUT) filled on output with list size
Return: pointer to an array of messages
*****************************************************************************/

T5PTR_DBCANMSG T5VMCan_GetSndMsgList (T5PTR_DBCANPORT pPort, T5_PTWORD pwNb)
{
    T5PTR_CANPORTDEF pPortDef;

    if (pwNb != NULL)
    {
        pPortDef = (T5PTR_CANPORTDEF)(pPort->pDef);
        *pwNb = pPortDef->wNbMsgOut;
    }
    return pPort->pMsgOut;
}

T5PTR_DBCANMSG T5VMCan_GetRcvMsgList (T5PTR_DBCANPORT pPort, T5_PTWORD pwNb)
{
    T5PTR_CANPORTDEF pPortDef;

    if (pwNb != NULL)
    {
        pPortDef = (T5PTR_CANPORTDEF)(pPort->pDef);
        *pwNb = pPortDef->wNbMsgIn;
    }
    return pPort->pMsgIn;
}

/****************************************************************************/
/* the rest of the file is for handling of FIFOs by the application */

/*****************************************************************************
_T5VMCan_FindPort
Search for a configured CAN port
Parameters:
    pDB (IN) pointer to the VMDB
    szPort (IN) settings of the searched port
Return: pointer to the port or NULL if not found
*****************************************************************************/

static T5PTR_DBCANPORT _T5VMCan_FindPort (T5PTR_DB pDB, T5_PTCHAR szPort)
{
    T5PTR_DBCANPORT pPort;
    T5_WORD wPort;

    /* no specification: 1rst port */
    pPort = T5GET_DBCANPORT (pDB);
    if (*szPort == '\0')
        return pPort;

    /* get by settings */
    for (wPort=0; wPort<pDB[T5TB_CANPORT].wNbItemUsed; wPort++, pPort++)
    {
        if (T5_STRCMP (szPort, pPort->szSettings) == 0)
            return pPort;
    }

    /* get by index (1 based) */
    if (*szPort >= '1' && *szPort <= '9')
    {
        wPort = T5_ATOI (szPort) - 1;
        if (wPort < pDB[T5TB_CANPORT].wNbItemUsed)
        {
            pPort = T5GET_DBCANPORT (pDB);
            pPort += wPort;
            return pPort;
        }
    }

    /* not found */
    return NULL;
}

/*****************************************************************************
T5VMCan_SendAppMsg
Put a sent message in the application FIFO
Parameters:
    pDB (IN) pointer to the VMDB
    szPort (IN) settings of the searched port
    dwID (IN) COB ID
    lLen (IN) message data length
    pData (IN) message data
    bRTR (IN) RTR flag
Return: TRUE if OK / FALSE if FIFO is full
*****************************************************************************/

T5_BOOL T5VMCan_SendAppMsg (T5PTR_DB pDB, T5_PTCHAR szPort, T5_DWORD dwID,
                            T5_LONG lLen, T5_PTBYTE pData, T5_BOOL bRTR)
{
#ifndef T5CAN_OEMALLOC
    T5PTR_DBCANPORT pPort;
    T5PTR_DBCANMSG pMsg;
    T5_WORD i;
    T5_UNUSED(bRTR);

    pPort = _T5VMCan_FindPort (pDB, szPort);
    if (pPort == NULL)
        return FALSE;

    pMsg = pPort->pSndFifo;
    if (pMsg == NULL)
        return FALSE;

    if (lLen < 0 || lLen > T5CAN_MAXDATA)
        return FALSE;

    for (i=0; i<pPort->wFifoSize; i++, pMsg++)
    {
        if ((pMsg->wFlags & T5CAN_TOSEND) == 0)
        {
            T5_MEMSET (pMsg, 0, sizeof (T5STR_DBCANMSG));
            pMsg->dwID = dwID;
            pMsg->bLen = (T5_BYTE)lLen;
            T5_MEMCPY (pMsg->data, pData, pMsg->bLen);
            pMsg->wFlags |= (T5CAN_TOSEND | T5CAN_APPFIFO);
            if (bRTR)
                pMsg->wFlags |= T5CAN_RTR;
            return TRUE;
        }
    }
#endif /*T5CAN_OEMALLOC*/
    return FALSE;
}

/*****************************************************************************
T5VMCan_SendAppMsg
Pop a received message from the application FIFO
Parameters:
    pDB (IN) pointer to the VMDB
    szPort (IN) settings of the searched port
    pdwID (OUT) on output: COB ID
    pbLen (OUT) on output: message data length
    pData (OUT) on output: message data
Return: TRUE if OK / FALSE if nothing received
*****************************************************************************/

T5_BOOL T5VMCan_RcvAppMsg (T5PTR_DB pDB, T5_PTCHAR szPort, T5_PTDWORD pdwID,
                           T5_PTBYTE pbLen, T5_PTBYTE pData)
{
#ifndef T5CAN_OEMALLOC
    T5PTR_DBCANPORT pPort;
    T5PTR_DBCANMSG pMsg;
    T5_WORD i;

    pPort = _T5VMCan_FindPort (pDB, szPort);
    if (pPort == NULL)
        return FALSE;

    pMsg = pPort->pRcvFifo;
    if (pMsg == NULL)
        return FALSE;

    for (i=0; i<pPort->wFifoSize; i++, pMsg++)
    {
        if ((pMsg->wFlags & T5CAN_RECEIVED) != 0)
        {
            *pdwID = pMsg->dwID;
            *pbLen = pMsg->bLen;
            T5_MEMCPY (pData, pMsg->data, pMsg->bLen);
            pMsg->wFlags &= ~T5CAN_RECEIVED;
            return TRUE;
        }
    }
#endif /*T5CAN_OEMALLOC*/
    return FALSE;
}

/*function block for the application *****************************************/


#define _P_PORT     (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[0]))
#define _P_DATA     (T5Tools_GetD8Array(pBase,pArgs[1]))
#define _P_DATASIZE pArgs[2]
#define _P_OK       (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))
#define _P_ID       (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[4]))
#define _P_LEN      (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[5]))

typedef struct
{
    T5_DWORD dwID;
    T5_BOOL bOK;
    T5_BYTE bLen;
} _str_FB_CANRCVMSG;

T5_DWORD T5stfb_CANRCVMSG (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass,
                           T5_PTR pInst, T5_PTWORD pArgs)
{
    _str_FB_CANRCVMSG *pData;
    T5_UNUSED(pClass);

    pData = (_str_FB_CANRCVMSG *)pInst;
    switch (wCommand)
    {
    case T5FBCMD_ACTIVATE :
#ifdef T5CAN_OEMALLOC
        pData->bOK = FALSE;
        pData->dwID = 0L;
        pData->bLen = 0;
#else /*T5CAN_OEMALLOC*/
        if (_P_DATASIZE < T5CAN_MAXDATA)
        {
            pData->bOK = FALSE;
            pData->dwID = 0L;
            pData->bLen = 0;
        }
        else
        {
            pData->bOK = T5VMCan_RcvAppMsg (pBase, _P_PORT + 2, &(pData->dwID),
                                            &(pData->bLen), _P_DATA);
        }
        _P_OK = pData->bOK;
        _P_ID = pData->dwID;
        _P_LEN = pData->bLen;
#endif /*T5CAN_OEMALLOC*/
        return 0L;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FB_CANRCVMSG);
    case T5FBCMD_ACCEPTCT :
        return 1L;
    default :
        return 0L;
    }
}

#undef _P_PORT
#undef _P_DATA
#undef _P_DATASIZE
#undef _P_OK
#undef _P_ID
#undef _P_LEN

/****************************************************************************/
/* J1939 forwarding */

static void _T5VMCan_JForward (T5PTR_DB pDB)
{
#ifdef T5DEF_CANJF
    T5PTR_CANDEF pDef;
    T5PTR_DBCANPORT pPort;
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_CANJF pJF;
    T5_PTCHAR sz, szPort[8];
    T5_BYTE i, bMask, nbPort;

    pPrivate = T5GET_DBPRIVATE (pDB);
    pDef = T5VMCode_GetCanDef (pPrivate->pCode);
    if (pDef->wVersion >= 3)
    {
        /* parse list of port names */
        for (i=0; i<8; i++)
            szPort[i] = "\0";
        nbPort = 0;
        sz = (T5_PTCHAR)T5_OFF (pPrivate->pCode, pDef->dwCanJFPorts);
        while (*sz != '\0')
        {
            if (nbPort < 8)
                szPort[nbPort++] = sz;
            while (*sz)
                sz++;
            sz++;
        }

        /* parse liste of forward records */
        pJF = (T5PTR_CANJF)T5_OFF (pPrivate->pCode, pDef->dwCanJF);
        pPort = T5GET_DBCANPORT(pDB);
        T5CANJF_Begin ();
        while (pJF->bFlags != T5CANJF_END)
        {
            bMask = 1;
            for (i=0; i<8; i++, bMask <<= 1)
            {
                if ((pJF->bDest & bMask) != 0 && *(szPort[i]) != '\0')
                {
                    if ((pJF->bFlags & T5CANJF_REPEAT) != 0)
                    {
                        T5CANJF_Repeat (szPort[pJF->bSrc], szPort[i]);
                    }
                    else if ((pJF->bFlags & T5CANJF_BRIDGE) != 0)
                    {
                        T5CANJF_Bridge (szPort[pJF->bSrc], szPort[i], pJF->dwID,
                            ((pJF->bFlags & T5CANJF_SAMASK) != 0) ? TRUE : FALSE,
                            ((pJF->bFlags & T5CANJF_PRIOMASK) != 0) ? TRUE : FALSE);
                    }
                    else if ((pJF->bFlags & T5CANJF_ROUTE) != 0)
                    {
                        T5CANJF_Route (szPort[pJF->bSrc], szPort[i], pJF->dwID, pJF->bSA,
                            ((pJF->bFlags & T5CANJF_SAMASK) != 0) ? TRUE : FALSE,
                            ((pJF->bFlags & T5CANJF_PRIOMASK) != 0) ? TRUE : FALSE);
                    }
                }
            }
            pJF++;
        }
        T5CANJF_End (); 
    }
#endif /*T5DEF_CANJF*/
}

/****************************************************************************/

#endif /*T5DEF_CANBUS*/

/* eof **********************************************************************/
