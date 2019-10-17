/*****************************************************************************/
/*CD NAME     : RTC_TIMER_CLR                                                */
/*****************************************************************************/
/*CD ROLE     : Clear timer alarm                                            */
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

/* RTC_Timer_CLR - Source code (requires t5vm.h include) */
/* Function block */

/* Argument list */

#define _P_CMD   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_RET   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[1]))
/* private block data */
typedef struct
{
	 T5_BOOL  bPreviousCommand;
} _str_FB_RTC_TIMER_CLR;

/* handler */

T5_DWORD RTC_TIMER_CLR (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   _str_FB_RTC_TIMER_CLR *pData;
	 
   T5_LONG	s32ReturnValue; /* this variable is the result of function call */
   
	 pData = (_str_FB_RTC_TIMER_CLR *)pInst;
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* activates the function block */
      /* enter your code here... */
      if (TRUE==_P_CMD)
      {
					if (FALSE==pData->bPreviousCommand)
					{      	
			    		s32ReturnValue=DstWMonitor_eApplication_Function.ps32ARtc_eTimer_Set(NULL);
			    		if (Ds32Monitor_eSuccess==s32ReturnValue)
							{
									_P_RET=Ds16Success;
							}
							else
							{
									_P_RET=Ds16Error;
							}
  				}
  				else
					{
							/* already done once */
    					_P_RET=Ds16NotProcessed;
					}
      }
      else
    	{
    			_P_RET=Ds16Parameters;
    	}
			pData->bPreviousCommand=_P_CMD;
      return 0L;

   case T5FBCMD_INITINSTANCE :
      /* initialize private data */
      /* enter your code here... */
      pData->bPreviousCommand = FALSE;
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
      return (T5_DWORD)sizeof(_str_FB_RTC_TIMER_CLR);

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_CMD
#undef _P_RET
