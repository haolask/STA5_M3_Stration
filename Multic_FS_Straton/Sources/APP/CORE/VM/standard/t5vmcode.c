/*****************************************************************************
T5VMcode.c : get information from the application code

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/*****************************************************************************
T5VMCode_CheckCode
Check if the application code is correct:
- main flag for intel/motorola endian
- name and version of the target
Parameters:
    pCode (IN) pointer to the application code
Return: ok or error
*****************************************************************************/

#ifndef T5APP_HEADER
#define T5APP_HEADER 1L
#endif /*T5APP_HEADER*/

T5_RET T5VMCode_CheckCode (T5_PTR pCode)
{
    T5_RET wRet;
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEPRODUCT pProduct;

    pHeader = (T5PTR_CODEHEADER)pCode;
    if ((pHeader->dwOne & 0x0fL) != (T5APP_HEADER & 0x0fL))
        wRet = T5RET_BADCODE;
    else
    {
        pProduct = T5GET_CODEPRODUCT(pHeader);
        if (T5_STRCMP (pProduct->szTarget, T5DEF_TARGET) != 0)
            wRet = T5RET_BADCODETARGET;
        /* version: don't check the smallest digit */
        else if (T5_MEMCMP (pProduct->szVersion, T5DEF_TARGETVERS, 3) != 0)
            wRet = T5RET_BADCODEVERSION;
        else
            wRet = T5RET_OK;
    }
    return wRet;
}

/*****************************************************************************
T5VMCode_GetPassword
Get the password for request server
Parameters:
    pCode (IN) pointer to the application code
Return: password or 0 if none
*****************************************************************************/

T5_DWORD T5VMCode_GetPassword (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 1)
        return 0;
    return pExtra->dwPassword;
}

/*****************************************************************************
T5VMCode_InitiateStatus
Initiate the application status according to the app configuration
Parameters:
    pCode (IN) pointer to the application code
    pStatus (OUT) pointer to the application status
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_InitiateStatus (T5_PTR pCode, T5PTR_DBSTATUS pStatus)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEAPP pApp;
    T5_PTBYTE pCrc;
    T5PTR_CODEINIT pCodeInit;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    pApp = T5GET_CODEAPP(pHeader);
    /* default all members to 0 */
    T5_MEMSET (pStatus, 0, sizeof (T5STR_DBSTATUS));
    /* get timing and version info from the app code */
    T5_STRCPY (pStatus->szAppName, pApp->szName);
    pStatus->dwTimTrigger = pApp->dwTrigger;
    pStatus->dwAppVersion = (T5_DWORD)(pApp->wVersion) & 0xffff;
    pStatus->dwAppDateStamp = pApp->dwDateStamp;
    pStatus->dwAppDataCRC = pApp->dwCrcData;
    pStatus->dwCmpVersion = pHeader->dwCmpVersion;
    /* status: app loaded - maybe running immediately */
    pStatus->wFlags = T5FLAG_APPHERE | T5FLAG_SYSINFO;
    pStatus->dwFlagsEx = 0L;
    if (!T5VMCode_IsCCPure (pCode))
        pStatus->dwFlagsEx |= T5FLAGEX_PCAVL;
    pCodeInit = T5GET_CODEINIT(pHeader);
    if ((pCodeInit->dwUserOptions & T5_UOPT_NOIOSTEP) != 0)
        pStatus->dwFlagsEx |= T5FLAGEX_NOSIO;
    if ((pCodeInit->dwUserOptions & T5_UOPT_RTRIGNO0) != 0)
        pStatus->dwFlagsEx |= T5FLAGEX_RTRIGNO0;
    if ((pCodeInit->dwUserOptions & T5_UOPT_USHR) != 0)
        pStatus->dwFlagsEx |= T5FLAGEX_USHR;

#ifndef T5DEF_ALWAYSSTARTPAUSED
    if (pApp->wStartMode == 0)
        pStatus->wFlags |= T5FLAG_RUN;
#endif /*T5DEF_ALWAYSSTARTPAUSED*/

#ifdef T5DEF_CTSEG
    if (T5VMCode_GetCTSegAllocSize (pCode))
        pStatus->wFlags |= T5FLAG_HASCT;
    /* smart lock used with CTSeg */
    if (pExtra->dwVers >= 19 && (pExtra->dwSMFlags & 0x00000001L) != 0)
        pStatus->dwFlagsEx |= T5FLAGEX_CTSM;
#endif /*T5DEF_CTSEG*/
    /* other SM flags */
    if (pExtra->dwVers >= 20 && (pExtra->dwSMFlags & 0x00000002L) != 0)
        pStatus->dwFlagsEx |= T5FLAGEX_DEBUG;

    /* get CRC at the end of the code */
    pCrc = ((T5_PTBYTE)pCode) + pApp->dwCodeSize - sizeof (T5_DWORD);
#ifdef T5DEF_USESTABLECRC
    pCrc -= sizeof (T5_DWORD);
#endif /*T5DEF_USESTABLECRC*/
    pStatus->dwAppCodeCRC = *(T5_DWORD *)pCrc;

    return T5RET_OK; /* TODO: check code CRC */
}

/*****************************************************************************
T5VMCode_UpdateStatus
Update the application status during hot restat
Parameters:
    pCode (IN) pointer to the application code
    pStatus (OUT) pointer to the application status
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_UpdateStatus (T5_PTR pCode, T5PTR_DBSTATUS pStatus)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEAPP pApp;
    T5_PTBYTE pCrc;

    /* check the application idenitification */
    if (!T5VMCode_CanUpdateStatus (pCode, pStatus))
        return T5RET_ERROR;
    /* go */
    pHeader = (T5PTR_CODEHEADER)pCode;
    pApp = T5GET_CODEAPP(pHeader);
    /* get timing and version info from the app code */
    pStatus->dwTimTrigger = pApp->dwTrigger;
    pStatus->dwAppVersion = (T5_DWORD)(pApp->wVersion) & 0xffff;
    pStatus->dwAppDateStamp = pApp->dwDateStamp;
    pStatus->dwAppDataCRC = pApp->dwCrcData;
    /* get CRC at the end of the code */
    pCrc = ((T5_PTBYTE)pCode) + pApp->dwCodeSize - sizeof (T5_DWORD);
#ifdef T5DEF_USESTABLECRC
    pCrc -= sizeof (T5_DWORD);
#endif /*T5DEF_USESTABLECRC*/
    pStatus->dwAppCodeCRC = *(T5_DWORD *)pCrc;
   
    return T5RET_OK; /* TODO: check code CRC */
}

/*****************************************************************************
T5VMCode_CanUpdateStatus
Check if the the application status is OK for hot restat
Parameters:
    pCode (IN) pointer to the application code
    pStatus (OUT) pointer to the application status
Return: TRUE if ok
*****************************************************************************/

T5_BOOL T5VMCode_CanUpdateStatus (T5_PTR pCode, T5PTR_DBSTATUS pStatus)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEAPP pApp;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pApp = T5GET_CODEAPP(pHeader);
    return (T5_STRCMP (pStatus->szAppName, pApp->szName) == 0);
}

/*****************************************************************************
T5VMCode_GetFullBuildStamp
Get the stamp mark of last full build stored in the application code
Parameters:
    pCode (IN) pointer to the application code
Return: stamp
*****************************************************************************/

T5_DWORD T5VMCode_GetFullBuildStamp (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEAPP pApp;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pApp = T5GET_CODEAPP(pHeader);
    return pApp->dwFullBuild;
}

/*****************************************************************************
T5VMCode_GetSfcCrc
Get the CRC of SFC steps and transitions
Parameters:
    pCode (IN) pointer to the application code
Return: stamp
*****************************************************************************/

T5_DWORD T5VMCode_GetSfcCrc (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEAPP pApp;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pApp = T5GET_CODEAPP(pHeader);
    return pApp->dwSfcCrc;
}

/*****************************************************************************
T5VMCode_GetNbProgAlloc
Get the total number of programs to be allocated (for memory sizing)
Parameters:
    pCode (IN) pointer to the application code
Return: number of programs (all sections)
*****************************************************************************/

T5_WORD T5VMCode_GetNbProgAlloc (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;
    T5_WORD wUsed;

    wUsed = T5VMCode_GetNbProg (pCode);

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers >= 17 && pExtra->dwAllocPOU > (T5_DWORD)wUsed)
        return (T5_WORD)(pExtra->dwAllocPOU);
    return wUsed;
}

/*****************************************************************************
T5VMCode_GetNbProg
Get the total number of programs used
Parameters:
    pCode (IN) pointer to the application code
Return: number of programs (all sections)
*****************************************************************************/

T5_WORD T5VMCode_GetNbProg (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEDEFPROG pDefProg;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pDefProg = T5GET_CODEDEFPROG(pHeader);
    return (pDefProg->wNbProg + 1); /* index is 1 based! */
}

/*****************************************************************************
T5VMCode_GetNbProgWithStack
Get the total number of programs requiring a call stack
Parameters:
    pCode (IN) pointer to the application code
Return: number of programs (all sections)
*****************************************************************************/

T5_WORD T5VMCode_GetNbProgWithStack (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEPRG pPrg;
    T5_WORD wNb;

    wNb = 0;
    /* get pointer to TIC code sequences in app code */
    pHeader = (T5PTR_CODEHEADER)pCode;
    pPrg = T5GET_CODEPRG(pHeader);
    /* scan */
    while (pPrg->dwCode != 0)
    {
        if (pPrg->wStyle == T5PRGSTYLE_DEFAULT)
            wNb += 1;
        pPrg++;
    }
    return wNb;
}

/*****************************************************************************
T5VMCode_GetCallStackSize
Get the call stack size (depth - used)
Parameters:
    pCode (IN) pointer to the application code
Return: allocation size for the call stack
*****************************************************************************/

T5_WORD T5VMCode_GetCallStackSize (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEDEFPROG pDefProg;
    T5_WORD wSize;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pDefProg = T5GET_CODEDEFPROG(pHeader);
    wSize = pDefProg->wStackSize;
    if (wSize == 0) /* old versions or the WB */
        wSize = 16;
    return wSize;
}

/*****************************************************************************
T5VMCode_GetCallStackAllocSize
Get the call stack size (depth - nb item to be allocated)
Parameters:
    pCode (IN) pointer to the application code
Return: allocation size for the call stack
*****************************************************************************/

T5_WORD T5VMCode_GetCallStackAllocSize (T5_PTR pCode)
{
    T5_WORD wSize;

    wSize = T5VMCode_GetCallStackSize (pCode);
    wSize *= (T5VMCode_GetNbProgWithStack (pCode) + 1);
    return wSize;
}

/*****************************************************************************
T5VMCode_SetProgramPointers
Initiate all pointers to program code plus program size
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN/OUT) full VM database
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_SetProgramPointers (T5_PTR pCode, T5PTR_DB pDB)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_DBPROG pPrgCode;
    T5PTR_CODEPRG pPrg;
    T5PTR_DB pDef;
    T5_WORD pNextStack;
    T5PTR_DBSTATUS pStatus;

    pDef = &(pDB[T5TB_PROG]);
    pStatus = T5GET_DBSTATUS(pDB);
    pNextStack = pStatus->wMaxCallStack;
    pHeader = (T5PTR_CODEHEADER)pCode;
    /* get pointer to TIC code sequences in app code */
    pPrg = T5GET_CODEPRG(pHeader);
    /* get list of code pointers in data base */
    pPrgCode = (T5PTR_DBPROG)(pDef->pData);
    /* copy */
    pDef->wNbItemUsed = 0;
    while (pDef->wNbItemUsed < (pDef->wNbItemAlloc-1) && pPrg->dwCode != 0)
    {
        (pDef->wNbItemUsed)++; /* index is 1 based! */
        pPrgCode[pDef->wNbItemUsed].pCode = T5_OFF(pCode, pPrg->dwCode);
        pPrgCode[pDef->wNbItemUsed].dwSize = pPrg->dwSize;
        pPrgCode[pDef->wNbItemUsed].wStyle = pPrg->wStyle;
        pPrgCode[pDef->wNbItemUsed].wStackDepth = 0;

        if (pPrg->wStyle == T5PRGSTYLE_DEFAULT
            && pNextStack <= (pDB[T5TB_CALLSTACK].wNbItemAlloc
                              - pStatus->wMaxCallStack))
        {
            pPrgCode[pDef->wNbItemUsed].wStackSave = pNextStack;
            pNextStack += pStatus->wMaxCallStack;
        }
        else
            pPrgCode[pDef->wNbItemUsed].wStackSave = 0;
        pPrg++;
    }
    return (pPrg->dwCode == 0) ? T5RET_OK : T5RET_ERROR;
}

/*****************************************************************************
T5VMCode_GetNbLockFlags
Get the total number of lock flags to be allocated (max var index)
Parameters:
    pCode (IN) pointer to the application code
Return: number of lock flags
*****************************************************************************/

T5_WORD T5VMCode_GetNbLockFlags (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_WORD wMax, wNb;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    wMax = pVarCount->wNbD8Alloc;
    wNb = pVarCount->wNbD16Alloc;
    if (wNb > wMax) wMax = wNb;
    wNb = pVarCount->wNbD32Alloc;
    if (wNb > wMax) wMax = wNb;
    wNb = pVarCount->wNbD64Alloc;
    if (wNb > wMax) wMax = wNb;
    wNb = pVarCount->wNbTimeAlloc;
    if (wNb > wMax) wMax = wNb;
    wNb = pVarCount->wNbStringAlloc;
    if (wNb > wMax) wMax = wNb;
    return (wMax + 1); /* index is 1 based! */
}

/*****************************************************************************
T5VMCode_GetNbDada8
Get the total number of DATA8 to be allocated (for memory sizing)
Parameters:
    pCode (IN) pointer to the application code
Return: number of 8 bit variables
*****************************************************************************/

T5_WORD T5VMCode_GetNbData8 (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    return (pVarCount->wNbD8Alloc + 1); /* index is 1 based! */
}

/*****************************************************************************
T5VMCode_GetDada8Sizing
Get fixed and used number of DATA8 to be allocated
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the DATA8 table header
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_GetData8Sizing (T5_PTR pCode, T5PTR_DB pDef)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_RET wRet;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    if (pVarCount->wNbD8Used < pDef->wNbItemAlloc)
    {
        pDef->wNbItemFixed = pVarCount->wNbD8Fixed + 1; /* index is 1 based!*/
        pDef->wNbItemUsed = pVarCount->wNbD8Used + 1; /* index is 1 based! */
        wRet = T5RET_OK;
    }
    else
        wRet = T5RET_DATA8OVERFLOW;
    return wRet;
}

/*****************************************************************************
T5VMCode_ApplyData8Init
Apply init values to data8 variables and constants
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the DATA8 table header
    bFixedAlso (IN) if TRUE, apply also init values for fixed variables
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_ApplyData8Init (T5_PTR pCode, T5PTR_DB pDef,
                                T5_BOOL bFixedAlso)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pCodeInit;
    T5_PTWORD pCodeInit8;
    T5_PTBYTE pData;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pCodeInit = T5GET_CODEINIT(pHeader);
    pCodeInit8 = T5GET_CODEINIT8(pHeader, pCodeInit);
    pData = (T5_PTBYTE)(pDef->pData);
    while (*pCodeInit8 != 0)
    {
        if (*pCodeInit8 < pDef->wNbItemUsed)
        {
            if (bFixedAlso || (*pCodeInit8 >= pDef->wNbItemFixed))
                pData[*pCodeInit8] = *(T5_PTBYTE)(pCodeInit8+1);
        }
        pCodeInit8 += 2;
    }
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_GetNbDada16
Get the total number of DATA16 to be allocated (for memory sizing)
Parameters:
    pCode (IN) pointer to the application code
Return: number of 16 bit variables
*****************************************************************************/

T5_WORD T5VMCode_GetNbData16 (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    return (pVarCount->wNbD16Alloc + 1); /* index is 1 based! */
}

/*****************************************************************************
T5VMCode_GetDada16Sizing
Get fixed and used number of DATA16 to be allocated
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the DATA16 table header
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_GetData16Sizing (T5_PTR pCode, T5PTR_DB pDef)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_RET wRet;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    if (pVarCount->wNbD16Used < pDef->wNbItemAlloc)
    {
        pDef->wNbItemFixed = pVarCount->wNbD16Fixed + 1; /* index is 1 based!*/
        pDef->wNbItemUsed = pVarCount->wNbD16Used + 1; /* index is 1 based! */
        wRet = T5RET_OK;
    }
    else
        wRet = T5RET_DATA16OVERFLOW;
    return wRet;
}

/*****************************************************************************
T5VMCode_ApplyData16Init
Apply init values to data16 variables and constants
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the DATA16 table header
    bFixedAlso (IN) if TRUE, apply also init values for fixed variables
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_ApplyData16Init (T5_PTR pCode, T5PTR_DB pDef, 
                                 T5_BOOL bFixedAlso)
{
#ifdef T5DEF_DATA16SUPPORTED
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pCodeInit;
    T5_PTWORD pCodeInit16;
    T5_PTWORD pData;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pCodeInit = T5GET_CODEINIT(pHeader);
    pCodeInit16 = T5GET_CODEINIT16(pHeader, pCodeInit);
    pData = (T5_PTWORD)(pDef->pData);
    while (*pCodeInit16 != 0)
    {
        if (*pCodeInit16 < pDef->wNbItemUsed)
        {
            if (bFixedAlso || (*pCodeInit16 >= pDef->wNbItemFixed))
                pData[*pCodeInit16] = pCodeInit16[1];
        }
        pCodeInit16 += 2;
    }
#endif /*T5DEF_DATA16SUPPORTED*/
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_GetNbDada32
Get the total number of DATA32 to be allocated (for memory sizing)
Parameters:
    pCode (IN) pointer to the application code
Return: number of 32 bit variables
*****************************************************************************/

T5_WORD T5VMCode_GetNbData32 (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    return (pVarCount->wNbD32Alloc + 1); /* index is 1 based! */
}

/*****************************************************************************
T5VMCode_GetDada32Sizing
Get fixed and used number of DATA32 to be allocated
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the DATA32 table header
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_GetData32Sizing (T5_PTR pCode, T5PTR_DB pDef)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_RET wRet;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    if (pVarCount->wNbD32Used < pDef->wNbItemAlloc)
    {
        pDef->wNbItemFixed = pVarCount->wNbD32Fixed + 1; /* index is 1 based!*/
        pDef->wNbItemUsed = pVarCount->wNbD32Used + 1; /* index is 1 based! */
        wRet = T5RET_OK;
    }
    else
        wRet = T5RET_DATA32OVERFLOW;
    return wRet;
}

/*****************************************************************************
T5VMCode_ApplyData32Init
Apply init values to data32 variables and constants
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the DATA32 table header
    bFixedAlso (IN) if TRUE, apply also init values for fixed variables
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_ApplyData32Init (T5_PTR pCode, T5PTR_DB pDef, 
                                 T5_BOOL bFixedAlso)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pCodeInit;
    T5_PTWORD pCodeInit32;
    T5_PTDWORD pData;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pCodeInit = T5GET_CODEINIT(pHeader);
    pCodeInit32 = T5GET_CODEINIT32(pHeader, pCodeInit);
    pData = (T5_PTDWORD)(pDef->pData);
    while (*pCodeInit32 != 0)
    {
        if (*pCodeInit32 < pDef->wNbItemUsed)
        {
            if (bFixedAlso || (*pCodeInit32 >= pDef->wNbItemFixed))
                pData[*pCodeInit32] = *(T5_PTDWORD)(pCodeInit32+2);
        }
        pCodeInit32 += 4;
    }
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_GetNbActiveTime
Get the total possible number of active timers
Parameters:
    pCode (IN) pointer to the application code
Return: max number of active timers
*****************************************************************************/

T5_WORD T5VMCode_GetNbActiveTime (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    return (pVarCount->wNbTimeActive + 1); /* +1 for list terminator */
}

/*****************************************************************************
T5VMCode_GetNbTime
Get the total number of TIME to be allocated (for memory sizing)
Parameters:
    pCode (IN) pointer to the application code
Return: number of TIME variables
*****************************************************************************/

T5_WORD T5VMCode_GetNbTime (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    return (pVarCount->wNbTimeAlloc + 1); /* index is 1 based! */
}

/*****************************************************************************
T5VMCode_GetTimeSizing
Get fixed and used number of TIME to be allocated
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the TIME table header
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_GetTimeSizing (T5_PTR pCode, T5PTR_DB pDef)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_RET wRet;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    if (pVarCount->wNbTimeUsed < pVarCount->wNbTimeAlloc)
    {
        pDef->wNbItemFixed = pVarCount->wNbTimeFixed + 1; /* index 1 based!*/
        pDef->wNbItemUsed = pVarCount->wNbTimeUsed + 1; /* index 1 based! */
        wRet = T5RET_OK;
    }
    else
        wRet = T5RET_TIMEOVERFLOW;
    return wRet;
}

/*****************************************************************************
T5VMCode_ApplyTimeInit
Apply init values to TIME variables and constants
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the TIME table header
    bFixedAlso (IN) if TRUE, apply also init values for fixed variables
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_ApplyTimeInit (T5_PTR pCode, T5PTR_DB pDef, 
                               T5_BOOL bFixedAlso)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pCodeInit;
    T5_PTWORD pCodeInitTime;
    T5_PTDWORD pData;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pCodeInit = T5GET_CODEINIT(pHeader);
    pCodeInitTime = T5GET_CODEINITTIME(pHeader, pCodeInit);
    pData = (T5_PTDWORD)(pDef->pData);
    while (*pCodeInitTime != 0)
    {
        if (*pCodeInitTime < pDef->wNbItemUsed)
        {
            if (bFixedAlso || (*pCodeInitTime >= pDef->wNbItemFixed))
                pData[*pCodeInitTime] = *(T5_PTDWORD)(pCodeInitTime+2);
        }
        pCodeInitTime += 4;
    }
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_GetNbFBInstance
Get the total number of FB instances to be allocated
Parameters:
    pCode (IN) pointer to the application code
Return: number of FB instances to be allocated
*****************************************************************************/

T5_WORD T5VMCode_GetNbFBInstance (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEFBI pDefFBI;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pDefFBI = T5GET_CODEFBI(pHeader);
    return (pDefFBI->wNbAlloc + 1); /* index is 1 based */
}

/*****************************************************************************
T5VMCode_GetInstanceList
Get list of application options
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to the list of instances (NULL terminated)
*****************************************************************************/

T5_PTR T5VMCode_GetInstanceList (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEFBI pDefFBI;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pDefFBI = T5GET_CODEFBI(pHeader);
    return ((T5_PTR)&(pDefFBI->cfFirstInstance));
}

/*****************************************************************************
T5VMCode_GetClassList
Get list of FB classes
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to list of classes
*****************************************************************************/

T5_PTR T5VMCode_GetClassList (T5_PTR pCode, T5_PTWORD pwNbClass)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEFBC pDef;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pDef = T5GET_CODEFBC(pHeader);
    *pwNbClass = pDef->wNbUsed;
    return ((T5_PTR)&(pDef->ccFirstClass));
}

/*****************************************************************************
T5VMCode_GetWishedFBIRawSize
Get list of instance size for FB classes
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to list of classes
*****************************************************************************/

T5_DWORD T5VMCode_GetWishedFBIRawSize (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEFBI pDefFBI;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pDefFBI = T5GET_CODEFBI(pHeader);
    return pDefFBI->dwRawSize;
}

/*****************************************************************************
T5VMCode_GetNbDta64
Get the total number of DATA64 to be allocated (for memory sizing)
Parameters:
    pCode (IN) pointer to the application code
Return: number of 64 bit variables
*****************************************************************************/

T5_WORD T5VMCode_GetNbData64 (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    return (pVarCount->wNbD64Alloc + 1); /* index is 1 based! */
}

/*****************************************************************************
T5VMCode_GetDada64Sizing
Get fixed and used number of DATA64 to be allocated
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the DATA64 table header
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_GetData64Sizing (T5_PTR pCode, T5PTR_DB pDef)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_RET wRet;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    if (pVarCount->wNbD64Used < pDef->wNbItemAlloc)
    {
        pDef->wNbItemFixed = pVarCount->wNbD64Fixed + 1; /* index is 1 based!*/
        pDef->wNbItemUsed = pVarCount->wNbD64Used + 1; /* index is 1 based! */
        wRet = T5RET_OK;
    }
    else
        wRet = T5RET_DATA64OVERFLOW;
    return wRet;
}

/*****************************************************************************
T5VMCode_ApplyData64Init
Apply init values to data64 variables and constants
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the DATA64 table header
    bFixedAlso (IN) if TRUE, apply also init values for fixed variables
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_ApplyData64Init (T5_PTR pCode, T5PTR_DB pDef,
                                T5_BOOL bFixedAlso)
{
#ifdef T5DEF_DATA64SUPPORTED
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pCodeInit;
    T5_PTWORD pCodeInit64;
    T5_PTDATA64 pData;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pCodeInit = T5GET_CODEINIT(pHeader);
    pCodeInit64 = T5GET_CODEINIT64(pHeader, pCodeInit);
    pData = (T5_PTDATA64)(pDef->pData);
    while (*pCodeInit64 != 0)
    {
        if (*pCodeInit64 < pDef->wNbItemUsed)
        {
            if (bFixedAlso || (*pCodeInit64 >= pDef->wNbItemFixed))
                T5_MEMCPY (pData + *pCodeInit64, pCodeInit64 + 2, sizeof (T5_DATA64));
        }
        pCodeInit64 += 6;
    }
#endif /*T5DEF_DATA64SUPPORTED*/
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_GetNbCustomFunc
Get total nb of custom function classes to be allocated
Parameters:
    pCode (IN) pointer to the application code
Return: nb of items to be allocated
*****************************************************************************/

T5_WORD T5VMCode_GetNbCustomFunc (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEFNC pFnc;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pFnc = T5GET_CODEFNC(pHeader);
    return (pFnc->wNbAlloc + 1); /* index is 1 based! */
}

/*****************************************************************************
T5VMCode_GetCustomFuncList
Get list of custom function classes
Parameters:
    pCode (IN) pointer to the application code
    pdwNbClass (OUT) number of classes
Return: pointer to the list of custom functions
*****************************************************************************/

T5_PTR T5VMCode_GetCustomFuncList (T5_PTR pCode, T5_PTWORD pwNbClass)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEFNC pFnc;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pFnc = T5GET_CODEFNC(pHeader);
    *pwNbClass = pFnc->wNbUsed;
    return ((T5_PTR)&(pFnc->ccFirstClass));
}

/*****************************************************************************
T5VMCode_GetNbString
Get the total number of STRINGS to be allocated (for memory sizing)
Parameters:
    pCode (IN) pointer to the application code
    pdwRawsize (OUT) full size of string buffer
Return: number of string variables
*****************************************************************************/

T5_WORD T5VMCode_GetNbString (T5_PTR pCode, T5_PTDWORD pdwRawSize)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5PTR_CODEINIT pInit;
    T5_PTBYTE pLen;
    T5_DWORD dwSize;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pInit = T5GET_CODEINIT(pHeader);
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    pLen = T5GET_CODESTRLEN(pHeader, pInit);
    dwSize = 0;
    while (*pLen)
    {
        dwSize += (((T5_DWORD)(*pLen) & 0xffL) + 3L);
        pLen++;
    }
    if (dwSize < pVarCount->dwStringBuf)
        dwSize = pVarCount->dwStringBuf;
    *pdwRawSize = dwSize;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    return (pVarCount->wNbStringAlloc + 1); /* index is 1 based! */
#else /*T5DEF_STRINGSUPPORTED*/
    *pdwRawSize = 0L;
    return 0;
#endif /*T5DEF_STRINGSUPPORTED*/
}

/*****************************************************************************
T5VMCode_GetStringSizing
Get fixed and used number of strings to be allocated
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the STRING table header
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_GetStringSizing (T5_PTR pCode, T5PTR_DB pDef)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    if (pVarCount->wNbStringUsed >= pDef->wNbItemAlloc)
        return T5RET_STRINGOVERFLOW;
    pDef->wNbItemFixed = pVarCount->wNbStringFixed + 1; /*index 1 based*/
    pDef->wNbItemUsed = pVarCount->wNbStringUsed + 1; /*index 1 based*/
#endif /*T5DEF_STRINGSUPPORTED*/
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_SetStringPointers
Allocate buffers for string variables
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer to the data base
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_SetStringPointers (T5_PTR pCode, T5PTR_DB pDB)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pInit;
    T5_PTBYTE pLen, pBuf;
    T5_PTBYTE *pString;
    T5_WORD wIndex;
    T5_DWORD dwSize;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pInit = T5GET_CODEINIT(pHeader);
    pLen = T5GET_CODESTRLEN(pHeader, pInit);
    pString = T5GET_DBSTRING(pDB);
    pBuf = T5GET_DBSTRBUF(pDB);
    wIndex = 0;
    dwSize = 0L;
    while (*pLen)
    {
        /* string variable index - 1 based */
        wIndex++;
        if (wIndex >= pDB[T5TB_STRING].wNbItemUsed)
            return T5RET_OK; /* can happen during On Line Change */
        /* set buffer and initialize string members */
        pString[wIndex] = pBuf;
        pBuf[0] = *pLen; /* max size */
        /* give space for the next */
        dwSize += (((T5_DWORD)(*pLen) & 0xffL) + 3L);
        if (dwSize > pDB[T5TB_STRBUF].dwRawSize)
            return T5RET_STRINGOVERFLOW;
        pBuf += (((T5_WORD)(*pLen) & 0xff) + 3);
        pLen++;
    }
#endif /*T5DEF_STRINGSUPPORTED*/
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_ApplyStringInit
Apply init values to STRING variables and constants
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) pointer the data base
    bFixedAlso (IN) if TRUE, apply also init values for fixed variables
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_ApplyStringInit (T5_PTR pCode, T5PTR_DB pDB,
                                T5_BOOL bFixedAlso)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pInit;
    T5_PTWORD pStrInit;
    T5PTR_DB pDef;
    T5_WORD wLen;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pInit = T5GET_CODEINIT(pHeader);
    pStrInit = T5GET_CODEINITSTR(pHeader, pInit);
    pDef = &(pDB[T5TB_STRING]);
    while (*pStrInit != 0)
    {
        if (*pStrInit < pDef->wNbItemUsed)
        {
            if (bFixedAlso || (*pStrInit >= pDef->wNbItemFixed))
            T5VMStr_Force (pDB, *pStrInit, (T5_PTBYTE)(pStrInit+1));
        }
        pStrInit++; /* now points to the string length */

        wLen = (T5_WORD)(*((T5_BYTE *)pStrInit)) & 0xff;
        if (wLen & 1) wLen++; /* Word aligned */
        wLen += 2; /* length + eos character */
        wLen /= 2; /* now a number of words */

        pStrInit += wLen;
    }
#endif /*T5DEF_STRINGSUPPORTED*/
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_GetNbStep
Get the total number of SFC steps to be allocated (for memory sizing)
Parameters:
    pCode (IN) pointer to the application code
Return: number of steps
*****************************************************************************/

T5_WORD T5VMCode_GetNbStep (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    return (pVarCount->wNbStepAlloc + 1); /* index is 1 based! */
}

/*****************************************************************************
T5VMCode_GetNbTrans
Get the total number of SFC transitions to be allocated (for memory sizing)
Parameters:
    pCode (IN) pointer to the application code
Return: number of transitions
*****************************************************************************/

T5_WORD T5VMCode_GetNbTrans (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    return (pVarCount->wNbTransAlloc + 1); /* index is 1 based! */
}

/*****************************************************************************
T5VMCode_GetVarTypeList
Get list of type masks
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to the list
*****************************************************************************/

T5_PTBYTE T5VMCode_GetVarTypeList (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pInit;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pInit = T5GET_CODEINIT(pHeader);
    return (T5GET_CODETYPES(pHeader, pInit));
}

/*****************************************************************************
T5VMCode_GetIODef
Get list of IO devices
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to the list of IO devices
*****************************************************************************/

T5_PTR T5VMCode_GetIODef (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;

    pHeader = (T5PTR_CODEHEADER)pCode;
    return T5GET_CODEIO(pHeader);
}

/*****************************************************************************
T5VMCode_GetPublishedList
Get list published variables
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to the list published variables
*****************************************************************************/

T5_PTR T5VMCode_GetPublishedList (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;

    pHeader = (T5PTR_CODEHEADER)pCode;
    return T5GET_CODEPUB(pHeader);
}

/*****************************************************************************
T5VMCode_GetAppName
Get the name of the application from the code
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to the application name in the code
*****************************************************************************/

T5_PTCHAR T5VMCode_GetAppName (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEAPP pApp;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pApp = T5GET_CODEAPP(pHeader);
    /* default all members to 0 */
    return (T5_PTCHAR)pApp->szName;
}

/*****************************************************************************
T5VMCode_GetNbXV
Get the total number of XV to be allocated (for memory sizing)
Parameters:
    pCode (IN) pointer to the application code
Return: number of 8 bit variables
*****************************************************************************/

T5_WORD T5VMCode_GetNbXV (T5_PTR pCode)
{
#ifdef T5DEF_XV
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    return (pVarCount->wNbXVAlloc + 1); /* index is 1 based! */
#else /*T5DEF_XV*/
    return 0;
#endif /*T5DEF_XV*/
}

/*****************************************************************************
T5VMCode_GetXVSizing
Get fixed and used number of XV to be allocated
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the DATA8 table header
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_GetXVSizing (T5_PTR pCode, T5PTR_DB pDef)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_RET wRet;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    if (pVarCount->wNbXVUsed < pDef->wNbItemAlloc)
    {
        pDef->wNbItemFixed = pVarCount->wNbXVUsed + 1; /* index is 1 based!*/
        pDef->wNbItemUsed = pVarCount->wNbXVUsed + 1; /* index is 1 based! */
        wRet = T5RET_OK;
    }
    else
        wRet = T5RET_XVOVERFLOW;
    return wRet;
}

/*****************************************************************************
T5VMCode_SetXVPointers
Initialize pointers for external variables (init to local data)
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the full database
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_SetXVPointers (T5_PTR pCode, T5PTR_DB pDB)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEXV pXVCode;
    T5_WORD wXVIndex;
    T5PTR_DB pXVTable;
    T5_PTBYTE *pXV;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pXVCode = T5GET_CODEXV(pHeader);
    pXVTable = &(pDB[T5TB_XV]);
    pXV = (T5_PTBYTE *)(pXVTable->pData);
    wXVIndex = 1;
    while (pXVCode->wIndex != 0 && wXVIndex < pXVTable->wNbItemUsed)
    {
        switch (pXVCode->wTicType)
        {
        case T5C_BOOL :
        case T5C_SINT :
            pXV[wXVIndex] = (T5GET_DBDATA8(pDB) + pXVCode->wIndex);
            break;
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT :
            pXV[wXVIndex] = (T5_PTBYTE)(T5GET_DBDATA16(pDB) + pXVCode->wIndex);
            break;
#endif /*T5DEF_DATA16SUPPORTED*/
        case T5C_DINT :
        case T5C_REAL :
            pXV[wXVIndex] = (T5_PTBYTE)(T5GET_DBDATA32(pDB) + pXVCode->wIndex);
            break;
#ifdef T5DEF_DATA64SUPPORTED
        case T5C_LREAL :
        case T5C_LINT :
            pXV[wXVIndex] = (T5_PTBYTE)(T5GET_DBDATA64(pDB) + pXVCode->wIndex);
            break;
#endif /*T5DEF_DATA64SUPPORTED*/
        case T5C_TIME :
            pXV[wXVIndex] = (T5_PTBYTE)(T5GET_DBTIME(pDB) + pXVCode->wIndex);
            break;
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING :
            pXV[wXVIndex] = *(T5GET_DBSTRING(pDB) + pXVCode->wIndex);
            break;
#endif /*T5DEF_STRINGSUPPORTED*/
        default : return T5RET_ERROR;
        }
        wXVIndex++;
        pXVCode++;
    }
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_BuildTaskList
Initialize the list of tasks
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the full database
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_BuildTaskList (T5_PTR pCode, T5PTR_DB pDB)
{
    T5_WORD wNbTask;
    T5PTR_CODEHEADER pHeader;
    T5_PTWORD pTask;
    T5PTR_DBTASK pDef;
    T5PTR_DB pTable;

    pTable = &(pDB[T5TB_TASK]);
    pHeader = (T5PTR_CODEHEADER)pCode;
    pTask = T5GET_CODETASK(pHeader);
    wNbTask = 0;
    pDef = T5GET_DBTASK(pDB);
    pTable->wNbItemFixed = pTable->wNbItemUsed = 0;
    while (*pTask != 0)
    {
        pDef[wNbTask].pName = (T5_PTCHAR)pTask + 4; /* skip two words */
        pDef[wNbTask].pPou = (T5_PTWORD)((T5_PTBYTE)pTask + pTask[1]);
        wNbTask++;
        pTable->wNbItemFixed = pTable->wNbItemUsed = wNbTask;
        if (wNbTask > T5MAX_TASK)
            return T5RET_TASKOVERFLOW;
        pTask = (T5_PTWORD)((T5_PTBYTE)pTask + *pTask);
    }
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_CanHOTBuildTaskList
Check if the list of tasks can be updated for hot restart or on line change
Parameters:
    pCode (IN) pointer to the application code
    pDef (IN) pointer to the full database
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_CanHOTBuildTaskList (T5_PTR pCode, T5PTR_DB pDB)
{
    T5_WORD wNbTask;
    T5PTR_CODEHEADER pHeader;
    T5_PTWORD pTask;
    T5_UNUSED(pDB);

    pHeader = (T5PTR_CODEHEADER)pCode;
    pTask = T5GET_CODETASK(pHeader);
    wNbTask = 0;
    while (*pTask != 0)
    {
        wNbTask++;
        if (wNbTask > T5MAX_TASK)
            return T5RET_TASKOVERFLOW;
        pTask = (T5_PTWORD)((T5_PTBYTE)pTask + *pTask);
    }
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_MBClientConfig
Get pointer to MODBUS configuration
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to MODBUS config or NULL if not supported
*****************************************************************************/

T5_PTR T5VMCode_MBClientConfig (T5_PTR pCode)
{
#ifdef T5DEF_MODBUSIOS
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 2)
        return T5_PTNULL;
    return T5_OFF(pHeader, pExtra->dwMBC);
#else /*T5DEF_MODBUSIOS*/
    return T5_PTNULL;
#endif /*T5DEF_MODBUSIOS*/
}

/*****************************************************************************
T5VMCode_MBAsiConfig
Get pointer to ASi configuration
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to ASi config or NULL if not supported
*****************************************************************************/

T5_PTR T5VMCode_AsiConfig (T5_PTR pCode)
{
#ifdef T5DEF_ASI
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 3)
        return T5_PTNULL;
    return T5_OFF(pHeader, pExtra->dwASI);
#else /*T5DEF_ASI*/
    return T5_PTNULL;
#endif /*T5DEF_ASI*/
}

/*****************************************************************************
T5VMCode_StaticEvents
Get pointer to static events configuration
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to config or NULL if not supported
*****************************************************************************/

T5_PTR T5VMCode_StaticEvents (T5_PTR pCode)
{
#ifdef T5DEF_EA
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 4)
        return T5_PTNULL;
    return T5_OFF(pHeader, pExtra->dwEvents);
#else /*T5DEF_EA*/
    return T5_PTNULL;
#endif /*T5DEF_EA*/
}

/*****************************************************************************
T5VMCode_GetEAX
Get pointer to static external events configuration
Parameters:
    pCode (IN) pointer to the application code
    ppCnx (OUT) pointer to the definition of external connections
    ppVar (OUT) pointer to the definition of external variables
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5VMCode_GetEAX (T5_PTR pCode, T5_PTR *ppCnx, T5_PTR *ppVar)
{
#ifdef T5DEF_EA
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 4)
        return FALSE;

    if (pExtra->dwEvtCnx == 0 || pExtra->dwEvtExt == 0)
        return FALSE;

    if (ppCnx != T5_PTNULL)
        *ppCnx = T5_OFF(pHeader, pExtra->dwEvtCnx);
    if (ppVar != T5_PTNULL)
        *ppVar = T5_OFF(pHeader, pExtra->dwEvtExt);

    return TRUE;
#else /*T5DEF_EA*/
    return FALSE;
#endif /*T5DEF_EA*/
}

/*****************************************************************************
T5VMCode_GetBusDrv
Get pointer to static bus driver configuration
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to main table or NULL
*****************************************************************************/

T5_PTR T5VMCode_GetBusDrv (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 5)
        return NULL;

    return T5_OFF(pHeader, pExtra->dwBusDrv);
}

/*****************************************************************************
T5VMCode_GetBusIOList
Get list of IO points list for bus drivers
Parameters:
    pCode (IN) pointer to the application code
Return: pointer to main table or NULL
*****************************************************************************/

T5_PTR T5VMCode_GetBusIOList (T5_PTR pCode)
{
	T5PTR_CODEHEADER pHeader;
	T5PTR_CODEEXTRA pExtra;
	T5_PTDWORD pdwExtra;

	pHeader = (T5PTR_CODEHEADER)pCode;
	pExtra = T5GET_CODEEXTRA(pHeader);
	if (pExtra->dwVers < 9)
		return NULL;

	pdwExtra = (T5_PTDWORD)pExtra;
	return T5_OFF(pHeader, pdwExtra[11]);
}

/*****************************************************************************
T5VMCode_GetNbIOs
Get total number of IOs in the application
Parameters:
    pCode (IN) pointer to the application code
    pdwNb (OUT) number of IOs if known
Return: TRUE if info is available / FALSE if older compiler version
*****************************************************************************/

T5_BOOL T5VMCode_GetNbIOs (T5_PTR pCode, T5_PTDWORD pdwNb)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    if (pdwNb == NULL)
        return FALSE;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 6)
    {
        *pdwNb = 0;
        return FALSE;
    }

    *pdwNb = pExtra->dwNbIOs;
    return TRUE;
}

/*****************************************************************************
T5VMCode_IsCCPure
Check if p-code is available
Parameters:
    pCode (IN) pointer to the app cope
Return: != 0 if no p-code available
*****************************************************************************/

T5_BOOL T5VMCode_IsCCPure (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 10)
        return FALSE;

    return ((pExtra->dwCCPure & 1L) != 0L);
}

#ifdef T5DEF_CTSEG

/*****************************************************************************
T5VMCode_GetCTSegDef
Get CTSEG segment definition
Parameters:
    pCode (IN) pointer to the app cope
Return: pointer to table in code
*****************************************************************************/

T5_PTR T5VMCode_GetCTSegDef (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 7)
        return NULL;
    if (pExtra->dwCTSeg == 0L)
        return NULL;
    return T5_OFF(pHeader, pExtra->dwCTSeg);
}

/*****************************************************************************
T5VMCode_GetCTTypeDef
Get CTSEG data types definition
Parameters:
    pCode (IN) pointer to the app cope
Return: pointer to table in code
*****************************************************************************/

T5_PTCHAR T5VMCode_GetCTTypeDef (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 8)
        return NULL;
    if (pExtra->dwCTTypes == 0L)
        return NULL;
    return (T5_PTCHAR)T5_OFF(pHeader, pExtra->dwCTTypes);
}

/*****************************************************************************
T5VMCode_GetCTSegAllocSize
Get size to be allocated for CTSEG segment
Parameters:
    pCode (IN) pointer to the app cope
Return: size in bytes
*****************************************************************************/

T5_DWORD T5VMCode_GetCTSegAllocSize (T5_PTR pCode)
{
    T5_PTDWORD pdwSize;

    pdwSize = (T5_PTDWORD)T5VMCode_GetCTSegDef (pCode);
    if (pdwSize == NULL)
        return 0L;
    return *pdwSize;
}

/*****************************************************************************
T5VMCode_GetCTSegUsedSize
Get size actually used for CTSEG segment
Parameters:
    pCode (IN) pointer to the app cope
Return: size in bytes
*****************************************************************************/

T5_DWORD T5VMCode_GetCTSegUsedSize (T5_PTR pCode)
{
    T5_PTDWORD pdwSize;

    pdwSize = (T5_PTDWORD)T5VMCode_GetCTSegDef (pCode);
    if (pdwSize == NULL)
        return 0L;
    return pdwSize[1];
}

/*****************************************************************************
T5VMCode_ApplyCTSegInit
Apply initial values within the CTSEG segment
Parameters:
    pCode (IN) pointer to the app cope
    pDB (IN) pointer to the VMDB
    dwStartOffset (IN) ignore data before this offset
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_ApplyCTSegInit (T5_PTR pCode, T5PTR_DB pDB, T5_DWORD dwStartOffset)
{
    T5_PTBYTE pData, pMem;
    T5_DWORD dwOffset;
    T5_BYTE bLen;

    pData = (T5_PTBYTE)T5VMCode_GetCTSegDef (pCode);
    if (pData == NULL)
        return T5RET_OK;

    pData += 8; /* 2 DWORDs for sizing */

    pMem = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);

    while (*pData != 0)
    {
        /* length on 1 byte */
        bLen = *pData++;
        /* offset on 4 bytes */
        T5_MEMCPY(&dwOffset, pData, sizeof (T5_DWORD));
        pData += sizeof (T5_DWORD);
        /* data */
        if (dwOffset >= dwStartOffset)
            T5_MEMCPY(pMem + dwOffset, pData, bLen);
        pData += bLen;
    }
    return T5RET_OK;
}

#endif /*T5DEF_CTSEG*/

/*****************************************************************************
T5VMCode_GetRecipes
Get definition of embedded recipes
Parameters:
    pCode (IN) pointer to the app cope
Return: pointer to table in code
*****************************************************************************/

#ifdef T5DEF_RECIPES

T5PTR_CODERCPL T5VMCode_GetRecipes (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 12)
        return NULL;
    return (T5PTR_CODERCPL)T5_OFF(pHeader, pExtra->dwRecipes);
}

#endif /*T5DEF_RECIPES*/

/*****************************************************************************
T5VMCode_GetVLsts
Get definition of embedded lists of variables
Parameters:
    pCode (IN) pointer to the app cope
Return: pointer to table in code
*****************************************************************************/

#ifdef T5DEF_VLST

T5PTR_CODEVLSTL T5VMCode_GetVLsts (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 13)
        return NULL;
    return (T5PTR_CODEVLSTL)T5_OFF(pHeader, pExtra->dwVLst);
}

#endif /*T5DEF_VLST*/

/*****************************************************************************
T5VMCode_GetDefStringTable
Get definition of embedded string tables
Parameters:
    pCode (IN) pointer to the app cope
Return: pointer to table in code
*****************************************************************************/

#ifdef T5DEF_STRINGTABLE

T5_DWORD T5VMCode_GetDefStringTable (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;
    T5_PTWORD pDef;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 14)
        return 0L;

    pDef = (T5_PTWORD)T5_OFF(pHeader, pExtra->dwStrTable);
    pDef++; /* skip version number */
    if (*pDef == 0)
        return 0L;

    return 1L;
}

/*****************************************************************************
T5VMCode_GetStringTable
Get definition of one string table
Parameters:
    pCode (IN) pointer to the app cope
    dwIndex (IN) table index
Return: pointer to table in code
*****************************************************************************/

T5_PTR T5VMCode_GetStringTable (T5_PTR pCode, T5_DWORD dwIndex)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;
    T5_PTWORD pDef;
    T5_PTDWORD pTable;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 14)
        return NULL;

    pDef = (T5_PTWORD)T5_OFF(pHeader, pExtra->dwStrTable);
    if (dwIndex < 1 || dwIndex > (T5_DWORD)(pDef[1]))
        return NULL;

    pTable = (T5_PTDWORD)pDef;
    pTable++; /* skip header */
    pTable += (dwIndex - 1);

    return T5_OFF(pHeader, *pTable);
}

#endif /*T5DEF_STRINGTABLE*/

/*****************************************************************************
T5VMCode_GetSignal
Get definition of an embedded signal
Parameters:
    pCode (IN) pointer to the app cope
    dwIndex (IN) index of the signal
Return: pointer to table in code
*****************************************************************************/

#ifdef T5DEF_SIGNAL

T5_PTR T5VMCode_GetSignal (T5_PTR pCode, T5_DWORD dwIndex)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;
    T5_PTWORD pDef;
    T5_PTDWORD pTable;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 14)
        return NULL;

    pDef = (T5_PTWORD)T5_OFF(pHeader, pExtra->dwSignal);
    if (dwIndex < 1 || dwIndex > (T5_DWORD)(pDef[1]))
        return NULL;

    pTable = (T5_PTDWORD)pDef;
    pTable++; /* skip header */
    pTable += (dwIndex - 1);

    return T5_OFF(pHeader, *pTable);
}

#endif /*T5DEF_SIGNAL*/

/*****************************************************************************
T5VMCode_GetCanDef
Get definition of CAN bus IOs
Parameters:
    pCode (IN) pointer to the app cope
Return: pointer to table in code
*****************************************************************************/

#ifdef T5DEF_CANBUS

T5PTR_CANDEF T5VMCode_GetCanDef (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 15)
        return NULL;

    return (T5PTR_CANDEF)T5_OFF(pHeader, pExtra->dwCanBus);
}

#endif /*T5DEF_CANBUS*/

/*****************************************************************************
T5VMCode_GetHeapSize
Get configured heeap size
Parameters:
    pCode (IN) pointer to the app cope
Return: wished size
*****************************************************************************/

T5_DWORD T5VMCode_GetHeapSize (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 16)
        return 0L;

    return pExtra->dwHeapSize;
}

/*****************************************************************************
T5VMCode_AllocHeap
Create the application heap
Parameters:
    pCode (IN) pointer to the app cope
    pDB (IN) pointer to the VMDB
    bHot (IN) TRUE in case of a hot restart
Return: ok or error
*****************************************************************************/

T5_RET T5VMCode_AllocHeap (T5_PTR pCode, T5PTR_DB pDB, T5_BOOL bHot)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTBYTE pBuffer;

    pPrivate = T5GET_DBPRIVATE(pDB);
    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 16 || pExtra->dwHeapSize == 0L)
    {
        pPrivate->pHeap = 0L;
        return T5RET_OK;
    }

    if ((pExtra->dwHeapSize & 0x80000000L) == 0L)
    {
        pBuffer = (T5_PTBYTE)pPrivate;
        pBuffer += sizeof (T5STR_DBPRIVATE);
        if (!bHot)
            pPrivate->pHeap = T5Heap_CreateHeapBuffer (pBuffer, pExtra->dwHeapSize, 0L);
        else
        {
            pPrivate->pHeap = T5Heap_RelocateHeapBuffer (pBuffer, pExtra->dwHeapSize, 0L);
            if (pPrivate->pHeap == NULL)
                return T5RET_HOTHEAP;
        }
    }
    else
    {
        if (bHot)
            return T5RET_HOTHEAP;
        pPrivate->pHeap = T5Heap_CreateHeapMalloc (64L, 0L);
    }
    return T5RET_OK;
}

/*****************************************************************************
T5VMCode_GetITs
Get configuration of soft ITs
Parameters:
    pCode (IN) pointer to the app cope
Return: pointer to the definition in code
*****************************************************************************/

T5_PTWORD T5VMCode_GetITs (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 18)
        return NULL;

    return (T5_PTWORD)T5_OFF(pHeader, pExtra->dwIT);
}

/*****************************************************************************
T5VMCode_GetITCount
Get number of configured soft ITs
Parameters:
    pCode (IN) pointer to the app cope
Return: number of events configured
*****************************************************************************/

T5_WORD T5VMCode_GetITCount (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 18)
        return 0;
    if (pExtra->dwVers < 20)
        return 16;
    return T5MAX_EVENT;
}

/*****************************************************************************
T5VMCode_IsCodeDebug
Check if code was compiled in debug mode
Parameters:
    pCode (IN) pointer to the app cope
Return: TRUE if compiled in debug
*****************************************************************************/

T5_BOOL T5VMCode_IsCodeDebug (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 20)
        return FALSE;
    return ((pExtra->dwSMFlags & 0x00000002L) != 0);
}

/*****************************************************************************
T5VMCode_GetInstanceNames
Get list of C instance names
Parameters:
    pCode (IN) pointer to the app cope
Return: points to the list of names
*****************************************************************************/

T5_PTCHAR T5VMCode_GetInstanceNames (T5_PTR pCode)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEEXTRA pExtra;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pExtra = T5GET_CODEEXTRA(pHeader);
    if (pExtra->dwVers < 22)
        return (T5_PTCHAR)("\0");

    return (T5_PTCHAR)T5_OFF(pHeader, pExtra->dwCFBINames);
}

/* eof **********************************************************************/
