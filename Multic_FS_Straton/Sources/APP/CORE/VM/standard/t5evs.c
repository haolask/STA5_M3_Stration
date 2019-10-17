/*****************************************************************************
T5EVS.c :    Event server

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5EVS_DEFINITIONS
T5EVS_DEFINITIONS
#endif /*T5EVS_DEFINITIONS*/

#ifdef T5DEF_EVENTSERVER

/****************************************************************************/
/* static services - MONO CLIENT */

static void _T5Evs_Send (T5PTR_EVS pEvs);
static void _T5Evs_Receive (T5PTR_EVS pEvs);
static void _T5Evs_FlushReadPointers (T5PTR_EVS pEvs);
static void _T5Evs_FlushSentEvents (T5PTR_EVS pEvs);
static void _T5Evs_Callback (T5PTR_EVS pEvs, T5_BYTE bClass, T5_BYTE bID,
                             T5_BYTE bLen, T5_PTBYTE pData);

/*****************************************************************************
T5Evs_GetMemorySize - MONO CLIENT
Get the size of memory to be allocated for event server
Parameters:
   pSettings (IN) event server settings
Return: size in bytes or 0 if error
*****************************************************************************/

T5_DWORD T5Evs1_GetMemorySize (T5PTR_EVSETTINGS pSettings)
{
    T5_DWORD dwSize;

    if (pSettings == NULL)
        return 0L;

    dwSize = sizeof (T5STR_EVS);
    dwSize += T5_ALIGNED (pSettings->dwQSSystem);
    dwSize += T5_ALIGNED (pSettings->dwQSAlarm);
    dwSize += T5_ALIGNED (pSettings->dwQSEvent);
    dwSize += T5_ALIGNED (pSettings->dwQSInfo);
    return dwSize;
}

/*****************************************************************************
T5Evs_Open - MONO CLIENT
Builds memory for event server and start listening
Parameters:
   pData (IN/OUT) event server runtime memory block
   pSettings (IN) event server settings
Return: OK or error
*****************************************************************************/

T5_RET T5Evs1_Open (T5_PTR pData, T5PTR_EVSETTINGS pSettings)
{
    T5PTR_EVS pEvs;
    T5_PTBYTE pQueue;

    if (pData == NULL || pSettings == NULL)
        return T5RET_ERROR;
    pEvs = (T5PTR_EVS)pData;
    T5_MEMSET (pEvs, 0, sizeof (T5STR_EVS));
    /* create listening socket */
    pEvs->sockListen = T5_INVSOCKET;
    pEvs->sockData = T5_INVSOCKET;
    if (pSettings->wPort != 0)
    {
        if (T5Socket_CreateListeningSocket (pSettings->wPort, 1,
                               &(pEvs->sockListen),T5_HNDNULL) != T5RET_OK)
            return T5RET_ERROR;
        if (pEvs->sockListen == T5_INVSOCKET)
            return T5RET_ERROR;
    }
    /* initialize message queues */
    pQueue = (T5_PTBYTE)pData;
    pQueue += sizeof (T5STR_EVS);
    T5Evsq_Init (&(pEvs->qSystem), pSettings->dwQSSystem,
                 pSettings->dwPswSystem, pQueue);
    pQueue += T5_ALIGNED (pSettings->dwQSSystem);
    T5Evsq_Init (&(pEvs->qAlarm), pSettings->dwQSAlarm,
                 pSettings->dwPswAlarm, pQueue);
    pQueue += T5_ALIGNED (pSettings->dwQSAlarm);
    T5Evsq_Init (&(pEvs->qEvent), pSettings->dwQSEvent,
                 pSettings->dwPswEvent, pQueue);
    pQueue += T5_ALIGNED (pSettings->dwQSEvent);
    T5Evsq_Init (&(pEvs->qInfo), pSettings->dwQSInfo,
                 pSettings->dwPswInfo, pQueue);
    /* initialize status */
    pEvs->wStatus = T5EVSTAT_LISTEN;
    return T5RET_OK;
}

/*****************************************************************************
T5Evs_Close - MONO CLIENT
Close the event server
Parameters:
   pData (IN/OUT) event server runtime memory block
*****************************************************************************/

void T5Evs1_Close (T5_PTR pData)
{
    T5PTR_EVS pEvs;

    if (pData == NULL)
        return;
    pEvs = (T5PTR_EVS)pData;
    if (pEvs->sockData != T5_INVSOCKET)
        T5Socket_CloseSocket (pEvs->sockData);
    if (pEvs->sockListen != T5_INVSOCKET)
        T5Socket_CloseSocket (pEvs->sockListen);
    pEvs->sockData = pEvs->sockListen = T5_INVSOCKET;
}

/*****************************************************************************
T5Evs_Activate - MONO CLIENT
Activates communication and current tasks (State machine)
Parameters:
   pData (IN/OUT) event server runtime memory block
   dwTimeStamp (IN) current time stamp in ms
*****************************************************************************/

void T5Evs1_Activate (T5_PTR pData, T5_DWORD dwTimeStamp)
{
    T5PTR_EVS pEvs;

    if (pData == NULL)
        return;
    pEvs = (T5PTR_EVS)pData;

    /* check that port is open
    if (pEvs->sockListen == T5_INVSOCKET)
        return;
    */

    switch (pEvs->wStatus)
    {
    case T5EVSTAT_LISTEN :
        if (pEvs->sockListen != T5_INVSOCKET)
            pEvs->sockData = T5Socket_Accept (pEvs->sockListen, NULL);
        if (pEvs->sockData != T5_INVSOCKET)
        {
            pEvs->dwLastSent = dwTimeStamp;
            pEvs->wStatus = T5EVSTAT_SENDING;
            T5EvsFrm_PutIdle (pEvs);
            _T5Evs_FlushReadPointers (pEvs);
            _T5Evs_Send (pEvs);
        }
        break;
    case T5EVSTAT_SENDING :
        _T5Evs_Send (pEvs);
        break;
    case T5EVSTAT_RECEIVING :
        if (T5_BOUNDTIME(dwTimeStamp - pEvs->dwLastSent) > T5EVS_TIMEOUT)
        {
            T5Socket_CloseSocket (pEvs->sockData);
            pEvs->sockData = T5_INVSOCKET;
            _T5Evs_FlushReadPointers (pEvs);
            pEvs->wRights = 0;
            pEvs->wStatus = T5EVSTAT_LISTEN;
            break;
        }
        _T5Evs_Receive (pEvs);
        if (pEvs->wStatus == T5EVSTAT_RECEIVING
            || pEvs->wStatus == T5EVSTAT_LISTEN)
            break;
        if (T5EvsFrm_GetAck (pEvs))
            _T5Evs_FlushSentEvents (pEvs);
        else
            _T5Evs_FlushReadPointers (pEvs);
        /* continue: send other pending events if any */
    case T5EVSTAT_READY :
        if (T5EvsFrm_PutEventList (pEvs))
        {
            pEvs->dwLastSent = dwTimeStamp;
            pEvs->wStatus = T5EVSTAT_SENDING;
            _T5Evs_Send (pEvs);
        }
        else if (T5_BOUNDTIME(dwTimeStamp - pEvs->dwLastSent)
                 > T5EVS_IDLEPERIOD)
        {
            pEvs->dwLastSent = dwTimeStamp;
            pEvs->wStatus = T5EVSTAT_SENDING;
            T5EvsFrm_PutIdle (pEvs);
            _T5Evs_FlushReadPointers (pEvs);
            _T5Evs_Send (pEvs);
        }
        break;
    default : break;
    }
}

/*****************************************************************************
_T5Evs_Send
Start sending a packet of events
Parameters:
   pEvs (IN/OUT) event server runtime memory block
*****************************************************************************/

static void _T5Evs_Send (T5PTR_EVS pEvs)
{
    T5_PTBYTE pData;
    T5_WORD i, wRec, wExpect;
    T5_BOOL bFail;
    T5_BYTE bDummy;

    wRec = 0;

    bFail = FALSE;
    for (i=0; i<1024 && !bFail && T5Socket_Receive (pEvs->sockData, 1, &bDummy, &bFail); i++)
        ; /*flush*/

    if (!bFail)
    {
        wExpect = pEvs->wBPakSize - pEvs->wBSize;
        pData = pEvs->bf + pEvs->wBSize;
        wRec = T5Socket_Send (pEvs->sockData, wExpect, pData, &bFail);
    }

    if (bFail)
    {
        T5Socket_CloseSocket (pEvs->sockData);
        pEvs->sockData = T5_INVSOCKET;
        _T5Evs_FlushReadPointers (pEvs);
        pEvs->wRights = 0;
        pEvs->wStatus = T5EVSTAT_LISTEN;
        return;
    }
    if (wRec)
    {
        pEvs->wBSize += wRec;
        if (pEvs->wBSize == pEvs->wBPakSize)
        {
            pEvs->wBPakSize = 0;
            pEvs->wBSize = 0;
            if (pEvs->wRights & T5EVS_NOACK)
            {
                _T5Evs_FlushSentEvents (pEvs);
                pEvs->wStatus = T5EVSTAT_READY;
            }
            else
            {
                pEvs->wStatus = T5EVSTAT_RECEIVING;
            }
        }
    }
}

/*****************************************************************************
_T5Evs_Receive
manage reception of acknoledges
Parameters:
   pEvs (IN/OUT) event server runtime memory block
*****************************************************************************/

static void _T5Evs_Receive (T5PTR_EVS pEvs)
{
    T5_PTBYTE pData;
    T5_WORD i, wRec, wExpect, wLen;
    T5_BOOL bFail;

    /* receive frame header */
    if (pEvs->wBPakSize == 0)
    {
        wExpect = 4 - pEvs->wBSize;
        pData = pEvs->bf + pEvs->wBSize;
        wRec = T5Socket_Receive (pEvs->sockData, wExpect, pData, &bFail);
        if (bFail)
        {
            T5Socket_CloseSocket (pEvs->sockData);
            pEvs->sockData = T5_INVSOCKET;
            _T5Evs_FlushReadPointers (pEvs);
            pEvs->wRights = 0;
            pEvs->wStatus = T5EVSTAT_LISTEN;
            return;
        }
        pEvs->wBSize += wRec;
        if (pEvs->wBSize == 4)
        {
            if (!T5CS_ISHEAD(pEvs->bf))
            {
                for (i=0; i<1024 && !bFail && T5Socket_Receive (pEvs->sockData, 1, pData, &bFail); i++)
                    ; /*flush*/
                if (bFail)
                {
                    T5Socket_CloseSocket (pEvs->sockData);
                    pEvs->sockData = T5_INVSOCKET;
                    _T5Evs_FlushReadPointers (pEvs);
                    pEvs->wRights = 0;
                    pEvs->wStatus = T5EVSTAT_LISTEN;
                    return;
                }
                else
                    pEvs->wStatus = T5EVSTAT_READY;
            }
            else
            {
                T5_COPYFRAMEWORD (&wLen, pEvs->bf + 2);
                pEvs->wBPakSize = wLen + 4;
            }
        }
    }
    /* receive frame data */
    if (pEvs->wBPakSize == 0)
        return;
    wExpect = pEvs->wBPakSize - pEvs->wBSize;
    pData = pEvs->bf + pEvs->wBSize;
    wRec = T5Socket_Receive (pEvs->sockData, wExpect, pData, &bFail);
    if (bFail)
    {
        T5Socket_CloseSocket (pEvs->sockData);
        pEvs->sockData = T5_INVSOCKET;
        _T5Evs_FlushReadPointers (pEvs);
        pEvs->wRights = 0;
        pEvs->wStatus = T5EVSTAT_LISTEN;
        return;
    }
    if (wRec)
    {
        pEvs->wBSize += wRec;
        if (pEvs->wBSize == pEvs->wBPakSize)
            pEvs->wStatus = T5EVSTAT_READY;
    }
}

/*****************************************************************************
_T5Evs_FlushReadPointers
Flush all read pointers in message queues
Parameters:
   pEvs (IN/OUT) event server runtime memory block
*****************************************************************************/

static void _T5Evs_FlushReadPointers (T5PTR_EVS pEvs)
{
    if (T5EVSHOOK_BEFORE)
    {
        T5Evsq_FlushReadPointer (&(pEvs->qSystem));
        T5Evsq_FlushReadPointer (&(pEvs->qAlarm));
        T5Evsq_FlushReadPointer (&(pEvs->qEvent));
        T5Evsq_FlushReadPointer (&(pEvs->qInfo));
        T5EVSHOOK_AFTER;
    }
}

/*****************************************************************************
_T5Evs_FlushEAEvents
Flush all pending EVENTS class events even if not sent
Parameters:
   pData (IN/OUT) event server runtime memory block
*****************************************************************************/

void T5Evs1_FlushEAEvents (T5_PTR pData)
{
    T5PTR_EVS pEvs;

    if (pData == NULL)
        return;
    pEvs = (T5PTR_EVS)pData;

    if (T5EVSHOOK_BEFORE)
    {
        /* ??? T5Evsq_FlushAllEvents (&(pEvs->qAlarm)); */
        T5Evsq_FlushAllEvents (&(pEvs->qEvent));
        T5EVSHOOK_AFTER;
    }
}

/*****************************************************************************
_T5Evs_FlushSentEvents
Flush read pointers after acknoledge
Parameters:
   pEvs (IN/OUT) event server runtime memory block
*****************************************************************************/

static void _T5Evs_FlushSentEvents (T5PTR_EVS pEvs)
{
    if (T5EVSHOOK_BEFORE)
    {
        T5Evsq_FlushReadData (&(pEvs->qSystem));
        T5Evsq_FlushReadData (&(pEvs->qAlarm));
        T5Evsq_FlushReadData (&(pEvs->qEvent));
        T5Evsq_FlushReadData (&(pEvs->qInfo));
        T5EVSHOOK_AFTER;
    }
}

/*****************************************************************************
T5Evs_SetCallback - MONO CLIENT
Registers a callback function
Parameters:
   pData (IN/OUT) event server runtime memory block
   pCallback (IN) pointer to callback function
Return: OK or error
*****************************************************************************/

T5_RET T5Evs1_SetCallback (T5_PTR pData, T5HND_EVSCB pCallback)
{
    T5PTR_EVS pEvs;
    T5_WORD i;

    if (pData == NULL || pCallback == T5_HNDNULL)
        return T5RET_ERROR;
    pEvs = (T5PTR_EVS)pData;

    for (i=0; i<T5MAX_EVSCB; i++)
    {
        if (pEvs->cb[i] == T5_HNDNULL)
        {
            pEvs->cb[i] = pCallback;
            return T5RET_OK;
        }
    }
    return T5RET_ERROR;
}

/*****************************************************************************
T5Evs_RemoveCallback - MONO CLIENT
Unregisters a callback function
Parameters:
   pData (IN/OUT) event server runtime memory block
   pCallback (IN) pointer to callback function
Return: OK or error
*****************************************************************************/

T5_RET T5Evs1_RemoveCallback (T5_PTR pData, T5HND_EVSCB pCallback)
{
    T5PTR_EVS pEvs;
    T5_WORD i, j;

    if (pData == NULL || pCallback == T5_HNDNULL)
        return T5RET_ERROR;
    pEvs = (T5PTR_EVS)pData;

    for (i=0; i<T5MAX_EVSCB; i++)
    {
        if (pEvs->cb[i] == pCallback)
        {
            for (j=i; j<(T5MAX_EVSCB-1); j++)
            {
                pEvs->cb[j] = pEvs->cb[j+1];
                pEvs->cb[T5MAX_EVSCB-1] = T5_HNDNULL;
            }
            return T5RET_OK;
        }
    }
    return T5RET_ERROR;
}

/*****************************************************************************
_T5Evs_Callback - MONO CLIENT
Calls all callback functions
Parameters:
   pEvs (IN/OUT) event server runtime memory block
   bClass (IN) event class
   bID (IN) event ID
   bLen (IN) length of event data
   pData (IN) event data
*****************************************************************************/

static void _T5Evs_Callback (T5PTR_EVS pEvs, T5_BYTE bClass, T5_BYTE bID,
                             T5_BYTE bLen, T5_PTBYTE pData)
{
    T5_WORD i;

    for (i=0; i<T5MAX_EVSCB && pEvs->cb[i] != T5_HNDNULL; i++)
        (pEvs->cb[i]) (bClass, bID, bLen, pData);
}

/*****************************************************************************
T5Evs_Trace - MONO CLIENT
Records a trace event
Parameters:
   pData (IN/OUT) event server runtime memory block
   szText (IN) null terminated trace message
Return: OK or error
*****************************************************************************/

T5_RET T5Evs1_Trace (T5_PTR pData, T5_PTCHAR pText)
{
    return T5Evs1_TextEvent (pData, T5EVS_INFO, T5EV_TRACE, pText);
}

/*****************************************************************************
T5Evs_OnAppStart / T5Evs_OnAppStop / T5Evs_OnAppChange - MONO CLIENT
Records a system event
Parameters:
   pData (IN/OUT) event server runtime memory block
   szAppName (IN) null terminated application name
Return: OK or error
*****************************************************************************/

T5_RET T5Evs1_OnAppStart (T5_PTR pData, T5_PTCHAR szAppName)
{
    return T5Evs1_TextEvent (pData, T5EVS_SYSTEM, T5EV_APPSTART, szAppName);
}

T5_RET T5Evs1_OnAppStop (T5_PTR pData, T5_PTCHAR szAppName)
{
    return T5Evs1_TextEvent (pData, T5EVS_SYSTEM, T5EV_APPSTOP, szAppName);
}

T5_RET T5Evs1_OnAppChange (T5_PTR pData, T5_PTCHAR szAppName)
{
    return T5Evs1_TextEvent (pData, T5EVS_SYSTEM, T5EV_APPCHANGE, szAppName);
}

/*****************************************************************************
T5Evs_TextEvent - MONO CLIENT
Records an event with text data
Parameters:
   pData (IN/OUT) event server runtime memory block
   bClass (IN) event class
   bID (IN) event ID
   szText (IN) null terminated event data
Return: OK or error
*****************************************************************************/

T5_RET T5Evs1_TextEvent (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                         T5_PTCHAR szText)
{
    T5_WORD wLen;

    wLen = T5_STRLEN(szText);
    if (wLen > 255)
        wLen = 255;
    return T5Evs1_Event (pData, bClass, bID, (T5_BYTE)wLen, szText);
}

/*****************************************************************************
T5Evs_Event - MONO CLIENT
Records an event
Parameters:
   pData (IN/OUT) event server runtime memory block
   bClass (IN) event class
   bID (IN) event ID
   bLen (IN) length of event data
   pData (IN) event data
Return: OK or error
*****************************************************************************/

T5_RET T5Evs1_Event (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                     T5_BYTE bLen, T5_PTR pArg)
{
    T5_DWORD dwDate, dwTime;

#ifdef T5DEF_RTCLOCKSTAMP
    dwDate = T5RtClk_GetCurDateStamp ();
    dwTime = T5RtClk_GetCurTimeStamp ();
#else /*T5DEF_RTCLOCKSTAMP*/
    dwDate = 0L;
    dwTime = 0L;
#endif /*T5DEF_RTCLOCKSTAMP*/

    return T5Evs1_EventDT (pData, bClass, bID, bLen, pArg,
                           dwDate, dwTime);
}

#ifndef T5EVS1_EVENTDT_ENTRY
#define T5EVS1_EVENTDT_ENTRY /*nothing*/
#endif /*T5EVS1_EVENTDT_ENTRY*/

#ifndef T5EVS1_EVENTDT_EXIT
#define T5EVS1_EVENTDT_EXIT /*nothing*/
#endif /*T5EVS1_EVENTDT_EXIT*/

#ifndef T5BIND_CANPRODUCETOSOCK
#define T5BIND_CANPRODUCETOSOCK(s) TRUE
#endif /*T5BIND_CANPRODUCETOSOCK*/

T5_RET T5Evs1_EventDT (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                     T5_BYTE bLen, T5_PTR pArg,
                     T5_DWORD dwDate, T5_DWORD dwTime)
{
    T5PTR_EVS pEvs;
    T5PTR_EVQUEUE pQueue;
    T5_PTBYTE pW;
    T5_RET wRet;

    T5EVS1_EVENTDT_ENTRY

    if (pData == NULL)
    {
        T5EVS1_EVENTDT_EXIT
        return T5RET_ERROR;
    }
    pEvs = (T5PTR_EVS)pData;

    if (!T5BIND_CANPRODUCETOSOCK (pEvs->sockData))
    {
        T5EVS1_EVENTDT_EXIT
        return T5RET_OK;
    }

    _T5Evs_Callback (pEvs, bClass, bID, bLen, (T5_PTBYTE)pArg);

    if ((pEvs->wRights & bClass) == 0)
    {
        T5EVS1_EVENTDT_EXIT
        return T5RET_OK;
    }

    if (bID == T5EV_OPTBROADCAST
        && (pEvs->wRights & T5EVS_NOBROADCAST) != 0)
    {
        T5EVS1_EVENTDT_EXIT
        return T5RET_OK;
    }

    if (pArg == NULL)
        bLen = 0;

    switch (bClass)
    {
    case T5EVS_SYSTEM :
        pQueue = &(pEvs->qSystem);
        break;
    case T5EVS_ALARM :
        pQueue = &(pEvs->qAlarm);
        break;
    case T5EVS_EVENT :
    case (T5EVS_EVENT | T5EVS_STRING) :
        pQueue = &(pEvs->qEvent);
        break;
    case T5EVS_INFO :
        pQueue = &(pEvs->qInfo);
        break;
    default :
        {
            T5EVS1_EVENTDT_EXIT
            return T5RET_ERROR;
        }
    }

    if (!T5EVSHOOK_BEFORE)
    {
        return T5RET_ERROR;
    }

    if ((pQueue->dwWrite + (T5_DWORD)bLen + 12L) > pQueue->dwSize)
    {
        pQueue->wLost += 1;
        wRet = T5RET_ERROR;
    }
    else
    {
        pW = pQueue->pData + pQueue->dwWrite;
        *pW++ = bClass;
        *pW++ = bID;
        *pW++ = bLen;
        *pW++ = (T5_BYTE)(pEvs->wID);
        T5_MEMCPY (pW, &dwDate, sizeof (T5_DWORD));
        T5_MEMCPY (pW+4, &dwTime, sizeof (T5_DWORD));
        if (bLen)
            T5_MEMCPY (pW+8, pArg, bLen);

        pQueue->dwWrite += ((T5_DWORD)bLen + 12L);
        pQueue->wFlags &= ~T5EVQUEUE_EMPTY;
        wRet = T5RET_OK;
    }
    T5EVSHOOK_AFTER;

    T5EVS1_EVENTDT_EXIT
    return wRet;
}

/****************************************************************************/
/* the rest of the file is the implementation of multiple connection server */

#ifdef T5MAX_EVCLIENT

/****************************************************************************/
/* static services - MULTIPLE CLIENTS */

static void _T5Evss_Callback (T5PTR_EVSS pEvss, T5_BYTE bClass, T5_BYTE bID,
                              T5_BYTE bLen, T5_PTBYTE pData);

/*****************************************************************************
T5Evs_GetMemorySize - MULTIPLE CLIENTS
Get the size of memory to be allocated for event server
Parameters:
   pSettings (IN) event server settings
Return: size in bytes or 0 if error
*****************************************************************************/

T5_DWORD T5Evs_GetMemorySize (T5PTR_EVSETTINGS pSettings)
{
    T5_DWORD dwSize;

    if (pSettings == NULL)
        return 0L;

    dwSize = T5Evs1_GetMemorySize (pSettings);
    dwSize *= T5MAX_EVCLIENT;
    dwSize += sizeof (T5STR_EVSS);

    return dwSize;
}

/*****************************************************************************
T5Evs_Open - MULTIPLE CLIENTS
Builds memory for event server and start listening
Parameters:
   pData (IN/OUT) event server runtime memory block
   pSettings (IN) event server settings
Return: OK or error
*****************************************************************************/

T5_RET T5Evs_Open (T5_PTR pData, T5PTR_EVSETTINGS pSettings)
{
    T5PTR_EVSS pEvss;
    T5_PTBYTE pClient;
    T5_WORD i;

    if (pData == NULL || pSettings == NULL)
        return T5RET_ERROR;

    T5_MEMSET (pData, 0, sizeof (T5STR_EVSS));

    pEvss = (T5PTR_EVSS)pData;
    pEvss->dwEvSize = T5Evs1_GetMemorySize (pSettings);
    pEvss->wPort = pSettings->wPort;
    pEvss->wFlags = 0;
    if (T5Socket_CreateListeningSocket (pEvss->wPort, T5MAX_EVCLIENT,
                           &(pEvss->sockListen), T5_HNDNULL) != T5RET_OK)
        return T5RET_ERROR;
    if (pEvss->sockListen == T5_INVSOCKET)
        return T5RET_ERROR;

    pSettings->wPort = 0;
    pClient = (T5_PTBYTE)pData;
    pClient += sizeof (T5STR_EVSS);
    for (i=0; i<T5MAX_EVCLIENT; i++, pClient += pEvss->dwEvSize)
        T5Evs1_Open (pClient, pSettings);
    pSettings->wPort = pEvss->wPort;
    return T5RET_OK;
}

/*****************************************************************************
T5Evs_Close - MULTIPLE CLIENTS
Close the event server
Parameters:
   pData (IN/OUT) event server runtime memory block
*****************************************************************************/

void T5Evs_Close (T5_PTR pData)
{
    T5PTR_EVSS pEvss;
    T5_PTBYTE pClient;
    T5_WORD i;

    if (pData == NULL)
        return;

    pEvss = (T5PTR_EVSS)pData;
    pClient = (T5_PTBYTE)pData;
    pClient += sizeof (T5STR_EVSS);
    for (i=0; i<T5MAX_EVCLIENT; i++, pClient += pEvss->dwEvSize)
        T5Evs1_Close (pClient);

    if (pEvss->sockListen != T5_INVSOCKET)
        T5Socket_CloseSocket (pEvss->sockListen);
    pEvss->sockListen = T5_INVSOCKET;
    pEvss->wFlags = 0;
}

/*****************************************************************************
T5Evs_Activate - MULTIPLE CLIENTS
Activates communication and current tasks (State machine)
Parameters:
   pData (IN/OUT) event server runtime memory block
   dwTimeStamp (IN) current time stamp in ms
*****************************************************************************/

#ifndef T5EVS_ACTIVATE_ENTRY
#define T5EVS_ACTIVATE_ENTRY /*nothing*/
#endif /*T5EVS_ACTIVATE_ENTRY*/

#ifndef T5EVS_ACTIVATE_EXIT
#define T5EVS_ACTIVATE_EXIT /*nothing*/
#endif /*T5EVS_ACTIVATE_EXIT*/

#ifndef T5EVS_NBACCEPTLOOP
#define T5EVS_NBACCEPTLOOP 8
#endif /*T5EVS_NBACCEPTLOOP*/

void T5Evs_Activate (T5_PTR pData, T5_DWORD dwTimeStamp)
{
    T5_SOCKET newsock;
    T5PTR_EVSS pEvss;
    T5_PTBYTE pClient;
    T5_WORD i, wOldRights, nbAccept;

    T5EVS_ACTIVATE_ENTRY

    if (pData == NULL)
    {
        T5EVS_ACTIVATE_EXIT
        return;
    }
    pEvss = (T5PTR_EVSS)pData;
    if (pEvss->sockListen == T5_INVSOCKET)
    {
        T5EVS_ACTIVATE_EXIT
        return;
    }
    /* listen for new connection */
    newsock = T5Socket_Accept (pEvss->sockListen, NULL);
    /* scan clients */
    pClient = (T5_PTBYTE)pData;
    pClient += sizeof (T5STR_EVSS);
    nbAccept = 0;
    for (i=0; i<T5MAX_EVCLIENT; i++, pClient += pEvss->dwEvSize)
    {
        /* activate existing connections */
        if (((T5PTR_EVS)pClient)->sockData != T5_INVSOCKET)
        {
            wOldRights = ((T5PTR_EVS)pClient)->wRights;
            T5Evs1_Activate (pClient, dwTimeStamp);
            if ((((T5PTR_EVS)pClient)->wRights & ~wOldRights) != 0)
                pEvss->wFlags |= T5EVSS_NEWCNX; /* mark new connection */
            else if (((T5PTR_EVS)pClient)->wRights == 0 && wOldRights != 0)
                pEvss->wFlags |= T5EVSS_NEWCNX; /* mark new connection */
        }
        /* create new connection if requested */
        else
        {
            if (newsock != T5_INVSOCKET)
            {
                ((T5PTR_EVS)pClient)->sockData = newsock;
                ((T5PTR_EVS)pClient)->wID = i + 1;
                T5Evs1_Activate (pClient, dwTimeStamp);
                nbAccept += 1;
                if (nbAccept > T5EVS_NBACCEPTLOOP)
                    newsock = T5_INVSOCKET;
                else
                    newsock = T5Socket_Accept (pEvss->sockListen, NULL);
            }
        }
    }
    /* remove new connection if overflow */
    if (newsock != T5_INVSOCKET)
        T5Socket_CloseSocket (newsock);
    T5EVS_ACTIVATE_EXIT
}

/*****************************************************************************
T5Evs_GetNbClient - MULTIPLE CLIENTS
Return the number of connected clients
Parameters:
   pData (IN/OUT) event server runtime memory block
*****************************************************************************/

T5_WORD T5Evs_GetNbClient (T5_PTR pData)
{
    T5PTR_EVSS pEvss;
    T5_WORD i, nb;
    T5_PTBYTE pClient;

    if (pData == NULL)
        return 0;
    pEvss = (T5PTR_EVSS)pData;
    nb = 0;
    pClient = (T5_PTBYTE)pData;
    pClient += sizeof (T5STR_EVSS);
    for (i=0; i<T5MAX_EVSCB; i++, pClient += pEvss->dwEvSize)
    {
        if (((T5PTR_EVS)pClient)->sockData != T5_INVSOCKET)
            nb += 1;
    }
    return nb;
}

/*****************************************************************************
T5Evs_SetCallback - MULTIPLE CLIENTS
Registers a callback function
Parameters:
   pData (IN/OUT) event server runtime memory block
   pCallback (IN) pointer to callback function
Return: OK or error
*****************************************************************************/

T5_RET T5Evs_SetCallback (T5_PTR pData, T5HND_EVSCB pCallback)
{
    T5PTR_EVSS pEvss;
    T5_WORD i;

    if (pData == NULL || pCallback == T5_HNDNULL)
        return T5RET_ERROR;
    pEvss = (T5PTR_EVSS)pData;

    for (i=0; i<T5MAX_EVSCB; i++)
    {
        if (pEvss->cb[i] == T5_HNDNULL)
        {
            pEvss->cb[i] = pCallback;
            return T5RET_OK;
        }
    }
    return T5RET_ERROR;
}

/*****************************************************************************
T5Evs_RemoveCallback - MULTIPLE CLIENTS
Unregisters a callback function
Parameters:
   pData (IN/OUT) event server runtime memory block
   pCallback (IN) pointer to callback function
Return: OK or error
*****************************************************************************/

T5_RET T5Evs_RemoveCallback (T5_PTR pData, T5HND_EVSCB pCallback)
{
    T5PTR_EVSS pEvss;
    T5_WORD i, j;

    if (pData == NULL || pCallback == T5_HNDNULL)
        return T5RET_ERROR;
    pEvss = (T5PTR_EVSS)pData;

    for (i=0; i<T5MAX_EVSCB; i++)
    {
        if (pEvss->cb[i] == pCallback)
        {
            for (j=i; j<(T5MAX_EVSCB-1); j++)
            {
                pEvss->cb[j] = pEvss->cb[j+1];
                pEvss->cb[T5MAX_EVSCB-1] = T5_HNDNULL;
            }
            return T5RET_OK;
        }
    }
    return T5RET_ERROR;
}

/*****************************************************************************
_T5Evs_Callback - MULTIPLE CLIENTS
Calls all callback functions
Parameters:
   pEvss (IN/OUT) event server runtime memory block
   bClass (IN) event class
   bID (IN) event ID
   bLen (IN) length of event data
   pData (IN) event data
*****************************************************************************/

static void _T5Evss_Callback (T5PTR_EVSS pEvss, T5_BYTE bClass, T5_BYTE bID,
                              T5_BYTE bLen, T5_PTBYTE pData)
{
    T5_WORD i;

    for (i=0; i<T5MAX_EVSCB && pEvss->cb[i] != T5_HNDNULL; i++)
        (pEvss->cb[i]) (bClass, bID, bLen, pData);
}

/*****************************************************************************
T5Evs_Trace - MULTIPLE CLIENTS
Records a trace event
Parameters:
   pData (IN/OUT) event server runtime memory block
   szText (IN) null terminated trace message
Return: OK or error
*****************************************************************************/

T5_RET T5Evs_Trace (T5_PTR pData, T5_PTCHAR pText)
{
    return T5Evs_TextEvent (pData, T5EVS_INFO, T5EV_TRACE, pText);
}

/*****************************************************************************
T5Evs_OnAppStart / T5Evs_OnAppStop / T5Evs_OnAppChange - MULTIPLE CLIENTS
Records a system event
Parameters:
   pData (IN/OUT) event server runtime memory block
   szAppName (IN) null terminated application name
Return: OK or error
*****************************************************************************/

T5_RET T5Evs_OnAppStart (T5_PTR pData, T5_PTCHAR szAppName)
{
    return T5Evs_TextEvent (pData, T5EVS_SYSTEM, T5EV_APPSTART, szAppName);
}

T5_RET T5Evs_OnAppStop (T5_PTR pData, T5_PTCHAR szAppName)
{
    return T5Evs_TextEvent (pData, T5EVS_SYSTEM, T5EV_APPSTOP, szAppName);
}

T5_RET T5Evs_OnAppChange (T5_PTR pData, T5_PTCHAR szAppName)
{
    return T5Evs_TextEvent (pData, T5EVS_SYSTEM, T5EV_APPCHANGE, szAppName);
}

/*****************************************************************************
T5Evs_TextEvent - MULTIPLE CLIENTS
Records an event with text data
Parameters:
   pData (IN/OUT) event server runtime memory block
   bClass (IN) event class
   bID (IN) event ID
   szText (IN) null terminated event data
Return: OK or error
*****************************************************************************/

T5_RET T5Evs_TextEvent (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                                 T5_PTCHAR szText)
{
    T5_WORD wLen;

    wLen = T5_STRLEN(szText);
    if (wLen > 255)
        wLen = 255;
    return T5Evs_Event (pData, bClass, bID, (T5_BYTE)wLen, szText);
}

/*****************************************************************************
T5Evs_Event - MULTIPLE CLIENTS
Records an event
Parameters:
   pData (IN/OUT) event server runtime memory block
   bClass (IN) event class
   bID (IN) event ID
   bLen (IN) length of event data
   pData (IN) event data
Return: OK or error
*****************************************************************************/

T5_RET T5Evs_Event (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                    T5_BYTE bLen, T5_PTR pArg)
{
    T5PTR_EVSS pEvss;
    T5_PTBYTE pClient;
    T5_WORD i;
    T5_RET wRet, wRetc;

    if (pData == NULL)
        return T5RET_ERROR;

    pEvss = (T5PTR_EVSS)pData;
    _T5Evss_Callback (pEvss, bClass, bID, bLen, (T5_PTBYTE)pArg);

    pClient = (T5_PTBYTE)pData;
    pClient += sizeof (T5STR_EVSS);
    wRet = T5RET_OK;
    for (i=0; i<T5MAX_EVCLIENT; i++, pClient += pEvss->dwEvSize)
    {
        if (((T5PTR_EVS)pClient)->sockData != T5_INVSOCKET)
        {
            wRetc = T5Evs1_Event (pClient, bClass, bID, bLen, (T5_PTBYTE)pArg);
            if (wRet == T5RET_OK)
                wRet = wRetc;
        }
    }
    return wRet;
}

/*****************************************************************************
T5Evs_EventTo - MULTIPLE CLIENTS
Records an event for only one client in a multiple client server
Parameters:
   pData (IN/OUT) event server runtime memory block
   bClass (IN) event class
   bID (IN) event ID
   bLen (IN) length of event data
   pArg (IN) event data
   wServer (IN) index of the mono client server
   pbNoServer (OUT) set to TRUE if server failed
   dwDate, dwTime (IN) event time stamp
Return: OK or error
*****************************************************************************/

T5_RET T5Evs_EventTo (T5_PTR pData, T5_BYTE bClass, T5_BYTE bID,
                      T5_BYTE bLen, T5_PTR pArg,
                      T5_WORD wServer, T5_PTBOOL pbNoServer,
                      T5_DWORD dwDate, T5_DWORD dwTime)
{
    T5PTR_EVSS pEvss;
    T5_PTBYTE pClient;

    if (pData == NULL)
        return T5RET_ERROR;

    pEvss = (T5PTR_EVSS)pData;

    if (wServer >= T5MAX_EVCLIENT)
    {
        *pbNoServer = TRUE;
        return T5RET_ERROR;
    }

    pClient = (T5_PTBYTE)pData;
    pClient += sizeof (T5STR_EVSS);
    pClient += (wServer * pEvss->dwEvSize);

    if (((T5PTR_EVS)pClient)->sockData == T5_INVSOCKET)
    {
        *pbNoServer = TRUE;
        return T5RET_ERROR;
    }

    *pbNoServer = FALSE;
    return T5Evs1_EventDT (pClient, bClass, bID, bLen, pArg, dwDate, dwTime);
}

/*****************************************************************************
T5Evs_GetCnxPrefs - MULTIPLE CLIENTS
Get rights and preferences of a client
Parameters:
   pData (IN/OUT) event server runtime memory block
   wServer (IN) index of the mono client server
Return: rights
*****************************************************************************/

T5_WORD T5Evs_GetCnxPrefs (T5_PTR pData, T5_WORD wServer)
{
    T5PTR_EVSS pEvss;
    T5_PTBYTE pClient;

    if (pData == NULL)
        return 0;
    pEvss = (T5PTR_EVSS)pData;
    if (wServer >= T5MAX_EVCLIENT)
        return 0;

    pClient = (T5_PTBYTE)pData;
    pClient += sizeof (T5STR_EVSS);
    pClient += (wServer * pEvss->dwEvSize);

    if (((T5PTR_EVS)pClient)->sockData == T5_INVSOCKET)
        return 0;

    return ((T5PTR_EVS)pClient)->wRights;
}

void T5Evs_GetCnxVsiPrefs (T5_PTR pData, T5_PTBOOL pFlags)
{
    T5PTR_EVSS pEvss;
    T5PTR_EVS pEvClient;
    T5_PTBYTE pClient;
    T5_WORD i;

    T5_MEMSET (pFlags, 0, T5MAX_EVCLIENT * sizeof (T5_BOOL));
    if (pData == NULL)
        return;

    pEvss = (T5PTR_EVSS)pData;
    pClient = (T5_PTBYTE)pData;
    pClient += sizeof (T5STR_EVSS);

    for (i=0; i<T5MAX_EVCLIENT; i++)
    {
        pEvClient = (T5PTR_EVS)pClient;
        pFlags[i] = (pEvClient->sockData != T5_INVSOCKET
                     && (pEvClient->wRights & T5EVS_VSI) != 0);
        pClient += pEvss->dwEvSize;
    }
}

/*****************************************************************************
T5Evs_HasNewConections - MULTIPLE CLIENTS
Check if a multiple client server has at least one new client registered
Parameters:
   pData (IN/OUT) event server runtime memory block
Return: TRUE if new client(s) connected
*****************************************************************************/

T5_BOOL T5Evs_HasNewConections (T5_PTR pData)
{
    T5_BOOL bNew;
    T5PTR_EVSS pEvss;

    if (pData == NULL)
        return FALSE;

    pEvss = (T5PTR_EVSS)pData;

    bNew = ((pEvss->wFlags & T5EVSS_NEWCNX) != 0);
    pEvss->wFlags &= ~T5EVSS_NEWCNX;
    return bNew;
}

/*****************************************************************************
T5Evs_FlushEAEvents - MULTIPLE CLIENTS
Flush all pending EVENTS class events even if not sent
Parameters:
   pData (IN/OUT) event server runtime memory block
*****************************************************************************/

void T5Evs_FlushEAEvents (T5_PTR pData)
{
    T5PTR_EVSS pEvss;
    T5_PTBYTE pClient;
    T5_WORD i;

    if (pData == NULL)
        return;

    pEvss = (T5PTR_EVSS)pData;

    pClient = (T5_PTBYTE)pData;
    pClient += sizeof (T5STR_EVSS);
    for (i=0; i<T5MAX_EVCLIENT; i++, pClient += pEvss->dwEvSize)
        T5Evs1_FlushEAEvents (pClient);
}

/****************************************************************************/

#endif /*T5MAX_EVCLIENT*/

/****************************************************************************/

#endif /*T5DEF_EVENTSERVER*/

/* eof **********************************************************************/
