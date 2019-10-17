/*****************************************************************************
T5VMiox.c :  DB access functions for use in I/O drivers

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/

void T5IOX_WriteChanBool (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                          T5_WORD wChan, T5_BOOL bValue)
{
    T5_PTBOOL pData8;
    T5_PTBYTE pLock;

    pLock = T5GET_DBLOCK(pDB);
    if (wChan < pIO->wNbChan
        && (pLock[pIO->wVA+wChan] & T5LOCK_DATA8) == 0)
    {
        pData8 = (T5_PTBOOL)T5GET_DBDATA8(pDB);
        pData8[pIO->wVA+wChan] = bValue;
    }
}

T5_BOOL T5IOX_ReadChanBool (T5PTR_CODEIO pIO, T5PTR_DB pDB, T5_WORD wChan)
{
    T5_PTBOOL pData8;

    if (wChan >= pIO->wNbChan)
        return FALSE;
    pData8 = (T5_PTBOOL)T5GET_DBDATA8(pDB);
    return pData8[pIO->wVA+wChan];
}

/****************************************************************************/

#ifdef T5DEF_SINTSUPPORTED

void T5IOX_WriteChanSint (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                          T5_WORD wChan, T5_CHAR cValue)
{
    T5_PTCHAR pData8;
    T5_PTBYTE pLock;

    pLock = T5GET_DBLOCK(pDB);
    if (wChan < pIO->wNbChan
        && (pLock[pIO->wVA+wChan] & T5LOCK_DATA8) == 0)
    {
        pData8 = (T5_PTCHAR)T5GET_DBDATA8(pDB);
        pData8[pIO->wVA+wChan] = cValue;
    }
}

T5_CHAR T5IOX_ReadChanSint (T5PTR_CODEIO pIO, T5PTR_DB pDB, T5_WORD wChan)
{
    T5_PTCHAR pData8;

    if (wChan >= pIO->wNbChan)
        return 0;
    pData8 = (T5_PTCHAR)T5GET_DBDATA8(pDB);
    return pData8[pIO->wVA+wChan];
}

#endif /*T5DEF_SINTSUPPORTED*/

/****************************************************************************/

#ifdef T5DEF_DATA16SUPPORTED

void T5IOX_WriteChanInt (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                         T5_WORD wChan, T5_SHORT iValue)
{
    T5_PTSHORT pData16;
    T5_PTBYTE pLock;

    pLock = T5GET_DBLOCK(pDB);
    if (wChan < pIO->wNbChan
        && (pLock[pIO->wVA+wChan] & T5LOCK_DATA16) == 0)
    {
        pData16 = (T5_PTSHORT)T5GET_DBDATA16(pDB);
        pData16[pIO->wVA+wChan] = iValue;
    }
}

T5_SHORT T5IOX_ReadChanInt (T5PTR_CODEIO pIO, T5PTR_DB pDB, T5_WORD wChan)
{
    T5_PTSHORT pData16;

    if (wChan >= pIO->wNbChan)
        return 0;
    pData16 = (T5_PTSHORT)T5GET_DBDATA16(pDB);
    return pData16[pIO->wVA+wChan];
}

#endif /*T5DEF_DATA16SUPPORTED*/

/****************************************************************************/

void T5IOX_WriteChanDint (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                          T5_WORD wChan, T5_LONG lValue)
{
    T5_PTLONG pData32;
#ifdef T5DEF_REALSUPPORTED
    T5_PTREAL pReal;
#endif /*T5DEF_REALSUPPORTED*/
    T5_PTBYTE pLock;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTBYTE pTypeMasks;

    pLock = T5GET_DBLOCK(pDB);
    if (wChan < pIO->wNbChan
        && (pLock[pIO->wVA+wChan] & T5LOCK_DATA32) == 0)
    {
        pPrivate = T5GET_DBPRIVATE(pDB);
        pTypeMasks = pPrivate->pTypeMasks;
        if (T5_ISD32REAL(pTypeMasks,pIO->wVA+wChan))
        {
#ifdef T5DEF_REALSUPPORTED
            pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
            pReal[pIO->wVA+wChan] = (T5_REAL)lValue;
#endif /*T5DEF_REALSUPPORTED*/
        }
        else
        {
            pData32 = (T5_PTLONG)T5GET_DBDATA32(pDB);
            pData32[pIO->wVA+wChan] = lValue;
        }
    }
}

T5_LONG T5IOX_ReadChanDint (T5PTR_CODEIO pIO, T5PTR_DB pDB, T5_WORD wChan)
{
    T5_PTLONG pData32;
#ifdef T5DEF_REALSUPPORTED
    T5_PTREAL pReal;
#endif /*T5DEF_REALSUPPORTED*/
    T5PTR_DBPRIVATE pPrivate;
    T5_PTBYTE pTypeMasks;

    pData32 = (T5_PTLONG)T5GET_DBDATA32(pDB);
#ifdef T5DEF_REALSUPPORTED
    pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
#endif /*T5DEF_REALSUPPORTED*/
    pPrivate = T5GET_DBPRIVATE(pDB);
    pTypeMasks = pPrivate->pTypeMasks;
    if (wChan >= pIO->wNbChan)
        return 0L;
    if (T5_ISD32REAL(pTypeMasks,pIO->wVA+wChan))
    {
#ifdef T5DEF_REALSUPPORTED
        return (T5_LONG)(pReal[pIO->wVA+wChan]);
#else /*T5DEF_REALSUPPORTED*/
        return 0L;
#endif /*T5DEF_REALSUPPORTED*/
    }
    else
        return (pData32[pIO->wVA+wChan]);
}

/****************************************************************************/

#ifdef T5DEF_REALSUPPORTED

void T5IOX_WriteChanReal (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                          T5_WORD wChan, T5_REAL rValue)
{
    T5_PTLONG pData32;
    T5_PTREAL pReal;
    T5_PTBYTE pLock;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTBYTE pTypeMasks;

    pData32 = (T5_PTLONG)T5GET_DBDATA32(pDB);
    pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    pLock = T5GET_DBLOCK(pDB);
    pPrivate = T5GET_DBPRIVATE(pDB);
    pTypeMasks = pPrivate->pTypeMasks;
    if (wChan < pIO->wNbChan
        && (pLock[pIO->wVA+wChan] & T5LOCK_DATA32) == 0)
    {
        if (T5_ISD32REAL(pTypeMasks,pIO->wVA+wChan))
            pReal[pIO->wVA+wChan] = rValue;
        else
            pData32[pIO->wVA+wChan] = (T5_LONG)rValue;
    }
}

T5_REAL T5IOX_ReadChanReal (T5PTR_CODEIO pIO, T5PTR_DB pDB, T5_WORD wChan)
{
    T5_PTLONG pData32;
    T5_PTREAL pReal;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTBYTE pTypeMasks;

    pData32 = (T5_PTLONG)T5GET_DBDATA32(pDB);
    pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    pPrivate = T5GET_DBPRIVATE(pDB);
    pTypeMasks = pPrivate->pTypeMasks;
    if (wChan >= pIO->wNbChan)
        return T5_ZEROREAL;
    if (T5_ISD32REAL(pTypeMasks,pIO->wVA+wChan))
        return (pReal[pIO->wVA+wChan]);
    else
        return (T5_REAL)(pData32[pIO->wVA+wChan]);
}

#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/

#ifdef T5DEF_LINTSUPPORTED

void T5IOX_WriteChanLint (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                          T5_WORD wChan, T5_LINT lValue)
{
    T5_PTLINT pData64;
#ifdef T5DEF_LREALSUPPORTED
    T5_PTLREAL pLReal;
#endif /*T5DEF_LREALSUPPORTED*/
    T5_PTBYTE pLock;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTBYTE pTypeMasks;

    pData64 = (T5_PTLINT)T5GET_DBDATA64(pDB);
#ifdef T5DEF_LREALSUPPORTED
    pLReal = (T5_PTLREAL)T5GET_DBDATA64(pDB);
#endif /*T5DEF_LREALSUPPORTED*/
    pLock = T5GET_DBLOCK(pDB);
    pPrivate = T5GET_DBPRIVATE(pDB);
    pTypeMasks = pPrivate->pTypeMasks;
    if (wChan < pIO->wNbChan
        && (pLock[pIO->wVA+wChan] & T5LOCK_DATA64) == 0)
    {
        if (T5_ISD64LREAL(pTypeMasks,pIO->wVA+wChan))
        {
#ifdef T5DEF_LREALSUPPORTED
            pLReal[pIO->wVA+wChan] = (T5_LREAL)lValue;
#endif /*T5DEF_LREALSUPPORTED*/
        }
        else
            pData64[pIO->wVA+wChan] = lValue;
    }
}

T5_LINT T5IOX_ReadChanLint (T5PTR_CODEIO pIO, T5PTR_DB pDB, T5_WORD wChan)
{
    T5_PTLINT pData64;
#ifdef T5DEF_LREALSUPPORTED
    T5_PTLREAL pLReal;
#endif /*T5DEF_LREALSUPPORTED*/
    T5PTR_DBPRIVATE pPrivate;
    T5_PTBYTE pTypeMasks;

    pData64 = (T5_PTLINT)T5GET_DBDATA64(pDB);
#ifdef T5DEF_LREALSUPPORTED
    pLReal = (T5_PTLREAL)T5GET_DBDATA64(pDB);
#endif /*T5DEF_LREALSUPPORTED*/
    pPrivate = T5GET_DBPRIVATE(pDB);
    pTypeMasks = pPrivate->pTypeMasks;
    if (wChan >= pIO->wNbChan)
        return (T5_LINT)0;
    if (T5_ISD64LREAL(pTypeMasks,pIO->wVA+wChan))
    {
#ifdef T5DEF_LREALSUPPORTED
        return (T5_LINT)(pLReal[pIO->wVA+wChan]);
#else /*T5DEF_LREALSUPPORTED*/
        return (T5_LINT)0;
#endif /*T5DEF_LREALSUPPORTED*/
    }
    else
        return (pData64[pIO->wVA+wChan]);
}

#endif /*T5DEF_LINTSUPPORTED*/

/****************************************************************************/

#ifdef T5DEF_LREALSUPPORTED

void T5IOX_WriteChanLReal (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                           T5_WORD wChan, T5_LREAL lValue)
{
#ifdef T5DEF_LINTSUPPORTED
    T5_PTLINT pData64;
#endif /*T5DEF_LINTSUPPORTED*/
    T5_PTLREAL pLReal;
    T5_PTBYTE pLock;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTBYTE pTypeMasks;

#ifdef T5DEF_LINTSUPPORTED
    pData64 = (T5_PTLINT)T5GET_DBDATA64(pDB);
#endif /*T5DEF_LINTSUPPORTED*/
    pLReal = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    pLock = T5GET_DBLOCK(pDB);
    pPrivate = T5GET_DBPRIVATE(pDB);
    pTypeMasks = pPrivate->pTypeMasks;
    if (wChan < pIO->wNbChan
        && (pLock[pIO->wVA+wChan] & T5LOCK_DATA64) == 0)
    {
        if (T5_ISD64LREAL(pTypeMasks,pIO->wVA+wChan))
            pLReal[pIO->wVA+wChan] = lValue;
        else
        {
#ifdef T5DEF_LINTSUPPORTED
            pData64[pIO->wVA+wChan] = (T5_LINT)lValue;
#endif /*T5DEF_LINTSUPPORTED*/
        }
    }
}

T5_LREAL T5IOX_ReadChanLReal (T5PTR_CODEIO pIO, T5PTR_DB pDB, T5_WORD wChan)
{
#ifdef T5DEF_LINTSUPPORTED
    T5_PTLINT pData64;
#endif /*T5DEF_LINTSUPPORTED*/
    T5_PTLREAL pLReal;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTBYTE pTypeMasks;

#ifdef T5DEF_LINTSUPPORTED
    pData64 = (T5_PTLINT)T5GET_DBDATA64(pDB);
#endif /*T5DEF_LINTSUPPORTED*/
    pLReal = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    pPrivate = T5GET_DBPRIVATE(pDB);
    pTypeMasks = pPrivate->pTypeMasks;
    if (wChan >= pIO->wNbChan)
        return T5_ZEROLREAL;
    if (T5_ISD64LREAL(pTypeMasks,pIO->wVA+wChan))
        return (pLReal[pIO->wVA+wChan]);
    else
    {
#ifdef T5DEF_LINTSUPPORTED
        return (T5_LREAL)(pData64[pIO->wVA+wChan]);
#else /*T5DEF_LINTSUPPORTED*/
        return T5_ZEROLREAL;
#endif /*T5DEF_LINTSUPPORTED*/
    }
}

#endif /*T5DEF_LREALSUPPORTED*/

/****************************************************************************/

#ifdef T5DEF_STRINGSUPPORTED

void T5IOX_WriteChanBinString (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                               T5_WORD wChan, T5_PTBYTE sValue)
{
    T5_PTBYTE pLock;
    T5_PTBYTE *pStr;
    T5_PTBYTE pString;
    T5_BYTE bLen;

    pLock = T5GET_DBLOCK(pDB);
    if (wChan < pIO->wNbChan
        && (pLock[pIO->wVA+wChan] & T5LOCK_STRING) == 0)
    {
        pStr = T5GET_DBSTRING(pDB);
        pString = pStr[pIO->wVA+wChan];
        bLen = *sValue; /* source current length */
        if (bLen > *pString) bLen = *pString; /* adjust to max if too long */
        pString[1] = bLen; /* copy current length */
        T5_MEMCPY (pString+2, sValue+1, bLen); /* copy characters */
        pString[2+bLen] = 0; /* eos mark */
    }
}

void T5IOX_WriteChanCString (T5PTR_CODEIO pIO, T5PTR_DB pDB,
                             T5_WORD wChan, T5_PTCHAR sValue)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pString;
    T5_PTBYTE pLock;
    T5_BYTE bLen;

    pLock = T5GET_DBLOCK(pDB);
    if (wChan < pIO->wNbChan
        && (pLock[pIO->wVA+wChan] & T5LOCK_STRING) == 0)
    {
        pStr = T5GET_DBSTRING(pDB);
        pString = pStr[pIO->wVA+wChan];
        
        bLen = (T5_BYTE)T5_STRLEN(sValue); /* source length */
        if (bLen > *pString) bLen = *pString; /* adjust to max if too long */
        pString[1] = bLen; /* copy current length */
        T5_MEMCPY (pString+2, sValue, bLen); /* copy characters */
        pString[2+bLen] = 0; /* eos mark */
    }
}

T5_PTBYTE T5IOX_ReadChanBinString (T5PTR_CODEIO pIO, T5PTR_DB pDB, T5_WORD wChan)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pString;

    if (wChan >= pIO->wNbChan)
        return (T5_PTBYTE)("\0");
    pStr = T5GET_DBSTRING(pDB);
    pString = pStr[pIO->wVA+wChan];
    return (pString + 1); /* points to the current length */
}

T5_PTCHAR T5IOX_ReadChanCString (T5PTR_CODEIO pIO, T5PTR_DB pDB, T5_WORD wChan)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pString;

    if (wChan >= pIO->wNbChan)
        return (T5_PTCHAR)"\0";
    pStr = T5GET_DBSTRING(pDB);
    pString = pStr[pIO->wVA+wChan];
    pString[2+pString[1]] = 0; /* ensure eos mark is here */
    return (T5_PTCHAR)(pString + 2); /* points to the first character */
}

#endif /*T5DEF_STRINGSUPPORTED*/

/* eof **********************************************************************/
