/*****************************************************************************
T5VMwarm.c : manage warm restart

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/

#ifdef T5DEF_RETAIN

/*****************************************************************************
T5VMWarm_PrepareData
Stores pre-processed pointers and sizes in the VM database
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN/OUT) pointer to the DB
*****************************************************************************/

void T5VMWarm_PrepareData (T5_PTR pCode, T5PTR_DB pDB)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pCodeInit;
    T5PTR_CODERETAIN pDef;
    T5PTR_DBRETAIN pRetain;
    T5_PTBYTE *pString;
    T5_WORD i, wLen;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pCodeInit = T5GET_CODEINIT(pHeader);
    pDef = T5GET_CODERETAIN(pHeader, pCodeInit);
    pRetain = T5GET_DBRETAIN(pDB);

    /* single data types */
    pRetain->pD8 = (T5_PTR)(T5GET_DBDATA8(pDB) + pDef->wIndexD8);
    pRetain->wSizeD8 = pDef->wNbD8;

    pRetain->pD16 = (T5_PTR)(T5GET_DBDATA16(pDB) + pDef->wIndexD16);
    pRetain->wSizeD16 = pDef->wNbD16 * sizeof (T5_WORD);

    pRetain->pD32 = (T5_PTR)(T5GET_DBDATA32(pDB) + pDef->wIndexD32);
    pRetain->wSizeD32 = pDef->wNbD32 * sizeof (T5_DWORD);

#ifdef T5DEF_DATA64SUPPORTED
    pRetain->pD64 = (T5_PTR)(T5GET_DBDATA64(pDB) + pDef->wIndexD64);
    pRetain->wSizeD64 = pDef->wNbD64 * sizeof (T5_DATA64);
#else /*T5DEF_DATA64SUPPORTED*/
    pRetain->pD64 = pRetain->pD32;
    pRetain->wSizeD64 = 0;
#endif /*T5DEF_DATA64SUPPORTED*/

    pRetain->pTime = (T5_PTR)(T5GET_DBTIME(pDB) + pDef->wIndexTime);
    pRetain->wSizeTime = pDef->wNbTime * sizeof (T5_DWORD);

    /* strings */
    pString = T5GET_DBSTRING(pDB);
    pRetain->wSizeString = 0;
    if (pDef->wNbString == 0)
        pRetain->pString = (T5_PTR)pString;
    else
    {
        pRetain->pString = (T5_PTR)(pString[pDef->wIndexString]);
        for (i=0; i<pDef->wNbString; i++)
        {
            wLen = (T5_WORD)(*(pString[i+pDef->wIndexString])) & 0xff;
            pRetain->wSizeString += (wLen + 3);
        }
    }

    /* store config and CRC */
    pRetain->szConf = (T5_PTCHAR)pDef->szConf;
    pRetain->dwCrc = pDef->dwCrc;

#ifdef T5DEF_CTSEG
    /* extra information for CTSeg */
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers >= 11)
    {
        pRetain->pCT = (T5_PTR)((T5_PTBYTE)(pDB[T5TB_CTSEG].pData) + pDef->dwIndexCT);
        pRetain->dwSizeCT = pDef->dwNbCT;
    }
#endif /*T5DEF_CTSEG*/
}

/*****************************************************************************
T5VMWarm_CanUpdatePointers
Check if RETAIN private data can be relinked on Hot restart
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN/OUT) pointer to the DB
Return: OK or error
*****************************************************************************/

T5_RET T5VMWarm_CanUpdatePointers (T5_PTR pCode, T5PTR_DB pDB)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pCodeInit;
    T5PTR_CODERETAIN pDef;
    T5PTR_DBRETAIN pRetain;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pCodeInit = T5GET_CODEINIT(pHeader);
    pDef = T5GET_CODERETAIN(pHeader, pCodeInit);
    pRetain = T5GET_DBRETAIN(pDB);

    if (pDef->dwCrc != pRetain->dwCrc)
        return T5RET_HOTRETAIN;
    
    return T5RET_OK;
}

/*****************************************************************************
T5VMWarm_UpdatePointers
Relink all pointers in RETAIN private data on Hot restart
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN/OUT) pointer to the DB
Return: OK or error
*****************************************************************************/

T5_RET T5VMWarm_UpdatePointers (T5_PTR pCode, T5PTR_DB pDB)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pCodeInit;
    T5PTR_CODERETAIN pDef;
    T5PTR_DBRETAIN pRetain;
    T5_PTBYTE *pString;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pCodeInit = T5GET_CODEINIT(pHeader);
    pDef = T5GET_CODERETAIN(pHeader, pCodeInit);
    pRetain = T5GET_DBRETAIN(pDB);

    /* check CRC */
    if (pDef->dwCrc != pRetain->dwCrc)
        return T5RET_HOTRETAIN;

    /* single data types */
    pRetain->pD8 = (T5_PTR)(T5GET_DBDATA8(pDB) + pDef->wIndexD8);
    pRetain->pD16 = (T5_PTR)(T5GET_DBDATA16(pDB) + pDef->wIndexD16);
    pRetain->pD32 = (T5_PTR)(T5GET_DBDATA32(pDB) + pDef->wIndexD32);
#ifdef T5DEF_DATA64SUPPORTED
    pRetain->pD64 = (T5_PTR)(T5GET_DBDATA64(pDB) + pDef->wIndexD64);
#else /*T5DEF_DATA64SUPPORTED*/
    pRetain->pD64 = pRetain->pD32;
#endif /*T5DEF_DATA64SUPPORTED*/
    pRetain->pTime = (T5_PTR)(T5GET_DBTIME(pDB) + pDef->wIndexTime);
    /* strings */
    pString = T5GET_DBSTRING(pDB);
    if (pDef->wNbString == 0)
        pRetain->pString = (T5_PTR)pString;
    else
        pRetain->pString = (T5_PTR)(pString[pDef->wIndexString]);

    /* config string */
    pRetain->szConf = (T5_PTCHAR)pDef->szConf;

    return T5RET_OK;
}

/*****************************************************************************
T5VMWarm_Load
Load stored value of RETAIN variables
Parameters:
    pDB (IN/OUT) pointer to the DB
    pArgs (IN) configuration string from the main caller
Return: OK or error
*****************************************************************************/

T5_RET T5VMWarm_Load (T5PTR_DB pDB, T5_PTR pArgs)
{
    T5PTR_DBRETAIN pRetain;

    pRetain = T5GET_DBRETAIN(pDB);
    return T5Retain_Load (pRetain, pArgs);
}

/*****************************************************************************
T5VMWarm_CanLoad
Check if RETAIN variables can be loaded
Parameters:
    pCode (IN) pointer to the application code
    pArgs (IN) configuration string from the main caller
Return: TRUE valid data available
*****************************************************************************/

T5_BOOL T5VMWarm_CanLoad (T5_PTR pCode, T5_PTR pArgs)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pCodeInit;
    T5PTR_CODERETAIN pDef;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pCodeInit = T5GET_CODEINIT(pHeader);
    pDef = T5GET_CODERETAIN(pHeader, pCodeInit);
    return T5Retain_CanLoad (pDef->dwCrc, pArgs);
}

/*****************************************************************************
T5VMWarm_CanStore
Check if RETAIN variables can be loaded
Parameters:
    pDB (IN/OUT) pointer to the DB
    pArgs (IN) configuration string from the main caller
Return: TRUE valid data available
*****************************************************************************/

T5_BOOL T5VMWarm_CanStore (T5PTR_DB pDB, T5_PTR pArgs)
{
    T5PTR_DBRETAIN pRetain;

    pRetain = T5GET_DBRETAIN(pDB);
    /* check if storage will be possible */
    if (!T5Retain_CanStore (pRetain, pArgs))
    {
        T5VMLog_Push (pDB, T5RET_RETAINSTORING, 0, 0L);
        return FALSE;
    }
    return TRUE;
}

/*****************************************************************************
T5VMWarm_ShutDown
Called at shutdown for closing the storage of RETAIN variables
Parameters:
    pDB (IN/OUT) pointer to the DB
    pArgs (IN) configuration string from the main caller
*****************************************************************************/

void T5VMWarm_ShutDown (T5PTR_DB pDB, T5_PTR pArgs)
{
    T5PTR_DBRETAIN pRetain;

    pRetain = T5GET_DBRETAIN(pDB);
    T5Retain_ShutDown (pRetain, pArgs);
}

/*****************************************************************************
T5VMWarm_CycleExchange
Called on each cycle for managing RETAIN variables
Parameters:
    pDB (IN/OUT) pointer to the DB
*****************************************************************************/

void T5VMWarm_CycleExchange (T5PTR_DB pDB)
{
    T5PTR_DBRETAIN pRetain;

    pRetain = T5GET_DBRETAIN(pDB);
    T5Retain_CycleExchange (pRetain);
}

/****************************************************************************/

T5_BOOL T5VMWarm_SaveToFile (T5PTR_DB pDB, T5_PTCHAR szFile)
{
#ifdef T5DEF_FILESUPPORTED
    T5PTR_DBRETAIN pRetain;
    T5_LONG f;

    pRetain = T5GET_DBRETAIN(pDB);
    f = T5File_Open (szFile, TRUE);
    if (!f)
        return FALSE;

    T5File_Write (f, &(pRetain->dwCrc), sizeof (T5_DWORD));
    if (pRetain->wSizeD8)
        T5File_Write (f, pRetain->pD8, pRetain->wSizeD8);
    if (pRetain->wSizeD16)
        T5File_Write (f, pRetain->pD16, pRetain->wSizeD16);
    if (pRetain->wSizeD32)
        T5File_Write (f, pRetain->pD32, pRetain->wSizeD32);
    if (pRetain->wSizeD64)
        T5File_Write (f, pRetain->pD64, pRetain->wSizeD64);
    if (pRetain->wSizeTime)
        T5File_Write (f, pRetain->pTime, pRetain->wSizeTime);
    if (pRetain->wSizeString)
        T5File_Write (f, pRetain->pString, pRetain->wSizeString);
    if (pRetain->dwSizeCT)
        T5File_Write (f, pRetain->pCT, pRetain->dwSizeCT);

    T5File_Close (f);
    return TRUE;
#else /*T5DEF_FILESUPPORTED*/
    return FALSE;
#endif /*T5DEF_FILESUPPORTED*/
}

T5_BOOL T5VMWarm_LoadFromFile (T5PTR_DB pDB, T5_PTCHAR szFile)
{
#ifdef T5DEF_FILESUPPORTED
    T5PTR_DBRETAIN pRetain;
    T5_LONG f;
    T5_DWORD dwCrc;

    pRetain = T5GET_DBRETAIN(pDB);
    f = T5File_Open (szFile, FALSE);
    if (!f)
        return FALSE;

    dwCrc = 0;
    if (!T5File_Read (f, &dwCrc, sizeof (T5_DWORD)) || dwCrc != pRetain->dwCrc)
    {
        T5File_Close (f);
        return FALSE;
    }

    if (pRetain->wSizeD8)
        T5File_Read (f, pRetain->pD8, pRetain->wSizeD8);
    if (pRetain->wSizeD16)
        T5File_Read (f, pRetain->pD16, pRetain->wSizeD16);
    if (pRetain->wSizeD32)
        T5File_Read (f, pRetain->pD32, pRetain->wSizeD32);
    if (pRetain->wSizeD64)
        T5File_Read (f, pRetain->pD64, pRetain->wSizeD64);
    if (pRetain->wSizeTime)
        T5File_Read (f, pRetain->pTime, pRetain->wSizeTime);
    if (pRetain->wSizeString)
        T5File_Read (f, pRetain->pString, pRetain->wSizeString);
    if (pRetain->dwSizeCT)
        T5File_Read (f, pRetain->pCT, pRetain->dwSizeCT);

    T5File_Close (f);
    return TRUE;
#else /*T5DEF_FILESUPPORTED*/
    return FALSE;
#endif /*T5DEF_FILESUPPORTED*/
}

T5_BOOL T5VMWarm_CheckFile (T5_DWORD dwCrcExpected, T5_PTCHAR szFile)
{
#ifdef T5DEF_FILESUPPORTED
    T5_LONG f;
    T5_DWORD dwCrc;

    f = T5File_Open (szFile, FALSE);
    if (!f)
        return FALSE;

    dwCrc = 0;
    if (!T5File_Read (f, &dwCrc, sizeof (T5_DWORD)) || dwCrc != dwCrcExpected)
    {
        T5File_Close (f);
        return FALSE;
    }

    T5File_Close (f);
    return TRUE;
#else /*T5DEF_FILESUPPORTED*/
    return FALSE;
#endif /*T5DEF_FILESUPPORTED*/
}

/****************************************************************************/

#endif /*T5DEF_RETAIN*/

/* eof **********************************************************************/
