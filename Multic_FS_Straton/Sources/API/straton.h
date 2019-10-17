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
//        straton : Straton app Export
////////////////////////////////////////////////////////////////////////////////
// FILE : straton.h (HEADER)
//----------------------------------------------------------------------------
// DESC : Straton app Export
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 637 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Sources/API/straton.h $
// Updated the :$Date: 2015-05-21 17:42:40 +0800 (周四, 21 五月 2015) $
// By 			:$Author: pcouronn $
////////////////////////////////////////////////////////////////////////////////
#ifndef STRATON_INCLUDE
#define STRATON_INCLUDE

//----------------------------------------------------------------------------//

extern unsigned long __StratonFunctions_start[];
extern unsigned long __StratonCode_start[];
extern unsigned long __StratonCode_end[];

////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// TYPEDEF
////////////////////////////////////////////////////////////////////////////////
typedef void            (* TpfWStraton_eAppMainInit)(void);
typedef void            (* TpfWStraton_eAppMain)(void);
typedef void            (* TpfWStraton_eAppQuickTask)(void);
typedef int             (* TpfWStraton_eDiagCustomDiag)(unsigned char *FpucFrame,unsigned int FuiLength ,unsigned char FucRequestSource);
typedef void            (* TpfWStraton_eActiDebugProcessFrameRx)(void);
typedef unsigned char   (* TpfWStraton_eActiDebugTestRx)(unsigned short FusNbOfData);
typedef TS32 						(* Tpfs32WStraton_eDiagRDBI_Exe)(TU16 Fu16DID, TU8 *Fpu8Buffer, TU16 Fu16LengthMax);
typedef TS32 						(* Tpfs32WStraton_eDiagWDBI_Exe)(TU16 Fu16DID, TU8 *Fpu8Buffer, TU16 Fu16LengthMax);

typedef struct
{
   TpfWStraton_eAppMainInit               pfAppMainInit;
   TpfWStraton_eAppMain                   pfAppMain;
   TpfWStraton_eAppQuickTask              pfAppQuickTask;
   TpfWStraton_eDiagCustomDiag            pfAppCustomDiag;
   TpfWStraton_eActiDebugProcessFrameRx   pfActiDebugProcessFrameReceived;
   TpfWStraton_eActiDebugTestRx           pfActiDebugProTestRx;
   Tpfs32WStraton_eDiagRDBI_Exe									pfs32DiagRDBI_Exe;  								// 6     0x018 
   Tpfs32WStraton_eDiagWDBI_Exe									pfs32DiagWDBI_Exe;  								// 7     0x01C 
} TstWStraton_eApplication_Function;

////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////
#define C_Straton_eAppFuncStartAdd  __StratonFunctions_start
#define DstWStraton_eAppliFunction   (*((TstWStraton_eApplication_Function *)C_Straton_eAppFuncStartAdd))

////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////


#endif	/*	STRATON_INCLUDE	*/
