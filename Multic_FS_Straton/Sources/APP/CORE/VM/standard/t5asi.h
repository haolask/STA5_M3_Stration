/*****************************************************************************
T5ASi.h :    ASi core engine definitions

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#ifndef _T5ASI_H_INCLUDED_
#define _T5ASI_H_INCLUDED_

/****************************************************************************/
/* macros: bit accesss in a 64bit byte array */

#define T5ASIBIT_SET(a,s)       (a)[s/8] |= (1 << ((s) % 8));
#define T5ASIBIT_RESET(a,s)     (a)[s/8] &= ~(1 << ((s) % 8));
#define T5ASIBIT_TEST(a,s)      (((a)[s/8] & (1 << ((s) % 8))) != 0)

/****************************************************************************/
/* static definition - main format= header + masters + variables */

#define T5ASI_SETCONF   0x0001      /* configurate master at startup */

typedef struct                      /* main configuration header */
{
    T5_WORD wNbMaster;              /* actual number of masters */
    T5_WORD wNbVar;                 /* actual number of variables */
    T5_WORD wNbMasterAlloc;         /* nb of masters allocated */
    T5_WORD wNbVarAlloc;            /* nb of variables allocated */
    T5_WORD wFlags;                 /* main flags */
    T5_WORD res1;
}
T5STR_ASIDEF;

typedef T5STR_ASIDEF *T5PTR_ASIDEF;

/****************************************************************************/
/* static definition - master */

typedef T5_BYTE T5_ASIPRF[2];
typedef T5_ASIPRF *T5PTR_ASIPRF;

typedef struct                      /* an ASi master and its slaves */
{
    T5_CHAR szSettings[64];         /* settings - null terminated */
    T5_ASIPRF wProfile[64];         /* profile - always in ASi endian */
}
T5STR_ASIMDEF;

typedef T5STR_ASIMDEF *T5PTR_ASIMDEF;

/****************************************************************************/
/* static definition - variable */

#define T5ASI_VINPUT    0x01        /* input data */
#define T5ASI_VOUTPUT   0x02        /* output data */
#define T5ASI_VSTATUS   0x04        /* slave status flags (always in) */
#define T5ASI_VDIAG     0x08        /* master diagnostic flag (in/out) */
#define T5ASI_VNEG      0x10        /* negated I/O */
#define T5ASI_VCT       0x80        /* linked to a var in CT segment */

typedef struct
{
    T5_WORD wT5Type;                /* T5 variable data type */
    T5_WORD wT5Offset;              /* offset in T5 database */
    T5_BYTE bMaster;                /* index of the ASi master */
    T5_BYTE bAddr;                  /* ASi slave address */
    T5_BYTE bMask;                  /* IO mask or flag index (1..32) */
    T5_BYTE bFlags;                 /* flags */
                                    /* followed by CT offset on 4 bytes */
                                    /* if flag T5ASI_VCT set */
}
T5STR_ASIVDEF;

typedef T5STR_ASIVDEF *T5PTR_ASIVDEF;

/****************************************************************************/
/* runtime: flags in slave status (A only if mapped to bit) */

#define T5ASI_A    0x0001           /* slave flag: active */
#define T5ASI_D    0x0002           /* slave flag: detected */
#define T5ASI_P    0x0004           /* slave flag: projected */
#define T5ASI_C    0x0008           /* slave flag: corrupted */
#define T5ASI_F    0x0010           /* slave flag: periph fault */

/****************************************************************************/
/* runtime: master execution flags */

/* first byte of ecFlags */

#define T5ASI_CONFIG_OK               0x01 /* no configuration error */
#define T5ASI_LDS0                    0x02 /* slave with address 0 existing */
#define T5ASI_AUTO_ADDRESS_ASSIGN     0x04 /* auto. programming activated */
#define T5ASI_AUTO_ADDRESS_AVAILABLE  0x08 /* autom. programming avalible */
#define T5ASI_CONFIGURATION_ACTIVE    0x10 /* configuration mode active */
#define T5ASI_NORMAL_OPERATION_ACTIVE 0x20 /* normal operation mode active */
#define T5ASI_APF                     0x40 /* ASI power fail */
#define T5ASI_OFFLINE_READY           0x80 /* the offline phase is active */

/* second byte of ecFlags */

#define T5ASI_PERIPHERY_OK            0x01 /* no peripheral fault */

/****************************************************************************/
/* runtime data - master */

typedef struct                      /* runtime data: a master & its slaves */
{
    T5PTR_ASIMDEF pDef;             /* static definition */
    T5_DWORD  hMaster;              /* handle of master (OEM) */
    T5_ASIPRF wPrjPrf[64];          /* projected profiles */
    T5_ASIPRF wDetPrf[64];          /* detected profiles */
    T5_BYTE   idi[32];              /* inputs */
    T5_BYTE   ido[32];              /* outputs */
    T5_BYTE   idomask[32];          /* for OEMs - not managed by STRATON */
    T5_BYTE   las[8];               /* list of active slaves */
    T5_BYTE   lds[8];               /* list of detected slaves */
    T5_BYTE   lps[8];               /* list of projected slaves */
    T5_BYTE   lcs[8];               /* list of corrupted slaves */
    T5_BYTE   lfs[8];               /* list of slaves with periph fault */
    T5_BYTE   ecFlags[2];           /* master status bits */
    T5_WORD   wFirstVarIn;          /* index of first input variable */
    T5_WORD   wNbVarIn;             /* number of input variables */
    T5_WORD   wFirstVarOut;         /* index of first output variable */
    T5_WORD   wNbVarOut;            /* number of output variables */
    T5_WORD   res1;
}
T5STR_ASIM;

typedef T5STR_ASIM *T5PTR_ASIM;

/****************************************************************************/
/* runtime data - variable */

#define T5ASIOPE_DATAIN     1       /* input */
#define T5ASIOPE_DATAOUT    2       /* output */
#define T5ASIOPE_DIAG       3       /* master execution flag */
#define T5ASIOPE_STATUS     4       /* slave status flag */
#define T5ASIOPE_DATANIN    5       /* negated input */
#define T5ASIOPE_DATANOUT   6       /* negated output */

typedef struct                      /* runtime data: an exchanged variable */
{
    T5PTR_ASIVDEF pDef;             /* static definition */
    T5_PTR    pT5Data;              /* pointer to T5 data */
    T5_PTBYTE pLock;                /* pointer to ASi data */
    T5_PTBYTE pASiData;             /* pointer to ASi data */
    T5_BYTE   bT5Type;              /* T5 variable data type */
    T5_BYTE   bMask;                /* mask in ASiData */
    T5_BYTE   bLockMask;            /* lock mask in VM db */
    T5_BYTE   bOpe;                 /* data exchange operation */
}
T5STR_ASIV;

typedef T5STR_ASIV *T5PTR_ASIV;

/****************************************************************************/
/* ASi runtime data - main header */

typedef struct                      /* main configuration header */
{
    T5STR_ASIDEF def;               /* main configuration and sizing */
    T5PTR_ASIM pMaster;             /* pointer to masters structures */
    T5PTR_ASIV pVar;                /* pointer to variables structures */
}
T5STR_ASI;

typedef T5STR_ASI *T5PTR_ASI;

/****************************************************************************/

#endif /*_T5ASI_H_INCLUDED_*/

/* eof **********************************************************************/
