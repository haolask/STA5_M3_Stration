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
//       Monitor IO API
////////////////////////////////////////////////////////////////////////////////
// FILE : monitor_IO.h (HEADER)
//----------------------------------------------------------------------------
// DESC : Monitor IO API
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 1260 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Include/monitor_IO.h $
// Updated the :$Date: 2016-04-19 23:38:47 +0800 (周二, 19 四月 2016) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
#ifndef MONITOR_IO_INCLUDE
#define MONITOR_IO_INCLUDE

//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////
#include "std_type.h"
#include "monitor_zed.h"

////////////////////////////////////////////////////////////////////////////////
// TYPEDEF
////////////////////////////////////////////////////////////////////////////////

// Multic-S IO Definitions & Structures
//--------------------------------------

// Frequency input
enum
{
	Ceu8MonitorIO_eFREQ1,
	Ceu8MonitorIO_eFREQ2,
	Ceu8MonitorIO_eNbOfFREQ
};
typedef struct{
	TU32	u32Ton;
	TU32	u32Toff;
	TU8		u8Bool;
	TU8		u8Status;
}TstMonitorIO_eFREQ;

typedef struct{
	TU16	u16Get_InVehSpeed;
	TU16	u16Get_InKCoef;
	TU16	u16Get_InEngSpeed;
}TstMonitorIO_eSPEEDIN;

// Analog inputs
enum
{
	Ceu8MonitorIO_eINA01,
	Ceu8MonitorIO_eINA02,
	Ceu8MonitorIO_eINA03,
	Ceu8MonitorIO_eINA04,
	Ceu8MonitorIO_eINA05,
	Ceu8MonitorIO_eINA06,
	Ceu8MonitorIO_eINA07,
	Ceu8MonitorIO_eINA08,
	Ceu8MonitorIO_eINA09,
	Ceu8MonitorIO_eINA10,
	Ceu8MonitorIO_eNbOfINA
};
typedef struct{
	TU32	u32Resistance;
	TU32	u32Voltage;
	TU8		u8Bool;
	TU8		u8Status;
}TstMonitorIO_eINA;

// Universal inputs
enum
{
	Ceu8MonitorIO_eUIN01,
	Ceu8MonitorIO_eUIN02,
	Ceu8MonitorIO_eUIN03,
	Ceu8MonitorIO_eUIN04,
	Ceu8MonitorIO_eUIN05,
	Ceu8MonitorIO_eUIN06,
	Ceu8MonitorIO_eUIN07,
	Ceu8MonitorIO_eUIN08,
	Ceu8MonitorIO_eUIN09,
	Ceu8MonitorIO_eUIN10,
	Ceu8MonitorIO_eUIN11,
	Ceu8MonitorIO_eUIN12,
	Ceu8MonitorIO_eUIN13,
	Ceu8MonitorIO_eUIN14,
	Ceu8MonitorIO_eUIN15,
	Ceu8MonitorIO_eUIN16,
	Ceu8MonitorIO_eUIN17,
	Ceu8MonitorIO_eUIN18,
	Ceu8MonitorIO_eUIN19,
	Ceu8MonitorIO_eUIN20,
	Ceu8MonitorIO_eUIN21,
	Ceu8MonitorIO_eUIN22,
	Ceu8MonitorIO_eUIN23,
	Ceu8MonitorIO_eUIN24,
	Ceu8MonitorIO_eNbOfUIN
};
typedef struct{
	TU32	u32Resistance;
	TU32	u32Voltage;
	TU8		u8Bool;
	TU8		u8Status;
}TstMonitorIO_eUIN;

// Logical inputs
enum
{
	Ceu8MonitorIO_eIN01,
	Ceu8MonitorIO_eIN02,
	Ceu8MonitorIO_eIN03,
	Ceu8MonitorIO_eIN04,
	Ceu8MonitorIO_eIN05,
	Ceu8MonitorIO_eIN06,
	Ceu8MonitorIO_eIN07,
	Ceu8MonitorIO_eIN08,
	Ceu8MonitorIO_eIN09,
	Ceu8MonitorIO_eIN10,
	Ceu8MonitorIO_eIN11,
	Ceu8MonitorIO_eIN12,
	Ceu8MonitorIO_eIN13,
	Ceu8MonitorIO_eIN14,
	Ceu8MonitorIO_eIN15,
	Ceu8MonitorIO_eIN16,
	Ceu8MonitorIO_eIN17,
	Ceu8MonitorIO_eIN18,
	Ceu8MonitorIO_eIN19,
	Ceu8MonitorIO_eIN20,
	Ceu8MonitorIO_eIN21,
	Ceu8MonitorIO_eIN22,
	Ceu8MonitorIO_eNbOfIN
};

// pushbutton inputs
enum
{
	Ceu8MonitorIO_ePB1,
	Ceu8MonitorIO_ePB2,
	Ceu8MonitorIO_ePB3,
	Ceu8MonitorIO_ePB4,
	Ceu8MonitorIO_ePB5,
	Ceu8MonitorIO_ePB6,
	Ceu8MonitorIO_eNbOfPB
};

// RTC	//SSpr_220216: Obsolete 3*18 = 54 bytes: 
/* 
enum
{
	Ceu8MonitorIO_eTimeDate,
	Ceu8MonitorIO_eAlarm1,
	Ceu8MonitorIO_eAlarm2,
	Ceu8MonitorIO_eNbOfRTCData
};
typedef struct{
	TU16	u16Year;
	TU16	u16Minute;
	TU16	u16Second;
	TU16	u16Date;
	TU16	u16Hour;
	TU16	u16Month;
	TU16	u16DayOfWeek;
	TU16	u16Rate;
	TU8		u8Status;
	TU8		u8Update;
}TstMonitorIO_eRTC;
*/

// Wake up
typedef struct{
	TU8		u8WK1;
	TU8		u8WK2;
	TU8		u8WK3;
	TU8		u8WK4;
	TU8		u8CAN1;
	TU8		u8CAN4;
	TU8		u8CAN5;
	TU8		u8RTC;
}TstMonitorIO_eWK;

// Outputs
enum
{
	Ceu8MonitorIO_eOUT01,
	Ceu8MonitorIO_eOUT02,
	Ceu8MonitorIO_eOUT03,
	Ceu8MonitorIO_eOUT04,
	Ceu8MonitorIO_eOUT05,
	Ceu8MonitorIO_eOUT06,
	Ceu8MonitorIO_eOUT07,
	Ceu8MonitorIO_eOUT08,
	Ceu8MonitorIO_eOUT09,
	Ceu8MonitorIO_eOUT10,
	Ceu8MonitorIO_eNbOfOUT
};
typedef struct{
	TU8		u8Command;
	TU16	u16DutyCycle;
	TU8		u8FailType;
	TU8		u8Status;
	TU16	u16Current;
}TstMonitorIO_eOUT;

// Sensor supplies
enum
{
	Ceu8MonitorIO_eSENSSUP1,
	Ceu8MonitorIO_eSENSSUP2,
	Ceu8MonitorIO_eNbOfSENSSUP
};
typedef struct{
	TU16	u16Command;
	TU8		u8Status;
}TstMonitorIO_eSENSSUP;

// Speed outputs
enum
{
	Ceu8MonitorIO_eSPEEDOUT1,
	Ceu8MonitorIO_eSPEEDOUT2,
	Ceu8MonitorIO_eNbOfSPEEDOUT
};
typedef struct{
	TU16	u16Set_OutVehSpeed;
	TU16	u16Set_OutEngSpeed;
	TU16	u16Set_OutFrequency;
	TU16	u16Set_OutDutyCycle;
	TU8		u8FailType;
	TU8		u8Status;
}TstMonitorIO_eSPEEDOUT;

// Warning Lights
enum
{
	Ceu8MonitorIO_eWL01,
	Ceu8MonitorIO_eWL02,
	Ceu8MonitorIO_eWL03,
	Ceu8MonitorIO_eWL04,
	Ceu8MonitorIO_eWL05,
	Ceu8MonitorIO_eWL06,
	Ceu8MonitorIO_eWL07,
	Ceu8MonitorIO_eWL08,
	Ceu8MonitorIO_eWL09,
	Ceu8MonitorIO_eWL10,
	Ceu8MonitorIO_eWL11,
	Ceu8MonitorIO_eWL12,
	Ceu8MonitorIO_eWL13,
	Ceu8MonitorIO_eWL14,
	Ceu8MonitorIO_eWL15,
	Ceu8MonitorIO_eWL16,
	Ceu8MonitorIO_eWL17,
	Ceu8MonitorIO_eWL18,
	Ceu8MonitorIO_eWL19,
	Ceu8MonitorIO_eWL20_1,
	Ceu8MonitorIO_eWL20_2,
	Ceu8MonitorIO_eWL20_3,
	Ceu8MonitorIO_eWL20_4,
	Ceu8MonitorIO_eWL20_5,
	Ceu8MonitorIO_eWL20_6,
	Ceu8MonitorIO_eNbOfWL
};

// Gauges
enum
{
	Ceu8MonitorIO_eGAUGE1,
	Ceu8MonitorIO_eGAUGE2,
	Ceu8MonitorIO_eGAUGE3,
	Ceu8MonitorIO_eGAUGE4,
	Ceu8MonitorIO_eNbOfGAUGE
};

// Distance
typedef struct{
	TU32	u32Get_Odometer;
	TU32	u32Set_Odometer;
	TU32	u32Get_Tripmeter;
	TU32    u32Set_Tripmeter;
	TU8		u8Reset_Tripmeter;
}TstMonitorIO_eDistance;


// Multic-S IO Card
//---------------------------------------------------------

typedef struct{
	TstMonitorIO_eFREQ			stFREQ[Ceu8MonitorIO_eNbOfFREQ];
	TstMonitorIO_eINA			stINA[Ceu8MonitorIO_eNbOfINA];
	TstMonitorIO_eUIN			stUIN[Ceu8MonitorIO_eNbOfUIN];
	TU16						u16VBAT_Voltage;
	TU16						u16VBATFS_Voltage;
	TU16						u16V3V3_Voltage;
	TU16						u16InternalTemperature;
	TU8							u8IN_Bool[Ceu8MonitorIO_eNbOfIN];
	TU16						u16BacklightFP_DutyCycle;																			//SSpr_220216: 8 to 16 bit (WORD)
	TU16						u16BacklightSC_DutyCycle;																			//SSpr_220216: 8 to 16 bit (WORD)
	TU8							u8PB_Bool[Ceu8MonitorIO_eNbOfPB];	
	TU8							u8SPARE[54-2];																								//SSpr_220216: Obsolete 3*18 = 54 bytes: TstMonitorIO_eRTC			stRTC[Ceu8MonitorIO_eNbOfRTCData];
	TstMonitorIO_eWK			stWK;
	TstMonitorIO_eOUT			stOUT[Ceu8MonitorIO_eNbOfOUT];
	TstMonitorIO_eSENSSUP		stSENSSUP[Ceu8MonitorIO_eNbOfSENSSUP];
	TstMonitorIO_eSPEEDIN		stSPEEDIN[Ceu8MonitorIO_eNbOfFREQ];
	TstMonitorIO_eSPEEDOUT		stSPEEDOUT[Ceu8MonitorIO_eNbOfSPEEDOUT];
	TU8							u8WL_Command[Ceu8MonitorIO_eNbOfWL];
	TU8							u8WL02_Status;
	TU8							u8WL03_Color;
	TU16						u16GAUGE_Angle[Ceu8MonitorIO_eNbOfGAUGE];
	TstMonitorIO_eDistance		stDistance;
}TstMonitorIO_eCard;

////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////
#define DpstMonitorIO ((TstMonitorIO_eCard*)DU32ZED_Addr_Multic_FS_Start)

////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////

#endif	/*	MONITOR_IO_INCLUDE	*/
