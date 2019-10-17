/*****************************************************************************
T5CSSock.c : communication server - socket based protocol

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_ETHERNET

/****************************************************************************/
/* protocol */

/*

frame header = 4 bytes - followed by data - no terminator

+-----+-----+---------+---------+---...---+
| 'T' | '5' | Nb (HB) | Nb (LB) | Data    |
+-----+-----+---------+---------+---...---+

'T' = 84 (0x54)
'5' = 53 (0x35)

*/

/****************************************************************************/
/* static services */

static void _T5CSSock_Send (T5PTR_CSPORT pPort);
static void _T5CSSock_Receive (T5PTR_CSPORT pPort);

/*****************************************************************************
T5CSSock_Accept
Accept client connections
Parameters:
    pData (IN) pointer to the communication server database
*****************************************************************************/

void T5CSSock_Accept (T5PTR_CS pData)
{
    T5_SOCKET sock;
    T5_WORD iFree;

    if (pData->sock == T5_INVSOCKET)
        return;

    sock = T5Socket_Accept (pData->sock, pData->pfCallback);
    if (sock == T5_INVSOCKET)
        return;

    iFree = 0;
    while (iFree < T5MAX_PORT && pData->csPort[iFree].wFlags != 0)
        iFree++;
    if (iFree >= T5MAX_PORT)
    {
        T5Socket_CloseSocket (sock);
        return;
    }

    pData->csPort[iFree].wFlags = T5CS_READY | T5CS_SOCKET;
    pData->csPort[iFree].sock = sock;
    pData->csPort[iFree].wID = iFree + 1;
    pData->csPort[iFree].dwTimeStamp = 0L;

#ifdef T5DEF_CSM5
    if ((pData->dwFlags & T5CS_M5) != 0)
    {
        pData->csPort[iFree].wFlags |= T5CS_M5;
    }
#endif /*T5DEF_CSM5*/
}

/*****************************************************************************
T5CSSock_CloseAllSockets
Close all open sockets
Parameters:
    pData (IN) pointer to the communication server database
*****************************************************************************/

void T5CSSock_CloseAllSockets (T5PTR_CS pData)
{
    T5_WORD i;

    for (i=0; i<T5MAX_PORT; i++)
    {
        if ((pData->csPort[i].wFlags & T5CS_SOCKET) != 0
            && pData->csPort[i].sock != T5_INVSOCKET)
        {
            T5Socket_CloseSocket (pData->csPort[i].sock);
            pData->csPort[i].sock = T5_INVSOCKET;
            pData->csPort[i].dwTimeStamp = 0L;
        }
    }
    if (pData->sock != T5_INVSOCKET)
        T5Socket_CloseSocket (pData->sock);
}

/*****************************************************************************
T5CSSock_ActivateAllSockets
Send and receive data on all client sockets
Parameters:
    pData (IN) pointer to the communication server database
*****************************************************************************/

void T5CSSock_ActivateAllSockets (T5PTR_CS pData)
{
    T5_WORD i;

    for (i=0; i<T5MAX_PORT; i++)
    {
#ifdef T5DEF_CSM5
        if ((pData->csPort[i].wFlags & T5CS_M5) != 0)
        {
            if ((pData->csPort[i].wFlags & T5CS_SOCKET) != 0
                && pData->csPort[i].sock != T5_INVSOCKET)
            {
                T5CSM5_Activate (&(pData->csPort[i]), pData->dwStamp);
            }
        }
        else
#endif /*T5DEF_CSM5*/
        {
            if ((pData->csPort[i].wFlags & T5CS_SOCKET) != 0
                && pData->csPort[i].sock != T5_INVSOCKET)
                _T5CSSock_Send (&(pData->csPort[i]));
            if ((pData->csPort[i].wFlags & T5CS_SOCKET) != 0
                && pData->csPort[i].sock != T5_INVSOCKET)
                _T5CSSock_Receive (&(pData->csPort[i]));
        }
    }
}

/*****************************************************************************
T5CSSock_CheckSilentSockets
Check and close possible silent sockets
Parameters:
    pData (IN) pointer to the communication server database
    dwTimeStamp (IN) current time stamp
    dwTimeout (IN) timeout
*****************************************************************************/

void T5CSSock_CheckSilentSockets (T5PTR_CS pData, T5_DWORD dwTimeStamp)
{
    /* default 10 seconds */
    T5CSSock_CheckSilentSocketsEx (pData, dwTimeStamp, 10000L);
}

void T5CSSock_CheckSilentSocketsEx (T5PTR_CS pData, T5_DWORD dwTimeStamp,
                                    T5_DWORD dwTimeout)
{
    T5_WORD i;

    for (i=0; i<T5MAX_PORT; i++)
    {
        if ((pData->csPort[i].wFlags & T5CS_SOCKET) != 0
            && pData->csPort[i].sock != T5_INVSOCKET)
        {
            if (pData->csPort[i].dwTimeStamp == 0L)
                pData->csPort[i].dwTimeStamp = dwTimeStamp;
            else if (T5_BOUNDTIME(dwTimeStamp - pData->csPort[i].dwTimeStamp) > dwTimeout)
            {
                T5Socket_CloseSocket (pData->csPort[i].sock);
                T5_MEMSET (&(pData->csPort[i]), 0, sizeof (T5STR_CSPORT));
            }
        }
    }
}

/*****************************************************************************
_T5CSSock_Send
Send data on a client socket (if any)
Parameters:
    pPort (IN) descriptor of the client connection
*****************************************************************************/

static void _T5CSSock_Send (T5PTR_CSPORT pPort)
{
    T5_BOOL bFail;
    T5_WORD wRec;
    T5_PTBYTE pData;

    /* return if nothing to send */
    if ((pPort->wFlags & (T5CS_DATAOUT | T5CS_SENDING)) == 0)
        return;
    /* send pending data */
    pData = &(pPort->bfOut[pPort->wPakSizeOut - pPort->wSizeOut]);
    wRec = T5Socket_Send (pPort->sock, pPort->wSizeOut, pData, &bFail);
    pPort->wSizeOut -= wRec;
    if (wRec)
        pPort->dwTimeStamp = 0L;
    /* close socket on error */
    if (bFail)
    {
        T5Socket_CloseSocket (pPort->sock);
        T5_MEMSET (pPort, 0, sizeof (T5STR_CSPORT));
    }
    /* if alive update flags */
    else if (pPort->wSizeOut == 0)
    {
        pPort->wFlags &= ~T5CS_DATAOUT;
        pPort->wFlags &= ~T5CS_SENDING;
        pPort->wPakSizeOut = 0;
    }
    else
        pPort->wFlags |= T5CS_SENDING;
}

/*****************************************************************************
_T5CSSock_Receive
Receive data on a client socket (if any)
Parameters:
    pPort (IN) descriptor of the client connection
*****************************************************************************/

#define T5_ISMBAP(b)    (!T5CS_ISHEAD(b) || (b[2]==0 && b[3]==0))

static void _T5CSSock_Receive (T5PTR_CSPORT pPort)
{
    T5_BOOL bFail;
    T5_WORD wRec, wExpect, wSize;
    T5_PTBYTE pData;

    /* return if there is already a full frame received */
    if ((pPort->wFlags & T5CS_DATAIN) != 0)
        return;

    pData = &(pPort->bfIn[pPort->wSizeIn]);

    if (pPort->wPakSizeIn == 0)
        wExpect = 4 - pPort->wSizeIn;
    else
        wExpect = pPort->wPakSizeIn - pPort->wSizeIn;

    wRec = T5Socket_Receive (pPort->sock, wExpect, pData, &bFail);
    /* close socket on error */
    if (bFail)
    {
        T5Socket_CloseSocket (pPort->sock);
        T5_MEMSET (pPort, 0, sizeof (T5STR_CSPORT));
        return;
    }
    /* return if nothing received */
    if (!wRec) return;
    pPort->dwTimeStamp = 0L;
    /* check and store received data */
    pPort->wSizeIn += wRec;
    /* check header */
    if (pPort->wSizeIn == 4)
    {
        /* MODBUS ? */
        if (T5_ISMBAP(pPort->bfIn))
        {
#ifdef T5DEF_MODBUSIOS
            if (pPort->bfIn[2] != 0 || pPort->bfIn[3] != 0)
                pPort->wSizeIn = 0;
            else
            {
                pPort->wPakSizeIn = 6;
                pPort->wFlags |= T5CS_RECEIVING;
            }
#else /*T5DEF_MODBUSIOS*/
            pPort->wSizeIn = 0;
#endif /*T5DEF_MODBUSIOS*/
        }
        /* T5 protocol */
        else
        {
            T5_COPYFRAMEWORD(&wSize, &(pPort->bfIn[2]));
            if (wSize == 0 || wSize > T5MAX_FRAME)
            {
                T5_PRINTF ("Invalid frame size - endianness maybe incorrect");
                pPort->wSizeIn = 0;
            }
            else
            {
                pPort->wPakSizeIn = wSize + 4; /* store header also */
                pPort->wFlags |= T5CS_RECEIVING;
            }
        }
    }
    /* check MODBUS frame length */
    else if (pPort->wSizeIn == pPort->wPakSizeIn
        && pPort->wSizeIn == 6 && T5_ISMBAP(pPort->bfIn))
    {
        T5_COPYFRAMEWORD(&wSize, &(pPort->bfIn[4]));
        if (wSize == 0 || wSize > 256)
            pPort->wSizeIn = 0;
        else
        {
            pPort->wPakSizeIn = wSize + 6; /* store header also */
            pPort->wFlags |= T5CS_RECEIVING;
        }
    }
    /* complete */
    else if (pPort->wSizeIn == pPort->wPakSizeIn)
    {
        pPort->wFlags &= ~T5CS_RECEIVING;
        pPort->wFlags |= T5CS_DATAIN;
    }
    else /* data expected */
    {
        pPort->wFlags |= T5CS_RECEIVING;
    }
}

/****************************************************************************/

#endif /*T5DEF_ETHERNET*/

/* eof **********************************************************************/
