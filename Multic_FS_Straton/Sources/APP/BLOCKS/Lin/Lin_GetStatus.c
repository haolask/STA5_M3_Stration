/*****************************************************************************/
/*CD NAME     : Lin_GetStatus                                                      */
/*****************************************************************************/
/*CD ROLE     : Get Status LIN bus                                */
/*****************************************************************************/
/*  PROJET    : Driver Generique                                             */
/*  CREE      : 20-056-17          AUTEUR : A.Chourabi                        */
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
/* LIN_GETSTATUS - Source code (requires t5vm.h include) */
/* function */

/* define argument list */
#if 0  //leizhu add for lin test 180709
#define _P_DID   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_Q   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[1]))

/* handler */

T5_DWORD LIN_GETSTATUS (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      _P_Q=DstWMonitor_eApplication_Function.s32MLin_eGetStatus_Exe( _P_DID);
      return 0L;

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* undefine argument list */

#undef _P_DID
#undef _P_Q
#endif
