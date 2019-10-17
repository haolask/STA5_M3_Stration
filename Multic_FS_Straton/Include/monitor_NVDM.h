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
//        monitor : Non Volatil Data Memory definition
////////////////////////////////////////////////////////////////////////////////
// FILE : monitor_NVDM.h (HEADER)
//----------------------------------------------------------------------------
// DESC :  monitor Non Volatil Data Memory definition
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 1098 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Include/monitor_NVDM.h $
// Updated the :$Date: 2016-02-09 21:45:37 +0800 (周二, 09 二月 2016) $
// By 			:$Author: pcouronn $
////////////////////////////////////////////////////////////////////////////////
#ifndef MONITOR_NVDM_INCLUDE
#define MONITOR_NVDM_INCLUDE

//----------------------------------------------------------------------------//

extern unsigned char __MON_NVDM_start[];

////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// TYPEDEF
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------
// AREA SIZES
//------------------------------------------------------------------
#define Du16MNVDM_iCS_Size                      0x002 // 2
#define Du16MNVDM_iRetainSize                   0x0FE // 254
#define Du16MNVDM_iAppParamSize                 0x0FE // 254
#define Du16MNVDM_iAppParamBannerSize           0x040 // 64
#define Du16MNVDM_iAppParamMappingSize          0x040 // 64
#define Du16MNVDM_iSPARE1_Size                  0x0FE // 254
#define Du16MNVDM_iSPARE2_Size                  0x002 // 2
#define Du16MNVDM_iSPARE3_Size                  0x040 // 64
#define Du16MNVDM_iSPARE4_Size                  0x040 // 64
#define Du16MNVDM_iDTC_NbSize                   0x002 // 2
#define Du16MNVDM_iDTC_CodeSize                 0x080 // 64*2 = 128
#define Du16MNVDM_iDTC_StatusSize               0x040 // 64*1 = 64
#define Du16MNVDM_iDTC_ContextSize              0x400 // 64*16= 1024

//------------------------------------------------------------------
// EEPROM mapping
//------------------------------------------------------------------
#define Du16MNVDM_iEepromAppRetainAddr        0x0000                                                                    // 0x0000
#define Du16MNVDM_iEepromAppRetain_CS_Addr   (Du16MNVDM_iEepromAppRetainAddr        + Du16MNVDM_iRetainSize          )  // 0x00FE
//#define Du16MNVDM_iEepromAppParamAddr        (Du16MNVDM_iEepromAppRetain_CS_Addr    + Du16MNVDM_iCS_Size             )  // 0x0100
//#define Du16MNVDM_iEepromAppParam_CS_Addr    (Du16MNVDM_iEepromAppParamAddr         + Du16MNVDM_iAppParamSize        )  // 0x01FE
//#define Du16MNVDM_iEepromAppParamBannerAddr  (Du16MNVDM_iEepromAppParam_CS_Addr     + Du16MNVDM_iCS_Size             )  // 0x0200
//#define Du16MNVDM_iEepromAppParamMappingAddr (Du16MNVDM_iEepromAppParamBannerAddr   + Du16MNVDM_iAppParamBannerSize  )  // 0x0240
//#define Du16MNVDM_iEepromSPARE1Addr          (Du16MNVDM_iEepromAppParamMappingAddr  + Du16MNVDM_iAppParamMappingSize )  // 0x0280
//#define Du16MNVDM_iEepromSPARE2Addr          (Du16MNVDM_iEepromSPARE1Addr           + Du16MNVDM_iSPARE1_Size         )  // 0x037E
//#define Du16MNVDM_iEepromSPARE3Addr          (Du16MNVDM_iEepromSPARE2Addr           + Du16MNVDM_iSPARE2_Size         )  // 0x0380
//#define Du16MNVDM_iEepromSPARE4Addr          (Du16MNVDM_iEepromSPARE3Addr           + Du16MNVDM_iSPARE3_Size         )  // 0x03C0
//#define Du16MNVDM_iEepromDTC_NbAddr          (Du16MNVDM_iEepromSPARE4Addr           + Du16MNVDM_iSPARE4_Size         )  // 0x0400
//#define Du16MNVDM_iEepromDTC_CodeAddr        (Du16MNVDM_iEepromDTC_NbAddr           + Du16MNVDM_iDTC_NbSize          )  // 0x0402
//#define Du16MNVDM_iEepromDTC_StatusAddr      (Du16MNVDM_iEepromDTC_CodeAddr         + Du16MNVDM_iDTC_CodeSize        )  // 0x0482
//#define Du16MNVDM_iEepromDTC_ContextAddr     (Du16MNVDM_iEepromDTC_StatusAddr       + Du16MNVDM_iDTC_StatusSize      )  // 0x04C2
//#define Du16MNVDM_iEepromDTC_CS_Addr         (Du16MNVDM_iEepromDTC_ContextAddr      + Du16MNVDM_iDTC_ContextSize     )  // 0x08C2

//------------------------------------------------------------------
// EEPROM image mapping          Non Volatil Data Mirror (RAM Image)
//------------------------------------------------------------------
#define Du32API_Addr_Retain            (__MON_NVDM_start)
#define Du32API_Addr_Retain_CS         (Du32API_Addr_Retain          + Du16MNVDM_iRetainSize)
#define Du32API_Addr_Appli_Param       (Du32API_Addr_Retain_CS       + Du16MNVDM_iCS_Size)
#define Du32API_Addr_Appli_Param_CS    (Du32API_Addr_Appli_Param     + Du16MNVDM_iAppParamSize)
#define Du32API_Addr_Appli_Banner      (Du32API_Addr_Appli_Param_CS  + Du16MNVDM_iCS_Size)
#define Du32API_Addr_Appli_Mapping     (Du32API_Addr_Appli_Banner    + Du16MNVDM_iAppParamBannerSize)
#define Du32API_Addr_SPARE1            (Du32API_Addr_Appli_Mapping   + Du16MNVDM_iAppParamMappingSize)
#define Du32API_Addr_SPARE2            (Du32API_Addr_SPARE1          + Du16MNVDM_iSPARE1_Size)
#define Du32API_Addr_SPARE3            (Du32API_Addr_SPARE2          + Du16MNVDM_iSPARE2_Size)
#define Du32API_Addr_SPARE4            (Du32API_Addr_SPARE3          + Du16MNVDM_iSPARE3_Size)

#define Du32API_Addr_Dtc_Nb            (Du32API_Addr_SPARE4          + Du16MNVDM_iSPARE4_Size)
#define Du32API_Addr_Dtc_Code          (Du32API_Addr_Dtc_Nb          + Du16MNVDM_iDTC_NbSize)
#define Du32API_Addr_Dtc_Status        (Du32API_Addr_Dtc_Code        + Du16MNVDM_iDTC_CodeSize)
#define Du32API_Addr_Dtc_Context       (Du32API_Addr_Dtc_Status      + Du16MNVDM_iDTC_StatusSize)
#define Du32API_Addr_Dtc_CS            (Du32API_Addr_Dtc_Context     + Du16MNVDM_iDTC_ContextSize)



////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////

#endif	/*	MONITOR_NVDM_INCLUDE	*/
