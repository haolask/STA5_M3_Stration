/*****************************************************************************
T5VMasi.c :  ASi I/Os main core

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_ASI

/****************************************************************************/
/* static services */

static void _T5VMAsi_Build (T5PTR_ASIDEF pDef, T5PTR_ASI pASI);
static void _T5VMAsi_Link (T5PTR_DB pDB, T5PTR_ASI pASI);
static void _T5VMAsi_FlushInputs (T5PTR_ASI pASI);
static void _T5VMAsi_FlushOutputs (T5PTR_ASI pASI);

/*****************************************************************************
T5VMAsi_GetSizing
Get the size of memory to be allocated for ASi
Parameters:
   pDef (IN) static configuration downloaded in the app code
Return: size in bytes or 0 if error
*****************************************************************************/

T5_DWORD T5VMAsi_GetSizing (T5PTR_ASIDEF pDef)
{
    T5_DWORD dwSize;

    dwSize = (T5_DWORD)(sizeof (T5STR_ASI));
    if (pDef == NULL)
        return dwSize;

    dwSize += (T5_DWORD)(pDef->wNbMasterAlloc * sizeof (T5STR_ASIM));
    dwSize += (T5_DWORD)(pDef->wNbVarAlloc * sizeof (T5STR_ASIV));
    return dwSize;
}

/*****************************************************************************
T5VMAsi_CanHotStart
Test if Hot Restart or On Line change is valid
Parameters:
   pDef (IN) new static configuration downloaded in the app code
   pASI (IN) current runtime configuration
Return: OK or error
*****************************************************************************/

T5_RET T5VMAsi_CanHotStart (T5PTR_ASIDEF pDef, T5PTR_ASI pASI)
{
    if (pASI == NULL || pDef == NULL)
        return T5RET_OK;

    if (pDef->wNbMaster != pASI->def.wNbMaster)
        return T5RET_HOTASI;

    if (pDef->wNbVar > pASI->def.wNbVarAlloc)
        return T5RET_HOTASI;

    return T5RET_OK;
}

/*****************************************************************************
T5VMAsi_HotStart
Perform Hot Restart or On Line change is valid
Parameters:
   pDef (IN) new static configuration downloaded in the app code
   pASI (IN) current runtime configuration
   pDB (IN) pointer to full VM database
Return: OK or error
*****************************************************************************/

T5_RET T5VMAsi_HotStart (T5PTR_ASIDEF pDef, T5PTR_ASI pASI, T5PTR_DB pDB)
{
    T5_RET wRet;
    T5_WORD wNbMasterAlloc;
    T5_WORD wNbVarAlloc;

    if (pASI == NULL)
        return T5RET_OK;

    if (T5VMAsi_CanHotStart (pDef, pASI) != T5RET_OK)
        return T5RET_HOTASI;

    if (pDef == NULL)
    {
        T5_MEMSET (pASI, 0, sizeof (T5STR_ASI));
        return T5RET_OK;
    }

    wNbMasterAlloc = pASI->def.wNbMasterAlloc;
    wNbVarAlloc = pASI->def.wNbVarAlloc;
    _T5VMAsi_Build (pDef, pASI);
    pASI->def.wNbMasterAlloc = wNbMasterAlloc;
    pASI->def.wNbVarAlloc = wNbVarAlloc;

    _T5VMAsi_Link (pDB, pASI);

    if (!T5Asi_IsHWOpen ())
    {
        wRet = T5Asi_Init (pASI);
        if (wRet != T5RET_OK)
            return wRet;
    }

    if ((pDef->wFlags & T5ASI_SETCONF) != 0)
        T5Asi_ApplyConfig (pASI);
    T5Asi_ReadConfig (pASI);

    return T5RET_OK;
}

/*****************************************************************************
T5VMAsi_Open
Initialize and open all ASi masters - builds the ASi runtime memory block
Parameters:
   pDef (IN) new static configuration downloaded in the app code
   pASI (IN) current runtime configuration
   pDB (IN) pointer to full VM database
Return: OK or error
*****************************************************************************/

T5_RET T5VMAsi_Open (T5PTR_ASIDEF pDef, T5PTR_ASI pASI, T5PTR_DB pDB)
{
    T5_RET wRet;

    if (pASI == NULL)
        return T5RET_OK;

    if (pDef == NULL)
    {
        T5_MEMSET (pASI, 0, sizeof (T5STR_ASI));
        return T5RET_OK;
    }
    _T5VMAsi_Build (pDef, pASI);
    _T5VMAsi_Link (pDB, pASI);

    wRet = T5Asi_Init (pASI);
    if (wRet != T5RET_OK)
        return wRet;

    if ((pDef->wFlags & T5ASI_SETCONF) != 0)
        T5Asi_ApplyConfig (pASI);
    T5Asi_ReadConfig (pASI);
    return T5RET_OK;
}

/*****************************************************************************
T5VMAsi_Close
Close all ASi masters
Parameters:
   pASI (IN) current runtime configuration
*****************************************************************************/

void T5VMAsi_Close (T5PTR_ASI pASI)
{
    T5_WORD i;

    if (pASI == NULL)
        return;

    for (i=0; i<pASI->def.wNbMaster; i++)
        T5_MEMSET (pASI->pMaster[i].ido, 0, sizeof (pASI->pMaster[i].ido));
    T5Asi_Exchange (pASI);

    T5Asi_Exit (pASI);
}

/*****************************************************************************
T5VMAsi_Exchange
Exchange all ASi data
Parameters:
   pASI (IN) current runtime configuration
*****************************************************************************/

void T5VMAsi_Exchange (T5PTR_ASI pASI)
{
    if (pASI == NULL)
        return;

    _T5VMAsi_FlushOutputs (pASI);
    T5Asi_Exchange (pASI);
    _T5VMAsi_FlushInputs (pASI);
}

/*****************************************************************************
_T5VMAsi_Build
Builds the ASi runtime memory block
Parameters:
   pDef (IN) new static configuration downloaded in the app code
   pASI (IN) current runtime configuration
*****************************************************************************/

static void _T5VMAsi_Build (T5PTR_ASIDEF pDef, T5PTR_ASI pASI)
{
    T5_WORD i;
    T5PTR_ASIM pMaster;
    T5PTR_ASIMDEF pMasterDef;
    T5PTR_ASIV pVar;
    T5PTR_ASIVDEF pVarDef;
    T5_DWORD dwSave;
    T5_PTBYTE pVarDefRaw;

    /* header */
    T5_MEMSET (pASI, 0, sizeof (T5STR_ASI));
    T5_MEMCPY (&(pASI->def), pDef, sizeof (T5STR_ASIDEF));
    /* masters */
    pMaster = (T5PTR_ASIM)(pASI + 1);
    pASI->pMaster = pMaster;
    pMasterDef = (T5PTR_ASIMDEF)(pDef + 1);
    for (i=0; i<pDef->wNbMaster; i++)
    {
        dwSave = pMaster->hMaster;
        T5_MEMSET (pMaster, 0, sizeof (T5STR_ASIM));
        pMaster->hMaster = dwSave;
        pMaster->pDef = pMasterDef;
        pMaster++;
        pMasterDef++;
    }
    /* variables */
    pVar = (T5PTR_ASIV)pMaster;
    pASI->pVar = pVar;
    pVarDefRaw = (T5_PTBYTE)pMasterDef;

    for (i=0; i<pDef->wNbVar; i++)
    {
        pVarDef = (T5PTR_ASIVDEF)pVarDefRaw;

        /* chain in master lists */
        pMaster = &(pASI->pMaster[pVarDef->bMaster]);
        if ((pVarDef->bFlags & T5ASI_VOUTPUT) != 0)
        {
            if (pMaster->wNbVarOut == 0)
                pMaster->wFirstVarOut = i;
            pMaster->wNbVarOut += 1;
        }
        else
        {
            if (pMaster->wNbVarIn == 0)
                pMaster->wFirstVarIn = i;
            pMaster->wNbVarIn += 1;
        }
        /* build */
        pVar->pDef = pVarDef;
        pVar->bT5Type = (T5_BYTE)(pVarDef->wT5Type);
        if ((pVarDef->bFlags & T5ASI_VDIAG) != 0)
        {
            pVar->bOpe = T5ASIOPE_DIAG;
            pVar->pASiData = (T5_PTBYTE)(&(pMaster->ecFlags));
            if (pVarDef->bMask > 8)
            {
                pVar->pASiData += 1;
                pVar->bMask = 1 << (pVarDef->bMask - 9);
            }
            else
            {
                pVar->bMask = 1 << (pVarDef->bMask - 1);
            }
        }
        else if ((pVarDef->bFlags & T5ASI_VSTATUS) == 0)
        {
            if ((pVarDef->bFlags & T5ASI_VOUTPUT) != 0)
            {
                pVar->pASiData = &(pMaster->ido[pVarDef->bAddr / 2]);
                if ((pVarDef->bFlags & T5ASI_VNEG) != 0)
                    pVar->bOpe = T5ASIOPE_DATANOUT;
                else
                    pVar->bOpe = T5ASIOPE_DATAOUT;
            }
            else
            {
                pVar->pASiData = &(pMaster->idi[pVarDef->bAddr / 2]);
                if ((pVarDef->bFlags & T5ASI_VNEG) != 0)
                    pVar->bOpe = T5ASIOPE_DATANIN;
                else
                    pVar->bOpe = T5ASIOPE_DATAIN;
            }
            pVar->bMask = pVarDef->bMask;
            if (pVarDef->bAddr % 2)
                pVar->bMask = (pVar->bMask << 4) & 0xf0;
        }
        else
        {
            pVar->bOpe = T5ASIOPE_STATUS;
            pVar->bMask = pVarDef->bMask;
            switch (pVar->bMask)
            {
            case T5ASI_A : pVar->pASiData = pMaster->las; break;
            case T5ASI_D : pVar->pASiData = pMaster->lds; break;
            case T5ASI_P : pVar->pASiData = pMaster->lps; break;
            case T5ASI_C : pVar->pASiData = pMaster->lcs; break;
            case T5ASI_F : pVar->pASiData = pMaster->lfs; break;
            default      : break;
            }
        }
        pVarDefRaw += sizeof (T5STR_ASIVDEF);
        if ((pVarDef->bFlags & T5ASI_VCT) != 0)
        {
            T5_MEMCPY (&(pVar->pT5Data), pVarDefRaw, sizeof (T5_DWORD));
            pVarDefRaw += sizeof (T5_DWORD);
        }
        pVar++;
    }
}

/*****************************************************************************
_T5VMAsi_Link
Link all ASi data to VM variables
Parameters:
   pDB (IN) pointer to full VM database
   pASI (IN) current runtime configuration
*****************************************************************************/

static void _T5VMAsi_Link (T5PTR_DB pDB, T5PTR_ASI pASI)
{
    T5PTR_ASIV pVar;
    T5_WORD i;
    T5_PTBYTE pLockBase;
    T5_PTBYTE pD8;
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pD16;
#endif /*T5DEF_DATA16SUPPORTED*/
    T5_PTDWORD pD32;
#ifdef T5DEF_XV
    T5_PTBYTE *pXV;
#endif /*T5DEF_XV*/
#ifdef T5DEF_CTSEG
    T5_PTBYTE pCTSeg;
    T5_DWORD dwCTOffset;
#endif /*T5DEF_CTSEG*/

    pLockBase = T5GET_DBLOCK(pDB);
    pVar = pASI->pVar;
    for (i=0; i<pASI->def.wNbVar; i++, pVar++)
    {
        pVar->pLock = pLockBase + pVar->pDef->wT5Offset;

        if ((pVar->pDef->bFlags & T5ASI_VCT) != 0)
        {
            pVar->bLockMask = 0; /* no lock supported */
#ifdef T5DEF_CTSEG
            pCTSeg = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
            T5_MEMCPY (&dwCTOffset, &(pVar->pT5Data), sizeof (T5_DWORD));
            pCTSeg += dwCTOffset;
            pVar->pT5Data = (T5_PTR)pCTSeg;
#else /*T5DEF_CTSEG*/
            pVar->pT5Data = T5_PTNULL;
#endif /*T5DEF_CTSEG*/
        }
        else if (pVar->pDef->wT5Type & T5C_EXTERN)
        {
            pVar->bLockMask = T5LOCK_XV;
#ifdef T5DEF_XV
            pXV = T5GET_DBXV(pDB);
            pVar->pT5Data = (T5_PTR)(pXV[pVar->pDef->wT5Offset]);
#else /*T5DEF_XV*/
            pVar->pT5Data = NULL;
#endif /*T5DEF_XV*/
        }
        else
        {
            pVar->pT5Data = NULL;
            switch (pVar->pDef->wT5Type)
            {
            case T5C_BOOL :
            case T5C_SINT :
            case T5C_USINT :
                pVar->bLockMask = T5LOCK_DATA8;
                pD8 = T5GET_DBDATA8(pDB);
                pVar->pT5Data = (T5_PTR)(pD8 + pVar->pDef->wT5Offset);
                break;
            case T5C_INT :
            case T5C_UINT :
                pVar->bLockMask = T5LOCK_DATA16;
#ifdef T5DEF_DATA16SUPPORTED
                pD16 = T5GET_DBDATA16(pDB);
                pVar->pT5Data = (T5_PTR)(pD16 + pVar->pDef->wT5Offset);
#endif /*T5DEF_DATA16SUPPORTED*/
                break;
            case T5C_DINT :
            case T5C_UDINT :
                pVar->bLockMask = T5LOCK_DATA32;
                pD32 = T5GET_DBDATA32(pDB);
                pVar->pT5Data = (T5_PTR)(pD32 + pVar->pDef->wT5Offset);
                break;
            case T5C_TIME :
                pVar->bLockMask = T5LOCK_TIME;
                pD32 = T5GET_DBTIME(pDB);
                pVar->pT5Data = (T5_PTR)(pD32 + pVar->pDef->wT5Offset);
                break;
            default :
                break;
            }
        }
    }
}

/*****************************************************************************
_T5VMAsi_FlushInputs
Exchange all input and status data
Parameters:
   pASI (IN) current runtime configuration
*****************************************************************************/

static void _T5VMAsi_FlushInputs (T5PTR_ASI pASI)
{
    T5PTR_ASIV pVar;
    T5_WORD i;
    T5_BYTE bData;

    pVar = pASI->pVar;
    for (i=0; i<pASI->def.wNbVar; i++, pVar++)
    {
        if (pVar->bOpe != T5ASIOPE_DATAOUT && pVar->bOpe != T5ASIOPE_DATANOUT
            && (*(pVar->pLock) & pVar->bLockMask) == 0
            && pVar->pASiData != NULL && pVar->pT5Data != NULL)
        {
            switch (pVar->bOpe)
            {
            case T5ASIOPE_DATANIN :
            case T5ASIOPE_DATAIN :
                if (pVar->bMask == 0x0f)
                    bData = *(pVar->pASiData) & 0x0f;
                else if (pVar->bMask == 0xf0)
                    bData = (*(pVar->pASiData) >> 4) & 0x0f;
                else if (pVar->bOpe == T5ASIOPE_DATANIN)
                    bData = ((*(pVar->pASiData) & pVar->bMask) != 0) ? 0 : 1;
                else
                    bData = ((*(pVar->pASiData) & pVar->bMask) != 0) ? 1 : 0;
                break;
            case T5ASIOPE_STATUS :
                bData = T5ASIBIT_TEST (pVar->pASiData, pVar->pDef->bAddr);
                break;
            case T5ASIOPE_DIAG :
                bData = ((*(pVar->pASiData) & pVar->bMask) != 0) ? 1 : 0;
                break;
            default :
                bData = 0;
                break;
            }
            switch (pVar->bT5Type)
            {
            case T5C_BOOL :
                *(T5_PTCHAR)(pVar->pT5Data) = (T5_CHAR)(bData != 0);
                break;
            case T5C_SINT :
            case T5C_USINT :
                *(T5_PTCHAR)(pVar->pT5Data) = (T5_CHAR)bData;
                break;
#ifdef T5DEF_DATA16SUPPORTED
            case T5C_INT :
            case T5C_UINT :
                *(T5_PTSHORT)(pVar->pT5Data) = (T5_SHORT)bData;
                break;
#endif /*T5DEF_DATA16SUPPORTED*/
            case T5C_DINT :
            case T5C_TIME :
            case T5C_UDINT :
                *(T5_PTDWORD)(pVar->pT5Data) = (T5_DWORD)bData;
                break;
            default : break;
            }
        }
    }
}

/*****************************************************************************
_T5VMAsi_FlushOutputs
Exchange all output data
Parameters:
   pASI (IN) current runtime configuration
*****************************************************************************/

static void _T5VMAsi_FlushOutputs (T5PTR_ASI pASI)
{
    T5PTR_ASIV pVar;
    T5_WORD i;
    T5_BYTE bData;

    pVar = pASI->pVar;
    for (i=0; i<pASI->def.wNbVar; i++, pVar++)
    {
        if ((pVar->bOpe == T5ASIOPE_DATAOUT || pVar->bOpe == T5ASIOPE_DATANOUT)
            && pVar->pASiData != NULL && pVar->pT5Data != NULL)
        {
            switch (pVar->bT5Type)
            {
            case T5C_BOOL :
            case T5C_SINT :
            case T5C_USINT :
                bData = *(T5_PTBYTE)(pVar->pT5Data);
                break;
#ifdef T5DEF_DATA16SUPPORTED
            case T5C_INT :
            case T5C_UINT :
                bData = (T5_BYTE)(*(T5_PTWORD)(pVar->pT5Data));
                break;
#endif /*T5DEF_DATA16SUPPORTED*/
            case T5C_DINT :
            case T5C_TIME :
            case T5C_UDINT :
                bData = (T5_BYTE)(*(T5_PTDWORD)(pVar->pT5Data));
                break;
            default :
                bData = 0;
                break;
            }
            if (pVar->bMask == 0x0f)
            {
                *(pVar->pASiData) &= 0xf0;
                *(pVar->pASiData) |= (bData & 0x0f);
            }
            else if (pVar->bMask == 0xf0)
            {
                *(pVar->pASiData) &= 0x0f;
                *(pVar->pASiData) |= ((bData << 4) & 0xf0);
            }
            else if (pVar->bOpe == T5ASIOPE_DATANOUT)
            {
                if (!bData)
                    *(pVar->pASiData) |= pVar->bMask;
                else
                    *(pVar->pASiData) &= ~(pVar->bMask);
            }
            else
            {
                if (bData)
                    *(pVar->pASiData) |= pVar->bMask;
                else
                    *(pVar->pASiData) &= ~(pVar->bMask);
            }
        }
    }
}

/*****************************************************************************
T5VMAsi_GetMasterProfiles
Get list of projected and detected profiles for a master
Parameters:
   pASI (IN) current runtime configuration
   wMaster (IN) 1-based index of the master
   pDest (OUT) destination buffer (projected then detected) - size 256 bytes
*****************************************************************************/

void T5VMAsi_GetMasterProfiles (T5PTR_ASI pASI, T5_WORD wMaster,
                                T5_PTBYTE pDest)
{
    if (wMaster < 1 || wMaster > pASI->def.wNbMaster)
        T5_MEMSET (pDest, 0xff, 256);
    else
    {
        T5_MEMCPY (pDest, pASI->pMaster[wMaster-1].wPrjPrf, 128);
        T5_MEMCPY (pDest+128, pASI->pMaster[wMaster-1].wDetPrf, 128);
    }
}

/****************************************************************************/

#endif /*T5DEF_ASI*/

/* eof **********************************************************************/
