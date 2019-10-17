/*****************************************************************************
T5rc.c :     resource managers - core implementation

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_RC

/****************************************************************************/

#ifdef T5DEF_BIGENDIAN
#define _T5_ENDIAN_ 1
#else /*T5DEF_BIGENDIAN*/
#define _T5_ENDIAN_ 0
#endif /*T5DEF_BIGENDIAN*/

/****************************************************************************/
/* static services */

static T5_PTCHAR _T5Rc_GetPath (T5_BYTE bKind);
static T5_DWORD _T55c_CRC32 (T5_DWORD dwCrc, T5_PTBYTE pCode, T5_DWORD dwSize);

/****************************************************************************/

T5_PTR T5Rc_Init_Buffer (T5_PTR pBuffer, T5_DWORD dwSize, T5_DWORD dwFlags)
{
    return T5Heap_CreateHeapBuffer (pBuffer, dwSize, dwFlags);
}

T5_PTR T5Rc_Init_Malloc (T5_DWORD dwGrowBy, T5_DWORD dwFlags)
{
    return T5Heap_CreateHeapMalloc (dwGrowBy, dwFlags);
}

void T5Rc_Release (T5_PTR pRCMan)
{
    T5Heap_DeleteHeap (pRCMan);
}

void T5Rc_Compact (T5_PTR pRCMan)
{
    T5Heap_Compact (pRCMan, 16L);
}

void T5Rc_FreeAllRC (T5_PTR pRCMan)
{
    T5Heap_FreeAll (pRCMan);
}

static T5_PTCHAR _T5Rc_GetPath (T5_BYTE bKind)
{
    switch (bKind)
    {
    case T5RC_SCRN : return (T5_PTCHAR)T5VFS_NAME_RSC;
    case T5RC_STRG : return (T5_PTCHAR)T5VFS_NAME_RST;
    case T5RC_BMPS : return (T5_PTCHAR)T5VFS_NAME_RBM;
    case T5RC_FONT : return (T5_PTCHAR)T5VFS_NAME_RFN;
    default : break;
    }
    return (T5_PTCHAR)"?";
}

static T5_DWORD _T55c_CRC32 (T5_DWORD dwCrc, T5_PTBYTE pCode, T5_DWORD dwSize)
{
    T5_DWORD b, i, mem1;
 
    for (b=0; b<dwSize; b++)
    {
        dwCrc ^= ((T5_DWORD)(*pCode) & 0xffL);
        for (i=1; i<= 8; i++)
        {
            mem1 = dwCrc;
            dwCrc /= 2;
            if (mem1 & 1) dwCrc ^= 0xa000a001;
        }
        pCode++; 
    }
    return dwCrc;
}


T5_BOOL T5Rc_CheckRCFile (T5_BYTE bKind)
{
    T5_BOOL bRet;
    T5_DWORD f;
    T5STR_RCFHEAD head;
    T5_DWORD dwFileSize, dwPakSize;
    T5_BYTE packet[64];
    T5_DWORD dwCrc;

    f = T5Vfs_OpenReadRand (_T5Rc_GetPath (bKind), &dwFileSize);
    if (f == 0L)
        bRet = FALSE;
    else
    {
        if (T5Vfs_Read (f, 0L, sizeof (head), &head) != sizeof (head)
            || head.bBigEndian != _T5_ENDIAN_ || head.bKind != bKind
            || (head.dwSize + sizeof (head)) != dwFileSize)
        {
            bRet = FALSE;
        }
        else
        {
            bRet = TRUE;
            dwFileSize = head.dwSize;
            dwCrc = 0xffffffffL;
            while (bRet && dwFileSize)
            {
                if (dwFileSize > 64L)
                {
                    dwPakSize = 64L;
                    dwFileSize -= 64L;
                }
                else
                {
                    dwPakSize = dwFileSize;
                    dwFileSize = 0L;
                }
                if (T5Vfs_Read (f, T5VFS_POS_CURRENT, dwPakSize, packet)
                        != dwPakSize)
                    bRet = FALSE;
                else
                    dwCrc = _T55c_CRC32 (dwCrc, packet, dwPakSize);
            }
            if (bRet && dwCrc != head.dwCRC)
                bRet = FALSE;
        }
        T5Vfs_Close (f);
    }
    return bRet;
}

T5_PTR T5Rc_LockRC (T5_PTR pRCMan, T5_DWORD hRC, T5_PTDWORD pdwSize)
{
    T5_PTBYTE pRec;

    pRec = (T5_PTBYTE)T5Heap_Lock (pRCMan, hRC);
    if (pRec != NULL)
    {
        if (pRec != NULL && pdwSize != NULL)
            *pdwSize = ((T5PTR_RCITEM)pRec)->dwSize;
        pRec += sizeof (T5STR_RCITEM);
    }
    return pRec;
}

void T5Rc_UnlockRC (T5_PTR pRCMan, T5_DWORD hRC)
{
    T5Heap_Unlock (pRCMan, hRC);
}

void T5Rc_FreeRC (T5_PTR pRCMan, T5_DWORD hRC)
{
    T5Heap_Free (pRCMan, hRC);
}

T5_DWORD T5Rc_LoadRCDirectory (T5_PTR pRCMan, T5_BYTE bKind,
                               T5_PTDWORD pdwCount)
{
    T5_DWORD f, hRC;
    T5STR_RCFHEAD head;
    T5_PTR *pDir;
    T5_DWORD dwRead;

    *pdwCount = 0L;
    f = T5Vfs_OpenReadRand (_T5Rc_GetPath (bKind), NULL);
    if (f == 0L)
        return 0L;

    if (!T5Vfs_Read (f, 0L, sizeof (head), &head) == sizeof (head))
    {
        T5Vfs_Close (f);
        return 0L;
    }
    if (head.dwNb == 0L)
    {
        T5Vfs_Close (f);
        return 0L;
    }
    hRC = T5Heap_Alloc (pRCMan, head.dwNb * sizeof (T5STR_RCENTRY));
    if (hRC == 0L)
    {
        T5Vfs_Close (f);
        return 0L;
    }
    pDir = T5Heap_Lock (pRCMan, hRC);
    if (pDir == NULL)
    {
        T5Heap_Free (pRCMan, hRC);
        T5Vfs_Close (f);
        return 0L;
    }
    dwRead = T5Vfs_Read (f, T5VFS_POS_CURRENT,
                         head.dwNb * sizeof (T5STR_RCENTRY), pDir);
    if (dwRead != (head.dwNb * sizeof (T5STR_RCENTRY)))
    {
        T5Heap_Unlock (pRCMan, hRC);
        T5Heap_Free (pRCMan, hRC);
        T5Vfs_Close (f);
        return 0L;
    }

    T5Vfs_Close (f);
    T5Heap_Unlock (pRCMan, hRC);
    *pdwCount = head.dwNb;
    return hRC;
}

T5_DWORD T5Rc_LoadRCByIndex (T5_PTR pRCMan, T5_BYTE bKind, T5_DWORD dwIndex)
{
    T5_DWORD f, hRC;
    T5STR_RCFHEAD head;
    T5STR_RCENTRY entry;
    T5_PTBYTE pRec;
    T5PTR_RCITEM pItem;

    hRC = 0L;
    f = T5Vfs_OpenReadRand (_T5Rc_GetPath (bKind), NULL);
    if (f == 0L)
        return 0L;
    if (T5Vfs_Read (f, 0L, sizeof (head), &head) == sizeof (head))
    {
        if (dwIndex >= 1L && dwIndex <= head.dwNb)
        {
            if (T5Vfs_Read (f,
                sizeof (T5STR_RCFHEAD) + ((dwIndex-1) * sizeof (T5STR_RCENTRY)),
                sizeof (entry),
                &entry) == sizeof (entry))
            {
                hRC = T5Heap_Alloc (pRCMan, entry.dwSize + sizeof (T5STR_RCITEM));
                if (hRC != 0L)
                {
                    pRec = (T5_PTBYTE)T5Heap_Lock (pRCMan, hRC);
                    if (pRec != NULL)
                    {
                        pItem = (T5PTR_RCITEM)pRec;
                        pItem->dwSize = entry.dwSize;
                        pItem->pData = pRec + sizeof (T5STR_RCITEM);
                        T5Vfs_Read (f,
                            sizeof (T5STR_RCFHEAD)
                                + head.dwNb * sizeof (T5STR_RCENTRY)
                                + entry.dwOffset,
                            entry.dwSize,
                            pRec + sizeof (T5STR_RCITEM)
                            );
                        T5Heap_Unlock (pRCMan, hRC);
                    }
                }
            }
        }
    }
    T5Vfs_Close (f);
    return hRC;
}

/****************************************************************************/
/* screen scripts parsing */

T5_BYTE T5Rcs_GetObjType (T5_PTBYTE pStream)
{
    if (*pStream != T5RC_SCA_OBTYPE)
        return T5RC_SCO_NULL;

    return (T5_BYTE)T5RCS_GetUnsignedProp (pStream);
}

T5_BYTE T5Rcs_GetProp (T5_PTBYTE pStream, T5_PTBYTE pbFormat)
{
    *pbFormat = pStream[1];
    return *pStream;
}

T5_PTBYTE T5Rcs_NextToken (T5_PTBYTE pStream)
{
    switch (pStream[1])
    {
    case T5RC_SCF_INT1 :
    case T5RC_SCF_UINT1 :
        pStream += 3;
        break;
    case T5RC_SCF_INT2 :
    case T5RC_SCF_UINT2 :
    case T5RC_SCF_RCID :
        pStream += 4;
        break;
    case T5RC_SCF_INT4 :
    case T5RC_SCF_UINT4 :
    case T5RC_SCF_REAL :
    case T5RC_SCF_PAIR :
        pStream += 6;
        break;
    case T5RC_SCF_INT8 :
    case T5RC_SCF_UINT8 :
    case T5RC_SCF_LREAL :
        pStream += 10;
        break;
    case T5RC_SCF_VAR :
        pStream += 5;
        break;
    case T5RC_SCF_CTVAR :
        pStream += 7;
        break;
    case T5RC_SCF_ARRAY :
        pStream += 7;
        break;
    case T5RC_SCF_CTARRAY :
        pStream += 9;
        break;
    case T5RC_SCF_STRING :
        pStream += 2;
        while (*pStream)
            pStream++;
        pStream++;
        break;
    case T5RC_SCF_NONE :
        pStream += 2;
        break;
    }
    return pStream;
}

T5_LONG T5RCS_GetSignedProp (T5_PTBYTE pStream)
{
    T5_SHORT s;
    T5_LONG l;

    switch (pStream[1])
    {
    case T5RC_SCF_INT1 :
        l = (T5_LONG)(*(T5_PTCHAR)(pStream+2));
        break;
    case T5RC_SCF_INT2 :
        T5_MEMCPY (&s, pStream + 2, sizeof (T5_SHORT));
        l = (T5_LONG)s;
        break;
    case T5RC_SCF_INT4 :
        T5_MEMCPY (&l, pStream + 2, sizeof (T5_LONG));
        break;
    default :
        l = 0L;
        break;
    }
    return l;
}

T5_DWORD T5RCS_GetUnsignedProp (T5_PTBYTE pStream)
{
    T5_WORD w;
    T5_DWORD dw;

    switch (pStream[1])
    {
    case T5RC_SCF_UINT1 :
        dw = (T5_DWORD)(pStream[2]);
        break;
    case T5RC_SCF_UINT2 :
    case T5RC_SCF_RCID :
        T5_MEMCPY (&w, pStream + 2, sizeof (T5_WORD));
        dw = (T5_LONG)w;
        break;
    case T5RC_SCF_UINT4 :
        T5_MEMCPY (&dw, pStream + 2, sizeof (T5_DWORD));
        break;
    default :
        dw = 0L;
        break;
    }
    return dw;
}

#ifdef T5DEF_REALSUPPORTED
T5_REAL T5RCS_GetRealProp (T5_PTBYTE pStream)
{
    T5_REAL r;

    if (pStream[1] != T5RC_SCF_REAL)
        return 0;
    T5_MEMCPY (&r, pStream + 2, sizeof (T5_REAL));
    return r;
}
#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_LREALSUPPORTED
T5_LREAL T5RCS_GetLrealProp (T5_PTBYTE pStream)
{
    T5_LREAL lr;

    if (pStream[1] != T5RC_SCF_LREAL)
        return 0;
    T5_MEMCPY (&lr, pStream + 2, sizeof (T5_LREAL));
    return lr;
}
#endif /*T5DEF_LREALSUPPORTED*/

#ifdef T5DEF_LINTSUPPORTED
T5_LINT T5RCS_GetLintProp (T5_PTBYTE pStream)
{
    T5_LINT li;

    if (pStream[1] != T5RC_SCF_INT8 && pStream[1] != T5RC_SCF_UINT8)
        return 0;
    T5_MEMCPY (&li, pStream + 2, sizeof (T5_LINT));
    return li;
}
#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_STRINGSUPPORTED
T5_PTCHAR T5RCS_GetStringProp (T5_PTBYTE pStream)
{
    if (pStream[1] != T5RC_SCF_STRING)
        return NULL;
    return (T5_PTCHAR)(pStream+2);
}
#endif /*T5DEF_STRINGSUPPORTED*/

T5_BOOL T5RCS_GetPairProp (T5_PTBYTE pStream, T5_PTWORD pwX, T5_PTWORD pwY)
{
    if (pStream[1] != T5RC_SCF_PAIR)
    {
        *pwX = *pwY = 0;
        return FALSE;
    }
    T5_MEMCPY (pwX, pStream + 2, sizeof (T5_WORD));
    T5_MEMCPY (pwY, pStream + 4, sizeof (T5_WORD));
    return TRUE;
}

T5_PTR T5RCS_GetVarProp (T5_PTBYTE pStream, T5PTR_DB pDB, T5_PTBYTE pbType)
{
    T5_PTBYTE pVar;
    T5_DWORD dwCTOffset;
    T5_WORD wIndex;

    switch (pStream[1])
    {
    case T5RC_SCF_VAR :
        *pbType = pStream[2];
        T5_MEMCPY (&wIndex, pStream + 3, sizeof (T5_WORD));
        pVar = (T5_PTBYTE)T5Tools_GetAnyParam (pDB, *pbType, wIndex,
                                               NULL, NULL);
        break;
    case T5RC_SCF_CTVAR :
        *pbType = pStream[2];
        T5_MEMCPY (&dwCTOffset, pStream + 3, sizeof (T5_DWORD));
        pVar = (T5_PTBYTE)pDB[T5TB_CTSEG].pData;
        pVar += dwCTOffset;
        break;
    default :
        *pbType = 0;
        pVar = NULL;
    }
    return (T5_PTR)pVar;
}

T5_PTR T5RCS_GetArrayProp (T5_PTBYTE pStream, T5PTR_DB pDB,
                           T5_PTBYTE pbType, T5_PTWORD pwDim)
{
    T5_PTBYTE pVar;
    T5_DWORD dwCTOffset;
    T5_WORD wIndex;

    switch (pStream[1])
    {
    case T5RC_SCF_ARRAY :
        *pbType = pStream[2];
        T5_MEMCPY (pwDim, pStream + 3, sizeof (T5_WORD));
        T5_MEMCPY (&wIndex, pStream + 5, sizeof (T5_WORD));
        pVar = (T5_PTBYTE)T5Tools_GetAnyParam (pDB, *pbType, wIndex,
                                               NULL, NULL);
        break;
    case T5RC_SCF_CTARRAY :
        *pbType = pStream[2];
        T5_MEMCPY (pwDim, pStream + 3, sizeof (T5_WORD));
        T5_MEMCPY (&dwCTOffset, pStream + 5, sizeof (T5_DWORD));
        pVar = (T5_PTBYTE)pDB[T5TB_CTSEG].pData;
        pVar += dwCTOffset;
        break;
    default :
        *pbType = 0;
        pVar = NULL;
    }
    return (T5_PTR)pVar;
}

/****************************************************************************/

#endif /*T5DEF_RC*/

/* eof **********************************************************************/
