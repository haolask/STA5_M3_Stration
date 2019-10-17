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
//        monitor : Monitor app Export
////////////////////////////////////////////////////////////////////////////////
// FILE : monitor.h (HEADER)
//----------------------------------------------------------------------------
// DESC : Monitor app Export
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 669 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Include/monitor_core.h $
// Updated the :$Date: 2015-06-01 18:59:53 +0800 (周一, 01 六月 2015) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
#ifndef MONITOR_CORE_INCLUDE
#define MONITOR_CORE_INCLUDE

////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////


//****************************************************************************//
// TYPEDEF
//****************************************************************************//
//----------------------------------------------------------------------------
// Time
//----------------------------------------------------------------------------
typedef struct
{
	TU8	u8SecondValue;		// Seconds (0 to 59)
	TU8	u8MinuteValue;		// Minuts (0 to 59)
	TU8	u8HourValue;		// Hours (0 to 23)
	TU8	u8WeekDayValue;	// Weekday (0 to 6), Sunday = 0 ... Saturday = 6
	TU8	u8DateDayValue;	// Dateday (1 to 31)
	TU8	u8MonthValue;		// Month (1 to 12)
	TU16	u16YearValue;		// Year (1900 to 2099)
} TstARtc_eRTCStruct;

// Alarm
typedef struct
{
	TU8	u8MinuteValue;		// Mxxx.xxxx b : Minute alarm
 					// M [0: inactive, 1: active], Minuts x (0 to 59)

	TU8	u8HourValue;		// Hxxx.xxxx b : Hour alarm
					// H [0: inactive, 1: active], Hours  x (0 to 23)

	TU8	u8DayValue;    		// Dxxx.xxxx b : Day alarm
					// D [0: inactive, 1: active], Day x     (1 to 31)

	TU8	u8WeekDayValue;   	// Wxxx.xxxx b : WeekDay alarm
					// W [0: inactive, 1: active], Weekday (0 to 6 = Sunday to Saturday)

	TU8	u8WakeUp;		// WakeUp function on alarm [0: inactive, 1: active]

} TstARtc_eRTCAlarm;

typedef struct
{
	TU8 u8Function; // Txxx.xxxx b : Timer function
	// T [0: inactive, 1: active], Function x (2: Seconds timer or 3:Minutes timer)
	TU8 u8CountDown; // Count-Down value (0 to 255)
	TU8 u8WakeUp; // WakeUp function on Timer expiration [0: inactive, 1: active]
} TstARtc_eRTCTimer;

////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////

#endif	/*	MONITOR_INCLUDE	*/
