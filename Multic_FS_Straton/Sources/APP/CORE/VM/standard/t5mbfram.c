/*****************************************************************************
T5MBfram.c : MODBUS frames encoding

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_MODBUSIOS

/****************************************************************************/
/* static services */

static T5_WORD _T5MBFram_Crc16 (T5_PTBYTE pS, T5_WORD wLen);
static T5_BYTE _T5MBFram_Lrc (T5_PTBYTE pS, T5_WORD wLen);

/*****************************************************************************
T5MBFram_Build
Build a MODBUS frame
Parameters:
    pFrame (IN) pointer to the frame buffer
    wDriver (IN) kind of MODBUS driver
    wSlave (IN) MODBUS slave/unit identifier
    pReq (IN) pointer to the MODBUS request descriptor
*****************************************************************************/

void T5MBFram_Build (T5PTR_MBFRAME pFrame, T5_WORD wDriver,
                     T5_WORD wSlave, T5PTR_MBREQ pReq)
{
    T5_PTBYTE pData;
    T5_WORD i, nb, wCRC, wSize, wNByte;

    pFrame->bLRC = FALSE;

    pFrame->wFrmCount += 1;
    if (pFrame->wFrmCount > 2000)
        pFrame->wFrmCount = 1;

    pData = pFrame->buffer;
    wSize = 0;
    /* MBAP or function code */
    if (wDriver == T5MB_TCP)
    {
        T5_COPYFRAMEWORD(pData, &(pFrame->wFrmCount));
        pData += 2;
        *pData++ = 0;
        *pData++ = 0;
        *pData++ = 0;
        *pData++ = 0;
        wSize += 6;
    }
    *pData++ = (T5_BYTE)wSlave;
    wSize += 1;
    /* MOBUS frame */
    *pData = (T5_BYTE)(pReq->pDef->wFunc);

    /*T5_PRINTF ("Modbus shot F%02X (%02X) [%lu]", *pData, pReq->pDef->wFunc, GetTickCount ());*/

    pData++;
    wSize++;
    switch (pReq->pDef->wFunc)
    {
    case T5MBF_RAPSS :
        T5_COPYFRAMEWORD (pData, &(pReq->pDef->wAddr)); /* addr */
        pData[2] = 0xff;                                /* bits */
        pData[3] = 0x01;                                /* always 1 */
        pData[4] = 0x00;                                /* always 0 */
        pData += 5;
        wSize += 5;
        break;
    case T5MBF_RIB : /* 2  read input bits */
    case T5MBF_RCB : /* 1  read coil bits */
    case T5MBF_RIW : /* 4  read input words */
    case T5MBF_RHW : /* 3  read holding words */
        T5_COPYFRAMEWORD(pData, &(pReq->pDef->wAddr));
        T5_COPYFRAMEWORD(pData+2, &(pReq->pDef->wNbItem));
        pData += 4;
        wSize += 4;
        break;
    case T5MBF_W1W : /* 6  write 1 word */
        T5_COPYFRAMEWORD(pData, &(pReq->pDef->wAddr));
        T5_COPYFRAMEWORD(pData+2, pReq->pData);
        pData += 4;
        wSize += 4;
        break;
    case T5MBF_WNW : /* 16 write N words */
        nb = pReq->pDef->wNbItem;
        T5_COPYFRAMEWORD(pData, &(pReq->pDef->wAddr));
        T5_COPYFRAMEWORD(pData+2, &nb);
        pData += 4;
        *pData++ = (T5_BYTE)(nb * 2);
        wSize += 5;
        for (i=0; i<nb; i++)
        {
            T5_COPYFRAMEWORD(pData, pReq->pData + 2 * i);
            pData += 2;
            wSize += 2;
        }
        break;
    case T5MBF_W1B : /* 5  write 1 bit */
        T5_COPYFRAMEWORD(pData, &(pReq->pDef->wAddr));
        pData += 2;
        if ((pReq->pData[0] & 0x01) != 0)
            *pData++ = 0xff;
        else
            *pData++ = 0x00;
        *pData++ = 0x00;
        wSize += 4;
        break;
    case T5MBF_WNB : /* 15 write N bit */
        nb = pReq->pDef->wNbItem;
        T5_COPYFRAMEWORD(pData, &(pReq->pDef->wAddr));
        T5_COPYFRAMEWORD(pData+2, &nb);
        pData += 4;
        wNByte = nb / 8;
        if (nb % 8)
            wNByte += 1;
        *pData++ = (T5_BYTE)wNByte;
        wSize += 5;
        for (i=0; i<wNByte; i++)
        {
            *pData++ = pReq->pData[i];
            wSize++;
        }
        break;
    default : break;
    }
    /* CRC */
    if (wDriver == T5MB_RS232)
    {
        wCRC = _T5MBFram_Crc16 (pFrame->buffer, wSize);
        T5_COPYFRAMEWORD(pData, &wCRC);
        wSize += 2;
    }
    /* length in MBAP */
    if (wDriver == T5MB_TCP)
    {
        wSize -= 6;
        T5_COPYFRAMEWORD(pFrame->buffer + 4, &wSize);
        wSize += 6;
    }
    pFrame->wSize = wSize;
    pFrame->wLenCur = 0;
    pFrame->dwTimeOut = (T5_DWORD)(pReq->pDef->wTimeOut);
}

/*****************************************************************************
_T5MBFram_Crc16
Calculate a MODBUS CRC16
Parameters:
    pS (IN) pointer to the frame buffer
    wLen (IN) number of bytes
Return: MODBUS CRC16
*****************************************************************************/

static T5_WORD _T5MBFram_Crc16 (T5_PTBYTE pS, T5_WORD wLen)
{
    T5_WORD i, j;
    T5_WORD wCRC;

    /* calculate */
    wCRC = 0xffff;
    for (i=0; i<wLen; i++)
    {
        wCRC ^= (((T5_WORD)(pS[i])) & 0x00ff);
        for (j=0; j<8; j++)
        {
            if (wCRC & 1)
            {
                wCRC >>= 1;
                wCRC ^= 0xa001;
            }
            else
                wCRC >>= 1;
        }
    }
    wCRC = ((wCRC << 8) & 0xff00)
         | ((wCRC >> 8) & 0x00ff);
    return wCRC;
}

/*****************************************************************************
_T5MBFram_Lrc
Calculate a MODBUS ASCII LRC
Parameters:
    pS (IN) pointer to the frame buffer
    wLen (IN) number of bytes
Return: LRC
*****************************************************************************/

static T5_BYTE _T5MBFram_Lrc (T5_PTBYTE pS, T5_WORD wLen)
{
    T5_BYTE bLrc=0;

    while (wLen--)
        bLrc += *pS++;
    return (T5_BYTE)(-((T5_CHAR)bLrc));
}

/*****************************************************************************
T5MBFram_Extract
Parse a MODBUS frame
Parameters:
    pFrame (IN) pointer to the frame buffer
    wDriver (IN) kind of MODBUS driver
    wSlave (IN) MODBUS slave/unit identifier
    pReq (IN) pointer to the MODBUS request descriptor
*****************************************************************************/

void T5MBFram_Extract (T5PTR_MBFRAME pFrame, T5_WORD wDriver,
                       T5_WORD wSlave, T5PTR_MBREQ pReq)
{
    T5_WORD wCRC, i, nb, nbHere;
    T5_PTBYTE pData;

#ifdef T5DEF_RTCLOCKSTAMP
    pReq->dwDt = T5RtClk_GetCurDateStamp ();
    pReq->dwTm = T5RtClk_GetCurTimeStamp ();
#endif /*T5DEF_RTCLOCKSTAMP*/

    pData = pFrame->buffer;
    /* possible timeout */
    if (pFrame->wError)
    {
        pReq->wStatus = T5MB_ERROR;
        pReq->wError = pFrame->wError;
        pReq->wNbFail += 1;
        if (pReq->wNbFail == 0)
            pReq->wNbFail += 1;
        return;
    }
    /* check CRC */
    if (wDriver == T5MB_RS232)
    {
        if (pFrame->bLRC)
        {
            if (pFrame->buffer[pFrame->wSize-1]
                != _T5MBFram_Lrc (pFrame->buffer, pFrame->wSize-1))
            {
                pReq->wStatus = T5MB_ERROR;
                pReq->wError = T5MBERR_CRC;
                pReq->wNbFail += 1;
                if (pReq->wNbFail == 0)
                    pReq->wNbFail += 1;
                return;
            }
        }
        else
        {
            T5_COPYFRAMEWORD (&wCRC, pFrame->buffer + pFrame->wSize - 2);
            if (_T5MBFram_Crc16 (pFrame->buffer, (T5_WORD)(pFrame->wSize - 2))
                    != wCRC)
            {
                pReq->wStatus = T5MB_ERROR;
                pReq->wError = T5MBERR_CRC;
                pReq->wNbFail += 1;
                if (pReq->wNbFail == 0)
                    pReq->wNbFail += 1;
                return;
            }
        }
    }
    if (wDriver == T5MB_TCP)
        pData += 6;
    /* check echoed slave ID */
    if (*pData != (T5_BYTE)wSlave)
    {
        pReq->wStatus = T5MB_ERROR;
        pReq->wError = T5MBERR_COMM;
        pReq->wNbFail += 1;
        if (pReq->wNbFail == 0)
            pReq->wNbFail += 1;
        return;
    }
    pData++;
    /* check MODBUS error code */
    if (*pData & 0x80)
    {
        pReq->wStatus = T5MB_ERROR;
        pReq->wError = (T5_WORD)(pData[1]);
        pReq->wNbFail += 1;
        if (pReq->wNbFail == 0)
            pReq->wNbFail += 1;
        return;
    }
    /* check echoed function code */
    if (*pData != (T5_BYTE)(pReq->pDef->wFunc))
    {
        pReq->wStatus = T5MB_ERROR;
        pReq->wError = T5MBERR_COMM;
        pReq->wNbFail += 1;
        if (pReq->wNbFail == 0)
            pReq->wNbFail += 1;
        return;
    }
    /* response is OK */
    pReq->wStatus = T5MB_READY;
    pReq->wError = 0;
    pReq->wNbOK += 1;
    if (pReq->wNbOK == 0)
        pReq->wNbOK += 1;
    /* explore response */
    pReq->wStatus = T5MB_READY;
    pReq->wError = 0;
    switch (*pData)
    {
    case T5MBF_RAPSS :
        /* byte count 16 * 48bits (big endian) */
        for (i=0, nbHere=0; i<16; i++)
        {
            T5_BYTE b[8];
            T5_MEMSET (b, 0, sizeof (b));
            T5_COPYFRAME64 (b, pData + 2 - 2 + (6 * nbHere));
            T5_MEMCPY (pReq->pData + 2 + (6 * i), b, 6); /* intel only */
            nbHere += 1;
        }
        pReq->wMask = 0;
        break;
    case T5MBF_RIB : /* 2  read input bits */
    case T5MBF_RCB : /* 1  read coil bits */
        nb = (T5_WORD)(pData[1]);
        if (nb > pReq->pDef->wDataSize)
            nb = pReq->pDef->wDataSize;
        pData += 2;
        T5_MEMCPY(pReq->pData, pData, nb);
        break;
    case T5MBF_RIW : /* 4  read input words */
    case T5MBF_RHW : /* 3  read holding words */
        nb = (T5_WORD)(pData[1]);
        if (nb > pReq->pDef->wDataSize)
            nb = pReq->pDef->wDataSize;
        pData += 2;
        nb /= 2;
        for (i=0; i<nb; i++)
            T5_COPYFRAMEWORD (pReq->pData + i * 2, pData + i * 2);
        break;
    default : break;
    }
}

/****************************************************************************/

#endif /*T5DEF_MODBUSIOS*/

/* eof **********************************************************************/
