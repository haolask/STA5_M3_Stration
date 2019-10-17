//############################################################################
// HEAD : (c) ACTIA 2010 : " Any copie and re-utilisation of this file without
//                           the written consent of ACTIA is forbidden "
//        --------------------------------------------------------------------
//        Classification :  (-)No   (-)Confident. ACTIA   (-)Confident. Client
//        --------------------------------------------------------------------
//        CustomDiag : application diag file
//############################################################################
// FILE : AppDiag.c (SOURCE)
//----------------------------------------------------------------------------
// DESC : AppDiag
//----------------------------------------------------------------------------
// LIST : * Liste of the functions
//----------------------------------------------------------------------------
// HIST :
// Version      :$Revision: 1743 $
// Branch       :$Branch $
// Updated the    :$Date: 2017-03-15 21:24:08 +0800 (周三, 15 三月 2017) $
// By           :$Author: ssprung $
//############################################################################


//****************************************************************************//
// INCLIB
//****************************************************************************//

//****************************************************************************//
// INCUSER
//****************************************************************************//
#include "CustomParams.cfg"

//****************************************************************************//
// DEFINITION
//****************************************************************************//

// Error codes.
#define Ds32CustomParams_eSuccess          0 // Operation successful
#define Ds32CustomParams_eIdentOutOfRange -1 // Ident out of range (out of maximum nb of params)
#define Ds32CustomParams_eBadPtr          -2 // Bad pointer for config
#define Ds32CustomParams_eIdentNotFound   -3 // Ident not found in
#define Ds32CustomParams_eBadOffset       -4 // Offset out of range
#define Ds32CustomParams_eBadMinMaxRange  -5 // Value to write outside of min/max range

//****************************************************************************//
// MACRO
//****************************************************************************//

//****************************************************************************//
// TYPEDEF
//****************************************************************************//

typedef struct
{
   TU16 u16Ident;          // byte   0 & 1
   TU16 u16AddressOffset;  // bytes  2 & 3
   TU8  u8Size;            // byte   4
   TU8  u8Position;        // byte   5
   TU8  tu8PADDING[2];     // byte   6 & 7
   TU32 u32DefaultValue;   // bytes  8 & 11
   TU32 u32MinValue;       // bytes  12 & 15
   TU32 u32MaxValue;       // bytes  16 & 19
}TstCustomParams_eParamConfig;

//#define Du8CDiage_eParameterSize sizeof(TU32)

//****************************************************************************//
// INTERN
//****************************************************************************//

//****************************************************************************//
// CONST
//****************************************************************************//

//****************************************************************************//
// PROTO
//****************************************************************************//

//============================================================================//
// NAME : vCustomParams_eInit
//============================================================================//
// ROLE : Custom Param initialization routine
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vCustomParams_eInit(void);


// TODO > SSPR: Add for all functions:  Function HEADER + Description   !!!

// INFO: Access memory 'direct' : used by FB (Get only)
TU32 u32CustomParams_eParameter_Get(TU8 u8Size, TU8 u8Position, TU16 u16Offset);

// INFO: Access memory : used by DIAG / FB (Write only)
TS32 s32CustomParams_eWriteParameter_Exe(TU16 u16ParamIdent, TU32* u32ParamValue);
TS32 s32CustomParams_eReadParameter_Exe(TU16 u16ParamIdent, TU32* u32ParamValue);

// INFO: Access config FLASH 'JP table' : used by FB
TS32 s32CustomParams_eReadConfig_Exe(TU16 u16ParamIdent, TstCustomParams_eParamConfig* pstConfig);


//****************************************************************************//
// CODE
//****************************************************************************//



