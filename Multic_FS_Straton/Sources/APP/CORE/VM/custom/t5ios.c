/*****************************************************************************
T5Ios.c : mmanage built-in IOs - TO BE COMPLETED WHEN PORTING
(c) COPALP 2002

just use entry points - no use of parameters except pDB
*****************************************************************************/

#include "t5vm.h"


/****************************************************************************/
/* Wirring IO Table */

#include "std_type.h"
#include "monitor.h"
#include "straton_map.h"
#include "wiring.h"

#define DulBoolInputStartAddr       (*(T5_PTDWORD)(Du32Wiring_eIOTableAddr)   )
#define DulAnaInputStartAddr        (*(T5_PTDWORD)(Du32Wiring_eIOTableAddr+4) )
#define DulFastBoolInputStartAddr   (*(T5_PTDWORD)(Du32Wiring_eIOTableAddr+8) )
#define DulFastAnaInputStartAddr    (*(T5_PTDWORD)(Du32Wiring_eIOTableAddr+12))
#define DulBoolOutputStartAddr      (*(T5_PTDWORD)(Du32Wiring_eIOTableAddr+16))
#define DulAnaOutputStartAddr       (*(T5_PTDWORD)(Du32Wiring_eIOTableAddr+20))
#define DulFastBoolOutputStartAddr  (*(T5_PTDWORD)(Du32Wiring_eIOTableAddr+24))
#define DulFastAnaOutputStartAddr   (*(T5_PTDWORD)(Du32Wiring_eIOTableAddr+28))

#define DulBoolInputEndAddr         DulAnaInputStartAddr
#define DulAnaInputEndAddr          DulBoolOutputStartAddr
#define DulBoolOutputEndAddr        DulAnaOutputStartAddr
#define DulAnaOutputEndAddr         DulFastBoolOutputStartAddr

#define DulInputFromEmWiStartAddr   (*(T5_PTDWORD)(__StratonEmWi_ZED_Wiring_start)   )
#define DulOutputToEmWiStartAddr    (*(T5_PTDWORD)(__StratonEmWi_ZED_Wiring_start+4) )

#define DulInputFromEmWiEndAddr     DulOutputToEmWiStartAddr
#define DulOutputToEmWiEndAddr      (*(T5_PTDWORD)(__StratonEmWi_ZED_Wiring_end) )

typedef union
{
   T5_DWORD Address[2]; // Source & Destination
   struct
   {
      T5_PTBYTE pucSrcPtr;
      T5_BYTE   uc;
      T5_BYTE   ucDestType;
      T5_WORD   usDestIndex;
   }BoolInput;

   struct
   {
      T5_PTWORD pusSrcPtr;
      T5_BYTE   uc;
      T5_BYTE   ucDestType;
      T5_WORD   usDestIndex;
   }AnaInput;

   struct
   {
      T5_BYTE   uc;
      T5_BYTE   ucSrcType;
      T5_WORD   usSrcIndex;
      T5_PTBYTE pucDestPtr;
   }BoolOutput;

   struct
   {
      T5_BYTE   uc;
      T5_BYTE   ucSrcType;
      T5_WORD   usSrcIndex;
      T5_PTWORD pusDestPtr;
   }AnaOutput;

}TuT5IO_iIO_SrcDest;

TuT5IO_iIO_SrcDest*  puT5IO_iWiring;

/****************************************************************************/
/* Wizard IO Table */

#define DulWizardStartAddr        ((T5_PTDWORD)CT_RAM_BaseAddressZEDFpga) //(__isagraf_card_start + 0x6000) )

#define DusNbOfBooWizardIn    512
#define DusNbOfAnaWizardIn    256
#define DusNbOfBooWizardOut   512
#define DusNbOfAnaWizardOut   256

typedef struct
{
   T5_WORD usInputVersion;
   T5_BYTE ucBooInput[DusNbOfBooWizardIn];
   T5_WORD usAnaInput[DusNbOfAnaWizardIn];
   T5_WORD usOutputVersion;
   T5_BYTE ucBooOutput[DusNbOfBooWizardOut];
   T5_WORD usAnaOutput[DusNbOfAnaWizardOut];
}TstT5IO_iWizardIO_SrcDest;

TstT5IO_iWizardIO_SrcDest* pstT5IO_iWizardIOCard;

/****************************************************************************/
/* not used */

T5_DWORD T5Ios_GetMemorySizing (T5_PTR pDef)
{
    return 0;
}

T5_RET T5Ios_CanHotRestart (T5_PTR pDef, T5PTR_DB pDB, T5_PTR pMem)
{
    return T5RET_ERROR;
}

T5_RET T5Ios_HotRestart (T5_PTR pDef, T5PTR_DB pDB, T5_PTR pMem)
{
    return T5RET_ERROR;
}

/****************************************************************************/
/* the profile */

#define _PRF_MIO    "MIO"

typedef struct
{
    T5_WORD wVers;
    T5_WORD wType;
    T5_WORD wIndex;
}
str_mio;

#define IO_TYPE_BI      0  /* Boolean input */
#define IO_TYPE_FL      1  /* Fault */
#define IO_TYPE_AI      2  /* Analog input */
#define IO_TYPE_EO      3  /* Equipment output */
#define IO_TYPE_BO      4  /* Boolean output */
#define IO_TYPE_AO      5  /* Analog output */
#define IO_TYPE_BWI     6  /* Boolean Wizard input */
#define IO_TYPE_AWI     7  /* Analog Wizard input */
#define IO_TYPE_BWO     8  /* Boolean Wizard output */
#define IO_TYPE_AWO     9  /* Analog Wizard output */
#define IO_TYPE_MAX     10

#define IO_INDEX_MAX    768

T5PTR_DBMAP pt5ios_iIO_VMDB[IO_TYPE_MAX][IO_INDEX_MAX];

/****************************************************************************/

T5_RET T5Ios_Open (T5_PTR pDef, T5PTR_DB pDB, T5_PTR pMem)
{
// debug pcou
   T5PTR_DBMAP pVar;       // variable in the map
   T5_WORD     wPos;       // position used for enumeration
   T5_PTCHAR   szProfile;
   str_mio*    pParams;

   T5_MEMSET (pt5ios_iIO_VMDB, 0, sizeof (pt5ios_iIO_VMDB));

   // Point to first variable into VMDB
   pVar = T5Map_GetFirst (pDB, &wPos);

   // Go through VMDB variables until end (while variable pointer is not null)
   while (pVar != NULL )
   {
      szProfile = T5Map_GetProfileName (pVar);

      // Identify variables with MIO profile
      if (szProfile != NULL && T5_STRCMP (szProfile, _PRF_MIO) == 0)
      {
         pParams = (str_mio *)T5Map_GetProps (pVar); // parameters

         if(pParams->wType < IO_TYPE_MAX)
         {
            if(pParams->wIndex < IO_INDEX_MAX)
            {
               // Store variable pointer into IO pointer table
               pt5ios_iIO_VMDB[pParams->wType][pParams->wIndex] = pVar;
            }
            else
            {
               // index out of range
            }
         }
         else
         {
            // unknown Type
         }
      }
      else
      {
         // not a MIO profile
      }

      pVar = T5Map_GetNext (pDB, &wPos);
   }

   // WIRING IO TABLES:
   //-----------------------------------------------------------
   puT5IO_iWiring = (TuT5IO_iIO_SrcDest*)DulBoolInputStartAddr;

   // WIRING EmWi IO TABLES:
   //-----------------------------------------------------------   
   pstT5IO_iWizardIOCard = (TstT5IO_iWizardIO_SrcDest*)DulWizardStartAddr;

//
   return T5RET_OK;
}

T5_RET T5Ios_Close (T5_PTR pDef, T5PTR_DB pDB, T5_PTR pMem)
{
    return T5RET_OK;
}


T5_RET T5Ios_Exchange (T5_PTR pDef, T5PTR_DB pDB, T5_PTR pMem)
{
// debug pcou
   T5PTR_DBMAP pVar;  // variable in the map
   T5_BOOL bValue;
   T5_WORD wValue;
   T5_WORD wProfilIndex;

   //----------------------------------------------------------------------------------------------------
   // Boolean Input Update
   //----------------------------------------------------------------------------------------------------
   puT5IO_iWiring = (TuT5IO_iIO_SrcDest*)DulBoolInputStartAddr;

   while((puT5IO_iWiring<=(TuT5IO_iIO_SrcDest*)(DulBoolInputEndAddr-sizeof(TuT5IO_iIO_SrcDest)))
      && (0!=puT5IO_iWiring->BoolInput.pucSrcPtr)
	  && (puT5IO_iWiring->BoolInput.ucDestType < IO_TYPE_MAX)
	  && (puT5IO_iWiring->BoolInput.usDestIndex < IO_INDEX_MAX))
   {

      pVar = pt5ios_iIO_VMDB  [puT5IO_iWiring->BoolInput.ucDestType]
                              [puT5IO_iWiring->BoolInput.usDestIndex];
      if(pVar!=0)
      {
         bValue = *(puT5IO_iWiring->BoolInput.pucSrcPtr);

         T5Map_SetDWordValue (pVar, bValue);
      }
      else
      {
         // no variable pointer from VMDB!
      }

      puT5IO_iWiring++;
   }

   //----------------------------------------------------------------------------------------------------
   // Analog Input Update
   //----------------------------------------------------------------------------------------------------
   puT5IO_iWiring = (TuT5IO_iIO_SrcDest*)DulAnaInputStartAddr;

   while((puT5IO_iWiring<=(TuT5IO_iIO_SrcDest*)(DulAnaInputEndAddr-sizeof(TuT5IO_iIO_SrcDest)))
      && (0!=puT5IO_iWiring->AnaInput.pusSrcPtr)
	  && (puT5IO_iWiring->AnaInput.ucDestType < IO_TYPE_MAX)
	  && (puT5IO_iWiring->AnaInput.usDestIndex < IO_INDEX_MAX))
   {

      pVar = pt5ios_iIO_VMDB  [puT5IO_iWiring->AnaInput.ucDestType]
                              [puT5IO_iWiring->AnaInput.usDestIndex];

      if(pVar!=0)
      {
         wValue = *(puT5IO_iWiring->AnaInput.pusSrcPtr);

         T5Map_SetDWordValue (pVar, wValue);
      }
      else
      {
         // no variable pointer from VMDB!
      }

      puT5IO_iWiring++;
   }

   //----------------------------------------------------------------------------------------------------
   // Boolean Output Update
   //----------------------------------------------------------------------------------------------------
   puT5IO_iWiring = (TuT5IO_iIO_SrcDest*)DulBoolOutputStartAddr;

   while((puT5IO_iWiring<=(TuT5IO_iIO_SrcDest*)(DulBoolOutputEndAddr-sizeof(TuT5IO_iIO_SrcDest)))
      && (0!=puT5IO_iWiring->BoolOutput.pucDestPtr)
	  && (puT5IO_iWiring->BoolOutput.ucSrcType < IO_TYPE_MAX)
	  && (puT5IO_iWiring->BoolOutput.usSrcIndex < IO_INDEX_MAX))
   {

      pVar = pt5ios_iIO_VMDB  [puT5IO_iWiring->BoolOutput.ucSrcType]
                              [puT5IO_iWiring->BoolOutput.usSrcIndex];

      if(pVar!=0)
      {
         bValue = (T5_BOOL)T5Map_GetDWordValue(pVar);

         *(puT5IO_iWiring->BoolOutput.pucDestPtr) = bValue;
      }
      else
      {
         // no variable pointer from VMDB!
      }

      puT5IO_iWiring++;
   }

   //----------------------------------------------------------------------------------------------------
   // Analog Output Update
   //----------------------------------------------------------------------------------------------------
   puT5IO_iWiring = (TuT5IO_iIO_SrcDest*)DulAnaOutputStartAddr;

   while((puT5IO_iWiring<=(TuT5IO_iIO_SrcDest*)(DulAnaOutputEndAddr-sizeof(TuT5IO_iIO_SrcDest)))
      && (0!=puT5IO_iWiring->AnaOutput.pusDestPtr)
	  && (puT5IO_iWiring->AnaOutput.ucSrcType < IO_TYPE_MAX)
	  && (puT5IO_iWiring->AnaOutput.usSrcIndex < IO_INDEX_MAX))
   {

      pVar = pt5ios_iIO_VMDB  [puT5IO_iWiring->AnaOutput.ucSrcType]
                              [puT5IO_iWiring->AnaOutput.usSrcIndex];

      if(pVar!=0)
      {
         wValue = (T5_WORD)T5Map_GetDWordValue(pVar);

         *(puT5IO_iWiring->AnaOutput.pusDestPtr) = wValue;
      }
      else
      {
         // no variable pointer from VMDB!
      }

      puT5IO_iWiring++;
   }

   //----------------------------------------------------------------------------------------------------
   // EmWi Input Update
   //----------------------------------------------------------------------------------------------------   
   puT5IO_iWiring = (TuT5IO_iIO_SrcDest*)DulInputFromEmWiStartAddr;

   while((puT5IO_iWiring<=(TuT5IO_iIO_SrcDest*)(DulInputFromEmWiEndAddr-sizeof(TuT5IO_iIO_SrcDest)))
      && (0!=puT5IO_iWiring->BoolInput.pucSrcPtr)
	  && (puT5IO_iWiring->BoolInput.usDestIndex < IO_INDEX_MAX)
	  && (puT5IO_iWiring->AnaInput.usDestIndex < IO_INDEX_MAX))
   {
      // Boolean Wizard Input Update
      if(IO_TYPE_BWI==puT5IO_iWiring->BoolInput.ucDestType)
      {
         pVar = pt5ios_iIO_VMDB  [puT5IO_iWiring->BoolInput.ucDestType]
                                 [puT5IO_iWiring->BoolInput.usDestIndex];
         if(pVar!=0)
         {
            bValue = *(puT5IO_iWiring->BoolInput.pucSrcPtr);

            T5Map_SetDWordValue (pVar, bValue);
         }
         else
         {
            // no variable pointer from VMDB!
         }         
      }
      // Analog Wizard Input Update
      else if(IO_TYPE_AWI==puT5IO_iWiring->AnaInput.ucDestType)
      {
         pVar = pt5ios_iIO_VMDB  [puT5IO_iWiring->AnaInput.ucDestType]
                                 [puT5IO_iWiring->AnaInput.usDestIndex];

         if(pVar!=0)
         {
            wValue = *(puT5IO_iWiring->AnaInput.pusSrcPtr);

            T5Map_SetDWordValue (pVar, wValue);
         }
         else
         {
            // no variable pointer from VMDB!
         }         
      }
      else
      {
         // not an input type!
      }

      puT5IO_iWiring++;
   }

   //----------------------------------------------------------------------------------------------------
   // EmWi Output Update
   //----------------------------------------------------------------------------------------------------
   puT5IO_iWiring = (TuT5IO_iIO_SrcDest*)DulOutputToEmWiStartAddr;

   while((puT5IO_iWiring<=(TuT5IO_iIO_SrcDest*)(DulOutputToEmWiEndAddr-sizeof(TuT5IO_iIO_SrcDest)))
      && (0!=puT5IO_iWiring->BoolOutput.pucDestPtr)
	  && (puT5IO_iWiring->BoolOutput.usSrcIndex < IO_INDEX_MAX)
	  && (puT5IO_iWiring->AnaOutput.usSrcIndex < IO_INDEX_MAX))
   {
      // Boolean Wizard Output Update
      if(IO_TYPE_BWO==puT5IO_iWiring->BoolOutput.ucSrcType)
      {
         pVar = pt5ios_iIO_VMDB  [puT5IO_iWiring->BoolOutput.ucSrcType]
                                 [puT5IO_iWiring->BoolOutput.usSrcIndex];

         if(pVar!=0)
         {
            bValue = (T5_BOOL)T5Map_GetDWordValue(pVar);

            *(puT5IO_iWiring->BoolOutput.pucDestPtr) = bValue;
         }
         else
         {
            // no variable pointer from VMDB!
         }
      }
      // Analog Output Update
      else if(IO_TYPE_AWO==puT5IO_iWiring->AnaOutput.ucSrcType)
      {
         pVar = pt5ios_iIO_VMDB  [puT5IO_iWiring->AnaOutput.ucSrcType]
                                 [puT5IO_iWiring->AnaOutput.usSrcIndex];

         if(pVar!=0)
         {
            wValue = (T5_WORD)T5Map_GetDWordValue(pVar);

            *(puT5IO_iWiring->AnaOutput.pusDestPtr) = wValue;
         }
         else
         {
            // no variable pointer from VMDB!
         }
      }
      else
      {
         // not an output type!
      }

      puT5IO_iWiring++;
   }
//
   return T5RET_OK;
}

/* eof **********************************************************************/
