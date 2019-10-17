/*****************************************************************************
T5VMlog.c :  manage stack of log messages and list of unresolved resources

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/
/* optional hooks */

#ifndef T5HOOK_LOGEVENTS

#define T5HOOK_ONLOGERROR(code) /* nothing */
#define T5HOOK_ONLOGUNRES(name) /* nothing */

#endif /*T5HOOK_LOGEVENTS*/

static T5_WORD M_SYS[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/*****************************************************************************
T5VMLog_Push
push a log message on the stack
Parameters:
    pDB (IN) pointer to the database
    wCode (IN) log message ID
    wArg (IN) main argument
    dwData (IN) detail argument
*****************************************************************************/

void T5VMLog_Push (T5PTR_DB pDB, T5_WORD wCode, T5_WORD wArg, T5_DWORD dwData)
{
    T5PTR_DBLOG pLog;
    T5_WORD wIndex;
    T5PTR_DBSTATUS pStatus;

    T5HOOK_ONLOGERROR(wCode);

    if (pDB == NULL)
        return;

    pStatus = T5GET_DBSTATUS(pDB);
    if (pStatus->wNbLog < T5MAX_LOG)
    {
        pLog = T5GET_DBLOG(pDB);
        wIndex = pStatus->wNbLog;
        pLog[wIndex].wCode = wCode;
        pLog[wIndex].wArg = wArg;
        pLog[wIndex].dwData = dwData;
        pStatus->wNbLog += 1;
        pStatus->wFlags |= T5FLAG_LOG;
    }
}

/*****************************************************************************
T5VMLog_PushSys
push a system log message (only for debugging)
Parameters:
    wCode (IN) log message ID
*****************************************************************************/

void T5VMLog_PushSys (T5_WORD wCode)
{
    T5_WORD i;

    for (i=0; i<16; i++)
    {
        if (M_SYS[i] == 0)
        {
            M_SYS[i] = wCode;
            return;
        }
    }
}

/*****************************************************************************
T5VMLog_Pop
pop a log message from the stack
Parameters:
    pDB (IN) pointer to the database
    pwCode (OUT) log message ID
    pwArg (OUT) main argument
    pdwData (OUT) detail argument
Return: TRUE=ok / FALSE=stack is empty
*****************************************************************************/

T5_BOOL T5VMLog_Pop (T5PTR_DB pDB, T5_PTWORD pwCode,
                     T5_PTWORD pwArg, T5_PTDWORD pdwData)
{
    T5_BOOL bRet;
    T5PTR_DBLOG pLog;
    T5_WORD i;
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    if (pStatus->wNbLog  == 0)
    {
        *pwCode = 0;
        *pwArg = 0;
        *pdwData = 0;
        bRet = FALSE;
    }
    else
    {
        pLog = T5GET_DBLOG(pDB);
        *pwCode = pLog->wCode;
        *pwArg = pLog->wArg;
        *pdwData = pLog->dwData;
        for (i=0; i<(pStatus->wNbLog - 1); i++)
        {
            pLog[i].wCode = pLog[i+1].wCode;
            pLog[i].wArg = pLog[i+1].wArg;
            pLog[i].dwData = pLog[i+1].dwData;
        }
        pStatus->wNbLog -= 1;
        if (pStatus->wNbLog == 0)
            pStatus->wFlags &= ~T5FLAG_LOG;
        bRet = TRUE;
    }
    return bRet;
}

/*****************************************************************************
T5VMLog_PopSys
pop a system log message (only for debugging)
Parameters:
    pwCode (OUT) log message ID
Return: true if ok - false if no message
*****************************************************************************/

T5_BOOL T5VMLog_PopSys (T5_PTWORD pwCode)
{
    *pwCode = M_SYS[0];
    if (M_SYS[0] == 0)
        return FALSE;

    T5_MEMMOVE (M_SYS, &(M_SYS[1]), 15 * sizeof (T5_WORD));
    M_SYS[15] = 0;
    return TRUE;
}

/*****************************************************************************
T5VMLog_RegisterUnresolved
register the name of an unresolved resource
Parameters:
    pDB (IN) pointer to the database
    szName (IN) name of the unresolved
*****************************************************************************/

void T5VMLog_RegisterUnresolved (T5PTR_DB pDB, T5_PTCHAR szName)
{
    T5PTR_DBUNRES pList;
    T5PTR_DBSTATUS pStatus;
    T5_PTCHAR pDst;

    T5HOOK_ONLOGUNRES(szName);

    pStatus = T5GET_DBSTATUS(pDB);
    pList = T5GET_DBUNRES(pDB);
    if (pStatus->wNbUnres < pDB[T5TB_UNRES].wNbItemAlloc)
    {
        pDst = (T5_PTCHAR)pList[pStatus->wNbUnres++].szName;
        if (T5_STRLEN (szName) < 16)
            T5_STRCPY (pDst, szName);
        else
        {
            T5_MEMCPY (pDst, szName, 13);
            pDst[13] = '.';
            pDst[14] = '.';
            pDst[15] = '\0';
        }
    }
}

/*****************************************************************************
T5VMLog_GetUnresolved
get the name of an unresolved resource
Parameters:
    pDB (IN) pointer to the database
    wIndex (IN) 0 based index of the unresolved
Return: pointer to the resource name or NULL if index is out of range
*****************************************************************************/

T5_PTCHAR T5VMLog_GetUnresolved (T5PTR_DB pDB, T5_WORD wIndex)
{
    T5_PTCHAR szName;
    T5PTR_DBUNRES pList;
    T5PTR_DBSTATUS pStatus;

    pStatus = T5GET_DBSTATUS(pDB);
    pList = T5GET_DBUNRES(pDB);
    if (wIndex < pStatus->wNbUnres)
    {
        szName = (T5_PTCHAR)pList[wIndex].szName;
    }
    else
        szName = T5_PTNULL;
    return szName;
}

/****************************************************************************/
/* smart lock */

#ifdef T5DEF_SMARTLOCK

#define T5_ALIGN4(s)    (((s)%4)?((s)+4-((s)%4)):(s))
#define T5_SMNEXT(p)    (T5PTR_SMLOCK)((T5_PTBYTE)p + (T5_DWORD)p->wSizeof)

#define T5_ISSMCT(p)        ((((p)->bFlags)&T5SML_CT)!=0)
#define T5_SETSMCT(p)       (p)->bFlags |= T5SML_CT
#define T5_RESETSMCT(p)     (p)->bFlags &= ~T5SML_CT
#define T5_ISSMREAPPLY(p)   ((((p)->bFlags)&T5SML_REAPPLY)!=0)
#define T5_SETSMREAPPLY(p)  (p)->bFlags |= T5SML_REAPPLY

/*****************************************************************************
T5VMLog_RegisterLockVar
Register a new locked variable
Parameters:
    pDB (IN) pointer to the database
    wType (IN) variable data type
    wIndex (IN) variable index outside CTSEG
    dwCTOffset (IN) variable offset inside of CTSEG
*****************************************************************************/

void T5VMLog_RegisterLockVar (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_DWORD dwCTOffset)
{
    T5PTR_SMLOCK pSL;
    T5_DWORD dwSize;
    T5_WORD wSizeof;
    T5_PTBYTE pData;

    if (T5MAX_SMARTLOCK < sizeof (T5STR_SMLOCK))
        return;
    /* get data in VMDB */
    if (dwCTOffset == 0L)
        pData = (T5_PTBYTE)T5Tools_GetAnyParam (pDB, wType, wIndex, NULL, NULL);
    else
        pData = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData) + dwCTOffset;

    /* search for end of list and check available space */
    pSL = T5GET_SMLOCK(pDB);
    dwSize = 0L;
    while (pSL->wSizeof != 0)
    {
        if (pSL->bType == (T5_BYTE)wType &&
            ((T5_ISSMCT(pSL) && dwCTOffset == pSL->dwIndex)
            || (!T5_ISSMCT(pSL) && (T5_DWORD)wIndex == pSL->dwIndex)))
        {
            return; /* already locked */
        }
        dwSize += (T5_DWORD)(pSL->wSizeof);
        pSL = T5_SMNEXT(pSL);
    }
    switch (wType)
    {
    case T5C_SINT   : wSizeof = 1; break;
    case T5C_INT    : wSizeof = 2; break;
    case T5C_DINT   :
    case T5C_TIME   : wSizeof = 4; break;
    case T5C_LINT   : wSizeof = 8; break;
    case T5C_STRING : wSizeof = (T5_WORD)(*pData) + 1; break;
    default : return;
    }
    wSizeof += sizeof (T5STR_SMLOCK);
    wSizeof = T5_ALIGN4 (wSizeof);
    if (((T5_DWORD)wSizeof + dwSize) > T5MAX_SMARTLOCK)
        return;

    /* add a new record */
    pSL->wSizeof = wSizeof;
    pSL->bType = (T5_BYTE)wType;
    pSL->dwIndex = (dwCTOffset != 0L) ? dwCTOffset : (T5_DWORD)wIndex;
    if ((dwCTOffset != 0L))
        T5_SETSMCT (pSL);
    else
        T5_RESETSMCT (pSL);

    /* set current value */
    switch (wType)
    {
    case T5C_SINT   : T5_MEMCPY (pSL+1, pData, 1); break;
    case T5C_INT    : T5_MEMCPY (pSL+1, pData, 2); break;
    case T5C_DINT   :
    case T5C_TIME   : T5_MEMCPY (pSL+1, pData, 4); break;
    case T5C_LINT   : T5_MEMCPY (pSL+1, pData, 8); break;
    case T5C_STRING : T5_MEMCPY (pSL+1, pData + 1, (T5_WORD)(pData[1]) + 1); break;
    default : break;
    }

    pSL = T5_SMNEXT(pSL);
    pSL->wSizeof = 0;

#ifdef T5HOOK_SMARTLOCK
    T5HOOK_SMARTLOCK_REGISTER(pDB, wType, wIndex, dwCTOffset);
#endif
}

/*****************************************************************************
T5VMLog_UnRegisterLockVar
Unregister a new locked variable
Parameters:
    pDB (IN) pointer to the database
    wType (IN) variable data type
    wIndex (IN) variable index outside CTSEG
    dwCTOffset (IN) variable offset inside of CTSEG
*****************************************************************************/

void T5VMLog_UnRegisterLockVar (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_DWORD dwCTOffset)
{
    T5STR_SMLOCK key;
    T5PTR_SMLOCK pSL, pNext, pNextNext;
    T5_PTBYTE pData;

    if (T5MAX_SMARTLOCK < sizeof (T5STR_SMLOCK))
        return;
    /* build key */
    key.bType = (T5_BYTE)wType;
    key.dwIndex = (dwCTOffset != 0L) ? dwCTOffset : (T5_DWORD)wIndex;
    key.bFlags = (dwCTOffset != 0L) ? T5SML_CT : 0;

    /* search for record */
    pSL = T5GET_SMLOCK(pDB);
    while (pSL->wSizeof != 0
        && (pSL->bType != key.bType || T5_ISSMCT(pSL) != T5_ISSMCT(&key) || pSL->dwIndex != key.dwIndex))
        pSL = T5_SMNEXT(pSL);
    if (pSL->wSizeof == 0)
        return;

    /* reapply */
    if (T5_ISSMREAPPLY(pSL))
    {
        if (!T5_ISSMCT(pSL))
            pData = (T5_PTBYTE)T5Tools_GetAnyParam (pDB, pSL->bType, (T5_WORD)(pSL->dwIndex), NULL, NULL);
        else
            pData = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData) + pSL->dwIndex;
        switch (pSL->bType)
        {
        case T5C_SINT   : T5_MEMCPY (pData, pSL+1, 1); break;
        case T5C_INT    : T5_MEMCPY (pData, pSL+1, 2); break;
        case T5C_DINT   :
        case T5C_TIME   : T5_MEMCPY (pData, pSL+1, 4); break;
        case T5C_LINT   : T5_MEMCPY (pData, pSL+1, 8); break;
        case T5C_STRING : T5_MEMCPY (pData + 1, pSL+1, (T5_WORD)(pData[1]) + 1); break;
        default : break;
        }
    }

    /* remove record */
    pNext = T5_SMNEXT(pSL);
    while (pNext->wSizeof > 0)
    {
        pNextNext = T5_SMNEXT(pNext);
        T5_MEMMOVE (pSL, pNext, pNext->wSizeof);
        pNext = pNextNext;
        pSL = T5_SMNEXT(pSL);
    }
    pSL->wSizeof = 0;

#ifdef T5HOOK_SMARTLOCK
    T5HOOK_SMARTLOCK_UNREGISTER(pDB, wType, wIndex, dwCTOffset);
#endif
}

/*****************************************************************************
T5VMLog_SetLockVar
Set the internal value of a locked variable
Parameters:
    pDB (IN) pointer to the database
    wType (IN) variable data type
    wIndex (IN) variable index outside CTSEG
    dwCTOffset (IN) variable offset inside of CTSEG
    pValue (IN) pointer to internal value
    bReApply (IN) if true, please re-apply internal value when unlocking
*****************************************************************************/

T5_BOOL T5VMLog_SetLockVar (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_DWORD dwCTOffset,
                            T5_PTR pValue, T5_BOOL bReApply)
{
    T5STR_SMLOCK key;
    T5PTR_SMLOCK pSL;
    T5_PTBYTE pData;

    if (T5MAX_SMARTLOCK < sizeof (T5STR_SMLOCK))
        return FALSE;
    pData = (T5_PTBYTE)pValue;

    /* check type */
    switch (wType)
    {
    case T5C_BOOL   :
    case T5C_USINT  : wType = T5C_SINT; break;
    case T5C_UINT   : wType = T5C_INT; break;
    case T5C_REAL   :
    case T5C_UDINT  : wType = T5C_DINT; break;
    case T5C_LREAL  :
    case T5C_ULINT  : wType = T5C_LINT; break;
    default : break;
    }

    /* build key */
    key.bType = (T5_BYTE)wType;
    key.dwIndex = (dwCTOffset != 0L) ? dwCTOffset : (T5_DWORD)wIndex;
    key.bFlags = (dwCTOffset != 0L) ? T5SML_CT : 0;

    /* search for record */
    pSL = T5GET_SMLOCK(pDB);
    while (pSL->wSizeof != 0
        && (pSL->bType != key.bType || T5_ISSMCT(pSL) != T5_ISSMCT(&key) || pSL->dwIndex != key.dwIndex))
        pSL = T5_SMNEXT(pSL);
    if (pSL->wSizeof == 0)
        return FALSE;

    /* set new value */
    switch (wType)
    {
    case T5C_SINT   : T5_MEMCPY (pSL+1, pData, 1); break;
    case T5C_INT    : T5_MEMCPY (pSL+1, pData, 2); break;
    case T5C_DINT   :
    case T5C_TIME   : T5_MEMCPY (pSL+1, pData, 4); break;
    case T5C_LINT   : T5_MEMCPY (pSL+1, pData, 8); break;
    case T5C_STRING : T5_MEMCPY (pSL+1, pData + 1, (T5_WORD)(pData[1]) + 1); break;
    default : break;
    }
    /* reapply flag */
    if (bReApply)
        T5_SETSMREAPPLY(pSL);
    return TRUE;
}

/*****************************************************************************
T5VMLog_UnRegisterAllLockVars
Unregister all locked variable
Parameters:
    pDB (IN) pointer to the database
*****************************************************************************/

void T5VMLog_UnRegisterAllLockVars (T5PTR_DB pDB)
{
    T5PTR_SMLOCK pSL;
    T5_PTBYTE pData;

    if (T5MAX_SMARTLOCK < sizeof (T5STR_SMLOCK))
        return;
    pSL = T5GET_SMLOCK(pDB);
    while (pSL->wSizeof != 0)
    {
        if (T5_ISSMREAPPLY (pSL))
        {
            if (!T5_ISSMCT(pSL))
                pData = (T5_PTBYTE)T5Tools_GetAnyParam (pDB, pSL->bType, (T5_WORD)(pSL->dwIndex), NULL, NULL);
            else
                pData = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData) + pSL->dwIndex;
            switch (pSL->bType)
            {
            case T5C_SINT   : T5_MEMCPY (pData, pSL+1, 1); break;
            case T5C_INT    : T5_MEMCPY (pData, pSL+1, 2); break;
            case T5C_DINT   :
            case T5C_TIME   : T5_MEMCPY (pData, pSL+1, 4); break;
            case T5C_LINT   : T5_MEMCPY (pData, pSL+1, 8); break;
            case T5C_STRING : T5_MEMCPY (pData + 1, pSL+1, (T5_WORD)(pData[1]) + 1); break;
            default : break;
            }
        }
        pSL = T5_SMNEXT(pSL);
    }
    pSL = T5GET_SMLOCK(pDB);
    pSL->wSizeof = 0;

#ifdef T5HOOK_SMARTLOCK
    if (!T5_ISSMCT(pSL))
        T5HOOK_SMARTLOCK_UNREGISTER(pDB, pSL->bType, pSL->dwIndex, 0);
    else
        T5HOOK_SMARTLOCK_UNREGISTER(pDB, pSL->bType, 0, pSL->dwIndex);
#endif
}

/*****************************************************************************
T5VMLog_GetNbLocked
Get count of locked variable
Parameters:
    pDB (IN) pointer to the database
Return: number of variables locked
*****************************************************************************/

T5_DWORD T5VMLog_GetNbLocked (T5PTR_DB pDB)
{
    T5PTR_SMLOCK pSL;
    T5_DWORD nb;

    if (T5MAX_SMARTLOCK < sizeof (T5STR_SMLOCK))
        return 0;
    pSL = T5GET_SMLOCK(pDB);
    nb = 0;
    while (pSL->wSizeof != 0)
    {
        nb  += 1;
        pSL = T5_SMNEXT(pSL);
    }
    return nb;
}

/*****************************************************************************
T5VMLog_GetLockedVariable
Get description of locked variable (for debugger)
Parameters:
    pDB (IN) pointer to the database
    wIndex (IN) index of variable in lock table
    pDest (OUT) destination frame buffer
    wMaxLen (IN) size of the frame buffer
Return: frame length
*****************************************************************************/

T5_WORD T5VMLog_GetLockedVariable (T5PTR_DB pDB, T5_WORD wIndex, T5_PTR pDest, T5_WORD wMaxLen)
{
    T5PTR_SMLOCK pSL;
    T5_WORD wLen;
    T5_BYTE bLen;
    T5_PTBYTE pData, pFrame;

    if (T5MAX_SMARTLOCK < sizeof (T5STR_SMLOCK))
        return 0;
    pSL = T5GET_SMLOCK(pDB);
    while (pSL->wSizeof != 0 && wIndex--)
        pSL = T5_SMNEXT(pSL);
    if (pSL->wSizeof == 0)
        return 0;

    pFrame = (T5_PTBYTE)pDest;
    wLen = 0;
    T5_COPYFRAMEWORD(pFrame, &(pSL->wSizeof));      pFrame += 2; wLen += 2;
    *pFrame = pSL->bType;                           pFrame += 1; wLen += 1;
    *pFrame = T5_ISSMCT(pSL);                       pFrame += 1; wLen += 1;
    T5_COPYFRAMEDWORD(pFrame, &(pSL->dwIndex));     pFrame += 4; wLen += 4;

    pData = (T5_PTBYTE)(pSL + 1);
    switch (pSL->bType)
    {
    case T5C_SINT :
        *pFrame = *pData;
        wLen += 1;
        break;
    case T5C_INT :
        T5_COPYFRAMEWORD (pFrame, pData);
        wLen += 2;
        break;
    case T5C_DINT :
    case T5C_TIME :
        T5_COPYFRAMEDWORD (pFrame, pData);
        wLen += 4;
        break;
    case T5C_LINT :
        T5_COPYFRAME64 (pFrame, pData);
        wLen += 8;
        break;
    case T5C_STRING :
        bLen = *pData;
        if ((T5_WORD)bLen > (wMaxLen - wLen - 1))
            bLen = (T5_BYTE)(wMaxLen - wLen - 1);
        *pFrame = bLen;
        T5_MEMCPY (pFrame+1, pData+1, bLen);
        pFrame[bLen+1] = 0;
        wLen += ((T5_WORD)bLen + 2);
        break;
    default : break;
    }
    return wLen;
}

/*****************************************************************************
T5VMLog_IsCTLocked
Check if a CTSEG variable is locked - updates its internal value
Parameters:
    pDB (IN) pointer to the database
    wType (IN) variable data type
    pSrc (OUT) index of the variable containing the new internal value
    wRef (IN) index of DINT variable containing the reference in CTSEG
Return: true if variable locked
*****************************************************************************/

T5_BOOL T5VMLog_IsCTLocked (T5PTR_DB pDB, T5_WORD wType, T5_WORD wSrc, T5_WORD wRef)
{
    T5_PTDWORD pData32;

    switch (wType)
    {
    case T5C_BOOL :
    case T5C_USINT :
        wType = T5C_SINT;
        break;
    case T5C_UINT :
        wType = T5C_INT;
        break;
    case T5C_UDINT :
    case T5C_REAL :
        wType = T5C_DINT;
        break;
    case T5C_ULINT :
    case T5C_LREAL :
        wType = T5C_LINT;
        break;
    default : break;
    }
    pData32 = T5GET_DBDATA32 (pDB);
    return T5VMLog_IsCTLockedRaw (pDB, wType, wSrc, pData32[wRef]);
}

/*****************************************************************************
T5VMLog_IsCTLockedRaw
Check if a CTSEG variable is locked - updates its internal value
Parameters:
    pDB (IN) pointer to the database
    wType (IN) variable data type
    pSrc (OUT) index of the variable containing the new internal value
    dwOffset (IN) variable offset CTSEG
Return: true if variable locked
*****************************************************************************/

T5_BOOL T5VMLog_IsCTLockedRaw (T5PTR_DB pDB, T5_WORD wType, T5_WORD wSrc, T5_DWORD dwOffset)
{
    T5PTR_SMLOCK pSL;
    T5_PTBYTE pData;

    if (T5MAX_SMARTLOCK < sizeof (T5STR_SMLOCK))
        return 0;
    pSL = T5GET_SMLOCK(pDB);
    while (pSL->wSizeof != 0)
    {
        if (T5_ISSMCT(pSL) && pSL->bType == (T5_BYTE)wType && pSL->dwIndex == dwOffset)
        {
            if (wSrc != 0)
            {
                pData = (T5_PTBYTE)T5Tools_GetAnyParam (pDB, wType, wSrc, NULL, NULL);
                switch (pSL->bType)
                {
                case T5C_SINT   : T5_MEMCPY (pSL+1, pData, 1); break;
                case T5C_INT    : T5_MEMCPY (pSL+1, pData, 2); break;
                case T5C_DINT   :
                case T5C_TIME   : T5_MEMCPY (pSL+1, pData, 4); break;
                case T5C_LINT   : T5_MEMCPY (pSL+1, pData, 8); break;
                case T5C_STRING : T5_MEMCPY (pSL+1, pData + 1, (T5_WORD)(pData[1]) + 1); break;
                default : break;
                }
            }
            return TRUE;
        }
        pSL = T5_SMNEXT(pSL);
    }
    return FALSE;
}

/*****************************************************************************
T5VMLog_IsCTLockedRaw
Check if a CTSEG variable is locked - no update of internal value
Parameters:
    pDB (IN) pointer to the database
    dwOffset (IN) variable offset CTSEG
Return: true if variable locked
*****************************************************************************/

T5_BOOL T5VMLog_IsCTLockedOff (T5PTR_DB pDB, T5_DWORD dwOffset)
{
    T5PTR_SMLOCK pSL;

    if (T5MAX_SMARTLOCK < sizeof (T5STR_SMLOCK))
        return 0;
    pSL = T5GET_SMLOCK(pDB);
    while (pSL->wSizeof != 0)
    {
        if (T5_ISSMCT(pSL) && pSL->dwIndex == dwOffset)
            return TRUE;
        pSL = T5_SMNEXT(pSL);
    }
    return FALSE;
}

/*****************************************************************************
T5VMLog_HasTempVarLocked
Check if any temporary variable is locked
Parameters:
    pDB (IN) pointer to the database
Return: true at least one temp variable locked
*****************************************************************************/

T5_BOOL T5VMLog_HasTempVarLocked (T5PTR_DB pDB)
{
    T5PTR_SMLOCK pSL;
    T5_WORD vi;

    if (T5MAX_SMARTLOCK < sizeof (T5STR_SMLOCK))
        return FALSE;
    pSL = T5GET_SMLOCK(pDB);
    while (pSL->wSizeof != 0)
    {
        if (!T5_ISSMCT (pSL))
        {
            vi = (T5_WORD)(pSL->dwIndex);
            switch (pSL->bType)
            {
            case T5C_SINT   : return (vi >= pDB[T5TB_DATA8].wNbItemFixed);
            case T5C_INT    : return (vi >= pDB[T5TB_DATA16].wNbItemFixed);
            case T5C_DINT   : return (vi >= pDB[T5TB_DATA32].wNbItemFixed);
            case T5C_TIME   : return (vi >= pDB[T5TB_DATA64].wNbItemFixed);
            case T5C_LINT   : return (vi >= pDB[T5TB_TIME].wNbItemFixed);
            case T5C_STRING : return (vi >= pDB[T5TB_STRING].wNbItemFixed);
            default : break;
            }
        }
        pSL = T5_SMNEXT(pSL);
    }
    return FALSE;
}

#endif /*T5DEF_SMARTLOCK*/

/* eof **********************************************************************/
