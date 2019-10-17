/*****************************************************************************/
/*CD NAME     : SPECIFCALIB_RD                                                */
/*****************************************************************************/
/*CD ROLE     : Get ECU specific predefined parameter                        */
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
#include "SpecifCalib.h"

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

/* SPECIFCALIB_RD - Source code (requires t5vm.h include) */
/* Function block */

/* Argument list */

#define _P_NAME   (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[0]))
#define _P_VALUE   (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_RETURNSTATUS   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[2]))

/* private block data */

typedef struct
{

} _str_FB_SPECIFCALIB_RD;

/* handler */

T5_DWORD SPECIFCALIB_RD (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   _str_FB_SPECIFCALIB_RD *pData;
   T5_DWORD u32ValueRead; /* This variable will received the requested value */
   T5_LONG	s32ReturnValue; /* this variable is the result of function call */
   T5_WORD u16CalibId; /* This variable will received to ID code associated to the name found for read request */
   T5_BYTE TextLength; /* Used for string compariqon */
   T5_BYTE u8LoopIndex;
   T5_BYTE u8Index;
   T5_BOOL u1Found; /* Flag used for reasearch */

   pData = (_str_FB_SPECIFCALIB_RD *)pInst;
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* activates the function block */
      /* enter your code here... */
      
      /* 1 Check is Name Id is defined inside CalibList */
      u16CalibId=0xFFFF; /* init to invalid */
      u1Found=FALSE; /* init to not found */
      u8LoopIndex=0;
      while ((u8LoopIndex<Ceu16WMonitor_eParameterNbr) && (FALSE==u1Found))
      {
      	  /* Count the number of identical characters */
      	  TextLength=0;
		      for (u8Index=0;u8Index<(_P_NAME[1]-2);u8Index++)
		      {
		      		if (CalibList[u8LoopIndex].NameId[u8Index]==_P_NAME[3+u8Index])
		      		{
		      				TextLength=TextLength+1;
		      		}
		      }
		      if (CalibList[u8LoopIndex].NameLength==TextLength)
		      {
		      		u16CalibId=CalibList[u8LoopIndex].IdCode;
		      		u1Found=TRUE;
		      }
		      else
	      	{
	      			/* nothing : keep not found values */
	      	}
	      	u8LoopIndex++;
      }
      /* 2 If ID has been found : read request */
      if (TRUE==u1Found)
    	{
      		s32ReturnValue=Ds32Monitor_eError; /* init to error code */
    			s32ReturnValue=DstWMonitor_eApplication_Function.s32WMonitor_eParameterData_Get(u16CalibId,&u32ValueRead);
					if (Ds32Monitor_eSuccess==s32ReturnValue)
  				{
  						_P_VALUE = u32ValueRead;
							_P_RETURNSTATUS=Ds32Success;
  				}
  				else
  				{
  						_P_VALUE = 0xFFFFFFFF;
							_P_RETURNSTATUS=Ds32Error;
  				}
    	}
			else
			{
  				_P_VALUE = 0xFFFFFFFF;
					_P_RETURNSTATUS=Ds32Parameters;
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
      return (T5_DWORD)sizeof(_str_FB_SPECIFCALIB_RD);

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_NAME
#undef _P_VALUE
#undef _P_RETURNSTATUS


