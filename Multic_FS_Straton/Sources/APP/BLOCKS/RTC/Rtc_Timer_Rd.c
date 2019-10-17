/*****************************************************************************/
/*CD NAME     : RTC_TIMER_RD                                                 */
/*****************************************************************************/
/*CD ROLE     : Get timer alarm status                                       */
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
/* RTC_Timer_RD - Source code (requires t5vm.h include) */
/* Function block */

/* Argument list */

#define _P_STATUS   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_COUNTER   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_UNIT   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))
#define _P_WK_ENABLE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))
#define _P_RET   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[4]))

/* private block data */

typedef struct
{

} _str_FB_RTC_TIMER_RD;

/* handler */

T5_DWORD RTC_TIMER_RD (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   _str_FB_RTC_TIMER_RD *pData;
	 TstARtc_eRTCTimer stRtcValue;	/* this structure holds timer values */
   T5_LONG	s32ReturnValue; /* this variable is the result of function call */
   T5_BYTE  u8ConfigState; /* this variable is intermediate state of the read process */

   pData = (_str_FB_RTC_TIMER_RD *)pInst;
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* activates the function block */
      /* enter your code here... */
      u8ConfigState=0;
		 	_P_COUNTER=0;
		 	_P_UNIT=FALSE;
 			_P_WK_ENABLE=FALSE;
      s32ReturnValue=DstWMonitor_eApplication_Function.ps32ARtc_eTimer_Get(&stRtcValue);
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
    				if ((stRtcValue.u8Function & 0x80) > 0)
  					{
  							/* Timer function is enabled */
						  	_P_RET=Ds16Success;
  					}
  					else
						{
								/* Timer function is disabled */
								_P_RET=Ds16StatusIsInactive;
						}
						/* Further reading */
	    			_P_COUNTER=stRtcValue.u8CountDown;
	    			if (stRtcValue.u8WakeUp == 0)
	    			{
	    					_P_WK_ENABLE=FALSE;
	    			}
	    			else
	  				{
	  						_P_WK_ENABLE=TRUE;
	  				}
						
						u8ConfigState = (stRtcValue.u8Function & 0x7F);
						if (2== u8ConfigState)
						{
									/* unit is seconds */
									_P_UNIT = FALSE;
						}
						else
						{
								if (3== u8ConfigState)
								{
									/* unit is minutes */
									_P_UNIT = TRUE;
								}
								else
								{
									/* incoherent setting */
									if ((stRtcValue.u8Function & 0x80) > 0)
									{
											/* Change return value from Ds16Success into Ds16Parameters */
											_P_RET=Ds16Parameters;
									}
									else
									{
											/* nothing : keep Ds16StatusIsInactive */
									}
								}
						}
    	}
    	else
    	{
    			/* busy or not set */
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
      return (T5_DWORD)sizeof(_str_FB_RTC_TIMER_RD);

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_STATUS
#undef _P_COUNTER
#undef _P_UNIT
#undef _P_WK_ENABLE
#undef _P_RET
