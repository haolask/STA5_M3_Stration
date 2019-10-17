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
//        UDS_APP : API Validation Test Application Layer
////////////////////////////////////////////////////////////////////////////////
// FILE : diag_app.c (SOURCE)
//----------------------------------------------------------------------------
// DESC : API Diagnostic Application Layer : handles execution and results
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 1783 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Sources/APP/COM/diag_app/diag_app.c $HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_BSP/Sources/BSP/COM/kwp_pro/kwp_pro.c $
// Updated the :$Date: 2017-04-07 23:10:02 +0800 (周五, 07 四月 2017) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
//#define DIAG_APP


////////////////////////////////////////////////////////////////////////////////
// INCLIB
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// INCUSER
////////////////////////////////////////////////////////////////////////////////
#include "diag_app.h"
//#include <string.h>
#include "monitor.h"
#include "straton_map.h"
#include "wiring.h"
#include "..\..\FILES\CUSTOM_PARAMS\CustomParams.h"


////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////

#define DstBoot_eBootIdent			(*((TstWVersion_eSWIdent *)(0x1803FF80)))

////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// TYPEDEF
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// INTERN
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// CODE
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Functions (Intern)
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Functions (Exported)
////////////////////////////////////////////////////////////////////////////////

//============================================================================//
// NAME : vADiag_eInit_Exe
//============================================================================//
// ROLE : Diagnostic function to initialize this DIAG module
//----------------------------------------------------------------------------//
// INPUT  : 
// OUTPUT : 
//============================================================================//
void vADiag_eInit_Exe(void)
{
	//
}


//============================================================================//
// NAME : s32ADiag_eRDBI_Exe
//============================================================================//
// ROLE : Diagnostic function to read DID
//----------------------------------------------------------------------------//
// INPUT  : Fu16DID 				DID value 
//          Fpu8Buffer 			Pointer to data buffer of the answer
//          Fu16LengthMax		Lenght maximal of the possible response
// OUTPUT : Response Size 	if Positive
//					<Reserved>			if 0 (Delayed request / request again)
//          Error Code    	if Negative
//============================================================================//
TS32 s32ADiag_eRDBI_Exe(TU16 Fu16DID, TU8 *Fpu8Buffer, TU16 Fu16LengthMax)
{
	TS32 	s32RetVal = Ds32ADiag_iSuccess;
	TU32  u32ParamValue;
	TU16 	u16Count,u16Count1, u16Size;
	TU8	u8SpaceFlg;
	TU8 *	pu8Data;

	if (Fpu8Buffer)
	{
		switch(Fu16DID)
		{
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// ActiGraf Banners (ReadOnly):
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			case Du16ADiag_iMulticS_Calib_Banner:
				u16Size = Du16ADiag_iActiGRAFBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					pu8Data = (TU8 *)((0x1807FF80));
					u16Count1 =0;
					u8SpaceFlg = 0;
					
					for(u16Count=0; u16Count<u16Size; )
					{
						if((pu8Data[u16Count1]) || ((u16Count > 0) && (u16Count < 4)))
						{
							Fpu8Buffer[u16Count] = pu8Data[u16Count1];
							u8SpaceFlg = 0;
							u16Count++;
						}					
						else
						{
							Fpu8Buffer[u16Count] = 0x20;
							if(!u8SpaceFlg)
							{
								u16Count++;
								u8SpaceFlg = 1;
							}
						}
						u16Count1++;
					}
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;

			case Du16ADiag_iMulticS_Boot_Banner:
				u16Size = Du16ADiag_iActiGRAFBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					pu8Data = (TU8 *)((0x1803FF80));//(TU8 *)Du32Straton_eActiGRAF_WorkBenchAppli_Banner;
					u16Count1 =0;
					u8SpaceFlg = 0;
					
					for(u16Count=0; u16Count<u16Size; )
					{
						if((pu8Data[u16Count1]) || ((u16Count > 0) && (u16Count < 4)))
						{
							Fpu8Buffer[u16Count] = pu8Data[u16Count1];
							u8SpaceFlg = 0;
							u16Count++;
						}					
						else
						{
							Fpu8Buffer[u16Count] = 0x20;
							if(!u8SpaceFlg)
							{
								u16Count++;
								u8SpaceFlg = 1;
							}
						}
						u16Count1++;
					}
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;
				
			case Du16ADiag_iMulticS_Monitor_BSP_Banner:
				u16Size = Du16ADiag_iActiGRAFBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					//pu8Data = (TU8 *) Du32Wiring_eActiGRAF_WorkBenchWiring_Banner;

					pu8Data = (TU8 *)((0x1827FF80));
					u16Count1 =0;
					u8SpaceFlg = 0;
					
					for(u16Count=0; u16Count<u16Size; )
					{
						if((pu8Data[u16Count1]) || ((u16Count > 0) && (u16Count < 4)))
						{
							Fpu8Buffer[u16Count] = pu8Data[u16Count1];
							u8SpaceFlg = 0;
							u16Count++;
						}					
						else
						{
							Fpu8Buffer[u16Count] = 0x20;
							if(!u8SpaceFlg)
							{
								u16Count++;
								u8SpaceFlg = 1;
							}
						}
						u16Count1++;
					}
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;
				
			case Du16ADiag_iActiGRAF_CustomStraton_Banner:
				u16Size = Du16ADiag_iActiGRAFBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					pu8Data = (TU8 *)((0x1843FF80));
					u16Count1 =0;
					u8SpaceFlg = 0;
					
					for(u16Count=0; u16Count<u16Size; )
					{
						if((pu8Data[u16Count1]) || ((u16Count > 0) && (u16Count < 4)))
						{
							Fpu8Buffer[u16Count] = pu8Data[u16Count1];
							u8SpaceFlg = 0;
							u16Count++;
						}					
						else
						{
							Fpu8Buffer[u16Count] = 0x20;
							if(!u8SpaceFlg)
							{
								u16Count++;
								u8SpaceFlg = 1;
							}
						}
						u16Count1++;
					}
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;
				
			case Du16ADiag_iActiGRAF_CustomWring_Banner:
				u16Size = Du16ADiag_iActiGRAFBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					pu8Data = (TU8 *)((0x1847FF80));
					u16Count1 =0;
					u8SpaceFlg = 0;
					
					for(u16Count=0; u16Count<u16Size; )
					{
						if((pu8Data[u16Count1]) || ((u16Count > 0) && (u16Count < 4)))
						{
							Fpu8Buffer[u16Count] = pu8Data[u16Count1];
							u8SpaceFlg = 0;
							u16Count++;
						}					
						else
						{
							Fpu8Buffer[u16Count] = 0x20;
							if(!u8SpaceFlg)
							{
								u16Count++;
								u8SpaceFlg = 1;
							}
						}
						u16Count1++;
					}
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;
				
			case Du16ADiag_iActiGRAF_CustomEmWizard_Banner:
				u16Size = Du16ADiag_iActiGRAFBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					pu8Data = (TU8 *)((0x18FFFF80));
					u16Count1 =0;
					u8SpaceFlg = 0;
					
					for(u16Count=0; u16Count<u16Size; )
					{
						if((pu8Data[u16Count1]) || ((u16Count > 0) && (u16Count < 4)))
						{
							Fpu8Buffer[u16Count] = pu8Data[u16Count1];
							u8SpaceFlg = 0;
							u16Count++;
						}					
						else
						{
							Fpu8Buffer[u16Count] = 0x20;
							if(!u8SpaceFlg)
							{
								u16Count++;
								u8SpaceFlg = 1;
							}
						}
						u16Count1++;
					}
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;

				

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// Custom App Parameters:
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			default :
				// VMS: 0x4000-0x42FF	: part of UDS VehicleManufacturerSpecific range
				//  =>  [0 ... 0x1FE] : MAX Ident. offset       (511 parameters MAX == 0x01FF)
				if( (Fu16DID >= Du16ADiag_iParam_BaseAddress) 
	 			 &&	(Fu16DID <  (Du16ADiag_iParam_BaseAddress + Du32CustomParams_eNbOfAppParamMax)) )
				{
					Fu16DID -= Du16ADiag_iParam_BaseAddress;		//reduce VMS offset => Ident. offset
			
					s32RetVal = 0;//s32CustomParams_eReadParameter_Exe(Fu16DID, &u32ParamValue);
			
					// Test return value and convert it to DIAG return value.
			    switch(s32RetVal)
			    {
			    	// case Ds32CustomParams_eSuccess	
						case  1:																	// data size (number of bits): [1..8, 16, 32(maximum)] bits
						case  2:
						case  3:
						case  4:
						case  5:
						case  6:
						case  7:
						case  8:
							if (1 <= Fu16LengthMax)
							{
								Fpu8Buffer[0] = (TU8) u32ParamValue;
								s32RetVal = 1;
							}
							else s32RetVal = Ds32ADiag_iTooLong;
							break;
						case 16:
							if (2 <= Fu16LengthMax)
							{
								Fpu8Buffer[0] = (TU8) (u32ParamValue >> 8);
								Fpu8Buffer[1] = (TU8) u32ParamValue;
								s32RetVal = 2;
							}
							else s32RetVal = Ds32ADiag_iTooLong;
							break;
						case 32:
							if (4 <= Fu16LengthMax)
							{
								Fpu8Buffer[0] = (TU8) (u32ParamValue >> 24);
								Fpu8Buffer[1] = (TU8) (u32ParamValue >> 16);
								Fpu8Buffer[2] = (TU8) (u32ParamValue >> 8);
								Fpu8Buffer[3] = (TU8) u32ParamValue;
								s32RetVal = 4;
							}
							else s32RetVal = Ds32ADiag_iTooLong;	
							break;
							
						case Ds32CustomParams_eIdentOutOfRange:
			      case Ds32CustomParams_eIdentNotFound:
			      	s32RetVal = Ds32ADiag_iOutOfRange;
			      	break;
			      	
			      //case Ds32CustomParams_eBadPtr:
			      //case Ds32CustomParams_eBadOffset:
			      //case Ds32CustomParams_eBadMinMaxRange:
			      default:																	// e.g. if data size == 0
			       	s32RetVal = Ds32ADiag_iError;
			        break;
					}				
				}
				else s32RetVal = Ds32ADiag_iOutOfRange;
				break;
		}
	}
	else s32RetVal = Ds32ADiag_iError;
	
	return s32RetVal;
}

#if 0
			case Du16ADiag_iActiGRAF_WorkBenchEmWizard_Banner:
				u16Size = Du16ADiag_iActiGRAFBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					pu8Data = (TU8 *) Du32Straton_eActiGRAF_WorkBenchEmWizard_Banner;
					for(u16Count=0; u16Count<u16Size; u16Count++)
					{
						Fpu8Buffer[u16Count] = pu8Data[u16Count];
					}		
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;
							
			case Du16ADiag_iActiGRAF_CustomAppli_Banner:
				u16Size = Du16ADiag_iActiGRAFBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					pu8Data = (TU8 *) Du32Straton_eActiGRAF_CustomAppli_Banner;
					for(u16Count=0; u16Count<u16Size; u16Count++)
					{
						Fpu8Buffer[u16Count] = pu8Data[u16Count];
					}
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;
				
			case Du16ADiag_iActiGRAF_CustomWiring_Banner:
				u16Size = Du16ADiag_iActiGRAFBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					pu8Data = (TU8 *) Du32Wiring_eActiGRAF_CustomWiring_Banner;
					for(u16Count=0; u16Count<u16Size; u16Count++)
					{
						Fpu8Buffer[u16Count] = pu8Data[u16Count];
					}
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;

			case Du16ADiag_iActiGRAF_CustomEmWizard_Banner:
				u16Size = Du16ADiag_iActiGRAFBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					pu8Data = (TU8 *) Du32Straton_eActiGRAF_CustomEmWizard_Banner;
					for(u16Count=0; u16Count<u16Size; u16Count++)
					{
						Fpu8Buffer[u16Count] = pu8Data[u16Count];
					}
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;
				
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// Custom App Param Banners:
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			case Du16ADiag_iParam_Mapping_Banner:
				u16Size = Du16MNVDM_iParamMappingBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					pu8Data = (TU8 *) Du32NVDM_Addr_Param_MappingBanner;
					for(u16Count=0; u16Count<u16Size; u16Count++)
					{
						Fpu8Buffer[u16Count] = pu8Data[u16Count];
					}
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;
				
			case Du16ADiag_iParam_Content_Banner:
				u16Size = Du16MNVDM_iParamContentBannerSize;
				if (u16Size <= Fu16LengthMax)
				{
					pu8Data = (TU8 *) Du32NVDM_Addr_Param_ContentBanner;
					for(u16Count=0; u16Count<u16Size; u16Count++)
					{
						Fpu8Buffer[u16Count] = pu8Data[u16Count];
					}
					s32RetVal = (TS32)u16Size;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;

#endif
//============================================================================//
// NAME : s32ADiag_eWDBI_Exe
//============================================================================//
// ROLE : Diagnostic function to write DID
//----------------------------------------------------------------------------//
// INPUT  : Fu16DID 				DID value 
//          Fpu8Buffer 			Pointer to data buffer of the request
//          Fu16LengthMax		Lenght maximal of the data
// OUTPUT : Response Size 	if Positive
//					<Reserved>			if 0 (Delayed request / request again)
//          Error Code    	if Negative
//============================================================================//
TS32 s32ADiag_eWDBI_Exe(TU16 Fu16DID, TU8 *Fpu8Buffer, TU16 Fu16LengthMax)
{
	TS32 	s32RetVal = Ds32ADiag_iSuccess;
	TU32  u32ParamValue;
	TU16 	u16Count, u16Size;
	TU8 *	pu8Data;

#if 0	
	if (Fpu8Buffer)
	{
		switch(Fu16DID)
		{
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// Custom App Param Banners:
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// TODO > SSPR: Banner should be linked to "Customer Param Config ID" - in administration point of view
			//              > THEN, Banner schould be recovered from 							- Additional information of "FLASH Customer Parameter Configuration" table // Not done today
			//              > THEN, at CustomParam INIT: Banner should be written to "Du32NVDM_Addr_Param_MappingBanner" - to be available in EEProm 
			//              > THEN: WDBI of this Banner should be take out !!!
			case Du16ADiag_iParam_Mapping_Banner:
				u16Size = Du16MNVDM_iParamMappingBannerSize;
				if (Fu16LengthMax <= u16Size)
				{
					pu8Data = (TU8 *) Du32NVDM_Addr_Param_MappingBanner;
					for(u16Count=0; u16Count<Fu16LengthMax; u16Count++)
					{
						pu8Data[u16Count] = Fpu8Buffer[u16Count];
					}
					s32RetVal = (TS32)Fu16LengthMax;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;
			
			// THIS Banner should be updated within the Parameter Data: by the DIAG-Tool !	
			// > IF Functional BLOCK writes also Data: THIS Banner must be updated dynamically !!!
			case Du16ADiag_iParam_Content_Banner:	
				u16Size = Du16MNVDM_iParamContentBannerSize;
				if (Fu16LengthMax <= u16Size)
				{
					pu8Data = (TU8 *) Du32NVDM_Addr_Param_ContentBanner;
					for(u16Count=0; u16Count<Fu16LengthMax; u16Count++)
					{
						pu8Data[u16Count] = Fpu8Buffer[u16Count];
					}
					s32RetVal = (TS32)Fu16LengthMax;
				}
				else s32RetVal = Ds32ADiag_iTooLong;
				break;		
				
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// Custom App Parameters:
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			default :	
				// THESE Parameter Data should be updated: by the DIAG-Tool !	
				// > IF Functional BLOCK writes also Data: THE Banner must be updated also dynamically !!!
				// VMS: 0x4000-0x42FF	: part of UDS VehicleManufacturerSpecific range
				//  =>  [0 ... 0x1FE] : MAX Ident. offset       (511 parameters MAX == 0x01FF)
				if( (Fu16DID >= Du16ADiag_iParam_BaseAddress) 
				 &&	(Fu16DID <  (Du16ADiag_iParam_BaseAddress + Du32CustomParams_eNbOfAppParamMax)) )
				{
					Fu16DID -= Du16ADiag_iParam_BaseAddress;		//reduce VMS offset => Ident. offset
			
					if(Fu16LengthMax)	//Lenght in BYTE
					{		 
						u32ParamValue = (TU32)Fpu8Buffer[0];
				
						// Process 32-bit MAX; Fu16LengthMax can be > 4, if other parametres must be written later
						if(Fu16LengthMax > 1)	
						{	
							u32ParamValue <<= 8; u32ParamValue += (TU32)Fpu8Buffer[1];
						}	
						if(Fu16LengthMax > 2)
						{
							u32ParamValue <<= 8; u32ParamValue += (TU32)Fpu8Buffer[2];
						}	
						if(Fu16LengthMax > 3)
						{
							u32ParamValue <<= 8; u32ParamValue += (TU32)Fpu8Buffer[3];
						}	
					
						s32RetVal = s32CustomParams_eWriteParameter_Exe(Fu16DID, &u32ParamValue);
						
						// Test return value and convert it to DIAG return value.
			      switch(s32RetVal)
			      {
			      	// case Ds32CustomParams_eSuccess	
							case  1:																// data size (number of bits): [1..8, 16, 32(maximum)] bits
							case  2:
							case  3:
							case  4:
							case  5:
							case  6:
							case  7:
							case  8:
								s32RetVal = 1;
								break;
							case 16:
								s32RetVal = 2;
								break;
							case 32:
								s32RetVal = 4;
								break;
			
							case Ds32CustomParams_eIdentOutOfRange:
							case Ds32CustomParams_eIdentNotFound:
			        	s32RetVal = Ds32ADiag_iOutOfRange;
			      		break;
			        
			        //case Ds32CustomParams_eBadPtr:
			        //case Ds32CustomParams_eBadOffset:
			        //case Ds32CustomParams_eBadMinMaxRange:
			      	default:																// e.g. if data size == 0
			        	s32RetVal = Ds32ADiag_iError; 	
			          break;
						}
						
						//TODO > SSPR: Check if request size was sufficient regarding the action //s32RetVal = Ds32ADiag_iTooLong
						//             - but ones written, it can't be locked here !!!
						//             e.g.: request == 1 BYTE,  but param == 4 BYTEs : 4 BYTEs are updated with the content of the BYTE !
						//             => TBC: UDS must check this case !!!
					}
					else s32RetVal = Ds32ADiag_iError;
				}
				else s32RetVal = Ds32ADiag_iOutOfRange;
				break;
		}
	}
	else s32RetVal = Ds32ADiag_iError;
#endif		
	return s32RetVal;
}
