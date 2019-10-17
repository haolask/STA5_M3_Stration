/*****************************************************************************
T5heap.h :   safe heap - definitions

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#ifndef _T5HEAP_H_INCLUDED_
#define _T5HEAP_H_INCLUDED_

/****************************************************************************/
/* Heap header */

#define T5HEAP_MALLOC   0x80000000L     /* flag: malloc() based heap */

typedef struct              /* heap main header */
{
    T5_PTBYTE pBuffer;      /* pointer to user memory if static buffer */
                            /* or pointer to this structure if malloc */
    T5_DWORD  dwGrowBy;     /* size of one packet of handles if malloc */
                            /* or used memory if static buffer */
    T5_DWORD  dwBufSize;    /* full size of user memory for static buffer */
    T5_DWORD  dwFlags;      /* creation flags */
    T5_DWORD  dwNbID;       /* number of handles if static buffer */
    T5_DWORD  dwNbFree;     /* nb freed handled if static buffer */
}
T5STR_HEAP;

typedef T5STR_HEAP *T5PTR_HEAP;

/* helpers */

#define T5_HEAP_ISMALLOC(p) \
    ((((T5PTR_HEAP)(p))->dwFlags & T5HEAP_MALLOC) != 0L)

/****************************************************************************/
/* buffer heap item - for static buffers only */

#define T5HEAP_LOCKED   0x80000000L     /* item is locked by the user */
#define T5HEAP_FREE     0x40000000L     /* item is freed */
#define T5HEAP_MASKS    0xc0000000L     /* item masks (rest is length) */

typedef struct                          /* heap item */
{
    T5_DWORD  dwSize;                   /* size of user data */
    T5_PTBYTE pData;                    /* pointer to user data */
}
T5STR_HEAPITEM;

typedef T5STR_HEAPITEM *T5PTR_HEAPITEM;

/* helpers */

#define T5HEAP_ITEMLOCKED(p) ((((p)->dwSize)&T5HEAP_LOCKED)!=0)
#define T5HEAP_ITEMFREE(p)   ((((p)->dwSize)&T5HEAP_FREE)!=0)
#define T5HEAP_ITEMEXIST(p)  (((p)->dwSize)!=0)
#define T5HEAP_ITEMSIZE(p)   (((p)->dwSize)&~(T5HEAP_MASKS))

/****************************************************************************/
/* APIs */

extern T5_PTR   T5Heap_CreateHeapBuffer (T5_PTR pBuffer, T5_DWORD dwSize,
                                         T5_DWORD dwFlags);
extern T5_PTR   T5Heap_RelocateHeapBuffer (T5_PTR pBuffer, T5_DWORD dwSize,
                                           T5_DWORD dwFlags);
extern T5_PTR   T5Heap_CreateHeapMalloc (T5_DWORD dwGrowBy, T5_DWORD dwFlags);
extern void     T5Heap_DeleteHeap (T5_PTR pHeap);
extern void     T5Heap_FreeAll (T5_PTR pHeap);
extern T5_DWORD T5Heap_Compact (T5_PTR pHeap, T5_DWORD dwMaxLoop);
extern T5_DWORD T5Heap_Alloc (T5_PTR pHeap, T5_DWORD dwSize);
extern void     T5Heap_Free (T5_PTR pHeap, T5_DWORD hMem);
extern T5_PTR   T5Heap_Lock (T5_PTR pHeap, T5_DWORD hMem);
extern void     T5Heap_Unlock (T5_PTR pHeap, T5_DWORD hMem);
extern T5_DWORD T5Heap_GetFreeSpace (T5_PTR pHeap); /* static buffers only */
extern T5_DWORD T5Heap_NbAlloc (T5_PTR pHeap);

/****************************************************************************/

#endif /*_T5HEAP_H_INCLUDED_*/

/* eof **********************************************************************/
