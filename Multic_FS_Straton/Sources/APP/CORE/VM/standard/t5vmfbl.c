/*****************************************************************************
T5VMfbl.c :  manage function block classes and handlers

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/
/* static services */

static void _T5HLIB_Handler (T5_PTCHAR szName, T5HND_FB pf,
                             T5_WORD wMode, T5_PTR pData,
                             T5_PTBOOL pbContinue);

/****************************************************************************/
/* default calback for unresolved classes */

T5_DWORD T5VMFbl_null (T5_WORD wCommand, T5PTR_DB pBase,
                       T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_UNUSED(pBase);
    T5_UNUSED(pClass);
    T5_UNUSED(pInst);
    T5_UNUSED(pArgs);

    if (wCommand == T5FBCMD_ACCEPTCT)
        return 1L;
    return 0L;
}

/****************************************************************************/
/* default calback for unresolved functions */

T5_DWORD T5VMFCust_null (T5_WORD wCommand, T5PTR_DB pBase,
                         T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs)
{
    T5_WORD wNbPar;
    T5_PTBYTE pData8;
    T5_PTLONG pData32;
    T5_PTDWORD pTime;
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 pData64;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTSHORT pData16;
#endif /*T5DEF_DATA16SUPPORTED*/
    T5_UNUSED(pClass);
    T5_UNUSED(pInst);

    if (wCommand == T5FBCMD_ACCEPTCT)
        return 1L;

    wNbPar = *(pArgs-1);
    switch (pArgs[wNbPar])
    {
    case T5C_BOOL :
        pData8 = T5GET_DBDATA8(pBase);
        pData8[pArgs[wNbPar-1]] = 0;
        break;
    case T5C_DINT :
    case T5C_REAL :
        pData32 = (T5_PTLONG)T5GET_DBDATA32(pBase);
        pData32[pArgs[wNbPar-1]] = 0L;
        break;
    case T5C_TIME :
        pTime = T5GET_DBTIME(pBase);
        pTime[pArgs[wNbPar-1]] = 0L;
        break;
#ifdef T5DEF_DATA64SUPPORTED
    case T5C_LREAL:
    case T5C_LINT :
        pData64 = T5GET_DBDATA64(pBase);
        pData64[pArgs[wNbPar-1]] = (T5_DATA64)0;
        break;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        pData16 = (T5_PTSHORT)T5GET_DBDATA16(pBase);
        pData16[pArgs[wNbPar-1]] = 0;
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
    case T5C_STRING :
        T5VMStr_Empty (pBase, pArgs[wNbPar-1]);
        break;
#endif /*T5DEF_STRINGSUPPORTED*/
    
    default : break;
    }
    return 0L;
}

/****************************************************************************/
/* modes for library scanning */

#define T5STFBMODE_COUNT            1
#define T5STFBMODE_GET              2
#define T5STFBMODE_SIZECLASS        4
#define T5STFBMODE_SIZEINSTANCE     6
#define T5STFBMODE_FINDHANDLER      7

typedef struct
{
    T5_DWORD dwCount;
    T5_STRING16 szName;
    T5_WORD wID;
    T5_WORD res1;
    T5_PTR pData;
    T5HND_FB pfHandler;
} _str_find;

/*****************************************************************************
_T5HLIB_Handler
library scanning callback
Parameters:
    szName (IN) name of the handler
    pf (IN) handler entry point
    wMode (IN) scanning mode
    pData (IN) pointer to scanning process data
    pbContinue (IN/OUT) pointer to a flag saying if processing is required 
pData points to a WORD counter in T5STFBMODE_COUNT mode
pData points to a DWORD size counter in T5STFBMODE_SIZECLASS mode
pData points to the database in T5STFBMODE_SIZEOFCLASS / T5STFBMODE_GET modes
pData points to a _str_find structure in T5STFBMODE_SIZEOFINSTANCE mode
Return: number of supported FB classes
*****************************************************************************/

static void _T5HLIB_Handler (T5_PTCHAR szName, T5HND_FB pf,
                             T5_WORD wMode, T5_PTR pData,
                             T5_PTBOOL pbContinue)
{
    T5PTR_DB pDB, pDef;
    T5PTR_DBFBCLASS pClass;
    T5_DWORD dwSize;
    _str_find *pFind;

    switch (wMode)
    {
    /* count all classes */
    case T5STFBMODE_COUNT :
        *(T5_PTWORD)pData += 1;
        break;
    /* find a handler using its name - stop when found */
    case T5STFBMODE_FINDHANDLER :
        if(*pbContinue == TRUE)
        {
            pFind = (_str_find *)pData;
            if (T5_STRCMP(pFind->szName, szName) == 0)
            {
                pFind->pfHandler = pf;
                *pbContinue = FALSE;
            }
        }
        break;
    /* cumulate class data size for all classes */
    case T5STFBMODE_SIZECLASS :
        dwSize = pf (T5FBCMD_SIZEOFCLASS,
                     T5_PTNULL, T5_PTNULL, T5_PTNULL, T5_PTNULL);
        *(T5_PTDWORD)pData += T5_ALIGNED(dwSize);
        break;
    /* (cumulate) get size of one instance - stop when class found */
    case T5STFBMODE_SIZEINSTANCE :
        pFind = (_str_find *)pData;
        if (*pbContinue
            && ((pFind->wID != 0
                && pFind->wID == (T5_WORD)pf (T5FBCMD_GETCLASSID,
                                T5_PTNULL, T5_PTNULL, T5_PTNULL, T5_PTNULL))
            || (pFind->wID == 0 && T5_STRCMP(pFind->szName, szName) == 0)))
        {
            dwSize = pf (T5FBCMD_SIZEOFINSTANCE,
                         T5_PTNULL, T5_PTNULL, T5_PTNULL, T5_PTNULL);
            pFind->dwCount += T5_ALIGNED(dwSize);
            *pbContinue = FALSE;
        }
        break;
    /* register all classes - dont stop */
    case T5STFBMODE_GET :
        pDB = (T5PTR_DB)pData;
        pDef = &(pDB[T5TB_FBCLASS]);
        if (pDef->wNbItemUsed < pDef->wNbItemAlloc)
        {
            pClass = (T5PTR_DBFBCLASS)(pDef->pData);
            pClass = &(pClass[pDef->wNbItemUsed]);

            pClass->pfHandler = pf;
            T5_STRCPY(pClass->szName, szName);
            pClass->wFlags = T5FBCLASS_VALID;

            dwSize = pf (T5FBCMD_SIZEOFCLASS,
                         T5_PTNULL, T5_PTNULL, T5_PTNULL, T5_PTNULL);
            pClass->dwClassSizeof = T5_ALIGNED(dwSize);
            dwSize = pf (T5FBCMD_SIZEOFINSTANCE,
                         T5_PTNULL, T5_PTNULL, T5_PTNULL, T5_PTNULL);
            pClass->dwInstSizeof = T5_ALIGNED(dwSize);
            pDef->wNbItemUsed += 1;
            pDef->wNbItemFixed += 1;
        }
        break;
    default : break;
    }
}

/*****************************************************************************
T5VMFbl_List
enumerate handlers
Parameters:
    wMode (IN) enumeration method
    pData (IN) caller data argument
    bStandard (IN) true=standard / false=oem
Return: TRUE if not finished
*****************************************************************************/

T5_BOOL T5VMFbl_List (T5_WORD wMode, T5_PTR pData, T5_BOOL bStandard)
{
    T5_BOOL bContinue, bEnum;
    T5_DWORD dwID;
    T5_PTCHAR szName;
    T5HND_FB pFB;

    if (bStandard)
    {
        bContinue = FALSE;
        /* list is in the macro */
        T5HLIBLIST_STD;
    }
    else
    {
        bContinue = TRUE;
        bEnum = T5Blocks_GetFirstID (&dwID);
        while (bContinue && bEnum)
        {
            szName = T5Blocks_GetName (dwID);
            pFB = T5Blocks_GetProc (dwID);
            if (szName != T5_PTNULL && pFB != T5_PTNULL)
                _T5HLIB_Handler (szName, pFB, wMode, pData, &bContinue);
            bEnum = T5Blocks_GetNextID (&dwID);
        }

#ifdef T5DEF_DLLBLOCKS

        if (T5Bdll_StartEnumerate ())
        {
            bEnum = T5Bdll_GetFirstID (&dwID);
            while (bContinue && bEnum)
            {
                szName = T5Bdll_GetName (dwID);
                pFB = T5Bdll_GetProc (dwID);
                if (szName != T5_PTNULL && pFB != T5_PTNULL)
                    _T5HLIB_Handler (szName, pFB, wMode, pData, &bContinue);
                bEnum = T5Bdll_GetNextID (&dwID);
            }
            T5Bdll_StopEnumerate ();
        }

#endif /*T5DEF_DLLBLOCKS*/

    }
    return bContinue;
}

/*****************************************************************************
_T5VMFbl_FindClass
search for a class handler
Parameters:
    szClass (IN) FB class name
Return: handler function or NULL if not found
*****************************************************************************/

#ifdef T5DEF_LOADONLYUSEDCLASSES

static T5HND_FB _T5VMFbl_FindClass (T5_PTCHAR szClass)
{
    _str_find find;

    T5_MEMSET (&find, 0, sizeof (find));
    T5_STRCPY (find.szName, szClass);
    T5VMFbl_List (T5STFBMODE_FINDHANDLER, &find, TRUE);
    if (find.pfHandler == NULL)
    {
#ifdef T5DEF_T5BLOCKS_FIND
        find.pfHandler = T5Blocks_Find (szClass);
#else /*T5DEF_T5BLOCKS_FIND*/
        T5VMFbl_List (T5STFBMODE_FINDHANDLER, &find, FALSE);
#endif /*T5DEF_T5BLOCKS_FIND*/
    }
    return find.pfHandler;
}

#endif /*T5DEF_LOADONLYUSEDCLASSES*/

/*****************************************************************************
T5VMFbl_GetNbClass
Get number of standard aznd custom FB classes supported by the VM
Parameters:
    pCode (IN) application code
Return: number of supported FB classes
*****************************************************************************/

T5_WORD T5VMFbl_GetNbClass (T5_PTR pCode)
{
    T5_WORD wCount;
    T5_UNUSED(pCode);

    wCount = 0;
#ifdef T5DEF_LOADONLYUSEDCLASSES
    T5VMCode_GetClassList (pCode, &wCount);
#else /*T5DEF_LOADONLYUSEDCLASSES*/
    T5VMFbl_List (T5STFBMODE_COUNT, &wCount, TRUE);
    T5VMFbl_List (T5STFBMODE_COUNT, &wCount, FALSE);
#endif /*T5DEF_LOADONLYUSEDCLASSES*/
    return wCount;
}

/*****************************************************************************
T5VMFbl_GetClassDataSize
Get size of memory needed for custom FB classes
Parameters:
    pCode (IN) application code
Return: size of raw memory to be allocated
*****************************************************************************/

T5_DWORD T5VMFbl_GetClassDataSize (T5_PTR pCode)
{
    T5_DWORD dwCount;
    T5_UNUSED(pCode);

    dwCount = 0L;
#ifdef T5DEF_LOADONLYUSEDCLASSES
    {
        T5_WORD i, wNbClass;
        T5PTR_CODECLASS pClass = (T5PTR_CODECLASS)T5VMCode_GetClassList (pCode, &wNbClass);
        for (i=0; i<wNbClass; i++)
        {
            T5HND_FB pf = _T5VMFbl_FindClass (pClass[i].szName);
            if (pf != NULL)
                dwCount += pf (T5FBCMD_SIZEOFCLASS, T5_PTNULL, T5_PTNULL, T5_PTNULL, T5_PTNULL);
        }
    }
#else /*T5DEF_LOADONLYUSEDCLASSES*/
    T5VMFbl_List (T5STFBMODE_SIZECLASS, &dwCount, TRUE);
    T5VMFbl_List (T5STFBMODE_SIZECLASS, &dwCount, FALSE);
#endif /*T5DEF_LOADONLYUSEDCLASSES*/
    return dwCount;
}

/*****************************************************************************
T5VMFbl_GetInstanceDataSizeExact
Get size of memory used for FB instances
Parameters:
    pCode (IN) application code
Return: size of raw memory to be allocated
*****************************************************************************/

T5_DWORD T5VMFbl_GetInstanceDataSizeExact (T5_PTR pCode)
{
    T5PTR_CODEFBINST pInst;
    T5PTR_CODECLASS pClass;
    T5_WORD i, wNbClass;
    T5_BOOL bFound;
    _str_find find;
#ifdef T5DEF_T5BLOCKS_FIND
    T5HND_FB pf;
#endif /*T5DEF_T5BLOCKS_FIND*/

    find.dwCount = 0;
    pInst = (T5PTR_CODEFBINST)T5VMCode_GetInstanceList (pCode);
    pClass = (T5PTR_CODECLASS)T5VMCode_GetClassList (pCode, &wNbClass);
    while (pInst->wID != 0)
    {
        if (!pInst->wIsStd)
        {
            find.wID = 0;
            for (i=0, bFound=FALSE; !bFound && i<wNbClass; i++)
            {
                if (pInst->wID == pClass[i].wID)
                {
#ifdef T5DEF_T5BLOCKS_FIND
                    T5_DWORD dwSize;
                    pf = T5Blocks_Find (pClass[i].szName);
                    if (pf != NULL)
                    {
                        dwSize = pf (T5FBCMD_SIZEOFINSTANCE,
                                     T5_PTNULL, T5_PTNULL, T5_PTNULL, T5_PTNULL);
                        find.dwCount += T5_ALIGNED(dwSize);
                    }
                    bFound = TRUE;
#else /*T5DEF_T5BLOCKS_FIND*/
                    T5_STRCPY(find.szName, pClass[i].szName);
                    T5VMFbl_List (T5STFBMODE_SIZEINSTANCE, &find, FALSE);
                    bFound = TRUE;
#endif /*T5DEF_T5BLOCKS_FIND*/
                }
            }
        }
        else
        {
            find.wID = pInst->wID;
            *(find.szName) = '\0';
            T5VMFbl_List (T5STFBMODE_SIZEINSTANCE, &find, TRUE);
        }
        pInst++;
    }
    return find.dwCount;
}

/*****************************************************************************
T5VMFbl_GetInstanceDataSize
Get size of memory to be allocated for FB instances
Parameters:
    pCode (IN) application code
Return: size of raw memory to be allocated
*****************************************************************************/

T5_DWORD T5VMFbl_GetInstanceDataSize (T5_PTR pCode)
{
    T5_DWORD dwCount, dwWished;

    dwCount = T5VMFbl_GetInstanceDataSizeExact (pCode);
    dwWished = T5VMCode_GetWishedFBIRawSize (pCode);
    if (dwCount < dwWished)
        dwCount = dwWished;
    return dwCount;
}

/*****************************************************************************
T5VMFbl_Link
Load all application classes
Establish the link with FB classes handlers
Parameters:
    pCode (IN) application code
    pDB (IN) application database
Return: number of supported FB classes
*****************************************************************************/

T5_RET T5VMFbl_Link (T5_PTR pCode, T5PTR_DB pDB)
{
    T5_BOOL bFound;
    T5PTR_CODECLASS pClass;
    T5_WORD i, j, wNbClass;
    T5PTR_DB pDef;
    T5PTR_DBFBCLASS pdClass;
    T5_PTBYTE pClassData;

    pDef = &(pDB[T5TB_FBCLASS]);
    pDef->wNbItemFixed = pDef->wNbItemUsed = 0;
    pdClass = (T5PTR_DBFBCLASS)(pDef->pData);

    /* init classes */
#ifdef T5DEF_LOADONLYUSEDCLASSES
    pClass = (T5PTR_CODECLASS)T5VMCode_GetClassList (pCode, &wNbClass);
    for (i=0; i<wNbClass && i < pDB[T5TB_FBCLASS].wNbItemAlloc; i++, pClass++)
    {
        pdClass[pDB[T5TB_FBCLASS].wNbItemUsed].pfHandler = _T5VMFbl_FindClass (pClass->szName);
        if (pdClass[pDB[T5TB_FBCLASS].wNbItemUsed].pfHandler)
        {
            T5_DWORD dwSize;

            T5_STRCPY (pdClass[pDB[T5TB_FBCLASS].wNbItemUsed].szName, pClass->szName);
            pdClass[pDB[T5TB_FBCLASS].wNbItemUsed].wFlags = T5FBCLASS_VALID;
            dwSize = pdClass[pDB[T5TB_FBCLASS].wNbItemUsed].pfHandler (T5FBCMD_SIZEOFCLASS,
                                            T5_PTNULL, T5_PTNULL, T5_PTNULL, T5_PTNULL);
            pdClass[pDB[T5TB_FBCLASS].wNbItemUsed].dwClassSizeof = T5_ALIGNED(dwSize);
            dwSize = pdClass[pDB[T5TB_FBCLASS].wNbItemUsed].pfHandler (T5FBCMD_SIZEOFINSTANCE,
                                            T5_PTNULL, T5_PTNULL, T5_PTNULL, T5_PTNULL);
            pdClass[pDB[T5TB_FBCLASS].wNbItemUsed].dwInstSizeof = T5_ALIGNED(dwSize);
            pDB[T5TB_FBCLASS].wNbItemUsed += 1;
            pDB[T5TB_FBCLASS].wNbItemFixed += 1;
        }
    }
#else /*T5DEF_LOADONLYUSEDCLASSES*/
    T5VMFbl_List (T5STFBMODE_GET, pDB, TRUE);
    T5VMFbl_List (T5STFBMODE_GET, pDB, FALSE);
#endif /*T5DEF_LOADONLYUSEDCLASSES*/
    /* link application classes */
    pClass = (T5PTR_CODECLASS)T5VMCode_GetClassList (pCode, &wNbClass);
    for (i=0; i<wNbClass; i++)
    {
        bFound = FALSE;
        for (j=0; !bFound && j<pDef->wNbItemAlloc; j++)
        {
            if (T5_STRCMP(pdClass[j].szName, pClass[i].szName) == 0)
            {
                pdClass[j].wID = pClass[i].wID;
                bFound = TRUE;
            }
        }
        if (!bFound)
        {
            if (pClass[i].wNbInst > 0)
                T5VMLog_RegisterUnresolved (pDB, (T5_PTCHAR)pClass[i].szName);
        }
    }
    /* give space for class data */
    pClassData = (T5_PTBYTE)(pDB[T5TB_FBCDATA].pData);
    pDef = &(pDB[T5TB_FBCLASS]);
    pdClass = (T5PTR_DBFBCLASS)(pDef->pData);
    for (i=0; i<pDef->wNbItemUsed; i++)
    {
        pdClass[i].pData = (T5_PTR)pClassData;
        pClassData += pdClass[i].dwClassSizeof;
        pdClass[i].pfHandler (T5FBCMD_INITCLASS, pDB, pdClass[i].pData,
                                T5_PTNULL, T5_PTNULL);
    }
    return T5RET_OK;
}

/*****************************************************************************
T5VMFbl_InitInstances
Initialize all FB instances
Parameters:
    pCode (IN) application code
    pDB (IN) application database
    bInitInstance (IN) instance initialization requires
Return: ok or error (bad code or not supported standard class)
*****************************************************************************/

T5_RET T5VMFbl_InitInstances (T5_PTR pCode, T5PTR_DB pDB,
                              T5_BOOL bInitInstance)
{
    T5PTR_CODEFBINST pInst;
    T5PTR_DBFBCLASS pdClass;
    T5_WORD i, wNbClass;
    T5PTR_DBFBINST pdInst;
    T5PTR_DB pDef;
    T5_BOOL bFound;
    T5_PTBYTE pInstData;
    T5_PTCHAR pInstName;
#ifndef T5DEF_ACCEPTMISSINGHANDLER
    T5PTR_DBSTATUS pStatus;
#endif /*T5DEF_ACCEPTMISSINGHANDLER*/

#ifndef T5DEF_ACCEPTMISSINGHANDLER
    pStatus = T5GET_DBSTATUS(pDB);
#endif /*T5DEF_ACCEPTMISSINGHANDLER*/
    pDef = &(pDB[T5TB_FBINST]);
    pdInst = T5GET_DBFBINST(pDB);
    pdClass = T5GET_DBFBCLASS(pDB);
    wNbClass = pDB[T5TB_FBCLASS].wNbItemUsed;
    pInstData = (T5_PTBYTE)(pDB[T5TB_FBIDATA].pData);
    pInstName = T5VMCode_GetInstanceNames (pCode);
    /* index of instances is 1 based */
    pdInst++;
    /* scan instance list */
    pInst = (T5PTR_CODEFBINST)T5VMCode_GetInstanceList (pCode);
    pDef->wNbItemFixed = pDef->wNbItemUsed = 0;
    while (pInst->wID != 0)
    {
        /* find the class */
        for (i=0, bFound=FALSE; !bFound && i<wNbClass; i++)
        {
            if (pInst->wID == pdClass[i].wID)
            {
                /* set instance descriptor */
                pdInst->pClassData = pdClass[i].pData;
                pdInst->pfHandler = pdClass[i].pfHandler;
                pdInst->wClass = i;
                pdInst->wFlags = pdClass[i].wFlags;
                pdInst->dwCRC = pInst->dwCRC;
                /* give instance data */
                pdInst->pData = pInstData;
                pInstData += pdClass[i].dwInstSizeof;
                /* init instance */
                if (bInitInstance)
                {
                    pdInst->pfHandler (T5FBCMD_INITINSTANCE, pDB,
                            pdInst->pClassData, pdInst->pData, T5_PTNULL);
                    if (*pInstName != '\0')
                    {
                        pdInst->pfHandler (T5FBCMD_SETINSTNAME, pDB,
                                pdInst->pClassData, pdInst->pData, (T5_PTWORD)(&pInstName));
                    }
                    if (pdInst->pfHandler (T5FBCMD_WANTKA, pDB,
                            pdInst->pClassData, pdInst->pData, T5_PTNULL))
                    {
                        pdInst->wFlags |= T5FBI_WANTKA;
                    }
                }
                /* one more successful */
                pDef->wNbItemFixed += 1;
                pDef->wNbItemUsed += 1;
                bFound = TRUE;
            }
        }
        if (!bFound)
        {
            /* set instance with default null handler */
            pdInst->pClassData = T5_PTNULL;
            pdInst->pfHandler = T5VMFbl_null;
            pdInst->wClass = 0;
            pdInst->wFlags = 0; /*invalid*/
            pdInst->dwCRC = 0L;
            pdInst->pData = pInstData;
            /* one more instance */
            pDef->wNbItemFixed += 1;
            pDef->wNbItemUsed += 1;
#ifndef T5DEF_ACCEPTMISSINGHANDLER
            pStatus->wFlags |= T5FLAG_HNDMISS;
#endif /*T5DEF_ACCEPTMISSINGHANDLER*/
        }
        pInst++;
        pdInst++;
        if (*pInstName != '\0')
        {
            while (*pInstName != '\0')
                pInstName++;
            pInstName++;
        }
    }
    return T5RET_OK;
}

/*****************************************************************************
T5VMFbl_InitNewInstances
Initialize only new FB instances for On Line Change
Parameters:
    pDB (IN) application database
    bFirst (IN) index of the first new instance
*****************************************************************************/

void T5VMFbl_InitNewInstances (T5PTR_DB pDB, T5_WORD wFirst)
{
    T5PTR_DB pDef;
    T5PTR_DBFBINST pdInst;
    T5_WORD i;

    pDef = &(pDB[T5TB_FBINST]);
    pdInst = T5GET_DBFBINST(pDB);

    for (i=1; i<=wFirst && i <= pDef->wNbItemUsed; i++)
    {
        pdInst[i].pfHandler (T5FBCMD_HOTRESTART, pDB,
                           pdInst[i].pClassData, pdInst[i].pData, T5_PTNULL);
    }
    wFirst += 1;
    for (i=wFirst; i <= pDef->wNbItemUsed; i++)
        pdInst[i].pfHandler (T5FBCMD_INITINSTANCE, pDB,
                           pdInst[i].pClassData, pdInst[i].pData, T5_PTNULL);
}

/*****************************************************************************
T5VMFbl_Shutdown
Close all instances and classes
Parameters:
    pDB (IN) application database
return: OK or error
*****************************************************************************/

void T5VMFbl_ShutDown (T5PTR_DB pDB)
{
    T5PTR_DBFBCLASS pdClass;
    T5_WORD i, wNbClass, wNbInst;
    T5PTR_DBFBINST pdInst;

    pdClass = T5GET_DBFBCLASS(pDB);
    pdInst = T5GET_DBFBINST(pDB);
    wNbClass = pDB[T5TB_FBCLASS].wNbItemUsed;
    wNbInst = pDB[T5TB_FBINST].wNbItemUsed;
    /* close instances */
    for (i=1; i<=wNbInst; i++)
        pdInst[i].pfHandler (T5FBCMD_EXITINSTANCE, pDB,
                            pdInst[i].pClassData, pdInst[i].pData, T5_PTNULL);
    /* close classes */
    for (i=0; i<wNbClass; i++)
        pdClass[i].pfHandler (T5FBCMD_EXITCLASS, pDB, pdClass[i].pData,
                                T5_PTNULL, T5_PTNULL);
}

/*****************************************************************************
T5VMFbl_KeepAlive
Call FBIs for keep alive
Parameters:
    pDB (IN) application database
return: OK or error
*****************************************************************************/

void T5VMFbl_KeepAlive (T5PTR_DB pDB)
{
    T5_WORD i, wNbInst;
    T5PTR_DBFBINST pdInst;

    pdInst = T5GET_DBFBINST(pDB);
    wNbInst = pDB[T5TB_FBINST].wNbItemUsed;
    /* close instances */
    for (i=1; i<=wNbInst; i++)
    {
        if ((pdInst[i].wFlags & T5FBI_WANTKA) != 0)
            pdInst[i].pfHandler (T5FBCMD_KEEPALIVE, pDB,
                                pdInst[i].pClassData, pdInst[i].pData, T5_PTNULL);
    }
}

/*****************************************************************************
T5VMFbl_InitCustomFunctions
initialize custom function classes
Parameters:
    pCode (IN) pointer to the application code
    pDB (IN) application database
return: OK or error
*****************************************************************************/

extern T5_RET T5VMFbl_InitCustomFunctions (T5_PTR pCode, T5PTR_DB pDB)
{
    T5PTR_CODEFNCLASS pClass;
    T5_WORD i, wNbClass;
    T5PTR_DB pDef;
    T5PTR_DBFCLASS pDefClass;
    _str_find find;
    T5_BOOL bContinue;
#ifndef T5DEF_ACCEPTMISSINGHANDLER
    T5PTR_DBSTATUS pStatus;
#endif /*T5DEF_ACCEPTMISSINGHANDLER*/

#ifndef T5DEF_ACCEPTMISSINGHANDLER
    pStatus = T5GET_DBSTATUS(pDB);
#endif /*T5DEF_ACCEPTMISSINGHANDLER*/
    pClass = (T5PTR_CODEFNCLASS)T5VMCode_GetCustomFuncList (pCode, &wNbClass);
    pDef = &(pDB[T5TB_FCLASS]);
    pDefClass = T5GET_DBFCLASS(pDB);

    pDef->wNbItemFixed = 0;
    pDef->wNbItemUsed = 0;

    for (i=0; i<wNbClass && pDef->wNbItemUsed < (pDef->wNbItemAlloc - 1); i++)
    {
        T5_STRCPY(find.szName, pClass[i].szName);
        find.pfHandler = T5_PTNULL;
        bContinue = T5VMFbl_List (T5STFBMODE_FINDHANDLER, &find, FALSE);
        pDef->wNbItemFixed += 1;
        pDef->wNbItemUsed += 1;
        T5_STRCPY(pDefClass[pDef->wNbItemUsed].szName, pClass[i].szName);
        if (!bContinue && find.pfHandler != T5_PTNULL)
        {
            pDefClass[pDef->wNbItemUsed].pfHandler = find.pfHandler;
            pDefClass[pDef->wNbItemUsed].wFlags = T5FCLASS_VALID;
        }
        else
        {
            pDefClass[pDef->wNbItemUsed].pfHandler = T5VMFCust_null;
            pDefClass[pDef->wNbItemUsed].wFlags = 0; /* invalid */
            T5VMLog_RegisterUnresolved (pDB, (T5_PTCHAR)pClass[i].szName);
#ifndef T5DEF_ACCEPTMISSINGHANDLER
            pStatus->wFlags |= T5FLAG_HNDMISS;
#endif /*T5DEF_ACCEPTMISSINGHANDLER*/
        }
    }
    return T5RET_OK;
}

/*****************************************************************************
T5VMFbl_GetInstanceCount (tools for drivers)
get count of FB instances for a class
Parameters:
    pDB (IN) application database
    szClass (in) FB class name
return: number of declared instances
*****************************************************************************/

T5_WORD T5VMFbl_GetInstanceCount (T5PTR_DB pDB, T5_PTCHAR szClass)
{
    T5PTR_DBFBCLASS pClass;
    T5PTR_DBFBINST pInst;
    T5_WORD i, nb, wClass, wTotal;
    T5_BOOL bFound;

    pClass = T5GET_DBFBCLASS (pDB);
    nb = pDB[T5TB_FBCLASS].wNbItemUsed;
    bFound = FALSE;
    wClass = 0;
    for (i=0; !bFound && i<nb; i++)
    {
        if (pClass->szName != NULL && T5_STRCMP (pClass->szName, szClass) == 0)
            bFound = TRUE;
        else
        {
            pClass++;
            wClass++;
        }
    }
    if (!bFound)
        return 0;

    pInst = T5GET_DBFBINST (pDB);
    nb = pDB[T5TB_FBINST].wNbItemUsed;
    wTotal = 0;
    pInst++;
    for (i=1; i<=nb; i++, pInst++)
    {
        if (pInst->wClass == wClass)
            wTotal += 1;
    }
    return wTotal;
}

/* eof **********************************************************************/
