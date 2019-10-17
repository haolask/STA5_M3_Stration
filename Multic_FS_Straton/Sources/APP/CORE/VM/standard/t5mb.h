/*****************************************************************************
T5mb.h :     MODBUS core engine definitions

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#ifndef _T5MB_H_INCLUDED_
#define _T5MB_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* MODBUS function codes */

#define T5MBF_RIB           2       /* read bit inputs */
#define T5MBF_RCB           1       /* read coils */
#define T5MBF_W1B           5       /* write 1 coil */
#define T5MBF_WNB           15      /* write N coils */
#define T5MBF_RIW           4       /* read input registers */
#define T5MBF_RHW           3       /* read holding registers */
#define T5MBF_W1W           6       /* write 1 register */
#define T5MBF_DIAG          8       /* diagnostic */
#define T5MBF_WNW           16      /* write N registers */
#define T5MBF_REX           7       /* read exception status */
#define T5MBF_RID           17      /* report slave ID */
#define T5MBF_RWR           23      /* read/write registers */
#define T5MBF_IDENT         43      /* read device identification */
#define T5MBF_RAPSS         0x65    /* read APSS register time stamps */

#define T5MBF_IB            2       /* server: bit inputs */
#define T5MBF_CB            1       /* server: coils */
#define T5MBF_HW            3       /* server: holding registers */
#define T5MBF_IW            4       /* server: input registers */

/****************************************************************************/
/* error codes */

#define T5MBERR_FUNC        1       /* invalid function code */
#define T5MBERR_ADDR        2       /* bad address */
#define T5MBERR_VALUE       3       /* bad value */
#define T5MBERR_FAIL        4       /* server failed */
#define T5MBERR_ACK         5       /* server acknoledge */
#define T5MBERR_BUSY        6       /* server is busy */
#define T5MBERR_PARITY      8       /* data parity error */
#define T5MBERR_PATH        10      /* gateway bad path */
#define T5MBERR_GATETARGET  11      /* gateway target failed */

#define T5MBERR_TIMEOUT     128     /* timeout on exchange */
#define T5MBERR_CRC         129     /* bad CRC */
#define T5MBERR_COMM        130     /* RS232 or timeout error */

/****************************************************************************/
/* static definition: objects (head + objects... + null object) */

#define T5MBDEF_NODE        1       /* a remote slave node */
#define T5MBDEF_REQ         2       /* a request */
#define T5MBDEF_VAR         3       /* a variable */
#define T5MBDEF_VARCT       4       /* a variable in CT segment */
#define T5MBDEF_SVAR        5       /* a scaled variable */
#define T5MBDEF_SVARCT      6       /* a scaled variable in CT segment */
#define T5MBDEF_SLAVE       7       /* a slave (for diagnostics) */

typedef struct                      /* main header - fixed 8 bytes */
{
    T5_DWORD dwAllocSize;           /* allocation size (for On Line Change) */
    T5_WORD  wVers;                 /* version = 1 */
    T5_WORD  wFlags;                /* reserved = 0 */
}
T5STR_MBDEFHEAD;

typedef T5STR_MBDEFHEAD * T5PTR_MBDEFHEAD;

typedef struct                      /* an object definition header */
{
    T5_WORD wSizeof;                /* sizeof the structure */
    T5_WORD wDefID;                 /* kind of structire */
    /* definition follows */
}
T5STR_MBDEF;

typedef T5STR_MBDEF * T5PTR_MBDEF;

/****************************************************************************/
/* static definition: port */

#define T5MBDEF_RECONNECT   0x0001  /* flag: reconnect on error */
#define T5MBDEF_SERIAL      0x0010  /* serial comm - def is ETHERNET */
#define T5MBDEF_SERVER      0x0100  /* unique MODBUS server */

typedef struct                      /* a slave MODBUS node */
{
    T5_WORD wSizeof;                /* sizeof the structure */
    T5_WORD wDefID;                 /* kind of structire */
    T5_WORD wID;                    /* node ID */
    T5_WORD wPort;                  /* port number or server slave nb */
    T5_WORD wReqDelay;              /* delay between requests (ms) */
    T5_WORD wFlags;                 /* type of node */
    T5_WORD wCnxDelay;              /* delay before reconnection (ms) */
    T5_CHAR szAddr[32];             /* IP address or COM settings */
}
T5STR_MBDEFNODE;

typedef T5STR_MBDEFNODE * T5PTR_MBDEFNODE;

/****************************************************************************/
/* static definition: slave */

typedef struct                      /* a slave MODBUS node */
{
    T5_WORD wSizeof;                /* sizeof the structure */
    T5_WORD wDefID;                 /* kind of structire */
    T5_WORD wNodeID;                /* ID of the slave node */
    T5_WORD wSlaveNo;               /* MODBUS slave number */
}
T5STR_MBDEFSLAVE;

typedef T5STR_MBDEFSLAVE * T5PTR_MBDEFSLAVE;

/****************************************************************************/
/* static definition: request */

#define T5MBMODE_STANDBY    0       /* standy mode (wait for operation) */
#define T5MBMODE_PERIODIC   1       /* periodic request */
#define T5MBMODE_ONCALL     2       /* one shot request on call */
#define T5MBMODE_ONCHANGE   3       /* one shot request on output change */

typedef struct                      /* a MODBUS request */
{
    T5_WORD wSizeof;                /* sizeof the structure */
    T5_WORD wDefID;                 /* kind of structure */
    T5_WORD wID;                    /* request ID */
    T5_WORD wNodeID;                /* ID of the slave node */
    T5_WORD wSlave;                 /* target/unit identifier */
    T5_WORD wMode;                  /* initial operating mode */
    T5_WORD wPeriod;                /* initial period in ms */
    T5_WORD wFunc;                  /* MODBUS function */
    T5_WORD wAddr;                  /* MODBUS address */
    T5_WORD wNbItem;                /* number of MODBUS bits or words */
    T5_WORD wRetry;                 /* number of trials (1..N) */
    T5_WORD wTimeOut;               /* time out in ms */
    T5_WORD wDataSize;              /* size of raw buffer */
    /* version V2 required */
    T5_WORD wErrPeriod;
}
T5STR_MBDEFREQ;

#define T5SIZEOF_MBDEFREQ_V1        (13 * sizeof (T5_WORD))
#define T5MB_ISREQ_V2(pr)           ((pr)->wSizeof > T5SIZEOF_MBDEFREQ_V1)

typedef T5STR_MBDEFREQ * T5PTR_MBDEFREQ;

/****************************************************************************/
/* static definition: variable */

#define T5MBVAR_INPUT       0       /* input for T5 runtime */
#define T5MBVAR_OUTPUT      1       /* output for T5 runtime */
#define T5MBVAR_STATUS      2       /* status input */
#define T5MBVAR_COMMAND     3       /* request activation command */
#define T5MBVAR_TRIAL       4       /* status: current trial (0=first) */
#define T5MBVAR_SHOT        5       /* command: one shot */
#define T5MBVAR_NBOK        6       /* status: success counter */
#define T5MBVAR_NBFAIL      7       /* status: fail counter */
#define T5MBVAR_RSTCNT      8       /* command: reset counters */
#define T5MBVAR_GOING       9       /* status: on-going request */
#define T5MBVAR_STATACK     10      /* status input - no reset */

#define T5MBVAR_SLVERROR    11      /* slave diag - last error */
#define T5MBVAR_SLVDTERROR  12      /* slave diag - last err date */
#define T5MBVAR_SLVTMERROR  13      /* slave diag - last err time */
#define T5MBVAR_SLVDTRESET  14      /* slave diag - reset date */
#define T5MBVAR_SLVTMRESET  15      /* slave diag - reset time */
#define T5MBVAR_SLVNBTRANS  16      /* slave diag - nb trans */
#define T5MBVAR_SLVNBFAIL   17      /* slave diag - nb failed */
#define T5MBVAR_SLVRESET    18      /* slave command - reset */

#define T5MBVAR_INSTRING    19      /* string input for T5 runtime */
#define T5MBVAR_OUTSTRING   20      /* string output for T5 runtime */

typedef struct                      /* a MODBUS request */
{
    T5_WORD wSizeof;                /* sizeof the structure */
    T5_WORD wDefID;                 /* kind of structure */
    T5_WORD wReqID;                 /* ID of the request */
    T5_WORD wOffset;                /* offset in request data */
    T5_WORD wNbWrd;                 /* number of words for D32/D64 */
    T5_WORD wMask;                  /* mask for BIT/D8/D16/D32 */
    T5_WORD wT5Type;                /* T5 variable data type */
    T5_WORD wT5Offset;              /* offset in T5 database */
    T5_WORD wOpe;                   /* operation */
    /* following: only if wDefID >= T5MBDEF_VARCT - unaligned! */
    T5_WORD wCTOffset1;             /* offset in CTSEG - 1st dword half */
    T5_WORD wCTOffset2;             /* offset in CTSEG - 2nd dword half */
    /* following: only if wDefID >= T5MBDEF_SVAR - unaligned! */
    T5_WORD wScaleMul1;             /* scale factor - 1st dword half */
    T5_WORD wScaleMul2;             /* scale factor - 2nd dword half */
    T5_WORD wScaleAdd1;             /* scale offset - 1st dword half */
    T5_WORD wScaleAdd2;             /* scale offset - 2nd dword half */
}
T5STR_MBDEFVAR;

typedef T5STR_MBDEFVAR * T5PTR_MBDEFVAR;

/****************************************************************************/
/* dynamic: frame */

#define T5MB_READY          0       /* ready slave - or OK request */
#define T5MB_CONNECTING     1       /* not ready - connect in progress */
#define T5MB_RECONNECT      2       /* not ready - connect in progress */
#define T5MB_SENDING        3       /* sending in progress */
#define T5MB_SENT           4       /* send complete */
#define T5MB_RECEIVING      5       /* receiving in progress */
#define T5MB_RECEIVED       6       /* receive complete */
#define T5MB_ERROR          7       /* error detected */
#define T5MB_REQDELAY       8       /* delay between requests */
#define T5MB_CNXDELAY       9       /* delay before reconnection */

typedef struct                      /* a MODBUS frame */
{
    T5_WORD  wStatus;               /* current status */
    T5_WORD  wError;                /* error code or 0 if OK */
    T5_WORD  wLenCur;               /* current length */
    T5_WORD  wSize;                 /* total or expected length */
    T5_WORD  wFrmCount;             /* frame counter */
    T5_BOOL  bLRC;                  /* LRC instead of CRC16 */
    T5_BOOL  res1;
    T5_DWORD dwDate;                /* date stamp: start for timeout */
    T5_DWORD dwTimeOut;             /* current request timeout */
    T5_BYTE  buffer[256+8];         /* data (maybe including MBAP) */
}
T5STR_MBFRAME;

typedef T5STR_MBFRAME * T5PTR_MBFRAME;

/****************************************************************************/
/* dynamic: slave */

typedef struct                      /* a MODBUS request */
{
    T5_WORD  wSlaveNo;              /* MODBUS slave number */
    T5_WORD  wLastError;            /* last detected error code */
    T5_DWORD dwDateLastError;       /* last error date stamp */
    T5_DWORD dwTimeLastError;       /* last error time stamp */
    T5_DWORD dwDateLastReset;       /* last reset date stamp */
    T5_DWORD dwTimeLastReset;       /* last reset date stamp */
    T5_DWORD dwNbTrans;             /* number of transactions */
    T5_DWORD dwNbFail;              /* number of failed transactions */
}
T5STR_MBSLAVE;

typedef T5STR_MBSLAVE * T5PTR_MBSLAVE;

/****************************************************************************/
/* dynamic: request */

typedef struct                      /* a MODBUS request */
{
    T5PTR_MBDEFREQ  pDef;           /* pointer to static definition */
    T5PTR_MBSLAVE   pSlave;         /* pointer to slave or NULL */
    T5_DWORD        dwDate;         /* date stamp: start of request */
    T5_PTBYTE       pData;          /* data raw buffer - variable size */
    T5_DWORD        dwDt;           /* stamp */
    T5_DWORD        dwTm;           /* stamp */
    T5_WORD         wStatus;        /* current status */
    T5_WORD         wError;         /* error code */
    T5_WORD         wTry;           /* current try index */
    T5_WORD         wNbOK;          /* counter: success */
    T5_WORD         wNbFail;        /* counter: fail */
    T5_WORD         wMask;          /* for RAPSS */
    T5_BYTE         bActive;        /* active flag for OnCall requests */
    T5_BYTE         bServer;        /* TRUE if server request */
    T5_BYTE         bShot;          /* one-shot command in progress */
    T5_BYTE         bChange;        /* after on line change */
}
T5STR_MBREQ;

typedef T5STR_MBREQ * T5PTR_MBREQ;

#define T5MB_ISREQSH(pReq) \
    (pReq->bServer \
     && (pReq->pDef->wFunc == T5MBF_CB || pReq->pDef->wFunc == T5MBF_HW))

/****************************************************************************/
/* dynamic: client */

#define T5MB_TCP            0       /* MODBUS/TCP on ETHERNET */
#define T5MB_RS232          1       /* RS232 serial driver */

#define T5MBDEF_UDP         0x8000  /* flag: UDP */

typedef struct                      /* a MODBUS client with a request */
{
    T5PTR_MBDEFNODE pDef;           /* slave static definition */
    T5PTR_MBREQ     pReq;           /* pointer to the list of requests */
    T5PTR_MBSLAVE   pSlave;         /* list of slaves */
    T5_DWORD        dwReqDelay;     /* current stamp for req delay */
    T5_DWORD        dwCnxDelay;     /* current stamp for cnx delay */
    T5_WORD         wID;            /* slave ID */
    T5_WORD         wReq;           /* index of the pending request */
    T5_WORD         wNbReq;         /* number of requests */
    T5_WORD         wDriver;        /* kind of communication driver */
    T5_WORD         wNbSlave;       /* number of slaves */
    T5_WORD         wFlags;         /* copy of flags */
    T5_DWORD        dwAsc;          /* MODBUS ASCII status */
    T5STR_MBFRAME   frame;          /* frame including status */
#ifdef T5DEF_ETHERNET
    T5_SOCKET       sock;           /* associated socket */
#endif /*T5DEF_ETHERNET*/
#ifdef T5DEF_SERIAL
    T5_SERIAL       ser;            /* associated serial port */
#endif /*T5DEF_SERIAL*/
}
T5STR_MBCLIENT;

typedef T5STR_MBCLIENT * T5PTR_MBCLIENT;

/****************************************************************************/
/* dynamic: variable */

typedef struct                      /* a MODBUS variable in T5 */
{
    T5PTR_MBDEFVAR  pDef;           /* variable exchange static definition */
    T5_PTBYTE       pData;          /* pointer to data in T5 database */
    T5PTR_MBREQ     pReq;           /* pointer to the request */
#ifndef T5DEF_MB_NOLOCK
    T5_PTBYTE       pLock;          /* pointer to lock state bit field */
#endif /*T5DEF_MB_NOLOCK*/
    T5_WORD         wOffset;        /* byte offset in request data */
    T5_WORD         wMask;          /* word mask in request data */
    T5_WORD         wOpe;
    T5_BYTE         bBit;           /* true if BIT request */
    T5_BYTE         bLockMask;      /* mask for lock state */
#ifndef T5DEF_MB_NOVSI
    T5PTR_DBMAPEX   pVSI;           /* pointer to VSI info if any */
#endif /*T5DEF_MB_NOVSI*/
}
T5STR_MBVAR;

typedef T5STR_MBVAR * T5PTR_MBVAR;

/****************************************************************************/
/* dynamic: header */

typedef struct                      /* a MODBUS client with a request */
{
    T5_DWORD       dwAllocSize;     /* full available buffer size */
    T5PTR_MBCLIENT pClient;         /* list of clients */
    T5PTR_MBCLIENT pServer;         /* server node */
    T5PTR_MBSLAVE  pSlave;          /* list of slaves */
    T5PTR_MBREQ    pReq;            /* list of requests */
    T5_PTBYTE      pReqData;        /* raw buffer for request data */
    T5PTR_MBVAR    pVar;            /* list of variables */
    T5_WORD        wNbClient;       /* number of clients */
    T5_WORD        wNbSlave;        /* number of slaves */
    T5_WORD        wNbReq;          /* number of requests */
    T5_WORD        wNbVar;          /* number of variables */
}
T5STR_MBHEAD;

typedef T5STR_MBHEAD * T5PTR_MBHEAD;

/****************************************************************************/
/* for MODBUS ASCII */

#define _T5MBA_ACTIVE           0x80000000L
#define _T5MBA_LASTHERE         0x40000000L
#define _T5MBA_HALFSENT         0x20000000L
#define _T5MBA_LAST             0x0000000fL

#define _T5MBA_ISACTIVE(pdw)    (((*pdw)&_T5MBA_ACTIVE)!=0L)
#define _T5MBA_HASLAST(pdw)     (((*pdw)&_T5MBA_LASTHERE)!=0L)
#define _T5MBA_ISHALFSENT(pdw)  (((*pdw)&_T5MBA_HALFSENT)!=0L)

#define _T5MBA_RESETALL(pdw) \
    { (*pdw) &= _T5MBA_ACTIVE; }
#define _T5MBA_GETLAST(pdw) \
    ((T5_BYTE)((*pdw)&_T5MBA_LAST))
#define _T5MBA_RESETLAST(pdw) \
    { (*pdw) &= ~_T5MBA_LAST; \
      (*pdw) &= ~_T5MBA_LASTHERE; }
#define _T5MBA_SETLAST(pdw,b) \
    { (*pdw) &= ~_T5MBA_LAST; \
      (*pdw) |= (((T5_DWORD)(b))&_T5MBA_LAST); \
      (*pdw) |= _T5MBA_LASTHERE; }
#define _T5MBA_RESETHALFSENT(pdw) \
    { (*pdw) &= ~_T5MBA_HALFSENT; }
#define _T5MBA_SETHALFSENT(pdw) \
    { (*pdw) |= _T5MBA_HALFSENT; }

/****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /*_T5MB_H_INCLUDED_*/

/* eof **********************************************************************/
