/*****************************************************************************
T5vfsoem.h : virtual file system - OEM definitions and implementation choices

TO BE FILLED DURING PORTING

This file is included only once in whole STRATON software.
So you can declare some static data and implement some functions here

(c) COPALP 2009
*****************************************************************************/

#ifndef _T5VFSOEM_H_INCLUDED_
#define _T5VFSOEM_H_INCLUDED_

#ifdef T5DEF_VFS

/****************************************************************************/
/* predefined memory / file items - see this file: */

#include "t5vfsitm.h"

#include "t5flash.h"

/****************************************************************************/
/* implementation of RAM memory buffers - use MALLOC from "C" */
/* see list of possible items in t5vfsitm.h */

#undef T5VFS_MALLOC

/****************************************************************************/
/* implementation of RAM memory buffers - use fixed static buffers */
/* see list of possible _RAM_ items in t5vfsitm.h */

#define T5VFS_RAMBUF

#ifdef T5VFS_RAMBUF

#define T5VFS_RAM_BUFPOINTER(item) \
    ((item == T5VFS_RAM_VMDB) ? T5Ram_Ptr() : NULL)

#define T5VFS_RAM_BUFSIZE(item) \
    (T5_DWORD)((item == T5VFS_RAM_VMDB) ? T5Ram_Size() : 0L)

#endif /*T5VFS_RAMBUF*/

/****************************************************************************/
/* implementation file system - use "C" file system */

#undef T5VFS_CFILES

#ifdef T5VFS_CFILES

#define T5VFS_FOPENRB(s)    (T5_DWORD)fopen ((s),"rb")
#define T5VFS_FOPENWB(s)    (T5_DWORD)fopen ((s),"wb")
#define T5VFS_FOPENAB(s)    (T5_DWORD)fopen ((s),"ab")
#define T5VFS_FCLOSE(f)     fclose ((FILE *)(f))
#define T5VFS_FSEEK(f,p)    (T5_DWORD)(fseek ((FILE *)(f), (p), SEEK_SET))
#define T5VFS_FREAD(d,n,f)  (T5_DWORD)fread ((d), 1, (size_t)(n), (FILE *)(f))
#define T5VFS_FWRITE(d,n,f) (T5_DWORD)fwrite ((d), 1, (size_t)(n), (FILE *)(f))
#define T5VFS_FEOF(f)       (T5_BOOL)feof ((FILE *)(f))
#define T5VFS_FRENAME(o,n)  (T5_BOOL)(rename ((o), (n)) == 0)
#define T5VFS_FDELETE(s)    (T5_BOOL)(remove (s) == 0)

static T5_DWORD T5VFS_GETSIZE (T5_PTCHAR szPath)
{
	struct stat st;
	if (stat (szPath, &st) != 0)
        return 0L;
    return (T5_DWORD)(st.st_size);
}

#endif /*T5VFS_CFILES*/

/****************************************************************************/
/* implementation file system - use flash or battery RAM */
/* see list of possible _FILE_ items in t5vfsitm.h */

#define T5VFS_FLASH

#ifdef T5VFS_FLASH

#define T5VFS_FLASH_BUFPOINTER(item) \
    ((item == T5VFS_RAM_CODE) ? T5Flash_Ptr() : NULL)
#define T5VFS_FLASH_BUFSIZE(item) \
    (T5_DWORD)((item == T5VFS_RAM_CODE) ? T5Flash_Size() : 0L)

/* following macros should memset/memcpy in case of battery RAM */
/* they should return TRUE/FALSE */
/* offsets and sizes are safe as already chrcked by STRATON */

#define T5VFS_FLASH_ERASE_BUFFER(item) \
    ((item == T5VFS_RAM_CODE) ? T5Flash_Erase() : FALSE)

#define T5VFS_FLASH_WRITE_PACKET(item, dwOffset, dwSize, pData) \
    ((item == T5VFS_RAM_CODE) ? T5Flash_Write(dwOffset, dwSize, pData) : FALSE)

#endif /*T5VFS_FLASH*/

/****************************************************************************/

#endif /*T5DEF_VFS*/

#endif /*_T5VFSOEM_H_INCLUDED_*/

/* eof **********************************************************************/
