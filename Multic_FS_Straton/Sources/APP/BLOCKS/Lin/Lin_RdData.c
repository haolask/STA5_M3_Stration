/*****************************************************************************/
/*CD NAME     : Lin_Rx                                                       */
/*****************************************************************************/
/*CD ROLE     : Read LIN frame                                */
/*****************************************************************************/
/*  PROJET    : Driver Generique                                             */
/*  CREE      : 02-05-14          AUTEUR : A.Chourabi                        */
/*****************************************************************************/
/*  Evolution :                                                              */
/*     V1.00 : Creation                                                      */
/*****************************************************************************/
#include "t5vm.h"
#include "std_type.h"
#include "monitor.h"


/*----------------------------------------------------------------------------------------
TYPE MEMO
------------------------------------------------------------------------------------------
typedef char            T5_CHAR;    SINT     short signed integer on 8 bits
typedef signed short    T5_SHORT;   INT      signed integer on 16 bits
typedef signed long     T5_LONG;    DINT     signed integer on 32 bits
typedef unsigned char   T5_BOOL;    BOOL     boolean (TRUE / FALSE)
typedef unsigned char   T5_BYTE;    USINT    unsigned interger on 8 bits
typedef unsigned short  T5_WORD;    UINT     unsigned interger on 16 bits
typedef unsigned long   T5_DWORD;   UDINT    unsigned interger on 32 bits
typedef float           T5_REAL;    REAL     single precision floating point on 32 bits
typedef double          T5_LREAL;   LREAL    double precision floating point on 64 bits
----------------------------------------------------------------------------------------*/
//****************************************************************************//
// DEFINITION
//****************************************************************************//


/* LIN_RdData - Source code (requires t5vm.h include) */
/* Function block */

/* Argument list */

#define _P_FRAMEPID   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_DATA   ((T5_PTBYTE)T5Tools_GetD8Array(pBase,pArgs[1]))
#define _P_DATA_SIZE   pArgs[2]
#define _P_LENGTH   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[3]))
#define _P_RET   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[4]))

/* private block data */

typedef struct
{
    T5_DWORD dwData; /* TODO: replace dwData by the items you need */
} _str_FB_LIN_RDDATA;

/* handler */

T5_DWORD LIN_RDDATA (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   _str_FB_LIN_RDDATA *pData;

   pData = (_str_FB_LIN_RDDATA *)pInst;
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      _P_RET=DstWMonitor_eApplication_Function.s32MLin_eReadFrame_Exe( _P_FRAMEPID, &_P_LENGTH, _P_DATA);
      return 0L;

   case T5FBCMD_INITINSTANCE :
      /* initialize private data */
      /* enter your code here... */
      return 0L;

   case T5FBCMD_EXITINSTANCE :
      /* release private data */
      /* enter your code here... */
      return 0L;

   case T5FBCMD_HOTRESTART :
      /* actuate private data for hot restart */
      /* enter your code here... */
      return 0L;

   case T5FBCMD_SIZEOFINSTANCE :
      /* dont change that */
      return (T5_DWORD)sizeof(_str_FB_LIN_RDDATA);

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_FRAMEPID
#undef _P_DATA
#undef _P_DATA_SIZE
#undef _P_LENGTH
#undef _P_RET

