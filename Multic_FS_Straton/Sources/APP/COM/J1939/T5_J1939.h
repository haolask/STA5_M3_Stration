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
// FILE : T5_J1939.h (HEADER)
//----------------------------------------------------------------------------
// DESC : J1939 protocol management for Straton application 
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 772 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Sources/APP/CORE/VM/custom/T5ACTIA.c $
// Updated the :$Date: 2015-07-22 14:16:15 +0200 (mer., 22 juil. 2015) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
/* T5_J1939.h */
#ifndef T5_J1939_INCLUDE
#define T5_J1939_INCLUDE

#include "T5_J1939.cfg"

//****************************************************************************//
// DEFINITION
//****************************************************************************//
#define Ds16J1939_eSuccess                0
#define Ds16J1939_eError                  -1
#define Ds16J1939_eErrorBadParameter      -2
#define Ds16J1939_eErrorNbInstPerChannel  -3
#define Ds16J1939_eErrorNbInstances       -4
#define Ds16J1939_eErrorOutOfMemory       -5
#define Ds16J1939_eErrorChannelNotWired   -6


enum {
   eu8J1939_ChannelA,
   eu8J1939_ChannelB,
   eu8J1939_ChannelC,
   eu8J1939_ChannelD,
   eu8J1939_ChannelE,   
   eu8J1939_NbOfChannel,
   
   // error values
   eu8J1939_NoChannel,
   eu8J1939_InvalidChannel   
};

enum {
   eu8J1939_TypeTx,
   eu8J1939_TypeRx,
   eu8J1939_NbOfType
};

enum
{
   eu3J1939_NotRegistred      =0U,
   eu3J1939_RegistredAppPGN   =1U,
   eu3J1939_RegistredCycRxPGN =2U,
   eu3J1939_RegistredCycTxPGN =3U,
   eu3J1939_RegistredReqPGN   =4U,
   eu3J1939_RegistrationErr   =5U,
   eu3J1939_InstanceInitErr   =6U,
   eu3J1939_NbOfRegistrationStatus
};

//****************************************************************************//
// MACRO
//****************************************************************************//
/*-----------------------------------------------------------------------*/
/* Constants for the address field according to the J1939 specification  */
/*-----------------------------------------------------------------------*/
#define Du8J1939_iBroadcastAddr  (TU8) 255 /* Broadcast address (global message) */
/*-----------------------------------------------------------------------*/
/* Constants for the PDU field according to the J1939 specification      */
/*-----------------------------------------------------------------------*/
#define Du8J1939_iGlobalPDU      (TU8) 240 /* All messages with a PDU higher */
/*-----------------------------------------------------------------------*/
/* Constants for the message types according to the J1939 specification  */
/*-----------------------------------------------------------------------*/
#define Du8J1939_iGlobalType     (TU8) 0 /* Global message */
#define Du8J1939_iSpecificType   (TU8) 1 /* Specific message */

//****************************************************************************//
// TYPEDEF
//****************************************************************************//
typedef struct
{
   TU16 u16PGN;
   TU8 u8SourceAddress;
   TU8 u8DestinationAddress;
   TU8 u8PduSpecific;
   TU8 u8PduFormat;
   TU8 u8DataPage;
   TU8 u8Priority;
} TstJ1939Identifier;

typedef TU8* Tpu8J1939_iDataPointer;

typedef enum
{
	Ceu16AMain_iJ1939WaitAdressClaim,
	Ceu16AMain_iJ1939AdressClaimSucceeded,
	Ceu16AMain_iJ1939AllAdressClaimSucceeded,
	Ceu16AMain_iJ1939Running,
	Ceu16AMain_iJ1939Fail
} Teu16AMain_iJ1939State;

typedef union
{
   TU16 u16ErrorCode;
   struct
   {
#ifdef T5DEF_BIGENDIAN
      TU8 u1Reserved :1;   // bit 7
      TU8 u2Class    :2;   // bit 5-6
      TU8 u5ID       :5;   // bit 0-4
      TU8 u8Number;
#else//T5DEF_BIGENDIAN
      TU8 u8Number;
      TU8 u5ID       :5;   // bit 0-4
      TU8 u2Class    :2;   // bit 5-6
      TU8 u1Reserved :1;   // bit 7
#endif//T5DEF_BIGENDIAN
   };
} TuJ1939ErrorCode;

typedef union
{
   TU32 u32SpcIn;
   struct
   {
#ifdef T5DEF_BIGENDIAN
      // bytes 3
      TU8 u8ErrorNb;
      
      // bytes 2
      TU8 u8Address;

      // byte 1
      TU8 u8NbOfBytes;
      
      // byte 0
      TU8 u3PgnRegistrationStatus:3;   // bit 5-7
      TU8 u1SentFromRequest      :1;   // bit 4
      TU8 u1RxTimeOut            :1;   // bit 3
      TU8 u1PgnDataOutOfMemory   :1;   // bit 2
      TU8 u1InvalidPortSettings  :1;   // bit 1
      TU8 u1ProtocolNotWired     :1;   // bit 0
      
#else // LITTLE ENDIAN
      // byte 0
      TU8 u1ProtocolNotWired     :1;   // bit 0
      TU8 u1InvalidPortSettings  :1;   // bit 1
      TU8 u1PgnDataOutOfMemory   :1;   // bit 2
      TU8 u1RxTimeOut            :1;   // bit 3
      TU8 u1SentFromRequest      :1;   // bit 4
      TU8 u3PgnRegistrationStatus:3;   // bit 5-7
      
      // byte 1
      TU8 u8NbOfBytes;
      
      // bytes 2
      TU8 u8Address;
      
      // bytes 3
      TU8 u8ErrorNb;
#endif//T5DEF_BIGENDIAN
   };
} TuJ1939_HwStatus;

typedef union
{
   TU32 u32SpcOut;
   struct
   {
#ifdef T5DEF_BIGENDIAN
      // bytes 2&3
      TU8 u8Byte3;
      TU8 u8Byte2;
      
      // byte 1
      TU8 u8Byte1;
      
      // byte 0
      TU8 u1Bit7                 :1;   // bit 7
      TU8 u1Bit6                 :1;   // bit 6
      TU8 u1Bit5                 :1;   // bit 5
      TU8 u1Bit4                 :1;   // bit 4
      TU8 u1Bit3                 :1;   // bit 3
      TU8 u1Bit2                 :1;   // bit 2
      TU8 u1Bit1                 :1;   // bit 1
      TU8 u1InhibitTransmission  :1;   // bit 0
#else//T5DEF_BIGENDIAN
      // byte 0
      TU8 u1InhibitTransmission  :1;   // bit 0
      TU8 u1Bit1                 :1;   // bit 1
      TU8 u1Bit2                 :1;   // bit 2
      TU8 u1Bit3                 :1;   // bit 3
      TU8 u1Bit4                 :1;   // bit 4
      TU8 u1Bit5                 :1;   // bit 5
      TU8 u1Bit6                 :1;   // bit 6
      TU8 u1Bit7                 :1;   // bit 7
      
      // byte 1
      TU8 u8Byte1;
      
      // bytes 2&3
      TU8 u8Byte2;
      TU8 u8Byte3;
#endif//T5DEF_BIGENDIAN
   };
} TuJ1939_HwCommand;

typedef struct
{
   TstWJ1939_eTPLMsgData* stMsg[Du8J1939_iMaxNbOfRequest];
   TU8 u8NbOfRequest;
} TstJ1939_Request;

//****************************************************************************//
// EXPORT
//****************************************************************************//


//****************************************************************************//
// PROTO
//****************************************************************************//
extern void vT5_J1939_PreOpen (void);
extern T5_RET retT5_J1939_Open (T5PTR_DBCANPORT pPort, T5_PTCHAR szSettings);
extern void vT5_J1939_PostClose (void);
extern void vT5_J1939_Close (T5PTR_DBCANPORT pPort);
extern void vT5_J1939_OnPreExchange (T5PTR_DBCANPORT pPort);
extern void vT5_J1939_OnPostExchange (T5PTR_DBCANPORT pPort);
extern T5_BOOL boolT5_J1939_GetBusOffFlag (T5PTR_DBCANPORT pPort);
extern T5_DWORD dwordT5_J1939_GetErrors (T5PTR_DBCANPORT pPort);
extern void vT5_J1939_OnSpcOut (T5PTR_DBCANPORT pPort, T5PTR_DBCANMSG pMsg);

extern T5_PTBYTE ptbyteT5_J1939_OemAlloc (T5PTR_DBCANPORT pPort, T5_DWORD dwID, T5_BYTE bLen);
#endif
