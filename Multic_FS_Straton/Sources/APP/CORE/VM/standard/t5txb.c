/*****************************************************************************
T5txb.c :    text buffers management function blocks

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_TXB

/****************************************************************************/
/* useful macros */

#define _F_BEGIN \
    switch (wCommand) \
    { \
    case T5FBCMD_ACTIVATE :

#define _F_END \
        return 0L; \
    case T5FBCMD_ACCEPTCT : \
        return 1L; \
    default : \
        return 0L; \
    }

/****************************************************************************/
/* TxbManager */

#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_NBUFFERS (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))

T5_DWORD TXBMANAGER (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBPRIVATE pPrivate;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    switch (wCommand)
    {
    case T5FBCMD_ACTIVATE :
        pStatus = T5GET_DBSTATUS(pBase);
        pPrivate = T5GET_DBPRIVATE(pBase);
        _P_NBUFFERS = pStatus->dwNbXS;
        _P_BOK = (pPrivate->pHeap != NULL);
        return 0L;
    case T5FBCMD_ACCEPTCT :
        return 1L;
    default :
        return 0L;
    }
}

#undef _P_BOK
#undef _P_NBUFFERS

/****************************************************************************/
/* TxbLastError */

#define _P_ERRNO    (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))

T5_DWORD TXBLASTERROR (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        pStatus = T5GET_DBSTATUS(pBase);
        _P_ERRNO = pStatus->dwErrorXS;
    }
    _F_END
}

#undef _P_ERRNO


/****************************************************************************/
/* TxbAppend */

#define _P_ANYTYPE  pArgs[0]
#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_DATA     pArgs[2]
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))

#define _P_DATATEXT (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[2]))

T5_DWORD TXBAPPEND (T5_WORD wCommand, T5PTR_DB pBase,
                    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_CHAR sz[128];
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        switch (_P_ANYTYPE)
        {
        case T5C_STRING :
            _P_BOK = T5XS_Append (pBase, _P_HTXB, _P_DATATEXT+2);
            break;
        default :
            T5VMCnv_NumToSZ (pBase, _P_ANYTYPE, _P_DATA, sz);
            _P_BOK = T5XS_Append (pBase, _P_HTXB, sz);
            break;
        }
    }
    _F_END
}

#undef _P_ANYTYPE
#undef _P_HTXB
#undef _P_DATA
#undef _P_BOK
#undef _P_DATATEXT

/****************************************************************************/
/* TxbAppendLine */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_SZTEXT   (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

T5_DWORD TXBAPPENDLINE (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_Append (pBase, _P_HTXB, _P_SZTEXT + 2);
        if (_P_BOK)
            _P_BOK = T5XS_Append (pBase, _P_HTXB, T5_FEOLSTRING);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_SZTEXT
#undef _P_BOK

/****************************************************************************/
/* TxbAppendTxb */

#define _P_HTXBDST  (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

T5_DWORD TXBAPPENDTXB (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_AppendXS (pBase, _P_HTXBDST, _P_HTXB);
    }
    _F_END
}

#undef _P_HTXBDST
#undef _P_HTXB
#undef _P_BOK

/****************************************************************************/
/* TxbAppendEol */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))

T5_DWORD TXBAPPENDEOL (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_Append (pBase, _P_HTXB, T5_FEOLSTRING);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_BOK

/****************************************************************************/
/* TxbClear */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))

T5_DWORD TXBCLEAR (T5_WORD wCommand, T5PTR_DB pBase,
                   T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_Set (pBase, _P_HTXB, "", 0);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_BOK

/****************************************************************************/
/* TxbCopy */

#define _P_HTXBDST  (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

T5_DWORD TXBCOPY (T5_WORD wCommand, T5PTR_DB pBase,
                  T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_Copy (pBase, _P_HTXBDST, _P_HTXB);
    }
    _F_END
}

#undef _P_HTXBDST
#undef _P_HTXB
#undef _P_BOK

/****************************************************************************/
/* TxbFree */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))

T5_DWORD TXBFREE (T5_WORD wCommand, T5PTR_DB pBase,
                  T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_Free (pBase, _P_HTXB);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_BOK

/****************************************************************************/
/* TxbGetData */

#define _P_HTXB         (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_ARDATA       ((T5_PTCHAR)T5Tools_GetD8Array(pBase,pArgs[1]))
#define _P_ARDATA_SIZE  pArgs[2]
#define _P_BOK          (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))

T5_DWORD TXBGETDATA (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        T5XS_Get (pBase, _P_HTXB, _P_ARDATA, _P_ARDATA_SIZE);
        pStatus = T5GET_DBSTATUS(pBase);
        _P_BOK =  (pStatus->dwErrorXS == 0L);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_ARDATA
#undef _P_ARDATA_SIZE
#undef _P_BOK

/****************************************************************************/
/* TxbGetLength */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_LEN      (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))

T5_DWORD TXBGETLENGTH (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_LEN = T5XS_GetLength (pBase, _P_HTXB);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_LEN

/****************************************************************************/
/* TxbGetLine */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_SZTEXT   (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

T5_DWORD TXBGETLINE (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_GetLine (pBase, _P_HTXB, _P_SZTEXT+2,
                               ((T5_DWORD)(*_P_SZTEXT) & 0xff) + 1L);
        _P_SZTEXT[1] = (T5_BYTE)T5_STRLEN (_P_SZTEXT+2);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_SZTEXT
#undef _P_BOK

/****************************************************************************/
/* TxbGetString */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_SZTEXT   (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))

T5_DWORD TXBGETSTRING (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        T5XS_Get (pBase, _P_HTXB, _P_SZTEXT+2,
                    ((T5_DWORD)(*_P_SZTEXT) & 0xff) + 1L);
        _P_SZTEXT[1] = (T5_BYTE)T5_STRLEN (_P_SZTEXT+2);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_SZTEXT

/****************************************************************************/
/* TxbNew */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))

T5_DWORD TXBNEW (T5_WORD wCommand, T5PTR_DB pBase,
                 T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_HTXB = T5XS_Alloc (pBase, 32L, NULL);
    }
    _F_END
}

#undef _P_HTXB

/****************************************************************************/
/* TxbNewString */

#define _P_SZTEXT   (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[0]))
#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))

T5_DWORD TXBNEWSTRING (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_HTXB = T5XS_Alloc (pBase, 0L, _P_SZTEXT+2);
    }
    _F_END
}

#undef _P_SZTEXT
#undef _P_HTXB

/****************************************************************************/
/* TxbReadFile */

#define _P_SZPATH   (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[0]))
#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))

T5_DWORD TXBREADFILE (T5_WORD wCommand, T5PTR_DB pBase,
                      T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_DWORD dwSize;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        _P_HTXB = 0L;
#ifdef T5DEF_FILEMGT
#ifdef T5DEF_FILESUPPORTED
        dwSize = T5File_GetSize (_P_SZPATH + 2);
        if (dwSize == 0)
        {
            pStatus = T5GET_DBSTATUS(pBase);
            pStatus->dwErrorXS = XSERR_READFILE;
        }
        else
        {
            _P_HTXB = T5XS_Alloc (pBase, dwSize + 1, NULL);
            if (_P_HTXB)
                T5XS_SetFile (pBase, _P_HTXB, _P_SZPATH + 2);
        }
#endif /*T5DEF_FILESUPPORTED*/
#endif /*T5DEF_FILEMGT*/
    }
    _F_END
}

#undef _P_SZPATH
#undef _P_HTXB

/****************************************************************************/
/* TxbRewind */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))

T5_DWORD TXBREWIND (T5_WORD wCommand, T5PTR_DB pBase,
                    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_Rewind (pBase, _P_HTXB);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_BOK

/****************************************************************************/
/* TxbSetData */

#define _P_HTXB         (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_ARDATA       ((T5_PTCHAR)T5Tools_GetD8Array(pBase,pArgs[1]))
#define _P_ARDATA_SIZE  pArgs[2]
#define _P_BOK          (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))

T5_DWORD TXBSETDATA (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_Set (pBase, _P_HTXB, _P_ARDATA, _P_ARDATA_SIZE);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_ARDATA
#undef _P_ARDATA_SIZE
#undef _P_BOK

/****************************************************************************/
/* TxbSetString */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_SZTEXT   (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

T5_DWORD TXBSETSTRING (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_Set (pBase, _P_HTXB, _P_SZTEXT+2, T5_STRLEN(_P_SZTEXT+2));
    }
    _F_END
}

#undef _P_HTXB
#undef _P_SZTEXT
#undef _P_BOK

/****************************************************************************/
/* TxbWriteFile */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_SZPATH   (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

T5_DWORD TXBWRITEFILE (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = FALSE;
#ifdef T5DEF_FILEMGT
#ifdef T5DEF_FILESUPPORTED
        _P_BOK = T5XS_SaveToFile (pBase, _P_HTXB, _P_SZPATH+2);
#endif /*T5DEF_FILESUPPORTED*/
#endif /*T5DEF_FILEMGT*/
    }
    _F_END
}

#undef _P_HTXB
#undef _P_SZPATH
#undef _P_BOK

/****************************************************************************/
/* TxbAnsiToUtf8 */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))

T5_DWORD TXBANSITOUTF8 (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_AnsiToUtf8 (pBase, _P_HTXB);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_BOK

/****************************************************************************/
/* TxbUtf8ToAnsi */

#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))

T5_DWORD TXBUTF8TOANSI (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XS_Utf8ToAnsi (pBase, _P_HTXB);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_BOK

/****************************************************************************/
/* Text buffers over TCP */


#define _P_SOCK     (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_HTXB     (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_NBSENT   (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[2]))

T5_DWORD TXBSENDTCP (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
#ifdef T5DEF_ETHERNET
        T5PTR_DBSYSRSC pRsc;
        T5_BOOL bFail;
        T5PTR_DBPRIVATE pPrivate;
        T5PTR_XS pXS;
        T5_PTCHAR pText;

        pPrivate = T5GET_DBPRIVATE(pBase);
        if (pPrivate->pHeap != NULL
            && (pRsc = T5VMRsc_FindSocket (pBase, _P_SOCK)) != NULL
            && _P_SOCK != (T5_LONG)T5_INVSOCKET)
        {
            pXS = (T5PTR_XS)T5Heap_Lock (pPrivate->pHeap, _P_HTXB);
            if (pXS != NULL)
            {
                if (pXS->dwKind == T5HP_TXH)
                {
                    pText = (T5_PTCHAR)T5Heap_Lock (pPrivate->pHeap, pXS->hText);
                    if (pText)
                    {
                        _P_NBSENT = (T5_LONG)T5Socket_Send (
                            (T5_SOCKET)_P_SOCK,
                            (T5_WORD)T5_STRLEN(pText), pText, &bFail);
                        T5Heap_Unlock (pPrivate->pHeap, pXS->hText);
                    }
                }
                T5Heap_Unlock (pPrivate->pHeap, _P_HTXB);
            }
        }
#endif /*T5DEF_ETHERNET*/
    }
    _F_END
}

#undef _P_SOCK
#undef _P_HTXB
#undef _P_NBSENT

/****************************************************************************/

#define _P_SOCK     (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_NBRCV    (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[2]))

T5_DWORD TXBRCVTCP (T5_WORD wCommand, T5PTR_DB pBase,
                    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
#ifdef T5DEF_ETHERNET
        T5PTR_DBSYSRSC pRsc;
        T5_BOOL bFail;
        T5_BYTE buffer[256];
        T5_WORD wLen;

        if ((pRsc = T5VMRsc_FindSocket (pBase, _P_SOCK)) != NULL)
        {
            if (T5XS_Set (pBase, _P_HTXB, "", 0))
            {
                while ((wLen = T5Socket_Receive ((T5_SOCKET)_P_SOCK, 255, buffer, &bFail)) > 0
                    && !bFail)
                {
                    buffer[wLen] = '\0';
                    T5XS_Append (pBase, _P_HTXB, (T5_PTCHAR)buffer);
                    _P_NBRCV += (T5_LONG)wLen;
                }
            }
        }
#endif /*T5DEF_ETHERNET*/
    }
    _F_END
}

#undef _P_SOCK
#undef _P_HTXB
#undef _P_NBRCV


/****************************************************************************/

#endif /*T5DEF_TXB*/

/* eof **********************************************************************/
