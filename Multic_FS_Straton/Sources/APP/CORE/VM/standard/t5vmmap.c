/*****************************************************************************
T5VMmap.c :  embedded variable map

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_VARMAP

/* macros *******************************************************************/

#define _T5VMMap_IndexHashCode(wIndex)     (wIndex % T5_HASHINDEX)
#define _T5VMMap_NumTagHashCode(wIndex)    (wIndex % T5_HASHTAG)

/* static services **********************************************************/

static T5_WORD _T5VMMap_SymbolHashCode (T5_PTBYTE szName);
static T5_PTR  _T5VMMap_GetDataPtr (T5PTR_DB pDB, T5PTR_CODEPUB pPub);

/*****************************************************************************
_T5Map_GetAt
Get a map object by index
Parameters:
    pDB (IN) VM database
    wItem (IN) index of searched object
Return: pointer to found object or NULL if not found
*****************************************************************************/

static T5PTR_DBMAP _T5Map_GetAt (T5PTR_DB pDB, T5_WORD wItem)
{
    T5_PTBYTE pMap;
    T5_DWORD dwOffset;

    if (wItem >= pDB[T5TB_VARMAP].wNbItemUsed)
        return NULL;

    dwOffset = (T5_DWORD)wItem;
    dwOffset *= pDB[T5TB_VARMAP].wSizeOf;
    pMap = (T5_PTBYTE)(pDB[T5TB_VARMAP].pData) + dwOffset;
    return (T5PTR_DBMAP)pMap;
}

/*****************************************************************************
T5VMMap_GetSizing
Get sizing info about variable map
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
    pCode (IN) pointer to app code
Return: OK or error
*****************************************************************************/

T5_RET T5VMMap_GetSizing (T5PTR_DB pDB, T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEPUBCOUNT pCount;
    T5_BOOL bVSI;

    /* get count in app code and check compatibility */
    pHeader = (T5PTR_CODEHEADER)pCode;
    pCount = T5GET_CODEPUBCOUNT(pHeader);
    if (pCount == T5_PTNULL
        || *(T5_PTWORD)pCount == 0
        || (pCount->wFlags != T5PUB_COUNT))
        return T5RET_ERROR;

    bVSI = ((pCount->wFlagsEx & T5PUB_VSI) != 0);

    /* var map items */
    pDB[T5TB_VARMAP].wSizeOf = sizeof (T5STR_DBMAP);
    if (bVSI)
        pDB[T5TB_VARMAP].wSizeOf += sizeof (T5STR_DBMAPEX);

    pDB[T5TB_VARMAP].wNbItemAlloc = pCount->wNbAlloc;
    pDB[T5TB_VARMAP].wNbItemFixed = pCount->wNbUsed;
    pDB[T5TB_VARMAP].wNbItemUsed = pCount->wNbUsed;

    /* hash code tables */
    pDB[T5TB_HASH].wSizeOf = sizeof (T5PTR_DBMAP);
    pDB[T5TB_HASH].wNbItemAlloc = T5_HASHSYB + T5_HASHINDEX + T5_HASHTAG;
    pDB[T5TB_HASH].wNbItemFixed = pDB[T5TB_HASH].wNbItemAlloc;
    pDB[T5TB_HASH].wNbItemUsed = pDB[T5TB_HASH].wNbItemAlloc;

    return T5RET_OK;
}

/*****************************************************************************
T5VMMap_CanHotRestart
Check if hot restart or on line change is possible
Parameters:
    pDB (IN) VM database
    pCode (IN) pointer to app code
Return: OK or error
*****************************************************************************/

T5_RET T5VMMap_CanHotRestart (T5PTR_DB pDB, T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEPUBCOUNT pCount;
    T5_WORD wSizeOf;

    /* get count in app code and check compatibility - OK if old */
    pHeader = (T5PTR_CODEHEADER)pCode;
    pCount = T5GET_CODEPUBCOUNT(pHeader);
    if (pCount == T5_PTNULL
        || *(T5_PTWORD)pCount == 0
        || (pCount->wFlags != T5PUB_COUNT))
        return T5RET_OK;

    /* check VSI option */
    wSizeOf = sizeof (T5STR_DBMAP);
    if ((pCount->wFlagsEx & T5PUB_VSI) != 0)
        wSizeOf += sizeof (T5STR_DBMAPEX);
    if (wSizeOf != pDB[T5TB_VARMAP].wSizeOf)
        return T5RET_ERROR;

    /* alloc size should be the same */
    if (pCount->wNbAlloc != pDB[T5TB_VARMAP].wNbItemAlloc)
        return T5RET_ERROR;

    return T5RET_OK;
}

/*****************************************************************************
T5VMMap_HotRestart
Perform a hot restart or on line change
Parameters:
    pDB (IN) VM database
    pCode (IN) pointer to app code
Return: OK or error
*****************************************************************************/

T5_RET T5VMMap_HotRestart (T5PTR_DB pDB, T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEPUBCOUNT pCount;
    T5PTR_DBMAP *pHash;
    T5_WORD wSizeOf;

    /* reset hash tables */
    pHash = T5GET_DBHASH(pDB);
    T5_MEMSET (pHash, 0, (pDB[T5TB_HASH].wNbItemUsed
                          * pDB[T5TB_HASH].wSizeOf));

    /* get count in app code and check compatibility - OK if old/empty */
    pHeader = (T5PTR_CODEHEADER)pCode;
    pCount = T5GET_CODEPUBCOUNT(pHeader);
    if (pCount == T5_PTNULL
        || *(T5_PTWORD)pCount == 0
        || (pCount->wFlags != T5PUB_COUNT))
    {
        pDB[T5TB_VARMAP].wNbItemFixed = 0;
        pDB[T5TB_VARMAP].wNbItemUsed = 0;
        return T5RET_OK;
    }

    /* check VSI option */
    wSizeOf = sizeof (T5STR_DBMAP);
    if ((pCount->wFlagsEx & T5PUB_VSI) != 0)
        wSizeOf += sizeof (T5STR_DBMAPEX);
    if (wSizeOf != pDB[T5TB_VARMAP].wSizeOf)
        return T5RET_ERROR;

    /* alloc size should be the same */
    if (pCount->wNbAlloc != pDB[T5TB_VARMAP].wNbItemAlloc)
        return T5RET_ERROR;

    /* adjust nb items */
    pDB[T5TB_VARMAP].wNbItemFixed = pCount->wNbUsed;
    pDB[T5TB_VARMAP].wNbItemUsed = pCount->wNbUsed;

    /* if OK rebuild map */
    return T5VMMap_Build (pDB, pCode);
}

/*****************************************************************************
T5VMMap_Build
Build the variable map
Parameters:
    pDB (IN) VM database
    pCode (IN) pointer to app code
Return: OK or error
*****************************************************************************/

T5_RET T5VMMap_Build (T5PTR_DB pDB, T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEPUBCOUNT pCount;
    T5_PTBYTE pByteCode;
    T5PTR_CODEPUB pPub;
    T5PTR_DBMAP *pHashIndex;
    T5PTR_DBMAP *pHashNumTag;
    T5PTR_DBMAP *pHashSymbol;
    T5PTR_DBMAP *pHead;
    T5PTR_DBMAP pItem;
    T5_WORD wNbItem;
    T5_PTBYTE pLockBase;
    T5PTR_DBPRIVATE pPrivate;
    T5_BOOL bSameCrc;

    /* prepare access to lock flags */
    pLockBase = T5GET_DBLOCK(pDB);
    /* get count in app code and check compatibility (ignore if old) */
    pHeader = (T5PTR_CODEHEADER)pCode;
    pCount = T5GET_CODEPUBCOUNT(pHeader);
    if (pCount == T5_PTNULL
        || *(T5_PTWORD)pCount == 0
        || (pCount->wFlags != T5PUB_COUNT))
        return T5RET_OK;
    /* check CRC */
    pPrivate = T5GET_DBPRIVATE(pDB);
    bSameCrc = (pCount->dwCRC == pPrivate->dwMapCrc);
    /* get pointers to hash tables */
    pHashIndex = T5GET_DBHASH(pDB);
    pHashNumTag = pHashIndex + T5_HASHINDEX;
    pHashSymbol = pHashNumTag + T5_HASHTAG;

    /* get pointer to first item in heap */
    pItem = T5GET_DBMAP(pDB);
    wNbItem = 0;

    /* scan item list and skip first recore (sizing) */
    pByteCode = (T5_PTBYTE)pCount;
    pByteCode += pCount->wSizeOf;
    while (*(T5_PTWORD)pByteCode != 0
        && wNbItem < pDB[T5TB_VARMAP].wNbItemUsed)
    {
        pPub = (T5PTR_CODEPUB)pByteCode;
        /* store useful pointers */
        pItem->pDef = (T5_PTR)pPub;
        pItem->pData = _T5VMMap_GetDataPtr (pDB, pPub);
        pItem->pLock = pLockBase + pPub->wIndex;
        /* register in index hash table */
        pHead = pHashIndex + _T5VMMap_IndexHashCode (pPub->wIndex);
        pItem->pNextIndex = *pHead;
        *pHead = pItem;
        /* register in num.tag hash table */
        if ((pPub->wFlags & T5PUB_HASNUMTAG) != 0)
        {
            pHead = pHashNumTag + _T5VMMap_NumTagHashCode (pPub->wNumTag);
            pItem->pNextTag = *pHead;
            *pHead = pItem;
        }
        /* register in symbol hash table */
        if ((pPub->wFlags & T5PUB_HASNAME) != 0)
        {
            pHead = pHashSymbol + _T5VMMap_SymbolHashCode (pByteCode
                                                  + sizeof (T5STR_CODEPUB));
            pItem->pNextSyb = *pHead;
            *pHead = pItem;
        }
        /* VSI info */
        if ((pPub->wFlags & T5PUB_VSI) != 0 && !bSameCrc)
        {
            T5_PTBYTE pInfo = (T5_PTBYTE)pItem + sizeof (T5STR_DBMAP);
            T5_MEMSET (pInfo, 0, sizeof (T5STR_DBMAPEX));
        }
        /* next one */
        wNbItem++;
        pByteCode += pPub->wSizeOf;
        pItem++;
        if ((pPub->wFlags & T5PUB_VSI) != 0)
            pItem = (T5PTR_DBMAP)((T5_PTBYTE)pItem + sizeof (T5STR_DBMAPEX));
    }
    /* remember CRC */
    pPrivate->dwMapCrc = pCount->dwCRC;

    return T5RET_OK;
}

/*****************************************************************************
T5VMMap_GetDataPtr
Get pointer to a variable in the VM database
Parameters:
    pCode (IN) pointer to app code
    pPub (IN) variable publishing info
Return: pointer to the data / NULL if error
*****************************************************************************/

static T5_PTR _T5VMMap_GetDataPtr (T5PTR_DB pDB, T5PTR_CODEPUB pPub)
{
    T5_PTR pData;
    T5_PTBYTE pD8;
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pD16;
#endif /*T5DEF_DATA16SUPPORTED*/
    T5_PTDWORD pD32;
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 pD64;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE *pString;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_XV
    T5_PTBYTE *pXV;
#endif /*T5DEF_XV*/
#ifdef T5DEF_CTSEG
    T5_PTBYTE pCTSeg;
    T5_PTBYTE pProps;
#endif /*T5DEF_CTSEG*/

    if (pPub->wTicType & T5C_EXTERN)
    {
#ifdef T5DEF_XV
        pXV = T5GET_DBXV(pDB);
        pData = (T5_PTR)(pXV[pPub->wIndex]);
#else /*T5DEF_XV*/
        pData = T5_PTNULL;
#endif /*T5DEF_XV*/
    }
    else if (pPub->wTicType == T5C_COMPLEX)
    {
#ifdef T5DEF_CTSEG
        pCTSeg = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
        pProps = (T5_PTBYTE)pPub;
        pProps += pPub->wProps;
        pCTSeg += *(T5_PTDWORD)pProps;
        pData = (T5_PTR)pCTSeg;
#else /*T5DEF_CTSEG*/
        pData = T5_PTNULL;
#endif /*T5DEF_CTSEG*/
    }
    else
    {
        switch (pPub->wTicType)
        {
        case T5C_BOOL :
        case T5C_SINT :
        case T5C_USINT :
            pD8 = T5GET_DBDATA8(pDB);
            pData = (T5_PTR)(pD8 + pPub->wIndex);
            break;
        case T5C_INT :
        case T5C_UINT :
#ifdef T5DEF_DATA16SUPPORTED
            pD16 = T5GET_DBDATA16(pDB);
            pData = (T5_PTR)(pD16 + pPub->wIndex);
#else /*T5DEF_DATA16SUPPORTED*/
            pData = T5_PTNULL;
#endif /*T5DEF_DATA16SUPPORTED*/
            break;
        case T5C_DINT :
        case T5C_UDINT :
        case T5C_REAL :
            pD32 = T5GET_DBDATA32(pDB);
            pData = (T5_PTR)(pD32 + pPub->wIndex);
            break;
        case T5C_TIME :
            pD32 = T5GET_DBTIME(pDB);
            pData = (T5_PTR)(pD32 + pPub->wIndex);
            break;
        case T5C_LINT :
        case T5C_ULINT :
        case T5C_LREAL :
#ifdef T5DEF_DATA64SUPPORTED
            pD64 = T5GET_DBDATA64(pDB);
            pData = (T5_PTR)(pD64 + pPub->wIndex);
#else /*T5DEF_DATA64SUPPORTED*/
            pData = T5_PTNULL;
#endif /*T5DEF_DATA64SUPPORTED*/
            break;
        case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
            pString = T5GET_DBSTRING(pDB);
            pData = (T5_PTR)(pString[pPub->wIndex]);
#else /*T5DEF_STRINGSUPPORTED*/
            pData = T5_PTNULL;
#endif /*T5DEF_STRINGSUPPORTED*/
            break;
        default :
            pData = T5_PTNULL;
            break;
        }
    }
    return pData;
}

/*****************************************************************************
T5VMMap_SymbolHashCode
Simple hash code function for symbols - CASE SENSITIVE
Parameters:
    szName (IN) variable name
Return: index in hash code table
*****************************************************************************/

static T5_WORD _T5VMMap_SymbolHashCode (T5_PTBYTE szName)
{
	T5_WORD wHash;
    T5_BYTE c;
    
    wHash = 0;
	while (*szName)
    {
        c = *szName;
        if (c >= 'a' && c <= 'z')
            c -= ('a' - 'A');
		wHash = (wHash << 5) + wHash + (T5_WORD)c;
        szName++;
    }
	return (wHash % T5_HASHSYB);
}

/*****************************************************************************
T5Map_GetByIndex
Find a variable using its type/index
Parameters:
    pDB (IN) VM database
    wType (IN) variable type (define in t5vmtic.h)
    wIndex (IN) variable index in database
Return: pointer to the variable publishing info or NULL if not found
*****************************************************************************/

T5PTR_DBMAP T5Map_GetByIndex (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex)
{
    T5PTR_DBMAP *pHash;
    T5PTR_DBMAP pItem;
    T5PTR_CODEPUB pPub;
    T5_WORD wPubType;

    /* adjust type for unsigned */
    switch (wType)
    {
    case T5C_USINT : wType = T5C_SINT; break;
    case T5C_UINT  : wType = T5C_INT; break;
    case T5C_UDINT : wType = T5C_DINT; break;
    case T5C_ULINT : wType = T5C_LINT; break;
    default : break;
    }
    /* get pointer to hash tables and hash list header */
    pHash = T5GET_DBHASH(pDB);
    pItem = pHash[_T5VMMap_IndexHashCode (wIndex)];
    /* search */
    while (pItem != T5_PTNULL)
    {
        pPub = (T5PTR_CODEPUB)(pItem->pDef);
        /* adjust type for comparison */
        switch (pPub->wTicType)
        {
        case T5C_USINT : wPubType = T5C_SINT; break;
        case T5C_UINT  : wPubType = T5C_INT; break;
        case T5C_UDINT : wPubType = T5C_DINT; break;
        case T5C_ULINT : wPubType = T5C_LINT; break;
        default        : wPubType = pPub->wTicType; break;
        }
        if (wPubType == wType && pPub->wIndex == wIndex)
            return pItem;
        pItem = (T5PTR_DBMAP)(pItem->pNextIndex);
    }
    return T5_PTNULL;
}

/*****************************************************************************
T5Map_GetByNumTag
Find a variable using its numerical tag
Parameters:
    pDB (IN) VM database
    wTag (IN) variable numerical tag
Return: pointer to the variable publishing info or NULL if not found
*****************************************************************************/

T5PTR_DBMAP T5Map_GetByNumTag (T5PTR_DB pDB, T5_WORD wTag)
{
    T5PTR_DBMAP *pHash;
    T5PTR_DBMAP pItem;
    T5PTR_CODEPUB pPub;

    /* get pointer to hash tables and hash list header */
    pHash = T5GET_DBHASH(pDB);
    pHash += T5_HASHINDEX;
    pItem = pHash[_T5VMMap_NumTagHashCode (wTag)];
    /* search */
    while (pItem != T5_PTNULL)
    {
        pPub = (T5PTR_CODEPUB)(pItem->pDef);
        if (pPub->wNumTag == wTag)
            return pItem;
        pItem = (T5PTR_DBMAP)(pItem->pNextIndex);
    }
    return T5_PTNULL;
}

/*****************************************************************************
T5Map_GetBySymbol
Find a variable using its symbol - CASE SENSITIVE
Parameters:
    pDB (IN) VM database
    wSymbol (IN) variable name (MUST BE UPPERCASE)
Return: pointer to the variable publishing info or NULL if not found
*****************************************************************************/

T5PTR_DBMAP T5Map_GetBySymbol (T5PTR_DB pDB, T5_PTCHAR szSymbol)
{
    T5PTR_DBMAP *pHash;
    T5PTR_DBMAP pItem;
    T5_PTCHAR pName;

    /* get pointer to hash tables and hash list header */
    pHash = T5GET_DBHASH(pDB);
    pHash += T5_HASHINDEX;
    pHash += T5_HASHTAG;
    pItem = pHash[_T5VMMap_SymbolHashCode ((T5_PTBYTE)szSymbol)];
    /* search */
    while (pItem != T5_PTNULL)
    {
        pName = (T5_PTCHAR)(pItem->pDef);
        pName += sizeof (T5STR_CODEPUB);
        if (T5VMStr_ICmp ((T5_PTBYTE)pName, (T5_PTBYTE)szSymbol) == 0) /* case insensitive */
            return pItem;
        pItem = (T5PTR_DBMAP)(pItem->pNextSyb);
    }
    return T5_PTNULL;
}

/*****************************************************************************
T5Map_GetSymbol
Get symbol of a mapped variable
Parameters:
    pItem (IN) pointer to the item in the var map
Return: pointer to the variable symbol - NULL if not found or no symbol
*****************************************************************************/

T5_PTCHAR T5Map_GetSymbol (T5PTR_DBMAP pItem)
{
    T5PTR_CODEPUB pPub;
    T5_PTCHAR pName;

    if (pItem == T5_PTNULL || pItem->pDef == T5_PTNULL)
        return T5_PTNULL;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    if ((pPub->wFlags & T5PUB_HASNAME) == 0)
        return T5_PTNULL;

    pName = (T5_PTCHAR)pPub;
    pName += sizeof (T5STR_CODEPUB);
    return pName;
}

/*****************************************************************************
T5Map_GetDescLength
Get length of map object description - for debugger
Parameters:
    pDB (IN) VM database
    wItem (IN) index of the map object
Return: frame length
*****************************************************************************/

T5_WORD T5Map_GetDescLength (T5PTR_DB pDB, T5_WORD wItem)
{
    T5PTR_DBMAP pMap;
    T5PTR_CODEPUB pPub;
    T5_PTCHAR pName;
    T5_WORD wLen;

    if (pDB[T5TB_VARMAP].wNbItemUsed == 0
        || wItem >= pDB[T5TB_VARMAP].wNbItemUsed)
        return 12;

    pMap = _T5Map_GetAt (pDB, wItem);
    pPub = (T5PTR_CODEPUB)(pMap->pDef);

    if ((pPub->wFlags & T5PUB_HASNAME) == 0)
        return 12;

    pName = (T5_PTCHAR)pPub;
    pName += sizeof (T5STR_CODEPUB);
    wLen = T5_STRLEN(pName);
    if (wLen > 255) wLen = 255;

    return (12 + wLen);
}

/*****************************************************************************
T5Map_GetDesc
Get map object description - for debugger
Frame:
    typ(2) + index(2) + strlen(2) + dim(2) + flags(2) + nbchar(1) + name + \0
Parameters:
    pDB (IN) VM database
    wItem (IN) index of the map object
    pFrame (OUT) frame buffer
Return: frame length
*****************************************************************************/

T5_WORD T5Map_GetDesc (T5PTR_DB pDB, T5_WORD wItem, T5_PTBYTE pFrame)
{
    T5PTR_DBMAP pMap;
    T5PTR_CODEPUB pPub;
    T5_PTCHAR pName;
    T5_WORD wLen;
    T5_PTBYTE pProp;
    T5_DWORD dwTypeInfo;
    T5_WORD wTypeInfo;

    if (pDB[T5TB_VARMAP].wNbItemUsed == 0
        || wItem >= pDB[T5TB_VARMAP].wNbItemUsed)
    {
        T5_MEMSET(pFrame, 0, 12);
        return 12;
    }

    pMap = _T5Map_GetAt (pDB, wItem);
    pPub = (T5PTR_CODEPUB)(pMap->pDef);

    T5_COPYFRAMEWORD(pFrame, &(pPub->wTicType));

    if ((pPub->wFlags & T5PUB_HASCTSEG) == 0)
    {
        T5_COPYFRAMEWORD(pFrame+2, &(pPub->wIndex));
        T5_COPYFRAMEWORD(pFrame+4, &(pPub->wStringLength));
        T5_COPYFRAMEWORD(pFrame+6, &(pPub->wDim));
    }
    else
    {
        pProp = (T5_PTBYTE)pPub;
        pProp += pPub->wProps;
        T5_COPYFRAMEDWORD(pFrame+2, pProp);
        dwTypeInfo = *(T5_PTDWORD)(pProp+4);
        wTypeInfo = (T5_WORD)dwTypeInfo;
        T5_COPYFRAMEWORD(pFrame+6, &wTypeInfo);
    }

    T5_COPYFRAMEWORD(pFrame+8, &(pPub->wFlags));

    if ((pPub->wFlags & T5PUB_HASNAME) == 0)
    {
        T5_MEMSET(pFrame+10, 0, 2);
        return 12;
    }

    pName = (T5_PTCHAR)pPub;
    pName += sizeof (T5STR_CODEPUB);
    wLen = T5_STRLEN(pName);
    if (wLen > 255) wLen = 255;

    pFrame[10] = (T5_BYTE)wLen;
    T5_MEMCPY(pFrame+11, pName, wLen);
    pFrame[11+wLen] = 0;

    return (wLen + 12);
}

/*****************************************************************************
T5Map_GetIndex
Get index of a mapped variable
Parameters:
    pItem (IN) pointer to the item in the var map
Return: index of the variable
*****************************************************************************/

T5_WORD T5Map_GetIndex (T5PTR_DBMAP pItem)
{
    T5PTR_CODEPUB pPub;

    if (pItem == T5_PTNULL || pItem->pDef == T5_PTNULL)
        return 0;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    return pPub->wIndex;
}

/*****************************************************************************
T5Map_GetType
Get type of a mapped variable
Parameters:
    pItem (IN) pointer to the item in the var map
Return: type of the variable (defined in t5tic.h)
*****************************************************************************/

T5_WORD T5Map_GetType (T5PTR_DBMAP pItem)
{
    T5PTR_CODEPUB pPub;

    if (pItem == T5_PTNULL || pItem->pDef == T5_PTNULL)
        return 0;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    return pPub->wTicType;
}

/*****************************************************************************
T5Map_GetDim
Get dimension of a mapped array
Parameters:
    pItem (IN) pointer to the item in the var map
Return: number of elements or 0 if variable is not an array
*****************************************************************************/

T5_WORD T5Map_GetDim (T5PTR_DBMAP pItem)
{
    T5PTR_CODEPUB pPub;
    T5_WORD wDim;

    if (pItem == T5_PTNULL || pItem->pDef == T5_PTNULL)
        return 0;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    wDim = pPub->wDim;
    if (wDim == 1)
        wDim = 0;
    return wDim;
}

/*****************************************************************************
T5Map_GetStringLength
Get max length of a mapped string variable
Parameters:
    pItem (IN) pointer to the item in the var map
Return: max string length or 0 if variable is not a string
*****************************************************************************/

T5_WORD T5Map_GetStringLenth (T5PTR_DBMAP pItem)
{
    T5PTR_CODEPUB pPub;

    if (pItem == T5_PTNULL || pItem->pDef == T5_PTNULL)
        return 0;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    if (pPub->wTicType != T5C_STRING)
        return 0;

    return pPub->wStringLength;
}

/*****************************************************************************
T5Map_GetNumTag
Get numerical tag of a mapped variable
Parameters:
    pItem (IN) pointer to the item in the var map
Return: num tag of the variable
*****************************************************************************/

T5_WORD T5Map_GetNumTag (T5PTR_DBMAP pItem)
{
    T5PTR_CODEPUB pPub;

    if (pItem == T5_PTNULL || pItem->pDef == T5_PTNULL)
        return 0;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    return pPub->wNumTag;
}

/*****************************************************************************
T5Map_GetProps
Get properties (OEM edfined) of a mapped variable
Parameters:
    pItem (IN) pointer to the item in the var map
Return: pointer to the properties structure - NULL if not found or no symbol
*****************************************************************************/

T5_PTR T5Map_GetProps (T5PTR_DBMAP pItem)
{
    T5PTR_CODEPUB pPub;
    T5_PTBYTE pProp;

    if (pItem == T5_PTNULL || pItem->pDef == T5_PTNULL)
        return T5_PTNULL;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    if ((pPub->wFlags & T5PUB_HASPROPS) == 0)
        return T5_PTNULL;

    pProp = (T5_PTBYTE)pPub;
    pProp += pPub->wProps;

    if ((pPub->wFlags & T5PUB_HASPROFILE) != 0)
        pProp += sizeof (T5_STRING16);

    if ((pPub->wFlags & T5PUB_HASCTSEG) != 0)
        pProp += (2 * sizeof (T5_DWORD));

    return (T5_PTR)pProp;
}

/*****************************************************************************
T5Map_GetPropsSize
Get sizz of properties (OEM edfined) of a mapped variable
Parameters:
    pItem (IN) pointer to the item in the var map
Return: properties byte stream size
*****************************************************************************/

T5_WORD T5Map_GetPropsSize (T5PTR_DBMAP pItem)
{
    T5PTR_CODEPUB pPub;
    T5_WORD wSize;

    if (pItem == T5_PTNULL || pItem->pDef == T5_PTNULL)
        return 0;
    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    if ((pPub->wFlags & T5PUB_HASPROPS) == 0)
        return 0;

    wSize = pPub->wSizeOf - pPub->wProps;
    if ((pPub->wFlags & T5PUB_HASPROFILE) != 0)
        wSize -= sizeof (T5_STRING16);
    if ((pPub->wFlags & T5PUB_HASCTSEG) != 0)
        wSize -= (2 * sizeof (T5_DWORD));
    return wSize;
}

/*****************************************************************************
T5Map_GetTypeInfo
Get pointer to the type definition (for complex CT vars only)
Parameters:
    pItem (IN) pointer to the item in the var map
Return: pointer to the type definition or NULL - definition starts with name
*****************************************************************************/

T5_PTBYTE T5Map_GetTypeInfo (T5PTR_DBMAP pItem, T5PTR_DB pDB)
{
#ifndef T5DEF_CTSEG
    return T5_PTNULL;
#else /*T5DEF_CTSEG*/
    T5PTR_CODEPUB pPub;
    T5_PTBYTE pProp;
    T5_PTBYTE pTypeInfo;
    T5PTR_DBPRIVATE pPrivate;

    if (pItem == T5_PTNULL || pItem->pDef == T5_PTNULL)
        return T5_PTNULL;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    if ((pPub->wFlags & T5PUB_HASCTSEG) == 0)
        return T5_PTNULL;

    pProp = (T5_PTBYTE)pPub;
    pProp += pPub->wProps;
    pProp += sizeof (T5_DWORD);

    pPrivate = T5GET_DBPRIVATE(pDB);
    pTypeInfo = (T5_PTBYTE)T5VMCode_GetCTTypeDef (pPrivate->pCode);
    pTypeInfo += *(T5_PTDWORD)pProp;

    return pTypeInfo;
#endif /*T5DEF_CTSEG*/
}

/*****************************************************************************
T5Map_GetProfileName
Get the profile name (OEM edfined) of a mapped variable
Parameters:
    pItem (IN) pointer to the item in the var map
Return: pointer to the profile name - NULL if not found or no profile embedded
*****************************************************************************/

T5_PTCHAR T5Map_GetProfileName (T5PTR_DBMAP pItem)
{
    T5PTR_CODEPUB pPub;
    T5_PTBYTE pProp;

    if (pItem == T5_PTNULL || pItem->pDef == T5_PTNULL)
        return T5_PTNULL;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    if ((pPub->wFlags & T5PUB_HASPROFILE) == 0)
        return T5_PTNULL;

    pProp = (T5_PTBYTE)pPub;
    pProp += pPub->wProps;

    if ((pPub->wFlags & T5PUB_HASCTSEG) != 0)
        pProp += (2 * sizeof (T5_DWORD));

    return (T5_PTCHAR)pProp;
}

/*****************************************************************************
T5Map_IsLocked
Check if a mapped variable is locked
Parameters:
    pDB (IN) VM database
    pItem (IN) pointer to the item in the var map
Return: TRUE if variable is locked
*****************************************************************************/

T5_BOOL T5Map_IsLocked (T5PTR_DBMAP pItem)
{
#ifdef T5DEF_DONTFORCELOCKEDVARS
    T5PTR_CODEPUB pPub;
    T5_BYTE bMask;

    if (pItem == T5_PTNULL || pItem->pDef == T5_PTNULL)
        return FALSE;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    if ((pPub->wTicType & T5C_EXTERN) != 0)
        bMask = T5LOCK_XV;
    else
    {
        switch (pPub->wTicType)
        {
        case T5C_BOOL   :
        case T5C_SINT  :
        case T5C_USINT : bMask = T5LOCK_DATA8; break;
        case T5C_INT  :
        case T5C_UINT  : bMask = T5LOCK_DATA16; break;
        case T5C_DINT   :
        case T5C_UDINT :
        case T5C_REAL   : bMask = T5LOCK_DATA32; break;
        case T5C_TIME   : bMask = T5LOCK_TIME; break;
        case T5C_LINT  :
        case T5C_ULINT  :
        case T5C_LREAL : bMask = T5LOCK_DATA64; break;
        case T5C_STRING   : bMask = T5LOCK_STRING; break;
        default        : bMask = 0; break;
        }
    }
    return ((*(pItem->pLock) & bMask) != 0);
#else /*T5DEF_DONTFORCELOCKEDVARS*/
    return FALSE;
#endif /*T5DEF_DONTFORCELOCKEDVARS*/
}

/*****************************************************************************
T5Map_CheckPointer
Check a DBMap pointer
Parameters:
    pDB (IN) VM database
    pMapPtr (IN) pointer to to be checked
Return: TRUE if it is a valid pointer to a DBMAP item
*****************************************************************************/

T5_BOOL T5Map_CheckPointer (T5PTR_DB pDB, T5PTR_DBMAP pMapPtr)
{
    T5PTR_DBMAP pMap;
    T5_WORD i;

    pMap = T5GET_DBMAP(pDB);
    for (i=0; i<pDB[T5TB_VARMAP].wNbItemUsed; i++)
    {
        if (pMap == pMapPtr)
            return TRUE;
        pMap = (T5PTR_DBMAP)((T5_PTBYTE)pMap + pDB[T5TB_VARMAP].wSizeOf);
    }
    return FALSE;
}

/*****************************************************************************
T5VMMap_GetFirst
Start enumerating mapped variables
Parameters:
    pDB (IN) VM database
    pwPos (OUT) pointer to the current position to be passed to GetNext()
Return: pointer to the first item (or NULL if empty)
*****************************************************************************/

T5PTR_DBMAP T5Map_GetFirst (T5PTR_DB pDB, T5_PTWORD pwPos)
{
    T5PTR_DBMAP pMap;

    if (pDB[T5TB_VARMAP].wNbItemUsed == 0)
        return NULL;
    if (pwPos != NULL)
        *pwPos = 0;
    pMap = T5GET_DBMAP(pDB);
    return pMap;
}

/*****************************************************************************
T5VMMap_GetNext
Enumerate mapped variables
Parameters:
    pDB (IN) VM database
    pwPos (IN/OUT) pointer to the current position
Return: pointer to the next item (or NULL if end of map)
*****************************************************************************/

T5PTR_DBMAP T5Map_GetNext (T5PTR_DB pDB, T5_PTWORD pwPos)
{
    if (pwPos == NULL)
        return NULL;
    if (pDB[T5TB_VARMAP].wNbItemUsed == 0)
        return NULL;
    if (*pwPos >= (pDB[T5TB_VARMAP].wNbItemUsed -1))
        return NULL;
    *pwPos += 1;
    return _T5Map_GetAt (pDB, *pwPos);
}

/*****************************************************************************
T5Map_Get???Value
Read variable value in VM database
Parameters:
    pItem (IN) pointer to the item in the var map
Return: variable value (cast in the wished type)
*****************************************************************************/

T5_LONG T5Map_GetLongValue (T5PTR_DBMAP pItem)
{
    T5_LONG lValue;
    T5PTR_CODEPUB pPub;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    switch (pPub->wTicType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
    case T5C_SINT :
        lValue = (T5_LONG)(*(T5_PTCHAR)(pItem->pData));
        break;
    case T5C_USINT :
        lValue = (T5_LONG)(*(T5_PTBYTE)(pItem->pData));
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        lValue = (T5_LONG)(*(T5_PTSHORT)(pItem->pData));
        break;
    case T5C_UINT :
        lValue = (T5_LONG)(*(T5_PTWORD)(pItem->pData));
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_UDINT :
    case T5C_TIME :
        lValue = *(T5_PTLONG)(pItem->pData);
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        lValue = (T5_LONG)(*(T5_PTREAL)(pItem->pData));
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        lValue = (T5_LONG)(*(T5_PTLINT)(pItem->pData));
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        lValue = (T5_LONG)(*(T5_PTLWORD)(pItem->pData));
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        lValue = (T5_LONG)(*(T5_PTLREAL)(pItem->pData));
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default :
        lValue = 0L;
        break;
    }
    return lValue;
}

/****************************************************************************/

T5_DWORD T5Map_GetDWordValue (T5PTR_DBMAP pItem)
{
    T5_DWORD dwValue;
    T5PTR_CODEPUB pPub;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    switch (pPub->wTicType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
    case T5C_SINT :
        dwValue = (T5_DWORD)(*(T5_PTCHAR)(pItem->pData));
        break;
    case T5C_USINT :
        dwValue = (T5_DWORD)(*(T5_PTBYTE)(pItem->pData));
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        dwValue = (T5_DWORD)(*(T5_PTSHORT)(pItem->pData));
        break;
    case T5C_UINT :
        dwValue = (T5_DWORD)(*(T5_PTWORD)(pItem->pData));
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_UDINT :
    case T5C_TIME :
        dwValue = *(T5_PTDWORD)(pItem->pData);
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        dwValue = (T5_DWORD)(*(T5_PTREAL)(pItem->pData));
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        dwValue = (T5_DWORD)(*(T5_PTLINT)(pItem->pData));
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        dwValue = (T5_DWORD)(*(T5_PTLWORD)(pItem->pData));
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        dwValue = (T5_DWORD)(*(T5_PTLREAL)(pItem->pData));
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default :
        dwValue = 0L;
        break;
    }
    return dwValue;
}

/****************************************************************************/

#ifdef T5DEF_LINTSUPPORTED

T5_LINT T5Map_GetLIntValue (T5PTR_DBMAP pItem)
{
    T5_LINT lValue;
    T5PTR_CODEPUB pPub;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    switch (pPub->wTicType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
    case T5C_SINT :
        lValue = (T5_LINT)(*(T5_PTCHAR)(pItem->pData));
        break;
    case T5C_USINT :
        lValue = (T5_LINT)(*(T5_PTBYTE)(pItem->pData));
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        lValue = (T5_LINT)(*(T5_PTSHORT)(pItem->pData));
        break;
    case T5C_UINT :
        lValue = (T5_LINT)(*(T5_PTWORD)(pItem->pData));
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
        lValue = (T5_LINT)(*(T5_PTLONG)(pItem->pData));
        break;
    case T5C_UDINT :
    case T5C_TIME :
        lValue = (T5_LINT)(*(T5_PTDWORD)(pItem->pData));
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        lValue = (T5_LINT)(*(T5_PTREAL)(pItem->pData));
        break;
#endif /*T5DEF_REALSUPPORTED*/
    case T5C_LINT :
        lValue = *(T5_PTLINT)(pItem->pData);
        break;
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        lValue = *(T5_PTLWORD)(pItem->pData);
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        lValue = (T5_LINT)(*(T5_PTLREAL)(pItem->pData));
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default :
        lValue = (T5_LINT)0;
        break;
    }
    return lValue;
}

#endif /*T5DEF_LINTSUPPORTED*/

/****************************************************************************/

#ifdef T5DEF_REALSUPPORTED

T5_REAL T5Map_GetRealValue (T5PTR_DBMAP pItem)
{
    T5_REAL rValue;
    T5PTR_CODEPUB pPub;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    switch (pPub->wTicType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
    case T5C_SINT :
        rValue = (T5_REAL)(*(T5_PTCHAR)(pItem->pData));
        break;
    case T5C_USINT :
        rValue = (T5_REAL)(*(T5_PTBYTE)(pItem->pData));
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        rValue = (T5_REAL)(*(T5_PTSHORT)(pItem->pData));
        break;
    case T5C_UINT :
        rValue = (T5_REAL)(*(T5_PTWORD)(pItem->pData));
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
        rValue = (T5_REAL)(*(T5_PTLONG)(pItem->pData));
        break;
    case T5C_UDINT :
    case T5C_TIME :
        rValue = (T5_REAL)(*(T5_PTDWORD)(pItem->pData));
        break;
    case T5C_REAL :
        rValue = *(T5_PTREAL)(pItem->pData);
        break;
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        rValue = (T5_REAL)(*(T5_PTLINT)(pItem->pData));
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        rValue = (T5_REAL)(*(T5_PTLWORD)(pItem->pData));
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        rValue = (T5_REAL)(*(T5_PTLREAL)(pItem->pData));
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default :
        rValue = (T5_REAL)0;
        break;
    }
    return rValue;
}

#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/

#ifdef T5DEF_LREALSUPPORTED

T5_LREAL T5Map_GetLRealValue (T5PTR_DBMAP pItem)
{
    T5_LREAL lValue;
    T5PTR_CODEPUB pPub;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);

    switch (pPub->wTicType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
    case T5C_SINT :
        lValue = (T5_LREAL)(*(T5_PTCHAR)(pItem->pData));
        break;
    case T5C_USINT :
        lValue = (T5_LREAL)(*(T5_PTBYTE)(pItem->pData));
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        lValue = (T5_LREAL)(*(T5_PTSHORT)(pItem->pData));
        break;
    case T5C_UINT :
        lValue = (T5_LREAL)(*(T5_PTWORD)(pItem->pData));
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
        lValue = (T5_LREAL)(*(T5_PTLONG)(pItem->pData));
        break;
    case T5C_UDINT :
    case T5C_TIME :
        lValue = (T5_LREAL)(*(T5_PTDWORD)(pItem->pData));
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        lValue = (T5_LREAL)(*(T5_PTREAL)(pItem->pData));
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        lValue = (T5_LREAL)(*(T5_PTLINT)(pItem->pData));
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        lValue = (T5_LREAL)(*(T5_PTLWORD)(pItem->pData));
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
    case T5C_LREAL :
        lValue = *(T5_PTLREAL)(pItem->pData);
        break;
    default :
        lValue = (T5_LREAL)0;
        break;
    }
    return lValue;
}

#endif /*T5DEF_LREALSUPPORTED*/

/****************************************************************************/

#ifdef T5DEF_STRINGSUPPORTED

T5_PTBYTE T5Map_GetStringValue (T5PTR_DBMAP pItem, T5_PTBYTE pbLen)
{
    T5PTR_CODEPUB pPub;
    T5_PTBYTE pStr;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    if (pPub->wTicType != T5C_STRING)
        return NULL;

    pStr = (T5_PTBYTE)(pItem->pData);
    if (pbLen != NULL)
        *pbLen = pStr[1];
    return (pStr + 2);
}

#endif /*T5DEF_STRINGSUPPORTED*/

/*****************************************************************************
T5Map_Set???Value
Write variable value in VM database
Parameters:
    pItem (IN) pointer to the item in the var map
    value (IN) new value for the variable
Return: OK or error
*****************************************************************************/

T5_RET T5Map_SetLongValue (T5PTR_DBMAP pItem, T5_LONG lValue)
{
    T5PTR_CODEPUB pPub;

    if (T5Map_IsLocked (pItem))
        return T5RET_ERROR;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    switch (pPub->wTicType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
        *(T5_PTCHAR)(pItem->pData) = (T5_CHAR)(lValue != 0L);
        break;
    case T5C_SINT :
        *(T5_PTCHAR)(pItem->pData) = (T5_CHAR)lValue;
        break;
    case T5C_USINT :
        *(T5_PTBYTE)(pItem->pData) = (T5_BYTE)lValue;
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        *(T5_PTSHORT)(pItem->pData) = (T5_SHORT)lValue;
        break;
    case T5C_UINT :
        *(T5_PTWORD)(pItem->pData) = (T5_WORD)lValue;
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_UDINT :
    case T5C_TIME :
        *(T5_PTLONG)(pItem->pData) = lValue;
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        *(T5_PTREAL)(pItem->pData) = (T5_REAL)lValue;
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        *(T5_PTLINT)(pItem->pData) = (T5_LINT)lValue;
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        *(T5_PTLWORD)(pItem->pData) = (T5_LINT)lValue;
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        *(T5_PTLREAL)(pItem->pData) = (T5_LREAL)lValue;
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default :
        return T5RET_ERROR;
    }
    return T5RET_OK;
}

/****************************************************************************/

T5_RET T5Map_SetDWordValue (T5PTR_DBMAP pItem, T5_DWORD dwValue)
{
    T5PTR_CODEPUB pPub;

    if (T5Map_IsLocked (pItem))
        return T5RET_ERROR;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    switch (pPub->wTicType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
        *(T5_PTCHAR)(pItem->pData) = (T5_CHAR)(dwValue != 0L);
        break;
    case T5C_SINT :
        *(T5_PTCHAR)(pItem->pData) = (T5_CHAR)dwValue;
        break;
    case T5C_USINT :
        *(T5_PTBYTE)(pItem->pData) = (T5_BYTE)dwValue;
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        *(T5_PTSHORT)(pItem->pData) = (T5_SHORT)dwValue;
        break;
    case T5C_UINT :
        *(T5_PTWORD)(pItem->pData) = (T5_WORD)dwValue;
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_UDINT :
    case T5C_TIME :
        *(T5_PTDWORD)(pItem->pData) = dwValue;
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        *(T5_PTREAL)(pItem->pData) = (T5_REAL)dwValue;
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        *(T5_PTLINT)(pItem->pData) = (T5_LINT)dwValue;
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        *(T5_PTLWORD)(pItem->pData) = (T5_LINT)dwValue;
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        *(T5_PTLREAL)(pItem->pData) = (T5_LREAL)dwValue;
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default :
        return T5RET_ERROR;
    }
    return T5RET_OK;
}

/****************************************************************************/

#ifdef T5DEF_LINTSUPPORTED

T5_RET T5Map_SetLIntValue (T5PTR_DBMAP pItem, T5_LINT lValue)
{
    T5PTR_CODEPUB pPub;

    if (T5Map_IsLocked (pItem))
        return T5RET_ERROR;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    switch (pPub->wTicType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
        *(T5_PTCHAR)(pItem->pData) = (T5_CHAR)(lValue != (T5_LINT)0);
        break;
    case T5C_SINT :
        *(T5_PTCHAR)(pItem->pData) = (T5_CHAR)lValue;
        break;
    case T5C_USINT :
        *(T5_PTBYTE)(pItem->pData) = (T5_BYTE)lValue;
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        *(T5_PTSHORT)(pItem->pData) = (T5_SHORT)lValue;
        break;
    case T5C_UINT :
        *(T5_PTWORD)(pItem->pData) = (T5_WORD)lValue;
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
        *(T5_PTLONG)(pItem->pData) = (T5_LONG)lValue;
        break;
    case T5C_UDINT :
    case T5C_TIME :
        *(T5_PTDWORD)(pItem->pData) = (T5_DWORD)lValue;
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        *(T5_PTREAL)(pItem->pData) = (T5_REAL)lValue;
        break;
#endif /*T5DEF_REALSUPPORTED*/
    case T5C_LINT :
        *(T5_PTLINT)(pItem->pData) = lValue;
        break;
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        *(T5_PTLWORD)(pItem->pData) = lValue;
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        *(T5_PTLREAL)(pItem->pData) = (T5_LREAL)lValue;
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default :
        return T5RET_ERROR;
    }
    return T5RET_OK;
}

#endif /*T5DEF_LINTSUPPORTED*/

/****************************************************************************/

#ifdef T5DEF_REALSUPPORTED

T5_RET T5Map_SetRealValue (T5PTR_DBMAP pItem, T5_REAL rValue)
{
    T5PTR_CODEPUB pPub;

    if (T5Map_IsLocked (pItem))
        return T5RET_ERROR;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    switch (pPub->wTicType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
        *(T5_PTCHAR)(pItem->pData) = (T5_CHAR)(rValue != (T5_REAL)0);
        break;
    case T5C_SINT :
        *(T5_PTCHAR)(pItem->pData) = (T5_CHAR)rValue;
        break;
    case T5C_USINT :
        *(T5_PTBYTE)(pItem->pData) = (T5_BYTE)rValue;
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        *(T5_PTSHORT)(pItem->pData) = (T5_SHORT)rValue;
        break;
    case T5C_UINT :
        *(T5_PTWORD)(pItem->pData) = (T5_WORD)rValue;
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
        *(T5_PTLONG)(pItem->pData) = (T5_LONG)rValue;
        break;
    case T5C_UDINT :
    case T5C_TIME :
        *(T5_PTDWORD)(pItem->pData) = (T5_DWORD)rValue;
        break;
    case T5C_REAL :
        *(T5_PTREAL)(pItem->pData) = rValue;
        break;
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        *(T5_PTLINT)(pItem->pData) = (T5_LINT)rValue;
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        *(T5_PTLWORD)(pItem->pData) = (T5_LINT)rValue;
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        *(T5_PTLREAL)(pItem->pData) = (T5_LREAL)rValue;
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default :
        return T5RET_ERROR;
    }
    return T5RET_OK;
}

#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/

#ifdef T5DEF_LREALSUPPORTED

T5_RET T5Map_SetLRealValue (T5PTR_DBMAP pItem, T5_LREAL lValue)
{
    T5PTR_CODEPUB pPub;

    if (T5Map_IsLocked (pItem))
        return T5RET_ERROR;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);

    switch (pPub->wTicType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
        *(T5_PTCHAR)(pItem->pData) = (T5_CHAR)(lValue != (T5_LREAL)0);
        break;
    case T5C_SINT :
        *(T5_PTCHAR)(pItem->pData) = (T5_CHAR)lValue;
        break;
    case T5C_USINT :
        *(T5_PTBYTE)(pItem->pData) = (T5_BYTE)lValue;
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        *(T5_PTSHORT)(pItem->pData) = (T5_SHORT)lValue;
        break;
    case T5C_UINT :
        *(T5_PTWORD)(pItem->pData) = (T5_WORD)lValue;
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
        *(T5_PTLONG)(pItem->pData) = (T5_LONG)lValue;
        break;
    case T5C_UDINT :
    case T5C_TIME :
        *(T5_PTDWORD)(pItem->pData) = (T5_DWORD)lValue;
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        *(T5_PTREAL)(pItem->pData) = (T5_REAL)lValue;
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        *(T5_PTLINT)(pItem->pData) = (T5_LINT)lValue;
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        *(T5_PTLWORD)(pItem->pData) = (T5_LINT)lValue;
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
    case T5C_LREAL :
        *(T5_PTLREAL)(pItem->pData) = lValue;
        break;
    default :
        return T5RET_ERROR;
    }
    return T5RET_OK;
}

#endif /*T5DEF_LREALSUPPORTED*/

/****************************************************************************/

#ifdef T5DEF_STRINGSUPPORTED

T5_RET T5Map_SetStringValue (T5PTR_DBMAP pItem, T5_PTBYTE pbValue,
                             T5_BYTE bLen)
{
    T5PTR_CODEPUB pPub;

    if (pbValue == NULL)
        return T5RET_ERROR;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    if (pPub->wTicType != T5C_STRING)
        return T5RET_ERROR;

    if (bLen == 0)
        bLen = (T5_BYTE)T5_STRLEN(pbValue);

    return T5VMStr_AscForce ((T5_PTBYTE)(pItem->pData), pbValue, bLen);
}

#endif /*T5DEF_STRINGSUPPORTED*/

/*****************************************************************************
T5Map_SetXVAddress
Set the address of an external variable
Parameters:
    pDB (IN/OUT) VM database header (filled on output)
    pItem (IN) pointer to a map item
    pData (IN) address of the location of the variable
Return: OK or error
*****************************************************************************/

T5_RET T5Map_SetXVAddress (T5PTR_DB pDB, T5PTR_DBMAP pItem, T5_PTR pData)
{
#ifdef T5DEF_XV

    T5_PTBYTE *pXV, pArray;
    T5PTR_CODEPUB pPub;
    T5_WORD i, wOffset;

    pXV = T5GET_DBXV(pDB);
    pPub = (T5PTR_CODEPUB)(pItem->pDef);

    if ((pPub->wTicType & T5C_EXTERN) == 0)
        return T5RET_ERROR;

    /* Set pointers in VM DB and in MAP record */
    pXV[pPub->wIndex] = (T5_PTBYTE)pData;
    pItem->pData = pData;

    /* Set following pointers in case of an array */
    if (pPub->wDim > 1)
    {
        switch (pPub->wTicType & ~T5C_EXTERN)
        {
        case T5C_BOOL   :
        case T5C_SINT  :
        case T5C_USINT : wOffset = 1; break;
        case T5C_INT  :
        case T5C_UINT  : wOffset = 2; break;
        case T5C_DINT   :
        case T5C_UDINT :
        case T5C_TIME   :
        case T5C_REAL   : wOffset = 4; break;
        case T5C_LINT  :
        case T5C_ULINT  :
        case T5C_LREAL : wOffset = 8; break;
        case T5C_STRING   : wOffset = pPub->wStringLength + 3; break;
        default        : wOffset = 0; break;
        }
        pArray = (T5_PTBYTE)pData;
        for (i=1; i<pPub->wDim; i++)
        {
            pArray += wOffset;
            pXV[pPub->wIndex + i] = pArray;
        }
    }
    return T5RET_OK;

#else /*T5DEF_XV*/
    return T5RET_ERROR;
#endif /*T5DEF_XV*/
}

/*****************************************************************************
T5Map_GetTypeName
Get the type name of a map variable
Parameters:
    pItem (IN) pointer to a map item
    pDB (IN/OUT) VM database header (filled on output)
Return: pointer to type name or "?" if not found
*****************************************************************************/

T5_PTCHAR T5Map_GetTypeName (T5PTR_DBMAP pItem, T5PTR_DB pDB)
{
    T5PTR_CODEPUB pPub;
    T5_PTCHAR pTypeName;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    switch (pPub->wTicType & ~T5C_EXTERN)
    {
    case T5C_COMPLEX : break;
    case T5C_BOOL    : return (T5_PTCHAR)"BOOL";
    case T5C_SINT    : return (T5_PTCHAR)"SINT";
    case T5C_USINT   : return (T5_PTCHAR)"USINT";
    case T5C_INT     : return (T5_PTCHAR)"INT";
    case T5C_UINT    : return (T5_PTCHAR)"UINT";
    case T5C_DINT    : return (T5_PTCHAR)"DINT";
    case T5C_UDINT   : return (T5_PTCHAR)"UDINT";
    case T5C_TIME    : return (T5_PTCHAR)"TIME";
    case T5C_REAL    : return (T5_PTCHAR)"REAL";
    case T5C_LINT    : return (T5_PTCHAR)"LINT";
    case T5C_ULINT   : return (T5_PTCHAR)"ULINT";
    case T5C_LREAL   : return (T5_PTCHAR)"LREAL";
    case T5C_STRING  : return (T5_PTCHAR)"STRING";
    default          : return (T5_PTCHAR)"?";
    }

    pTypeName = (T5_PTCHAR)T5Map_GetTypeInfo (pItem, pDB);
    if (pItem == NULL)
        pTypeName = (T5_PTCHAR)"?";
    return pTypeName;
}

/*****************************************************************************
T5Map_GetVSI
Get variable status info for a map variable
Parameters:
    pItem (IN) pointer to a map item
Return: pointer to variable status info structure or NULL if not found
*****************************************************************************/

T5PTR_DBMAPEX T5Map_GetVSI (T5PTR_DBMAP pItem)
{
    T5PTR_CODEPUB pPub;
    T5_PTBYTE pInfo;

    pPub = (T5PTR_CODEPUB)(pItem->pDef);
    if ((pPub->wFlags & T5PUB_VSI) == 0)
        return NULL;
    pInfo = (T5_PTBYTE)pItem;
    pInfo += sizeof (T5STR_DBMAP);
    return (T5PTR_DBMAPEX)pInfo;
}

/*****************************************************************************
T5Map_GetVSIBit
Get value of one VSI bit
Parameters:
    pDB (IN) VM database
    wType (IN) variable data type
    wIndex (IN) variable index (outside CTSEG)
    dwBit (IN) bit number
Return: bit value
*****************************************************************************/

T5_BOOL T5Map_GetVSIBit (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex,
                         T5_DWORD dwBit)
{
    T5PTR_DBMAP pMap;
    T5PTR_DBMAPEX pVSI;
    T5_PTDWORD pdwBit;
    
    pMap = T5Map_GetByIndex (pDB, wType, wIndex);
    if (pMap == NULL)
        return FALSE;
    pVSI = T5Map_GetVSI (pMap);
    if (pVSI == NULL)
        return FALSE;

    if (dwBit < 32L)
        pdwBit = &(pVSI->dwStatLo);
    else if (dwBit < 64L)
    {
        pdwBit = &(pVSI->dwStatHi);
        dwBit -= 32L;
    }
    else
        return FALSE;

    return (((*pdwBit & (1L << dwBit)) != 0) ? TRUE : FALSE);
}

/*****************************************************************************
T5Map_SetVSIBit
Set value of one VSI bit
Parameters:
    pDB (IN) VM database
    wType (IN) variable data type
    wIndex (IN) variable index (outside CTSEG)
    dwBit (IN) bit number
    bValue (IN) bit value
Return: true if OK
*****************************************************************************/

T5_BOOL T5Map_SetVSIBit (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex,
                         T5_DWORD dwBit, T5_BOOL bValue)
{
    T5PTR_DBMAP pMap;
    T5PTR_DBMAPEX pVSI;
    T5_PTDWORD pdwBit;
    
    pMap = T5Map_GetByIndex (pDB, wType, wIndex);
    if (pMap == NULL)
        return FALSE;
    pVSI = T5Map_GetVSI (pMap);
    if (pVSI == NULL)
        return FALSE;

    if (dwBit < 32L)
        pdwBit = &(pVSI->dwStatLo);
    else if (dwBit < 64L)
    {
        pdwBit = &(pVSI->dwStatHi);
        dwBit -= 32L;
    }
    else
        return FALSE;

    if (bValue)
        *pdwBit |= ((T5_DWORD)(1UL << dwBit));
    else
        *pdwBit &= ~((T5_DWORD)(1UL << dwBit));
    return TRUE;
}

/*****************************************************************************
T5Map_ResetSetVSIBits
Reset all VSI bits
Parameters:
    pDB (IN) VM database
    wType (IN) variable data type
    wIndex (IN) variable index (outside CTSEG)
Return: true if OK
*****************************************************************************/

T5_BOOL T5Map_ResetSetVSIBits (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex)
{
    T5PTR_DBMAP pMap;
    T5PTR_DBMAPEX pVSI;
    
    pMap = T5Map_GetByIndex (pDB, wType, wIndex);
    if (pMap == NULL)
        return FALSE;
    pVSI = T5Map_GetVSI (pMap);
    if (pVSI == NULL)
        return FALSE;

    pVSI->dwStatLo = 0L;
    pVSI->dwStatHi = 0L;
    return TRUE;
}

/*****************************************************************************
T5Map_GetVSIDate
Get VSI date stamp
Parameters:
    pDB (IN) VM database
    wType (IN) variable data type
    wIndex (IN) variable index (outside CTSEG)
Return: date stamp
*****************************************************************************/

T5_DWORD T5Map_GetVSIDate (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex)
{
    T5PTR_DBMAP pMap;
    T5PTR_DBMAPEX pVSI;
    
    pMap = T5Map_GetByIndex (pDB, wType, wIndex);
    if (pMap == NULL)
        return 0L;
    pVSI = T5Map_GetVSI (pMap);
    if (pVSI == NULL)
        return 0L;

    return pVSI->dwDate;
}

/*****************************************************************************
T5Map_GetVSIDate
Set VSI date stamp
Parameters:
    pDB (IN) VM database
    wType (IN) variable data type
    wIndex (IN) variable index (outside CTSEG)
    dwValue (IN) new date stamp
Return: true if OK
*****************************************************************************/

T5_BOOL T5Map_SetVSIDate (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex,
                          T5_DWORD dwValue)
{
    T5PTR_DBMAP pMap;
    T5PTR_DBMAPEX pVSI;
    
    pMap = T5Map_GetByIndex (pDB, wType, wIndex);
    if (pMap == NULL)
        return FALSE;
    pVSI = T5Map_GetVSI (pMap);
    if (pVSI == NULL)
        return FALSE;

    pVSI->dwDate = dwValue;
    return TRUE;
}

/*****************************************************************************
T5Map_GetVSITime
Get VSI time stamp
Parameters:
    pDB (IN) VM database
    wType (IN) variable data type
    wIndex (IN) variable index (outside CTSEG)
Return: time stamp
*****************************************************************************/

T5_DWORD T5Map_GetVSITime (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex)
{
    T5PTR_DBMAP pMap;
    T5PTR_DBMAPEX pVSI;
    
    pMap = T5Map_GetByIndex (pDB, wType, wIndex);
    if (pMap == NULL)
        return 0L;
    pVSI = T5Map_GetVSI (pMap);
    if (pVSI == NULL)
        return 0L;

    return pVSI->dwTime;
}

/*****************************************************************************
T5Map_SetVSITime
Set VSI time stamp
Parameters:
    pDB (IN) VM database
    wType (IN) variable data type
    wIndex (IN) variable index (outside CTSEG)
    dwValue (IN) new time stamp
Return: true if OK
*****************************************************************************/

T5_BOOL T5Map_SetVSITime (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex,
                          T5_DWORD dwValue)
{
    T5PTR_DBMAP pMap;
    T5PTR_DBMAPEX pVSI;
    
    pMap = T5Map_GetByIndex (pDB, wType, wIndex);
    if (pMap == NULL)
        return FALSE;
    pVSI = T5Map_GetVSI (pMap);
    if (pVSI == NULL)
        return FALSE;

    pVSI->dwTime = dwValue;
    return TRUE;
}

/*****************************************************************************
T5Map_StampVSI / T5Map_StampVSIGMT
Set VSI date/time stamp with current system clock
Parameters:
    pDB (IN) VM database
    wType (IN) variable data type
    wIndex (IN) variable index (outside CTSEG)
Return: true if OK
*****************************************************************************/

T5_BOOL T5Map_StampVSI (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex)
{
    T5PTR_DBMAP pMap;
    T5PTR_DBMAPEX pVSI;
    
    pMap = T5Map_GetByIndex (pDB, wType, wIndex);
    if (pMap == NULL)
        return FALSE;
    pVSI = T5Map_GetVSI (pMap);
    if (pVSI == NULL)
        return FALSE;

#ifdef T5DEF_RTCLOCKSTAMP
    pVSI->dwDate = T5RtClk_GetCurDateStamp ();
    pVSI->dwTime = T5RtClk_GetCurTimeStamp ();
#else /*T5DEF_RTCLOCKSTAMP*/
    pVSI->dwDate = 0L;
    pVSI->dwTime = 0L;
#endif /*T5DEF_RTCLOCKSTAMP*/

    return TRUE;
}

T5_BOOL T5Map_StampVSIGMT (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex)
{
#ifdef T5DEF_RTCLOCKSTAMPENH

    T5PTR_DBMAP pMap;
    T5PTR_DBMAPEX pVSI;
    T5_BOOL bDST;
    
    pMap = T5Map_GetByIndex (pDB, wType, wIndex);
    if (pMap == NULL)
        return FALSE;
    pVSI = T5Map_GetVSI (pMap);
    if (pVSI == NULL)
        return FALSE;

    T5RtClk_GetCurDateTimeStamp (FALSE, &(pVSI->dwDate), &(pVSI->dwTime), &bDST);
    return TRUE;
#else /*T5DEF_RTCLOCKSTAMPENH*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSTAMPENH*/
}

/*****************************************************************************
T5Map_CopyVSI
Copy VSI info of a variable to another one
Parameters:
    pDB (IN) VM database
    wType (IN) variable data type
    wDst (IN) destination variable index (outside CTSEG)
    wSrc (IN) source variable index (outside CTSEG)
    bBits (IN) true = copy bits
    bStamp (IN) true = copy date/time stamp
Return: true if OK
*****************************************************************************/

T5_BOOL T5Map_CopyVSI (T5PTR_DB pDB, T5_WORD wType, T5_WORD wDst, T5_WORD wSrc,
                       T5_BOOL bBits, T5_BOOL bStamp)
{
    T5PTR_DBMAP pMap;
    T5PTR_DBMAPEX pSrc=NULL, pDst=NULL;
    
    pMap = T5Map_GetByIndex (pDB, wType, wSrc);
    if (pMap != NULL)
        pSrc = T5Map_GetVSI (pMap);
    pMap = T5Map_GetByIndex (pDB, wType, wDst);
    if (pMap != NULL)
        pDst = T5Map_GetVSI (pMap);
    if (pSrc == NULL || pDst == NULL)
        return FALSE;

    if (bBits)
    {
        pDst->dwStatLo = pSrc->dwStatLo;
        pDst->dwStatHi = pSrc->dwStatHi;
    }
    if (bStamp)
    {
        pDst->dwDate = pSrc->dwDate;
        pDst->dwTime = pSrc->dwTime;
    }
    return TRUE;
}

/*****************************************************************************
T5Map_GetByData
Get a map object from a variable data pointer
Parameters:
    pDB (IN) VM database
    pData (IN) pointer to the variable
Return: found object or NULL if not found
*****************************************************************************/

T5PTR_DBMAP T5Map_GetByData (T5PTR_DB pDB, T5_PTR pData)
{
    T5_WORD i;
    T5PTR_DBMAP pMap;

    if (pData == NULL)
        return NULL;

    pMap = T5GET_DBMAP(pDB);
    for (i=0; i<pDB[T5TB_VARMAP].wNbItemUsed; i++)
    {
        if (pData == pMap->pData)
            return pMap;
        pMap = (T5PTR_DBMAP)((T5_PTBYTE)pMap + pDB[T5TB_VARMAP].wSizeOf);
    }
    return NULL;
}

/*****************************************************************************
T5Map_GetVSIByData
Get VSI info of a map object from a variable data pointer
Parameters:
    pDB (IN) VM database
    pData (IN) pointer to the variable
Return: VSI info of found object or NULL if not found
*****************************************************************************/

T5PTR_DBMAPEX T5Map_GetVSIByData (T5PTR_DB pDB, T5_PTR pData)
{
    T5PTR_DBMAP pMap;

    pMap = T5Map_GetByData (pDB, pData);
    if (pMap == NULL)
        return NULL;
    return T5Map_GetVSI (pMap);
}

/*****************************************************************************
T5Map_IsArrayOfSingleVar
Check if a map object points to an array of single variables
Parameters:
    pDB (IN) pointer to teh whole VMDB
    pVar (IN) pointer to the DBMAP object
    pwCount (OUT) total item count of the array
    pwType (OUT) T5C_ data type of elements
Return: pointer to the array or NULL if not an array of single variables
*****************************************************************************/

T5_PTR T5Map_IsArrayOfSingleVar (T5PTR_DB pDB, T5PTR_DBMAP pVar,
                                 T5_PTWORD pwCount, T5_PTWORD pwType)
{
    T5PTR_CODEPUB pDef;
    T5_PTCHAR szType;
    T5_WORD dummy;

    if (pwCount == NULL)
        pwCount = &dummy;
    if (pwType == NULL)
        pwType = &dummy;
    *pwCount = 0;
    *pwType = 0;

    pDef = (T5PTR_CODEPUB)(pVar->pDef);
    if (pDef->wTicType != T5C_COMPLEX)
    {
        if (pDef->wDim < 2)
            return NULL;
        *pwType = pDef->wTicType;
        *pwCount = pDef->wDim;
        return pVar->pData;
    }

    szType = T5Map_GetTypeName (pVar, pDB);
    if (szType == NULL)
        return NULL;
    if (T5_MEMCMP (szType, "BOOL[", 5) == 0)
        *pwType = T5C_BOOL;
    else if (T5_MEMCMP (szType, "SINT[", 5) == 0)
        *pwType = T5C_SINT;
    else if (T5_MEMCMP (szType, "USINT[", 6) == 0 || T5_MEMCMP (szType, "BYTE[", 5) == 0)
        *pwType = T5C_USINT;
    else if (T5_MEMCMP (szType, "INT[", 4) == 0)
        *pwType = T5C_INT;
    else if (T5_MEMCMP (szType, "UINT[", 5) == 0 || T5_MEMCMP (szType, "WORD[", 5) == 0)
        *pwType = T5C_UINT;
    else if (T5_MEMCMP (szType, "DINT[", 5) == 0)
        *pwType = T5C_DINT;
    else if (T5_MEMCMP (szType, "UDINT[", 6) == 0 || T5_MEMCMP (szType, "DWORD[", 6) == 0)
        *pwType = T5C_UDINT;
    else if (T5_MEMCMP (szType, "LINT[", 5) == 0)
        *pwType = T5C_LINT;
    else if (T5_MEMCMP (szType, "ULINT[", 6) == 0 || T5_MEMCMP (szType, "LWORD[", 6) == 0)
        *pwType = T5C_ULINT;
    else if (T5_MEMCMP (szType, "REAL[", 5) == 0)
        *pwType = T5C_REAL;
    else if (T5_MEMCMP (szType, "LREAL[", 6) == 0)
        *pwType = T5C_LREAL;
    else if (T5_MEMCMP (szType, "TIME[", 5) == 0)
        *pwType = T5C_TIME;
    else if (T5_MEMCMP (szType, "STRING[", 7) == 0)
        *pwType = T5C_STRING;
    else
        return NULL;

    *pwCount = 1;
    while (*szType != '[')
        szType++;
    szType++;
    while (*szType >= '0' && *szType <= '9')
    {
        *pwCount *= T5_ATOI (szType);
        while (*szType >= '0' && *szType <= '9')
            szType++;
        if (*szType == ',')
            szType++;
    }
    return pVar->pData;
}

/****************************************************************************/

#endif /*T5DEF_VARMAP*/

/* eof **********************************************************************/
