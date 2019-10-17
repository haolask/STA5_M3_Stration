/*****************************************************************************
T5CSSer.c :  communication server - RS232 serial protocol

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_SERIAL

#ifndef T5CS_RCVSERTO
#define T5CS_RCVSERTO 500L
#endif /*T5CS_RCVSERTO*/

/****************************************************************************/
/* protocol */

/*

frame header = 4 bytes - followed by data - CRC16

+-----+-----+---------+---------+---...---+----------+----------+
| 'T' | '5' | Nb (HB) | Nb (LB) | Data    | CRC (HB) | CRC (LB) |
+-----+-----+---------+---------+---...---+----------+----------+

'T' = 84 (0x54)
'5' = 53 (0x35)
The Nb value (length of data) does not include the CRC

*/

/****************************************************************************/
/* static services */

static void _T5CSSerial_Send (T5PTR_CSPORT pPort);
static void _T5CSSerial_Receive (T5PTR_CSPORT pPort, T5_DWORD dwStamp);
static T5_WORD _T5CSSerial_Crc16 (T5_PTBYTE pBuffer, T5_WORD wLen);

/*****************************************************************************
T5CSSer_Open
Open the SERIAL com port if requested
Parameters:
    pData (IN) pointer to the communication server database
    szConfig (IN) pointer to COM settings string
*****************************************************************************/

void T5CSSer_Open (T5PTR_CS pData, T5_PTCHAR szConfig)
{
    T5_SERIAL serial;

    if (szConfig == NULL)
        return;

    T5Serial_Initialize (&serial);
#ifdef T5DEF_SERIALINFO
    T5Serial_Info (&serial, T5SERINFO_P_T5);
#endif /*T5DEF_SERIALINFO*/
    if (T5Serial_Open (&serial, szConfig))
    {
#ifdef T5DEF_SERIALINFO
    T5Serial_Info (&serial, T5SERINFO_T5);
#endif /*T5DEF_SERIALINFO*/

        T5_MEMCPY (&(pData->csPort[0].ser), &serial, sizeof (T5_SERIAL));
        pData->csPort[0].wFlags = T5CS_READY | T5CS_SERIAL;
        pData->csPort[0].wID = 1;
    }
}

/*****************************************************************************
T5CSSer_Close
Close the SERIAL com port if any
Parameters:
    pData (IN) pointer to the communication server database
*****************************************************************************/

void T5CSSer_Close (T5PTR_CS pData)
{
    if (!T5Serial_IsValid (&(pData->csPort[0].ser)))
        return;

    T5Serial_Close (&(pData->csPort[0].ser));
    pData->csPort[0].wFlags &= ~T5CS_READY;
    pData->csPort[0].wID = 1;
}

/*****************************************************************************
T5CSSer_Activate
Activate the serial communication for send and receive
Parameters:
    pData (IN) pointer to the communication server database
*****************************************************************************/

void T5CSSer_Activate (T5PTR_CS pData)
{
    if ((pData->csPort[0].wFlags & T5CS_SERIAL) == 0
        || !T5Serial_IsValid (&(pData->csPort[0].ser)))
        return;

#ifdef T5DEF_CSM5
    if ((pData->dwFlags & T5CS_M5) != 0)
    {
        T5CSM5_Activate (pData->csPort, pData->dwStamp);
    }
    else
#endif /*T5DEF_CSM5*/
    {
        _T5CSSerial_Send (pData->csPort);

        if (pData->dwStamp)
        {
            if (pData->csPort->dwTimeStamp && pData->csPort->wSizeIn != 0
                && T5_BOUNDTIME(pData->dwStamp - pData->csPort->dwTimeStamp) > T5CS_RCVSERTO)
            {
                pData->csPort->wSizeIn = 0;
                pData->csPort->wPakSizeIn = 0;
	            pData->csPort->wFlags = T5CS_READY | T5CS_SERIAL;
            }
        }

        _T5CSSerial_Receive (pData->csPort, pData->dwStamp);
    }
}

/*****************************************************************************
_T5CSSerial_Send
Send pending data on the serial port
Parameters:
    pPort (IN) pointer to the serial port descriptor
*****************************************************************************/

static void _T5CSSerial_Send (T5PTR_CSPORT pPort)
{
    T5_WORD wRec, wCrc;
    T5_PTBYTE pData;

    /* return if nothing to send */
    if ((pPort->wFlags & (T5CS_DATAOUT | T5CS_SENDING)) == 0)
        return;
    /* pack Crc (and thus increase frame size) */
    if (pPort->wPakSizeOut == pPort->wSizeOut && (pPort->wFlags & 0x0008) == 0)
    {
        wCrc = _T5CSSerial_Crc16 (pPort->bfOut,
                                  (T5_WORD)(pPort->wPakSizeOut));

        if (!T5CS_ISHEAD(pPort->bfOut))
        {
            wCrc = ((wCrc << 8) & 0xff00)
                 | ((wCrc >> 8) & 0x00ff);
        }

        T5_COPYFRAMEWORD(&(pPort->bfOut[pPort->wPakSizeOut]), &wCrc);
        pPort->wPakSizeOut += 2;
        pPort->wSizeOut += 2;
        pPort->wFlags |= 0x0008;
    }
    /* send pending data */
    pData = &(pPort->bfOut[pPort->wPakSizeOut - pPort->wSizeOut]);
    wRec = T5Serial_Send (&(pPort->ser), pPort->wSizeOut, pData);
    pPort->wSizeOut -= wRec;
    /* update flags */
    if (pPort->wSizeOut == 0)
    {
        pPort->wFlags &= ~T5CS_DATAOUT;
        pPort->wFlags &= ~T5CS_SENDING;
        pPort->wFlags &= ~0x0008;
        pPort->wPakSizeOut = 0;

#ifdef T5DEF_SERIALINFO
        T5Serial_Info (&(pPort->ser), T5SERINFO_SND);
#endif /*T5DEF_SERIALINFO*/
    }
    else
        pPort->wFlags |= T5CS_SENDING;
}

/*****************************************************************************
_T5CSSerial_Receive
Listen and receive characters on the serial port
Parameters:
    pPort (IN) pointer to the serial port descriptor
    dwStamp (IN) current time stamp or 0 if not supported
*****************************************************************************/

static void _T5CSSerial_Receive (T5PTR_CSPORT pPort, T5_DWORD dwStamp)
{
    T5_WORD wRec, wExpect, wSize, wCrc;
    T5_PTBYTE pData;
    T5_BYTE dummy;
    T5_BOOL bResynchro;

    /* return if there is already a full frame received */
    if ((pPort->wFlags & T5CS_DATAIN) != 0)
        return;

    pData = &(pPort->bfIn[pPort->wSizeIn]);

    if (pPort->wPakSizeIn == 0) wExpect = 4 - pPort->wSizeIn;
    else wExpect = pPort->wPakSizeIn - pPort->wSizeIn;

    wRec = T5Serial_Receive (&(pPort->ser), wExpect, pData);
    /* return if nothing received */
    if (!wRec)
        return;
    /* update stamp if any */
    if (dwStamp)
        pPort->dwTimeStamp = dwStamp;
    /* check and store received data */
    bResynchro = FALSE;
    pPort->wSizeIn += wRec;
    if (pPort->wSizeIn == 4) /* check header */
    {
        if (!T5CS_ISHEAD(pPort->bfIn))
        {
            switch (pPort->bfIn[1])
            {
            case T5MBF_RIB : /* read bit inputs */
            case T5MBF_RCB : /* read coils */
            case T5MBF_RIW : /* read input registers */
            case T5MBF_RHW : /* read holding registers */
                pPort->wPakSizeIn = 6;
                pPort->wFlags |= T5CS_RECEIVING;
                break;
            case T5MBF_W1B : /* write 1 coil */
            case T5MBF_W1W : /* write 1 register */
            case T5MBF_DIAG : /* diagnostic */
                pPort->wPakSizeIn = 8;
                pPort->wFlags |= T5CS_RECEIVING;
                break;
            case T5MBF_WNB : /* write N coils */
            case T5MBF_WNW : /* write N registers */
                pPort->wPakSizeIn = 8;
                pPort->wFlags |= T5CS_RECEIVING;
                break;
            case T5MBF_IDENT :
                pPort->wPakSizeIn = 7;
                pPort->wFlags |= T5CS_RECEIVING;
                break;
            default :
                bResynchro = TRUE;
                break;
            }
        }
        else
        {
            T5_COPYFRAMEWORD(&wSize, &(pPort->bfIn[2]));
            if (wSize == 0 || wSize > T5MAX_FRAME)
            {
                T5_PRINTF ("Invalid frame size - endianness maybe incorrect");
                bResynchro = TRUE;
            }
            else
            {
                pPort->wPakSizeIn = wSize + 6; /* + header and Crc */
                pPort->wFlags |= T5CS_RECEIVING;
            }
        }
    }
    /* check MODBUS frame length - WN */
    else if (pPort->wSizeIn == pPort->wPakSizeIn
             && pPort->wSizeIn == 8 && !T5CS_ISHEAD(pPort->bfIn)
             && (pPort->bfIn[1] == T5MBF_WNB || pPort->bfIn[1] == T5MBF_WNW))
    {
        T5_COPYFRAMEWORD(&wCrc, &(pPort->bfIn[pPort->wPakSizeIn-2]));
        wCrc = ((wCrc << 8) & 0xff00) | ((wCrc >> 8) & 0x00ff);
        /* complete answer */
        if (wCrc == _T5CSSerial_Crc16 (pPort->bfIn,
                                       (T5_WORD)(pPort->wPakSizeIn-2)))
        {
            pPort->wFlags &= ~T5CS_RECEIVING;
            pPort->wFlags |= T5CS_DATAIN;

#ifdef T5DEF_SERIALINFO
            T5Serial_Info (&(pPort->ser), T5SERINFO_RCV);
#endif /*T5DEF_SERIALINFO*/
        }
        /* incomplete question */
        else
        {
            pPort->wPakSizeIn = 9 + ((T5_WORD)(pPort->bfIn[6]) & 0x00ff);
            pPort->wFlags |= T5CS_RECEIVING;
        }
    }
    /* check MODBUS frame length - RN */
    else if (pPort->wSizeIn == pPort->wPakSizeIn
             && pPort->wSizeIn == 6 && !T5CS_ISHEAD(pPort->bfIn)
             && (pPort->bfIn[1] == T5MBF_RIB || pPort->bfIn[1] == T5MBF_RCB
             || pPort->bfIn[1] == T5MBF_RIW || pPort->bfIn[1] == T5MBF_RHW))
    {
        T5_COPYFRAMEWORD(&wCrc, &(pPort->bfIn[pPort->wPakSizeIn-2]));
        wCrc = ((wCrc << 8) & 0xff00) | ((wCrc >> 8) & 0x00ff);
        /* complete answer */
        if (wCrc == _T5CSSerial_Crc16 (pPort->bfIn,
                                       (T5_WORD)(pPort->wPakSizeIn-2)))
        {
            pPort->wFlags &= ~T5CS_RECEIVING;
            pPort->wFlags |= T5CS_DATAIN;

#ifdef T5DEF_SERIALINFO
            T5Serial_Info (&(pPort->ser), T5SERINFO_RCV);
#endif /*T5DEF_SERIALINFO*/
        }
        /* incomplete question */
        else
        {
            pPort->wPakSizeIn = 7;
            pPort->wFlags |= T5CS_RECEIVING;
        }
    }
    /* check MODBUS frame length - RN */
    else if (pPort->wSizeIn == pPort->wPakSizeIn
             && pPort->wSizeIn == 7 && !T5CS_ISHEAD(pPort->bfIn)
             && (pPort->bfIn[1] == T5MBF_RIB || pPort->bfIn[1] == T5MBF_RCB
             || pPort->bfIn[1] == T5MBF_RIW || pPort->bfIn[1] == T5MBF_RHW))
    {
        T5_COPYFRAMEWORD(&wCrc, &(pPort->bfIn[pPort->wPakSizeIn-2]));
        wCrc = ((wCrc << 8) & 0xff00) | ((wCrc >> 8) & 0x00ff);
        /* complete answer */
        if (wCrc == _T5CSSerial_Crc16 (pPort->bfIn,
                                       (T5_WORD)(pPort->wPakSizeIn-2)))
        {
            pPort->wFlags &= ~T5CS_RECEIVING;
            pPort->wFlags |= T5CS_DATAIN;

#ifdef T5DEF_SERIALINFO
            T5Serial_Info (&(pPort->ser), T5SERINFO_RCV);
#endif /*T5DEF_SERIALINFO*/
        }
        /* incomplete question */
        else
        {
            pPort->wPakSizeIn = 8;
            pPort->wFlags |= T5CS_RECEIVING;
        }
    }
    /* check MODBUS frame length - RN */
    else if (pPort->wSizeIn == pPort->wPakSizeIn
             && pPort->wSizeIn == 8 && !T5CS_ISHEAD(pPort->bfIn)
             && (pPort->bfIn[1] == T5MBF_RIB || pPort->bfIn[1] == T5MBF_RCB
             || pPort->bfIn[1] == T5MBF_RIW || pPort->bfIn[1] == T5MBF_RHW))
    {
        T5_COPYFRAMEWORD(&wCrc, &(pPort->bfIn[pPort->wPakSizeIn-2]));
        wCrc = ((wCrc << 8) & 0xff00) | ((wCrc >> 8) & 0x00ff);
        /* complete question */
        if (wCrc == _T5CSSerial_Crc16 (pPort->bfIn,
                                       (T5_WORD)(pPort->wPakSizeIn-2)))
        {
            pPort->wFlags &= ~T5CS_RECEIVING;
            pPort->wFlags |= T5CS_DATAIN;

#ifdef T5DEF_SERIALINFO
            T5Serial_Info (&(pPort->ser), T5SERINFO_RCV);
#endif /*T5DEF_SERIALINFO*/
        }
        /* incomplete answer */
        else
        {
            pPort->wPakSizeIn = ((T5_WORD)(pPort->bfIn[2]) & 0xff) + 5;
            pPort->wFlags |= T5CS_RECEIVING;
            if (pPort->wPakSizeIn <= pPort->wSizeIn)
                bResynchro = TRUE;
        }
    }
    else if (pPort->wSizeIn == pPort->wPakSizeIn) /* complete */
    {
        pPort->wFlags &= ~T5CS_RECEIVING;
        T5_COPYFRAMEWORD(&wCrc, &(pPort->bfIn[pPort->wPakSizeIn-2]));

        if (!T5CS_ISHEAD(pPort->bfIn))
        {
            wCrc = ((wCrc << 8) & 0xff00)
                 | ((wCrc >> 8) & 0x00ff);
        }

        if (wCrc == _T5CSSerial_Crc16 (pPort->bfIn,
                                       (T5_WORD)(pPort->wPakSizeIn-2)))
        {
            pPort->wFlags |= T5CS_DATAIN;

#ifdef T5DEF_SERIALINFO
            T5Serial_Info (&(pPort->ser), T5SERINFO_RCV);
#endif /*T5DEF_SERIALINFO*/
        }
        else
        {
            bResynchro = TRUE;
        }
    }
    else /* data expected */
    {
        pPort->wFlags |= T5CS_RECEIVING;
    }

#ifdef T5DEF_MBSENDFORFLUSH
    if (pPort->wFlags & T5CS_DATAIN)
        T5Serial_Send (&(pPort->ser), 0, NULL);
#endif

    /* flush on error */
    if (bResynchro)
    {
        pPort->wSizeIn = 0;
        pPort->wPakSizeIn = 0;
    	while (T5Serial_Receive (&(pPort->ser), 1, &dummy));
        pPort->wFlags = T5CS_READY | T5CS_SERIAL;
    }
}

/*****************************************************************************
_T5CSSerial_Crc16
Calculate a MODBUS like CRC16
Parameters:
    pBuffer (IN) pointer to the frame buffer
    wLen (IN) length of the frame
Return: CRC16
*****************************************************************************/

static T5_WORD _T5CSSerial_Crc16 (T5_PTBYTE pBuffer, T5_WORD wLen)
{
    T5_WORD i, j;
    T5_WORD wCrc;

    /* calculate */
    wCrc = 0xffff;
    for (i=0; i<wLen; i++)
    {
        wCrc ^= (((T5_WORD)(pBuffer[i])) & 0x00ff);
        for (j=0; j<8; j++)
        {
            if (wCrc & 1)
            {
                wCrc >>= 1;
                wCrc ^= 0xa001;
            }
            else
                wCrc >>= 1;
        }
    }
    return wCrc;
}

/****************************************************************************/

#endif /*T5DEF_SERIAL*/

/* eof **********************************************************************/

