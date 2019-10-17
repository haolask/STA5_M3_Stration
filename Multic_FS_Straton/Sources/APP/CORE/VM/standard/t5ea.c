/*****************************************************************************
T5EA.c :     Value change events - main core

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_EA

/****************************************************************************/
/* static services */

#ifdef T5DEF_EAHYSTER
static void    _T5EA_SetHyster (T5PTR_EACHANGE pEA, T5_PTR pHP, T5_PTR pHN);
#endif /*T5DEF_EAHYSTER*/

static void    _T5EA_RemoveEvent (T5PTR_DB pDB, T5_WORD wIndex);
static T5_DWORD _T5EA_ManageEvents (T5PTR_DB pDB, T5_PTR pEvss,
                                   T5_BOOL bBroadcast, T5_BOOL bPrivate);
static T5_BOOL _T5EA_IsChanged (T5PTR_EACHANGE pEA);
static void    _T5EA_SaveChange (T5PTR_EACHANGE pEA);

static void    _T5EAX_OnError (T5PTR_EACNX pCnx);
static void    _T5EAX_OpenPort (T5PTR_EACNX pCnx, T5_BOOL bReconnect);
static T5_BOOL _T5EAX_Receive (T5PTR_EACNX pCnx);
static T5_BOOL _T5EAX_Send (T5PTR_EACNX pCnx);
static T5_DWORD _T5EAX_OnEvent (T5PTR_DB pDB, T5PTR_EACNX pCnx);
static void    _T5EAX_DispatchStatus (T5PTR_EACNX pCnx);
static void    _T5EAX_OnRemoteAppStop (T5PTR_EACNX pCnx);
static void    _T5EAX_OnChangeEvent (T5PTR_DB pDB, T5PTR_EACNX pCnx, T5_WORD wEvent,
                                     T5_DWORD dwDate, T5_DWORD dwTime,
                                     T5_BYTE bSrcType, T5_PTBYTE pValue,
                                     T5_PTBYTE pFrame, T5_BYTE bFrameLen);
static T5_BOOL _T5EAX_CanUpdateEvent (T5PTR_EAEXT pVar, T5PTR_EAEXT pVarAux,
                                      T5_DWORD dwDate, T5_DWORD dwTime, T5_BOOL bForRed);
static void    _T5EAX_Copy (T5_BYTE bDstType, T5_PTBYTE pDst,
                            T5_BYTE bSrcType, T5_PTBYTE pSrc);
static void    _T5EAX_CopyDWStatus (T5_BYTE bDstType, T5_PTBYTE pDst,
                                    T5_DWORD dwValue);
static void    _T5EAX_DispatchStatus (T5PTR_EACNX pCnx);
static T5_DWORD _T5EAX_MakeVarStatus (T5PTR_EACNX pCnx);
static T5_BYTE  _T5EAX_GetDataSize (T5_BYTE bSrcType);

/*****************************************************************************
_T5EA_SetHyster
Set hysteresis for a new published variable
must be called before pEA->wNbCnx is increased
Parameters:
    pEA (IN) registered event
    pHP, pHN (IN) positive and negative hysteresis
*****************************************************************************/

#ifdef T5DEF_EAHYSTER

static void _T5EA_SetHyster (T5PTR_EACHANGE pEA, T5_PTR pHP, T5_PTR pHN)
{
    T5_BOOL bP, bN;

    if (pHP == NULL || pHN == NULL)
        return;

    /* skip if geater than existing one */
    bP = bN = TRUE;
    if (pEA->wNbCnx != 0)
    {
        switch (pEA->bTicType)
        {
        case T5C_SINT :
        case T5C_USINT :
            if (*(T5_PTBYTE)pHP > *(T5_PTBYTE)(pEA->bHP))
                bP = FALSE;
            if (*(T5_PTBYTE)pHN > *(T5_PTBYTE)(pEA->bHN))
                bN = FALSE;
            break;
        case T5C_INT :
        case T5C_UINT :
#ifdef T5DEF_DATA16SUPPORTED
            if (*(T5_PTWORD)pHP > *(T5_PTWORD)(pEA->bHP))
                bP = FALSE;
            if (*(T5_PTWORD)pHN > *(T5_PTWORD)(pEA->bHN))
                bN = FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
            break;
        case T5C_DINT :
        case T5C_UDINT :
        case T5C_TIME :
            if (*(T5_PTDWORD)pHP > *(T5_PTDWORD)(pEA->bHP))
                bP = FALSE;
            if (*(T5_PTDWORD)pHN > *(T5_PTDWORD)(pEA->bHN))
                bN = FALSE;
            break;
        case T5C_REAL :
#ifdef T5DEF_REALSUPPORTED
            if (*(T5_PTREAL)pHP > *(T5_PTREAL)(pEA->bHP))
                bP = FALSE;
            if (*(T5_PTREAL)pHN > *(T5_PTREAL)(pEA->bHN))
                bN = FALSE;
#endif /*T5DEF_REALSUPPORTED*/
            break;
        case T5C_LINT :
#ifdef T5DEF_LINTSUPPORTED
            if (*(T5_PTLINT)pHP > *(T5_PTLINT)(pEA->bHP))
                bP = FALSE;
            if (*(T5_PTLINT)pHN > *(T5_PTLINT)(pEA->bHN))
                bN = FALSE;
#endif /*T5DEF_LINTSUPPORTED*/
            break;
        case T5C_ULINT :
#ifdef T5DEF_ULINTSUPPORTED
            if (*(T5_PTLWORD)pHP > *(T5_PTLWORD)(pEA->bHP))
                bP = FALSE;
            if (*(T5_PTLWORD)pHN > *(T5_PTLWORD)(pEA->bHN))
                bN = FALSE;
#endif /*T5DEF_ULINTSUPPORTED*/
            break;
        case T5C_LREAL :
#ifdef T5DEF_LREALSUPPORTED
            if (*(T5_PTLREAL)pHP > *(T5_PTLREAL)(pEA->bHP))
                bP = FALSE;
            if (*(T5_PTLREAL)pHN > *(T5_PTLREAL)(pEA->bHN))
                bN = FALSE;
#endif /*T5DEF_LREALSUPPORTED*/
            break;
        default :
            bP = FALSE;
            bN = FALSE;
            break;
        }
    }
    /* copy hysteresis values */
    if (bP)
        T5_MEMCPY (pEA->bHP, pHP, pEA->bSizeOf);
    if (bN)
        T5_MEMCPY (pEA->bHN, pHN, pEA->bSizeOf);
    if (bP || bN)
        pEA->wFlags |= T5EA_HYST;
}

#endif /*T5DEF_EAHYSTER*/

/*****************************************************************************
T5EA_RegisterChangeEvent
Register a new published variable
Parameters:
    pDB (IN) pointer to the VM database
    hCnx (IN) ID of the owner connection (0 for broadcast)
    bTicType (IN) data type of the variable
    wOffset (IN) index of the variable in the VM database
    wID (IN) event ID
    pHP, pHN (IN) positive and negative hysteresis
    dwCTOffset (IN) offset in CTSeg if != 0
Return: 1-based ID of the registered event or 0 if fail
*****************************************************************************/

T5_WORD T5EA_RegisterChangeEvent (T5PTR_DB pDB, T5_WORD hCnx,
                                  T5_BYTE bTicType, T5_WORD wOffset,
                                  T5_WORD wID, T5_PTR pHP, T5_PTR pHN,
                                  T5_DWORD dwCTOffset)
{
    T5PTR_EACHANGE pEA;
    T5_WORD i, wNb;
    T5_PTBYTE pD8;
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pD16;
#endif /*T5DEF_DATA16SUPPORTED*/
    T5_PTDWORD pD32;
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 pD64;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_CTSEG
    T5_PTBYTE pCTSeg;
#endif /*T5DEF_CTSEG*/
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE *pS;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_XV
    T5_PTBYTE *pXV;
#endif /*T5DEF_XV*/

    /* check 1-based connection ID */
    if (hCnx > T5MAX_EACNX)
        return 0;

    hCnx -= 1;
    /* search for existing event */
    pEA = T5GET_EACHANGE (pDB);
    wNb = pDB[T5TB_EACHANGE].wNbItemUsed;

    if (hCnx != 0xffff)
    {
        for (i=0; i<wNb; i++)
        {
            if (pEA[i].bTicType == bTicType && pEA[i].wOffset == wOffset
                && pEA[i].dwCTOffset == dwCTOffset
                && hCnx != 0xffff
                && (pEA[i].wFlags & T5EA_BROADCAST) == 0)
            {
                if (!T5DEFEACNX_ISNONE(pEA[i].bCnxState[hCnx]))
                    return 0;
                T5DEFEACNX_SETFAILED (pEA[i].bCnxState[hCnx]);
                pEA[i].wFlags = T5EA_HASFAIL;

#ifdef T5DEF_EAHYSTER
                _T5EA_SetHyster (&(pEA[i]), pHP, pHN);
#endif /*T5DEF_EAHYSTER*/

                pEA[i].wNbCnx += 1;
                return (i + 1);
            }
        }
    }
    /* not found: check for overflow */
    if (wNb >= pDB[T5TB_EACHANGE].wNbItemAlloc)
        return 0;
    /* not found: allocate new event */
    pEA = &(pEA[wNb]);
    pDB[T5TB_EACHANGE].wNbItemUsed += 1;

    T5_MEMSET (pEA, 0, sizeof (T5STR_EACHANGE));
    pEA->bTicType = bTicType;
    pEA->wOffset = wOffset;
    pEA->dwCTOffset = dwCTOffset;

    if (hCnx != 0xffff)
        T5DEFEACNX_SETOK (pEA->bCnxState[hCnx]);
    else
    {
        for (i=0; i<T5MAX_EACNX; i++)
            T5DEFEACNX_SETFAILED (pEA->bCnxState[i]);
        pEA->wFlags = T5EA_HASFAIL | T5EA_BROADCAST;
        pEA->wID = wID;
    }

    switch (bTicType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
    case T5C_SINT :
    case T5C_USINT :
        pEA->bSizeOf = 1;
        break;
    case T5C_INT :
    case T5C_UINT :
#ifdef T5DEF_DATA16SUPPORTED
        pEA->bSizeOf = 2;
#endif /*T5DEF_DATA16SUPPORTED*/
        break;
    case T5C_DINT :
    case T5C_REAL :
    case T5C_UDINT :
        pEA->bSizeOf = 4;
        break;
    case T5C_TIME :
        pEA->bSizeOf = 4;
        break;
    case T5C_LINT :
    case T5C_ULINT :
    case T5C_LREAL :
#ifdef T5DEF_DATA64SUPPORTED
        pEA->bSizeOf = 8;
#endif /*T5DEF_DATA64SUPPORTED*/
        break;
    case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
        pEA->bSizeOf = 1;
#endif /*T5DEF_STRINGSUPPORTED*/
        break;
    default : break;
    }

    if (pEA->bSizeOf == 0)
    {
        pEA->bTicType = 0;
        return 0;
    }

#ifdef T5DEF_XV
    pXV = T5GET_DBXV(pDB);
#endif /*T5DEF_XV*/


#ifdef T5DEF_CTSEG
    pCTSeg = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    if (dwCTOffset)
    {
        pEA->pValue = pCTSeg + dwCTOffset;
#ifdef T5DEF_STRINGSUPPORTED
        if (bTicType == T5C_STRING)
            pEA->pValue += 1;
#endif /*T5DEF_STRINGSUPPORTED*/
    }
#ifdef T5DEF_XV
    else if ((bTicType & T5C_EXTERN) != 0)
    {
        pXV = T5GET_DBXV(pDB);
        pEA->pValue = (T5_PTBYTE)(pXV[wOffset]);
        if ((bTicType & T5C_EXTERN) == T5C_STRING)
            pEA->pValue += 1;
    }
#endif /*T5DEF_XV*/
    else
#endif /*T5DEF_CTSEG*/
    {
        switch (bTicType)
        {
        case T5C_BOOL :
        case T5C_SINT :
        case T5C_USINT :
            pD8 = T5GET_DBDATA8(pDB);
            pEA->pValue = (T5_PTBYTE)(pD8 + wOffset);
            break;
        case T5C_INT :
        case T5C_UINT :
#ifdef T5DEF_DATA16SUPPORTED
            pD16 = T5GET_DBDATA16(pDB);
            pEA->pValue = (T5_PTBYTE)(pD16 + wOffset);
#endif /*T5DEF_DATA16SUPPORTED*/
            break;
        case T5C_DINT :
        case T5C_REAL :
        case T5C_UDINT :
            pD32 = T5GET_DBDATA32(pDB);
            pEA->pValue = (T5_PTBYTE)(pD32 + wOffset);
            break;
        case T5C_TIME :
            pD32 = T5GET_DBTIME(pDB);
            pEA->pValue = (T5_PTBYTE)(pD32 + wOffset);
            break;
        case T5C_LINT :
        case T5C_ULINT :
        case T5C_LREAL :
#ifdef T5DEF_DATA64SUPPORTED
            pD64 = T5GET_DBDATA64(pDB);
            pEA->pValue = (T5_PTBYTE)(pD64 + wOffset);
#endif /*T5DEF_DATA64SUPPORTED*/
            break;
        case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
            pS = T5GET_DBSTRING(pDB);
            pEA->pValue = (T5_PTBYTE)(pS[wOffset] + 1);
#endif /*T5DEF_STRINGSUPPORTED*/
            break;

        default : break;
        }
    }

#ifdef T5DEF_VARMAP
    if (pEA->pValue != NULL)
    {
        if (bTicType == T5C_STRING)
            pEA->pVSI = T5Map_GetVSIByData (pDB, pEA->pValue - 1);
        else
            pEA->pVSI = T5Map_GetVSIByData (pDB, pEA->pValue);
    }
    else
        pEA->pVSI = NULL;
#endif /*T5DEF_VARMAP*/

#ifdef T5DEF_EAHYSTER
    _T5EA_SetHyster (pEA, pHP, pHN);
#endif /*T5DEF_EAHYSTER*/

    pEA->wNbCnx += 1;
    return (wNb + 1);
}

/*****************************************************************************
_T5EA_RemoveEvent
Remove a registered event
Parameters:
    pDB (IN) pointer to the VM database
    wIndex (IN) index of the event
*****************************************************************************/

static void _T5EA_RemoveEvent (T5PTR_DB pDB, T5_WORD wIndex)
{
    T5_WORD wNb;
    T5PTR_EACHANGE pEA;

    wNb = pDB[T5TB_EACHANGE].wNbItemUsed;
    if (wNb == 0 || wIndex >= wNb)
        return;

    pEA = T5GET_EACHANGE (pDB);
    if (wIndex < (wNb-1))
        T5_MEMCPY (&(pEA[wIndex]), &(pEA[wNb-1]), sizeof (T5STR_EACHANGE));

    pEA[wNb-1].bTicType = 0;
    pEA[wNb-1].wNbCnx = 0;

    pDB[T5TB_EACHANGE].wNbItemUsed -= 1;
}

/*****************************************************************************
T5EA_UnregisterChangeEvent
Unregister a new published variable
Parameters:
    pDB (IN) pointer to the VM database
    hCnx (IN) ID of the owner connection (0 for broadcast)
    bTicType (IN) data type of the variable
    wOffset (IN) index of the variable in the VM database
    dwCTOffset (IN) offset in CTSeg if != 0
Return: OK or error
*****************************************************************************/

T5_RET T5EA_UnregisterChangeEvent (T5PTR_DB pDB, T5_WORD hCnx,
                                   T5_BYTE bTicType, T5_WORD wOffset,
                                   T5_DWORD dwCTOffset)
{
    T5PTR_EACHANGE pEA;
    T5_WORD i, wNb;

    /* check 1-based connection ID */
    if (hCnx > T5MAX_EACNX)
        return T5RET_ERROR;
    hCnx -= 1;
    /* search for existing event */
    pEA = T5GET_EACHANGE (pDB);
    wNb = pDB[T5TB_EACHANGE].wNbItemUsed;
    for (i=0; i<wNb; i++)
    {
        if (pEA[i].bTicType == bTicType && pEA[i].wOffset == wOffset
            && pEA[i].dwCTOffset == dwCTOffset)
        {
            if (hCnx == 0xffff) /* broadcast */
            {
                if ((pEA[i].wFlags & T5EA_BROADCAST) != 0)
                {
                    _T5EA_RemoveEvent (pDB, i);
                    return T5RET_OK;
                }
                else
                    return T5RET_ERROR;
            }
            else if ((pEA[i].wFlags & T5EA_BROADCAST) == 0
                && !T5DEFEACNX_ISNONE (pEA[i].bCnxState[hCnx]))
            {
                T5DEFEACNX_SETNONE (pEA[i].bCnxState[hCnx]);
                pEA[i].wNbCnx -= 1;
                if (pEA[i].wNbCnx == 0)
                {
                    _T5EA_RemoveEvent (pDB, i);
                }
                return T5RET_OK;
            }
        }
    }
    /* not found */
    return T5RET_ERROR;
}

/*****************************************************************************
T5EA_UnregisterAllBroadcastEvents
Unregister all registered broadcast events
Parameters:
    pDB (IN) pointer to the VM database
Return: OK or error
*****************************************************************************/

T5_RET T5EA_UnregisterAllBroadcastEvents (T5PTR_DB pDB)
{
    T5PTR_EACHANGE pEA;
    T5_WORD i, wNb;

    /* search for existing event */
    pEA = T5GET_EACHANGE (pDB);
    wNb = pDB[T5TB_EACHANGE].wNbItemUsed;
    for (i=0; i<wNb; i++)
    {
        if ((pEA[i].wFlags & T5EA_BROADCAST) != 0)
        {
            pEA[i].wNbCnx = 0;
            pEA[i].bTicType = 0;
        }
    }
    return T5RET_OK;
}

/*****************************************************************************
T5EA_RegisterStaticEvents
Register all broadcast events from the application code
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer to the VM database
Return: OK or error
*****************************************************************************/

T5_RET T5EA_RegisterStaticEvents (T5_PTR pCode, T5PTR_DB pDB)
{
    T5_PTBYTE pEACode;
    T5PTR_EADEFVARH pDef;
    T5_PTBYTE pHP, pHN;
    T5_RET wRet, wREA;
    T5_DWORD dwCTOffset;

    T5EA_UnregisterAllBroadcastEvents (pDB);

    if (pCode == T5_PTNULL)
        return T5RET_OK;
    pEACode = (T5_PTBYTE)T5VMCode_StaticEvents (pCode);
    if (pEACode == T5_PTNULL)
        return T5RET_OK;

    wRet = T5RET_OK;
    while (*pEACode != 0)
    {
        pDef = (T5PTR_EADEFVARH)pEACode;

        pEACode += sizeof (T5STR_EADEFVARH);
        if (pDef->wHSizeof != 0)
        {
            pHP = pEACode;
            pHN = pHP + pDef->wHSizeof;
            pEACode += (2 * pDef->wHSizeof);
        }
        else
        {
            pHP = NULL;
            pHN = NULL;
        }

        if ((pDef->bOpe & T5EAOPE_CTSEG) != 0)
        {
            T5_MEMCPY (&dwCTOffset, pEACode, sizeof (T5_DWORD));
            pEACode += sizeof (T5_DWORD);
        }
        else
        {
            dwCTOffset = 0L;
        }

        wREA = T5EA_RegisterChangeEvent (
            pDB, 0,
            pDef->bTicType, pDef->wOffset,
            pDef->wEvent,
            pHP, pHN, dwCTOffset);
        if (wREA == 0)
            wRet = T5RET_ERROR;
    }
    return wRet;
}

/*****************************************************************************
T5EA_Open
Register all broadcast events and inform clients that the server starts
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer to the VM database
    pEvss (IN) pointer to an instance of an event server
Return: OK or error
*****************************************************************************/

T5_RET T5EA_Open (T5_PTR pCode, T5PTR_DB pDB, T5_PTR pEvss)
{
    pDB[T5TB_EACHANGE].wNbItemUsed = 0;
    T5_MEMSET (pDB[T5TB_EACHANGE].pData, 0,
               ((T5_DWORD)(pDB[T5TB_EACHANGE].wNbItemAlloc) * (T5_DWORD)(pDB[T5TB_EACHANGE].wSizeOf)));
    if (pEvss != T5_PTNULL)
       T5Evs_Event (pEvss, T5EVS_SYSTEM, T5EV_EAOPEN, 0, T5_PTNULL);
    return T5EA_RegisterStaticEvents (pCode, pDB);
}

/*****************************************************************************
T5EA_Close
Unregister all broadcast events and inform clients that the server stops
Parameters:
    pDB (IN) pointer to the VM database
    pEvss (IN) pointer to an instance of an event server
*****************************************************************************/

void T5EA_Close (T5PTR_DB pDB, T5_PTR pEvss)
{
    if (pEvss != T5_PTNULL)
    {
        T5Evs_FlushEAEvents (pEvss);
        T5Evs_Event (pEvss, T5EVS_SYSTEM, T5EV_EACLOSE, 0, T5_PTNULL);
    }
    pDB[T5TB_EACHANGE].wNbItemUsed = 0;
    T5_MEMSET (pDB[T5TB_EACHANGE].pData, 0,
               ((T5_DWORD)(pDB[T5TB_EACHANGE].wNbItemAlloc) * (T5_DWORD)(pDB[T5TB_EACHANGE].wSizeOf)));
}

/*****************************************************************************
T5EA_ForceGeneralUpdate
Force an update of all configured events
Parameters:
    pDB (IN) pointer to the VM database
*****************************************************************************/

void T5EA_ForceGeneralUpdate (T5PTR_DB pDB)
{
    T5PTR_EACHANGE pEA;
    T5_WORD i, nb;

    pEA = T5GET_EACHANGE (pDB);
    nb = pDB[T5TB_EACHANGE].wNbItemUsed;
    for (i=0; i<nb; i++, pEA++)
    {
        if (pEA->bTicType != 0 && (pEA->wFlags & T5EA_BROADCAST) != 0)
            pEA->wFlags &= ~T5EA_DEFINED;
    }
}

/*****************************************************************************
T5EA_ManageChangeEvents
Manage all registered events (change detection)
Parameters:
    pDB (IN) pointer to the VM database
    pEvss (IN) pointer to an instance of an event server
*****************************************************************************/

void T5EA_ManageChangeEvents (T5PTR_DB pDB, T5_PTR pEvss) /* all */
{
    T5PTR_DBSTATUS pStatus;
    T5_BOOL bFrozen;
    T5PTR_EVSS pSrv;
    T5_DWORD nb;

    pStatus = T5GET_DBSTATUS(pDB);
    bFrozen = (T5_BOOL)((pStatus->wFlags & T5FLAG_EAFREEZE) == 0);

    pSrv = (T5PTR_EVSS) pEvss;
    if (pSrv == NULL)
        return;
    /* if (!bFrozen && (pSrv->wFlags & T5EVSS_FROZEN) != 0)
        T5EA_ForceGeneralUpdate (pDB); */

    if (bFrozen)
        pSrv->wFlags |= T5EVSS_FROZEN;
    else
        pSrv->wFlags &= ~T5EVSS_FROZEN;

    nb = _T5EA_ManageEvents (pDB, pEvss, bFrozen, TRUE);
    T5_DIAGHOOK (T5_DIAGHOOK_BIND_PROD, nb);
}

void T5EA_ManageBroadcastEvents (T5PTR_DB pDB, T5_PTR pEvss) /* broadcast only */
{
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if ((pStatus->wFlags & T5FLAG_EAFREEZE) == 0)
        _T5EA_ManageEvents (pDB, pEvss, TRUE, FALSE);
}

void T5EA_ManagePrivateEvents (T5PTR_DB pDB, T5_PTR pEvss) /* private only */
{
    _T5EA_ManageEvents (pDB, pEvss, FALSE, TRUE);
}

static T5_DWORD _T5EA_ManageEvents (T5PTR_DB pDB, T5_PTR pEvss,
                                    T5_BOOL bBroadcast, T5_BOOL bPrivate)
{
    T5PTR_EACHANGE pEA;
    T5_SHORT i;
    T5_WORD wNb, iCnx, wID, wXLen;
    T5_BOOL bChange, bChangeVSI, bFail, bNewCnx, bPublic;
    T5_RET wRet;
    T5_BYTE bEv, bSizeOf;
    T5_BYTE bData[256];
    /* id(2) + type(1) + offset(2) + value(1..8) + {CT(4)} + {vsi(8)} */
    T5_DWORD dwDate, dwTime, dwd, dwt;
    T5_BOOL bVSI[T5MAX_EVCLIENT];
    T5_PTBYTE pExtra;
    T5_DWORD dwNbEvt;

    bEv = bSizeOf = 0;
    wXLen = 0;
    bPublic = FALSE;
    dwNbEvt = 0;

    if (pDB == T5_PTNULL || pEvss == T5_PTNULL)
        return 0;

    wNb = pDB[T5TB_EACHANGE].wNbItemUsed;
    if (!wNb)
        return 0;

#ifdef T5DEF_RTCLOCKSTAMP
    dwDate = T5RtClk_GetCurDateStamp ();
    dwTime = T5RtClk_GetCurTimeStamp ();
#else /*T5DEF_RTCLOCKSTAMP*/
    dwDate = 0L;
    dwTime = 0L;
#endif /*T5DEF_RTCLOCKSTAMP*/

    T5Evs_GetCnxVsiPrefs (pEvss, bVSI);

    pEA = T5GET_EACHANGE (pDB);
    pEA += (wNb-1);
    bNewCnx = T5Evs_HasNewConections (pEvss);
    for (i=(T5_SHORT)(wNb-1); i>=0; i--, pEA--)
    {
        pExtra = NULL;
        if (pEA->bTicType != 0)
            bPublic = ((pEA->wFlags & T5EA_BROADCAST) != 0);
        if (pEA->bTicType != 0
            && ((bPublic && bBroadcast) || (!bPublic && bPrivate)))
        {
            if (bNewCnx)
            {
                for (iCnx=0; iCnx<T5MAX_EACNX; iCnx++)
                {
                    if (bPublic && T5DEFEACNX_ISNONE(pEA->bCnxState[iCnx]))
                    {
                        T5DEFEACNX_SETFAILED (pEA->bCnxState[iCnx]);
                        pEA->wNbCnx += 1;
                    }
                    else if (!bPublic && !T5DEFEACNX_ISNONE(pEA->bCnxState[iCnx]))
                        T5DEFEACNX_SETFAILED (pEA->bCnxState[iCnx]);
                }
                pEA->wFlags |= T5EA_HASFAIL;
                bChange = TRUE;
            }
            else if ((pEA->wFlags & T5EA_DEFINED) == 0)
            {
                pEA->wFlags |= T5EA_DEFINED;
                bChange = TRUE;
            }
            else
            {
                bChange = _T5EA_IsChanged (pEA);
            }

            bChangeVSI = pEA->pVSI && (pEA->bLastVSIHi != pEA->pVSI->dwStatHi
                                     || pEA->bLastVSILo != pEA->pVSI->dwStatLo);
            if (bChangeVSI)
            {
                pEA->bLastVSIHi = pEA->pVSI->dwStatHi;
                pEA->bLastVSILo = pEA->pVSI->dwStatLo;
                bChange = TRUE;
            }

            dwd = (pEA->pVSI != NULL) ? pEA->pVSI->dwDate : 0;
            if (dwd)
                dwt = pEA->pVSI->dwTime;
            else
            {
                dwd = dwDate;
                dwt = dwTime;
            }

            if (bChange || (pEA->wFlags & T5EA_HASFAIL) != 0)
            {
                dwNbEvt += 1;

                if (bPublic)
                {
                    bEv = T5EV_EVBROADCAST;
                    wID = pEA->wID;
                }
                else
                {
                    bEv = T5EV_EVPRIVATE;
                    wID = i+1;
                }

                T5_COPYFRAMEWORD(bData, &wID);
                bData[2] = pEA->bTicType;
                T5_COPYFRAMEWORD(bData+3, &(pEA->wOffset));

                if (pEA->bTicType != T5C_STRING)
                {
                    bSizeOf = pEA->bSizeOf;
                    switch (bSizeOf)
                    {
                    case 2 : T5_COPYFRAMEWORD(bData+5, pEA->pValue); break;
                    case 4 : T5_COPYFRAMEDWORD(bData+5, pEA->pValue); break;
                    case 8 : T5_COPYFRAME64(bData+5, pEA->pValue); break;
                    default: T5_MEMCPY (bData+5, pEA->pValue, pEA->bSizeOf);
                    }
                }
                else 
                {
                    if (*(pEA->pValue) < T5EVS_EAMAXVAL)
                    {
                        bSizeOf = *(pEA->pValue) + 1;
                        T5_MEMCPY (bData+5, pEA->pValue, bSizeOf);
                    }
                    else
                    {
                        bSizeOf = 1;
                        bData[5] = 0;
                        pExtra = pEA->pValue;
                    }
                }

                wXLen = 5;
                if (pEA->dwCTOffset || pEA->wOffset == 0)
                {
                    T5_MEMSET(bData+3, 0, 2); /* erase offset */
                    T5_COPYFRAMEDWORD(bData+wXLen+bSizeOf, &(pEA->dwCTOffset));
                    wXLen += 4;
                }
                if (pEA->pVSI)
                {
                    T5_COPYFRAMEDWORD(bData+wXLen+bSizeOf, &(pEA->pVSI->dwStatHi));
                    T5_COPYFRAMEDWORD(bData+wXLen+4+bSizeOf, &(pEA->pVSI->dwStatLo));
                }
            }
            if (bChange)
            {
                _T5EA_SaveChange (pEA);

                pEA->wFlags &= ~T5EA_HASFAIL;
                for (iCnx=0; iCnx<T5MAX_EACNX; iCnx++)
                {
                    if (!T5DEFEACNX_ISNONE (pEA->bCnxState[iCnx]))
                    {
                        bFail = FALSE;
                        wRet = T5RET_OK;
                        if (pExtra)
                        {
                            wRet = T5Evs_EventTo (
                                pEvss, T5EVS_EVENT|T5EVS_STRING, bEv,
                                *pExtra, pExtra+1, iCnx, &bFail, dwd, dwt);
                        }
                        if (!bFail && wRet == T5RET_OK)
                        {
                            if (pEA->pVSI
                                && (bChangeVSI || T5DEFEACNX_ISFAILED(pEA->bCnxState[iCnx]))
                                && bVSI[iCnx])
                            {
                                wRet = T5Evs_EventTo (
                                    pEvss, T5EVS_EVENT, bEv,
                                    (T5_BYTE)(bSizeOf + wXLen + 8),
                                    bData, iCnx, &bFail, dwd, dwt);
                            }
                            else
                            {
                                wRet = T5Evs_EventTo (
                                    pEvss, T5EVS_EVENT, bEv,
                                    (T5_BYTE)(bSizeOf + wXLen),
                                    bData, iCnx, &bFail, dwd, dwt);
                            }
                        }
                        if (bFail)
                        {
                            T5DEFEACNX_SETNONE (pEA->bCnxState[iCnx]);
                            if (pEA->wNbCnx)
                                pEA->wNbCnx -= 1;
                        }
                        else if (wRet == T5RET_OK)
                            T5DEFEACNX_SETOK (pEA->bCnxState[iCnx]);
                        else
                        {
                            T5DEFEACNX_SETFAILED (pEA->bCnxState[iCnx]);
                            pEA->wFlags |= T5EA_HASFAIL;
                        }
                    }
                }
            }
            else if ((pEA->wFlags & T5EA_HASFAIL) != 0)
            {
                pEA->wFlags &= ~T5EA_HASFAIL;
                for (iCnx=0; iCnx<T5MAX_EACNX; iCnx++)
                {
                    if (T5DEFEACNX_ISFAILED (pEA->bCnxState[iCnx]))
                    {
                        bFail = FALSE;
                        wRet = T5RET_OK;
                        if (pExtra)
                        {
                            wRet = T5Evs_EventTo (
                                pEvss, T5EVS_EVENT|T5EVS_STRING, bEv,
                                *pExtra, pExtra+1, iCnx, &bFail, dwd, dwt);
                        }
                        if (!bFail && wRet == T5RET_OK)
                        {
                            if (pEA->pVSI && bVSI[iCnx])
                            {
                                wRet = T5Evs_EventTo (
                                    pEvss, T5EVS_EVENT, bEv,
                                    (T5_BYTE)(bSizeOf + wXLen + 8),
                                    bData, iCnx, &bFail, dwd, dwt);
                            }
                            else
                            {
                                wRet = T5Evs_EventTo (
                                    pEvss, T5EVS_EVENT, bEv,
                                    (T5_BYTE)(bSizeOf + wXLen),
                                    bData, iCnx, &bFail, dwd, dwt);
                            }
                        }
                        if (bFail)
                        {
                            T5DEFEACNX_SETNONE (pEA->bCnxState[iCnx]);
                            if (pEA->wNbCnx)
                                pEA->wNbCnx -= 1;
                        }
                        else if (wRet == T5RET_OK)
                            T5DEFEACNX_SETOK (pEA->bCnxState[iCnx]);
                        else
                        {
                            T5DEFEACNX_SETFAILED (pEA->bCnxState[iCnx]);
                            pEA->wFlags |= T5EA_HASFAIL;
                        }
                    }
                }
            }
            /* auto remove lost private events (no more client) */
            if (pEA->wNbCnx == 0 && !bPublic)
                _T5EA_RemoveEvent (pDB, i);
        }
    }
    return dwNbEvt;
}

/*****************************************************************************
_T5EA_IsChanged
Test for value change, with hysteresis
Parameters:
    pEA (IN) pointer to a valid event
Return: TRUE if significant change
*****************************************************************************/

static T5_BOOL _T5EA_IsChanged (T5PTR_EACHANGE pEA)
{
#ifdef T5DEF_EAHYSTER
    T5_BOOL bRet;
    T5_PTBYTE pLast, pValue, pHP, pHN;
#endif /*T5DEF_EAHYSTER*/

#ifdef T5DEF_STRINGSUPPORTED
    if (pEA->bTicType == T5C_STRING)
    {
        if (pEA->bLast[2] != pEA->pValue[0])
            return TRUE;
        if (*(T5_PTWORD)(pEA->bLast) != T5Tools_Crc16 (pEA->pValue+1, pEA->pValue[0]))
            return TRUE;
        return FALSE;
    }
#endif /*T5DEF_STRINGSUPPORTED*/

    if (T5_MEMCMP (pEA->bLast, pEA->pValue, pEA->bSizeOf) == 0)
        return FALSE;

#ifdef T5DEF_EAHYSTER
    if ((pEA->wFlags & T5EA_HYST) == 0)
        return TRUE;

    pLast = pEA->bLast;
    pValue = pEA->pValue;
    pHP = pEA->bHP;
    pHN = pEA->bHN;
    bRet = FALSE;
    switch (pEA->bTicType)
    {
    case T5C_BOOL :
        bRet = (pEA->bLast[0] != pEA->pValue[0]);
        break;
    case T5C_SINT :
        if (*(T5_PTCHAR)pValue >= (*(T5_PTCHAR)pLast + *(T5_PTCHAR)pHP)
            || *(T5_PTCHAR)pValue <= (*(T5_PTCHAR)pLast - *(T5_PTCHAR)pHN))
            bRet = TRUE;
        break;
    case T5C_USINT :
        if (*pValue >= (*pLast + *pHP)
            || (*pHN <= *pLast && *pValue <= (*pLast - *pHN)))
            bRet = TRUE;
        break;
    case T5C_INT :
#ifdef T5DEF_DATA16SUPPORTED
        if (*(T5_PTSHORT)pValue >= (*(T5_PTSHORT)pLast + *(T5_PTSHORT)pHP)
            || *(T5_PTSHORT)pValue <= (*(T5_PTSHORT)pLast - *(T5_PTSHORT)pHN))
            bRet = TRUE;
#endif /*T5DEF_DATA16SUPPORTED*/
        break;
    case T5C_UINT :
#ifdef T5DEF_DATA16SUPPORTED
        if (*(T5_PTWORD)pValue >= (*(T5_PTWORD)pLast + *(T5_PTWORD)pHP)
            || (*(T5_PTWORD)pHN <= *(T5_PTWORD)pLast
                && *(T5_PTWORD)pValue <= (*(T5_PTWORD)pLast - *(T5_PTWORD)pHN)))
            bRet = TRUE;
#endif /*T5DEF_DATA16SUPPORTED*/
        break;
    case T5C_DINT :
        if (*(T5_PTLONG)pValue >= (*(T5_PTLONG)pLast + *(T5_PTLONG)pHP)
            || *(T5_PTLONG)pValue <= (*(T5_PTLONG)pLast - *(T5_PTLONG)pHN))
            bRet = TRUE;
        break;
    case T5C_REAL :
#ifdef T5DEF_REALSUPPORTED
        if (*(T5_PTREAL)pValue >= (*(T5_PTREAL)pLast + *(T5_PTREAL)pHP)
            || *(T5_PTREAL)pValue <= (*(T5_PTREAL)pLast - *(T5_PTREAL)pHN))
            bRet = TRUE;
#endif /*T5DEF_REALSUPPORTED*/
        break;
    case T5C_UDINT :
    case T5C_TIME :
        if (*(T5_PTDWORD)pValue >= (*(T5_PTDWORD)pLast + *(T5_PTDWORD)pHP)
            || (*(T5_PTDWORD)pHN <= *(T5_PTDWORD)pLast
                && *(T5_PTDWORD)pValue <= (*(T5_PTDWORD)pLast - *(T5_PTDWORD)pHN)))
            bRet = TRUE;
        break;
    case T5C_LINT :
#ifdef T5DEF_LINTSUPPORTED
        if (*(T5_PTLINT)pValue >= (*(T5_PTLINT)pLast + *(T5_PTLINT)pHP)
            || *(T5_PTLINT)pValue <= (*(T5_PTLINT)pLast - *(T5_PTLINT)pHN))
            bRet = TRUE;
#endif /*T5DEF_LINTSUPPORTED*/
        break;
    case T5C_ULINT :
#ifdef T5DEF_ULINTSUPPORTED
        if (*(T5_PTLWORD)pValue >= (*(T5_PTLWORD)pLast + *(T5_PTLWORD)pHP)
            || *(T5_PTLWORD)pValue <= (*(T5_PTLWORD)pLast - *(T5_PTLWORD)pHN))
            bRet = TRUE;
#endif /*T5DEF_ULINTSUPPORTED*/
        break;
    case T5C_LREAL :
#ifdef T5DEF_LREALSUPPORTED
        if (*(T5_PTLREAL)pValue >= (*(T5_PTLREAL)pLast + *(T5_PTLREAL)pHP)
            || *(T5_PTLREAL)pValue <= (*(T5_PTLREAL)pLast - *(T5_PTLREAL)pHN))
            bRet = TRUE;
#endif /*T5DEF_LREALSUPPORTED*/
        break;
    case T5C_STRING :
        bRet = TRUE;
        break;
    default : break;
    }
    return bRet;
#else /*T5DEF_EAHYSTER*/
    return TRUE;
#endif /*T5DEF_EAHYSTER*/
}

/*****************************************************************************
_T5EA_SaveChange
Remember the new value of a variable
Parameters:
    pEA (IN) pointer to a valid event
*****************************************************************************/

static void _T5EA_SaveChange (T5PTR_EACHANGE pEA)
{
    if (pEA->bTicType != T5C_STRING)
    {
        T5_MEMCPY (pEA->bLast, pEA->pValue, pEA->bSizeOf);
    }
    else
    {
        pEA->bLast[2] = pEA->pValue[0];
        *(T5_PTWORD)(pEA->bLast) = T5Tools_Crc16 (pEA->pValue+1, pEA->pValue[0]);
    }
}

/*****************************************************************************
T5EAX_GetNbCnx
get the number of external connection from the application code
Parameters:
    pCode (IN) pointer to the application code
Return: number of external connections
*****************************************************************************/

T5_WORD T5EAX_GetNbCnx (T5_PTR pCode)
{
    T5_PTWORD pCnx;
    T5_WORD wNbCnx;

    if (!T5VMCode_GetEAX (pCode, (T5_PTR *)(&pCnx), T5_PTNULL))
        return 0;

    wNbCnx = 0;
    while (*pCnx != 0)
    {
        wNbCnx += 1;
        pCnx = (T5_PTWORD)((T5_PTBYTE)pCnx + *pCnx);
    }
    return wNbCnx;
}

/*****************************************************************************
T5EAX_GetNbVar
get the number of external variables from the application code
Parameters:
    pCode (IN) pointer to the application code
Return: number of external variables
*****************************************************************************/

T5_WORD T5EAX_GetNbVar (T5_PTR pCode)
{
    T5_PTBYTE pExt;
    T5_WORD wNbVar;

    if (!T5VMCode_GetEAX (pCode, NULL, (T5_PTR *)(&pExt)))
        return 0;

    wNbVar = 0;
    while (*pExt != 0)
    {
        wNbVar += 1;
        if ((pExt[1] /*bOpe*/ & T5EAOPE_CTSEG) != 0)
            pExt += sizeof (T5_DWORD);
        pExt += T5STR_EADEFVAR_SIZEOF;
    }
    return wNbVar;
}

/*****************************************************************************
T5EAX_CanBuildHOT
check if hot restart is possible
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer to the VM database
Return: OK or error
*****************************************************************************/

T5_RET T5EAX_CanBuildHOT (T5_PTR pCode, T5PTR_DB pDB)
{
    if (T5EAX_GetNbCnx (pCode) > pDB[T5TB_EACNX].wNbItemAlloc)
        return T5RET_ERROR;
    if (T5EAX_GetNbVar (pCode) > pDB[T5TB_EAEXT].wNbItemAlloc)
        return T5RET_ERROR;
    return T5RET_OK;
}

/*****************************************************************************
T5EAX_BuildHOT
rebuilds value change events for hot restart or On Line change
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer to the VM database
Return: OK or error
*****************************************************************************/

T5_RET T5EAX_BuildHOT (T5_PTR pCode, T5PTR_DB pDB)
{
    if (T5EAX_GetNbCnx (pCode) > pDB[T5TB_EACNX].wNbItemAlloc)
        return T5RET_ERROR;
    if (T5EAX_GetNbVar (pCode) > pDB[T5TB_EAEXT].wNbItemAlloc)
        return T5RET_ERROR;

    return T5EAX_Build (pCode, pDB);
}

/*****************************************************************************
T5EAX_Build
builds value change events for a cold start
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer to the VM database
Return: OK or error
*****************************************************************************/

T5_RET T5EAX_Build (T5_PTR pCode, T5PTR_DB pDB)
{
    T5PTR_EADEFCNX pCnx;
    T5_PTBYTE pExtDef;
    T5PTR_EADEFVAR pExt;
    T5PTR_EACNX pDbCnx;
    T5PTR_EAEXT pDbExt;
    T5_DWORD dwSize;

    if (!T5VMCode_GetEAX (pCode, (T5_PTR *)(&pCnx), (T5_PTR *)(&pExtDef)))
        return T5RET_OK;
    
    /* build all connection records */
    pDB[T5TB_EACNX].wNbItemFixed = pDB[T5TB_EACNX].wNbItemUsed
                                 = pDB[T5TB_EACNX].wNbItemAlloc;
    pDbCnx = T5GET_EACNX(pDB);
    while (*(T5_PTWORD)pCnx != 0)
    {
        T5_MEMSET (pDbCnx, 0, sizeof (T5STR_EACNX));
        pDbCnx->pDef = pCnx;

        pDbCnx->pVar = T5GET_EAEXT(pDB);
        pDbCnx->pVar += pCnx->wVar1;
        pDbCnx->wNbVar = pCnx->wNbVar;

        if ((pCnx->wFlags & T5EA_REDMASTER) != 0)
        {
            pDbCnx->pVarAux = pDbCnx->pVar;
            pDbCnx->pVarAux += pCnx->wNbVar;
        }
        else if ((pCnx->wFlags & T5EA_REDSLAVE) != 0)
        {
            pDbCnx->pVarAux = pDbCnx->pVar;
            pDbCnx->pVarAux -= pCnx->wNbVar;
        }
        else
            pDbCnx->pVarAux = NULL;

        pDbCnx->sock = T5_INVSOCKET;
        pDbCnx->wStatus = T5EA_ERROR;

        T5_MEMSET (pDbCnx->bfOut, 0, sizeof (pDbCnx->bfOut));
        pDbCnx->bfOut[0] = 'T';
        pDbCnx->bfOut[1] = '5';
        pDbCnx->bfOut[2] = 0;
        pDbCnx->bfOut[3] = 10;
        pDbCnx->bfOut[4] = T5CSRQ_EVS;
        pDbCnx->bfOut[5] = T5EVS_SYSTEM | T5EVS_EVENT | T5EVS_VSI;
        /* 6 to 9 = password (0) */
        dwSize = sizeof (pDbCnx->bfIn);
        T5_COPYFRAMEDWORD (pDbCnx->bfOut + 10, &dwSize);

        pCnx = (T5PTR_EADEFCNX)((T5_PTBYTE)pCnx + *(T5_PTWORD)pCnx);
        pDbCnx++;
    }

    /* build all variable records - dont link yet */
    pDB[T5TB_EAEXT].wNbItemFixed = pDB[T5TB_EAEXT].wNbItemUsed
                                 = pDB[T5TB_EAEXT].wNbItemAlloc;
    pDbExt = T5GET_EAEXT(pDB);
    while (*pExtDef != 0) /* null type as terminator */
    {
        pExt = (T5PTR_EADEFVAR)pExtDef;

        T5_MEMSET (pDbExt, 0, sizeof (T5STR_EAEXT));
        pDbExt->wEvent = pExt->wEvent;
        pDbExt->bOpe = pExt->bOpe & ~T5EAOPE_CTSEG;
        pDbExt->wOffset = pExt->wOffset;
        pDbExt->bDstType = pExt->bTicType;
        pDbExt->dwCTOffset = 0;

        pExtDef += T5STR_EADEFVAR_SIZEOF;
        if ((pExt->bOpe & T5EAOPE_CTSEG) != 0)
        {
            pDbExt->wOffset = 0;
            T5_MEMCPY (&(pDbExt->dwCTOffset), pExtDef, sizeof (T5_DWORD));
            T5_MEMCPY (&(pDbExt->pData), pExtDef, sizeof (T5_DWORD));
            pExtDef += sizeof (T5_DWORD);
        }
        else
            pDbExt->pData = T5_PTNULL;

        pDbExt++;
    }

    return T5RET_OK;
}

/*****************************************************************************
T5EAX_Link
establish the link between EA server and VM variables
Parameters:
    pDB (IN) pointer to the VM database
Return: OK or error
*****************************************************************************/

T5_RET T5EAX_Link (T5PTR_DB pDB)
{
    T5PTR_EAEXT pDbExt;
    T5_WORD i;
    T5_PTBYTE pLockBase;
    T5_PTBYTE pD8;
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
#ifdef T5DEF_CTSEG
    T5_DWORD dwCTOffset;
#endif /*T5DEF_CTSEG*/

    pLockBase = T5GET_DBLOCK(pDB);
    pDbExt = T5GET_EAEXT(pDB);
    for (i=0; i < pDB[T5TB_EAEXT].wNbItemUsed; i++, pDbExt++)
    {
        pDbExt->pLock = pLockBase + pDbExt->wOffset;

        if (pDbExt->pData != T5_PTNULL)
        {
            pDbExt->bLockMask = 0; /* lock not supported */
#ifdef T5DEF_CTSEG
            T5_MEMCPY (&dwCTOffset, &(pDbExt->pData), sizeof (T5_DWORD));
            pDbExt->pData = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
            pDbExt->pData += dwCTOffset;
#else /*T5DEF_CTSEG*/
            pDbExt->pData = T5_PTNULL;
#endif /*T5DEF_CTSEG*/
        }
        else if (pDbExt->bDstType & T5C_EXTERN)
        {
            pDbExt->bLockMask = T5LOCK_XV;
#ifdef T5DEF_XV
            pXV = T5GET_DBXV(pDB);
            pDbExt->pData = (T5_PTBYTE)(pXV[pDbExt->wOffset]);
#endif /*T5DEF_XV*/
        }
        else
        {
            switch (pDbExt->bDstType)
            {
            case T5C_BOOL :
            case T5C_SINT :
            case T5C_USINT :
                pDbExt->bLockMask = T5LOCK_DATA8;
                pD8 = T5GET_DBDATA8(pDB);
                pDbExt->pData = (T5_PTBYTE)(pD8 + pDbExt->wOffset);
                break;
            case T5C_INT :
            case T5C_UINT :
                pDbExt->bLockMask = T5LOCK_DATA16;
#ifdef T5DEF_DATA16SUPPORTED
                pD16 = T5GET_DBDATA16(pDB);
                pDbExt->pData = (T5_PTBYTE)(pD16 + pDbExt->wOffset);
#endif /*T5DEF_DATA16SUPPORTED*/
                break;
            case T5C_DINT :
            case T5C_REAL :
            case T5C_UDINT :
                pDbExt->bLockMask = T5LOCK_DATA32;
                pD32 = T5GET_DBDATA32(pDB);
                pDbExt->pData = (T5_PTBYTE)(pD32 + pDbExt->wOffset);
                break;
            case T5C_TIME :
                pDbExt->bLockMask = T5LOCK_TIME;
                pD32 = T5GET_DBTIME(pDB);
                pDbExt->pData = (T5_PTBYTE)(pD32 + pDbExt->wOffset);
                break;
            case T5C_LINT :
            case T5C_ULINT :
            case T5C_LREAL :
                pDbExt->bLockMask = T5LOCK_DATA64;
#ifdef T5DEF_DATA64SUPPORTED
                pD64 = T5GET_DBDATA64(pDB);
                pDbExt->pData = (T5_PTBYTE)(pD64 + pDbExt->wOffset);
#endif /*T5DEF_DATA64SUPPORTED*/
                break;
            case T5C_STRING :
                pDbExt->bLockMask = T5LOCK_STRING;
#ifdef T5DEF_STRINGSUPPORTED
                pString = T5GET_DBSTRING(pDB);
                pDbExt->pData = (T5_PTBYTE)(pString[pDbExt->wOffset]);
#endif /*T5DEF_STRINGSUPPORTED*/
                break;
            default : break;
            }
        }
#ifdef T5DEF_VARMAP
    if (pDbExt->pData != NULL)
        pDbExt->pVSI = T5Map_GetVSIByData (pDB, pDbExt->pData);
    else
        pDbExt->pVSI = NULL;
#endif /*T5DEF_VARMAP*/
    }
    return T5RET_OK;
}

/*****************************************************************************
T5EAX_Open
Open all connections to external variables
Parameters:
    pDB (IN) pointer to the VM database
Return: OK or error
*****************************************************************************/

T5_RET T5EAX_Open (T5PTR_DB pDB)
{
    T5PTR_EACNX pCnx;
    T5_WORD i;

    pCnx = T5GET_EACNX(pDB);
    for (i=0; i < pDB[T5TB_EACNX].wNbItemUsed; i++, pCnx++)
        _T5EAX_OpenPort (pCnx, FALSE);
    return T5RET_OK;
}

/*****************************************************************************
_T5EAX_OnError
Handle a communication error
Parameters:
    pCnx (IN) handle of the external connection
*****************************************************************************/

static void _T5EAX_OnError (T5PTR_EACNX pCnx)
{
    T5Socket_CloseSocket (pCnx->sock);
    pCnx->sock = T5_INVSOCKET;
    pCnx->dwLastIn = 0L; /* arm next timeout */
    pCnx->wStatus = T5EA_ERROR;
    pCnx->wFlags &= ~T5EA_APPHERE;
    _T5EAX_OnRemoteAppStop (pCnx);
    _T5EAX_DispatchStatus (pCnx);
}

/*****************************************************************************
_T5EAX_OpenPort
Open an external connection
Parameters:
    pCnx (IN) handle of the external connection
    bReconnect (IN) TRUE if called for a re-connection after failure
*****************************************************************************/

static void _T5EAX_OpenPort (T5PTR_EACNX pCnx, T5_BOOL bReconnect)
{
    T5_BOOL bWait;
    T5PTR_EADEFCNX pDef;
    T5_UNUSED(bReconnect);

    pDef = (T5PTR_EADEFCNX)(pCnx->pDef);
    /* for future
    if (bReconnect && (pDef->wFlags & T5EA_RECONNECT) == 0)
        return;
    */

    pCnx->wPacketIn = pCnx->wSizeIn = 0;
    pCnx->dwLastIn = 0L;
    if (T5Socket_CreateConnectedSocket (pDef->szAddr, pDef->wPort,
                                        &(pCnx->sock),
                                        &bWait, NULL) != T5RET_OK)
        pCnx->wStatus = T5EA_ERROR;
    else if (bWait)
        pCnx->wStatus = T5EA_CONNECTING;
    else
    {
        pCnx->wStatus = T5EA_RECEIVING;
        pCnx->wFlags = 0;
    }
    _T5EAX_OnRemoteAppStop (pCnx);
    _T5EAX_DispatchStatus (pCnx);
}

/*****************************************************************************
T5EAX_Close
Close all connections to external variables
Parameters:
    pDB (IN) pointer to the VM database
*****************************************************************************/

void T5EAX_Close (T5PTR_DB pDB)
{
    T5PTR_EACNX pCnx;
    T5_WORD i;

    pCnx = T5GET_EACNX(pDB);
    for (i=0; i < pDB[T5TB_EACNX].wNbItemUsed; i++, pCnx++)
    {
        if (pCnx->sock != T5_INVSOCKET)
        {
            T5Socket_CloseSocket (pCnx->sock);
            pCnx->sock = T5_INVSOCKET;
            pCnx->wStatus = T5EA_ERROR;
        }
    }
}

/*****************************************************************************
T5EAX_Activate
Manage all connections to external variables
Parameters:
    pDB (IN) pointer to the VM database
    dwTimeStamp (IN) current time stamp in milliseconds (used for timeouts)
*****************************************************************************/

void T5EAX_Activate (T5PTR_DB pDB, T5_DWORD dwTimeStamp)
{
    T5PTR_EACNX pCnx;
    T5_WORD i;
    T5_BOOL bTimeOut, bFail;
    T5_DWORD dwNb;

    dwNb = 0;
    pCnx = T5GET_EACNX(pDB);
    for (i=0; i < pDB[T5TB_EACNX].wNbItemUsed; i++, pCnx++)
    {
        /* check for time out */
        if (pCnx->dwLastIn == 0L)
        {
            pCnx->dwLastIn = dwTimeStamp;
            bTimeOut = FALSE;
        }
        else if (pCnx->wStatus == T5EA_CONNECTING)
            bTimeOut= FALSE;
        else
        {
            bTimeOut = (pCnx->dwLastIn != 0L
                       && T5_BOUNDTIME(dwTimeStamp - pCnx->dwLastIn)
                          > T5EVS_TIMEOUT);
        }
        /* state machine */
        switch (pCnx->wStatus)
        {
        case T5EA_CONNECTING :
            if (T5Socket_CheckPendingConnect (pCnx->sock, &bFail) == T5RET_OK)
            {
                pCnx->wPacketIn = pCnx->wSizeIn = 0;
                pCnx->wStatus = T5EA_RECEIVING;
                _T5EAX_DispatchStatus (pCnx);
            }
            else if (bFail || bTimeOut)
            {
                _T5EAX_OnError (pCnx);
            }
            break;
        case T5EA_ERROR :
            if (bTimeOut)
                _T5EAX_OpenPort (pCnx, TRUE);
            break;
        case T5EA_TIMEOUT :
            _T5EAX_OnError (pCnx);
            _T5EAX_OpenPort (pCnx, TRUE);
            break;
        case T5EA_RECEIVING :
            if (_T5EAX_Receive (pCnx))
            {
                dwNb += _T5EAX_OnEvent (pDB, pCnx);
                pCnx->wSizeOut = 0;
                pCnx->dwLastIn = 0L;
                if (pCnx->wStatus == T5EA_TIMEOUT)
                {
                    pCnx->wStatus = T5EA_SENDING;
                    _T5EAX_DispatchStatus (pCnx);
                }
                else
                    pCnx->wStatus = T5EA_SENDING;
                /* continue sending */
            }
            else
            {
                if (bTimeOut && pCnx->wStatus != T5EA_ERROR)
                {
                    pCnx->wStatus = T5EA_TIMEOUT;
                    _T5EAX_DispatchStatus (pCnx);
                }
                break;
            }
        case T5EA_SENDING :
            if (_T5EAX_Send (pCnx))
            {
                pCnx->wPacketIn = pCnx->wSizeIn = 0;
                pCnx->dwLastIn = 0L;
                pCnx->wStatus = T5EA_RECEIVING;
            }
            break;
        default : break;
        }
    }
    T5_DIAGHOOK (T5_DIAGHOOK_BIND_RCV, dwNb);
}

/*****************************************************************************
_T5EAX_Receive
Handle reception of frames
Parameters:
    pCnx (IN) handle of the external connection
Return: TRUE if frame received
*****************************************************************************/

static T5_BOOL _T5EAX_Receive (T5PTR_EACNX pCnx)
{
    T5_PTBYTE pData;
    T5_WORD i, wRec, wExpect, wLen;
    T5_BOOL bFail;

    /* receive frame header */
    if (pCnx->wPacketIn == 0)
    {
        wExpect = 4 - pCnx->wSizeIn;
        pData = pCnx->bfIn + pCnx->wSizeIn;
        wRec = T5Socket_Receive (pCnx->sock, wExpect, pData, &bFail);
        if (bFail)
        {
            _T5EAX_OnError (pCnx);
            return FALSE;
        }
        pCnx->wSizeIn += wRec;
        if (pCnx->wSizeIn != 4)
            return FALSE;
        else
        {
            if (!T5CS_ISHEAD(pCnx->bfIn))
            {
                for (i=0; i<1024 && T5Socket_Receive (pCnx->sock, 1, pData, &bFail); i++)
                    ; /*flush*/
                _T5EAX_OnError (pCnx);
                return FALSE;
            }
            else
            {
                T5_COPYFRAMEWORD (&wLen, pCnx->bfIn + 2);
                pCnx->wPacketIn = wLen + 4;
            }
        }
    }
    /* receive frame data */
    wExpect = pCnx->wPacketIn - pCnx->wSizeIn;
    pData = pCnx->bfIn + pCnx->wSizeIn;
    wRec = T5Socket_Receive (pCnx->sock, wExpect, pData, &bFail);
    if (bFail)
    {
        _T5EAX_OnError (pCnx);
    }
    else if (wRec)
    {
        pCnx->wSizeIn += wRec;
        if (pCnx->wSizeIn == pCnx->wPacketIn)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*****************************************************************************
_T5EAX_Send
Handle emission of frames
Parameters:
    pCnx (IN) handle of the external connection
Return: TRUE if frame completely sent
*****************************************************************************/

static T5_BOOL _T5EAX_Send (T5PTR_EACNX pCnx)
{
    T5_PTBYTE pData;
    T5_WORD i, wRec, wExpect;
    T5_BOOL bFail;
    T5_BYTE bDummy;

    bFail = FALSE;
    for (i=0; i<1024 && !bFail && T5Socket_Receive (pCnx->sock, 1, &bDummy, &bFail); i++)
        ; /*flush*/
    if (bFail)
    {
        _T5EAX_OnError (pCnx);
        return FALSE;
    }

    wExpect = 14 - pCnx->wSizeOut;
    pData = pCnx->bfOut + pCnx->wSizeOut;
    wRec = T5Socket_Send (pCnx->sock, wExpect, pData, &bFail);
    if (bFail)
    {
        _T5EAX_OnError (pCnx);
    }
    else if (wRec)
    {
        pCnx->wSizeOut += wRec;
        if (pCnx->wSizeOut == 14)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*****************************************************************************
_T5EAX_GetCnxStatus
Calculate the value of a connection status
Parameters:
    pCnx (IN) handle of the external connection
Return: status value
*****************************************************************************/

static T5_BYTE _T5EAX_GetCnxStatus (T5PTR_EACNX pCnx)
{
    T5_BYTE bStatus;

    switch (pCnx->wStatus)
    {
        case T5EA_CONNECTING : bStatus = 1; break;
        case T5EA_ERROR      : bStatus = 2; break;
        case T5EA_TIMEOUT    : bStatus = 3; break;
        default              : bStatus = 0; break;
    }
    if (bStatus == 0 && (pCnx->wFlags & T5EA_APPHERE) == 0)
        bStatus = 4;
    return bStatus;
}

/*****************************************************************************
_T5EAX_DispatchStatus
Dispatch connection status to all linked variables
Parameters:
    pCnx (IN) handle of the external connection
*****************************************************************************/

#ifndef T5BIND_ONCONSUMESTATE
#define T5BIND_ONCONSUMESTATE(cnx) /*nothing*/
#endif /*T5BIND_ONCONSUMESTATE*/

static void _T5EAX_DispatchStatus (T5PTR_EACNX pCnx)
{
    T5PTR_EAEXT pVar;
    T5_WORD i;
    T5_BYTE bStatus;
    T5PTR_EADEFCNX pDef;
    T5PTR_EACNX pAuxCnx;
    T5_DWORD dwMask;

    pVar = pCnx->pVar;
    if (pVar == NULL)
        return;

    pDef = (T5PTR_EADEFCNX)(pCnx->pDef);
    bStatus = _T5EAX_GetCnxStatus (pCnx);
    T5BIND_ONCONSUMESTATE (pCnx);

    if ((pDef->wFlags & T5EA_REDMASTER) != 0)
    {
        pAuxCnx = pCnx + 1;
        bStatus |= (_T5EAX_GetCnxStatus (pAuxCnx) << 4);
        T5BIND_ONCONSUMESTATE (pAuxCnx);
    }
    else if ((pDef->wFlags & T5EA_REDSLAVE) != 0)
    {
        bStatus <<= 4;
        pAuxCnx = pCnx - 1;
        bStatus |= (T5_BYTE)(_T5EAX_GetCnxStatus (pAuxCnx));
        T5BIND_ONCONSUMESTATE (pAuxCnx);
    }

    for (i=0; i<pCnx->wNbVar && pVar->wEvent == 0; i++, pVar++)
    {
        if (pVar->pData != NULL && (*(pVar->pLock) & pVar->bLockMask) == 0)
        {
            if (pVar->bOpe == T5EAOPE_CS)
            {
                switch (pVar->bDstType & ~T5C_EXTERN)
                {
                case T5C_BOOL :
                    *(pVar->pData) = (bStatus != 0);
                    break;
                case T5C_SINT :
                case T5C_USINT :
                    *(pVar->pData) = bStatus;
                    break;
                case T5C_INT :
                case T5C_UINT :
                    *(T5_PTWORD)(pVar->pData) = (T5_WORD)(bStatus) & 0xff;
                    break;
                case T5C_DINT :
                case T5C_UDINT :
                    *(T5_PTDWORD)(pVar->pData) = (T5_DWORD)(bStatus) & 0xffL;
                    break;
                default : break;
                }
            }
            else if ((pVar->bOpe & ~T5EAOPE_MCIDX) == T5EAOPE_MCS)
            {
                dwMask = 1 << (T5EAOPE_MCSINDEX(pVar->bOpe) - 1);
                switch (pVar->bDstType & ~T5C_EXTERN)
                {
                case T5C_BOOL :
                    *(pVar->pData) = (bStatus != 0);
                    break;
                case T5C_SINT :
                case T5C_USINT :
                    if (bStatus == 0)
                        *(pVar->pData) &= ~(T5_BYTE)dwMask;
                    else
                        *(pVar->pData) |= (T5_BYTE)dwMask;
                    break;
                case T5C_INT :
                case T5C_UINT :
                    if (bStatus == 0)
                        *(T5_PTWORD)(pVar->pData) &= ~(T5_WORD)dwMask;
                    else
                        *(T5_PTWORD)(pVar->pData) |= (T5_WORD)dwMask;
                    break;
                    break;
                case T5C_DINT :
                case T5C_UDINT :
                    if (bStatus == 0)
                        *(T5_PTDWORD)(pVar->pData) &= ~dwMask;
                    else
                        *(T5_PTDWORD)(pVar->pData) |= dwMask;
                    break;
                default : break;
                }
            }
        }
    }
}

/*****************************************************************************
_T5EAX_OnEvent
Manage a frame containing events
Parameters:
    pCnx (IN) handle of the external connection
Return: number of events received
*****************************************************************************/

static T5_DWORD _T5EAX_OnEvent (T5PTR_DB pDB, T5PTR_EACNX pCnx)
{
    T5_PTBYTE pFrame, pValue;
    T5_BYTE bNb, bSrcType;
    T5_WORD wLen, wEvent;
    T5_DWORD dwDate, dwTime;
    T5_BOOL bStart, bStop;
    T5_WORD wMsgSize;
    T5_DWORD dwNb;

    dwNb = 0;
    pFrame = pCnx->bfIn;
    T5_COPYFRAMEWORD (&wMsgSize, pFrame + 2);
    wMsgSize -= 2;
    pFrame += 4; /* skip header */
    if (*pFrame != T5CSRQ_EVS)
        return 0;
    bStart = bStop = FALSE;
    pFrame++;
    bNb = *pFrame++;

#ifndef T5DEF_EXTEAPACKETS
    while (bNb--)
#else /*T5DEF_EXTEAPACKETS*/
    while (wMsgSize > 0)
#endif /*T5DEF_EXTEAPACKETS*/
    {
        wLen = (T5_WORD)(pFrame[2]) & 0xff;
        if (pFrame[0] == T5EVS_EVENT && pFrame[1] == T5EV_EVBROADCAST)
        {
            T5_COPYFRAMEDWORD (&dwDate, pFrame + 4);
            T5_COPYFRAMEDWORD (&dwTime, pFrame + 8);
            T5_COPYFRAMEWORD (&wEvent, pFrame + 12);
            bSrcType = pFrame[14];
            pValue = pFrame + 17;
            _T5EAX_OnChangeEvent (pDB, pCnx, wEvent, dwDate, dwTime,
                                  bSrcType, pValue, pFrame+12, (T5_BYTE)wLen);
            bStart = TRUE;
            dwNb += 1;
        }
        else if (pFrame[0] == T5EVS_SYSTEM && pFrame[1] == T5EV_EACLOSE)
        {
            _T5EAX_OnRemoteAppStop (pCnx);
            bStop = TRUE;
        }
        pFrame += (wLen + 12);
        if (wMsgSize > (wLen + 12))
            wMsgSize -= (wLen + 12);
        else
            wMsgSize = 0;
    }
    if (bStop)
    {
        pCnx->wFlags &= ~T5EA_APPHERE;
        _T5EAX_DispatchStatus (pCnx);
    }
    else if (bStart && (pCnx->wFlags & T5EA_APPHERE) == 0)
    {
        pCnx->wFlags |= T5EA_APPHERE;
        _T5EAX_DispatchStatus (pCnx);
    }
    return dwNb;
}

/*****************************************************************************
_T5EAX_MakeVarStatus
Calculate a redundant connection status
Parameters:
    pCnx (IN) handle of the external connection
Return: redundant status
*****************************************************************************/

static T5_DWORD _T5EAX_MakeVarStatus (T5PTR_EACNX pCnx)
{
    T5_DWORD dwStatus;
    T5PTR_EADEFCNX pDef;
    T5PTR_EACNX pAuxCnx;
    
    dwStatus = (_T5EAX_GetCnxStatus (pCnx) != 0) ? 1L : 0L;

    pDef = (T5PTR_EADEFCNX)(pCnx->pDef);
    if ((pDef->wFlags & T5EA_REDMASTER) != 0)
    {
        pAuxCnx = pCnx + 1;
        if (_T5EAX_GetCnxStatus (pAuxCnx) != 0)
            dwStatus |= 2L;
    }
    else if ((pDef->wFlags & T5EA_REDSLAVE) != 0)
    {
        dwStatus <<= 1;
        pAuxCnx = pCnx - 1;
        if (_T5EAX_GetCnxStatus (pAuxCnx) != 0)
            dwStatus += 1L;
    }
    return dwStatus;
}

/*****************************************************************************
_T5EAX_OnRemoteAppStop
Dispatch connection status when external application stops
Parameters:
    pCnx (IN) handle of the external connection
*****************************************************************************/

static void _T5EAX_OnRemoteAppStop (T5PTR_EACNX pCnx)
{
    T5PTR_EAEXT pVar;
    T5_WORD i;

    pVar = pCnx->pVar;
    if (pVar == NULL)
        return;

    for (i=0; i<pCnx->wNbVar; i++, pVar++)
    {
        if (!T5EAOPE_ISCS(pVar->bOpe) && pVar->pData != NULL
            && (*(pVar->pLock) & pVar->bLockMask) == 0)
        {
            switch (pVar->bOpe)
            {
            case T5EAOPE_VS :
                _T5EAX_CopyDWStatus (pVar->bDstType, pVar->pData,
                                     _T5EAX_MakeVarStatus (pCnx));
                break;
                /*
            case T5EAOPE_XV :
            case T5EAOPE_VT :
            case T5EAOPE_VD :
                _T5EAX_CopyDWStatus (pVar->bDstType, pVar->pData, 0L);
                break;
                */
            default : break;
            }
        }
    }
}

/*****************************************************************************
_T5EAX_CheckLock
Check if the variable for a received event is locked
Parameters:
    pCnx (IN) handle of the external connection
    pVar (IN) pointer to the variable
    bSrcType (IN) datatype of the source variable
    pValue (IN) pointer to variable value in frame
Return: TUE if variable is not locked
*****************************************************************************/

static T5_BOOL _T5EAX_CheckLock (T5PTR_DB pDB, T5PTR_EAEXT pVar,
                                 T5_BYTE bSrcType, T5_PTBYTE pValue)
{
#ifdef T5DEF_SMARTLOCK_EA
#ifdef T5DEF_SMARTLOCK
    if (bSrcType != T5C_STRING && pVar->bDstType != T5C_STRING)
    {
        T5_BYTE buffer[8];
        _T5EAX_Copy (pVar->bDstType, buffer, bSrcType, pValue);
        return !T5VMLog_SetLockVar (pDB, pVar->bDstType, pVar->wOffset, pVar->dwCTOffset, buffer, TRUE);
    }
    else
#endif /*T5DEF_SMARTLOCK*/
#endif /*T5DEF_SMARTLOCK_EA*/
    return ((*(pVar->pLock) & pVar->bLockMask) == 0);
}

/*****************************************************************************
_T5EAX_OnChangeEvent
Handle one value change event - refresh all linked variables
Parameters:
    pCnx (IN) handle of the external connection
    wEvent (IN) event ID
    dwDate, dwTime (IN) event time stamp
    bSrcType (IN) datatype of the source variable
    pValue (IN) pointer to variable value in frame
    pFrame (IN) event frame
    bFrameLen (IN) event frame length
*****************************************************************************/

static void _T5EAX_OnChangeEvent (T5PTR_DB pDB, T5PTR_EACNX pCnx, T5_WORD wEvent,
                                  T5_DWORD dwDate, T5_DWORD dwTime,
                                  T5_BYTE bSrcType, T5_PTBYTE pValue,
                                  T5_PTBYTE pFrame, T5_BYTE bFrameLen)
{
    T5PTR_EAEXT pVar, pVarAux;
    T5_WORD i;
    T5PTR_EADEFCNX pDef;
    T5_BYTE bDataLen;
    T5_BOOL bVSI;

    pVar = pCnx->pVar;
    pVarAux = pCnx->pVarAux;
    if (pVar == NULL)
        return;

    bDataLen = 5 + _T5EAX_GetDataSize (bSrcType);
    if (pFrame[3] == 0 && pFrame[4] == 0)
        bDataLen += 4;
    bVSI = ((bDataLen + 8) <= bFrameLen);

    pDef = (T5PTR_EADEFCNX)(pCnx->pDef);
    for (i=0; i<pCnx->wNbVar && pVar->wEvent <= wEvent; i++, pVar++)
    {
        if (pVar->wEvent == wEvent
            && !T5EAOPE_ISCS(pVar->bOpe) && pVar->pData != NULL
            && _T5EAX_CanUpdateEvent (pVar, pVarAux, dwDate, dwTime, (T5_BOOL)((pDef->wFlags & T5EA_REDRT) != 0))
            && _T5EAX_CheckLock (pDB, pVar, bSrcType, pValue))
        {
            switch (pVar->bOpe)
            {
            case T5EAOPE_XV :
                _T5EAX_Copy (pVar->bDstType, pVar->pData, bSrcType, pValue);
                if (bVSI && pVar->pVSI != NULL)
                {
                    T5_COPYFRAMEDWORD(&(pVar->pVSI->dwStatHi), pFrame+bDataLen);
                    T5_COPYFRAMEDWORD(&(pVar->pVSI->dwStatLo), pFrame+bDataLen+4);
                    pVar->pVSI->dwDate = dwDate;
                    pVar->pVSI->dwTime = dwTime;
                }
                break;
            case T5EAOPE_VS :
                _T5EAX_CopyDWStatus (pVar->bDstType, pVar->pData,
                                     _T5EAX_MakeVarStatus (pCnx));
                break;
            case T5EAOPE_VT :
                _T5EAX_CopyDWStatus (pVar->bDstType, pVar->pData, dwTime);
                break;
            case T5EAOPE_VD :
                _T5EAX_CopyDWStatus (pVar->bDstType, pVar->pData, dwDate);
                break;
            default : break;
            }
        }
        if (pVarAux != NULL)
            pVarAux++;
    }
}

/*****************************************************************************
_T5EAX_CanUpdateEvent
Check if a redundant event is valid
Parameters:
    pVar (IN) main connection variable
    pVarAux (IN) redundant connection variable
    dwDate, dwTime (IN) event time stamp
    bForRed (IN) false = dual binding / true = redundancy
Return: TRUE if the even must be processed
*****************************************************************************/

static T5_BOOL _T5EAX_CanUpdateEvent (T5PTR_EAEXT pVar, T5PTR_EAEXT pVarAux,
                                      T5_DWORD dwDate, T5_DWORD dwTime, T5_BOOL bForRed)
{
    if (pVarAux == NULL)
        return TRUE;

    if (bForRed) /* networks to redundant runtimes */
        return TRUE;

    if (dwDate < pVarAux->dwDateStamp)
        return FALSE;

    if (dwDate == pVarAux->dwDateStamp && dwTime < pVarAux->dwTimeStamp)
        return FALSE;

    pVar->dwDateStamp = dwDate;
    pVar->dwTimeStamp = dwTime;

    return TRUE;
}

/*****************************************************************************
_T5EAX_CopyDWStatus
Update a linked variable with a d-word value
Parameters:
    bDstType (IN) data type of the destination variable
    pDst (IN) pointer to the destination variable buffer
    dwValue (IN) new value
*****************************************************************************/

static void _T5EAX_CopyDWStatus (T5_BYTE bDstType, T5_PTBYTE pDst,
                                 T5_DWORD dwValue)
{
    T5VMCnv_ConvBin (pDst, &dwValue, bDstType & ~T5C_EXTERN, T5C_UDINT);
}

/*****************************************************************************
_T5EAX_Copy
Update a linked variable with an event value (from frame) - copy or cast
Parameters:
    bDstType (IN) data type of the destination variable
    pDst (IN) pointer to the destination variable buffer
    bSrcType (IN) data type of the source variable
    pSrc (IN) pointer to the source value (in frame)
*****************************************************************************/

static void _T5EAX_Copy (T5_BYTE bDstType, T5_PTBYTE pDst,
                         T5_BYTE bSrcType, T5_PTBYTE pSrc)
{
    T5_NUMBUFFER buffer;

    T5_MEMSET (&buffer, 0, sizeof (buffer));
    switch (bSrcType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
    case T5C_SINT :
    case T5C_USINT :
        buffer.b[0] = *pSrc;
        break;
    case T5C_INT :
    case T5C_UINT :
        T5_COPYFRAMEWORD (&buffer,pSrc);
        break;
    case T5C_DINT :
    case T5C_UDINT :
    case T5C_TIME :
    case T5C_REAL :
        T5_COPYFRAMEDWORD (&buffer,pSrc);
        break;
    case T5C_LINT :
    case T5C_ULINT :
    case T5C_LREAL :
        T5_COPYFRAME64 (&buffer,pSrc);
        break;
    default : break;
    }
    T5VMCnv_ConvBin (pDst, &buffer, bDstType & ~T5C_EXTERN, bSrcType & ~T5C_EXTERN);
}

/*****************************************************************************
_T5EAX_GetDataSize
Get value buffer size for a received event
Parameters:
    bScrType (IN) data type of the received event
Return: data size in bytes
*****************************************************************************/

static T5_BYTE _T5EAX_GetDataSize (T5_BYTE bSrcType)
{
    switch (bSrcType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
    case T5C_SINT :
    case T5C_USINT :
        return 1;
    case T5C_INT :
    case T5C_UINT :
        return 2;
    case T5C_DINT :
    case T5C_UDINT :
    case T5C_TIME :
    case T5C_REAL :
        return 4;
    case T5C_LINT :
    case T5C_ULINT :
    case T5C_LREAL :
        return 8;
    default :
        return 0;
    }
}

/****************************************************************************/

#endif /*T5DEF_EA*/

/* eof **********************************************************************/
