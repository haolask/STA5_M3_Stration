/*****************************************************************************/
/*CD NAME     : Lin_init                                                       */
/*****************************************************************************/
/*CD ROLE     : LIN Initialisation                                   */
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

//Availables LIN Baudrates
#define Du16Com_eLinBaudrate19200   19200U
#define Du16Com_eLinBaudrate14400   14400U
#define Du16Com_eLinBaudrate9600    9600U
#define Du16Com_eLinBaudrate4800    4800U
#define Du16Com_eLinBaudrate2400    2400U
#define Du16Com_eLinBaudrate1200    1200U

//----------------------------------------------------------------------------
// Controllers List
//----------------------------------------------------------------------------
//eSCI Controller used
#define  Du16DLin_eSciCtrlrAUsed    1U
#define  Du16DLin_eSciCtrlrBUsed    1U
#define  Du16DLin_eSciCtrlrCUsed    0U

//----------------------------------------------------------------------------
// Controllers List
//----------------------------------------------------------------------------
enum
{
#if (Du16DLin_eSciCtrlrAUsed>0U)
   Ceu16DLin_eCtrlrA,
#endif
#if (Du16DLin_eSciCtrlrBUsed>0U)
   Ceu16DLin_eCtrlrB,
#endif
#if (Du16DLin_eSciCtrlrCUsed>0U)
   Ceu16DLin_eCtrlrC,
#endif

   Ceu16DLin_eNbofController
};


/* LIN_Init - Source code (requires t5vm.h include) */
/* Function */

/* Argument list */

#define _P_BAUDRATE   (*((T5_PTWORD)(T5GET_DBDATA16(pBase))+pArgs[0]))
#define _P_RET   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[1]))

/* handler */

T5_DWORD LIN_INIT (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
        _P_RET=DstWMonitor_eApplication_Function.iMLin_eInit_Exe(_P_BAUDRATE);
      return 0L;

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_BAUDRATE
#undef _P_RET