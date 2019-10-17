/*****************************************************************************
T5vfs.c :    virtual file system - core implementation

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_VFS

/****************************************************************************/
/* OEM configuration */

#include "t5vfsoem.h"

#ifdef T5VFS_MALLOC
#ifdef T5VFS_RAMBUF
#error T5VFS_MALLOC and T5VFS_RAMBUF cannot be defined together
#endif
#endif

#ifndef T5VFS_MALLOC
#ifndef T5VFS_RAMBUF
#error either T5VFS_MALLOC or T5VFS_RAMBUF must be defined
#endif
#endif

#ifdef T5VFS_CFILES
#ifdef T5VFS_FLASH
#error T5VFS_CFILES and T5VFS_FLASH cannot be defined together
#endif
#endif

#ifndef T5VFS_CFILES
#ifndef T5VFS_FLASH
#error either T5VFS_CFILES or T5VFS_FLASH must be defined
#endif
#endif

/****************************************************************************/
/* predefined items */

typedef struct
{
    T5_WORD   wCode;
    T5_BOOL   bFile;
    T5_BOOL   bReadonly;
    T5_PTCHAR szPath;
}
str_itm;

static str_itm _ITEMS[] = {
    { T5VFS_RAM_VMDB,           FALSE,  FALSE,  (T5_PTCHAR)T5VFS_NAME_HOT     },
    { T5VFS_RAM_CODE,           FALSE,  FALSE,  (T5_PTCHAR)T5VFS_NAME_COD     },
    { T5VFS_RAM_CHANGE,         FALSE,  FALSE,  (T5_PTCHAR)T5VFS_NAME_UPD     },
    { T5VFS_FILE_VMDB,          TRUE,   FALSE,  (T5_PTCHAR)T5VFS_NAME_HOT     },
    { T5VFS_FILE_CODE,          TRUE,   FALSE,  (T5_PTCHAR)T5VFS_NAME_COD     },
    { T5VFS_FILE_CHANGE,        TRUE,   FALSE,  (T5_PTCHAR)T5VFS_NAME_UPD     },
    { T5VFS_FILE_SOURCE,        TRUE,   FALSE,  (T5_PTCHAR)T5VFS_NAME_UPL     },
    { T5VFS_FILE_RCSCREENS,     TRUE,   FALSE,  (T5_PTCHAR)T5VFS_NAME_RSC     },
    { T5VFS_FILE_RCSTRINGS,     TRUE,   FALSE,  (T5_PTCHAR)T5VFS_NAME_RST     },
    { T5VFS_FILE_RCBITMAPS,     TRUE,   FALSE,  (T5_PTCHAR)T5VFS_NAME_RBM     },
    { T5VFS_FILE_RCFONTS,       TRUE,   FALSE,  (T5_PTCHAR)T5VFS_NAME_RFN     },
    { T5VFS_FILE_REG,           TRUE,   FALSE,  (T5_PTCHAR)T5VFS_NAME_REG     },
    { T5VFS_FILE_REGCHANGE,     TRUE,   FALSE,  (T5_PTCHAR)T5VFS_NAME_REGNEXT },
    { T5VFS_FILE_SYSINFO,       TRUE,   TRUE,   (T5_PTCHAR)T5VFS_NAME_INFO     },
{ 0, FALSE, FALSE, NULL } };

/*****************************************************************************
_T5Vfs_GetItem
find a predefined RAM or file item
Parameters:
    szPath (IN) searched item name
    bFile (IN) if FALSE, a RAM object is searched - otherwise a file
Return: item identifier or 0 if not found
*****************************************************************************/

static T5_WORD _T5Vfs_GetItem (T5_PTCHAR szPath, T5_BOOL bFile)
{
    T5_WORD i;

    for (i=0; _ITEMS[i].szPath != NULL; i++)
    {
        if (bFile == _ITEMS[i].bFile
            && T5_STRCMP(szPath, _ITEMS[i].szPath) == 0)
        {
            return _ITEMS[i].wCode;
        }
    }
    return 0;
}

/*****************************************************************************
_T5Vfs_IsFileItem
test if a predefined item if a file
Parameters:
    wItem (IN) item identifier
Return: TRUE if it is a file - FALSE means RAM or unknown item
*****************************************************************************/

#ifdef T5VFS_FLASH
static T5_BOOL _T5Vfs_IsFileItem (T5_WORD wItem)
{
    T5_WORD i;

    for (i=0; _ITEMS[i].szPath != NULL; i++)
    {
        if (_ITEMS[i].bFile && _ITEMS[i].wCode == wItem)
            return TRUE;
    }
    return FALSE;
}
#endif /*T5VFS_FLASH*/

/*****************************************************************************
T5Vfs_CleanSRAMBuffers
memset all file buffers if implemented in SRAM
*****************************************************************************/

void T5Vfs_CleanSRAMBuffers (void)
{
#ifdef T5VFS_FLASH
    T5_WORD i;
    T5_PTR pBuffer;
    T5_DWORD dwSize;

    for (i=0; _ITEMS[i].szPath != NULL; i++)
    {
        if (!_ITEMS[i].bReadonly && _ITEMS[i].bFile)
        {
            pBuffer = T5VFS_FLASH_BUFPOINTER(_ITEMS[i].wCode);
            dwSize = T5VFS_FLASH_BUFSIZE(_ITEMS[i].wCode);
            if (pBuffer != NULL && dwSize != 0L)
            {
                T5_MEMSET (pBuffer, 0, dwSize);
            }
        }
    }
#endif /*T5VFS_FLASH*/
}

/*****************************************************************************
T5Vfs_FileInFlash
gives the indication whether files are directly read from flash or loaded in
memory from real files
Return: TRUE if files are in flash (read from direct pointers)
*****************************************************************************/

T5_BOOL T5Vfs_FileInFlash (void)
{
#ifdef T5VFS_FLASH
    return TRUE;
#else /*T5VFS_FLASH*/
    return FALSE;
#endif /*T5VFS_FLASH*/
}

/****************************************************************************/
/* current position of open files in flash */

#ifdef T5VFS_FLASH
static T5_DWORD _FLASH_POS [T5VFS_FILE_MAX];
#endif /*T5VFS_FLASH*/

/*****************************************************************************
T5Vfs_GetMaxRamFileSize
get the maximum size of a RAM memory object
Parameters:
    szPath (IN) item name
Return: maxim size in bytes if buffer - T5VFS_SIZE_INFINITE if malloc
*****************************************************************************/

T5_DWORD T5Vfs_GetMaxRamFileSize (T5_PTCHAR szPath)
{
#ifdef T5VFS_MALLOC
    T5_UNUSED(szPath);
    return T5VFS_SIZE_INFINITE;
#endif /*T5VFS_MALLOC*/

#ifdef T5VFS_RAMBUF
    T5_WORD wItem;

    wItem = _T5Vfs_GetItem (szPath, FALSE);
    if (wItem == 0)
        return 0L;
    return T5VFS_RAM_BUFSIZE(wItem);
#endif /*T5VFS_RAMBUF*/

    return 0L;
}

/*****************************************************************************
T5Vfs_CreateRamFile
Allocate or link to a RAM object
Parameters:
    szPath (IN) item name
    dwSize (IN) required size
Return: pointer to the RAM memory or NULL if not enough space
*****************************************************************************/

T5_PTR T5Vfs_CreateRamFile (T5_PTCHAR szPath, T5_DWORD dwSize)
{
    T5_WORD wItem;

    wItem = _T5Vfs_GetItem (szPath, FALSE);
    if (wItem == 0)
        return NULL;

#ifdef T5VFS_MALLOC
    return T5_MALLOC(dwSize);
#endif /*T5VFS_MALLOC*/

#ifdef T5VFS_RAMBUF
    if (dwSize > T5VFS_RAM_BUFSIZE(wItem))
        return NULL;
    return T5VFS_RAM_BUFPOINTER(wItem);
#endif /*T5VFS_RAMBUF*/

    return NULL;
}

/*****************************************************************************
T5Vfs_ReleaseRamFile
Release a RAM object
Parameters:
    pMem (IN) pointer to the object memory
*****************************************************************************/

void T5Vfs_ReleaseRamFile (T5_PTR pMem)
{
#ifdef T5VFS_MALLOC
    T5_FREE(pMem);
#endif /*T5VFS_MALLOC*/
}

/*****************************************************************************
T5Vfs_GetSize
Get the size of a FILE object
Parameters:
    szPath (IN) item name
    pdwSize (OUT) full size of the file in bytes
Return: TRUE if OK - FALSE if file not existing or empty
*****************************************************************************/

T5_BOOL T5Vfs_GetSize (T5_PTCHAR szPath, T5_PTDWORD pdwSize)
{
#ifdef T5VFS_CFILES
    *pdwSize = T5VFS_GETSIZE (szPath);
    return (*pdwSize != 0L);
#endif /*T5VFS_CFILES*/

#ifdef T5VFS_FLASH
    T5_WORD wItem;
    T5_PTR pBuffer;
    T5_DWORD dwSize;

    wItem = _T5Vfs_GetItem (szPath, TRUE);
    if (wItem == 0)
        return FALSE;
    pBuffer = T5VFS_FLASH_BUFPOINTER(wItem);
    dwSize = T5VFS_FLASH_BUFSIZE(wItem);
    if (pBuffer == NULL || dwSize < sizeof (T5_DWORD))
        return FALSE;
    *pdwSize = *(T5_PTDWORD)pBuffer;
    return (*pdwSize != 0L);
#endif /*T5VFS_FLASH*/
}

/*****************************************************************************
_T5Vfs_LinkFlash
Link to a flash buffer
Parameters:
    wItem (IN) file item identifier
    pdwSize (OUT) full size of the file in bytes
Return: pointer to flash memory or NULL if fail
*****************************************************************************/

#ifdef T5VFS_FLASH
static T5_PTBYTE _T5Vfs_LinkFlash (T5_WORD wItem, T5_PTDWORD pdwSize)
{
    T5_PTR pBuffer;
    T5_DWORD dwSize;

    pBuffer = T5VFS_FLASH_BUFPOINTER(wItem);
    dwSize = T5VFS_FLASH_BUFSIZE(wItem);
    if (pBuffer == NULL || dwSize < sizeof (T5_DWORD))
        return NULL;
    if (pdwSize != NULL)
        *pdwSize = *(T5_PTDWORD)pBuffer;
    return (((T5_PTBYTE)pBuffer) + sizeof (T5_DWORD));
    return NULL;
}
#endif /*T5VFS_FLASH*/

/*****************************************************************************
T5Vfs_LoadToMemory
Load a file object in memory (or link for flash)
Parameters:
    szPath (IN) file item name
    pdwSize (OUT) full size of the file in bytes
Return: pointer to memory or NULL if fail
*****************************************************************************/

T5_PTR T5Vfs_LoadToMemory (T5_PTCHAR szPath, T5_PTDWORD pdwSize)
{
#ifdef T5VFS_CFILES
    T5_DWORD dwSize, f, dwReq;
    T5_PTR pRam;
    T5_PTBYTE pPacket;
    T5_BOOL bOK;

    dwSize = T5VFS_GETSIZE (szPath);
    if (dwSize == 0L)
        return NULL;
    pRam = T5Vfs_CreateRamFile (szPath, dwSize);
    if (pRam == NULL)
        return NULL;
    if (pdwSize != NULL)
        *pdwSize = dwSize;

    if ((f = T5VFS_FOPENRB (szPath)) == 0L)
        bOK = FALSE;
    else
    {
        bOK = TRUE;
        pPacket = (T5_PTBYTE)pRam;
        while (bOK && dwSize > 0)
        {
            if (dwSize > 512)
                dwReq = 512;
            else
                dwReq = dwSize;
            if (T5VFS_FREAD(pPacket, dwReq, f) != dwReq)
                bOK = FALSE;
            else
            {
                pPacket += dwReq;
                dwSize -= dwReq;
            }
        }
        T5VFS_FCLOSE (f);
    }
    if (!bOK)
    {
        T5Vfs_ReleaseRamFile (pRam);
        return NULL;
    }
    return pRam;
#endif /*T5VFS_CFILES*/

#ifdef T5VFS_FLASH
    T5_WORD wItem;

    wItem = _T5Vfs_GetItem (szPath, TRUE);
    if (wItem == 0)
        return NULL;
    return _T5Vfs_LinkFlash (wItem, pdwSize);
#endif /*T5VFS_FLASH*/

    return NULL;
}

/*****************************************************************************
T5Vfs_ReleaseFromMemory
Release a file object from memory
Parameters:
    pMem (IN) pointer to loaded memory
*****************************************************************************/

void T5Vfs_ReleaseFromMemory (T5_PTR pMem)
{
#ifdef T5VFS_CFILES
    T5Vfs_ReleaseRamFile (pMem);
#endif /*T5VFS_CFILES*/
}

/*****************************************************************************
T5Vfs_OpenReadRand
Open a file for random read access
Parameters:
    szPath (IN) file item name
    pdwSize (OUT) full size of the file in bytes
Return: file handle or 0 if fail
*****************************************************************************/

T5_DWORD T5Vfs_OpenReadRand (T5_PTCHAR szPath, T5_PTDWORD pdwSize)
{
#ifdef T5VFS_CFILES
    if (pdwSize != NULL)
        *pdwSize = T5VFS_GETSIZE (szPath);
    return T5VFS_FOPENRB (szPath);
#endif /*T5VFS_CFILES*/

#ifdef T5VFS_FLASH
    T5_WORD wItem;

    wItem = _T5Vfs_GetItem (szPath, TRUE);
    if (wItem == 0)
        return 0L;
    if (!_T5Vfs_LinkFlash (wItem, pdwSize))
        return 0L;
    _FLASH_POS[wItem] = 0L;
    return (T5_DWORD)wItem;
#endif /*T5VFS_FLASH*/

    return 0L;
}

/*****************************************************************************
T5Vfs_Read
Read a packet in a file open for random read access
Parameters:
    hVFS (IN) file handle
    dwPos (IN) read position of T5VFS_POS_CURRENT for sequential read
    dwSize (IN) wished size
    pMem (OUT) pointer to the buffer where to copy read data
Return: number of bytes read
*****************************************************************************/

T5_DWORD T5Vfs_Read (T5_DWORD hVFS, T5_DWORD dwPos, T5_DWORD dwSize, T5_PTR pMem)
{
#ifdef T5VFS_CFILES
    if (dwPos != T5VFS_POS_CURRENT)
        T5VFS_FSEEK (hVFS, dwPos);
    return T5VFS_FREAD (pMem, dwSize, hVFS);
#endif /*T5VFS_CFILES*/

#ifdef T5VFS_FLASH
    T5_WORD wItem;
    T5_PTBYTE pBuffer;
    T5_DWORD dwBufSize;

    wItem = (T5_WORD)hVFS;
    if (!_T5Vfs_IsFileItem (wItem))
        return 0L;
    pBuffer = _T5Vfs_LinkFlash (wItem, &dwBufSize);
    if (pBuffer == NULL)
        return 0L;
    if (dwPos == T5VFS_POS_CURRENT)
        dwPos = _FLASH_POS[wItem];
    if (dwPos >= dwBufSize)
        return 0L;
    if ((dwPos + dwSize) > dwBufSize)
        dwSize = dwBufSize - dwPos;
    T5_MEMCPY (pMem, pBuffer + dwPos, dwSize);
    _FLASH_POS[wItem] = dwPos + dwSize;
    return dwSize;
#endif /*T5VFS_FLASH*/

    return 0L;
}

/*****************************************************************************
T5Vfs_OpenWriteCreate
Open a file for sequential write access and reset its contents
Parameters:
    szPath (IN) file item name
    dwWishedSize (OUT) wished size for the full file
Return: file handle or 0 if fail
*****************************************************************************/

T5_DWORD T5Vfs_OpenWriteCreate (T5_PTCHAR szPath, T5_DWORD dwWishedSize)
{
#ifdef T5VFS_CFILES
    T5_UNUSED(dwWishedSize);
    return T5VFS_FOPENWB (szPath);
#endif /*T5VFS_CFILES*/

#ifdef T5VFS_FLASH
    T5_WORD wItem;

    wItem = _T5Vfs_GetItem (szPath, TRUE);
    if (wItem == 0)
        return 0L;
    if ((dwWishedSize + sizeof (T5_DWORD)) > T5VFS_FLASH_BUFSIZE(wItem))
        return 0L;

    if (!T5VFS_FLASH_ERASE_BUFFER (wItem))
        return 0L;
    if (!T5VFS_FLASH_WRITE_PACKET (wItem, 0, sizeof (T5_DWORD), &dwWishedSize))
        return 0L;
    _FLASH_POS[wItem] = 0L;
    return (T5_DWORD)wItem;
#endif /*T5VFS_FLASH*/

    return 0L;
}

/*****************************************************************************
T5Vfs_OpenWriteAppend
Open a file for sequential write access - append data, do not reset contents
Parameters:
    szPath (IN) file item name
    dwWishedSize (OUT) wished size for the full appended data
Return: file handle or 0 if fail
*****************************************************************************/

T5_DWORD T5Vfs_OpenWriteAppend (T5_PTCHAR szPath, T5_DWORD dwWishedSize)
{
#ifdef T5VFS_CFILES
    T5_UNUSED(dwWishedSize);
    return T5VFS_FOPENAB (szPath);
#endif /*T5VFS_CFILES*/

#ifdef T5VFS_FLASH
    T5_WORD wItem;
    T5_DWORD dwBufSize;

    wItem = _T5Vfs_GetItem (szPath, TRUE);
    if (wItem == 0)
        return 0L;
    if (!_T5Vfs_LinkFlash (wItem, &dwBufSize))
        return 0L;
    if ((_FLASH_POS[wItem] + dwWishedSize) > dwBufSize)
        return 0L;
    return (T5_DWORD)wItem;
#endif /*T5VFS_FLASH*/

    return 0L;
}

/*****************************************************************************
T5Vfs_Write
Write a packet in a file - SEQUENTIAL ONLY
Parameters:
    hVFS (IN) file handle
    dwPos (IN) read position - must be T5VFS_POS_CURRENT
    dwSize (IN) wished size
    pMem (IN) pointer to the buffer to be written
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5Vfs_Write (T5_DWORD hVFS, T5_DWORD dwPos, T5_DWORD dwSize, T5_PTR pMem)
{
#ifdef T5VFS_CFILES
    if (dwPos != T5VFS_POS_CURRENT)
        T5VFS_FSEEK (hVFS, dwPos);
    return (T5VFS_FWRITE (pMem, dwSize, hVFS) == dwSize);
#endif /*T5VFS_CFILES*/

#ifdef T5VFS_FLASH
    T5_WORD wItem;
    T5_DWORD dwBufSize;

    wItem = (T5_WORD)hVFS;
    if (!_T5Vfs_IsFileItem (wItem))
        return FALSE;
    if (!_T5Vfs_LinkFlash (wItem, &dwBufSize))
        return FALSE;
    if (dwPos != T5VFS_POS_CURRENT)
        return FALSE;
    if ((_FLASH_POS[wItem] + dwSize) > dwBufSize)
        return FALSE;
    if (!T5VFS_FLASH_WRITE_PACKET (wItem, _FLASH_POS[wItem] + sizeof (T5_DWORD),
                                   dwSize, pMem))
        return FALSE;
    _FLASH_POS[wItem] += dwSize;
    return TRUE;
#endif /*T5VFS_FLASH*/

    return FALSE;
}

/*****************************************************************************
T5Vfs_Close
Close an open file
Parameters:
    hVFS (IN) file handle
*****************************************************************************/

void T5Vfs_Close (T5_DWORD hVFS)
{
#ifdef T5VFS_CFILES
    T5VFS_FCLOSE (hVFS);
#endif /*T5VFS_CFILES*/
}

/*****************************************************************************
T5Vfs_Eof
Check the end of file mark of a file open for read
Parameters:
    hVFS (IN) file handle
Return: TRUE if end of file reached
*****************************************************************************/

T5_BOOL T5Vfs_Eof (T5_DWORD hVFS)
{
#ifdef T5VFS_CFILES
    return T5VFS_FEOF (hVFS);
#endif /*T5VFS_CFILES*/

#ifdef T5VFS_FLASH
    T5_WORD wItem;
    T5_DWORD dwBufSize;

    wItem = (T5_WORD)hVFS;
    if (!_T5Vfs_IsFileItem (wItem))
        return TRUE;
    if (_T5Vfs_LinkFlash (wItem, &dwBufSize) == NULL)
        return TRUE;
    return (_FLASH_POS[wItem] >= dwBufSize);
#endif /*T5VFS_FLASH*/

    return TRUE;
}

/*****************************************************************************
T5Vfs_Rename
Rename a file - TRUE FILES ONLY / NO SUPPORT FOR FLASH
Parameters:
    szOld (IN) current file name
    szNew (IN) new file name
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5Vfs_Rename (T5_PTCHAR szOld, T5_PTCHAR szNew)
{
#ifdef T5VFS_CFILES
    return T5VFS_FRENAME (szOld, szNew);
#endif /*T5VFS_CFILES*/
    /* no support for FLASH or SRAM */
    return FALSE;
}

/*****************************************************************************
T5Vfs_Delete
Remove a file - TRUE FILES ONLY / NO SUPPORT FOR FLASH
Parameters:
    szPath (IN) file name
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5Vfs_Delete (T5_PTCHAR szPath)
{
#ifdef T5VFS_CFILES
    return T5VFS_FDELETE (szPath);
#endif /*T5VFS_CFILES*/
    /* no support for FLASH or SRAM */
    return FALSE;
}

/****************************************************************************/

#endif /*T5DEF_VFS*/

/* eof **********************************************************************/
