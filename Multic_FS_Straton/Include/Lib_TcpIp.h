//////////////////////////////////////////////////////////////////////////////
//
//      O       OOOO  OOOOOOO   O       O         ## ## ##
//     O O     O         O      O      O O
//    O   O    O         O      O     O   O      ## ## ##
//   OOOOOOO   O         O      O    OOOOOOO
//  O       O   OOOO     O      O   O       O   ## ## ##
//
//----------------------------------------------------------------------------
// HEAD : (c) ACTIA Automotive 2009 : " Any copy and re-use of this
//        file without the written consent of ACTIA is forbidden "
//        --------------------------------------------------------------------
//        Classification :  (-)No   (-)Confident. ACTIA   (-)Confident. Client
//        --------------------------------------------------------------------
//        EthernetIf : Ethernet interface API
//////////////////////////////////////////////////////////////////////////////
// FILE : Lib_TcpIp.h (HEADER)
//----------------------------------------------------------------------------
// DESC : Ethernet interface API
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 669 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Include/Lib_TcpIp.h $
// Updated the :$Date: 2011-02-09$
// By 			:$Author: bliu $
//////////////////////////////////////////////////////////////////////////////
#ifndef ETHERNET_IFACE_INCLUDE
#define ETHERNET_IFACE_INCLUDE

#include "std_type.h"
#include "ver_type.h"

//////////////////////////////////////////////////////////////////////////////
// DEFINITION
//////////////////////////////////////////////////////////////////////////////

/*----------------------------------------------------------------------------
 * Tcp/IP buffer size options: 1,large size; 0, Medium size options
 ----------------------------------------------------------------------------*/
#ifndef SIZE_OPT
#define SIZE_OPT 1
#endif

//Ethernet controller DMA descriptors number
#if (SIZE_OPT == 1)
#define Du8WEthIf_RxDescNb 8 //This value needs to be precisely defined (how many frame can we receive while we are treating another one ?)
#define Du8WEthIf_TxDescNb 8 //4 //One buffer pending and and one buffer transmitting		//TODO::JCLi change 4->8 20150324
#else
#define Du8WEthIf_RxDescNb 4 //This value needs to be precisely defined (how many frame can we receive while we are treating another one ?)
#define Du8WEthIf_TxDescNb 8 //4//One buffer pending and and one buffer transmitting		//TODO::JCLi change 4->8 20150324
#endif

enum
{
	Ceu8WEthIf_eTXBufInd = 0,
	Ceu8WEthIf_eTXRingInd = 1,
	Ceu8WEthIf_eRXRingInd = 2
};

//Error codes
enum
{
	Ces32WEthIf_eAbort = -10,
	Ces32WEthIf_eError = -1,
	Ces32WEthIf_eSuccess = 0
};

//Link modes
enum
{	//TODO:: Modified by JCLi 20150319
  	Ces32WEthIf_eAutoNegotiation  = 0,
	Ces32WEthIf_eForce10MbpsHd = 1,
	Ces32WEthIf_eForce100MbpsHd = 2,
	Ces32WEthIf_eForce10MbpsFd = 5,
	Ces32WEthIf_eForce100MbpsFd = 6
};


//Indicates this packet's data should be immediately passed to the application
#define Du8WEthIf_ePBUF_FLAG_PUSH      0x01

//Flag for TCP protocol control block
#define Du8WEthIf_eACK_DELAY   (0x01)   // Delayed ACK.
#define Du8WEthIf_eACK_NOW     (0x02)   // Immediate ACK.
#define Du8WEthIf_eINFR        (0x04)   // In fast recovery.
#define Du8WEthIf_eTIMESTAMP   (0x08)   // Timestamp option enabled
#define Du8WEthIf_eRXCLOSED    (0x10)   // rx closed by tcp_shutdown
#define Du8WEthIf_eFIN         (0x20)   // Connection was closed locally (FIN segment enqueued)
#define Du8WEthIf_eNODELAY     (0x40)   // Disable Nagle algorithm
#define Du8WEthIf_eNAGLEMEMERR (0x80)   // nagle enabled, memerr, try to output to prevent delayed ACK to happen

// Set-up TCP send options
#define Du8WEthIf_eWRITE_COPY	0x01	//If set, the given tcp data is copied internally.
										//If not set, only the  reference is given to the stack. The
										//application must ensure the data is not modified until the tcp data is sent
#define Du8WEthIf_eWRITE_MORE	0x02	//If set, the PUSH flag is *NOT* set to the tcp segment.
										//The last tcp segment must have the PUSH flag.
#define Du8WEthIf_eWRITE_IMM	0x80	//If set, the stack will try to send the tcp packet immediately during the tcp_send call
										//If not set, the stack will wait if it can concatenate the data with other tcp segments


//////////////////////////////////////////////////////////////////////////////
// MACRO
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// TYPEDEF
//////////////////////////////////////////////////////////////////////////////
typedef struct
{
  /* Ip configuration */
  TU8 tu8IpAddr[4];
  TU8 tu8IpNetMsk[4];
  TU8 tu8IpGateway[4];

  /* MAC configuration */
  TU8 tu8MacAddr[6];
  TU8 u8LnkStatus;    /* 0: link down, 1: Link up */
  TU8 u8LnkMode;      /* 0: Half duplex, 1: Full duplex */
  TU8 u8LnkSpeed;     /* 0: 10Mbps, 1: 100Mbps */

  /* Network interface */
  TU8 u8EthIfStatus;  /* 0: if down, 1: if up */
  char* tcHostname;   /* The hostname for this network interface, NULL is a valid value */
}TstWEthIf_eStatus;

/* LwIp buffer definition */
typedef struct _stWEthIf_ePBuf
{
  /** next pbuf in singly linked pbuf chain */
  struct _stWEthIf_ePBuf *pstNext;

  /** pointer to the actual data in the buffer */
  void *pvPayload;

  /**
   * total length of this buffer and all next buffers in chain
   * belonging to the same packet.
   *
   * For non-queue packet chains this is the invariant:
   * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
   */
  TU16 u16Tot_len;

  /** length of this buffer */
  TU16 u16Len;

  /** pbuf_type as u8_t instead of enum to save space */
  TU8 u8Type;

  /** misc flags */
  TU8 u8Flags;

  /**
   * the reference count always equals the number of pointers
   * that refer to this pbuf. This can be pointers from an application,
   * the stack itself, or pbuf->next pointers from a chain.
   */
  TU16 u16Ref;
}TstWEthIf_ePBuf;

//--- TCP ---//

/* PCB handle */
typedef void* TpvWEthIf_eTCPPcbHandle;

/* Prototype of the call-back called when a connection is accepted*/
typedef TS32 (*TfnWEthIf_eTcpAccepted)( void *pvArg, TpvWEthIf_eTCPPcbHandle FpvNewPcb, void *FpvAddr, TU16 Fu16Port, TS32 Fs32Error );

/* Prototype of the call-back called when stack is connected */
typedef TS32 (*TfnWEthIf_eTcpConnected)( void *pvArg, TpvWEthIf_eTCPPcbHandle FpvNewPcb, TS32 Fs32Error );

/* Prototype of the call-back called when a packet is transmitted and acknowledged by remote side*/
typedef TS32 (*TfnWEthIf_eTcpSent)( void *pvArg, TpvWEthIf_eTCPPcbHandle FpvPcb, TU16 Fu16Len );

/* Prototype of the call-back called when a packet received*/
typedef TS32 (*TfnWEthIf_eTcpRecv)( void *pvArg, TpvWEthIf_eTCPPcbHandle FpvPcb, TstWEthIf_ePBuf *FpstRcvBuff, TS32 Fs32Error );


//--- UDP ---//

/* UDP PCB handle */
typedef void* TpvWEthIf_eUDPPcbHandle;

/* Prototype of the call-back called when a packet is received*/
typedef void (*TfnWEthIf_eUdpRecv)( void *pvArg, TpvWEthIf_eUDPPcbHandle FpvPcb, TstWEthIf_ePBuf *FpstRcvBuff, void *FpvAddr, TU16 Fu16Port );

//////////////////////////////////////////////////////////////////////////////
// EXPORT
//////////////////////////////////////////////////////////////////////////////
extern const TstWVersion_eSWIdent CstTcpIpLib_eSWIdent;

//////////////////////////////////////////////////////////////////////////////
// PROTO
//////////////////////////////////////////////////////////////////////////////

//============================================================================//
// NAME : vWEthIf_eBg_Tsk
//============================================================================//
// ROLE : Function to be called in the background task.
//        It processes the received frames and manages the periodic stack tasks and link status.
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
extern void vWEthIf_eBg_Tsk( void );

//============================================================================//
// NAME : vWEthIf_eConfig_Exe
//============================================================================//
// ROLE : This function initializes and configures the network interface.
//        Warning: Shall be called before running the background task (vWEthIf_eBg_Tsk)
//----------------------------------------------------------------------------//
// INPUT  : - u8IpAddr[] : IP address of the unit. 0 if dynamic address is used
//          - u8IpNetMsk[] : IP Net mask of the unit. 0 if dynamic address is used
//          - u8IpNetGw[] : IP default gateway of the unit. 0 if dynamic address is used
//          - pcHostName[]: DHCP Host name
//          - u8MacAddr[] : MAC address of the unit. Null if manufacturing address is used
//          - u8PhyMode : Force link speed :
//                        {
//                          Ces32WFec_eForce10MbpsHd
//                          Ces32WFec_eForce10MbpsFd
 //                         Ces32WFec_eAutoNegotiation
//                        }
// OUTPUT : Ces32WEthIf_eSuccess id everything is ok
//============================================================================//
extern TS32 s32WEthIf_eConfig_Exe( TU8 u8IpAddr[], TU8 u8IpNetMsk[], TU8 u8IpNetGw[], char pcHostName[], TU8 u8MacAddr[], TU8 u8PhyMode );

//============================================================================//
// NAME : vWEthIf_eStatus_Get
//============================================================================//
// ROLE : Get the Ethernet link status
//----------------------------------------------------------------------------//
// INPUT  : - *FpstStatus : pointer to Ethernet link status structure (allocated by caller)
// OUTPUT :
//============================================================================//
extern void vWEthIf_eStatus_Get( TstWEthIf_eStatus *FpstStatus );

//============================================================================//
// NAME : pstWEthIf_ePBufAlloc_Exe
//============================================================================//
// ROLE : Allocate a lwIP buffer (transport level).
// The memory is allocated from the tcp_ip stack heap which is low on memory.
// It is mainly to be used for UDP send.
//----------------------------------------------------------------------------//
// INPUT  : - Fu16Size : Buffer size in bytes
// OUTPUT : Return a handle to the buffer
//============================================================================//
extern TstWEthIf_ePBuf * pstWEthIf_ePBufAlloc_Exe( TU16 Fu16Size );

//============================================================================//
// NAME : pstWEthIf_ePBufAllocByRef_Exe
//============================================================================//
// ROLE : Allocate a lwIP buffer (transport level).
// The memory is *NOT* allocated.
// The application must provide a valid buffer in TstWEthIf_ePBuf->pvPayload
// It is mainly to be used for UDP send.
//----------------------------------------------------------------------------//
// INPUT  : - Fu16Size : Buffer size in bytes
// OUTPUT : Return a handle to the buffer
//============================================================================//
extern TstWEthIf_ePBuf * pstWEthIf_ePBufAllocByRef_Exe( TU16 Fu16Size );

//============================================================================//
// NAME : u8WEthIf_ePBufFree_Exe
//============================================================================//
// ROLE : Free a lwIP buffer
//----------------------------------------------------------------------------//
// INPUT  : - *FpstPBuf : LwIp buffer to free
// OUTPUT : Return the number of deallocated pbuf
//============================================================================//
extern TU8 u8WEthIf_ePBufFree_Exe( TstWEthIf_ePBuf *FpstPBuf );

//============================================================================//
// NAME : vWEthIf_ePBufCat_Exe
//============================================================================//
// ROLE : Concatenate 2 lwIP buffer
//----------------------------------------------------------------------------//
// INPUT  : - *FpstPBuf1 : Head LwIp buffer
//          - *FpstPBuf2 : Tail LwIp buffer
// OUTPUT :
//============================================================================//
extern void vWEthIf_ePBufCat_Exe( TstWEthIf_ePBuf *FpstPBuf1, TstWEthIf_ePBuf *FpstPBuf2 );

//============================================================================//
// NAME : pstWEthIf_ePBufDeChain_Exe
//============================================================================//
// ROLE : Remove the first pbuf of a pbuf chain
//----------------------------------------------------------------------------//
// INPUT  : - *FpstPBuf : Pbuf to dechain
// OUTPUT : - new pbuf reference
//============================================================================//
extern TstWEthIf_ePBuf *pstWEthIf_ePBufDeChain_Exe( TstWEthIf_ePBuf *FpstPBuf );

//============================================================================//
// NAME : vWEthIf_ePBufRef_Exe
//============================================================================//
// ROLE : Increase LwIp buffer reference
//----------------------------------------------------------------------------//
// INPUT  : - *FpstPBuf : LwIp buffer
// OUTPUT :
//============================================================================//
extern void vWEthIf_ePBufRef_Exe( TstWEthIf_ePBuf *FpstPBuf );

//============================================================================//
// NAME : s32WEthIf_eTcpListen_Exe
//============================================================================//
// ROLE : Set the listening port.
//----------------------------------------------------------------------------//
// INPUT  : - Fpu8IpAddr[] : IP address to bind, NULL is any address.
//          - Fu16Port : Port to listen.
//          - FfnTcpAccepted : Function that shall be called when a connection is accepted.
// OUTPUT : Return Ces32WEthIf_eSuccess if operation is successful.
//============================================================================//
extern TS32 s32WEthIf_eTcpListen_Exe( TU8 Fpu8IpAddr[], TU16 Fu16Port, TfnWEthIf_eTcpAccepted FfnTcpAccepted );

//============================================================================//
// NAME : s32WEthIf_eTcpConnect_Exe
//============================================================================//
// ROLE : connect to a port.
//----------------------------------------------------------------------------//
// INPUT  : - Fpu8IpAddr[] : IP address to bind.
//          - Fu16Port : Port to connect.
//          - FfnTcpConnected : Function that shall be called when stack is connected
// OUTPUT : Return Ces32WEthIf_eSuccess if operation is successful.
//============================================================================//
extern TS32 s32WEthIf_eTcpConnect_Exe( TU8 Fpu8IpAddr[], TU16 Fu16Port, TfnWEthIf_eTcpConnected FfnTcpConnected );

//============================================================================//
// NAME : vWEthIf_eTcpSetUp_Exe
//============================================================================//
// ROLE : Set the receive and sent call-back. Shall be set once the connection is established.
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb : Handle to the socket
//          - FfnTcpSent : Function that shall be called when data is sent and acknowledged by remote
//          - FfnTcpRcv : Function that shall be called when data is received.
// OUTPUT :
//============================================================================//
extern void vWEthIf_eTcpSetUp_Exe( TpvWEthIf_eTCPPcbHandle FpvPcb, TfnWEthIf_eTcpSent FfnTcpSent, TfnWEthIf_eTcpRecv FfnTcpRcv );

//============================================================================//
// NAME : s32WEthIf_eTcpSend_Exe
//============================================================================//
// ROLE : Send TCP data
// Use vWEthIf_eTcpSendOptions_Push / vWEthIf_eTcpSendOptions_Pop to set send options
// by default : options = Du8WEthIf_eWRITE_COPY|Du8WEthIf_eWRITE_IMM
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb : Handle to the PCB
//          - *FpvPayload : data to send
//          - Fu16Size : Number of bytes to send
// OUTPUT : Return Ces32WEthIf_eSuccess if operation is successful.
//============================================================================//
extern TS32 s32WEthIf_eTcpSend_Exe( TpvWEthIf_eTCPPcbHandle FpvPcb, void *FpvPayload, TU16 Fu16Size );

//============================================================================//
// NAME : vWEthIf_eTcpSendOptions_Push
//============================================================================//
// ROLE : Push some s32WEthIf_eTcpSend_Exe options (4 push max.)
// by default : options = Du8WEthIf_eWRITE_COPY|Du8WEthIf_eWRITE_IMM
//----------------------------------------------------------------------------//
// INPUT  : - Fu8Options: Options to be OR'ed
//				Du8WEthIf_eWRITE_COPY
//				Du8WEthIf_eWRITE_MORE
//				Du8WEthIf_eWRITE_IMM
// OUTPUT :
//============================================================================//
extern void vWEthIf_eTcpSendOptions_Push( TU8 Fu8Options );

//============================================================================//
// NAME : vWEthIf_eTcpSendOptions_Pop
//============================================================================//
// ROLE : Get back to the previous send options
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
extern void vWEthIf_eTcpSendOptions_Pop( void );

//============================================================================//
// NAME : u32WEthIf_eSendBufSz_Get
//============================================================================//
// ROLE : Return the amount of data that can be sent.
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb : Handle to the PCB
// OUTPUT : Number of bytes that can be sent.
//============================================================================//
extern TU32 u32WEthIf_eSendBufSz_Get( TpvWEthIf_eTCPPcbHandle FpvPcb );

//============================================================================//
// NAME : vWEthIf_eTcpRcvd_Exe
//============================================================================//
// ROLE : Notify the amount of data processed and now the stack can read more.
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb : Handle to the PCB
//          - Fu16Len : Number of bytes processed
// OUTPUT :
//============================================================================//
extern void vWEthIf_eTcpRcvd_Exe( TpvWEthIf_eTCPPcbHandle FpvPcb, TU16 Fu16Len );

//============================================================================//
// NAME : s32WEthIf_eTcpClose_Exe
//============================================================================//
// ROLE : Close the session
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb : Handle to the PCB
// OUTPUT : Return Ces32WEthIf_eSuccess if operation is successful.
//============================================================================//
extern TS32 s32WEthIf_eTcpClose_Exe( TpvWEthIf_eTCPPcbHandle FpvPcb );

//============================================================================//
// NAME : vWEthIf_eTcpAbort_Exe
//============================================================================//
// ROLE : Abort a connection (server send RST on the bus).
// The TCP receive call-back must return Ces32WEthIf_eAbort if vWEthIf_eTcpAbort_Exe is called.
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb : Handle to the PCB
// OUTPUT :
//============================================================================//
extern void vWEthIf_eTcpAbort_Exe( TpvWEthIf_eTCPPcbHandle FpvPcb );

//============================================================================//
// NAME : pstWEthIf_eUdpCreate_Exe
//============================================================================//
// ROLE : Create a UDP PCB
//----------------------------------------------------------------------------//
// INPUT  : - FfnUdpRcv : Function that shall be called when a packet is received
// OUTPUT : Return a pointer to the PCB handle
//============================================================================//
extern TpvWEthIf_eUDPPcbHandle pstWEthIf_eUdpCreate_Exe( TfnWEthIf_eUdpRecv FfnUdpRcv );

//============================================================================//
// NAME : vWEthIf_eUdpRemove_Exe
//============================================================================//
// ROLE : Remove a UDP PCB
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb : Handle to the PCB to remove
// OUTPUT :
//============================================================================//
extern void vWEthIf_eUdpRemove_Exe( TpvWEthIf_eUDPPcbHandle FpvPcb );

//============================================================================//
// NAME : s32WEthIf_eUdpBind_Exe
//============================================================================//
// ROLE : Bind a socket
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb : Handle to the PCB
//          - Fpu8IpAddr[] : IP address to bind
//          - Fu16Port : Port
// OUTPUT : Return Ces32WEthIf_eSuccess if operation is successful.
//============================================================================//
extern TS32 s32WEthIf_eUdpBind_Exe( TpvWEthIf_eUDPPcbHandle FpvPcb, TU8 Fpu8IpAddr[], TU16 Fu16Port );

//============================================================================//
// NAME : s32WEthIf_eUdpConnect_Exe
//============================================================================//
// ROLE : connect a socket
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb : Handle to the PCB
//          - Fpu8IpAddr[] : IP address to connect
//          - Fu16Port : Port
// OUTPUT : Return Ces32WEthIf_eSuccess if operation is successful.
//============================================================================//
extern TS32 s32WEthIf_eUdpConnect_Exe( TpvWEthIf_eUDPPcbHandle FpvPcb, TU8 Fpu8IpAddr[], TU16 Fu16Port );

//============================================================================//
// NAME : s32WEthIf_eUdpSendTo_Exe
//============================================================================//
// ROLE : Send data to dest
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb : Handle to the PCB
//          - *FpstPBuf : Pointer to the buffer to send
//          - Fpu8IpAddr[] : dest IP address
//          - Fu16Port : dest port
// OUTPUT : Return Ces32WEthIf_eSuccess if operation is successful.
//============================================================================//
extern TS32 s32WEthIf_eUdpSendTo_Exe( TpvWEthIf_eUDPPcbHandle FpvPcb, TstWEthIf_ePBuf *FpstPBuf, TU8 Fpu8IpAddr[], TU16 Fu16Port );

//============================================================================//
// NAME : s32WEthIf_eUdpSend_Exe
//============================================================================//
// ROLE : send data to binded dest
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb : Handle to the PCB
//          - *FpstPBuf : Pointer to the buffer to send
// OUTPUT : Return Ces32WEthIf_eSuccess if operation is successful.
//============================================================================//
extern TS32 s32WEthIf_eUdpSend_Exe( TpvWEthIf_eUDPPcbHandle FpvPcb, TstWEthIf_ePBuf *FpstPBuf );

//============================================================================//
// NAME : vWEthIf_eSetFlagPCB_Exe
//============================================================================//
// ROLE : Set flag in TCP protocol control block
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb  : Handle to the PCB
//          - Fu8Flag : Flag to put
// OUTPUT : 
//============================================================================//
extern void vWEthIf_eSetFlagPCB_Exe( TpvWEthIf_eTCPPcbHandle FpvPcb , TU8 Fu8Flag );

//============================================================================//
// NAME : vWEthIf_eResetSetFlagPCB_Exe
//============================================================================//
// ROLE : Reset flag in TCP protocol control block
//----------------------------------------------------------------------------//
// INPUT  : - FpvPcb  : Handle to the PCB
//          - Fu8Flag : Flag to remove
// OUTPUT : 
//============================================================================//
extern void vWEthIf_eResetSetFlagPCB_Exe( TpvWEthIf_eTCPPcbHandle FpvPcb , TU8 Fu8Flag );

//============================================================================//
// NAME : pu8WEthIf_eGetDescBufPnt_exe
//============================================================================//
// ROLE : get the pointer of Ethernet buffer and descriptor ring
//----------------------------------------------------------------------------//
// INPUT  : - u8index: which pointer will be taken
//			- u8Num: the number of ppu8Eth_iTxBufTable
// OUTPUT : Returns pointer or NULL
//============================================================================//
extern TU8 * pu8WEthIf_eGetDescBufPnt_exe(TU8 u8index, TU8 u8Num);

#endif //ETHERNET_IFACE_INCLUDE
