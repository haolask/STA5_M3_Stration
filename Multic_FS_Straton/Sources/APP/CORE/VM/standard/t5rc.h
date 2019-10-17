/*****************************************************************************
T5rc.h :     resources manager - definitions

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#ifndef _T5RC_H_INCLUDED_
#define _T5RC_H_INCLUDED_

#ifdef T5DEF_RC

/****************************************************************************/
/* file structure

- File header: T5STR_RCFHEAD
- Directory entries: T5STR_RCENTRY[nb_resources]
- Resources contents: unaligned byte stream

The size in file header includes directory + resources contents.
The CRC32 in file header is performed on directory + resources contents.

Indexes passed to T5Rc_LoadRCByIndex are indexes in the directory + 1
    (from 1 to N)

*/

/****************************************************************************/
/* screen resources */

/* predefined objects */

#define T5RC_SCO_NULL       0   /* null object - used as end of list */
#define T5RC_SCO_SCREEN     255 /* container pseudo-object: screen params */

/* predefined attributes */

#define T5RC_SCA_NULL       0   /* null attribute - used as end of list */
#define T5RC_SCA_OBTYPE     255 /* type of object: unsigned */

/* predefined container attributes */

#define T5RC_SCC_CRCIEC     1   /* CRC of the IEC project */
#define T5RC_SCC_NBITEM     2   /* number of object in the screen */
#define T5RC_SCC_SIZE       3   /* screen size */
#define T5RC_SCC_BACKCOL    4   /* background color index */
#define T5RC_SCC_PALETTE    5   /* palette ID */
#define T5RC_SCC_BACKBMP    6   /* background bitmap */

/* properties formats */

#define T5RC_SCF_NONE       0   /* no contents */
#define T5RC_SCF_INT1       1   /* signed 8 bit integer */
#define T5RC_SCF_INT2       2   /* signed 16 bit integer */
#define T5RC_SCF_INT4       3   /* signed 32 bit integer */
#define T5RC_SCF_INT8       4   /* signed 64 bit integer */
#define T5RC_SCF_UINT1      5   /* unsigned 8 bit integer */
#define T5RC_SCF_UINT2      6   /* unsigned 16 bit integer */
#define T5RC_SCF_UINT4      7   /* unsigned 32 bit integer */
#define T5RC_SCF_UINT8      8   /* unsigned 64 bit integer */
#define T5RC_SCF_REAL       9   /* 32 bit float */
#define T5RC_SCF_LREAL      10  /* 64 bit float */
#define T5RC_SCF_PAIR       11  /* pair of unsigned 16 bit integer */
#define T5RC_SCF_STRING     12  /* null terminated string */
#define T5RC_SCF_RCID       13  /* ID of resource  */
#define T5RC_SCF_VAR        14  /* variable: type:byte + index:word */
#define T5RC_SCF_CTVAR      15  /* variable: type:byte + ctoffset:word */
#define T5RC_SCF_ARRAY      16  /* array: type:byte + dim:word + index:word */
#define T5RC_SCF_CTARRAY    17  /* array: type:byte + dim:word ctoffset:word */

/* parser */

extern T5_BYTE T5Rcs_GetObjType (T5_PTBYTE pStream);
extern T5_BYTE T5Rcs_GetProp (T5_PTBYTE pStream, T5_PTBYTE pbFormat);
extern T5_PTBYTE T5Rcs_NextToken (T5_PTBYTE pStream);

extern T5_LONG T5RCS_GetSignedProp (T5_PTBYTE pStream);
extern T5_DWORD T5RCS_GetUnsignedProp (T5_PTBYTE pStream);

#ifdef T5DEF_REALSUPPORTED
extern T5_REAL T5RCS_GetRealProp (T5_PTBYTE pStream);
#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_LREALSUPPORTED
extern T5_LREAL T5RCS_GetLrealProp (T5_PTBYTE pStream);
#endif /*T5DEF_LREALSUPPORTED*/

#ifdef T5DEF_LINTSUPPORTED
extern T5_LINT T5RCS_GetLintProp (T5_PTBYTE pStream);
#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_STRINGSUPPORTED
extern T5_PTCHAR T5RCS_GetStringProp (T5_PTBYTE pStream);
#endif /*T5DEF_STRINGSUPPORTED*/

extern T5_BOOL T5RCS_GetPairProp (T5_PTBYTE pStream,
                                  T5_PTWORD pwX, T5_PTWORD pwY);
extern T5_PTR T5RCS_GetVarProp (T5_PTBYTE pStream, T5PTR_DB pDB,
                                T5_PTBYTE pbType);
extern T5_PTR T5RCS_GetArrayProp (T5_PTBYTE pStream, T5PTR_DB pDB,
                                  T5_PTBYTE pbType, T5_PTWORD pwDim);

/* usage

format: { ObjType { Prop } NullProp } NullObj

T5_PTBYTE pStream = ... pointer to screen resources ...
T5_BYTE bObType;
T5_BYTE bPropType, bPropFormat;

while ((bObType = T5Rcs_GetObjType (pStream)) != T5RC_SCO_NULL)
{
    ...

    // skip object type
    pStream = T5Rcs_NextToken (pStream);
    // go through list of properties
    while ((bPropType = T5Rcs_GetProp (pStream, &bPropFormat))
                != T5RC_SCA_NULL)
    {
        ... T5RCS_Get???Prop() ...

        // move to next property
        pStream = T5Rcs_NextToken (pStream);
    }
    // skip null property (end of list)
    pStream = T5Rcs_NextToken (pStream);
}

*/

/****************************************************************************/
/* font resources */

#define T5RCFONT_SYSTEM 0   /* system font (name) */
#define T5RCFONT_USER   1   /* user defined font (bitstream) */

typedef struct
{
    T5_BYTE  bType;         /* type of font */
    T5_BYTE  res1;
    T5_WORD  wFlags;        /* reserved for extensions */
    T5_WORD  wWidth;        /* character width in pixels */
    T5_WORD  wHeight;       /* character height in pixels */
    T5_WORD  wNbChar;       /* number of characters */
    T5_WORD  wFirstChar;    /* ASCII code of the 1rst character */
    T5_DWORD dwStreamSize;  /* size in bytes of the stream following */
    /* stream */            /* null terminated name or bitstream */
}
T5STR_RCFONTHEAD;

typedef T5STR_RCFONTHEAD *T5PTR_RCFONTHEAD;

/****************************************************************************/
/* bitmap resources */

#define T5RCBMP_SYSTEM  0   /* system bitmap (pathname) */
#define T5RCBMP_USER    1   /* user defined bitmap (bitstream) */

typedef struct
{
    T5_BYTE  bType;         /* type of bitmap */
    T5_BYTE  bPixSize;      /* number of bits per pixel (_user only) */
    T5_WORD  wFlags;        /* reserved for extensions */
    T5_WORD  wWidth;        /* width in pixels */
    T5_WORD  wHeight;       /* height in pixels */
    T5_DWORD dwPalette;     /* ID of the palette */
    T5_DWORD dwStreamSize;  /* size in bytes of the stream following */
    /* stream */            /* null terminated name or bitstream */
}
T5STR_RCBMPHEAD;

typedef T5STR_RCBMPHEAD *T5PTR_RCBMPTHEAD;

/****************************************************************************/
/* types of resources */

#define T5RC_SCRN       1   /* screens (scripts) */
#define T5RC_STRG       2   /* character strings */
#define T5RC_BMPS       3   /* bitmaps */
#define T5RC_FONT       4   /* character fonts */

/****************************************************************************/
/* resource file header */

typedef struct
{
    T5_BYTE  bBigEndian;    /* TRUE if big endian */
    T5_BYTE  bVersion;      /* reserved for extensions - initially 0 */
    T5_BYTE  bKind;         /* kind of resource */
    T5_BYTE  res;
    T5_DWORD dwSize;        /* size of file contents area */
    T5_DWORD dwNb;          /* number of resources in directory */
    T5_DWORD dwCRC;         /* CRC calculated on file contents */
}
T5STR_RCFHEAD;

typedef T5STR_RCFHEAD *T5PTR_RCFHEAD;

/****************************************************************************/
/* resource file directory entry */

typedef struct
{
    T5_DWORD dwSize;        /* size in bytes */
    T5_DWORD dwUserID;      /* uder defined resource ID */
    T5_DWORD dwOffset;      /* offset in file contents area */
}
T5STR_RCENTRY;

typedef T5STR_RCENTRY *T5PTR_RCENTRY;

/****************************************************************************/
/* resource entry in memory heap */

#define T5RCF_SCRN          0x80000000L     /* screens (scripts) */
#define T5RCF_STRG          0x40000000L     /* character strings */
#define T5RCF_BMPS          0x20000000L     /* bitmaps */
#define T5RCF_FONT          0x10000000L     /* character fonts */

#define T5RCF_FROMFLASH     0x00000001L     /* direct pointer from flash */

typedef struct
{
    T5_DWORD dwSize;        /* size in bytes */
    T5_PTR   pData;         /* pointer to resource data */
}
T5STR_RCITEM;

typedef T5STR_RCITEM *T5PTR_RCITEM;

/****************************************************************************/
/* APIs */

extern T5_BOOL  T5Rc_CheckRCFile (T5_BYTE bKind);

extern T5_PTR   T5Rc_Init_Buffer (T5_PTR pBuffer, T5_DWORD dwSize,
                                  T5_DWORD dwFlags);
extern T5_PTR   T5Rc_Init_Malloc (T5_DWORD dwGrowBy, T5_DWORD dwFlags);

extern void     T5Rc_Release (T5_PTR pRCMan);
extern void     T5Rc_Compact (T5_PTR pRCMan);

extern T5_DWORD T5Rc_LoadRCDirectory (T5_PTR pRCMan, T5_BYTE bKind,
                                      T5_PTDWORD pdwCount);
extern T5_DWORD T5Rc_LoadRCByIndex (T5_PTR pRCMan, T5_BYTE bKind,
                                    T5_DWORD dwIndex);
extern T5_PTR   T5Rc_LockRC (T5_PTR pRCMan, T5_DWORD hRC, T5_PTDWORD pdwSize);
extern void     T5Rc_UnlockRC (T5_PTR pRCMan, T5_DWORD hRC);
extern void     T5Rc_FreeRC (T5_PTR pRCMan, T5_DWORD hRC);
extern void     T5Rc_FreeAllRC (T5_PTR pRCMan);

/****************************************************************************/

#endif /*T5DEF_RC*/

#endif /*_T5RC_H_INCLUDED_*/

/* eof **********************************************************************/
