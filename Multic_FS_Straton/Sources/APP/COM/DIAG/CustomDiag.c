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
#include "std_type.h"
#include "monitor.h"
#include "straton_map.h"
#include "CustomDiag.h"
#include "wiring.h"

//****************************************************************************//
// DEFINITION
//****************************************************************************//

//****************************************************************************//
// MACRO
//****************************************************************************//


//****************************************************************************//
// TYPEDEF
//****************************************************************************//

//****************************************************************************//
// INTERN
//****************************************************************************//
TstADiag_AppliParamConfig* pstADiag_eAppliParamConfig;

unsigned short usADiag_iNbOfParameterConfigured;

//****************************************************************************//
// CONST
//****************************************************************************//

extern TstWVersion_eSWIdent CstApp_eSWIdent;
extern unsigned char PstCDiag_eActiGRAFWorkBenchVerForAppliBanner[CT_ActiGRAFBannerSize];
extern unsigned char PstCDiag_eActiGRAFWorkBenchVerForWiringBanner[CT_ActiGRAFBannerSize];
extern unsigned char PstCDiag_eActiGRAFAppliSoftBanner[CT_ActiGRAFBannerSize];
extern unsigned char PstCDiag_eActiGRAFWiringSoftBanner[CT_ActiGRAFBannerSize];

//****************************************************************************//
// PROTO
//****************************************************************************//


//****************************************************************************//
// CODE
//****************************************************************************//

////////////////////////////////////////////////////////////////////////////////
// Functions (Intern)
////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------
// NAME usMNVDM_eCRC16_Calc
// ROLE Calculate CRC16 of a memory area
// IN   Area start addr, Area size
// OUT  CRC16
//-------------------------------------------------------------------
unsigned short usMNVDM_eCRC16_Calc(unsigned char* RF_PtrBufferArea, unsigned short RF_AreaSize)
{
   unsigned short Polynome = 0x1021 & 0xfffe;
   unsigned short Crc;
   unsigned short Numero_Bit;
   unsigned short Donnee ;
   unsigned short usTmp;

   Crc  = (unsigned short)(*RF_PtrBufferArea++) << 8;
   Crc += (unsigned short)(*RF_PtrBufferArea++);
   RF_AreaSize-=2;

   for (; RF_AreaSize != 0 ; RF_AreaSize--)
   {
      Donnee = (unsigned short)(*RF_PtrBufferArea++);
      for ( Numero_Bit = 0x80; Numero_Bit > 0; Numero_Bit >>= 1)
      {
         usTmp = ((Crc >> 15) & 0x0001);
         Crc = (Crc << 1) | usTmp;    // rotation vers la gauche

         //Crc = _irol_(Crc,1);
         if(Crc & 1)
         Crc ^= Polynome;

         if(Donnee & Numero_Bit)
         Crc ^= 1;
      }
   }
   return(Crc);
 }

//============================================================================//
// NAME : vCDiag_UpdateDefaultParam_exe
//============================================================================//
// ROLE : custom diag routine
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vCDiag_UpdateDefaultParam_exe(void)
{
   unsigned char  ucDataPos=0;
   unsigned short usDataMask=0;
   unsigned short usParamIndex=0;
   unsigned short usDataDefault;
   unsigned short usAddrOffset;

   for(usParamIndex=0; usParamIndex<usADiag_iNbOfParameterConfigured; usParamIndex++)
   {
      usAddrOffset = pstADiag_eAppliParamConfig[usParamIndex].usAddressOffset;
      if(usAddrOffset < Du16MNVDM_iAppParamSize)
      {
         usDataDefault  = pstADiag_eAppliParamConfig[usParamIndex].usDefaultValue;

         switch(pstADiag_eAppliParamConfig[usParamIndex].ucSize)
         {
            case 0:
               // Size=0 -> do nothing
               break;

            case 1:
            case 2:
            case 3:
            case 4:
               ucDataPos      = pstADiag_eAppliParamConfig[usParamIndex].ucPosition;
               usDataMask     = 0x00FF >> (8-pstADiag_eAppliParamConfig[usParamIndex].ucSize);
               //reset val to 0
               ((unsigned char *)Du32API_Addr_Appli_Param)[usAddrOffset] &= (unsigned char)(~(usDataMask<<ucDataPos));
               // set val to default val
               ((unsigned char *)Du32API_Addr_Appli_Param)[usAddrOffset] |= (unsigned char)((usDataDefault & usDataMask)<<ucDataPos);
               break;

            case 8:
               ((unsigned char *)Du32API_Addr_Appli_Param)[usAddrOffset]  = (unsigned char) (usDataDefault & 0x00FF);
               break;

            case 16:
               ((unsigned char *)Du32API_Addr_Appli_Param)[usAddrOffset]  = (unsigned char)((usDataDefault & 0xFF00)>>8); // MSB
               ((unsigned char *)Du32API_Addr_Appli_Param)[usAddrOffset+1]= (unsigned char) (usDataDefault & 0x00FF);     // LSB
               break;

            default:
               // Size invalid  -> do nothing
               break;
         }
      }
      else
      {
         // Address offset out of range
      }
   }
}

//============================================================================//
// NAME : vApp_CustomDiagInit
//============================================================================//
// ROLE : custom diag routine
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vApp_CustomDiagInit(void)
{
   unsigned short usChecksum_Calculated;
   unsigned short usChecksum_Memorised;

   unsigned long ulAppParamConfig_StartAddr;
   unsigned long ulAppParamConfig_Size;
   unsigned long ulAppParamAddr;

   pstADiag_eAppliParamConfig = (TstADiag_AppliParamConfig*)Du32ADiag_eAppParamConfigStartAddress;

   // check param config table address
   ulAppParamConfig_StartAddr = (unsigned long)((unsigned char*)Du32Straton_eAppParamConfig_start);
   ulAppParamConfig_Size = (unsigned long)((unsigned char*)Du32Straton_eAppParamConfig_size);

   if((ulAppParamConfig_StartAddr+ulAppParamConfig_Size)<Du32ADiag_eAppParamConfigStartAddress)
   {
      // Du32ADiag_eAppParamConfigStartAddress out of range
      usADiag_iNbOfParameterConfigured = 0;
   }
   else
   {
      usADiag_iNbOfParameterConfigured = CeuiADiag_NbOfAppParam;
      if (usADiag_iNbOfParameterConfigured > CeuiADiag_NbOfAppParamMax)
      {
         usADiag_iNbOfParameterConfigured = CeuiADiag_NbOfAppParamMax;
      }
      else {} // empty else

      // check that param config is not going out of range
      ulAppParamAddr = Du32ADiag_eAppParamConfigStartAddress + (usADiag_iNbOfParameterConfigured*sizeof(TstADiag_AppliParamConfig));

      if(ulAppParamAddr>(ulAppParamConfig_StartAddr+ulAppParamConfig_Size))
      {
         // Nb of parameter in the config makes the config table going out of range
         // Limit the number of parameter to avoid going out of range:
         usADiag_iNbOfParameterConfigured = (unsigned short)((ulAppParamConfig_StartAddr+ulAppParamConfig_Size) - Du32ADiag_eAppParamConfigStartAddress)/sizeof(TstADiag_AppliParamConfig);
      }
      else
      {} // empty else
   }


   // calculate checksum
   usChecksum_Calculated = usMNVDM_eCRC16_Calc((unsigned char *)(Du32API_Addr_Appli_Param),Du16MNVDM_iAppParamSize);
   usChecksum_Memorised = *((unsigned short*)(Du32API_Addr_Appli_Param_CS));

   if(usChecksum_Calculated!=usChecksum_Memorised)
   {
      // use defaulf param
      vCDiag_UpdateDefaultParam_exe();
   }
   else
   {
      // CS correct, do nothing
   }
}

//============================================================================//
// NAME : iCDiag_ProcessReadIdentification_Exe
//============================================================================//
// ROLE :
//----------------------------------------------------------------------------//
// INPUT :
// OUTPUT :
//============================================================================//
int iCDiag_ProcessReadIdentification_Exe(unsigned char *pucFrame,unsigned int FuiLength ,unsigned char FucRequestSource)
{
   unsigned char ucSousSID;
   unsigned char ucCounter=0;
   int iReturnValue=0;
   unsigned char * pucCustomBanner;
   unsigned char ucBannerSize;

   ucSousSID = pucFrame[1];

   switch(ucSousSID)
   {
      case CeuiADiag_CustomerAppliSoftBanner:
         pucCustomBanner = (unsigned char*) &(CstApp_eSWIdent.u8V0);
         ucBannerSize = sizeof(TstWVersion_eSWIdent);
         break;
      case CeuiADiag_ActiGRAFWorkBenchVerForAppli: 
         pucCustomBanner = (unsigned char*) Du32Straton_eActiGRAF_AppWorkbBanner_start; // &(PstCDiag_eActiGRAFWorkBenchVerForAppliBanner[0]);
         ucBannerSize = CT_ActiGRAFBannerSize;
         break;
      case CeuiADiag_ActiGRAFWorkBenchVerForWiring:
         pucCustomBanner = (unsigned char*) Du32Wiring_eActiGRAF_WirWorkbBanner_start; // &(PstCDiag_eActiGRAFWorkBenchVerForWiringBanner[0]);
         ucBannerSize = CT_ActiGRAFBannerSize;
         break;
      case CeuiADiag_ActiGRAFAppliSoftBanner:
         pucCustomBanner = (unsigned char*) Du32Straton_eActiGRAF_AppliBanner_start; // &(PstCDiag_eActiGRAFAppliSoftBanner[0]);
         ucBannerSize = CT_ActiGRAFBannerSize;
         break;
      case CeuiADiag_ActiGRAFWiringSoftBanner:
         pucCustomBanner = (unsigned char*) Du32Wiring_eActiGRAF_WiringBanner_start; // &(PstCDiag_eActiGRAFWiringSoftBanner[0]);
         ucBannerSize = CT_ActiGRAFBannerSize;
         break;
      default:
         pucCustomBanner = 0;
         ucBannerSize = 0;
         break;
   }

   if(0!=pucCustomBanner)
   {
      for(ucCounter=0; ucCounter<ucBannerSize; ucCounter++)
      {
         pucFrame[ucCounter+2]=pucCustomBanner[ucCounter];
      }

      iReturnValue= 2 + ucBannerSize;
   }
   else
   {
      pucFrame[1]  = CT_ServiceNotSupported;
      iReturnValue =-CT_ServiceNotSupported;
   }

   return (iReturnValue);
}

//============================================================================//
// NAME : iADiag_iReadApplicationParameter_Exe
//============================================================================//
// ROLE : return Application Parameter value
//----------------------------------------------------------------------------//
// INPUT :
// OUTPUT :
//============================================================================//
int iADiag_iReadApplicationParameter_Exe(unsigned char *FpucRequestData, unsigned char *FpucResponseData)
{
   int iReturnValue=0;
   unsigned char  ucByteIndex=0;
   unsigned char  ucDataByte=0;
   unsigned char  ucDataPos=0;
   unsigned char  ucDataMask=0;
   unsigned short usParamIndex=0;
   unsigned short usParamIdent = (unsigned short) FpucRequestData[0];

   for(usParamIndex=0; usParamIndex<usADiag_iNbOfParameterConfigured; usParamIndex++)
   {
      if(pstADiag_eAppliParamConfig[usParamIndex].usIdent == usParamIdent)
      {
         if(pstADiag_eAppliParamConfig[usParamIndex].usAddressOffset < Du16MNVDM_iAppParamSize)
         {
            switch(pstADiag_eAppliParamConfig[usParamIndex].ucSize)
            {
               case 0:
                  // Size=0 -> Ident out of range
                  return(-CT_RequestOutOfRange);
                  break;
               case 1:
               case 2:
               case 3:
               case 4:
                  ucDataByte = ((unsigned char *)Du32API_Addr_Appli_Param)[pstADiag_eAppliParamConfig[usParamIndex].usAddressOffset];
                  ucDataPos  = pstADiag_eAppliParamConfig[usParamIndex].ucPosition;
                  ucDataMask = 0xFF >> (8-pstADiag_eAppliParamConfig[usParamIndex].ucSize);

                  FpucResponseData[ucByteIndex++] = (ucDataByte>>ucDataPos) & ucDataMask;
                  return(ucByteIndex);
                  break;
               case 8:
                  FpucResponseData[ucByteIndex++] = ((unsigned char *)Du32API_Addr_Appli_Param)[pstADiag_eAppliParamConfig[usParamIndex].usAddressOffset];
                  return(ucByteIndex);
                  break;
               case 16:
                  FpucResponseData[ucByteIndex++] = ((unsigned char *)Du32API_Addr_Appli_Param)[pstADiag_eAppliParamConfig[usParamIndex].usAddressOffset+1]; // LSB
                  FpucResponseData[ucByteIndex++] = ((unsigned char *)Du32API_Addr_Appli_Param)[pstADiag_eAppliParamConfig[usParamIndex].usAddressOffset];   // MSB
                  return(ucByteIndex);
                  break;
               default:
                  // Size invalid out of range
                  return(-CT_RequestOutOfRange);
                  break;
            }
         }
         else
         {
            // Address offset out of range
            return(-CT_RequestOutOfRange);
         }
      }
      else
      {
         // Carry on searching ident
      }
   }

   // No ident founded
   return (-CT_RequestOutOfRange);
}

//============================================================================//
// NAME : iCDiag_iReadDataByLocalId_Exe
//============================================================================//
// ROLE :
//----------------------------------------------------------------------------//
// INPUT :
// OUTPUT :
//============================================================================//
int iCDiag_iReadDataByLocalId_Exe(unsigned char *pucFrame,unsigned int FuiLength ,unsigned char FucRequestSource)
{
   int iDataLength=0;
   int iReturnValue=0;
   unsigned char ucSousSID=pucFrame[1];

   switch(ucSousSID)
   {
      case CeuiADiag_ApplicationParameters:
         iReturnValue = iADiag_iReadApplicationParameter_Exe(&pucFrame[2],&pucFrame[2]);
         if(iReturnValue>=0)
         {
            //Transmit Pos Response
            pucFrame[1]=ucSousSID;
            iReturnValue+=2;
         }
         else
         {
            //Transmit Neg response
            pucFrame[1]=CT_RequestOutOfRange;
            iReturnValue=(-CT_RequestOutOfRange);
         }
         break;

      default:
         //Transmit Neg response
         pucFrame[1]=CT_RequestOutOfRange;
         iReturnValue=(-CT_RequestOutOfRange);
         break;
   }

   return(iReturnValue);
}

//============================================================================//
// NAME : iADiag_iWriteApplicationParameter_Exe
//============================================================================//
// ROLE : return Application Parameter value
//----------------------------------------------------------------------------//
// INPUT :
// OUTPUT :
//============================================================================//
int iADiag_iWriteApplicationParameter_Exe(unsigned char *FpucRequestData, unsigned char *FpucResponseData)
{
   int iReturnValue=0;
   unsigned char  ucByteIndex=0;
   unsigned char  ucDataPos=0;
   unsigned short usDataMask=0;
   unsigned short usParamIndex=0;
   unsigned short usParamIdent =  (unsigned short) FpucRequestData[0];
   unsigned short usParamValue =(((unsigned short) FpucRequestData[2])<<8)
                                + (unsigned short) FpucRequestData[1];
   unsigned short usAddrOffset;
   unsigned char  ucParamSize;
   unsigned long  ulParamMaxVal;
   unsigned char  ucI;

   for(usParamIndex=0; usParamIndex<usADiag_iNbOfParameterConfigured; usParamIndex++)
   {
      usAddrOffset = pstADiag_eAppliParamConfig[usParamIndex].usAddressOffset;
      if(pstADiag_eAppliParamConfig[usParamIndex].usIdent == usParamIdent)
      {
         if(usAddrOffset < Du16MNVDM_iAppParamSize)
         {
            ucParamSize = pstADiag_eAppliParamConfig[usParamIndex].ucSize;
            ulParamMaxVal=1;
            for(ucI=0; ucI<ucParamSize; ucI++)
            {
               ulParamMaxVal *= 2;
            }
            if(usParamValue>=ulParamMaxVal)
            {
               // removed to allow functionality with SDIAG // return(-CT_RequestOutOfRange);
            }

            switch(ucParamSize)
            {
               case 0:
                  // Size=0 -> Ident out of range
                  return(-CT_RequestOutOfRange);
                  break;
               case 1:
               case 2:
               case 3:
               case 4:
                  ucDataPos      = pstADiag_eAppliParamConfig[usParamIndex].ucPosition;
                  usDataMask     = 0x00FF >> (8-ucParamSize);
                  //reset val to 0
                  ((unsigned char *)Du32API_Addr_Appli_Param)[usAddrOffset] &= (unsigned char)(~(usDataMask<<ucDataPos));
                  // set val
                  ((unsigned char *)Du32API_Addr_Appli_Param)[usAddrOffset] |= (unsigned char)((usParamValue & usDataMask)<<ucDataPos);
                  return(ucByteIndex);
                  break;
               case 8:
                  ((unsigned char *)Du32API_Addr_Appli_Param)[usAddrOffset]  = (unsigned char) (usParamValue & 0x00FF);
                  return(ucByteIndex);
                  break;
               case 16:
                  ((unsigned char *)Du32API_Addr_Appli_Param)[usAddrOffset]  = (unsigned char)((usParamValue & 0xFF00)>>8); // MSB
                  ((unsigned char *)Du32API_Addr_Appli_Param)[usAddrOffset+1]= (unsigned char) (usParamValue & 0x00FF);     // LSB
                  return(ucByteIndex);
                  break;
               default:
                  // Size invalid out of range
                  return(-CT_RequestOutOfRange);
                  break;
            }
         }
         else
         {
            // Address offset out of range
            return(-CT_RequestOutOfRange);
         }
      }
      else
      {
         // Carry on searching ident
      }
   }

   // No ident founded
   return (-CT_RequestOutOfRange);
}

//============================================================================//
// NAME : iCDiag_iWriteDataByLocalId_Exe
//============================================================================//
// ROLE :
//----------------------------------------------------------------------------//
// INPUT :
// OUTPUT :
//============================================================================//
int iCDiag_iWriteDataByLocalId_Exe(unsigned char *pucFrame,unsigned int FuiLength ,unsigned char FucRequestSource)
{
   int iDataLength=0;
   int iReturnValue=0;
   unsigned char ucSousSID=pucFrame[1];

   switch(ucSousSID)
   {
      case CeuiADiag_ApplicationParameters:
         iReturnValue = iADiag_iWriteApplicationParameter_Exe(&pucFrame[2],&pucFrame[2]);
         if(iReturnValue>=0)
         {
            //Transmit Pos Response
            pucFrame[1]=ucSousSID;
            iReturnValue+=2;
         }
         else
         {
            //Transmit Neg response
            pucFrame[1]=CT_RequestOutOfRange;
            iReturnValue=(-CT_RequestOutOfRange);
         }
         break;

      default:
         //Transmit Neg response
         pucFrame[1]=CT_RequestOutOfRange;
         iReturnValue=(-CT_RequestOutOfRange);
         break;
   }

   return(iReturnValue);
}

//============================================================================//
// NAME : iApp_CustomDiag
//============================================================================//
// ROLE : custom diag routine
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
int iApp_CustomDiag(unsigned char *FpucFrame,unsigned int FuiLength ,unsigned char FucRequestSource)
{
   int iReturnValue;
   unsigned char ucSID;

   iReturnValue = -CT_ServiceNotSupported;
   ucSID = FpucFrame[0];

   switch(ucSID)
   {
      case CeiADiag_eReadEcuIdentification:
         iReturnValue = iCDiag_ProcessReadIdentification_Exe(FpucFrame,FuiLength,FucRequestSource);
         break;

      case CeiADiag_eReadDataByLocalIdentifier:
         iReturnValue = iCDiag_iReadDataByLocalId_Exe(FpucFrame,FuiLength,FucRequestSource);
         break;

      case CeiADiag_eWriteDataByLocalIdentifier:
         iReturnValue = iCDiag_iWriteDataByLocalId_Exe(FpucFrame,FuiLength,FucRequestSource);
         break;

      default:
         FpucFrame[1] = CT_ServiceNotSupported;
         iReturnValue =-CT_ServiceNotSupported;
         break;
   }

   return(iReturnValue);
}

