/*****************************************************************************
T5MM.c :     memory manager

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/

void T5MM_Open (T5PTR_MM pMM, T5_PTR pConf)
{
    T5_MEMSET (pMM, 0, sizeof (T5STR_MM));

    pMM->pConf = pConf;
    pMM->mmbCode.wID = T5MM_CODE;
    pMM->mmbVmDb.wID = T5MM_VMDB;
    pMM->mmbChange.wID = T5MM_CHANGE;
}

void T5MM_Close (T5PTR_MM pMM)
{
    T5Memory_Free (&(pMM->mmbCode), pMM->pConf);
    T5Memory_Free (&(pMM->mmbVmDb), pMM->pConf);
}

/****************************************************************************/

T5_RET T5MM_LoadCode (T5PTR_MM pMM, T5_PTCHAR szAppName)
{
    return T5Memory_Load (&(pMM->mmbCode), szAppName, pMM->pConf);
}

T5_RET T5MM_FreeCode (T5PTR_MM pMM)
{
    return T5Memory_Free (&(pMM->mmbCode), pMM->pConf);
}

T5_PTR T5MM_LinkCode (T5PTR_MM pMM)
{
    if (T5Memory_Link (&(pMM->mmbCode), pMM->pConf) != T5RET_OK)
        return T5_PTNULL;
    return pMM->mmbCode.pData;
}

void T5MM_UnlinkCode (T5PTR_MM pMM)
{
    T5Memory_Unlink (&(pMM->mmbCode), pMM->pConf);
}

/****************************************************************************/

#ifdef T5DEF_ONLINECHANGE

T5_RET T5MM_LoadChange (T5PTR_MM pMM, T5_PTCHAR szAppName)
{
    return T5Memory_Load (&(pMM->mmbChange), szAppName, pMM->pConf);
}

T5_RET T5MM_FreeChange (T5PTR_MM pMM)
{
    return T5Memory_Free (&(pMM->mmbChange), pMM->pConf);
}

T5_PTR T5MM_LinkChange (T5PTR_MM pMM)
{
    if (T5Memory_Link (&(pMM->mmbChange), pMM->pConf) != T5RET_OK)
        return T5_PTNULL;
    return pMM->mmbChange.pData;
}

void T5MM_UnlinkChange (T5PTR_MM pMM)
{
    T5Memory_Unlink (&(pMM->mmbChange), pMM->pConf);
}

T5_RET T5MM_RegisterChange (T5PTR_MM pMM)
{
    /* changes must be loaded */
    if ((pMM->mmbChange.wFlags & T5MMB_LOADED) == 0)
        return T5RET_ERROR;
    /* original code must have nobody linked to */
    if ((pMM->mmbCode.wFlags & T5MMB_LINKED) != 0)
        return T5RET_ERROR;
    /* free original code */
    T5Memory_Free (&(pMM->mmbCode), pMM->pConf);
    /* replace original code with change */
    T5_MEMCPY(&(pMM->mmbCode), &(pMM->mmbChange), sizeof (T5STR_MMB));
    pMM->mmbCode.wID = T5MM_CODE;
    pMM->mmbCode.wChanged = 1;
    /* reset change descriptor */
    T5_MEMSET(&(pMM->mmbChange), 0, sizeof (T5STR_MMB));
    pMM->mmbChange.wID = T5MM_CHANGE;
    return T5RET_OK;
}

T5_RET T5MM_SaveChange (T5PTR_MM pMM, T5_PTCHAR szAppName)
{
    if (pMM->mmbCode.wChanged)
        return T5Memory_Save (&(pMM->mmbCode), szAppName, pMM->pConf);
    return T5RET_OK;
}

T5_RET T5MM_SaveChangeA (T5PTR_MM pMM, T5_PTCHAR szAppName)
{
    /* if szAppName is not NULL, save even if no change */
    if (pMM->mmbCode.wChanged || strlen(szAppName) > 0)
        return T5Memory_Save (&(pMM->mmbCode), szAppName, pMM->pConf);
    return T5RET_OK;
}

#endif /*T5DEF_ONLINECHANGE*/

/****************************************************************************/

T5_RET T5MM_WriteCodeRequest (T5PTR_MM pMM, T5_WORD wRequest,
                              T5PTR_CSLOAD pLoad)
{
    switch (wRequest)
    {
    case T5CSRQ_BEGINLOAD :
        return T5Memory_StartWrite (&(pMM->mmbCode), pLoad, pMM->pConf);
    case T5CSRQ_LOAD :
        return T5Memory_Write (&(pMM->mmbCode), pLoad, pMM->pConf);
    case T5CSRQ_ENDLOAD :
        return T5Memory_EndWrite (&(pMM->mmbCode), pLoad, pMM->pConf);
    default :
        return T5RET_ERROR;
    }
}

T5_RET T5MM_WriteSyncRequest (T5PTR_MM pMM, T5_WORD wRequest,
                              T5PTR_CSLOAD pLoad)
{
    T5STR_MMB mmb;

    T5_MEMSET (&mmb, 0, sizeof (mmb));
    mmb.wID = T5MM_CODE;
    switch (wRequest)
    {
    case T5CSRQ_BEGINLOAD :
        return T5Memory_StartWrite (&mmb, pLoad, pMM->pConf);
    case T5CSRQ_LOAD :
        return T5Memory_Write (&mmb, pLoad, pMM->pConf);
    case T5CSRQ_ENDLOAD :
        return T5Memory_EndWrite (&mmb, pLoad, pMM->pConf);
    default :
        return T5RET_ERROR;
    }
}

#ifdef T5DEF_ONLINECHANGE

T5_RET T5MM_WriteChangeRequest (T5PTR_MM pMM, T5_WORD wRequest,
                                T5PTR_CSLOAD pLoad)
{
    switch (wRequest)
    {
    case T5CSRQ_BEGINLOAD :
        return T5Memory_StartWrite (&(pMM->mmbChange), pLoad, pMM->pConf);
    case T5CSRQ_LOAD :
        return T5Memory_Write (&(pMM->mmbChange), pLoad, pMM->pConf);
    case T5CSRQ_ENDLOAD :
        return T5Memory_EndWrite (&(pMM->mmbChange), pLoad, pMM->pConf);
    default :
        return T5RET_ERROR;
    }
}

#endif /*T5DEF_ONLINECHANGE*/

/****************************************************************************/

T5_RET T5MM_AllocRawVMDB (T5PTR_MM pMM, T5_DWORD dwRawSize)
{
    return T5Memory_Alloc (&(pMM->mmbVmDb), dwRawSize, pMM->pConf);
}

T5_RET T5MM_FreeVMDB (T5PTR_MM pMM)
{
    return T5Memory_Free (&(pMM->mmbVmDb), pMM->pConf);
}

T5_PTR T5MM_LinkVMDB (T5PTR_MM pMM)
{
    if (T5Memory_Link (&(pMM->mmbVmDb), pMM->pConf) != T5RET_OK)
        return T5_PTNULL;
    return pMM->mmbVmDb.pData;
}

void T5MM_UnlinkVMDB (T5PTR_MM pMM)
{
    T5Memory_Unlink (&(pMM->mmbVmDb), pMM->pConf);
}

T5_RET T5MM_LoadVMDB (T5PTR_MM pMM, T5_PTCHAR szAppName)
{
    return T5Memory_Load (&(pMM->mmbVmDb), szAppName, pMM->pConf);
}

T5_RET T5MM_SaveVMDB (T5PTR_MM pMM, T5_PTCHAR szAppName)
{
    return T5Memory_Save (&(pMM->mmbVmDb), szAppName, pMM->pConf);
}

/* eof **********************************************************************/
