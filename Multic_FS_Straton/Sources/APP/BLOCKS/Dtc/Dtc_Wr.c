/*****************************************************************************/
/*CD NAME     : DTC_WRITE                                              */
/*****************************************************************************/
/*CD ROLE     : Write a DTC into   EEPROM                                    */
/*****************************************************************************/
/*  PROJET    : Driver Generique                                             */
/*  CREE      : 02-05-13          AUTEUR : P.Couronné                        */
/*****************************************************************************/
/*  Evolution :                                                              */
/*     V1.00 : Creation                                                      */
/*****************************************************************************/
#include "t5vm.h"
#include "std_type.h"
#include "monitor.h"
#include "dtc.h"

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


/* DTC_WR - Source code (requires t5vm.h include) */
/* Function */

/*====================*/
/* Argument list */
/*====================*/

#define _P_ID   (*((T5_PTWORD)(T5GET_DBDATA16(pBase))+pArgs[0]))
#define _P_STATUS   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_CONTEXT   ((T5_PTBYTE)T5Tools_GetD8Array(pBase,pArgs[2]))
#define _P_CONTEXT_SIZE   pArgs[3]
#define _P_TOTALNB   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[4]))

/*====================*/
/* handler */
/*====================*/

T5_DWORD DTC_WR (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   T5_LONG s32FunctionResult;
   
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* enter your code here... */
      // Call associated NVDM function
      s32FunctionResult=DstWMonitor_eApplication_Function.piMNVDM_eDTC_Store(_P_ID,_P_STATUS,_P_CONTEXT);
      _P_TOTALNB=(T5_BYTE)s32FunctionResult;
      return 0L;

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_ID
#undef _P_STATUS
#undef _P_CONTEXT
#undef _P_CONTEXT_SIZE
#undef _P_TOTALNB








