/*//////////////////////////////////////////////////////////////////////////////
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
//        Monitor memory regions included in linker script
////////////////////////////////////////////////////////////////////////////////
// FILE : MONITOR_MEMORY.ld (HEADER)
//----------------------------------------------------------------------------
// DESC :  Monitor memory regions included in linker script (INCLUDE MONITOR_MEMORY.ld)
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 1222 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Include/monitor_memory.ld $
// Updated the :$Date: 2016-03-25 18:23:36 +0800 (周五, 25 三月 2016) $
// By 			:$Author: pcouronn $
//////////////////////////////////////////////////////////////////////////////*/

/* to include inside MEMORY command of project linker script */

   /* Monitor FLASH ROM areas */
   MON_FUNC          (rx): ORIGIN = 0x18080100, LENGTH = 0x00000300  /* Monitor functions pointer table */
   MON_CODE          (rx): ORIGIN = 0x18080400, LENGTH = 0x001A0000  /* Monitor code */
/* MON_SPAR          (r) : ORIGIN = 0x18220400, LENGTH = 0x0005FB80     Monitor spare area */
   MON_IDEN          (r) : ORIGIN = 0x1827FF80, LENGTH = 0x00000044  /* Monitor Software Identification (66 bytes) */
/* MON_SPAR          (r) : ORIGIN = 0x1827FFC2, LENGTH = 0x00000034     Monitor spare area */
   MON_ZVAL          (r) : ORIGIN = 0x1827FFF8, LENGTH = 0x00000004  /* Monitor ZoneValue */
   MON_CKSM          (r) : ORIGIN = 0x1827FFFC, LENGTH = 0x00000004  /* Monitor CheckSum */

   /* Straton FLASH ROM areas */
   STR_FUNC          (rx): ORIGIN = 0x18280000, LENGTH = 0x00000060  /* Straton functions pointer table */
   STR_CODE          (rx): ORIGIN = 0x18280060, LENGTH = 0x00064000  /* Straton VM code */
/* STR_SPAR          (rx): ORIGIN = 0x182E4060, LENGTH = 0x000314F0     Straton spare area */
   STR_T5AP          (rx): ORIGIN = 0x18315550, LENGTH = 0x000FEF00  /* Straton T5 Application Code */
   AGF_IDEN          (r) : ORIGIN = 0x18414450, LENGTH = 0x00000100  /* ActiGRAF Softwares Identification */
   STR_PARA          (r) : ORIGIN = 0x18414550, LENGTH = 0x00001000  /* Straton Parameter Configuration */
   STR_EMW_ZED_WIR   (r) : ORIGIN = 0x18415550, LENGTH = 0x00006000  /* Straton-EmWi ZED Wiring */
/* STR_SPAR          (r) : ORIGIN = 0x1841B550, LENGTH = 0x00024A30     Straton spare area */
   STR_IDEN          (r) : ORIGIN = 0x1843FF80, LENGTH = 0x00000044  /* Straton Software Identification (66 bytes) */
/* STR_SPAR          (r) : ORIGIN = 0x1843FFC2, LENGTH = 0x00000034     Straton spare area */
   STR_ZVAL          (r) : ORIGIN = 0x1843FFF8, LENGTH = 0x00000004  /* Straton ZoneValue */
   STR_CKSM          (r) : ORIGIN = 0x1843FFFC, LENGTH = 0x00000004  /* Straton CheckSum */

   /* EmWizard FLASH ROM areas */
   EMW_FUNC          (rx): ORIGIN = 0x18480000, LENGTH = 0x00000060  /* EmWizard functions pointer table */
   EMW_CODE          (rx): ORIGIN = 0x18480060, LENGTH = 0x00B7FF20  /* EmWizard code */
   EMW_IDEN          (r) : ORIGIN = 0x18FFFF80, LENGTH = 0x00000044  /* EmWizard Software Identification (66 bytes) */
/* EMP_SPAR          (r) : ORIGIN = 0x18FFFFC4, LENGTH = 0x00000034     EmWizard spare area */
   EMW_ZVAL          (r) : ORIGIN = 0x18FFFFF8, LENGTH = 0x00000004  /* EmWizard ZoneValue */
   EMW_CKSM          (r) : ORIGIN = 0x18FFFFFC, LENGTH = 0x00000004  /* EmWizard CheckSum */
      
   /* Monitor - Straton RAM areas */
   MON_ZED           (rw): ORIGIN = 0x20020000, LENGTH = 0x00008000  /* Monitor - Straton Data Exchange Area */
   MON_ZED_VM        (rw): ORIGIN = 0x20020000, LENGTH = 0x00000300  /* Monitor - Straton VM Data */
   MON_ZED_MULTICS   (rw): ORIGIN = 0x20020300, LENGTH = 0x00000300  /* Monitor - Straton Multic-S */
   MON_ZED_POWERXX   (rw): ORIGIN = 0x20020600, LENGTH = 0x00002900  /* Monitor - Straton PowerXX */
   MON_ZED_SWP       (rw): ORIGIN = 0x20022F00, LENGTH = 0x00000410  /* Monitor - Straton SWP */
   MON_ZED_J1939     (rw): ORIGIN = 0x20023310, LENGTH = 0x00002D10  /* Monitor - Straton J1939 */
   MON_ZED_SPARE     (rw): ORIGIN = 0x20026020, LENGTH = 0x00001FE0  /* Monitor - Straton Spare */
   STR_ZED           (rw): ORIGIN = 0x20028000, LENGTH = 0x00001000  /* Straton - EmWi Data Exchange Area */

   MON_RAM           (rw): ORIGIN = 0x20029000, LENGTH = 0x00064000  /* Monitor RAM */
   STR_RAM           (rw): ORIGIN = 0x2008D000, LENGTH = 0x00019000  /* Straton RAM */
   STR_VMDB          (rw): ORIGIN = 0x200A6000, LENGTH = 0x00032000  /* Straton Virtual Machine Data Base */
   STACKS            (rw): ORIGIN = 0x200D8000, LENGTH = 0x00018000  /* temporary for stacks */
/* STR_SPAR          (r) : ORIGIN = 0x200F0000, LENGTH = 0x0000F000     Straton spare area */

   MON_NVDM          (rw): ORIGIN = 0x200FF000, LENGTH = 0x00001000  /* Monitor Non Volatil Data Memory */

   EMW_RAM           (rw): ORIGIN = 0x4c100000, LENGTH = 0x00300000  /* Monitor RAM */
   