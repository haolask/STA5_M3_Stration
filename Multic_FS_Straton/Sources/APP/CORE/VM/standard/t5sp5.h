/*****************************************************************************
T5sp5.h :    SP5 Safe protocol - definitions

DO NOT ALTER THIS !

(c) COPALP 2007
*****************************************************************************/

#ifndef _T5SP5_H_INCLUDED_
#define _T5SP5_H_INCLUDED_

#ifdef T5DEF_SP5

/****************************************************************************/
/* frame to word */

#define SP5_TOHB(x)     ((((T5_WORD)(x)) << 8) & 0xff00)
#define SP5_TOLB(x)     (((T5_WORD)(x)) & 0x00ff)

#ifdef T5DEF_BIGENDIAN

#define SP5_TOW(x)      (SP5_TOHB(x[0]) | SP5_TOLB(x[1]))

#else /*T5DEF_BIGENDIAN*/

#define SP5_TOW(x)      (SP5_TOHB(x[1]) | SP5_TOLB(x[0]))

#endif /*T5DEF_BIGENDIAN*/

/****************************************************************************/
/* unit numbers */

#define SP5_ISBROADCAST(u)  ((u)==0)
#define SP5_ISGROUPCAST(u)  (((u)&0x8000)!= 0)
#define SP5_ISCAST(u)       (SP5_ISBROADCAST(u)||SP5_ISGROUPCAST(u))

/****************************************************************************/
/* limits */

#define SP5MAX_FRAME    T5MAX_FRAME
#define SP5_PAKSIZE     240

typedef T5_BYTE         SP5_FRAME[SP5MAX_FRAME];

#define SP5_MAXPATH     255
typedef T5_CHAR         SP5_PATH[SP5_MAXPATH+1];

#define SP5_MAXPSW      8
typedef T5_CHAR         SP5_PSW[SP5_MAXPSW];

#define SP5_MAXFT       1000

/****************************************************************************/
/* frame identifier and terminator */

#define SP5_H           "SP5"       /* frame header */
#define SP5_H1          'S'
#define SP5_H2          'P'
#define SP5_H3          '5'
#define SP5_HLEN        3

#define SP5_EM          ']'         /* end marker */

/****************************************************************************/
/* kinds of frame */

#define SP5FK_Q1        'Q'         /* question - no slicing */
#define SP5FK_QS        'S'         /* question - slicing enabled */
#define SP5FK_N         'N'         /* question - ask for next ans. packet */
#define SP5FK_A         'A'         /* answer */
#define SP5FK_FE        '?'         /* frame error report */
#define SP5FK_SL        'p'         /* slicing error report */

/****************************************************************************/
/* packet numbering */

#define SP5PAK_SINGLE   0           /* single packet - no slicing */
#define SP5PAK_LAST     0x80        /* OR flag for last packets */
                                    /* packets numbered from 1 to 127 */

/****************************************************************************/
/* file transfer - return checks */

#define SP5FT_OK        0           /* return: OK */
#define SP5FT_ERRPSW    1           /* return: bad password */
#define SP5FT_ERRFNC    2           /* return: cant execute function */
#define SP5FT_ERRARG    3           /* return: bad function arguments */
#define SP5FT_ERRIO     4           /* return: file access error */
#define SP5FT_ERRCOM    5           /* return: comm error */

/****************************************************************************/
/* file transfer - messages */

#define SP5FT_HEAD      "F5"        /* message header */

#define SP5FT_OR        1           /* function: open for read */
#define SP5FT_OW        2           /* function: open for write */
#define SP5FT_CR        3           /* function: close for read */
#define SP5FT_CW        4           /* function: close for write */
#define SP5FT_R1        5           /* function: read random */
#define SP5FT_W1        6           /* function: write append */

/* messages:

    Q: F5 - OR(1) - psw(8) - pathlen(1) - path - knownsize(4)
    A: F5 - OR(1) - rc(1) - size(4) - crc(4)

    Q: F5 - OW(1) - psw(8) - pathlen(1) - path
    A: F5 - OW(1) - rc(1) - size(4) - crc(4)

    Q: F5 - CR(1) - psw(8) - pathlen(1) - path
    A: F5 - CR(1) - rc(1) - size(4) - crc(4)

    Q: F5 - CW(1) - psw(8) - pathlen(1) - path - size(4) - crc(4)
    A: F5 - CW(1) - rc(1) - size(4) - crc(4)

    Q: F5 - R1(1) - psw(8) - offset(4) - len(2)
    A: F5 - R1(1) - rc(1) - offset(4) - len(2) - data

    Q: F5 - W1(1) - psw(8) - offset(4) - len(2) - data
    A: F5 - W1(1) - rc(1)

*/

/****************************************************************************/
/* frame layer */

typedef struct
{
    T5_BYTE         un[2];          /* unit number - big endian */
    T5_BYTE         fk;             /* kind of frame */
    T5_BYTE         fn;             /* frame number */
    T5_BYTE         mn;             /* message number */
    T5_BYTE         pn;             /* packet number */
    T5_BYTE         ds[2];          /* data size big endian */
}
T5STR_SP5H;

typedef T5STR_SP5H  *T5PTR_SP5H;

#define SP5_GETUN(h)    SP5_TOW((h)->un)
#define SP5_GETDS(h)    SP5_TOW((h)->ds)

#define SP5FL_READY 0               /* state machine: nothing to do */
#define SP5FL_PM    1               /* state machine: protocol mark */
#define SP5FL_HEAD  2               /* state machine: header */
#define SP5FL_DATA  3               /* state machine: data */
#define SP5FL_CRC   4               /* state machine: crc */
#define SP5FL_EM    5               /* state machine: end marker */

typedef struct
{
    T5_BYTE         bRcvState;      /* state machine - receiving */
    T5_BYTE         bSndState;      /* state machine - sending */
    T5_WORD         wRcvCount;      /* byte counter - received */
    T5_WORD         wSndCount;      /* byte counter - sent */
    T5_WORD         wRcvCRC;        /* CRC - received */
    T5_WORD         wSndCRC;        /* CRC - sent */
    T5_WORD         res1;
    T5_SERIAL       port;           /* serial port */
}
T5STR_SP5FL;

typedef T5STR_SP5FL *T5PTR_SP5FL;

/****************************************************************************/
/* data link layer */

#define SP5DL_MASTER    0x0001      /* flag: master connection */
#define SP5DL_SLAVE     0x0002      /* flag: slave connection */
#define SP5DL_CANCAST   0x0004      /* flag: slave accepts broad/group cast */

#define SP5DLS_LISTEN   0           /* slave state: listening */
#define SP5DLS_WAITANS  1           /* slave state: wait for app answer */
#define SP5DLS_ANSWER   2           /* slave state: sending answer */
#define SP5DLS_SERVICE  3           /* slave state: sending service frame */

#define SP5DLM_READY    0           /* master state: ready - nothing to do */
#define SP5DLM_SENDING  1           /* master state: sending question */
#define SP5DLM_WAIT     2           /* master state: waiting for answer */

typedef struct
{
    T5_WORD         wFlags;         /* port flags */
    T5_WORD         wUnit;          /* unit number */
    T5_WORD         nFlush;         /* nb of received frames flushed */
    T5_WORD         nError;         /* nb of received bad frames */
    T5_WORD         nOK;            /* nb of received correct frames */
    T5_WORD         nRepeat;        /* nb of frames repeated */
    T5_BYTE         bState;         /* state machine */
    T5_BYTE         bFN;            /* number of last valid answer */
    T5_BYTE         bResetRcv;      /* reset receiver for a new frame */
    T5_BYTE         bResetSnd;      /* reset sender for a new frame */
    T5_DWORD        dwReqStart;     /* time stamp of the current request */
    T5_DWORD        dwTimeout;      /* master: timeout */
    T5_WORD         wNbTrial;       /* master: number of trials */
    T5_WORD         wTrial;         /* master: trial count */
    T5_PTBYTE       pDataSnd;       /* pointer to app data - sent */
    T5_PTBYTE       pDataRcv;       /* pointer to app data - received */
    T5STR_SP5H      hSend;          /* frame header of last sent frame */
    T5STR_SP5H      hWork;          /* current working header */
    T5STR_SP5FL     port;           /* frame layer port */
}
T5STR_SP5DL;

typedef T5STR_SP5DL *T5PTR_SP5DL;

/****************************************************************************/
/* transport layer */

#define SP5TL_SLICE     0x0100      /* flag: slicing enabled */

#define SP5TLS_LISTEN   0           /* slave state: listening */
#define SP5TLS_WAITANS  1           /* slave state: wait for app answer */

#define SP5TLM_READY    0           /* master state: ready */
#define SP5TLM_SENDING  1           /* master state: sending question */
#define SP5TLM_WAIT     2           /* master state: waiting for answer */

typedef struct
{
    T5_WORD         wFlags;         /* port flags */
    T5_WORD         wSize;          /* app data received or full sent size */

    T5_BYTE         bState;         /* state machine */
    T5_BYTE         bMsg;           /* last received message number */
    T5_BYTE         bSlice;         /* slicing is enabled for answer */
    T5_BYTE         bPak;           /* number of last packet */

    SP5_FRAME       dataSnd;        /* sent data buffer */
    SP5_FRAME       dataRcv;        /* received data buffer */
    SP5_FRAME       dataApp;        /* application data buffer */
    T5STR_SP5DL     port;           /* data link layer port */
}
T5STR_SP5TL;

typedef T5STR_SP5TL *T5PTR_SP5TL;

/****************************************************************************/
/* file transport layer - slave */

#define SP5FTLS_IDLE    0           /* file transfer mode: nothing */
#define SP5FTLS_READ    1           /* file transfer mode: reading */
#define SP5FTLS_WRITE   2           /* file transfer mode: writing */

typedef struct
{
    SP5_PATH        szPath;         /* current file path */
    SP5_PSW         szPsw;          /* password */
    T5_BYTE         bMode;          /* current operating mode */
    T5_BYTE         res1;
    T5_BYTE         res2;
    T5_BYTE         res3;
}
T5STR_SP5FTLS;

typedef T5STR_SP5FTLS *T5PTR_SP5FTLS;

/****************************************************************************/
/* file transport layer - master */

#define SP5FTLM_IDLE    0           /* file transfer mode: nothing */
#define SP5FTLM_READ    1           /* file transfer mode: reading */
#define SP5FTLM_WRITE   2           /* file transfer mode: writing */

typedef struct
{
    SP5_PATH        szLocal;        /* current local file path */
    SP5_PATH        szRemote;       /* current remote file path */
    SP5_PSW         szPsw;          /* password */
    T5_DWORD        dwSize;         /* current exchange size */
    T5_DWORD        dwTotal;        /* total file size */
    T5_BYTE         bState;         /* current operating mode */
    T5_BYTE         res1;
    T5_BYTE         res2;
    T5_BYTE         res3;
}
T5STR_SP5FTLM;

typedef T5STR_SP5FTLM *T5PTR_SP5FTLM;

/****************************************************************************/
/* frame layer API */

extern T5_BOOL  T5SP5Frm_Open (T5PTR_SP5FL pFL, T5_PTCHAR szConfig);
extern void     T5SP5Frm_Close (T5PTR_SP5FL pFL);
extern T5_BOOL  T5SP5Frm_Send (T5PTR_SP5FL pFL, T5PTR_SP5H pHeader,
                               T5_PTBYTE pData, T5_BOOL bReset);
extern T5_BOOL  T5SP5Frm_Receive (T5PTR_SP5FL pFL, T5PTR_SP5H pHeader,
                                  T5_PTBYTE pData, T5_BOOL bReset);
extern T5_BOOL  T5SP5Frm_CheckCRC (T5PTR_SP5FL pFL, T5PTR_SP5H pHeader,
                                  T5_PTBYTE pData);

/****************************************************************************/
/* data link layer API - Slave */

extern T5_BOOL  T5SP5Dls_Open (T5PTR_SP5DL pDL, T5_WORD wFlags,
                               T5_WORD wUnit, T5_PTCHAR szConfig,
                               T5_PTBYTE pDataSnd, T5_PTBYTE pDataRcv);
extern void     T5SP5Dls_Close (T5PTR_SP5DL pDL);
extern T5_BOOL  T5SP5Dls_Listen (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader);
extern T5_BOOL  T5SP5Dls_Send (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader);
extern void     T5SP5Dls_DontSend (T5PTR_SP5DL pDL);

/****************************************************************************/
/* data link layer API - Master */

extern T5_BOOL  T5SP5Dlm_Open (T5PTR_SP5DL pDL, T5_WORD wFlags,
                               T5_PTCHAR szConfig,
                               T5_DWORD dwTimeout, T5_WORD wNbTrial,
                               T5_PTBYTE pDataSnd, T5_PTBYTE pDataRcv);
extern void     T5SP5Dlm_Close (T5PTR_SP5DL pDL);
extern T5_BOOL  T5SP5Dlm_Send (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader,
                               T5_DWORD dwTimeStamp);
extern T5_BOOL  T5SP5Dlm_GetAnswer (T5PTR_SP5DL pDL, T5PTR_SP5H pHeader,
                                    T5_DWORD dwTimeStamp, T5_PTBOOL pbOK);
extern void     T5SP5Dlm_AbortRequest (T5PTR_SP5DL pDL);

/****************************************************************************/
/* transport layer API - Slave */

extern T5_BOOL  T5SP5Tls_Open (T5PTR_SP5TL pTL, T5_WORD wFlags,
                               T5_WORD wUnit, T5_PTCHAR szConfig);
extern void     T5SP5Tls_Close (T5PTR_SP5TL pTL);
extern T5_WORD  T5SP5Tls_Listen (T5PTR_SP5TL pTL, T5_WORD wMaxLen,
                                 T5_PTBYTE pData, T5_PTWORD pwUnit);
extern T5_BOOL  T5SP5Tls_Send (T5PTR_SP5TL pTL, T5_WORD wLen, T5_PTBYTE pData);
extern void     T5SP5Tls_DontSend (T5PTR_SP5TL pTL);

/****************************************************************************/
/* transport layer API - Master */

extern T5_BOOL  T5SP5Tlm_Open (T5PTR_SP5TL pTL, T5_WORD wFlags,
                               T5_PTCHAR szConfig,
                               T5_DWORD dwTimeout, T5_WORD wNbTrial);
extern void     T5SP5Tlm_Close (T5PTR_SP5TL pTL);
extern T5_BOOL  T5SP5Tlm_Send (T5PTR_SP5TL pTL, T5_WORD wUnit,
                               T5_WORD wLen, T5_PTBYTE pData,
                               T5_DWORD dwTimeStamp);
extern T5_WORD  T5SP5Tlm_GetAnswer (T5PTR_SP5TL pTL, T5_WORD wMaxLen,
                                    T5_PTBYTE pData, T5_PTWORD pwUnit,
                                    T5_DWORD dwTimeStamp, T5_PTBOOL pbOK);
extern void     T5SP5Tlm_AbortRequest (T5PTR_SP5TL pTL);

/****************************************************************************/
/* file transfer layer API - Slave */

extern T5_BOOL  T5SP5Ftls_Open (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                T5_PTCHAR szPassword,
                                T5_WORD wFlags, T5_WORD wUnit,
                                T5_PTCHAR szConfig);
extern void     T5SP5Ftls_SetPassword (T5PTR_SP5FTLS pFT,
                                       T5_PTCHAR szPassword);
extern void     T5SP5Ftls_Close (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT);
extern T5_WORD  T5SP5Ftls_Listen (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                  T5_WORD wMaxLen, T5_PTBYTE pData,
                                  T5_PTWORD pwUnit);
extern T5_BOOL  T5SP5Ftls_Send (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                T5_WORD wLen, T5_PTBYTE pData);
extern void     T5SP5Ftls_DontSend (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT);

/****************************************************************************/
/* file transfer layer API - Master */

extern T5_BOOL  T5SP5Ftlm_Open (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                T5_WORD wFlags, T5_PTCHAR szConfig,
                                T5_DWORD dwTimeout, T5_WORD wNbTrial);
extern void     T5SP5Ftlm_Close (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT);
extern T5_BOOL  T5SP5Ftlm_Send (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                T5_WORD wUnit, T5_WORD wLen, T5_PTBYTE pData,
                                T5_DWORD dwTimeStamp);
extern T5_WORD  T5SP5Ftlm_GetAnswer (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                     T5_WORD wMaxLen, T5_PTBYTE pData,
                                     T5_PTWORD pwUnit, T5_DWORD dwTimeStamp,
                                     T5_PTBOOL pbOK);
extern void     T5SP5Ftlm_AbortRequest (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT);
extern T5_BOOL  T5SP5Ftlm_SendFile (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                    T5_PTCHAR szLocal, T5_PTCHAR szRemote,
                                    T5_PTCHAR szPassword,
                                    T5_WORD wUnit, T5_DWORD dwTimeStamp);
extern T5_BOOL  T5SP5Ftlm_RcvFile (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                   T5_PTCHAR szLocal, T5_PTCHAR szRemote,
                                   T5_PTCHAR szPassword,
                                   T5_WORD wUnit, T5_DWORD dwTimeStamp);

/****************************************************************************/
/* file transfer layer API - Common services */

extern T5_BOOL T5SP5Ftls_SeekTo (T5_LONG f, T5_DWORD dwOffset);

extern T5_DWORD T5SP5Ftls_FileCrcUpTo (T5_PTCHAR szPath, T5_DWORD dwKnownSize);
extern T5_DWORD T5SP5Ftls_Crc (T5_WORD wLen, T5_PTBYTE pData, T5_DWORD dwCrc);

/****************************************************************************/

#endif /*T5DEF_SP5*/

#endif /*_T5SP5_H_INCLUDED_*/

/* eof **********************************************************************/

