/*****************************************************************************
T5MBsrv.c :  MODBUS frames encoding

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_MODBUSIOS

/****************************************************************************/
/* byte masks */

static T5_WORD _T5_MASK8[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

/****************************************************************************/
/* static services */

static T5PTR_MBREQ _T5MBSrv_FindReq (T5PTR_MBCLIENT pServer,
                                     T5_WORD wOpe, T5_WORD wAdd, T5_WORD wNb);

static T5_WORD _T5MBSrv_ReadWords (T5PTR_MBCLIENT pServer,
                                   T5_WORD wOpe, T5_WORD wAdd, T5_WORD wNb,
                                   T5_PTBYTE pIn, T5_PTBYTE pOut,
                                   T5_PTBYTE bError);

static T5_WORD _T5MBSrv_Write1Word (T5PTR_MBCLIENT pServer,
                                    T5_WORD wAdd, T5_WORD wValue,
                                    T5_PTBYTE pIn, T5_PTBYTE pOut,
                                    T5_PTBYTE bError);

static T5_WORD _T5MBSrv_WriteWords (T5PTR_MBCLIENT pServer,
                                    T5_WORD wAdd, T5_WORD wNb,
                                    T5_PTBYTE pIn, T5_PTBYTE pOut,
                                    T5_PTBYTE bError);

static T5_WORD _T5MBSrv_ReadBits (T5PTR_MBCLIENT pServer,
                                  T5_WORD wOpe, T5_WORD wAdd, T5_WORD wNb,
                                  T5_PTBYTE pIn, T5_PTBYTE pOut,
                                  T5_PTBYTE bError);

static T5_WORD _T5MBSrv_Write1Bit (T5PTR_MBCLIENT pServer,
                                   T5_WORD wAdd, T5_WORD wValue,
                                   T5_PTBYTE pIn, T5_PTBYTE pOut,
                                   T5_PTBYTE bError);

static T5_WORD _T5MBSrv_WriteBits (T5PTR_MBCLIENT pServer,
                                   T5_WORD wAdd, T5_WORD wNb,
                                   T5_PTBYTE pIn, T5_PTBYTE pOut,
                                   T5_PTBYTE bError);

static void _T5MBSrv_SetBit (T5_PTBYTE pBase,
                             T5_WORD wOffset, T5_BYTE bValue);
static T5_BYTE _T5VMSrv_GetBit (T5_PTBYTE pBase, T5_WORD wOffset);

static T5_WORD _T5MBSrv_Serve43 (T5PTR_MBCLIENT pServer, T5_PTBYTE pIn, T5_PTBYTE pOut);
static T5_WORD _T5MBSrv_Serve08 (T5PTR_MBCLIENT pServer, T5_PTBYTE pIn, T5_PTBYTE pOut);

#undef T5MB_EMULATE_RAPSS
#ifdef T5MB_EMULATE_RAPSS
static T5_WORD _T5MBSrv_ServeRAPSS (T5PTR_MBCLIENT pServer, T5_PTBYTE pIn, T5_PTBYTE pOut);
#endif /*T5MB_EMULATE_RAPSS*/

/****************************************************************************/

#ifdef T5DEF_MBSHIFTSRVADDR
extern T5_WORD T5MB_ShiftSrvAddr (T5_WORD wRcvAddr, T5_BYTE bReqType);
#endif /*T5DEF_MBSHIFTSRVADDR*/

/*****************************************************************************
T5MBSrv_Serve
Serve a slave request
Parameters:
    pServer (IN) server object
    pIn (IN) received request
    pOut (OUT) answer buffer
Return: answer length
*****************************************************************************/

T5_WORD T5MBSrv_Serve (T5PTR_MBCLIENT pServer, T5_PTBYTE pIn, T5_PTBYTE pOut)
{
    T5_BYTE bError;
    T5_WORD wLen, wAdd, wArg;

    if (pIn[1] == T5MBF_IDENT)
        return _T5MBSrv_Serve43 (pServer, pIn, pOut);
    if (pIn[1] == T5MBF_DIAG)
        return _T5MBSrv_Serve08 (pServer, pIn, pOut);

#ifdef T5MB_EMULATE_RAPSS
    if (pIn[1] == T5MBF_RAPSS)
        return _T5MBSrv_ServeRAPSS (pServer, pIn, pOut);
#endif /*T5MB_EMULATE_RAPSS*/

    if (pIn[1] == T5MBF_RID)
    {
        pOut[0] = pIn[0];
        pOut[1] = pIn[1];
        pOut[2] = 1;
        pOut[3] = 0xff;
        return 4;
    }

    bError = 0;
    wLen = 0;
    /* echo slave number and function code */
    pOut[0] = pIn[0];
    pOut[1] = pIn[1];
    /* extract MODBUS address and argument (nb or value) */
    T5_COPYFRAMEWORD (&wAdd, pIn + 2);
    T5_COPYFRAMEWORD (&wArg, pIn + 4);

#ifdef T5DEF_MBSHIFTSRVADDR
    wAdd = T5MB_ShiftSrvAddr (wAdd, pIn[1]);
#endif /*T5DEF_MBSHIFTSRVADDR*/
    
    /* process function */
    switch (pIn[1])
    {
    case T5MBF_RIB : /* read bit inputs */
        wLen = _T5MBSrv_ReadBits (pServer, T5MBF_IB,
                                  wAdd, wArg, pIn, pOut, &bError);
        break;
    case T5MBF_RCB : /* read coils */
        wLen = _T5MBSrv_ReadBits (pServer, T5MBF_CB,
                                  wAdd, wArg, pIn, pOut, &bError);
        break;
    case T5MBF_W1B : /* write 1 coil */
        wLen = _T5MBSrv_Write1Bit (pServer, wAdd, wArg, pIn, pOut, &bError);
        break;
    case T5MBF_WNB : /* write N coils */
        wLen = _T5MBSrv_WriteBits (pServer, wAdd, wArg, pIn, pOut, &bError);
        break;
    case T5MBF_RIW : /* read input registers */
        wLen = _T5MBSrv_ReadWords (pServer, T5MBF_IW,
                                   wAdd, wArg, pIn, pOut, &bError);
        break;
    case T5MBF_RHW : /* read holding registers */
        wLen = _T5MBSrv_ReadWords (pServer, T5MBF_HW,
                                   wAdd, wArg, pIn, pOut, &bError);
        break;
    case T5MBF_W1W : /* write 1 register */
        wLen = _T5MBSrv_Write1Word (pServer, wAdd, wArg, pIn, pOut, &bError);
        break;
    case T5MBF_WNW : /* write N registers */
        wLen = _T5MBSrv_WriteWords (pServer, wAdd, wArg, pIn, pOut, &bError);
        break;
    default :
        bError = T5MBERR_FUNC;
        break;
    }
    /* exception answer */
    if (bError)
    {
        pOut[1] = pIn[1] | 0x80;
        pOut[2] = bError;
        wLen = 3;
    }
    return wLen;
}

/*****************************************************************************
_T5MBSrv_FindReq
Serach for a request
Parameters:
    pServer (IN) server object
    wOpe (IN) kind of request
    wAdd (IN) MODBUS address
    wNb (IN) number of MODBUS items
Return: pointer to the request or NULL if not found
*****************************************************************************/

static T5PTR_MBREQ _T5MBSrv_FindReq (T5PTR_MBCLIENT pServer,
                                     T5_WORD wOpe, T5_WORD wAdd, T5_WORD wNb)
{
    T5PTR_MBREQ pReq, pFound;
    T5_WORD i;

    pReq = pServer->pReq;
    pFound = T5_PTNULL;
    for (i=0; pFound == T5_PTNULL && i<pServer->wNbReq; i++, pReq++)
    {
        if (wOpe == pReq->pDef->wFunc
            && wAdd >= (pReq->pDef->wAddr)
            && (wAdd + wNb) <= (pReq->pDef->wAddr + pReq->pDef->wNbItem))
        {
#ifdef T5DEF_RTCLOCKSTAMP
            pReq->dwDt = T5RtClk_GetCurDateStamp ();
            pReq->dwTm = T5RtClk_GetCurTimeStamp ();
#else /*T5DEF_RTCLOCKSTAMP*/
            pReq->dwDt = 0L;
            pReq->dwTm = 0L;
#endif /*T5DEF_RTCLOCKSTAMP*/
            pFound = pReq;
        }
    }
    return pFound;
}

/*****************************************************************************
_T5MBSrv_Read/Write...
Serve a request
Parameters:
    pServer (IN) server object
    wOpe (IN) kind of request
    wAdd (IN) MODBUS address
    wNb (IN) number of MODBUS items
    pIn (IN) received request
    pOut (OUT) answer buffer
    bError (OUT) error answer
Return: answer length
*****************************************************************************/

static T5_WORD _T5MBSrv_ReadWords (T5PTR_MBCLIENT pServer,
                                   T5_WORD wOpe, T5_WORD wAdd, T5_WORD wNb,
                                   T5_PTBYTE pIn, T5_PTBYTE pOut,
                                   T5_PTBYTE bError)
{
    T5PTR_MBREQ pReq;
    T5_PTWORD pReg;
    T5_WORD i;
    T5_UNUSED(pIn);

    /* check absolute number */
    if (wNb > 125)
    {
        *bError = T5MBERR_ADDR;
        return 0;
    }
    /* find served data block */
    pReq = _T5MBSrv_FindReq (pServer, wOpe, wAdd, wNb);
    if (pReq == T5_PTNULL)
    {
        *bError = T5MBERR_ADDR;
        return 0;
    }
    *bError = 0;
    /* copy registers */
    pOut[2] = (T5_BYTE)(wNb * 2);
    pReg = (T5_PTWORD)(pReq->pData);
    pReg += (wAdd - pReq->pDef->wAddr);
    for (i=0; i<wNb; i++)
        T5_COPYFRAMEWORD (pOut + 3 + 2 * i, pReg + i);
    return (wNb * 2 + 3);
}

static T5_WORD _T5MBSrv_Write1Word (T5PTR_MBCLIENT pServer,
                                    T5_WORD wAdd, T5_WORD wValue,
                                    T5_PTBYTE pIn, T5_PTBYTE pOut,
                                    T5_PTBYTE bError)
{
    T5PTR_MBREQ pReq;
    T5_PTWORD pReg;

    /* find served data block */
    pReq = _T5MBSrv_FindReq (pServer, T5MBF_HW, wAdd, 1);
    if (pReq == T5_PTNULL)
    {
        *bError = T5MBERR_ADDR;
        return 0;
    }
    *bError = 0;
    /* copy register */
    pReg = (T5_PTWORD)(pReq->pData);
    pReg[wAdd - pReq->pDef->wAddr] = wValue;

#ifdef T5DEF_MBSREADQ
    /* mark as changed */
    pReg = (T5_PTWORD)(pReq->pData + pReq->pDef->wDataSize);
    pReg[wAdd - pReq->pDef->wAddr] = 0xffff;
#endif /*T5DEF_MBSREADQ*/

    /* echo frame */
    T5_MEMCPY (pOut+2, pIn+2, 4);
    return 6;
}

static T5_WORD _T5MBSrv_WriteWords (T5PTR_MBCLIENT pServer,
                                    T5_WORD wAdd, T5_WORD wNb,
                                    T5_PTBYTE pIn, T5_PTBYTE pOut,
                                    T5_PTBYTE bError)
{
    T5PTR_MBREQ pReq;
    T5_PTWORD pReg;
    T5_WORD i;

    /* find served data block */
    pReq = _T5MBSrv_FindReq (pServer, T5MBF_HW, wAdd, wNb);
    if (pReq == T5_PTNULL)
    {
        *bError = T5MBERR_ADDR;
        return 0;
    }
    *bError = 0;
    /* copy register */
    pReg = (T5_PTWORD)(pReq->pData);
    pReg += (wAdd - pReq->pDef->wAddr);
    for (i=0; i<wNb; i++)
        T5_COPYFRAMEWORD (pReg + i, pIn + 7 + 2 * i);

#ifdef T5DEF_MBSREADQ
    /* mark as changed */
    pReg = (T5_PTWORD)(pReq->pData + pReq->pDef->wDataSize);
    pReg += (wAdd - pReq->pDef->wAddr);
    for (i=0; i<wNb; i++)
        pReg[i] = 0xffff;
#endif /*T5DEF_MBSREADQ*/

    /* echo frame */
    T5_MEMCPY (pOut+2, pIn+2, 4);
    return 6;
}

static T5_WORD _T5MBSrv_ReadBits (T5PTR_MBCLIENT pServer,
                                  T5_WORD wOpe, T5_WORD wAdd, T5_WORD wNb,
                                  T5_PTBYTE pIn, T5_PTBYTE pOut,
                                  T5_PTBYTE bError)
{
    T5PTR_MBREQ pReq;
    T5_PTBYTE pBit;
    T5_WORD i, wByteCount, wBase;
    T5_UNUSED(pIn);

    /* check absolute number */
    if (wNb > 2000)
    {
        *bError = T5MBERR_ADDR;
        return 0;
    }
    /* find served data block */
    pReq = _T5MBSrv_FindReq (pServer, wOpe, wAdd, wNb);
    if (pReq == T5_PTNULL)
    {
        *bError = T5MBERR_ADDR;
        return 0;
    }
    *bError = 0;
    /* copy bits */
    wByteCount = wNb / 8;
    if (wNb % 8)
        wByteCount += 1;
    pOut[2] = (T5_BYTE)(wByteCount);
    pBit = pReq->pData;
    wBase = wAdd - pReq->pDef->wAddr;
    for (i=0; i<wNb; i++)
        _T5MBSrv_SetBit (pOut+3, i,
                         _T5VMSrv_GetBit (pBit, (T5_WORD)(i+wBase)));
    return (wByteCount + 3);
}

static T5_WORD _T5MBSrv_Write1Bit (T5PTR_MBCLIENT pServer,
                                   T5_WORD wAdd, T5_WORD wValue,
                                   T5_PTBYTE pIn, T5_PTBYTE pOut,
                                   T5_PTBYTE bError)
{
    T5PTR_MBREQ pReq;
    T5_PTBYTE pBit;

    /* find served data block */
    pReq = _T5MBSrv_FindReq (pServer, T5MBF_CB, wAdd, 1);
    if (pReq == T5_PTNULL)
    {
        *bError = T5MBERR_ADDR;
        return 0;
    }
    *bError = 0;
    /* copy bit */
    pBit = pReq->pData;
    _T5MBSrv_SetBit (pBit, (T5_WORD)(wAdd - pReq->pDef->wAddr),
                     (T5_BYTE)(wValue != 0));

#ifdef T5DEF_MBSREADQ
    /* mark as changed */
    pBit = pReq->pData + pReq->pDef->wDataSize;
    _T5MBSrv_SetBit (pBit, (T5_WORD)(wAdd - pReq->pDef->wAddr), TRUE);
#endif /*T5DEF_MBSREADQ*/

    /* echo frame */
    T5_MEMCPY (pOut+2, pIn+2, 4);
    return 6;
}

static T5_WORD _T5MBSrv_WriteBits (T5PTR_MBCLIENT pServer,
                                   T5_WORD wAdd, T5_WORD wNb,
                                   T5_PTBYTE pIn, T5_PTBYTE pOut,
                                   T5_PTBYTE bError)
{
    T5PTR_MBREQ pReq;
    T5_PTBYTE pBit;
    T5_WORD i, wBase;

    /* find served data block */
    pReq = _T5MBSrv_FindReq (pServer, T5MBF_CB, wAdd, wNb);
    if (pReq == T5_PTNULL)
    {
        *bError = T5MBERR_ADDR;
        return 0;
    }
    *bError = 0;
    /* copy bits */
    pBit = pReq->pData;
    wBase = wAdd - pReq->pDef->wAddr;
    for (i=0; i<wNb; i++)
        _T5MBSrv_SetBit (pBit, (T5_WORD)(i+wBase),
                         _T5VMSrv_GetBit (pIn+7, i));

#ifdef T5DEF_MBSREADQ
    /* mark as changed */
    pBit = pReq->pData + pReq->pDef->wDataSize;
    for (i=0; i<wNb; i++)
        _T5MBSrv_SetBit (pBit, (T5_WORD)(i+wBase), TRUE);
#endif /*T5DEF_MBSREADQ*/

    /* echo frame */
    T5_MEMCPY (pOut+2, pIn+2, 4);
    return 6;
}

/*****************************************************************************
_T5MBSrv_SetBit
Set a bit
Parameters:
    pBase (IN) request buffer
    wOffset (IN) bit offset in the request
    bValue (OUT) bit value
*****************************************************************************/

static void _T5MBSrv_SetBit (T5_PTBYTE pBase,
                             T5_WORD wOffset, T5_BYTE bValue)
{
    pBase += (wOffset / 8);
    if (bValue)
        *pBase |= _T5_MASK8[wOffset % 8];
    else
        *pBase &= ~_T5_MASK8[wOffset % 8];
}

/*****************************************************************************
_T5VMSrv_GetBit
Get a bit
Parameters:
    pBase (IN) request buffer
    wOffset (IN) bit offset in the request
Return: bit value
*****************************************************************************/

static T5_BYTE _T5VMSrv_GetBit (T5_PTBYTE pBase, T5_WORD wOffset)
{
    pBase += (wOffset / 8);
    return (T5_BYTE)((*pBase & _T5_MASK8[wOffset % 8]) != 0);
}

/****************************************************************************/

#ifdef T5MB_EMULATE_RAPSS

static T5_WORD _T5MBSrv_ServeRAPSS (T5PTR_MBCLIENT pServer, T5_PTBYTE pIn, T5_PTBYTE pOut)
{
    T5_WORD i;
    T5_DWORD dwStamp;

    dwStamp = T5RtClk_GetCurTimeStamp ();

    *pOut++ = pIn[0];
    *pOut++ = pIn[1];
    *pOut++ = (16 * 6) + 2;
    for (i=0; i<16; i++)
    {
        *pOut++ = 0;
        *pOut++ = 0;
        T5_COPYFRAMEDWORD (pOut, &dwStamp);
        pOut += 4;
    }
    *pOut++ = 0;
    *pOut++ = 0;
    return 3 + (16 * 6) + 2;
}

#endif /*T5MB_EMULATE_RAPSS*/

/*****************************************************************************
_T5MBSrv_Serve08
Serve a request #08 - sub function 0 only
Parameters:
    pServer (IN) server object
    pIn (IN) received request
    pOut (OUT) answer buffer
Return: answer length
*****************************************************************************/

static T5_WORD _T5MBSrv_Serve08 (T5PTR_MBCLIENT pServer, T5_PTBYTE pIn, T5_PTBYTE pOut)
{
    T5_WORD wSub, wLen;

    T5_COPYFRAMEWORD (&wSub, pIn + 2);
    switch (wSub)
    {
    case 0 : /* echo */
        T5_MEMCPY (pOut, pIn, 6);
        wLen = 6;
        break;
    default : /* not supported */
        pOut[0] = pIn[0];
        pOut[1] = 0x88;
        pOut[2] = 1;
        wLen = 3;
    }
    return wLen;
}

/*****************************************************************************
_T5MBSrv_Serve43
Serve a request #43
Parameters:
    pServer (IN) server object
    pIn (IN) received request
    pOut (OUT) answer buffer
Return: answer length
*****************************************************************************/

/* MODBUS device identification */
#ifndef T5DEF_MODBUSFIXEDIDENT
static T5_CHAR _szMBVendor[52] = { '\0' };
static T5_CHAR _szMBProduct[52] = { '\0' };
static T5_CHAR _szMBRevision[52] = { '\0' };
static T5_WORD _wMBSpecific[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
#endif /*T5DEF_MODBUSFIXEDIDENT*/

static T5_WORD _T5MBSrv_Serve43 (T5PTR_MBCLIENT pServer, T5_PTBYTE pIn, T5_PTBYTE pOut)
{
    T5_BOOL bError;
    T5_WORD wLen;
    T5_BYTE bLen;

    /* echo slave number, function code and MEI type/code */
    bError = FALSE;
    wLen = 5;
    pOut[0] = pIn[0];
    pOut[1] = pIn[1];
    pOut[2] = pIn[2];
    pOut[3] = pIn[3];
    pOut[4] = pIn[3];

    /* check arguments */
    if (pIn[2] != 14)
    bError = TRUE;

    switch (pIn[3])
    {
    case 1 : /* basic - stream */
        if (pIn[4] > 2)
            bError = TRUE;
        else
        {
            pOut[wLen++] = 0; /* no more follows */
            pOut[wLen++] = 0; /* no more follows */
            pOut[wLen++] = (3 - pIn[4]); /* nb objects */
            /* vendor name */
            if (pIn[4] < 1)
            {
                T5_PTCHAR szVendor = (T5_PTCHAR)"COPA-DATA France";
#ifndef T5DEF_MODBUSFIXEDIDENT
                if (*_szMBVendor != '\0')
                    szVendor = _szMBVendor;
#endif /*T5DEF_MODBUSFIXEDIDENT*/
                bLen = (T5_BYTE)T5_STRLEN (szVendor);
                pOut[wLen++] = 0;
                pOut[wLen++] = bLen;
                T5_MEMCPY (pOut + wLen, szVendor, bLen);
                wLen += (T5_WORD)bLen;
            }
            /* product code */
            if (pIn[4] < 2)
            {
                T5_PTCHAR szProduct = (T5_PTCHAR)"T5";
#ifndef T5DEF_MODBUSFIXEDIDENT
                if (*_szMBProduct != '\0')
                    szProduct = _szMBProduct;
#endif /*T5DEF_MODBUSFIXEDIDENT*/
                bLen = (T5_BYTE)T5_STRLEN (szProduct);
                pOut[wLen++] = 1;
                pOut[wLen++] = bLen;
                T5_MEMCPY (pOut + wLen, szProduct, bLen);
                wLen += (T5_WORD)bLen;
            }
            /* revision */
            {
                T5_PTCHAR szRev = T5_DKVERSION;
#ifndef T5DEF_MODBUSFIXEDIDENT
                if (*_szMBRevision != '\0')
                    szRev = _szMBRevision;
#endif /*T5DEF_MODBUSFIXEDIDENT*/
                bLen = (T5_BYTE)T5_STRLEN (szRev);
                pOut[wLen++] = 2;
                pOut[wLen++] = bLen;
                T5_MEMCPY (pOut + wLen, szRev, bLen);
                wLen += (T5_WORD)bLen;
            }
        }
        break;
    case 4 : /* extended - individual */
        if (pIn[4] < 0x80 || pIn[4] > 0x87)
            bError = TRUE;
        else
        {
            pOut[4] = 0x83;
            pOut[wLen++] = 0; /* no more follows */
            pOut[wLen++] = 0; /* no more follows */
            pOut[wLen++] = 1; /* nb objects */
            pOut[wLen++] = pIn[4];
            pOut[wLen++] = 2;
#ifndef T5DEF_MODBUSFIXEDIDENT
            T5_COPYFRAMEWORD(pOut+wLen, &(_wMBSpecific[pIn[4] - 0x80]));
            wLen += 2;
#else /*T5DEF_MODBUSFIXEDIDENT*/
            pOut[wLen++] = 0;
            pOut[wLen++] = 0;
#endif /*T5DEF_MODBUSFIXEDIDENT*/
        }
        break;
    default :
        bError = TRUE;
        break;
    }

    if (bError)
    {
        pOut[1] |= 0x80;
        pOut[3] = 1;
        wLen = 4;
    }
    return wLen;
}

/*****************************************************************************
T5MBSrv_SetDevIdent
Set Device Identification data
Parameters:
    szVendor (IN) vendor name
    szProduct (IN) product name
    szRevision (IN) product revision
    pwExt (IN) extend data words
    wExtCount (IN) number of extend data words
Return: RUE if OK
*****************************************************************************/

T5_BOOL T5MBSrv_SetDevIdent (T5_PTCHAR szVendor, T5_PTCHAR szProduct,
                             T5_PTCHAR szRevision, T5_PTWORD pwExt, T5_WORD wExtCount)
{
#ifndef T5DEF_MODBUSFIXEDIDENT
    T5_WORD wLen;

    wLen = T5_STRLEN (szVendor);
    if (wLen < 52)
        T5_STRCPY (_szMBVendor, szVendor);
    else
    {
        T5_MEMCPY (_szMBVendor, szVendor, 51);
        _szMBVendor[51] = '\0';
    }

    wLen = T5_STRLEN (szProduct);
    if (wLen < 52)
        T5_STRCPY (_szMBProduct, szProduct);
    else
    {
        T5_MEMCPY (_szMBProduct, szProduct, 51);
        _szMBProduct[51] = '\0';
    }

    wLen = T5_STRLEN (szRevision);
    if (wLen < 52)
        T5_STRCPY (_szMBRevision, szRevision);
    else
    {
        T5_MEMCPY (_szMBRevision, szRevision, 51);
        _szMBRevision[51] = '\0';
    }

    if (pwExt == NULL)
        T5_MEMSET (_wMBSpecific, 0, sizeof (_wMBSpecific));
    else
    {
        if (wExtCount > 8)
            wExtCount = 8;
        T5_MEMCPY (_wMBSpecific, pwExt, wExtCount * sizeof (T5_WORD));
    }
    return TRUE;
#endif /*T5DEF_MODBUSFIXEDIDENT*/
    return FALSE;
}

/****************************************************************************/

#endif /*T5DEF_MODBUSIOS*/

/* eof **********************************************************************/
