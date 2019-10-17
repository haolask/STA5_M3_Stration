/*****************************************************************************
T5xs.c :     string buffers

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_TXB

/****************************************************************************/

T5_DWORD T5XS_Alloc (T5PTR_DB pDB, T5_DWORD dwSize, T5_PTCHAR szText)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5_DWORD hXS, hText, dwTextSize;
    T5_PTCHAR pText;
    T5PTR_XS pXS;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* fix and check size */
    if (szText != NULL)
    {
        dwTextSize = T5_STRLEN (szText) + 1L;
        if (dwTextSize > dwSize)
            dwSize = dwTextSize;
    }
    if (dwSize == 0L)
        return 0L;
    /* allocate buffers for TXB and text */
    hXS = T5Heap_Alloc (pPrivate->pHeap, sizeof (T5STR_XS));
    if (hXS == 0L)
    {
        pStatus->dwErrorXS = XSERR_OVERFLOW;
        return 0L;
    }
    hText = T5Heap_Alloc (pPrivate->pHeap, dwSize);
    if (hText == 0L)
    {
        T5Heap_Free (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_OVERFLOW;
        return 0L;
    }
    /* fill TXB header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    pXS->dwKind = T5HP_TXH;
    pXS->dwSize = dwSize;
    pXS->hText = hText;
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    /* fill text if specified */
    if (szText != NULL)
    {
        pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, hText);
        T5_STRCPY (pText, szText);
        T5Heap_Unlock (pPrivate->pHeap, hText);
    }
    pStatus->dwNbXS += 1L;
    pStatus->dwErrorXS = 0L;
    return hXS;
}

T5_BOOL T5XS_Free (T5PTR_DB pDB, T5_DWORD hXS)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    /* free text */
    T5Heap_Free (pPrivate->pHeap, pXS->hText);
    /* unlock and free header */
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    T5Heap_Free (pPrivate->pHeap, hXS);
    pStatus->dwErrorXS = 0;
    if (pStatus->dwNbXS > 0L)
        pStatus->dwNbXS -= 1L;
    T5Heap_Compact (pPrivate->pHeap, 4);
    return TRUE;
}

T5_DWORD T5XS_GetLength (T5PTR_DB pDB, T5_DWORD hXS)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_PTCHAR pText;
    T5_DWORD dwLen;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return 0L;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return 0L;
    }
    /* get text length */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
    dwLen = T5_STRLEN(pText);
    T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    pStatus->dwErrorXS = 0L;
    return dwLen;
}

T5_DWORD T5XS_Get (T5PTR_DB pDB, T5_DWORD hXS, T5_PTCHAR pDst, T5_DWORD dwBufSize)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_PTCHAR pText;
    T5_DWORD dwLen;

    if (dwBufSize < 1L)
        return 0L;
    dwBufSize--;
    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return 0L;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return 0L;
    }
    /* get and copy text */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
    dwLen = T5_STRLEN(pText);
    if (dwLen > dwBufSize)
        dwLen = dwBufSize;
    T5_MEMCPY (pDst, pText, dwLen);
    pDst[dwLen] = '\0';
    T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    pStatus->dwErrorXS = 0L;
    return dwLen;
}

T5_BOOL T5XS_SetAllocSize (T5PTR_DB pDB, T5_DWORD hXS, T5_DWORD dwWished)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_BOOL bRet;
    T5_DWORD hNewText;
    T5_PTCHAR pText, pNewText;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return 0L;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    /* check if realloc needed */
    if (dwWished < pXS->dwSize)
    {
        pStatus->dwErrorXS = 0L;
        bRet = TRUE;
    }
    else
    {
        /* allocate new text buffer */
        hNewText = T5Heap_Alloc (pPrivate->pHeap, dwWished);
        if (hNewText == 0L)
        {
            pStatus->dwErrorXS = XSERR_OVERFLOW;
            bRet = FALSE;
        }
        else
        {
            /* copy text */
            pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
            pNewText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, hNewText);
            T5_STRCPY (pNewText, pText);
            T5Heap_Unlock (pPrivate->pHeap, hNewText);
            T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
            /* free old buffer and remember new one */
            T5Heap_Free (pPrivate->pHeap, pXS->hText);
            pXS->hText = hNewText;
            pXS->dwSize = dwWished;
            pStatus->dwErrorXS = 0L;
            bRet = TRUE;
        }
    }
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    return bRet;
}

T5_BOOL T5XS_Set (T5PTR_DB pDB, T5_DWORD hXS, T5_PTR pSrc, T5_DWORD dwLen)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_PTCHAR pText;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* check header and fix buffer size */
    if (!T5XS_SetAllocSize (pDB, hXS, dwLen + 1L))
        return FALSE;
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    /* copy text */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
    T5_MEMCPY (pText, pSrc, dwLen);
    pText[dwLen] = '\0';
    T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
    /* unlock header */
    pXS->dwPos = 0;
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    pStatus->dwErrorXS = 0L;
    return TRUE;
}

T5_BOOL T5XS_Copy (T5PTR_DB pDB, T5_DWORD hDst, T5_DWORD hSrc)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_PTCHAR pText;
    T5_BOOL bRet;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hSrc);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hSrc);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    /* get text and copy */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
    bRet = T5XS_Set (pDB, hDst, pText, T5_STRLEN(pText));
    T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hSrc);
    pStatus->dwErrorXS = 0L;
    return bRet;
}

T5_BOOL T5XS_Rewind (T5PTR_DB pDB, T5_DWORD hXS)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    /* reset position */
    pXS->dwPos = 0L;
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    pStatus->dwErrorXS = 0L;
    return TRUE;
}

T5_BOOL T5XS_GetLine (T5PTR_DB pDB, T5_DWORD hXS, T5_PTCHAR pDst,
                      T5_DWORD dwBufSize)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_PTCHAR pText;
    T5_BOOL bRet, bEOL;
    T5_DWORD dwCount;

    if (dwBufSize < 1L)
        return FALSE;
    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    /* get line of text */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
    /* check for end of text */
    if (pXS->dwPos >= pXS->dwSize || pText[pXS->dwPos] == '\0')
    {
        *pDst = '\0';
        T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        return FALSE;
    }
    /* copy */
    bRet = TRUE;
    bEOL = FALSE;
    dwCount = 1;
    pStatus->dwErrorXS = 0L;
    while (bRet && !bEOL && pXS->dwPos<pXS->dwSize && pText[pXS->dwPos] != '\0')
    {
        if (dwCount >= (dwBufSize - 1))
        {
            pStatus->dwErrorXS = XSERR_TRUNCATE;
            bRet = FALSE;
        }
        else if (T5_MEMCMP (pText+pXS->dwPos, T5_FEOLSTRING,
                            T5_STRLEN(T5_FEOLSTRING)) == 0)
        {
            pXS->dwPos += (T5_DWORD)T5_STRLEN(T5_FEOLSTRING);
            bEOL = TRUE;
        }
        else
        {
            *pDst = pText[pXS->dwPos];
            pXS->dwPos += 1;
            pDst += 1;
            dwCount += 1;
        }
    }
    *pDst = '\0';
    T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    return TRUE;
}

T5_BOOL T5XS_Append (T5PTR_DB pDB, T5_DWORD hXS, T5_PTCHAR pSrc)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_DWORD dwLen;
    T5_PTCHAR pText;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    /* check and fix length */
    dwLen = T5_STRLEN(pSrc) + T5XS_GetLength (pDB, hXS) + 1;
    if (!T5XS_SetAllocSize (pDB, hXS, dwLen))
        return FALSE;
    /* concatenate */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    /* reset position */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
    T5_STRCAT (pText, pSrc);
    T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    pStatus->dwErrorXS = 0L;
    return TRUE;
}

T5_BOOL T5XS_AppendXS (T5PTR_DB pDB, T5_DWORD hDst, T5_DWORD hSrc)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_PTCHAR pText;
    T5_BOOL bRet;
    pStatus = T5GET_DBSTATUS(pDB);

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hSrc);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hSrc);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    /* get text length */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
    bRet = T5XS_Append (pDB, hDst, pText);
    T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hSrc);
    return bRet;
}

T5_BOOL T5XS_SetFile (T5PTR_DB pDB, T5_DWORD hXS, T5_PTCHAR szPath)
{
#ifdef T5DEF_FILESUPPORTED
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_PTCHAR pText;
    T5_BOOL bRet;
    T5_LONG f;
    T5_DWORD dwSize, dwPack;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    /* lock text */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
    /* read file */
    f = T5File_Open (szPath, FALSE);
    if (f == 0L)
    {
        *pText = '\0';
        pStatus->dwErrorXS = XSERR_READFILE;
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
        pStatus->dwErrorXS = 0;
        dwSize = pXS->dwSize - 1L;
        while (dwSize)
        {
            dwPack = (dwSize > 1024L) ? 1024L : dwSize;
            if (T5File_Read (f, pText, dwPack))
            {
                pText += dwPack;
                dwSize -= dwPack;
            }
            else
                dwSize = 0L;
        }
        T5File_Close (f);
        *pText = '\0';
    }
    /* unlock text */
    T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    pStatus->dwErrorXS = 0L;
    return bRet;
#else /*T5DEF_FILESUPPORTED*/
    return FALSE;
#endif /*T5DEF_FILESUPPORTED*/
}

T5_BOOL T5XS_SaveToFile (T5PTR_DB pDB, T5_DWORD hXS, T5_PTCHAR szPath)
{
#ifdef T5DEF_FILESUPPORTED
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_PTCHAR pText;
    T5_LONG f;
    T5_DWORD dwSize, dwPack;
    T5_BOOL bRet;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    /* lock text */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
    /* save to file */
    f = T5File_Open (szPath, TRUE);
    if (f == 0L)
    {
        bRet = FALSE;
        pStatus->dwErrorXS = XSERR_WRITEFILE;
    }
    else
    {
        bRet = TRUE;
        pStatus->dwErrorXS = 0;
        dwSize = T5_STRLEN (pText);
        while (bRet && dwSize > 0)
        {
            dwPack = (dwSize > 1024L) ? 1024L : dwSize;
            if (!T5File_Write (f, pText, dwPack))
            {
                pStatus->dwErrorXS = XSERR_WRITEFILE;
                bRet = FALSE;
            }
            else
            {
                dwSize -= dwPack;
                pText += dwPack;
            }
        }
        T5File_Close (f);
    }
    /* unlock text */
    T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    pStatus->dwErrorXS = 0L;
    return bRet;
#else /*T5DEF_FILESUPPORTED*/
    return FALSE;
#endif /*T5DEF_FILESUPPORTED*/
}

T5_BOOL T5XS_AnsiToUtf8 (T5PTR_DB pDB, T5_DWORD hXS)
{
#ifdef T5DEF_UNICODE
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_PTCHAR pText, pConv;
    T5_BOOL bRet;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    /* get text and convert */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
    pConv = T5Unicod_AllocUtf8Conversion (pText);
    T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    /* check conversion */
    if (pConv == NULL)
    {
        pStatus->dwErrorXS = XSERR_NOTSUP;
        return FALSE;
    }
    /* store conversion */
    bRet = T5XS_Set (pDB, hXS, pConv, T5_STRLEN(pConv));
    /* free conversion buffer */
    T5Unicod_FreeConversion (pConv);
    return bRet;
#else /*T5DEF_UNICODE*/
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    pStatus->dwErrorXS = XSERR_NOTSUP;
    return FALSE;
#endif /*T5DEF_UNICODE*/
}

T5_BOOL T5XS_Utf8ToAnsi (T5PTR_DB pDB, T5_DWORD hXS)
{
#ifdef T5DEF_UNICODE
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_DBSTATUS pStatus;
    T5PTR_XS pXS;
    T5_PTCHAR pText, pConv;
    T5_BOOL bRet;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return FALSE;
    T5Heap_Compact (pPrivate->pHeap, 4);
    pStatus = T5GET_DBSTATUS(pDB);
    /* lock and check header */
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
    {
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    if (pXS->dwKind != T5HP_TXH)
    {
        T5Heap_Unlock (pPrivate->pHeap, hXS);
        pStatus->dwErrorXS = XSERR_BADHANDLE;
        return FALSE;
    }
    /* get text and convert */
    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
    pConv = T5Unicod_AllocAnsiConversion (pText);
    T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
    /* unlock header */
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    /* check conversion */
    if (pConv == NULL)
    {
        pStatus->dwErrorXS = XSERR_NOTSUP;
        return FALSE;
    }
    /* store conversion */
    bRet = T5XS_Set (pDB, hXS, pConv, T5_STRLEN(pConv));
    /* free conversion buffer */
    T5Unicod_FreeConversion (pConv);
    return bRet;
#else /*T5DEF_UNICODE*/
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    pStatus->dwErrorXS = XSERR_NOTSUP;
    return FALSE;
#endif /*T5DEF_UNICODE*/
}

T5_DWORD T5XS_GetTextHandle (T5PTR_DB pDB, T5_DWORD hXS)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_XS pXS;
    T5_DWORD hText;

    pPrivate = T5GET_DBPRIVATE(pDB);
    if (pPrivate->pHeap == NULL)
        return 0L;
    pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, hXS);
    if (pXS == NULL)
        return 0L;
    hText = pXS->hText;
    T5Heap_Unlock (pPrivate->pHeap, hXS);
    return hText;
}

/****************************************************************************/

#endif /*T5DEF_TXB*/

/* eof **********************************************************************/
