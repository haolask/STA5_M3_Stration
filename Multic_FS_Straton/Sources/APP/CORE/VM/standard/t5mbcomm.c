/*****************************************************************************
T5MBcomm.c : MODBUS communication

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_MODBUSIOS

/****************************************************************************/
/* static services */

#ifdef T5DEF_SERIAL
static void _T5MBComm_ReceiveSerial (T5PTR_MBCLIENT pClient);
#endif /*T5DEF_SERIAL*/

#ifdef T5DEF_ETHERNET
static void _T5MBComm_ReceiveTCP (T5PTR_MBCLIENT pClient);
#endif /*T5DEF_ETHERNET*/

#ifdef T5DEF_UDP
static void _T5MBComm_ReceiveUDP (T5PTR_MBCLIENT pClient);
#endif /*T5DEF_UDP*/

/****************************************************************************/

#define _ISUDP(c)     (T5_MEMCMP(pClient->pDef->szAddr, "UDP:", 4) == 0)

/*****************************************************************************
T5MBComm_Open
Open a MODBUS communication port
Parameters:
    pClient (IN) port descriptor
Return: new status of the port
*****************************************************************************/

T5_WORD T5MBComm_Open (T5PTR_MBCLIENT pClient)
{
    T5_WORD wStatus;

    wStatus = T5MB_ERROR;

    if (_ISUDP(pClient))
    {
        pClient->wFlags |= T5MBDEF_UDP;
#ifdef T5DEF_UDP
        if (T5Socket_UdpCreate (0, &(pClient->sock)) == T5RET_OK)
            wStatus = T5MB_READY;
#endif /*T5DEF_UDP*/
    }
    else if (pClient->wDriver == T5MB_RS232)
    {
#ifdef T5DEF_SERIAL
        T5Serial_Initialize (&(pClient->ser));
#ifdef T5DEF_SERIALINFO
            T5Serial_Info (&(pClient->ser), T5SERINFO_P_MBM);
#endif /*T5DEF_SERIALINFO*/
            if (T5SerialAsc_Open (&(pClient->ser), pClient->pDef->szAddr, &(pClient->dwAsc)))
        {
#ifdef T5DEF_SERIALINFO
            T5Serial_Info (&(pClient->ser), T5SERINFO_MBM);
#endif /*T5DEF_SERIALINFO*/

            wStatus = T5MB_READY;
        }
#endif /*T5DEF_SERIAL*/
    }
    else
    {
#ifdef T5DEF_ETHERNET
#ifdef T5DEF_TCPCLIENT
        T5_BOOL bWait;

        if (T5Socket_CreateConnectedSocket (pClient->pDef->szAddr,
                                            pClient->pDef->wPort,
                                            &(pClient->sock),
                                            &bWait, NULL) == T5RET_OK)
        {
            if (bWait)
                wStatus = T5MB_CONNECTING;
            else
                wStatus = T5MB_READY;
        }
#endif /*T5DEF_TCPCLIENT*/
#endif /*T5DEF_ETHERNET*/
    }
    pClient->frame.wStatus = wStatus;
    if (wStatus == T5MB_ERROR)
        pClient->frame.wError = T5MBERR_COMM;
    else if (wStatus != T5MB_CONNECTING)
        pClient->frame.wError = 0;
    return wStatus;
}

/*****************************************************************************
T5MBComm_Close
Close a MODBUS communication port
Parameters:
    pClient (IN) port descriptor
*****************************************************************************/

void T5MBComm_Close (T5PTR_MBCLIENT pClient)
{
    if ((pClient->wFlags & T5MBDEF_UDP) != 0)
    {
#ifdef T5DEF_UDP
        if (pClient->sock != T5_INVSOCKET)
        {
            T5Socket_CloseSocket (pClient->sock);
            pClient->sock = T5_INVSOCKET;
        }
#endif /*T5DEF_UDP*/
    }
    else if (pClient->wDriver == T5MB_RS232)
    {
#ifdef T5DEF_SERIAL
        T5SerialAsc_Close (&(pClient->ser), &(pClient->dwAsc));
        T5Serial_Initialize (&(pClient->ser));
#endif /*T5DEF_SERIAL*/
    }
    else
    {
#ifdef T5DEF_ETHERNET
        if (pClient->sock != T5_INVSOCKET)
        {
            T5Socket_CloseSocket (pClient->sock);
            pClient->sock = T5_INVSOCKET;
        }
#endif /*T5DEF_ETHERNET*/
    }
}

/*****************************************************************************
T5MBComm_CheckPendingConnect
Check pending connection on a MODBUS communication port
Parameters:
    pClient (IN) port descriptor
*****************************************************************************/

void T5MBComm_CheckPendingConnect (T5PTR_MBCLIENT pClient)
{
#ifdef T5DEF_ETHERNET
#ifdef T5DEF_TCPCLIENT
    T5_BOOL bFail;

    if (pClient->wDriver == T5MB_RS232)
        return;

    if (T5Socket_CheckPendingConnect (pClient->sock, &bFail) == T5RET_OK)
    {
        pClient->frame.wStatus = T5MB_READY;
        pClient->frame.wError = 0;
    }
    else if (bFail)
    {
        T5Socket_CloseSocket (pClient->sock);
        pClient->sock = T5_INVSOCKET;
        pClient->frame.wStatus = T5MB_ERROR;
        pClient->frame.wError = T5MBERR_COMM;
    }
#endif /*T5DEF_TCPCLIENT*/
#endif /*T5DEF_ETHERNET*/
}

/*****************************************************************************
T5MBComm_Send
Send pending data on a MODBUS communication port
Parameters:
    pClient (IN) port descriptor
*****************************************************************************/

void T5MBComm_Send (T5PTR_MBCLIENT pClient)
{
    T5_WORD wNb;
    T5PTR_MBFRAME pFrame;
#ifdef T5DEF_UDP
    T5_UDPADDR addr;
    T5_PTCHAR szAddr;
#endif /*T5DEF_UDP*/
#ifdef T5DEF_ETHERNET
    T5_BOOL bFail;
    T5_BYTE ir, ardummy[32];
#endif /*T5DEF_ETHERNET*/

    pFrame = &(pClient->frame);
    if (_ISUDP(pClient))
    {
#ifdef T5DEF_UDP
        if (pClient->sock != T5_INVSOCKET && pFrame->wSize)
        {
            for (ir = 0; ir<32 && T5Socket_UdpRecvFrom (pClient->sock, ardummy, sizeof (ardummy), &addr); ir++)
                ; /*skip*/

            szAddr = pClient->pDef->szAddr;
            szAddr += 4; /* skip "UDP:" */
            T5Socket_UdpAddrMake (&addr, szAddr, pClient->pDef->wPort);
            if (T5Socket_UdpSendTo (pClient->sock, &addr,
                                    pFrame->buffer, pFrame->wSize) == T5RET_OK)
            {
                pFrame->wLenCur = pFrame->wSize;
                pFrame->wStatus = T5MB_SENT;
                pFrame->wError = 0;
            }
        }
#endif /*T5DEF_UDP*/
    }
    else if (pClient->wDriver == T5MB_RS232)
    {
#ifdef T5DEF_SERIAL
        if (T5Serial_IsValid (&(pClient->ser)))
        {
            T5SerialAsc_Flush (&(pClient->ser), &(pClient->dwAsc));
            wNb = T5SerialAsc_Send (&(pClient->ser),
                                    (T5_WORD)(pFrame->wSize - pFrame->wLenCur),
                                    pFrame->buffer + pFrame->wLenCur,
                                    &(pClient->dwAsc));
            pFrame->wLenCur += wNb;
            if (pFrame->wLenCur == pFrame->wSize)
            {
                pFrame->wStatus = T5MB_SENT;
                pFrame->wError = 0;

#ifdef T5DEF_SERIALINFO
                T5Serial_Info (&(pClient->ser), T5SERINFO_SND);
#endif /*T5DEF_SERIALINFO*/
            }
        }
#endif /*T5DEF_SERIAL*/
    }
    else
    {
#ifdef T5DEF_ETHERNET
        if (pClient->sock != T5_INVSOCKET)
        {
            for (ir=0; ir<32 && T5Socket_Receive (pClient->sock, sizeof (ardummy),
                                                  ardummy, &bFail); ir++)
                ; /*skip*/

            wNb = T5Socket_Send (pClient->sock,
                                 (T5_WORD)(pFrame->wSize - pFrame->wLenCur),
                                 pFrame->buffer + pFrame->wLenCur, &bFail);
            if (bFail)
            {
                T5Socket_CloseSocket (pClient->sock);
                pClient->sock = T5_INVSOCKET;
                pFrame->wStatus = T5MB_ERROR;
                pFrame->wError = T5MBERR_COMM;
            }
            else
            {
                pFrame->wLenCur += wNb;
                if (pFrame->wLenCur == pFrame->wSize)
                {
                    pFrame->wStatus = T5MB_SENT;
                    pFrame->wError = 0;
                }
            }
        }
#endif /*T5DEF_ETHERNET*/
    }
}

/*****************************************************************************
T5MBComm_Receive
Receive pending data on a MODBUS communication port
Check for reception time-out
Parameters:
    pClient (IN) port descriptor
    dwTimeStamp (IN) current time stamp
*****************************************************************************/

void T5MBComm_Receive (T5PTR_MBCLIENT pClient, T5_DWORD dwTimeStamp)
{
    if (T5_BOUNDTIME(dwTimeStamp - pClient->frame.dwDate)
        >= (T5_DWORD)(pClient->frame.dwTimeOut))
    {
#ifdef T5DEF_ETHERNET
#ifdef T5DEF_MBCTP_TIMEOUTISFAIL
        if (pClient->wDriver == T5MB_TCP && !_ISUDP(pClient))
        {
            T5Socket_CloseSocket (pClient->sock);
            pClient->sock = T5_INVSOCKET;
            pClient->frame.wStatus = T5MB_ERROR;
        }
        else
#endif /*T5DEF_MBCTP_TIMEOUTISFAIL*/
#endif /*T5DEF_ETHERNET*/
        {
            pClient->frame.wStatus = T5MB_RECEIVED;
        }
        pClient->frame.wError = T5MBERR_TIMEOUT;
        return;
    }

    if (_ISUDP(pClient))
    {
#ifdef T5DEF_UDP
        _T5MBComm_ReceiveUDP (pClient);
#endif /*T5DEF_UDP*/
    }
    else if (pClient->wDriver == T5MB_RS232)
    {
#ifdef T5DEF_SERIAL
        _T5MBComm_ReceiveSerial (pClient);
#endif /*T5DEF_SERIAL*/
    }
    else
    {
#ifdef T5DEF_ETHERNET
        _T5MBComm_ReceiveTCP (pClient);
#endif /*T5DEF_ETHERNET*/
    }
}

/*****************************************************************************
_T5MBComm_ReceiveTCP
Receive pending data on a MODBUS/TCP communication port
Parameters:
    pClient (IN) port descriptor
*****************************************************************************/

#ifdef T5DEF_ETHERNET

static void _T5MBComm_ReceiveTCP (T5PTR_MBCLIENT pClient)
{
    T5PTR_MBFRAME pFrame;
    T5_WORD wNb, wCnt;
    T5_BOOL bFail;

    pFrame = &(pClient->frame);

    if (pFrame->wSize == 0)
    {
        wNb = T5Socket_Receive (pClient->sock,
                                (T5_WORD)(6 - pFrame->wLenCur),
                                pFrame->buffer + pFrame->wLenCur, &bFail);
        if (bFail)
        {
            T5Socket_CloseSocket (pClient->sock);
            pClient->sock = T5_INVSOCKET;
            pFrame->wStatus = T5MB_ERROR;
            pFrame->wError = T5MBERR_COMM;
        }
        else
        {
            pFrame->wLenCur += wNb;
            if (pFrame->wLenCur == 6)
            {
                T5_COPYFRAMEWORD(&wNb, pFrame->buffer + 4);
                if (wNb > 256)
                    pFrame->wLenCur = 0;
                else
                    pFrame->wSize = wNb + 6;
            }
        }
    }
    if (pFrame->wSize != 0)
    {
        wNb = T5Socket_Receive (pClient->sock,
                                (T5_WORD)(pFrame->wSize - pFrame->wLenCur),
                                pFrame->buffer + pFrame->wLenCur, &bFail);
        if (bFail)
        {
            T5Socket_CloseSocket (pClient->sock);
            pClient->sock = T5_INVSOCKET;
            pFrame->wStatus = T5MB_ERROR;
            pFrame->wError = T5MBERR_COMM;
        }
        else
        {
            pFrame->wLenCur += wNb;
            if (pFrame->wLenCur == pFrame->wSize)
            {
                T5_COPYFRAMEWORD(&wCnt, pFrame->buffer);
                if (wCnt != pFrame->wFrmCount)
                {
                    pFrame->wLenCur = 0;
                    pFrame->wSize = 0;
                    /*
                    T5_PRINTF ("ERROR rcv frame %u => %u", pFrame->wFrmCount, wCnt);
                    */
                }
                else
                {
                    pFrame->wStatus = T5MB_RECEIVED;
                    pFrame->wError = 0;
                }
            }
        }
    }
}

#endif /*T5DEF_ETHERNET*/

/*****************************************************************************
_T5MBComm_ReceiveSerial
Receive pending data on a serial MODBUS communication port
Parameters:
    pClient (IN) port descriptor
*****************************************************************************/

#ifdef T5DEF_SERIAL

static void _T5MBComm_ReceiveSerial (T5PTR_MBCLIENT pClient)
{
    T5PTR_MBFRAME pFrame;
    T5_WORD wNb;
    T5_BYTE bFC;
    T5_BOOL bFail;

    pFrame = &(pClient->frame);
    bFail = FALSE;
    /* slave number, function code and 1rst data byte (may be length) */
    if (pFrame->wSize == 0)
    {
        wNb = T5SerialAsc_Receive (&(pClient->ser), (T5_WORD)(3 - pFrame->wLenCur),
                                   pFrame->buffer + pFrame->wLenCur, &(pClient->dwAsc));
        pFrame->wLenCur += wNb;
        if (pFrame->wLenCur == 3)
        {
            bFC = pFrame->buffer[1];
            switch (bFC)
            {
            case T5MBF_RIB :
            case T5MBF_RCB :
            case T5MBF_RIW :
            case T5MBF_RHW :
                /* read requests: sl+fc+nb+data(nb)+crc(2) */
                pFrame->wSize = ((T5_WORD)(pFrame->buffer[2]) & 0x00ff) + 5;
                if (_T5MBA_ISACTIVE(&(pClient->dwAsc)))
                {
                    pFrame->wSize -= 1;
                    pFrame->bLRC = TRUE;
                }
                break;
            case T5MBF_W1B :
            case T5MBF_WNB :
            case T5MBF_W1W :
            case T5MBF_WNW :
                /* write requests: sl+fc+data(4)+crc(2) */
                pFrame->wSize = 8;
                if (_T5MBA_ISACTIVE(&(pClient->dwAsc)))
                {
                    pFrame->wSize -= 1;
                    pFrame->bLRC = TRUE;
                }
                break;
            default :
                /* exceptions: sl+fc+code+crc(2) */
                if ((bFC & 0x80) != 0)
                {
                    pFrame->wSize = 5;
                    if (_T5MBA_ISACTIVE(&(pClient->dwAsc)))
                    {
                        pFrame->wSize -= 1;
                        pFrame->bLRC = TRUE;
                    }
                }
                /* unknown or not supported function codes: skip */
                else
                    pFrame->wLenCur = 0;
                    /* bFail = TRUE; */
                break;
            }
        }
    }
    /* rest of the frame including CRC */
    if (pFrame->wSize != 0)
    {
        wNb = T5SerialAsc_Receive (&(pClient->ser),
                                   (T5_WORD)(pFrame->wSize - pFrame->wLenCur),
                                   pFrame->buffer + pFrame->wLenCur,
                                   &(pClient->dwAsc));
        pFrame->wLenCur += wNb;
        if (pFrame->wLenCur == pFrame->wSize)
        {
            pFrame->wStatus = T5MB_RECEIVED;
            pFrame->wError = 0;

#ifdef T5DEF_SERIALINFO
            T5Serial_Info (&(pClient->ser), T5SERINFO_RCV);
#endif /*T5DEF_SERIALINFO*/
        }
    }
    /* flush reception buffer if error */
    if (bFail)
    {
        pFrame->wStatus = T5MB_ERROR;
        pFrame->wError = T5MBERR_COMM;
        T5SerialAsc_Flush (&(pClient->ser), &(pClient->dwAsc));
    }
}

#endif /*T5DEF_SERIAL*/

/*****************************************************************************
_T5MBComm_ReceiveUDP
reception on UDP
Parameters:
    pClient (IN) channel object
*****************************************************************************/

#ifdef T5DEF_UDP

static void _T5MBComm_ReceiveUDP (T5PTR_MBCLIENT pClient)
{
    T5_UDPADDR addr;
    T5PTR_MBFRAME pFrame;
    T5_WORD wNb;

    pFrame = &(pClient->frame);
    wNb = T5Socket_UdpRecvFrom (pClient->sock, pFrame->buffer,
                                sizeof (pFrame->buffer), &addr);
    if (wNb != 0)
    {
        pFrame->wLenCur = pFrame->wSize = wNb;
        pFrame->wStatus = T5MB_RECEIVED;
        pFrame->wError = 0;
    }
}

#endif /*T5DEF_UDP*/

/****************************************************************************/
/* for MODBUS ASCII */

#ifdef T5DEF_SERIAL

T5_BOOL T5SerialAsc_Open (T5_PTSERIAL pSerial, T5_PTCHAR szConfig,
                          T5_PTDWORD pdwStatus)
{
    T5_BOOL bRet, bActive;

    if (T5_MEMCMP (szConfig, "ASCII:", 6) == 0)
    {
        szConfig += 6;
        bActive = TRUE;
    }
    else
    {
        *pdwStatus = 0L;
        bActive = FALSE;
    }
    bRet = T5Serial_Open (pSerial, szConfig);
    if (bRet && bActive)
        *pdwStatus = _T5MBA_ACTIVE;
    return bRet;
}

void T5SerialAsc_Close (T5_PTSERIAL pSerial, T5_PTDWORD pdwStatus)
{
    *pdwStatus = 0L;
    T5Serial_Close (pSerial);
}

void T5SerialAsc_Flush (T5_PTSERIAL pSerial, T5_PTDWORD pdwStatus)
{
    T5_BYTE b;

    _T5MBA_RESETALL (pdwStatus);
    while (T5Serial_Receive (pSerial, 1, &b))
        /*skip*/ ;
}

T5_BYTE _FmtNibble (T5_BYTE b)
{
    if (b < 10)
        return b + '0';
    if (b < 16)
        return b - 10 + 'A';
    return 255; /* error */
}

T5_WORD T5SerialAsc_Send (T5_PTSERIAL pSerial, T5_WORD wSize,
                          T5_PTR pData, T5_PTDWORD pdwStatus)
{
    T5_PTBYTE pb;
    T5_WORD wSent;
    T5_BYTE nibble, lrc;

    if (!_T5MBA_ISACTIVE(pdwStatus) || wSize < 2)
        return T5Serial_Send (pSerial, wSize, pData);

    wSent=0;
    lrc = 0;
    pb = (T5_PTBYTE)pData;

    nibble = ':';
    T5Serial_Send (pSerial, 1, &nibble);

    wSize -= 2;
    while (wSize)
    {
        /* send highest nibbel if not already done */
        if (!_T5MBA_ISHALFSENT (pdwStatus))
        {
            nibble = _FmtNibble (((*pb) >> 4) & 0x0f);
            if (T5Serial_Send (pSerial, 1, &nibble) != 1)
                return wSent;
        }

        /* try to send second nibble */
        nibble = _FmtNibble ((*pb) & 0x0f);
        if (T5Serial_Send (pSerial, 1, &nibble) != 1)
        {
            _T5MBA_SETHALFSENT (pdwStatus); /* remember 1rst is sent */
            return wSent;
        }
        lrc += *pb;

        /* 2 nibbles were sent */
        _T5MBA_RESETHALFSENT (pdwStatus);
        /* next one */
        wSent += 1;
        wSize -= 1;
        pb++;
    }

    lrc = (T5_BYTE)(-((T5_CHAR)lrc));

    nibble = _FmtNibble (((lrc) >> 4) & 0x0f);
    T5Serial_Send (pSerial, 1, &nibble);
    nibble = _FmtNibble (lrc & 0x0f);
    T5Serial_Send (pSerial, 1, &nibble);

    nibble = '\r';
    T5Serial_Send (pSerial, 1, &nibble);
    nibble = '\n';
    T5Serial_Send (pSerial, 1, &nibble);

    return wSent + 2;
}

T5_BYTE _ParseNibble (T5_BYTE b)
{
    if (b >= '0' && b <= '9')
        return b - '0';
    if (b >= 'A' && b <= 'F')
        return b + 10 - 'A';
    if (b >= 'a' && b <= 'f')
        return b + 10 - 'a';
    return 255; /* error */
}

T5_BOOL _ReceiveNibble (T5_PTSERIAL pSerial, T5_PTBYTE pData,
                        T5_PTDWORD pdwStatus)
{
    T5_UNUSED(pdwStatus);
    while (T5Serial_Receive (pSerial, 1, pData) == 1)
    {
        *pData = _ParseNibble (*pData);
        if (*pData != 255)
            return TRUE;
    }
    return FALSE;
}

T5_WORD T5SerialAsc_Receive (T5_PTSERIAL pSerial, T5_WORD wSize,
                             T5_PTR pData, T5_PTDWORD pdwStatus)
{
    T5_WORD wNb;
    T5_PTBYTE pb;
    T5_BYTE nibble;

    if (!_T5MBA_ISACTIVE(pdwStatus))
        return T5Serial_Receive (pSerial, wSize, pData);

    wNb = 0;
    pb = (T5_PTBYTE)pData;

    while (wNb < wSize)
    {
        if (_T5MBA_HASLAST(pdwStatus))
        {
            /* first nibble already received */
            *pb = _T5MBA_GETLAST (pdwStatus);
        }
        else
        {
            /* receive first nibble */
            if (!_ReceiveNibble (pSerial, &nibble, pdwStatus))
                return wNb;
            *pb = (nibble << 4);
        }
        /* receive second nibble */
        if (!_ReceiveNibble (pSerial, &nibble, pdwStatus))
        {
            _T5MBA_SETLAST (pdwStatus, *pb); /* remember 1rst nibble */
            return wNb;
        }
        *pb |= (nibble & 0x0f);
        /* 2 nibbles were received */
        _T5MBA_RESETLAST (pdwStatus);
        /* next one */
        wNb += 1;
        pb++;
    }
    return wNb;
}

#endif /*T5DEF_SERIAL*/

/****************************************************************************/

#endif /*T5DEF_MODBUSIOS*/

/* eof **********************************************************************/
