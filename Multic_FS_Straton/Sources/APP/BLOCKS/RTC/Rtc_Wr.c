/*****************************************************************************/
/*CD NAME     : RTC_WR                                                       */
/*****************************************************************************/
/*CD ROLE     : Get date and time information                                */
/*****************************************************************************/
/*  PROJET    : Driver Generique                                             */
/*  CREE      : 01-03-16          AUTEUR : Bbea                              */
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

/* RTC_WR - Source code (requires t5vm.h include) */
/* Function */

/* Argument list */

#define _P_YEAR   (*((T5_PTWORD)(T5GET_DBDATA16(pBase))+pArgs[0]))
#define _P_MONTH   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_DATEDAY   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[2]))
#define _P_WEEKDAY   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[3]))
#define _P_HOUR   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[4]))
#define _P_MINUTE   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[5]))
#define _P_SECOND   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[6]))
#define _P_RETURNSTATUS   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[7]))

/* handler */

T5_DWORD RTC_WR (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
	 TstARtc_eRTCStruct stRtcValue;	/* this structure holds date and time values */
   T5_LONG	s32ReturnValue; /* this variable is the result of function call */
   T5_BYTE	u8Validity; /* result of validity check */

   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* enter your code here... */
      /* check parameters valid range */
      u8Validity = 0xFF;
      if (	((_P_YEAR>1999) && (_P_YEAR<2100))
      			&& ((_P_MONTH>0) && (_P_MONTH<13))
      			&& ((_P_DATEDAY>0) && (_P_DATEDAY<32))
      			&& (_P_WEEKDAY<7)
      			&& (_P_HOUR<24)
      			&& (_P_MINUTE<60)
      			&& (_P_SECOND<60)
      	 )
      {
      		u8Validity = 1;
      }
      else
      {
      		u8Validity = 0;
      }
      /* parameters are valid */
      
      if (1==u8Validity)
      {
		      stRtcValue.u16YearValue=_P_YEAR;
		      stRtcValue.u8MonthValue=_P_MONTH;
		      stRtcValue.u8DateDayValue=_P_DATEDAY;
		      stRtcValue.u8WeekDayValue=_P_WEEKDAY;
		      stRtcValue.u8HourValue=_P_HOUR;
		      stRtcValue.u8MinuteValue=_P_MINUTE;
		      stRtcValue.u8SecondValue=_P_SECOND;
      	
      		s32ReturnValue=DstWMonitor_eApplication_Function.ps32ARtc_eWrite_Exe(&stRtcValue);
      		if (Ds32Monitor_eError==s32ReturnValue)
      		{
      				_P_RETURNSTATUS=Ds16Parameters;
      		}
      		else
      		{
      				if (Ds32Monitor_eSuccess==s32ReturnValue)
      				{
      						_P_RETURNSTATUS=Ds16Success;
      				}
      				else
      				{
      						_P_RETURNSTATUS=Ds16Error;
      				}
      		}
      }
      /* parameters are wrong */
      else
      {
      		_P_RETURNSTATUS=Ds16Parameters;
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

#undef _P_YEAR
#undef _P_MONTH
#undef _P_DATEDAY
#undef _P_WEEKDAY
#undef _P_HOUR
#undef _P_MINUTE
#undef _P_SECOND
#undef _P_RETURNSTATUS


