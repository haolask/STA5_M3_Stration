/*****************************************************************************
T5Memory.c : system memory management - Use of Virtual File System
(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/*****************************************************************************
T5Memory_Alloc
allocates a block of memory
Parameters:
    mmb (IN/OUT) memory block descriptor
    dwSize (IN) wished block size
    pConf (IN) pointer to OEM data
return: OK or error
*****************************************************************************/

T5_RET T5Memory_Alloc (T5PTR_MMB mmb, T5_DWORD dwSize, T5_PTR pConf)
{
    if (mmb->wID != T5MM_VMDB)
        return T5RET_ERROR;
    /* memory block must be free */
    if (mmb->wFlags != 0)
        return T5RET_ERROR;
    /* try to allocate memory */
    mmb->mem = T5Vfs_CreateRamFile ((T5_PTCHAR)T5VFS_NAME_HOT, dwSize);
    if (mmb->mem == NULL)
        return T5RET_ERROR;
    T5_MEMSET (mmb->mem, 0, (size_t)dwSize);
    /* OK - done */
    mmb->wFlags = T5MMB_LOADED;
    mmb->wNbLink = 0;
    mmb->dwSize = dwSize;
    mmb->pData = NULL;
    return T5RET_OK;
}

/*****************************************************************************
T5Memory_Load
allocates a block of memory and load contents from backup support
Parameters:
    mmb (IN/OUT) memory block descriptor
    szAppName (IN) application name
    pConf (IN) pointer to OEM data
return: OK or error
*****************************************************************************/

T5_RET T5Memory_Load (T5PTR_MMB mmb, T5_PTCHAR szAppName, T5_PTR pConf)
{
    T5_DWORD dwSize;
    T5_BOOL bRet;
    T5_DWORD f;
    T5_PTCHAR szFile;

    /* memory block must be free */
    if (mmb->wFlags != 0)
        return T5RET_ERROR;

    /* allocate and load in RAM in case of VMDB */
    if (mmb->wID == T5MM_VMDB)
    {
        if (!T5Vfs_GetSize ((T5_PTCHAR)T5VFS_NAME_HOT, &dwSize))
            return T5RET_ERROR;
        if (T5Memory_Alloc (mmb, dwSize, pConf) != T5RET_OK)
            return T5RET_ERROR;
        if ((f = T5Vfs_OpenReadRand ((T5_PTCHAR)T5VFS_NAME_HOT, NULL)) == 0L)
            bRet = FALSE;
        else
        {
            bRet = (T5Vfs_Read (f, 0, dwSize, mmb->mem) == dwSize);
            T5Vfs_Close (f);
        }
        if (!bRet)
        {
            T5Vfs_ReleaseRamFile (mmb->mem);
            mmb->mem = NULL;
            mmb->wFlags = 0;
            mmb->wNbLink = 0;
            mmb->dwSize = 0L;
            mmb->pData = NULL;
            return T5RET_ERROR;
        }
        else
            return T5RET_OK;
    }

    /* others: simply load from file or flash */
    switch (mmb->wID)
    {
    case T5MM_CODE   : szFile = (T5_PTCHAR)T5VFS_NAME_COD; break;
    case T5MM_CHANGE : szFile = (T5_PTCHAR)T5VFS_NAME_UPD; break;
    default          : return T5RET_ERROR;
    }
    mmb->mem = T5Vfs_LoadToMemory (szFile, &dwSize);
    if (mmb->mem == NULL || dwSize == 0L)
        return T5RET_ERROR;

    mmb->wFlags = T5MMB_LOADED;
    mmb->wNbLink = 0;
    mmb->dwSize = dwSize;
    mmb->pData = NULL;
    return T5RET_OK;
}

/*****************************************************************************
T5Memory_Free
release a block of memory
Parameters:
    mmb (IN/OUT) memory block descriptor
    pConf (IN) pointer to OEM data
return: OK or error
*****************************************************************************/

T5_RET T5Memory_Free (T5PTR_MMB mmb, T5_PTR pConf)
{
    /* memory block must be allocated with no client linked */
    if (mmb->wFlags != T5MMB_LOADED)
        return T5RET_ERROR;

    /* VMDB is always in RAM */
    if (mmb->wID == T5MM_VMDB)
        T5Vfs_ReleaseRamFile (mmb->mem);
    /* others are from disk or flash */
    else
        T5Vfs_ReleaseFromMemory (mmb->mem);

    /* OK - done */
    mmb->mem = NULL;
    mmb->wFlags = 0;
    mmb->wNbLink = 0;
    mmb->dwSize = 0L;
    mmb->pData = NULL;
    return T5RET_OK;
}

/*****************************************************************************
T5Memory_Link
establish a link to a block of memory
Parameters:
    mmb (IN/OUT) memory block descriptor
    pConf (IN) pointer to OEM data
return: OK or error
*****************************************************************************/

T5_RET T5Memory_Link (T5PTR_MMB mmb, T5_PTR pConf)
{
    /* memory must be allocated */
    if ((mmb->wFlags & T5MMB_LOADED) == 0)
        return T5RET_ERROR;
    /* link */
    mmb->wNbLink += 1;
    mmb->wFlags |= T5MMB_LINKED;
    mmb->pData = mmb->mem;
    return T5RET_OK;
}

/*****************************************************************************
T5Memory_Unink
release a link to a block of memory
Parameters:
    mmb (IN/OUT) memory block descriptor
    pConf (IN) pointer to OEM data
return: OK or error
*****************************************************************************/

T5_RET T5Memory_Unlink (T5PTR_MMB mmb, T5_PTR pConf)
{
    /* memory must be allocated */
    if ((mmb->wFlags & T5MMB_LOADED) == 0)
        return T5RET_ERROR;
    /* memory must be linked */
    if ((mmb->wFlags & T5MMB_LINKED) == 0)
        return T5RET_ERROR;
    /* unlink */
    mmb->wNbLink -= 1;
    if (mmb->wNbLink == 0)
    {
        mmb->wFlags &= ~T5MMB_LINKED;
        mmb->pData = NULL;
    }
    return T5RET_OK;
}

/*****************************************************************************
T5Memory_StartWrite
start writing backup support for download
Parameters:
    mmb (IN/OUT) memory block descriptor
    pLoad (IN) download information
    pConf (IN) pointer to OEM data
return: OK or error
*****************************************************************************/

T5_RET T5Memory_StartWrite (T5PTR_MMB mmb, T5PTR_CSLOAD pLoad, T5_PTR pConf)
{
    T5_PTCHAR szFile;
    T5_DWORD f;

    /* memory must be free */
    if (mmb->wFlags != 0)
        return T5RET_ERROR;
    /* check if this kind of block can be loaded */
    switch (mmb->wID)
    {
    case T5MM_CODE   : szFile = (T5_PTCHAR)T5VFS_NAME_COD; break;
    case T5MM_CHANGE : szFile = (T5_PTCHAR)T5VFS_NAME_UPD; break;
    default          : return T5RET_ERROR;
    }
    /* create the file empty with the wished total size */
    if ((f = T5Vfs_OpenWriteCreate (szFile, pLoad->dwSize)) == 0)
        return T5RET_ERROR;
    T5Vfs_Close (f);
    return T5RET_OK;
}

/*****************************************************************************
T5Memory_Write
write a packet of data to backup support during download
Parameters:
    mmb (IN/OUT) memory block descriptor
    pLoad (IN) download information
    pConf (IN) pointer to OEM data
return: OK or error
*****************************************************************************/

T5_RET T5Memory_Write (T5PTR_MMB mmb, T5PTR_CSLOAD pLoad, T5_PTR pConf)
{
    T5_PTCHAR szFile;
    T5_DWORD f;

    /* memory must be free */
    if (mmb->wFlags != 0)
        return T5RET_ERROR;
    /* check if this kind of block can be loaded */
    switch (mmb->wID)
    {
    case T5MM_CODE   : szFile = (T5_PTCHAR)T5VFS_NAME_COD; break;
    case T5MM_CHANGE : szFile = (T5_PTCHAR)T5VFS_NAME_UPD; break;
    default          : return T5RET_ERROR;
    }
    /* open in Append mode and add packet */
    if ((f = T5Vfs_OpenWriteAppend (szFile, pLoad->dwSize)) == 0)
        return T5RET_ERROR;
    if (!T5Vfs_Write (f, T5VFS_POS_CURRENT, pLoad->dwSize, pLoad->pData))
    {
        T5Vfs_Close (f);
        return T5RET_ERROR;
    }
    T5Vfs_Close (f);
    return T5RET_OK;
}

/*****************************************************************************
T5Memory_EndWrite
finish writing backup support at the end of download
Parameters:
    mmb (IN/OUT) memory block descriptor
    pLoad (IN) download information
    pConf (IN) pointer to OEM data
return: OK or error
*****************************************************************************/

T5_RET T5Memory_EndWrite (T5PTR_MMB mmb, T5PTR_CSLOAD pLoad, T5_PTR pConf)
{
    T5_PTCHAR szFile;

    /* memory must be free */
    if (mmb->wFlags != 0)
        return T5RET_ERROR;
    /* check if this kind of block can be loaded */
    switch (mmb->wID)
    {
    case T5MM_CODE   : szFile = (T5_PTCHAR)T5VFS_NAME_COD; break;
    case T5MM_CHANGE : szFile = (T5_PTCHAR)T5VFS_NAME_UPD; break;
    default          : return T5RET_ERROR;
    }
    if (szFile == NULL)
        return T5RET_ERROR;
    /* ok */
    return T5RET_OK;
}

/*****************************************************************************
T5Memory_Save
Save a memory block to backup support
Parameters:
    mmb (IN/OUT) memory block descriptor
    szAppName (IN) application name
    pConf (IN) pointer to OEM data
return: OK or error
*****************************************************************************/

T5_RET T5Memory_Save (T5PTR_MMB mmb, T5_PTCHAR szAppName, T5_PTR pConf)
{
    T5_PTCHAR szFile;
	T5_DWORD f;

    /* memory must be allocated */
    if ((mmb->wFlags & T5MMB_LOADED) == 0)
        return T5RET_ERROR;
    /* check if this kind of block can be saved */
    switch (mmb->wID)
    {
    case T5MM_CODE : szFile = (T5_PTCHAR)T5VFS_NAME_COD; break; /* maybe for changes */
    case T5MM_VMDB : szFile = (T5_PTCHAR)T5VFS_NAME_HOT; break;
    default        : return T5RET_ERROR;
    }
    if (szFile == NULL)
        return T5RET_ERROR;
    /* open the file for creation and copy whole data */
    if ((f = T5Vfs_OpenWriteCreate (szFile, mmb->dwSize)) == 0)
        return T5RET_ERROR;
    if (!T5Vfs_Write (f, T5VFS_POS_CURRENT, mmb->dwSize, mmb->mem))
    {
        T5Vfs_Close (f);
        return T5RET_ERROR;
    }
    T5Vfs_Close (f);
    return T5RET_OK;
}

/* eof **********************************************************************/
