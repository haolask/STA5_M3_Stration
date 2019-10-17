/*****************************************************************************
T5vfs.h :    virtual file system - definitions

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#ifndef _T5VFS_H_INCLUDED_
#define _T5VFS_H_INCLUDED_

#ifdef T5DEF_VFS

/****************************************************************************/

#define T5VFS_SIZE_INFINITE     0xffffffffL
#define T5VFS_POS_CURRENT       0xffffffffL

/****************************************************************************/
/* APIs */

extern void T5Vfs_CleanSRAMBuffers (void);

extern T5_DWORD T5Vfs_GetMaxRamFileSize (T5_PTCHAR szPath);
extern T5_PTR T5Vfs_CreateRamFile (T5_PTCHAR szPath, T5_DWORD dwSize);
extern void T5Vfs_ReleaseRamFile (T5_PTR pMem);

extern T5_BOOL T5Vfs_FileInFlash (void);

extern T5_BOOL T5Vfs_GetSize (T5_PTCHAR szPath, T5_PTDWORD pdwSize);
extern T5_PTR T5Vfs_LoadToMemory (T5_PTCHAR szPath, T5_PTDWORD pdwSize);
extern void T5Vfs_ReleaseFromMemory (T5_PTR pMem);

extern T5_DWORD T5Vfs_OpenReadRand (T5_PTCHAR szPath, T5_PTDWORD pdwSize);
extern T5_DWORD T5Vfs_Read (T5_DWORD hVFS, T5_DWORD dwPos,
                            T5_DWORD dwSize, T5_PTR pMem);

extern T5_DWORD T5Vfs_OpenWriteCreate (T5_PTCHAR szPath,
                                       T5_DWORD dwWishedSize);
extern T5_DWORD T5Vfs_OpenWriteAppend (T5_PTCHAR szPath,
                                       T5_DWORD dwWishedSize);
extern T5_BOOL T5Vfs_Write (T5_DWORD hVFS, T5_DWORD dwPos,
                            T5_DWORD dwSize, T5_PTR pMem);

extern void T5Vfs_Close (T5_DWORD hVFS);
extern T5_BOOL T5Vfs_Eof (T5_DWORD hVFS);
extern T5_BOOL T5Vfs_Rename (T5_PTCHAR szOld, T5_PTCHAR szNew);
extern T5_BOOL T5Vfs_Delete (T5_PTCHAR szPath);

/****************************************************************************/

#endif /*T5DEF_VFS*/

#endif /*_T5VFS_H_INCLUDED_*/

/* eof **********************************************************************/
