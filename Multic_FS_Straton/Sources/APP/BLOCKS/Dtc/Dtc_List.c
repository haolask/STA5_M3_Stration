/*****************************************************************************/
/*CD NAME     : DTC_LIST                                                     */
/*****************************************************************************/
/*CD ROLE     : Get the list of all DTC codes memorized                      */
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

/* DTC_LIST - Source code (requires t5vm.h include) */
/* Function */

/*====================*/
/* Argument list */
/*====================*/

#define _P_LIST   ((T5_PTWORD)T5Tools_GetD16Array(pBase,pArgs[0]))
#define _P_LIST_SIZE   pArgs[1]
#define _P_TOTALNB   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[2]))

/*====================*/
/* handler */
/*====================*/

T5_DWORD DTC_LIST (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   T5_BYTE u8FunctionResult;
   
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* enter your code here... */
      // Call associated NVDM function
      u8FunctionResult=DstWMonitor_eApplication_Function.pucMNVDM_eDTC_GetCodesList(_P_LIST);
      _P_TOTALNB=u8FunctionResult;
      return 0L;

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_LIST
#undef _P_LIST_SIZE
#undef _P_TOTALNB









