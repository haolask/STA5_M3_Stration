/*****************************************************************************
T5CSM5.c :   communication server - mixed M5/T5 protocols

DO NOT ALTER THIS !

(c) COPALP 2011
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_CSM5

/****************************************************************************/

static void _T5CSM5_Send (T5PTR_CSPORT pPort);
static void _T5CSM5_Receive (T5PTR_CSPORT pPort, T5_DWORD dwTimeStamp);
static void _T5CSMS_ProcessQuestion (T5PTR_CSPORT pPort);
static void _T5CSMS_AbortReceiving (T5PTR_CSPORT pPort);
static T5_BOOL _T5CSM5_RcvByte (T5PTR_CSPORT pPort, T5_PTBYTE pb);
static T5_WORD _T5CSM5_SndBytes (T5PTR_CSPORT pPort, T5_WORD wLen, T5_PTBYTE pb);

/*****************************************************************************
T5CSM5_Activate
Main state machine
Parameters:
    pPort (IN) communication port
    dwTimeStamp (IN) current time stamp
*****************************************************************************/

void T5CSM5_Activate (T5PTR_CSPORT pPort, T5_DWORD dwTimeStamp)
{
    if ((pPort->wFlags & (T5CS_DATAOUT | T5CS_SENDING)) != 0)
        _T5CSM5_Send (pPort);
    if ((pPort->wFlags & T5CS_DATAIN) == 0)
        _T5CSM5_Receive (pPort, dwTimeStamp);
}

/*****************************************************************************
_T5CSM5_Send
State machine for sending
Parameters:
    pPort (IN) communication port
*****************************************************************************/

static void _T5CSM5_Send (T5PTR_CSPORT pPort)
{
    T5_WORD wCrc;

    if ((pPort->wFlags & T5CS_DATAOUT) != 0)
    {
        if (pPort->bM5Req)
        {
            T5_MEMMOVE(pPort->bfOut+4, pPort->bfOut+2, pPort->wPakSizeOut-2);
            pPort->bfOut[2] = pPort->bReqID;
            pPort->bfOut[3] = (T5_BYTE)(pPort->wTaskNo);
            pPort->bfOut[0] = 'M';
            pPort->wPakSizeOut += 2;
        }

        if (pPort->bM5Req || (pPort->wFlags & T5CS_SERIAL) != 0)
        {
            wCrc = T5Tools_Crc16 (pPort->bfOut, pPort->wPakSizeOut);
            T5_COPYFRAMEWORD (pPort->bfOut + pPort->wPakSizeOut, &wCrc);
            pPort->wPakSizeOut += 2;
        }
        pPort->wFlags &= ~T5CS_DATAOUT;
        pPort->wFlags |= T5CS_SENDING;
        pPort->wSizeOut = 0;
    }

    if ((pPort->wFlags & T5CS_SENDING) != 0)
    {
        pPort->wSizeOut += _T5CSM5_SndBytes (
            pPort,
            (pPort->wPakSizeOut - pPort->wSizeOut),
            pPort->bfOut + pPort->wSizeOut);
        if (pPort->wSizeOut == pPort->wPakSizeOut)
        {
            pPort->wFlags &= ~T5CS_DATAOUT;
            pPort->wFlags &= ~T5CS_SENDING;
            pPort->wPakSizeOut = 0;
            pPort->wSizeOut = 0;
        }
    }
}

/*****************************************************************************
_T5CSM5_Receive
State machine for receiving
Parameters:
    pPort (IN) communication port
    dwTimeStamp (IN) current time stamp
*****************************************************************************/

static void _T5CSM5_Receive (T5PTR_CSPORT pPort, T5_DWORD dwTimeStamp)
{
    T5_BYTE b;

    while (_T5CSM5_RcvByte (pPort, &b))
    {
        switch (pPort->wSizeIn)
        {
        /* header - 1rst char */
        case 0 :
            if (b == 'T' || b == 'M')
            {
                pPort->bfIn[0] = b;
                pPort->wSizeIn += 1;
                pPort->bM5Req = (b == 'M');
                pPort->wFlags |= T5CS_RECEIVING;
                pPort->dwTimeStamp = dwTimeStamp;
            }
            break;
        /* header - 2nd char */
        case 1 :
            if (b == '5')
            {
                pPort->bfIn[1] = b;
                pPort->wSizeIn += 1;
            }
            else
                _T5CSMS_AbortReceiving (pPort);
            break;
        case 2 :
            pPort->bfIn[2] = b;
            pPort->wSizeIn += 1;
            break;
        case 3 :
            pPort->bfIn[3] = b;
            pPort->wSizeIn += 1;
            if (pPort->bM5Req == FALSE)
            {
                T5_COPYFRAMEWORD (&(pPort->wPakSizeIn), pPort->bfIn + 2);
                if (pPort->wPakSizeIn > T5MAX_FRAME)
                    _T5CSMS_AbortReceiving (pPort);
                else
                {
                    pPort->wPakSizeIn += 4; /* header */
                    if ((pPort->wFlags & T5CS_SERIAL) != 0)
                        pPort->wPakSizeIn += 2; /* CRC */
                }
            }
            break;
        case 4 :
            pPort->bfIn[4] = b;
            pPort->wSizeIn += 1;
            /* can be a complete T5 frame with only 1 byte data */
            if (pPort->bM5Req == FALSE && pPort->wSizeIn == pPort->wPakSizeIn)
            {
                _T5CSMS_ProcessQuestion (pPort);
                return;
            }
            break;
        case 5 :
            pPort->bfIn[5] = b;
            pPort->wSizeIn += 1;
            if (pPort->bM5Req == TRUE)
            {
                T5_COPYFRAMEWORD (&(pPort->wPakSizeIn), pPort->bfIn + 4);
                if (pPort->wPakSizeIn > T5MAX_FRAME)
                    _T5CSMS_AbortReceiving (pPort);
                else
                    pPort->wPakSizeIn += (6 + 2); /* header + CRC */
            }
            /* can also be a complete T5 frame with only 2 bytes data */
            else if (pPort->wSizeIn == pPort->wPakSizeIn)
            {
                _T5CSMS_ProcessQuestion (pPort);
                return;
            }
            break;
        default :
            pPort->bfIn[pPort->wSizeIn] = b;
            pPort->wSizeIn += 1;
            if (pPort->wSizeIn == pPort->wPakSizeIn)
            {
                _T5CSMS_ProcessQuestion (pPort);
                return;
            }
            break;
        }
    }
    if ((pPort->wFlags & T5CS_RECEIVING) != 0
        && T5_BOUNDTIME (dwTimeStamp - pPort->dwTimeStamp) > 3000)
    {
        _T5CSMS_AbortReceiving (pPort);
    }
}

/*****************************************************************************
_T5CSMS_AbortReceiving
Abort a reception
Parameters:
    pPort (IN) communication port
*****************************************************************************/

static void _T5CSMS_AbortReceiving (T5PTR_CSPORT pPort)
{
    pPort->wSizeIn = 0;
    pPort->wPakSizeIn = 0;
    pPort->bM5Req = FALSE;
    pPort->wFlags &= ~T5CS_RECEIVING;
}

/*****************************************************************************
_T5CSMS_ProcessQuestion
Manage a received request
Parameters:
    pPort (IN) communication port
*****************************************************************************/

static void _T5CSMS_ProcessQuestion (T5PTR_CSPORT pPort)
{
    T5_WORD wLen, wCrc;

    if (pPort->bM5Req || (pPort->wFlags & T5CS_SERIAL) != 0)
    {
        wLen = pPort->wPakSizeIn-2;
        T5_COPYFRAMEWORD (&wCrc, pPort->bfIn + wLen);
        if (wCrc != T5Tools_Crc16 (pPort->bfIn, wLen))
        {
            _T5CSMS_AbortReceiving (pPort);
            return;
        }
    }

    if (pPort->bM5Req)
    {
        pPort->bReqID = pPort->bfIn[2];
        pPort->wTaskNo = (T5_WORD)(pPort->bfIn[3]) & 0xff;
        pPort->bfIn[0] = 'T';
        T5_MEMMOVE(pPort->bfIn+2, pPort->bfIn+4, pPort->wPakSizeIn-4);
        pPort->wPakSizeIn -= 2;
    }

    pPort->wFlags &= ~T5CS_RECEIVING;
    pPort->wFlags |= T5CS_DATAIN;
}

/*****************************************************************************
_T5CSM5_RcvByte
Reception of a byte (TCP or serial)
Parameters:
    pPort (IN) communication port
    pb (OUT) received byte
Return: TRUE if byte received
*****************************************************************************/

static T5_BOOL _T5CSM5_RcvByte (T5PTR_CSPORT pPort, T5_PTBYTE pb)
{
    T5_BOOL bFail;
    T5_WORD nb;

#ifdef T5DEF_SERIAL
    if ((pPort->wFlags & T5CS_SERIAL) != 0)
    {
        if (T5Serial_Receive (&(pPort->ser), 1, pb))
            return TRUE;
        return FALSE;
    }
#endif /*T5DEF_SERIAL*/

#ifdef T5DEF_ETHERNET
    if ((pPort->wFlags & T5CS_SOCKET) != 0)
    {
        nb = T5Socket_Receive (pPort->sock, 1, pb, &bFail);
        if (bFail)
        {
            T5Socket_CloseSocket (pPort->sock);
            T5_MEMSET (pPort, 0, sizeof (T5STR_CSPORT));
            return FALSE;
        }
        return (nb == 1);
    }
#endif /*T5DEF_ETHERNET*/

    return FALSE;
}

/*****************************************************************************
_T5CSM5_SndBytes
Send bytes (TCP or serial)
Parameters:
    pPort (IN) communication port
    wLen (IN) number of bytes to send
    pb (IN) bytes to send
Return: number of sent bytes
*****************************************************************************/

static T5_WORD _T5CSM5_SndBytes (T5PTR_CSPORT pPort, T5_WORD wLen, T5_PTBYTE pb)
{
    T5_BOOL bFail;
    T5_WORD nb;

#ifdef T5DEF_SERIAL
    if ((pPort->wFlags & T5CS_SERIAL) != 0)
    {
        return T5Serial_Send (&(pPort->ser), wLen, pb);
    }
#endif /*T5DEF_SERIAL*/

#ifdef T5DEF_ETHERNET
    if ((pPort->wFlags & T5CS_SOCKET) != 0)
    {
        nb = T5Socket_Send (pPort->sock, wLen, pb, &bFail);
        if (bFail)
        {
            T5Socket_CloseSocket (pPort->sock);
            T5_MEMSET (pPort, 0, sizeof (T5STR_CSPORT));
            return 0;
        }
        return nb;
    }
#endif /*T5DEF_ETHERNET*/

    return 0;
}

/****************************************************************************/

#endif /*T5DEF_CSM5*/

/* eof **********************************************************************/
