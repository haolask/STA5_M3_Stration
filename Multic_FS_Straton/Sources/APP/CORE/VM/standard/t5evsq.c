/*****************************************************************************
T5EVSQ.c :   Event server message queue

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_EVENTSERVER

/*****************************************************************************
T5Evsq_Init
Initializes a message queue
Parameters:
   pQueue (IN) message queue
   dwPass (IN) password
   pData (IN) pointer to FUFI buffer
*****************************************************************************/

void T5Evsq_Init (T5PTR_EVQUEUE pQueue,
                  T5_DWORD dwSize, T5_DWORD dwPass, T5_PTR pData)
{
    pQueue->dwSize = dwSize;
    pQueue->dwPass = dwPass;
    pQueue->pData = (T5_PTBYTE)pData;
    pQueue->wFlags = T5EVQUEUE_EMPTY;
    pQueue->dwRead = 0L;
    pQueue->dwWrite = 0L;
}

/*****************************************************************************
T5Evsq_FlushAllEvents
Remove all pending events from a queue
Parameters:
   pQueue (IN) message queue
*****************************************************************************/

void T5Evsq_FlushAllEvents (T5PTR_EVQUEUE pQueue)
{
    pQueue->wFlags = T5EVQUEUE_EMPTY;
    pQueue->dwRead = 0L;
    pQueue->dwWrite = 0L;
}

/*****************************************************************************
T5Evsq_FlushReadPointer
Reset the read pointer of a queue
Parameters:
   pQueue (IN) message queue
*****************************************************************************/

void T5Evsq_FlushReadPointer (T5PTR_EVQUEUE pQueue)
{
    pQueue->dwRead = 0L;
    pQueue->wLostSent = 0;
}

/*****************************************************************************
T5Evsq_FlushReadData
Flush read data in a queue and adjust read / write pointers
Parameters:
   pQueue (IN) message queue
*****************************************************************************/

void T5Evsq_FlushReadData (T5PTR_EVQUEUE pQueue)
{
    T5_DWORD i, nbRead;

    /* sent events */
    nbRead = pQueue->dwRead;
    if (nbRead)
    {
        for (i=nbRead; i<pQueue->dwWrite; i++)
            pQueue->pData[i-nbRead] = pQueue->pData[i];
        pQueue->dwWrite -= nbRead;
        if (pQueue->dwWrite == 0L)
            pQueue->wFlags = T5EVQUEUE_EMPTY;
    }
    pQueue->dwRead = 0L;
    /* notified lost events */
    pQueue->wLost -= pQueue->wLostSent;
}

/****************************************************************************/

#endif /*T5DEF_EVENTSERVER*/

/* eof **********************************************************************/
