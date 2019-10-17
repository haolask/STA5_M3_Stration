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
//        straton : Straton mapping Export
////////////////////////////////////////////////////////////////////////////////
// FILE : straton_map.h (HEADER)
//----------------------------------------------------------------------------
// DESC : Straton mapping Export
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 827 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Sources/APP/CORE/base/straton_map.h $
// Updated the :$Date: 2015-08-07 22:59:16 +0800 (周五, 07 八月 2015) $
// By 			:$Author: pcouronn $
////////////////////////////////////////////////////////////////////////////////
#ifndef STRATON_MAP_INCLUDE
#define STRATON_MAP_INCLUDE

//----------------------------------------------------------------------------//

extern unsigned long __StratonZED_start[];
extern unsigned long __ActiGRAF_Banners_start[];
extern unsigned char __AppParamConfig_start[];
extern unsigned char __AppParamConfig_size[];
extern unsigned char __StratonEmWi_ZED_Wiring_start[];
extern unsigned char __StratonEmWi_ZED_Wiring_end[];

////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// TYPEDEF
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////
#define CT_RAM_BaseAddressZEDFpga ((unsigned long)__StratonZED_start)

//----------------------------------------------------------------------------
//	ActiGRAF banners
//----------------------------------------------------------------------------
#define Du32Straton_eActiGRAF_AppWorkbBanner_start (__ActiGRAF_Banners_start + 0)
#define Du32Straton_eActiGRAF_SpareBanner1_start   (__ActiGRAF_Banners_start + 0x40)
#define Du32Straton_eActiGRAF_AppliBanner_start    (__ActiGRAF_Banners_start + 0x80)
#define Du32Straton_eActiGRAF_SpareBanner2_start   (__ActiGRAF_Banners_start + 0xC0)

// Customer parameter config
//---------------------------
#define Du32Straton_eAppParamConfig_start (__AppParamConfig_start)
#define Du32Straton_eAppParamConfig_size  (__AppParamConfig_size)

////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////


#endif	/*	STRATON_MAP_INCLUDE	*/