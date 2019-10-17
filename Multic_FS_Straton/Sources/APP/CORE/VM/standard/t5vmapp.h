/*****************************************************************************
T5VMapp.h :  VM app code - definitions

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#ifndef _T5VMAPP_H_INCLUDED_
#define _T5VMAPP_H_INCLUDED_

/****************************************************************************/
/* access to an offset in code */

#define T5_OFF(b,o)    (((T5_PTBYTE)(b))+(o))

/****************************************************************************/
/* main header */

typedef struct
{
    T5_DWORD    dwOne;
    T5_DWORD    dwProduct;
    T5_DWORD    dwApp;
    T5_DWORD    dwCFunc;
    T5_DWORD    dwFBC;
    T5_DWORD    dwTasks;
    T5_DWORD    dwXV;
    T5_DWORD    dwFBI;
    T5_DWORD    dwVarCount;
    T5_DWORD    dwInit;
    T5_DWORD    dwExtra;
    T5_DWORD    dwIO;
    T5_DWORD    dwDefProg;
    T5_DWORD    dwUnused4; /*dwProgType;*/
    T5_DWORD    dwCode;
    T5_DWORD    dwCmpVersion; /*compiler version - since 8.1 */
    T5_DWORD    dwPublish;
    T5_DWORD    dwUnused7; /*dwCRC;*/
}
T5STR_CODEHEADER;

typedef T5STR_CODEHEADER        *T5PTR_CODEHEADER;

/****************************************************************************/
/* code of programs */

typedef struct
{
    T5_DWORD    dwCode;
    T5_DWORD    dwSize;
    T5_WORD     wStyle;
    T5_WORD     res1;
}
T5STR_CODEPRG;

typedef T5STR_CODEPRG           *T5PTR_CODEPRG;
#define T5GET_CODEPRG(h)        ((T5PTR_CODEPRG)T5_OFF((h),(h)->dwCode))

/****************************************************************************/
/* product identification for compatibility check */

typedef struct
{
    T5_STRING16 szTarget;
    T5_STRING8  szVersion;
}
T5STR_CODEPRODUCT;

typedef T5STR_CODEPRODUCT       *T5PTR_CODEPRODUCT;
#define T5GET_CODEPRODUCT(h)    ((T5PTR_CODEPRODUCT)T5_OFF((h),(h)->dwProduct))

/****************************************************************************/
/* extras */

typedef struct
{
    /* version stamp */
    T5_DWORD dwVers;
    /* info available if version is >= 1 */
    T5_DWORD dwPassword;
    /* info available if version is >= 2 */
    T5_DWORD dwMBC;
    /* info available if version is >= 3 */
    T5_DWORD dwASI;
    /* info available if version is >= 4 */
    T5_DWORD dwEvents; /* { T5STR_EADEFVARH } null(2) */
    T5_DWORD dwEvtCnx; /* { T5STR_EADEFCNX } nul(2) */
    T5_DWORD dwEvtExt; /* { T5STR_EADEFVAR } null(2) */
    /* info available if version is >= 5 */
    T5_DWORD dwBusDrv; /* { T5STR_BUSDRV } null(4) */
    /* info available if version is >= 6 */
    T5_DWORD dwNbIOs;
    /* info available if version is >= 7 */
    T5_DWORD dwCTSeg;  /* CT segment definition - can be NULL! */
    /* info available if version is >= 8 */
    T5_DWORD dwCTTypes; /* complex types definition - can be NULL! */
    /* info available if version is >= 9 */
	T5_DWORD dwBusIOList;
    /* info available if version is >= 10 */
    T5_DWORD dwCCPure;  /* TRUE if no p-code available (only compiled) */
    /* info available if version is >= 11 */
    /* extra info in RETAIN cfg area */
    /* info available if version is >= 12 */
    T5_DWORD dwRecipes;
    /* info available if version is >= 13 */
    T5_DWORD dwVLst;
    /* info available if version is >= 14 */
    T5_DWORD dwStrTable;
    T5_DWORD dwSignal;
    /* info available if version is >= 15 */
    T5_DWORD dwCanBus;
    /* info available if version is >= 16 */
    T5_DWORD dwHeapSize; /* highest bit = malloc | 0 = no heap */
    /* info available if version is >= 17 */
    T5_DWORD dwAllocPOU; /* allocated space for POU list */
    /* info available if version is >= 18 */
    T5_DWORD dwIT;
    /* if version >= version 19 info : CTSEG + SmartLock enabled flag */
    T5_DWORD dwSMFlags;
    /* if version >= version 20 then IT count = 64 instead of 16 */
    /* if version >= version 20 then dwSMFlags is OR-masked with 0x02 when compiled in debug */
    /* info available if version is >= 22 */
    T5_DWORD dwCFBINames; /* list of C instance names */
    /* info available if version is >= 23 */
    T5_DWORD dwXRetain; /* list of named non volatile variables (custom) */

    /* ...and more in the future for bigger version number... */
}
T5STR_CODEEXTRA;

typedef T5STR_CODEEXTRA         *T5PTR_CODEEXTRA;
#define T5GET_CODEEXTRA(h)      ((T5PTR_CODEEXTRA)T5_OFF((h),(h)->dwExtra))

/****************************************************************************/
/* program list definition */

typedef struct
{
    T5_WORD     wNbProg;
    T5_WORD     wStackSize;
}
T5STR_CODEDEFPROG;

typedef T5STR_CODEDEFPROG       *T5PTR_CODEDEFPROG;
#define T5GET_CODEDEFPROG(h)    ((T5PTR_CODEDEFPROG)T5_OFF((h),(h)->dwDefProg))

/****************************************************************************/
/* application identification */

typedef struct
{
   T5_STRING16  szName;
   T5_DWORD     dwDateStamp;
   T5_DWORD     dwCrcData;
   T5_DWORD     dwTrigger;
   T5_DWORD     dwCodeSize;
   T5_WORD      wStartMode;
   T5_WORD      wVersion;
   T5_DWORD     dwFullBuild;
   T5_DWORD     dwSfcCrc;
} 
T5STR_CODEAPP;

typedef T5STR_CODEAPP           *T5PTR_CODEAPP;
#define T5GET_CODEAPP(h)        ((T5PTR_CODEAPP)T5_OFF((h),(h)->dwApp))

/****************************************************************************/
/* init values and var types */

#define T5_UOPT_NOIOSTEP    0x00000001L /* no IO exchange while stepping */
#define T5_UOPT_RTRIGNO0    0x00000002L /* no R_TRIG output on scan0 */
#define T5_UOPT_USHR        0x00000004L /* SHR is unsigned */

typedef struct
{
    T5_DWORD    dwInit8;
    T5_DWORD    dwInit16;
    T5_DWORD    dwInit32; 
    T5_DWORD    dwInit64; 
    T5_DWORD    dwInitTime;
    T5_DWORD    dwInitString;
    T5_DWORD    dwStringLength;
    T5_DWORD    dwVarTypes;
    T5_DWORD    res1;
    T5_DWORD    dwRetain;
    T5_DWORD    dwUserOptions;
    T5_WORD     res2;
    T5_WORD     res3;
}
T5STR_CODEINIT;

typedef T5STR_CODEINIT          *T5PTR_CODEINIT;
#define T5GET_CODEINIT(h)       ((T5PTR_CODEINIT)T5_OFF((h),(h)->dwInit))
#define T5GET_CODESTRLEN(h,ih)  ((T5_PTBYTE)T5_OFF((h),(ih)->dwStringLength))
#define T5GET_CODEINITSTR(h,ih) ((T5_PTWORD)T5_OFF((h),(ih)->dwInitString))
#define T5GET_CODEINIT8(h,ih)   ((T5_PTWORD)T5_OFF((h),(ih)->dwInit8))
#define T5GET_CODEINIT16(h,ih)  ((T5_PTWORD)T5_OFF((h),(ih)->dwInit16))
#define T5GET_CODEINIT32(h,ih)  ((T5_PTWORD)T5_OFF((h),(ih)->dwInit32))
#define T5GET_CODEINITTIME(h,ih) ((T5_PTWORD)T5_OFF((h),(ih)->dwInitTime))
#define T5GET_CODEINIT64(h,ih)  ((T5_PTWORD)T5_OFF((h),(ih)->dwInit64))

typedef struct
{
    T5_STRING64 szConf;
    T5_WORD     wNbD8;
    T5_WORD     wIndexD8;
    T5_WORD     wNbD32;
    T5_WORD     wIndexD32;
    T5_WORD     wNbTime;
    T5_WORD     wIndexTime;
    T5_WORD     wNbString;
    T5_WORD     wIndexString;
    T5_WORD     wNbD16;
    T5_WORD     wIndexD16;
    T5_WORD     wNbD64;
    T5_WORD     wIndexD64;
    T5_DWORD    dwCrc;
    /* following info available only if extra version mark is >= 11 */
    T5_DWORD    dwNbCT;
    T5_DWORD    dwIndexCT;
}
T5STR_CODERETAIN;

typedef T5STR_CODERETAIN         *T5PTR_CODERETAIN;
#define T5GET_CODERETAIN(h,ih)   ((T5PTR_CODERETAIN)T5_OFF((h),(ih)->dwRetain))

/****************************************************************************/
/* variable types */

#define T5GET_CODETYPES(h,ih)   ((T5_PTBYTE)T5_OFF((h),(ih)->dwVarTypes))

#define T5_D8SINT               0x01
#define T5_D32REAL              0x04
#define T5_D64LREAL             0x08

#define T5_ISD8SINT(tp,index)   ((((T5_PTBYTE)(tp))[index]&T5_D8SINT)!=0)
#define T5_ISD32REAL(tp,index)  ((((T5_PTBYTE)(tp))[index]&T5_D32REAL)!=0)
#define T5_ISD64LREAL(tp,index) ((((T5_PTBYTE)(tp))[index]&T5_D64LREAL)!=0)

/****************************************************************************/
/* number of variables */

typedef struct 
{
   T5_WORD wNbD8Alloc;
   T5_WORD wNbD8Used;
   T5_WORD wNbD8Fixed;
   T5_WORD wNbD8Inp;
   T5_WORD wNbD8Out;
   T5_WORD res1;

   T5_WORD wNbD16Alloc;
   T5_WORD wNbD16Used;
   T5_WORD wNbD16Fixed;
   T5_WORD wNbD16Inp;
   T5_WORD wNbD16Out;
   T5_WORD res2;

   T5_WORD wNbD32Alloc;
   T5_WORD wNbD32Used;
   T5_WORD wNbD32Fixed;
   T5_WORD wNbD32Inp;
   T5_WORD wNbD32Out;
   T5_WORD res3;

   T5_WORD wNbD64Alloc;
   T5_WORD wNbD64Used;
   T5_WORD wNbD64Fixed;
   T5_WORD wNbD64Inp;
   T5_WORD wNbD64Out;
   T5_WORD res4;

   T5_WORD wNbTimeAlloc;
   T5_WORD wNbTimeUsed;
   T5_WORD wNbTimeFixed;
   T5_WORD wNbTimeInp;
   T5_WORD wNbTimeOut;
   T5_WORD wNbTimeActive;

   T5_WORD wNbStringAlloc;
   T5_WORD wNbStringUsed;
   T5_WORD wNbStringFixed;
   T5_WORD wNbStringInp;
   T5_WORD wNbStringOut;
   T5_WORD res5;

   T5_WORD wNbStepAlloc;
   T5_WORD wNbStepUsed;
   T5_WORD res6;
   T5_WORD res7;

   T5_WORD wNbXVAlloc;
   T5_WORD wNbXVUsed;

   T5_WORD wNbTransAlloc;
   T5_WORD wNbTransUsed;
   T5_WORD res10;
   T5_WORD res11;

   T5_DWORD dwStringBuf;
} 
T5STR_CODEVARCOUNT;

typedef T5STR_CODEVARCOUNT      *T5PTR_CODEVARCOUNT;
#define T5GET_CODEVARCOUNT(h)   ((T5PTR_CODEVARCOUNT)T5_OFF((h),(h)->dwVarCount))

/****************************************************************************/
/* C FB classes */

typedef struct 
{
    T5_WORD     wID;
    T5_WORD     wNbInst;
    T5_STRING16 szName;
} 
T5STR_CODECLASS;

typedef T5STR_CODECLASS         *T5PTR_CODECLASS;

typedef struct 
{
    T5_WORD wNbAlloc;
    T5_WORD wNbUsed;
    T5STR_CODECLASS ccFirstClass; /* list */
} 
T5STR_CODEFBC;

typedef T5STR_CODEFBC           *T5PTR_CODEFBC;
#define T5GET_CODEFBC(h)        ((T5PTR_CODEFBC)T5_OFF((h),(h)->dwFBC))

/****************************************************************************/
/* FB instances */

typedef struct 
{
    T5_WORD  wIsStd;
    T5_WORD  wID;
    T5_DWORD dwCRC; /* based on class name */
} 
T5STR_CODEFBINST;    /* just after T5STR_CODEFBI header */

typedef T5STR_CODEFBINST        *T5PTR_CODEFBINST;

typedef struct 
{
    T5_WORD  wNbAlloc;
    T5_WORD  wNbFixed;
    T5_WORD  wNbUsed;
    T5_WORD  res1;
    T5_DWORD dwCrc;
    T5_DWORD dwRawSize;
    T5STR_CODEFBINST cfFirstInstance; /* list */
} 
T5STR_CODEFBI;

typedef T5STR_CODEFBI           *T5PTR_CODEFBI;
#define T5GET_CODEFBI(h)        ((T5PTR_CODEFBI)T5_OFF((h),(h)->dwFBI))

/****************************************************************************/
/* custom functions */

typedef struct 
{
    T5_WORD     wID;
    T5_WORD     res1;
    T5_STRING16 szName;
} 
T5STR_CODEFNCLASS;

typedef T5STR_CODEFNCLASS       *T5PTR_CODEFNCLASS;

typedef struct 
{
    T5_WORD wNbAlloc;
    T5_WORD wNbUsed;
    T5STR_CODEFNCLASS ccFirstClass; /* list */
} 
T5STR_CODEFNC;

typedef T5STR_CODEFNC           *T5PTR_CODEFNC;
#define T5GET_CODEFNC(h)        ((T5PTR_CODEFNC)T5_OFF((h),(h)->dwCFunc))

/****************************************************************************/
/* IOs */

typedef struct
{
    T5_WORD wSizeOf;
    T5_WORD wKey;
    T5_WORD wVA;
    T5_WORD wSlot;
    T5_WORD wSubSlot;
    T5_WORD wNbChan;
    /* continue with OEM defined parameters */
}
T5STR_CODEIO;

typedef T5STR_CODEIO            *T5PTR_CODEIO;
#define T5GET_CODEIO(h)         ((T5PTR_CODEIO)T5_OFF((h),(h)->dwIO))

/****************************************************************************/
/* published variables */

#define T5PUB_HASNAME       0x0001
#define T5PUB_HASNUMTAG     0x0002
#define T5PUB_HASPROPS      0x0004
#define T5PUB_HASPROFILE    0x0008
#define T5PUB_LOCALVAR      0x0010
#define T5PUB_HASCTSEG      0x0100
#define T5PUB_VSI           0x0200
#define T5PUB_FIRSTITEM     0x0400
#define T5PUB_NEXTITEM      0x0800
#define T5PUB_COUNT         0x8000

typedef struct              /* THE FIRST RECORD GIVES COUNT */
{
    T5_WORD wSizeOf;        /* must be 16 ! */
    T5_WORD wNbAlloc;       /* number of tags to be allocated */
    T5_WORD wNbUsed;        /* number of tags defined */
    T5_WORD wFlagsEx;       /* can be T5PUB_VSI */
    T5_DWORD dwCRC;         /* map CRC */
    T5_WORD res1;
    T5_WORD wFlags;         /* must be T5PUB_COUNT ! */
}
T5STR_CODEPUBCOUNT;

typedef T5STR_CODEPUBCOUNT      *T5PTR_CODEPUBCOUNT;
#define T5GET_CODEPUBCOUNT(h)   ((T5PTR_CODEPUBCOUNT)T5_OFF((h),(h)->dwPublish))

typedef struct
{
    T5_WORD wSizeOf;
    T5_WORD wTicType;   /* defined in t5tic.h */
    T5_WORD wIndex;
    T5_WORD wDim;
    T5_WORD wStringLength;
    T5_WORD wProps;     /* offset to properties */
    T5_WORD wNumTag;
    T5_WORD wFlags;
    /* continue with name (struct base + sizeof (T5STR_CODEPUB) */
    /* continue with properties (struct base + ->wProps) */
    /* Properties format:
        - CTSeg offset (if T5PUB_HASCTSEG)
        - TypeInfo offset (if T5PUB_HASCTSEG)
        - STRING16 ProfileName (if HASPROFILE)
        - OEM defined properties (T5PUB_HASPROPS)
    */
}
T5STR_CODEPUB;

typedef T5STR_CODEPUB           *T5PTR_CODEPUB;
#define T5GET_CODEPUB(h)        ((T5PTR_CODEPUB)T5_OFF((h),(h)->dwPublish))

/****************************************************************************/
/* External variables */

typedef struct
{
    T5_WORD wTicType;
    T5_WORD wIndex;
}
T5STR_CODEXV;

typedef T5STR_CODEXV            *T5PTR_CODEXV;
#define T5GET_CODEXV(h)         ((T5PTR_CODEXV)T5_OFF((h),(h)->dwXV))

/****************************************************************************/
/* Tasks lists */

/* offset 0:        (WORD)  full size of task description */
/*                          = 0 for the end of list of tasks */
/* offset 2:        (WORD)  OFFLIST = offset to list of programs */
/* offset 4:        (BYTES) task name (default cycle is the first) */
/* offset OFFLIST:  (WORDS) program indexes (0 terminated) */

#define T5TASK_DEFAULT          T5_ASCII("<CYCLE>") /* default idle cycle */
#define T5GET_CODETASK(h)       ((T5_PTWORD)T5_OFF((h),(h)->dwTasks))

/****************************************************************************/
/* BUS drivers */

#define T5BD_BUS        1       /* kind: main bus */
#define T5BD_MASTER     2       /* kind: master */
#define T5BD_SLAVE      3       /* kind: slave */
#define T5BD_VAR        4       /* kind: linked variable */

#define T5BD_PBUS       5       /* kind: main bus prop list */
#define T5BD_PMASTER    6       /* kind: master prop list */
#define T5BD_PSLAVE     7       /* kind: slave prop list */
#define T5BD_PVAR       8       /* kind: linked variable prop list */

#define T5BD_PINT       1       /* property type: integer */
#define T5BD_PFLOAT     2       /* property type: float */
#define T5BD_PSTRING    3       /* property type: string */
#define T5BD_PLSTRING   4       /* property type: null terminated long string */
#define T5BD_reserved5  5       /* should not be used */
#define T5BD_VARPROP    6       /* property is a variable */

#define T5BD_USA        0       /* english */
#define T5BD_GER        1       /* german */
#define T5BD_FRA        2       /* french */
#define T5BD_ITA        3       /* itatlian */
#define T5BD_SPA        4       /* spanish */

typedef struct                  /* directory of buses */
{
    T5_DWORD    dwDef;          /* offset to bus configuration - NULL term. */
    T5_STRING16 szDllPrefix;    /* prefix of handling DLL */
}
T5STR_CODEBUSDRV;

typedef T5STR_CODEBUSDRV        *T5PTR_CODEBUSDRV;

typedef struct                  /* pro list */
{
    T5_WORD wSizeof;            /* size of the record */
    T5_WORD wKind;              /* kind of record */
    /* {bKind - bSizeof} */
}
T5STR_BDP;

typedef T5STR_BDP               *T5PTR_BDP;

typedef struct                  /* bus main record */
{
    T5_WORD wSizeof;            /* size of the record */
    T5_WORD wKind;              /* kind of record */
    T5_WORD wVersion;           /* driver version number */
    T5_WORD wLge;               /* user language (USA...) */
    T5_DWORD dwCrcBus;          /* CRC for bus configuration */
    T5_DWORD dwCrcVar;          /* CRC for linked variables */
    /* params... */
}
T5STR_BDBUS;

typedef T5STR_BDBUS             *T5PTR_BDBUS;

typedef struct                  /* master record */
{
    T5_WORD wSizeof;            /* size of the record */
    T5_WORD wKind;              /* kind of record */
    T5_DWORD dwID;              /* master ID */
    /* params... */
}
T5STR_BDMASTER;

typedef T5STR_BDMASTER          *T5PTR_BDMASTER;

typedef struct                  /* slave record */
{
    T5_WORD wSizeof;            /* size of the record */
    T5_WORD wKind;              /* kind of record */
    T5_DWORD dwID;              /* slave ID */
    /* continue with params... */
}
T5STR_BDSLAVE;

typedef T5STR_BDSLAVE           *T5PTR_BDSLAVE;

typedef struct                  /* master record */
{
    T5_WORD wSizeof;            /* size of the record */
    T5_WORD wKind;              /* kind of record */
    T5_DWORD dwID;              /* variable link ID */
    T5_WORD  wType;             /* variable type */
    T5_WORD  wIndex;            /* variable index */
    T5_DWORD dwCTOffset;        /* offset in CT segment if not 0 */
    /* continue with params... */
}
T5STR_BDVAR;

typedef T5STR_BDVAR             *T5PTR_BDVAR;

/****************************************************************************/
/* BUS drivers - IO direction definition */

/* format of the embedded definition:
- nb of "unknown direction" IO points (DWORD)
- nb of "input" IO points (DWORD)
- nb of "output" IO points (DWORD)
- list of "unknown dir" IO points (T5STR_IOPOINT[])
- list of "input" IO points (T5STR_IOPOINT[])
- list of "output" IO points (T5STR_IOPOINT[])
*/

typedef struct              /* list entry: one IO point */
{
	T5_BYTE  bType;         /* data type - see below */
	T5_BYTE  res1;
	T5_WORD  wIndex;        /* index in table if single variables */
	T5_DWORD dwCTOffset;    /* if non zero: index in CT segment */
} T5STR_IOPOINT;            /* if zero: this is a single variable */

typedef T5STR_IOPOINT *T5PTR_IOPOINT;

/* possible values for type (you can find some macros defined in t5tic.h)
1 = BOOL (boolean value on 1 byte)
2 = DINT (32 bit signed integer)
3 = REAL (32 bit float)
4 = TIME (32 bit integer)
10 = SINT (8 bit signed integer)
11 = INT (16 bit signed integer)
12 = LINT (64 bit signed integer)
13 = LREAL (64 bit float)
14 = USINT (8 bit unsigned integer)
15 = UINT (16 bit unsigned integer)
16 = UDINT (32 bit unsigned integer)
17 = ULINT (64 bit unsigned integer)
*/

/****************************************************************************/
/* complex type definition */

#define T5TYPEDEF_BASE          1   /* base */
#define T5TYPEDEF_ARRAY1        2   /* array 1 dim */
#define T5TYPEDEF_ARRAY2        3   /* array 2 dims */
#define T5TYPEDEF_ARRAY3        4   /* array 3 dims */
#define T5TYPEDEF_STRUCT        5   /* structure */
#define T5TYPEDEF_MEMBER        6   /* struct member */
#define T5TYPEDEF_CFB           7   /* C FB instance */

/* type definition raw list
   starts with total definition raw size on a DWORD
   stops with bKind=0

   ========================================================================================
   | WARNING: ALL ITEMS ARE PACKED! ALWAYS USE MEMCPY FOR SAFE ACCESS TO WORDS AND DWORDS |
   ========================================================================================

BASE:
   T5_CHAR  szNull= 0; // no name for base types
   T5_BYTE  bKind;
   T5_BYTE  bTicType;
   T5_BYTE  bStringLength

ARRAY1:
   T5_CHAR  szName[];
   T5_BYTE  bKind;
   T5_WORD  wBaseTypeIndex;
   T5_DWORD wSizeofElt;
   T5_DWORD dwDim;

ARRAY2:
   T5_CHAR  szName[];
   T5_BYTE  bKind;
   T5_WORD  wBaseTypeIndex;
   T5_DWORD wSizeofElt;
   T5_DWORD dwDimLow;
   T5_DWORD dwDimHigh;

ARRAY3:
   T5_CHAR  szName[];
   T5_BYTE  bKind;
   T5_WORD  wBaseTypeIndex;
   T5_DWORD wSizeofElt;
   T5_DWORD dwDimLow;
   T5_DWORD dwDimMedium;
   T5_DWORD dwDimHigh;

STRUCT:
   T5_CHAR  szName[];
   T5_BYTE  bKind;
   T5_WORD  wNbMember;
   T5_WORD  wFirstMemberIndex;

MEMBER:
   T5_CHAR  szName[];
   T5_BYTE  bKind;
   T5_WORD  wBaseTypeIndex;
   T5_DWORD dwOffset;

CFB:
   T5_CHAR  szName[];
   T5_BYTE  bKind;
   T5_WORD  wNbMember;
   T5_WORD  wFirstMemberIndex;

*/

/****************************************************************************/
/* recipes */

typedef struct                  /* list of recipes */
{
    T5_WORD  wVersion;          /* version mark */
    T5_WORD  wCount;            /* number of embedded recipes */
    T5_DWORD dwRcp1;            /* code offset to the first recipe */
    /* ...other offsets... */
}
T5STR_CODERCPL;

typedef T5STR_CODERCPL          *T5PTR_CODERCPL;
#define T5GET_CODERCPL(h)       ((T5PTR_CODERCPL)T5_OFF((h),(h)->dwRecipes))

typedef struct                  /* recipe */
{
    T5_WORD  wNbVar;            /* number of variables */
    T5_WORD  wNbCol;            /* number of columns */
    T5_DWORD dwVar1;            /* code offset to the first variable */
    /* ...other offsets... */
}
T5STR_CODERCP;

typedef T5STR_CODERCP           *T5PTR_CODERCP;

typedef struct                  /* variable - recipe raw */
{
    T5_WORD  wType;             /* data type */
    T5_WORD  wOffset;           /* variable offset (or 0 if CTSeg) */
    T5_DWORD dwCTOffset;        /* offset in CTSeg segment */
    T5_BYTE  bValue[4];         /* list of values - variable length */
}
T5STR_CODERCPV;

typedef T5STR_CODERCPV          *T5PTR_CODERCPV;

/****************************************************************************/
/* lists of variables */

typedef struct                  /* list of lists */
{
    T5_WORD  wVersion;          /* version mark */
    T5_WORD  wCount;            /* number of embedded list */
    T5_DWORD dwVLst1;           /* code offset to the first list */
    /* ...other offsets... */
}
T5STR_CODEVLSTL;

typedef T5STR_CODEVLSTL         *T5PTR_CODEVLSTL;
#define T5GET_CODEVLSTL(h)      ((T5PTR_CODEVLSTL)T5_OFF((h),(h)->dwVLst))

typedef struct                  /* list of variables */
{
    T5_WORD  wNbVar;            /* number of variables */
    T5_WORD  res1;              /* reserved */
    T5_DWORD res2;              /* reserved */
    /* continue with variables */
}
T5STR_CODEVLST;

typedef T5STR_CODEVLST          *T5PTR_CODEVLST;

typedef struct                  /* variable of a list */
{
    T5_WORD  wType;             /* data type */
    T5_WORD  wOffset;           /* variable offset (or 0 if CTSeg) */
    T5_DWORD dwCTOffset;        /* offset in CTSeg segment */
}
T5STR_CODEVLSTV;

typedef T5STR_CODEVLSTV         *T5PTR_CODEVLSTV;

/****************************************************************************/
/* CAN bus configuration */

#ifdef T5DEF_CANBUS

typedef struct                  /* CAN configuration main entry point */
{
    T5_WORD     wVersion;       /* config version number */
    T5_WORD     wNbPort;        /* number of CAN ports */
    T5_WORD     wNbMsgIn;       /* number of received messages */
    T5_WORD     wNbMsgOut;      /* number of sent messages */
    T5_WORD     wNbVar;         /* number of variables */
    T5_WORD     wFifoSize;      /* app FIFO size (2 for each port) */
    T5_DWORD    dwPortOffset;   /* offset to the 1rst port */
    T5_DWORD    dwInMsgOffset;  /* offset to the 1rst rcv message */
    T5_DWORD    dwOutMsgOffset; /* offset to the 1rst snd message */
    T5_DWORD    dwVarOffset;    /* offset to the 1rst variable */
    /* info available if version is >= 2 */
    T5_DWORD    dwScaleOffset;  /* offset to the 1rst var scaling */
    /* info available if version is >= 3 */
    T5_DWORD    dwCanJFPorts;   /* offset to J1939 port names */
    T5_DWORD    dwCanJF;        /* offset to J1939 route table */
}
T5STR_CANDEF;

typedef T5STR_CANDEF            *T5PTR_CANDEF;

#define T5CANPORT_2A        0   /* CAN 2.0A */
#define T5CANPORT_2B        1   /* CAN 2.0B */

#define T5CANPORT_CO        0x0001  /* CANopen port */
#define T5CANPORT_J1939     0x0002  /* J1939 port */

typedef struct                  /* CAN configuration main entry point */
{
    T5_WORD     wVersion;       /* port config version number */
    T5_WORD     wNbMsgIn;       /* number of RCV messages */
    T5_WORD     wNbMsgOut;      /* number of SND messages */
    T5_WORD     wMsgIn;         /* index of the 1rst RCV message */
    T5_WORD     wMsgOut;        /* index of the 1rst SND message */
    T5_WORD     wType;          /* type of CAN network */
    T5_WORD     wRate;          /* Baud rate (KB) */
    T5_WORD     wFlags;         /* special settings */
    T5_CHAR     szSettings[63]; /* port settings */
    T5_BYTE     bECU;           /* J1939 ECU number */
}
T5STR_CANPORTDEF;

typedef T5STR_CANPORTDEF        *T5PTR_CANPORTDEF;

#define T5CANMSG_RCV        0   /* mode: received */
#define T5CANMSG_PERIO      1   /* mode: sent periodically */
#define T5CANMSG_SHOT       2   /* mode: sent on request (on short) */
#define T5CANMSG_ONCHANGE   3   /* mode: sent on change of value */

#define T5CANMSG_GETMODE(m)     ((m) & 0x0f)

#define T5CANMSG_HASDATA  0x80  /* mode OR-flag: init data specified */
#define T5CANMSG_RTR      0x40  /* mode OR-flag: RTR message */
#define T5CANMSG_SAMASK   0x20  /* J1939: mask received SA */
#define T5CANMSG_PRIOMASK 0x10  /* J1939: mask received priority */

typedef struct                  /* CAN Message */
{
    T5_DWORD    dwID;           /* CAN identifier */
    T5_DWORD    dwTMin;         /* min period or J1939 period */
    T5_DWORD    dwTMax;         /* max period or J1939 delay or J1939 timeout */
    T5_WORD     wPort;          /* parent port index */
    T5_BYTE     bLen;           /* length */
    T5_BYTE     bMode;          /* exchange mode */
 /* T5_BYTE     data[8]            only if (mode & T5CANMSG_HASDATA) */
}
T5STR_CANMSGDEF;

typedef T5STR_CANMSGDEF         *T5PTR_CANMSGDEF;

#define T5CANOPE_IN     0x01    /* operation: received data */
#define T5CANOPE_OUT    0x02    /* operation: sent data */
#define T5CANOPE_BSIN   0x11    /* operation: received bit string */
#define T5CANOPE_BSOUT  0x12    /* operation: sent bit string */
#define T5CANOPE_SPNIN  0x21    /* operation: received bit string */
#define T5CANOPE_SPNOUT 0x22    /* operation: sent bit string */
#define T5CANOPE_WARN   0x81    /* diag operation: warning counter */
#define T5CANOPE_BUSOFF 0x82    /* diag operation: bus off */
#define T5CANOPE_RCV    0x83    /* diag operation: msg received */
#define T5CANOPE_RCVGLO 0x84    /* diag operation: rcv global flag */
#define T5CANOPE_CMD    0x85    /* diag operation: snd activation */
#define T5CANOPE_SPSTAT 0x86    /* diag operation: specific in */
#define T5CANOPE_SPCMD  0x87    /* diag operation: specific out */
#define T5CANOPE_SETLEN 0x88    /* diag operation: set message length */

/* NOTE: bSize is in bits for T5CANOPE_BSIN / T5CANOPE_BSOUT */

typedef struct                  /* CAN Variable */
{
    T5_WORD     wMsg;           /* index of the message */
    T5_BYTE     bOffset;        /* offset in the message */
    T5_BYTE     bOpe;           /* operation and format */
    T5_BYTE     bMask;          /* bit mask */
    T5_BYTE     bSize;          /* data size in message */
    T5_BYTE     bFormat;        /* TIC type in message */
    T5_BYTE     bEndian;        /* 1=big endian / 0=little endian */
    T5_BYTE     bOutMsg;        /* 1=send message / 0=received message */
    T5_BYTE     bType;          /* TIC type */
    T5_WORD     wIndex;         /* index in VMDB */
    T5_DWORD    dwCTOffset;     /* offset in CTSEG - if wIndex=0 */
}
T5STR_CANVARDEF;

typedef T5STR_CANVARDEF         *T5PTR_CANVARDEF;

#ifdef T5DEF_REALSUPPORTED

typedef struct                  /* CAN Variable */
{
    T5_REAL     rFactor;        /* scaling factor */
    T5_REAL     rOffset;        /* scaling offset */
}
T5STR_CANVARSCALE;

typedef T5STR_CANVARSCALE       *T5PTR_CANVARSCALE;

#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_CANJF

#define T5CANJF_REPEAT          0x80    /* flag: port repeater */
#define T5CANJF_BRIDGE          0x40    /* flag: message bridge */
#define T5CANJF_ROUTE           0x20    /* flag: address routing */
#define T5CANJF_SAMASK          0x01    /* SA mask */
#define T5CANJF_PRIOMASK        0x02    /* priority mask */
#define T5CANJF_END             0x00    /* end of list */

typedef struct                  /* J1939 forwarding */
{
    T5_BYTE     bFlags;         /* flags */
    T5_BYTE     bSrc;           /* source port */
    T5_BYTE     bDest;          /* destination ports (bits) */
    T5_BYTE     bSA;            /* routing subst SA */
    T5_DWORD    dwID;           /* message ID */
}
T5STR_CANJF;

typedef T5STR_CANJF             *T5PTR_CANJF;

#endif /*T5DEF_CANJF*/

#endif /*T5DEF_CANBUS*/

/****************************************************************************/

#endif /*_T5VMAPP_H_INCLUDED_*/

/* eof **********************************************************************/

