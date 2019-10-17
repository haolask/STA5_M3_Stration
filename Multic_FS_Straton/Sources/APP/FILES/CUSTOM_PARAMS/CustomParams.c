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
// Version      :$Revision: 1783 $
// Branch       :$Branch $
// Updated the    :$Date: 2017-04-07 23:10:02 +0800 (周五, 07 四月 2017) $
// By           :$Author: ssprung $
//############################################################################


//****************************************************************************//
// INCLIB
//****************************************************************************//


//****************************************************************************//
// INCUSER
//****************************************************************************//

#include "monitor.h"
#include "ver_type.h"
#include "CustomParams.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

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
TstCustomParams_eParamConfig * pstCustomParams_iParamConfig;
TU32 u32CustomParams_iNbOfParameterConfigured = 0;


//****************************************************************************//
// CONST
//****************************************************************************//


//****************************************************************************//
// PROTO
//****************************************************************************//
TU16 u16CustomParams_iCRC16_Calc(TU8* RF_PtrBufferArea, TU16 RF_AreaSize);
TS32 s32CustomParams_iIndexFromIndent_Get(TU16 u16ParamIdent);
void vCustomParams_iUpdateDefault_Exe(void);
void vCustomParams_iParameter_Set(TU8 u8Size, TU8 u8Position, TU16 u16Offset, TU32 u32Value);



//****************************************************************************//
// CODE
//****************************************************************************//

////////////////////////////////////////////////////////////////////////////////
// Functions (Intern)
////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------
// NAME u16CustomParams_iCRC16_Calc
// ROLE Calculate CRC16 of a memory area
// IN   Area start addr, Area size
// OUT  CRC16
//-------------------------------------------------------------------
TU16 u16CustomParams_iCRC16_Calc(TU8* RF_PtrBufferArea, TU16 RF_AreaSize)
{
  TU16 Polynome = 0x1021 & 0xfffe;
  TU16 Crc;
  TU16 Numero_Bit;
  TU16 Donnee ;
  TU16 u16Tmp;

  Crc  = (TU16)(*RF_PtrBufferArea++) << 8;
  Crc += (TU16)(*RF_PtrBufferArea++);
  RF_AreaSize-=2;

  for (; RF_AreaSize != 0 ; RF_AreaSize--)
  {
    Donnee = (TU16)(*RF_PtrBufferArea++);
    for ( Numero_Bit = 0x80; Numero_Bit > 0; Numero_Bit >>= 1)
    {
      u16Tmp = ((Crc >> 15) & 0x0001);
      Crc = (Crc << 1) | u16Tmp;    // rotation vers la gauche

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
// NAME : s32CustomParams_iIndexFromIndent_Get
//============================================================================//
// ROLE : Get index from indent number (different if ID is not sorted in memory).
//----------------------------------------------------------------------------//
// INPUT  : ParamID 
// OUTPUT : IdentNotFound or IdentOutOfRange if error, else ParamIdentIndex (from 0 to NbParamMax-1)
//============================================================================//
TS32 s32CustomParams_iIndexFromIndent_Get(TU16 u16ParamIdent)
{
   TU16 tmpIdent;
   TS32 s32RetVal;

   TU16 u16Index      = 0;
   TU8  u1Found       = FALSE;
   TU16 u16TableIndex = 0xFFFF;

   // Check that u16ParamIdent is part of actual number of parameters
   if (u16ParamIdent < u32CustomParams_iNbOfParameterConfigured)
   {
      //Parameters are saved accordingly to their ID order. Check ID anyway.
      tmpIdent = pstCustomParams_iParamConfig[u16ParamIdent].u16Ident;

      if(u16ParamIdent == tmpIdent)
      {
          u16TableIndex= tmpIdent;
          u1Found= TRUE;
      }
      else
      {
          //Search for the ID
          while ((u16Index < u32CustomParams_iNbOfParameterConfigured) && (FALSE == u1Found))
          {
             tmpIdent = pstCustomParams_iParamConfig[u16Index].u16Ident;

              if(u16ParamIdent == tmpIdent)
              {
                  u1Found= TRUE;
                  u16TableIndex= u16Index;
              }

              ++u16Index;
          }
      }

      // If found ?
      if(u1Found == TRUE)
      {
         s32RetVal= (u16TableIndex);
      }
      else
      {
         s32RetVal = (Ds32CustomParams_eIdentNotFound);
      }
   }
   else
   {
      s32RetVal= (Ds32CustomParams_eIdentOutOfRange);
   }

   return (s32RetVal);
}


//============================================================================//
// NAME : vCustomParams_iUpdateDefault_Exe
//============================================================================//
// ROLE : custom diag routine
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vCustomParams_iUpdateDefault_Exe(void)
{
   TU8  u8DataPos=0;
   TU8  u8Size;
   TU16 u32ParamIndex=0;
   TU32 u32DefaultValue;
   TU16 u16AddrOffset;

   for(u32ParamIndex=0; u32ParamIndex<u32CustomParams_iNbOfParameterConfigured; u32ParamIndex++)
   {
      u16AddrOffset = pstCustomParams_iParamConfig[u32ParamIndex].u16AddressOffset;

      if(u16AddrOffset < Du16MNVDM_iParamDataSize)
      {
         u8Size          = pstCustomParams_iParamConfig[u32ParamIndex].u8Size;
         u8DataPos       = pstCustomParams_iParamConfig[u32ParamIndex].u8Position;
         u32DefaultValue = pstCustomParams_iParamConfig[u32ParamIndex].u32DefaultValue;

         vCustomParams_iParameter_Set(u8Size,u8DataPos, u16AddrOffset, u32DefaultValue);

      }
   }
}


//============================================================================//
// NAME : vCustomParams_iParameter_Set
//============================================================================//
// ROLE : Set parameter according offset, size, position and value
//----------------------------------------------------------------------------//
// INPUT  : size, offset position and value to write
// OUTPUT :
//============================================================================//
void vCustomParams_iParameter_Set(TU8 u8Size, TU8 u8Position, TU16 u16Offset, TU32 u32Value)
{
	TU8 u8Data;

  if (u8Size < 16)
  {
    // ============= 6) Read current contents in RAM and insert value ======================
    u8Data= ((TU8*)(Du32NVDM_Addr_Param_Data + u16Offset))[0];

    if (u8Size < 8)
    {
    	//read position
      ((TU8*)(Du32NVDM_Addr_Param_Data + u16Offset))[0]= (u8Data & (TU8)(~((0xFF>>(8-u8Size)) << u8Position)))+ (TU8) (u32Value << u8Position);
    }
    else
    {
      //byte format
      ((TU8*)(Du32NVDM_Addr_Param_Data + u16Offset))[0]=(TU8) (u32Value & 0x00FF);
    }
  }
  else
  {
  	if(u8Size > 16)
    {
      //32 bits write.
      //TODO > SSPR: Don't use pointer, write byte-by-byte (little endian) ?
      //             AFTER test, it works - BUT why ? <check compiler options>
      ((TU32*)(Du32NVDM_Addr_Param_Data + u16Offset))[0]= u32Value;
    }
    else
    {
      //16 bits write.
      //TODO > SSPR: Don't use pointer, write byte-by-byte (little endian) ?
      //             AFTER test, it works - BUT why ? <check compiler options>
      ((TU16*)(Du32NVDM_Addr_Param_Data + u16Offset))[0]= (TU16) u32Value;
    }
  }
}




////////////////////////////////////////////////////////////////////////////////
// Functions (Exported)
////////////////////////////////////////////////////////////////////////////////

//============================================================================//
// NAME : u32CustomParams_eParameter_Get
//============================================================================//
// ROLE : Get parameter according offset, size, position
//----------------------------------------------------------------------------//
// INPUT  : size, offset and position
// OUTPUT : Readed value from NVDM
//============================================================================//
TU32 u32CustomParams_eParameter_Get(TU8 u8Size, TU8 u8Position, TU16 u16Offset)
{
	TU32 u32RetVal;
  TU8  u8Data;

  //read in RAM
  if (u8Size > 8)
  {
  	if(u8Size > 16)
    {
    	//read 32bits
      //TODO > SSPR: Don't use pointer, write byte-by-byte (little endian) ?
      //             AFTER test, it works - BUT why ? <check compiler options>
      u32RetVal = ((TU32*)(Du32NVDM_Addr_Param_Data + u16Offset))[0];
    }
    else
    {
    	//read 16bits.
      //TODO > SSPR: Don't use pointer, write byte-by-byte (little endian) ?
      //             AFTER test, it works - BUT why ? <check compiler options>
      u32RetVal = (TU32)((TU16*)(Du32NVDM_Addr_Param_Data + u16Offset))[0];
    }
  }
  else
  {
  	//read 8bits
    u8Data =  (TU32) ((TU8*)(Du32NVDM_Addr_Param_Data + u16Offset))[0];

    if (u8Size < 8)
    {
    	//apply size and position mask
      u32RetVal = (TU32) ((u8Data>>u8Position) & (0xFF>>(8-u8Size)));
    }
    else
    {
    	//nothing : keep full 8bits-data
      u32RetVal = (TU32) u8Data;
    }
  }

	return u32RetVal;
}



//============================================================================//
// NAME : vCustomParams_eInit
//============================================================================//
// ROLE : Custom Param initialization routine
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT :
//============================================================================//
void vCustomParams_eInit(void)
{
   TU32 u32AppParamConfig_StartAddr;
   TU32 u32AppParamConfig_Size;
   TU32 u32AppParamAddr;
   TU16 u16ChecksumConfigId;
   TU16 u16ChecksumParams;

   // Set global pointer on params configs.
   pstCustomParams_iParamConfig = (TstCustomParams_eParamConfig*) Du32CustomParams_eParamConfigStart;

   // check param config table address
   u32AppParamConfig_StartAddr = (TU32)((TU8*) Du32CustomParams_eParamStartAddr);
   u32AppParamConfig_Size =      (TU32)((TU8*) Du32CustomParams_eParamSize);

   if( (u32AppParamConfig_StartAddr + u32AppParamConfig_Size) < (TU32) Du32CustomParams_eParamStartAddr)
   {
      // Du32CustomParams_eAppParamConfigStartAddress out of range
      u32CustomParams_iNbOfParameterConfigured = 0;
   }
   else
   {
      u32CustomParams_iNbOfParameterConfigured = Du32CustomParams_eNbOfAppParam;

      if (u32CustomParams_iNbOfParameterConfigured > Du32CustomParams_eNbOfAppParamMax)
      {
         u32CustomParams_iNbOfParameterConfigured = Du32CustomParams_eNbOfAppParamMax;
      }
      else {} // empty else

      // check that param config is not going out of range
      u32AppParamAddr = (TU32) Du32CustomParams_eParamConfigStart + (u32CustomParams_iNbOfParameterConfigured*sizeof(TstCustomParams_eParamConfig));

      if(u32AppParamAddr > (u32AppParamConfig_StartAddr + u32AppParamConfig_Size))
      {
         // Nb of parameter in the config makes the config table going out of range
         // Limit the number of parameter to avoid going out of range:
         u32CustomParams_iNbOfParameterConfigured = (TU32) ((u32AppParamConfig_StartAddr + u32AppParamConfig_Size) - (TU32) Du32CustomParams_eParamConfigStart)/sizeof(TstCustomParams_eParamConfig);
      }
      else
      {} // empty else
   }

   // calculate checksum for ConfigId.
   u16ChecksumConfigId = u16CustomParams_iCRC16_Calc( (TU8*)Du32CustomParams_eParamConfigStart, (Du16MNVDM_iParamDataSize - Du32CustomParams_eSpareSize - Du32CustomParams_eNbParamSize) );
   //																																			PararmCrcBeginAddr   (      PararmCrcEndAddr -                PararmCrcBeginAddr)
   u16ChecksumParams   = u16CustomParams_iCRC16_Calc( (TU8*)Du32NVDM_Addr_Param_MappingBanner, (Du32NVDM_Addr_Param_CS - Du32NVDM_Addr_Param_MappingBanner) );

   // Test checksum from configId, set to default values if needed and update ConfigId.
   if(u16ChecksumConfigId != *((TU16*)Du32NVDM_Addr_Param_ConfigID) )
   {
      // Set config to defaults values.
      vCustomParams_iUpdateDefault_Exe();

      // Update ConfigId with new CRC.
      *((TU16*)Du32NVDM_Addr_Param_ConfigID)= u16ChecksumConfigId;
   }
   else if(u16ChecksumParams != *((TU16*)(Du32NVDM_Addr_Param_CS)) )
   {
      // For params, just set defaults values.
      vCustomParams_iUpdateDefault_Exe();
   }
}

//============================================================================//
// NAME : s32CustomParams_eWriteParameter_Exe
//============================================================================//
// ROLE : custom diag routine
//----------------------------------------------------------------------------//
// INPUT  :
// OUTPUT : < 0 if error (by ident, etc.), else: data size (number of bits).
//============================================================================//
TS32 s32CustomParams_eWriteParameter_Exe(TU16 u16ParamIdent, TU32* u32ParamValue)
{
   TS32 s32RetVal;
   TU16 u16TableIndex;  // Index in the Config Table where config of PARAMi is located
   TU16 u16Offset;
   TU8  u8Size;
   TU8  u8Position;

   u16TableIndex = 0xFFFF;
   u16Offset     = 0;
   u8Size        = 0;
   u8Position    = 0;

   // ============= 1) Find the address of the parameter config table at Param config pointer address =============
   // Check address range in Application code area between __AppParamConfig_start and __AppParamConfig_size
   if ( (((TU32)pstCustomParams_iParamConfig) < Du32CustomParams_eParamConfigStart) 
   	 || (((TU32)pstCustomParams_iParamConfig) > Du32CustomParams_eParamEndAddr) ) 
   {
      //error : this address is outranged
      u32CustomParams_iNbOfParameterConfigured=0;
      s32RetVal = Ds32CustomParams_eBadPtr;
   }
   else
   {
      // ============= 2) Search for parameter number Ident =========================================================
      s32RetVal= s32CustomParams_iIndexFromIndent_Get(u16ParamIdent);

      // If >= 0, s32RetVal = table index.
      if(s32RetVal >= 0)
      {
         u16TableIndex= (TU16) s32RetVal;

         // ============= 3) Check that value to write is within defined Min-Max range ================================
         if ( (*u32ParamValue < pstCustomParams_iParamConfig[u16TableIndex].u32MinValue) ||
              (*u32ParamValue > pstCustomParams_iParamConfig[u16TableIndex].u32MaxValue) )
         {
            // Set defaults values.
            s32RetVal = Ds32CustomParams_eBadMinMaxRange;
         }
         else
         {
            // ============= 4) Read offset value in order to find current data in the RAM ==========================
            u16Offset = pstCustomParams_iParamConfig[u16TableIndex].u16AddressOffset;

            if ((TU32) u16Offset > Du16MNVDM_iParamDataSize)
            {
              //error : outranged
              s32RetVal = Ds32CustomParams_eBadOffset;
            }
            else
            {
               // ============= 5) Adjust mask with the actual size and position =========================
               u8Size= pstCustomParams_iParamConfig[u16TableIndex].u8Size;
               u8Position= pstCustomParams_iParamConfig[u16TableIndex].u8Position;

               // Write parameter to NVDM memory.
               vCustomParams_iParameter_Set(u8Size, u8Position, u16Offset, *u32ParamValue);

               // Ok, value written.
               s32RetVal = (TS32)u8Size; //Ds32CustomParams_eSuccess;
            }
         }
      }
   }

   // Return value.
   return s32RetVal;
}

//============================================================================//
// NAME : s32CustomParams_eReadParameter_Exe
//============================================================================//
// ROLE : return Application Parameter value
//----------------------------------------------------------------------------//
// INPUT :
// OUTPUT : Return -1 if error, else: data size (number of bits).
//============================================================================//
TS32 s32CustomParams_eReadParameter_Exe(TU16 u16ParamIdent, TU32* u32ParamValue)
{
   TS32 s32RetVal;
   TU16 u16TableIndex; // Index in the Config Table where config of PARAMi is located
   TU16 u16Offset;
   TU8 u8Size;
   TU8 u8Position;

   u16TableIndex = 0xFFFF;
   u16Offset     = 0;
   u8Size        = 0;
   u8Position    = 0;

   // ============= 1) Find the address of the parameter config table at Param config pointer address =============
   // Check address range in Application code area between __AppParamConfig_start and __AppParamConfig_size
   if ( (((TU32)pstCustomParams_iParamConfig) < Du32CustomParams_eParamConfigStart) 
   	||  (((TU32)pstCustomParams_iParamConfig) > Du32CustomParams_eParamEndAddr) )
   {
      //error : this address is outranged
      u32CustomParams_iNbOfParameterConfigured=0;
      s32RetVal = Ds32CustomParams_eBadPtr;
   }
   else
   {
      // ============= 2) Search for parameter number Ident =========================================================
      s32RetVal = s32CustomParams_iIndexFromIndent_Get(u16ParamIdent);

      // If s32RetVal >= 0, = ident index.
      if (s32RetVal >= 0)
      {
         // Get index.
         u16TableIndex = (TU16)s32RetVal;

         // ============= 3) With consideration of actual size and position within the 32-bits-value =========
         u8Size = pstCustomParams_iParamConfig[u16TableIndex].u8Size;
         u8Position = pstCustomParams_iParamConfig[u16TableIndex].u8Position;

         //Read offset value in order to find data in the RAM
         u16Offset = pstCustomParams_iParamConfig[u16TableIndex].u16AddressOffset;

         if ((TU32) u16Offset > Du16MNVDM_iParamDataSize)
         {
            //error : outranged
            s32RetVal = Ds32CustomParams_eBadOffset;
         }
         else
         {
            // Get value
            *u32ParamValue = u32CustomParams_eParameter_Get(u8Size, u8Position, u16Offset);

            s32RetVal = (TS32)u8Size; //Ds32CustomParams_eSuccess;
         }
      }
   }

   return s32RetVal;
}


//============================================================================//
// NAME : s32CustomParams_eReadConfig_Exe
//============================================================================//
// ROLE : Get a copy of the parameter configuration (from flash to ram)
//----------------------------------------------------------------------------//
// INPUT  : ParamID to copy, pstConfig (pointer on the config struct)
// OUTPUT : Return BAD_PTR if bad pointer (address range).
//============================================================================//
TS32 s32CustomParams_eReadConfig_Exe(TU16 u16ParamIdent, TstCustomParams_eParamConfig* pstConfig)
{
   TS32 s32RetVal;
   TU16 u16TableIndex; // Index in the Config Table where config of PARAMi is located
   TU16 u16Offset;

   u16TableIndex = 0xFFFF;
   u16Offset     = 0;

   // ============= 1) Find the address of the parameter config table at Param config pointer address =============
   // Check address range in Application code area between __AppParamConfig_start and __AppParamConfig_size
   if ( (((TU32)pstCustomParams_iParamConfig) < Du32CustomParams_eParamConfigStart) ||
        (((TU32)pstCustomParams_iParamConfig) > Du32CustomParams_eParamEndAddr) )
   {
      //error : this address is outranged
      u32CustomParams_iNbOfParameterConfigured = 0;
      s32RetVal = Ds32CustomParams_eBadPtr;
   }
   else
   {
      // ============= 2) Search for parameter number Ident =========================================================
      s32RetVal = s32CustomParams_iIndexFromIndent_Get(u16ParamIdent);

      // If s32RetVal >= 0, = ident index.
      if (s32RetVal >= 0)
      {
         // Get index.
         u16TableIndex = (TU16) s32RetVal;

         // Copy param from flash to pointer.
         pstConfig->u16AddressOffset= pstCustomParams_iParamConfig[u16TableIndex].u16AddressOffset;
         pstConfig->u16Ident=         pstCustomParams_iParamConfig[u16TableIndex].u16Ident;
         pstConfig->u32DefaultValue=  pstCustomParams_iParamConfig[u16TableIndex].u32DefaultValue;
         pstConfig->u32MaxValue=      pstCustomParams_iParamConfig[u16TableIndex].u32MaxValue;
         pstConfig->u32MinValue=      pstCustomParams_iParamConfig[u16TableIndex].u32MinValue;
         pstConfig->u8Position=       pstCustomParams_iParamConfig[u16TableIndex].u8Position;
         pstConfig->u8Size=           pstCustomParams_iParamConfig[u16TableIndex].u8Size;

         s32RetVal = Ds32CustomParams_eSuccess;
      }
   }

   return s32RetVal;
}

