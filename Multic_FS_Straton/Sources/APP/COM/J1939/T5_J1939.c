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
//        T5_J1939 : J1939 protocol management for Straton application
////////////////////////////////////////////////////////////////////////////////
// FILE : T5_J1939.c (SOURCE)
//----------------------------------------------------------------------------
// DESC : J1939 protocol management for Straton application 
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 772 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Sources/APP/CORE/VM/custom/T5ACTIA.c $
// Updated the :$Date: 2015-07-22 14:16:15 +0200 (mer., 22 juil. 2015) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
/* T5_J1939.c */

#define T5_J1939
////////////////////////////////////////////////////////////////////////////////
// INCLIB
////////////////////////////////////////////////////////////////////////////////
#include "t5vm.h"
#include "std_type.h"

#include "Monitor.h"
#include "Lib_J1939.h"

////////////////////////////////////////////////////////////////////////////////
// INCUSER
////////////////////////////////////////////////////////////////////////////////
#include "T5_J1939.h"

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

T5_DWORD dwEndAddressClaim, dwCurrent;

//**************************************************
// FOR TEST APP
//**************************************************
TU8 u8AMain_iECUHandle;

//*FOR TEST APP end*********************************

TU8 u8J1939_iNbOfInstance;
TU16 u16J1939MsgIndex[Du8J1939_iMaxNbOfInstance];
T5PTR_DBCANPORT pPortUsedOnInstance[Du8J1939_iMaxNbOfInstance];
Tpu8J1939_iDataPointer pu8J1939_iPointerInDataPool = 0;
Teu16AMain_iJ1939State eu16J1939_iState[Du8J1939_iMaxNbOfInstance];
TstJ1939_Request pstJ1939_iRequest[Du8J1939_iMaxNbOfInstance];
TU8 u8NbOfRequestInPreExchange[Du8J1939_iMaxNbOfInstance];

////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////
void vT5_J1939_eErrCallBack_Exe(TU8 Fu8InstNum, TstWJ1939_eErrorData* FpstErrorDesc);
void vT5_J1939_eRxCallBack_Exe(TU8 Fu8InstNum, TstWJ1939_eTPLMsgData* FpstMsg);
void vT5_J1939_eNwmCallBack_Exe(TU8 Fu8InstNum, TU8 Fu8SA, TU8 * Fpu8Data);
void vT5_J1939_eReqCallBack_Exe(TU8 Fu8InstNum, TstWJ1939_eTPLMsgData* FpstMsg);

////////////////////////////////////////////////////////////////////////////////
// Functions (Intern)
////////////////////////////////////////////////////////////////////////////////

//============================================================================//
// NAME : s16T5_J1939_SetAllMessageStatus_exe
//============================================================================//
// ROLE : Set message status in a J1939 instance
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
//    uHWStatus: HW Status value
// OUTPUT : OK or error
//============================================================================//
TS16 s16T5_J1939_SetAllMessageStatus_exe (T5PTR_DBCANPORT pPort, TuJ1939_HwStatus* puHWStatus)
{  
   T5PTR_DBCANMSG pMsg;
   T5_WORD wMsgCount;
   T5_WORD wNbMsg;
   TuJ1939_HwStatus* puJ1939_HwStatus;

   if((0 == pPort)
    ||(0 ==puHWStatus))
   {
      return(Ds16J1939_eErrorBadParameter);
   }
   // Tx msg
   pMsg = T5VMCan_GetSndMsgList(pPort,&wNbMsg);
   for (wMsgCount=0; wMsgCount<wNbMsg; wMsgCount++)
   {
      puJ1939_HwStatus = (TuJ1939_HwStatus*)(&(pMsg->dwSpcIn));
      puJ1939_HwStatus->u32SpcIn = puHWStatus->u32SpcIn;
      pMsg++;         
   }

   // Rx msg
   pMsg = T5VMCan_GetRcvMsgList(pPort,&wNbMsg);
   for (wMsgCount=0; wMsgCount<wNbMsg; wMsgCount++)
   {
      puJ1939_HwStatus = (TuJ1939_HwStatus*)(&(pMsg->dwSpcIn));
      puJ1939_HwStatus->u32SpcIn = puHWStatus->u32SpcIn;
      pMsg++;         
   }

   return(Ds16J1939_eSuccess);
}

//============================================================================//
// NAME : s32T5_J1939_InitInstance_exe
//============================================================================//
// ROLE : Initialize a J1939 instance
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
//    szSettings (IN) configuration string entered in configuration
// OUTPUT : OK or error
//============================================================================//
TS16 s16T5_J1939_InitInstance_exe (T5PTR_DBCANPORT pPort, T5_PTCHAR szSettings)
{  
	TS16 s16RetVal;
   TU8 u8J1939Channel;
   TU8 u8InstanceIndex;
   T5PTR_CANPORTDEF pPortDef;
   TuJ1939_HwStatus uJ1939_HwStatus;
   
   u8J1939Channel = pPort->oem.u8ProtocolChannel;

   // Get port definition
   pPortDef = (T5PTR_CANPORTDEF)pPort->pDef;

   s16RetVal = (TS16) DstWMonitor_eApplication_Function.s32WJ1939_eInitInstance_Exe  (  u8J1939Channel,
                                                                                 pPortDef->bECU,
                                                                                 vT5_J1939_eErrCallBack_Exe,
                                                                                 vT5_J1939_eRxCallBack_Exe,
                                                                                 vT5_J1939_eNwmCallBack_Exe
                                                                              );

   if(s16RetVal>=0)
   {
      // Update instance value
      u8InstanceIndex = s16RetVal;
      u8J1939_iNbOfInstance = u8InstanceIndex+1;
      pPort->oem.u8ProtocolInstance = u8InstanceIndex;
      pPortUsedOnInstance[u8InstanceIndex]= pPort;
      s16RetVal = Ds16J1939_eSuccess;
   }
   else
   {
      switch(s16RetVal)
      {
         case Ds16J1939_eErrorNbInstPerChannel:
         case Ds16J1939_eErrorNbInstances:
            uJ1939_HwStatus.u1InvalidPortSettings = 1;
            break;
         case Ds16J1939_eErrorChannelNotWired:
            uJ1939_HwStatus.u1ProtocolNotWired = 1;
            break;
         case Ds16J1939_eError:
         case Ds16J1939_eErrorBadParameter:
         case Ds16J1939_eErrorOutOfMemory:
         default:   
            break;
      }
      uJ1939_HwStatus.u3PgnRegistrationStatus = eu3J1939_InstanceInitErr;
      s16RetVal = s16T5_J1939_SetAllMessageStatus_exe(pPort, (TuJ1939_HwStatus*)(&uJ1939_HwStatus.u32SpcIn));
      
      s16RetVal = Ds16J1939_eError;
   }
   
   return(s16RetVal);
}

//============================================================================//
// NAME : vT5_J1939_DecodeIdent
//============================================================================//
// ROLE : Decode fields in J1939 identifiers
//----------------------------------------------------------------------------//
// INPUT  :
//    TU32 u32Ident : 29 bit J1939 identifier
//    TstJ1939Identifier* pstIdent : identifier structure pointer
// OUTPUT :
//============================================================================//
void vT5_J1939_DecodeIdent (TU32 u32Ident, TstJ1939Identifier* pstIdent)
{
   (*pstIdent).u8SourceAddress   = (TU8) (u32Ident & 0x000000FF);
   (*pstIdent).u8PduSpecific     = (TU8)((u32Ident & 0x0000FF00)>>8);
   (*pstIdent).u8PduFormat       = (TU8)((u32Ident & 0x00FF0000)>>16);
   (*pstIdent).u8DataPage        = (TU8)((u32Ident & 0x01000000)>>24);
   (*pstIdent).u8Priority        = (TU8)((u32Ident & 0x1C000000)>>26);
   
   if((*pstIdent).u8PduFormat >= Du8J1939_iGlobalPDU)
   {
      (*pstIdent).u16PGN = (TU16)((u32Ident & 0x00FFFF00)>>8);     
      (*pstIdent).u8DestinationAddress = Du8WJ1939_eGlobalAddress; // broadcast
   }
   else
   {
      (*pstIdent).u16PGN = (TU16)((u32Ident & 0x00FF0000)>>8);     
      (*pstIdent).u8DestinationAddress = (*pstIdent).u8PduSpecific; // destination address
   }
}

//============================================================================//
// NAME : pT5_J1939_FindMsg
//============================================================================//
// ROLE : Find a J1939 message from Straton message list
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN) pointer to the port object in driver memory
//    u32SearchPGN (IN) PGN of message searched
//    u8MsgType: eu8J1939_TypeTx or eu8J1939_TypeRx
//    u8Addr: For Rx msg: =SA, for Tx msg: =DA
// OUTPUT :
//    pointer to found message - NULL if not found
//============================================================================//
T5PTR_DBCANMSG pT5_J1939_FindMsg (T5PTR_DBCANPORT pPort, TU32 u32SearchPGN, TU8 u8Addr, TU8 u8MsgType)
{
   TstJ1939Identifier stMsgIdent;
   T5PTR_DBCANMSG pMsg;
   T5PTR_CANMSGDEF pMsgDef;
   T5_WORD wMsgCount;
   T5_WORD wNbMsg;
   TU32 u32MsgPGN;

   switch(u8MsgType)
   {
      case eu8J1939_TypeTx:
         pMsg = T5VMCan_GetSndMsgList(pPort,&wNbMsg);
         break;
      case eu8J1939_TypeRx:
         pMsg = T5VMCan_GetRcvMsgList(pPort,&wNbMsg);
         break;
      default:
         return NULL;
   }
   for (wMsgCount=0; wMsgCount<wNbMsg; wMsgCount++)
   {
      // Decode identifier
      vT5_J1939_DecodeIdent(pMsg->dwID, &stMsgIdent);
      u32MsgPGN = ((TU32)(stMsgIdent.u8DataPage)<<16) + stMsgIdent.u16PGN;
      
      // Find message with corresponding PGN
      if(u32MsgPGN == u32SearchPGN)
      {
         if(eu8J1939_TypeRx==u8MsgType)
         {           
            // Get message definition
            pMsgDef = (T5PTR_CANMSGDEF)pMsg->pDef;
            // Check if SA correct or ignored or masked
            if((stMsgIdent.u8SourceAddress == u8Addr)
            || (Du8WJ1939_eGlobalAddress == u8Addr)
            || (0 != (pMsgDef->bMode & T5CANMSG_SAMASK)))
            {
               return(pMsg);
            }
            else
            {
               // carry on
            }
         }
         else //if(eu8J1939_TypeTx==u8MsgType)
         {
            // check the DA correct or ignored
            if((stMsgIdent.u8DestinationAddress == u8Addr)
            || (Du8WJ1939_eGlobalAddress == u8Addr))
            {
               return(pMsg);                              
            }
            else
            {
               // carry on
            }
         }
      }
      else
      {
         // carry on
      }
      // update message pointer
      pMsg++;
   }
   return NULL;
}

//============================================================================//
// NAME : ptbyteT5_J1939_OemAlloc
//============================================================================//
// ROLE : Allocate a J1939 Message buffer in the J1939 data pool
//        and return to the Straton VM the data pointer of a message
//----------------------------------------------------------------------------//
// INPUT  :
// T5PTR_DBCANPORT pPort : CAN port
// T5_DWORD dwID : message identifier
// T5_BYTE bLen : message data length
// OUTPUT : Data pointer
//============================================================================//
T5_PTBYTE ptbyteT5_J1939_OemAlloc (T5PTR_DBCANPORT pPort, T5_DWORD dwID, T5_BYTE bLen)
{
   T5_PTBYTE ptbytePointer;
   // at first call, initialise to start address:
   if(0==pu8J1939_iPointerInDataPool)
   {
      pu8J1939_iPointerInDataPool = (TU8*)Du32J1939_eDataPoolStartAddr;      
   }
   
   if(((TU32)pu8J1939_iPointerInDataPool+bLen)>(Du32J1939_eDataPoolEndAddr+1))
   {
      // out of memory
      ptbytePointer = 0;
   }
   else
   {
      ptbytePointer = pu8J1939_iPointerInDataPool;
      pu8J1939_iPointerInDataPool += bLen;
   }
   
   return(ptbytePointer);
}

//============================================================================//
// NAME : s16T5_J1939_Register_Messages_exe
//============================================================================//
// ROLE : Register messages in a J1939 instance
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
//    u8MsgType: eu8J1939_TypeTx or eu8J1939_TypeRx
// OUTPUT : OK or error
//============================================================================//
TS16 s16T5_J1939_Register_Messages_exe (T5PTR_DBCANPORT pPort, TU8 u8MsgType)
{  
	TS16 s16RetVal;
   TstJ1939Identifier stMsgIdent;
   T5PTR_DBCANMSG pMsg, pMsgSamePgn;
   T5_WORD wMsgCount;
   T5_WORD wNbMsg;
   TU16 u16MsgPeriod;
   T5PTR_CANMSGDEF pMsgDef;
   TuJ1939_HwStatus* puJ1939_HwStatus;
   TuJ1939_HwStatus* puJ1939_HwStatusSamePgn;
   TuJ1939_HwCommand* puJ1939_HwCommand;
   TU8 u8Active;

   switch(u8MsgType)
   {
      case eu8J1939_TypeTx:
         // Get Tx message list
         pMsg = T5VMCan_GetSndMsgList(pPort,&wNbMsg);
         break;

      case eu8J1939_TypeRx:
         // Get Rx message list
         pMsg = T5VMCan_GetRcvMsgList(pPort,&wNbMsg);
         break;

         default:
         return(Ds16J1939_eErrorBadParameter);
         break;
   }

   for (wMsgCount=0; wMsgCount<wNbMsg; wMsgCount++)
   {
      // save message index in oem field
      pMsg->oem.u16MsgIndex = u16J1939MsgIndex[pPort->oem.u8ProtocolInstance];
      
      // get specific hardware status & command variable
      puJ1939_HwStatus = (TuJ1939_HwStatus*)(&(pMsg->dwSpcIn));
      puJ1939_HwCommand = (TuJ1939_HwCommand*)(&(pMsg->dwSpcOut));
      
      // Initialised the registration status to "Not Registered"
      puJ1939_HwStatus->u3PgnRegistrationStatus = eu3J1939_NotRegistred;

      // Check if the message buffer has been allocated in data pool
      if(0==pMsg->data)
      {
         puJ1939_HwStatus->u1PgnDataOutOfMemory = 1;
      }
      else
      {
         // Decode identifier
         vT5_J1939_DecodeIdent(pMsg->dwID, &stMsgIdent);

         // Get message definition
         pMsgDef = (T5PTR_CANMSGDEF)pMsg->pDef;
                  
         u16MsgPeriod = pMsgDef->dwTMin;

         if(eu8J1939_TypeTx==u8MsgType)
         {
            // Cyclic Tx Messages (period > 0)
            if((u16MsgPeriod > 0)
            && (T5CANMSG_PERIO == T5CANMSG_GETMODE (pMsgDef->bMode)))
            {
               if(puJ1939_HwCommand->u1InhibitTransmission)
               {
                  u8Active = 0;
               }
               else
               {
                  u8Active = 1;
               }
               s16RetVal = DstWMonitor_eApplication_Function.s16WJ1939_eRegisterCycTxPGN_Exe(
                                                               pPort->oem.u8ProtocolInstance,   //TU8 Fu8InstNum,
                                                               pMsg->data,                      //TU8* Fpu8Data,
                                                               pMsgDef->bLen,                   //TU16 Fu16Length,
                                                               u16MsgPeriod,                    //TU16 Fu16CycleTime,
                                                               stMsgIdent.u8DestinationAddress, //TU16 Fu16Remote,
                                                               stMsgIdent.u16PGN,               //TU16 Fu16PGN,
                                                               stMsgIdent.u8DataPage,           //TU8 Fu8DataPage,
                                                               stMsgIdent.u8Priority,           //TU8 Fu8Prio,
                                                               u8Active                         //TU8 Fu8Active
                                                            );
               puJ1939_HwStatus->u3PgnRegistrationStatus = eu3J1939_RegistredCycTxPGN;
            }
            else
            {
               // All one shot messages are declared as messages sent on request
               // (can also be sent by the application: manage in post exchange function)
               s16RetVal = DstWMonitor_eApplication_Function.s16WJ1939_eRegisterReqPGN_Exe(
                                                               pPort->oem.u8ProtocolInstance,   //TU8 Fu8InstNum,
                                                               stMsgIdent.u8Priority,           //TU8 Fu8Prio,
                                                               pMsgDef->bLen,                   //TU16 Fu16Length,
                                                               0, //pMsg->data,                 //TU8* Fpu8Data,
                                                               stMsgIdent.u16PGN,               //TU16 Fu16PGN,
                                                               stMsgIdent.u8DataPage,           //TU8 Fu8DataPage
                                                               vT5_J1939_eReqCallBack_Exe     //TpfnWJ1939_eMsgClbk FpfnReqCallback
                                                            );
               puJ1939_HwStatus->u3PgnRegistrationStatus = eu3J1939_RegistredReqPGN;
            }
         }
         else if(eu8J1939_TypeRx==u8MsgType)
         {
            // Register as Cyclic Rx Messages with:
            //--------------------------------------------
            //       period > 0   (cyclic)
            // AND   nb data <= 8 (to read nb of byte rx, need to use App PGN)
            // AND   masked SA    (to filter SA, need to use App PGN)
            // --> but to read SA of Rx msg when this one has a masked SA, need to use App PGN!
            //--------------------------------------------
            // --> therefore, need to always use App PGN!
            //--------------------------------------------
            /*
            if((u16MsgPeriod > 0)
            && (pMsg->bLen <= 8)
            && (0 != (pMsgDef->bMode & T5CANMSG_SAMASK)))
            {
               s16RetVal = DstWMonitor_eApplication_Function.s16WJ1939_eRegisterCycRxPGN_Exe(
                                                               pPort->oem.u8ProtocolInstance,   //TU8 Fu8InstNum
                                                               pMsg->data,                      //TU8* Fpu8Data
                                                               u16MsgPeriod,                    //TU16 Fu16CycleTime
                                                               stMsgIdent.u16PGN,               //TU16 Fu16PGN
                                                               stMsgIdent.u8DataPage            //TU8 Fu8DataPage
                                                            );
               puJ1939_HwStatus->u3PgnRegistrationStatus = eu3J1939_RegistredCycRxPGN;
            }
            else
            */
            {
               s16RetVal = DstWMonitor_eApplication_Function.s16WJ1939_eRegisterAppPGN_Exe(
                                                               pPort->oem.u8ProtocolInstance,   //TU8 Fu8InstNum
                                                               stMsgIdent.u16PGN,               //TU16 Fu16PGN
                                                               stMsgIdent.u8DataPage            //TU8 Fu8DataPage
                                                            );                  
               // Init rx time stamp
               pMsg->oem.u16RxTimeStamp = (TU16)T5_GetTickCount();

               if(Ds16WJ1939_eSuccess==s16RetVal)
               {
                  puJ1939_HwStatus->u3PgnRegistrationStatus = eu3J1939_RegistredAppPGN;                  
               }
               else
               {
                  // check if the Pgn has already been registered
                  pMsgSamePgn = pT5_J1939_FindMsg (pPort, stMsgIdent.u16PGN, Du8WJ1939_eGlobalAddress, eu8J1939_TypeRx);
                  
                  if(pMsgSamePgn != pMsg)
                  {
                     // it is not the first message with this Pgn:
                     // set registration status and s16RetVal according to the registration status of the first message with same Pgn
                     puJ1939_HwStatusSamePgn = (TuJ1939_HwStatus*)(&(pMsgSamePgn->dwSpcIn));
                     puJ1939_HwStatus->u3PgnRegistrationStatus = puJ1939_HwStatusSamePgn->u3PgnRegistrationStatus;
                     if((eu3J1939_RegistrationErr!=puJ1939_HwStatus->u3PgnRegistrationStatus)
                     && (eu3J1939_NotRegistred!=puJ1939_HwStatus->u3PgnRegistrationStatus))
                     {
                        s16RetVal = Ds16WJ1939_eSuccess;                     
                     }
                     else
                     {
                        s16RetVal = Ds16WJ1939_eError;
                     }
                  }
                  else
                  {
                     // it is the first message with this Pgn:
                     // --> keep s16RetVal error
                  }                  
               }
            }
         }
         else
         {
            return(Ds16J1939_eErrorBadParameter);
         }

         if(Ds16WJ1939_eSuccess!=s16RetVal)
         {
            puJ1939_HwStatus->u3PgnRegistrationStatus = eu3J1939_RegistrationErr;
         }
         else
         {
            // empty else
         }
      }
      
      // update message index and pointer
      u16J1939MsgIndex[pPort->oem.u8ProtocolInstance]++;
      pMsg++;         
   }
   return(Ds16J1939_eSuccess);
}

//============================================================================//
// NAME : s16T5_J1939_ActivateAllCyclicTxPGNs_exe
//============================================================================//
// ROLE : Activate All Cyclic Tx PGNs in a J1939 instance
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
//    Fu8Activate: 1=Activate or 0=dis-activate
// OUTPUT : OK or error
//============================================================================//
TS16 s16T5_J1939_ActivateAllCyclicTxPGNs_exe (T5PTR_DBCANPORT pPort, TU8 Fu8Activate)
{  
   TS16 s16RetVal, s16Return;
   TstJ1939Identifier stMsgIdent;
   T5PTR_DBCANMSG pMsg;
   T5_WORD wMsgCount;
   T5_WORD wNbMsg;
   TU16 u16MsgPeriod;
   TU8 u8PortInstance;
   T5PTR_CANMSGDEF pMsgDef;

   u8PortInstance = pPort->oem.u8ProtocolInstance;

   s16RetVal = Ds16J1939_eSuccess;
   s16Return = Ds16J1939_eSuccess;
   pMsg = T5VMCan_GetSndMsgList(pPort,&wNbMsg);

   for (wMsgCount=0; wMsgCount<wNbMsg; wMsgCount++)
   {
      // Check if the message buffer has been allocated in data pool
      if(0==pMsg->data)
      {
         s16RetVal = Ds16J1939_eErrorOutOfMemory;                                  
      }
      else
      {
         // Get message definition
         pMsgDef = (T5PTR_CANMSGDEF)pMsg->pDef;

         // Decode identifier
         vT5_J1939_DecodeIdent(pMsg->dwID, &stMsgIdent);

         u16MsgPeriod = pMsgDef->dwTMin;

         // Cyclic Tx Messages (period > 0)
         if(u16MsgPeriod > 0)
         {
            s16RetVal = DstWMonitor_eApplication_Function.s16WJ1939_eActivateCycPGNs_Exe(
                                                            u8PortInstance,         //TU8 Fu8InstNum,
                                                            Fu8Activate,            //TU8 Fu8Activate,
                                                            stMsgIdent.u16PGN,      //TU16 Fu16PGN,
                                                            stMsgIdent.u8DataPage); //TU8 Fu8DataPage
         }
         else
         {
            // one shot message manage in post exchange function
         }
      }
      
      if(Ds16J1939_eSuccess!=s16RetVal)
      {
         s16Return = s16RetVal;
      }
      else
      {
         // nothing to do
      }
      // update message pointer
      pMsg++;         
   }
   return(s16Return);
}

//============================================================================//
// NAME : vT5_J1939_AddrClaim_exe
//============================================================================//
// ROLE : J1939 Straton address claim state machine:
//          Must be called in the application cycle
//          (ex: in vT5_J1939_OnPreExchange or vT5_J1939_OnPostExchange)
//          carry out the address claim during the first cycles at start up
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
// OUTPUT :
//============================================================================//
void vT5_J1939_AddrClaim_exe (T5PTR_DBCANPORT pPort)
{
	TS16 s16RetVal;
   TU8 u8PortInstance, u8InstanceIndex;
   TU8 u8AddressClaimStatus;
   TU16 u16State;

   u8PortInstance = pPort->oem.u8ProtocolInstance;
   
   switch(eu16J1939_iState[u8PortInstance])
   {
      case Ceu16AMain_iJ1939WaitAdressClaim:
         //-------------------------------
         // Get Address Claim Status
         //-------------------------------
         u8AddressClaimStatus = DstWMonitor_eApplication_Function.u8WJ1939_eGetState_Exe(u8PortInstance);
         
         //-------------------------------
         // Set transition
         //-------------------------------
         if(Du16WJ1939_eACSuccess==u8AddressClaimStatus)
         {
            eu16J1939_iState[u8PortInstance]=Ceu16AMain_iJ1939AdressClaimSucceeded;
            // no break: go directly below to Ceu16AMain_iJ1939AdressClaimSucceeded
         }
         else if(Du16WJ1939_eACFailed==u8AddressClaimStatus)
         {
            eu16J1939_iState[u8PortInstance]=Ceu16AMain_iJ1939Fail;
            break;
         }
         else //if(Du16WJ1939_eACBusy==u8AddressClaimStatus)
         {
            // stay in Ceu16AMain_iJ1939WaitAdressClaim
            break;
         }

      case Ceu16AMain_iJ1939AdressClaimSucceeded:
         //--------------------------------------------------------------
         // Wait for all instances to pass the Wait Address Claim status
         //--------------------------------------------------------------
         u8InstanceIndex = Du16WJ1939_eInstance1;
         do
         {
            u16State = eu16J1939_iState[u8InstanceIndex];
            u8InstanceIndex++;
         }while( (u8InstanceIndex < u8J1939_iNbOfInstance) && (u16State != Ceu16AMain_iJ1939WaitAdressClaim) );

         // If all instances passed the Wait Address Claim status
         if( u8InstanceIndex == u8J1939_iNbOfInstance )
         {
            eu16J1939_iState[u8PortInstance]=Ceu16AMain_iJ1939AllAdressClaimSucceeded;
            dwEndAddressClaim = T5_GetTickCount();
         }
         else
         {
            break;
         }

      case Ceu16AMain_iJ1939AllAdressClaimSucceeded:
         dwCurrent = T5_GetTickCount();
         
         //if((dwCurrent-dwEndAddressClaim)>=5000) // test: wait for few sec
         //{
            //s16T5_J1939_ActivateAllCyclicTxPGNs_exe(pPort, 1);
            
            // Register messages in a J1939 instance
            s16RetVal = s16T5_J1939_Register_Messages_exe(pPort, eu8J1939_TypeTx);
            s16RetVal = s16T5_J1939_Register_Messages_exe(pPort, eu8J1939_TypeRx);

            
            //-------------------------------
            // Set transition
            //-------------------------------
            eu16J1939_iState[u8PortInstance]=Ceu16AMain_iJ1939Running;
         //}
         //else
         //{
            // stay in Ceu16AMain_iJ1939AdressClaimSucceeded until all instances pass the AC wait status
         //}
         break;
      
      case Ceu16AMain_iJ1939Running:
         // nothing to do
         break;
      
      case Ceu16AMain_iJ1939Fail:
         // TBD
         break;
         
      default:
         break;
   }
}

////////////////////////////////////////////////////////////////////////////////
// Functions (Exported)
////////////////////////////////////////////////////////////////////////////////

//============================================================================//
// NAME : vT5_J1939_PreOpen
//============================================================================//
// ROLE : J1939 Straton pre open phase:
//        before open CAN port
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vT5_J1939_PreOpen (void)
{
   TU8 u8Instance, u8Index;
   
   for(u8Instance=0; u8Instance<Du8J1939_iMaxNbOfInstance; u8Instance++)
   {
      u16J1939MsgIndex[u8Instance] = 0;
      pPortUsedOnInstance[u8Instance] = 0;
      eu16J1939_iState[u8Instance] = Ceu16AMain_iJ1939WaitAdressClaim;

      for(u8Index=0; u8Index<Du8J1939_iMaxNbOfRequest; u8Index++)
      {
         pstJ1939_iRequest[u8Instance].stMsg[u8Index] = 0;
      }
      pstJ1939_iRequest[u8Instance].u8NbOfRequest = 0;
   }

   u8J1939_iNbOfInstance = Du16WJ1939_eInstance1;
}

//============================================================================//
// NAME : retT5_J1939_Open
//============================================================================//
// ROLE : J1939 Straton open phase:
//        Open CAN port 
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
//    szSettings (IN) configuration string entered in configuration
// OUTPUT : OK or error
//============================================================================//
T5_RET retT5_J1939_Open (T5PTR_DBCANPORT pPort, T5_PTCHAR szSettings)
{  
	TS16 s16RetVal;
      
   // Define protocol channel
	//-------------------------------
   switch(szSettings[0])
   {
      case 'A':
         pPort->oem.u8ProtocolChannel = eu8J1939_ChannelA;
         break;
      case 'B':
         pPort->oem.u8ProtocolChannel = eu8J1939_ChannelB;
         break;
      case 'C':
         pPort->oem.u8ProtocolChannel = eu8J1939_ChannelC;
         break;
      case 'D':
         pPort->oem.u8ProtocolChannel = eu8J1939_ChannelD;
         break;
      case 'E':
         pPort->oem.u8ProtocolChannel = eu8J1939_ChannelE;
         break;
      default:
         pPort->oem.u8ProtocolChannel = eu8J1939_InvalidChannel;
         return T5RET_ERROR;
         break;
   }

	// Setup the J1939 Stack Instance 
	//-------------------------------
   s16RetVal = s16T5_J1939_InitInstance_exe (pPort, szSettings);
   
   if(Ds16J1939_eSuccess!=s16RetVal)
   {
      return T5RET_ERROR; //(s16RetVal);
   }
   else
   {
      // Register messages in a J1939 instance -> moved after Address Claim
      //s16RetVal = s16T5_J1939_Register_Messages_exe(pPort, eu8J1939_TypeTx);
      //s16RetVal = s16T5_J1939_Register_Messages_exe(pPort, eu8J1939_TypeRx);
   }

   return T5RET_OK;
}

//============================================================================//
// NAME : vT5_J1939_PostClose
//============================================================================//
// ROLE : J1939 Straton post close phase:
//        Called by the STRATON driver after closing CAN ports
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vT5_J1939_PostClose (void)
{
}

//============================================================================//
// NAME : vT5_J1939_Close
//============================================================================//
// ROLE : J1939 Straton close phase:
//        Close a CAN port
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
// OUTPUT :
//============================================================================//
void vT5_J1939_Close (T5PTR_DBCANPORT pPort)
{
}

//============================================================================//
// NAME : vT5_J1939_OnPreExchange
//============================================================================//
// ROLE : J1939 Straton pre exchange phase:
//          Called by the STRAATON driver before exchanging driver data
//          You can use T5VMCan_FindRcvMsg to find a received message in driver memory
//          Call T5VMCan_MarkRcvMsg to indicate that a message was received
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
// OUTPUT :
//============================================================================//
void vT5_J1939_OnPreExchange (T5PTR_DBCANPORT pPort)
{
   T5PTR_DBCANMSG pMsg;
   T5_WORD wMsgCount;
   T5_WORD wNbMsg;
   TU8 u8PortInstance;
   T5PTR_CANMSGDEF pMsgDef;
   TU16 u16MsgPeriod;
   TU16 u16TimeElapseSinceRx;
   TU8 u8DataByteIndex;
   TuJ1939_HwStatus* puJ1939_HwStatus;
   
   u8PortInstance = pPort->oem.u8ProtocolInstance;
   
   if(Ceu16AMain_iJ1939Running!=eu16J1939_iState[u8PortInstance])
   {
      // execute address claim process and register messages
      vT5_J1939_AddrClaim_exe(pPort);      
   }
   else
   {
      // Get Rx message list
      pMsg = T5VMCan_GetRcvMsgList(pPort,&wNbMsg);
      for (wMsgCount=0; wMsgCount<wNbMsg; wMsgCount++)
      {
         // Get message definition
         pMsgDef = (T5PTR_CANMSGDEF)pMsg->pDef;

         u16MsgPeriod = pMsgDef->dwTMin;

         // Get specific hardware status variable
         puJ1939_HwStatus = (TuJ1939_HwStatus*)(&(pMsg->dwSpcIn));
         
         // Cyclic Rx Messages (period > 0 & nb data <= 8)
         if(eu3J1939_RegistredCycRxPGN == puJ1939_HwStatus->u3PgnRegistrationStatus)
         {
            // Set Nb of byte received
            puJ1939_HwStatus->u8NbOfBytes = pMsg->bLen;

            // Set address received (SA) --> can not be defined with CYC Rx msg
            puJ1939_HwStatus->u8Address = Du8WJ1939_eGlobalAddress;

            if(0==pMsg->oem.u8NbOfTimeOutPeriod)
            {
               // The time out error has NOT been detected in error callback
               //------------------------------------------------------------
               // Mark message as received
               T5VMCan_MarkRcvMsg (pPort,pMsg);
            }
            else
            {
               // The time out error HAS BEEN DETECTED in error callback, this one put all data to 0xFF
               //------------------------------------------------------------
               // if a frame has been received since the callback, data should be <> 0xFF
               for(u8DataByteIndex=0; u8DataByteIndex < pMsgDef->bLen; u8DataByteIndex++)
               {
                  if(0xFF != *((TU8*)(pMsg->data + u8DataByteIndex)))
                  {
                     // Mark message as received
                     T5VMCan_MarkRcvMsg (pPort,pMsg);
                     // Reset error flags
                     pMsg->oem.u8NbOfTimeOutPeriod = 0;
                     puJ1939_HwStatus->u1RxTimeOut = 0;
                     break;
                  }
                  else
                  {
                     // carry on checking data
                     // As long as all Data = 0xFF, do not execute T5VMCan_MarkRcvMsg, such that
                     // Straton application keeps old data value.
                  }
               }
            }
         }
         // APL messages (non cyclic or nb of data>8)
         else if(eu3J1939_RegistredAppPGN == puJ1939_HwStatus->u3PgnRegistrationStatus)
         {
            // Nb of byte received --> managed in vT5_J1939_eRxCallBack_Exe
            // Mark message as received --> managed in vT5_J1939_eRxCallBack_Exe
            
            // Time out flag
            if(0 != u16MsgPeriod)
            {
               // Calculate time since last received, expressed in ms on 16 bits (65,535 sec max)
               u16TimeElapseSinceRx = (TU16)T5_GetTickCount() - pMsg->oem.u16RxTimeStamp;
               pMsg->oem.u8NbOfTimeOutPeriod = u16TimeElapseSinceRx / u16MsgPeriod;
               // Check time out
               if(pMsg->oem.u8NbOfTimeOutPeriod >= pMsgDef->dwTMax)
               {
                  // Set time out flag
                  puJ1939_HwStatus->u1RxTimeOut = 1;
               }
               else
               {
                  // puJ1939_HwStatus->u1RxTimeOut set to 0 once message is received
                  // see vT5_J1939_eRxCallBack_Exe
               }               
            }
            else // (0==u16MsgPeriod)
            {
               // One shot message, no time out management
            }
         }
         else
         {
            // The Rx message has not been registered correctly ?
         }
         pMsg++;         
      }            
   }
   // Save number of request in preExchange in case one request occurred during application execution
   u8NbOfRequestInPreExchange[u8PortInstance] = pstJ1939_iRequest[u8PortInstance].u8NbOfRequest;
}

//============================================================================//
// NAME : vT5_J1939_OnPostExchange
//============================================================================//
// ROLE : J1939 Straton post exchange phase:
//          Called by the STRATON driver after exchanging driver data
//          You can use T5VMCan_PeekNextSndMsg/T5VMCan_PopNextSndMsg to get the list
//          of messages to be sent
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
// OUTPUT :
//============================================================================//
void vT5_J1939_OnPostExchange (T5PTR_DBCANPORT pPort)
{
   T5PTR_DBCANMSG pMsg;
   TstWJ1939_eTPLMsgData stMsg = {0};
   TstJ1939Identifier stMsgIdent;
   TS16 s16RetVal;
   TU8 u8PortInstance;
   T5PTR_CANMSGDEF pMsgDef;
   TuJ1939_HwStatus* puJ1939_HwStatus;
   TuJ1939_HwCommand* puJ1939_HwCommand;
   T5PTR_CANPORTDEF pPortDef;
   TU16 u16MsgPeriod;
   TU8 u8ReqIndex;
   TU8 u8NbOfReqPre, u8NbOfReqPost, u8NbOfReqBetweenPrePost;

   u8PortInstance = pPort->oem.u8ProtocolInstance;

   // Get port definition
   pPortDef = (T5PTR_CANPORTDEF)pPort->pDef;

   if(Ceu16AMain_iJ1939Running!=eu16J1939_iState[u8PortInstance])
   {
      // address claim process executed in vT5_J1939_OnPreExchange
      return;
   }
   else
   {
      // carry on
   }

   //------------------------------------------
   // Manage messages sent by the application
   //------------------------------------------
   pMsg = T5VMCan_PopNextSndMsg (pPort);
   while (pMsg != NULL)
   {
      // Get message definition
      pMsgDef = (T5PTR_CANMSGDEF)pMsg->pDef;
      u16MsgPeriod = pMsgDef->dwTMin;
      
      // get specific hardware status & command variable
      puJ1939_HwStatus = (TuJ1939_HwStatus*)(&(pMsg->dwSpcIn));
      puJ1939_HwCommand = (TuJ1939_HwCommand*)(&(pMsg->dwSpcOut));
      
      // Cyclic Tx Messages (period > 0)
      if(eu3J1939_RegistredCycTxPGN == puJ1939_HwStatus->u3PgnRegistrationStatus)
      {
         // Nothing to do
      }
      // one shot message or Requested message
      else if(eu3J1939_RegistredReqPGN == puJ1939_HwStatus->u3PgnRegistrationStatus)
      {
         // Check if the message buffer has been allocated in data pool
         if(0==pMsg->data)
         {
            puJ1939_HwStatus->u1PgnDataOutOfMemory = 1;
         }
         else
         {
            if(puJ1939_HwCommand->u1InhibitTransmission)
            {
               // do nothing
            }
            else
            {
               // Decode identifier
               vT5_J1939_DecodeIdent(pMsg->dwID, &stMsgIdent);

               stMsg.u8Addr      = stMsgIdent.u8DestinationAddress;
               stMsg.pu8Data     = pMsg->data;
               if(pMsg->bLen <= pMsgDef->bLen)
               {
                  stMsg.u16Length   = pMsg->bLen;  // length set by application (dynamic)
               }
               else
               {
                  stMsg.u16Length   = pMsgDef->bLen;     // limit to maximum length set in message definition (static)
               }
               stMsg.u8Remote    = 0x00;
               stMsg.u16PGN      = stMsgIdent.u16PGN;
               stMsg.u8DataPage  = stMsgIdent.u8DataPage;
               stMsg.u8PrioType  = stMsgIdent.u8Priority;
               
               s16RetVal = DstWMonitor_eApplication_Function.s16WJ1939_eSendMsg_Exe(
                                                               pPort->oem.u8ProtocolInstance, //TU8 Fu8InstNum,
                                                               &stMsg);                         //TstWJ1939_eTPLMsgData * FpstMsg,                  
            }
         }
      }      
      else
      {
         // The Tx message has not been registered correctly ?
      }
      // Set Nb of byte transmitted
      puJ1939_HwStatus->u8NbOfBytes = stMsg.u16Length;
      // Set address transmitted (DA)
      puJ1939_HwStatus->u8Address = stMsg.u8Addr;
      
      // Get next message to be sent
      pMsg = T5VMCan_PopNextSndMsg(pPort);
   }

   //---------------------------------------------------------------------------------
   // Check for requested messages (sent in vT5_J1939_eReqCallBack_Exe)
   // and reset flag u1SentFromRequest since it must have been read by the application
   //---------------------------------------------------------------------------------
   u8NbOfReqPre   = u8NbOfRequestInPreExchange[u8PortInstance];
   u8NbOfReqPost  = pstJ1939_iRequest[u8PortInstance].u8NbOfRequest;
   if(0!=u8NbOfReqPre)
   {
      // take number of request saved in preExchange in case one request occurred during application execution
      // to reset flag only if we are sure the application could read it
      for(u8ReqIndex=0; u8ReqIndex<u8NbOfReqPre; u8ReqIndex++)
      {
         pMsg = (T5PTR_DBCANMSG)pstJ1939_iRequest[u8PortInstance].stMsg[u8ReqIndex];
         // Get specific hardware status variable and reset flag u1SentFromRequest
         puJ1939_HwStatus = (TuJ1939_HwStatus*)(&(pMsg->dwSpcIn));
         puJ1939_HwStatus->u1SentFromRequest = 0;         
      }

      // in case requests occured durring application, shift them to begining of table to be read in next app cycle
      if(u8NbOfReqPost > u8NbOfReqPre)
      {
         u8NbOfReqBetweenPrePost = u8NbOfReqPost - u8NbOfReqPre;
         for(u8ReqIndex=0; u8ReqIndex<u8NbOfReqBetweenPrePost; u8ReqIndex++)
         {
            pstJ1939_iRequest[u8PortInstance].stMsg[u8ReqIndex] = pstJ1939_iRequest[u8PortInstance].stMsg[u8NbOfReqPre+u8ReqIndex];
         }         
         pstJ1939_iRequest[u8PortInstance].u8NbOfRequest = u8NbOfReqBetweenPrePost;         
      }
      else
      {
         pstJ1939_iRequest[u8PortInstance].u8NbOfRequest = 0;         
      }
   }
   else
   {
      // nothing to do
   }
}

//============================================================================//
// NAME : boolT5_J1939_GetBusOffFlag
//============================================================================//
// ROLE : Get the CAN "Bus Off" error flag
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
// OUTPUT : BUS OFF flag
//============================================================================//
T5_BOOL boolT5_J1939_GetBusOffFlag (T5PTR_DBCANPORT pPort)
{
   return(0);
}

//============================================================================//
// NAME : dwordT5_J1939_GetErrors
//============================================================================//
// ROLE : Get the J1939 errors
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
// OUTPUT : J1939 errors value
//============================================================================//
T5_DWORD dwordT5_J1939_GetErrors (T5PTR_DBCANPORT pPort)
{
   return(0);
}

//============================================================================//
// NAME : vT5_J1939_OnSpcOut
//============================================================================//
// ROLE : 
//          This function is called when a new "hardware specific control" value
//          is passed to a configured CAN message
//----------------------------------------------------------------------------//
// INPUT  :
//    pPort (IN/OUT) pointer to the port object in driver memory
//    pMsg (IN/OUT) pointer to the message object in driver memory
// OUTPUT :
//============================================================================//
void vT5_J1939_OnSpcOut (T5PTR_DBCANPORT pPort, T5PTR_DBCANMSG pMsg)
{
   TuJ1939_HwCommand* puJ1939_HwCommand;
   TS16 s16RetVal;
   TstJ1939Identifier stMsgIdent;
   TU16 u16MsgPeriod;
   TU8 u8PortInstance;
   T5PTR_CANMSGDEF pMsgDef;
   TU8 u8Active;

   // Get instance from port
   u8PortInstance = pPort->oem.u8ProtocolInstance;
   
   // get specific hardware command variable
   puJ1939_HwCommand = (TuJ1939_HwCommand*)(&(pMsg->dwSpcOut));

   // Get message definition
   pMsgDef = (T5PTR_CANMSGDEF)pMsg->pDef;

   // Decode identifier
   vT5_J1939_DecodeIdent(pMsg->dwID, &stMsgIdent);

   u16MsgPeriod = pMsgDef->dwTMin;

   // Cyclic Tx Messages (period > 0)
   if(u16MsgPeriod > 0)
   {
      if(puJ1939_HwCommand->u1InhibitTransmission)
      {
         u8Active = 0;
      }
      else
      {
         u8Active = 1;
      }
      s16RetVal = DstWMonitor_eApplication_Function.s16WJ1939_eActivateCycPGNs_Exe(
                                                      u8PortInstance,         //TU8 Fu8InstNum,
                                                      u8Active,               //TU8 Fu8Activate,
                                                      stMsgIdent.u16PGN,      //TU16 Fu16PGN,
                                                      stMsgIdent.u8DataPage); //TU8 Fu8DataPage
   }
   else
   {
      // one shot message manage in post exchange function
   }

}

////////////////////////////////////////////////////////////////////////////////
// Callback Functions (Exported)
////////////////////////////////////////////////////////////////////////////////

//============================================================================//
// NAME : vT5_J1939_eErrCallBack_Exe
//============================================================================//
// ROLE : J1939 Stack Error Callback
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vT5_J1939_eErrCallBack_Exe(TU8 Fu8InstNum, TstWJ1939_eErrorData* FpstErrorDesc)
{
   TuJ1939ErrorCode uErrorCode;
   T5PTR_DBCANMSG pMsg;
   T5PTR_DBCANPORT pPort;
   TuJ1939_HwStatus* puJ1939_HwStatus;
   TU8 u8DataByteIndex;
   T5PTR_CANMSGDEF pMsgDef;

   uErrorCode.u16ErrorCode = FpstErrorDesc->u16ErrorCode;

   // Get port used on instance
   if(0!=pPortUsedOnInstance[Fu8InstNum])
   {
      pPort = pPortUsedOnInstance[Fu8InstNum];      
   }
   else return;

   // Find message in Rx message list
   pMsg = pT5_J1939_FindMsg (pPort, FpstErrorDesc->u32AddInfo, Du8WJ1939_eGlobalAddress, eu8J1939_TypeRx);
   if(NULL == pMsg)
   {
      // Found message in Tx message list
      pMsg = pT5_J1939_FindMsg (pPort, FpstErrorDesc->u32AddInfo, Du8WJ1939_eGlobalAddress, eu8J1939_TypeTx);      
   }
   
   if(NULL == pMsg)
   {
      // message not found, do nothing
   }
   else
   {
      // Get message definition
      pMsgDef = (T5PTR_CANMSGDEF)pMsg->pDef;
      // Get specific hardware status variable
      puJ1939_HwStatus = (TuJ1939_HwStatus*)(&(pMsg->dwSpcIn));           
      puJ1939_HwStatus->u8ErrorNb = uErrorCode.u8Number;

      switch(uErrorCode.u5ID)
      {
         case Du16WJ1939_eErrorERRId:
         case Du16WJ1939_eErrorDLLId:
         case Du16WJ1939_eErrorTPLId:
         case Du16WJ1939_eErrorNWMId:
         case Du16WJ1939_eErrorAPLId:
            break;

         // CYCLIC error
         //--------------------------------
         case Du16WJ1939_eErrorCYCId:
            switch(uErrorCode.u8Number)
            {
               case Du16WJ1939_eErrorCYCRxTimeout:

                  // set data to "not available" (0xFF)
                  for(u8DataByteIndex=0; u8DataByteIndex < pMsgDef->bLen; u8DataByteIndex++)
                  {
                     //pMsg->data[u8DataByteIndex] = 0xFF;
                     *((TU8*)(pMsg->data + u8DataByteIndex)) = 0xFF;
                  }
                  
                  // update nb of time out period
                  if(pMsg->oem.u8NbOfTimeOutPeriod < 0xFF)
                  {
                     pMsg->oem.u8NbOfTimeOutPeriod++;
                  }
                  else
                  {
                     pMsg->oem.u8NbOfTimeOutPeriod = 0xFF;
                  }

                  // Set time out flag
                  if(pMsg->oem.u8NbOfTimeOutPeriod >= pMsgDef->dwTMax)
                  {
                     // set time out
                     puJ1939_HwStatus->u1RxTimeOut = 1;
                  }
                  else
                  {
                     // puJ1939_HwStatus->u1RxTimeOut set to 0 once data available (<> 0xFF)
                     // see in vT5_J1939_OnPreExchange
                  }
                  break;
               case Du16WJ1939_eErrorCYCTxInvalidHandle:
               case Du16WJ1939_eErrorCYCTxQueueOverrun:
                  break;
               default:
                  break;
            }
            break;

         case Du16WJ1939_eErrorMEMId:
         default:
            break;
      }      
   }
}

//============================================================================//
// NAME : vT5_J1939_eRxCallBack_Exe
//============================================================================//
// ROLE : J1939 Stack Error Callback
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vT5_J1939_eRxCallBack_Exe(TU8 Fu8InstNum, TstWJ1939_eTPLMsgData* FpstMsg)
{
   T5PTR_DBCANMSG pMsg;
   TstJ1939Identifier stMsgIdent;
   T5PTR_DBCANPORT pPort;
   TuJ1939_HwStatus* puJ1939_HwStatus;
   TU32 u32MsgPGN;
   
   // Get port used on instance
   if(0!=pPortUsedOnInstance[Fu8InstNum])
   {
      pPort = pPortUsedOnInstance[Fu8InstNum];      
   }
   else return;

   // Find message in Rx message list
   u32MsgPGN = ((TU32)(FpstMsg->u8DataPage)<<16) + FpstMsg->u16PGN;
   pMsg = pT5_J1939_FindMsg (pPort, u32MsgPGN, FpstMsg->u8Addr, eu8J1939_TypeRx);
      
   if(NULL == pMsg)
   {
      // message not found, do nothing
   }
   else
   {
      // Check if the message buffer has been allocated in data pool
      if(0==pMsg->data)
      {
         // no data buffer allocated, do nothing
      }         
      else
      {
         // Get DATA
         DstWMonitor_eApplication_Function.ppcAPI_eMemcpy_Exe((char *)pMsg->data,(const char *)FpstMsg->pu8Data,FpstMsg->u16Length);
         // Get specific hardware status variable
         puJ1939_HwStatus = (TuJ1939_HwStatus*)(&(pMsg->dwSpcIn));
         // Set Nb of byte received
         puJ1939_HwStatus->u8NbOfBytes = FpstMsg->u16Length;
         // Set address received (SA)
         puJ1939_HwStatus->u8Address = FpstMsg->u8Addr;
         // Record rx time stamp
         pMsg->oem.u16RxTimeStamp = (TU16)T5_GetTickCount();
         // Reset error flags
         pMsg->oem.u8NbOfTimeOutPeriod = 0;
         puJ1939_HwStatus->u1RxTimeOut = 0;
         // Mark msg as received for data update
         T5VMCan_MarkRcvMsg (pPort,pMsg);
      } // end else of if(0==pMsg->data)
   } // end else of if(NULL == pMsg)
}

//============================================================================//
// NAME : vT5_J1939_eNwmCallBack_Exe
//============================================================================//
// ROLE : J1939 Network Callback
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vT5_J1939_eNwmCallBack_Exe(TU8 Fu8InstNum, TU8 Fu8SA, TU8 * Fpu8Data)
{
/* TBD
	TS16 s16Status;
	TunName unClaimedDeviceName;

	DstWMonitor_eApplication_Function.vWJ1939_eGetDeviceName_Exe( Fpu8Data, &unClaimedDeviceName );

	// Register the ECU instance 2
	if( (unClaimedDeviceName.stFields.u3ECUInstance == 2) && (Fu8InstNum == Du16WJ1939_eInstance3) )
	{
		s16Status = DstWMonitor_eApplication_Function.s16WJ1939_eRegisterDevice_Exe( Fu8InstNum, &unClaimedDeviceName, &u8AMain_iECUHandle );
	}
*/
}

//============================================================================//
// NAME : vT5_J1939_eReqCallBack_Exe
//============================================================================//
// ROLE : J1939 Request Callback
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vT5_J1939_eReqCallBack_Exe(TU8 Fu8InstNum, TstWJ1939_eTPLMsgData* FpstMsg)
{
   T5PTR_DBCANMSG pMsg;
   T5PTR_DBCANPORT pPort;
   TuJ1939_HwStatus* puJ1939_HwStatus;
   TU32 u32MsgPGN;
   TstJ1939Identifier stMsgIdent;
   TS16 s16RetVal;
   
   // Get port used on instance
   if(0!=pPortUsedOnInstance[Fu8InstNum])
   {
      pPort = pPortUsedOnInstance[Fu8InstNum];      
   }
   else return;

   // Find message in Tx message list
   u32MsgPGN = ((TU32)(FpstMsg->u8DataPage)<<16) + FpstMsg->u16PGN;
   pMsg = pT5_J1939_FindMsg (pPort, u32MsgPGN, Du8WJ1939_eGlobalAddress, eu8J1939_TypeTx);
   
   if(NULL == pMsg)
   {
      // message not found, do nothing
   }
   else
   {
      // FpstMsg = message to send   
      // Fields already set by J1939 stack:
      // FpstMsg->u16PGN
      // FpstMsg->u8DataPage
      // FpstMsg->u8Addr = addr of ECU requesting PGN

      // Decode identifier
      vT5_J1939_DecodeIdent(pMsg->dwID, &stMsgIdent);
      
      //---------------------------------------------------------------------------------------------------------------------------------------------------
      // Same algorithm as in J1939 library: -->                                       // --> req_callback in  APLreq.c :
      //---------------------------------------------------------------------------------------------------------------------------------------------------
      // In case of a global request, set the target address to 0xFF                   // /* In case of a global request, set the target address to 0xFF */
      if (FpstMsg->u8PrioType == Du8J1939_iGlobalType)                                 // if (ps_msg->b_prio_or_type == TYP_k_GLO)
      {                                                                                // {
        FpstMsg->u8Addr = Du8J1939_iBroadcastAddr;                                     //   ps_msg->b_addr = 0xFF;
      }                                                                                // }

      // Set remaining fields of the response message                                  // /* Set remaining fields of the response message */
      FpstMsg->u16Length   = pMsg->bLen;                                               // ps_msg->w_length       = pw_fltr_to_clbk->w_length;
      FpstMsg->u8PrioType  = stMsgIdent.u8Priority;                                    // ps_msg->b_prio_or_type = pw_fltr_to_clbk->b_prio;

      // Copy data of the response message                                             // /* Copy data of the response message */
      DstWMonitor_eApplication_Function.ppcAPI_eMemcpy_Exe(FpstMsg->pu8Data,           // GOE_EnterCriticalSection(b_inst_num);
                                                           pMsg->data,                 // MEMCPY(ps_msg->pb_data, pw_fltr_to_clbk->pb_data, ps_msg->w_length);
                                                           FpstMsg->u16Length);        // GOE_LeaveCriticalSection(b_inst_num);

      // Send response of request message                                              // /* Send response of request message */
      s16RetVal = DstWMonitor_eApplication_Function.s16WJ1939_eSendMsg_Exe(Fu8InstNum, // if (!APL_SendMsg(b_inst_num, ps_msg))
                                                                           FpstMsg);   // {
                                                                                       //   PACK_PGN(dw_pgn, ps_msg->w_pgn, ps_msg->b_data_page);
                                                                                       //   ERR_SetAsyncError(b_inst_num, APL_ERR_REQ_RESPONSE, dw_pgn);
                                                                                       // }
      //---------------------------------------------------------------------------------------------------------------------------------------------------
                                                                           
      // Get specific hardware status variable and set flag u1SentFromRequest
      puJ1939_HwStatus = (TuJ1939_HwStatus*)(&(pMsg->dwSpcIn));
      puJ1939_HwStatus->u1SentFromRequest = 1;
      // Set Nb of byte transmitted
      puJ1939_HwStatus->u8NbOfBytes = FpstMsg->u16Length;
      // Set address transmitted (DA)
      puJ1939_HwStatus->u8Address = FpstMsg->u8Addr;
      
      // Save requested msg pointer in FIFO, used to reset flag u1SentFromRequest once read
      pstJ1939_iRequest[Fu8InstNum].stMsg[pstJ1939_iRequest[Fu8InstNum].u8NbOfRequest] = (TstWJ1939_eTPLMsgData*)pMsg;
      if(pstJ1939_iRequest[Fu8InstNum].u8NbOfRequest<Du8J1939_iMaxNbOfRequest)
      {
         pstJ1939_iRequest[Fu8InstNum].u8NbOfRequest++;         
      }
      else
      {
         pstJ1939_iRequest[Fu8InstNum].u8NbOfRequest = Du8J1939_iMaxNbOfRequest;
      }
   }
}
