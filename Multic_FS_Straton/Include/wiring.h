////////////////////////////////////////////////////////////////////////////////
//
//      O       OOOO  OOOOOOO   O       O         ## ## ##
//     O O     O         O      O      O O
//    O   O    O         O      O     O   O      ## ## ##
//   OOOOOOO   O         O      O    OOOOOOO
//  O       O   OOOO     O      O   O       O   ## ## ##
//
//----------------------------------------------------------------------------
// HEAD : (c) ACTIA Automotive 2014 : " Any copy and re-use of this
//        file without the written consent of ACTIA is forbidden "
//        --------------------------------------------------------------------
//        Classification :  (-)No   (-)Confident. ACTIA   (-)Confident. Client
//        --------------------------------------------------------------------
//        CALIB : Board Calibration
////////////////////////////////////////////////////////////////////////////////
// FILE : boot.h (CONFIGURATION)
//----------------------------------------------------------------------------//
// DESC : Board Calibration
//----------------------------------------------------------------------------//
// HIST :
// Version     :$Revision: 669 $
// Url         :$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Include/wiring.h $
// Updated the :$Date: 2015-06-01 18:59:53 +0800 (周一, 01 六月 2015) $
// By          :$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
#ifndef WIRING_INCLUDE
#define WIRING_INCLUDE

#include "std_type.h"
#include "ver_type.h"
#include "wiring_io.h"
#include "wiring_j1939.h"

//----------------------------------------------------------------------------
// Wiring mapping
//----------------------------------------------------------------------------
#define Du32Wiring_eDiagDataBaseAddr      (0x18440000)
#define Du32Wiring_eActiGRAFBannersAddr   (0x1844FF00)
#define Du32Wiring_eIOTableAddr           (0x18450000)
#define Du32Wiring_eParametersAddr        (0x18458000)
#define Du32Wiring_eEquipVariablesAddr    (0x1845E000)
#define Du32Wiring_eSpareAddr             (0x18460000)
#define Du32Wiring_eSoftIdentAddr         (0x1847FF80)

//----------------------------------------------------------------------------
// Software ident
//----------------------------------------------------------------------------
#define DstWiring_eWiringIdent         (*((TstWVersion_eSWIdent *)(Du32Wiring_eSoftIdentAddr)))

//----------------------------------------------------------------------------
// ActiGRAF banners
//----------------------------------------------------------------------------
#define Du32Wiring_eActiGRAF_WirWorkbBanner_start   (Du32Wiring_eActiGRAFBannersAddr + 0)
#define Du32Wiring_eActiGRAF_WiringBanner_start     (Du32Wiring_eActiGRAFBannersAddr + 128)

//----------------------------------------------------------------------------
// Parameters
//----------------------------------------------------------------------------
#define DstWiring_eCanParamBaseAddr       (Du32Wiring_eParametersAddr + 0x0000)  // 0x18458000
#define Du32Wiring_eMulticSIOAddr         (Du32Wiring_eParametersAddr + 0x0020)  // 0x18458020
#define DstWiring_ePowerBaseAddr          (Du32Wiring_eParametersAddr + 0x0310)  // 0x18458310
#define Du32Wiring_eSWPAddr               (Du32Wiring_eParametersAddr + 0x2110)  // 0x1845A110
#define DstWiring_eJ1939BaseAddr          (Du32Wiring_eParametersAddr + 0x2120)  // 0x1845A120

//----------------------------
// CAN_Network config & param
//----------------------------

// Type Define
#define Du16Com_eCanProtocolNbr            3U
typedef TU8 Tu8CANNetworkConfiguration[Du16Com_eCanProtocolNbr];

// Param Address
#define DstWiring_eCanProtocolAddr        (DstWiring_eCanParamBaseAddr + 0x0000) // | 0x10
#define DstWiring_eCanBaudRateAddr        (DstWiring_eCanParamBaseAddr + 0x0010) // | 0x10

//----------------------------
// Multic-S IO Parameter
//----------------------------
#define DpstWiring_eIOParameter        ((TstWiringIO_eParameter *)(Du32Wiring_eMulticSIOAddr))

//----------------------------
// Power_pro config & param
//----------------------------

#define DstWiring_eNbOfPower33              (DstWiring_ePowerBaseAddr   + 0x0000) // | 0x1
#define DstWiring_eNbOfPower15              (DstWiring_ePowerBaseAddr   + 0x0001) // | 0x1
#define DstWiring_eNbOfPower66              (DstWiring_ePowerBaseAddr   + 0x0002) // | 0x1

#define DstWiring_ePower33_Config           (DstWiring_ePowerBaseAddr   + 0x0004) // | 0x14
#define DstWiring_ePower15_Config           (DstWiring_ePowerBaseAddr   + 0x0018) // | 0x14
#define DstWiring_ePower66_Config           (DstWiring_ePowerBaseAddr   + 0x002C) // | 0x14

#define DstWiring_eParam_Power33_1          (DstWiring_ePowerBaseAddr   + 0x0040) // | 0xBE (190)
#define DstWiring_eParam_Power33_2          (DstWiring_ePowerBaseAddr   + 0x00FE) // | 0xBE (190)
#define DstWiring_eParam_Power33_3          (DstWiring_ePowerBaseAddr   + 0x01BC) // | 0xBE (190)
#define DstWiring_eParam_Power33_4          (DstWiring_ePowerBaseAddr   + 0x027A) // | 0xBE (190)
#define DstWiring_eParam_Power33_5          (DstWiring_ePowerBaseAddr   + 0x0338) // | 0xBE (190)
#define DstWiring_eParam_Power33_6          (DstWiring_ePowerBaseAddr   + 0x03F6) // | 0xBE (190)
#define DstWiring_eParam_Power33_7          (DstWiring_ePowerBaseAddr   + 0x04B4) // | 0xBE (190)
#define DstWiring_eParam_Power33_8          (DstWiring_ePowerBaseAddr   + 0x0572) // | 0xBE (190)
#define DstWiring_eParam_Power33_9          (DstWiring_ePowerBaseAddr   + 0x0630) // | 0xBE (190)
#define DstWiring_eParam_Power33_10         (DstWiring_ePowerBaseAddr   + 0x06EE) // | 0xBE (190)

#define DstWiring_eParam_Power15_1          (DstWiring_ePowerBaseAddr   + 0x07AC) // | 0xBE (190)
#define DstWiring_eParam_Power15_2          (DstWiring_ePowerBaseAddr   + 0x086A) // | 0xBE (190)
#define DstWiring_eParam_Power15_3          (DstWiring_ePowerBaseAddr   + 0x0928) // | 0xBE (190)
#define DstWiring_eParam_Power15_4          (DstWiring_ePowerBaseAddr   + 0x09E6) // | 0xBE (190)
#define DstWiring_eParam_Power15_5          (DstWiring_ePowerBaseAddr   + 0x0AA4) // | 0xBE (190)
#define DstWiring_eParam_Power15_6          (DstWiring_ePowerBaseAddr   + 0x0B62) // | 0xBE (190)
#define DstWiring_eParam_Power15_7          (DstWiring_ePowerBaseAddr   + 0x0C20) // | 0xBE (190)
#define DstWiring_eParam_Power15_8          (DstWiring_ePowerBaseAddr   + 0x0CDE) // | 0xBE (190)
#define DstWiring_eParam_Power15_9          (DstWiring_ePowerBaseAddr   + 0x0D9C) // | 0xBE (190)
#define DstWiring_eParam_Power15_10         (DstWiring_ePowerBaseAddr   + 0x0E5A) // | 0xBE (190)

#define DstWiring_eParam_Power66A_1         (DstWiring_ePowerBaseAddr   + 0x0F18) // | 0xBE (190)
#define DstWiring_eParam_Power66B_1         (DstWiring_ePowerBaseAddr   + 0x0FD6) // | 0xBE (190)
#define DstWiring_eParam_Power66A_2         (DstWiring_ePowerBaseAddr   + 0x1094) // | 0xBE (190)
#define DstWiring_eParam_Power66B_2         (DstWiring_ePowerBaseAddr   + 0x1152) // | 0xBE (190)
#define DstWiring_eParam_Power66A_3         (DstWiring_ePowerBaseAddr   + 0x1210) // | 0xBE (190)
#define DstWiring_eParam_Power66B_3         (DstWiring_ePowerBaseAddr   + 0x12CE) // | 0xBE (190)
#define DstWiring_eParam_Power66A_4         (DstWiring_ePowerBaseAddr   + 0x138C) // | 0xBE (190)
#define DstWiring_eParam_Power66B_4         (DstWiring_ePowerBaseAddr   + 0x144A) // | 0xBE (190)
#define DstWiring_eParam_Power66A_5         (DstWiring_ePowerBaseAddr   + 0x1508) // | 0xBE (190)
#define DstWiring_eParam_Power66B_5         (DstWiring_ePowerBaseAddr   + 0x15C6) // | 0xBE (190)
#define DstWiring_eParam_Power66A_6         (DstWiring_ePowerBaseAddr   + 0x1684) // | 0xBE (190)
#define DstWiring_eParam_Power66B_6         (DstWiring_ePowerBaseAddr   + 0x1742) // | 0xBE (190)
#define DstWiring_eParam_Power66A_7         (DstWiring_ePowerBaseAddr   + 0x1800) // | 0xBE (190)
#define DstWiring_eParam_Power66B_7         (DstWiring_ePowerBaseAddr   + 0x18BE) // | 0xBE (190)
#define DstWiring_eParam_Power66A_8         (DstWiring_ePowerBaseAddr   + 0x197C) // | 0xBE (190)
#define DstWiring_eParam_Power66B_8         (DstWiring_ePowerBaseAddr   + 0x1A3A) // | 0xBE (190)
#define DstWiring_eParam_Power66A_9         (DstWiring_ePowerBaseAddr   + 0x1AF8) // | 0xBE (190)
#define DstWiring_eParam_Power66B_9         (DstWiring_ePowerBaseAddr   + 0x1BB6) // | 0xBE (190)
#define DstWiring_eParam_Power66A_10        (DstWiring_ePowerBaseAddr   + 0x1C74) // | 0xBE (190)
#define DstWiring_eParam_Power66B_10        (DstWiring_ePowerBaseAddr   + 0x1D32) // | 0xBE (190)

//----------------------------
// SWP_pro config & param
//----------------------------
#define Du8Wiring_eSWP_Config				(Du32Wiring_eSWPAddr	+ 0x0000) // | 0x1
#define Du8Wiring_eSWP_Param				(Du32Wiring_eSWPAddr	+ 0x0001) // | 0x1

//----------------------------
// J1939 config & param
//----------------------------

#define DstWiring_eJ1939A_Config            (DstWiring_eJ1939BaseAddr   + 0x0000) // | 0x10
#define DstWiring_eJ1939A_Config_NbTx       (DstWiring_eJ1939A_Config   + 0x0002) // | 0x2
#define DstWiring_eJ1939A_Config_NbRx       (DstWiring_eJ1939A_Config   + 0x0004) // | 0x2
#define DstWiring_eJ1939A_Param             (DstWiring_eJ1939A_Config   + 0x0010) // | 0xA00

#define DstWiring_eJ1939B_Config            (DstWiring_eJ1939BaseAddr   + 0x0A10) // | 0x10
#define DstWiring_eJ1939B_Config_NbTx       (DstWiring_eJ1939B_Config   + 0x0002) // | 0x2
#define DstWiring_eJ1939B_Config_NbRx       (DstWiring_eJ1939B_Config   + 0x0004) // | 0x2
#define DstWiring_eJ1939B_Param             (DstWiring_eJ1939B_Config   + 0x0010) // | 0xA00

#define DstWiring_eJ1939C_Config            (DstWiring_eJ1939BaseAddr   + 0x1420) // | 0x10
#define DstWiring_eJ1939C_Config_NbTx       (DstWiring_eJ1939C_Config   + 0x0002) // | 0x2
#define DstWiring_eJ1939C_Config_NbRx       (DstWiring_eJ1939C_Config   + 0x0004) // | 0x2
#define DstWiring_eJ1939C_Param             (DstWiring_eJ1939C_Config   + 0x0010) // | 0xA00

#define DstWiring_eJ1939D_Config            (DstWiring_eJ1939BaseAddr   + 0x1E30) // | 0x10
#define DstWiring_eJ1939D_Config_NbTx       (DstWiring_eJ1939D_Config   + 0x0002) // | 0x2
#define DstWiring_eJ1939D_Config_NbRx       (DstWiring_eJ1939D_Config   + 0x0004) // | 0x2
#define DstWiring_eJ1939D_Param             (DstWiring_eJ1939D_Config   + 0x0010) // | 0xA00

#define DstWiring_eJ1939E_Config            (DstWiring_eJ1939BaseAddr   + 0x2840) // | 0x10
#define DstWiring_eJ1939E_Config_NbTx       (DstWiring_eJ1939E_Config   + 0x0002) // | 0x2
#define DstWiring_eJ1939E_Config_NbRx       (DstWiring_eJ1939E_Config   + 0x0004) // | 0x2
#define DstWiring_eJ1939E_Param             (DstWiring_eJ1939E_Config   + 0x0010) // | 0xA00


#endif
