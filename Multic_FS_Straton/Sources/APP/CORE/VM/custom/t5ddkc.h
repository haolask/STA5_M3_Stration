/*****************************************************************************
T5ddkc.h :   DDK - simplified C interface - definitions

DO NOT ALTER THIS !

(c) COPALP 2008
*****************************************************************************/

#ifndef _T5DDKC_H_INCLUDED_
#define _T5DDKC_H_INCLUDED_

#ifdef  T5DEF_DDKC

/****************************************************************************/
/* VMDB format - in table T5TB_DDKC (raw)
- T5STR_DDKCTOC
- { T5STR_DDKC }
- data bus 1
- data bus 2...
*/

/****************************************************************************/
/* bus driver callback */

typedef T5_DWORD T5HDNSPEC (*T5HDN_DDKC)(
    T5_WORD wCommand,       /* see below */
    T5_WORD wVersion,       /* configuration version number */
    T5PTR_DB pDB,           /* full VMDB - NULL for T5DDKCH_GETNODESIZE */
    T5_WORD wLevel,         /* 1=bus .. 4=var */
    T5_PTR pNode,           /* node - NULL for T5DDKCH_GETNODESIZE */
    T5_DWORD dwTimeStamp,   /* time stamp - for T5DDKCH_EXCHANGE only */
    T5_PTR pArgs            /* reserved for extensions */
    );

/* wCommand argument - possible values */

#define T5DDKCH_GETNODESIZE     1   /* get size of a single node */
#define T5DDKCH_BUILDNODE       2   /* construct a single node */
#define T5DDKCH_OPEN            3   /* open driver (bus node only) */
#define T5DDKCH_CLOSE           4   /* close driver (bus node only) */
#define T5DDKCH_EXCHANGE        5   /* exchange IOs (bus node only) */

/* return value:

T5DDKCH_GETNODESIZE : number of bytes to allocate for the node
T5DDKCH_BUILDNODE   : 0 - unused
T5DDKCH_OPEN        : T5RET_OK or T5RET_ERROR
T5DDKCH_CLOSE       : 0 - unused
T5DDKCH_EXCHANGE    : 0 - unused

*/

#define T5DDK_DECLAREHANDLER(pf) \
    extern T5_DWORD pf (T5_WORD wCommand, T5_WORD wVersion, T5PTR_DB pDB, \
        T5_WORD wLevel, T5_PTR pNode, T5_DWORD dwTimeStamp, T5_PTR pArgs);

#define T5DDK_LINKHANDLER(szName, pf) \
    if (T5_STRCMP ((szName), szDriverName) == 0) return (pf);

/****************************************************************************/
/* bus driver TOC and entries */

typedef struct                  /* main header of bus briver list */
{
    T5_WORD     wNb;            /* number of linked drivers */
    T5_WORD     res1;
    T5_WORD     res2;
    T5_WORD     res3;
}
T5STR_DDKCTOC;

typedef T5STR_DDKCTOC *T5PTR_DDKCTOC;

typedef struct                  /* bus driver main descriptor */
{
    T5HDN_DDKC  pfDriver;       /* main driver callback */
    T5_PTR      szName;         /* pointer to driver name */
    T5_PTR      pNodes;         /* pointer to root node */
    T5_WORD     wVersion;       /* configuration version number */
    T5_WORD     res1;
}
T5STR_DDKCDRV;

typedef T5STR_DDKCDRV *T5PTR_DDKCDRV;

/****************************************************************************/
/* bus driver node header */

typedef struct                  /* node header - links */
{
    T5_PTR pParent;             /* pointer to parent node */
    T5_PTR pChild;              /* pointer to first child node */
    T5_PTR pSibling;            /* pointer to next brother node */
    T5_PTR pDef;                /* pointer to static definition */
}
T5STR_DDKCNODE;

typedef T5STR_DDKCNODE *T5PTR_DDKCNODE;

/****************************************************************************/
/* OEM implementation */

extern void T5DDKC_IpInit (void);
extern void T5DDKC_IpExit (void);
extern T5HDN_DDKC T5DDKC_IpGetHandler (T5_PTCHAR szDriverName);

/****************************************************************************/
/* helpers for the driver */

extern T5_PTR T5DDKH_GetDef (T5_PTR pNode);

#define T5DDKH_GetDefBus(p)     (T5PTR_BDBUS)T5DDKH_GetDef(p)
#define T5DDKH_GetDefMaster(p)  (T5PTR_BDMASTER)T5DDKH_GetDef(p)
#define T5DDKH_GetDefSlave(p)   (T5PTR_BDSLAVE)T5DDKH_GetDef(p)
#define T5DDKH_GetDefVar(p)     (T5PTR_BDVAR)T5DDKH_GetDef(p)

extern T5_PTBYTE T5DDKH_GetProps (T5_PTR pNode);
extern T5_PTR T5DDKH_GetParent (T5_PTR pNode);
extern T5_PTR T5DDKH_GetChild (T5_PTR pNode);
extern T5_PTR T5DDKH_GetSibling (T5_PTR pNode);
extern T5_PTR T5DDKH_FindBus (T5PTR_DB pDB, T5_PTCHAR szName);

extern T5_BOOL T5DDKH_ReadVarBool (T5PTR_DB pDB, T5_PTR pNode);
extern void T5DDKH_WriteVarBool (T5PTR_DB pDB, T5_PTR pNode, T5_BOOL bValue);

extern T5_LONG T5DDKH_ReadVarLong (T5PTR_DB pDB, T5_PTR pNode);
extern void T5DDKH_WriteVarLong (T5PTR_DB pDB, T5_PTR pNode, T5_LONG lValue);

extern T5_DWORD T5DDKH_ReadVarDword (T5PTR_DB pDB, T5_PTR pNode);
extern void T5DDKH_WriteVarDword (T5PTR_DB pDB, T5_PTR pNode, T5_DWORD dwValue);

#ifdef T5DEF_REALSUPPORTED
extern T5_REAL T5DDKH_ReadVarReal (T5PTR_DB pDB, T5_PTR pNode);
extern void T5DDKH_WriteVarReal (T5PTR_DB pDB, T5_PTR pNode, T5_REAL rValue);
#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/
/* Helpers */

#ifdef T5DDKC_ALIGN4
#define T5DDKC_ALIGN(s)     (((s)%4)?((s)+4-((s)%4)):(s))
#else /*T5DDKC_ALIGN4*/
#define T5DDKC_ALIGN(s)     T5_ALIGNED(s)
#endif /*T5DDKC_ALIGN4*/

/****************************************************************************/

#endif /*T5DEF_DDKC*/

#endif /*_T5DDKC_H_INCLUDED_*/

/* eof **********************************************************************/
