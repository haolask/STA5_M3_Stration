/*****************************************************************************
T5VMclon.c : clone VM database

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/* static services **********************************************************/

static T5_PTR _T5VMClone_GetData (T5PTR_DB pExternalBase, T5_WORD wTable);

/*****************************************************************************
T5VMClone_GetSize
Get the size of memory to be allocated for the clone
Parameters:
   pExternalBase (IN) pointer to the external database
Return: size in bytes or 0 if error
*****************************************************************************/

T5_DWORD T5VMClone_GetSize (T5PTR_DB pExternalBase)
{
    T5_DWORD dwSize;
    T5PTR_DBPRIVATE pPrivate;

    /* check DB version */
    pPrivate = (T5PTR_DBPRIVATE)_T5VMClone_GetData (pExternalBase,
                                                    T5TB_PRIVATE);
    if (pPrivate->dwVers != K5DBVERS)
        return 0L;

    /* local data headers */
    dwSize = (T5MAX_TABLE * sizeof (T5STR_DBTABLE));

    /* local private info - pointers to code */
    dwSize += sizeof (T5STR_DBPRIVATE);

    /* local string pointers */
#ifdef T5DEF_STRINGSUPPORTED
    dwSize += ((T5_DWORD)(pExternalBase[T5TB_STRING].wNbItemAlloc)
                * (T5_DWORD)(pExternalBase[T5TB_STRING].wSizeOf));
#endif /*T5DEF_STRINGSUPPORTED*/

    /* local variable map */
#ifdef T5DEF_VARMAP
    dwSize += ((T5_DWORD)(pExternalBase[T5TB_VARMAP].wNbItemAlloc)
                * (T5_DWORD)(pExternalBase[T5TB_VARMAP].wSizeOf));
    dwSize += ((T5_DWORD)(pExternalBase[T5TB_HASH].wNbItemAlloc)
                * (T5_DWORD)(pExternalBase[T5TB_HASH].wSizeOf));
#endif /*T5DEF_VARMAP*/

    /* external variables */
#ifdef T5DEF_XV
    dwSize += ((T5_DWORD)(pExternalBase[T5TB_XV].wNbItemAlloc)
                * (T5_DWORD)(pExternalBase[T5TB_XV].wSizeOf));
#endif /*T5DEF_XV*/

    /* MODBUS */
#ifdef T5DEF_MODBUSIOS
    dwSize += pExternalBase[T5TB_MBC].dwRawSize;
#endif /*T5DEF_MODBUSIOS*/

    return dwSize;
}

/*****************************************************************************
T5VMClone_Build
Build the clone
Parameters:
   pExternalBase (IN) pointer to the external database
   pClone (IN) pointer to clone local memory
   pCode (IN) pointer to the application code
Return: ok or error
*****************************************************************************/

T5_RET T5VMClone_Build (T5PTR_DB pExternalBase, T5PTR_DB pClone, T5_PTR pCode)
{
    T5_DWORD dwTab;
    T5_WORD i;
    T5_PTBYTE pData;
    T5PTR_DBPRIVATE pPrivate, pClonePrivate;
    T5_PTBYTE *pCloneStrings;
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pStrBuf;
#endif /*T5DEF_STRINGSUPPORTED*/

    /* check DB version */
    pPrivate = (T5PTR_DBPRIVATE)_T5VMClone_GetData (pExternalBase,
                                                    T5TB_PRIVATE);
    if (pPrivate->dwVers != K5DBVERS)
        return T5RET_ERROR;

    /* copy data base header */
    T5_MEMCPY (pClone, pExternalBase,
               (T5MAX_TABLE * sizeof (T5STR_DBTABLE)));

    /* first update all links to the original base */
    pData = (T5_PTBYTE)pExternalBase;
    pData += (T5MAX_TABLE * sizeof (T5STR_DBTABLE));
	for (i=0; i<T5MAX_TABLE; i++)
	{
		dwTab = pExternalBase[i].dwRawSize;
		if (dwTab == 0L)
            dwTab = (T5_DWORD)(pExternalBase[i].wNbItemAlloc)
                    * (T5_DWORD)(pExternalBase[i].wSizeOf);
		dwTab = T5_ALIGNED(dwTab);
        pClone[i].pData = (T5_PTR)pData;
        pData += dwTab;
	}

    /* close private data - patch pointers to code */
    pClonePrivate = (T5PTR_DBPRIVATE)((T5_PTBYTE)pClone
                                + (T5MAX_TABLE * sizeof (T5STR_DBTABLE)));
    pClone[T5TB_PRIVATE].pData = (T5_PTR)pClonePrivate;
    T5_MEMCPY (pClonePrivate, pPrivate, sizeof (T5STR_DBPRIVATE));
    pClonePrivate->pCode = pCode;
    if (pCode != NULL)
        pClonePrivate->pTypeMasks = T5VMCode_GetVarTypeList (pCode);

    /* local string pointers */
    pCloneStrings = (T5_PTBYTE *)((T5_PTBYTE)pClonePrivate
                                  + sizeof (T5STR_DBPRIVATE));
#ifdef T5DEF_STRINGSUPPORTED
    pStrBuf = (T5_PTBYTE)_T5VMClone_GetData (pExternalBase,
                                             T5TB_STRBUF);
    pClone[T5TB_STRING].pData = (T5_PTR)pCloneStrings;
    for (i=1; i<=pClone[T5TB_STRING].wNbItemUsed; i++)
    {
        pCloneStrings[i] = pStrBuf;
        pStrBuf += (((T5_WORD)(*pStrBuf) & 0xff) + 3);
    }
#else /*T5DEF_STRINGSUPPORTED*/
    T5_MEMSET (&(pClone[T5TB_STRING]), 0, sizeof (T5STR_DBTABLE));
#endif /*T5DEF_STRINGSUPPORTED*/

    /* local variable map */
#ifdef T5DEF_VARMAP
    pData = (T5_PTBYTE)pCloneStrings
            + ((T5_DWORD)(pClone[T5TB_STRING].wNbItemAlloc) * (T5_DWORD)(pClone[T5TB_STRING].wSizeOf));
    pClone[T5TB_VARMAP].pData = pData;
    pData += ((T5_DWORD)(pClone[T5TB_VARMAP].wNbItemAlloc) * (T5_DWORD)(pClone[T5TB_VARMAP].wSizeOf));
    pClone[T5TB_HASH].pData = pData;
    pData += ((T5_DWORD)(pClone[T5TB_HASH].wNbItemAlloc) * (T5_DWORD)(pClone[T5TB_HASH].wSizeOf));
    if (pCode) T5VMMap_Build (pClone, pCode);
#else /*T5DEF_VARMAP*/
    T5_MEMSET (&(pClone[T5TB_VARMAP]), 0, sizeof (T5STR_DBTABLE));
    T5_MEMSET (&(pClone[T5TB_HASH]), 0, sizeof (T5STR_DBTABLE));
#endif /*T5DEF_VARMAP*/

    /* external variables */
#ifdef T5DEF_XV
    pClone[T5TB_XV].pData = pData;
    pData += ((T5_DWORD)(pClone[T5TB_XV].wNbItemAlloc) * (T5_DWORD)(pClone[T5TB_XV].wSizeOf));
    T5VMCode_SetXVPointers (pCode, pClone);
#endif /*T5DEF_XV*/

    /* MODBUS */
#ifdef T5DEF_MODBUSIOS
    pClone[T5TB_MBC].pData = pData;
    pData += pClone[T5TB_MBC].dwRawSize;

    T5MB_Build (T5VMCode_MBClientConfig (pCode),
                pClone[T5TB_MBC].pData, pClone[T5TB_MBC].dwRawSize);
    T5MB_Link (pClone[T5TB_MBC].pData, pClone);
#endif /*T5DEF_MODBUSIOS*/

    /* reclaculate pointers and size of RETAIN spaces */
#ifdef T5DEF_RETAIN
    T5VMWarm_PrepareData (pCode, pClone);
#endif /*T5DEF_RETAIN*/

    return T5RET_OK;
}

/*****************************************************************************
_T5VMClone_GetData
Get the address of the contents of a table in the external database
Parameters:
   pExternalBase (IN) pointer to the external database
   wTable (IN) index of the wished table
Return: pointer to the contents of table (data)
*****************************************************************************/

static T5_PTR _T5VMClone_GetData (T5PTR_DB pExternalBase, T5_WORD wTable)
{
    T5_WORD i;
    T5_DWORD dwSize, dwTab;

    dwSize = (T5MAX_TABLE * sizeof (T5STR_DBTABLE));
    for (i=0; i<wTable; i++)
    {
		dwTab = pExternalBase[i].dwRawSize;
		if (dwTab == 0L)
            dwTab = (T5_DWORD)(pExternalBase[i].wNbItemAlloc)
                     * (T5_DWORD)(pExternalBase[i].wSizeOf);
		dwTab = T5_ALIGNED(dwTab);
        dwSize += dwTab;
    }
    return (T5_PTR)((T5_PTBYTE)pExternalBase + dwSize);
}

/* eof **********************************************************************/
