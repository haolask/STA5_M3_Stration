/*****************************************************************************
T5heap.c :   safe heap - core implementation

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#include "t5vm.h"

#define T5_ALIGNED4(s)      (((s)%4)?((s)+4-((s)%4)):(s))

/****************************************************************************/
/* static functions */

static void _T5Heap_FreeAll_MALLOC (T5PTR_HEAP pHeap);
static T5_DWORD _T5Heap_Alloc_MALLOC (T5PTR_HEAP pHeap, T5_DWORD dwSize);
static void _T5Heap_Free_MALLOC (T5PTR_HEAP pHeap, T5_DWORD hMem);
static T5_PTR _T5Heap_Lock_MALLOC (T5PTR_HEAP pHeap, T5_DWORD hMem);
static void _T5Heap_Unlock_MALLOC (T5PTR_HEAP pHeap, T5_DWORD hMem);
static T5_DWORD _T5Heap_NbAlloc_MALLOC (T5PTR_HEAP pHeap);

static void _T5Heap_FreeAll_BUFFER (T5PTR_HEAP pHeap);
static T5_BOOL _T5Heap_Compact_BUFFER (T5PTR_HEAP pHeap);
static T5_DWORD _T5Heap_Alloc_BUFFER (T5PTR_HEAP pHeap, T5_DWORD dwSize);
static void _T5Heap_Free_BUFFER (T5PTR_HEAP pHeap, T5_DWORD hMem);
static T5_PTR _T5Heap_Lock_BUFFER (T5PTR_HEAP pHeap, T5_DWORD hMem);
static void _T5Heap_Unlock_BUFFER (T5PTR_HEAP pHeap, T5_DWORD hMem);
static T5_DWORD _T5Heap_NbAlloc_BUFFER (T5PTR_HEAP pHeap);

static T5_DWORD _T5Heap_GetFreeSpace_BUFFER (T5PTR_HEAP pHeap);
static T5PTR_HEAPITEM _T5Heap_GetByPos (T5PTR_HEAP pHeap, T5_DWORD dwPos);

/****************************************************************************/
/* generic calls

The following functions are valid for any kind of heap.
Notes:
- T5Heap_GetFreeSpace() means nothing for a heap using malloc()
- T5Heap_Compact() makes nothing for a heap using malloc()

For heaps implemented in static buffers, the application is responsible for:
- locking records and unlocking them immediately after use
- call T5Heap_Compact() regularly to avoid heap fragmentation
Heaps in static buffers cannot be compacted correctly if some records remain
locked in memory.

*/

/*****************************************************************************
T5Heap_DeleteHeap
Release a heap an free all related memory
Parameters:
     pHeap (IN) pointer to main heap
*****************************************************************************/

void T5Heap_DeleteHeap (T5_PTR pHeap)
{
    T5Heap_FreeAll (pHeap);
    if (T5_HEAP_ISMALLOC (pHeap))
        T5_FREE (pHeap);
}

/*****************************************************************************
T5Heap_FreeAll
Free all items allocated in a heap (even if locked)
Parameters:
     pHeap (IN) pointer to main heap
*****************************************************************************/

void T5Heap_FreeAll (T5_PTR pHeap)
{
    if (T5_HEAP_ISMALLOC (pHeap))
        _T5Heap_FreeAll_MALLOC ((T5PTR_HEAP)pHeap);
    else
        _T5Heap_FreeAll_BUFFER ((T5PTR_HEAP)pHeap);
}

/*****************************************************************************
T5Heap_GetFreeSpace
Return the number of bytes available for user memory in a heap
Parameters:
     pHeap (IN) pointer to main heap
Return: available size for static buffers - 0xffffffffL for malloc heap
*****************************************************************************/

T5_DWORD T5Heap_GetFreeSpace (T5_PTR pHeap)
{
    if (T5_HEAP_ISMALLOC (pHeap))
        return 0xffffffffL;
    return _T5Heap_GetFreeSpace_BUFFER ((T5PTR_HEAP)pHeap);
}

/*****************************************************************************
T5Heap_Compact
Compact a heap allocated in a static buffer
Parameters:
     pHeap (IN) pointer to main heap
     dwMaxLoop (IN) maximum number of movements to be performed (>= 1)
Return: number of movements performed
*****************************************************************************/

T5_DWORD T5Heap_Compact (T5_PTR pHeap, T5_DWORD dwMaxLoop)
{
    T5_DWORD dwDone;
    T5_BOOL bRet;

    if (T5_HEAP_ISMALLOC (pHeap))
        return 0;

    if (dwMaxLoop == 0L)
        dwMaxLoop = 1L;
    dwDone = 0L;
    bRet = TRUE;
    while (dwDone < dwMaxLoop && bRet)
    {
        bRet = _T5Heap_Compact_BUFFER ((T5PTR_HEAP)pHeap);
        if (bRet)
            dwDone += 1;
    }
    return dwDone;
}

/*****************************************************************************
T5Heap_Alloc
Allocate a new record
Parameters:
     pHeap (IN) pointer to main heap
     dwSize (IN) wished size in bytes
Return: handle of the memory object - 0 if fail
*****************************************************************************/

T5_DWORD T5Heap_Alloc (T5_PTR pHeap, T5_DWORD dwSize)
{
    if (T5_HEAP_ISMALLOC (pHeap))
        return _T5Heap_Alloc_MALLOC ((T5PTR_HEAP)pHeap, dwSize);
    else
        return _T5Heap_Alloc_BUFFER ((T5PTR_HEAP)pHeap, dwSize);
}

/*****************************************************************************
T5Heap_Free
Release a record (even if locked)
Parameters:
     pHeap (IN) pointer to main heap
     hMem (IN) handle of the memory object
*****************************************************************************/

void T5Heap_Free (T5_PTR pHeap, T5_DWORD hMem)
{
    if (T5_HEAP_ISMALLOC (pHeap))
        _T5Heap_Free_MALLOC ((T5PTR_HEAP)pHeap, hMem);
    else
        _T5Heap_Free_BUFFER ((T5PTR_HEAP)pHeap, hMem);
}

/*****************************************************************************
T5Heap_Lock
Lock a record in memory and get the pointer to user data
Parameters:
     pHeap (IN) pointer to main heap
     hMem (IN) handle of the memory object
Return: pointer to user data - NULL if fail
*****************************************************************************/

T5_PTR T5Heap_Lock (T5_PTR pHeap, T5_DWORD hMem)
{
    if (T5_HEAP_ISMALLOC (pHeap))
        return _T5Heap_Lock_MALLOC ((T5PTR_HEAP)pHeap, hMem);
    else
        return _T5Heap_Lock_BUFFER ((T5PTR_HEAP)pHeap, hMem);
}

/*****************************************************************************
T5Heap_Unlock
Unlock a record in memory
The record can then be moved in case of static buffers
Parameters:
     pHeap (IN) pointer to main heap
     hMem (IN) handle of the memory object
*****************************************************************************/

void T5Heap_Unlock (T5_PTR pHeap, T5_DWORD hMem)
{
    if (T5_HEAP_ISMALLOC (pHeap))
        _T5Heap_Unlock_MALLOC ((T5PTR_HEAP)pHeap, hMem);
    else
        _T5Heap_Unlock_BUFFER ((T5PTR_HEAP)pHeap, hMem);
}

/*****************************************************************************
T5Heap_NbAlloc
returns the number of allocated records (for debug only)
Parameters:
     pHeap (IN) pointer to main heap
Return: number of records currently allocated
*****************************************************************************/

T5_DWORD T5Heap_NbAlloc (T5_PTR pHeap)
{
    if (pHeap == NULL)
        return 0;
    else if (T5_HEAP_ISMALLOC (pHeap))
        return _T5Heap_NbAlloc_MALLOC ((T5PTR_HEAP)pHeap);
    else
        return _T5Heap_NbAlloc_BUFFER ((T5PTR_HEAP)pHeap);
}

/****************************************************************************/
/* MALLOC 

The following functions implement a safe heap based on malloc() and free()
functions from the see language.
The macros T5_MALLOC and T5_FREE must be defined.

*/

/*****************************************************************************
T5Heap_CreateHeapMalloc
Create a safe heap based on malloc/free "C" calls
Parameters:
     dwGrowBy (IN) packet size for allocation of handles
     dwFlags (IN) reserved for extensions
Return: pointer to main heap entry
*****************************************************************************/

T5_PTR T5Heap_CreateHeapMalloc (T5_DWORD dwGrowBy, T5_DWORD dwFlags)
{
#ifdef T5DEF_MALLOCSUPPORTED
    T5PTR_HEAP pHeap;
    T5_UNUSED(dwFlags);

    if (dwGrowBy < 3)
        dwGrowBy = 3;
    /* allocate main block */
    pHeap = (T5PTR_HEAP)T5_MALLOC (sizeof (T5STR_HEAP));
    if (pHeap == NULL)
        return NULL;
    T5_MEMSET (pHeap, 0, sizeof (T5STR_HEAP));
    pHeap->dwGrowBy = dwGrowBy;
    pHeap->dwFlags |= T5HEAP_MALLOC;
    return pHeap;
#else /*T5DEF_MALLOCSUPPORTED*/
    return NULL;
#endif /*T5DEF_MALLOCSUPPORTED*/
}

/*****************************************************************************
_T5Heap_FreeAll_MALLOC
Free all items allocated in a malloc heap (even if locked)
Space for handles is also released
Parameters:
     pHeap (IN) pointer to main heap
*****************************************************************************/

static void _T5Heap_FreeAll_MALLOC (T5PTR_HEAP pHeap)
{
#ifdef T5DEF_MALLOCSUPPORTED
    T5_PTR *pBuffer, pNext;
    T5_DWORD i;

    /* for all blocks */
    pBuffer = (T5_PTR *)(pHeap->pBuffer);
    while (pBuffer != NULL)
    {
        /* free used items */
        for (i=0; i<pHeap->dwGrowBy; i++)
        {
            if (pBuffer[i] != NULL)
                T5_FREE (pBuffer[i]);
        }
        pNext = (T5_PTR *)(pBuffer[pHeap->dwGrowBy]);
        /* free block */
        T5_FREE (pBuffer);
        /* next block */
        pBuffer = (T5_PTR *)pNext;
    }
    pHeap->pBuffer = NULL;
#endif /*T5DEF_MALLOCSUPPORTED*/
}

/*****************************************************************************
_T5Heap_NbAlloc_MALLOC
returns the number of allocated records (for debug only)
Parameters:
     pHeap (IN) pointer to main heap
Return: number of records currently allocated
*****************************************************************************/

static T5_DWORD _T5Heap_NbAlloc_MALLOC (T5PTR_HEAP pHeap)
{
    T5_DWORD dwCount = 0;

#ifdef T5DEF_MALLOCSUPPORTED
    T5_PTR *pBuffer, pNext;
    T5_DWORD i;

    /* for all blocks */
    pBuffer = (T5_PTR *)(pHeap->pBuffer);
    while (pBuffer != NULL)
    {
        /* free used items */
        for (i=0; i<pHeap->dwGrowBy; i++)
        {
            if (pBuffer[i] != NULL)
                dwCount += 1;
        }
        pNext = (T5_PTR *)(pBuffer[pHeap->dwGrowBy]);
        /* next block */
        pBuffer = (T5_PTR *)pNext;
    }
#endif /*T5DEF_MALLOCSUPPORTED*/
    return dwCount;
}

/*****************************************************************************
_T5Heap_Alloc_MALLOC
Allocate a new record in a malloc heap
Parameters:
     pHeap (IN) pointer to main heap
     dwSize (IN) wished size in bytes
Return: handle of the memory object - 0 if fail
*****************************************************************************/

static T5_DWORD _T5Heap_Alloc_MALLOC (T5PTR_HEAP pHeap, T5_DWORD dwSize)
{
#ifdef T5DEF_MALLOCSUPPORTED
    T5_PTR *pBuffer;
    T5_DWORD i, hMem;
    T5_BOOL bSearch;

    if (dwSize == 0L)
        return 0L;
    /* handle counter - start at 1 */
    hMem = 1;
    /* create first block if heap is empty */
    if (pHeap->pBuffer == NULL)
    {
        pHeap->pBuffer = (T5_PTBYTE)T5_MALLOC (
            (pHeap->dwGrowBy + 1) * sizeof (T5_PTR)
            );
        if (pHeap->pBuffer == NULL)
            return 0L;
        T5_MEMSET (
            pHeap->pBuffer, 0,
            (pHeap->dwGrowBy + 1) * sizeof (T5_PTR)
            );
        pBuffer = (T5_PTR *)(pHeap->pBuffer);
        /* allocate item */
        pBuffer[0] = T5_MALLOC (dwSize);
        if (pBuffer[0] == NULL)
            return 0L;
        T5_MEMSET (pBuffer[0], 0, dwSize);
        return hMem;
    }
    /* search for unused item in all blocks */
    else
    {
        pBuffer = (T5_PTR *)(pHeap->pBuffer);
        bSearch = TRUE;
        while (bSearch)
        {
            for (i=0; i<pHeap->dwGrowBy; i++)
            {
                if (pBuffer[i] == NULL)
                {
                    pBuffer[i] = T5_MALLOC (dwSize);
                    if (pBuffer[i] == NULL)
                        return 0L;
                    T5_MEMSET (pBuffer[i], 0, dwSize);
                    return hMem;
                }
                hMem += 1L;
            }
            if (pBuffer[pHeap->dwGrowBy] != NULL)
                pBuffer = (T5_PTR *)(pBuffer[pHeap->dwGrowBy]);
            else
                bSearch = FALSE;
        }
    }
    /* allocate a new block */
    pBuffer[pHeap->dwGrowBy] = T5_MALLOC (
        (pHeap->dwGrowBy + 1) * sizeof (T5_PTR)
        );
    if (pBuffer[pHeap->dwGrowBy] == NULL)
        return 0L;
    T5_MEMSET (
        pBuffer[pHeap->dwGrowBy], 0,
        (pHeap->dwGrowBy + 1) * sizeof (T5_PTR)
        );
    /* allocate the first item of the new block */
    pBuffer = (T5_PTR *)(pBuffer[pHeap->dwGrowBy]);
    pBuffer[0] = T5_MALLOC (dwSize);
    if (pBuffer[0] == NULL)
        return 0L;
    T5_MEMSET (pBuffer[0], 0, dwSize);
    return hMem;
#else /*T5DEF_MALLOCSUPPORTED*/
    return 0L;
#endif /*T5DEF_MALLOCSUPPORTED*/
}

/*****************************************************************************
_T5Heap_Free_MALLOC
Release a record of a malloc heap (even if locked)
Parameters:
     pHeap (IN) pointer to main heap
     hMem (IN) handle of the memory object
*****************************************************************************/

static void _T5Heap_Free_MALLOC (T5PTR_HEAP pHeap, T5_DWORD hMem)
{
#ifdef T5DEF_MALLOCSUPPORTED
    T5_PTR *pBuffer;

    if (hMem == 0)
        return;
    hMem -= 1L;
    /* search for the block */
    pBuffer = (T5_PTR *)(pHeap->pBuffer);
    while (pBuffer != NULL && hMem >= pHeap->dwGrowBy)
    {
        pBuffer = (T5_PTR *)(pBuffer[pHeap->dwGrowBy]);
        hMem -= pHeap->dwGrowBy;
    }
    /* free item in the block found */
    if (pBuffer != NULL && pBuffer[hMem] != NULL)
    {
        T5_FREE (pBuffer[hMem]);
        pBuffer[hMem] = NULL;
    }
#endif /*T5DEF_MALLOCSUPPORTED*/
}

/*****************************************************************************
_T5Heap_Lock_MALLOC
Lock a record of a malloc heap and get the pointer to user data
Parameters:
     pHeap (IN) pointer to main heap
     hMem (IN) handle of the memory object
Return: pointer to user data - NULL if fail
*****************************************************************************/

static T5_PTR _T5Heap_Lock_MALLOC (T5PTR_HEAP pHeap, T5_DWORD hMem)
{
#ifdef T5DEF_MALLOCSUPPORTED
    T5_PTR *pBuffer;

    if (hMem == 0)
        return NULL;
    hMem -= 1L;
    /* search for the block */
    pBuffer = (T5_PTR *)(pHeap->pBuffer);
    while (pBuffer != NULL && hMem >= pHeap->dwGrowBy)
    {
        pBuffer = (T5_PTR *)(pBuffer[pHeap->dwGrowBy]);
        hMem -= pHeap->dwGrowBy;
    }
    /* find item in the block */
    if (pBuffer != NULL)
        return (pBuffer[hMem]);
    return NULL;
#else /*T5DEF_MALLOCSUPPORTED*/
    return NULL;
#endif /*T5DEF_MALLOCSUPPORTED*/
}

/*****************************************************************************
_T5Heap_Unlock_MALLOC
Unlock a record in memory (makes nothing particular for malloc heaps)
Parameters:
     pHeap (IN) pointer to main heap
     hMem (IN) handle of the memory object
*****************************************************************************/

static void _T5Heap_Unlock_MALLOC (T5PTR_HEAP pHeap, T5_DWORD hMem)
{
    T5_UNUSED(pHeap);
    T5_UNUSED(hMem);
    /* nothing */
}

/****************************************************************************/
/* BUFFER

The following functions implement a safe heap in a static fixed size buffer.
It is required that the buffer starts at an address multiple of 4

*/

/*****************************************************************************
T5Heap_CreateHeapBuffer
Create a safe heap in a static buffer
Parameters:
     pBuffer (IN) pointer to the static buffer
     dwSize (IN) size of the buffer (must be aligned at 4)
     dwFlags (IN) reserved for extensions
Return: pointer to main heap entry
*****************************************************************************/

T5_PTR T5Heap_CreateHeapBuffer (T5_PTR pBuffer, T5_DWORD dwSize,
                                T5_DWORD dwFlags)
{
    T5_PTBYTE pRaw;
    T5PTR_HEAP pHeap;

    dwSize -= (dwSize % 4);
    if (dwSize <= (sizeof (T5STR_HEAP) + 8))
        return FALSE;

    T5_MEMSET (pBuffer, 0, dwSize);

    pRaw = (T5_PTBYTE)pBuffer;
    T5_MEMCPY (pRaw, "T5SH", 4);
    T5_MEMCPY (pRaw+4, &dwSize, 4);
    T5_MEMCPY (pRaw+8, &pBuffer, 4);
    pBuffer = (T5_PTR)(pRaw + 12);
    dwSize -= 12L;

    dwSize -= sizeof (T5STR_HEAP);

    pRaw = (T5_PTBYTE)pBuffer + dwSize;
    pHeap = (T5PTR_HEAP)pRaw;

    pHeap->pBuffer = (T5_PTBYTE)pBuffer;
    pHeap->dwBufSize = dwSize;
    pHeap->dwFlags = dwFlags & ~T5HEAP_MALLOC;

    return pHeap;
}

/*****************************************************************************
T5Heap_RelocateHeapBuffer
Relocate a safe heap in a static buffer - for hot restart
Parameters:
     pBuffer (IN) pointer to the static buffer
     dwSize (IN) size of the buffer (must be aligned at 4)
     dwFlags (IN) reserved for extensions
Return: pointer to main heap entry or NULL if relocation not possible
*****************************************************************************/

T5_PTR T5Heap_RelocateHeapBuffer (T5_PTR pBuffer, T5_DWORD dwSize,
                                  T5_DWORD dwFlags)
{
    T5_PTDWORD pdw;
    T5_PTBYTE pRaw, pOldBuffer;
    T5PTR_HEAP pHeap;
    T5PTR_HEAPITEM pItem;
    T5_DWORD i;
    T5_UNUSED(dwFlags);

    pdw = (T5_PTDWORD)pBuffer;
    if (T5_MEMCMP (pdw, "T5SH", 4) != 0)
        return NULL;
    if (pdw[1] != dwSize)
        return NULL;

    pRaw = (T5_PTBYTE)pBuffer;
    T5_MEMCPY (&pOldBuffer, pRaw + 8, 4);

    pHeap = (T5PTR_HEAP)(pRaw + (dwSize - sizeof (T5STR_HEAP)));
    pHeap->pBuffer = pRaw + 12;

    T5_MEMCPY (pRaw+8, &pBuffer, 4);

    pItem = (T5PTR_HEAPITEM)pHeap;
    pItem--;
    for (i=0; i<pHeap->dwNbID; i++, pItem--)
    {
        if (pItem->pData != NULL)
        {
            pItem->pData -= (T5_DWORD)pOldBuffer;
            pItem->pData += (T5_DWORD)pBuffer;
        }
    }
    return (T5_PTR)pHeap;
}

/*****************************************************************************
_T5Heap_FreeAll_BUFFER
Free all items allocated in a static buffer heap (even if locked)
Parameters:
     pHeap (IN) pointer to main heap
*****************************************************************************/

static void _T5Heap_FreeAll_BUFFER (T5PTR_HEAP pHeap)
{
    pHeap->dwNbID = 0L;
    pHeap->dwGrowBy = 0L;
    pHeap->dwNbFree = 0L;
}

/*****************************************************************************
_T5Heap_Alloc_BUFFER
Allocate a new record in a static heap
Parameters:
     pHeap (IN) pointer to main heap
     dwSize (IN) wished size in bytes
Return: handle of the memory object - 0 if fail
*****************************************************************************/

static T5_DWORD _T5Heap_Alloc_BUFFER (T5PTR_HEAP pHeap, T5_DWORD dwSize)
{
    T5_DWORD hMem;
    T5PTR_HEAPITEM pItem;

    /* check aand fix size */
    if (dwSize == 0L)
        return 0L;
    dwSize = T5_ALIGNED4 (dwSize);
    /* check available space */
    if (dwSize + pHeap->dwGrowBy
        + ((pHeap->dwNbID+1) * sizeof (T5STR_HEAPITEM))
            > pHeap->dwBufSize)
        return 0L;
    /* alloc or reuse ID */
    pItem = (T5PTR_HEAPITEM)pHeap;
    hMem = 1;
    pItem--;
    while (hMem <= pHeap->dwNbID && T5HEAP_ITEMEXIST(pItem))
    {
        pItem--;
        hMem++;
    }
    if (hMem > pHeap->dwNbID)
        pHeap->dwNbID = hMem;
    /* create record */
    pItem->dwSize = dwSize; /* not locked - not freed */
    pItem->pData = pHeap->pBuffer + pHeap->dwGrowBy;
    pHeap->dwGrowBy += dwSize;
    /* fill user memory with null bytes */
    T5_MEMSET (pItem->pData, 0, pItem->dwSize);
    return hMem;
}

/*****************************************************************************
_T5Heap_Free_BUFFER
Release a record in a static heap (even if locked)
Parameters:
     pHeap (IN) pointer to main heap
     hMem (IN) handle of the memory object
*****************************************************************************/

static void _T5Heap_Free_BUFFER (T5PTR_HEAP pHeap, T5_DWORD hMem)
{
    T5PTR_HEAPITEM pItem;

    if (hMem < 1 || hMem > pHeap->dwNbID)
        return;

    pItem = (T5PTR_HEAPITEM)pHeap;
    pItem -= hMem;

    if (!T5HEAP_ITEMEXIST(pItem) || T5HEAP_ITEMFREE(pItem))
        return;

    pItem->dwSize |= T5HEAP_FREE;
    pHeap->dwNbFree += 1L;
}

/*****************************************************************************
_T5Heap_Lock_BUFFER
Lock a record in a static heap and get the pointer to user data
Parameters:
     pHeap (IN) pointer to main heap
     hMem (IN) handle of the memory object
Return: pointer to user data - NULL if fail
*****************************************************************************/

static T5_PTR _T5Heap_Lock_BUFFER (T5PTR_HEAP pHeap, T5_DWORD hMem)
{
    T5PTR_HEAPITEM pItem;

    if (hMem < 1 || hMem > pHeap->dwNbID)
        return NULL;

    pItem = (T5PTR_HEAPITEM)pHeap;
    pItem -= hMem;
    if (!T5HEAP_ITEMEXIST(pItem) || T5HEAP_ITEMFREE(pItem)
        || T5HEAP_ITEMLOCKED(pItem))
        return NULL;

    pItem->dwSize |= T5HEAP_LOCKED;
    return pItem->pData;
}

/*****************************************************************************
_T5Heap_Unlock_BUFFER
Unlock a record in a static heap
The record can then be moved in case of static buffers
Parameters:
     pHeap (IN) pointer to main heap
     hMem (IN) handle of the memory object
*****************************************************************************/

static void _T5Heap_Unlock_BUFFER (T5PTR_HEAP pHeap, T5_DWORD hMem)
{
    T5PTR_HEAPITEM pItem;

    if (hMem < 1 || hMem > pHeap->dwNbID)
        return;

    pItem = (T5PTR_HEAPITEM)pHeap;
    pItem -= hMem;
    if (!T5HEAP_ITEMEXIST(pItem) || T5HEAP_ITEMFREE(pItem))
        return;

    pItem->dwSize &= ~T5HEAP_LOCKED;
}

/*****************************************************************************
_T5Heap_GetByPos
Find a handle of the record at thhe specified user memory address
The record can then be moved in case of static buffers
Parameters:
     pHeap (IN) pointer to main heap
     dwPos (IN) byte offset in the user area of the static heap
Return: pointer to the object descriptor or NULL if not found
*****************************************************************************/

static T5PTR_HEAPITEM _T5Heap_GetByPos (T5PTR_HEAP pHeap, T5_DWORD dwPos)
{
    T5PTR_HEAPITEM pItem;
    T5_DWORD i;

    /* look for holes */
    pItem = (T5PTR_HEAPITEM)pHeap;
    pItem--;
    for (i=0; i<pHeap->dwNbID; i++, pItem--)
    {
        if ((T5_DWORD)(pItem->pData - pHeap->pBuffer) == dwPos)
            return pItem;
    }
    return NULL;
}

/*****************************************************************************
T5Heap_Compact
Compact a heap allocated in a static buffer - maximum one movement done
Parameters:
     pHeap (IN) pointer to main heap
Return: TRUE if optimization done
*****************************************************************************/

static T5_BOOL _T5Heap_Compact_BUFFER (T5PTR_HEAP pHeap)
{
    T5PTR_HEAPITEM pItem, pNext;
    T5_DWORD i, dwPosNext;
    T5_BOOL bDone;

    bDone = FALSE;
    if (pHeap->dwNbFree != 0L)
    {
        /* look for holes */
        pItem = (T5PTR_HEAPITEM)pHeap;
        pItem--;
        for (i=0; !bDone && i<pHeap->dwNbID; i++, pItem--)
        {
            /* if it is a hole */
            if (T5HEAP_ITEMFREE(pItem))
            {
                /* get next position in the user data */
                dwPosNext = (T5_DWORD)(pItem->pData - pHeap->pBuffer)
                          + T5HEAP_ITEMSIZE(pItem);
                /* last packet -> delete the hole */
                if (dwPosNext == pHeap->dwGrowBy)
                {
                    pHeap->dwGrowBy -= T5HEAP_ITEMSIZE(pItem);
                    pItem->dwSize = 0L;
                    pItem->pData = NULL;
                    bDone = TRUE;
                    pHeap->dwNbFree -= 1L;
                }
                else
                {
                    /* search for following item in user data */
                    pNext = _T5Heap_GetByPos (pHeap, dwPosNext);
                    /* other hole: merge */
                    if (pNext != NULL && T5HEAP_ITEMFREE(pNext))
                    {
                        pItem->dwSize += T5HEAP_ITEMSIZE(pNext);
                        pNext->dwSize = 0L;
                        pNext->pData = NULL;
                        bDone = TRUE;
                        pHeap->dwNbFree -= 1L;
                    }
                    /* unlock record: move data and swap records */
                    else if (pNext != NULL && !T5HEAP_ITEMLOCKED(pNext))
                    {
                        T5_MEMMOVE (pItem->pData, pNext->pData, pNext->dwSize);
                        pNext->pData = pItem->pData;
                        pItem->pData = pNext->pData + pNext->dwSize;
                        bDone = TRUE;
                    }
                }
            }
        }
    }
    /* remove unused IDs */
    pItem = (T5PTR_HEAPITEM)pHeap;
    pItem -= pHeap->dwNbID;
    while (pHeap->dwNbID > 0 && !T5HEAP_ITEMEXIST(pItem))
    {
        pHeap->dwNbID -= 1L;
        pItem++;
    }
    return bDone;
}

/*****************************************************************************
_T5Heap_GetFreeSpace_BUFFER
Return the number of bytes available for user memory in a static heap
Parameters:
     pHeap (IN) pointer to main heap
Return: available size in bytes
*****************************************************************************/

static T5_DWORD _T5Heap_GetFreeSpace_BUFFER (T5PTR_HEAP pHeap)
{
    T5_DWORD dwUsed;

    dwUsed = pHeap->dwGrowBy
             + ((pHeap->dwNbID+1) * sizeof (T5STR_HEAPITEM));
    if (dwUsed >= pHeap->dwBufSize)
        return 0L;
    return pHeap->dwBufSize - dwUsed;
}

/*****************************************************************************
_T5Heap_FreeAll_BUFFER
returns the number of allocated records (for debug only)
Parameters:
     pHeap (IN) pointer to main heap
Return: number of records currently allocated
*****************************************************************************/

static T5_DWORD _T5Heap_NbAlloc_BUFFER (T5PTR_HEAP pHeap)
{
    T5PTR_HEAPITEM pItem;
    T5_DWORD i, dwCount;

    dwCount = 0;
    pItem = (T5PTR_HEAPITEM)pHeap;
    pItem--;
    for (i=0; i<pHeap->dwNbID; i++, pItem--)
    {
        if (pItem->pData != NULL && !T5HEAP_ITEMFREE(pItem))
            dwCount++;
    }
    return dwCount;
}

/* eof **********************************************************************/
