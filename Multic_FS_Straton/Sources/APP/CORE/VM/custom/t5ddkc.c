/*****************************************************************************
T5ddkc.c :   DDK - simplified C interface - core

DO NOT ALTER THIS !

(c) COPALP 2008
*****************************************************************************/

#include "t5vm.h"
#include "t5ddkc.h"

#ifdef T5DEF_DDKC

/****************************************************************************/

static T5_BOOL _T5DDKC_OPEN = FALSE;

/****************************************************************************/
/* dynalinks */

void T5DDKC_OpenLinks (void)
{
    T5DDKC_IpInit ();
}

void T5DDKC_CloseLinks (void)
{
    T5DDKC_IpExit ();
}

/****************************************************************************/
/* evaluate full size */

static _T5DDKC_EvalDrvRamSize (T5_PTR pBusDef, T5HDN_DDKC pfDrv)
{
    T5_DWORD dwSize, dwTotal;
    T5_PTBYTE pDef;
    T5_PTWORD pwDef;
    T5PTR_BDBUS pBus;
    T5_WORD wVersion;

    dwTotal = 0L;
    wVersion = 1;
    pDef = (T5_PTBYTE)pBusDef;
    pwDef = (T5_PTWORD)pDef;
    while (*pwDef != 0)
    {
        switch (pwDef[1])
        {
        case T5BD_BUS :
            pBus = (T5PTR_BDBUS)pDef;
            wVersion = pBus->wVersion;
            dwSize = pfDrv (T5DDKCH_GETNODESIZE, wVersion, NULL, 1, NULL, 0L,
                            pDef + sizeof (T5STR_BDBUS));
            dwTotal += (T5DDKC_ALIGN(dwSize) + sizeof (T5STR_DDKCNODE));
            break;
        case T5BD_MASTER :
            dwSize = pfDrv (T5DDKCH_GETNODESIZE, wVersion, NULL, 2, NULL, 0L,
                            pDef + sizeof (T5STR_BDMASTER));
            dwTotal += (T5DDKC_ALIGN(dwSize) + sizeof (T5STR_DDKCNODE));
            break;
        case T5BD_SLAVE :
            dwSize = pfDrv (T5DDKCH_GETNODESIZE, wVersion, NULL, 3, NULL, 0L,
                            pDef + sizeof (T5STR_BDSLAVE));
            dwTotal += (T5DDKC_ALIGN(dwSize) + sizeof (T5STR_DDKCNODE));
            break;
        case T5BD_VAR :
            dwSize = pfDrv (T5DDKCH_GETNODESIZE, wVersion, NULL, 4, NULL, 0L,
                            pDef + sizeof (T5STR_BDVAR));
            dwTotal += (T5DDKC_ALIGN(dwSize) + sizeof (T5STR_DDKCNODE));
            break;
        default : break;
        }
        pDef += *pwDef;
        pwDef = (T5_PTWORD)pDef;
    }
    return dwTotal;
}

T5_DWORD T5DDKC_GetSize (T5_PTR pCode)
{
    T5PTR_CODEBUSDRV pDef;
    T5HDN_DDKC pfDrv;
    T5_PTR pBusDef;
    T5_DWORD dwTotal;

    pDef = (T5PTR_CODEBUSDRV)T5VMCode_GetBusDrv (pCode);
    if (pDef == NULL)
        return 0;

    dwTotal = 0L;
    while (pDef->dwDef != 0L)
    {
        if (dwTotal == 0L)
            dwTotal = sizeof (T5STR_DDKCTOC);
        pBusDef = T5_OFF (pCode, pDef->dwDef);
        dwTotal += sizeof (T5STR_DDKCDRV);
        pfDrv = T5DDKC_IpGetHandler (pDef->szDllPrefix);
        if (pfDrv != NULL)
            dwTotal += _T5DDKC_EvalDrvRamSize (pBusDef, pfDrv);
        pDef++;
    }
    return dwTotal;
}

/****************************************************************************/
/* build / open */

static T5_DWORD _T5DDKC_BuildBus (T5PTR_DB pDB, T5PTR_DDKCDRV pEntry,
                                  T5_PTBYTE pBase, T5_PTR pBusDef)
{
    T5_DWORD dwTotal;
    T5_DWORD dwSize;
    T5_PTBYTE pDef;
    T5_PTWORD pwDef;
    T5PTR_DDKCNODE pBus, pMaster, pSlave, pVar;
    T5PTR_BDBUS pBDef;

    dwTotal = 0L;
    pBus = pMaster = pSlave = pVar = NULL;
    pDef = (T5_PTBYTE)pBusDef;
    pwDef = (T5_PTWORD)pDef;
    while (*pwDef != 0)
    {
        switch (pwDef[1])
        {
        case T5BD_BUS :
            pBDef = (T5PTR_BDBUS)pDef;
            pBus = (T5PTR_DDKCNODE)pBase;
            pMaster = pSlave = pVar = NULL;
            pEntry->pNodes = pBus;
            pEntry->wVersion = pBDef->wVersion;
            memset (pBus, 0, sizeof (T5STR_DDKCNODE));
            pBus->pDef = pDef;
            dwSize = pEntry->pfDriver (T5DDKCH_BUILDNODE,
                                       pEntry->wVersion, pDB, 1,
                                       pBus+1, 0L, NULL);
            dwSize = T5DDKC_ALIGN (dwSize);
            dwTotal += (dwSize + sizeof (T5STR_DDKCNODE));
            pBase += (dwSize + sizeof (T5STR_DDKCNODE));
            break;
        case T5BD_MASTER :
            if (pBus != NULL)
            {
                if (pMaster == NULL)
                    pBus->pChild = (T5PTR_DDKCNODE)pBase;
                else
                    pMaster->pSibling = (T5PTR_DDKCNODE)pBase;
                pMaster = (T5PTR_DDKCNODE)pBase;
                pSlave = pVar = NULL;
                memset (pMaster, 0, sizeof (T5STR_DDKCNODE));
                pMaster->pParent = pBus;
                pMaster->pDef = pDef;
                dwSize = pEntry->pfDriver (T5DDKCH_BUILDNODE,
                                           pEntry->wVersion, pDB, 2,
                                           pMaster+1, 0L, NULL);
                dwSize = T5DDKC_ALIGN (dwSize);
                dwTotal += (dwSize + sizeof (T5STR_DDKCNODE));
                pBase += (dwSize + sizeof (T5STR_DDKCNODE));
            }
            break;
        case T5BD_SLAVE :
            if (pMaster != NULL)
            {
                if (pSlave == NULL)
                    pMaster->pChild = (T5PTR_DDKCNODE)pBase;
                else
                    pSlave->pSibling = (T5PTR_DDKCNODE)pBase;
                pSlave = (T5PTR_DDKCNODE)pBase;
                pVar = NULL;
                memset (pSlave, 0, sizeof (T5STR_DDKCNODE));
                pSlave->pParent = pMaster;
                pSlave->pDef = pDef;
                dwSize = pEntry->pfDriver (T5DDKCH_BUILDNODE,
                                           pEntry->wVersion, pDB, 3,
                                           pSlave+1, 0L, NULL);
                dwSize = T5DDKC_ALIGN (dwSize);
                dwTotal += (dwSize + sizeof (T5STR_DDKCNODE));
                pBase += (dwSize + sizeof (T5STR_DDKCNODE));
            }
            break;
        case T5BD_VAR :
            if (pSlave != NULL)
            {
                if (pVar == NULL)
                    pSlave->pChild = (T5PTR_DDKCNODE)pBase;
                else
                    pVar->pSibling = (T5PTR_DDKCNODE)pBase;
                pVar = (T5PTR_DDKCNODE)pBase;
                memset (pVar, 0, sizeof (T5STR_DDKCNODE));
                pVar->pParent = pSlave;
                pVar->pDef = pDef;
                dwSize = pEntry->pfDriver (T5DDKCH_BUILDNODE,
                                           pEntry->wVersion, pDB, 4,
                                           pVar+1, 0L, NULL);
                dwSize = T5DDKC_ALIGN (dwSize);
                dwTotal += (dwSize + sizeof (T5STR_DDKCNODE));
                pBase += (dwSize + sizeof (T5STR_DDKCNODE));
            }
            break;
        default : break;
        }
        pDef += *pwDef;
        pwDef = (T5_PTWORD)pDef;
    }
    return dwTotal;
}

T5_RET T5DDKC_Build (T5PTR_DB pDB, T5_PTR pCode)
{
    T5PTR_CODEBUSDRV pDef;
    T5_WORD i, nbDrv;
    T5_PTBYTE pBase;
    T5PTR_DDKCTOC pToc;
    T5PTR_DDKCDRV pEntry;
    T5_PTR pBusDef;
    T5_RET wRet;
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    pDef = (T5PTR_CODEBUSDRV)T5VMCode_GetBusDrv (pCode);
    if (pDef == NULL)
        return T5RET_OK;

    nbDrv = 0;
    while (pDef[nbDrv].dwDef != 0L)
        nbDrv += 1;
    pBase = (T5_PTBYTE)(pDB[T5TB_DDKC].pData);

    if (nbDrv == 0)
        return T5RET_OK;

    /* main header */
    pToc = (T5PTR_DDKCTOC)pBase;
    pToc->wNb = nbDrv;
    pBase += sizeof (T5STR_DDKCTOC);

    /* driver entries */
    pEntry = (T5PTR_DDKCDRV)pBase;
    for (i=0; i<nbDrv; i++)
    {
        pEntry[i].pfDriver = T5DDKC_IpGetHandler (pDef[i].szDllPrefix);
        if (pEntry[i].pfDriver == NULL)
        {
            pStatus->wFlags |= T5FLAG_HNDMISS;
            T5VMLog_RegisterUnresolved (pDB, pDef[i].szDllPrefix);
            return T5RET_OK;
        }
        pEntry[i].szName = pDef[i].szDllPrefix;
    }
    pBase += (nbDrv * sizeof (T5STR_DDKCDRV));

    /* bus nodes */
    for (i=0; i<nbDrv; i++)
    {
        pBusDef = T5_OFF (pCode, pDef[i].dwDef);
        pBase += _T5DDKC_BuildBus (pDB, &(pEntry[i]), pBase, pBusDef);
        if (pEntry[i].pNodes == NULL)
            return T5RET_ERROR;
    }

    /* open */
    wRet = T5RET_OK;
    for (i=0; i<nbDrv; i++)
    {
        wRet |= pEntry[i].pfDriver (T5DDKCH_OPEN, pEntry[i].wVersion, pDB, 1,
                                (T5PTR_DDKCNODE)(pEntry[i].pNodes)+1, 0L, NULL);
    }
    _T5DDKC_OPEN = TRUE;

    /* close open drivers if one fails during open */
    if (wRet != T5RET_OK)
        T5DDKC_Close (pDB);

    return wRet;
}

/****************************************************************************/
/* hot start and On Line change */

T5_RET T5DDKC_CanBuildHot (T5PTR_DB pDB, T5_PTR pCode)
{
    T5_DWORD dwNewSize;

    dwNewSize = T5DDKC_GetSize (pCode);
    if (dwNewSize > pDB[T5TB_DDKC].dwRawSize)
        return T5RET_ERROR;
    return T5RET_OK;
}

T5_RET T5DDKC_BuildHot (T5PTR_DB pDB, T5_PTR pCode)
{
    if (_T5DDKC_OPEN)
        T5DDKC_Close (pDB);
    return T5DDKC_Build (pDB, pCode);
}

/****************************************************************************/
/* close */

void T5DDKC_Close (T5PTR_DB pDB)
{
    T5_WORD i;
    T5_PTBYTE pBase;
    T5PTR_DDKCTOC pToc;
    T5PTR_DDKCDRV pEntry;

    _T5DDKC_OPEN = FALSE;

    pBase = (T5_PTBYTE)(pDB[T5TB_DDKC].pData);
    if (pBase == NULL || pDB[T5TB_DDKC].dwRawSize == 0L)
        return;

    pToc = (T5PTR_DDKCTOC)pBase;
    pBase += sizeof (T5STR_DDKCTOC);
    pEntry = (T5PTR_DDKCDRV)pBase;

    for (i=0; i < pToc->wNb; i++)
    {
        if (pEntry[i].pfDriver != NULL)
        {
            pBase = (T5_PTBYTE)(pEntry[i].pNodes) + sizeof (T5STR_DDKCNODE);
            pEntry[i].pfDriver (T5DDKCH_CLOSE, pEntry[i].wVersion,
                                pDB, 1, pBase, 0L, NULL);
        }
    }
}

/****************************************************************************/
/* exchange */

void T5DDKC_Exchange (T5PTR_DB pDB, T5_DWORD dwTimeStamp)
{
    T5_WORD i;
    T5_PTBYTE pBase;
    T5PTR_DDKCTOC pToc;
    T5PTR_DDKCDRV pEntry;

    pBase = (T5_PTBYTE)(pDB[T5TB_DDKC].pData);
    if (pBase == NULL || pDB[T5TB_DDKC].dwRawSize == 0L)
        return;

    pToc = (T5PTR_DDKCTOC)pBase;
    pBase += sizeof (T5STR_DDKCTOC);
    pEntry = (T5PTR_DDKCDRV)pBase;

    for (i=0; i < pToc->wNb; i++)
    {
        if (pEntry[i].pfDriver != NULL)
        {
            pBase = (T5_PTBYTE)(pEntry[i].pNodes) + sizeof (T5STR_DDKCNODE);
            pEntry[i].pfDriver (T5DDKCH_EXCHANGE, pEntry[i].wVersion, pDB, 1,
                                pBase, dwTimeStamp, NULL);
        }
    }
}

/****************************************************************************/
/* helpers for the driver - find bus root node */

T5_PTR T5DDKH_FindBus (T5PTR_DB pDB, T5_PTCHAR szName)
{
    T5_WORD i;
    T5_PTBYTE pBase;
    T5PTR_DDKCTOC pToc;
    T5PTR_DDKCDRV pEntry;

    pBase = (T5_PTBYTE)(pDB[T5TB_DDKC].pData);
    if (pBase == NULL || pDB[T5TB_DDKC].dwRawSize == 0L)
        return NULL;

    pToc = (T5PTR_DDKCTOC)pBase;
    pBase += sizeof (T5STR_DDKCTOC);
    pEntry = (T5PTR_DDKCDRV)pBase;

    for (i=0; i < pToc->wNb; i++)
    {
        if (pEntry[i].pfDriver != NULL
            && T5_STRCMP (szName, pEntry[i].szName) == 0)
        {
            pBase = (T5_PTBYTE)(pEntry[i].pNodes) + sizeof (T5STR_DDKCNODE);
            return pBase;
        }
    }
    return NULL;
}

/****************************************************************************/
/* helpers for the driver - get pointers */

static T5PTR_DDKCNODE _T5DDKH_GetNodePtr (T5_PTR pNode)
{
    T5PTR_DDKCNODE pBase;

    if (pNode == NULL)
        return NULL;
    pBase = (T5PTR_DDKCNODE)pNode;
    pBase--;
    return pBase;
}

T5_PTR T5DDKH_GetDef (T5_PTR pNode)
{
    T5PTR_DDKCNODE pBase;

    pBase = _T5DDKH_GetNodePtr (pNode);
    if (pBase == NULL)
        return NULL;
    return pBase->pDef;
}

T5_PTBYTE T5DDKH_GetProps (T5_PTR pNode)
{
    T5PTR_DDKCNODE pBase;
    T5_PTBYTE pDef;

    pBase = _T5DDKH_GetNodePtr (pNode);
    if (pBase == NULL)
        return NULL;

    pDef = (T5_PTBYTE)(pBase->pDef);
    if (pDef == NULL)
        return NULL;

    switch (((T5PTR_BDP)pDef)->wKind)
    {
    case T5BD_BUS    : return (pDef + sizeof (T5STR_BDBUS));
    case T5BD_MASTER : return (pDef + sizeof (T5STR_BDMASTER));
    case T5BD_SLAVE  : return (pDef + sizeof (T5STR_BDSLAVE));
    case T5BD_VAR    : return (pDef + sizeof (T5STR_BDVAR));
    default : break;
    }
    return NULL;
}

T5_PTR T5DDKH_GetParent (T5_PTR pNode)
{
    T5PTR_DDKCNODE pBase;

    pBase = _T5DDKH_GetNodePtr (pNode);
    if (pBase == NULL)
        return NULL;
    if (pBase->pParent == NULL)
        return NULL;
    return (T5PTR_DDKCNODE)(pBase->pParent) + 1;
}

T5_PTR T5DDKH_GetChild (T5_PTR pNode)
{
    T5PTR_DDKCNODE pBase;

    pBase = _T5DDKH_GetNodePtr (pNode);
    if (pBase == NULL)
        return NULL;
    if (pBase->pChild == NULL)
        return NULL;
    return (T5PTR_DDKCNODE)(pBase->pChild) + 1;
}

T5_PTR T5DDKH_GetSibling (T5_PTR pNode)
{
    T5PTR_DDKCNODE pBase;

    pBase = _T5DDKH_GetNodePtr (pNode);
    if (pBase == NULL)
        return NULL;
    if (pBase->pSibling == NULL)
        return NULL;
    return (T5PTR_DDKCNODE)(pBase->pSibling) + 1;
}

/****************************************************************************/
/* helpers for the driver - VMDB read/write */

T5_PTBYTE _T5DDKH_GetData (T5PTR_DB pDB, T5PTR_BDVAR pDef, T5_PTBYTE pbLock)
{
    T5_PTBYTE pData;

    if (pDef->dwCTOffset != 0L)
    {
        pData = (T5_PTBYTE)pDB[T5TB_CTSEG].pData;
        pData += pDef->dwCTOffset;
        if (pbLock != NULL)
            *pbLock = FALSE;
    }
    else
    {
        pData = (T5_PTBYTE)T5Tools_GetAnyParam (pDB, pDef->wType,
                                        pDef->wIndex, pbLock, NULL);
    }
    return pData;
}

T5_BOOL T5DDKH_ReadVarBool (T5PTR_DB pDB, T5_PTR pNode)
{
    T5PTR_BDVAR pDef;
    T5_PTBYTE pData;
    T5_BOOL bValue;

    pDef = T5DDKH_GetDefVar (pNode);
    if (pDef == NULL || pDef->wKind != T5BD_VAR)
        return FALSE;

    pData = _T5DDKH_GetData (pDB, pDef, NULL);
    if (pData == NULL)
        return FALSE;

    T5VMCnv_ConvBin (&bValue, pData, TIC_BOO, (T5_BYTE)(pDef->wType));
    return bValue;
}

T5_LONG T5DDKH_ReadVarLong (T5PTR_DB pDB, T5_PTR pNode)
{
    T5PTR_BDVAR pDef;
    T5_PTBYTE pData;
    T5_LONG lValue;

    pDef = T5DDKH_GetDefVar (pNode);
    if (pDef == NULL || pDef->wKind != T5BD_VAR)
        return FALSE;

    pData = _T5DDKH_GetData (pDB, pDef, NULL);
    if (pData == NULL)
        return FALSE;

    T5VMCnv_ConvBin (&lValue, pData, TIC_ANA, (T5_BYTE)(pDef->wType));
    return lValue;
}

T5_DWORD T5DDKH_ReadVarDword (T5PTR_DB pDB, T5_PTR pNode)
{
    T5PTR_BDVAR pDef;
    T5_PTBYTE pData;
    T5_DWORD dwValue;

    pDef = T5DDKH_GetDefVar (pNode);
    if (pDef == NULL || pDef->wKind != T5BD_VAR)
        return FALSE;

    pData = _T5DDKH_GetData (pDB, pDef, NULL);
    if (pData == NULL)
        return FALSE;

    T5VMCnv_ConvBin (&dwValue, pData, TIC_UDINT, (T5_BYTE)(pDef->wType));
    return dwValue;
}

#ifdef T5DEF_REALSUPPORTED

T5_REAL T5DDKH_ReadVarReal (T5PTR_DB pDB, T5_PTR pNode)
{
    T5PTR_BDVAR pDef;
    T5_PTBYTE pData;
    T5_REAL rValue;

    pDef = T5DDKH_GetDefVar (pNode);
    if (pDef == NULL || pDef->wKind != T5BD_VAR)
        return FALSE;

    pData = _T5DDKH_GetData (pDB, pDef, NULL);
    if (pData == NULL)
        return FALSE;

    T5VMCnv_ConvBin (&rValue, pData, TIC_REA, (T5_BYTE)(pDef->wType));
    return rValue;
}

#endif /*T5DEF_REALSUPPORTED*/

void T5DDKH_WriteVarBool (T5PTR_DB pDB, T5_PTR pNode, T5_BOOL bValue)
{
    T5PTR_BDVAR pDef;
    T5_PTBYTE pData;
    T5_BOOL bLock;

    pDef = T5DDKH_GetDefVar (pNode);
    if (pDef == NULL || pDef->wKind != T5BD_VAR)
        return;

    pData = _T5DDKH_GetData (pDB, pDef, &bLock);
    if (pData == NULL || bLock)
        return;

    T5VMCnv_ConvBin (pData, &bValue, (T5_BYTE)(pDef->wType), TIC_BOO);
}

void T5DDKH_WriteVarLong (T5PTR_DB pDB, T5_PTR pNode, T5_LONG lValue)
{
    T5PTR_BDVAR pDef;
    T5_PTBYTE pData;
    T5_BOOL bLock;

    pDef = T5DDKH_GetDefVar (pNode);
    if (pDef == NULL || pDef->wKind != T5BD_VAR)
        return;

    pData = _T5DDKH_GetData (pDB, pDef, &bLock);
    if (pData == NULL || bLock)
        return;

    T5VMCnv_ConvBin (pData, &lValue, (T5_BYTE)(pDef->wType), TIC_ANA);
}

void T5DDKH_WriteVarDword (T5PTR_DB pDB, T5_PTR pNode, T5_DWORD dwValue)
{
    T5PTR_BDVAR pDef;
    T5_PTBYTE pData;
    T5_BOOL bLock;

    pDef = T5DDKH_GetDefVar (pNode);
    if (pDef == NULL || pDef->wKind != T5BD_VAR)
        return;

    pData = _T5DDKH_GetData (pDB, pDef, &bLock);
    if (pData == NULL || bLock)
        return;

    T5VMCnv_ConvBin (pData, &dwValue, (T5_BYTE)(pDef->wType), TIC_UDINT);
}


#ifdef T5DEF_REALSUPPORTED

void T5DDKH_WriteVarReal (T5PTR_DB pDB, T5_PTR pNode, T5_REAL rValue)
{
    T5PTR_BDVAR pDef;
    T5_PTBYTE pData;
    T5_BOOL bLock;

    pDef = T5DDKH_GetDefVar (pNode);
    if (pDef == NULL || pDef->wKind != T5BD_VAR)
        return;

    pData = _T5DDKH_GetData (pDB, pDef, &bLock);
    if (pData == NULL || bLock)
        return;

    T5VMCnv_ConvBin (pData, &rValue, (T5_BYTE)(pDef->wType), TIC_REA);
}

#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/

#endif /*T5DEF_DDKC*/

/* eof **********************************************************************/
