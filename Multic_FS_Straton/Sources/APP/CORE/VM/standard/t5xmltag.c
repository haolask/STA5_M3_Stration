/*****************************************************************************
T5xmltag.c : XML tag object

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_XML

/****************************************************************************/

static T5_DWORD _T5XML_AllocString (T5_PTR pHeap, T5_PTCHAR szText)
{
    T5_DWORD hStr;
    T5PTR_XMLSTR pStr;
    T5_PTCHAR pText;

    hStr = T5Heap_Alloc (pHeap, sizeof (T5STR_XMLSTR) + T5_STRLEN (szText) + 1L);
    if (hStr == 0L)
        return 0L;
    pStr = (T5PTR_XMLSTR)T5Heap_Lock (pHeap, hStr);
    pStr->dwSize = T5_STRLEN (szText) + 1L;
    pText = ((T5_PTCHAR)pStr) + sizeof (T5STR_XMLSTR);
    T5_STRCPY (pText, szText);
    T5Heap_Unlock (pHeap, hStr);
    return hStr;
}

static T5_DWORD _T5XML_ReallocString (T5_PTR pHeap, T5_DWORD hStr, T5_PTCHAR szText)
{
    T5PTR_XMLSTR pStr;
    T5_PTCHAR pText;
    T5_DWORD hNew;

    /* first allocation */
    if (hStr == 0L)
        return _T5XML_AllocString (pHeap, szText);
    /* small enbough: keep the same buffer */
    pStr = (T5PTR_XMLSTR)T5Heap_Lock (pHeap, hStr);
    if (T5_STRLEN(szText) < pStr->dwSize)
    {
        pText = ((T5_PTCHAR)pStr) + sizeof (T5STR_XMLSTR);
        T5_STRCPY (pText, szText);
        T5Heap_Unlock (pHeap, hStr);
        return hStr;
    }
    /* realloc */
    hNew = _T5XML_AllocString (pHeap, szText);
    T5Heap_Unlock (pHeap, hStr);
    T5Heap_Free (pHeap, hStr);
    return hNew;
}

static T5_DWORD _T5XML_ConcatString (T5_PTR pHeap, T5_DWORD hStr, T5_PTCHAR szText)
{
    T5PTR_XMLSTR pStr, pNewStr;
    T5_PTCHAR pText, pNewText;
    T5_DWORD hNew, dwCatLen;

    /* first allocation */
    if (hStr == 0L)
        return _T5XML_AllocString (pHeap, szText);
    /* small enbough: keep the same buffer */
    pStr = (T5PTR_XMLSTR)T5Heap_Lock (pHeap, hStr);
    pText = ((T5_PTCHAR)pStr) + sizeof (T5STR_XMLSTR);
    dwCatLen = T5_STRLEN(szText) + T5_STRLEN(pText);
    if (dwCatLen < pStr->dwSize)
    {
        T5_STRCAT (pText, szText);
        T5Heap_Unlock (pHeap, hStr);
        return hStr;
    }
    /* realloc and cat */
    hNew = T5Heap_Alloc (pHeap, dwCatLen + 1L + sizeof (T5STR_XMLSTR));
    pNewStr = (T5PTR_XMLSTR)T5Heap_Lock (pHeap, hNew);
    pNewStr->dwSize = dwCatLen + 1L;
    pNewText = ((T5_PTCHAR)pNewStr) + sizeof (T5STR_XMLSTR);
    T5_STRCPY (pNewText, pText);
    T5_STRCAT (pNewText, szText);
    T5Heap_Unlock (pHeap, hNew);
    /* free old buffer */
    T5Heap_Unlock (pHeap, hStr);
    T5Heap_Free (pHeap, hStr);
    return hNew;
}

static T5_BOOL _T5XML_CheckString (T5_PTR pHeap, T5_DWORD hStr, T5_PTCHAR szText)
{
    T5_PTCHAR pText;
    T5_BOOL bSame;

    if (hStr == 0L)
        return FALSE;
    pText = (T5_PTCHAR)T5Heap_Lock (pHeap, hStr);
    pText += + sizeof (T5STR_XMLSTR);
    bSame = (T5VMStr_ICmp ((T5_PTBYTE)pText, (T5_PTBYTE)szText) == 0);
    T5Heap_Unlock (pHeap, hStr);
    return bSame;
}

/****************************************************************************/

static T5_DWORD _T5XML_CreateAttrib (T5_PTR pHeap, T5_PTCHAR szName, T5_PTCHAR szValue)
{
    T5_DWORD hAttrib;
    T5PTR_XMLATTR pAttrib;

    hAttrib = T5Heap_Alloc (pHeap, sizeof (T5STR_XMLATTR));
    if (hAttrib == 0L)
        return 0L;

    pAttrib = (T5PTR_XMLATTR)T5Heap_Lock (pHeap, hAttrib);
    pAttrib->hName = _T5XML_AllocString (pHeap, szName);
    if (pAttrib->hName == 0L)
    {
        T5Heap_Unlock (pHeap, hAttrib);
        T5Heap_Free (pHeap, hAttrib);
        return 0L;
    }
    pAttrib->hText = _T5XML_AllocString (pHeap, szValue);
    if (pAttrib->hText == 0L)
    {
        T5Heap_Free (pHeap, pAttrib->hName);
        T5Heap_Unlock (pHeap, hAttrib);
        T5Heap_Free (pHeap, hAttrib);
        return 0L;
    }
    T5Heap_Unlock (pHeap, hAttrib);
    return hAttrib;
}

/****************************************************************************/

static T5_DWORD _T5XML_DeleteTag (T5_PTR pHeap, T5_DWORD hTag)
{
    T5PTR_XMLTAG pXml;
    T5_DWORD hSibling, hNext, hAttrib;
    T5PTR_XMLATTR pAttrib;
    T5_DWORD nb = 0;

    while (hTag)
    {
        pXml = (T5PTR_XMLTAG)T5Heap_Lock (pHeap, hTag);
        hSibling = pXml->hSibling;
        /* delete tag name and contents */
        T5Heap_Free (pHeap, pXml->hName);
        if (pXml->hCont)
            T5Heap_Free (pHeap, pXml->hCont);
        /* delete child tag */
        if (pXml->hChild)
            nb += _T5XML_DeleteTag (pHeap, pXml->hChild);
        /* delete attrib list */
        hAttrib = pXml->hAttrib;
        while (hAttrib)
        {
            pAttrib = (T5PTR_XMLATTR)T5Heap_Lock (pHeap, hAttrib);
            hNext = pAttrib->hNext;
            T5Heap_Free (pHeap, pAttrib->hName);
            T5Heap_Free (pHeap, pAttrib->hText);
            T5Heap_Unlock (pHeap, hAttrib);
            T5Heap_Free (pHeap, hAttrib);
            hAttrib = hNext;
        }
        /* delete tag */
        T5Heap_Unlock (pHeap, hTag);
        T5Heap_Free (pHeap, hTag);
        nb += 1;
        /* delete sibling tags - dont recurse in raw list! */
        hTag = hSibling;
    }
    return nb;
}

static void _T5XML_RemoveChildTag (T5_PTR pHeap, T5_DWORD hParent,
                                   T5_DWORD hTag, T5_DWORD hSibling)
{
    T5PTR_XMLTAG pXml;
    T5_BOOL bDone;
    T5_DWORD hCur, hNext;

    bDone = FALSE;
    hCur = 0L;
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pHeap, hParent);
    if (pXml->hChild == hTag)
    {
        pXml->hChild = hSibling;
        bDone = TRUE;
    }
    else
        hCur = pXml->hChild;
    T5Heap_Unlock (pHeap, hParent);
    while (!bDone && hCur != 0L)
    {
        pXml = (T5PTR_XMLTAG)T5Heap_Lock (pHeap, hCur);
        hNext = pXml->hSibling;
        if (pXml->hSibling == hTag)
        {
            pXml->hSibling = hSibling;
            bDone = TRUE;
        }
        T5Heap_Unlock (pHeap, hCur);
        hCur = hNext;
    }
}

/****************************************************************************/

T5_DWORD T5XML_Alloc (T5PTR_DB pDB, T5_PTCHAR szName)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5_DWORD hName, hXml;
    T5PTR_XMLTAG pXml;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* allocate name string */
    hName = _T5XML_AllocString (pPrivate->pHeap, szName);
    if (hName == 0L)
    {
        pStatus->dwErrorXML = XMLERR_OVERFLOW;
        return 0L;
    }
    /* allocate record */
    hXml = T5Heap_Alloc (pPrivate->pHeap, sizeof (T5STR_XMLTAG));
    if (hXml == 0L)
    {
        pStatus->dwErrorXML = XMLERR_OVERFLOW;
        T5Heap_Free (pPrivate->pHeap, hName);
        return 0L;
    }
    /* init */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    pXml->dwKind = T5HP_XML;
    pXml->hName = hName;
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    pStatus->dwErrorXML = 0L;
    pStatus->dwNbXML += 1L;
    return hXml;
}

T5_BOOL T5XML_Free (T5PTR_DB pDB, T5_DWORD hXml)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml;
    T5_DWORD hParent, hSibling, nb;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return FALSE;
    }
    if (pXml->dwKind != T5HP_XML)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return FALSE;
    }
    /* remember lmocation in parent list */
    hParent = pXml->hParent;
    hSibling = pXml->hSibling;
    pXml->hSibling = 0L;
    /* unlock record */
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    /* remove from parent list */
    if (hParent)
        _T5XML_RemoveChildTag (pPrivate->pHeap, hParent, hXml, hSibling);
    /* delete record */
    nb = _T5XML_DeleteTag (pPrivate->pHeap, hXml) + 1;
    pStatus->dwErrorXML = 0L;
    if (pStatus->dwNbXML > nb)
        pStatus->dwNbXML -= nb;
    else
        pStatus->dwNbXML = 0;
    T5Heap_Compact (pPrivate->pHeap, 16);
    return TRUE;
}

T5_BOOL T5XML_GetTagName (T5PTR_DB pDB, T5_DWORD hXml,
                          T5_PTCHAR pBuffer, T5_DWORD dwBufSize)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml;
    T5_DWORD dwLen;
    T5_PTCHAR pText;

    if (dwBufSize < 1L)
        return FALSE;
    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return FALSE;
    }
    if (pXml->dwKind != T5HP_XML || pXml->hName == 0L)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return FALSE;
    }
    /* get text */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXml->hName);
    pText += sizeof (T5STR_XMLSTR);
    dwLen = T5_STRLEN (pText);
    if (dwLen > (dwBufSize - 1L))
        dwLen = dwBufSize - 1L;
    T5_MEMCPY (pBuffer, pText, dwLen);
    pBuffer[dwLen] = '\0';
    T5Heap_Unlock (pPrivate->pHeap, pXml->hName);
    /* unlock and free record */
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    pStatus->dwErrorXML = 0L;
    return TRUE;
}

T5_BOOL T5XML_SetTagContents (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR pBuffer)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml;
    T5_BOOL bRet;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return FALSE;
    }
    if (pXml->dwKind != T5HP_XML)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return FALSE;
    }
    /* set text */
    pXml->hCont = _T5XML_ReallocString (pPrivate->pHeap, pXml->hCont, pBuffer);
    bRet = (pXml->hCont != 0L);
    /* unlock and free record */
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    if (bRet)
        pStatus->dwErrorXML = 0L;
    else
        pStatus->dwErrorXML = XMLERR_OVERFLOW;
    return bRet;
}

T5_BOOL T5XML_AppendContents (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR pBuffer)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml;
    T5_BOOL bRet;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return FALSE;
    }
    if (pXml->dwKind != T5HP_XML)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return FALSE;
    }
    /* set text */
    pXml->hCont = _T5XML_ConcatString (pPrivate->pHeap, pXml->hCont, pBuffer);
    bRet = (pXml->hCont != 0L);
    /* unlock and free record */
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    if (bRet)
        pStatus->dwErrorXML = 0L;
    else
        pStatus->dwErrorXML = XMLERR_OVERFLOW;
    return bRet;
}

T5_BOOL T5XML_GetTagContents (T5PTR_DB pDB, T5_DWORD hXml,
                              T5_PTCHAR pBuffer, T5_DWORD dwBufSize)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml;
    T5_DWORD dwLen;
    T5_PTCHAR pText;

    if (dwBufSize < 1L)
        return FALSE;
    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return FALSE;
    }
    if (pXml->dwKind != T5HP_XML)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return FALSE;
    }
    /* get text */
    if (pXml->hCont == 0L)
        *pBuffer = '\0';
    else
    {
        pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXml->hCont);
        pText += sizeof (T5STR_XMLSTR);
        dwLen = T5_STRLEN (pText);
        if (dwLen > (dwBufSize - 1L))
            dwLen = dwBufSize - 1L;
        T5_MEMCPY (pBuffer, pText, dwLen);
        pBuffer[dwLen] = '\0';
        T5Heap_Unlock (pPrivate->pHeap, pXml->hCont);
    }
    /* unlock and free record */
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    pStatus->dwErrorXML = 0L;
    return TRUE;
}

T5_BOOL T5XML_SetTagContentsXS (T5PTR_DB pDB, T5_DWORD hXml, T5_DWORD hXS)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5_DWORD hText;
    T5_PTCHAR pText;
    T5_BOOL bRet;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* check and lock text */
    hText = T5XS_GetTextHandle (pDB, hXS);
    if (hText == 0L)
    {
        pStatus->dwErrorXS = XMLERR_BADTXB;
        return FALSE;
    }
    /* set contents */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, hText);
    bRet = T5XML_SetTagContents (pDB, hXml, pText);
    T5Heap_Unlock (pPrivate->pHeap, hText);
    return bRet;
}

T5_DWORD T5XML_GetTagContentsXS (T5PTR_DB pDB, T5_DWORD hXml)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml;
    T5_DWORD hXS;
    T5_PTCHAR pText;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return 0L;
    }
    if (pXml->dwKind != T5HP_XML)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return 0L;
    }
    /* get text */
    if (pXml->hCont == 0L)
        hXS = T5XS_Alloc (pDB, 32L, NULL);
    else
    {
        pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXml->hCont);
        pText += sizeof (T5STR_XMLSTR);
        hXS = T5XS_Alloc (pDB, 0L, pText);
        T5Heap_Unlock (pPrivate->pHeap, pXml->hCont);
    }
    /* unlock and free record */
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    if (hXS == 0L)
        pStatus->dwErrorXML = XMLERR_OVERFLOW;
    else
        pStatus->dwErrorXML = 0L;
    return hXS;
}
T5_DWORD T5XML_AddChild (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR szName)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml, pQueue;
    T5_DWORD hChild, hQueue, hNext;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return 0L;
    }
    if (pXml->dwKind != T5HP_XML)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return 0L;
    }
    /* create child node */
    hChild = T5XML_Alloc (pDB, szName);
    if (hChild == 0L)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_OVERFLOW;
        return 0L;
    }
    /* first child */
    if (pXml->hChild == 0L)
        pXml->hChild = hChild;
    /* add to queue */
    else
    {
        hQueue = pXml->hChild;
        pQueue = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hQueue);
        while (pQueue->hSibling)
        {
            hNext = pQueue->hSibling;
            T5Heap_Unlock (pPrivate->pHeap, hQueue);
            hQueue = hNext;
            pQueue = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hQueue);
        }
        pQueue->hSibling = hChild;
        T5Heap_Unlock (pPrivate->pHeap, hQueue);
    }
    /* unlock record */
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    /* set child parent handle */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hChild);
    pXml->hParent = hXml;
    T5Heap_Unlock (pPrivate->pHeap, hChild);
    pStatus->dwErrorXML = 0L;
    return hChild;
}

static T5_DWORD _T5XML_SearchSibling (T5_PTR pHeap, T5_DWORD hNext, T5_PTCHAR szName)
{
    T5PTR_XMLTAG pXml;
    T5_PTCHAR pText;
    T5_BYTE bComp;
    T5_DWORD hSibling;

    while (hNext != 0L)
    {
        pXml = (T5PTR_XMLTAG)T5Heap_Lock (pHeap, hNext);
        pText = (T5_PTCHAR)T5Heap_Lock (pHeap, pXml->hName);
        pText += sizeof (T5STR_XMLSTR);
        bComp = T5VMStr_ICmp ((T5_PTBYTE)pText, (T5_PTBYTE)szName);
        T5Heap_Unlock (pHeap, pXml->hName);
        hSibling = pXml->hSibling;
        T5Heap_Unlock (pHeap, hNext);
        if (bComp == 0)
            return hNext;
        hNext = hSibling;
    }
    return 0L;
}

T5_DWORD T5XML_FirstChild (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR szName)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml;
    T5_DWORD hChild;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return 0L;
    }
    if (pXml->dwKind != T5HP_XML)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return 0L;
    }
    /* get first child */
    hChild = pXml->hChild;
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    pStatus->dwErrorXML = 0L;
    /* search for wished name */
    if (hChild && szName != NULL && *szName != '\0')
        hChild = _T5XML_SearchSibling (pPrivate->pHeap, hChild, szName);
    return hChild;
}

T5_DWORD T5XML_NextChild (T5PTR_DB pDB, T5_DWORD hXml, T5_DWORD hChild,
                          T5_PTCHAR szName)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml;
    T5_DWORD hNext;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hChild);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return 0L;
    }
    if (pXml->dwKind != T5HP_XML || pXml->hParent != hXml)
    {
        T5Heap_Unlock (pPrivate->pHeap, hChild);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return 0L;
    }
    /* get first child */
    hNext = pXml->hSibling;
    T5Heap_Unlock (pPrivate->pHeap, hChild);
    pStatus->dwErrorXML = 0L;
    /* search for wished name */
    if (hNext && szName != NULL && *szName != '\0')
        hNext = _T5XML_SearchSibling (pPrivate->pHeap, hNext, szName);
    return hNext;
}

T5_BOOL T5XML_SetTagAttrib (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR szAttrib,
                            T5_PTCHAR pBuffer)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml;
    T5_DWORD hAttrib, hNext;
    T5_BOOL bSet, bFail;
    T5PTR_XMLATTR pAttrib;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return FALSE;
    }
    if (pXml->dwKind != T5HP_XML)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return FALSE;
    }
    /* add first attribute */
    if (pXml->hAttrib == 0L)
    {
        pXml->hAttrib = _T5XML_CreateAttrib (pPrivate->pHeap, szAttrib, pBuffer);
        bFail = FALSE;
    }
    /* loop in attribute list */
    else
    {
        hAttrib = pXml->hAttrib;
        bSet = FALSE;
        bFail = FALSE;
        while (!bSet && !bFail && hAttrib != 0L)
        {
            hNext = 0L;
            pAttrib = (T5PTR_XMLATTR)T5Heap_Lock (pPrivate->pHeap, hAttrib);
            if (_T5XML_CheckString (pPrivate->pHeap, pAttrib->hName, szAttrib))
            {
                pAttrib->hText = _T5XML_ReallocString (pPrivate->pHeap, pAttrib->hText, pBuffer);
                if (pAttrib->hText == 0L)
                    bFail = TRUE;
                bSet = TRUE;
            }
            else if (pAttrib->hNext != 0L)
                hNext = pAttrib->hNext;
            else
            {
                pAttrib->hNext = _T5XML_CreateAttrib (pPrivate->pHeap, szAttrib, pBuffer);
                if (pAttrib->hNext == 0L)
                    bFail = TRUE;
                bSet = TRUE;
            }
            T5Heap_Unlock (pPrivate->pHeap, hAttrib);
            hAttrib = hNext;
        }

    }
    /* unlock record */
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    if (bFail)
        pStatus->dwErrorXML = XMLERR_OVERFLOW;
    else
        pStatus->dwErrorXML = 0L;
    return !bFail;
}

T5_BOOL T5XML_GetTagAttrib (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR szAttrib,
                            T5_PTCHAR pBuffer, T5_DWORD dwBufSize)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml;
    T5_DWORD hAttrib, hNext, dwLen;
    T5_BOOL bSet;
    T5PTR_XMLATTR pAttrib;
    T5_PTCHAR pText;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return FALSE;
    }
    if (pXml->dwKind != T5HP_XML)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return FALSE;
    }
    /* add first attribute */
    if (pXml->hAttrib == 0L)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        *pBuffer = '\0';
        return FALSE;
    }
    /* loop in attribute list */
    hAttrib = pXml->hAttrib;
    bSet = FALSE;
    while (!bSet && hAttrib != 0L)
    {
        hNext = 0L;
        pAttrib = (T5PTR_XMLATTR)T5Heap_Lock (pPrivate->pHeap, hAttrib);
        if (_T5XML_CheckString (pPrivate->pHeap, pAttrib->hName, szAttrib))
        {
            if (pAttrib->hText == 0L)
                *pBuffer = '\0';
            else
            {
                pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pAttrib->hText);
                pText += sizeof (T5STR_XMLSTR);
                dwLen = T5_STRLEN (pText);
                if (dwLen > (dwBufSize - 1L))
                    dwLen = dwBufSize - 1L;
                T5_MEMCPY (pBuffer, pText, dwLen);
                pBuffer[dwLen] = '\0';
                T5Heap_Unlock (pPrivate->pHeap, pAttrib->hText);
            }
            bSet = TRUE;
        }
        else
            hNext = pAttrib->hNext;
        T5Heap_Unlock (pPrivate->pHeap, hAttrib);
        hAttrib = hNext;
    }
    /* unlock record */
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    pStatus->dwErrorXML = 0L;
    if (!bSet)
        *pBuffer = '\0';
    return bSet;
}

T5_DWORD T5XML_GetParent (T5PTR_DB pDB, T5_DWORD hXml)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XMLTAG pXml;
    T5_DWORD hParent;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pPrivate->pHeap, hXml);
    if (pXml == NULL)
    {
        pStatus->dwErrorXS = XMLERR_BADHANDLE;
        return 0L;
    }
    if (pXml->dwKind != T5HP_XML)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXml);
        pStatus->dwErrorXML = XMLERR_BADHANDLE;
        return 0L;
    }
    /* get first child */
    hParent = pXml->hParent;
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    pStatus->dwErrorXML = 0L;
    return hParent;
}

/****************************************************************************/

#endif /*T5DEF_XML*/

/* eof **********************************************************************/
