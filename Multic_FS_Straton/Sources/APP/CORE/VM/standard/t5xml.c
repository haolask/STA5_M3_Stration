/*****************************************************************************
T5xml.c :    XML management function blocks

DO NOT ALTER THIS !

(c) COPA-DATA France 2009
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_XML

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
/* XMLManager */

#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_NBUFFERS (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_NTAGS    (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[2]))

T5_DWORD XMLMANAGER (T5_WORD wCommand, T5PTR_DB pBase,
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
        _P_BOK = (pPrivate->pHeap != NULL);
        _P_NBUFFERS = pStatus->dwNbXML;
        _P_NTAGS = pStatus->dwNbXML; /* ... */
        return 0L;
    case T5FBCMD_ACCEPTCT :
        return 1L;
    default :
        return 0L;
    }
}

#undef _P_BOK
#undef _P_NBUFFERS
#undef _P_NTAGS

/****************************************************************************/
/* XmlLastError */

#define _P_ERRNO    (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))

T5_DWORD XMLLASTERROR (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        pStatus = T5GET_DBSTATUS(pBase);
        _P_ERRNO = pStatus->dwErrorXML;
    }
    _F_END
}

#undef _P_ERRNO

/****************************************************************************/
/* XmlNewDoc */

#define _P_SZTAG        (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[0]))
#define _P_HROOTTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))

T5_DWORD XMLNEWDOC (T5_WORD wCommand, T5PTR_DB pBase,
                    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_HROOTTAG = T5XML_Alloc (pBase, _P_SZTAG + 2);
    }
    _F_END
}

#undef _P_SZTAG
#undef _P_HROOTTAG

/****************************************************************************/
/* XmlFreeDoc */

#define _P_HROOTTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_BOK          (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))

T5_DWORD XMLFREEDOC (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XML_Free (pBase, _P_HROOTTAG);
    }
    _F_END
}

#undef _P_HROOTTAG
#undef _P_BOK

/****************************************************************************/
/* XmlGetTagName */

#define _P_HTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_SZTAG    (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))

T5_DWORD XMLGETTAGNAME (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_DWORD dwLen;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        dwLen = (T5_DWORD)(T5_BYTE)(_P_SZTAG[0]);
        if (T5XML_GetTagName (pBase, _P_HTAG, _P_SZTAG+2, dwLen + 1L))
            _P_SZTAG[1] = (T5_CHAR)T5_STRLEN(_P_SZTAG+2);
        else
            _P_SZTAG[1] = _P_SZTAG[2] = 0;
    }
    _F_END
}

#undef _P_HTAG
#undef _P_SZTAG

/****************************************************************************/
/* XmlCheckTagName */

#define _P_HTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_SZTAG    (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

T5_DWORD XMLCHECKTAGNAME (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_CHAR sz[256];
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        if (!T5XML_GetTagName (pBase, _P_HTAG, sz, sizeof (sz)))
            _P_BOK = FALSE;
        else _P_BOK = (T5VMStr_ICmp ((T5_PTBYTE)sz, (T5_PTBYTE)(_P_SZTAG+2)) == 0);
    }
    _F_END
}

#undef _P_HTAG
#undef _P_SZTAG
#undef _P_BOK

/****************************************************************************/
/* XmlSetTagCont */

#define _P_ANYTYPE  pArgs[0]
#define _P_HTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_DATA     pArgs[2]
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))

#define _P_DATATEXT (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[2]))

T5_DWORD XMLSETTAGCONT (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    char sz[128];
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        switch (_P_ANYTYPE)
        {
        case T5C_STRING :
            _P_BOK = T5XML_SetTagContents (pBase, _P_HTAG, _P_DATATEXT+2);
            break;
        default :
            T5VMCnv_NumToSZ (pBase, _P_ANYTYPE, _P_DATA, sz);
            _P_BOK = T5XML_SetTagContents (pBase, _P_HTAG, sz);
            break;
        }
    }
    _F_END
}

#undef _P_ANYTYPE
#undef _P_HTAG
#undef _P_DATA
#undef _P_BOK
#undef _P_DATATEXT

/****************************************************************************/
/* XmlGetTagCont */

#define _P_ANYTYPE  pArgs[0]
#define _P_HTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_DATA     pArgs[2]
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))

#define _P_DATATEXT (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[2]))

T5_DWORD XMLGETTAGCONT (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    char sz[128];
    T5_DWORD dwLen;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        switch (_P_ANYTYPE)
        {
        case T5C_STRING :
            dwLen = (T5_DWORD)(T5_BYTE)(_P_DATATEXT[0]);
            _P_BOK = T5XML_GetTagContents (pBase, _P_HTAG, _P_DATATEXT+2, dwLen);
            if (_P_BOK)
                _P_DATATEXT[1] = (T5_CHAR)T5_STRLEN(_P_DATATEXT+2);
            else
                _P_DATATEXT[1] = _P_DATATEXT[2] = 0;
            break;
        default :
            _P_BOK = T5XML_GetTagContents (pBase, _P_HTAG, sz, sizeof (sz));
            if (_P_BOK)
                T5VMCnv_SZToNum (pBase, _P_ANYTYPE, _P_DATA, sz);
            break;
        }
    }
    _F_END
}

#undef _P_ANYTYPE
#undef _P_HTAG
#undef _P_DATA
#undef _P_BOK
#undef _P_DATATEXT

/****************************************************************************/
/* XmlSetTagContT */

#define _P_HTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

T5_DWORD XMLSETTAGCONTT (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XML_SetTagContentsXS (pBase, _P_HTAG, _P_HTXB);
    }
    _F_END
}

#undef _P_HTAG
#undef _P_HTXB
#undef _P_BOK

/****************************************************************************/
/* XmlGetTagContT */

#define _P_HTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_HTXB     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))

T5_DWORD XMLGETTAGCONTT (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_HTXB = T5XML_GetTagContentsXS (pBase, _P_HTAG);
    }
    _F_END
}

#undef _P_HTAG
#undef _P_HTXB

/****************************************************************************/
/* XmlAddChild */

#define _P_HTAG         (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_SZTAG        (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_HCHILDTAG    (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[2]))

T5_DWORD XMLADDCHILD (T5_WORD wCommand, T5PTR_DB pBase,
                      T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_HCHILDTAG = T5XML_AddChild (pBase, _P_HTAG, _P_SZTAG+2);
    }
    _F_END
}

#undef _P_HTAG
#undef _P_SZTAG
#undef _P_HCHILDTAG

/****************************************************************************/
/* XmlFirstChild */

#define _P_HTAG         (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_SZTAG        (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_HCHILDTAG    (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[2]))

T5_DWORD XMLFIRSTCHILD (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_HCHILDTAG = T5XML_FirstChild (pBase, _P_HTAG, _P_SZTAG+2);
    }
    _F_END
}

#undef _P_HTAG
#undef _P_SZTAG
#undef _P_HCHILDTAG

/****************************************************************************/
/* XmlNextChild */

#define _P_HTAG         (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_HCHILDTAG    (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_SZTAG        (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[2]))
#define _P_HCHILDNEXT   (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[3]))

T5_DWORD XMLNEXTCHILD (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_HCHILDNEXT = T5XML_NextChild (pBase, _P_HTAG, _P_HCHILDTAG, _P_SZTAG+2);
    }
    _F_END
}

#undef _P_HTAG
#undef _P_HCHILDTAG
#undef _P_SZTAG
#undef _P_HCHILDNEXT

/****************************************************************************/
/* XmlGetTagAttrib */

#define _P_ANYTYPE  pArgs[0]
#define _P_HTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_SZATTRIB (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[2]))
#define _P_DATA     pArgs[3]
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[4]))

#define _P_DATATEXT (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[3]))

T5_DWORD XMLGETTAGATTRIB (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    char sz[128];
    T5_DWORD dwLen;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        switch (_P_ANYTYPE)
        {
        case T5C_STRING :
            dwLen = (T5_DWORD)(T5_BYTE)(_P_DATATEXT[0]);
            _P_BOK = T5XML_GetTagAttrib (pBase, _P_HTAG, _P_SZATTRIB+2, _P_DATATEXT+2, dwLen);
            if (_P_BOK)
                _P_DATATEXT[1] = (T5_CHAR)T5_STRLEN(_P_DATATEXT+2);
            else
                _P_DATATEXT[1] = _P_DATATEXT[2] = 0;
            break;
        default :
            _P_BOK = T5XML_GetTagAttrib (pBase, _P_HTAG, _P_SZATTRIB+2, sz, sizeof (sz));
            if (_P_BOK)
                T5VMCnv_SZToNum (pBase, _P_ANYTYPE, _P_DATA, sz);
            break;
        }
    }
    _F_END
}

#undef _P_ANYTYPE
#undef _P_HTAG
#undef _P_SZATTRIB
#undef _P_DATA
#undef _P_BOK
#undef _P_DATATEXT

/****************************************************************************/
/* XmlSetTagAttrib */

#define _P_ANYTYPE  pArgs[0]
#define _P_HTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_SZATTRIB (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[2]))
#define _P_DATA     pArgs[3]
#define _P_BOK      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[4]))

#define _P_DATATEXT (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[3]))

T5_DWORD XMLSETTAGATTRIB (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    char sz[128];
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        switch (_P_ANYTYPE)
        {
        case T5C_STRING :
            _P_BOK = T5XML_SetTagAttrib (pBase, _P_HTAG, _P_SZATTRIB+2, _P_DATATEXT+2);
            break;
        default :
            T5VMCnv_NumToSZ (pBase, _P_ANYTYPE, _P_DATA, sz);
            _P_BOK = T5XML_SetTagAttrib (pBase, _P_HTAG, _P_SZATTRIB+2, sz);
            break;
        }
    }
    _F_END
}

#undef _P_ANYTYPE
#undef _P_HTAG
#undef _P_SZATTRIB
#undef _P_DATA
#undef _P_BOK
#undef _P_DATATEXT

/****************************************************************************/
/* XmlWriteDocFile */

#define _P_HROOTTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_SZPATH       (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_BOK          (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

T5_DWORD XMLWRITEDOCFILE (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XML_WriteToFile (pBase, _P_HROOTTAG, _P_SZPATH+2);
    }
    _F_END
}

#undef _P_HROOTTAG
#undef _P_SZPATH
#undef _P_BOK

/****************************************************************************/
/* XmlWriteDocTxb */

#define _P_HROOTTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_HTXB         (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_BOK          (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

T5_DWORD XMLWRITEDOCTXB (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_BOK = T5XML_WriteToXS (pBase, _P_HROOTTAG, _P_HTXB);
    }
    _F_END
}

#undef _P_HROOTTAG
#undef _P_HTXB
#undef _P_BOK

/****************************************************************************/
/* XmlParseDocFile */

#define _P_SZPATH       (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[0]))
#define _P_HROOTTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))

T5_DWORD XMLPARSEDOCFILE (T5_WORD wCommand, T5PTR_DB pBase,
                          T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_HROOTTAG = T5XML_ParseFile (pBase, _P_SZPATH+2);
    }
    _F_END
}

#undef _P_SZPATH
#undef _P_HROOTTAG

/****************************************************************************/
/* XmlParseDocTxb */

#define _P_HTXB         (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_HROOTTAG     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))

T5_DWORD XMLPARSEDOCTXB (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_HROOTTAG = T5XML_ParseXS (pBase, _P_HTXB);
    }
    _F_END
}

#undef _P_HTXB
#undef _P_HROOTTAG

/****************************************************************************/
/* XmlGetSybValue */

#define _P_SZSYB        (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[0]))
#define _P_KEEPCASE     (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_VALUE        (*((T5_PTBYTE *)(T5GET_DBSTRING(pBase))+pArgs[2]))

T5_DWORD XMLGETSYBVALUE (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_CHAR sz[128];
    T5_BYTE bLen, bMax, bType;
    T5_PTBYTE pSrc;
    T5_PTR pData;
    T5_UNUSED (pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        bMax = _P_VALUE[0];
        _P_VALUE[1] = _P_VALUE[2] = '\0';
        pData = T5VmSyb_ParseSingle (pBase, (T5_PTCHAR)_P_SZSYB+2, &bType);
        if (pData == NULL)
            return 0L;
        switch (bType & ~T5C_EXTERN)
        {
        case T5C_COMPLEX :
            break;
        case T5C_STRING :
            pSrc = (T5_PTBYTE)(pData);
            bLen = pSrc[1];
            if (bLen > bMax)
                bLen = bMax;
            _P_VALUE[1] = bLen;
            T5_MEMCPY (_P_VALUE + 2, pSrc + 2, bLen);
            _P_VALUE[bLen+2] = '\0';
            break;
        default :
            T5VMCnv_NumToSZPtr (pData, (T5_WORD)bType, sz);
            bLen = (T5_BYTE)T5_STRLEN(sz);
            if (bLen > bMax)
                bLen = bMax;
            _P_VALUE[1] = bLen;
            T5_MEMCPY (_P_VALUE + 2,sz, bLen);
            _P_VALUE[bLen+2] = '\0';
            break;
        }
    }
    _F_END
}

#undef _P_SZSYB
#undef _P_KEEPCASE
#undef _P_VALUE

/****************************************************************************/
/* XmlSetSybValue */

#define _P_SZSYB        (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[0]))
#define _P_VALUE        (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_KEEPCASE     (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))
#define _P_BOK          (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[3]))

T5_DWORD XMLSETSYBVALUE (T5_WORD wCommand,T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_BYTE bLen, bType;
    T5_PTBYTE pDst;
    T5_PTR pData;
    T5_UNUSED (pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        _P_BOK = TRUE;
        pData = T5VmSyb_ParseSingle (pBase, (T5_PTCHAR)_P_SZSYB + 2, &bType);
        if (pData == NULL)
        {
            _P_BOK = FALSE;
            return 0L;
        }
        switch (bType & ~T5C_EXTERN)
        {
        case T5C_COMPLEX :
            _P_BOK = FALSE;
            break;
        case T5C_STRING :
            pDst = (T5_PTBYTE)(pData);
            bLen = _P_VALUE[1];
            if (bLen > pDst[0])
                bLen = pDst[0];
            pDst[1] = bLen;
            T5_MEMCPY (pDst + 2, _P_VALUE + 2, bLen);
            pDst[bLen+2] = '\0';
            break;
        default :
            T5VMCnv_SZToNumPtr (pData, (T5_WORD)bType, _P_VALUE + 2);
            break;
        }
    }
    _F_END
}

#undef _P_SZSYB
#undef _P_VALUE
#undef _P_KEEPCASE
#undef _P_BOK

/****************************************************************************/
/* V2 - XmlGetParent */

#define _P_HTAG         (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_HPARENT      (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))

T5_DWORD XMLGETPARENT (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_HPARENT = T5XML_GetParent (pBase, _P_HTAG);
    }
    _F_END
}

#undef _P_HTAG
#undef _P_HPARENT

/****************************************************************************/
/* HMI menus */

/****************************************************************************/
/* hmiNewMenu */

#define _P_PARENT   (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_TEXT     (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_MENU     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[2]))

T5_DWORD HMINEWMENU (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_DWORD hTag;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        if (_P_PARENT)
            hTag = T5XML_AddChild (pBase, _P_PARENT, (T5_PTCHAR)"MNU");
        else
            hTag = T5XML_Alloc (pBase, (T5_PTCHAR)"MNU");
        if (hTag)
        {
            T5XML_SetTagContents (pBase, hTag, _P_TEXT+2);
            T5XML_SetTagAttrib (pBase, hTag, (T5_PTCHAR)"ID", (T5_PTCHAR)"M");
        }
        _P_MENU = hTag;
    }
    _F_END
}

#undef _P_PARENT
#undef _P_TEXT
#undef _P_MENU

/****************************************************************************/
/* hmiNewMenuItem */

#define _P_MENU     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_TEXT     (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_ID       (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[2]))
#define _P_ITEM     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[3]))

T5_DWORD HMINEWMENUITEM (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_DWORD hTag;
    T5_CHAR szID[12];
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        hTag = T5XML_AddChild (pBase, _P_MENU, (T5_PTCHAR)"MNU");
        if (hTag)
        {
            T5XML_SetTagContents (pBase, hTag, _P_TEXT+2);
            T5_LTOA (szID, _P_ID);
            T5XML_SetTagAttrib (pBase, hTag, (T5_PTCHAR)"ID", szID);
        }
        _P_ITEM = hTag;
    }
    _F_END
}

#undef _P_MENU
#undef _P_TEXT
#undef _P_ID
#undef _P_ITEM

/****************************************************************************/
/* hmiDelMenu */

#define _P_MENU (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_OK   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))

T5_DWORD HMIDELMENU (T5_WORD wCommand, T5PTR_DB pBase,
                     T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_OK = T5XML_Free (pBase, _P_MENU);
    }
    _F_END
}

#undef _P_MENU
#undef _P_OK

/****************************************************************************/
/* hmiRenameMenu */

#define _P_ITEM     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_TEXT     (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[1]))
#define _P_OK       (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

T5_DWORD HMIRENAMEMENU (T5_WORD wCommand, T5PTR_DB pBase,
                        T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);
    _F_BEGIN
    {
        _P_OK = T5XML_SetTagContents (pBase, _P_ITEM, _P_TEXT+2);
    }
    _F_END
}

#undef _P_ITEM
#undef _P_TEXT
#undef _P_OK

/****************************************************************************/
/* hmiMenu */

#define _P_UP       (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_DN       (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_OK       (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))
#define _P_ESC      (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))
#define _P_MENU     (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[4]))
#define _P_SELID    (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[5]))
#define _P_SELPOS   (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[6]))
#define _P_TITLE    (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[7]))
#define _P_TEXTS    (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[8]))

#define _MAX_ITEM   32

typedef struct
{
    T5_BOOL bPrevUP;
    T5_BOOL bPrevDN;
    T5_BOOL bPrevOK;
    T5_BOOL bPrevESC;
    T5_DWORD hCurMenu;
    T5_DWORD hRoot;
    T5_DWORD dwItems[_MAX_ITEM];
    T5_DWORD dwNb;
    T5_DWORD dwSelect;
    T5_CHAR szTitle[256];
    T5_CHAR szMnu[256];
} _str_FB_HMIMENU;

T5_DWORD HMIMENU (T5_WORD wCommand, T5PTR_DB pBase,
                  T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    _str_FB_HMIMENU *pData;
    T5_BOOL bReload;
    T5_CHAR szID[128];
    T5_DWORD hTag;
    T5_BYTE bLen, bMax;
    T5_UNUSED(pClass);

    pData = (_str_FB_HMIMENU *)pInst;
    switch (wCommand)
    {
    case T5FBCMD_ACTIVATE :
        _P_SELID = 0L;
        /* check if reload needed */
        bReload = FALSE;
        if (pData->hCurMenu == 0L || pData->hRoot != _P_MENU)
        {
            pData->hRoot = pData->hCurMenu = _P_MENU;
            bReload = TRUE;
        }
        else if (_P_OK && !(pData->bPrevOK))
        {
            if (T5XML_GetTagAttrib (pBase, pData->dwItems[pData->dwSelect], (T5_PTCHAR)"ID", szID, sizeof (szID))
                && T5_STRCMP (szID, "M") == 0)
            {
                pData->hCurMenu = pData->dwItems[pData->dwSelect];
                bReload = TRUE;
            }
        }
        else if (_P_ESC && !(pData->bPrevESC) && pData->hCurMenu != pData->hRoot)
        {
            hTag = T5XML_GetParent (pBase, pData->hCurMenu);
            if (hTag != 0L)
            {
                pData->hCurMenu = hTag;
                bReload = TRUE;
            }
        }
        /* reload menu if needed */
        if (bReload)
        {
            T5XML_GetTagContents (pBase, pData->hCurMenu, pData->szTitle, sizeof (pData->szTitle));
            pData->dwSelect = 0L;
            pData->dwNb = 0L;
            pData->szMnu[0] = '\0';
            hTag = T5XML_FirstChild (pBase, pData->hCurMenu, (T5_PTCHAR)"\0");
            while (hTag && pData->dwNb < _MAX_ITEM)
            {
                pData->dwItems[pData->dwNb] = hTag;
                pData->dwNb += 1L;
                T5XML_GetTagContents (pBase, hTag, szID, sizeof (szID));
                if (pData->szMnu[0] != '\0' && T5_STRLEN(pData->szMnu) < 256)
                    T5_STRCAT (pData->szMnu, "|");
                if ((T5_STRLEN(szID) + T5_STRLEN(pData->szMnu)) < 256)
                    T5_STRCAT (pData->szMnu, szID);
                hTag = T5XML_NextChild (pBase, pData->hCurMenu, hTag, (T5_PTCHAR)"\0");
            }
        }
        /* up / down / OK */
        else if (pData->dwNb && _P_UP && !(pData->bPrevUP))
        {
            if (pData->dwSelect > 0L)
                pData->dwSelect -= 1L;
            else
                pData->dwSelect = pData->dwNb - 1L;
        }
        else if (pData->dwNb && _P_DN && !(pData->bPrevDN))
        {
            if (pData->dwSelect < (pData->dwNb - 1))
                pData->dwSelect += 1L;
            else
                pData->dwSelect = 0L;
        }
        else if (_P_OK && !(pData->bPrevOK)
            && T5XML_GetTagAttrib (pBase, pData->dwItems[pData->dwSelect], (T5_PTCHAR)"ID", szID, sizeof (szID))
            && T5_STRCMP (szID, "M") != 0)
        {
            _P_SELID = T5_ATOI(szID);
        }
        /* trig inputs */
        pData->bPrevUP = _P_UP;
        pData->bPrevDN = _P_DN;
        pData->bPrevOK = _P_OK;
        pData->bPrevESC = _P_ESC;
        /* set outputs */
        _P_SELPOS = pData->dwSelect + 1L;
        bMax = *(T5_PTBYTE)_P_TITLE;
        bLen = (T5_BYTE)T5_STRLEN (pData->szTitle);
        if (bLen > bMax)
            bLen = bMax;
        _P_TITLE[1] = bLen;
        T5_MEMCPY (_P_TITLE+2, pData->szTitle, bLen);
        _P_TITLE[bLen+2] = '\0';
        bMax = *(T5_PTBYTE)_P_TEXTS;
        bLen = (T5_BYTE)T5_STRLEN (pData->szMnu);
        if (bLen > bMax)
            bLen = bMax;
        _P_TEXTS[1] = bLen;
        T5_MEMCPY (_P_TEXTS+2, pData->szMnu, bLen);
        _P_TEXTS[bLen+2] = '\0';
        return 0L;
    case T5FBCMD_INITINSTANCE :

        return 0L;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FB_HMIMENU);
    case T5FBCMD_ACCEPTCT :
        return 1L;
    default :
        return 0L;
    }
}

#undef _P_UP
#undef _P_DN
#undef _P_OK
#undef _P_ESC
#undef _P_MENU
#undef _P_SELID
#undef _P_SELPOS
#undef _P_TITLE
#undef _P_TEXTS
#undef _MAX_ITEM

/****************************************************************************/

#endif /*T5DEF_XML*/

/* eof **********************************************************************/
