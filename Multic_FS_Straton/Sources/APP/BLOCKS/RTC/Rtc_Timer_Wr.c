/*****************************************************************************/
/*CD NAME     : RTC_TIMER_WR                                                 */
/*****************************************************************************/
/*CD ROLE     : Set a timer alarm                                            */
/*****************************************************************************/
/*  PROJET    : Driver Generique                                             */
/*  CREE      : 01-06-16          AUTEUR : Bbea                              */
/*****************************************************************************/
/*  Evolution :                                                              */
/*     V1.00 : Creation                                                      */
/*****************************************************************************/
#include "t5vm.h"
#include "std_type.h"
#include "monitor.h"
#include "Rtc.h"

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

/* RTC_Timer_WR - Source code (requires t5vm.h include) */
/* Function */

/* Argument list */

#define _P_ENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_SEC_MIN   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_LOADVALUE   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[2]))
#define _P_WAKEUPENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))
#define _P_RET   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[4]))

/* handler */

T5_DWORD RTC_TIMER_WR (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
	 TstARtc_eRTCTimer stRtcValue;	/* this structure holds timer values */
   T5_LONG	s32ReturnValue; /* this variable is the result of function call */
   T5_BYTE u8BuildParameter;
   
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* enter your code here... */
      if (FALSE==_P_SEC_MIN)
    	{
    			/* seconds timer */
    			u8BuildParameter=2;
    	}
    	else
    	{
    			/* minutes timer */
    			u8BuildParameter=3;
    	}
    	if (TRUE==_P_ENABLE)
  		{
  				u8BuildParameter=(0x80 | u8BuildParameter);
  		}
      stRtcValue.u8Function=u8BuildParameter;
      stRtcValue.u8CountDown=_P_LOADVALUE;
      stRtcValue.u8WakeUp=(T5_BYTE)_P_WAKEUPENABLE;
      
      s32ReturnValue=DstWMonitor_eApplication_Function.ps32ARtc_eTimer_Set(&stRtcValue);
  		if (Ds32Monitor_eSuccess==s32ReturnValue)
			{
					_P_RET=Ds16Success;
			}
			else
			{
					_P_RET=Ds16Error;
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

#undef _P_ENABLE
#undef _P_SEC_MIN
#undef _P_LOADVALUE
#undef _P_WAKEUPENABLE
#undef _P_RET
