/*****************************************************************************/
/*CD NAME     : PARAMETER_READ                                               */
/*****************************************************************************/
/*CD ROLE     : Restore a parameter from EEPROM                              */
/*****************************************************************************/
/*  PROJET    : Driver Generique                                             */
/*  CREE      : 02-05-13          AUTEUR : P.Couronné                        */
/*****************************************************************************/
/*  Evolution :                                                              */
/*     V1.00 : Creation                                                      */
/*****************************************************************************/
#include "t5vm.h"
#include "std_type.h"
#include "monitor.h"
#include "straton_map.h"
#include "param.h"

/*----------------------------------------------------------------------------------------
TYPE MEMO
------------------------------------------------------------------------------------------
typedef char            T5_CHAR;    SINT     short signed integer on 8 bits
typedef signed short    T5_SHORT;   INT      signed integer on 16 bits
typedef signed long     T5_LONG;    DINT     signed integer on 32 bits
typedef unsigned char   T5_BOOL;    BOOL     boolean (TRUE / FALSE)
typedef unsigned char   T5_BYTE;    USINT    unsigned interger on 8 bits
typedef unsigned short  T5_WORD;    UINT     unsigned interger on 16 bits
typedef unsigned long   T5_DWORD;   UDINT    unsigned interger on 32 bits
typedef float           T5_REAL;    REAL     single precision floating point on 32 bits
typedef double          T5_LREAL;   LREAL    double precision floating point on 64 bits
----------------------------------------------------------------------------------------*/

/* PARAM_RD - Source code (requires t5vm.h include) */
/* Function block */

/*====================*/
/* Argument list */
/*====================*/

#define _P_ID   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_REQTYPE   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_VALUE   (*((T5_PTWORD)(T5GET_DBDATA16(pBase))+pArgs[2]))
#define _P_RET   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[3]))

/*====================*/
/* private block data */
/*====================*/

typedef struct
{
    T5_DWORD dwData; /* TODO: replace dwData by the items you need */
} _str_FB_PARAM_RD;


/*====================*/
/* Additional defines */
/*====================*/
/* Possible values for input argument _P_REQTYPE */
#define REQTYPE_RAW        (T5_BYTE)0
#define REQTYPE_MIN        (T5_BYTE)1
#define REQTYPE_MAX        (T5_BYTE)2
#define REQTYPE_DEFAULT    (T5_BYTE)3
#define REQTYPE_TESTED     (T5_BYTE)4
/* Returned values for output _P_RET */
#define RET_IDLE    (T5_SHORT)0
#define RET_OK      (T5_SHORT)1
#define RET_ARG     (T5_SHORT)2
#define RET_PTR     (T5_SHORT)3
#define RET_NOID    (T5_SHORT)4
#define RET_OFFSET    (T5_SHORT)5
#define RET_OKDEF    (T5_SHORT)6

/* Address of Param Config Pointer inside Flash memory mapping */
/* check compliance with API.cfg and spec P212125 req#360 req#362 */
//extern unsigned char __monitor_param_start[];

#define Du32FlashAreaForParamCfgStartAddr   (T5_DWORD)(Du32Straton_eAppParamConfig_start)
#define Du32FlashAreaForParamCfgEndAddr     (T5_DWORD)((T5_DWORD)Du32Straton_eAppParamConfig_start + Du32Straton_eAppParamConfig_size)

//#define Du32FlashAddr_ParamCfg               (__monitor_param_start   + 0x5FD0)
//#define Dtu32FlashAddr_ParamCfg_Contents     ((T5_DWORD *)  (__monitor_param_start   + 0x00005FD0))


/*====================*/
/* Additional typedef */
/*====================*/

/*====================*/
/* Private Data */
/*====================*/
TstParam_AppliParamConfig* pstParam_eAppliParamConfig;//Table of structure Config Table for all parameters
T5_DWORD u32ParamCfgTableFlashAddr; //Address value where Param config table is located (somewhere between __AppParamConfig_start and __AppParamConfig_size)
T5_WORD u16NumberOfParameters; //Actual number of parameters indicated in the fisrt WORD of the Config Table

/*====================*/
/* handler */
/*====================*/

T5_DWORD PARAM_RD (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   _str_FB_PARAM_RD *pData;
   T5_WORD u16TableIndex; // Index in the Config Table where config of PARAMi is located
   T5_WORD u16Data; //Requested item of PARAMi config from Config Table (16 bits)
   T5_WORD u16Calculation;
   T5_WORD u16Index; //Loop index
   T5_BYTE u8Data; //Requested item of PARAMi config from Config Table (8 bits)
   T5_BYTE u8Size;
   T5_BYTE u8Position;
   T5_BOOL u1Found;

   pData = (_str_FB_PARAM_RD *)pInst;
   
   u1Found = FALSE;
   u16Index = 0;
   u16TableIndex = 0xFFFF;
   u16Data = 0xFFFF;
   u16Calculation = 0;
   u8Data = 0xFF;
   u8Size=0;
   u8Position=0;
   u32ParamCfgTableFlashAddr=0;
   u16NumberOfParameters=0;
   
  
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* activates the function block */
      
      // ============= 1) Find the address of the parameter config table at Param config pointer address ============= 
      // Contents at ParamConfigPointerAddress = Address of the Config Table
      // u32ParamCfgTableFlashAddr=Dtu32FlashAddr_ParamCfg_Contents[0];
      // **NON CONFORMITY** : ACTIGRAF uses fixed address 0x1FF510 and does not indicate this Param Config Pointer at all
      u32ParamCfgTableFlashAddr = Du32FlashAreaForParamCfgStartAddr; //0x1FF510; /* **TO BE REMOVED once pointer address available** */
      // Check address range in Application code area between __AppParamConfig_start and __AppParamConfig_size
      if ((u32ParamCfgTableFlashAddr<Du32FlashAreaForParamCfgStartAddr) || (u32ParamCfgTableFlashAddr>Du32FlashAreaForParamCfgEndAddr))
      {
        //error : this address is outranged
        u16NumberOfParameters=0;
        _P_RET = RET_PTR;
      }
      else
      {
        // Table is at this adress. Its contents starts with Nb of parameters then 256*12bytes for 256 parameters

        // ============= 2) Read the total number of parameters at the beginning of the Config Table ================== 
        u16NumberOfParameters = ((T5_WORD *)u32ParamCfgTableFlashAddr)[0];
        // The rest of the Config Table starts after. See spec P212125 req#712
        pstParam_eAppliParamConfig = (TstParam_AppliParamConfig*)(u32ParamCfgTableFlashAddr+2);
        // ============= 3) Search for parameter number _P_ID ========================================================= 
        // Check that _P_ID is part of actual number of parameters
        if (_P_ID<u16NumberOfParameters)
        {
            //Parameters are saved accordingly to their ID order. Check ID anyway.
            u16Calculation = pstParam_eAppliParamConfig[_P_ID].usIdent;
            if (_P_ID==u16Calculation)
            {
                u16TableIndex=_P_ID;
                u1Found=TRUE;
            }
            else
            {
                //Search for the ID
                u16Index=0;
                u1Found=FALSE;
                u16TableIndex=0xFFFF;
                while ((u16Index<u16NumberOfParameters) && (FALSE==u1Found))
                {
                    u16Calculation = pstParam_eAppliParamConfig[u16Index].usIdent;
                    if (_P_ID ==u16Calculation)
                    {
                        u1Found=TRUE;
                        u16TableIndex=u16Index;
                    }
                    u16Index=u16Index+1;
                }
            }
            // ============= 4) Handle associated request =========================================================== 
            if (TRUE==u1Found)
            {
                // ============= 5) With consideration of actual size and position within the 16-bits-value =========
                u8Size=pstParam_eAppliParamConfig[u16TableIndex].ucSize;
                u8Position=pstParam_eAppliParamConfig[u16TableIndex].ucPosition;
                 
                switch (_P_REQTYPE)
                {
                    case REQTYPE_RAW:
                        //Read offset value in order to find data in the RAM
                        u16Calculation = pstParam_eAppliParamConfig[u16TableIndex].usAddressOffset;
                        if ((T5_DWORD)u16Calculation>Du16MNVDM_iAppParamSize)
                        {
                            //error : outranged
                            _P_RET = RET_OFFSET;
                        }
                        else
                        {
                            //read in RAM
                            if (u8Size>8)
                            {
                                //read 16bits
                                u16Data = ((T5_WORD *)(Du32API_Addr_Appli_Param + u16Calculation))[0];
                            }
                            else
                            {
                                //read 8bits
                                u8Data = ((T5_BYTE *)(Du32API_Addr_Appli_Param + u16Calculation))[0];
                                if (u8Size<8)
                                {
                                    //apply size and position mask
                                    u16Data = (T5_WORD) ((u8Data>>u8Position) & (0xFF>>(8-u8Size)));
                                }
                                else
                                {
                                    //nothing : keep full 8bits-data
                                    u16Data = (T5_WORD)u8Data;
                                }
                            }
                
                            _P_RET = RET_OK;
                        }
                        
                        break;
                    case REQTYPE_MIN:
                        //Read Min value
                        u16Data = pstParam_eAppliParamConfig[u16TableIndex].usMinValue;
                        _P_RET = RET_OK;
                        break;
                    case REQTYPE_MAX:
                        //Read Max value
                        u16Data = pstParam_eAppliParamConfig[u16TableIndex].usMaxValue;
                        _P_RET = RET_OK;
                        break;
                    case REQTYPE_DEFAULT:
                        //Read default value
                        u16Data = pstParam_eAppliParamConfig[u16TableIndex].usDefaultValue;
                        _P_RET = RET_OK;
                        break;
                    case REQTYPE_TESTED:
                        //Read offset value in order to find data in the RAM
                        u16Calculation = pstParam_eAppliParamConfig[u16TableIndex].usAddressOffset;
                        if ((T5_DWORD)u16Calculation>Du16MNVDM_iAppParamSize)
                        {
                            //error : outranged
                            _P_RET = RET_OFFSET;
                        }
                        else
                        {
                            //read in RAM
                            if (u8Size>8)
                            {
                                //read 16bits
                                u16Data = ((T5_WORD *)(Du32API_Addr_Appli_Param   + u16Calculation))[0];
                            }
                            else
                            {
                                //read 8bits
                                u8Data = ((T5_BYTE *)(Du32API_Addr_Appli_Param + u16Calculation))[0];
                                if (u8Size<8)
                                {
                                    //apply size and position mask
                                    u16Data = (T5_WORD) ((u8Data>>u8Position) & (0xFF>>(8-u8Size)));
                                }
                                else
                                {
                                    //nothing : keep full 8bits-data
                                    u16Data = (T5_WORD)u8Data;
                                }
                            }
                            
                            //Check valid range
                            if (    (u16Data>pstParam_eAppliParamConfig[u16TableIndex].usMaxValue)
                                    || (u16Data < pstParam_eAppliParamConfig[u16TableIndex].usMinValue)
                               )
                            {
                                //send default value instead
                                u16Data = pstParam_eAppliParamConfig[u16TableIndex].usDefaultValue;
                                _P_RET = RET_OKDEF;
                            }
                            else
                            {
                                //keep raw value as it is
                            _P_RET = RET_OK;
                            }
                        }
                        break;
                    default:
                        //error : invalid request
                        _P_RET = RET_ARG;
                        break;
                }
            }
            else
            {
                //error : ID was not found within the u16NumberOfParameters
                _P_RET = RET_NOID;
            }
        }
        else
        {
            //error : requested ID number is outranged compared to the encoded actual number of parameters
            _P_RET = RET_ARG;
        }
      }
      _P_VALUE=u16Data;
      return 0L;

   case T5FBCMD_INITINSTANCE :
      /* initialize private data */
      /* enter your code here... */
      return 0L;

   case T5FBCMD_EXITINSTANCE :
      /* release private data */
      /* enter your code here... */
      return 0L;

   case T5FBCMD_HOTRESTART :
      /* actuate private data for hot restart */
      /* enter your code here... */
      return 0L;

   case T5FBCMD_SIZEOFINSTANCE :
      /* dont change that */
      return (T5_DWORD)sizeof(_str_FB_PARAM_RD);

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/*====================*/
/* Undefine argument list */
/*====================*/

#undef _P_ID
#undef _P_REQTYPE
#undef _P_VALUE
#undef _P_RET







