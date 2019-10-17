/*****************************************************************************
T5VMsrv.c :  data server (read objects and execute)

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/
/* static services */

static T5_BYTE _GetMissingInfo (T5PTR_DB pDB, T5_PTBYTE pDest);
static T5_BYTE _GetSysInfo (T5PTR_DB pDB, T5_WORD wIndex, T5_PTBYTE pDest);
static T5_BYTE _GetSysInfoFB (T5_WORD wIndex, T5_PTBYTE pDest);
static T5_WORD _GetCTTypeDef (T5PTR_DB pDB, T5_WORD wIndex, T5_PTBYTE pWrite);

/*****************************************************************************
_T5VM_TraceFrame
Comm: trace the contents of a frame
Parameters:
    cHead (IN) character to output at the beggining of the trace line
    pFrame (IN) pointer to the frame
    wLen (IN) frame length
*****************************************************************************/

#ifdef T5DEF_TRACEFRAMES
static void _T5VM_TraceFrame (T5_BYTE cHead, T5_PTBYTE pFrame, T5_WORD wLen)
{
    char str[512], *ptr;
    T5_WORD i;

    sprintf (str, "%c(%u):", cHead, wLen);
    ptr = str;
    for (i=0; (i<128 && i<wLen); i++)
    {
        while (*ptr) ptr++;
        sprintf (ptr, " %02X", pFrame[i]);
    }
    T5TRACECOM(str);
}
#endif /*T5DEF_TRACEFRAMES*/

/*****************************************************************************
T5VM_Serve
Comm: execute a request from comm
In frame :  <bID> <bReq> <wLen> [data]
Out frame : <bID> <bRet> <wLen> [data]
Parameters:
    pDB (IN) pointer the DB - CAN BE NULL IF NO APP LOADED!!!
    pIn (IN) input frame
    wOut (IN) output frame
Return: size of the full out frame
*****************************************************************************/

T5_WORD T5VM_Serve (T5PTR_DB pDB, T5_PTBYTE pIn, T5_PTBYTE pOut)
{
    T5_RET wPort;

#ifdef T5DEF_TRACEPORT
    wPort = T5DEF_TRACEPORT;
#else /*T5DEF_TRACEPORT*/
    wPort = 0;
#endif /*T5DEF_TRACEPORT*/
    return T5VM_ServeEx (pDB, pIn, pOut, wPort);
}

T5_WORD T5VM_ServeEx (T5PTR_DB pDB, T5_PTBYTE pIn, T5_PTBYTE pOut, T5_WORD wEvsPort)
{
    T5_RET wRet;
    T5_WORD wLenIn, wLenOut;
    T5_DWORD dwPass;
    T5PTR_DBPRIVATE pPrivate;

    T5TRACECOMDW("Serve", pIn);
    T5_COPYFRAMEWORD(&wLenIn, pIn+2);
    wLenOut = 0;
    switch (pIn[1])
    {
    case T5REQ_INIT :
        if (pDB == NULL)
            wRet = T5RET_OK;
        else
        {
            if (wLenIn < 4)
                dwPass = 0;
            else
            {
                T5_COPYFRAMEDWORD(&dwPass, pIn+4);
            }
            pPrivate = T5GET_DBPRIVATE(pDB);
            if (pPrivate->dwPassword == 0 || pPrivate->dwPassword == dwPass)
                wRet = T5RET_OK;
            else
                wRet = T5RET_ERROR;
        }
        T5_COPYFRAMEWORD(pOut+4, &wEvsPort);
        dwPass = T5_GETDOWNLOADPASSWORD();
        T5_COPYFRAMEDWORD(pOut+6, &dwPass);
        wLenOut = 6;
        break;
    case T5REQ_EXECUTE :
        wRet = T5VM_Execute (pDB, pIn+4, wLenIn);
        break;
    case T5REQ_READ :
        wLenOut = T5VM_ReadObjects (pDB, pIn+4, pOut+4);
        if (wLenOut) wRet = T5RET_OK;
        else wRet = T5RET_SERVEREAD;
        break;
#ifdef T5DEF_EA
    case T5REQ_SUBSCRIBE :
        wLenOut = T5VM_SrvSubscribe (pDB, pIn+4, pOut+4);
        if (wLenOut) wRet = T5RET_OK;
        else wRet = T5RET_SERVEREAD;
        break;
    case T5REQ_UNSUBSCRIBE :
        wRet = T5VM_SrvUnsubscribe (pDB, pIn+4);
        break;
#ifdef T5DEF_CTSEG
    case T5REQ_SUBSCRIBEL :
        wLenOut = T5VM_SrvSubscribeL (pDB, pIn+4, pOut+4);
        if (wLenOut) wRet = T5RET_OK;
        else wRet = T5RET_SERVEREAD;
        break;
    case T5REQ_UNSUBSCRIBEL :
        wRet = T5VM_SrvUnsubscribeL (pDB, pIn+4);
        break;
#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_EA*/
    case T5REQ_READTABLE :
        wLenOut = T5VM_SrvPollTable (pDB, pIn+4, pOut+4);
        wRet = T5RET_OK;
        break;
    default :
        wRet = T5RET_BADREQUEST;
        break;
    }
    pOut[0] = pIn[0]; /* echo caller ID */
    if (wRet == T5RET_OK) pOut[1] = 0;
    else pOut[1] = 0xff;
    T5_COPYFRAMEWORD(pOut+2, &wLenOut);

#ifdef T5DEF_TRACEFRAMES
    _T5VM_TraceFrame ('Q', pIn, (T5_WORD)(wLenIn+4));
    _T5VM_TraceFrame ('A', pOut, (T5_WORD)(wLenOut+4));
#endif /*T5DEF_TRACEFRAMES*/

    return (wLenOut + 4);
}

/*****************************************************************************
T5VM_Execute
Comm: execute a TIC request from comm
In frame:  sequence of TIC code
Out frame: none
Parameters:
    pDB (IN) pointer the DB - CAN BE NULL IF NO APP LOADED!!!
    pAsk (IN) input frame (TIC code sequence)
    wSize (IN) length of the sequence (in bytes!)
Return: OK or ERROR
*****************************************************************************/

T5_RET T5VM_Execute (T5PTR_DB pDB, T5_PTBYTE pTic, T5_WORD wSize)
{
    T5PTR_DB pExec;
    T5_PTBYTE pFrame, pBuffer;
    T5_PTWORD pDest;
    T5_WORD i;
    T5PTR_DBSTATUS pStatus;

    /* cant execute if no app loaded */
    if (pDB == T5_PTNULL)
        return 0;
    pStatus = T5GET_DBSTATUS(pDB);

    /* check buffer size */
    pExec = &(pDB[T5TB_EXEC]);
    if ((wSize + pStatus->wExecSize + sizeof (T5_WORD)) > pExec->wNbItemAlloc)
        return T5RET_EXECSIZE;

    /* store request */
    pBuffer = (T5_PTBYTE)(pExec->pData);
    pBuffer += pStatus->wExecSize;
    pFrame = pTic;
    pDest = (T5_PTWORD)pBuffer;
    for (i=0; i < (wSize / sizeof (T5_WORD)); i++)
    {
        T5_COPYFRAMEWORD(pDest, pFrame);
        pDest++;
        pFrame += 2;
    }
    *(T5_PTWORD)(pBuffer + wSize) = T5C_RET; /* end of sequence */
    pStatus->wExecSize += wSize;
    return T5RET_OK;
}

/*****************************************************************************
T5VM_ReadObjects
Comm: read object values
In frame:  <bNb> { <cType> <wIndex> }
Out frame: <bNb> { <cType> <wIndex> <value> }
Parameters:
    pDB (IN) pointer the DB - CAN BE NULL IF NO APP LOADED!!!
    pAsk (IN) input frame (list of objects to read)
    pReply (OUT) output frame (list of values + identification)
Return: reply frame langth or 0 if error
*****************************************************************************/

T5_WORD T5VM_ReadObjects (T5PTR_DB pDB, T5_PTBYTE pAsk, T5_PTBYTE pReply)
{
    T5_BYTE nb, nbDone, bLen, bType;
    T5_BOOL bOK;
    T5_WORD wSize, wFree, wIndex;
    T5PTR_DBSTATUS pStatus;
    T5_PTBYTE pWrite, ptx;
    T5_DWORD dw;
    T5_WORD w1, w2;
    T5PTR_DBPROG pProg;
    T5_PTDWORD ptdw;
    T5_PTWORD ptw;
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE *pStr, pString;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 p64;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_SFC
    T5PTR_DBSTEP pStep;
    T5PTR_DBTRANS pTrans;
#endif /*T5DEF_SFC*/
#ifdef T5DEF_XV
    T5_PTBYTE *pXV;
#endif /*T5DEF_XV*/
    T5PTR_DBCALLSTACK pStack;
#ifdef T5DEF_ASI
    T5PTR_ASI pASI;
#endif /*T5DEF_ASI*/
#ifdef T5DEF_CTSEG
    T5_PTBYTE pCTSeg;
    T5_DWORD dwCTOffset;
#endif /*T5DEF_CTSEG*/
    T5_PTBYTE pData;

    if (pDB != T5_PTNULL)
    {
        pStatus = T5GET_DBSTATUS(pDB);
        pProg = T5GET_DBPROG(pDB);
        pStack = T5GET_DBCALLSTACK(pDB);
    }
    else
    {
        pStatus = NULL;
        pProg = NULL;
        pStack = NULL;
    }

#ifdef T5DEF_CTSEG
    if (pDB != NULL)
        pCTSeg = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
    else
        pCTSeg = NULL;
#endif /*T5DEF_CTSEG*/

    wFree = T5MAX_FRAME - 2; /* for ID and RC */
    nb = *pAsk++;
    nbDone = 0;
    wSize = 1;
    bOK = TRUE;
    pWrite = pReply + 1;
    while (nb && bOK)
    {
        dwCTOffset = 0L;
        nb--;
        if (wFree <= 3)
            bOK = FALSE;
        else
        {
            bType = *pAsk;
#ifdef T5DEF_CTSEG
            if (bType == T5T_CTSEG && pCTSeg != NULL)
            {
                pAsk++; /* skip CTSEG type  and get actual data type */
                bType = *pAsk++;
                wIndex = 0;
                /* get CT address */
                T5_COPYFRAMEDWORD(&dwCTOffset, pAsk);
                pAsk += 4;
                pData = pCTSeg + dwCTOffset;
                /* echo type and 0 index */
                pWrite[0] = bType;
                pWrite[1] = pWrite[2] = 0;
            }
            else
#endif /*T5DEF_CTSEG*/
            {
                pData = NULL;
                /* echo type and index */
                T5_MEMCPY(pWrite, pAsk, 3);
                /* skip type and remember index */
                T5_COPYFRAMEWORD(&wIndex, pAsk + 1);
                pAsk += 3;
            }
            pWrite += 3;
            wFree -= 3;
            wSize += 3;
            switch (bType)
            {
            case 0 : /* sysinfo: not documented */
                if (wFree < 280)
                    bOK = FALSE;
                else
                {
                    bLen = _GetSysInfo (pDB, wIndex, pWrite+1);
                    *pWrite = bLen;
                    pWrite += (bLen + 1);
                    wFree -= (bLen + 1);
                    wSize += (bLen + 1);
                    nbDone++;
                }
                break;
            case T5T_APPNAME :
                /* len + (max)15char + '\0'*/
                if (wFree < 17)
                    bOK = FALSE;
                else if (pDB == T5_PTNULL) /* no app */
                {
                    *pWrite++ = 0;
                    *pWrite++ = 0;
                    wFree -= 2;
                    wSize += 2;
                    nbDone++;
                }
                else
                {
                    ptx = (T5_PTBYTE)(pStatus->szAppName);
                    bLen = (T5_BYTE)T5_STRLEN(ptx);
                    *pWrite++ = bLen;
                    T5_MEMCPY(pWrite, ptx, bLen);
                    pWrite += bLen;
                    *pWrite++ = 0;
                    wFree -= (bLen + 2);
                    wSize += (bLen + 2);
                    nbDone++;
                }
                break;
            case T5T_APPSTATUS :
                /* status on one word */
                if (wFree < 2)
                    bOK = FALSE;
                else
                {
                    if (pDB != T5_PTNULL)
                    {
                        T5_COPYFRAMEWORD(pWrite, &(pStatus->wFlags));
                        pWrite += 2;
                        if ((pStatus->wFlags & T5FLAG_PROGSTEP) != 0)
                        {
                            if (wFree < 6)
                                bOK = FALSE;
                            else
                            {
                                w1 = pDB[T5TB_CALLSTACK].wNbItemUsed; 
                                if (w1 == 0)
                                {
                                    T5_COPYFRAMEWORD(pWrite,
                                                     &(pStatus->wCurPrg));
                                }
                                else
                                {
                                    T5_COPYFRAMEWORD(pWrite,
                                                     &(pStack[w1].wProg));
                                }
                                pWrite += 2;
                                T5_COPYFRAMEWORD(pWrite, &(pStatus->wCurPos));
                                pWrite += 2;
                                wFree -= 4;
                                wSize += 4;
                            }
                        }
                    }
                    else
                    {
                        *pWrite++ = 0;
                        *pWrite++ = 0;
                    }
                    wFree -= 2;
                    wSize += 2;
                    nbDone++;
                }
                break;

            case T5T_CALLSTACK :
                /* prg on 2 bytes then pos on 2 bytes */
                if (wFree < 4 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    w1 = pDB[T5TB_CALLSTACK].wNbItemUsed;
                    if (wIndex >= T5T_BKPOFFSET
                        && wIndex < (T5T_BKPOFFSET+T5MAX_BKP)
                        && pStatus->bkp[wIndex-T5T_BKPOFFSET].wStyle != T5BKP_ONESHOT)
                    {
                        w1 = pStatus->bkp[wIndex-T5T_BKPOFFSET].wProg;
                        if (pStatus->bkp[wIndex-T5T_BKPOFFSET].wStyle == T5BKP_VIEW)
                            w1 |= 0x8000;
                        T5_COPYFRAMEWORD(pWrite, &w1);
                        w1 = pStatus->bkp[wIndex-T5T_BKPOFFSET].wPos;
                        T5_COPYFRAMEWORD(pWrite+2, &w1);
                    }
                    else if (wIndex > 0 && wIndex <= w1)
                    {
                        T5_COPYFRAMEWORD(pWrite, &(pStack[wIndex-1].wProg));
                        w1 = pStack[wIndex-1].wPos - 2;
                        T5_COPYFRAMEWORD(pWrite+2, &w1);
                    }
                    else
                        T5_MEMSET (pWrite, 0, 4);
                    pWrite += 4;
                    wFree -= 4;
                    wSize += 4;
                    nbDone++;
                }
                break;

            case T5T_CODESFX :
                /* 4 byte string */
                if (wFree < 4)
                    bOK = FALSE;
                else
                {
                    T5_MEMCPY(pWrite, T5DEF_TARGETCODE, 4);
                    pWrite += 4;
                    wFree -= 4;
                    wSize += 4;
                    nbDone++;
                }
                break;
            case T5T_VERSION :
                /* version on one dword */
                if (wFree < 4 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    T5_COPYFRAMEDWORD(pWrite, &(pStatus->dwAppVersion));
                    pWrite += 4;
                    wFree -= 4;
                    wSize += 4;
                    nbDone++;
                }
                break;
            case T5T_DATESTAMP :
                /* stamp on one dword */
                if (wFree < 4 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    T5_COPYFRAMEDWORD(pWrite, &(pStatus->dwAppDateStamp));
                    pWrite += 4;
                    wFree -= 4;
                    wSize += 4;
                    nbDone++;
                }
                break;
            case T5T_CRC :
                /* CRC on one dword */
                if (wFree < 4 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    switch (wIndex)
                    {
                    case T5T_CRCDATA :
                        dw = pStatus->dwAppDataCRC;
                        break;
                    case T5T_CRCCODE :
                        dw = pStatus->dwAppCodeCRC;
                        break;
                    default :
                        dw = 0;
                        break;
                    }
                    T5_COPYFRAMEDWORD(pWrite, &dw);
                    pWrite += 4;
                    wFree -= 4;
                    wSize += 4;
                    nbDone++;
                }
                break;
            case T5T_TC :
                /* time on one dword */
                if (wFree < 4 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    switch (wIndex)
                    {
                    case T5T_TCCURRENT :
                        dw = pStatus->dwTimCycle;
                        break;
                    case T5T_TCTRIGGER :
                        dw = pStatus->dwTimTrigger;
                        break;
                    case T5T_TCMAX :
                        dw = pStatus->dwTimMax;
                        break;
                    case T5T_TCOVER :
                        dw = pStatus->dwOverflow;
                        break;
                    default :
                        dw = 0;
                        break;
                    }
                    T5_COPYFRAMEDWORD(pWrite, &dw);
                    pWrite += 4;
                    wFree -= 4;
                    wSize += 4;
                    nbDone++;
                }
                break;
            case T5T_LOG :
                /* Log message on two words plus one dword */
                if (wFree < 8)
                    bOK = FALSE;
                else
                {
                    if (pDB == T5_PTNULL)
                    {
                        T5VMLog_PopSys (&w1);
                        w2 = 0;
                        dw = 0;
                        T5_COPYFRAMEWORD(pWrite, &w1);
                        pWrite += 2;
                        T5_COPYFRAMEWORD(pWrite, &w2);
                        pWrite += 2;
                        T5_COPYFRAMEDWORD(pWrite, &dw);
                        pWrite += 4;
                    }
                    else
                    {
                        if (T5VMLog_PopSys (&w1))
                        {
                            w2 = 0;
                            dw = 0;
                        }
                        else
                            T5VMLog_Pop (pDB, &w1, &w2, &dw);
                        T5_COPYFRAMEWORD(pWrite, &w1);
                        pWrite += 2;
                        T5_COPYFRAMEWORD(pWrite, &w2);
                        pWrite += 2;
                        T5_COPYFRAMEDWORD(pWrite, &dw);
                        pWrite += 4;
                    }
                    wFree -= 8;
                    wSize += 8;
                    nbDone++;
                }
                break;
            case T5T_PROG :
                /* program state on a word */
                if (wFree < 2 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    T5_COPYFRAMEWORD(pWrite, &(pProg[wIndex].wFlags));
                    pWrite += 2;
                    wFree -= 2;
                    wSize += 2;
                    nbDone++;
                }
                break;
            case T5T_BYTE :
                /* lock state on 1 byte + value on 1 byte */
                if (wFree < 2 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
#ifdef T5DEF_CTSEG
                    if (pData != NULL)
                    {
                        *pWrite++ = T5VMLog_IsCTLockedRaw (pDB, T5C_SINT, 0, dwCTOffset);
                        *pWrite++ = *pData;
                    }
                    else
#endif /*T5DEF_CTSEG*/
                    {
                        ptx = T5GET_DBLOCK(pDB);
                        *pWrite++ = ((ptx[wIndex] & T5LOCK_DATA8) != 0);
                        ptx = T5GET_DBDATA8(pDB);
                        *pWrite++ = ptx[wIndex];
                    }
                    wFree -= 2;
                    wSize += 2;
                    nbDone++;
                }
                break;

            case T5T_DWORD :
                /* lock state on 1 byte + value on 4 bytes */
                if (wFree < 5 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
#ifdef T5DEF_CTSEG
                    if (pData != NULL)
                    {
                        *pWrite++ = T5VMLog_IsCTLockedRaw (pDB, T5C_DINT, 0, dwCTOffset);
                        T5_COPYFRAMEDWORD(pWrite, pData);
                    }
                    else
#endif /*T5DEF_CTSEG*/
                    {
                        ptx = T5GET_DBLOCK(pDB);
                        *pWrite++ = ((ptx[wIndex] & T5LOCK_DATA32) != 0);
                        ptdw = T5GET_DBDATA32(pDB);
                        T5_COPYFRAMEDWORD(pWrite, &(ptdw[wIndex]));
                    }
                    pWrite += 4;
                    wFree -= 5;
                    wSize += 5;
                    nbDone++;
                }
                break;

            case T5T_WORD :
                /* lock state on 1 byte + value on 2 bytes */
                if (wFree < 3 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
#ifdef T5DEF_CTSEG
                    if (pData != NULL)
                    {
                        *pWrite++ = T5VMLog_IsCTLockedRaw (pDB, T5C_INT, 0, dwCTOffset);
                        T5_COPYFRAMEWORD(pWrite, pData);
                    }
                    else
#endif /*T5DEF_CTSEG*/
                    {
                        ptx = T5GET_DBLOCK(pDB);
                        *pWrite++ = ((ptx[wIndex] & T5LOCK_DATA16) != 0);
                        ptw = T5GET_DBDATA16(pDB);
                        T5_COPYFRAMEWORD(pWrite, &(ptw[wIndex]));
                    }
                    pWrite += 2;
                    wFree -= 3;
                    wSize += 3;
                    nbDone++;
                }
                break;

            case T5T_TIME :
                /* lock state on 1 byte + value on 4 bytes */
                if (wFree < 5 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
#ifdef T5DEF_CTSEG
                    if (pData != NULL)
                    {
                        *pWrite++ = T5VMLog_IsCTLockedRaw (pDB, T5C_TIME, 0, dwCTOffset);
                        T5_COPYFRAMEDWORD(pWrite, pData);
                    }
                    else
#endif /*T5DEF_CTSEG*/
                    {
                        ptx = T5GET_DBLOCK(pDB);
                        *pWrite++ = ((ptx[wIndex] & T5LOCK_TIME) != 0);
                        ptdw = T5GET_DBTIME(pDB);
                        T5_COPYFRAMEDWORD(pWrite, &(ptdw[wIndex]));
                    }
                    pWrite += 4;
                    wFree -= 5;
                    wSize += 5;
                    nbDone++;
                }
                break;

            case T5T_MISSING :
                if (wFree < 257 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    bLen = _GetMissingInfo (pDB, pWrite+1);
                    *pWrite = bLen;
                    pWrite[bLen+1] = 0;
                    pWrite += (bLen + 2);
                    wFree -= (bLen + 2);
                    wSize += (bLen + 2);
                    nbDone++;
                }
                break;

            case T5T_LWORD :
                /* lock state on 1 byte + value on 8 bytes */
#ifdef T5DEF_DATA64SUPPORTED
                if (wFree < 9 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
#ifdef T5DEF_CTSEG
                    if (pData != NULL)
                    {
                        *pWrite++ = T5VMLog_IsCTLockedRaw (pDB, T5C_LINT, 0, dwCTOffset);
                        T5_COPYFRAME64(pWrite, pData);
                    }
                    else
#endif /*T5DEF_CTSEG*/
                    {
                        ptx = T5GET_DBLOCK(pDB);
                        *pWrite++ = ((ptx[wIndex] & T5LOCK_DATA64) != 0);
                        p64 = (T5_PTDATA64)T5GET_DBDATA64(pDB);
                        T5_COPYFRAME64(pWrite, &(p64[wIndex]));
                    }
                    pWrite += 8;
                    wFree -= 9;
                    wSize += 9;
                    nbDone++;
                }
#else /*T5DEF_DATA64SUPPORTED*/
                bOK = FALSE;
#endif /*T5DEF_DATA64SUPPORTED*/
                break;
            case T5T_STRING :
                /* lock state on 1 byte + len + chars + '\0' */
#ifdef T5DEF_STRINGSUPPORTED
                if (pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
#ifdef T5DEF_CTSEG
                    if (pData != NULL)
                    {
                        pString = pData;
                    }
                    else
#endif /*T5DEF_CTSEG*/
                    {
                        pStr = T5GET_DBSTRING(pDB);
                        pString = pStr[wIndex];
                    }
                    if (wFree < (T5_WORD)(pString[1] + 3))
                        bOK = FALSE;
                    else
                    {
                        ptx = T5GET_DBLOCK(pDB);
                        if (dwCTOffset)
                            *pWrite++ = T5VMLog_IsCTLockedRaw (pDB, T5C_STRING, 0, dwCTOffset);
                        else
                            *pWrite++ = ((ptx[wIndex] & T5LOCK_STRING) != 0);

                        T5_MEMCPY (pWrite, pString+1,
                                           (T5_WORD)(pString[1] + 2));
                        pWrite += (T5_WORD)(pString[1] + 2);
                        wFree -= (T5_WORD)(pString[1] + 3);
                        wSize += (T5_WORD)(pString[1] + 3);
                        nbDone++;
                    }
                }
#else /*T5DEF_STRINGSUPPORTED*/
                bOK = FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
                break;

            case T5T_STEP :
#ifdef T5DEF_SFC
                /* activity on 1 byte + breakpoint on 1 byte */
                if (wFree < 2 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    pStep = T5GET_DBSTEP(pDB);
                    if (pStep[wIndex].wStatus == T5STEP_N
                        || pStep[wIndex].wStatus == T5STEP_P0)
                        *pWrite++ = 1;
                    else
                        *pWrite++ = 0;
                    *pWrite++ = (T5_BYTE)(pStep[wIndex].wBreak);
                    wFree -= 2;
                    wSize += 2;
                    nbDone++;
                }
#else /*T5DEF_SFC*/
                bOK = FALSE;
#endif /*T5DEF_SFC*/
                break;

            case T5T_STEPT :
#ifdef T5DEF_SFC
                /* activity on 1 byte + breakpoint on 1 byte */
                if (wFree < 6 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    pStep = T5GET_DBSTEP(pDB);
                    if (pStep[wIndex].wStatus == T5STEP_N
                        || pStep[wIndex].wStatus == T5STEP_P0)
                        *pWrite++ = 1;
                    else
                        *pWrite++ = 0;
                    *pWrite++ = (T5_BYTE)(pStep[wIndex].wBreak);
                    T5_COPYFRAMEDWORD(pWrite, &(pStep[wIndex].dwAcTime));
                    pWrite += 4;
                    wFree -= 6;
                    wSize += 6;
                    nbDone++;
                }
#else /*T5DEF_SFC*/
                bOK = FALSE;
#endif /*T5DEF_SFC*/
                break;

            case T5T_TRANS :
#ifdef T5DEF_SFC
                /* validity on 1 byte + breakpoint on 1 byte */
                if (wFree < 2 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    pTrans = T5GET_DBTRANS(pDB);
                    if (pTrans[wIndex].wValid) *pWrite++ = 1;
                    else *pWrite++ = 0;
                    *pWrite++ = (T5_BYTE)(pTrans[wIndex].wBreak);
                    wFree -= 2;
                    wSize += 2;
                    nbDone++;
                }
#else /*T5DEF_SFC*/
                bOK = FALSE;
#endif /*T5DEF_SFC*/
                break;

#ifdef T5DEF_XV
            case T5T_X_D8 : /* 8 bit external variable */
                /* lock state on 1 byte + value on 1 byte */
                if (wFree < 2 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    ptx = T5GET_DBLOCK(pDB);
                    *pWrite++ = ((ptx[wIndex] & T5LOCK_XV) != 0);
                    pXV = T5GET_DBXV(pDB);
                    *pWrite++ = *(pXV[wIndex]);
                    wFree -= 2;
                    wSize += 2;
                    nbDone++;
                }
                break;
#ifdef T5DEF_DATA16SUPPORTED
            case T5T_X_D16 : /* 16 bit external variable */
                /* lock state on 1 byte + value on 2 bytes */
                if (wFree < 3 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    ptx = T5GET_DBLOCK(pDB);
                    *pWrite++ = ((ptx[wIndex] & T5LOCK_XV) != 0);
                    pXV = T5GET_DBXV(pDB);
                    T5_COPYFRAMEWORD(pWrite, pXV[wIndex]);
                    pWrite += 2;
                    wFree -= 3;
                    wSize += 3;
                    nbDone++;
                }
                break;
#endif /*T5DEF_DATA16SUPPORTED*/
            case T5T_X_D32 : /* 32 bit external variable */
            case T5T_X_TIME : /* TIME external variable */
                /* lock state on 1 byte + value on 4 bytes */
                if (wFree < 5 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    ptx = T5GET_DBLOCK(pDB);
                    *pWrite++ = ((ptx[wIndex] & T5LOCK_XV) != 0);
                    pXV = T5GET_DBXV(pDB);
                    T5_COPYFRAMEDWORD(pWrite, pXV[wIndex]);
                    pWrite += 4;
                    wFree -= 5;
                    wSize += 5;
                    nbDone++;
                }
                break;
#ifdef T5DEF_DATA64SUPPORTED
            case T5T_X_D64 : /* 64 bit external variable */
                /* lock state on 1 byte + value on 8 bytes */
                if (wFree < 9 || pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    ptx = T5GET_DBLOCK(pDB);
                    *pWrite++ = ((ptx[wIndex] & T5LOCK_XV) != 0);
                    pXV = T5GET_DBXV(pDB);
                    T5_COPYFRAME64(pWrite, pXV[wIndex]);
                    pWrite += 8;
                    wFree -= 9;
                    wSize += 9;
                    nbDone++;
                }
                break;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
            case T5T_X_STRING : /* STRING external variable */
                /* lock state on 1 byte + len + chars + '\0' */
                if (pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
                    pXV = T5GET_DBXV(pDB);
                    pString = pXV[wIndex];
                    if (wFree < (T5_WORD)(pString[1] + 3))
                        bOK = FALSE;
                    else
                    {
                        ptx = T5GET_DBLOCK(pDB);
                        *pWrite++ = ((ptx[wIndex] & T5LOCK_XV) != 0);
                        T5_MEMCPY (pWrite, pString+1,
                                           (T5_WORD)(pString[1] + 2));
                        pWrite += (T5_WORD)(pString[1] + 2);
                        wFree -= (T5_WORD)(pString[1] + 3);
                        wSize += (T5_WORD)(pString[1] + 3);
                        nbDone++;
                    }
                }
                break;
#endif /*T5DEF_STRINGSUPPORTED*/
#endif /*T5DEF_XV*/

            case T5T_BSMPSET : /* sampling trace - settings */
                if (pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
#ifdef T5DEF_BSAMPLING
                    w1 = T5VMBsmp_GetSettingsSize (pDB, wIndex);
                    if (wFree < w1)
                        bOK = FALSE;
                    else
                    {
                        w1 = T5VMBsmp_GetSettings (pDB, wIndex, pWrite);
                        pWrite += w1;
                        wFree -= w1;
                        wSize += w1;
                    }
#else /*T5DEF_BSAMPLING*/
                    if (wFree < 2)
                        bOK = FALSE;
                    else
                    {
                        *pWrite++ = 0;
                        *pWrite++ = 0;
                        wFree -= 2;
                        wSize += 2;
                    }
#endif /*T5DEF_BSAMPLING*/
                    nbDone++;
                }
                break;
            case T5T_BSMPDATA : /* sampling trace - data */
                if (pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
#ifdef T5DEF_BSAMPLING

                    
                    w1 = T5VMBsmp_GetDataSize (pDB, wIndex);
                    if (wFree < w1)
                        bOK = FALSE;
                    else
                    {
                        w1 = T5VMBsmp_GetData (pDB, wIndex, pWrite);
                        pWrite += w1;
                        wFree -= w1;
                        wSize += w1;
                    }
#else /*T5DEF_BSAMPLING*/
                    if (wFree < 2)
                        bOK = FALSE;
                    else
                    {
                        *pWrite++ = 0;
                        *pWrite++ = 0;
                        wFree -= 2;
                        wSize += 2;
                    }
#endif /*T5DEF_BSAMPLING*/
                    nbDone++;
                }
                break;

            case T5T_MAP :
                if (pDB == T5_PTNULL)
                    bOK = FALSE;
                else
                {
#ifdef T5DEF_VARMAP
                    w1 = T5Map_GetDescLength (pDB, wIndex);
                    if (wFree < w1)
                        bOK = FALSE;
                    else
                    {
                        w1 = T5Map_GetDesc (pDB, wIndex, pWrite);
                        pWrite += w1;
                        wFree -= w1;
                        wSize += w1;
                    }
                    nbDone++;

                    {
                        T5_BOOL bMore = (bOK && nb == 0 && nbDone < 50 && w1 > 12);
                        wIndex += 1;
                        while (bMore
                            && (w1 = T5Map_GetDescLength (pDB, wIndex)) <= (wFree-15))
                        {
                            *pWrite = T5T_MAP;
                            T5_COPYFRAMEWORD(pWrite+1, &wIndex);
                            pWrite += 3;
                            wFree -= 3;
                            wSize += 3;

                            w1 = T5Map_GetDesc (pDB, wIndex, pWrite);
                            pWrite += w1;
                            wFree -= w1;
                            wSize += w1;

                            nbDone++;
                            wIndex += 1;
                            bMore = (nbDone < 50 && w1 > 12);
                        }
                    }

#else /*T5DEF_VARMAP*/
                    if (wFree < 2)
                        bOK = FALSE;
                    else
                    {
                        *pWrite++ = 0;
                        *pWrite++ = 0;
                        wFree -= 2;
                        wSize += 2;
                    }
                    nbDone++;
#endif /*T5DEF_VARMAP*/
                }
                break;


            case T5T_ASIPRF :
                if (pDB == T5_PTNULL)
                    bOK = FALSE;
                else if (wFree < 256)
                    bOK = FALSE;
                else
                {
#ifdef T5DEF_ASI
                    pASI = T5GET_ASI(pDB);
                    T5VMAsi_GetMasterProfiles (pASI, wIndex, pWrite);
#else /*T5DEF_ASI*/
                    T5_MEMSET (pWrite, 0xff, 256);
#endif /*T5DEF_ASI*/
                    pWrite += 256;
                    wFree -= 256;
                    wSize += 256;
                    nbDone++;
                }
                break;

            case T5T_CTTYPEDEF :
                /* dwTotalSize - wPakSize - wFlags - data(max=512) */
                if (pDB == T5_PTNULL)
                    bOK = FALSE;
                else if (wFree < 520)
                    bOK = FALSE;
                else
                {
                    w1 = _GetCTTypeDef (pDB, wIndex, pWrite);
                    pWrite += w1;
                    wFree -= w1;
                    wSize += w1;
                }
                break;

            case T5T_STEPPING : /* reserved for extensions */
            default :
                bOK = FALSE;
                break;
            }
        }
    }
    *pReply = nbDone;

    return (bOK) ? wSize : 0;
}

/*****************************************************************************
_GetCTTypeDef
Comm: includes in a read frame a packet of CT type definition data
Parameters:
    pDB (IN) pointer the DB
    wIndex (IN) index of the wished packet (packet = 512 bytes)
    pDest (IN) pointer to the current position in the read answer frame
Return: siz of data added to the frame
*****************************************************************************/

static T5_WORD _GetCTTypeDef (T5PTR_DB pDB, T5_WORD wIndex, T5_PTBYTE pWrite)
{
#ifndef T5DEF_CTSEG
    T5_MEMSET(pWrite, 0, 8);
    return 8;
#else /*T5DEF_CTSEG*/
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBPRIVATE pPrivate;
    T5_DWORD dwTotalSize, dwOffset;
    T5_PTBYTE pDef;
    T5_WORD wFlags, wPakSize;

    pStatus = T5GET_DBSTATUS(pDB);
    pPrivate = T5GET_DBPRIVATE(pDB);
    pDef = (T5_PTBYTE)T5VMCode_GetCTTypeDef (pPrivate->pCode);
    /* get total size */
    if (pDef == NULL || (pStatus->wFlags & T5FLAG_HASCT) == 0)
        dwTotalSize = 0L;
    else
        dwTotalSize = *(T5_PTDWORD)pDef;
    /* make flags */
#ifdef T5DEF_BIGENDIAN
    wFlags = T5T_BIGENDIAN;
#else /*T5DEF_BIGENDIAN*/
    wFlags = 0;
#endif /*T5DEF_BIGENDIAN*/
    /* get packet size - check index */
    dwOffset = (T5_DWORD)wIndex * 512L;
    if (dwTotalSize == 0 || dwOffset >= dwTotalSize)
        wPakSize = 0;
    else if ((dwTotalSize - dwOffset) < 512L)
        wPakSize = (T5_WORD)(dwTotalSize - dwOffset);
    else
        wPakSize = 512;
    /* put header: dwTotalSize + wPakSize + wFlags */
    T5_COPYFRAMEDWORD(pWrite, &dwTotalSize);
    T5_COPYFRAMEWORD(pWrite+4, &wPakSize);
    T5_COPYFRAMEWORD(pWrite+6, &wFlags);
    /* put data */
    if (wPakSize != 0)
        T5_MEMCPY(pWrite+8, pDef + dwOffset, wPakSize);
    return (8 + wPakSize);
#endif /*T5DEF_CTSEG*/
}

/*****************************************************************************
_GetMissingInfo
Comm: includes in a read frame the list of unresolved resources
Parameters:
    pDB (IN) pointer the DB
    pDest (IN) pointer to the current position in the read answer frame
Return: siz of data added to the frame
*****************************************************************************/

static T5_BYTE _GetMissingInfo (T5PTR_DB pDB, T5_PTBYTE pDest)
{
    T5_BYTE bLen, bFree, bNameLen;
    T5_WORD wIndex;
    T5_PTCHAR szName;
    T5_BOOL bFirst;

    bLen = 0;
    bFree = 255;
    wIndex = 0;
    bFirst = TRUE;
    while ((szName = T5VMLog_GetUnresolved (pDB, wIndex++)) != T5_PTNULL)
    {
        bNameLen = (T5_BYTE)T5_STRLEN(szName);
        if ((bNameLen + 1) <= bFree)
        {
            if (bFirst) *pDest++ = ' ';
            else *pDest++ = ',';
            bFirst = FALSE;
            T5_STRCPY(pDest, szName);
            pDest += bNameLen;
			bLen += (bNameLen + 1);
			bFree -= (bNameLen + 1);
        }
    }
    return bLen;
}

/*****************************************************************************
_GetSysInfo
Comm: includes in a read frame a system information
Parameters:
    pDB (IN) pointer the DB
    wIndex (IN) type of sys info
    pDest (IN) pointer to the current position in the read answer frame
Return: siz of data added to the frame
*****************************************************************************/

static T5_BYTE _GetSysInfo (T5PTR_DB pDB, T5_WORD wIndex, T5_PTBYTE pDest)
{
    T5_BYTE bLen;
    T5PTR_DBSTATUS pStatus;    
    T5PTR_DBPRIVATE pPrivate;
    T5_DWORD dwCaps;
#ifdef T5_DESCRIPTION
    T5_WORD wLen;
#endif /*T5_DESCRIPTION*/
    T5_WORD i;
    T5_PTBYTE pLock;

    bLen = 0;
    switch (wIndex)
    {
    case T5T_SYSFLAGSEX :
        dwCaps = 0L;
        if (pDB != NULL)
        {
            pStatus = T5GET_DBSTATUS(pDB);
            dwCaps = pStatus->dwFlagsEx;
        }
        dwCaps |= T5FLAGEX_CAPS;
        T5_COPYFRAMEDWORD (pDest, &dwCaps);
        bLen = 4;
        break;
    case T5T_SYSCAPS1 :
#ifdef T5_CAPSGROUP1
        dwCaps = T5_CAPSGROUP1; 
#else /*T5_CAPSGROUP1*/
        dwCaps = 0L;
#endif /*T5_CAPSGROUP1*/
        T5_COPYFRAMEDWORD (pDest, &dwCaps);
        bLen = 4;
        break;
    case T5T_SYSCAPS2 :
#ifdef T5_CAPSGROUP2
        dwCaps = T5_CAPSGROUP2; 
#else /*T5_CAPSGROUP2*/
        dwCaps = 0;
#endif /*T5_CAPSGROUP2*/
        T5_COPYFRAMEDWORD (pDest, &dwCaps);
        bLen = 4;
        break;
    case T5T_SYSVERS :
#ifdef T5_DKVERS_H
        dwCaps = (T5_DKVERS_H << 16) & 0x00ff0000L ;
        dwCaps |= ((T5_DKVERS_M << 8) & 0x0000ff00L);
        dwCaps |= (T5_DKVERS_L & 0x000000ffL);
#else /*T5_DKVERS_H*/
        dwCaps = 0L;
#endif
        T5_COPYFRAMEDWORD (pDest, &dwCaps);
        bLen = 4;
        break;

    case T5T_SYS_NAME :
#ifdef T5_DESCRIPTION
        wLen = (T5_WORD)(T5_STRLEN(T5_DESCRIPTION));
        if (wLen > 255)
            wLen = 255;
        bLen = (T5_BYTE)wLen;
        T5_MEMCPY (pDest, T5_DESCRIPTION, bLen);
        pDest[bLen] = '\0';
        bLen += 1;
#endif /*T5_DESCRIPTION*/
        break;
    case T5T_SYS_AUTHOR :
#ifdef T5_AUTHOR
        wLen = (T5_WORD)(T5_STRLEN(T5_AUTHOR));
        if (wLen > 255)
            wLen = 255;
        bLen = (T5_BYTE)wLen;
        T5_MEMCPY (pDest, T5_AUTHOR, bLen);
        pDest[bLen] = '\0';
        bLen += 1;
#endif /*T5_AUTHOR*/
        break;
    case T5T_SYS_DBSIZE :
        if (pDB == NULL)
            dwCaps = 0L;
        else
            dwCaps = T5VM_GetTotalDBSize (pDB);
        T5_COPYFRAMEDWORD (pDest, &dwCaps);
        bLen = 4;
        break;
    case T5T_SYS_HEAPF :
        if (pDB == NULL)
            dwCaps = 0L;
        else
        {
            pPrivate = T5GET_DBPRIVATE(pDB);
            if (pPrivate->pHeap == NULL)
                dwCaps = 0L;
            else
                dwCaps = T5Heap_GetFreeSpace (pPrivate->pHeap);
        }
        T5_COPYFRAMEDWORD (pDest, &dwCaps);
        bLen = 4;
        break;
    case T5T_SYS_ELP_S :
        if (pDB == NULL)
            dwCaps = 0L;
        else
        {
            pStatus = T5GET_DBSTATUS(pDB);
            dwCaps = pStatus->dwElapsed;
        }
        T5_COPYFRAMEDWORD (pDest, &dwCaps);
        bLen = 4;
        break;
    case T5T_SYS_ELP_MS :
        if (pDB == NULL)
            dwCaps = 0L;
        else
        {
            pStatus = T5GET_DBSTATUS(pDB);
            dwCaps = pStatus->dwElapsedMS;
        }
        T5_COPYFRAMEDWORD (pDest, &dwCaps);
        bLen = 4;
        break;

    case T5T_SYS_OXC :
#ifdef T5_OXC
        dwCaps = T5_OXC; 
#else /*T5_OXC*/
        dwCaps = 0;
#endif /*T5_OXC*/
        T5_COPYFRAMEDWORD (pDest, &dwCaps);
        bLen = 4;
        break;
    case T5T_SYS_FB :
    case T5T_SYS_FB_NX :
        bLen = _GetSysInfoFB (wIndex, pDest);
        break;
    case T5T_SYS_LOCKLST :
        if (pDB == NULL)
        {
            dwCaps = 0;
            T5_COPYFRAMEDWORD (pDest, &dwCaps);
            bLen = 4;
        }
        else
        {
            pStatus = T5GET_DBSTATUS(pDB);
            T5_COPYFRAMEDWORD (pDest, &(pStatus->dwNbLock));
            bLen = 4;
            pLock = T5GET_DBLOCK(pDB);
            for (i=0; i<pDB[T5TB_LOCK].wNbItemAlloc && bLen <= 248; i++)
            {
                if (pLock[i])
                {
                    T5_COPYFRAMEWORD (pDest+bLen, &i);
                    pDest[bLen+2] = pLock[i];
                    bLen += 3;
                }
            }
            i = 0xffff; /* end of list */
            T5_COPYFRAMEWORD (pDest+bLen, &i);
            bLen += 2;
        }
        break;
    case T5T_SYS_NBSMLK :
#ifdef T5DEF_SMARTLOCK
        dwCaps = (pDB == NULL) ? 0 : T5VMLog_GetNbLocked (pDB); 
#else /*T5DEF_SMARTLOCK*/
        dwCaps = 0;
#endif /*T5DEF_SMARTLOCK*/
        T5_COPYFRAMEDWORD (pDest, &dwCaps);
        bLen = 4;
        break;
    default :
        if (wIndex >= T5T_SYS_SMLK)
        {
#ifdef T5DEF_SMARTLOCK
        bLen = (pDB == NULL) ? 0 : (T5_BYTE)T5VMLog_GetLockedVariable (pDB, wIndex & 0x0fff, pDest, 255);
#endif /*T5DEF_SMARTLOCK*/
        }            
        break;
    }
    return bLen;
}

static T5_BYTE  _bScan = 0;
static T5_DWORD _dwFB = 0L;

/*****************************************************************************
_GetSysInfoFB
Comm: answers a sysinfo request for FB class description
Parameters:
    wIndex (IN) class ID
    pDest (OUT) frame buffer
Return: frame length
*****************************************************************************/

static T5_BYTE _GetSysInfoFB (T5_WORD wIndex, T5_PTBYTE pDest)
{
    T5_PTCHAR szName;

    szName = NULL;
    switch (wIndex)
    {
    case T5T_SYS_FB :
        if (T5Blocks_GetFirstID (&_dwFB))
        {
            _bScan = 1; /* static */
            szName = T5Blocks_GetName (_dwFB);
        }
        else
            _bScan = 0;
        break;
    case T5T_SYS_FB_NX :
        if (_bScan == 1)
        {
            if (T5Blocks_GetNextID (&_dwFB))
                szName = T5Blocks_GetName (_dwFB);
            else
            {
#ifdef T5DEF_DLLBLOCKS
                if (!T5Bdll_StartEnumerate ())
                    _bScan = 0;
                else if (T5Bdll_GetFirstID (&_dwFB))
                {
                    _bScan = 2; /* DLL */
                    szName = T5Bdll_GetName (_dwFB);
                }
                else
                {
                    T5Bdll_StopEnumerate ();
                    _bScan = 0;
                }
#else /*T5DEF_DLLBLOCKS*/
                _bScan = 0; /* finished */
#endif /*T5DEF_DLLBLOCKS*/
            }
        }
#ifdef T5DEF_DLLBLOCKS
        else if (_bScan == 2)
        {
            if (T5Bdll_GetNextID (&_dwFB))
                szName = T5Bdll_GetName (_dwFB);
            else
            {
                T5Bdll_StopEnumerate ();
                _bScan = 0;
            }
        }
#endif /*T5DEF_DLLBLOCKS*/
        break;
    default :
        break;
    }

    if (szName == NULL)
        return 0;

    T5_STRCPY (pDest, szName);
    return (T5_BYTE)(T5_STRLEN (pDest) + 1);
}

/*****************************************************************************
T5VM_SrvSubscribe
Comm: subscribe to events
In frame:  <bNb> { type offset(2) cnx(2) Hsizeof(1) HP HN }
Out frame: <bNb> { type offset(2) rc }
Parameters:
    pDB (IN) pointer the DB - CAN BE NULL IF NO APP LOADED!!!
    pAsk (IN) input frame (list of objects to read)
    pReply (OUT) output frame (list of values + identification)
Return: reply frame langth or 0 if error
*****************************************************************************/

#ifdef T5DEF_EA

T5_WORD T5VM_SrvSubscribe (T5PTR_DB pDB, T5_PTBYTE pAsk, T5_PTBYTE pReply)
{
    T5_BYTE bNb, bSizeof;
    T5_WORD wOffset, wCnx;
    T5_WORD wLenOut, wRet;
    T5_BYTE bHP[8], bHN[8];

    if (pDB == NULL)
        return 0;

    bNb = *pAsk++;
    *pReply++ = bNb;
    wLenOut = 1 + (bNb * 4);
    while (bNb--)
    {
        /* echo type and offset */
        T5_MEMCPY (pReply, pAsk, 3);
        pReply += 3;
        /* extract main info */
        T5_COPYFRAMEWORD (&wOffset, pAsk + 1);
        T5_COPYFRAMEWORD (&wCnx, pAsk + 3);
        /* extract optional hysteresis */
#ifdef T5DEF_EAHYSTER
        bSizeof = pAsk[5];
        switch (bSizeof)
        {
        case 1 :
            *bHP = *(pAsk + 6);
            *bHN = *(pAsk + 7);
            break;
        case 2 :
            T5_COPYFRAMEWORD (bHP, pAsk + 6);
            T5_COPYFRAMEWORD (bHN, pAsk + 8);
            break;
        case 4 :
            T5_COPYFRAMEDWORD (bHP, pAsk + 6);
            T5_COPYFRAMEDWORD (bHN, pAsk + 10);
            break;
        case 8 :
            T5_COPYFRAME64 (bHP, pAsk + 6);
            T5_COPYFRAME64 (bHN, pAsk + 14);
            break;
        default : break;
        }
#else /*T5DEF_EAHYSTER*/
        bSizeof = 0;
#endif /*T5DEF_EAHYSTER*/
        /* register */
        wRet = T5EA_RegisterChangeEvent (
            pDB, wCnx, *pAsk, wOffset, 0,
            ((bSizeof) ? bHP : NULL),
            ((bSizeof) ? bHN : NULL),
            0L /* CTSeg offset */
            );
        /* return: 0 if OK */
        *pReply++ = (wRet == 0) ? 1 : 0;
        /* next */
        pAsk += (6 + 2 * pAsk[5]);
    }
    return wLenOut;
}

#endif /*T5DEF_EA*/

/*****************************************************************************
T5VM_SrvSubscribeL
Comm: subscribe to events (support for CT items)
In frame:  <bNb> { type offset(2) CToffset(4) cnx(2) Hsizeof(1) HP HN }
Out frame: <bNb> { type offset(2) CToffset(4) rc }
Parameters:
    pDB (IN) pointer the DB - CAN BE NULL IF NO APP LOADED!!!
    pAsk (IN) input frame (list of objects to read)
    pReply (OUT) output frame (list of values + identification)
Return: reply frame langth or 0 if error
*****************************************************************************/

#ifdef T5DEF_EA
#ifdef T5DEF_CTSEG

T5_WORD T5VM_SrvSubscribeL (T5PTR_DB pDB, T5_PTBYTE pAsk, T5_PTBYTE pReply)
{
    T5_BYTE bNb, bSizeof;
    T5_WORD wOffset, wCnx;
    T5_DWORD dwCTOffset;
    T5_WORD wLenOut, wRet;
    T5_BYTE bHP[8], bHN[8];

    if (pDB == NULL)
        return 0;

    bNb = *pAsk++;
    *pReply++ = bNb;
    wLenOut = 1 + (bNb * 8);
    while (bNb--)
    {
        /* echo type and offset */
        T5_MEMCPY (pReply, pAsk, 7);
        pReply += 7;
        /* extract main info */
        T5_COPYFRAMEWORD (&wOffset, pAsk + 1);
        T5_COPYFRAMEDWORD (&dwCTOffset, pAsk + 3);
        T5_COPYFRAMEWORD (&wCnx, pAsk + 7);
        /* extract optional hysteresis */
#ifdef T5DEF_EAHYSTER
        bSizeof = pAsk[9];
        switch (bSizeof)
        {
        case 1 :
            *bHP = *(pAsk + 10);
            *bHN = *(pAsk + 11);
            break;
        case 2 :
            T5_COPYFRAMEWORD (bHP, pAsk + 10);
            T5_COPYFRAMEWORD (bHN, pAsk + 12);
            break;
        case 4 :
            T5_COPYFRAMEDWORD (bHP, pAsk + 10);
            T5_COPYFRAMEDWORD (bHN, pAsk + 14);
            break;
        case 8 :
            T5_COPYFRAME64 (bHP, pAsk + 10);
            T5_COPYFRAME64 (bHN, pAsk + 18);
            break;
        default : break;
        }
#else /*T5DEF_EAHYSTER*/
        bSizeof = 0;
#endif /*T5DEF_EAHYSTER*/
        /* register */
        wRet = T5EA_RegisterChangeEvent (
            pDB, wCnx, *pAsk, wOffset, 0,
            ((bSizeof) ? bHP : NULL),
            ((bSizeof) ? bHN : NULL),
            dwCTOffset
            );
        /* return: 0 if OK */
        *pReply++ = (wRet == 0) ? 1 : 0;
        /* next */
        pAsk += (10 + 2 * pAsk[9]);
    }
    return wLenOut;
}

#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_EA*/

/*****************************************************************************
T5VM_SrvUnsubscribe
Comm: unsubscribe from events
In frame:  <bNb> { type offset(2) cnx(2) }
Out frame: none
Parameters:
    pDB (IN) pointer the DB - CAN BE NULL IF NO APP LOADED!!!
    pAsk (IN) input frame (list of objects to read)
Return: OK or error
*****************************************************************************/

#ifdef T5DEF_EA

T5_WORD T5VM_SrvUnsubscribe (T5PTR_DB pDB, T5_PTBYTE pAsk)
{
    T5_BYTE bNb;
    T5_WORD wOffset, wCnx;

    if (pDB == NULL)
        return T5RET_OK;

    bNb = *pAsk++;
    while (bNb--)
    {
        T5_COPYFRAMEWORD (&wOffset, pAsk + 1);
        T5_COPYFRAMEWORD (&wCnx, pAsk + 3);
        T5EA_UnregisterChangeEvent (pDB, wCnx, *pAsk, wOffset,
                                    0L /* CTSeg offset */
                                    );
        pAsk += 5;
    }
    return T5RET_OK;
}

#endif /*T5DEF_EA*/

/*****************************************************************************
T5VM_SrvUnsubscribeL
Comm: unsubscribe from events - support for CTSeg items
In frame:  <bNb> { type offset(2) CToffset(4) cnx(2) }
Out frame: none
Parameters:
    pDB (IN) pointer the DB - CAN BE NULL IF NO APP LOADED!!!
    pAsk (IN) input frame (list of objects to read)
Return: OK or error
*****************************************************************************/

#ifdef T5DEF_EA
#ifdef T5DEF_CTSEG

T5_WORD T5VM_SrvUnsubscribeL (T5PTR_DB pDB, T5_PTBYTE pAsk)
{
    T5_BYTE bNb;
    T5_WORD wOffset, wCnx;
    T5_DWORD dwCTOffset;

    if (pDB == NULL)
        return T5RET_OK;

    bNb = *pAsk++;
    while (bNb--)
    {
        T5_COPYFRAMEWORD (&wOffset, pAsk + 1);
        T5_COPYFRAMEDWORD (&dwCTOffset, pAsk + 3);
        T5_COPYFRAMEWORD (&wCnx, pAsk + 7);
        T5EA_UnregisterChangeEvent (pDB, wCnx, *pAsk, wOffset, dwCTOffset);
        pAsk += 9;
    }
    return T5RET_OK;
}

#endif /*T5DEF_CTSEG*/
#endif /*T5DEF_EA*/

/*****************************************************************************
T5VM_SrvPollTable
Comm: poll table segment
In frame:  <bTable> <dwOffset> <wSize> <dwCrc>
Out frame: <bTable> <dwOffset> <wSize> <data>
Parameters:
    pDB (IN) pointer the DB - CAN BE NULL IF NO APP LOADED!!!
    pAsk (IN) input frame (list of objects to read)
Return: reply frame langth or 0 if error
*****************************************************************************/

T5_WORD T5VM_SrvPollTable (T5PTR_DB pDB, T5_PTBYTE pAsk, T5_PTBYTE pReply)
{
    T5_BYTE bTable;
    T5_WORD wSize;
    T5_DWORD dwOffset;
    T5_DWORD dwCrc;
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBPRIVATE pPrivate;
    T5_PTBYTE pData;
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODEINIT pInit;

    /* parse request */
    bTable = pAsk[0];
    T5_COPYFRAMEDWORD (&dwOffset, pAsk + 1);
    T5_COPYFRAMEWORD (&wSize, pAsk + 5);
    T5_COPYFRAMEDWORD (&dwCrc, pAsk + 7);

    /* no DB? */
    if (pDB == NULL)
    {
        *pReply = T5RET_NOAPP;
        return 1;
    }
    /* check args */
    if ((bTable < 254 && bTable >= T5MAX_TABLE)
        || wSize > (T5MAX_FRAME - 24))
    {
        *pReply = T5RET_BADREQUEST;
        return 1;
    }
    /* check CRC */
    pStatus = T5GET_DBSTATUS(pDB);
    if (pStatus->dwAppDataCRC != dwCrc)
    {
        *pReply = T5RET_BADCRC;
        return 1;
    }
    /* header */
    pReply[0] = T5RET_OK;
    pReply[1] = bTable;
    T5_COPYFRAMEDWORD (pReply + 2, &dwOffset);
    T5_COPYFRAMEWORD (pReply + 6, &wSize);
    /* data */
    if (bTable == 255) /* DB header */
        pData = (T5_PTBYTE)pDB;
    else if (bTable == 254) /* strings lengths */
    {
        pPrivate = T5GET_DBPRIVATE(pDB);
        pHeader = (T5PTR_CODEHEADER)(pPrivate->pCode);
        pInit = T5GET_CODEINIT(pHeader);
        pData = T5GET_CODESTRLEN(pHeader, pInit);
    }
    else
        pData = (T5_PTBYTE)(pDB[bTable].pData);
    T5_MEMCPY (pReply + 8, pData + dwOffset, wSize);
    return wSize + 8;
}

/* eof **********************************************************************/
