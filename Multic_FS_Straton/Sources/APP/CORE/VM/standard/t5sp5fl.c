/*****************************************************************************
T5sp5fl.c :  SP5 Safe protocol - frame layer

DO NOT ALTER THIS !

(c) COPALP 2007
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_SP5

/****************************************************************************/

#undef _SP5_TRACE

#ifdef _SP5_TRACE

static T5_WORD _SendTrace (T5_PTSERIAL pSerial, T5_WORD wSize, T5_PTR pData)
{
    T5_WORD i, nb;
    T5_PTBYTE p;
    
    nb = T5Serial_Send (pSerial, wSize, pData);

    p = (T5_PTBYTE)pData;
    for (i=0; i<nb; i++)
        printf ("%02X ", p[i]);

    return nb;
}

static T5_WORD _ReceiveTrace (T5_PTSERIAL pSerial, T5_WORD wSize, T5_PTR pData)
{
    T5_WORD i, nb;
    T5_PTBYTE p;
    
    nb = T5Serial_Receive (pSerial, wSize, pData);

    p = (T5_PTBYTE)pData;
    for (i=0; i<nb; i++)
        printf ("%02X ", p[i]);

    return nb;
}

static void _Eol (void)
{
    printf ("\n");
}

#define _SND_       _SendTrace
#define _RCV_       _ReceiveTrace
#define _EOFRAME    _Eol()

#else /*_SP5_TRACE*/

#define _SND_       T5Serial_Send
#define _RCV_       T5Serial_Receive
#define _EOFRAME    /*nothing*/

#endif /*_SP5_TRACE*/

/****************************************************************************/

static T5_BOOL _T5SP5Frm_FindInData (T5PTR_SP5FL pFL, T5PTR_SP5H pHeader,
                                     T5_PTBYTE pData);
static T5_WORD _T5SP5Frm_Crc16 (T5_WORD wCrc, T5_PTBYTE pBuffer, T5_WORD wLen);

/****************************************************************************/

T5_BOOL T5SP5Frm_Open (T5PTR_SP5FL pFL, T5_PTCHAR szConfig)
{
    T5_BOOL bRet;

    T5_MEMSET (pFL, 0, sizeof (T5STR_SP5FL));
    pFL->bRcvState = SP5FL_PM;
    T5Serial_Initialize (&(pFL->port));

    if (szConfig == NULL)
        return FALSE;

#ifdef T5DEF_SERIALINFO
    T5Serial_Info (&(pFL->port), T5SERINFO_P_SP5);
#endif /*T5DEF_SERIALINFO*/

    bRet = T5Serial_Open (&(pFL->port), szConfig);

#ifdef T5DEF_SERIALINFO
    T5Serial_Info (&(pFL->port), T5SERINFO_SP5);
#endif /*T5DEF_SERIALINFO*/

    return bRet;
}

void T5SP5Frm_Close (T5PTR_SP5FL pFL)
{
    T5Serial_Close (&(pFL->port));
    T5_MEMSET (pFL, 0, sizeof (T5STR_SP5FL));
    T5Serial_Initialize (&(pFL->port));
}

T5_BOOL T5SP5Frm_Send (T5PTR_SP5FL pFL, T5PTR_SP5H pHeader,
                       T5_PTBYTE pData, T5_BOOL bReset)
{
    T5_PTBYTE pPM, pH;
    T5_BYTE bEM;
    T5_WORD wLen, wCRC;

    if (bReset)
    {
        pFL->bSndState = SP5FL_PM;
        pFL->wSndCount = 0;
        pFL->wSndCRC = 0xffff;
    }

    if (pFL->bSndState == SP5FL_PM)
    {
        pPM = (T5_PTBYTE)SP5_H;
        pFL->wSndCount += _SND_ (&(pFL->port),
                                 (T5_WORD)(SP5_HLEN - pFL->wSndCount),
                                 pPM + pFL->wSndCount);
        if (pFL->wSndCount == SP5_HLEN)
        {
            pFL->bSndState = SP5FL_HEAD;
            pFL->wSndCount = 0;
            /* 'SP5' PM is not in CRC calculation */
        }
    }

    if (pFL->bSndState == SP5FL_HEAD)
    {
        pH = (T5_PTBYTE)pHeader;
        pFL->wSndCount += _SND_ (&(pFL->port),
                                 (T5_WORD)(sizeof (T5STR_SP5H) - pFL->wSndCount),
                                 pH + pFL->wSndCount);
        if (pFL->wSndCount == sizeof (T5STR_SP5H))
        {
            T5_COPYFRAMEWORD (&wLen, pHeader->ds);
            if (wLen == 0)
                pFL->bSndState = SP5FL_CRC;
            else
                pFL->bSndState = SP5FL_DATA;
            pFL->wSndCRC = _T5SP5Frm_Crc16 (pFL->wSndCRC, pH, sizeof (T5STR_SP5H));
            pFL->wSndCount = 0;
        }
    }

    if (pFL->bSndState == SP5FL_DATA)
    {
        T5_COPYFRAMEWORD (&wLen, pHeader->ds);
        pFL->wSndCount += _SND_ (&(pFL->port),
                                 (T5_WORD)(wLen - pFL->wSndCount),
                                 pData + pFL->wSndCount);
        if (pFL->wSndCount == wLen)
        {
            pFL->bSndState = SP5FL_CRC;
            pFL->wSndCount = 0;
            pFL->wSndCRC = _T5SP5Frm_Crc16 (pFL->wSndCRC, pData, wLen);
        }
    }

    if (pFL->bSndState == SP5FL_CRC)
    {
        T5_COPYFRAMEWORD (&wCRC, &(pFL->wSndCRC));
        pH = (T5_PTBYTE)(&wCRC);
        pFL->wSndCount += _SND_ (&(pFL->port),
                                 (T5_WORD)(2 - pFL->wSndCount),
                                 pH + pFL->wSndCount);
        if (pFL->wSndCount == 2)
        {
            pFL->bSndState = SP5FL_EM;
            pFL->wSndCount = 0;
        }
    }

    if (pFL->bSndState == SP5FL_EM)
    {
        bEM = SP5_EM;
        if (_SND_ (&(pFL->port), 1, &bEM) == 1)
        {
            pFL->bSndState = SP5FL_READY;
            pFL->wSndCount = 0;
            _EOFRAME;  /* complete! */

#ifdef T5DEF_SERIALINFO
            T5Serial_Info (&(pFL->port), T5SERINFO_SND);
#endif /*T5DEF_SERIALINFO*/

            return TRUE;
        }
    }

    return FALSE;
}

T5_BOOL T5SP5Frm_Receive (T5PTR_SP5FL pFL, T5PTR_SP5H pHeader,
                          T5_PTBYTE pData, T5_BOOL bReset)
{
    T5_BYTE b;
    T5_PTBYTE pH;
    T5_WORD wLen, wCRC;
    T5_WORD wRcvCountBefore;

    if (bReset)
    {
        pFL->bRcvState = SP5FL_PM;
        pFL->wRcvCount = 0;
        pFL->wRcvCRC = 0xffff;
    }

    wRcvCountBefore = pFL->wRcvCount;
    if (pFL->bRcvState == SP5FL_PM && _RCV_ (&(pFL->port), 1, &b) == 1)
    {
        switch (pFL->wRcvCount)
        {
        case 0 : if (b != SP5_H1)
                     b = 0; break;
        case 1 : if (b != SP5_H2)
                     b = 0; break;
        case 2 : if (b != SP5_H3)
                     b = 0; break;
        default: b = 0; break;
        }
        if (b != 0)
            pFL->wRcvCount += 1;
        else
            pFL->wRcvCount = 0;
        if (pFL->wRcvCount == 3)
        {
            pFL->bRcvState = SP5FL_HEAD;
            pFL->wRcvCount = 0;
            /* 'SP5' PM is not in CRC calculation */
        }
    }

    if (pFL->bRcvState == SP5FL_HEAD)
    {
        pH = (T5_PTBYTE)pHeader;
        pFL->wRcvCount += _RCV_ (&(pFL->port),
                                 (T5_WORD)(sizeof (T5STR_SP5H) - pFL->wRcvCount),
                                  pH + pFL->wRcvCount);
        if (pFL->wRcvCount == sizeof (T5STR_SP5H))
        {
            T5_COPYFRAMEWORD (&wLen, pHeader->ds);
            if (wLen > SP5MAX_FRAME) /* invalid data length */
                pFL->bRcvState = SP5FL_PM;
            else if (wLen == 0)
                pFL->bRcvState = SP5FL_CRC;
            else
                pFL->bRcvState = SP5FL_DATA;
            pFL->wRcvCount = 0;
        }
    }

    if (pFL->bRcvState == SP5FL_DATA)
    {
        T5_COPYFRAMEWORD (&wLen, pHeader->ds);
        pFL->wRcvCount += _RCV_ (&(pFL->port),
                                 (T5_WORD)(wLen - pFL->wRcvCount),
                                 pData + pFL->wRcvCount);
        if (pFL->wRcvCount == wLen)
        {
            pFL->bRcvState = SP5FL_CRC;
            pFL->wRcvCount = 0;
        }
    }

    if (pFL->bRcvState == SP5FL_CRC)
    {
        pH = (T5_PTBYTE)(&(pFL->wRcvCRC));
        pFL->wRcvCount += _RCV_ (&(pFL->port),
                                 (T5_WORD)(2 - pFL->wRcvCount),
                                 pH + pFL->wRcvCount);
        if (pFL->wRcvCount == 2)
        {
            T5_COPYFRAMEWORD (&wCRC, &(pFL->wRcvCRC));
            pFL->wRcvCRC = wCRC;
            pFL->bRcvState = SP5FL_EM;
            pFL->wRcvCount = 0;
        }
    }

    if (pFL->bRcvState == SP5FL_EM)
    {
        if (_RCV_ (&(pFL->port), 1, &b) == 1)
        {
            if (b != SP5_EM) /* invalid end marker */
            {
                pFL->bRcvState = SP5FL_PM;
                pFL->wRcvCount = 0;
            }
            else
            {
                _EOFRAME; /* complete! */
                return TRUE;
            }
        }
    }

    /* check for frame in data */
    if (pFL->bRcvState == SP5FL_DATA
        && wRcvCountBefore != pFL->wRcvCount
        && pFL->wRcvCount > (T5_WORD)(sizeof (T5STR_SP5H) + 6)
        && pData[pFL->wRcvCount-1] == SP5_EM
        && _T5SP5Frm_FindInData (pFL, pHeader, pData))
    {
        _EOFRAME; /* complete! */
        return TRUE;
    }

    return FALSE;
}

static T5_BOOL _T5SP5Frm_FindInData (T5PTR_SP5FL pFL,
                                     T5PTR_SP5H pHeader, T5_PTBYTE pData)
{
    T5_WORD wPos, wLen, wCrcCalc, wCrcFrame;
    T5PTR_SP5H ph;

    wPos = 0;
    while (wPos < (T5_WORD)(pFL->wRcvCount - sizeof (T5STR_SP5H) + 6))
    {
        if (T5_MEMCMP (pData + wPos, SP5_H, SP5_HLEN) == 0)
        {
            wPos += SP5_HLEN;
            ph = (T5PTR_SP5H)(pData+wPos);
            T5_COPYFRAMEWORD (&wLen, &(ph->ds));
            if ((T5_WORD)(wPos + wLen + sizeof (T5STR_SP5H) + 3)
                == pFL->wRcvCount)
            {
                wCrcCalc = _T5SP5Frm_Crc16 (
                    0xffff,
                    pData + wPos,
                    (T5_WORD)(wLen + sizeof (T5STR_SP5H)));
                T5_COPYFRAMEWORD (
                    &wCrcFrame,
                    pData + (wPos + sizeof (T5STR_SP5H) + wLen));
                if (wCrcCalc == wCrcFrame)
                {
                    T5_MEMCPY (pHeader, pData+wPos, sizeof (T5STR_SP5H));
                    pFL->wRcvCRC = wCrcFrame;
                    T5_MEMCPY (pData, pData + (wPos+sizeof(T5STR_SP5H)), wLen);
                    return TRUE;
                }
            }
        }
        wPos++;
    }
    return FALSE;
}

T5_BOOL T5SP5Frm_CheckCRC (T5PTR_SP5FL pFL, T5PTR_SP5H pHeader, T5_PTBYTE pData)
{
    T5_WORD wCrc, wLen;

    wCrc = _T5SP5Frm_Crc16 (0xffff, (T5_PTBYTE)pHeader, sizeof (T5STR_SP5H));
    T5_COPYFRAMEWORD (&wLen, &(pHeader->ds));
    if (wLen)
        wCrc = _T5SP5Frm_Crc16 (wCrc, pData, wLen);
    return (wCrc == pFL->wRcvCRC);
}

/****************************************************************************/

static T5_WORD _T5SP5Frm_Crc16 (T5_WORD wCrc, T5_PTBYTE pBuffer, T5_WORD wLen)
{
    T5_WORD i, j;

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

#endif /*T5DEF_SP5*/

/* eof **********************************************************************/
