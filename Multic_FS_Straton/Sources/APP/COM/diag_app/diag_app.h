////////////////////////////////////////////////////////////////////////////////
//
//      O       OOOO  OOOOOOO   O       O         ## ## ##
//     O O     O         O      O      O O
//    O   O    O         O      O     O   O      ## ## ##
//   OOOOOOO   O         O      O    OOOOOOO
//  O       O   OOOO     O      O   O       O   ## ## ##
//
//----------------------------------------------------------------------------
// HEAD : (c) ACTIA Automotive 2014 : " Any copy and re-use of this
//        file without the written consent of ACTIA is forbidden "
//        --------------------------------------------------------------------
//        Classification :  (-)No   (-)Confident. ACTIA   (-)Confident. Client
//        --------------------------------------------------------------------
//        UDS_APP : API Validation Test Application Layer
////////////////////////////////////////////////////////////////////////////////
// FILE : diag_app.h (HEADER)
//----------------------------------------------------------------------------
// DESC : API Diagnostic Application Layer : handles execution and results
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 1783 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Sources/APP/COM/diag_app/diag_app.h $HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_BSP/Sources/BSP/COM/kwp_pro/kwp_pro.h $
// Updated the :$Date: 2017-04-07 23:10:02 +0800 (周五, 07 四月 2017) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
#ifndef DIAG_APP_INCLUDE
#define DIAG_APP_INCLUDE

//----------------------------------------------------------------------------//
#include "diag_app.cfg"

//----------------------------------------------------------------------------//

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
// EXPORT
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////

//============================================================================//
// NAME : s32ADiag_eInit_Exe
//============================================================================//
// ROLE : Diagnostic function to initialize this DIAG module
//----------------------------------------------------------------------------//
// INPUT  : 
// OUTPUT : 
//============================================================================//
void vADiag_eInit_Exe(void);


//============================================================================//
// NAME : s32WEmWizard_iDiagRDBI_Exe
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
TS32 s32ADiag_eRDBI_Exe(TU16 Fu16DID, TU8 *Fpu8Buffer, TU16 Fu16LengthMax);


//============================================================================//
// NAME : s32ADiag_eWDBI_Exe
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
TS32 s32ADiag_eWDBI_Exe(TU16 Fu16DID, TU8 *Fpu8Buffer, TU16 Fu16LengthMax);


#endif	/*	DIAG_APP_INCLUDE	*/
