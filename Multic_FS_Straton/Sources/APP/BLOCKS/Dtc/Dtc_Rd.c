/*****************************************************************************/
/*CD NAME     : DTC_READ                                                     */
/*****************************************************************************/
/*CD ROLE     : Read a DTC from   EEPROM                                     */
/*****************************************************************************/
/*  PROJET    : Driver Generique                                             */
/*  CREE      : 06-06-13          AUTEUR : BK Beaugeois                      */
/*****************************************************************************/
/*  Evolution :                                                              */
/*     V1.00 : Creation                                                      */
/*****************************************************************************/
#include "t5vm.h"
#include "std_type.h"
#include "monitor.h"
#include "dtc.h"

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

/* DTC_RD - Source code (requires t5vm.h include) */
/* Function block */

/*====================*/
/* Argument list */
/*====================*/

#define _P_ID   (*((T5_PTWORD)(T5GET_DBDATA16(pBase))+pArgs[0]))
#define _P_CONTEXT   ((T5_PTBYTE)T5Tools_GetD8Array(pBase,pArgs[1]))
#define _P_CONTEXT_SIZE   pArgs[2]
#define _P_TOTALNB   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[3]))
#define _P_OCC   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[4]))
#define _P_STATE   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[5]))


/*====================*/
/* Private data */
/*====================*/
T5_BYTE u8Status;
T5_BYTE u8FullContextTable[16];

/*====================*/
/* private block data */
/*====================*/

typedef struct
{
    T5_DWORD dwData; /* TODO: replace dwData by the items you need */
} _str_FB_DTC_RD;

/*====================*/
/* handler */
/*====================*/

T5_DWORD DTC_RD (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   _str_FB_DTC_RD *pData;
   T5_BYTE  u8IndexContext;
   T5_BYTE  u8FunctionResult;

   pData = (_str_FB_DTC_RD *)pInst;
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* activates the function block */
      /* enter your code here... */
      // Call associated NVDM function
      u8FunctionResult=DstWMonitor_eApplication_Function.pucMNVDM_eDTC_GetContext(_P_ID,&u8Status,u8FullContextTable);
      if (u8Status>0)
      {
        _P_STATE=TRUE;
      }
      else
      {
        _P_STATE=FALSE;
      }
      _P_OCC=u8FullContextTable[0];
      _P_TOTALNB=u8FunctionResult;
      for (u8IndexContext=0;u8IndexContext<15;u8IndexContext++)
      {
      
        _P_CONTEXT[u8IndexContext]=u8FullContextTable[u8IndexContext+1];
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
      return (T5_DWORD)sizeof(_str_FB_DTC_RD);

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_ID
#undef _P_CONTEXT
#undef _P_CONTEXT_SIZE
#undef _P_TOTALNB
#undef _P_OCC
#undef _P_STATE








