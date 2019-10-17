/*****************************************************************************
T5tools.c  : basic calls to VM DB

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/*****************************************************************************
T5Tools_GetAnyParam
Get the address in the VMDB of a parameter identified by an index outside CTSEG
Parameters:
    pDB (IN) pointer to the VMDB
    wTicType (IN) variable type
    wIndex (IN) variable index
    pbLock (OUT) TRUE if the variable is currently locked
    pwSizeof (OUT) size of the variable in bytes
return: pointer to the variable or NULL if invalid parameters
*****************************************************************************/

T5_PTR T5Tools_GetAnyParam (T5PTR_DB pDB, T5_WORD wTicType, T5_WORD wIndex,
                            T5_PTBOOL pbLock, T5_PTWORD pwSizeof)
{
    T5_PTBYTE pData8, pLock;
    T5_PTDWORD pData32;
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pData16;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 pData64;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE *pString;
#endif /*T5DEF_STRINGSUPPORTED*/

    if (pbLock != NULL)
        pLock = T5GET_DBLOCK(pDB);
    else
        pLock = NULL;
    switch (wTicType)
    {
    case T5C_BOOL :
    case T5C_SINT :
    case T5C_USINT :
        pData8 = T5GET_DBDATA8(pDB);
        if (pbLock != NULL)
            *pbLock = ((pLock[wIndex] & T5LOCK_DATA8) != 0);
        if (pwSizeof)
            *pwSizeof = 1;
        return pData8 + wIndex;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
    case T5C_UINT :
        pData16 = T5GET_DBDATA16(pDB);
        if (pbLock != NULL)
            *pbLock = ((pLock[wIndex] & T5LOCK_DATA16) != 0);
        if (pwSizeof)
            *pwSizeof = 2;
        return pData16 + wIndex;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_UDINT :
    case T5C_REAL :
        pData32 = T5GET_DBDATA32(pDB);
        if (pbLock != NULL)
            *pbLock = ((pLock[wIndex] & T5LOCK_DATA32) != 0);
        if (pwSizeof)
            *pwSizeof = 4;
        return pData32 + wIndex;
    case T5C_TIME :
        pData32 = T5GET_DBTIME(pDB);
        if (pbLock != NULL)
            *pbLock = ((pLock[wIndex] & T5LOCK_TIME) != 0);
        if (pwSizeof)
            *pwSizeof = 4;
        return pData32 + wIndex;
#ifdef T5DEF_DATA64SUPPORTED
    case T5C_LINT :
    case T5C_ULINT :
    case T5C_LREAL :
        pData64 = T5GET_DBDATA64(pDB);
        if (pbLock != NULL)
            *pbLock = ((pLock[wIndex] & T5LOCK_DATA64) != 0);
        if (pwSizeof)
            *pwSizeof = 8;
        return pData64 + wIndex;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
    case T5C_STRING :
        pString = T5GET_DBSTRING(pDB);
        if (pbLock != NULL)
            *pbLock = ((pLock[wIndex] & T5LOCK_STRING) != 0);
        if (pwSizeof)
            *pwSizeof = (T5_WORD)(pString[wIndex][0]) + 3;
        return pString[wIndex];
#endif /*T5DEF_STRINGSUPPORTED*/
    default : break;
    }
    return NULL;
}

/*****************************************************************************
T5Tools_GetAnyArray
Get the base address of an array - not in CTSEG segment
Parameters:
    pDB (IN) pointer to the VMDB
    wTicType (IN) array data type
    wIndex (IN) array base index
return: pointer to the array or NULL if invalid parameters
*****************************************************************************/

T5_PTR T5Tools_GetAnyArray (T5PTR_DB pDB, T5_WORD wTicType, T5_WORD wIndex)
{
    T5_PTBYTE pData8;
    T5_PTDWORD pData32;
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pData16;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 pData64;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE *pString;
#endif /*T5DEF_STRINGSUPPORTED*/

#ifdef T5DEF_CTSEG
    T5_PTBYTE pCT;

    pCT = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    if (pDB[T5TB_CTSEG].dwRawSize)
    {
        pData32 = T5GET_DBDATA32(pDB);
        return (pCT + pData32[wIndex]);
    }
#endif /*T5DEF_CTSEG*/

    switch (wTicType)
    {
    case T5C_BOOL :
    case T5C_SINT :
    case T5C_USINT :
        pData8 = T5GET_DBDATA8(pDB);
        return pData8 + wIndex;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
    case T5C_UINT :
        pData16 = T5GET_DBDATA16(pDB);
        return pData16 + wIndex;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_UDINT :
    case T5C_REAL :
        pData32 = T5GET_DBDATA32(pDB);
        return pData32 + wIndex;
    case T5C_TIME :
        pData32 = T5GET_DBTIME(pDB);
        return pData32 + wIndex;
#ifdef T5DEF_DATA64SUPPORTED
    case T5C_LINT :
    case T5C_ULINT :
    case T5C_LREAL :
        pData64 = T5GET_DBDATA64(pDB);
        return pData64 + wIndex;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
    case T5C_STRING :
        pString = T5GET_DBSTRING(pDB);
        return pString[wIndex];
#endif /*T5DEF_STRINGSUPPORTED*/
    default : break;
    }
    return NULL;
}

/*****************************************************************************
T5Tools_GetAnyArray
Get the base address of a referenced item in CTSEG segment
Parameters:
    pDB (IN) pointer to the VMDB
    wOffset (IN) array of the DINT variable containing the reference
return: pointer to the variable or NULL if invalid parameters
*****************************************************************************/

T5_PTR T5Tools_GetCTParam (T5PTR_DB pDB, T5_WORD wOffset) /* CTSeg only! */
{
    T5_PTBYTE pArray;
#ifdef T5DEF_CTSEG
    T5_PTBYTE pCT;
    T5_PTDWORD pData32;
#endif /*T5DEF_CTSEG*/

    pArray = NULL;
#ifdef T5DEF_CTSEG
    pCT = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    if (pDB[T5TB_CTSEG].dwRawSize)
    {
        pData32 = T5GET_DBDATA32(pDB);
        pArray = (pCT + pData32[wOffset]);
    }
#endif /*T5DEF_CTSEG*/
    return (T5_PTR)pArray;
}

/*****************************************************************************
T5Tools_GetD??Array
Get the base address of an array - takes care of CTSEG option
Parameters:
    pDB (IN) pointer to the VMDB
    wOffset (IN) array index outside CTSEG or reference DINT variable
return: pointer to the array or NULL if invalid parameters
*****************************************************************************/

T5_PTBYTE T5Tools_GetD8Array (T5PTR_DB pDB, T5_WORD wOffset)
{
    T5_PTBYTE pArray;

#ifdef T5DEF_CTSEG
    T5_PTBYTE pCT;
    T5_PTDWORD pData32;

    pCT = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    if (pDB[T5TB_CTSEG].dwRawSize)
    {
        pData32 = T5GET_DBDATA32(pDB);
        pArray = (pCT + pData32[wOffset]);
    }
    else
#endif /*T5DEF_CTSEG*/
    {
        pArray = T5GET_DBDATA8(pDB);
        pArray += wOffset;
    }
    return pArray;
}

#ifdef T5DEF_DATA16SUPPORTED

T5_PTWORD T5Tools_GetD16Array (T5PTR_DB pDB, T5_WORD wOffset)
{
    T5_PTWORD pArray;

#ifdef T5DEF_CTSEG
    T5_PTBYTE pCT;
    T5_PTDWORD pData32;

    pCT = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    if (pDB[T5TB_CTSEG].dwRawSize)
    {
        pData32 = T5GET_DBDATA32(pDB);
        pArray = (T5_PTWORD)(pCT + pData32[wOffset]);
    }
    else
#endif /*T5DEF_CTSEG*/
    {
        pArray = T5GET_DBDATA16(pDB);
        pArray += wOffset;
    }
    return pArray;
}

#endif /*T5DEF_DATA16SUPPORTED*/

T5_PTDWORD T5Tools_GetD32Array (T5PTR_DB pDB, T5_WORD wOffset)
{
    T5_PTDWORD pArray;

#ifdef T5DEF_CTSEG
    T5_PTBYTE pCT;
    T5_PTDWORD pData32;

    pCT = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    if (pDB[T5TB_CTSEG].dwRawSize)
    {
        pData32 = T5GET_DBDATA32(pDB);
        pArray = (T5_PTDWORD)(pCT + pData32[wOffset]);
    }
    else
#endif /*T5DEF_CTSEG*/
    {
        pArray = T5GET_DBDATA32(pDB);
        pArray += wOffset;
    }
    return pArray;
}

T5_PTDWORD T5Tools_GetTimeArray (T5PTR_DB pDB, T5_WORD wOffset)
{
    T5_PTDWORD pArray;

#ifdef T5DEF_CTSEG
    T5_PTBYTE pCT;
    T5_PTDWORD pData32;

    pCT = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    if (pDB[T5TB_CTSEG].dwRawSize)
    {
        pData32 = T5GET_DBDATA32(pDB);
        pArray = (T5_PTDWORD)(pCT + pData32[wOffset]);
    }
    else
#endif /*T5DEF_CTSEG*/
    {
        pArray = T5GET_DBTIME(pDB);
        pArray += wOffset;
    }
    return pArray;
}

#ifdef T5DEF_DATA64SUPPORTED

T5_PTDATA64 T5Tools_GetD64Array (T5PTR_DB pDB, T5_WORD wOffset)
{
    T5_PTDATA64 pArray;

#ifdef T5DEF_CTSEG
    T5_PTBYTE pCT;
    T5_PTDWORD pData32;

    pCT = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    if (pDB[T5TB_CTSEG].dwRawSize)
    {
        pData32 = T5GET_DBDATA32(pDB);
        pArray = (T5_PTDATA64)(pCT + pData32[wOffset]);
    }
    else
#endif /*T5DEF_CTSEG*/
    {
        pArray = T5GET_DBDATA64(pDB);
        pArray += wOffset;
    }
    return pArray;
}

#endif /*T5DEF_DATA64SUPPORTED*/

#ifdef T5DEF_STRINGSUPPORTED

T5_PTR T5Tools_GetStringArray (T5PTR_DB pDB, T5_WORD wOffset, T5_PTBOOL pbRaw)
{
    T5_PTBYTE pArray, *pString;

#ifdef T5DEF_CTSEG
    T5_PTBYTE pCT;
    T5_PTDWORD pData32;

    pCT = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    if (pDB[T5TB_CTSEG].dwRawSize)
    {
        pData32 = T5GET_DBDATA32(pDB);
        pArray = pCT + pData32[wOffset];
        if (pbRaw)
            *pbRaw = TRUE;
    }
    else
#endif /*T5DEF_CTSEG*/
    {
        pString = T5GET_DBSTRING(pDB);
        pString += wOffset;
        pArray = (T5_PTBYTE)pString;
        if (pbRaw)
            *pbRaw = FALSE;
    }
    return (T5_PTR)pArray;
}

#endif /*T5DEF_STRINGSUPPORTED*/

/*****************************************************************************
T5Tools_Crc16
Calculates a CRC16
Parameters:
    pBuffer (IN) data buffer
    wLen (IN) data length
return: CRC16
*****************************************************************************/

T5_WORD T5Tools_Crc16 (T5_PTBYTE pBuffer, T5_WORD wLen)
{
    T5_WORD i, j;
    T5_WORD wCrc;

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

/* eof **********************************************************************/
