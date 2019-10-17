/*****************************************************************************
T5xmlwr.c :  XML writer

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_XML

/****************************************************************************/

#define T5XML_FHEADER   "<?xml version=\"1.0\" standalone=\"yes\"?>"

typedef struct
{
    T5_LONG f;
    T5_PTCHAR sz;
    T5_DWORD nb;
}
T5STR_XOF, *T5PTR_XOF;

/****************************************************************************/

static T5_BOOL _T5XML_Append (T5PTR_XOF px, T5_PTCHAR sz, T5_BOOL bConvert)
{
    T5_DWORD dwLen;
    T5_BOOL bRet;
    T5_PTCHAR szConvert;

    bRet = TRUE;
    dwLen = T5_STRLEN(sz);
    if (!bConvert)
    {
        if (px->f)
            bRet &= T5File_Write (px->f, sz, dwLen);
        if (px->sz)
        {
            T5_MEMCPY (px->sz, sz, dwLen);
            px->sz += dwLen;
        }
        px->nb += dwLen;
    }
    else
    {
        while (*sz)
        {
            switch (*sz)
            {
            case '<' : dwLen = 4; szConvert = (T5_PTCHAR)"&lt;"; break;
            case '>' : dwLen = 4; szConvert = (T5_PTCHAR)"&gt;"; break;
            case '&' : dwLen = 5; szConvert = (T5_PTCHAR)"&amp;"; break;
            case '\'': dwLen = 6; szConvert = (T5_PTCHAR)"&apos;"; break;
            case '\"': dwLen = 6; szConvert = (T5_PTCHAR)"&quot;"; break;
            default  : dwLen = 1; szConvert = sz; break;
            }
            if (px->f)
                bRet &= T5File_Write (px->f, szConvert, dwLen);
            if (px->sz)
            {
                T5_MEMCPY (px->sz, szConvert, dwLen);
                px->sz += dwLen;
            }
            px->nb += dwLen;
            sz++;
        }
    }
    return bRet;
}

static T5_BOOL _T5XML_Write (T5_PTR pHeap, T5_DWORD hXml, T5PTR_XOF px)
{
    T5PTR_XMLTAG pXml, pChild;
    T5_BOOL bRet;
    T5_PTCHAR pText, pTagName;
    T5_DWORD hAttrib, hNext, hChild;
    T5PTR_XMLATTR pAttrib;

    bRet = TRUE;
    /* lock tag and tag name */
    pXml = (T5PTR_XMLTAG)T5Heap_Lock (pHeap, hXml);
    pTagName = (T5_PTCHAR)T5Heap_Lock (pHeap, pXml->hName);
    pTagName += sizeof (T5STR_XMLSTR);
    /* name */
    bRet &= _T5XML_Append (px, (T5_PTCHAR)"<", FALSE);
    bRet &= _T5XML_Append (px, pTagName, TRUE);
    /* attributes */
    hAttrib = pXml->hAttrib;
    while (hAttrib)
    {
        pAttrib = (T5PTR_XMLATTR)T5Heap_Lock (pHeap, hAttrib);
        hNext = pAttrib->hNext;
        if (pAttrib->hName)
        {
            bRet &= _T5XML_Append (px, (T5_PTCHAR)" ", FALSE);
            pText = (T5_PTCHAR)T5Heap_Lock (pHeap, pAttrib->hName);
            pText += sizeof (T5STR_XMLSTR);
            bRet &= _T5XML_Append (px, pText, TRUE);
            T5Heap_Unlock (pHeap, pAttrib->hName);
            bRet &= _T5XML_Append (px, (T5_PTCHAR)"=\"", FALSE);
            if (pAttrib->hText)
            {
                pText = (T5_PTCHAR)T5Heap_Lock (pHeap, pAttrib->hText);
                pText += sizeof (T5STR_XMLSTR);
                bRet &= _T5XML_Append (px, pText, TRUE);
                T5Heap_Unlock (pHeap, pAttrib->hText);
            }
            bRet &= _T5XML_Append (px, (T5_PTCHAR)"\"", FALSE);
        }
        T5Heap_Unlock (pHeap, hAttrib);
        hAttrib = hNext;
    }
    /* children */
    if (pXml->hChild)
    {
        bRet &= _T5XML_Append (px, (T5_PTCHAR)">", FALSE);
        hChild = pXml->hChild;
        while (hChild)
        {
            pChild = (T5PTR_XMLTAG)T5Heap_Lock (pHeap, hChild);
            hNext = pChild->hSibling;
            T5Heap_Unlock (pHeap, hChild);
            bRet &= _T5XML_Write (pHeap, hChild, px);
            hChild = hNext;
        }
        bRet &= _T5XML_Append (px, (T5_PTCHAR)"</", FALSE);
        bRet &= _T5XML_Append (px, pTagName, TRUE);
        bRet &= _T5XML_Append (px, (T5_PTCHAR)">", FALSE);
    }
    /* or contents */
    else if (pXml->hCont)
    {
        bRet &= _T5XML_Append (px, (T5_PTCHAR)">", FALSE);
        pText = (T5_PTCHAR)T5Heap_Lock (pHeap, pXml->hCont);
        pText += sizeof (T5STR_XMLSTR);
        bRet &= _T5XML_Append (px, pText, TRUE);
        T5Heap_Unlock (pHeap, pXml->hCont);
        bRet &= _T5XML_Append (px, (T5_PTCHAR)"</", FALSE);
        bRet &= _T5XML_Append (px, pTagName, TRUE);
        bRet &= _T5XML_Append (px, (T5_PTCHAR)">", FALSE);
    }
    /* or empty tag */
    else
        bRet &= _T5XML_Append (px, (T5_PTCHAR)"/>", FALSE);
    /* unlock tag and tag name */
    T5Heap_Unlock (pHeap, pXml->hName);
    T5Heap_Unlock (pHeap, hXml);
    return bRet;
}

/****************************************************************************/

T5_BOOL T5XML_WriteToFile (T5PTR_DB pDB, T5_DWORD hXml, T5_PTCHAR szFile)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5STR_XOF xof;
    T5_BOOL bRet;
    T5PTR_XMLTAG pXml;

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
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    /* parse file */
    T5_MEMSET (&xof, 0, sizeof (xof));
    xof.f = T5File_Open (szFile, TRUE);
    if (xof.f == 0L)
        bRet = FALSE;
    else
    {
        bRet = _T5XML_Append (&xof, T5XML_FHEADER, FALSE);
        if (bRet)
            bRet = _T5XML_Write (pPrivate->pHeap, hXml, &xof);
        T5File_Close (xof.f);
    }
    if (bRet)
        pStatus->dwErrorXML = 0L;
    else
        pStatus->dwErrorXML = XMLERR_WRITEFILE;
    return bRet;
}

T5_BOOL T5XML_WriteToXS (T5PTR_DB pDB, T5_DWORD hXml, T5_DWORD hXS)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5STR_XOF xof;
    T5_BOOL bRet;
    T5PTR_XMLTAG pXml;
    T5_DWORD hText;

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
    T5Heap_Unlock (pPrivate->pHeap, hXml);
    /* count size */
    bRet = TRUE;
    T5_MEMSET (&xof, 0, sizeof (xof));
    bRet = _T5XML_Append (&xof, T5XML_FHEADER, FALSE);
    if (bRet)
        bRet = _T5XML_Write (pPrivate->pHeap, hXml, &xof);
    if (!bRet)
    {
        pStatus->dwErrorXML = 100;
        return FALSE;
    }
    /* check and allocate XS buffer */
    xof.nb += 1L; /* null terminator */
    if (!T5XS_SetAllocSize (pDB, hXS, xof.nb))
    {
        pStatus->dwErrorXML = XMLERR_OVERFLOW;
        return FALSE;
    }
    /* get XS text handle */
    hText = T5XS_GetTextHandle (pDB, hXS);
    if (hText == 0L)
    {
        pStatus->dwErrorXS = XMLERR_BADTXB;
        return FALSE;
    }
    /* copy */
    xof.sz = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, hText);
    xof.nb = 0L;
    bRet = _T5XML_Append (&xof, T5XML_FHEADER, FALSE);
    if (bRet)
        bRet = _T5XML_Write (pPrivate->pHeap, hXml, &xof);
    /* add terminating \0 */
    xof.sz = '\0';
    T5Heap_Unlock (pPrivate->pHeap, hText);
    return bRet;
}

/****************************************************************************/

#endif /*T5DEF_XML*/

/* eof **********************************************************************/
