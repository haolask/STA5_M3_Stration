//////////////////////////////////////////////////////////////////////////////
//
//      O       OOOO  OOOOOOO   O       O         ## ## ##
//     O O     O         O      O      O O
//    O   O    O         O      O     O   O      ## ## ##
//   OOOOOOO   O         O      O    OOOOOOO
//  O       O   OOOO     O      O   O       O   ## ## ##
//
//----------------------------------------------------------------------------
// HEAD : (c) ACTIA Automotive 2011 : " Any copy and re-use of this
//        file without the written consent of ACTIA is forbidden "
//        --------------------------------------------------------------------
//        Classification :  (-)No   (-)Confident. ACTIA   (-)Confident. Client
////////////////////////////////////////////////////////////////////////////////
// MODULE> J1939 >MODULE
//----------------------------------------------------------------------------
// FILE> J1939.h (HEADER) >FILE
//----------------------------------------------------------------------------
// DESC> J1939 Librairy API >DESC
//----------------------------------------------------------------------------
// HIST>
// Version 		:$Revision: 1207 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/ADDONS/Lib_J1939/Sources/API/Lib_J1939.h $
// Updated the :$Date: 2016-03-14 16:55:45 +0100 (lun., 14 mars 2016) $
// By 			:$Author: pcouronn $
// >HIST
////////////////////////////////////////////////////////////////////////////////
#ifndef J1939_LIB_INCLUDE
#define J1939_LIB_INCLUDE
//----------------------------------------------------------------------------//
#include "std_type.h"
#include "ver_type.h"
//----------------------------------------------------------------------------//


//////////////////////////////////////////////////////////////////////////////
// DEFINITION
//////////////////////////////////////////////////////////////////////////////
//Instance Identifiers ( Check that used library support the number of instance you need)
#define Du16WJ1939_eInstance1                0
#define Du16WJ1939_eInstance2                1
#define Du16WJ1939_eInstance3                2
#define Du16WJ1939_eInstance4                3
#define Du16WJ1939_eInstance5                4
#define Du16WJ1939_eInstance6                5
#define Du16WJ1939_eInstance7                6
#define Du16WJ1939_eInstance8                7
#define Du16WJ1939_eInstance9                8
#define Du16WJ1939_eInstance10               9
#define Du16WJ1939_eInstance11               10
#define Du16WJ1939_eInstance12               11
#define Du16WJ1939_eInstance13               12
#define Du16WJ1939_eInstance14               13
#define Du16WJ1939_eInstance15               14
#define Du16WJ1939_eInstance16               15
#define Du16WJ1939_eInstance17               16
#define Du16WJ1939_eInstance18               17
#define Du16WJ1939_eInstance19               18
#define Du16WJ1939_eInstance20               19
#define Du16WJ1939_eNbOfInstance             20

//Shared Instance MB
#define Du16WJ1939_eMBSharedInstance1        0xF0
#define Du16WJ1939_eMBSharedInstance2        0xF1
#define Du16WJ1939_eMBSharedInstance3        0xF2
#define Du16WJ1939_eMBSharedInstance4        0xF3
#define Du16WJ1939_eMBSharedInstance5        0xF4
#define Du16WJ1939_eMBSharedInstance6        0xF5
#define Du16WJ1939_eMBSharedInstance7        0xF6
#define Du16WJ1939_eMBSharedInstance8        0xF7
#define Du16WJ1939_eMBSharedInstance9        0xF8
#define Du16WJ1939_eMBSharedInstance10       0xF9
#define Du16WJ1939_eMBSharedInstance11       0xFA
#define Du16WJ1939_eMBSharedInstance12       0xFB
#define Du16WJ1939_eMBSharedInstance13       0xFC
#define Du16WJ1939_eMBSharedInstance14       0xFD
#define Du16WJ1939_eMBSharedInstance15       0xFE

//Return Codes
#define Ds16WJ1939_eSuccess                  0
#define Ds16WJ1939_eError                    -1

//Error Identifiers
#define Du16WJ1939_eErrorERRId               0U	
#define Du16WJ1939_eErrorDLLId               1U
#define Du16WJ1939_eErrorTPLId               2U
#define Du16WJ1939_eErrorNWMId               3U
#define Du16WJ1939_eErrorAPLId               4U
#define Du16WJ1939_eErrorCYCId               5U
#define Du16WJ1939_eErrorMEMId               6U

//Error Classes
#define Du16WJ1939_eErrorClassWarning        0U
#define Du16WJ1939_eErrorClassError          1U
#define Du16WJ1939_eErrorClassFatal          2U

//APL Error Numbers
#define Du16WJ1939_eErrorAPLTimeExceeded     1U    //Main Manager Task was not called in the required sequence-API
#define Du16WJ1939_eErrorAPLTxLong           2U    //The data field of the transmit message is too long-API
#define Du16WJ1939_eErrorAPLNoMemory         3U    //No memory for the data field of the transmit message allocated-API
#define Du16WJ1939_eErrorAPLRegRequest       4U    //An error occurred during registration of a request PGN-API
#define Du16WJ1939_eErrorAPLTxNoAddr         5U    //Device has no address claimed-API
#define Du16WJ1939_eErrorAPLDevList          6U    //Device list is disabled-API
#define Du16WJ1939_eErrorAPLInvalidPtr       7U    //Invalid pointer passed to an API function
#define Du16WJ1939_eErrorAPLReqResponse      11U   //A request message couldn't be answered from the request handler because
                                                   //there was no place in the queue.
//CYC Error Numbers
#define Du16WJ1939_eErrorCYCRxRegistration   1U    //The maximal number of receive messages is exceeded.-API
#define Du16WJ1939_eErrorCYCTxRegistration   2U    //The maximal number of transmit messages is exceeded.-API
#define Du16WJ1939_eErrorCYCRxTimeout        11U   //A registred message wasn't received in the given time-API
#define Du16WJ1939_eErrorCYCTxInvalidHandle  12U   //A message couldn't be sent due to an invalid address handle
#define Du16WJ1939_eErrorCYCTxQueueOverrun   13U   //A message couldn't be sent due to an overrun of the transmit queue
//DLL Error Numbers
#define Du16WJ1939_eErrorDLLErrTxOverrun     1U    //Overrun in the Tx Queue
#define Du16WJ1939_eErrorDLLErrRxOverrun     15U   //Overrun in the Rx Queue
//MEM Error Numbers
#define Du16WJ1939_eErrorMEMAllocMemory      11U   //Allocate memory failed-API
#define Du16WJ1939_eErrorMEMFreeMemory       12U   //Free memory failed-API
//NWM Error Numbers
#define Du16WJ1939_eErrorNWMDeviceReg        1U    //The device is already registered in the device list-API
#define Du16WJ1939_eErrorNWMInvalidHdl       2U    //The device handle is invalid-API
#define Du16WJ1939_eErrorNWMInvalidAddr      3U    //The device address is invalid-API
#define Du16WJ1939_eErrorNWMNoAddr           11U   //The device can't claim a free address-API
#define Du16WJ1939_eErrorNWMListFull         12U   //The maximum number of nodes in the network is exceeded-API
#define Du16WJ1939_eErrorNWMInvalidClaim     13U   //Address claim with invalid address received-API
//TPL Error Numbers
#define Du16WJ1939_eErrorTPLTxOvrn           1U    //No free Entry in the TxQueue of the TPL available-API
#define Du16WJ1939_eErrorTPLConfFilter       2U    //Too many PGNs tried to register in the config filter-API
#define Du16WJ1939_eErrorTPLUnexpBAMFrm      11U   //Unexpected BAM frame received-API
#define Du16WJ1939_eErrorTPLUnexpRTSFrm      12U   //Unexpected RTS frame received-API
#define Du16WJ1939_eErrorTPLUnexpCTSFrm      13U   //Unexpected CTS frame received-API
#define Du16WJ1939_eErrorTPLUnexpEOMFrm      14U   //Unexpected EOM frame received-API
#define Du16WJ1939_eErrorTPLUnexpCAFrm       15U   //Unexpected CA frame received-API
#define Du16WJ1939_eErrorTPLUnexpDTFrm       16U   //Unexpected DT frame received-API
#define Du16WJ1939_eErrorTPLRxOvrn           17U   //No free Rx Msg Buffer in the TPL for a global message available-API
#define Du16WJ1939_eErrorTPLTxTimeout0       18U   //Timer overrun for Tx Timeout T0-API
#define Du16WJ1939_eErrorTPLTxTimeout2       19U   //Timer overrun for Tx Timeout T2-API
#define Du16WJ1939_eErrorTPLRxTimeout0       20U   //Timer overrun for Rx Timeout T0-API
#define Du16WJ1939_eErrorTPLRxTimeout1       21U   //Timer overrun for Rx Timeout T1-API
#define Du16WJ1939_eErrorTPLRxTimeout2       22U   //Timer overrun for Rx Timeout T2-API
#define Du16WJ1939_eErrorTPLRxLong           23U   //The length of the received message is too long for a seg msg-API
#define Du16WJ1939_eErrorTPLSendMsg          24U   //Error by Sending: Can't send a segmented message-API
#define Du16WJ1939_eErrorTPLSendCA           25U   //Sending a CA message failed-API
#define Du16WJ1939_eErrorTPLSendNACK         26U   //Sending a NACK message failed-API

//Address Claiming Status
#define Du16WJ1939_eACBusy                   0U    //Address claiming in process
#define Du16WJ1939_eACSuccess                1U    //Address claimed
#define Du16WJ1939_eACFailed                 2U    //Cannot claim an address

//Special Addresses
#define Du8WJ1939_eGlobalAddress             255U
#define Du8WJ1939_eZeroAddress               254U

//Addressing types
#define Du16WJ1939_eAddrGlobal               0U
#define Du16WJ1939_eAddrSpecific             1U

//PGN Predefined
#define Du16WJ1939_ePGNAddressClaim          0xEE00


//////////////////////////////////////////////////////////////////////////////
// MACRO
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// TYPEDEF
//////////////////////////////////////////////////////////////////////////////
typedef union
{   struct
   {  
      TU32 u21IdNumber              :21;
      TU32 u11ManufCode             :11;
      TU32 u3ECUInstance            :3;
      TU32 u5FunctionInstance       :5;
      TU32 u8Function               :8;
      TU32 u1Reserved               :1;
      TU32 u7VehicleSystem          :7;
      TU32 u4VehicleSystemInstance  :4;
      TU32 u3IndustryGroup          :3;
      TU32 u1AutoConfCapable        :1; 
   }stFields;
   TU8 tu8Data[8];                  //Byte order is inverted : Byte 1 is byte 8 of Address claim frame
}  TunName;

typedef struct
{
   TU16  u16ErrorCode;     // 16 bit error code
   TU32  u32AddInfo;       // optional additional error information
} TstWJ1939_eErrorData;

// Error Callback
typedef void (*TpfnWJ1939_eErrorClbk)(TU8 Fu8InstNum, TstWJ1939_eErrorData* FpstErrorCode );

// Structure of a message on Transport Layer
typedef struct
{
   TU8   u8Addr;           // node address (src/trg)
   TU8   u8PrioType;       // priority (1-7) or messaget type (0/1)
   TU16  u16Length;        // length of the data (in bytes)
   TU16  u16PGN;           // parameter group numbe
   TU8   u8DataPage;       // DP-bit of PGN
   TU8   u8Remote;         // handle of remote device
   TU8*  pu8Data;          // pointer to the data
   TU8   u8Dst;            // destination address
} TstWJ1939_eTPLMsgData;

// Message Callback
// Parameter : Instance Nb, Message structure
typedef void (*TpfnWJ1939_eMsgClbk)(TU8 Fu8InstNum, TstWJ1939_eTPLMsgData* FpstMsg);

// NWM Callback
// Parameter : Instance Nb, Address, Data pointer
typedef void (*TpfnWJ1939_eNWMClbk)(TU8 Fu8InstNum, TU8 Fu8AddrClaimed, TU8* Fpu8Data);

typedef struct
{
   // CAN Configuration
   TU8  u8InitCANBus;         //Configure the buadrate and set bus ON 
   TU16 u16Baudrate_kbps;     //Baudrate to be used when InitCANBus is set
   TU8  u8CtrlId;             //CAN controller Id
   TU8  u8ProtocolId;         //CAN protocol Id
   TU8  u8MBTx;               //Object for transmission
   TU8  u8MBSpRx;             //Object for point to point reception
   TU16 u16SpMask;            //Special Mask Id used for reception
   // Task Configuration
   TU8  u8TaskId;             //Task Event Id
   TU8  u8TaskInit;           //Task Init
   TU8  u8TaskPrio;           //Task Priority
   // J1939 Host Configuration
   TU8  u8DeviceAddr;         //Device Address
   TunName  unDeviceName;     //Device Name
   TpfnWJ1939_eErrorClbk   pfnErrorCallback;
   TpfnWJ1939_eMsgClbk     pfnRxCallback;
} TstWJ1939_eConfData;


//////////////////////////////////////////////////////////////////////////////
// EXPORT
//////////////////////////////////////////////////////////////////////////////
extern const TstWVersion_eSWIdent CstJ1939Lib_eSWIdent;

//////////////////////////////////////////////////////////////////////////////
// HOOK FUNCTIONS ( called by the lib)
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// SERVICE PROTOTYPES
//////////////////////////////////////////////////////////////////////////////


//FH>
//============================================================================//
// NAME>	s16WJ1939_eInit_Exe >NAME
//============================================================================//
// ROLE>	Init and start a J1939 protocol instance >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance 
// FstConf : Configuration data structure
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eInit_Exe(TU8 Fu8InstNum, TstWJ1939_eConfData * FpstConf );

//FH>
//============================================================================//
// NAME>	s16WJ1939_eRegisterAppPGN_Exe >NAME
//============================================================================//
// ROLE>	Register Rx Callback for App PGN >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance 
// Fu16PGN : PGN value
// Fu8DataPage : PGN data page
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eRegisterAppPGN_Exe( TU8 Fu8InstNum, TU16 Fu16PGN, TU8 Fu8DataPage);

//FH>
//============================================================================//
// NAME>	s16WJ1939_eRegisterReqPGN_Exe >NAME
//============================================================================//
// ROLE>	Register a message for request handling >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance
// Fu8Prio : Priority
// Fu16Length : Data length
// Fpu8Data : Data buffer (if NULL callback will be used)
// Fu16PGN : PGN Number
// Fu8DataPage : Data page
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eRegisterReqPGN_Exe( TU8 Fu8InstNum, TU8 Fu8Prio, TU16 Fu16Length,
                                    TU8 * Fpu8Data, TU16 Fu16PGN,TU8 Fu8DataPage
                                   ,TpfnWJ1939_eMsgClbk FpfnReqCallback
                                  );

//FH>
//============================================================================//
// NAME>	s16WJ1939_eSendRequest_Exe >NAME
//============================================================================//
// ROLE>	Sends a request message >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance
// Fpu8Addr : Destination Address
// Fu8DataPage : Data page
// Fu16PGN : PGN Number
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eSendRequest_Exe( TU8 Fu8InstNum, TU8 Fu8Addr, TU8 Fu8DataPage,TU16 Fu16PGN);


//FH>
//============================================================================//
// NAME>	s16WJ1939_eSendMsg_Exe >NAME
//============================================================================//
// ROLE> Send a J1939 Message >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance 
// FpstMsg : message strcuture
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eSendMsg_Exe(TU8 Fu8InstNum, TstWJ1939_eTPLMsgData * FpstMsg);


//FH>
//============================================================================//
// NAME>	s16WJ1939_eRegisterCycTxPGN_Exe >NAME
//============================================================================//
// ROLE> Register Cyclic Tx PGN >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance 
// Fpu8Data : pointer on static data
// Fu16Length : Data length
// Fu16CycleTime : Repetition rate
// Fu16Remote : Remote address ( not currently used : Set 0xFF )
// Fu16PGN :  PGN value
// Fu8DataPage : PGN data page
// Fu8Prio : PGN priotity
// Fu8Active : Activate transission
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eRegisterCycTxPGN_Exe(  TU8 Fu8InstNum,      TU8* Fpu8Data,    TU16 Fu16Length,
                                       TU16 Fu16CycleTime,  TU16 Fu16Remote,  TU16 Fu16PGN,
                                       TU8 Fu8DataPage,     TU8 Fu8Prio,      TU8 Fu8Active);


//FH>
//============================================================================//
// NAME>	s16WJ1939_eRegisterCycRxPGN_Exe >NAME
//============================================================================//
// ROLE> Register Cyclic Rx PGN >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance 
// Fpu8Data : pointer on static data
// Fu16Length : Data length
// Fu16CycleTime : Repetition rate
// Fu16Remote : Remote address ( not currently used : Set 0xFF )
// Fu16PGN :  PGN value
// Fu8DataPage : PGN data page
// Fu8Prio : PGN priotity
// Fu8Active : Activate transission
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eRegisterCycRxPGN_Exe(  TU8 Fu8InstNum,   TU8* Fpu8Data, TU16 Fu16CycleTime, 
                                       TU16 Fu16PGN,     TU8 Fu8DataPage);


//FH>
//============================================================================//
// NAME>	s16WJ1939_eClearAppPGNs_Exe >NAME
//============================================================================//
// ROLE> Clear the list with registred PGNs>ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance 
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eClearAppPGNs_Exe(TU8 Fu8InstNum);


//FH>
//============================================================================//
// NAME>	s16WJ1939_eClearCycTxPGNs_Exe >NAME
//============================================================================//
// ROLE> Clear all messages which are registred for cyclic tranmission PGNs>ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance 
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eClearCycTxPGNs_Exe(TU8 Fu8InstNum);


//FH>
//============================================================================//
// NAME>	s16WJ1939_eClearCycRxPGNs_Exe >NAME
//============================================================================//
// ROLE> Clear all messages which are registred for cyclic reception PGNs>ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance 
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eClearCycRxPGNs_Exe(TU8 Fu8InstNum);


//============================================================================//	
//FH>
//============================================================================//
// NAME>	s16WJ1939_eActivateCycPGNs_Exe >NAME
//============================================================================//
// ROLE> Activate cyclcic PGN>ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance 
// Fu8Activate : Activate value
// Fu16PGN : PGN value
// Fu8DataPage : PGN data page
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eActivateCycPGNs_Exe(TU8 Fu8InstNum,TU8 Fu8Activate, TU16 Fu16PGN, TU8 Fu8DataPage);


//FH>
//============================================================================//
// NAME>	u8WJ1939_eGetBits >NAME
//============================================================================//
// ROLE>	Get bits from 1 PGN data byte 
// no range checking >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>  
//	Fpu8Data  : pointer on PGN data
//	Fu8BytePos: position of the  byte
//	Fu8Bit 	 : position of the first bit
//	Fu8Mask 	 : Bits Mask
// >INPUT
// OUTPUT> Value >OUTPUT
//============================================================================//
//>FH
TU8 u8WJ1939_eGetBits( TU8 *Fpu8Data , TU8 Fu8BytePos , TU8 Fu8BitPos, TU8 Fu8Mask );


//FH>
//============================================================================//
// NAME>	u8WJ1939_eGetMultiBytesBits >NAME
//============================================================================//
// ROLE>	Get bits from 2 PGN data bytes 
// no range checking >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>  
//	Fpu8Data  : pointer on PGN data
//	Fu8BytePos: position of the  first byte
//	Fu8Bit 	 : position of the first bit on the first byte
//	Fu16Mask  : Bits Mask
// >INPUT
// OUTPUT> Value >OUTPUT
//============================================================================//
//>FH
TU16 u16WJ1939_eGetMultiBytesBits( TU8 *Fpu8Data , TU8 Fu8BytePos , TU8 Fu8BitPos, TU16 Fu16Mask) ;


//FH>
//============================================================================//
// NAME>	u8WJ1939_eGetU8 >NAME
//============================================================================//
// ROLE>	Get a byte from PGN data 
// no range checking >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>  
//	Fpu8Data 	: pointer on PGN data
//	Fu8StartPos : position of the first byte
// >INPUT
// OUTPUT> Value >OUTPUT
//============================================================================//
//>FH
TU8 u8WJ1939_eGetU8( TU8 *Fpu8Data  , TU8 Fu8StartPos);


//FH>
//============================================================================//
// NAME>	u16WJ1939_eGetU16 >NAME
//============================================================================//
// ROLE>	Get 16 bits word from PGN data
// no range checking >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>  
//	Fpu8Data 	: pointer on PGN data
//	Fu8StartPos : position of the first byte
// >INPUT
// OUTPUT> Value >OUTPUT
//============================================================================//
//>FH
TU16 u16WJ1939_eGetU16( TU8 *Fpu8Data  , TU8 Fu8StartPos);


//FH>
//============================================================================//
// NAME>	u3WJ1939_eGetU32 >NAME
//============================================================================//
// ROLE>	Get 32 bits word from PGN data
// no range checking >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>  
//	Fpu8Data 	: pointer on PGN data
//	Fu8StartPos : position of the first byte
// >INPUT
// OUTPUT> Value >OUTPUT
//============================================================================//
//>FH
TU32 u32WJ1939_eGetU32( TU8 *Fpu8Data  , TU8 Fu8StartPos);


//FH>
//============================================================================//
// NAME>	vWJ1939_eSetBits >NAME
//============================================================================//
// ROLE>	Set bits to 1 PGN data byte 
// no range checking >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>  
//	Fpu8Data  : pointer on PGN data
//	Fu8BytePos: position of the  byte
//	Fu8Bit 	 : position of the first bit
//	Fu8Mask 	 : Bits Mask
//	Fu8Value	 : Value
// >INPUT
// OUTPUT> Value >OUTPUT
//============================================================================//
//>FH
void vWJ1939_eSetBits( TU8 *Fpu8Data , TU8 Fu8BytePos , TU8 Fu8BitPos, TU8 Fu8Mask , TU8 Fu8Value );


//FH>
//============================================================================//
// NAME>	vWJ1939_eSetMultiBytesBits >NAME
//============================================================================//
// ROLE>	Set bits to  2 PGN data bytes
// no range checking >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>  
//	Fpu8Data  : pointer on PGN data
//	Fu8BytePos: position of the first byte
//	Fu8Bit 	 : position of the first bit on first byte
//	Fu16Mask  : Bits Mask
//	Fu16Value : Value
// >INPUT
// OUTPUT> Value >OUTPUT
//============================================================================//
//>FH
void vWJ1939_eSetMultiBytesBits( TU8 *Fpu8Data , TU8 Fu8BytePos , TU8 Fu8BitPos, TU16 Fu16Mask , TU16 Fu16Value );

//FH>
//============================================================================//
// NAME>	vWJ1939_eSetU8 >NAME
//============================================================================//
// ROLE>	Get a byte from PGN data 
// no range checking >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>  
//	Fpu8Data   : pointer on PGN data
//	Fu8StartPos: position of the first byte
//	Fu8Value	  : Value
// >INPUT
// OUTPUT> Value >OUTPUT
//============================================================================//
//>FH
void vWJ1939_eSetU8( TU8 *Fpu8Data  , TU8 Fu8StartPos , TU8 Fu8Value);

//FH>
//============================================================================//
// NAME>	vWJ1939_eSetU16 >NAME
//============================================================================//
// ROLE>	Get 16 bits word from PGN data
// no range checking >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>  
//	Fpu8Data 	: pointer on PGN data
//	Fu8StartPos : position of the first byte
//	Fu16Value	: Value
// >INPUT
// OUTPUT> Value >OUTPUT
//============================================================================//
//>FH
void vWJ1939_eSetU16( TU8 *Fpu8Data  , TU8 Fu8StartPos, TU16 Fu16Value);

//FH>
//============================================================================//
// NAME>	vWJ1939_eSetU32 >NAME
//============================================================================//
// ROLE>	Set 32 bits word to PGN data
// no range checking >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>  
//	Fpu8Data 	: pointer on PGN data
//	Fu8StartPos : position of the first byte
//	Fu32Value	: Value
// >INPUT
// OUTPUT> Value >OUTPUT
//============================================================================//
//>FH
void vWJ1939_eSetU32( TU8 *Fpu8Data  , TU8 Fu8StartPos , TU32 Fu32Value);

//FH>
//============================================================================//
// NAME>	vWJ1939_eEnableAddrFilter_Exe >NAME
//============================================================================//
// ROLE>	Enable Destination Address filter
//       for specific PGN >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
void vWJ1939_eEnableAddrFilter_Exe(TU8 Fu8InstNum);

//FH>
//============================================================================//
// NAME>	vWJ1939_eDisableAddrFilter_Exe >NAME
//============================================================================//
// ROLE>	Enable Destination Address filter
//       for specific PGN >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
void vWJ1939_eDisableAddrFilter_Exe(TU8 Fu8InstNum);

//FH>
//============================================================================//
// NAME>	s16WJ1939_eRegisterNWMCallback_Exe >NAME
//============================================================================//
// ROLE>	Register a callback when receiving an address claim message  >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance
// FpfnNWMCallback : Callback to be called when an address claim is received
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eRegisterNWMCallback_Exe(TU8 Fu8InstNum, TpfnWJ1939_eNWMClbk FpfnNWMCallback);

//FH>
//============================================================================//
// NAME>	s16WJ1939_eRegisterDevice_Exe >NAME
//============================================================================//
// ROLE>	Register a remote device in the device list
//       and returns a device handle.  >ROLE
//----------------------------------------------------------------------------//
// REQ>  >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance
// FpunName : Device Name of the node
// Fpu8Handle  : The device handle allows to address a device which is arbitrary
//               address capable.
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TS16 s16WJ1939_eRegisterDevice_Exe(TU8 Fu8InstNum, TunName * FpunName, TU8 * Fpu8Handle);

//FH>
//============================================================================//
// NAME>	u8WJ1939_eGetState_Exe >NAME
//============================================================================//
// ROLE>	Return the current state of the address claiming procedure. >ROLE
//----------------------------------------------------------------------------//
// REQ> >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fu8InstNum : Protocol Instance
// >INPUT
// OUTPUT>  >OUTPUT
//============================================================================//
//>FH
TU8 u8WJ1939_eGetState_Exe(TU8 Fu8InstNum);

//FH>
//============================================================================//
// NAME>	vWJ1939_eGetDeviceName_Exe >NAME
//============================================================================//
// ROLE>	Get the device name from received data of an address claim message >ROLE
//----------------------------------------------------------------------------//
// REQ> >REQ
//----------------------------------------------------------------------------//
// INPUT>
// Fpu8Data : Received Data
// FpunDeviceName : Pointer to buffer to be filled with device name
// >INPUT
// OUTPUT> >OUTPUT
//============================================================================//
//>FH
void vWJ1939_eGetDeviceName_Exe(TU8 * Fpu8Data, TunName * FpunDeviceName );

#endif
