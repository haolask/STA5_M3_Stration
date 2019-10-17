/*****************************************************************************
T5retain.c : support of RETAIN variables - CAN BE CHANGED FOR PORT PURPOSE
(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"
#include "std_type.h"
#include "monitor.h"

/****************************************************************************/

#ifdef T5DEF_RETAIN

T5_DWORD dwSizeRetain;
T5_WORD  wSizeCRC;

/*****************************************************************************
T5Retain_CanLoad
Check if RETAIN variables can be loaded
Parameters:
    dwCrc (IN) expected RETAIN data CRC
    pArgs (IN) configuration string from the main caller
Return: TRUE if valid data available
*****************************************************************************/

T5_BOOL T5Retain_CanLoad (T5_DWORD dwCrc, T5_PTR pArgs)
{
   T5_DWORD dwCrcStored;

   // CRC stored at begining of NVDM area
   dwCrcStored = *((T5_PTDWORD)Du32API_Addr_Retain + 0);

   if(dwCrc != dwCrcStored)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

/*****************************************************************************
T5Retain_Load
Load RETAIN variables
Parameters:
    pRetain (IN) pointers and size in the DB
    pArgs (IN) configuration string from the main caller
Return: OK or error
*****************************************************************************/

T5_RET T5Retain_Load (T5PTR_DBRETAIN pRetain, T5_PTR pArgs)
{
   T5_DWORD  dwNVDM_Index = 0;
   T5_WORD   wRetain_Index = 0;
   T5_DWORD  dw_RetainCrc_Stored;
   
   // LOAD ORDER MUST BE THE SAME AS STORE ORDER !!

   // 1 - Check retain CRC
   // note: The retain CRC (pRetain->dwCrc)is not a CRC on the retain values,
   //       but depends on the retain structure (nb of retain declared).
   //       It is calculated by the work bench only (not by the Straton VM).
   //       It is stored in EEPROM with the rest of the retain variable.
   dw_RetainCrc_Stored = *((T5_PTDWORD)Du32API_Addr_Retain + 0);
   
   // If the Retain CRC stored is different to the Retain CRC calculated by the workbench,
   // it means some retain variables have been added or suppressed in the application code.
   // In that case, do not load the retain!
   if(dw_RetainCrc_Stored != pRetain->dwCrc)
   {
      return(T5RET_ERROR);
   }
   else
   {
      // carry on
   }
   
   wSizeCRC = sizeof(pRetain->dwCrc);
   dwNVDM_Index = wSizeCRC;

   // 2 - Load 8bit retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeD8; wRetain_Index++)
   {
      *((T5_PTBYTE)pRetain->pD8 + wRetain_Index) = *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index);
      dwNVDM_Index++;
   }
   // 3 - Load 16bit retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeD16; wRetain_Index++)
   {
      *((T5_PTBYTE)pRetain->pD16 + wRetain_Index) = *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index);
      dwNVDM_Index++;
   }
   // 4 - Load 32bit retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeD32; wRetain_Index++)
   {
      *((T5_PTBYTE)pRetain->pD32 + wRetain_Index) = *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index);
      dwNVDM_Index++;
   }
   // 5- Load 64bit retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeD64; wRetain_Index++)
   {
      *((T5_PTBYTE)pRetain->pD64 + wRetain_Index) = *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index);
      dwNVDM_Index++;
   }
   // 6 - Load Time retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeTime; wRetain_Index++)
   {
      *((T5_PTBYTE)pRetain->pTime + wRetain_Index) = *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index);
      dwNVDM_Index++;
   }
   // 7 - Load String retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeString; wRetain_Index++)
   {
      *((T5_PTBYTE)pRetain->pString + wRetain_Index) = *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index);
      dwNVDM_Index++;
   }

   return T5RET_OK;
}

/*****************************************************************************
T5Retain_CanStore
Check if RETAIN variables can be stored (enough space)
Parameters:
    pRetain (IN) pointers and size in the DB
    pArgs (IN) configuration string from the main caller
Return: TRUE if valid data available
*****************************************************************************/

T5_BOOL T5Retain_CanStore (T5PTR_DBRETAIN pRetain, T5_PTR pArgs)
{
   wSizeCRC = sizeof(pRetain->dwCrc);

   dwSizeRetain = wSizeCRC
               +  pRetain->wSizeD8
               +  pRetain->wSizeD16
               +  pRetain->wSizeD32
               +  pRetain->wSizeD64
               +  pRetain->wSizeTime
               +  pRetain->wSizeString;


   if(dwSizeRetain > Du16MNVDM_iRetainSize)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

/*****************************************************************************
T5Retain_ShutDown
Called at shutdown for closing the storage of RETAIN variables
Parameters:
    pRetain (IN) pointers and size in the DB
    pArgs (IN) configuration string from the main caller
*****************************************************************************/

void T5Retain_ShutDown (T5PTR_DBRETAIN pRetain, T5_PTR pArgs)
{
}

/*****************************************************************************
T5Retain_CycleExchange
Called on each cycle for managing RETAIN variables
Parameters:
    pRetain (IN) pointers and size in the DB
    pArgs (IN) configuration string from the main caller
*****************************************************************************/

void T5Retain_CycleExchange (T5PTR_DBRETAIN pRetain)
{
   T5_DWORD  dwNVDM_Index = 0;
   T5_WORD   wRetain_Index = 0;

   // STORE ORDER MUST BE THE SAME AS LOAD ORDER !!

   // 1 - Store CRC
   *((T5_PTDWORD)Du32API_Addr_Retain + 0) = pRetain->dwCrc;

   wSizeCRC = sizeof(pRetain->dwCrc);
   dwNVDM_Index = wSizeCRC;

   // 2 - Store 8bit retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeD8; wRetain_Index++)
   {
      *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index) = *((T5_PTBYTE)pRetain->pD8 + wRetain_Index);
      dwNVDM_Index++;
   }
   // 3 - Store 16bit retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeD16; wRetain_Index++)
   {
      *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index) = *((T5_PTBYTE)pRetain->pD16 + wRetain_Index);
      dwNVDM_Index++;
   }
   // 4 - Store 32bit retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeD32; wRetain_Index++)
   {
      *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index) = *((T5_PTBYTE)pRetain->pD32 + wRetain_Index);
      dwNVDM_Index++;
   }
   // 5- Store 64bit retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeD64; wRetain_Index++)
   {
      *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index) = *((T5_PTBYTE)pRetain->pD64 + wRetain_Index);
      dwNVDM_Index++;
   }
   // 6 - Store Time retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeTime; wRetain_Index++)
   {
      *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index) = *((T5_PTBYTE)pRetain->pTime + wRetain_Index);
      dwNVDM_Index++;
   }
   // 7 - Store String retain
   for(wRetain_Index=0; wRetain_Index<pRetain->wSizeString; wRetain_Index++)
   {
      *((T5_PTBYTE)Du32API_Addr_Retain + dwNVDM_Index) = *((T5_PTBYTE)pRetain->pString + wRetain_Index);
      dwNVDM_Index++;
   }
}

/****************************************************************************/

#endif /*T5DEF_RETAIN*/

/* eof **********************************************************************/
