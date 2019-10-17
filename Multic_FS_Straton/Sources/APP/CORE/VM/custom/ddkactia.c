/*****************************************************************************
T5BnsDrv.c : ACTIA - DDKC driver

DO NOT ALTER THIS !

(c) COPALP 2008
*****************************************************************************/

#include "t5vm.h"
#include "t5ddkc.h"
#include "..\UAPI\uapi.h"

/****************************************************************************/
/* properties

BUS:
    none

MASTER:
    0 : type (byte)
    1 : channel (byte)

SLAVE:
    0 : freq (float)
    4 : low (float)
    8 : high (float)

VAR:
    0 : mode (byte)

*/

/****************************************************************************/
/* sent by the compiler - properties */

#define _TYP_LSD    0
#define _TYP_HSD    1
#define _TYP_AN     2
#define _TYP_FQ     3

#define _MOD_DI     0
#define _MOD_DO     1
#define _MOD_AI     2
#define _MOD_AO     3
#define _MOD_ST     4
#define _MOD_FI     5
#define _MOD_FO     6

typedef struct
{
    T5_BYTE bType;
    T5_BYTE bChannel;
}
str_mst_prp;

typedef struct
{
    T5_REAL rFreq;
    T5_REAL rLow;
    T5_REAL rHigh;
}
str_slv_prp;

typedef struct
{
    T5_BYTE bMode;
}
str_var_prp;

/****************************************************************************/
/* objects in RAM for each node */

typedef struct
{
    T5_BYTE b;
}
str_node_bus;

typedef struct
{
    T5_DWORD dwIdent;
}
str_node_mst;

typedef struct
{
    T5_BYTE b;
}
str_node_slv;

typedef struct
{
    T5_BYTE b;
}
str_node_var;

/****************************************************************************/

static void _BuildMaster (str_node_mst *pNode)
{
#define LSD_OFFSET	ID_LSD01_220MA
#define HSD_OFFSET	ID_HSD01_500HZ_3A
#define AN_OFFSET		ID_AN01
#define FQ_OFFSET		ID_FQ01
	
    str_mst_prp *pProps = (str_mst_prp *)T5DDKH_GetProps (pNode);
    T5_DWORD dwIdent = 0;

    switch (pProps->bType)
    {
    case _TYP_LSD :
        dwIdent = LSD_OFFSET + pProps->bChannel -1;
        break;
    case _TYP_HSD :
        dwIdent = HSD_OFFSET + pProps->bChannel -1;
        break;
    case _TYP_AN  :
        dwIdent = AN_OFFSET + pProps->bChannel -1;
        break;
    case _TYP_FQ  :
        dwIdent = FQ_OFFSET + pProps->bChannel -1;
       break;
    }

    pNode->dwIdent = dwIdent;
}

static T5_DWORD _Open (str_node_bus *pNode)
{
    str_node_mst *pMst;
    str_node_slv *pSlv;
    str_slv_prp *pProps;
	 str_mst_prp *pPropm;

	 Input_Config_union_t		Input_Config;		// Structure to Setup Input Channels
	 Output_Config_struct_t  Output_Config;		// Structure to Setup PWM Channels

    pMst = (str_node_mst *)T5DDKH_GetChild (pNode);
    while (pMst)
    {
        /* for each master */
        pPropm = (str_mst_prp *)T5DDKH_GetProps (pMst);


		  
        pSlv = (str_node_slv *)T5DDKH_GetChild (pMst);
        while (pSlv)
        {
            /* for each slave */
            pProps = (str_slv_prp *)T5DDKH_GetProps (pSlv);

            // TODO: open slave
				switch(pPropm->bType)
				{

					case _TYP_LSD :
						break;
               case _TYP_HSD :
						Output_Config.Dither_Period_u16_2ms       = 0;
						Output_Config.Dither_Amplitude_u16_b2     = 0;
						Output_Config.PWM_Frequency_u16_b1_Hz     = 5000;
						Output_Config.PWM_Duty_Cycle_u16_b2       = 0;
						Configure_Output_Channel(pMst->dwIdent,&Output_Config);
						break;
					case _TYP_AN  :
						//AN conf
	               Input_Config.Analog_struct.Analog_Low_Pass_u32_b3_Hz = (U32_t)(1000*(pProps->rFreq));  
	               Input_Config.Analog_struct.Analog_Low_Threshold_Swicth_u16 = (U16_t)(1000*(pProps->rLow));  
	               Input_Config.Analog_struct.Analog_High_Threshold_Swicth_u16 = (U16_t)(1000*(pProps->rHigh));  
	               Configure_Input_Channel(pMst->dwIdent,IO_SET_ANALOG_SWITCH,&Input_Config);
						break;
               case _TYP_FQ  :
	               //FQ conf
                  Input_Config.Frequency_struct.Zero_Frequency_Timeout_u16_ms	= 1000;		// 1s timeout 
	               Input_Config.Frequency_struct.Frequency_Max_u32_b1_Hz			= 6500;		// 6.5 kHz Max frequency
	               Input_Config.Frequency_struct.Frequency_Polarity_u8			= IO_SET_RISING_EDGES;
	               Input_Config.Frequency_struct.Frequency_Mode_u16				= IO_SET_MULTI_EDGES_MEAS;
	               Configure_Input_Channel(pMst->dwIdent,IO_SET_FREQUENCY,&Input_Config);						
					   break;
					default:
						break;
				}				
				//Next slave
            pSlv = (str_node_slv *)T5DDKH_GetSibling (pSlv);
        }
        pMst = (str_node_mst *)T5DDKH_GetSibling (pMst);
    }
    return 0; /* 0=OK */
}

static T5_DWORD _Exchange (T5PTR_DB pDB, str_node_bus *pNode)
{
    str_node_mst *pMst;
    str_node_slv *pSlv;
    str_node_var *pVar;
    str_var_prp *pProps;
	 S32_t s32Val;

    /* for each master */
    pMst = (str_node_mst *)T5DDKH_GetChild (pNode);
    while (pMst)
    {
        /* use pMst->dwiDent */
        pSlv = (str_node_slv *)T5DDKH_GetChild (pMst);
        while (pSlv)
        {
            pVar = (str_node_var *)T5DDKH_GetChild (pSlv);
            while (pVar)
            {
                /* for each variable */
                pProps = (str_var_prp *)T5DDKH_GetProps (pVar);
                switch (pProps->bMode)
                {
                   case _MOD_DI :
                       s32Val = Read_Input_Data(pMst->dwIdent,IO_PARAM_SWITCH_DEBOUNCED );
                       if(0 == s32Val)
			   			  {	  
		   				     T5DDKH_WriteVarBool(pDB, pVar , 0);
			   			  }
	   					  else
		   				  {
			   				  T5DDKH_WriteVarBool(pDB, pVar , 1);  
				   		  }	  
                       break;
                   case _MOD_DO :
		   				  if(0 == T5DDKH_ReadVarBool (pDB, pVar) )
	   					  {
		   					  s32Val  = 0;
			   			  }
		   				  else
	   					  {
                          s32Val  = 10000;
			   			  }	  
                       Write_Output_Data (pMst->dwIdent,IO_PARAM_DUTY_CYCLE,s32Val );
                       break;
                   case _MOD_AI :
                       s32Val = Read_Input_Data(pMst->dwIdent,IO_PARAM_ANALOG_FILTERED );
				   		  T5DDKH_WriteVarLong(pDB, pVar ,(T5_LONG ) s32Val);
                       break;
                   case _MOD_AO :
	   					  s32Val = T5DDKH_ReadVarLong (pDB, pVar);
                       Write_Output_Data (pMst->dwIdent,IO_PARAM_DUTY_CYCLE,s32Val );
                       break;
                   case _MOD_ST :
                       s32Val = Read_Output_Status(pMst->dwIdent);
	   					  T5DDKH_WriteVarLong(pDB, pVar ,(T5_LONG ) s32Val);
                       break;
                   case _MOD_FI :
                       s32Val = Read_Input_Data(pMst->dwIdent,IO_PARAM_FREQUENCY );
		   				  T5DDKH_WriteVarLong(pDB, pVar ,(T5_LONG ) s32Val);
                       break;
                   case _MOD_FO :
				   		  s32Val = T5DDKH_ReadVarLong (pDB, pVar);
                       Write_Output_Data (pMst->dwIdent,IO_PARAM_FREQUENCY,s32Val );
                       break;
                   default : break;
                }
                pVar = (str_node_var *)T5DDKH_GetSibling (pVar);
            }

            pSlv = (str_node_slv *)T5DDKH_GetSibling (pSlv);
        }
        pMst = (str_node_mst *)T5DDKH_GetSibling (pMst);
    }
    return 0; /* 0=OK */
}

/****************************************************************************/
/* driver entry */

T5_DWORD DDK_ACTIA (
    T5_WORD wCommand,       /* reason for call */
    T5_WORD wVersion,       /* configuration version number */
    T5PTR_DB pDB,           /* full VMDB - NULL for T5DDKCH_GETNODESIZE */
    T5_WORD wLevel,         /* 1=bus .. 4=var */
    T5_PTR pNode,           /* node - NULL for T5DDKCH_GETNODESIZE */
    T5_DWORD dwTimeStamp,   /* time stamp - for T5DDKCH_EXCHANGE only */
    T5_PTR pArgs            /* T5DDKCH_GETNODESIZE only: properties */
    )
{
    T5_DWORD dwRet;

    dwRet = 0L;
    switch (wCommand)
    {
    /* allocate node structures *********************************************/
    case T5DDKCH_GETNODESIZE :
        switch (wLevel)
        {
        case 1 : /* bus */
            return sizeof (str_node_bus);
        case 2 : /* master */
            return sizeof (str_node_mst);
        case 3 : /* slave */
            return sizeof (str_node_slv);
        case 4 : /* var */
            return sizeof (str_node_var);
        default : break;
        }
        break;
    /* build node structures ************************************************/
    case T5DDKCH_BUILDNODE :
        switch (wLevel)
        {
        case 1 : /* bus */
            return sizeof (str_node_bus);
        case 2 : /* master */
            _BuildMaster ((str_node_mst *)pNode);
            return sizeof (str_node_mst);
        case 3 : /* slave */
            return sizeof (str_node_slv);
        case 4 : /* var */
            return sizeof (str_node_var);
        default : break;
        }
        break;
    /* open driver **********************************************************/
    case T5DDKCH_OPEN : /* pNode = bus */
        dwRet = _Open ((str_node_bus *)pNode);
        break;
    /* close driver *********************************************************/
    case T5DDKCH_CLOSE : /* pNode = bus */
        break;
    /* exchange I/Os ********************************************************/
    case T5DDKCH_EXCHANGE : /* pNode = bus */
        _Exchange (pDB, (str_node_bus *)pNode);
        break;
    default : break;
    }
    return dwRet;
}

/* eof **********************************************************************/
