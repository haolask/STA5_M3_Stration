/*****************************************************************************
T5xmlprs.c : XML parser

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_XML

/****************************************************************************/

#define _ISBLANK(c) ((c)==' '||(c)=='\t'||(c)=='\r'||(c)=='\n')
#define _ISSEP(c)   ((c)=='<'||(c)=='>'||(c)=='/'||(c)=='='||(c)=='\''||(c)=='\"')

#define T5XML_FHEADER   "<?xml version=\"1.0\" standalone=\"yes\"?>"

typedef struct
{
    T5_LONG f;
    T5_PTCHAR sz;
    T5_CHAR next;
}
T5STR_XIF, *T5PTR_XIF;

static T5_CHAR szToken[512];  /* for names and values */
static T5_CHAR szAttrib[512]; /* for attribute names */

/****************************************************************************/

static T5_BOOL _T5XML_GetChar (T5PTR_XIF px)
{
    T5_BOOL bRet;

    bRet = FALSE;
    if (px->f)
    {
        bRet = T5File_Read (px->f, &(px->next), 1);
        if (!bRet)
            px->next = '\0';
    }
    else if (px->sz)
    {
        bRet = (*(px->sz) != '\0');
        if (bRet)
        {
            px->next = *(px->sz);
            px->sz += 1;
        }
        else
            px->next = '\0';
    }
    return bRet;
}

static void _T5XML_SkipUpTo (T5PTR_XIF px, T5_CHAR chr)
{
    while (px->next != chr && px->next != '\0')
        _T5XML_GetChar (px);
}

static void _T5XML_SkipBlanks (T5PTR_XIF px)
{
    while (_ISBLANK(px->next))
        _T5XML_GetChar (px);
}

static T5_CHAR _T5XML_GetAmpChar (T5PTR_XIF px)
{
    T5_CHAR szAmp[10];
    T5_WORD wLen;

    if (px->next != '&')
        return 0;
    _T5XML_GetChar (px);
    wLen = 0;
    while (px->next != ';' && px->next != '\0' && wLen < 9)
    {
        szAmp[wLen++] = px->next;
        _T5XML_GetChar (px);
    }
    szAmp[wLen] = '\0';
    if (px->next != ';')
        return 0;
    _T5XML_GetChar (px);
    if (T5_STRCMP (szAmp, "lt") == 0)
        return '<';
    if (T5_STRCMP (szAmp, "gt") == 0)
        return '>';
    if (T5_STRCMP (szAmp, "amp") == 0)
        return '&';
    if (T5_STRCMP (szAmp, "apos") == 0)
        return '\'';
    if (T5_STRCMP (szAmp, "quot") == 0)
        return '\"';
    return 0;
}

static T5_BOOL _T5XML_GetName (T5PTR_XIF px)
{
    T5_WORD wLen = 0;
    T5_CHAR cAmp;

    while (!_ISBLANK(px->next) && !_ISSEP(px->next))
    {
        if (wLen >= (sizeof (szToken)-1) || px->next == '\0')
        {
            *szToken = '\0';
            return FALSE;
        }
        if (px->next == '&')
        {
            cAmp = _T5XML_GetAmpChar (px);
            if (cAmp == 0)
                return FALSE;
            szToken[wLen++] = cAmp;
        }
        else
        {
            szToken[wLen++] = px->next;
            _T5XML_GetChar (px);
        }
    }
    szToken[wLen] = '\0';
    return (*szToken != '\0');
}

static T5_BOOL _T5XML_GetValue (T5PTR_XIF px)
{
    T5_WORD wLen = 0;
    T5_CHAR cAmp;

    while (px->next != '\"' && px->next !='\0')
    {
        if (wLen >= (sizeof (szToken)-1) || px->next == '\0')
        {
            *szToken = '\0';
            return FALSE;
        }
        if (px->next == '&')
        {
            cAmp = _T5XML_GetAmpChar (px);
            if (cAmp == 0)
                return FALSE;
            szToken[wLen++] = cAmp;
        }
        else
        {
            szToken[wLen++] = px->next;
            _T5XML_GetChar (px);
        }
    }
    szToken[wLen] = '\0';
    return TRUE;
}

static T5_DWORD _T5XML_Parse (T5PTR_DB pDB, T5PTR_XIF px, T5_DWORD hParent, T5_PTDWORD pdwError)
{
    T5_DWORD hTag, hChild, dwDummy;
    T5_BOOL bEmpty, bEnd;
    T5_WORD wLen;
    T5_CHAR chr;

    /* search for beginning of tag */
    do
    {
        _T5XML_SkipUpTo (px, '<');
        if (px->next == '<')
        {
            _T5XML_GetChar (px);
            if (px->next == '?' || px->next == '!')
                _T5XML_SkipUpTo (px, '>');
        }
    } while (px->next == '>');
    if (px->next == '\0')
        return 0L;
    /* get tag name and create the tag */
    _T5XML_SkipBlanks (px);
    if (!_T5XML_GetName (px))
    {
        *pdwError = XMLERR_XMLSYNTAX;
        return 0L;
    }
    if (hParent)
        hTag = T5XML_AddChild (pDB, hParent, szToken);
    else
        hTag = T5XML_Alloc (pDB, szToken);
    if (hTag == 0L)
    {
        *pdwError = XMLERR_OVERFLOW;
        return 0L;
    }
    /* list of attributes */
    _T5XML_SkipBlanks (px);
    while (px->next != '>' && px->next != '/' && px->next != '\0')
    {
        /* attribute name */
        if (!_T5XML_GetName (px))
        {
            *pdwError = XMLERR_XMLSYNTAX;
            T5XML_Free (pDB, hTag);
            return 0L;
        }
        T5_STRCPY (szAttrib, szToken);
        /* = */
        _T5XML_SkipBlanks (px);
        if (px->next != '=')
        {
            *pdwError = XMLERR_XMLSYNTAX;
            T5XML_Free (pDB, hTag);
            return 0L;
        }
        _T5XML_GetChar (px);
        _T5XML_SkipBlanks (px);
        /* search for value */
        _T5XML_SkipBlanks (px);
        if (px->next != '\"')
        {
            *pdwError = XMLERR_XMLSYNTAX;
            T5XML_Free (pDB, hTag);
            return 0L;
        }
        _T5XML_GetChar (px);
        /* value */
        if (!_T5XML_GetValue (px) || px->next != '\"')
        {
            *pdwError = XMLERR_XMLSYNTAX;
            T5XML_Free (pDB, hTag);
            return 0L;
        }
        _T5XML_GetChar (px);
        /* set attribute */
        if (!T5XML_SetTagAttrib (pDB, hTag, szAttrib, szToken))
        {
            *pdwError = XMLERR_OVERFLOW;
            T5XML_Free (pDB, hTag);
            return 0L;
        }
        _T5XML_SkipBlanks (px);
    }
    /* empty tag ? */
    bEmpty = (px->next == '/');
    if (bEmpty)
    {
        _T5XML_GetChar (px);
        if (px->next != '>')
        {
            *pdwError = XMLERR_XMLSYNTAX;
            T5XML_Free (pDB, hTag);
            return 0L;
        }
        _T5XML_GetChar (px);
        return hTag;
    }
    else
    {
        if (px->next != '>')
        {
            *pdwError = XMLERR_XMLSYNTAX;
            T5XML_Free (pDB, hTag);
            return 0L;
        }
        _T5XML_GetChar (px);
    }
    /* contents and children */
    bEnd = FALSE;
    wLen = 0;
    while (!bEnd)
    {
        if (px->next == '\0')
        {
            *pdwError = XMLERR_XMLSYNTAX;
            T5XML_Free (pDB, hTag);
            return 0L;
        }
        /* contents */
        else if (px->next != '<')
        {
            if (px->next == '&')
                chr = _T5XML_GetAmpChar (px);
            else
            {
                chr = px->next;
                _T5XML_GetChar (px);
            }
            if (T5_STRLEN(szToken) >= (sizeof (szToken) - 1))
            {
                szToken[wLen] = '\0';
                if (!T5XML_AppendContents (pDB, hTag, szToken))
                {
                    *pdwError = XMLERR_OVERFLOW;
                    T5XML_Free (pDB, hTag);
                    return 0L;
                }
                *szToken = '\0';
                wLen = 0;
            }
            szToken[wLen++] = chr;
        }
        /* child or end tag */
        else
        {
            if (wLen)
            {
                szToken[wLen] = '\0';
                if (!T5XML_AppendContents (pDB, hTag, szToken))
                {
                    *pdwError = XMLERR_OVERFLOW;
                    T5XML_Free (pDB, hTag);
                    return 0L;
                }
                *szToken = '\0';
                wLen = 0;
            }
            hChild = _T5XML_Parse (pDB, px, hTag, &dwDummy);
            if (hChild == 0L)
            {
                /* end tag: marker */
                if (px->next != '/')
                {
                    *pdwError = XMLERR_XMLSYNTAX;
                    T5XML_Free (pDB, hTag);
                    return 0L;
                }
                _T5XML_GetChar (px);
                /* end tag: get and check name */
                if (!_T5XML_GetName (px))
                {
                    *pdwError = XMLERR_XMLSYNTAX;
                    T5XML_Free (pDB, hTag);
                    return 0L;
                }
                if (!T5XML_GetTagName (pDB, hTag, szAttrib, sizeof (szAttrib))
                    || T5VMStr_ICmp ((T5_PTBYTE)szAttrib, (T5_PTBYTE)szToken))
                {
                    *pdwError = XMLERR_XMLSYNTAX;
                    T5XML_Free (pDB, hTag);
                    return 0L;
                }
                /* end tag character */
                _T5XML_SkipBlanks (px);
                if (px->next != '>')
                {
                    *pdwError = XMLERR_XMLSYNTAX;
                    T5XML_Free (pDB, hTag);
                    return 0L;
                }
                _T5XML_GetChar (px);
                bEnd = TRUE;
            }
        }
    }
    return hTag;
}

/****************************************************************************/

T5_DWORD T5XML_ParseFile (T5PTR_DB pDB, T5_PTCHAR szFile)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5STR_XIF xif;
    T5_DWORD hRoot, dwError;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    pStatus = T5GET_DBSTATUS(pDB);
    T5_MEMSET (&xif, 0, sizeof (xif));
    xif.f = T5File_Open (szFile, FALSE);
    if (xif.f == 0L)
    {
        pStatus->dwErrorXML = XMLERR_READFILE;
        return 0L;
    }

    _T5XML_GetChar (&xif);
    dwError = 0L;
    hRoot = _T5XML_Parse (pDB, &xif, 0L, &dwError);
    T5File_Close (xif.f);

    if (hRoot == 0 && dwError == 0L)
        dwError = XMLERR_XMLSYNTAX;
    pStatus->dwErrorXML = dwError;
    T5Heap_Compact (pPrivate->pHeap, 16);
    return hRoot;
}

T5_DWORD T5XML_ParseXS (T5PTR_DB pDB, T5_DWORD hXS)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5STR_XIF xif;
    T5_DWORD hText, hRoot, dwError;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    pStatus = T5GET_DBSTATUS(pDB);
    T5_MEMSET (&xif, 0, sizeof (xif));
    hText = T5XS_GetTextHandle (pDB, hXS);
    if (hText == 0L)
    {
        pStatus->dwErrorXML = XMLERR_BADTXB;
        return 0L;
    }
    xif.sz = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, hText);
    _T5XML_GetChar (&xif);
    dwError = 0L;
    hRoot = _T5XML_Parse (pDB, &xif, 0L, &dwError);
    T5Heap_Unlock (pPrivate->pHeap, hText);

    if (hRoot == 0 && dwError == 0L)
        dwError = XMLERR_XMLSYNTAX;
    pStatus->dwErrorXML = dwError;
    T5Heap_Compact (pPrivate->pHeap, 16);
    return hRoot;
}

/****************************************************************************/

#endif /*T5DEF_XML*/

/* eof **********************************************************************/
