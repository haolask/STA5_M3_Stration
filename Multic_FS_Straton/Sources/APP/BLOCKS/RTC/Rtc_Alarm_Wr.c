/*****************************************************************************/
/*CD NAME     : RTC_ALARM_WR                                                 */
/*****************************************************************************/
/*CD ROLE     : Set an      alarm                                            */
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

/* RTC_Alarm_WR - Source code (requires t5vm.h include) */
/* Function */

/* Argument list */

#define _P_MINUTEENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_MINUTEVALUE   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_HOURENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))
#define _P_HOURVALUE   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[3]))
#define _P_DATEENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[4]))
#define _P_DATEVALUE   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[5]))
#define _P_WDAYENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[6]))
#define _P_WDAYVALUE   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[7]))
#define _P_WAKEUPENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[8]))
#define _P_RET   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[9]))

/* handler */

T5_DWORD RTC_ALARM_WR (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
	 TstARtc_eRTCAlarm stRtcValue;	/* this structure holds alarm values */
   T5_LONG	s32ReturnValue; /* this variable is the result of function call */
   
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* enter your code here... */
      s32ReturnValue=Ds32Monitor_eSuccess;
      if (TRUE==_P_MINUTEENABLE)
    	{
    			if (_P_MINUTEVALUE <60)
    			{
    					stRtcValue.u8MinuteValue=0x80+_P_MINUTEVALUE;
    			}
    			else
  				{
  						/* wrong parameter */
  						s32ReturnValue=Ds32Monitor_eError;
  				}
    	}
    	else
  		{
  				/* don't care */
  				stRtcValue.u8MinuteValue=0;
  		}
      if (TRUE==_P_HOURENABLE)
    	{
    			if (_P_HOURVALUE <24)
    			{
    					stRtcValue.u8HourValue=0x80+_P_HOURVALUE;
    			}
    			else
  				{
  						/* wrong parameter */
  						s32ReturnValue=Ds32Monitor_eError;
  				}
    	}
    	else
  		{
  				/* don't care */
  				stRtcValue.u8HourValue=0;
  		}
      if (TRUE==_P_DATEENABLE)
    	{
    			if ((_P_DATEVALUE > 0) && (_P_DATEVALUE < 32))
    			{
    					stRtcValue.u8DayValue=0x80+_P_DATEVALUE;
    			}
    			else
  				{
  						/* wrong parameter */
  						s32ReturnValue=Ds32Monitor_eError;
  				}
    	}
    	else
  		{
  				/* don't care */
  				stRtcValue.u8DayValue=1;
  		}
      if (TRUE==_P_WDAYENABLE)
    	{
    			if (_P_WDAYVALUE < 7)
    			{
    					stRtcValue.u8WeekDayValue=0x80+_P_WDAYVALUE;
    			}
    			else
  				{
  						/* wrong parameter */
  						s32ReturnValue=Ds32Monitor_eError;
  				}
    	}
    	else
  		{
  				/* don't care */
  				stRtcValue.u8WeekDayValue=0;
  		}
  		if (TRUE==_P_WAKEUPENABLE)
			{
					stRtcValue.u8WakeUp=1;
			}
  		else
			{
					stRtcValue.u8WakeUp=0;
			}
			/* Case of valid request */
			if (Ds32Monitor_eSuccess==s32ReturnValue)
			{
					s32ReturnValue=DstWMonitor_eApplication_Function.ps32ARtc_eAlarm_Set(&stRtcValue);
					if (Ds32Monitor_eSuccess==s32ReturnValue)
					{
							_P_RET=Ds16Success;
					}
					else
					{
							/* error */
							_P_RET=Ds16Error;
					}
			}
			else
			{
					/* wrong parameter */
					_P_RET=Ds16Parameters;
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

#undef _P_MINUTEENABLE
#undef _P_MINUTEVALUE
#undef _P_HOURENABLE
#undef _P_HOURVALUE
#undef _P_DATEENABLE
#undef _P_DATEVALUE
#undef _P_WDAYENABLE
#undef _P_WDAYVALUE
#undef _P_WAKEUPENABLE
#undef _P_RET

