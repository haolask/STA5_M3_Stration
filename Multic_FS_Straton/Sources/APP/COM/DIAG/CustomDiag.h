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
// Version      :$Revision: 1006 $
// Branch       :$Branch $
// Updated the    :$Date: 2015-12-03 17:38:15 +0800 (周四, 03 十二月 2015) $
// By           :$Author: pcouronn $
//############################################################################


//****************************************************************************//
// INCLIB
//****************************************************************************//

//****************************************************************************//
// INCUSER
//****************************************************************************//
#include "CustomDiag.cfg"

//****************************************************************************//
// DEFINITION
//****************************************************************************//
#define Du32ADiag_eAppParamConfigStartAddress ((unsigned long)Du32Straton_eAppParamConfig_start + 2) //0x001FF512  // ActiGRAF Address
#define CeuiADiag_NbOfAppParam               ((unsigned short)*((unsigned short*)(Du32ADiag_eAppParamConfigStartAddress-2))) // 256

//----------------------------------------------------------------------------
//Legislated SID (used on KWP)
//----------------------------------------------------------------------------
#define CeiADiag_eStartDiagnosticSession        0x10  //0
#define CeiADiag_eEcuReset                      0x11  //1
#define CeiADiag_ReadFreezeFrameData            0x12  //2
#define CeiADiag_ReadDiagnosticTroubleCodes     0x13  //3
#define CeiADiag_ClearDiagnosticInformation     0x14  //4
#define CeiADiag_eReadEcuIdentification         0x1A  //5
#define CeiADiag_eStopDiagnosticSession         0x20  //6
#define CeiADiag_eReadDataByLocalIdentifier     0x21  //7
#define CeiADiag_eReadMemoryByAddress           0x23  //8
#define CeiADiag_eSecurityAccess                0x27  //9
#define CeiADiag_eInOutControlByLocalIdent      0x30  //10
#define CeiADiag_eStartRoutineByLocalIdent      0x31  //11
#define CeiADiag_eStopRoutineByLocalIdent       0x32  //12
#define CeiADiag_eRequestDownload               0x34  //13
#define CeiADiag_RequestUpload                  0x35  //14
#define CeiADiag_eTransferData                  0x36  //15
#define CeiADiag_eRequestTransfertExit          0x37  //16
#define CeiADiag_eWriteDataByLocalIdentifier    0x3B  //17
#define CeiADiag_eWriteMemoryByAddress          0x3D  //18
#define CeiADiag_eTesterPresent                 0x3E  //19
#define CeiADiag_eStartCommunication            0x81  //20
#define CeiADiag_eStopCommunication             0x82  //21
//Other
#define CeiADiag_eSIDnbr                        22

//----------------------------------------------------------------------------
//Legislated SID (used on UDS)
//----------------------------------------------------------------------------

#define CeiADiag_eDiagnosticSessionControl      0x10
#define CeiADiag_eReadDTCInformation            0x19
#define CeiADiag_eReadDataByIdentifier          0x22
#define CeiADiag_eWriteDataByIdentifier         0x2E
#define CeiADiag_eInOutControlByIdentifier      0x2F
#define CeiADiag_eRoutineControl                0x31

// response codes
//----------------
#define CT_NACK                              0x7F
#define CT_GeneralReject                     0x10
#define CT_ServiceNotSupported               0x11
#define CT_RequestSequenceError              0x22
#define CT_ConditionsNotCorrect              0x22
#define CT_RoutineNotComplete                0x23
#define CT_RequestOutOfRange                 0x31
#define CT_SecurityAccessDenied              0x33
#define CT_InvalidKey                        0x35
#define CT_IllegalAddressInBlockTransfert    0x74
#define CT_ReqCorrectlyRcvd_RspPending       0x78
#define CT_FlashAccessFailure                0x81

// sub services of CeiADiag_eRoutineControl
//------------------------------------------------------
#define CeuiCDiag_DisplayBS1     0xFE01
#define CeuiCDiag_DisplayBS2     0xFE02
#define CeuiCDiag_PlaySound1     0xFE11
#define CeuiCDiag_DisplayVideo1  0xFE21
#define CeuiCDiag_DisplayVideo2  0xFE22
#define CeuiCDiag_DisplayVideo3  0xFE23
#define CeuiCDiag_DisplayVideo4  0xFE24

#define Ceuc_VideoWidth          320
#define Ceuc_VideoHight          220

// sub services of CeiADiag_eReadEcuIdentification
//------------------------------------------------------
#define CeuiADiag_CustomerAppliSoftBanner       0x86
#define CeuiADiag_ActiGRAFWorkBenchVerForAppli  0x9A
#define CeuiADiag_ActiGRAFWorkBenchVerForWiring 0x9B
#define CeuiADiag_ActiGRAFAppliSoftBanner       0x9C
#define CeuiADiag_ActiGRAFWiringSoftBanner      0x9D

// sub services of CeiADiag_eReadDataByLocalIdentifier
//                 CeiADiag_eWriteDataByLocalIdentifier
//------------------------------------------------------
#define CeuiADiag_ApplicationParameters         0x40


//****************************************************************************//
// MACRO
//****************************************************************************//

//****************************************************************************//
// TYPEDEF
//****************************************************************************//

typedef struct
{
   unsigned short usIdent;          // byte   0 & 1
   unsigned short usAddressOffset;  // bytes  2 & 3
   unsigned char  ucSize;           // byte   4
   unsigned char  ucPosition;       // byte   5
   unsigned short usDefaultValue;   // bytes  6 & 7
   unsigned short usMinValue;       // bytes  8 & 9
   unsigned short usMaxValue;       // bytes 10 & 11
//   unsigned short usSpare1;         // bytes 12 & 13
//   unsigned short usSpare2;         // bytes 14 & 15
}TstADiag_AppliParamConfig;

//****************************************************************************//
// INTERN
//****************************************************************************//

//****************************************************************************//
// CONST
//****************************************************************************//

//****************************************************************************//
// PROTO
//****************************************************************************//

//****************************************************************************//
// CODE
//****************************************************************************//


