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
//        test_J1939 : J1939 test application
////////////////////////////////////////////////////////////////////////////////
// FILE : test_J1939.c (SOURCE)
//----------------------------------------------------------------------------
// DESC : J1939 protocol test application 
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 772 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Sources/APP/CORE/VM/custom/T5ACTIA.c $
// Updated the :$Date: 2015-07-22 14:16:15 +0200 (mer., 22 juil. 2015) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
/* test_J1939.c */

#define test_J1939
////////////////////////////////////////////////////////////////////////////////
// INCLIB
////////////////////////////////////////////////////////////////////////////////
#include "Monitor.h"
#include "bsp_com.h" // for test

////////////////////////////////////////////////////////////////////////////////
// INCUSER
////////////////////////////////////////////////////////////////////////////////
#include "test_J1939.h"

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

////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////




//****************************************************************************//
// DEFINITION
//****************************************************************************//

//****************************************************************************//
// MACRO
//****************************************************************************//


//****************************************************************************//
// TYPEDEF
//****************************************************************************//
typedef enum
{
	Ceu16AMain_iJ1939WaitAdressClaim,
	Ceu16AMain_iJ1939TestSend
} Teu16AMain_iJ1939State;


//****************************************************************************//
// PROTO
//****************************************************************************//

//****************************************************************************//
// CONST
//****************************************************************************//

//****************************************************************************//
// EXPORT
//****************************************************************************//


//****************************************************************************//
// INTERN
//****************************************************************************//
TstWJ1939_eConfData stAMain_iJ1939Conf;
Teu16AMain_iJ1939State eu16AMain_iJ1939State;
TU8 u8AMain_iECUHandle;

TU8	tu8AMain_iCyclicRxPGNData[Du8AMain_iCycRxPGNSize];
TU8	tu8AMain_iCyclicTxPGNData[Du8AMain_iCycTxPGNSize];
TU8	tu8AMain_iReqPGNData[Du8AMain_iReqPGNSize];

struct
{
	TU32 u32ErrCount;
	TstWJ1939_eErrorData stErrorCode;
}tstAMain_iError[Du8AMain_iNbOfInstance];

TU32 u32AMain_iRxCount[Du8AMain_iNbOfInstance];


//****************************************************************************//
// CODE
//****************************************************************************//

////////////////////////////////////////////////////////////////////////////////
// Functions (Intern)
////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
// Functions (Exported)
////////////////////////////////////////////////////////////////////////////////


//============================================================================//
// NAME : Application_Init_Hook
//============================================================================//
// ROLE : System Initialization
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vT5_J1939_Init (void)
{
	TS16 s16Status = 0;
	TU8 u8Count;

	eu16AMain_iJ1939State = Ceu16AMain_iJ1939WaitAdressClaim;
	u8AMain_iECUHandle = 0U;

	//---------------------------------------------------------------------------
	//Setup the J1939 Stack Instance #1 on CAN1 (used for data reception)
	//---------------------------------------------------------------------------
	stAMain_iJ1939Conf.u8InitCANBus		= 1;					//Request CAN bus configuration
	stAMain_iJ1939Conf.u16Baudrate_kbps	= 250U;				//250 k
	stAMain_iJ1939Conf.u8CtrlId			= Du16WCan_eController1;	//CAN1
	stAMain_iJ1939Conf.u8ProtocolId		= Du16WCan_eProtocol_3; //Du16WCan_eProtocol_1;		//Protocol 1
	stAMain_iJ1939Conf.u8MBSpRx			= Du16WCan_eMB_0;	//Must use MB_0 to receive
	stAMain_iJ1939Conf.u16SpMask		= Du16WCan_eMB_0;
	stAMain_iJ1939Conf.u8MBTx			= Du16WCan_eMB_16;
	stAMain_iJ1939Conf.u8TaskId			= 0;//Ceu16WCore_eAPP10Task;
	stAMain_iJ1939Conf.u8TaskInit		= 1;
	stAMain_iJ1939Conf.u8TaskPrio		= 1;
	stAMain_iJ1939Conf.pfnRxCallback	= vAMain_eJ1939RxCallBack_Exe;
	stAMain_iJ1939Conf.pfnErrorCallback = vAMain_eJ1939ErrCallBack_Exe;
	stAMain_iJ1939Conf.u8DeviceAddr		= 0x00;

	stAMain_iJ1939Conf.unDeviceName.stFields.u1AutoConfCapable 			= 	1;
	stAMain_iJ1939Conf.unDeviceName.stFields.u3IndustryGroup			=   0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u4VehicleSystemInstance 	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u7VehicleSystem         	=	0x3F; //Unknown
	stAMain_iJ1939Conf.unDeviceName.stFields.u1Reserved              	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u8Function              	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u5FunctionInstance      	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u3ECUInstance           	=	1;
	stAMain_iJ1939Conf.unDeviceName.stFields.u11ManufCode            	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u21IdNumber             	=   0x00124357;

	s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eInit_Exe(Du16WJ1939_eInstance1, &stAMain_iJ1939Conf);
	s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eRegisterNWMCallback_Exe(Du16WJ1939_eInstance1,vAMain_eJ1939NwmCallBack_Exe);

	//---------------------------------------------------------------------------
	//Setup the J1939 Stack Instance #2 sharing CAN1 with instance #1 (used for data reception)
	//---------------------------------------------------------------------------

	stAMain_iJ1939Conf.u8InitCANBus		= 0;					//Request CAN bus configuration
	stAMain_iJ1939Conf.u16Baudrate_kbps	= 250U;				//250 k
	stAMain_iJ1939Conf.u8CtrlId			= Du16WCan_eController1;	//CAN1
	stAMain_iJ1939Conf.u8ProtocolId		= Du16WCan_eProtocol_2;		//Protocol 2
	stAMain_iJ1939Conf.u8MBSpRx			= Du16WJ1939_eMBSharedInstance1;
	stAMain_iJ1939Conf.u16SpMask		= Du16WCan_eMB_0;
	stAMain_iJ1939Conf.u8MBTx			= Du16WCan_eMB_17;
	stAMain_iJ1939Conf.u8TaskId			= 0; //Ceu16WCore_eAPP10Task;
	stAMain_iJ1939Conf.u8TaskInit		= 0;
	stAMain_iJ1939Conf.u8TaskPrio		= 1;
	stAMain_iJ1939Conf.pfnRxCallback	= vAMain_eJ1939RxCallBack_Exe;
	stAMain_iJ1939Conf.pfnErrorCallback = vAMain_eJ1939ErrCallBack_Exe;
	stAMain_iJ1939Conf.u8DeviceAddr		= 0x01;

	stAMain_iJ1939Conf.unDeviceName.stFields.u1AutoConfCapable 			= 	1;
	stAMain_iJ1939Conf.unDeviceName.stFields.u3IndustryGroup			=   0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u4VehicleSystemInstance 	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u7VehicleSystem         	=	0x3F; //Unknown
	stAMain_iJ1939Conf.unDeviceName.stFields.u1Reserved              	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u8Function              	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u5FunctionInstance      	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u3ECUInstance           	=	2;
	stAMain_iJ1939Conf.unDeviceName.stFields.u11ManufCode            	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u21IdNumber             	=   0x00124357;

	s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eInit_Exe(Du16WJ1939_eInstance2, &stAMain_iJ1939Conf);
	s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eRegisterNWMCallback_Exe(Du16WJ1939_eInstance2,vAMain_eJ1939NwmCallBack_Exe);

	//---------------------------------------------------------------------------
	//Setup the J1939 Stack Instance #3 on CAN3 (used for data transmission)
	//---------------------------------------------------------------------------

	stAMain_iJ1939Conf.u8InitCANBus		= 1;					//Request CAN bus configuration
	stAMain_iJ1939Conf.u16Baudrate_kbps	= 250U;				//250 k
	stAMain_iJ1939Conf.u8CtrlId			= Du16WCan_eController3;	//CAN3
	stAMain_iJ1939Conf.u8ProtocolId		= Du16WCan_eProtocol_2; //Du16WCan_eProtocol_1;		//Protocol 1
	stAMain_iJ1939Conf.u8MBSpRx			= Du16WCan_eMB_0;		//Must use MB_0 to receive
	stAMain_iJ1939Conf.u16SpMask		= Du16WCan_eMB_0;
	stAMain_iJ1939Conf.u8MBTx			= Du16WCan_eMB_16;
	stAMain_iJ1939Conf.u8TaskId			= 0; //Ceu16WCore_eAPP10Task;
	stAMain_iJ1939Conf.u8TaskInit		= 0;
	stAMain_iJ1939Conf.u8TaskPrio		= 1;
	stAMain_iJ1939Conf.pfnRxCallback	= vAMain_eJ1939RxCallBack_Exe;
	stAMain_iJ1939Conf.pfnErrorCallback = vAMain_eJ1939ErrCallBack_Exe;
	stAMain_iJ1939Conf.u8DeviceAddr		= 0x00;
	
	stAMain_iJ1939Conf.unDeviceName.stFields.u1AutoConfCapable 			= 	1;
	stAMain_iJ1939Conf.unDeviceName.stFields.u3IndustryGroup			=   0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u4VehicleSystemInstance 	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u7VehicleSystem         	=	0x3F; //Unknown
	stAMain_iJ1939Conf.unDeviceName.stFields.u1Reserved              	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u8Function              	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u5FunctionInstance      	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u3ECUInstance           	=	3;
	stAMain_iJ1939Conf.unDeviceName.stFields.u11ManufCode            	=	0;
	stAMain_iJ1939Conf.unDeviceName.stFields.u21IdNumber             	=   0x00124357;

	s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eInit_Exe(Du16WJ1939_eInstance3, &stAMain_iJ1939Conf);
	s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eRegisterNWMCallback_Exe(Du16WJ1939_eInstance3,vAMain_eJ1939NwmCallBack_Exe);



}


//============================================================================//
// NAME : vT5_J1939_exe
//============================================================================//
// ROLE : J1939 Straton application
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vT5_J1939_exe(void)
{
	static TU32 u32TimeCount = 0;
	TS16 s16Status = 0;
  	static TU16 u16TimeCount = 0U;
	TU8 u8Count;
	TU8 u8State;
	TstWJ1939_eTPLMsgData stMsg;
	TU8 tu8Data[Du8AMain_iCycRxPGNSize];

   switch( eu16AMain_iJ1939State )
   {
      case Ceu16AMain_iJ1939WaitAdressClaim:
			/* Check instances address claims states */
			u8Count = 0;
			do
			{
				u8State = DstWMonitor_eApplication_Function.u8WJ1939_eGetState_Exe(u8Count);
				u8Count++;
			}while( (u8Count < Du8AMain_iNbOfInstance) && (u8State == Du16WJ1939_eACSuccess) );

			/* If AC succeed for every instances */
			if( u8Count == Du8AMain_iNbOfInstance )
			{
				// Register received the cyclic 0xFDD7 PGN on instance #1
				s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eRegisterAppPGN_Exe( Du16WJ1939_eInstance1, 0xFDD7, 0);
				s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eRegisterAppPGN_Exe( Du16WJ1939_eInstance2, 0xFDD7, 0);

				//Register Cyclic Tx PGN 0xFDD7 on instance #3
				for(u8Count = 0U; u8Count < Du8AMain_iCycTxPGNSize; u8Count++)
				{
					tu8AMain_iCyclicTxPGNData[u8Count] = u8Count;
				}
				s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eRegisterCycTxPGN_Exe( Du16WJ1939_eInstance3, //2, //3,			// Instance Number
																tu8AMain_iCyclicTxPGNData,		// PGN Data
																Du8AMain_iCycTxPGNSize,			// Data size
																500,							// cycle time
																Du8WJ1939_eGlobalAddress,		// Remote Broad Cast Addressing
																0xFDD7,							// PGN
																0,								// Data Page
																6,								// Priority
																1);								// PGN Cyclic Enabled

				// Handle the Request PGN 0xFDD5 on instance #2

				for(u8Count = 0U; u8Count < Du8AMain_iReqPGNSize; u8Count++)
				{
					tu8AMain_iReqPGNData[u8Count] = u8Count + 0x10;
				}
				s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eRegisterReqPGN_Exe(Du16WJ1939_eInstance1,			// Instance Number
																7U,							// Priority
																Du8AMain_iReqPGNSize,		// Length
																tu8AMain_iReqPGNData,		// Data
																0xFDD5,						// PGN
																0U);						// Data Page

				// Register received request PGN on instance #3

				s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eRegisterAppPGN_Exe( Du16WJ1939_eInstance3, 0xFDD5, 0);

				//Register a cyclic Rx PGN on instance #2

				s16Status |= DstWMonitor_eApplication_Function.s16WJ1939_eRegisterCycRxPGN_Exe( Du16WJ1939_eInstance2,
															tu8AMain_iCyclicRxPGNData,
															60,
															0xE000,
															0);

				eu16AMain_iJ1939State = Ceu16AMain_iJ1939TestSend;
			}
         break;

      case Ceu16AMain_iJ1939TestSend:

      // Send PGN request
         s16Status = DstWMonitor_eApplication_Function.s16WJ1939_eSendRequest_Exe( Du16WJ1939_eInstance3, Du8WJ1939_eGlobalAddress, 0U, 0xFDD5);

         // Send a message using indirect addressing

         if( u8AMain_iECUHandle != 0 )
         {
            u32TimeCount++;
            if( Du8AMain_iCycRxPGNSize >= 7 )
            {
               DstWMonitor_eApplication_Function.vWJ1939_eSetU8( tu8Data, 0, 0xf8);
               DstWMonitor_eApplication_Function.vWJ1939_eSetBits( tu8Data, 0, 1, 0x03, 0x01 );
               DstWMonitor_eApplication_Function.vWJ1939_eSetU16( tu8Data, 1, 0xc000 );
               DstWMonitor_eApplication_Function.vWJ1939_eSetMultiBytesBits( tu8Data, 1, 1, 0x3ff, 0x2ab );
               DstWMonitor_eApplication_Function.vWJ1939_eSetU32( tu8Data, 3, (TU32)u32TimeCount );
            }

            stMsg.u16Length = Du8AMain_iCycRxPGNSize;
            stMsg.u16PGN = 0xE000;
            stMsg.u8Addr = Du8WJ1939_eZeroAddress;
            stMsg.u8DataPage = 0U;
            stMsg.u8PrioType = 7;
            stMsg.u8Remote = u8AMain_iECUHandle;
            stMsg.pu8Data = tu8Data;
            s16Status = DstWMonitor_eApplication_Function.s16WJ1939_eSendMsg_Exe( Du16WJ1939_eInstance3, &stMsg);
         }
         else
         {
            // If handle is not available, wait for device by requesting address claim 
            s16Status = DstWMonitor_eApplication_Function.s16WJ1939_eSendRequest_Exe( Du16WJ1939_eInstance3, Du8WJ1939_eGlobalAddress, 0U, Du16WJ1939_ePGNAddressClaim);
         }

         break;

      default:
         break;

   }
   
}

//============================================================================//
// NAME : vAMain_eJ1939ErrCallBack_Exe
//============================================================================//
// ROLE : J1939 Stack Error Callback
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vAMain_eJ1939ErrCallBack_Exe(TU8 Fu8InstNum, TstWJ1939_eErrorData* FpstErrorDesc)
{
	tstAMain_iError[Fu8InstNum].u32ErrCount++;
	tstAMain_iError[Fu8InstNum].stErrorCode.u16ErrorCode = FpstErrorDesc->u16ErrorCode;
	tstAMain_iError[Fu8InstNum].stErrorCode.u32AddInfo = FpstErrorDesc->u32AddInfo;
}

//============================================================================//
// NAME : vAMain_eJ1939ErrCallBack_Exe
//============================================================================//
// ROLE : J1939 Stack Error Callback
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vAMain_eJ1939RxCallBack_Exe(TU8 Fu8InstNum, TstWJ1939_eTPLMsgData* FpstMsg)
{
	TU32 u32Data;
	TU16 u16Data;
	TU8 u8Data;
	TU16 u16BitData;
	TU8 u8BitData;

	u32AMain_iRxCount[Fu8InstNum]++;

	/* Check data */
	u32Data = DstWMonitor_eApplication_Function.u32WJ1939_eGetU32( FpstMsg->pu8Data, 1);
	u16Data = DstWMonitor_eApplication_Function.u16WJ1939_eGetU16( FpstMsg->pu8Data, 3);
	u8Data = DstWMonitor_eApplication_Function.u8WJ1939_eGetU8( FpstMsg->pu8Data, 2);
	u16BitData = DstWMonitor_eApplication_Function.u16WJ1939_eGetMultiBytesBits( FpstMsg->pu8Data, 	1,		//byte pos
																	3, 		//bit pos
																	0x01ff);//mask
	u8BitData = DstWMonitor_eApplication_Function.u8WJ1939_eGetBits( FpstMsg->pu8Data, 	3,		//byte pos
														1, 	  	//bit pos
														0x02);	//mask
}

//============================================================================//
// NAME : vAMain_eJ1939NwmCallBack_Exe
//============================================================================//
// ROLE : J1939 Network Callback
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vAMain_eJ1939NwmCallBack_Exe(TU8 Fu8InstNum, TU8 Fu8SA, TU8 * Fpu8Data)
{
	TS16 s16Status;
	TunName unClaimedDeviceName;

	DstWMonitor_eApplication_Function.vWJ1939_eGetDeviceName_Exe( Fpu8Data, &unClaimedDeviceName );

	/* Register the ECU instance 2 */
	if( (unClaimedDeviceName.stFields.u3ECUInstance == 2) && (Fu8InstNum == Du16WJ1939_eInstance3) )
	{
		s16Status = DstWMonitor_eApplication_Function.s16WJ1939_eRegisterDevice_Exe( Fu8InstNum, &unClaimedDeviceName, &u8AMain_iECUHandle );
	}
}

