/*****************************************************************************
T5cs.h :     Communication Server - definitions

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#ifndef _T5CS_H_INCLUDED_
#define _T5CS_H_INCLUDED_

/****************************************************************************/
/* node discovery UDP port */

#define T5CS_DISCPORT       0x2ba5 /* 11173 */

/****************************************************************************/
/* serial protocol info */

#define T5SERINFO_T5        1   /* protocol = T5 / MODBUS RTU Slave */
#define T5SERINFO_SP5       2   /* protocol = SP5 remote access protocol */
#define T5SERINFO_SERIO     3   /* protocol = SERIO function block */
#define T5SERINFO_MBM       4   /* protocol = MODBUS RTU Master */
#define T5SERINFO_MBS       5   /* protocol = MODBUS RTU Slave FB */
#define T5SERINFO_SND       10  /* frame sent */
#define T5SERINFO_RCV       11  /* frame received */
#define T5SERINFO_UNEXPRCV  12  /* unexpected frame received - flushed */

#define T5SERINFO_P_T5      101   /* before open = T5 / MODBUS RTU Slave */
#define T5SERINFO_P_SP5     102   /* before open = SP5 remote protocol */
#define T5SERINFO_P_SERIO   103   /* before open = SERIO function block */
#define T5SERINFO_P_MBM     104   /* before open = MODBUS RTU Master */
#define T5SERINFO_P_MBS     105   /* before open = MODBUS RTU Slave FB */

/****************************************************************************/
/* Frame header */

#ifndef T5CS_HEAD1
#define T5CS_HEAD1  'T'
#endif /*T5CS_HEAD1*/

#ifndef T5CS_HEAD2
#define T5CS_HEAD2  '5'
#endif /*T5CS_HEAD2*/

#ifndef T5CS_ISHEAD
#define T5CS_ISHEAD(s)  (((T5_BYTE)(s[0]) == (T5_BYTE)T5CS_HEAD1) \
                        && ((T5_BYTE)(s[1]) == (T5_BYTE)T5CS_HEAD2))
#endif /*T5CS_ISHEAD*/

/****************************************************************************/
/* Callback on comm events */

typedef void (*T5HND_CS)(void);

/****************************************************************************/
/* port descriptor - flags */

#define T5CS_READY      0x0001  /* port is ready (open) */
#define T5CS_DATAIN     0x0002  /* data IN is available (complete frame) */
#define T5CS_DATAOUT    0x0004  /* data OUT is here (complete frame) */

#define T5CS_RECEIVING  0x0010  /* port is receiving a frame */
#define T5CS_SENDING    0x0020  /* port is sending a frame */

#define T5CS_SOCKET     0x0100  /* port is a socket */
#define T5CS_SERIAL     0x0200  /* port is a serial port */
#define T5CS_SP5        0x0400  /* port is a safe protocol */
#define T5CS_M5         0x0800  /* port is T5/M5 - also used for CS */

/****************************************************************************/
/* port descriptor */

typedef struct
{
    T5_WORD wID;                /* port ID (given by the server) */
    T5_WORD wFlags;             /* status */
    T5_WORD wTaskNo;            /* M5 task number */

    T5_WORD wSizeIn;            /* nb bytes received */
    T5_WORD wPakSizeIn;         /* expected rcv full frame size */
    T5_WORD wSizeOut;           /* nb bytes sent */
    T5_WORD wPakSizeOut;        /* full snd frame size */

    T5_BOOL bM5Req;             /* last data in was M5 */
    T5_BYTE bReqID;             /* M5 request ID echoed */

    T5_BYTE bfIn[T5MAX_FRAME+12];  /* rcv buffer */
    T5_BYTE bfOut[T5MAX_FRAME+12]; /* snd buffer */

    T5_DWORD dwTimeStamp;       /* stamp for checking silent sockets */

#ifdef T5DEF_ETHERNET
    T5_SOCKET sock;             /* associated socket */
#endif /*T5DEF_ETHERNET*/

#ifdef T5DEF_SERIAL
    T5_SERIAL ser;              /* associated serial port */
#endif /*T5DEF_SERIAL*/

} T5STR_CSPORT;

typedef T5STR_CSPORT * T5PTR_CSPORT;

/****************************************************************************/
/* CS database */

typedef struct
{
    T5_DWORD dwFTHandle;        /* file transfer: current file */
    T5_DWORD dwFTCrc;           /* file transfer: file CRC */
    T5_DWORD dwFTDirHandle;     /* file transfer: current directory */
    T5_DWORD dwFlags;           /* flags */
    T5_DWORD dwStamp;           /* last activation stamp */
    T5_DWORD dwCan;             /* CAN gateway */

    T5HND_CS pfCallback;        /* callback routine (can be NULL) */
    T5STR_CSPORT csPort[T5MAX_PORT]; /* port descriptors */

#ifdef T5DEF_ETHERNET
    T5_SOCKET sock;             /* listening socket */
#endif /*T5DEF_ETHERNET*/

#ifdef T5DEF_CSDISC
    T5_SOCKET sockDisc;         /* UDB listening socket for discovery */
#endif /*T5DEF_CSDISC*/

#ifdef T5DEF_SP5
    T5STR_SP5TL   sp5;          /* safe protocol data */
    T5STR_SP5FTLS sp5ft;        /* safe protocol - file transfer data */
#endif /*T5DEF_SP5*/

    T5_WORD wcsPort;
} T5STR_CS;

typedef T5STR_CS * T5PTR_CS;

#ifndef T5CS_SP5PASSWORD
#define T5CS_SP5PASSWORD "\0"
#endif  /*T5CS_SP5PASSWORD*/

/****************************************************************************/
/* Communication server settings */

typedef struct
{
    T5_WORD wEthernetPort;      /* socket port or 0 if no socket */
    T5_WORD wUnitSP5;           /* SP5 unit number */

    T5_PTCHAR szSerialConf;     /* serial port config. or NULL if no serial */
} T5STR_CSSET;

typedef T5STR_CSSET * T5PTR_CSSET;

/****************************************************************************/
/* Protocol */

#define T5CSRQ_VMREQ        1   /* request served by the VM */
#define T5CSRQ_BEGINLOAD	2   /* start download */
#define T5CSRQ_LOAD			3   /* download */
#define T5CSRQ_ENDLOAD		4   /* end of download */
#define T5CSRQ_STOP         5   /* stop application */
#define T5CSRQ_START        6   /* cold / warm start */
#define T5CSRQ_HOTSTART     7   /* hot restart */
#define T5CSRQ_CHANGE       8   /* on line change */

#define T5CSRQ_MODBUSRTU    16  /* MODBUS RTU frame on RS link */
#define T5CSRQ_MODBUSTCP    17  /* MODBUS frame on ETHERNET */
#define T5CSRQ_EVS          18  /* Event Manager */
#define T5CSRQ_FT           19  /* File transfer */
#define T5CSRQ_REG          20  /* Parameter registry protocol */
#define T5CSRQ_CS           21  /* CS command */
#define T5CSRQ_BINDUDP      22  /* Binding on UDP */
#define T5CSRQ_CAN          23  /* CAN gateway */
#define T5CSRQ_SYNCCODE     24  /* code synchronization */

#define T5CSRQ_USER         128 /* oem defined request */

/****************************************************************************/
/* CCAN gateway protocol */

#define T5CSCAN_2B          0x01
#define T5CSCAN_RTR         0x02

/****************************************************************************/
/* Download protocol */

#define T5CSLOADCODE        1   /* load: application code */
#define T5CSLOADSYMBOL      2   /* load: symbols */
#define T5CSLOADCHANGE      3   /* load: on line change */
#define T5CSLOADSYNC        4   /* load: synchronize code */

typedef struct                  /* header of a download request */
{
    T5_BYTE   bType;            /* type of loaded file */
    T5_BYTE   res1;
    T5_BYTE   res2;
    T5_BYTE   res3;
    T5_DWORD  dwSize;           /* file size or packet size */
    T5_PTCHAR szName;           /* application name */
    T5_PTR    pData;            /* pointer to request data */
} T5STR_CSLOAD;

typedef T5STR_CSLOAD *T5PTR_CSLOAD;

/****************************************************************************/
/* start protocol */

typedef struct                  /* header of a start request */
{
    T5_BOOL   bWarm;            /* warm start */
    T5_BOOL   bCC;              /* cycle stepping mode */
    T5_BYTE   res1;
    T5_BYTE   res2;
    T5_PTCHAR szName;           /* application name */
} T5STR_CSSTART;

typedef T5STR_CSSTART *T5PTR_CSSTART;

/****************************************************************************/
/* file transfer - definitions */

#define T5FT_OW             1       /* function= start write */
#define T5FT_XW             2       /* function= write packet*/
#define T5FT_CW             3       /* function= end write */
#define T5FT_OR             4       /* function= start read */
#define T5FT_XR             5       /* function= read packet */
#define T5FT_CR             6       /* function= end read */
#define T5FT_OD             7       /* function= start read directory */
#define T5FT_XD             8       /* function= read dierctory entry */
#define T5FT_CD             9       /* function= end read directory */
#define T5FT_RM             10      /* function= remove file */

#define T5FT_PAKSIZE        800     /* packet size for read */

#define T5FTERR_UNKNOWN     1       /* error: unknown request */
#define T5FTERR_OPENWRITE   2       /* error: cant open for write */
#define T5FTERR_WRITE       3       /* error: cant write */
#define T5FTERR_CLOSE       4       /* error: cant close file */
#define T5FTERR_OPENREAD    5       /* error: cant open for read */
#define T5FTERR_READ        6       /* error: cant read */
#define T5FTERR_DELETE      7       /* error: cant remove file */
#define T5FTERR_OPENDIR     8       /* error: cant browse directory */
#define T5FTERR_CLOSEDIR    9       /* error: cant close directory */
#define T5FTERR_GETDIR      10      /* error: cant read directory entry */

/****************************************************************************/

#endif /*_T5CS_H_INCLUDED_*/

/* eof **********************************************************************/

