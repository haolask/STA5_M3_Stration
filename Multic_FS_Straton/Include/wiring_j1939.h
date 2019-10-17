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
//        CALIB : Board Calibration
////////////////////////////////////////////////////////////////////////////////
// FILE : boot.h (CONFIGURATION)
//----------------------------------------------------------------------------//
// DESC : Board Calibration
//----------------------------------------------------------------------------//
// HIST :
// Version 		:$Revision: 1307 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_WIRING/Sources/wiring_j1939.h $
// Updated the :$Date: 2016-05-24 01:23:08 +0800 (周二, 24 五月 2016) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
#ifndef WIRING_J1939_INCLUDE
#define WIRING_J1939_INCLUDE

#include "std_type.h"
#include "ver_type.h"


// Multic-S J1939 Configuration and Parameters definitions
//--------------------------------------------------------
#define Du8Wiring_eJ1939_MaxNbOfMB 			64																			//Max MB's of a channel



// Multic-S J1939 Configuration and Parameters types
//--------------------------------------------------
// J1939 Configuration of the channel
typedef struct{
	TU16	u16Spare_1;
	TU16  u16Spare_2;
	TU16  u16NbRx;																																//Number of MB's for (Du16WCan_eCanMBModeExtRx)
	TU16  u16Spare_4;
	TU16  u16Spare_5;
	TU16  u16Spare_6;
	TU16  u16Spare_7;
	TU16  u16Spare_8;
}TstWiring_eJ1939_CONFIG;


typedef struct{
	TU16 	u16MBId;						//Message buffer ID [0 .. 63]												//Used by (s32WCan_eSetupMB_Exe) or (s32WCan_eRXMask_Set)
	TU16 	u16Spare_1;
	TU32	u32Ident;						//CAN identifier 		[0 .. 0x1FFF.FFFF]							//Used by (s32WCan_eSetupMB_Exe)
	TU32  u32RxMask;					//Acceptance mask		[0 .. 0x1FFF.FFFF, 0xFFFF.FFFF]	//If not 0xFFFF.FFFF: Used by (s32WCan_eRXMask_Set) 
	TU16  u16Spare_2;
	TU16  u16Spare_3;	
}TstWiring_eJ1939_PARAM;



// Multic-S J1939 Configuration and Parameters structure type
//-----------------------------------------------------------
typedef struct{
	TstWiring_eJ1939_CONFIG 	stConfig;
	TstWiring_eJ1939_PARAM		tstParam[Du8Wiring_eJ1939_MaxNbOfMB];
}TstWiring_eJ1939_ConfigParam;


#endif
