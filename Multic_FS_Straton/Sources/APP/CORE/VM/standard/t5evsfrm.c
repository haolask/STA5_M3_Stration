/*****************************************************************************
T5EvsFrm.c : Event server - build and parse frames

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_EVENTSERVER

/****************************************************************************/
/* static services */

static T5_BOOL _T5EvsFrm_PutQueueLost (T5PTR_EVS pEvs,
                                       T5PTR_EVQUEUE pQueue, T5_BYTE bID);
static T5_BOOL _T5EvsFrm_PutQueueEvents (T5PTR_EVS pEvs,
                                         T5PTR_EVQUEUE pQueue);

/*****************************************************************************
T5EvsFrm_GetAck
Manage client acknoledge - set preferences
Parameters:
   pEvs (IN) event server runtime data
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5EvsFrm_GetAck (T5PTR_EVS pEvs)
{
    T5_WORD wPref, wSize;
    T5_DWORD dwPass;

    /* check frame */
    if (!T5CS_ISHEAD(pEvs->bf) || pEvs->bf[4] != T5CSRQ_EVS)
        return FALSE;
    /* get preferences */
    wPref = (T5_WORD)(pEvs->bf[5]) & 0xff;
    T5_COPYFRAMEDWORD (&dwPass, pEvs->bf + 6);
    /* check password and set client rights */
    if (dwPass == pEvs->qSystem.dwPass)
        pEvs->wRights = wPref;
    else if (dwPass == pEvs->qAlarm.dwPass)
        pEvs->wRights = wPref & ~T5EVS_SYSTEM;
    else if (dwPass == pEvs->qEvent.dwPass)
        pEvs->wRights = wPref & ~(T5EVS_SYSTEM|T5EVS_ALARM);
    else if (dwPass == pEvs->qInfo.dwPass)
        pEvs->wRights = wPref & ~(T5EVS_SYSTEM|T5EVS_ALARM|T5EVS_EVENT);
    else
        pEvs->wRights = 0;

    T5_COPYFRAMEWORD (&wSize, pEvs->bf + 2);
    if (wSize < 10)
        pEvs->dwMaxBuf = 0;
    else
    {
        T5_COPYFRAMEDWORD (&(pEvs->dwMaxBuf), pEvs->bf + 10);
    }

    return TRUE;
}

/*****************************************************************************
T5EvsFrm_PutIdle
Generates an IDLE notification
Parameters:
   pEvs (IN) event server runtime data
*****************************************************************************/

void T5EvsFrm_PutIdle (T5PTR_EVS pEvs)
{
    T5STR_EVENT evt;
    T5_BYTE bVers[3];

    evt.bClass = T5EVS_INFO;
    evt.bID = T5EV_IDLE;
    evt.bLen = 3;
    evt.bCnx = (T5_BYTE)(pEvs->wID);
#ifdef T5DEF_RTCLOCKSTAMP
    evt.dwDate = T5RtClk_GetCurDateStamp ();
    evt.dwTime = T5RtClk_GetCurTimeStamp ();
#else /*T5DEF_RTCLOCKSTAMP*/
    evt.dwDate = 0L;
    evt.dwTime = 0L;
#endif /*T5DEF_RTCLOCKSTAMP*/
    bVers[0] = T5_DKVERS_H;
    bVers[1] = T5_DKVERS_M;
    bVers[2] = T5_DKVERS_L;
    T5EvsFrm_PutHeader (pEvs);
    T5EvsFrm_PutEvent (pEvs, &evt, bVers);
}

/*****************************************************************************
T5EvsFrm_PutHeader
Initializes a notification frame
Parameters:
   pEvs (IN) event server runtime data
*****************************************************************************/

void T5EvsFrm_PutHeader (T5PTR_EVS pEvs)
{
    T5_PTBYTE pFrame;

    pFrame = pEvs->bf;
    pFrame[0] = 'T';
    pFrame[1] = '5';
    pFrame[2] = 0;
    pFrame[3] = 4;
    pFrame[4] = T5CSRQ_EVS;
    pFrame[5] = 0;
    pEvs->wBPakSize = 6;
    pEvs->wBSize = 0;
}

/*****************************************************************************
T5EvsFrm_PutEvent
Add an event to a notification frame
Parameters:
   pEvs (IN) event server runtime data
   pEvent (IN) event description
   pData (IN) event data
Return: TRUE if OK / FALSE if frame overflow
*****************************************************************************/

T5_BOOL T5EvsFrm_PutEvent (T5PTR_EVS pEvs, T5PTR_EVENT pEvent,
                           T5_PTBYTE pData)
{
    T5_PTBYTE pFrame, pRec;
    T5_WORD wLen;
    T5_DWORD dwMax;

    pFrame = pEvs->bf;

#ifndef T5DEF_EXTEAPACKETS
    /* check frame size */
    if (pFrame[5] == 0xff)
        return FALSE;
#endif /*T5DEF_EXTEAPACKETS*/

    dwMax = pEvs->dwMaxBuf;
    if (dwMax > T5MAX_EAFRM)
        dwMax = T5MAX_EAFRM;
    else if (dwMax == 0L)
        dwMax = T5MAX_FRAME;
    if (dwMax > 0x0000ffffL)
        dwMax = 0x0000ffffL;
    if ((T5_DWORD)(pEvs->wBPakSize + pEvent->bLen + 12) >= dwMax)
        return FALSE;

    pRec = pFrame + pEvs->wBPakSize;
    /* copy event data */
    *pRec++ = pEvent->bClass;
    *pRec++ = pEvent->bID;
    *pRec++ = pEvent->bLen;
    *pRec++ = pEvent->bCnx;
    T5_COPYFRAMEDWORD (pRec, &(pEvent->dwDate));
    T5_COPYFRAMEDWORD (pRec+4, &(pEvent->dwTime));
    if (pEvent->bLen)
        T5_MEMCPY (pRec+8, pData, pEvent->bLen);
    /* upgrade length and number of events */
    pEvs->wBPakSize += ((T5_WORD)(pEvent->bLen) + 12);
    wLen = pEvs->wBPakSize - 4;
    T5_COPYFRAMEWORD (pFrame+2, &wLen);

    if (pFrame[5] != 0xff)
        pFrame[5] += 1;
    return TRUE;
}

/*****************************************************************************
T5EvsFrm_PutEventList
Builds a notification frame with pending events
Parameters:
   pEvs (IN) event server runtime data
Return: TRUE if OK / FALSE nothing to send
*****************************************************************************/

T5_BOOL T5EvsFrm_PutEventList (T5PTR_EVS pEvs)
{
    T5_BOOL bRet;
    T5_WORD wPref;

    bRet = FALSE;
    wPref = pEvs->wRights;
    T5EvsFrm_PutHeader (pEvs);
    /* system events */
    if ((wPref & T5EVS_SYSTEM) != 0)
    {
        /* lost events */
        if (pEvs->qSystem.wLost != 0)
            bRet |= _T5EvsFrm_PutQueueLost (pEvs, &(pEvs->qSystem),
                                            T5EV_QOV_SYSTEM);
        if (pEvs->qAlarm.wLost != 0)
            bRet |= _T5EvsFrm_PutQueueLost (pEvs, &(pEvs->qAlarm),
                                            T5EV_QOV_ALARM);
        if (pEvs->qEvent.wLost != 0)
            bRet |= _T5EvsFrm_PutQueueLost (pEvs, &(pEvs->qEvent),
                                            T5EV_QOV_EVENT);
        if (pEvs->qInfo.wLost != 0)
            bRet |= _T5EvsFrm_PutQueueLost (pEvs, &(pEvs->qInfo),
                                            T5EV_QOV_INFO);
        /* queue events */
        if ((pEvs->qSystem.wFlags & T5EVQUEUE_EMPTY) == 0)
            bRet |= _T5EvsFrm_PutQueueEvents (pEvs, &(pEvs->qSystem));
    }
    /* other queue events */
    if ((wPref & T5EVS_ALARM) != 0
        && (pEvs->qAlarm.wFlags & T5EVQUEUE_EMPTY) == 0)
        bRet |= _T5EvsFrm_PutQueueEvents (pEvs, &(pEvs->qAlarm));
    if ((wPref & T5EVS_EVENT) != 0
        && (pEvs->qEvent.wFlags & T5EVQUEUE_EMPTY) == 0)
        bRet |= _T5EvsFrm_PutQueueEvents (pEvs, &(pEvs->qEvent));
    if ((wPref & T5EVS_INFO) != 0
        && (pEvs->qInfo.wFlags & T5EVQUEUE_EMPTY) == 0)
        bRet |= _T5EvsFrm_PutQueueEvents (pEvs, &(pEvs->qInfo));
    return bRet;
}

/*****************************************************************************
_T5EvsFrm_PutQueueLost
Add system events to a notification frame (lost messages)
Parameters:
   pEvs (IN) event server runtime data
   pQueue (IN) event queue to be checked
   bID (IN) ID of the system event to send
Return: TRUE if OK / FALSE if no event
*****************************************************************************/

static T5_BOOL _T5EvsFrm_PutQueueLost (T5PTR_EVS pEvs,
                                       T5PTR_EVQUEUE pQueue, T5_BYTE bID)
{
    T5STR_EVENT evt;

    evt.bClass = T5EVS_SYSTEM;
    evt.bID = bID;
    evt.bLen = 0;
    evt.bCnx = (T5_BYTE)(pEvs->wID);
#ifdef T5DEF_RTCLOCKSTAMP
    evt.dwDate = T5RtClk_GetCurDateStamp ();
    evt.dwTime = T5RtClk_GetCurTimeStamp ();
#else /*T5DEF_RTCLOCKSTAMP*/
    evt.dwDate = 0L;
    evt.dwTime = 0L;
#endif /*T5DEF_RTCLOCKSTAMP*/
    pQueue->wLostSent = pQueue->wLost;
    T5EvsFrm_PutEvent (pEvs, &evt, NULL);
    return TRUE;
}

/*****************************************************************************
_T5EvsFrm_PutQueueEvents
Add message queue events to a notification frame
Parameters:
   pEvs (IN) event server runtime data
   pQueue (IN) event queue to be checked
Return: TRUE if OK / FALSE if no event
*****************************************************************************/

static T5_BOOL _T5EvsFrm_PutQueueEvents (T5PTR_EVS pEvs, T5PTR_EVQUEUE pQueue)
{
    T5_BOOL bRet, bOK;
    T5STR_EVENT evt;
    T5_PTBYTE pData;
    T5_WORD wLen;

    bRet = FALSE;
    bOK = TRUE;
    pData = pQueue->pData;
    while (bOK && pQueue->dwRead < pQueue->dwWrite)
    {
        evt.bClass = pData[0];
        evt.bID = pData[1];
        evt.bLen = pData[2];
        evt.bCnx = pData[3];
        T5_MEMCPY (&(evt.dwDate), pData+4, sizeof (T5_DWORD));
        T5_MEMCPY (&(evt.dwTime), pData+8, sizeof (T5_DWORD));
        bOK = T5EvsFrm_PutEvent (pEvs, &evt, pData+12);
        if (bOK)
        {
            wLen = (T5_WORD)(evt.bLen) + 12;
            pData += wLen;
            pQueue->dwRead += (T5_DWORD)wLen;
            bRet = TRUE;
        }
    }
    return bRet;
}

/****************************************************************************/

#endif /*T5DEF_EVENTSERVER*/

/* eof **********************************************************************/
