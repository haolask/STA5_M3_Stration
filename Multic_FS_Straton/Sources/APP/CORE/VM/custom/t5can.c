/*****************************************************************************
T5Can.c :    CAN-bus hardware interface - TO BE FILLED WHEN PORTING

(c) COPALP 2006
*****************************************************************************/

#include "t5vm.h"
#include "std_type.h"
#include "monitor.h"
#include "wiring.h"
#include "T5_J1939.h"

#ifdef T5DEF_CANBUS

T5_WORD wUserProMsgIndex[Du16WMonitor_eCanControllerNb];
T5_BYTE bUserProAvailableInConf[Du16WMonitor_eCanControllerNb];
T5_WORD wUserProErrorCode[Du16WMonitor_eCanControllerNb];
//---------------------------------------------------------------
// ACTIA OEM variable definitions
//---------------------------------------------------------------

// Hardware Specific Status
//-------------------------
#define T5CAN_HwStatus_InvalidPortSettings   0x00000001  // bit0
#define T5CAN_HwStatus_ProtocolNotAvailable  0x00000002  // bit1
//#define T5CAN_HwStatus_TBD                 0x00000004  // bit2
//...
//#define T5CAN_HwStatus_TBD                 0x80000000  // bit31

// Hardware Specific Control
//-------------------------
#define T5CAN_HwControl_StopMsgTx            0x00000001  // bit0
//#define T5CAN_HwControl_TBD                0x00000002  // bit1
//#define T5CAN_HwControl_TBD                0x00000004  // bit2
//...
//#define T5CAN_HwControl_TBD                0x80000000  // bit31

// CAN Network Config
//-------------------------
#define CucCANNetworkConfiguration ((Tu8CANNetworkConfiguration*)DstWiring_eCanProtocolAddr)

//CAN event
//-------------------------
void vPUSER_iCANError_Idc1(T5_WORD wErrorCode);
void vPUSER_iCANError_Idc2(T5_WORD wErrorCode);
void vPUSER_iCANError_Idc3(T5_WORD wErrorCode);
void vPUSER_iCANError_Idc4(T5_WORD wErrorCode);
void vPUSER_iCANError_Idc5(T5_WORD wErrorCode);

void vPUSER_iCANReceive_Idc1(T5_WORD wObjectId, T5_WORD wStatus);
void vPUSER_iCANReceive_Idc2(T5_WORD wObjectId, T5_WORD wStatus);
void vPUSER_iCANReceive_Idc3(T5_WORD wObjectId, T5_WORD wStatus);
void vPUSER_iCANReceive_Idc4(T5_WORD wObjectId, T5_WORD wStatus);
void vPUSER_iCANReceive_Idc5(T5_WORD wObjectId, T5_WORD wStatus);

void vPUSER_iCANTransmit_Cfm1(T5_WORD wObjectId, T5_WORD wStatus);
void vPUSER_iCANTransmit_Cfm2(T5_WORD wObjectId, T5_WORD wStatus);
void vPUSER_iCANTransmit_Cfm3(T5_WORD wObjectId, T5_WORD wStatus);
void vPUSER_iCANTransmit_Cfm4(T5_WORD wObjectId, T5_WORD wStatus);
void vPUSER_iCANTransmit_Cfm5(T5_WORD wObjectId, T5_WORD wStatus);

TfPUSER_eEvt* pvCANError_Idc[Du16WMonitor_eCanControllerNb]=
{
   (TfPUSER_eEvt*)vPUSER_iCANError_Idc1,
   (TfPUSER_eEvt*)vPUSER_iCANError_Idc2,
   (TfPUSER_eEvt*)vPUSER_iCANError_Idc3,
   (TfPUSER_eEvt*)vPUSER_iCANError_Idc4,
   (TfPUSER_eEvt*)vPUSER_iCANError_Idc5
};

TfPUSER_eEvtObj* pvCANReceive_Idc[Du16WMonitor_eCanControllerNb]=
{
   (TfPUSER_eEvtObj*)vPUSER_iCANReceive_Idc1,
   (TfPUSER_eEvtObj*)vPUSER_iCANReceive_Idc2,
   (TfPUSER_eEvtObj*)vPUSER_iCANReceive_Idc3,
   (TfPUSER_eEvtObj*)vPUSER_iCANReceive_Idc4,
   (TfPUSER_eEvtObj*)vPUSER_iCANReceive_Idc5
};

TfPUSER_eEvtObj* pvCANTransmit_Cfm[Du16WMonitor_eCanControllerNb]=
{
   (TfPUSER_eEvtObj*)vPUSER_iCANTransmit_Cfm1,
   (TfPUSER_eEvtObj*)vPUSER_iCANTransmit_Cfm2,
   (TfPUSER_eEvtObj*)vPUSER_iCANTransmit_Cfm3,
   (TfPUSER_eEvtObj*)vPUSER_iCANTransmit_Cfm4,
   (TfPUSER_eEvtObj*)vPUSER_iCANTransmit_Cfm5
};

/*****************************************************************************
vPUSER_iCANError_Idc
Called by the monitor CAN User protocol in case of error event.
*****************************************************************************/
void vPUSER_iCANError_Idc(T5_WORD wErrorCode, T5_BYTE  bCANCtrlId)
{
   wUserProErrorCode[bCANCtrlId] = wErrorCode;
   return;
}
void vPUSER_iCANError_Idc1(T5_WORD wErrorCode)
{
   vPUSER_iCANError_Idc(wErrorCode, Du16WMonitor_eCanController1);
   return;
}
void vPUSER_iCANError_Idc2(T5_WORD wErrorCode)
{
   vPUSER_iCANError_Idc(wErrorCode, Du16WMonitor_eCanController2);
   return;
}
void vPUSER_iCANError_Idc3(T5_WORD wErrorCode)
{
   vPUSER_iCANError_Idc(wErrorCode, Du16WMonitor_eCanController3);
   return;
}
void vPUSER_iCANError_Idc4(T5_WORD wErrorCode)
{
   vPUSER_iCANError_Idc(wErrorCode, Du16WMonitor_eCanController4);
   return;
}
void vPUSER_iCANError_Idc5(T5_WORD wErrorCode)
{
   vPUSER_iCANError_Idc(wErrorCode, Du16WMonitor_eCanController5);
   return;
}

/*****************************************************************************
vPUSER_iCANReceive_Idc
Called by the monitor CAN User protocol in case of reception event.
*****************************************************************************/
void vPUSER_iCANReceive_Idc(T5_WORD wObjectId, T5_WORD wStatus, T5_BYTE  bCANCtrlId)
{
   // Message recieved: reset CAN errors
   wUserProErrorCode[bCANCtrlId] = Ds32WMonitor_eCanStatusBusOn; //(=0)
   return;
}
void vPUSER_iCANReceive_Idc1(T5_WORD wObjectId, T5_WORD wStatus)
{
   vPUSER_iCANReceive_Idc(wObjectId, wStatus, Du16WMonitor_eCanController1);
   return;
}
void vPUSER_iCANReceive_Idc2(T5_WORD wObjectId, T5_WORD wStatus)
{
   vPUSER_iCANReceive_Idc(wObjectId, wStatus, Du16WMonitor_eCanController2);
   return;
}
void vPUSER_iCANReceive_Idc3(T5_WORD wObjectId, T5_WORD wStatus)
{
   vPUSER_iCANReceive_Idc(wObjectId, wStatus, Du16WMonitor_eCanController3);
   return;
}
void vPUSER_iCANReceive_Idc4(T5_WORD wObjectId, T5_WORD wStatus)
{
   vPUSER_iCANReceive_Idc(wObjectId, wStatus, Du16WMonitor_eCanController4);
   return;
}
void vPUSER_iCANReceive_Idc5(T5_WORD wObjectId, T5_WORD wStatus)
{
   vPUSER_iCANReceive_Idc(wObjectId, wStatus, Du16WMonitor_eCanController5);
   return;
}

/*****************************************************************************
vPUSER_iCANTransmit_Cfm
Called by the monitor CAN User protocol in case of transmission confirm event.
*****************************************************************************/
void vPUSER_iCANTransmit_Cfm(T5_WORD wObjectId, T5_WORD wStatus, T5_BYTE  bCANCtrlId)
{
   // Message transmitted: reset CAN errors
   wUserProErrorCode[bCANCtrlId] = Ds32WMonitor_eCanStatusBusOn; //(=0)
   return;
}
void vPUSER_iCANTransmit_Cfm1(T5_WORD wObjectId, T5_WORD wStatus)
{
   vPUSER_iCANTransmit_Cfm(wObjectId, wStatus, Du16WMonitor_eCanController1);
   return;
}
void vPUSER_iCANTransmit_Cfm2(T5_WORD wObjectId, T5_WORD wStatus)
{
   vPUSER_iCANTransmit_Cfm(wObjectId, wStatus, Du16WMonitor_eCanController2);
   return;
}
void vPUSER_iCANTransmit_Cfm3(T5_WORD wObjectId, T5_WORD wStatus)
{
   vPUSER_iCANTransmit_Cfm(wObjectId, wStatus, Du16WMonitor_eCanController3);
   return;
}
void vPUSER_iCANTransmit_Cfm4(T5_WORD wObjectId, T5_WORD wStatus)
{
   vPUSER_iCANTransmit_Cfm(wObjectId, wStatus, Du16WMonitor_eCanController4);
   return;
}
void vPUSER_iCANTransmit_Cfm5(T5_WORD wObjectId, T5_WORD wStatus)
{
   vPUSER_iCANTransmit_Cfm(wObjectId, wStatus, Du16WMonitor_eCanController5);
   return;
}

/*****************************************************************************
T5Can_PreOpen
Called by the STRATON driver before opening CAN ports
*****************************************************************************/

void T5Can_PreOpen (void)
{
   T5_BYTE bCANCtrlId;

   vT5_J1939_PreOpen();

   for(bCANCtrlId=0; bCANCtrlId<Du16WMonitor_eCanControllerNb; bCANCtrlId++)
   {
      wUserProMsgIndex[bCANCtrlId] = 0;
      bUserProAvailableInConf[bCANCtrlId] = 0;
      wUserProErrorCode[bCANCtrlId] = Ds32WMonitor_eCanStatusBusOn; //(=0)
   }
//
}

/*****************************************************************************
T5Can_GetCANPro
Get CAN protocol and CAN controller ID
Parameters:
    pPort (IN/OUT) pointer to the port object in driver memory
    szSettings (IN) configuration string entered in configuration
Return: OK or error
*****************************************************************************/
T5_RET T5Can_GetCANPro (T5PTR_DBCANPORT pPort,  T5_PTCHAR szSettings)
{

   if(T5CANPORT_J1939 == (*(T5PTR_CANPORTDEF)((*pPort).pDef)).wFlags)
   {
      (*pPort).oem.u8ProtocolId = eu8CAN_J1939ProtocolId;      
   }
   else if( (szSettings[0]=='U')
         && (szSettings[1]=='S')
         && (szSettings[2]=='E')
         && (szSettings[3]=='R')
         && (szSettings[4]=='_')
         && (szSettings[5]=='C')
         && (szSettings[6]=='A')
         && (szSettings[7]=='N'))
   {
      (*pPort).oem.u8ProtocolId = eu8CAN_UserProtocolId;
   }
   else
   {
      // no protocol defined
      (*pPort).oem.u8ProtocolId  = eu8CAN_InvalidProtocolId;
      return T5RET_ERROR;
   }

   return T5RET_OK;
}

/*****************************************************************************
T5Can_Open
Open a CAN port
Parameters:
    pPort (IN/OUT) pointer to the port object in driver memory
    szSettings (IN) configuration string entered in configuration
Return: OK or error
*****************************************************************************/

T5_RET T5Can_Open (T5PTR_DBCANPORT pPort, T5_PTCHAR szSettings)
{
   T5_RET DummyRet;

   T5PTR_DBCANMSG    pstConfMsg;
   T5_WORD  wTypeTx;
   T5_WORD  wTypeRx;
   T5_WORD  wMsgCount;
   T5_WORD  wNbMsg;
   T5_BYTE  bCANCtrlId;
   T5_BYTE  bCANProId;
   T5_BYTE  bProNbr;
   T5_DWORD dwHWStatus=0;

   // Check for valid setting
   if(T5RET_ERROR!=T5Can_GetCANPro(pPort, szSettings))
   {
      // Get protocol id from (*pPort).oem variable
      bCANProId = (T5_BYTE)(*pPort).oem.u8ProtocolId;

      // J1939 PROTOCOL
      //----------------
      if(eu8CAN_J1939ProtocolId==bCANProId)
      {
         DummyRet = retT5_J1939_Open (pPort, szSettings);
      }
      // USER PROTOCOL
      //----------------
      else if(eu8CAN_UserProtocolId==bCANProId)
      {
         // Define protocol channel
         switch(szSettings[8])
         {
            case '1':
               (*pPort).oem.u8ProtocolChannel = Du16WMonitor_eCanController1;
               break;
            case '2':
               (*pPort).oem.u8ProtocolChannel = Du16WMonitor_eCanController2;
               break;
            case '3':
               (*pPort).oem.u8ProtocolChannel = Du16WMonitor_eCanController3;
               break;
            case '4':
               (*pPort).oem.u8ProtocolChannel = Du16WMonitor_eCanController4;
               break;
            case '5':
               (*pPort).oem.u8ProtocolChannel = Du16WMonitor_eCanController5;
               break;
            default:
               (*pPort).oem.u8ProtocolChannel = Du16WMonitor_eInvalidCtrl;
               return T5RET_ERROR;
               break;
         }

         // Get CAN ctrl from (*pPort).oem variable
         bCANCtrlId= (T5_BYTE)(*pPort).oem.u8ProtocolChannel;

         // Verify if user protocol defined in network configuration
         for (bProNbr=0; bProNbr<Du16Com_eCanProtocolNbr;bProNbr++)
         {
            if(CucCANNetworkConfiguration[bCANCtrlId][bProNbr] == Du16WMonitor_eUserProtocol)
            {
               bUserProAvailableInConf[bCANCtrlId]=1;
            }
            else
            {
               // do nothing
            }
         }

         if(bUserProAvailableInConf[bCANCtrlId])
         {
            // Configure user protocol frames
            //-------------------------------

            if ((*pPort).wFlags & T5CAN_2B)
            {
               // Extended Format
               wTypeTx = Du16WMonitor_eCanMBModeExtTx;
               wTypeRx = Du16WMonitor_eCanMBModeExtRx;
            }
            else
            {
               // Standard Format
               wTypeTx = Du16WMonitor_eCanMBModeStdTx;
               wTypeRx = Du16WMonitor_eCanMBModeStdRx;
            }

            // Tx Msg
            pstConfMsg = T5VMCan_GetSndMsgList(pPort,&wNbMsg);

            for (wMsgCount=0; wMsgCount<wNbMsg; wMsgCount++)
            {
               DstWMonitor_eApplication_Function.piPUSER_eConfigMessage_Exe(bCANCtrlId,wUserProMsgIndex[bCANCtrlId], (*pstConfMsg).dwID, wTypeTx);

               // save message index in oem field
               (*pstConfMsg).oem.u16MsgIndex = (TU16)wUserProMsgIndex[bCANCtrlId];

               wUserProMsgIndex[bCANCtrlId]++;
               pstConfMsg++;
            }

            // Rx Msg
            pstConfMsg = T5VMCan_GetRcvMsgList(pPort,&wNbMsg);

            for (wMsgCount=0; wMsgCount<wNbMsg; wMsgCount++)
            {
               DstWMonitor_eApplication_Function.piPUSER_eConfigMessage_Exe(bCANCtrlId,wUserProMsgIndex[bCANCtrlId],(*pstConfMsg).dwID,wTypeRx);

               // save message index in oem field
               (*pstConfMsg).oem.u16MsgIndex = (TU16)wUserProMsgIndex[bCANCtrlId];

               wUserProMsgIndex[bCANCtrlId]++;
               pstConfMsg++;
            }

            // Configure user protocol events
            //-------------------------------
            DstWMonitor_eApplication_Function.piPUSER_eConfigEvents_Exe( bCANCtrlId,
                                         (TfPUSER_eEvt*)   (pvCANError_Idc   [bCANCtrlId]),
                                         (TfPUSER_eEvtObj*)(pvCANReceive_Idc [bCANCtrlId]),
                                         (TfPUSER_eEvtObj*)(pvCANTransmit_Cfm[bCANCtrlId]));
         }
         else
         {
            // user protocol not available in network configuration
            bCANProId  = eu8CAN_NoProtocolId;
            bCANCtrlId = Du16WMonitor_eInvalidCtrl;
            dwHWStatus |= T5CAN_HwStatus_ProtocolNotAvailable;
         }
      }
      // OTHER PROTOCOL ?
      //----------------
      else
      {
         // other than user protocol: not supported
         bCANProId  = eu8CAN_NoProtocolId;
         bCANCtrlId = Du16WMonitor_eInvalidCtrl;
         dwHWStatus |= T5CAN_HwStatus_ProtocolNotAvailable;
      }
   }
   else
   {
      // not valid port settings
      bCANProId  = eu8CAN_InvalidProtocolId;
      bCANCtrlId = Du16WMonitor_eInvalidCtrl;
      dwHWStatus |= T5CAN_HwStatus_InvalidPortSettings;
   }

   // Store HW Status in each port messages (if !=0)
   if(dwHWStatus)
   {
      // Tx
      pstConfMsg = T5VMCan_GetSndMsgList(pPort,&wNbMsg);
      for (wMsgCount=0; wMsgCount<wNbMsg; wMsgCount++)
      {
         (*pstConfMsg).dwSpcIn |= dwHWStatus;
         pstConfMsg++;
      }
      // Rx
      pstConfMsg = T5VMCan_GetRcvMsgList(pPort,&wNbMsg);
      for (wMsgCount=0; wMsgCount<wNbMsg; wMsgCount++)
      {
         (*pstConfMsg).dwSpcIn |= dwHWStatus;
         pstConfMsg++;
      }
   }
   else
   {
      // HW status = 0 -> no update
   }

   return T5RET_OK;
}

/*****************************************************************************
T5Can_PostClose
Called by the STRATON driver after closing CAN ports
*****************************************************************************/

void T5Can_PostClose (void)
{
   vT5_J1939_PostClose();
}

/*****************************************************************************
T5Can_Close
Close a CAN port
Parameters:
    pPort (IN/OUT) pointer to the port object in driver memory
*****************************************************************************/

void T5Can_Close (T5PTR_DBCANPORT pPort)
{
   T5_BYTE  bCANProId;

   // Find Controller ID and protocol from (*pPort).oem variable
   bCANProId  = (T5_BYTE) (*pPort).oem.u8ProtocolId;

   // J1939 PROTOCOL
   //----------------
   if(eu8CAN_J1939ProtocolId==bCANProId)
   {
      vT5_J1939_Close(pPort);
   }
   // USER PROTOCOL
   //----------------
   else if(eu8CAN_UserProtocolId==bCANProId)
   {
      // TBD
   }
   // OTHER PROTOCOL ?
   //----------------
   else
   {
      // other than user protocol: not supported yet
   }
}

/*****************************************************************************
T5Can_OnPreExchange
Called by the STRAATON driver before exchanging driver data
You can use T5VMCan_FindRcvMsg to find a received message in driver memory
Call T5VMCan_MarkRcvMsg to indicate that a message was received
Parameters:
    pPort (IN/OUT) pointer to the port object in driver memory
*****************************************************************************/

void T5Can_OnPreExchange (T5PTR_DBCANPORT pPort)
{
   T5PTR_CANPORTDEF  pstPortDef;
   T5PTR_DBCANMSG    pstConfMsg;

   T5_WORD  wMsgCount;
   T5_WORD  wNbMsg;
   T5_BYTE  bCANCtrlId;
   T5_BYTE  bCANProId;
   T5_SHORT sMessageStatus;
   T5_WORD  wFrameIndex;

   // Find Controller ID and protocol from (*pPort).oem variable
   bCANProId  = (T5_BYTE) (*pPort).oem.u8ProtocolId;
   bCANCtrlId = (T5_BYTE) (*pPort).oem.u8ProtocolChannel;

   // J1939 PROTOCOL
   //----------------
   if(eu8CAN_J1939ProtocolId==bCANProId)
   {
      vT5_J1939_OnPreExchange(pPort);
   }
   // USER PROTOCOL
   //----------------
   else if(eu8CAN_UserProtocolId==bCANProId)
   {
      // Find Rx Msg
      pstConfMsg = T5VMCan_GetRcvMsgList(pPort,&wNbMsg);

      for (wMsgCount=0; wMsgCount<wNbMsg; wMsgCount++)
      {
         // message index saved in oem field
         wFrameIndex = (*pstConfMsg).oem.u16MsgIndex;

         sMessageStatus = DstWMonitor_eApplication_Function.piPUSER_eGetState_Exe(bCANCtrlId, wFrameIndex);

         if((Ds32WMonitor_eMBStatusRxNewData     == sMessageStatus)
          ||(Ds32WMonitor_eMBStatusRxOverWrData  == sMessageStatus))
         {
            //Get DATA
            (*pstConfMsg).bLen = (T5_BYTE)DstWMonitor_eApplication_Function.piPUSER_eGetData_Exe(bCANCtrlId, wFrameIndex, (*pstConfMsg).data);
            T5VMCan_MarkRcvMsg (pPort,pstConfMsg);
         }
         else
         {
            // message not received
         }

         pstConfMsg++;
      }
   }
   // OTHER PROTOCOL ?
   //----------------
   else
   {
      // other than user protocol: not supported yet
   }
}

/*****************************************************************************
T5Can_OnPostExchange
Called by the STRATON driver after exchanging driver data
You can use T5VMCan_PeekNextSndMsg/T5VMCan_PopNextSndMsg to get the list
of messages to be sent
Parameters:
    pPort (IN/OUT) pointer to the port object in driver memory
*****************************************************************************/

void T5Can_OnPostExchange (T5PTR_DBCANPORT pPort)
{
   T5PTR_CANPORTDEF  pstPortDef;
   T5PTR_DBCANMSG    pstConfMsg;

   T5_BYTE  bCANCtrlId;
   T5_BYTE  bCANProId;
   T5_WORD  wFrameIndex;


   // Find Controller ID and protocol from (*pPort).oem variable
   bCANProId  = (T5_BYTE) (*pPort).oem.u8ProtocolId;
   bCANCtrlId = (T5_BYTE) (*pPort).oem.u8ProtocolChannel;

   // J1939 PROTOCOL
   //----------------
   if(eu8CAN_J1939ProtocolId==bCANProId)
   {
      vT5_J1939_OnPostExchange(pPort);
   }
   // USER PROTOCOL
   //----------------
   else if(eu8CAN_UserProtocolId==bCANProId)
   {
      pstConfMsg = T5VMCan_PopNextSndMsg(pPort);
      while (pstConfMsg != NULL)
      {
         // message index saved in oem field?
         wFrameIndex = (*pstConfMsg).oem.u16MsgIndex;

         if((*pstConfMsg).dwSpcOut & T5CAN_HwControl_StopMsgTx)
         {
            // transmission stopped
         }
         else
         {
            //Send Frame
            DstWMonitor_eApplication_Function.piPUSER_eSendFrame_Exe(bCANCtrlId, wFrameIndex, (*pstConfMsg).bLen, (*pstConfMsg).data);
         }

         pstConfMsg = T5VMCan_PopNextSndMsg(pPort);
      }
   }
   // OTHER PROTOCOL ?
   //----------------
   else
   {
      // other than user protocol: not supported yet
   }
//   
}

/*****************************************************************************
T5Can_GetBusOffFlag
Get the CAN "Bus Off" error flag
Parameters:
    pPort (IN/OUT) pointer to the port object in driver memory
Return: BUS OFF flag
*****************************************************************************/

T5_BOOL T5Can_GetBusOffFlag (T5PTR_DBCANPORT pPort)
{
   T5_BYTE  bCANCtrlId;
   T5_BYTE  bCANProId;

   // Find Controller ID and protocol from (*pPort).oem variable
   bCANProId  = (T5_BYTE) (*pPort).oem.u8ProtocolId;
   bCANCtrlId = (T5_BYTE) (*pPort).oem.u8ProtocolChannel;

   if((Du16WMonitor_eInvalidCtrl != bCANCtrlId)
   && (eu8CAN_UserProtocolId == bCANProId))
   {
      if(Ds32WMonitor_eCanStatusBusOff==wUserProErrorCode[bCANCtrlId])
      {
         return TRUE;
      }
      else
      {
         return FALSE;
      }
   }
   else
   {
      return FALSE;
   }
}

/*****************************************************************************
T5Can_GetErrors
Get the CAN errors (hardware specific)
Parameters:
    pPort (IN/OUT) pointer to the port object in driver memory
Return: CAN errors value
*****************************************************************************/

T5_DWORD T5Can_GetErrors (T5PTR_DBCANPORT pPort)
{
   T5_BYTE  bCANCtrlId;
   T5_BYTE  bCANProId;

   // Find Controller ID and protocol from (*pPort).oem variable
   bCANProId  = (T5_BYTE) (*pPort).oem.u8ProtocolId;
   bCANCtrlId = (T5_BYTE) (*pPort).oem.u8ProtocolChannel;

   if((Du16WMonitor_eInvalidCtrl != bCANCtrlId)
   && (eu8CAN_UserProtocolId == bCANProId))
   {
      // CAN Bus status wErrorCode:
      //-----------------------------------
      // 0: Ds32WMonitor_eCanStatusBusOn
      // 1: Ds32WMonitor_eCanStatusStuffError
      // 2: Ds32WMonitor_eCanStatusFrameFormError
      // 3: Ds32WMonitor_eCanStatusFrameAckError
      // 4: Ds32WMonitor_eCanStatusFrameBit1Error
      // 5: Ds32WMonitor_eCanStatusFrameBit0Error
      // 6: Ds32WMonitor_eCanStatusFrameCrcError
      // 7: Ds32WMonitor_eCanStatusBusWarning
      // 8: Ds32WMonitor_eCanStatusBusError
      // 9: Ds32WMonitor_eCanStatusBusOff

      return((T5_DWORD)wUserProErrorCode[bCANCtrlId]);
   }
   else
   {
      return 0L;
   }
}

/*****************************************************************************
T5CAN_OnSpcOut
This function is called when a new "hardware specific control" value is passed
to a configured CAN message
Parameters:
    pPort (IN/OUT) pointer to the port object in driver memory
    pMsg (IN/OUT) pointer to the message object in driver memory
*****************************************************************************/

void T5CAN_OnSpcOut (T5PTR_DBCANPORT pPort, T5PTR_DBCANMSG pMsg)
{
   T5_BYTE  bCANProId;

   // Find Controller ID and protocol from (*pPort).oem variable
   bCANProId  = (T5_BYTE) (*pPort).oem.u8ProtocolId;

   // J1939 PROTOCOL
   //----------------
   if(eu8CAN_J1939ProtocolId==bCANProId)
   {
      vT5_J1939_OnSpcOut(pPort,pMsg);
   }
   // USER PROTOCOL
   //----------------
   else if(eu8CAN_UserProtocolId==bCANProId)
   {
      // TBD
   }
   // OTHER PROTOCOL ?
   //----------------
   else
   {
      // other than user protocol: not supported yet
   }
}

#ifdef T5CAN_OEMALLOC
/*****************************************************************************
T5CAN_OemAlloc
This function is called ... TBD
Parameters:
*****************************************************************************/
T5_PTBYTE T5CAN_OemAlloc (T5PTR_DBCANPORT pPort, T5_DWORD dwID, T5_BYTE bLen)
{
   T5_PTBYTE ptbyteRetVal;

   // T5CAN_OemAlloc CALLED BEFORE T5CAN_OPEN!
   //-> therefore we have not yet identified the protocol in oem variable!
   // use port flag instead:
   
   // J1939 PROTOCOL
   //----------------
   if(T5CANPORT_J1939 == (*(T5PTR_CANPORTDEF)((*pPort).pDef)).wFlags)
   {
      ptbyteRetVal = ptbyteT5_J1939_OemAlloc(pPort, dwID, bLen);
   }
   // USER PROTOCOL
   //----------------
   else
   {
      // TO BE DEFINED!
      // use J1939 memory area for now, until we decide if we use another area
      // or if we put all CAN protocol in same area
      ptbyteRetVal = ptbyteT5_J1939_OemAlloc(pPort, dwID, bLen);
   }
   
   return(ptbyteRetVal);
}

/*****************************************************************************
T5CAN_OemFree
This function is called ... TBD
Parameters:
*****************************************************************************/
void T5CAN_OemFree (T5PTR_DBCANPORT pPort, T5_PTBYTE pBuffer)
{
   
}
#endif /*T5CAN_OEMALLOC*/

/****************************************************************************/

#endif /*T5DEF_CANBUS*/

/* eof **********************************************************************/
