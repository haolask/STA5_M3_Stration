/* flash/RAM access */

extern T5_DWORD T5Ram_Size (void);
extern T5_PTR T5Ram_Ptr (void);

extern T5_BOOL T5Flash_GotToRam(void);

extern T5_DWORD T5Flash_Size (void);
extern T5_PTR T5Flash_Ptr (void);
extern T5_BOOL T5Flash_Erase (void);
extern T5_BOOL T5Flash_Write (T5_DWORD dwOffset, T5_DWORD dwSize, T5_PTR pData);
