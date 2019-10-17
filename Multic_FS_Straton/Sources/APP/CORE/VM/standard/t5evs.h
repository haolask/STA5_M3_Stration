/*****************************************************************************
T5evs.h :    Event Server - definitions

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#ifndef _T5EVS_H_INCLUDED_
#define _T5EVS_H_INCLUDED_

#ifdef T5DEF_EVENTSERVER

#ifndef T5MAX_EAFRM
#define T5MAX_EAFRM T5MAX_FRAME
#endif /*T5MAX_EAFRM*/

#ifndef T5MAX_EACLIFRM
#define T5MAX_EACLIFRM T5MAX_FRAME
#endif /*T5MAX_EACLIFRM*/

/****************************************************************************/
/* protocol

srv> 'T' | '5' | Nb(2) | 18 | NbEv | Event - { Event }
ack> 'T' | '5' | Nb(2) | 18 | Pref | Password(4)

evt= bClass | bID | bLen | bRes | dwDate(4) | dwTime(4) | data

******************************************************************************
** EA event data:

event (2)
type (1)
index (2)
value (var)
CToffset (4) [optional]
VSIbits (8) [optional]

*****************************************************************************/
/* constants */

#define T5EVS_EAMAXVAL   (255 - 32)

/* period of idle messages = 2 seconds */

#ifndef T5EVS_IDLEPERIOD
#define T5EVS_IDLEPERIOD 2000
#endif /*T5EVS_IDLEPERIOD*/

/* idle timeout */

#ifndef T5EVS_TIMEOUT
#define T5EVS_TIMEOUT (T5EVS_IDLEPERIOD+1000)
#endif /*T5EVS_TIMEOUT*/

/****************************************************************************/
/* event classes and wishes */

#define T5EVS_SYSTEM        0x01
#define T5EVS_ALARM         0x02
#define T5EVS_EVENT         0x04
#define T5EVS_INFO          0x08

#define T5EVS_NOACK         0x20    /* client wish: will not send ack */
#define T5EVS_VSI           0x40    /* client wish: want VSI bits */
#define T5EVS_NOBROADCAST   0x80    /* client wish: don't want broadcasts */

#define T5EVS_STRING        0x80    /* event mask: string for next event */

/****************************************************************************/
/* predefined events - all optional broadcasts must be 255 */

#define T5EV_OPTBROADCAST   255 /* optional broadcast event */

#define T5EV_IDLE           0   /* INFO: idle (keep alive) - no arg */
#define T5EV_TRACE          1   /* INFO: general trace - szText */

#define T5EV_QOV_SYSTEM     1   /* SYSTEM: system queue overflow - wNbLost */
#define T5EV_QOV_ALARM      2   /* SYSTEM: alarm queue overflow - wNbLost */
#define T5EV_QOV_EVENT      3   /* SYSTEM: event queue overflow - wNbLost */
#define T5EV_QOV_INFO       4   /* SYSTEM: info queue overflow - wNbLost */
#define T5EV_APPSTART       5   /* SYSTEM: app start - szAppName */
#define T5EV_APPSTOP        6   /* SYSTEM: app stop - szAppName */
#define T5EV_APPCHANGE      7   /* SYSTEM: app on line change - szAppName */
#define T5EV_EAOPEN         8   /* SYSTEM: EA server starts */
#define T5EV_EACLOSE        9   /* SYSTEM: EA server stops */

#define T5EV_EVBROADCAST    T5EV_OPTBROADCAST /* EVENT: broadcast event */
#define T5EV_EVPRIVATE      2   /* EVENT: peer to peer event */

/****************************************************************************/
/* basic event descriptor */

typedef struct                  /* common event header */
{
    T5_BYTE   bClass;           /* class of event */
    T5_BYTE   bID;              /* ID of the event */
    T5_BYTE   bLen;             /* number of bytes in argument data */
    T5_BYTE   bCnx;             /* connection ID */
    T5_DWORD  dwDate;           /* date stamp */
    T5_DWORD  dwTime;           /* time stamp */
}
T5STR_EVENT;

typedef T5STR_EVENT *T5PTR_EVENT;

/****************************************************************************/
/* message queue */

#define T5EVQUEUE_EMPTY 0x0001  /* flag: event queue is empty */

typedef struct                  /* event queue descritor */
{
    T5_DWORD  dwSize;           /* full size of buffer */
    T5_DWORD  dwPass;           /* password */
    T5_PTBYTE pData;            /* base pointer to buffer */
    T5_DWORD  dwRead;           /* read pointer */
    T5_DWORD  dwWrite;          /* write pointer */
    T5_WORD   wFlags;           /* status flags */
    T5_WORD   wLost;            /* number of lost events */
    T5_WORD   wLostSent;        /* number of lost events notified */
    T5_WORD   res;
}
T5STR_EVQUEUE;

typedef T5STR_EVQUEUE *T5PTR_EVQUEUE;

/****************************************************************************/
/* callback */

typedef void (*T5HND_EVSCB)(T5_BYTE bClass, T5_BYTE bID,
                            T5_BYTE bLen, T5_PTBYTE pData);

#define T5MAX_EVSCB     8

/****************************************************************************/
/* event manager data */

#define T5EVSTAT_LISTEN     0   /* no client here */
#define T5EVSTAT_READY      1   /* idle */
#define T5EVSTAT_SENDING    2   /* sending data */
#define T5EVSTAT_RECEIVING  3   /* wait for login/ack telegram */

typedef struct
{
    /* messages queues */
    T5STR_EVQUEUE qSystem;      /* system queue */
    T5STR_EVQUEUE qAlarm;       /* alarm queue */
    T5STR_EVQUEUE qEvent;       /* event queue */
    T5STR_EVQUEUE qInfo;        /* info queue */
    /* track lost messages */
    T5_WORD   wNbLostSystem;    /* system events lost */
    T5_WORD   wNbLostAlarm;     /* alarm events lost */
    T5_WORD   wNbLostEvent;     /* event events lost */
    T5_WORD   wNbLostInfo;      /* info events lost */
    /* sockets */
    T5_SOCKET sockListen;       /* listening socket */
    T5_SOCKET sockData;         /* socket for data exchange */
    T5_BYTE   bf[T5MAX_EAFRM];  /* snd/rcv buffer */
    T5_WORD   wBSize;           /* buffer current size */
    T5_WORD   wBPakSize;        /* buffer full or expected size */
    T5_DWORD  dwLastSent;       /* time stamp of last sent frame */
    /* callbacks */
    T5HND_EVSCB cb[T5MAX_EVSCB];
    /* client status */
    T5_WORD   wRights;          /* allowed and wished classes */
    T5_WORD   wStatus;          /* current communication status */
    T5_WORD   wID;              /* connection ID */
    T5_WORD   res;
    T5_DWORD  dwMaxBuf;         /* client buffer size */
}
T5STR_EVS;

typedef T5STR_EVS *T5PTR_EVS;

/****************************************************************************/
/* Main descriptor for memory allocation and open */

typedef struct
{
    T5_DWORD  dwQSSystem;       /* size of system queue */
    T5_DWORD  dwQSAlarm;        /* size of alarm queue */
    T5_DWORD  dwQSEvent;        /* size of event queue */
    T5_DWORD  dwQSInfo;         /* size of info queue */
    T5_DWORD dwPswSystem;       /* password: system level */
    T5_DWORD dwPswAlarm;        /* password: alarm level */
    T5_DWORD dwPswEvent;        /* password: event level */
    T5_DWORD dwPswInfo;         /* password: info level */
    T5_WORD  wPort;             /* ethernet port number */
}
T5STR_EVSETTINGS;

typedef T5STR_EVSETTINGS *T5PTR_EVSETTINGS;

/****************************************************************************/
/* Multi client event server */

#ifdef T5MAX_EVCLIENT

#define T5EVSS_NEWCNX       0x0001  /* new connection (used by EA server) */
#define T5EVSS_FROZEN       0x0002  /* production is frozen */

typedef struct                      /* server data header */
{
    T5_DWORD  dwEvSize;             /* memory size for one connection */
    T5_WORD   wPort;                /* port number */
    T5_WORD   wFlags;               /* flags */
    T5_SOCKET sockListen;           /* listening socket */
    T5HND_EVSCB cb[T5MAX_EVSCB];    /* callback functions */
}
T5STR_EVSS;

typedef T5STR_EVSS *T5PTR_EVSS;

#endif /*T5MAX_EVCLIENT*/

/****************************************************************************/
/* Value change events - static definitions */

#ifdef T5DEF_EA

#define T5EAOPE_XV      1           /* operation: data exchange */
#define T5EAOPE_CS      2           /* operation: connection status */
#define T5EAOPE_VS      3           /* operation: variable status */
#define T5EAOPE_VT      4           /* operation: variable time stamp */
#define T5EAOPE_VD      5           /* operation: variable data stamp */
#define T5EAOPE_MCS     6           /* operation: multiple connection status */
#define T5EAOPE_MCIDX   0x70        /* MCS mask */
#define T5EAOPE_CTSEG   0x80        /* ope mask if CTSeg variable */

#define T5EAOPE_ISCS(s) (((s)==T5EAOPE_CS)||(((s)&~T5EAOPE_MCIDX)==T5EAOPE_MCS))
#define T5EAOPE_MCSINDEX(s) (((s)>>4)&0x07)

typedef struct                      /* exchanged variable (consumed) */
{
    T5_BYTE bTicType;               /* data type - see t5tic.h */
    T5_BYTE bOpe;                   /* data exchange - operation */
                                    /* possibly masked with T5EAOPE_CTSEG */
    T5_WORD wOffset;                /* variable offset */
    T5_WORD wEvent;                 /* event ID */
                                    /* continue with 4 byte CTSeg offset */
                                    /* if wOffset == 0 */
}
T5STR_EADEFVAR;

typedef T5STR_EADEFVAR *T5PTR_EADEFVAR;

#define T5STR_EADEFVAR_SIZEOF 6

typedef struct                      /* exchanged variable (pubished) */
{
    T5_BYTE bTicType;               /* data type - see t5tic.h */
    T5_BYTE bOpe;                   /* data exchange - operation */
    T5_WORD wOffset;                /* variable offset */
    T5_WORD wEvent;                 /* event ID */
    T5_WORD wHSizeof;               /* size of hyst value */
                                    /* continue with HystP and HystN */
                                    /* continue with 4 byte CTSeg offset */
                                    /* if wOffset == 0 */
}
T5STR_EADEFVARH;

typedef T5STR_EADEFVARH *T5PTR_EADEFVARH;

#define T5EA_RECONNECT  0x0001      /* flag: reconnect after fail */
#define T5EA_REDMASTER  0x0010      /* redundant network (master) */
#define T5EA_REDSLAVE   0x0020      /* redundant network (slave) */
#define T5EA_REDRT      0x0040      /* redundant network to redundant RTs */

typedef struct                      /* client connection */
{
    T5_WORD wLen;                   /* structure length in bytes */
    T5_WORD wPort;                  /* TCP port */
    T5_WORD wFlags;                 /* flags */
    T5_WORD wVar1;                  /* index of first variable */
    T5_WORD wNbVar;                 /* nb connected variables */
    T5_CHAR szAddr[2];              /* TCP address (variable length) */
}
T5STR_EADEFCNX;

typedef T5STR_EADEFCNX *T5PTR_EADEFCNX;

#endif /*T5DEF_EA*/

/****************************************************************************/
/* produced events - dynamic */

#ifdef T5DEF_EA

#ifdef T5MAX_EVCLIENT
#define T5MAX_EACNX         T5MAX_EVCLIENT
#else /*T5MAX_EVCLIENT*/
#define T5MAX_EACNX         4
#endif /*T5MAX_EVCLIENT*/

#define _T5DEFEACNX_NONE    0       /* connection not established */
#define _T5DEFEACNX_OK      1       /* everything OK */
#define _T5DEFEACNX_FAILED  2       /* fail on last send - resend needed */

#define _T5DEFEACNX_STATUS  0x03

#define T5DEFEACNX_ISNONE(s) \
    (((s) & _T5DEFEACNX_STATUS) == _T5DEFEACNX_NONE)
#define T5DEFEACNX_ISOK(s) \
    (((s) & _T5DEFEACNX_STATUS) == _T5DEFEACNX_OK)
#define T5DEFEACNX_ISFAILED(s) \
    (((s) & _T5DEFEACNX_STATUS) == _T5DEFEACNX_FAILED)

#define T5DEFEACNX_SETNONE(s) \
    (s) = (((s) & ~_T5DEFEACNX_STATUS) | _T5DEFEACNX_NONE)
#define T5DEFEACNX_SETOK(s) \
    (s) = (((s) & ~_T5DEFEACNX_STATUS) | _T5DEFEACNX_OK)
#define T5DEFEACNX_SETFAILED(s) \
    (s) = (((s) & ~_T5DEFEACNX_STATUS) | _T5DEFEACNX_FAILED)

#define T5EA_DEFINED        0x0001  /* TRUE if already sent */
#define T5EA_HASFAIL        0x0002  /* at least one cnx failed */
#define T5EA_BROADCAST      0x0004  /* broadcast to all clients */
#define T5EA_HYST           0x0008  /* hysteresis defined */

typedef struct
{
    T5_BYTE bSizeOf;                    /* size of bValue (1..8) */
    T5_BYTE bTicType;                   /* TIC data format */
    T5_WORD wFlags;                     /* flags */
    T5_WORD wOffset;                    /* offset in VMDB */
    T5_WORD wNbCnx;                     /* number of connected clients */
    T5_WORD wID;                        /* configurated ID */
    T5_WORD res;
    T5_DWORD dwCTOffset;
    T5_PTBYTE pValue;                   /* pointer to value in VMDB */
#ifdef T5DEF_VARMAP
    T5PTR_DBMAPEX pVSI;                 /* pointer to VSI info */
    T5_DWORD bLastVSIHi;                /* last value */
    T5_DWORD bLastVSILo;                /* last value */
#endif /*T5DEF_VARMAP*/
    T5_BYTE bLast[8];                   /* last value */
#ifdef T5DEF_EAHYSTER
    T5_BYTE bHP[8];                     /* positive hysteresis */
    T5_BYTE bHN[8];                     /* negative hysteresis */
#endif /*T5DEF_EAHYSTER*/
    T5_BYTE bCnxState[T5MAX_EACNX];     /* attached connections */
}
T5STR_EACHANGE;

typedef T5STR_EACHANGE *T5PTR_EACHANGE;

#endif /*T5DEF_EA*/

/****************************************************************************/
/* consumed events - dynamic */

#ifdef T5DEF_EA
#ifdef T5DEF_ETHERNET

#define T5EA_ERROR      0           /* status: on error - don't retry */
#define T5EA_CONNECTING 1           /* status: connecting to server */
#define T5EA_RECEIVING  2           /* status: waiting for event */
#define T5EA_SENDING    3           /* status: sending acknoledge */
#define T5EA_TIMEOUT    4           /* status: timeout when receiving */

typedef struct
{
    T5_PTBYTE  pData;               /* pointer to T5 variable */
    T5_PTBYTE  pLock;               /* pointer to lock mask */
#ifdef T5DEF_VARMAP
    T5PTR_DBMAPEX pVSI;             /* pointer to VSI info */
#endif /*T5DEF_VARMAP*/
    T5_DWORD   dwTimeStamp;         /* time stamp */
    T5_DWORD   dwDateStamp;         /* date stamp */
    T5_DWORD   dwCTOffset;          /* T5 variable CT offset if any */
    T5_WORD    wOffset;             /* T5 variable offset */
    T5_WORD    wEvent;              /* event ID */
    T5_BYTE    bLockMask;           /* mask for lock state */
    T5_BYTE    bOpe;                /* data exchange operation */
    T5_BYTE    bDstType;            /* type of T5 variable */
    T5_BYTE    res1;
}
T5STR_EAEXT;

typedef T5STR_EAEXT     *T5PTR_EAEXT;

#define T5EA_APPHERE    0x0001      /* extern application is running */

typedef struct
{
    T5_PTR      pDef;               /* pointer to static definition */
    T5_SOCKET   sock;               /* associated socket */
    T5PTR_EAEXT pVar;               /* pointer to the first variable */
    T5PTR_EAEXT pVarAux;            /* pointer to the redundant variables */
    T5_BYTE     bfIn[T5MAX_EACLIFRM];  /* rcv buffer */
    T5_BYTE     bfOut[16];          /* snd buffer - ack = 10 bytes */
    T5_DWORD    dwLastIn;           /* time stamp of last bytes received */
    T5_WORD     wStatus;            /* connection status */
    T5_WORD     wPacketIn;          /* full expected rcv frame size */
    T5_WORD     wSizeIn;            /* number of bytes received */
    T5_WORD     wSizeOut;           /* number of bytes sent */
    T5_WORD     wNbVar;             /* number of child variables */
    T5_WORD     wFlags;             /* flags */
}
T5STR_EACNX;

typedef T5STR_EACNX     *T5PTR_EACNX;

#endif /*T5DEF_ETHERNET*/
#endif /*T5DEF_EA*/

/****************************************************************************/

#endif /*T5DEF_EVENTSERVER*/

#endif /*_T5EVS_H_INCLUDED_*/

/* eof **********************************************************************/
