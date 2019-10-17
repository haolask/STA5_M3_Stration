////////////////////////////////////////////////////////////////////////////////
//       _________________________________________ ___ ___ ___
//      /  ___      ___  ________ ___ ___        /   /   /   /
//     /  /   |   /  __|/__   __//  //   | (R)  /___/___/___/
//    /  / /| | /  /      /  /  /  // /| |     /   /   /   /
//   /  / __  |(  (___   /  /  /  // __  |    /___/___/___/
//  /  /_/  |_| \_____| /__/  /__//_/  |_|   /   /   /   /
// /________________________________________/___/___/___/
//----------------------------------------------------------------------------
// HEAD : (c) ACTIA Automotive 2015 : " Any copy and re-use of this
//        file without the written consent of ACTIA is forbidden "
//        --------------------------------------------------------------------
//        Classification :  (-)No   (-)Confident. ACTIA   (-)Confident. Client
//        --------------------------------------------------------------------
//        T5ACTIA : Straton Application 
////////////////////////////////////////////////////////////////////////////////
// FILE : T5ACTIA.c (SOURCE)
//----------------------------------------------------------------------------
// DESC : Straton Application 
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 1209 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Sources/APP/CORE/VM/custom/T5ACTIA.c $
// Updated the :$Date: 2016-03-16 21:59:57 +0800 (周三, 16 三月 2016) $
// By 			:$Author: pcouronn $
////////////////////////////////////////////////////////////////////////////////
/* T5ACTIA.c */
////////////////////////////////////////////////////////////////////////////////
// INCLIB
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// INCUSER
////////////////////////////////////////////////////////////////////////////////
#include "t5vm.h"
#include "std_type.h"
#include "monitor.h"


extern void T5Memory_Init (void);

////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// TYPEDEF
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// INTERN
////////////////////////////////////////////////////////////////////////////////

/* static data - VM stuff */

static T5_DWORD         dwDynaLink = 0L;    /* dynamic link VM data */
static T5STR_MM         mmData;             /* MM database */
static T5STR_CS         csData;             /* CS database */
static T5PTR_DB         pDB = NULL;         /* pointer to VM database */
static T5_PTR           pCode = NULL;       /* pointer to the app. codev*/
static T5_DWORD	ulStratonKernelTimeBase = 0L;

////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////

/* static services */

static void _Init (void);
static void _Exit_ (void);
static void _Serve (void);
static T5_WORD _Start (T5PTR_CSSTART pStart);
static T5_BOOL _LoadCode (T5_PTCHAR szName);
static T5_WORD _Stop (void);
static void _FreeCode (void);
static void _FreeDB (void);

/* system */

T5_DWORD T5_GetTickCount (void)
{
   T5_DWORD Exectime_u32_ms = 0;
   // TODO
   Exectime_u32_ms = (T5_DWORD)DstWMonitor_eApplication_Function.pu32Core_eExecutionTime_Exe(); // DpstWZed_eVMDataExchange->u16TimeBase;
   //Exectime_u64_us /= 1000L; //�s --> ms

   return ((T5_DWORD )(Exectime_u32_ms)) ;
}

void T5_Sleep (T5_DWORD dwMs)
{
   // TODO
}

/* the main loop */

void Application_Init_Hook (void)
{
   //*******************************************
   // initsct initialise .data & .bss variables
   // ! DO NOT DELETE !
   (void)initsct();
   //*******************************************

   T5STR_CSSTART start;

   _Init();

   //    T5Flash_GotToRam ();
   start.bCC = FALSE;
   start.bWarm = TRUE;
   start.szName = (T5_PTCHAR)"T5";
   _Start (&start);

   DpstWZed_eVMDataExchange->u16CyclePer = *(T5VM_GetTriggerPtr(pDB));  // Application cycle period set in Straton Workbench in us
}

void Application_Background_Hook (void)
{
   T5_DWORD dwStart, dwStartReal, dwStop, dwWait;

   _Serve ();

   if (pDB == NULL)
   return;

   // TO DO PTO_Watchdog_Reload(PTO_MAGIC_NB );
   dwStartReal = T5_GetTickCount ();   

   dwStart = ulStratonKernelTimeBase;
   
   T5Def_Cycle (pDB, dwStart, NULL);

   if(ulStratonKernelTimeBase < T5_24HMS)
   {
   	ulStratonKernelTimeBase+=50L;
   }
   else
   {
   	ulStratonKernelTimeBase = 0L;
   }

   dwStop = T5_GetTickCount ();
   dwWait = T5VM_RegisterCycleTime (pDB,
   ((T5_24HMS + dwStop - dwStartReal) % T5_24HMS) * 1000L);
   dwWait /= 1000L;
   if (dwWait == 0)
   dwWait = 1;

#if 0

   dwStart = T5_GetTickCount ();   
   T5Def_Cycle (pDB, dwStart, NULL);
   dwStop = T5_GetTickCount ();
   dwWait = T5VM_RegisterCycleTime (pDB,
   ((T5_24HMS + dwStop - dwStart) % T5_24HMS) * 1000L);
   dwWait /= 1000L;
   if (dwWait == 0)
   dwWait = 1;

#endif
   // dwWait = time before next cycle
}

void T5_Exit (void)
{
   _Exit_ ();
}

/* initialize / release VM data */

static void _Init (void)
{
   T5STR_CSSET settings;
   ulStratonKernelTimeBase = 0;

   dwDynaLink = T5VM_OpenDynaLinks ();
   T5_MEMSET (&mmData, 0, sizeof (mmData));
   T5_MEMSET (&csData, 0, sizeof (csData));
   pDB = NULL;
   pCode = NULL;
   T5MM_Open (&mmData, NULL);
   T5_MEMSET (&settings, 0, sizeof (settings));
   settings.szSerialConf = (T5_PTCHAR)"COM1"; // TODO
   settings.wEthernetPort = 1100;
   T5CS_Open (&csData, &settings, NULL);
}

static void _Exit_ (void)
{
   _Stop ();
   T5CS_Close (&csData);
   T5MM_Close (&mmData);
   T5VM_CloseDynaLinks (dwDynaLink);
}

/* comm server - only one request per cycle */

static void _Serve (void)
{
   T5_WORD wReq, wCaller, wRet;
   T5STR_CSLOAD load;
   T5STR_CSSTART start;

   //T5CS_Activate (&csData);
   //T5CS_ActivateStamp (&csData, T5_GetTickCount ());
   T5CS_ActivateStamp (&csData, ulStratonKernelTimeBase);//T5_GetTickCount ());
   if ((wReq = T5CS_GetQuestion (&csData, &wCaller)) != 0)
   {
      if (wReq == T5CSRQ_VMREQ)
      T5Def_ServeVMRequest (&csData, pDB, wCaller);
      /*
      else if (wReq == T5CSRQ_MODBUSRTU || wReq == T5CSRQ_MODBUSTCP)
      T5Def_ServeModbusRequest (&csData, pDB, wReq, wCaller);
      */
      else
      {
         wRet = T5RET_ERROR;
         switch (wReq)
         {
            case T5CSRQ_BEGINLOAD :
            case T5CSRQ_LOAD :
            case T5CSRQ_ENDLOAD :
               if (T5CS_GetLoadInfo (&csData, wCaller, &load)
               && load.bType == T5CSLOADCODE)
               {
                  wRet = T5MM_WriteCodeRequest (&mmData, wReq, &load);
                  /* auto start after successful download of code */
                  if (wRet == T5RET_OK && wReq == T5CSRQ_ENDLOAD)
                  {
                     start.bCC = start.bWarm = FALSE;
                     start.szName = load.szName;
                     //wRet = _Start (&start);
                  }
               }
               break;
            case T5CSRQ_START :
               if (T5CS_GetStartInfo (&csData, wCaller, &start))
               wRet = _Start (&start);
               break;
            case T5CSRQ_STOP :
               if (T5CS_GetStartInfo (&csData, wCaller, &start))
               wRet = _Stop ();
               break;
            default :
            break;
         }
         T5CS_SendRC (&csData, wCaller, wRet);
      }
      T5CS_ReleaseQuestion (&csData, wCaller);
   }
}

/* start / stop VM */

static T5_WORD _Start (T5PTR_CSSTART pStart)
{
   T5_WORD wRet = T5RET_ERROR;
   T5STR_DBTABLE tApp[T5MAX_TABLE];
   T5_DWORD dwSize;

   if (pDB == NULL && _LoadCode (pStart->szName))
   {
      T5_MEMSET (tApp, 0, sizeof (tApp));
      wRet = T5VM_GetDBSize (pCode, tApp);
      if (wRet == T5RET_OK)
      {
         dwSize = T5VM_GetTotalDBSize (tApp);
         wRet = T5MM_AllocRawVMDB (&mmData, dwSize);
         if (wRet == T5RET_OK)
         {
            pDB = (T5PTR_DB)T5MM_LinkVMDB (&mmData);
            if (pDB == NULL)
            wRet = T5RET_ERROR;
            else
            {
               memcpy (pDB, tApp, sizeof (tApp));
               T5VM_BuildDBMainLinks (pDB);
               wRet = T5Def_ColdStart (pCode, pDB, NULL, NULL,
                                         pStart->bWarm, pStart->bCC,
                                         pStart->szName);
            }
         }
      }
      if (wRet != T5RET_OK)
      {
         _FreeDB ();
         _FreeCode ();
      }
   }
   return wRet;
}

static T5_BOOL _LoadCode (T5_PTCHAR szName)
{
   _FreeCode ();
   if (T5MM_LoadCode (&mmData, szName) == T5RET_OK)
   pCode = T5MM_LinkCode (&mmData);
   return (pCode != NULL);
}

static T5_WORD _Stop (void)
{
   T5_WORD wRet = T5RET_ERROR;

   if (pDB != NULL)
   {
      T5Def_Stop (pDB, NULL, NULL, T5VMCode_GetAppName (pCode));
      _FreeDB ();
      _FreeCode ();
      wRet = 0;
   }
   return wRet;
}

static void _FreeCode (void)
{
   if (pCode != NULL)
   {
      T5MM_UnlinkCode (&mmData);
      T5MM_FreeCode (&mmData);
      pCode = NULL;
   }
}

static void _FreeDB (void)
{
   if (pDB != NULL)
   {
      T5MM_UnlinkVMDB (&mmData);
      T5MM_FreeVMDB (&mmData);
      pDB = NULL;
   }
}
