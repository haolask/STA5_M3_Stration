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
// Version     :$Revision: 890 $
// Url         :$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_MONITOR/Sources/API/monitor.h $
// Updated the :$Date: 2015-09-28 17:05:39 +0800 (周一, 28 九月 2015) $
// By          :$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
#ifndef MONITOR_INCLUDE
#define MONITOR_INCLUDE

#include "monitor_zed.h"
#include "monitor_vm.h"
#include "monitor_powerxx.h"
#include "monitor_SWP.h"
#include "monitor_IO.h"
#include "monitor_can.h"
#include "monitor_NVDM.h"
#include "monitor_core.h"
#include "Lib_TcpIp.h"
#include "Lib_J1939.h"
#include "stdint.readme"
//#include "j19_mon.h"

//----------------------------------------------------------------------------//
// "__xxxxx_start" and "__xxxxx_end" defined in GNU_DS-5-QSPI.ld
extern unsigned long __MonitorFunctions_start[];
extern unsigned long __MonitorCode_start[];
extern unsigned long __MonitorCode_end[];

////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////

//****************************************************************************//
// TYPEDEF
//****************************************************************************//
typedef void   (* TpfAPI_ActiDebug_eInit_Exe         )(unsigned char);
typedef void   (* TpfAPI_ActiDebug_eClose_Exe        )(void);
typedef int    (* TpfAPI_ActiDebug_eTXBuffer_Exe     )(unsigned char*, unsigned short);
typedef int    (* TpfAPI_ActiDebug_eGetRxBuffLen_Exe )(void);
typedef int    (* TpfAPI_ActiDebug_eReadBytes_Exe    )(unsigned char*,unsigned char);
typedef int    (* TpfAPI_ActiDebug_eTXByte_Exe       )(unsigned char*);

typedef int    (* TpfAPI_ActiDebug_eReadOneByte_Exe  )(unsigned char*);
typedef void   (* TpfAPI_ActiDebug_eConfigBaudate_Exe)(unsigned long );


//User Protocol
typedef  void TfPUSER_eEvt(short);
typedef  void TfPUSER_eEvtObj(short , short);

typedef int    (* TpfAPI_PUSER_eConfigMessage_Exe)(unsigned short FusCtrlId,unsigned char FucFrame,unsigned long FulIdent,unsigned short FusType);
typedef void   (* TpfAPI_PUSER_eConfigEvents_Exe )(unsigned short FusCtrlId,TfPUSER_eEvt *FpfError_Idc,TfPUSER_eEvtObj *FpfRx_Idc,TfPUSER_eEvtObj *FpfTx_Idc);
typedef int    (* TpfAPI_PUSER_eSendFrame_Exe    )(unsigned short FusCtrlId,unsigned char FucFrame, unsigned short FusSize,unsigned char* FpucData);
typedef int    (* TpfAPI_PUSER_eGetData_Exe      )(unsigned short FusCtrlId,unsigned char FucFrame, unsigned char* FpucData);

typedef int    (* TpfAPI_PUSER_eGetState_Exe     )(unsigned short FusCtrlId,unsigned char FucFrame);
typedef int    (* TpfAPI_PUSER_eDeleteMessage_Exe)(unsigned short FusCtrlId,unsigned char FucFrame);

typedef void*  (* TpfAPI_eMalloc_Exe             )(unsigned int); //TSizeAPI_eSize);
typedef char*  (* TpfAPI_eStrcpy_Exe             )(char*, const char*);
typedef void   (* TpfAPI_eFree_Exe               )(void*);
typedef void*  (* TpfAPI_eMemset_Exe             )(void*, int, unsigned long);
typedef int    (* TpfAPI_eSprintf_s_ld_Exe       )(char*, const char*, long);
typedef int    (* TpfAPI_eSprintf_2d_2d_2d_Exe   )(char*, unsigned int, unsigned int, unsigned int);

typedef char*  (* TpfAPI_eStrcat_Exe             )(char * , const char *);
typedef int*   (* TpfAPI_eStrcmp_Exe             )(char * , const char *);
typedef int    (* TpfAPI_eStrlen_Exe             )(char *);
typedef char*  (* TpfAPI_eMemcpy_Exe             )(char * , const char * , unsigned long);
typedef char*  (* TpfAPI_eMemmove_Exe            )(char * , const char * , unsigned long);
typedef int    (* TpfAPI_eMemcmp_Exe             )(char * , const char * , unsigned long);
typedef int    (* TpfAPI_eAtoi_Exe               )(char *);
typedef int    (* TpfAPII_eAtof_Exe              )(char *);
typedef int    (* TpfAPI_eLTOA_Exe               )(char * , const char* , long);
typedef int    (* TpfAPI_eRTOA_Exe               )(char * , const char* , long);


//FreeRTOS

typedef long BaseType_t;
typedef void * TaskHandle_t;
typedef void * QueueHandle_t;
typedef QueueHandle_t SemaphoreHandle_t;
typedef TU32 TickType_t;
typedef TickType_t portTickType;
typedef unsigned long UBaseType_t;
typedef void         (*TaskFunction_t                    )( void * );
typedef BaseType_t   (*TpfbMonitor_xTaskCreate           )(TaskFunction_t,const char * const,const uint16_t,void * const,UBaseType_t,TaskHandle_t * const);
typedef void         (*TpfvMonitor_vSemaphoreCreateBinary)(SemaphoreHandle_t);
typedef BaseType_t   (*TpfbMonitor_xSemaphoreTake        )(SemaphoreHandle_t ,TickType_t);
typedef BaseType_t   (*TpfbMonitor_xSemaphoreGive        )(SemaphoreHandle_t);
typedef TickType_t   (*TpftMonitor_xTaskGetTickCount     )(void);
typedef void         (*TpfvMonitor_vTaskDelayUntil       )(TickType_t * const , const TickType_t );

//Draw_drv
/* Color Depth */
enum
{
   Ces32WMON_eColIndex8 = 8,
   Ces32WMON_eCol16Bits = 16,
   Ces32WMON_eCol32Bits = 32
};
enum
{
   Ces32WMON_eHorizontal = 0,
   Ces32WMON_eVertical = 1
};
/* Define the layers */
enum
{
   Ces32WMON_eLayer0 = 0,
   Ces32WMON_eLayer1,
   Ces32WMON_eLayer2,
   Ces32WMON_eLayer3,
   Ces32WMON_eNoLayer = -1
};
typedef struct
{
   TS32 s32X;  /* X-axis position */
   TS32 s32Y;  /* Y-axis position */
}DstWMmi_ePoint;
typedef struct
{
   DstWMmi_ePoint stOrig;  /* Top-left rectangle coordinates */
   TU16 u16HSize;          /* Horizontal size */
   TU16 u16VSize;          /* Vertical size */
}DstWMmi_eRect;
typedef struct
{
   void *pvSurface;           /* Pointer to the surface */
   TU16 u16ScrHSize;          /* Surface Horizontal size in pixel */
   TU16 u16ScrVSize;          /* Surface Vertical size in pixel*/
   TU16 u16HStride;           /* Surface Effective Horizontal size in pixel with alignment (can be greater than H size) */
   TU16 u16ScrColDepth;       /* Screen Color depth in bits (16 or 32)*/
   TU32 u32Size;              /* Surface size in bytes */
   void *pvMemPointer;           /* Pointer to the memory (for internal purpose) */
}DstWMmi_eSurface;
typedef struct
{
   DstWMmi_eSurface stSurface;      /* Frame buffer surface information */
   TU16 u16LayerValue;           /* Layer value where the higher value corresponds to the upper layer.
                                       0 is background layer and 0xffff means the frame buffer has no layer affected */
   TU16 u16ScrOrient;            /* Screen orientation : 1: Vertical | 0: Horizontal */
   DstWMmi_eRect stFBScrArea; /* Area to draw the Frame Buffer on the screen */
}DstWMmi_eFBuff;


typedef DstWMmi_eFBuff* (* TpfpstWMonitor_eMmiFBAlloc_Exe)  (TU16,TU16,TU16,TU16);
typedef void  (* TpfvWMonitor_eMmiFBFree_Exe  )(DstWMmi_eFBuff * );
typedef void  (* TpfvWMonitor_eMmiFBScreen_Set)(DstWMmi_eFBuff * ,DstWMmi_eRect *);
typedef void  (* TpfvWMonitor_eMmiCLUT_Set    )(TU16 , void* );
typedef void  (* TpfvWMonitor_eMmiFBLayer_Set )(DstWMmi_eFBuff *, TU16 , TU8 );
typedef void  (* TpfvWMonitor_eMmiFBWrite_Exe )(DstWMmi_eFBuff *, void *, TU16 , TU16 , TU16 ,TU16 , TU16 , void * );
typedef void  (* TpfvWMonitor_eMmiFBLayer_Swap)(DstWMmi_eFBuff *, DstWMmi_eFBuff * );
typedef void  (* TpfvWMonitor_eStopLayer_Exe)( TU16 Fu16Layer );
typedef void  (* TpfvWMonitor_eSurfaceAlloc_Exe)( DstWMmi_eSurface *FpstSurface, TU16 Fu16HSize, TU16 Fu16VSize, TU16 Fu16ColDepth );
typedef void  (* TpfvWMonitor_eSurfaceFree_Exe)( DstWMmi_eSurface *FpstSurface );
typedef void  (* TpfvWMonitor_eVSync_Wait)( void );

//Vid_drv
enum
{
	Du8WMON_eVid_Channel1 = 0,
	Du8WMON_eVid_Channel2,
	Du8WMON_eVid_Channel3,
	Du8WMON_eVid_Channel4
};

typedef struct
{
	// Select  needed Video position
	TU16	u16XPos;		// Horizontal position
	TU16	u16YPos;		// Vertical position

	// Select needed Video size
	TU16	u16XResol;		// Horizontal resolution
	TU16	u16YResol;		// Vertical resolution

	// Set display position on LCD
	TU16	u16DisplayXPos;
	TU16	u16DisplayYPos;
	// Set display size on LCD
	TU16	u16DisplayXWidth;
	TU16	u16DisplayYWidth;
	// Image characteristics
	TU16	u16Brightness;		// Brightness, value 0..255
	TU16	u16Contrast;		// Contrast, value 0..255
	TU16	u16Saturation;		// Saturation, value 0..255
	TS16	s16Hue;

}DstWMmi_eVidConfig;

typedef TS32 (*Tpfs32WMonitor_eVidInit_Exe)( void );
typedef TS32 (*Tpfs32WMonitor_eVidDetect_Exe)( TU8 * Fpu8DetectedVideo );
typedef TS32 (*Tpfs32WMonitor_eVidDisplay_Exe)( TU8 Fu8VidId, TU8 Fu8FrameBufferWrMode , DstWMmi_eVidConfig * FpstVidConfig , DstWMmi_eFBuff * FpstFrameBuff);
typedef TS32 (*Tpfs32WMonitor_eVideoBlend_Exe)( TU8 Fu8State);
typedef void (*TpfvWMonitor_eGraphicsAlphaBlending)(TU16 Fu16Layer, TU16 Fu16HSize, TU16 Fu16VSize, TU16 Fu16XPos, TU16 Fu16YPos, TU8 Fu8OnOff);
typedef TS32 (*Tpfs32WMonitor_eVidErase_Exe)( TU8 Fu8VidId);


//MMI - SND:
typedef TS32  (* Tpfs32WMonitor_eMmiCheckSndPresent_Exe )( void );
typedef void  (* TpfvWMonitor_eMmiSoundStop_Exe         )( void );
typedef TS32  (* Tpfs32WMonitor_eMmiSoundStatus_Get     )( void );
typedef TS32  (* Tpfs32WMonitor_eMmiSoundVolume_Set     )( TU8 Fu8Volume );
typedef TS32  (* Tpfs32WMonitor_eMmiSoundVolume_Get     )( TU8 * Fpu8Volume );
typedef TS32  (* Tpfs32WMonitor_eMmiPlayBeep_Exe        )( TU16 Fu16BeepFreq, TU32 Fu32Duration );


//MON - APP:
typedef TS32  (* Tpfs32WMonitor_eEnterIntoSleepMode_Req )( void );
typedef TS32  (* Tpfs32WMonitor_eParameterData_Set      )( TU16 Fu16ID, TU32 *Fpu32Data );
typedef TS32  (* Tpfs32WMonitor_eParameterData_Get      )( TU16 Fu16ID, TU32 *Fpu32Data );
typedef int   (* TpfAPI_MIO_eSetOutput_Exe      )(unsigned char, unsigned char);
typedef int   (* TpfAPI_MFpga_ePushCmd_Exe      )(void*);


//TCP_IP stack access
typedef TstWEthIf_ePBuf *        (* TpfAPI_WEthIf_ePBufAlloc_Exe      )(TU16 Fu16Size );
typedef TU8                      (* TpfAPI_WEthIf_ePBufFree_Exe       )(TstWEthIf_ePBuf *FpstPBuf );
typedef void                     (* TpfAPI_WEthIf_ePBufCat_Exe        )(TstWEthIf_ePBuf *FpstPBuf1, TstWEthIf_ePBuf *FpstPBuf2 );
typedef void                     (* TpfAPI_WEthIf_ePBufRef_Exe        )(TstWEthIf_ePBuf *FpstPBuf );
typedef TS32                     (* TpfAPI_WEthIf_eTcpListen_Exe      )(TU8 Fpu8IpAddr[], TU16 Fu16Port, TfnWEthIf_eTcpAccepted FfnTcpAccepted );
typedef TS32                     (* TpfAPI_WEthIf_eTcpConnect_Exe     )(TU8 Fpu8IpAddr[], TU16 Fu16Port, TfnWEthIf_eTcpConnected FfnTcpConnected );
typedef void                     (* TpfAPI_WEthIf_eTcpSetUp_Exe       )(TpvWEthIf_eTCPPcbHandle FpvPcb, TfnWEthIf_eTcpSent FfnTcpSent, TfnWEthIf_eTcpRecv FfnTcpRcv );
typedef TS32                     (* TpfAPI_WEthIf_eTcpSend_Ex         )(TpvWEthIf_eTCPPcbHandle FpvPcb, void *FpvPayload, TU16 Fu16Size );
typedef TU32                     (* TpfAPI_WEthIf_eSendBufSz_Get      )(TpvWEthIf_eTCPPcbHandle FpvPcb );
typedef void                     (* TpfAPI_WEthIf_eTcpRcvd_Exe        )(TpvWEthIf_eTCPPcbHandle FpvPcb, TU16 Fu16Len );
typedef TS32                     (* TpfAPI_WEthIf_eTcpClose_Exe       )(TpvWEthIf_eTCPPcbHandle FpvPcb );
typedef TpvWEthIf_eUDPPcbHandle  (* TpfAPI_WEthIf_eUdpCreate_Exe      )(TfnWEthIf_eUdpRecv FfnUdpRcv );
typedef void                     (* TpfAPI_WEthIf_eUdpRemove_Exe      )(TpvWEthIf_eUDPPcbHandle FpvPcb );
typedef TS32                     (* TpfAPI_WEthIf_eUdpBind_Exe        )(TpvWEthIf_eUDPPcbHandle FpvPcb, TU8 Fpu8IpAddr[], TU16 Fu16Port );
typedef TS32                     (* TpfAPI_WEthIf_eUdpConnect_Exe     )(TpvWEthIf_eUDPPcbHandle FpvPcb, TU8 Fpu8IpAddr[], TU16 Fu16Port );
typedef TS32                     (* TpfAPI_WEthIf_eUdpSendTo_Exe      )(TpvWEthIf_eUDPPcbHandle FpvPcb, TstWEthIf_ePBuf *FpstPBuf, TU8 Fpu8IpAddr[], TU16 Fu16Port );
typedef TS32                     (* TpfAPI_WEthIf_eUdpSend_Exe        )(TpvWEthIf_eUDPPcbHandle FpvPcb, TstWEthIf_ePBuf *FpstPBuf );
typedef void                     (* TpfAPI_WEthIf_eSetFlagPCB_Exe     )(TpvWEthIf_eTCPPcbHandle FpvPcb , TU8 Fu8Flag );
typedef void                     (* TpfAPI_WEthIf_eResetSetFlagPCB_Exe)(TpvWEthIf_eTCPPcbHandle FpvPcb , TU8 Fu8Flag );


//DTC
typedef int             (* TpfAPI_MNVDM_eDTC_Store       )(unsigned short FusDTC_Code, unsigned char FucStatus, unsigned  char *FpucDTC_ContextPtr);
typedef void            (* TpfAPI_MNVDM_eDTC_EraseAll    )(void);
typedef unsigned char   (* TpfAPI_MNVDM_eDTC_GetCodesList)(unsigned  short *FpusDTC_CodesListPtr);
typedef unsigned char   (* TpfAPI_MNVDM_eDTC_GetContext  )(unsigned short FusDTC_Code, unsigned char *FpucDTC_Status, unsigned  char *FpucDTC_ContextPtr);


//LIN
typedef int (*TpfAPI_MLin_eInit_Exe          )(unsigned short Fu16Baudrate);
typedef int (*TpfAPI_MLin_eTransmitFrame_Exe )(unsigned char Fu8FrameDID, unsigned char Fu8DataSize , unsigned char *Fpu8Data, unsigned short Fu16LinVersion);
typedef int (*TpfAPI_MLin_eReadRequest_Exe   )(unsigned char Fu8FrameDID, unsigned char Fu8DataSize ,  unsigned short Fu16LinVersion);
typedef int (*TpfAPI_MLin_eReadFrame_Exe     )(unsigned char Fu8FrameDID, unsigned char *Fpu8FrameSize ,  unsigned char *Fpu8Data);
typedef TS32	 (* TpfAPI_MLin_eGetStatus_Exe     )(TU8 Fu8FrameDID);


//RTC
typedef int    (* TpfAPI_ARtc_eWrite_Exe)(TstARtc_eRTCStruct *FpstRTCTimeStruct);
typedef int    (* TpfAPI_ARtc_eRead_Exe )(TstARtc_eRTCStruct *FpstRTCTimeStruct);
typedef int    (* TpfAPI_ARtc_eAlarm_Set)(TstARtc_eRTCAlarm *FpstRTCAlarmStruct);
typedef int    (* TpfAPI_ARtc_eAlarm_Get)(TstARtc_eRTCAlarm *FpstRTCAlarmStruct);
typedef int    (* TpfAPI_ARtc_eTimer_Set)(TstARtc_eRTCTimer *FpstRTCTimerStruct);
typedef int    (* TpfAPI_ARtc_eTimer_Get)(TstARtc_eRTCTimer *FpstRTCTimerStruct);
typedef int    (* TpfAPI_ARtc_eReadTimestamp_Exe)(TU32* Fpu32Time);


//Core
typedef TU32   (* TpfAPI_Core_eExecutionTime_Exe)(void);
typedef void   (* TpfvWMonitor_v7_dma_map_area  )(uint32_t , uint32_t , uint32_t);


//J1939 STACK
typedef TS32   (* TpfAPI_eInitInstance_Exe       )(TU8 Fu8J1939Channel, TU8 Fu8DeviceAddr, TpfnWJ1939_eErrorClbk FpfnErrorCallback, TpfnWJ1939_eMsgClbk FpfnRxCallback, TpfnWJ1939_eNWMClbk FpfnNWMCallback);
typedef TS16   (* TpfAPI_eRegisterAppPGN_Exe     )(TU8 Fu8InstNum, TU16 Fu16PGN, TU8 Fu8DataPage);
typedef TS16   (* TpfAPI_eRegisterReqPGN_Exe     )(TU8 Fu8InstNum, TU8 Fu8Prio, TU16 Fu16Length, TU8 * Fpu8Data, TU16 Fu16PGN, TU8 Fu8DataPage, TpfnWJ1939_eMsgClbk FpfnReqCallback);
typedef TS16   (* TpfAPI_eSendRequest_Exe        )(TU8 Fu8InstNum, TU8 Fu8Addr, TU8 Fu8DataPage, TU16 Fu16PGN);
typedef TS16   (* TpfAPI_eSendMsg_Exe            )(TU8 Fu8InstNum, TstWJ1939_eTPLMsgData * FpstMsg);
typedef TS16   (* TpfAPI_eRegisterCycTxPGN_Exe   )(TU8 Fu8InstNum, TU8* Fpu8Data, TU16 Fu16Length, TU16 Fu16CycleTime, TU16 Fu16Remote, TU16 Fu16PGN, TU8 Fu8DataPage, TU8 Fu8Prio, TU8 Fu8Active);
typedef TS16   (* TpfAPI_eRegisterCycRxPGN_Exe   )(TU8 Fu8InstNum, TU8* Fpu8Data, TU16 Fu16CycleTime, TU16 Fu16PGN, TU8 Fu8DataPage);
typedef TS16   (* TpfAPI_eClearAppPGNs_Exe       )(TU8 Fu8InstNum);
typedef TS16   (* TpfAPI_eClearCycTxPGNs_Exe     )(TU8 Fu8InstNum);
typedef TS16   (* TpfAPI_eClearCycRxPGNs_Exe     )(TU8 Fu8InstNum);
typedef TS16   (* TpfAPI_eActivateCycPGNs_Exe    )(TU8 Fu8InstNum,TU8 Fu8Activate, TU16 Fu16PGN, TU8 Fu8DataPage);
typedef TU8    (* TpfAPI_eGetBits                )(TU8 *Fpu8Data, TU8 Fu8BytePos, TU8 Fu8BitPos, TU8 Fu8Mask );
typedef TU16   (* TpfAPI_eGetMultiBytesBits      )(TU8 *Fpu8Data, TU8 Fu8BytePos, TU8 Fu8BitPos, TU16 Fu16Mask) ;
typedef TU8    (* TpfAPI_eGetU8                  )(TU8 *Fpu8Data, TU8 Fu8StartPos);
typedef TU16   (* TpfAPI_eGetU16                 )(TU8 *Fpu8Data, TU8 Fu8StartPos);
typedef TU32   (* TpfAPI_eGetU32                 )(TU8 *Fpu8Data, TU8 Fu8StartPos);
typedef void   (* TpfAPI_eSetBits                )(TU8 *Fpu8Data, TU8 Fu8BytePos, TU8 Fu8BitPos, TU8 Fu8Mask, TU8 Fu8Value );
typedef void   (* TpfAPI_eSetMultiBytesBits      )(TU8 *Fpu8Data, TU8 Fu8BytePos, TU8 Fu8BitPos, TU16 Fu16Mask, TU16 Fu16Value );
typedef void   (* TpfAPI_eSetU8                  )(TU8 *Fpu8Data, TU8 Fu8StartPos, TU8 Fu8Value);
typedef void   (* TpfAPI_eSetU16                 )(TU8 *Fpu8Data, TU8 Fu8StartPos, TU16 Fu16Value);
typedef void   (* TpfAPI_eSetU32                 )(TU8 *Fpu8Data, TU8 Fu8StartPos, TU32 Fu32Value);
typedef void   (* TpfAPI_eEnableAddrFilter_Exe   )(TU8 Fu8InstNum);
typedef void   (* TpfAPI_eDisableAddrFilter_Exe  )(TU8 Fu8InstNum);
typedef TS16   (* TpfAPI_eRegisterNWMCallback_Exe)(TU8 Fu8InstNum, TpfnWJ1939_eNWMClbk FpfnNWMCallback);
typedef TS16   (* TpfAPI_eRegisterDevice_Exe     )(TU8 Fu8InstNum, TunName * FpunName, TU8 * Fpu8Handle);
typedef TU8    (* TpfAPI_eGetState_Exe           )(TU8 Fu8InstNum);
typedef void   (* TpfAPI_eGetDeviceName_Exe      )(TU8 * Fpu8Data, TunName * FpunDeviceName );

typedef void   (* TpfAPI_eGetFBWriteDmaFlag_Exe    )( TU8* Fpu8Flag);

typedef TU32   (* TpfAPI_eJ1939_eRxId_Set   )( TU8 Fu8J1939Channel, TU32 Fu32J1939RX_MSG_ID, TU8 Fu8Mask_SA );
//typedef TU32   (* TpfAPI_eJ1939_eRxId_Set   )( TU8 Fu8J1939Channel, TU32 Fu32J1939RX_MSG_ID, TU8 Fu8Mask_SA );
typedef void   (* TpfAPI_eCommonLog_Exe  )(unsigned long level, const char* fmt, ...);
typedef void   (* TpfAPI_eCommonLogData_Exe  )(unsigned long level, const char* prefix, unsigned char* data, int size);


//standard and math library
//typedef float   (* TpffWMonitor_MathCosf)     (float);
//typedef float   (* TpffWMonitor_MathSinf)     (float);
//typedef float   (* TpffWMonitor_MathAcosf)    (float);
//typedef float   (* TpffWMonitor_MathAsinf)    (float);
//typedef float   (* TpffWMonitor_MathSqrtf)    (float);
//typedef float   (* TpffWMonitor_MathPowf)     (float, float);
//typedef int      (* TpfiWMonitor_StdRand)     (void);
//typedef void    (* TpfvWMonitor_StdSrand)     (unsigned int);
//typedef void    (* TpfvWMonitor_StdVa_start)  (va_list, paramN);
//typedef void    (* TpfvWMonitor_StdVa_end)    (va_list);
//fortest 


// Spare
typedef void (* TpfAPI_eSpare_Exe)(void);


//Monitor_eApplication_Function structure type
typedef struct                                                                      // index offset
{                                                                                                 
   TpfAPI_ActiDebug_eInit_Exe                   pvPActiDebug_eInit_Exe;             // 0     0x000
   TpfAPI_ActiDebug_eClose_Exe                  pvPActiDebug_eClose_Exe;            // 1     0x004
   TpfAPI_ActiDebug_eTXBuffer_Exe               piPActiDebug_eTXBuffer_Exe;         // 2     0x008
   TpfAPI_ActiDebug_eGetRxBuffLen_Exe           piPActiDebug_eGetRxBuffLen_Exe;     // 3     0x00C
   TpfAPI_ActiDebug_eReadBytes_Exe              piPActiDebug_eReadBytes_Exe;        // 4     0x010
   TpfAPI_ActiDebug_eTXByte_Exe                 piPActiDebug_eTXByte_Exe;           // 5     0x014
   TpfAPI_ActiDebug_eConfigBaudate_Exe          pvPActiDebug_eConfigBaudate_Exe;    // 6     0x018
   TpfAPI_ActiDebug_eReadOneByte_Exe            piPActiDebug_eReadOneByte_Exe;      // 7     0x01C
   TpfAPI_eMalloc_Exe                           ppvAPI_eMalloc_Exe;                 // 8     0x020
   TpfAPI_eStrcpy_Exe                           ppcAPI_eStrcpy_Exe;                 // 9     0x024
   TpfAPI_eFree_Exe                             pvAPI_eFree_Exe;                    // 10    0x028
   TpfAPI_eMemset_Exe                           ppvAPI_eMemset_Exe;                 // 11    0x02C
   TpfAPI_eSprintf_s_ld_Exe                     piAPI_eSprintf_s_ld_Exe;            // 12    0x030
   TpfAPI_eSprintf_2d_2d_2d_Exe                 piAPI_eSprintf_2d_2d_2d_Exe;        // 13    0x034
   TpfAPI_eStrcat_Exe                           ppcAPI_eStrcat_Exe ;                // 14    0x038
   TpfAPI_eStrcmp_Exe                           ppiAPI_eStrcmp_Exe;                 // 15    0x03C
   TpfAPI_eStrlen_Exe                           ppiAPI_eStrlen_Exe;                 // 16    0x040
   TpfAPI_eMemcpy_Exe                           ppcAPI_eMemcpy_Exe;                 // 17    0x044
   TpfAPI_eMemmove_Exe                          ppcAPI_eMemmove_Exe;                // 18    0x048
   TpfAPI_eMemcmp_Exe                           ppiAPI_eMemcmp_Exe;                 // 19    0x04C
   TpfAPI_eAtoi_Exe                             ppiAPI_eAtoi_Exe ;                  // 20    0x050
   TpfAPII_eAtof_Exe                            ppiAPI_eAtof_Exe;                   // 21    0x054
   TpfAPI_eLTOA_Exe                             ppiAPI_eLTOA_Exe ;                  // 22    0x058
   TpfAPI_eRTOA_Exe                             ppiAPI_eRTOA_Exe ;                  // 23    0x05C
   TpfAPI_MIO_eSetOutput_Exe                    piMIO_eAppSetOutput_Exe;            // 24    0x060
   TpfAPI_MFpga_ePushCmd_Exe                    piMFpga_ePushCmd_Exe;               // 25    0x064
   TpfAPI_PUSER_eConfigMessage_Exe              piPUSER_eConfigMessage_Exe;         // 26    0x068
   TpfAPI_PUSER_eConfigEvents_Exe               piPUSER_eConfigEvents_Exe;          // 27    0x06C
   TpfAPI_PUSER_eSendFrame_Exe                  piPUSER_eSendFrame_Exe;             // 28    0x070
   TpfAPI_PUSER_eGetData_Exe                    piPUSER_eGetData_Exe;               // 29    0x074
   TpfAPI_PUSER_eGetState_Exe                   piPUSER_eGetState_Exe;              // 30    0x078
   TpfAPI_PUSER_eDeleteMessage_Exe              piPUSER_eDeleteMessage_Exe;         // 31    0x07C
   Tpfs32WMonitor_eEnterIntoSleepMode_Req       s32WMonitor_eEnterIntoSleepMode_Req;// 32    0x080
   TpfAPI_WEthIf_ePBufAlloc_Exe                 ppstWEthIf_ePBufAlloc_Exe;          // 33    0x084
   TpfAPI_WEthIf_ePBufFree_Exe                  pu8WEthIf_ePBufFree_Exe;            // 34    0x088
   TpfAPI_WEthIf_ePBufCat_Exe                   pvWEthIf_ePBufCat_Exe;              // 35    0x08C
   TpfAPI_WEthIf_ePBufRef_Exe                   pvWEthIf_ePBufRef_Exe;              // 36    0x090
   TpfAPI_WEthIf_eTcpListen_Exe                 ps32WEthIf_eTcpListen_Exe;          // 37    0x094
   TpfAPI_WEthIf_eTcpConnect_Exe                ps32WEthIf_eTcpConnect_Exe;         // 38    0x098
   TpfAPI_WEthIf_eTcpSetUp_Exe                  pvWEthIf_eTcpSetUp_Exe;             // 39    0x09C
   TpfAPI_WEthIf_eTcpSend_Ex                    ps32WEthIf_eTcpSend_Exe;            // 40    0x0A0
   TpfAPI_WEthIf_eSendBufSz_Get                 pu32WEthIf_eSendBufSz_Get;          // 41    0x0A4
   TpfAPI_WEthIf_eTcpRcvd_Exe                   pvWEthIf_eTcpRcvd_Exe;              // 42    0x0A8
   TpfAPI_WEthIf_eTcpClose_Exe                  ps32WEthIf_eTcpClose_Exe;           // 43    0x0AC
   TpfAPI_WEthIf_eUdpCreate_Exe                 ppstWEthIf_eUdpCreate_Exe;          // 44    0x0B0
   TpfAPI_WEthIf_eUdpRemove_Exe                 pvWEthIf_eUdpRemove_Exe;            // 45    0x0B4
   TpfAPI_WEthIf_eUdpBind_Exe                   ps32WEthIf_eUdpBind_Exe;            // 46    0x0B8
   TpfAPI_WEthIf_eUdpConnect_Exe                ps32WEthIf_eUdpConnect_Exe;         // 47    0x0BC
   TpfAPI_WEthIf_eUdpSendTo_Exe                 ps32WEthIf_eUdpSendTo_Exe;          // 48    0x0C0
   TpfAPI_WEthIf_eUdpSend_Exe                   ps32WEthIf_eUdpSend_Exe;            // 49    0x0C4
   TpfAPI_WEthIf_eSetFlagPCB_Exe                pvWEthIf_eSetFlagPCB_Exe;           // 50    0x0C8
   TpfAPI_WEthIf_eResetSetFlagPCB_Exe           pvWEthIf_eResetSetFlagPCB_Exe;      // 51    0x0CC
   TpfAPI_MNVDM_eDTC_Store                      piMNVDM_eDTC_Store;                 // 52    0x0D0
   TpfAPI_MNVDM_eDTC_EraseAll                   pvMNVDM_eDTC_EraseAll;              // 53    0x0D4
   TpfAPI_MNVDM_eDTC_GetCodesList               pucMNVDM_eDTC_GetCodesList;         // 54    0x0D8
   TpfAPI_MNVDM_eDTC_GetContext                 pucMNVDM_eDTC_GetContext;           // 55    0x0DC
   TpfAPI_MLin_eInit_Exe                        s32MLin_eInit_Exe;                  // 56    0x0E0
   TpfAPI_MLin_eTransmitFrame_Exe               s32MLin_eTransmitFrame_Exe;         // 57    0x0E4
   TpfAPI_MLin_eReadRequest_Exe                 s32MLin_eReadRequest_Exe;           // 58    0x0E8
   TpfAPI_MLin_eReadFrame_Exe                   s32MLin_eReadFrame_Exe;             // 59    0x0EC
   TpfAPI_ARtc_eWrite_Exe                       ps32ARtc_eWrite_Exe;                // 60    0x0F0
   TpfAPI_ARtc_eRead_Exe                        ps32ARtc_eRead_Exe;                 // 61    0x0F4
   TpfAPI_ARtc_eAlarm_Set                       ps32ARtc_eAlarm_Set;                // 62    0x0F8
   TpfAPI_ARtc_eAlarm_Get                       ps32ARtc_eAlarm_Get;                // 63    0x0FC
   TpfAPI_Core_eExecutionTime_Exe               pu32Core_eExecutionTime_Exe;        // 64    0x100
   TpfbMonitor_xTaskCreate                      bMonitor_xTaskCreate;               // 65    0x104
   TpfvMonitor_vSemaphoreCreateBinary           vMonitor_vSemaphoreCreateBinary;    // 66    0x108
   TpfbMonitor_xSemaphoreTake                   bMonitor_xSemaphoreTake;            // 67    0x10C
   TpfbMonitor_xSemaphoreGive                   bMonitor_xSemaphoreGive;            // 68    0x110
   TpftMonitor_xTaskGetTickCount                tMonitor_xTaskGetTickCount;         // 69    0x114
   TpfvMonitor_vTaskDelayUntil                  vMonitor_vTaskDelayUntil;           // 70    0x118
   TpfpstWMonitor_eMmiFBAlloc_Exe               pstWMonitor_eMmiFBAlloc_Exe;        // 71    0x11C
   TpfvWMonitor_eMmiFBFree_Exe                  vWMonitor_eMmiFBFree_Exe;           // 72    0x120
   TpfvWMonitor_eMmiFBScreen_Set                vWMonitor_eMmiFBScreen_Set;         // 73    0x124
   TpfvWMonitor_eMmiCLUT_Set                    vWMonitor_eMmiCLUT_Set;             // 74    0x128
   TpfvWMonitor_eMmiFBLayer_Set                 vWMonitor_eMmiFBLayer_Set;          // 75    0x12C
   TpfvWMonitor_eMmiFBWrite_Exe                 vWMonitor_eMmiFBWrite_Exe;          // 76    0x130
   TpfvWMonitor_eMmiFBLayer_Swap                vWMonitor_eMmiFBLayer_Swap;         // 77    0x134
   TpfvWMonitor_v7_dma_map_area                 vWMonitor_v7_dma_map_area;          // 78    0x138
   TpfAPI_eInitInstance_Exe                     s32WJ1939_eInitInstance_Exe;        // 79    0x13C
   TpfAPI_eRegisterAppPGN_Exe                   s16WJ1939_eRegisterAppPGN_Exe;      // 80    0x140
   TpfAPI_eRegisterReqPGN_Exe                   s16WJ1939_eRegisterReqPGN_Exe;      // 81    0x144
   TpfAPI_eSendRequest_Exe                      s16WJ1939_eSendRequest_Exe;         // 82    0x148
   TpfAPI_eSendMsg_Exe                          s16WJ1939_eSendMsg_Exe;             // 83    0x14C
   TpfAPI_eRegisterCycTxPGN_Exe                 s16WJ1939_eRegisterCycTxPGN_Exe;    // 84    0x150
   TpfAPI_eRegisterCycRxPGN_Exe                 s16WJ1939_eRegisterCycRxPGN_Exe;    // 85    0x154
   TpfAPI_eClearAppPGNs_Exe                     s16WJ1939_eClearAppPGNs_Exe;        // 86    0x158
   TpfAPI_eClearCycTxPGNs_Exe                   s16WJ1939_eClearCycTxPGNs_Exe;      // 87    0x15C
   TpfAPI_eClearCycRxPGNs_Exe                   s16WJ1939_eClearCycRxPGNs_Exe;      // 88    0x160
   TpfAPI_eActivateCycPGNs_Exe                  s16WJ1939_eActivateCycPGNs_Exe;     // 89    0x164
   TpfAPI_eGetBits                              u8WJ1939_eGetBits;                  // 90    0x168
   TpfAPI_eGetMultiBytesBits                    u16WJ1939_eGetMultiBytesBits;       // 91    0x16C
   TpfAPI_eGetU8                                u8WJ1939_eGetU8;                    // 92    0x170
   TpfAPI_eGetU16                               u16WJ1939_eGetU16;                  // 93    0x174
   TpfAPI_eGetU32                               u32WJ1939_eGetU32;                  // 94    0x178
   TpfAPI_eSetBits                              vWJ1939_eSetBits;                   // 95    0x17C
   TpfAPI_eSetMultiBytesBits                    vWJ1939_eSetMultiBytesBits;         // 96    0x180
   TpfAPI_eSetU8                                vWJ1939_eSetU8;                     // 97    0x184
   TpfAPI_eSetU16                               vWJ1939_eSetU16;                    // 98    0x188
   TpfAPI_eSetU32                               vWJ1939_eSetU32;                    // 99    0x18C
   TpfAPI_eEnableAddrFilter_Exe                 vWJ1939_eEnableAddrFilter_Exe;      // 100   0x190
   TpfAPI_eDisableAddrFilter_Exe                vWJ1939_eDisableAddrFilter_Exe;     // 101   0x194
   TpfAPI_eRegisterNWMCallback_Exe              s16WJ1939_eRegisterNWMCallback_Exe; // 102   0x198
   TpfAPI_eRegisterDevice_Exe                   s16WJ1939_eRegisterDevice_Exe;      // 103   0x19C
   TpfAPI_eGetState_Exe                         u8WJ1939_eGetState_Exe;             // 104   0x1A0
   TpfAPI_eGetDeviceName_Exe                    vWJ1939_eGetDeviceName_Exe;         // 105   0x1A4
   Tpfs32WMonitor_eMmiCheckSndPresent_Exe       s32WMonitor_eMmiCheckSndPresent_Exe;// 106   0x1A8
   TpfvWMonitor_eMmiSoundStop_Exe               vWMonitor_eMmiSoundStop_Exe;        // 107   0x1AC
   Tpfs32WMonitor_eMmiSoundStatus_Get           s32WMonitor_eMmiSoundStatus_Get;    // 108   0x1B0
   Tpfs32WMonitor_eMmiSoundVolume_Set           s32WMonitor_eMmiSoundVolume_Set;    // 109   0x1B4
   Tpfs32WMonitor_eMmiSoundVolume_Get           s32WMonitor_eMmiSoundVolume_Get;    // 110   0x1B8
   Tpfs32WMonitor_eMmiPlayBeep_Exe              s32WMonitor_eMmiPlayBeep_Exe;       // 111   0x1BC
   Tpfs32WMonitor_eVidInit_Exe                  s32WMonitor_eVidInit_Exe;           // 112   0x1C0
   Tpfs32WMonitor_eVidDetect_Exe                s32WMonitor_eVidDetect_Exe;         // 113   0x1C4
   Tpfs32WMonitor_eVidDisplay_Exe               s32WMonitor_eVidDisplay_Exe;        // 114   0x1C8
   Tpfs32WMonitor_eVideoBlend_Exe               s32WMonitor_eVideoBlend_Exe;        // 115   0x1CC
   TpfvWMonitor_eStopLayer_Exe                  vWMonitor_eStopLayer_Exe;           // 116   0x1D0
   Tpfs32WMonitor_eParameterData_Set            s32WMonitor_eParameterData_Set;     // 117   0x1D4
   Tpfs32WMonitor_eParameterData_Get            s32WMonitor_eParameterData_Get;     // 118   0x1D8        
   TpfvWMonitor_eSurfaceAlloc_Exe				vWMonitor_eSurfaceAlloc_Exe;		// 119   0x1DC
   TpfvWMonitor_eSurfaceFree_Exe				vWMonitor_eSurfaceFree_Exe;			// 120   0x1E0
   TpfvWMonitor_eVSync_Wait						vWMonitor_eVSync_Wait;				// 121   0x1E4
   TpfAPI_ARtc_eReadTimestamp_Exe				ps32ARtc_eReadTimestamp_Exe;        // 122   0x1E8
   TpfAPI_ARtc_eTimer_Set						ps32ARtc_eTimer_Set;				// 123   0x1EC
   TpfAPI_ARtc_eTimer_Get						ps32ARtc_eTimer_Get;				// 124   0x1F0
   TpfAPI_eMalloc_Exe                           ppvAPI_eMallocVram_Exe;             // 125   0x1F4
   TpfAPI_eFree_Exe                             pvAPI_eFreeVram_Exe;                // 126   0x1F8
   TpfvWMonitor_eGraphicsAlphaBlending			vWMonitor_eGraphicsAlphaBlending;	// 127   0x1FC
   Tpfs32WMonitor_eVidErase_Exe					s32WMonitor_eVidErase_Exe;			// 128	 0x200
   TpfAPI_eJ1939_eRxId_Set						s32J1939_eRxId_Set;					// 128	 0x204
   TpfAPI_eCommonLog_Exe					vLOG_AVHCommon_Log;			
   TpfAPI_eCommonLogData_Exe				vLOG_AVHCommon_LogData;
   TpfAPI_MLin_eGetStatus_Exe                   s32MLin_eGetStatus_Exe; 						// 129	 0x208  //leizhu changed for add lin 180705
//   TpffWMonitor_MathCosf                        fWMonitor_MathCosf;                 //
//   TpffWMonitor_MathSinf                        fWMonitor_MathSinf;                 //
//   TpffWMonitor_MathAcosf                       fWMonitor_MathAcosf;                //
//   TpffWMonitor_MathAsinf                       fWMonitor_MathAsinf;                //
//   TpffWMonitor_MathSqrtf                       fWMonitor_MathSqrtf;                //
//   TpffWMonitor_MathPowf                        fWMonitor_MathPowf;                 //
//   TpfiWMonitor_StdRand                         iWMonitor_StdRand;                  //
//   TpfvWMonitor_StdSrand                        vWMonitor_StdSrand;                 //
//   TpfvWMonitor_StdVa_start                     vWMonitor_StdVa_start;              //
//   TpfvWMonitor_StdVa_end                       vWMonitor_StdVa_end;                //
} TstWMonitor_eApplication_Function;



////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////
#define C_Monitor_eAppFuncStartAdd           __MonitorFunctions_start
#define DstWMonitor_eApplication_Function  (*((TstWMonitor_eApplication_Function *)C_Monitor_eAppFuncStartAdd))

//Generic Return Codes (applicable to all monitor modules)
#define Ds32Monitor_eSuccess               0
#define Ds32Monitor_eError                -1

#define Ds32Monitor_ePENDING               1
#define Ds32Monitor_eCRCError             -2

//----------------------------------------------------------------------------
// I/O Application ID channel list
//----------------------------------------------------------------------------
// FIX > SSPR:   ONLY FOR quick TEST !  - Straton will not access to the LED in this way !
enum
{
   Ceu16WMonitor_eLED20AA = 94,  // 94
   Ceu16WMonitor_eLED20AB,
   Ceu16WMonitor_eLED20BA,
   Ceu16WMonitor_eLED20BB,
   Ceu16WMonitor_eLED20CA,
   Ceu16WMonitor_eLED20CB  // 99
};

//----------------------------------------------------------------------------
// Monitor Retain variables
//----------------------------------------------------------------------------
enum
{
   	Ceu16WMonitor_eInKCoef, 		// offset should start with 0
   	//CHENGJIE TANG for	EEP SPI TO I2C 2017-12-19	B
   	Ceu16WMonitor_eGaugeZeroPosOffset1,		//No use
	Ceu16WMonitor_eGaugeZeroPosOffset2,		//No use
	Ceu16WMonitor_eGaugeZeroPosOffset3,		//No use
	Ceu16WMonitor_eGaugeZeroPosOffset4,		//No use
	//CHENGJIE TANG for	EEP SPI TO I2C 2017-12-19	E
	Ceu16WMonitor_eParameterNbr,
};



////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////

#endif   /* MONITOR_INCLUDE   */
