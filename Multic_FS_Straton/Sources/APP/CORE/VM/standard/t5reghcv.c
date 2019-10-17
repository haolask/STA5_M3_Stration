/*****************************************************************************
T5RegHcv.c : T5 Registry - host converter

DO NOT ALTER THIS !

(c) COPALP 2008
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/

#ifdef T5DEF_REG

/****************************************************************************/

static void _T5RegHcv_Convert (T5PTR_T5REGHEAD pHead);
static void _T5RegHcv_SwapDW (T5_PTDWORD pdw);

/*****************************************************************************
T5RegHcv_ToHost
Convert a registry in local format
Parameters:
    pReg (IN/OUT) pointer to registry block
Return: TRUE if converted
*****************************************************************************/

T5_BOOL T5RegHcv_ToHost (T5_PTR pReg)
{
    T5PTR_T5REGHEAD pHead;

    pHead = (T5PTR_T5REGHEAD) pReg;
    if (T5_MEMCMP (pHead->szID, T5REG_HEADER, sizeof (pHead->szID)) == 0)
        return FALSE;
    _T5RegHcv_Convert (pHead);
    return TRUE;
}

/*****************************************************************************
T5RegHcv_ToRTI
Convert a registry in little endian format
Parameters:
    pReg (IN/OUT) pointer to registry block
Return: TRUE if converted
*****************************************************************************/

T5_BOOL T5RegHcv_ToRTI (T5_PTR pReg)
{
    T5PTR_T5REGHEAD pHead;

    pHead = (T5PTR_T5REGHEAD) pReg;
    if (T5_MEMCMP (pHead->szID, T5REG_HEADER_LE, sizeof (pHead->szID)) == 0)
        return FALSE;
    _T5RegHcv_Convert (pHead);
    return TRUE;
}

/*****************************************************************************
T5RegHcv_ToRTM
Convert a registry in big endian format
Parameters:
    pReg (IN/OUT) pointer to registry block
Return: TRUE if converted
*****************************************************************************/

T5_BOOL T5RegHcv_ToRTM (T5_PTR pReg)
{
    T5PTR_T5REGHEAD pHead;

    pHead = (T5PTR_T5REGHEAD) pReg;
    if (T5_MEMCMP (pHead->szID, T5REG_HEADER_LE, sizeof (pHead->szID)) == 0)
        return FALSE;
    _T5RegHcv_Convert (pHead);
    return TRUE;
}

/*****************************************************************************
_T5RegHcv_Convert
Convert a registry (change endian)
Parameters:
    pReg (IN/OUT) pointer to registry block
*****************************************************************************/

static void _T5RegHcv_Convert (T5PTR_T5REGHEAD pHead)
{
    T5_PTDWORD pHash;
    T5_DWORD i;
    T5PTR_T5REGITEM pItem;

    /* header */
    _T5RegHcv_SwapDW (&(pHead->dwFullSize));
    _T5RegHcv_SwapDW (&(pHead->dwBValSize));
    _T5RegHcv_SwapDW (&(pHead->pBVal));
    _T5RegHcv_SwapDW (&(pHead->dwNbRec));
    _T5RegHcv_SwapDW (&(pHead->dwDate));
    _T5RegHcv_SwapDW (&(pHead->dwTime));
    _T5RegHcv_SwapDW (&(pHead->dwVersion));
    _T5RegHcv_SwapDW (&(pHead->pXML));
    _T5RegHcv_SwapDW (&(pHead->dwFlags));
    _T5RegHcv_SwapDW (&(pHead->res));

    /* hash code entries */
    pHash = T5REG_GETHASH(pHead);
    for (i=0; i<T5REG_HSIZE; i++)
        _T5RegHcv_SwapDW (&(pHash[i]));

    /* records */
    pItem = T5REG_GETITEMS(pHead);
    for (i=0; i<pHead->dwNbRec; i++, pItem++)
    {
        _T5RegHcv_SwapDW (&(pItem->dwID));
        _T5RegHcv_SwapDW (&(pItem->dwParent));
        _T5RegHcv_SwapDW (&(pItem->dwNext));
        _T5RegHcv_SwapDW (&(pItem->dwFlags));
        _T5RegHcv_SwapDW (&(pItem->pPassword));
        _T5RegHcv_SwapDW (&(pItem->pName));
        _T5RegHcv_SwapDW (&(pItem->pValue));
    }
}

/*****************************************************************************
_T5RegHcv_SwapDW
Swap the bytes of a DWORD
Parameters:
    pdw (IN/OUT) pointer to DWORD value
*****************************************************************************/

static void _T5RegHcv_SwapDW (T5_PTDWORD pdw)
{
    T5_PTBYTE pb;
    T5_BYTE b;

    pb = (T5_PTBYTE)pdw;
    b = pb[0];
    pb[0] = pb[3];
    pb[3] = b;
    b = pb[1];
    pb[1] = pb[2];
    pb[2] = b;
}

/****************************************************************************/

#endif /*T5DEF_REG*/

/* eof **********************************************************************/
