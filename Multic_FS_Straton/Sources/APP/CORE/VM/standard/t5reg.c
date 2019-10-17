/*****************************************************************************
T5Reg.c :    T5 Registry - main core

DO NOT ALTER THIS !

(c) COPALP 2008
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/

#ifdef T5DEF_REG

/*****************************************************************************
_T5Reg_GetCRC
Calculate a CRC32 on raw memory
Parameters:
    pCode (IN) pointer to memory
    dwSize (IN) number of bytes to be checked
Return: CRC32
*****************************************************************************/

static T5_DWORD _T5Reg_GetCRC (T5_PTBYTE pCode, T5_DWORD dwSize)
{
    T5_DWORD dwCalcCrc;
    T5_DWORD b, i, mem1;
 
    dwCalcCrc = 0xffffffffL;
    for (b=0; b<dwSize; b++)
    {
        dwCalcCrc ^= ((T5_DWORD)(*pCode) & 0xffL);
        for (i=1; i<= 8; i++)
        {
            mem1 = dwCalcCrc;
            dwCalcCrc /= 2;
            if (mem1 & 1) dwCalcCrc ^= 0xa000a001;
        }
        pCode++; 
    }
    return dwCalcCrc;
}

/*****************************************************************************
T5Reg_CheckRegistry
Check registry consistency (size, checksum and endian)
Parameters:
    pReg (IN) pointer to registry in memory
    dwSize (IN) size of registry block in bytes
    bCheckEndian (IN) check endian if TRUE
Return: OK if registry is valid
*****************************************************************************/

T5_BOOL T5Reg_CheckRegistry (T5_PTR pReg, T5_DWORD dwSize, T5_BOOL bCheckEndian)
{
    T5_DWORD dwCrcCalc, dwCrcFile;

    if (pReg == NULL || dwSize < T5REGSIZE_HEAD)
        return FALSE;
    if (bCheckEndian)
    {
        if (T5_MEMCMP(pReg, T5REG_HEADER, T5_STRLEN(T5REG_HEADER)) != 0)
            return FALSE;
    }
    else
    {
        if (T5_MEMCMP(pReg, T5REG_HEADER_BE, T5_STRLEN(T5REG_HEADER)) != 0
            && T5_MEMCMP(pReg, T5REG_HEADER_LE, T5_STRLEN(T5REG_HEADER)) != 0)
            return FALSE;
    }

    dwCrcCalc = _T5Reg_GetCRC ((T5_PTBYTE)pReg, dwSize - 4);
    T5_COPYFRAMEDWORD (&dwCrcFile, ((T5_PTBYTE)pReg + dwSize - 4));
    if (dwCrcCalc != dwCrcFile)
        return FALSE;

    return TRUE;
}

/*****************************************************************************
T5Reg_GetXmlFileName
Get the prefix of the XML definition file used to build a registry
Parameters:
    pReg (IN) pointer to registry in memory
Return: pointer to XML file or "\0" if fail
*****************************************************************************/

T5_PTCHAR T5Reg_GetXmlFileName (T5_PTR pReg)
{
    T5PTR_T5REGHEAD pHead;

    if (pReg == NULL)
        return (T5_PTCHAR)"\0";

    pHead = (T5PTR_T5REGHEAD)pReg;
    if (pHead->pXML == 0L)
        return (T5_PTCHAR)"\0";
    return T5REG_GETSTRING (pHead, pHead->pXML);
}

/*****************************************************************************
T5Reg_GetAdminPassword
Get the administration password
Parameters:
    pReg (IN) pointer to registry in memory
Return: pointer to encrypted password or "\0" if none
*****************************************************************************/

T5_PTCHAR T5Reg_GetAdminPassword (T5_PTR pReg)
{
    T5PTR_T5REGHEAD pHead;

    if (pReg == NULL)
        return (T5_PTCHAR)"\0";

    pHead = (T5PTR_T5REGHEAD)pReg;
    if (pHead->pPassword == 0L)
        return (T5_PTCHAR)"\0";
    return T5REG_GETSTRING (pHead, pHead->pPassword);
}

/*****************************************************************************
T5Reg_UpdateCRC
Recalculate and update the CRC32 in memory
Parameters:
    pReg (IN) pointer to registry in memory
*****************************************************************************/

void T5Reg_UpdateCRC (T5_PTR pReg)
{
    T5PTR_T5REGHEAD pHead;
    T5_PTBYTE pCrc;
    T5_DWORD dwCrcCalc;

    if (pReg == NULL)
        return;

    pHead = (T5PTR_T5REGHEAD)pReg;
    dwCrcCalc = _T5Reg_GetCRC ((T5_PTBYTE)pReg, pHead->dwFullSize - 4);
    pCrc = (T5_PTBYTE)pReg;
    pCrc += (pHead->dwFullSize - 4);
    T5_COPYFRAMEDWORD (pCrc, &dwCrcCalc);
}

/*****************************************************************************
T5Reg_SymbolHashCodeLen / T5Reg_SymbolHashCode
Hash code function
Parameters:
    szName (IN) pointer to the string
    wLen (IN) specifies the string length if not null terminated
Return: hash code value
*****************************************************************************/

T5_WORD T5Reg_SymbolHashCodeLen (T5_PTBYTE szName, T5_WORD wLen)
{
	T5_WORD wHash;
    
    wHash = 0;
	while (wLen--)
		wHash = (wHash << 5) + wHash + (T5_WORD)(*szName++);
    wHash %= T5REG_HSIZE;
	return wHash;
}

T5_WORD T5Reg_SymbolHashCode (T5_PTBYTE szName)
{
    return T5Reg_SymbolHashCodeLen (szName, T5_STRLEN(szName));
}

/*****************************************************************************
_T5Reg_IsItYou
Compare an item for hash table search
Parameters:
    pReg (IN) pointer to registry in memory
    pItem (IN) pointer to item
    dwParent (IN) searched parent ID
    szName (IN) searched name
    wLen (IN) name string length
Return: TRUE if match
*****************************************************************************/

static T5_BOOL _T5Reg_IsItYou (T5_PTR pReg, T5PTR_T5REGITEM pItem,
                               T5_DWORD dwParent,
                               T5_PTCHAR szName, T5_WORD wLen)
{
    T5_PTCHAR pName;

    if (pItem->dwParent != dwParent)
        return FALSE;

    if (pItem->pName == 0L)
        return FALSE;

    pName = T5REG_GETSTRING(pReg, pItem->pName);
    if (T5_MEMCMP (pName, szName, wLen) != 0)
        return FALSE;
    if (pName[wLen] != '\0')
        return FALSE;

    return TRUE;
}

/*****************************************************************************
_T5Reg_Find
Serach for an item in hash table
Parameters:
    pReg (IN) pointer to registry in memory
    dwParent (IN) searched parent ID
    szName (IN) searched name
    wLen (IN) name string length
Return: pointer to found item or NULL if fail
*****************************************************************************/

static T5PTR_T5REGITEM _T5Reg_Find (T5_PTR pReg, T5_DWORD dwParent,
                                    T5_PTCHAR szName, T5_WORD wLen)
{
    T5_PTDWORD pdwHash;
    T5PTR_T5REGITEM pItem;
    T5_DWORD dwItem;

    pdwHash = T5REG_GETHASH(pReg);
    pdwHash += T5Reg_SymbolHashCodeLen ((T5_PTBYTE)szName, wLen);

    dwItem = *pdwHash;
    while (dwItem != 0L)
    {
        pItem = T5REG_GETBYID(pReg, dwItem);
        if (_T5Reg_IsItYou (pReg, pItem, dwParent, szName, wLen))
            return pItem;
        dwItem = pItem->dwNext;
    }
    return NULL;
}

/*****************************************************************************
T5Reg_FindItem
Search for an item by pathname in registry
Parameters:
    pReg (IN) pointer to registry in memory
    szPath (IN) searched pathname (case sensitive!)
Return: pointer to found item or NULL if fail
*****************************************************************************/

T5PTR_T5REGITEM T5Reg_FindItem (T5_PTR pReg, T5_PTCHAR szPath)
{
    T5_DWORD dwParent;
    T5PTR_T5REGITEM pItem;
    T5_WORD wLen;

    if (T5REG_ISSEP(*szPath)) /* accept one leading '/' */
        szPath++;

    dwParent = 0L;
    wLen = 0;
    pItem = NULL;
    while (*szPath != '\0')
    {
        wLen = 0;
        while (szPath[wLen] != '\0' && !T5REG_ISSEP(szPath[wLen]))
            wLen++;
        pItem = _T5Reg_Find (pReg, dwParent, szPath, wLen);
        if (pItem == NULL)
            return NULL;

        szPath += wLen;
        if (T5REG_ISSEP(*szPath))
        {
            if ((pItem->dwFlags & T5REGF_FOLDER) == 0L)
                return NULL;
            dwParent = pItem->dwID;
            szPath++;
        }
    }

    if (pItem == NULL || (pItem->dwFlags & (T5REGF_FOLDER|T5REGF_ROOT|T5REGF_LAST)) != 0)
        return NULL;

    return pItem;
}

/*****************************************************************************
T5Reg_GetItem
Search for an item by ID in registry
Parameters:
    pReg (IN) pointer to registry in memory
    ID (IN) searched item ID
Return: pointer to found item or NULL if fail
*****************************************************************************/

T5PTR_T5REGITEM T5Reg_GetItem (T5_PTR pReg, T5_DWORD dwID)
{
    T5PTR_T5REGHEAD pHead;

    pHead = (T5PTR_T5REGHEAD)pReg;
    if (pHead == NULL || dwID > pHead->dwNbRec)
        return NULL;
    return T5REG_GETBYID (pReg, dwID);
}

/*****************************************************************************
T5Reg_LockRegistry / T5Reg_UnlockRegistry
Lock or unlock registry for write
Parameters:
    pReg (IN) pointer to registry in memory
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5Reg_LockRegistry (T5_PTR pReg)
{
    T5PTR_T5REGHEAD pHead;

    pHead = (T5PTR_T5REGHEAD)pReg;
    if (pHead == NULL || (pHead->dwFlags & T5REG_LOCK) != 0L)
        return FALSE;

    pHead->dwFlags |= T5REG_LOCK;
    return TRUE;
}

T5_BOOL T5Reg_UnlockRegistry (T5_PTR pReg)
{
    T5PTR_T5REGHEAD pHead;

    pHead = (T5PTR_T5REGHEAD)pReg;
    if (pHead == NULL || (pHead->dwFlags & T5REG_LOCK) == 0L)
        return FALSE;

    pHead->dwFlags &= ~T5REG_LOCK;
    return TRUE;
}

/*****************************************************************************
T5Reg_IsRegistryLocked
Check if the registry is locked for write
Parameters:
    pReg (IN) pointer to registry in memory
Return: TRUE if locked
*****************************************************************************/

T5_BOOL T5Reg_IsRegistryLocked (T5_PTR pReg)
{
    T5PTR_T5REGHEAD pHead;

    pHead = (T5PTR_T5REGHEAD)pReg;
    if (pHead == NULL)
        return FALSE;
    
    return ((pHead->dwFlags & T5REG_LOCK) != 0L);
}

/*****************************************************************************
T5Reg_GetValue
Get the value of a parameter and copy it to a local buffer - NOT FOR STRINGS!
Parameters:
    pReg (IN) pointer to registry in memory
    pItem (IN) pointer to item
    pBuffer (OUT) where to copy data - can be NULL
    wSizeof (IN) sizeof of the buffer - for safety
Return: pointer to value
*****************************************************************************/

T5_PTR T5Reg_GetValue (T5_PTR pReg, T5PTR_T5REGITEM pItem, T5_PTR pBuffer,
                       T5_WORD wSizeof)
{
    T5_PTBYTE pValue;

    if (pItem == NULL || (pItem->dwFlags & T5REGF_FOLDER) != 0L)
        return NULL;
    if (pItem->bTicType == T5C_STRING)
        return NULL;

    pValue = T5REG_GETBYTES(pReg, pItem->pValue);
    if (pBuffer != NULL)
    {
        if (wSizeof < (T5_WORD)(pItem->bMaxLen))
            return NULL;
        T5_MEMCPY (pBuffer, pValue, pItem->bMaxLen);
    }
    return pValue;
}

/*****************************************************************************
T5Reg_GetString
Get the value of a STRING parameter
Parameters:
    pReg (IN) pointer to registry in memory
    pItem (IN) pointer to item
Return: pointer to string value in registry
*****************************************************************************/

T5_PTBYTE T5Reg_GetString (T5_PTR pReg, T5PTR_T5REGITEM pItem)
{
    if (pItem == NULL || (pItem->dwFlags & T5REGF_FOLDER) != 0L)
        return NULL;

    if (pItem->bTicType != T5C_STRING)
        return NULL;

    return T5REG_GETBYTES(pReg, pItem->pValue);
}

/*****************************************************************************
T5Reg_GetItemPassword
Get the password of a parameter
Parameters:
    pReg (IN) pointer to registry in memory
    pItem (IN) pointer to item
Return: pointer to encrypted password or "\0" if none
*****************************************************************************/

T5_PTCHAR T5Reg_GetItemPassword (T5_PTR pReg, T5PTR_T5REGITEM pItem)
{
    if (pItem == NULL)
        return NULL;

    if (pItem->pPassword == 0L)
        return (T5_PTCHAR)"\0";
    return T5REG_GETSTRING(pReg, pItem->pPassword);
}

/*****************************************************************************
T5Reg_CheckPassword
Check uncrypted password
Parameters:
    szCrypt (IN) pointer encrypted password
    szClean (IN) pointer to readable password
Return: TRUE if match
*****************************************************************************/

T5_DWORD T5Reg_Crypt (T5_PTCHAR szClean)
{
    T5_DWORD dwHash;

    dwHash = 49031L;
    while (*szClean)
    {
        dwHash *= 31;
        dwHash += *szClean++;
    }
    return (dwHash & 0x7fffffffL);
}

T5_BOOL T5Reg_CheckPassword (T5_PTCHAR szCrypt, T5_PTCHAR szClean)
{
    T5_CHAR szHash[T5REGMAX_PASSWORD+1];

    if (szCrypt == NULL || *szCrypt == '\0')
        return TRUE;
    if (szClean == NULL || *szClean == '\0')
        return FALSE;
    T5_LTOA (szHash, T5Reg_Crypt (szClean));
    return (T5_STRCMP (szCrypt, szHash) == 0);
}

/*****************************************************************************
T5Reg_CheckPut
Check if a parameter can be written
Parameters:
    pReg (IN) pointer to registry in memory
    pItem (IN) pointer to item
    szPassword (IN) readable password
Return: TRUE if parameter can be WRITE accessed
*****************************************************************************/

T5_BOOL T5Reg_CheckPut (T5_PTR pReg, T5PTR_T5REGITEM pItem,
                        T5_PTCHAR szPassword)
{
    T5_PTCHAR szPsw;

    if (T5Reg_IsRegistryLocked (pReg))
        return FALSE;

    if ((pItem->dwFlags & T5REGF_RO) != 0L)
        return FALSE;

    if ((pItem->dwFlags & T5REGF_PSW) != 0L && pItem->pPassword != 0L)
    {
        szPsw = T5REG_GETSTRING (pReg, pItem->pPassword);
        if (!T5Reg_CheckPassword (szPsw, szPassword))
            return FALSE;
    }
    return TRUE;
}

/*****************************************************************************
T5Reg_PutValue
Write a parameter - NOT FOR STRINGS!
Parameters:
    pReg (IN) pointer to registry in memory
    pItem (IN) pointer to item
    pBuffer (IN) pointer to application buffer
    szPassword (IN) readable password
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5Reg_PutValue (T5_PTR pReg, T5PTR_T5REGITEM pItem, T5_PTR pBuffer,
                        T5_PTCHAR szPassword)
{
    T5_PTBYTE pValue;

    if (!T5Reg_CheckPut (pReg, pItem, szPassword))
        return FALSE;
    if (T5Reg_IsRegistryLocked (pReg) || (pItem->dwFlags & T5REGF_RO) != 0L)
        return FALSE;
    if (pItem->bTicType == T5C_STRING)
        return FALSE;
    pValue = T5REG_GETBYTES(pReg, pItem->pValue);
    T5_MEMCPY (pValue, pBuffer, pItem->bMaxLen);
    return TRUE;
}

T5_BOOL T5Reg_PutValueA (T5_PTR pReg, T5PTR_T5REGITEM pItem, T5_PTR pBuffer)
{
    T5_PTBYTE pValue;

    if (T5Reg_IsRegistryLocked (pReg))
        return FALSE;
    if (pItem->bTicType == T5C_STRING)
        return FALSE;
    pValue = T5REG_GETBYTES(pReg, pItem->pValue);
    T5_MEMCPY (pValue, pBuffer, pItem->bMaxLen);
    return TRUE;
}

/*****************************************************************************
T5Reg_PutValue
Write a STRING parameter
Parameters:
    pReg (IN) pointer to registry in memory
    pItem (IN) pointer to item
    pBuffer (IN) pointer to application buffer (null terminated string)
    szPassword (IN) readable password
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5Reg_PutString (T5_PTR pReg, T5PTR_T5REGITEM pItem,
                         T5_PTBYTE pBuffer, T5_PTCHAR szPassword)
{
    T5_PTBYTE pValue;
    T5_WORD wLen;

    if (!T5Reg_CheckPut (pReg, pItem, szPassword))
        return FALSE;
    if (T5Reg_IsRegistryLocked (pReg) || (pItem->dwFlags & T5REGF_RO) != 0L)
        return FALSE;
    if (pItem->bTicType != T5C_STRING)
        return FALSE;
    pValue = T5REG_GETBYTES(pReg, pItem->pValue);
    wLen = T5_STRLEN(pBuffer);
    if (wLen > (T5_WORD)(pItem->bMaxLen))
        wLen = (T5_WORD)(pItem->bMaxLen);
    T5_MEMCPY(pValue, pBuffer, wLen);
    pValue[wLen] = '\0';
    return TRUE;
}

T5_BOOL T5Reg_PutStringA (T5_PTR pReg, T5PTR_T5REGITEM pItem, T5_PTBYTE pBuffer)
{
    T5_PTBYTE pValue;
    T5_WORD wLen;

    if (T5Reg_IsRegistryLocked (pReg))
        return FALSE;
    if (pItem->bTicType != T5C_STRING)
        return FALSE;
    pValue = T5REG_GETBYTES(pReg, pItem->pValue);
    wLen = T5_STRLEN(pBuffer);
    if (wLen > (T5_WORD)(pItem->bMaxLen))
        wLen = (T5_WORD)(pItem->bMaxLen);
    T5_MEMCPY(pValue, pBuffer, wLen);
    pValue[wLen] = '\0';
    return TRUE;
}

/*****************************************************************************
T5Reg_Read
Read a parameter by string
Parameters:
    pReg (IN) pointer to registry
    szPath (IN) parameter pathname
    szValue (OUT) filled on output with value as string
Return: TRUE if successful
*****************************************************************************/

T5_BOOL T5Reg_Read (T5_PTR pReg, T5_PTCHAR szPath, T5_PTCHAR szValue)
{
    T5PTR_T5REGITEM pItem;
    T5_PTBYTE pValue;
    T5_WORD w;
    T5_DWORD dw;

    *szValue = '\0';
    pItem = T5Reg_FindItem (pReg, szPath);
    if (pItem == NULL)
        return FALSE;

    pValue = T5REG_GETBYTES(pReg, pItem->pValue);
    if (pValue == NULL)
        return FALSE;

    switch (pItem->bTicType)
    {
    case T5C_STRING :
        T5_STRCPY (szValue, pValue);
        break;
    case T5C_BOOL :
    case T5C_USINT :
        T5_LTOA (szValue, (T5_DWORD)(*pValue));
        break;
    case T5C_SINT :
        T5_LTOA (szValue, (T5_LONG)(T5_CHAR)(*pValue));
        break;
    case T5C_INT :
        T5_MEMCPY (&w, pValue, sizeof (w));
        T5_LTOA (szValue, (T5_LONG)(T5_SHORT)w);
        break;
    case T5C_UINT :
        T5_MEMCPY (&w, pValue, sizeof (w));
        T5_LTOA (szValue, (T5_LONG)w);
        break;
    case T5C_DINT :
        T5_MEMCPY (&dw, pValue, sizeof (dw));
        T5_LTOA (szValue, (T5_LONG)dw);
        break;
    case T5C_UDINT :
    case T5C_TIME :
        T5_MEMCPY (&dw, pValue, sizeof (dw));
        T5VMStr_UtoSZ (szValue, dw);
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        {
            T5_REAL r;
            T5_MEMCPY (&r, pValue, sizeof (r));
            T5_RTOA (szValue, r);
        }
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        {
            T5_LREAL lr;
            T5_MEMCPY (&lr, pValue, sizeof (lr));
            T5_RTOA (szValue, lr);
        }
        break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
    case T5C_ULINT :
        {
            T5_LINT li;
            T5_MEMCPY (&li, pValue, sizeof (li));
            T5VMStr_LINTtoSZ (szValue, li);
        }
        break;
#endif /*T5DEF_LINTSUPPORTED*/
    default :
        return FALSE;
    }
    return TRUE;
}

/*****************************************************************************
T5Reg_Write
Write a parameter by string
Parameters:
    pReg (IN) pointer to registry
    szPath (IN) parameter pathname
    szPassword (IN) password in readable form
    szValue (IN) new value in string format
Return: TRUE if successful
*****************************************************************************/

T5_BOOL T5Reg_Write (T5_PTR pReg, T5_PTCHAR szPath, T5_PTCHAR szPassword, T5_PTCHAR szValue)
{
    T5PTR_T5REGITEM pItem;

    pItem = T5Reg_FindItem (pReg, szPath);
    if (pItem == NULL)
        return FALSE;

    switch (pItem->bTicType)
    {
    case T5C_STRING :
        return T5Reg_PutString (pReg, pItem, (T5_PTBYTE)szValue, szPassword);
    case T5C_BOOL :
    case T5C_USINT :
        {
            T5_BYTE b = (T5_BYTE)(T5_ATOI (szValue));
            return T5Reg_PutValue (pReg, pItem, &b, szPassword);
        }
    case T5C_SINT :
        {
            T5_CHAR c = (T5_CHAR)(T5_ATOI (szValue));
            return T5Reg_PutValue (pReg, pItem, &c, szPassword);
        }
    case T5C_INT :
        {
            T5_SHORT i = (T5_SHORT)(T5_ATOI (szValue));
            return T5Reg_PutValue (pReg, pItem, &i, szPassword);
        }
    case T5C_UINT :
        {
            T5_WORD i = (T5_WORD)(T5_ATOI (szValue));
            return T5Reg_PutValue (pReg, pItem, &i, szPassword);
        }
    case T5C_DINT :
        {
            T5_LONG i = (T5_LONG)(T5_ATOI (szValue));
            return T5Reg_PutValue (pReg, pItem, &i, szPassword);
        }
    case T5C_UDINT :
    case T5C_TIME :
        {
            T5_DWORD i = (T5_DWORD)(T5_ATOI (szValue));
            return T5Reg_PutValue (pReg, pItem, &i, szPassword);
        }
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        {
            T5_REAL r = (T5_REAL)(T5_ATOF (szValue));
            return T5Reg_PutValue (pReg, pItem, &r, szPassword);
        }
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        {
            T5_LREAL r = (T5_LREAL)(T5_ATOF (szValue));
            return T5Reg_PutValue (pReg, pItem, &r, szPassword);
        }
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
    case T5C_ULINT :
        {
            T5_LINT li = T5VMStr_SZtoLINT (szValue);
            return T5Reg_PutValue (pReg, pItem, &li, szPassword);
        }
#endif /*T5DEF_LINTSUPPORTED*/
    default : break;
    }
    return FALSE;
}

/****************************************************************************/

#endif /*T5DEF_REG*/

/* eof **********************************************************************/
