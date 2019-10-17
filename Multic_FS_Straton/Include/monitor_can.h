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
//        monitor : CAN protocol definition
////////////////////////////////////////////////////////////////////////////////
// FILE : monitor_can.h (HEADER)
//----------------------------------------------------------------------------
// DESC :  monitor CAN protocol definition
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 669 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Include/monitor_can.h $
// Updated the :$Date: 2015-06-01 18:59:53 +0800 (周一, 01 六月 2015) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
#ifndef MONITOR_CAN_INCLUDE
#define MONITOR_CAN_INCLUDE

//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// TYPEDEF
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//CAN controllers List
//----------------------------------------------------------------------------
#define  Du16WMonitor_eCanController1     0U
#define  Du16WMonitor_eCanController2     1U
#define  Du16WMonitor_eCanController3     2U
#define  Du16WMonitor_eCanController4     3U
#define  Du16WMonitor_eCanController5     4U
#define  Du16WMonitor_eCanControllerNb    5U
#define  Du16WMonitor_eInvalidCtrl        0xFF

//MB modes
#define  Du16WMonitor_eCanMBModeUnused  0U
#define  Du16WMonitor_eCanMBModeStdRx   1U
#define  Du16WMonitor_eCanMBModeExtRx   2U
#define  Du16WMonitor_eCanMBModeStdTx   3U
#define  Du16WMonitor_eCanMBModeExtTx   4U

//CAN MB status
#define  Ds32WMonitor_eMBStatusBadParameter   -1
#define  Ds32WMonitor_eMBStatusMBNotSet       -2
#define  Ds32WMonitor_eMBStatusTxPending      1U    //Tx pending
#define  Ds32WMonitor_eMBStatusTxIdle         2U    //Tx MB is idle
#define  Ds32WMonitor_eMBStatusRxNewData      3U    //Rx MB has new data
#define  Ds32WMonitor_eMBStatusRxOverWrData   4U    //Rx MB has overwritten data
#define  Ds32WMonitor_eCanMBStatusRxIdle      5U    //Rx MB is idle

//CAN Bus status
#define Ds32WMonitor_eCanStatusBusOn             0U
#define Ds32WMonitor_eCanStatusStuffError        1U
#define Ds32WMonitor_eCanStatusFrameFormError    2U
#define Ds32WMonitor_eCanStatusFrameAckError     3U
#define Ds32WMonitor_eCanStatusFrameBit1Error    4U
#define Ds32WMonitor_eCanStatusFrameBit0Error    5U
#define Ds32WMonitor_eCanStatusFrameCrcError     6U
#define Ds32WMonitor_eCanStatusBusWarning        7U
#define Ds32WMonitor_eCanStatusBusError          8U
#define Ds32WMonitor_eCanStatusBusOff            9U

//----------------------------------------------------------------------------
//Protocol Id list
//----------------------------------------------------------------------------
#define Du16WMonitor_eNoProDefined    0x00
//#define Du16WMonitor_eNoProDefined  0x01
#define Du16WMonitor_eSWProtocol      0x02
#define Du16WMonitor_eJ1939AProtocol  0x03
#define Du16WMonitor_eJ1939BProtocol  0x04
//#define Du16WMonitor_eNoProDefined  0x05
//#define Du16WMonitor_eNoProDefined  0x06
#define Du16WMonitor_eDOCProtocol     0x07
#define Du16WMonitor_eUserProtocol    0x08
#define Du16WMonitor_ePowerProtocol   0x09
#define Du16WMonitor_eJ1939CProtocol  0x0A
#define Du16WMonitor_eJ1939DProtocol  0x0B
#define Du16WMonitor_eJ1939EProtocol  0x0C

#define Du16WMonitor_eProtocolNbr     0x0D
#define Du16WMonitor_eInvalidProtocol 0xFF

////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////

#endif	/*	MONITOR_CAN_INCLUDE	*/
