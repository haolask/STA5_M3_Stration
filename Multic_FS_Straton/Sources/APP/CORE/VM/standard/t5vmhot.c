/*****************************************************************************
T5VMhot.c :  manage variable buffers during Hot Restart

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/

#ifdef T5DEF_HOTRESTART

/* static services **********************************************************/

static T5_BOOL _T5VMHot_PrcMapData8 (T5_PTR pCode, T5PTR_DB pDB,
                                     T5_BOOL bResetNew, T5_RET *pRet);
static T5_BOOL _T5VMHot_PrcMapData16 (T5_PTR pCode, T5PTR_DB pDB,
                                      T5_BOOL bResetNew, T5_RET *pRet);
static T5_BOOL _T5VMHot_PrcMapData32 (T5_PTR pCode, T5PTR_DB pDB,
                                      T5_BOOL bResetNew, T5_RET *pRet);
#ifdef T5DEF_DATA64SUPPORTED
static T5_BOOL _T5VMHot_PrcMapData64 (T5_PTR pCode, T5PTR_DB pDB,
                                      T5_BOOL bResetNew, T5_RET *pRet);
#endif /*T5DEF_DATA64SUPPORTED*/
static T5_BOOL _T5VMHot_PrcMapTime (T5_PTR pCode, T5PTR_DB pDB,
                                    T5_BOOL bResetNew, T5_RET *pRet);
#ifdef T5DEF_STRINGSUPPORTED
static T5_BOOL _T5VMHot_PrcMapString (T5_PTR pCode, T5PTR_DB pDB,
                                      T5_BOOL bResetNew, T5_RET *pRet);
#endif /*T5DEF_STRINGSUPPORTED*/
static T5_BOOL _T5VMHot_PrcMapInstances (T5_PTR pCode, T5PTR_DB pDB,
                                         T5_RET *pRet);
static T5_BOOL _T5VMHot_PrcMapCFunc (T5_PTR pCode, T5PTR_DB pDB,
                                     T5_RET *pRet);
static T5_BOOL _T5VMHot_PrcMapPrograms (T5_PTR pCode, T5PTR_DB pDB,
                                        T5_RET *pRet);
#ifdef T5DEF_XV
static T5_BOOL _T5VMHot_PrcMapXV (T5_PTR pCode, T5PTR_DB pDB, T5_RET *pRet);
#endif /*T5DEF_XV*/

/*****************************************************************************
T5VMHot_PrcVarMap
Check and update the variable data map during hot restart
Parameters:
    pCode (IN) application code
    pDB (IN) full VM data map
    bResetNew (IN) if TRUE: update of the data map required
    pRet (OUT) error code
Return: TRUE if ok
*****************************************************************************/

T5_BOOL T5VMHot_PrcVarMap (T5_PTR pCode, T5PTR_DB pDB, T5_BOOL bResetNew,
                           T5_RET *pRet)
{
    *pRet = T5RET_OK;
    if (!_T5VMHot_PrcMapData8 (pCode, pDB, bResetNew, pRet))
        return FALSE;
    if (!_T5VMHot_PrcMapData16 (pCode, pDB, bResetNew, pRet))
        return FALSE;
    if (!_T5VMHot_PrcMapData32 (pCode, pDB, bResetNew, pRet))
        return FALSE;
#ifdef T5DEF_DATA64SUPPORTED
    if (!_T5VMHot_PrcMapData64 (pCode, pDB, bResetNew, pRet))
        return FALSE;
#endif /*T5DEF_DATA64SUPPORTED*/
    if (!_T5VMHot_PrcMapTime (pCode, pDB, bResetNew, pRet))
        return FALSE;
#ifdef T5DEF_STRINGSUPPORTED
    if (!_T5VMHot_PrcMapString (pCode, pDB, bResetNew, pRet))
        return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
    if (!bResetNew && !_T5VMHot_PrcMapInstances (pCode, pDB, pRet))
        return FALSE;
    if (!bResetNew && !_T5VMHot_PrcMapCFunc (pCode, pDB, pRet))
        return FALSE;
    if (!bResetNew && !_T5VMHot_PrcMapPrograms (pCode, pDB, pRet))
        return FALSE;
#ifdef T5DEF_XV
    if (!_T5VMHot_PrcMapXV (pCode, pDB, pRet))
        return FALSE;
#endif /*T5DEF_XV*/
    return TRUE;
}

/*****************************************************************************
_T5VMHot_PrcMapData8
Check and update the variable map during hot restart: DATA8 variables
Parameters:
    pCode (IN) application code
    pDB (IN) full VM data map
    bResetNew (IN) if TRUE: update of the data map required
    pRet (OUT) error code
Return: TRUE if ok
*****************************************************************************/

static T5_BOOL _T5VMHot_PrcMapData8 (T5_PTR pCode, T5PTR_DB pDB,
                                     T5_BOOL bResetNew, T5_RET *pRet)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_WORD wSize;
    T5_PTBYTE ptr;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);

    if ((pVarCount->wNbD8Alloc + 1) != pDB[T5TB_DATA8].wNbItemAlloc)
    {
        *pRet = T5RET_HOTALLOC8;
        return FALSE;
    }

    if ((pVarCount->wNbD8Fixed + 1) < pDB[T5TB_DATA8].wNbItemFixed)
    {
        *pRet = T5RET_HOTFIXED8;
        return FALSE;
    }

    if (bResetNew)
    {
        wSize = (pVarCount->wNbD8Fixed + 1) - pDB[T5TB_DATA8].wNbItemFixed;
        ptr = T5GET_DBDATA8(pDB);
        ptr += pDB[T5TB_DATA8].wNbItemFixed;
        T5_MEMSET(ptr,0,wSize);

        pDB[T5TB_DATA8].wNbItemFixed = pVarCount->wNbD8Fixed + 1;
        pDB[T5TB_DATA8].wNbItemUsed = pVarCount->wNbD8Used + 1;
    }

    return TRUE;
}

/*****************************************************************************
_T5VMHot_PrcMapData16
Check and update the variable map during hot restart: DATA16 variables
Parameters:
    pCode (IN) application code
    pDB (IN) full VM data map
    bResetNew (IN) if TRUE: update of the data map required
    pRet (OUT) error code
Return: TRUE if ok
*****************************************************************************/

static T5_BOOL _T5VMHot_PrcMapData16 (T5_PTR pCode, T5PTR_DB pDB,
                                      T5_BOOL bResetNew, T5_RET *pRet)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_WORD wSize;
    T5_PTWORD ptr;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);

    if ((pVarCount->wNbD16Alloc + 1) != pDB[T5TB_DATA16].wNbItemAlloc)
    {
        *pRet = T5RET_HOTALLOC16;
        return FALSE;
    }

    if ((pVarCount->wNbD16Fixed + 1) < pDB[T5TB_DATA16].wNbItemFixed)
    {
        *pRet = T5RET_HOTFIXED16;
        return FALSE;
    }

    if (bResetNew)
    {
        wSize = (pVarCount->wNbD16Fixed + 1) - pDB[T5TB_DATA16].wNbItemFixed;
        wSize *= sizeof (T5_WORD);
        ptr = T5GET_DBDATA16(pDB);
        ptr += pDB[T5TB_DATA16].wNbItemFixed;
        T5_MEMSET(ptr,0,wSize);

        pDB[T5TB_DATA16].wNbItemFixed = pVarCount->wNbD16Fixed + 1;
        pDB[T5TB_DATA16].wNbItemUsed = pVarCount->wNbD16Used + 1;
    }

    return TRUE;
}

/*****************************************************************************
_T5VMHot_PrcMapData32
Check and update the variable map during hot restart: DATA32 variables
Parameters:
    pCode (IN) application code
    pDB (IN) full VM data map
    bResetNew (IN) if TRUE: update of the data map required
    pRet (OUT) error code
Return: TRUE if ok
*****************************************************************************/

static T5_BOOL _T5VMHot_PrcMapData32 (T5_PTR pCode, T5PTR_DB pDB,
                                      T5_BOOL bResetNew, T5_RET *pRet)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_WORD wSize;
    T5_PTDWORD ptr;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);

    if ((pVarCount->wNbD32Alloc + 1) != pDB[T5TB_DATA32].wNbItemAlloc)
    {
        *pRet = T5RET_HOTALLOC32;
        return FALSE;
    }

    if ((pVarCount->wNbD32Fixed + 1) < pDB[T5TB_DATA32].wNbItemFixed)
    {
        *pRet = T5RET_HOTFIXED32;
        return FALSE;
    }

    if (bResetNew)
    {
        wSize = (pVarCount->wNbD32Fixed + 1) - pDB[T5TB_DATA32].wNbItemFixed;
        wSize *= sizeof (T5_DWORD);
        ptr = T5GET_DBDATA32(pDB);
        ptr += pDB[T5TB_DATA32].wNbItemFixed;
        T5_MEMSET(ptr,0,wSize);

        pDB[T5TB_DATA32].wNbItemFixed = pVarCount->wNbD32Fixed + 1;
        pDB[T5TB_DATA32].wNbItemUsed = pVarCount->wNbD32Used + 1;
    }

    return TRUE;
}

/*****************************************************************************
_T5VMHot_PrcMapData64
Check and update the variable map during hot restart: DATA64 variables
Parameters:
    pCode (IN) application code
    pDB (IN) full VM data map
    bResetNew (IN) if TRUE: update of the data map required
    pRet (OUT) error code
Return: TRUE if ok
*****************************************************************************/

#ifdef T5DEF_DATA64SUPPORTED

static T5_BOOL _T5VMHot_PrcMapData64 (T5_PTR pCode, T5PTR_DB pDB,
                                      T5_BOOL bResetNew, T5_RET *pRet)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_WORD wSize;
    T5_PTDATA64 ptr;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);

    if ((pVarCount->wNbD64Alloc + 1) != pDB[T5TB_DATA64].wNbItemAlloc)
    {
        *pRet = T5RET_HOTALLOC64;
        return FALSE;
    }

    if ((pVarCount->wNbD64Fixed + 1) < pDB[T5TB_DATA64].wNbItemFixed)
    {
        *pRet = T5RET_HOTFIXED64;
        return FALSE;
    }

    if (bResetNew)
    {
        wSize = (pVarCount->wNbD64Fixed + 1) - pDB[T5TB_DATA64].wNbItemFixed;
        wSize *= sizeof (T5_DATA64);
        ptr = T5GET_DBDATA64(pDB);
        ptr += pDB[T5TB_DATA64].wNbItemFixed;
        T5_MEMSET(ptr,0,wSize);

        pDB[T5TB_DATA64].wNbItemFixed = pVarCount->wNbD64Fixed + 1;
        pDB[T5TB_DATA64].wNbItemUsed = pVarCount->wNbD64Used + 1;
    }

    return TRUE;
}

#endif /*T5DEF_DATA64SUPPORTED*/

/*****************************************************************************
_T5VMHot_PrcMapTime
Check and update the variable map during hot restart: TIME variables
Parameters:
    pCode (IN) application code
    pDB (IN) full VM data map
    bResetNew (IN) if TRUE: update of the data map required
    pRet (OUT) error code
Return: TRUE if ok
*****************************************************************************/

static T5_BOOL _T5VMHot_PrcMapTime (T5_PTR pCode, T5PTR_DB pDB,
                                    T5_BOOL bResetNew, T5_RET *pRet)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_WORD wSize;
    T5_PTDWORD ptr;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);

    if ((pVarCount->wNbTimeAlloc + 1) != pDB[T5TB_TIME].wNbItemAlloc)
    {
        *pRet = T5RET_HOTALLOCT;
        return FALSE;
    }

    if ((pVarCount->wNbTimeFixed + 1) < pDB[T5TB_TIME].wNbItemFixed)
    {
        *pRet = T5RET_HOTFIXEDT;
        return FALSE;
    }

    if (bResetNew)
    {
        wSize = (pVarCount->wNbTimeFixed + 1) - pDB[T5TB_TIME].wNbItemFixed;
        wSize *= sizeof (T5_DWORD);
        ptr = T5GET_DBTIME(pDB);
        ptr += pDB[T5TB_TIME].wNbItemFixed;
        T5_MEMSET(ptr,0,wSize);

        pDB[T5TB_TIME].wNbItemFixed = pVarCount->wNbTimeFixed + 1;
        pDB[T5TB_TIME].wNbItemUsed = pVarCount->wNbTimeUsed + 1;
    }

    return TRUE;
}

#ifdef T5DEF_STRINGSUPPORTED

/*****************************************************************************
_T5VMHot_PrcMapString
Check and update the variable map during hot restart: STRING variables
Parameters:
    pCode (IN) application code
    pDB (IN) full VM data map
    bResetNew (IN) if TRUE: update of the data map required
    pRet (OUT) error code
Return: TRUE if ok
*****************************************************************************/

static T5_BOOL _T5VMHot_PrcMapString (T5_PTR pCode, T5PTR_DB pDB,
                                      T5_BOOL bResetNew, T5_RET *pRet)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5PTR_CODEINIT pInit;
    T5_WORD i, wFixedBefore;
    T5_BOOL bOK;
    T5_PTBYTE *pString;
    T5_PTBYTE pLen, pBuf;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pInit = T5GET_CODEINIT(pHeader);
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    pLen = T5GET_CODESTRLEN(pHeader, pInit);

    /* check number of variables */
    if ((pVarCount->wNbStringAlloc + 1) != pDB[T5TB_STRING].wNbItemAlloc)
    {
        *pRet = T5RET_HOTALLOCS;
        return FALSE;
    }

    if ((pVarCount->wNbStringFixed + 1) < pDB[T5TB_STRING].wNbItemFixed)
    {
        *pRet = T5RET_HOTFIXEDS;
        return FALSE;
    }

    if (pVarCount->dwStringBuf > pDB[T5TB_STRBUF].dwRawSize)
    {
        *pRet = T5RET_HOTSTRBUF;
        return FALSE;
    }

    /* check string length */
    wFixedBefore = pDB[T5TB_STRING].wNbItemFixed;
    bOK = TRUE;
    pBuf = T5GET_DBSTRBUF(pDB);
    for (i=1; bOK && i<wFixedBefore; i++)
    {
        if (*pBuf != *pLen) bOK = FALSE;
        pBuf += (((T5_WORD)(*pLen) & 0xff) + 3);
        pLen++;
    }
    if (!bOK)
    {
        *pRet = T5RET_HOTSTRLEN;
        return FALSE;
    }

    /* set new variables */
    if (bResetNew)
    {

        pDB[T5TB_STRING].wNbItemFixed = pVarCount->wNbStringFixed + 1;
        pDB[T5TB_STRING].wNbItemUsed = pVarCount->wNbStringUsed + 1;

        pString = T5GET_DBSTRING(pDB);
        pLen = T5GET_CODESTRLEN(pHeader, pInit);
        pLen += (wFixedBefore-1);
        for (i=wFixedBefore; i<pDB[T5TB_STRING].wNbItemUsed; i++)
        {
            pString[i] = pBuf;
            pBuf[0] = *pLen; /* max size */
            pBuf[1] = pBuf[2] = 0;
            pBuf += (((T5_WORD)(*pLen) & 0xff) + 3);
            pLen++;
        }
    }
    return TRUE;
}

/*****************************************************************************
_T5VMHot_PrcMapInstances
Check and update the variable map during hot restart: FB instances
Parameters:
    pCode (IN) application code
    pDB (IN) full VM data map
    pRet (OUT) error code
Return: TRUE if ok
*****************************************************************************/

#endif /*T5DEF_STRINGSUPPORTED*/

static T5_BOOL _T5VMHot_PrcMapInstances (T5_PTR pCode, T5PTR_DB pDB, T5_RET *pRet)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEFBI pDef;
    T5PTR_CODEFBINST pInst;
    T5PTR_DBFBINST pdInst;
    T5_WORD i, wFixedBefore;
    T5_BOOL bOK;
    T5_DWORD dwNewSize;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pDef = T5GET_CODEFBI(pHeader);

    /* check limits */
    if ((pDef->wNbAlloc + 1) != pDB[T5TB_FBINST].wNbItemAlloc)
    {
        *pRet = T5RET_HOTALLOCFBI;
        return FALSE;
    }

    if ((pDef->wNbFixed + 1) < pDB[T5TB_FBINST].wNbItemFixed)
    {
        *pRet = T5RET_HOTFIXEDFBI;
        return FALSE;
    }

    if (pDef->dwRawSize > pDB[T5TB_FBIDATA].dwRawSize)
        return FALSE;

    dwNewSize = T5VMFbl_GetInstanceDataSize (pCode);
    if (dwNewSize > pDB[T5TB_FBIDATA].dwRawSize)
    {
        *pRet = T5RET_HOTFBIBUF;
        return FALSE;
    }

    /* check existing FB instances */
    wFixedBefore = pDB[T5TB_FBINST].wNbItemFixed;
    bOK = TRUE;
    pInst = &(pDef->cfFirstInstance);
    pdInst = T5GET_DBFBINST(pDB);
    for (i=1; bOK && i<=wFixedBefore; i++)
    {
        if (pdInst[i].dwCRC != pInst->dwCRC)
            bOK = FALSE;
        pInst++;
    }        
    if (!bOK)
    {
        *pRet = T5RET_HOTFBI;
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************
_T5VMHot_PrcMapCFunc
Check and update the variable map during hot restart: custom C functions
Parameters:
    pCode (IN) application code
    pDB (IN) full VM data map
    pRet (OUT) error code
Return: TRUE if ok
*****************************************************************************/

static T5_BOOL _T5VMHot_PrcMapCFunc (T5_PTR pCode, T5PTR_DB pDB, T5_RET *pRet)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEFNC pDef;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pDef = T5GET_CODEFNC(pHeader);

    /* check limits */
    if ((pDef->wNbAlloc + 1) != pDB[T5TB_FCLASS].wNbItemAlloc)
    {
        *pRet = T5RET_HOTALLOCCFC;
        return FALSE;
    }

    return  TRUE;
}

/*****************************************************************************
_T5VMHot_PrcMapPrograms
Check the list of programs
Parameters:
    pCode (IN) application code
    pDB (IN) full VM data map
    pRet (OUT) error code
Return: TRUE if ok
*****************************************************************************/

static T5_BOOL _T5VMHot_PrcMapPrograms (T5_PTR pCode, T5PTR_DB pDB, T5_RET *pRet)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_DBPROG pPrgCode;
    T5PTR_DB pDef;
    T5PTR_CODEPRG pPrg;
    T5_WORD wNb;

    /* get pointer to TIC code sequences in app code */
    pHeader = (T5PTR_CODEHEADER)pCode;
    pPrg = T5GET_CODEPRG(pHeader);
    /* get list of code pointers in data base */
    pDef = &pDB[T5TB_PROG];
    pPrgCode = (T5PTR_DBPROG)(pDef->pData);
    wNb = 1; /* index is 1 based! */
    /* count and check styles */
    while (wNb < pDef->wNbItemAlloc && pPrg->dwCode != 0)
    {
        if ((pPrgCode[wNb].wStyle == T5PRGSTYLE_SFC || pPrgCode[wNb].wStyle == T5PRGSTYLE_CHILD)
            && pPrgCode[wNb].wStyle != pPrg->wStyle)
        {
            *pRet = T5RET_HOTPRGSTYLE;
            return FALSE;
        }
        wNb++;
        pPrg++;
    }
    /* check program count */
    if (/*wNb != (pDef->wNbItemUsed + 1) ||*/ pPrg->dwCode != 0)
    {
        *pRet = T5RET_HOTPRGNB;
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************
_T5VMHot_PrcMapXV
Check and update the variable map during hot restart: DATA8 variables
Parameters:
    pCode (IN) application code
    pDB (IN) full VM data map
    bResetNew (IN) if TRUE: update of the data map required
    pRet (OUT) error code
Return: TRUE if ok
*****************************************************************************/

#ifdef T5DEF_XV

static T5_BOOL _T5VMHot_PrcMapXV (T5_PTR pCode, T5PTR_DB pDB, T5_RET *pRet)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);

    if ((pVarCount->wNbXVAlloc + 1) != pDB[T5TB_XV].wNbItemAlloc)
    {
        *pRet = T5RET_HOTALLOCXV;
        return FALSE;
    }

    if ((pVarCount->wNbXVUsed + 1) < pDB[T5TB_XV].wNbItemFixed)
    {
        *pRet = T5RET_HOTFIXEDXV;
        return FALSE;
    }

    return TRUE;
}

#endif /*T5DEF_XV*/

/*****************************************************************************
T5VMHot_UndoVarMap
undo an On Line Change of the variable map
Parameters:
    pCode (IN) new application code
    pDB (IN) full VM data map
Return: TRUE if ok
*****************************************************************************/

T5_BOOL T5VMHot_UndoVarMap (T5_PTR pCode, T5PTR_DB pDB)
{
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEVARCOUNT pVarCount;
    T5_RET ret;

    pHeader = (T5PTR_CODEHEADER)pCode;
    pVarCount = T5GET_CODEVARCOUNT(pHeader);
    /* data8 */
    pDB[T5TB_DATA8].wNbItemFixed = pVarCount->wNbD8Fixed + 1;
    pDB[T5TB_DATA8].wNbItemUsed = pVarCount->wNbD8Used + 1;
    /* data16 */
    pDB[T5TB_DATA16].wNbItemFixed = pVarCount->wNbD16Fixed + 1;
    pDB[T5TB_DATA16].wNbItemUsed = pVarCount->wNbD16Used + 1;
    /* data32 */
    pDB[T5TB_DATA32].wNbItemFixed = pVarCount->wNbD32Fixed + 1;
    pDB[T5TB_DATA32].wNbItemUsed = pVarCount->wNbD32Used + 1;
#ifdef T5DEF_DATA64SUPPORTED
    /* data64 */
    pDB[T5TB_DATA64].wNbItemFixed = pVarCount->wNbD64Fixed + 1;
    pDB[T5TB_DATA64].wNbItemUsed = pVarCount->wNbD64Used + 1;
#endif /*T5DEF_DATA64SUPPORTED*/
    /* time */
    pDB[T5TB_TIME].wNbItemFixed = pVarCount->wNbTimeFixed + 1;
    pDB[T5TB_TIME].wNbItemUsed = pVarCount->wNbTimeUsed + 1;
#ifdef T5DEF_STRINGSUPPORTED
    /* string */
    pDB[T5TB_STRING].wNbItemFixed = pVarCount->wNbStringFixed + 1;
    pDB[T5TB_STRING].wNbItemUsed = pVarCount->wNbStringUsed + 1;
#endif /*T5DEF_STRINGSUPPORTED*/

    return _T5VMHot_PrcMapPrograms (pCode, pDB, &ret);
}

/****************************************************************************/

#endif /*T5DEF_HOTRESTART*/

/* eof **********************************************************************/

