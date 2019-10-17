/*****************************************************************************
T5mm.h :     memory manager - definitions

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#ifndef _T5MM_H_INCLUDED_
#define _T5MM_H_INCLUDED_

/****************************************************************************/
/* memory block identitification */

#define T5MM_CODE       1
#define T5MM_VMDB       2
#define T5MM_CHANGE     3

/****************************************************************************/
/* memory block information */

#define T5MMB_LOADED    0x0001  /* memory has been loaded or created */
#define T5MMB_LINKED    0x0002  /* at least one client linked */

typedef struct
{
    T5_WORD     wID;            /* block identifier */
    T5_WORD     wFlags;         /* flags for memory */
    T5_WORD     wNbLink;        /* number of linked clients */
    T5_WORD     wChanged;       /* non zero if changed */
    T5_DWORD    dwSize;         /* sizeof memory block */
    T5_PTR      pData;          /* pointer to linked memory */
    T5_MEM      mem;            /* handle of memory (system info) */
} T5STR_MMB;

typedef T5STR_MMB *T5PTR_MMB;

/****************************************************************************/
/* memory manager private information */

typedef struct
{
    T5_PTR      pConf;          /* application defined data */
    T5STR_MMB   mmbCode;        /* application code */
    T5STR_MMB   mmbVmDb;        /* VM data base */
    T5STR_MMB   mmbChange;      /* changed code */
} T5STR_MM;

typedef T5STR_MM *T5PTR_MM;

/****************************************************************************/

#endif /*_T5MM_H_INCLUDED_*/

/* eof **********************************************************************/

