/*****************************************************************************/
/*CD NAME     : PARAMETER_WRITE                                              */
/*****************************************************************************/
/*CD ROLE     : Write a parameter into   EEPROM                              */
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

/* PARAM_WR - Source code (requires t5vm.h include) */
/* Function */

/* Argument list */

#define _P_ID   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_VALUE   (*((T5_PTWORD)(T5GET_DBDATA16(pBase))+pArgs[1]))
#define _P_Q   (*((T5_PTSHORT)(T5GET_DBDATA16(pBase))+pArgs[2]))

/*====================*/
/* Additional defines */
/*====================*/

/* Returned values for output _P_Q */
#define RET_IDLE    (T5_SHORT)0
#define RET_OK      (T5_SHORT)1
#define RET_ARG     (T5_SHORT)2
#define RET_PTR     (T5_SHORT)3
#define RET_NOID    (T5_SHORT)4
#define RET_OFFSET    (T5_SHORT)5

/* Address of Param Config Pointer inside Flash memory mapping */
/* check compliance with API.cfg and spec P212125 req#360 req#362 */
//extern unsigned char __monitor_param_start[];

#define Du32FlashAreaForPointerCfgStartAddr   (T5_DWORD)(Du32Straton_eAppParamConfig_start)
#define Du32FlashAreaForPointerCfgEndAddr     (T5_DWORD)((T5_DWORD)Du32Straton_eAppParamConfig_start + Du32Straton_eAppParamConfig_size)

//#define Du32FlashAddr_ParamCfg               (__monitor_param_start   + 0x5FD0)
//#define Dtu32FlashAddr_ParamCfg_Contents     ((T5_DWORD *)  (__monitor_param_start   + 0x00005FD0))


/*====================*/
/* Additional typedef */
/*====================*/

/*====================*/
/* Private Data */
/*====================*/

TstParam_AppliParamConfig* pstTableOfAppliParamConfig;//Table of structure Config Table for all parameters
T5_DWORD u32ConfigTableFlashAddr; //Address value where Param config table is located (somewhere between __AppParamConfig_start and __AppParamConfig_size)
T5_WORD u16TotalNumberOfParameters; //Actual number of parameters indicated in the fisrt WORD of the Config Table


/* handler */

T5_DWORD PARAM_WR (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   T5_WORD u16TableIndex; // Index in the Config Table where config of PARAMi is located
   //T5_WORD u16Data; //Requested item of PARAMi config from Config Table 16bits
   T5_WORD u16Calculation;
   T5_WORD u16Index; //Loop index
   T5_BYTE u8Data; //Requested item of PARAMi config from Config Table 8bits
   T5_BYTE u8Size;
   T5_BYTE u8Position;
   T5_BOOL u1Found;
   
   u1Found = FALSE;
   u16Index = 0;
   u16TableIndex = 0xFFFF;
   u16Calculation = 0;
   u8Data = 0xFF;
   u8Size=0;
   u8Position=0;
   u32ConfigTableFlashAddr=0;
   u16TotalNumberOfParameters=0;
   
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      
      // ============= 1) Find the address of the parameter config table at Param config pointer address ============= 
      // Contents at ParamConfigPointerAddress = Address of the Config Table
      // u32ConfigTableFlashAddr=Dtu32FlashAddr_ParamCfg_Contents[0];
      // **NON CONFORMITY** : ACTIGRAF uses fixed address 0x1FF510 and does not indicate this Param Config Pointer at all
      u32ConfigTableFlashAddr = Du32FlashAreaForPointerCfgStartAddr; //0x1FF510; /* **TO BE REMOVED once pointer address available** */
      // Check address range in Application code area between __AppParamConfig_start and __AppParamConfig_size
      if ((u32ConfigTableFlashAddr<Du32FlashAreaForPointerCfgStartAddr) || (u32ConfigTableFlashAddr>Du32FlashAreaForPointerCfgEndAddr))
      {
        //error : this address is outranged
        u16TotalNumberOfParameters=0;
        _P_Q = RET_PTR;
      }
      else
      {
        // Table is at this adress. Its contents starts with Nb of parameters then 256*12bytes for 256 parameters

        // ============= 2) Read the total number of parameters at the beginning of the Config Table ================== 
        u16TotalNumberOfParameters = ((T5_WORD *)u32ConfigTableFlashAddr)[0];
        // The rest of the Config Table starts after. See spec P212125 req#712
        pstTableOfAppliParamConfig = (TstParam_AppliParamConfig*)(u32ConfigTableFlashAddr+2);
        
        // ============= 3) Search for parameter number _P_ID ========================================================= 
        // Check that _P_ID is part of actual number of parameters
        if (_P_ID<u16TotalNumberOfParameters)
        {
            //Parameters are saved accordingly to their ID order. Check ID anyway.
            u16Calculation = pstTableOfAppliParamConfig[_P_ID].usIdent;
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
                while ((u16Index<u16TotalNumberOfParameters) && (FALSE==u1Found))
                {
                    u16Calculation = pstTableOfAppliParamConfig[u16Index].usIdent;
                    if (_P_ID ==u16Calculation)
                    {
                        u1Found=TRUE;
                        u16TableIndex=u16Index;
                    }
                    u16Index=u16Index+1;
                }
            }
             
            if (TRUE==u1Found)
            {
                // ============= 4) Check that value to write is within defined Min-Max range ================================
                if (    (_P_VALUE < pstTableOfAppliParamConfig[u16TableIndex].usMinValue)
                        || (_P_VALUE > pstTableOfAppliParamConfig[u16TableIndex].usMaxValue)
                   )
                {
                    //error : outranged
                    _P_Q = RET_ARG;
                }
                else
                {
                    // ============= 5) Read offset value in order to find current data in the RAM ==========================
                    u16Calculation = pstTableOfAppliParamConfig[u16TableIndex].usAddressOffset;
                    if ((T5_DWORD)u16Calculation>Du16MNVDM_iAppParamSize)
                    {
                        //error : outranged
                        _P_Q = RET_OFFSET;
                    }
                    else
                    {
                        // ============= 6) Adjust mask with the actual size and position ========================= 
                        u8Size=pstTableOfAppliParamConfig[u16TableIndex].ucSize;
                        
                        if (u8Size<16)
                        {
                           // ============= 7) Read current contents in RAM and insert value ======================
                            //u16Data = ((T5_WORD *)(Du32API_Addr_Appli_Param + u16Calculation))[0];
                            u8Data = ((T5_BYTE *)(Du32API_Addr_Appli_Param + u16Calculation))[0];
                            if (u8Size<8)
                            {
                                //read position
                                u8Position=pstTableOfAppliParamConfig[u16TableIndex].ucPosition;
                                //shift position :    (_P_VALUE<<u8Position)
                                //build mask and apply on current data :   { ~ [ (0xFF >> (8-u8Size)) << u8Position ] }
                                //add and write
                                //((T5_WORD *)(Du32API_Addr_Appli_Param + u16Calculation))[0]= (u16Data & ((0x00FF >> (8-u8Size)) << u8Position)) + (_P_VALUE<<u8Position);
                                ((T5_BYTE *)(Du32API_Addr_Appli_Param + u16Calculation))[0]=(u8Data & (T5_BYTE)(~(  (0xFF>>(8-u8Size)) << u8Position   )))+ (T5_BYTE)(_P_VALUE<<u8Position);
                            }
                            else
                            {
                                //byte format
                                //((T5_WORD *)(Du32API_Addr_Appli_Param + u16Calculation))[0]=(u16Data & 0xFF00) + (_P_VALUE & 0x00FF);
                                ((T5_BYTE *)(Du32API_Addr_Appli_Param + u16Calculation))[0]=(T5_BYTE)(_P_VALUE & 0x00FF);
                            }
                            
                        }
                        else
                        {
                            // keep word format and write it
                            ((T5_WORD *)(Du32API_Addr_Appli_Param + u16Calculation))[0]=_P_VALUE;
                        }
                        
                        _P_Q = RET_OK;
                    }
                }
                
            }
            else
            {
                //error : ID was not found within the u16TotalNumberOfParameters
                _P_Q = RET_NOID;
            }
        }
        else
        {
            //error : requested ID number is outranged compared to the encoded actual number of parameters
            _P_Q = RET_ARG;
        }
      }
      return 0L;

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_ID
#undef _P_VALUE
#undef _P_Q





