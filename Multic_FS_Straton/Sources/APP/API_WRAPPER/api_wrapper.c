////////////////////////////////////////////////////////////////////////////////
//              _________________________________________ ___ ___ ___
//             /  ___      ___  ________ ___ ___        /   /   /   /
//            /  /   |   /  __|/__   __//  //   | (R)  /___/___/___/
//           /  / /| | /  /      /  /  /  // /| |     /   /   /   /
//          /  / __  |(  (___   /  /  /  // __  |    /___/___/___/
//         /  /_/  |_| \_____| /__/  /__//_/  |_|   /   /   /   /
//        /________________________________________/___/___/___/
//----------------------------------------------------------------------------
// HEAD : (c) ACTIA Automotive 2015 : " Any copy and re-use of this 
//        file without the written consent of ACTIA is forbidden "
//        --------------------------------------------------------------------
//        Classification :  (-)No   (-)Confident. ACTIA   (-)Confident. Client
//        --------------------------------------------------------------------
//        API_WRAPPER : Localized Straton apllication Function pointer table
////////////////////////////////////////////////////////////////////////////////
// FILE : api_wrapper.c (SOURCE)
//----------------------------------------------------------------------------
// DESC : monitor's call-back pointer table localized in the application
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 805 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Sources/APP/API_WRAPPER/api_wrapper.c $
// Updated the :$Date: 2015-07-31 01:47:28 +0800 (周五, 31 七月 2015) $
// By 			:$Author: pcouronn $
////////////////////////////////////////////////////////////////////////////////
#define API_WRAPPER

////////////////////////////////////////////////////////////////////////////////
// INCLIB
////////////////////////////////////////////////////////////////////////////////
#include "std_type.h"
#include "monitor.h"
#include "api_wrapper.h"

////////////////////////////////////////////////////////////////////////////////
// INCUSER
////////////////////////////////////////////////////////////////////////////////

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
// PROTO
////////////////////////////////////////////////////////////////////////////////
void vWStraton_iInit_Hook(void);
void vWStraton_iAppMain_Task(void);
void vWStraton_iAppQuickTask(void);
int  vWStraton_iAppCustomDiag(unsigned char *FpucFrame,unsigned int FuiLength ,unsigned char FucRequestSource);
void vWStraton_iActiDebugProcessFrameReceived(void);
unsigned char vWStraton_iActiDebugProTestRx(unsigned short FusNbOfData);
TS32 s32WStraton_iDiagRDBI_Exe(TU16 Fu16DID, TU8 *Fpu8Buffer, TU16 Fu16LengthMax); 
TS32 s32WStraton_iDiagWDBI_Exe(TU16 Fu16DID, TU8 *Fpu8Buffer, TU16 Fu16LengthMax);

////////////////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////////////////
const TstWStraton_eApplication_Function __attribute__ ((section ("__StratonFunctions"))) CstWStraton_eApplication_Function =
{
   (TpfWStraton_eAppMainInit)             vWStraton_iInit_Hook,                     // pfAppMainInit
   (TpfWStraton_eAppMain)                 vWStraton_iAppMain_Task,                  // pfAppMain
   (TpfWStraton_eAppQuickTask)            vWStraton_iAppQuickTask,                  // pfAppQuickTask
   (TpfWStraton_eDiagCustomDiag)          vWStraton_iAppCustomDiag,                 // pfAppCustomDiag
   (TpfWStraton_eActiDebugProcessFrameRx) vWStraton_iActiDebugProcessFrameReceived, // pfActiDebugProcessFrameReceived
   (TpfWStraton_eActiDebugTestRx)         vWStraton_iActiDebugProTestRx,            // pfActiDebugProTestRx
   (Tpfs32WStraton_eDiagRDBI_Exe)					s32WStraton_iDiagRDBI_Exe,								// pfs32DiagRDBI_Exe   
   (Tpfs32WStraton_eDiagWDBI_Exe)					s32WStraton_iDiagWDBI_Exe									// pfs32DiagWDBI_Exe
};


////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// INTERN
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// REG
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// CODE
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Functions (Intern)
////////////////////////////////////////////////////////////////////////////////
//============================================================================//
// NAME : vWStraton_iInit_Hook
//============================================================================//
// ROLE : Application Init Hook
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vWStraton_iInit_Hook(void)
{
   vApp_CustomDiagInit();
   Application_Init_Hook();
}


//============================================================================//
// NAME : vWStraton_iAppMain_Task
//============================================================================//
// ROLE : Application main task 
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vWStraton_iAppMain_Task(void)
{
   Application_Background_Hook();
}

//============================================================================//
// NAME : vWStraton_iAppQuickTask
//============================================================================//
// ROLE : Application quick task 
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vWStraton_iAppQuickTask(void)
{
   
}

//============================================================================//
// NAME : vWStraton_iAppCustomDiag
//============================================================================//
// ROLE : Application customer diagnostic task 
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
int  vWStraton_iAppCustomDiag(unsigned char *FpucFrame,unsigned int FuiLength ,unsigned char FucRequestSource)
{
   int iReturn;
   iReturn = 0;
   return(iReturn);
}

//============================================================================//
// NAME : vWStraton_iActiDebugProcessFrameReceived
//============================================================================//
// ROLE : ActiGRAF debug protocol frame received 
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vWStraton_iActiDebugProcessFrameReceived(void)
{
   
}

//============================================================================//
// NAME : vWStraton_iActiDebugProTestRx
//============================================================================//
// ROLE : ActiGRAF debug protocol frame test reception 
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
unsigned char vWStraton_iActiDebugProTestRx(unsigned short FusNbOfData)
{
   unsigned char ucReturn;
   ucReturn=0;   
   return(ucReturn);
}


//============================================================================//
// NAME : s32WStraton_iDiagRDBI_Exe
//============================================================================//
// ROLE : Diagnostic function to read DID
//----------------------------------------------------------------------------//
// INPUT  : Fu16DID 				DID value 
//          Fpu8Buffer 			Pointer to data buffer of the answer
//          Fu16LengthMax		Lenght maximal of the possible response
// OUTPUT : Response Size 	if Positive
//					<Reserved>			if 0 (Delayed request / request again)
//          Error Code    	if Negative
//============================================================================//
TS32 s32WStraton_iDiagRDBI_Exe(TU16 Fu16DID, TU8 *Fpu8Buffer, TU16 Fu16LengthMax)
{
	return s32ADiag_eRDBI_Exe(Fu16DID, Fpu8Buffer, Fu16LengthMax);
}


//============================================================================//
// NAME : s32WStraton_iDiagWDBI_Exe
//============================================================================//
// ROLE : Diagnostic function to write DID
//----------------------------------------------------------------------------//
// INPUT  : Fu16DID 				DID value 
//          Fpu8Buffer 			Pointer to data buffer of the request
//          Fu16LengthMax		Lenght maximal of the data
// OUTPUT : Response Size 	if Positive
//					<Reserved>			if 0 (Delayed request / request again)
//          Error Code    	if Negative
//============================================================================//
TS32 s32WStraton_iDiagWDBI_Exe(TU16 Fu16DID, TU8 *Fpu8Buffer, TU16 Fu16LengthMax)
{
	return s32ADiag_eWDBI_Exe(Fu16DID, Fpu8Buffer, Fu16LengthMax);
}


////////////////////////////////////////////////////////////////////////////////
// Functions (Exported / IO API)
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Functions (Exported / IO API)
////////////////////////////////////////////////////////////////////////////////

