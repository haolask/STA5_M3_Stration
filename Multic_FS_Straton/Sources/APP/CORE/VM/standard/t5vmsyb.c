/*****************************************************************************
T5VMSyb.c  : symbol parser

DO NOT ALTER THIS !

(c) COPALP 2008
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/

#ifdef T5DEF_VARMAP

/* parse single object symbols **********************************************/

static T5PTR_DBMAP _T5VmSyb_GetMapItem (T5PTR_DB pDB, T5_PTCHAR *pszSyb)
{
    T5PTR_DBMAP pMap;
    T5_CHAR szSyb[256];
    T5_WORD wLen;

    wLen = 0;
    while (**pszSyb != '\0' && **pszSyb != '.' && **pszSyb != '[')
    {
        if (wLen > 255)
            return NULL;
        szSyb[wLen] = **pszSyb;
        wLen += 1;
        *pszSyb += 1;
    }
    szSyb[wLen] = '\0';

    pMap = T5Map_GetBySymbol (pDB, szSyb);
    if (pMap != NULL)
        return pMap;

    while (**pszSyb == '.')
    {
        do
        {
            if (wLen > 255)
                return NULL;
            szSyb[wLen] = **pszSyb;
            wLen += 1;
            *pszSyb += 1;
        }
        while (**pszSyb != '\0' && **pszSyb != '.' && **pszSyb != '[');
        szSyb[wLen] = '\0';
        pMap = T5Map_GetBySymbol (pDB, szSyb);
        if (pMap != NULL)
            return pMap;
    }

    return NULL;
}

static T5_PTR _T5VmSyb_ParseMapArrayItem (T5PTR_DB pDB, T5PTR_DBMAP pMap, T5_PTCHAR szSyb)
{
    T5_WORD wIndex;
    T5PTR_CODEPUB pPub;
    T5_PTBYTE pBase;
    T5_PTBYTE *pStrings;

    if (*szSyb != '[')
        return NULL;
    szSyb += 1;

    wIndex = 0;
    while (*szSyb >= '0' && *szSyb <= '9')
    {
        wIndex *= 10;
        wIndex += (T5_WORD)(*szSyb - '0') & 0xff;
        szSyb++;
    }

    if (T5VMStr_ICmp ((T5_PTBYTE)szSyb, (T5_PTBYTE)"]") != 0) /* case insensitive */
        return NULL;

    pPub = (T5PTR_CODEPUB)(pMap->pDef);
    if (wIndex >= pPub->wDim)
        return NULL;

    pBase = (T5_PTBYTE)(pMap->pData);
    switch (pPub->wTicType)
    {
    case T5C_BOOL :
    case T5C_SINT :
    case T5C_USINT :
        return (T5_PTR)(pBase + wIndex);
    case T5C_INT :
    case T5C_UINT :
        return (T5_PTR)(pBase + 2 * wIndex);
    case T5C_DINT :
    case T5C_UDINT :
    case T5C_REAL :
    case T5C_TIME :
        return (T5_PTR)(pBase + 4 * wIndex);
    case T5C_LINT :
    case T5C_ULINT :
    case T5C_LREAL :
        return (T5_PTR)(pBase + 8 * wIndex);
    case T5C_STRING :
        pStrings = T5GET_DBSTRING(pDB);
        return (T5_PTR)(pStrings[pPub->wIndex + wIndex]);
    default : break;
    }
    return NULL;
}

static T5_PTBYTE _T5VmSyb_SkipName (T5_PTCHAR pszSyb)
{
    while (*pszSyb)
        pszSyb++;
    pszSyb++;
    return (T5_PTBYTE)pszSyb;
}

static T5_BOOL _T5VmSyb_PastCTDims (T5_PTCHAR *pszSyb, T5_WORD nb, T5_PTBYTE pDims,
                                    T5_PTDWORD pdwOffset)
{
    T5_WORD i;
    T5_DWORD dwIndex, dwMax;

    if (**pszSyb != '[')
        return FALSE;
    *pszSyb += 1;

    *pdwOffset = 0L;
    for (i=0; i<nb; i++)
    {
        dwIndex = 0L;
        while (**pszSyb >= '0' && **pszSyb <= '9')
        {
            dwIndex *= 10L;
            dwIndex += (T5_DWORD)(**pszSyb - '0') & 0x0ffL;
            *pszSyb += 1;
        }

        T5_MEMCPY (&dwMax, pDims + ((nb-i-1) * 4), sizeof (T5_DWORD));
        if (dwIndex >= dwMax)
            return FALSE;

        *pdwOffset *= dwMax;
        *pdwOffset += dwIndex;

        if (i < (nb-1))
        {
            if (**pszSyb != ',')
                return FALSE;
            *pszSyb += 1;
        }
    }

    if (**pszSyb != ']')
        return FALSE;
    *pszSyb += 1;

    return TRUE;
}

static T5_PTBYTE _T5VmSyb_ParseCTItem (T5PTR_DB pDB, T5_PTBYTE pData, T5_PTBYTE pType,
                                       T5_PTCHAR *pszSyb, T5_PTBYTE pbType, T5_PTCHAR *pszType)
{
    T5_WORD i, nb, wMember, wLen, wBase;
    T5_PTBYTE pMember;
    T5_PTBYTE pTypeInfo;
    T5_DWORD dwOffset, dwSizeof;
    T5PTR_DBPRIVATE pPrivate;

    pPrivate = T5GET_DBPRIVATE(pDB);
    pTypeInfo = (T5_PTBYTE)T5VMCode_GetCTTypeDef (pPrivate->pCode);
    if (pszType != NULL)
        *pszType = (T5_PTCHAR)pType;
    pType = (T5_PTBYTE)_T5VmSyb_SkipName ((T5_PTCHAR)pType);

    if (**pszSyb == '.')
    {
        *pszSyb += 1;
        if (*pType != T5TYPEDEF_STRUCT && *pType != T5TYPEDEF_CFB)
            return NULL;
        pType++;

        T5_MEMCPY (&nb, pType, sizeof (T5_WORD));
        pType += 2;
        T5_MEMCPY (&wMember, pType, sizeof (T5_WORD));
        pMember = pTypeInfo + wMember;
        for (i=0; i<nb; i++)
        {
            wLen = (T5_WORD)T5_STRLEN(pMember);
            if (T5VMStr_NICmp (pMember, (T5_PTBYTE)(*pszSyb), wLen) == 0
                && ((*pszSyb)[wLen] == '\0' || (*pszSyb)[wLen] == '.' || (*pszSyb)[wLen] == '['))
            {
                *pszSyb += wLen;

                pMember = _T5VmSyb_SkipName ((T5_PTCHAR)pMember);
                if (*pMember++ != T5TYPEDEF_MEMBER)
                    return NULL;

                T5_MEMCPY (&wBase, pMember, sizeof (T5_WORD));
                T5_MEMCPY (&dwOffset, pMember+2, sizeof (T5_DWORD));

                pData += dwOffset;
                pType = pTypeInfo + wBase;
                return _T5VmSyb_ParseCTItem (pDB, pData, pType, pszSyb, pbType, pszType);
            }
            else
            {
                pMember = _T5VmSyb_SkipName ((T5_PTCHAR)pMember);
                pMember += 7;
            }
        }
        return NULL;
    }
    else if (**pszSyb == '[')
    {
        switch (*pType)
        {
        case T5TYPEDEF_ARRAY1 : nb = 1; break;
        case T5TYPEDEF_ARRAY2 : nb = 2; break;
        case T5TYPEDEF_ARRAY3 : nb = 3; break;
        default : return NULL;
        }
        pType++;

        T5_MEMCPY (&wBase, pType, sizeof (T5_WORD));
        T5_MEMCPY (&dwSizeof, pType+2, sizeof (T5_DWORD));

        if (!_T5VmSyb_PastCTDims (pszSyb, nb, pType + 6, &dwOffset))
            return NULL;

        pData += (dwOffset * dwSizeof);
        pType = pTypeInfo + wBase;
        return _T5VmSyb_ParseCTItem (pDB, pData, pType, pszSyb, pbType, pszType);
    }
    else if (**pszSyb == '\0')
    {
        if (*pType != T5TYPEDEF_BASE)
        {
            if (pszType != NULL)
            {
                *pbType = T5C_COMPLEX;
                return pData;
            }
            else
                return NULL;
        }
        *pbType = pType[1];
        return pData;
    }
    return NULL;
}

static T5_PTR _T5VmSyb_ParseCTSegItem (T5PTR_DB pDB, T5PTR_DBMAP pMap, T5_PTCHAR szSyb,
                                       T5_PTBYTE pbType, T5_PTCHAR *pszType)
{
    T5_PTBYTE pType;
    
    pType = T5Map_GetTypeInfo (pMap, pDB);
    if (pType == NULL)
        return NULL;

    return (T5_PTR)_T5VmSyb_ParseCTItem (pDB, (T5_PTBYTE)(pMap->pData), pType, &szSyb, pbType, pszType);
}

T5_PTR T5VmSyb_ParseSingle (T5PTR_DB pDB, T5_PTCHAR szSyb, T5_PTBYTE pbType)
{
    T5PTR_DBMAP pMap;
    T5PTR_CODEPUB pPub;

    *pbType = 0;
    pMap = _T5VmSyb_GetMapItem (pDB, &szSyb);
    if (pMap == NULL)
        return NULL;
    pPub = (T5PTR_CODEPUB)(pMap->pDef);

    /* CTSeg item */
    if (pPub->wTicType == T5C_COMPLEX)
        return _T5VmSyb_ParseCTSegItem (pDB, pMap, szSyb, pbType, NULL);

    *pbType = (T5_BYTE)(pPub->wTicType);

    /* array in DB tables? */
    if (pPub->wDim > 1)
        return _T5VmSyb_ParseMapArrayItem (pDB, pMap, szSyb);

    /* single */
    if (*szSyb != '\0')
        return NULL;
    return pMap->pData;
}

T5_PTR T5VmSyb_Parse (T5PTR_DB pDB, T5_PTCHAR szSyb, T5_PTBYTE pbType, T5_PTCHAR *pszType)
{
    T5PTR_DBMAP pMap;
    T5PTR_CODEPUB pPub;

    if (pszType != NULL)
        *pszType = '\0';

    *pbType = 0;
    pMap = _T5VmSyb_GetMapItem (pDB, &szSyb);
    if (pMap == NULL)
        return NULL;
    pPub = (T5PTR_CODEPUB)(pMap->pDef);

    /* CTSeg item */
    if (pPub->wTicType == T5C_COMPLEX)
        return _T5VmSyb_ParseCTSegItem (pDB, pMap, szSyb, pbType, pszType);

    *pbType = (T5_BYTE)(pPub->wTicType);

    /* array in DB tables? */
    if (pPub->wDim > 1)
        return _T5VmSyb_ParseMapArrayItem (pDB, pMap, szSyb);

    /* single */
    if (*szSyb != '\0')
        return NULL;

    if (pszType != NULL)
        *pszType = T5Map_GetTypeName (pMap, pDB);
    return pMap->pData;
}

/* enumerate members ********************************************************/

static T5_BOOL _T5VmSyb_EnumMembers (T5PTR_DB pDB, T5_PTBYTE pType,
                                     T5_PTCHAR szBuffer, T5_WORD wBufSize,
                                     T5HND_CTSYBENUM pfCallback, T5_DWORD dwData)
{
    T5PTR_DBPRIVATE pPrivate;
    T5_PTBYTE pTypeInfo, pBaseType, pMember;
    T5_PTCHAR szName, szMember;
    T5_BOOL bOK;
    T5_CHAR szIndex[32];
    T5_DWORD i, j, k, dim1, dim2, dim3;
    T5_WORD wLen, wOrgLen, nb;
    T5_WORD wBaseTypeIndex;

    pPrivate = T5GET_DBPRIVATE(pDB);
    pTypeInfo = (T5_PTBYTE)T5VMCode_GetCTTypeDef (pPrivate->pCode);

    bOK = TRUE;
    szName = (T5_PTCHAR)pType;
    pType = (T5_PTBYTE)_T5VmSyb_SkipName (szName);
    switch (*pType)
    {
    case T5TYPEDEF_BASE :
        pfCallback (szBuffer, pType[1], dwData);
        break;
    case T5TYPEDEF_ARRAY1 :
        T5_MEMCPY (&wBaseTypeIndex, pType + 1, sizeof (T5_WORD));
        pBaseType = pTypeInfo + wBaseTypeIndex;
        T5_MEMCPY (&dim1, pType + 7, sizeof (T5_DWORD));
        wOrgLen = T5_STRLEN (szBuffer);
        for (i=0; bOK && i<dim1; i++)
        {
            sprintf ((char *)szIndex, "[%lu]", i);
            wLen = T5_STRLEN (szIndex);
            if ((wLen + wOrgLen) >= wBufSize)
                bOK = FALSE;
            else
            {
                T5_STRCAT (szBuffer, szIndex);
                bOK &= _T5VmSyb_EnumMembers (pDB, pBaseType, szBuffer, wBufSize, pfCallback, dwData);
                szBuffer[wOrgLen] = '\0';
            }
        }
        break;
    case T5TYPEDEF_ARRAY2 :
        T5_MEMCPY (&wBaseTypeIndex, pType + 1, sizeof (T5_WORD));
        pBaseType = pTypeInfo + wBaseTypeIndex;
        T5_MEMCPY (&dim1, pType + 7, sizeof (T5_DWORD));
        T5_MEMCPY (&dim2, pType + 11, sizeof (T5_DWORD));
        wOrgLen = T5_STRLEN (szBuffer);
        for (i=0; bOK && i<dim1; i++)
        {
            for (j=0; bOK && j<dim2; j++)
            {
                sprintf ((char *)szIndex, "[%lu,%lu]", i, j);
                wLen = T5_STRLEN (szIndex);
                if ((wLen + wOrgLen) >= wBufSize)
                    bOK = FALSE;
                else
                {
                    T5_STRCAT (szBuffer, szIndex);
                    bOK &= _T5VmSyb_EnumMembers (pDB, pBaseType, szBuffer, wBufSize, pfCallback, dwData);
                    szBuffer[wOrgLen] = '\0';
                }
            }
        }
        break;
    case T5TYPEDEF_ARRAY3 :
        T5_MEMCPY (&wBaseTypeIndex, pType + 1, sizeof (T5_WORD));
        pBaseType = pTypeInfo + wBaseTypeIndex;
        T5_MEMCPY (&dim1, pType + 7, sizeof (T5_DWORD));
        T5_MEMCPY (&dim2, pType + 11, sizeof (T5_DWORD));
        T5_MEMCPY (&dim3, pType + 15, sizeof (T5_DWORD));
        wOrgLen = T5_STRLEN (szBuffer);
        for (i=0; bOK && i<dim1; i++)
        {
            for (j=0; bOK && j<dim2; j++)
            {
                for (k=0; bOK && k<dim3; k++)
                {
                    sprintf ((char *)szIndex, "[%lu,%lu,%lu]", i, j,k);
                    wLen = T5_STRLEN (szIndex);
                    if ((wLen + wOrgLen) >= wBufSize)
                        bOK = FALSE;
                    else
                    {
                        T5_STRCAT (szBuffer, szIndex);
                        bOK &= _T5VmSyb_EnumMembers (pDB, pBaseType, szBuffer, wBufSize, pfCallback, dwData);
                        szBuffer[wOrgLen] = '\0';
                    }
                }
            }
        }
        break;
    case T5TYPEDEF_STRUCT :
    case T5TYPEDEF_CFB :
        T5_MEMCPY (&nb, pType+1, sizeof (T5_WORD));
        T5_MEMCPY (&wBaseTypeIndex, pType+3, sizeof (T5_WORD));
        wOrgLen = T5_STRLEN (szBuffer);
        pMember = pTypeInfo + wBaseTypeIndex;
        while (nb--)
        {
            szMember = (T5_PTCHAR)pMember;
            pMember = (T5_PTBYTE)_T5VmSyb_SkipName (szMember);
            T5_MEMCPY (&wBaseTypeIndex, pMember + 1, sizeof (T5_WORD));
            pBaseType = pTypeInfo + wBaseTypeIndex;
            wLen = T5_STRLEN (szMember);
            if ((wLen + wOrgLen + 1) >= wBufSize)
                bOK = FALSE;
            else
            {
                T5_STRCAT (szBuffer, ".");
                T5_STRCAT (szBuffer, szMember);
                bOK &= _T5VmSyb_EnumMembers (pDB, pBaseType, szBuffer, wBufSize, pfCallback, dwData);
                szBuffer[wOrgLen] = '\0';
            }
            pMember += 7;
        }
        break;
    default : break;
    }
    return bOK;
}

T5_BOOL T5VmSyb_EnumCTMembers (T5PTR_DB pDB, T5PTR_DBMAP pMap,
                               T5_PTCHAR szBuffer, T5_WORD wBufSize,
                               T5HND_CTSYBENUM pfCallback, T5_DWORD dwData)
{
    T5_PTBYTE pType;
    T5PTR_CODEPUB pPub;

    pPub = (T5PTR_CODEPUB)(pMap->pDef);
    if (pPub->wTicType != T5C_COMPLEX)
        return FALSE;

    pType = T5Map_GetTypeInfo (pMap, pDB);
    if (pType == NULL)
        return FALSE;

    *szBuffer = '\0';
    return _T5VmSyb_EnumMembers (pDB, pType, szBuffer, wBufSize, pfCallback, dwData);
}

/****************************************************************************/

#endif /*T5DEF_VARMAP*/

/* eof **********************************************************************/
