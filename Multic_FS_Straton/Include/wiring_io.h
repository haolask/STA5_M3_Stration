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
// Version 		:$Revision: 1134 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Include/wiring_io.h $
// Updated the :$Date: 2016-02-11 21:49:19 +0800 (周四, 11 二月 2016) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
#ifndef WIRING_IO_INCLUDE
#define WIRING_IO_INCLUDE

#include "std_type.h"
#include "ver_type.h"

// Multic-S IO Parameters Definitions & Structures
//--------------------------------------
// Frequency input
enum
{
	Ceu8WiringIO_eFREQ1,
	Ceu8WiringIO_eFREQ2,
	Ceu8WiringIO_eNbOfFREQ
};
typedef struct{
	TU8   u8SourceSelectIn;
	TU8   u8TimeOut;
}TstWiringIO_eFREQ;

typedef struct{
	TU16   u16Set_InKCoef;
	TU16   u16Set_InEngFactor;
}TstWiringIO_eSPEEDIN;

// Analog inputs
enum
{
	Ceu8WiringIO_eINA01,
	Ceu8WiringIO_eINA02,
	Ceu8WiringIO_eINA03,
	Ceu8WiringIO_eINA04,
	Ceu8WiringIO_eINA05,
	Ceu8WiringIO_eINA06,
	Ceu8WiringIO_eINA07,
	Ceu8WiringIO_eINA08,
	Ceu8WiringIO_eINA09,
	Ceu8WiringIO_eINA10,
	Ceu8WiringIO_eNbOfINA
};

typedef struct{
	TU16   	u16Lthreshold;
	TU16   	u16Hthreshold;
	TU8		u8Filter;
	TU8		u8Mode;
	TU8		u8Pull_Up;
}TstWiringIO_eINA;

// Universal inputs
enum
{
	Ceu8WiringIO_eUIN01,
	Ceu8WiringIO_eUIN02,
	Ceu8WiringIO_eUIN03,
	Ceu8WiringIO_eUIN04,
	Ceu8WiringIO_eUIN05,
	Ceu8WiringIO_eUIN06,
	Ceu8WiringIO_eUIN07,
	Ceu8WiringIO_eUIN08,
	Ceu8WiringIO_eUIN09,
	Ceu8WiringIO_eUIN10,
	Ceu8WiringIO_eUIN11,
	Ceu8WiringIO_eUIN12,
	Ceu8WiringIO_eUIN13,
	Ceu8WiringIO_eUIN14,
	Ceu8WiringIO_eUIN15,
	Ceu8WiringIO_eUIN16,
	Ceu8WiringIO_eUIN17,
	Ceu8WiringIO_eUIN18,
	Ceu8WiringIO_eUIN19,
	Ceu8WiringIO_eUIN20,
	Ceu8WiringIO_eUIN21,
	Ceu8WiringIO_eUIN22,
	Ceu8WiringIO_eUIN23,
	Ceu8WiringIO_eUIN24,
	Ceu8WiringIO_eNbOfUIN
};
typedef struct{
	TU16   	u16Lthreshold;
	TU16   	u16Hthreshold;
	TU8		u8Filter;
	TU8		u8Mode;
	TU8		u8Pull_Up;
}TstWiringIO_eUIN;

// Logical inputs
enum
{
	Ceu8WiringIO_eIN01,
	Ceu8WiringIO_eIN02,
	Ceu8WiringIO_eIN03,
	Ceu8WiringIO_eIN04,
	Ceu8WiringIO_eIN05,
	Ceu8WiringIO_eIN06,
	Ceu8WiringIO_eIN07,
	Ceu8WiringIO_eIN08,
	Ceu8WiringIO_eIN09,
	Ceu8WiringIO_eIN10,
	Ceu8WiringIO_eIN11,
	Ceu8WiringIO_eIN12,
	Ceu8WiringIO_eIN13,
	Ceu8WiringIO_eIN14,
	Ceu8WiringIO_eIN15,
	Ceu8WiringIO_eIN16,
	Ceu8WiringIO_eIN17,
	Ceu8WiringIO_eIN18,
	Ceu8WiringIO_eIN19,
	Ceu8WiringIO_eIN20,
	Ceu8WiringIO_eIN21,
	Ceu8WiringIO_eIN22,
	Ceu8WiringIO_eIN23,
	Ceu8WiringIO_eIN24,
	Ceu8WiringIO_eIN25,
	Ceu8WiringIO_eIN26,
	Ceu8WiringIO_eIN27,
	Ceu8WiringIO_eIN28,
	Ceu8WiringIO_eIN29,
	Ceu8WiringIO_eIN30,
	Ceu8WiringIO_eIN31,
	Ceu8WiringIO_eIN32,
	Ceu8WiringIO_eIN33,
	Ceu8WiringIO_eIN34,
	Ceu8WiringIO_eIN35,
	Ceu8WiringIO_eIN36,
	Ceu8WiringIO_eIN37,
	Ceu8WiringIO_eIN38,
	Ceu8WiringIO_eIN39,
	Ceu8WiringIO_eIN40,
	Ceu8WiringIO_eIN41,
	Ceu8WiringIO_eIN42,
	Ceu8WiringIO_eIN43,
	Ceu8WiringIO_eIN44,
	Ceu8WiringIO_eIN45,
	Ceu8WiringIO_eIN46,
	Ceu8WiringIO_eIN47,
	Ceu8WiringIO_eIN48,
	Ceu8WiringIO_eIN49,
	Ceu8WiringIO_eIN50,
	Ceu8WiringIO_eIN51,
	Ceu8WiringIO_eIN52,
	Ceu8WiringIO_eIN53,
	Ceu8WiringIO_eIN54,
	Ceu8WiringIO_eIN55,
	Ceu8WiringIO_eIN56,
	Ceu8WiringIO_eIN57,
	Ceu8WiringIO_eIN58,
	Ceu8WiringIO_eIN59,
	Ceu8WiringIO_eIN60,
	Ceu8WiringIO_eIN61,
	Ceu8WiringIO_eIN62,
	Ceu8WiringIO_eIN63,
	Ceu8WiringIO_eIN64,
	Ceu8WiringIO_eIN65,
	Ceu8WiringIO_eIN66,
	Ceu8WiringIO_eIN67,
	Ceu8WiringIO_eIN68,
	Ceu8WiringIO_eIN69,
	Ceu8WiringIO_eIN70,
	Ceu8WiringIO_eIN71,
	Ceu8WiringIO_eIN72,
	Ceu8WiringIO_eIN73,
	Ceu8WiringIO_eIN74,
	Ceu8WiringIO_eIN75,
	Ceu8WiringIO_eIN76,
	Ceu8WiringIO_eIN77,
	Ceu8WiringIO_eIN78,
	Ceu8WiringIO_eIN79,
	Ceu8WiringIO_eIN80,
	Ceu8WiringIO_eIN81,
	Ceu8WiringIO_eIN82,
	Ceu8WiringIO_eIN83,
	Ceu8WiringIO_eIN84,
	Ceu8WiringIO_eIN85,
	Ceu8WiringIO_eIN86,
	Ceu8WiringIO_eIN87,
	Ceu8WiringIO_eIN88,
	Ceu8WiringIO_eIN89,
	Ceu8WiringIO_eIN90,
	Ceu8WiringIO_eIN91,
	Ceu8WiringIO_eIN92,
	Ceu8WiringIO_eIN93,
	Ceu8WiringIO_eIN94,
	Ceu8WiringIO_eIN95,
	Ceu8WiringIO_eIN96,
	Ceu8WiringIO_eIN97,
	Ceu8WiringIO_eIN98,
	Ceu8WiringIO_eIN99,
	Ceu8WiringIO_eIN100,
	Ceu8WiringIO_eIN101,
	Ceu8WiringIO_eIN102,
	Ceu8WiringIO_eIN103,
	Ceu8WiringIO_eIN104,
	Ceu8WiringIO_eIN105,
	Ceu8WiringIO_eIN106,
	Ceu8WiringIO_eIN107,
	Ceu8WiringIO_eIN108,
	Ceu8WiringIO_eIN109,
	Ceu8WiringIO_eIN110,
	Ceu8WiringIO_eIN111,
	Ceu8WiringIO_eIN112,
	Ceu8WiringIO_eIN113,
	Ceu8WiringIO_eIN114,
	Ceu8WiringIO_eIN115,
	Ceu8WiringIO_eIN116,
	Ceu8WiringIO_eIN117,
	Ceu8WiringIO_eIN118,
	Ceu8WiringIO_eIN119,
	Ceu8WiringIO_eIN120,
	Ceu8WiringIO_eIN121,
	Ceu8WiringIO_eIN122,
	Ceu8WiringIO_eIN123,
	Ceu8WiringIO_eIN124,
	Ceu8WiringIO_eIN125,
	Ceu8WiringIO_eIN126,
	Ceu8WiringIO_eIN127,
	Ceu8WiringIO_eIN128,
	Ceu8WiringIO_eIN129,
	Ceu8WiringIO_eIN130,
	Ceu8WiringIO_eIN131,
	Ceu8WiringIO_eIN132,
	Ceu8WiringIO_eIN133,
	Ceu8WiringIO_eIN134,
	Ceu8WiringIO_eIN135,
	Ceu8WiringIO_eNbOfIN
};

// Outputs
enum
{
	Ceu8WiringIO_eOUT01,
	Ceu8WiringIO_eOUT02,
	Ceu8WiringIO_eOUT03,
	Ceu8WiringIO_eOUT04,
	Ceu8WiringIO_eOUT05,
	Ceu8WiringIO_eOUT06,
	Ceu8WiringIO_eOUT07,
	Ceu8WiringIO_eOUT08,
	Ceu8WiringIO_eOUT09,
	Ceu8WiringIO_eOUT10,
	Ceu8WiringIO_eOUT11,
	Ceu8WiringIO_eOUT12,
	Ceu8WiringIO_eOUT13,
	Ceu8WiringIO_eOUT14,
	Ceu8WiringIO_eOUT15,
	Ceu8WiringIO_eNbOfOUT
};

typedef struct{
	TU16   	u16Frequency;
	TU8   	u8Dynamic;
	TU8		u8LoadTyper;
	TU8		u8OC_RANGE;
	TU8		u8OC_NTHLD;
	TU8		u8OC_MTHLD;
	TU8		u8OC_HTHLD;
	TU8		u8OC_TIME;
}TstWiringIO_eOUT;

// Speed outputs
enum
{
	Ceu8WiringIO_eSPEEDOUT1,
	Ceu8WiringIO_eSPEEDOUT2,
	Ceu8WiringIO_eNbOfSPEEDOUT
};

typedef struct{
	TU16	u16Set_OutKCoef;
	TU16	u16Set_OutEngFactor;
	TU8		u8SourceSelectOut;
	TU8		u8SpeedOutMode;
}TstWiringIO_eSPEEDOUT;

// Multic-S IO Parameter
//---------------------------------------------------------
typedef struct{
	TstWiringIO_eFREQ     	stFREQ[Ceu8WiringIO_eNbOfFREQ];
	TstWiringIO_eINA				stINA[Ceu8WiringIO_eNbOfINA];
	TstWiringIO_eUIN				stUIN[Ceu8WiringIO_eNbOfUIN];
	TU8											u8IN_Filter[Ceu8WiringIO_eNbOfIN];
	TstWiringIO_eOUT				stOUT[Ceu8WiringIO_eNbOfOUT];
	TstWiringIO_eSPEEDIN		stSPEEDIN[Ceu8WiringIO_eNbOfFREQ];
	TstWiringIO_eSPEEDOUT		stSPEEDOUT[Ceu8WiringIO_eNbOfSPEEDOUT];
	TU8											u8DISTANCE_SourceSelectIn;
}TstWiringIO_eParameter;


#endif
