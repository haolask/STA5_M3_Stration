/*****************************************************************************
T5VMdb.h :   VM data base - definitions

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#ifndef _T5VMDB_H_INCLUDED_
#define _T5VMDB_H_INCLUDED_

/****************************************************************************/
/* DB format version number */

#define K5DBVERS        0x8000026L

/****************************************************************************/
/* hooks for diagnostic counters */

#ifndef T5_DIAGHOOK
#define T5_DIAGHOOK /* nothing */
#endif /*T5_DIAGHOOK*/

#define T5_DIAGHOOK_USER                0 /* 0 to ffff reserved to OEM */

#define T5_DIAGHOOK_BIND_PROD           0x00010001
#define T5_DIAGHOOK_BIND_RCV            0x00010002

#define T5_DIAGHOOK_CYCLE_BEGIN         0x00020001
#define T5_DIAGHOOK_CYCLE_BINDCONS      0x00020002
#define T5_DIAGHOOK_CYCLE_IOS           0x00020003
#define T5_DIAGHOOK_CYCLE_MODBUS        0x00020004
#define T5_DIAGHOOK_CYCLE_PROGS         0x00020005
#define T5_DIAGHOOK_CYCLE_BINDPROD      0x00020006

#define T5_DIAGHOOK_DIVZERO             0x00030001
#define T5_DIAGHOOK_BADINDEX            0x00030002

/****************************************************************************/
/* calling convention for all function handlers */

#ifndef T5HDNSPEC
#define T5HDNSPEC /* nothing */
#endif

/****************************************************************************/
/* macros for declaring and exploring libraries */

#define T5HLIB_BEGIN    bContinue = TRUE;
#define T5HLIB_FB(s,p)  if (bContinue) \
                        _T5HLIB_Handler ((T5_PTCHAR)(s), (p), wMode, pData, &bContinue);
#define T5HLIB_END      ;

/****************************************************************************/
/* constants */

#define T5_24HMS        86400000L   /* 24h - nb of millisecons */

#ifndef T5_MAXSINT
#define T5_MAXSINT      0x7f
#endif /*T5_MAXSINT*/

#ifndef T5_MAXINT
#define T5_MAXINT       0x7fff
#endif /*T5_MAXINT*/

#ifndef T5_MAXDINT
#define T5_MAXDINT      0x7fffffffL
#endif /*T5_MAXDINT*/

#ifdef T5DEF_LINTSUPPORTED

#ifndef T5_MAXLINT
#define T5_MAXLINT      0x7fffffffffffffffLL
#endif /*T5_MAXLINT*/

#ifndef T5_MAXULINT
#define T5_MAXULINT      0xffffffffffffffffULL
#endif /*T5_MAXULINT*/

#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_REALSUPPORTED

#ifndef T5_MAXREAL
#define T5_MAXREAL      1.0e37F
#endif /*T5_MAXREAL*/

#ifndef T5_MINREAL
#define T5_MINREAL      1.0e-37F
#endif /*T5_MINREAL*/

#ifndef T5_ZEROREAL
#define T5_ZEROREAL     0.0F
#endif /*T5_ZEROREAL*/

#ifndef T5_ONEREAL
#define T5_ONEREAL      1.0F
#endif /*T5_ONEREAL*/

#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_LREALSUPPORTED

#ifndef T5_MAXLREAL
#define T5_MAXLREAL     1.0e307
#endif /*T5_MAXLREAL*/

#ifndef T5_MINLREAL
#define T5_MINLREAL     1.0e-307
#endif /*T5_MINLREAL*/

#ifndef T5_ZEROLREAL
#define T5_ZEROLREAL    0.0
#endif /*T5_ZEROLREAL*/

#ifndef T5_ONELREAL
#define T5_ONELREAL     1.0
#endif /*T5_ONELREAL*/

#else /*T5DEF_LREALSUPPORTED*/

#ifndef T5_MAXLREAL
#define T5_MAXLREAL     1.0e37F
#endif /*T5_MAXLREAL*/

#ifndef T5_MINLREAL
#define T5_MINLREAL     1.0e-37F
#endif /*T5_MINLREAL*/

#ifndef T5_ZEROLREAL
#define T5_ZEROLREAL    0.0F
#endif /*T5_ZEROLREAL*/

#ifndef T5_ONELREAL
#define T5_ONELREAL     1.0F
#endif /*T5_ONELREAL*/

#endif /*T5DEF_LREALSUPPORTED*/

#ifdef T5DEF_ULINTSUPPORTED

#ifndef T5_ULL80
#define T5_ULL80    0x8000000000000000ULL
#endif /*T5_ULL80*/

#ifndef T5_ULLFE
#define T5_ULLFE    0xfffffffffffffffeULL
#endif /*T5_ULLFE*/

#ifndef T5_ULL7F
#define T5_ULL7F    0x7fffffffffffffffULL
#endif /*T5_ULL7F*/

#endif /*T5DEF_ULINTSUPPORTED*/

/****************************************************************************/
/* helpers */

#define T5_BOUNDTIME(t) ((T5_24HMS+(t))%T5_24HMS)
#define T5_TOXC         0xdbba0L

typedef union /* for aligned data */
{
    T5_BYTE b[8];
    T5_DWORD d[2];
#ifdef T5DEF_DATA64SUPPORTED
    T5_DATA64 l;
#endif /*T5DEF_DATA64SUPPORTED*/
}
T5_NUMBUFFER;

#ifndef T5_UNUSED
#define T5_UNUSED(v) (void)(v)
#endif /*T5_UNUSED*/

/* safe copy from a byte tream - "p" MUST BE T5_PTBYTE! - p is moved after the copy */

#define T5_SFGETBYTE(p, byValue) \
    { T5_MEMCPY(&byValue, p , sizeof(T5_BYTE)); p+=sizeof(T5_BYTE); }
#define T5_SFGETWORD(p, wValue) \
    { T5_MEMCPY(&wValue, p , sizeof(T5_WORD)); p+=sizeof(T5_WORD); }
#define T5_SFGETDWORD(p, dwValue) \
    { T5_MEMCPY(&dwValue, p , sizeof(T5_DWORD)); p+=sizeof(T5_DWORD); }
#define T5_SFGETLWORD(p, lwValue) \
    { T5_MEMCPY(&lwValue, p , 8); p+=8; }
#define T5_SFGETSTR8(p, pstrValue) \
    { T5_MEMCPY((T5_PTBYTE)pstrValue, p , 8); p+=8; }
#define T5_SFGETSTR16(p, pstrValue) \
    { T5_MEMCPY((T5_PTBYTE)pstrValue, p , 16); p+=16; }
#define T5_SFGETSTR32(p, pstrValue) \
    { T5_MEMCPY((T5_PTBYTE)pstrValue, p , 32); p+=32; }
#define T5_SFGETSTR64(p, pstrValue) \
    { T5_MEMCPY((T5_PTBYTE)pstrValue, p , 64); p+=64; }
#define T5_SFGETSTR0(p, pstrValue) \
    { T5_STRCPY((T5_PTBYTE)pstrValue, p); while (*p) { p++; } p++; }

/****************************************************************************/
/* RT database */

#define T5TB_STATUS     0           /* main application status and control */
#define T5TB_LOG        1           /* stack of log messages */
#define T5TB_EXEC       2           /* tic code from comm */
#define T5TB_PRIVATE    3           /* global data private to the VM */
#define T5TB_PROG       4           /* programs */
#define T5TB_LOCK       5           /* 8 bit data: lock flags */
#define T5TB_DATA8      6           /* 8 bit data: bool, sint */
#define T5TB_DATA16     7           /* 8 bit data: bool, sint */
#define T5TB_DATA32     8           /* 32 bit data: dint, real */
#define T5TB_DATA64     9           /* 64 bit data: lint, lreal */
#define T5TB_TIME       10          /* timers */
#define T5TB_ACTIME     11          /* active timers */
#define T5TB_FBCLASS    12          /* C or std function block classes */
#define T5TB_FBINST     13          /* C or std function block instance */
#define T5TB_FBCDATA    14          /* data of FB classes */
#define T5TB_FBIDATA    15          /* data of FB instance */
#define T5TB_FCLASS     16          /* custom function classes */
#define T5TB_STRING     17          /* character strings */
#define T5TB_STRBUF     18          /* string buffers */
#define T5TB_SYSRSC     19          /* system resources */
#define T5TB_STEP       20          /* SFC steps */
#define T5TB_TRANS      21          /* SFC transitions */
#define T5TB_UNRES      22          /* unresolved resources */
#define T5TB_IODATA     23          /* private memory for IO devices */
#define T5TB_VARMAP     24          /* embedded variable map */
#define T5TB_HASH       25          /* embedded variable map - hash tables */
#define T5TB_XV         26          /* external variables */
#define T5TB_BSAMPLING  27          /* digital sampling trace */
#define T5TB_TASK       28          /* tasks lists */
#define T5TB_CALLSTACK  29          /* sub program call stack */
#define T5TB_MBC        30          /* MODBUS Client */
#define T5TB_ASI        31          /* ASi I/Os */
#define T5TB_EACHANGE   32          /* EA change events */
#define T5TB_EACNX      33          /* EA external connections */
#define T5TB_EAEXT      34          /* EA external events (extern vars) */
#define T5TB_CTSEG      35          /* CT variable segment */
#define T5TB_CANPORT    36          /* CAN bus driver - ports */
#define T5TB_CANMSG     37          /* CAN bus driver - messages */
#define T5TB_CANVAR     38          /* CAN bus driver - messages */
#define T5TB_DDKC       39          /* DDK C data */

#define T5MAX_TABLE     40          /* number of tables in the database */

typedef struct                      /* entry of a table in the data base */
{
    T5_PTR      pData;      /*PTR*/ /* pointer to the table */
    T5_DWORD    dwRawSize;          /* size of the buffer for raw data */
    T5_WORD     wSizeOf;            /* size of an item (1 for raw data) */
    T5_WORD     wNbItemFixed;       /* number of items that cannot be moved */
    T5_WORD     wNbItemUsed;        /* number of items used in the table */
    T5_WORD     wNbItemAlloc;       /* total nb of items allocated in mem. */
} T5STR_DBTABLE;                    /* wNbItem.. members are 0 for raw data */

typedef T5STR_DBTABLE * T5PTR_DB;

#define T5GET_DBLOCK(h)             ((T5_PTBYTE)((h)[T5TB_LOCK].pData))
#define T5GET_DBDATA8(h)            ((T5_PTBYTE)((h)[T5TB_DATA8].pData))
#define T5GET_DBDATA16(h)           ((T5_PTWORD)((h)[T5TB_DATA16].pData))
#define T5GET_DBDATA32(h)           ((T5_PTDWORD)((h)[T5TB_DATA32].pData))
#define T5GET_DBDATA64(h)           ((T5_PTDATA64)((h)[T5TB_DATA64].pData))
#define T5GET_DBTIME(h)             ((T5_PTDWORD)((h)[T5TB_TIME].pData))
#define T5GET_DBFBCDATA(h)          ((T5_PTBYTE)((h)[T5TB_FBCDATA].pData))
#define T5GET_DBFBIDATA(h)          ((T5_PTBYTE)((h)[T5TB_FBIDATA].pData))
#define T5GET_DBSTRING(h)           ((T5_PTBYTE *)((h)[T5TB_STRING].pData))
#define T5GET_DBSTRBUF(h)           ((T5_PTBYTE)((h)[T5TB_STRBUF].pData))
#define T5GET_DBIODATA(h)           ((T5_PTBYTE)((h)[T5TB_IODATA].pData))
#define T5GET_DBXV(h)               ((T5_PTBYTE *)((h)[T5TB_XV].pData))
#define T5GET_MBC(h)                ((h)[T5TB_MBC].pData)

#define T5GET_ASI(h)                ((T5PTR_ASI)((h)[T5TB_ASI].pData))
#define T5GET_EACHANGE(h)           ((T5PTR_EACHANGE)((h)[T5TB_EACHANGE].pData))
#define T5GET_EACNX(h)              ((T5PTR_EACNX)((h)[T5TB_EACNX].pData))
#define T5GET_EAEXT(h)              ((T5PTR_EAEXT)((h)[T5TB_EAEXT].pData))

/****************************************************************************/
/* lock flags */

#define T5LOCK_DATA8    0x01
#define T5LOCK_DATA16   0x02
#define T5LOCK_DATA32   0x04
#define T5LOCK_DATA64   0x08
#define T5LOCK_TIME     0x10
#define T5LOCK_STRING   0x20
#define T5LOCK_XV       0x80

/****************************************************************************/
/* T5TB_CALLSTACK: sub-program call stack */

typedef struct                      /* one calling position */
{
    T5_WORD     wProg;              /* index of the calling program */
    T5_WORD     wPos;               /* offset in the code */
} T5STR_DBCALLSTACK;

typedef T5STR_DBCALLSTACK    *T5PTR_DBCALLSTACK;
#define T5GET_DBCALLSTACK(h) ((T5PTR_DBCALLSTACK)((h)[T5TB_CALLSTACK].pData))

/****************************************************************************/
/* active breakpoints */

#define T5BKP_DELETED       0       /* style: simple user breakpoint */
#define T5BKP_SIMPLE        1       /* style: simple user breakpoint */
#define T5BKP_ONESHOT       2       /* style: one shot (for WB) */
#define T5BKP_VIEW          3       /* style: view point */

typedef struct                      /* one calling position */
{
    T5_WORD wProg;                  /* index of the calling program */
    T5_WORD wPos;                   /* offset in the code */
    T5_WORD wStyle;                 /* breakpoint style */
    T5_WORD wFlags;                 /* reserved */
} T5STR_BKP;

typedef T5STR_BKP *T5PTR_BKP;

/****************************************************************************/
/* T5TB_STATUS: VM status and control */

#define T5FLAG_HASCT        0x1000      /* CT segment exists */
#define T5FLAG_BKP          0x2000      /* active breakpoints installed */
#define T5FLAG_HNDMISS      0x4000      /* cant start - missing handlers */
#define T5FLAG_APPHERE      0x8000      /* application is loaded */

#define T5FLAG_ONECYCLE     0x0100      /* single cycle mode */
#define T5FLAG_EAFREEZE     0x0200      /* freeze event production */
#define T5FLAG_SYSINFO      0x0400      /* sysinfo request is available */
#define T5FLAG_REDPSV       0x0800      /* reserved for redundancy networking */

#define T5FLAG_RUN          0x0001      /* application is running */
#define T5FLAG_ERROR        0x0002      /* application stopped on error */
#define T5FLAG_SFCBREAK     0x0004      /* application stopped on SFC breakpt */
#define T5FLAG_PROGSTEP     0x0008      /* application stopped between 2 progs */

#define T5FLAG_RUNMASK      0x000f      /* application stopped between 2 progs */

#define T5FLAG_LOG          0x0010      /* log message(s) in stack */
#define T5FLAG_DEGREES      0x0020      /* trigo functions are in degrees */
#define T5FLAG_VLOCK        0x0040      /* locked variables */
#define T5FLAG_CCHANGE      0x0080      /* accept on line change with CC code */

#define T5FLAGEX_CCAVL      0x00000001  /* compiled code available */
#define T5FLAGEX_CCACT      0x00000002  /* compiled code active */
#define T5FLAGEX_PCAVL      0x00000004  /* p-code available */
#define T5FLAGEX_NOSIO      0x00000008  /* no IO exchange while stepping */
#define T5FLAGEX_CHGCYC     0x00000010  /* cycle just after an On Line Change */
#define T5FLAGEX_CTSM       0x00000020  /* Smart Lock table used for CTSeg */
#define T5FLAGEX_WARM       0x00000040  /* application started in warm start */
#define T5FLAGEX_RTRIGNO0   0x00000080  /* application started in warm start */
#define T5FLAGEX_DEBUG      0x00000100  /* application compiled in debug mode */
#define T5FLAGEX_USHR       0x00000200  /* run SHR as unsigned */
#define T5FLAGEX_NOFILEFUNC 0x00000400  /* Unsupported file function called */

#define T5FLAGEX_CAPS       0x80000000  /* reserved for communication */

#define T5MAX_BKP           16          /* max number of active breakpoints */

#define T5MAX_EVENT         64          /* max number of events */

#ifndef T5_OEMSTATUS
#define T5_OEMSTATUS /* nothing */
#endif /*T5_OEMSTATUS*/

typedef struct                      /* WM status and control */
{
    /* application */
    T5_STRING16 szAppName;          /* application name */
    /* timing */                    /* DONT CHANGE THE ORDER OF TIMERS! */
    T5_DWORD    dwTimTrigger;       /* trigger cycle time in microsec */
    T5_DWORD    dwTimCycle;         /* last cycle duration in microsec */
    T5_DWORD    dwTimMax;           /* maximum cycle duration in microsec */
    T5_DWORD    dwTimStamp;         /* cycle begin time stamp in ms */
    T5_DWORD    dwOverflow;         /* number of cycle overflows */
    T5_DWORD    dwCycleCount;       /* number of cycles performed */
    /* versioning */
    T5_DWORD    dwAppVersion;       /* application version number */
    T5_DWORD    dwAppDateStamp;     /* application date stamp */
    T5_DWORD    dwAppCodeCRC;       /* application CRC (the code) */
    T5_DWORD    dwAppDataCRC;       /* application CRC (data map) */
    T5_DWORD    dwCmpVersion;       /* compiler version */
    /* status */
    T5_WORD     wFlags;             /* application status */
    T5_WORD     wCurPrg;            /* stepping: current program */
    T5_WORD     wCurPos;            /* stepping: current code position */
    T5_WORD     wCurTag;            /* stepping: current code tag */
    /* log */
    T5_WORD     wNbLog;             /* number of stacked messages */
    T5_WORD     wNbUnres;           /* number of stacked unresolved */
    /* exec buffer */
    T5_WORD     wExecSize;          /* size of exec buffer */
    /* active timers */
    T5_WORD     wNbActime;          /* number of running active timers */
    /* actite breakpoints */
    T5STR_BKP   bkp[T5MAX_BKP];     /* possible active breakpoints */
    /* OEM defined info */
    T5_OEMSTATUS
    /* extensions */
    T5_DWORD    dwFlagsEx;          /* extended flags */
    T5_DWORD    dwID;               /* app code ID */
    T5_DWORD    dwFirstStamp;       /* time stamp of 1rst cycle */
    T5_DWORD    dwNbLock;           /* number of locked variables */
    T5_WORD     wMaxCallStack;      /* max call stack depth */
    /* CSV options */
    T5_CHAR     cCsvSep;            /* CSV separator */
    T5_CHAR     cCsvDec;            /* CSV decimal point */
    /* XS buffers */
    T5_DWORD    dwNbXS;             /* allocated XS buffers */
    T5_DWORD    dwErrorXS;          /* XS last error */
    /* XML buffers */
    T5_DWORD    dwNbXML;            /* allocated XML tags */
    T5_DWORD    dwErrorXML;         /* XML last error */
    /* time elapsed since start */
    T5_DWORD    dwElapsed;          /* number of seconds elapsed */
    T5_DWORD    dwElapsedMS;        /* rest: milliseconds elapsed */
    /* events */
#ifdef T5DEF_IT
    T5_BOOL     bRunning;
    T5_BOOL     bSignalEvent;
    T5_BYTE     bEvStackPtr;
    T5_BYTE     res1;
    T5_BYTE     bEvStack[T5MAX_EVENT];
    T5_BYTE     bEvSig[T5MAX_EVENT];
#endif /*T5DEF_IT*/
} T5STR_DBSTATUS;

typedef T5STR_DBSTATUS      *T5PTR_DBSTATUS;
#define T5GET_DBSTATUS(h)   ((T5PTR_DBSTATUS)((h)[T5TB_STATUS].pData))

#define T5STAT_CHGCYC(s)    ((((s)->dwFlagsEx & T5FLAGEX_CHGCYC) != 0L) ? 1 : 0)

/****************************************************************************/
/* T5TB_LOG: stack of log messages and smart lock table */

typedef struct                      /* one log message */
{
    T5_WORD     wCode;              /* message ID */
    T5_WORD     wArg;               /* main message argument */
    T5_DWORD    dwData;             /* auxiliary message argument */
} T5STR_DBLOG;

typedef T5STR_DBLOG         *T5PTR_DBLOG;
#define T5GET_DBLOG(h)      ((T5PTR_DBLOG)((h)[T5TB_LOG].pData))
#define T5DBLOG_RAWSIZE     (T5MAX_LOG * sizeof (T5STR_DBLOG))

#ifndef T5MAX_SMARTLOCK
#define T5MAX_SMARTLOCK 0
#endif /*T5MAX_SMARTLOCK*/

#define T5SML_CT            0x01
#define T5SML_REAPPLY       0x02

typedef struct                      /* one locked variable */
{
    T5_WORD     wSizeof;            /* total size of record */
    T5_BYTE     bType;              /* data type */
    T5_BYTE     bFlags;             /* flags: CT/REAPPLY */
    T5_DWORD    dwIndex;            /* index or CTSeg offset */
    /* followed by data */
} T5STR_SMLOCK;

typedef T5STR_SMLOCK        *T5PTR_SMLOCK;
#define T5GET_SMLOCK(h)     (T5PTR_SMLOCK)((T5_PTBYTE)((h)[T5TB_LOG].pData)+T5DBLOG_RAWSIZE)

/****************************************************************************/
/* T5TB_UNRES: stack of unresolved resources */

typedef struct
{
    T5_STRING16 szName;             /* unresolved handler name */
} T5STR_DBUNRES;

typedef T5STR_DBUNRES       *T5PTR_DBUNRES;
#define T5GET_DBUNRES(h)    ((T5PTR_DBUNRES)((h)[T5TB_UNRES].pData))

/****************************************************************************/
/* T5TB_PRIVATE: VM private data */

#define T5TYPEMASK_SINT     0x01    /* type mask: D8 is SINT */
#define T5TYPEMASK_REAL     0x04    /* type mask: D32 is REAL */
#define T5TYPEMASK_LREAL    0x08    /* type mask: D64 is LREAL */

typedef struct                      /* one program */
{
    T5_PTR    pD8;          /*PTR*/ /* pointer to D8 packet */
    T5_PTR    pD16;         /*PTR*/ /* pointer to D16 packet */
    T5_PTR    pD32;         /*PTR*/ /* pointer to D32 packet */
    T5_PTR    pD64;         /*PTR*/ /* pointer to D64 packet */
    T5_PTR    pTime;        /*PTR*/ /* pointer to TIME packet */
    T5_PTR    pString;      /*PTR*/ /* pointer to STRING packet */
    T5_WORD   wSizeD8;              /* size of D8 packet */
    T5_WORD   wSizeD16;             /* size of D16 packet */
    T5_WORD   wSizeD32;             /* size of D32 packet */
    T5_WORD   wSizeD64;             /* size of D64 packet */
    T5_WORD   wSizeTime;            /* size of TIME packet */
    T5_WORD   wSizeString;          /* size of STRING packet */
    T5_DWORD  dwCrc;                /* CRC */
    T5_PTCHAR szConf;               /* reserved */
    T5_DWORD  dwUser;               /* OEM data */
                                    /* following info may be missing (NULL) */
    T5_PTR    pCT;          /*PTR*/ /* pointer to packet in CTSeg segment */
    T5_DWORD  dwSizeCT;             /* size of packet in CTSeg segment */
} T5STR_DBRETAIN;

typedef T5STR_DBRETAIN      *T5PTR_DBRETAIN;

typedef T5_BOOL T5HDNSPEC (*T5HND_STF)(T5PTR_DB pDB, T5_PTWORD pArgs);

typedef struct                      /* one program */
{
    T5_DWORD    dwVers;             /* DB version number */
    T5_PTR      pCode;      /*PTR*/ /* pointer to the app code */
    T5_DWORD    dwFullBuild;        /* stamp of last full build */
    T5_DWORD    dwSfcCrc;           /* CRC of SFC steps and transitions */
    T5_PTBYTE   pTypeMasks; /*PTR*/ /* list of type masks (VA indexed) */
    T5_DWORD    dwPassword;         /* comm password */
    T5_DWORD    dwCTUsedSize;       /* used size in CT segment */
    T5STR_DBRETAIN retain;          /* definition of retain packets */
    T5HND_STF   pfStf;      /*PTR*/ /* pointer to std functions handler */
    T5_DWORD    dwStrTable;         /* active string table index */
    T5_PTR      pStrTable;  /*PTR*/ /* pointer to active string table */
    T5_PTR      pReg;       /*PTR*/ /* pointer to registry */
    T5_PTBYTE   pSetPoints; /*PTR*/ /* pointer to set point tables */
    T5_DWORD    dwMapCrc;           /* CRC of variable map */
    /* following MUST be the last one and alignesd to 4! */
    T5_PTR      pHeap;              /* private safe heap */
    /* heap buffer if static */
} T5STR_DBPRIVATE;

typedef T5STR_DBPRIVATE     *T5PTR_DBPRIVATE;
#define T5GET_DBPRIVATE(h)  ((T5PTR_DBPRIVATE)((h)[T5TB_PRIVATE].pData))
#define T5GET_DBRETAIN(h)   (&(T5GET_DBPRIVATE(h)->retain))

/****************************************************************************/
/* T5TB_PROG: programs */

#define T5FLAG_PRGACTIVE    0x0001  /* program active */
#define T5FLAG_PRGSUSPENDED 0x0002  /* program suspended */
#define T5FLAG_PRGINEXEC    0x0008  /* program currently executed */
#define T5FLAG_PRGTOSTART   0x0010  /* SFC program must be started now */
#define T5FLAG_PRGTOSTOP    0x0020  /* SFC program must be stopped now */

#define T5PRGSTYLE_DEFAULT  0       /* non SFC program */
#define T5PRGSTYLE_SFC      1       /* main SFC program */
#define T5PRGSTYLE_CHILD    2       /* child SFC program */
#define T5PRGSTYLE_ONCALL   3       /* called (sub-program) */

typedef struct                      /* one program */
{
    T5_PTR      pCode;      /*PTR*/ /* pointer to the code */
    T5_DWORD    dwSize;             /* code size */
    T5_DWORD    dwPos;              /* wait position */
    T5_WORD     wFlags;             /* auxiliary message argument */
    T5_WORD     wStyle;             /* exec style */
    T5_WORD     wStackDepth;        /* size of saved call stack */
    T5_WORD     wStackSave;         /* index of save buffer in CALLSTACK */
    T5_WORD     wCount;             /* cycle counter for scheduling */
    T5_WORD     res1;
} T5STR_DBPROG;

typedef T5STR_DBPROG        *T5PTR_DBPROG;
#define T5GET_DBPROG(h)     ((T5PTR_DBPROG)((h)[T5TB_PROG].pData))

/****************************************************************************/
/* T5TB_ACTIME: active timers */

typedef struct                      /* one program */
{
    T5_PTDWORD  pdwTime;            /* pointer to the variable */
    T5_DWORD    dwLastUpdate;       /* stamp of last update */
} T5STR_DBACTIME;

typedef T5STR_DBACTIME      *T5PTR_DBACTIME;
#define T5GET_DBACTIME(h)   ((T5PTR_DBACTIME)((h)[T5TB_ACTIME].pData))

/****************************************************************************/
/* T5TB_FBCLASS: FB classes */

typedef T5_DWORD T5HDNSPEC (*T5HND_FB)(T5_WORD wCommand, T5PTR_DB pBase,
                            T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

#define T5FBCMD_ACTIVATE        0   /* command: activates instance */
#define T5FBCMD_GETCLASSID      1   /* command: get class ID (std only) */
#define T5FBCMD_SIZEOFCLASS     2   /* command: get sizeof class data */
#define T5FBCMD_SIZEOFINSTANCE  3   /* command: get sizeof inst data */
#define T5FBCMD_INITCLASS       4   /* command: init class data */
#define T5FBCMD_EXITCLASS       5   /* command: close class */
#define T5FBCMD_INITINSTANCE    6   /* command: init inst data */
#define T5FBCMD_EXITINSTANCE    7   /* command: close instance */
#define T5FBCMD_HOTRESTART      8   /* command: reinitialize instance */
#define T5FBCMD_ACCEPTCT        9   /* command: accept CTSeg arguments */
#define T5FBCMD_KEEPALIVE       10  /* command: keep alive */
#define T5FBCMD_WANTKA          11  /* command: want keep alive calls */
#define T5FBCMD_SETINSTNAME     12  /* command: set instance name (in pArgs) */

#define T5FBCLASS_VALID     0x0001  /* the class is valid */

#define T5FB_GETINSTNAMEARG(a)  (*((T5_PTCHAR *)(a)))

typedef struct                      /* one class */
{
    T5_STRING16 szName;             /* class name */
    T5_DWORD    dwClassSizeof;      /* sizeof class data */
    T5_DWORD    dwInstSizeof;       /* sizeof instance data */
    T5HND_FB    pfHandler;  /*PTR*/ /* FB class handler */
    T5_PTR      pData;      /*PTR*/ /* pointer to class data */
    T5_WORD     wFlags;             /* flags */
    T5_WORD     wID;                /* class ID */
} T5STR_DBFBCLASS;

typedef T5STR_DBFBCLASS     *T5PTR_DBFBCLASS;
#define T5GET_DBFBCLASS(h)  ((T5PTR_DBFBCLASS)((h)[T5TB_FBCLASS].pData))

/****************************************************************************/
/* T5TB_FCLASS: Custom function classes */

#define T5FCLASS_VALID      0x0001  /* the class is valid */

typedef struct                      /* one class */
{
    T5_STRING16 szName;             /* class name */
    T5HND_FB    pfHandler;  /*PTR*/ /* FB class handler */
    T5_WORD     wFlags;             /* flags */
    T5_WORD     res1;               /* class ID */
} T5STR_DBFCLASS;

typedef T5STR_DBFCLASS      *T5PTR_DBFCLASS;
#define T5GET_DBFCLASS(h)   ((T5PTR_DBFCLASS)((h)[T5TB_FCLASS].pData))

/****************************************************************************/
/* T5TB_FBINST: FB instances */

#define T5FBI_WANTKA        0x8000  /* want keep-alive signal */

typedef struct                      /* one instance */
{
    T5_WORD     wClass;             /* index of the class in DB */
    T5_WORD     wFlags;             /* flags */
    T5HND_FB    pfHandler;  /*PTR*/ /* FB class handler */
    T5_PTR      pClassData; /*PTR*/ /* pointer to class data */
    T5_PTR      pData;      /*PTR*/ /* pointer to instance data */
    T5_DWORD    dwCRC;              /* CRC based on class name */
} T5STR_DBFBINST;

typedef T5STR_DBFBINST      *T5PTR_DBFBINST;
#define T5GET_DBFBINST(h)   ((T5PTR_DBFBINST)((h)[T5TB_FBINST].pData))

/****************************************************************************/
/* T5TB_SYSRSC: system resources */

#define T5SYSRSC_MALLOC     1       /* memory block given by T5_MALLOC */
#define T5SYSRSC_FILE       2       /* file open with T5File_Open */
#define T5SYSRSC_ARRAY      3       /* array allocated with T5_MALLOC */
#define T5SYSRSC_SOCKET     4       /* socket */

typedef struct                      /* one log message */
{
    T5_DWORD    dwObject;           /* system resource handle */
    T5_DWORD    dwSize;             /* size (if needed) */
    T5_WORD     wUsed;              /* non zero if used - can be an id */
    T5_WORD     wType;              /* type of resource */
} T5STR_DBSYSRSC;

typedef T5STR_DBSYSRSC      *T5PTR_DBSYSRSC;
#define T5GET_DBSYSRSC(h)   ((T5PTR_DBSYSRSC)((h)[T5TB_SYSRSC].pData))

/****************************************************************************/
/* T5TB_STEP / T5TB_TRANS: sfc steps and transitions */

#define T5SFCBREAK_P1       1       /* break on step activation */
#define T5SFCBREAK_P0       2       /* break on step de-activation */
#define T5SFCBREAK_T        3       /* break on transition */

#define T5STEP_INACTIVE     0       /* step is inactive */
#define T5STEP_N            1       /* step is active */
#define T5STEP_P1           2       /* step is asked to be activated */
#define T5STEP_P0           3       /* step is asked to be de-activated */

typedef struct                      /* sfc step */
{
    T5_DWORD    dwAcTime;           /* activity duration */
    T5_DWORD    dwLastUpdate;       /* time stamp of last update */
    T5_WORD     wStatus;            /* status */
    T5_WORD     wBreak;             /* breakpoint style */
} T5STR_DBSTEP;

typedef T5STR_DBSTEP        *T5PTR_DBSTEP;
#define T5GET_DBSTEP(h)     ((T5PTR_DBSTEP)((h)[T5TB_STEP].pData))

typedef struct                      /* sfc transition */
{
    T5_WORD     wValid;             /* non zero if valid */
    T5_WORD     wBreak;             /* breakpoint style */
} T5STR_DBTRANS;

typedef T5STR_DBTRANS       *T5PTR_DBTRANS;
#define T5GET_DBTRANS(h)    ((T5PTR_DBTRANS)((h)[T5TB_TRANS].pData))

/****************************************************************************/
/* Embedded variable map */

typedef struct
{
    T5_PTR pDef;            /*PTR*/ /* pointer to statc definition */
    T5_PTR pData;           /*PTR*/ /* pointer to variable in database */
    T5_PTBYTE pLock;        /*PTR*/ /* pointer to lock state */
    T5_PTR pNextIndex;      /*PTR*/ /* next in index hash list */
    T5_PTR pNextSyb;        /*PTR*/ /* next in symbol hash list */
    T5_PTR pNextTag;        /*PTR*/ /* next in num.tag hash list */
} T5STR_DBMAP;

typedef T5STR_DBMAP         *T5PTR_DBMAP;
#define T5GET_DBMAP(h)      ((T5PTR_DBMAP)((h)[T5TB_VARMAP].pData))

#define T5GET_DBHASH(h)     ((T5PTR_DBMAP *)((h)[T5TB_HASH].pData))

typedef struct
{
    T5_DWORD dwStatHi;      /* status bits (HSB) */
    T5_DWORD dwStatLo;      /* status bits (LSB) */
    T5_DWORD dwDate;        /* date stamp */
    T5_DWORD dwTime;        /* time stampe */
} T5STR_DBMAPEX;

typedef T5STR_DBMAPEX       *T5PTR_DBMAPEX;

/****************************************************************************/
/* digital sampling trace */

#define T5DBMAX_SIGNAL      8           /* fixed! stored on byte's bits */
#define T5MAX_BSAMPLING     900         /* sampling trace buffer size */

typedef struct
{
    T5_DWORD dwPeriod;                  /* period in ms or 0 */
    T5_DWORD dwLastDate;                /* last stamping */
    T5_DWORD dwDelay;                   /* running delay */
    T5_DWORD dwCountH;                  /* sample count - High */
    T5_DWORD dwCountL;                  /* sample count - Low */
    T5_DWORD dwDateStamp;               /* date stamping */
    T5_DWORD dwTimeStamp;               /* time of the day stamping */
    T5_WORD  wOnDelay;                  /* On delay in ms or 0 */
    T5_WORD  wOffDelay;                 /* Off delay in ms or 0 */
    T5_WORD  wIndex[T5DBMAX_SIGNAL];    /* traced variables */
    T5_WORD  wFlags;                    /* flags */
    T5_WORD  wOnTrigger;                /* on triggering variable */
    T5_WORD  wOffTrigger;               /* off triggering variable */
    T5_WORD  wpNext;                    /* read pointer */
    T5_BYTE  bOnPrev;                   /* memo ON trigger */
    T5_BYTE  bOffPrev;                  /* memo OFF trigger */
    T5_BYTE  bData[T5MAX_BSAMPLING];    /* data */
} T5STR_DBBSAMPLING;

typedef T5STR_DBBSAMPLING   *T5PTR_DBBSAMPLING;
#define T5GET_DBBSAMPLING(h) ((T5PTR_DBBSAMPLING)((h)[T5TB_BSAMPLING].pData))

/****************************************************************************/
/* digital sampling trace */

#ifndef T5MAX_TASK
#define T5MAX_TASK  1       /* old targets: just one task: the cycle */
#endif /*T5MAX_TASK*/

typedef struct
{
    T5_PTCHAR pName;        /*PTR*/ /* pointer to task name */
    T5_PTWORD pPou;         /*PTR*/ /* pointer to list of program indexes */
} T5STR_DBTASK;

typedef T5STR_DBTASK         *T5PTR_DBTASK;
#define T5GET_DBTASK(h)      ((T5PTR_DBTASK)((h)[T5TB_TASK].pData))

/****************************************************************************/
/* CAN bus */

#ifdef T5DEF_CANBUS

/* callback for calls from a driver implemented as DLL */

#define T5CANHC_PEEK    1
#define T5CANHC_POP     2
#define T5CANHC_FIND    3
#define T5CANHC_MARK    4

typedef T5_PTR T5HDNSPEC (*T5HND_CAN)(T5_DWORD dwCommand,
                                      T5_PTR pPort, T5_PTR pMsg, T5_DWORD dwID);

/* oem defined data types */

#ifndef T5_CANPORT
#define T5_CANPORT T5_DWORD
#endif /*T5_CANPORT*/

#ifndef T5_CANMSG
#define T5_CANMSG T5_DWORD
#endif /*T5_CANMSG*/

typedef T5_CANPORT *T5_PTCANPORT;
typedef T5_CANMSG  *T5_PTCANMSG;

/* message length */
#ifndef T5CAN_MAXDATA
#define T5CAN_MAXDATA 8
#endif /*T5CAN_MAXDATA*/

/* flags */

#define T5CAN_OUT       0x8000  /* flag: out message (SND) */
#define T5CAN_RECEIVED  0x0001  /* flag: message received */
#define T5CAN_TOSEND    0x0002  /* flag: message is in send queue */
#define T5CAN_CHANGED   0x0004  /* flag: data in message (SND) has changed */
#define T5CAN_SHOT      0x0008  /* flag: data in message (SND) shot by command */
#define T5CAN_RTR       0x0010  /* flag: RTR message (SND) */
#define T5CAN_APPFIFO   0x0020  /* flag: message from app. FIFOs (SND or RCV) */

#define T5CAN_2B        0x0001  /* flag(port): CAN2B used */
#define T5CAN_SCAN0     0x0002  /* flag(port): first scan */
#define T5CAN_RECVS     0x0004  /* flag(port): at least one message received */

/* structures */

typedef struct
{
    T5_PTR          pDef;       /* pointer to static def - T5PTR_CANMSGDEF */
    T5_PTR          pNext;      /* pointer to newt message in send queue */
    T5_DWORD        dwID;       /* CAN id */
#ifdef T5CAN_OEMALLOC
    T5_PTBYTE       data;
#else /*T5CAN_OEMALLOC*/
    T5_BYTE         data[T5CAN_MAXDATA];    /* CAN message */
#endif /*T5CAN_OEMALLOC*/
    T5_DWORD        dwLastSnd;  /* stamp of last emission */
    T5_DWORD        dwSpcIn;    /* HW specific input data */
    T5_DWORD        dwSpcOut;   /* HW specific output data */
    T5_WORD         wFlags;     /* message status */
    T5_BYTE         bLen;       /* message length */
    T5_BYTE         bCO;        /* CANopen mark */
    T5_CANMSG       oem;        /* hardware specific runtime data */
} T5STR_DBCANMSG;

typedef T5STR_DBCANMSG       *T5PTR_DBCANMSG;
#define T5GET_DBCANMSG(h)    ((T5PTR_DBCANMSG)((h)[T5TB_CANMSG].pData))

typedef struct
{
    T5_PTR          pDef;       /* pointer to static def - T5PTR_CANPORTDEF */
    T5PTR_DBCANMSG  pMsgIn;     /* pointer to first received message */
    T5PTR_DBCANMSG  pMsgOut;    /* pointer to first sent message */
    T5PTR_DBCANMSG  pOutHead;   /* pointer to send FIFO */
    T5PTR_DBCANMSG  pOutQueue;  /* queue of send FIFO */
    T5PTR_DBCANMSG  pRcvFifo;   /* receive - application FIFO */
    T5PTR_DBCANMSG  pSndFifo;   /* send - application FIFO */
    T5_PTCHAR       szSettings; /* point to configuration string */
    T5_WORD         wFlags;     /* port status */
    T5_WORD         wRate;      /* baud rate (KB) */
    T5_WORD         wFifoSize;  /* App FIFOs size */
    T5_WORD         res1;
    T5HND_CAN       pfCB;       /* callback for driver */
    T5_CANPORT      oem;        /* hardware specific runtime data */
} T5STR_DBCANPORT;

typedef T5STR_DBCANPORT      *T5PTR_DBCANPORT;
#define T5GET_DBCANPORT(h)   ((T5PTR_DBCANPORT)((h)[T5TB_CANPORT].pData))

typedef struct
{
    T5_PTR          pDef;       /* pointer to static def - T5PTR_CANVARDEF */
#ifdef T5DEF_REALSUPPORTED
    T5_PTR          pScale;     /* pointer to scaling info */
#endif /*T5DEF_REALSUPPORTED*/
    T5PTR_DBCANMSG  pMsg;       /* pointer to the message */
    T5_PTBYTE       pData;      /* pointer to data in VMDB */
    T5_PTBYTE       pLock;      /* pointer to lock status */
    T5_BYTE         bLockMask;  /* mask for lock bitfield */
    T5_BYTE         res1;
    T5_BYTE         res2;
    T5_BYTE         res3;
} T5STR_DBCANVAR;

typedef T5STR_DBCANVAR       *T5PTR_DBCANVAR;
#define T5GET_DBCANVAR(h)    ((T5PTR_DBCANVAR)((h)[T5TB_CANVAR].pData))

/* helpers for callback */

#define _T5CAN_PEEK(p) \
    (T5PTR_DBCANMSG)((p)->pfCB)(T5CANHC_PEEK, (p), NULL, 0L)
#define _T5CAN_POP(p) \
    (T5PTR_DBCANMSG)((p)->pfCB)(T5CANHC_POP, (p), NULL, 0L)
#define _T5CAN_FIND(p,i) \
    (T5PTR_DBCANMSG)((p)->pfCB)(T5CANHC_FIND, (p), NULL, (i))
#define _T5CAN_MARK(p,m) \
    ((p)->pfCB)(T5CANHC_MARK, (p), (m), 0L)

/* hooks */

#ifdef T5CAN_OEMALLOC
extern T5_PTBYTE T5CAN_OemAlloc (T5PTR_DBCANPORT pPort, T5_DWORD dwID, T5_BYTE bLen);
extern void T5CAN_OemFree (T5PTR_DBCANPORT pPort, T5_PTBYTE pBuffer);
#endif /*T5CAN_OEMALLOC*/

#endif /*T5DEF_CANBUS*/

/****************************************************************************/
/* status bits */

                                        /* BIT NUMBERS */
#define T5VSB_ST_M1         0           /* User status */
#define T5VSB_ST_M2         1           /* User status */
#define T5VSB_ST_M3         2           /* User status */
#define T5VSB_ST_M4         3           /* User status */
#define T5VSB_ST_M5         4           /* User status */
#define T5VSB_ST_M6         5           /* User status */
#define T5VSB_ST_M7         6           /* User status */
#define T5VSB_ST_M8         7           /* User status */
#define T5VSB_SELEC         8           /* Select */
#define T5VSB_REV           9           /* Revision */
#define T5VSB_DIREC         10          /* Desired direction */
#define T5VSB_RTE           11          /* Runtime exceeded */
#define T5VSB_MVALUE        12          /* Manual value */
#define T5VSB_ST_14         13          /* User status */
#define T5VSB_ST_15         14          /* User status */
#define T5VSB_ST_16         15          /* User status */
#define T5VSB_GR            16          /* General request */
#define T5VSB_SPONT         17          /* Spontaneous */
#define T5VSB_I_BIT         18          /* I-BIT Invalid */
#define T5VSB_SUWI          19          /* SU/WI Summer/winter time announcement */
#define T5VSB_N_UPD         20          /* Switched off */
#define T5VSB_RT_E          21          /* Realtime external */
#define T5VSB_RT_I          22          /* Realtime internal */
#define T5VSB_NSORT         23          /* Not sortable */
#define T5VSB_DM_TR         24          /* Default message trafo value */
#define T5VSB_RM_TR         25          /* Run message trafo value */
#define T5VSB_INFO          26          /* Info for variable */
#define T5VSB_AVALUE        27          /* Alternative value */
#define T5VSB_RES28         28          /* reserved */
#define T5VSB_ACTUAL        29          /* Not updated */
#define T5VSB_WINTER        30          /* Winter time */
#define T5VSB_RES31         31          /* reserved */
#define T5VSB_TCB0          32          /* Transmission cause */
#define T5VSB_TCB1          33          /* Transmission cause */
#define T5VSB_TCB2          34          /* Transmission cause */
#define T5VSB_TCB3          35          /* Transmission cause */
#define T5VSB_TCB4          36          /* Transmission cause */
#define T5VSB_TCB5          37          /* Transmission cause */
#define T5VSB_PN_BIT        38          /* P/N bit */
#define T5VSB_T_BIT         39          /* Test bit */
#define T5VSB_WR_ACK        40          /* Acknowledge writing */
#define T5VSB_WR_SUC        41          /* Writing successful */
#define T5VSB_NORM          42          /* Normal status */
#define T5VSB_ABNORM        43          /* Deviation normal status */
#define T5VSB_BL_BIT        44          /* IEC-Status "blocked" */
#define T5VSB_SP_BIT        45          /* IEC-Status "sustituted" */
#define T5VSB_NT_BIT        46          /* IEC-Status "not topical" */
#define T5VSB_OV_BIT        47          /* IEC-Status "overflow" */
#define T5VSB_SE_BIT        48          /* IEC-Status "select" */
#define T5VSB_TIME_INVAL    49          /* Time invalid */
#define T5VSB_SFE           50          /* Switching case detected */
#define T5VSB_SFI           51          /* Switch case detection disabled */
#define T5VSB_T_INVAL       49
#define T5VSB_CB_TRIP       50
#define T5VSB_CB_TR_I       51

                                        /* FLAGS - LO */
#define T5VSM_L_ST_M1       0x00000001L /* User status */
#define T5VSM_L_ST_M2       0x00000002L /* User status */
#define T5VSM_L_ST_M3       0x00000004L /* User status */
#define T5VSM_L_ST_M4       0x00000008L /* User status */
#define T5VSM_L_ST_M5       0x00000010L /* User status */
#define T5VSM_L_ST_M6       0x00000020L /* User status */
#define T5VSM_L_ST_M7       0x00000040L /* User status */
#define T5VSM_L_ST_M8       0x00000080L /* User status */
#define T5VSM_L_SELEC       0x00000100L /* Select */
#define T5VSM_L_REV         0x00000200L /* Revision */
#define T5VSM_L_DIREC       0x00000400L /* Desired direction */
#define T5VSM_L_RTE         0x00000800L /* Runtime exceeded */
#define T5VSM_L_MVALUE      0x00001000L /* Manual value */
#define T5VSM_L_ST_14       0x00002000L /* User status */
#define T5VSM_L_ST_15       0x00004000L /* User status */
#define T5VSM_L_ST_16       0x00008000L /* User status */
#define T5VSM_L_GR          0x00010000L /* General request */
#define T5VSM_L_SPONT       0x00020000L /* Spontaneous */
#define T5VSM_L_I_BIT       0x00040000L /* -BIT Invalid I-BIT Invalid */
#define T5VSM_L_SUWI        0x00080000L /* /WI Summer/winter time announcement */
#define T5VSM_L_N_UPD       0x00100000L /* Switched off */
#define T5VSM_L_RT_E        0x00200000L /* Realtime external */
#define T5VSM_L_RT_I        0x00400000L /* Realtime internal */
#define T5VSM_L_NSORT       0x00800000L /* Not sortable */
#define T5VSM_L_DM_TR       0x01000000L /* Default message trafo value */
#define T5VSM_L_RM_TR       0x02000000L /* Run message trafo value */
#define T5VSM_L_INFO        0x04000000L /* Info for variable */
#define T5VSM_L_AVALUE      0x08000000L /* Alternative value */
#define T5VSM_L_RES28       0x10000000L /* reserved */
#define T5VSM_L_ACTUAL      0x20000000L /* Not updated */
#define T5VSM_L_WINTER      0x40000000L /* Winter time */
#define T5VSM_L_RES31       0x80000000L /* reserved */

                                        /* FLAGS - HI */
#define T5VSM_H_TCB0        0x00000001L /* Transmission cause */
#define T5VSM_H_TCB1        0x00000002L /* Transmission cause */
#define T5VSM_H_TCB2        0x00000004L /* Transmission cause */
#define T5VSM_H_TCB3        0x00000008L /* Transmission cause */
#define T5VSM_H_TCB4        0x00000010L /* Transmission cause */
#define T5VSM_H_TCB5        0x00000020L /* Transmission cause */
#define T5VSM_H_PN_BIT      0x00000040L /* P/N bit */
#define T5VSM_H_T_BIT       0x00000080L /* Test bit */
#define T5VSM_H_WR_ACK      0x00000100L /* Acknowledge writing */
#define T5VSM_H_WR_SUC      0x00000200L /* Writing successful */
#define T5VSM_H_NORM        0x00000400L /* Normal status */
#define T5VSM_H_ABNORM      0x00000800L /* Deviation normal status */
#define T5VSM_H_BL_BIT      0x00001000L /* IEC-Status "blocked" */
#define T5VSM_H_SP_BIT      0x00002000L /* IEC-Status "sustituted" */
#define T5VSM_H_NT_BIT      0x00004000L /* IEC-Status "not topical" */
#define T5VSM_H_OV_BIT      0x00008000L /* IEC-Status "overflow" */
#define T5VSM_H_SE_BIT      0x00010000L /* IEC-Status "select" */
#define T5VSM_H_TIME_INVAL  0x00020000L /* Time invalid */
#define T5VSM_H_SFE         0x00040000L /* Switching case detected */
#define T5VSM_H_SFI         0x00080000L /* Switch case detection disabled */
#define T5VSM_T_INVAL       0x00020000L
#define T5VSM_CB_TRIP       0x00040000L
#define T5VSM_CB_TR_I       0x00080000L

/****************************************************************************/      
/* text buffer records */

#ifdef T5DEF_TXB

#define T5HP_TXH             0x00425854 /* "TXB" */

typedef struct          /* text buffer record */
{
    T5_DWORD dwKind;    /* = "XSB" */
    T5_DWORD dwSize;    /* allocated size */
    T5_DWORD dwPos;     /* sequential read position */
    T5_DWORD hText;     /* text buffer */
}
T5STR_XS, *T5PTR_XS;

#endif /*T5DEF_TXB*/

/****************************************************************************/      
/* XML records */

#ifdef T5DEF_XML

#define T5HP_XML         0x004C4D58 /* "XML" */

typedef struct          /* XML tag record */
{
    T5_DWORD dwKind;    /* = "XSB" */
    T5_DWORD hName;     /* tag name handle */
    T5_DWORD hCont;     /* tag contents handle */
    T5_DWORD hChild;    /* first child handle */
    T5_DWORD hParent;   /* parent handle */
    T5_DWORD hSibling;  /* sibling child handle */
    T5_DWORD hAttrib;   /* first attribute handle */
}
T5STR_XMLTAG, *T5PTR_XMLTAG;

typedef struct          /* XML attribute record */
{
    T5_DWORD hNext;     /* handle of next attribute */
    T5_DWORD hName;     /* handle of string: name */
    T5_DWORD hText;     /* handle of string: value */
}
T5STR_XMLATTR, *T5PTR_XMLATTR;

typedef struct          /* XML string record */
{
    T5_DWORD dwSize;    /* allocated string size */
    /* + text: "name=value" */
}
T5STR_XMLSTR, *T5PTR_XMLSTR;

#endif /*T5DEF_XML*/

/****************************************************************************/      

#endif /*_T5VMDB_H_INCLUDED_*/

/* eof **********************************************************************/
