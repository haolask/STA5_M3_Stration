/*****************************************************************************/
/*CD NAME     : RTC_ALARM_RD                                                 */
/*****************************************************************************/
/*CD ROLE     : Get an alarm setting                                         */
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

/* RTC_Alarm_RD - Source code (requires t5vm.h include) */
/* Function block */

/* Argument list */

#define _P_STATUS   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_MINUTEENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_MINUTEVALUE   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[2]))
#define _P_HOURENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))
#define _P_HOURVALUE   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[4]))
#define _P_DATEENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[5]))
#define _P_DATEVALUE   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[6]))
#define _P_WDAYENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[7]))
#define _P_WDAYVALUE   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[8]))
#define _P_WAKEUPENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[9]))
#define _P_RET   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[10]))

/* private block data */

typedef struct
{

} _str_FB_RTC_ALARM_RD;

/* handler */

T5_DWORD RTC_ALARM_RD (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   _str_FB_RTC_ALARM_RD *pData;
	 TstARtc_eRTCAlarm stRtcValue;	/* this structure holds alarm values */
   T5_LONG	s32ReturnValue; /* this variable is the result of function call */

   pData = (_str_FB_RTC_ALARM_RD *)pInst;
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* activates the function block */
      /* enter your code here... */
      s32ReturnValue=DstWMonitor_eApplication_Function.ps32ARtc_eAlarm_Get(&stRtcValue);
      if (Ds32WCore_eSuccessActiveStatus==s32ReturnValue)
    	{
    			/* IT has occured*/
    			_P_STATUS = TRUE;
    	}
    	else
  		{
  				_P_STATUS = FALSE;
  		}
      if ((Ds32Monitor_eSuccess==s32ReturnValue) || (Ds32WCore_eSuccessActiveStatus==s32ReturnValue))
    	{
    			if ((stRtcValue.u8MinuteValue & 0x80) >0)
  				{
  						_P_MINUTEENABLE = TRUE;
  				}
  				else
					{
							_P_MINUTEENABLE = FALSE;
					}
					_P_MINUTEVALUE = (stRtcValue.u8MinuteValue & 0x7F);
					
    			if ((stRtcValue.u8HourValue & 0x80) >0)
  				{
  						_P_HOURENABLE = TRUE;
  				}
  				else
					{
							_P_HOURENABLE = FALSE;
					}
					_P_HOURVALUE = (stRtcValue.u8HourValue & 0x7F);
					
    			if ((stRtcValue.u8DayValue & 0x80) >0)
  				{
  						_P_DATEENABLE = TRUE;
  				}
  				else
					{
							_P_DATEENABLE = FALSE;
					}
					_P_DATEVALUE = (stRtcValue.u8DayValue & 0x7F);
					
    			if ((stRtcValue.u8WeekDayValue & 0x80) >0)
  				{
  						_P_WDAYENABLE = TRUE;
  				}
  				else
					{
							_P_WDAYENABLE = FALSE;
					}
					_P_WDAYVALUE = (stRtcValue.u8WeekDayValue & 0x7F);
					
    			if (1==stRtcValue.u8WakeUp)
  				{
  						_P_WAKEUPENABLE = TRUE;
  				}
  				else
					{
							_P_WAKEUPENABLE = FALSE;
					}
					_P_RET = Ds16Success;
    	}
    	else
  		{
					_P_MINUTEENABLE = FALSE;
					_P_MINUTEVALUE = 0xFF;
					_P_HOURENABLE = FALSE;
					_P_HOURVALUE = 0xFF;
					_P_DATEENABLE = FALSE;
					_P_DATEVALUE = 0xFF;
					_P_WDAYENABLE = FALSE;
					_P_WDAYVALUE = 0xFF;
					_P_WAKEUPENABLE = FALSE;

  				_P_RET=Ds16Error;
  		}
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
      return (T5_DWORD)sizeof(_str_FB_RTC_ALARM_RD);

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_STATUS
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
