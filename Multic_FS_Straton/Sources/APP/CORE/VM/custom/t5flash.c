/* flash/RAM access */

#include "t5vm.h"

//#define FORTEST

#ifdef FORTEST

/* RAM buffer for VMDB */
static T5_BYTE _RAM[8000];
static T5_BYTE _FLASH[3000];


T5_DWORD T5Ram_Size (void)
{
    return 8000;
}

T5_PTR T5Ram_Ptr (void)
{
    return _RAM;
}

T5_DWORD T5Flash_Size (void)
{
	return 3000;
}

T5_PTR T5Flash_Ptr (void)
{
	return _FLASH;
}

void T5Flash_GotToRam (void)
{
}

T5_BOOL T5Flash_Erase (void)
{
    T5_MEMSET (_FLASH, 0, 3000);
    return TRUE;
}

T5_BOOL T5Flash_Write (T5_DWORD dwOffset, T5_DWORD dwSize, T5_PTR pData)
{
    memcpy (_FLASH+dwOffset, pData, dwSize);
    return TRUE;
}



#else //FORTEST

/* RAM buffer for VMDB */
extern T5_DWORD   __VMDB_ram_size[];
#define _MAX_VMDB __VMDB_ram_size

extern T5_DWORD __VMDB_ram_start[];
#define _VMDB   __VMDB_ram_start

T5_DWORD T5Ram_Size (void)
{
    return _MAX_VMDB;
}

T5_PTR T5Ram_Ptr (void)
{
    return _VMDB;
}

/* flash access for CODE */
extern T5_DWORD   __code_app_size[];
#define _MAX_CODE __code_app_size

extern T5_DWORD    __code_app_start[];
#define _INT_FLASH __code_app_start


static T5_BOOL bFlashInRam = FALSE;

T5_DWORD T5Flash_Size (void)
{
   return (T5_DWORD)_MAX_CODE;
}

T5_PTR T5Flash_Ptr (void)
{
   return (T5_PTR )_INT_FLASH;
}

void T5Flash_GotToRam (void)
{
   bFlashInRam = TRUE;
}

/* following are called only for a download from the WB */

T5_BOOL T5Flash_Erase (void)
{
//   unsigned long ulAddrOffset;
//   unsigned long ulTest1, ulTest2;
//   //début de téléchargement
//
//    T5_MEMSET (__code_ram_start, 0, __code_ram_size);
//    T5_MEMSET (_INT_FLAH, 0, _MAX_CODE);
//
//    for(ulAddrOffset=0; ulAddrOffset<_MAX_CODE; ulAddrOffset++)
//    {
//       ((unsigned char*)(_INT_FLAH))[ulAddrOffset]=0;
//       ulTest1++;
//       ulTest2 = ulTest1;
//
//    }
   return TRUE;
}

T5_BOOL T5Flash_Write (T5_DWORD dwOffset, T5_DWORD dwSize, T5_PTR pData)
{
//    memcpy (__code_ram_start+dwOffset, pData, dwSize);
   return TRUE;
}


#endif //FORTEST
