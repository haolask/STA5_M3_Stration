/*****************************************************************************
T5VMsrv.h :  VM definitions for data server

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#ifndef _T5VMSRV_H_INCLUDED_
#define _T5VMSRV_H_INCLUDED_

/****************************************************************************/
/* protocol */

#define T5REQ_INIT          1   /* re-init the communication (unused) */
#define T5REQ_READ          2   /* read objects */
#define T5REQ_EXECUTE       3   /* execute TIC sequence */
#define T5REQ_SUBSCRIBE     4   /* subscribe to events */
#define T5REQ_UNSUBSCRIBE   5   /* unsubscribe from events */
#define T5REQ_SUBSCRIBEL    6   /* subscribe to events - accept CTSeg */
#define T5REQ_UNSUBSCRIBEL  7   /* unsubscribe from events  - accept CTSeg */
#define T5REQ_READTABLE     8   /* polling - table segments */

/****************************************************************************/
/* read requests - data types */

#define T5T_APPNAME     1   /* 16 char string: app name */
#define T5T_APPSTATUS   2   /* word: app  status */
#define T5T_CODESFX     3   /* 4 char string: code suffix */
#define T5T_VERSION     4   /* dword: app version */
#define T5T_DATESTAMP   5   /* dword: app dat/time stamp */
#define T5T_CRC         6   /* dword */
#define T5T_TC          7   /* dword */
#define T5T_STEPPING    8   /* dword (reserved) */

#define T5T_BYTE        9   /* 1+byte: (lock+) bool or sint */
#define T5T_WORD        10  /* 1+word: (lock+) reserved */
#define T5T_DWORD       11  /* 1+dword: (lock+) dint or real */
#define T5T_LWORD       12  /* 1+64word: (lock+) lint or lreal */
#define T5T_TIME        13  /* 1+dword: (lock+) time */
#define T5T_STRING      14  /* 1+len+chars+\0: (lock+) string */
#define T5T_PROG        15  /* byte: status */

#define T5T_LOG         16  /* word + word + dword */

#define T5T_MISSING     17  /* len+chars+\0 */

#define T5T_STEP        18  /* 2 bytes: activity + breakpoint */
#define T5T_TRANS       19  /* 2 bytes: validity + breakpoint */

#define T5T_X_D8        20  /* 8 bit external variable */
#define T5T_X_D16       21  /* 16 bit external variable */
#define T5T_X_D32       22  /* 32 bit external variable */
#define T5T_X_D64       23  /* 64 bit external variable */
#define T5T_X_TIME      24  /* TIME external variable */
#define T5T_X_STRING    25  /* STRING external variable */

#define T5T_BSMPSET     26  /* sampling trace - settings */
#define T5T_BSMPDATA    27  /* sampling trace - data */

#define T5T_MAP         28  /* map object */
#define T5T_CALLSTACK   29  /* call stack item */
#define T5T_ASIPRF      30  /* ASi profiles: 256 bytes */

#define T5T_CTSEG       31  /* CTSEG data */
#define T5T_CTTYPEDEF   32  /* CT type definition: index is pak of 512 byte */

#define T5T_STEPT       33  /* 6 bytes: activity + bkp + activity duration */

/****************************************************************************/
/* flags echoed by T5T_CTTYPEDEF */

#define T5T_BIGENDIAN   0x0001

/****************************************************************************/
/* index: T5T_CRC */

#define T5T_CRCDATA     0   /* index for T5T_CRC: data */
#define T5T_CRCCODE     1   /* index for T5T_CRC: code */

/****************************************************************************/
/* index: T5T_TC */

#define T5T_TCCURRENT   0   /* index for T5T_TC: last measured cyle */
#define T5T_TCTRIGGER   1   /* index for T5T_TC: cycle programmed period */
#define T5T_TCMAX       2   /* index for T5T_TC: maximum */
#define T5T_TCOVER      3   /* index for T5T_TC: nb of overflows */

/****************************************************************************/
/* index: T5T_CALLSTACK */

#define T5T_BKPOFFSET   128 /* pseudo call stack offset of 1rst breakpoint */

/****************************************************************************/
/* sys info */

#define T5T_SYSFLAGSEX  1       /* extended status flags */
#define T5T_SYSCAPS1    2       /* capabilities - flags */
#define T5T_SYSCAPS2    3       /* capabilities - flags */
#define T5T_SYSVERS     4       /* version mark (from T5DK) */
#define T5T_SYS_FB      5       /* FB - get first */
#define T5T_SYS_FB_NX   6       /* FB - get next */
#define T5T_SYS_NAME    7       /* T5DK project name */
#define T5T_SYS_AUTHOR  8       /* T5DK author */
#define T5T_SYS_OXC     9       /* OXC code */
#define T5T_SYS_LOCKLST 10      /* list of locked variables maybe not complete  */
#define T5T_SYS_DBSIZE  11      /* VMDB size */
#define T5T_SYS_HEAPF   12      /* heap free size */
#define T5T_SYS_ELP_S   13      /* seconds elapsed since startup */
#define T5T_SYS_ELP_MS  14      /* rest (milliseconds) elapsed since startup */
#define T5T_SYS_NBSMLK  15      /* number of records in smart locked table */

#define T5T_SYS_SMLK    0xf000  /* smart lock table record (base) */

/****************************************************************************/

#endif /*_T5VMSRV_H_INCLUDED_*/

/* eof **********************************************************************/

