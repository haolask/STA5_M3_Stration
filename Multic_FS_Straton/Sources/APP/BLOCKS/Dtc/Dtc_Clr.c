/*****************************************************************************/
/*CD NAME     : DTC_CLR                                                      */
/*****************************************************************************/
/*CD ROLE     : Erase all DTC                                                */
/*****************************************************************************/
/*  PROJET    : Driver Generique                                             */
/*  CREE      : 02-08-13          AUTEUR : BK Beaugeois                      */
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

/* DTC_CLR - Source code (requires t5vm.h include) */
/* Function */

/* Argument list */

#define _P_CMD   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_Q   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[1]))

/* handler */

T5_DWORD DTC_CLR (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* enter your code here... */
      if (TRUE==_P_CMD)
      {
            // Call associated NVDM function
            DstWMonitor_eApplication_Function.pvMNVDM_eDTC_EraseAll();
            _P_Q=1;
      }
      else
      {
          //Return no execution information
          _P_Q=0;
      }
      return 0L;

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_CMD
#undef _P_Q










