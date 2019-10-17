/*****************************************************************************/
/*CD NAME     : BEEP_CONTROL                                                        */
/*****************************************************************************/
/*CD ROLE     : To control the beep                             			*/
/*****************************************************************************/
/*  PROJET    : Driver Generique                                             */
/*  CREE      : 18-09-14          AUTEUR : fangyunmeng                        */
/*****************************************************************************/
/*  Evolution :                                                              */
/*     V1.00 : Creation                                                      */
/*****************************************************************************/
#include "t5vm.h"
#include "std_type.h"
#include "monitor.h"

/*====================*/
/* private block data */
/*====================*/
typedef struct
{
    T5_BYTE bBEEPONOFF; /* TODO: replace dwData by the items you need */
} _str_FB_BEEP_ST;

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

/* Sleep - Source code (requires t5vm.h include) */
/* Function */

/* Argument list */

#define _P_VOL	(*((T5_PTWORD)(T5GET_DBDATA16(pBase))+pArgs[0]))
#define _P_FQ		(*((T5_PTWORD)(T5GET_DBDATA16(pBase))+pArgs[0]))
#define _P_Time	(*((T5_PTWORD)(T5GET_DBDATA16(pBase))+pArgs[1]))
#define _P_Qvol			(*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_QPlayBeep		(*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))
#define _P_QStop			(*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))
/* handler */

T5_DWORD BEEP_VOL (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{

   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
    	DstWMonitor_eApplication_Function.s32WMonitor_eMmiSoundVolume_Set((T5_WORD)_P_VOL);
        //_P_Qvol = 1;
      return 0L;

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

T5_DWORD BEEP_PlayBeep (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{

   _str_FB_BEEP_ST *pmem;
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
    	DstWMonitor_eApplication_Function.s32WMonitor_eMmiPlayBeep_Exe((T5_WORD)_P_FQ,(T5_DWORD)_P_Time);
      return 0L;

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

T5_DWORD BEEP_STOP (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{

   _str_FB_BEEP_ST *pmem;
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
    	DstWMonitor_eApplication_Function.vWMonitor_eMmiSoundStop_Exe();
      return 0L;

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}


/* Undefine argument list */

#undef _P_FQ		
#undef _P_VOL	
#undef _P_Time	
#undef _P_Qvol		
#undef _P_QPlayBeep		
#undef _P_QStop
