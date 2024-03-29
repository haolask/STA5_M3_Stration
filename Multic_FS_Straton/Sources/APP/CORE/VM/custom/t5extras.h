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
//        Straton VM : include files for T5
////////////////////////////////////////////////////////////////////////////////
// FILE : t5extras.h (HEADER)
//----------------------------------------------------------------------------
// DESC :  include files for T5
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 1181 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Sources/APP/CORE/VM/custom/t5extras.h $
// Updated the :$Date: 2016-03-03 20:43:39 +0800 (周四, 03 三月 2016) $
// By 			:$Author: pcouronn $
////////////////////////////////////////////////////////////////////////////////
#include "std_type.h"
#include "monitor.h"

////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// TYPEDEF
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
   TU8 u8ProtocolId;
   TU8 u8ProtocolChannel;
   TU8 u8ProtocolInstance;
   TU8 u8Spare;
}Tst_CANPORT;

typedef struct
{
//   TU8* pu8Data;
   TU16 u16MsgIndex;
   TU16 u16RxTimeStamp;
   TU8 u8NbOfTimeOutPeriod;
}Tst_CANMSG;

////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////
enum
{
   eu8CAN_UserProtocolId,
   eu8CAN_J1939ProtocolId,
   eu8CAN_NbOfSupportedProtocol,
   
   // error values:
   eu8CAN_NoProtocolId,
   eu8CAN_InvalidProtocolId
};

////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////

