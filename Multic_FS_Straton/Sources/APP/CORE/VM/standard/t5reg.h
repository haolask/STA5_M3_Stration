/*****************************************************************************
T5Reg.h   :  T5 Registry apis

DO NOT ALTER THIS !

(c) COPALP 2008
*****************************************************************************/

#ifndef _T5REG_H_INCLUDED_
#define _T5REG_H_INCLUDED_

/****************************************************************************/

#ifdef T5DEF_REG

/****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* limits */

#define T5REGMAX_STRING     200
#define T5REGMAX_PASSWORD   15
#define T5REGMAX_REGGET     230

/****************************************************************************/
/* STATIC: main format 

- header            sizeof (T5STR_T5REGHEAD)
- hash table        T5REG_HSIZE * sizeof (DWORD) - CONTAINS IDS NOT OFFSETS!
- records           sizeof (T5STR_T5REGITEM)
- strings           raw buffers - null terminated strings
- values            raw buffers (not aligned!)
- CRC32             raw CRC

*/

/****************************************************************************/
/* Protocol - max req size = 240 - max password = 15 chars

- GetSegment: (max size = 230)

Q= 'G' + dwOffset + dwSize
A= 'G' + dwOffset + dwSize + data
E= 'E' + wErrCode

- SaveRegistry

Q= 'S'
A= 'S'
E= 'E' + wErrCode

- SelectParam

Q= 'I' + dwItem + szPassword + '\0'
A= 'I' + dwItem
E= 'E' + wErrCode

- ReleaseParam

Q= 'R'
A= 'R'
E= 'E' + wErrCode

- PutParam (max string = 200 characters)

Q= 'P' + dwItem + bDataLen + data
A= 'P' + dwItem
E= 'E' + wErrCode

*/

#define T5REGFC_GET         'G'
#define T5REGFC_SAVE        'S'
#define T5REGFC_SELECT      'I'
#define T5REGFC_RLS         'R'
#define T5REGFC_PUT         'P'
#define T5REGFC_ERROR       'E'

#define T5REGERR_UNKNOWN    0xffff  /* bad request */
#define T5REGERR_BADOFFSET  0x1001  /* get: bad offset */
#define T5REGERR_SAVE       0x2001  /* save: cant save registry (busy) */
#define T5REGERR_BADID      0x3001  /* select: bad item ID */
#define T5REGERR_PSW        0x3002  /* select: bad password */
#define T5REGERR_NOSEL      0x4001  /* put: no valid selection */
#define T5REGERR_BADSEL     0x4002  /* put: bad selection */
#define T5REGERR_PUT        0x4003  /* put: access denied */

/****************************************************************************/
/* Registry header */

#define T5REG_HEADER_BE     "T5RM"
#define T5REG_HEADER_LE     "T5RI"

#ifdef T5DEF_BIGENDIAN
#define T5REG_HEADER        T5REG_HEADER_BE
#else /*T5DEF_BIGENDIAN */
#define T5REG_HEADER        T5REG_HEADER_LE
#endif /*T5DEF_BIGENDIAN*/

#define T5REG_HSIZE         17

#define T5REG_LOCK          0x00000001

typedef struct
{
    T5_CHAR  szID[4];       /* Fixed header = T5REG_HEADER */
    T5_DWORD dwFullSize;    /* full registry size in bytes */
    T5_DWORD dwBValSize;    /* size of value buffers in bytes */
    T5_DWORD pBVal;         /* offset to value buffers */
    T5_DWORD dwNbRec;       /* number of records */
    T5_DWORD dwDate;        /* last write date stamp (rtclk conv.) */
    T5_DWORD dwTime;        /* last write time stamp (rtclk conv.) */
    T5_DWORD dwVersion;     /* DB format version number (from XML) */
    T5_DWORD pXML;          /* offset to name of XML file */
    T5_DWORD dwFlags;       /* flags */
    T5_DWORD pPassword;     /* offset to administration password */
    T5_DWORD res;
}
T5STR_T5REGHEAD;

typedef T5STR_T5REGHEAD *T5PTR_T5REGHEAD;

/****************************************************************************/
/* Record */

#define T5REGF_FOLDER       0x00000001      /* record is a folder */
#define T5REGF_ROOT         0x00000002      /* record is the first of list */
#define T5REGF_LAST         0x00000004      /* record is the last of list */

#define T5REGF_PSW          0x00000010      /* record is protected */
#define T5REGF_RO           0x00000020      /* record is read only */

typedef struct
{
    T5_DWORD dwID;          /* unique ID = index in list */
    T5_DWORD dwParent;      /* ID of the parent folder item */
    T5_DWORD dwNext;        /* next ID in hash lists */
    T5_DWORD dwFlags;       /* flags */
    T5_DWORD pPassword;     /* offset to encrypted password */
    T5_DWORD pName;         /* offset to name */
    T5_DWORD pValue;        /* offset to value */
    T5_BYTE  bTicType;      /* data format in TIC convention */
    T5_BYTE  bMaxLen;       /* data buffer size or max string length */
    T5_BYTE  res1;
    T5_BYTE  res2;
}
T5STR_T5REGITEM;

typedef T5STR_T5REGITEM *T5PTR_T5REGITEM;

/****************************************************************************/
/* Application data */

typedef struct
{
    T5PTR_T5REGHEAD pReg;
    T5_DWORD dwram;
    T5_DWORD dwFile;
    T5PTR_T5REGITEM pSelect;
    T5_CHAR szPassword[T5REGMAX_PASSWORD+1];
}
T5STR_T5REGAPI;

typedef T5STR_T5REGAPI *T5PTR_T5REGAPI;

/****************************************************************************/
/* Helpers - calculate offsets */

#define T5REG_ISSEP(c) (((c)=='/')||((c)=='\\'))

#define T5REGSIZE_HEAD      (sizeof (T5STR_T5REGHEAD))
#define T5REGSIZE_HASH      (T5REG_HSIZE * sizeof (T5_DWORD))

#define T5REG_GETHASH(base) \
    ((T5_PTDWORD)((T5_PTBYTE)(base) + T5REGSIZE_HEAD))

#define T5REG_GETITEMS(base) \
    ((T5PTR_T5REGITEM)((T5_PTBYTE)(base) + T5REGSIZE_HEAD + T5REGSIZE_HASH))

#define T5REG_GETBYID(base, dwID) \
    (T5REG_GETITEMS(base) + dwID)

#define T5REG_GETSTRING(base, dwOffset) \
    ((T5_PTCHAR)((T5_PTBYTE)(base) + dwOffset))

#define T5REG_GETBYTES(base, dwOffset) \
    ((T5_PTBYTE)((T5_PTBYTE)(base) + dwOffset))

/****************************************************************************/
/* API - core */

extern T5_WORD T5Reg_SymbolHashCode (T5_PTBYTE szName);
extern T5_WORD T5Reg_SymbolHashCodeLen (T5_PTBYTE szName, T5_WORD wLen);
extern T5_BOOL T5Reg_CheckRegistry (T5_PTR pReg, T5_DWORD dwSize,
                                    T5_BOOL bCheckEndian);
extern T5_PTCHAR T5Reg_GetXmlFileName (T5_PTR pReg);
extern T5_PTCHAR T5Reg_GetAdminPassword (T5_PTR pReg);
extern void T5Reg_UpdateCRC (T5_PTR pReg);
extern T5_BOOL T5Reg_CheckPassword (T5_PTCHAR szCrypt, T5_PTCHAR szClean);
extern T5_DWORD T5Reg_Crypt (T5_PTCHAR szClean);

extern T5_BOOL T5Reg_LockRegistry (T5_PTR pReg);
extern T5_BOOL T5Reg_UnlockRegistry (T5_PTR pReg);
extern T5_BOOL T5Reg_IsRegistryLocked (T5_PTR pReg);

extern T5PTR_T5REGITEM T5Reg_FindItem (T5_PTR pReg, T5_PTCHAR szPath);
extern T5PTR_T5REGITEM T5Reg_GetItem (T5_PTR pReg, T5_DWORD dwID);
extern T5_PTCHAR T5Reg_GetItemPassword (T5_PTR pReg, T5PTR_T5REGITEM pItem);

extern T5_PTR T5Reg_GetValue (T5_PTR pReg, T5PTR_T5REGITEM pItem,
                              T5_PTR pBuffer, T5_WORD wSizeof);
extern T5_PTBYTE T5Reg_GetString (T5_PTR pReg, T5PTR_T5REGITEM pItem);

extern T5_BOOL T5Reg_CheckPut (T5_PTR pReg, T5PTR_T5REGITEM pItem,
                               T5_PTCHAR szPassword);
extern T5_BOOL T5Reg_PutValue (T5_PTR pReg, T5PTR_T5REGITEM pItem,
                               T5_PTR pBuffer, T5_PTCHAR szPassword);
extern T5_BOOL T5Reg_PutString (T5_PTR pReg, T5PTR_T5REGITEM pItem,
                                T5_PTBYTE pBuffer, T5_PTCHAR szPassword);
extern T5_BOOL T5Reg_PutValueA (T5_PTR pReg, T5PTR_T5REGITEM pItem,
                                T5_PTR pBuffer);
extern T5_BOOL T5Reg_PutStringA (T5_PTR pReg, T5PTR_T5REGITEM pItem,
                                 T5_PTBYTE pBuffer);

extern T5_BOOL T5Reg_Read (T5_PTR pReg, T5_PTCHAR szPath, T5_PTCHAR szValue);
extern T5_BOOL T5Reg_Write (T5_PTR pReg, T5_PTCHAR szPath, T5_PTCHAR szPassword, T5_PTCHAR szValue);

/****************************************************************************/
/* API - application level */

extern T5_BOOL T5RegApi_Open (T5PTR_T5REGAPI pApi, T5_PTCHAR szPath,
                              T5_BOOL bCheckEndian);
extern void T5RegApi_Close (T5PTR_T5REGAPI pApi);
extern T5_BOOL T5RegApi_Save (T5PTR_T5REGAPI pApi, T5_PTCHAR szPath);

extern T5_BOOL T5RegApi_Read (T5PTR_T5REGAPI pApi, T5_PTCHAR szPath,
                              T5_PTCHAR szValue); /*buffer must be char[256]*/

extern T5_BOOL T5RegApi_Write (T5PTR_T5REGAPI pApi, T5_PTCHAR szPath,
                               T5_PTCHAR szPassword, T5_PTCHAR szValue);

extern T5_WORD T5RegApi_Serve (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                               T5_PTBYTE pAnswer, T5_PTCHAR szPath);
extern T5_WORD T5RegApi_ServeT5 (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                                 T5_PTBYTE pAnswer, T5_PTCHAR szPath);

/****************************************************************************/
/* API - application level - helper for use with T5CS server */

#ifndef T5DEF_NOT5CS

extern void T5RegApi_ServeT5CS (T5PTR_T5REGAPI pApi, T5PTR_CS pCS,
                                T5_WORD wCaller, T5_PTCHAR szPath);

#endif /*T5DEF_NOT5CS*/

/****************************************************************************/
/* API - host converter */

extern T5_BOOL T5RegHcv_ToHost (T5_PTR pReg);
extern T5_BOOL T5RegHcv_ToRTI (T5_PTR pReg);
extern T5_BOOL T5RegHcv_ToRTM (T5_PTR pReg);

/****************************************************************************/
/* API - host client interface */

extern T5_WORD T5RegHcl_BuildGet (T5_PTBYTE pQuestion, T5_DWORD dwOffset,
                                  T5_DWORD dwSize);
extern T5_WORD T5RegHcl_BuildSave (T5_PTBYTE pQuestion);
extern T5_WORD T5RegHcl_BuildSelect (T5_PTBYTE pQuestion, T5_DWORD dwItem,
                                     T5_PTCHAR szPassword);
extern T5_WORD T5RegHcl_BuildRls (T5_PTBYTE pQuestion);
extern T5_WORD T5RegHcl_BuildPut (T5_PTBYTE pQuestion, T5_DWORD dwItem,
                                  T5_BYTE bLen, T5_PTBYTE pData);

extern T5_BOOL T5RegHcl_IsError (T5_PTBYTE pAnswer, T5_PTWORD pwCode);
extern T5_BOOL T5RegHcl_ParseGet (T5_PTBYTE pAnswer, T5_PTDWORD pdwOffset,
                                  T5_PTDWORD pdwSize);
extern T5_PTBYTE T5RegHcl_ParseGetData (T5_PTBYTE pAnswer);
extern T5_BOOL T5RegHcl_ParseSave (T5_PTBYTE pAnswer);
extern T5_BOOL T5RegHcl_ParseSelect (T5_PTBYTE pAnswer);
extern T5_WORD T5RegHcl_ParseRls (T5_PTBYTE pAnswer);
extern T5_WORD T5RegHcl_ParsePut (T5_PTBYTE pAnswer);

/****************************************************************************/
/* OEM API - RAM file/flash exchange */

#define T5REGOEM_XML    1   /* XML definition file */
#define T5REGOEM_REG    2   /* registry - binary image */

extern T5_DWORD T5RegOem_GetFileSize (T5_WORD wID, T5_PTCHAR szName);
extern T5_DWORD T5RegOem_OpenWrite (T5_WORD wID, T5_PTCHAR szName,
                                    T5_DWORD dwSize);
extern T5_DWORD T5RegOem_OpenRead (T5_WORD wID, T5_PTCHAR szName);
extern void T5RegOem_Close (T5_DWORD dwf);
extern T5_BOOL T5RegOem_Write (T5_DWORD dwf, T5_DWORD dwSize, T5_PTBYTE pData);
extern T5_BOOL T5RegOem_Read (T5_DWORD dwf, T5_DWORD dwSize, T5_PTBYTE pData);

extern T5_DWORD T5RegOem_AllocRAM (T5_DWORD dwSize);
extern T5_PTR T5RegOem_LinkRAM (T5_DWORD dwram);
extern void T5RegOem_UnlinkRAM (T5_DWORD dwram, T5_PTR pRam);
extern void T5RegOem_FreeRAM (T5_DWORD dwram, T5_PTR pRam);

/****************************************************************************/

#ifdef __cplusplus
}
#endif

/****************************************************************************/

#endif /*T5DEF_REG*/

/****************************************************************************/

#endif /*_T5REG_H_INCLUDED_*/

/* eof **********************************************************************/
