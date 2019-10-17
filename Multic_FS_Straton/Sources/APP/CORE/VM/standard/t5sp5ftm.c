/*****************************************************************************
T5sp5ftm.c : SP5 Safe protocol - file transfer layer - MASTER

DO NOT ALTER THIS !

(c) COPALP 2007
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_SP5

/****************************************************************************/
/* file access wrappers */

#ifndef SP5FF_EXIST
#define SP5FF_EXIST T5File_Exist
#endif /*SP5FF_EXIST*/

#ifndef SP5FF_COPY
#define SP5FF_COPY T5File_Copy
#endif /*SP5FF_COPY*/

#ifndef SP5FF_GETSIZE
#define SP5FF_GETSIZE T5File_GetSize
#endif /*SP5FF_GETSIZE*/

#ifndef SP5FF_OPEN
#define SP5FF_OPEN T5File_Open
#endif /*SP5FF_OPEN*/

#ifndef SP5FF_OPENA
#define SP5FF_OPENA T5File_OpenAppend
#endif /*SP5FF_OPENA*/

#ifndef SP5FF_CLOSE
#define SP5FF_CLOSE T5File_Close
#endif /*SP5FF_CLOSE*/

#ifndef SP5FF_READ
#define SP5FF_READ T5File_Read
#endif /*SP5FF_READ*/

#ifndef SP5FF_WRITE
#define SP5FF_WRITE T5File_Write
#endif /*SP5FF_WRITE*/

#ifndef SP5FF_DELETE
#define SP5FF_DELETE T5File_Delete
#endif /*SP5FF_DELETE*/

#ifndef SP5FF_SEEKTO
#define SP5FF_SEEKTO T5SP5Ftls_SeekTo
#endif /*SP5FF_SEEKTO*/

/****************************************************************************/

static T5_BOOL _T5SP5Ftlm_OnOpenRead (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                      T5_PTBYTE answer, T5_WORD wUnit,
                                      T5_DWORD dwTimeStamp, T5_PTBYTE bRC);
static T5_BOOL _T5SP5Ftlm_OnRead_1 (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                    T5_PTBYTE answer, T5_WORD wUnit,
                                    T5_DWORD dwTimeStamp, T5_PTBYTE bRC);
static T5_BOOL _T5SP5Ftlm_OnCloseRead (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                       T5_PTBYTE answer, T5_PTBYTE bRC);
static T5_BOOL _T5SP5Ftlm_OnOpenWrite (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                       T5_PTBYTE answer, T5_WORD wUnit,
                                       T5_DWORD dwTimeStamp, T5_PTBYTE bRC);
static T5_BOOL _T5SP5Ftlm_OnWrite_1 (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                     T5_PTBYTE answer, T5_WORD wUnit,
                                     T5_DWORD dwTimeStamp, T5_PTBYTE bRC);
static T5_BOOL _T5SP5Ftlm_OnCloseWrite (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                        T5_PTBYTE answer, T5_PTBYTE bRC);

/****************************************************************************/

T5_BOOL T5SP5Ftlm_Open (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                        T5_WORD wFlags, T5_PTCHAR szConfig,
                        T5_DWORD dwTimeout, T5_WORD wNbTrial)
{
    if (pFT != NULL)
        T5_MEMSET (pFT, 0, sizeof (T5STR_SP5FTLM));

    return T5SP5Tlm_Open (pTL, wFlags, szConfig, dwTimeout, wNbTrial);
}

void T5SP5Ftlm_Close (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT)
{
    T5_UNUSED(pFT);
    T5SP5Tlm_Close (pTL);
}

T5_BOOL T5SP5Ftlm_Send (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                        T5_WORD wUnit, T5_WORD wLen, T5_PTBYTE pData,
                        T5_DWORD dwTimeStamp)
{
    if (pFT != NULL && pFT->bState != SP5FTLM_IDLE)
        return FALSE;

    return T5SP5Tlm_Send (pTL, wUnit, wLen, pData, dwTimeStamp);
}

void T5SP5Ftlm_AbortRequest (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT)
{
    if (pFT != NULL)
        T5_MEMSET (pFT, 0, sizeof (T5STR_SP5FTLM));

    T5SP5Tlm_AbortRequest (pTL);
}

T5_BOOL T5SP5Ftlm_SendFile (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                            T5_PTCHAR szLocal, T5_PTCHAR szRemote,
                            T5_PTCHAR szPassword,
                            T5_WORD wUnit, T5_DWORD dwTimeStamp)
{
    T5_PTBYTE question;
    T5_WORD wLen;

    if (pFT == NULL || pFT->bState != SP5FTLM_IDLE
        || pTL->bState != SP5TLM_READY || SP5_ISCAST(wUnit)
        || T5_STRLEN(szLocal) > SP5_MAXPATH
        || T5_STRLEN(szRemote) > SP5_MAXPATH
        || T5_STRLEN(szPassword) > SP5_MAXPSW
        || !SP5FF_EXIST (szLocal))
    {
        return FALSE;
    }

    T5_MEMSET (pFT->szPsw, 0, sizeof (pFT->szPsw));
    T5_MEMCPY (pFT->szPsw, szPassword, T5_STRLEN(szPassword));

    question = pTL->dataApp;
    T5_MEMCPY (question, SP5FT_HEAD, 2);
    question[2] = SP5FT_OW;
    wLen = 3;
    T5_MEMCPY (question+wLen, pFT->szPsw, sizeof (pFT->szPsw));
    wLen += sizeof (pFT->szPsw);
    question[wLen] = (T5_BYTE)T5_STRLEN (szRemote);
    T5_STRCPY (question+wLen+1, szRemote);
    wLen += (1 + question[wLen]);

    if (!T5SP5Tlm_Send (pTL, wUnit, wLen, NULL, dwTimeStamp))
        return FALSE;

    pFT->dwSize = 0L;
    pFT->dwTotal = SP5FF_GETSIZE (szLocal);
    T5_STRCPY (pFT->szLocal, szLocal);
    T5_STRCPY (pFT->szRemote, szRemote);
    pFT->bState = SP5FTLM_WRITE;
    return TRUE;
}

T5_BOOL T5SP5Ftlm_RcvFile (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                           T5_PTCHAR szLocal, T5_PTCHAR szRemote,
                           T5_PTCHAR szPassword,
                           T5_WORD wUnit, T5_DWORD dwTimeStamp)
{
    T5_PTBYTE question;
    T5_DWORD dwSize;
    T5_WORD wLen;

    if (pFT == NULL || pFT->bState != SP5FTLM_IDLE
        || pTL->bState != SP5TLM_READY || SP5_ISCAST(wUnit)
        || T5_STRLEN(szLocal) > SP5_MAXPATH
        || T5_STRLEN(szRemote) > SP5_MAXPATH
        || T5_STRLEN(szPassword) > SP5_MAXPSW)
    {
        return FALSE;
    }

    T5_MEMSET (pFT->szPsw, 0, sizeof (pFT->szPsw));
    T5_MEMCPY (pFT->szPsw, szPassword, T5_STRLEN(szPassword));

    dwSize = SP5FF_GETSIZE(szLocal);

    question = pTL->dataApp;
    T5_MEMCPY (question, SP5FT_HEAD, 2);
    question[2] = SP5FT_OR;
    wLen = 3;
    T5_MEMCPY (question+wLen, pFT->szPsw, sizeof (pFT->szPsw));
    wLen += sizeof (pFT->szPsw);
    question[wLen] = (T5_BYTE)T5_STRLEN (szRemote);
    T5_STRCPY (question+wLen+1, szRemote);
    wLen += (1 + question[wLen]);
    T5_COPYFRAMEDWORD (question+wLen, &dwSize);
    wLen += 4;

    if (!T5SP5Tlm_Send (pTL, wUnit, wLen, NULL, dwTimeStamp))
        return FALSE;

    pFT->dwSize = pFT->dwTotal = 0L;
    T5_STRCPY (pFT->szLocal, szLocal);
    T5_STRCPY (pFT->szRemote, szRemote);
    pFT->bState = SP5FTLM_READ;
    return TRUE;
}

T5_WORD T5SP5Ftlm_GetAnswer (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                             T5_WORD wMaxLen, T5_PTBYTE pData,
                             T5_PTWORD pwUnit, T5_DWORD dwTimeStamp,
                             T5_PTBOOL pbOK)
{
    T5_WORD wRet, wUnit;
    T5_BOOL bOK, bDone;
    T5_BYTE bRC;

    wRet = T5SP5Tlm_GetAnswer (pTL, wMaxLen, pData, &wUnit, dwTimeStamp, &bOK);
    if (pbOK != NULL)
        *pbOK = bOK;
    if (pwUnit != NULL)
        *pwUnit = wUnit;
    if (pFT == NULL || pFT->bState == SP5FTLM_IDLE || (bOK && wRet == 0))
        return wRet;

    if (wRet < 4 || T5_MEMCMP (pData, SP5FT_HEAD, 2) != 0)
    {
        pFT->bState = SP5FTLM_IDLE;
        *pbOK = FALSE;
        return 0;
    }

    if (pData[3] != SP5FT_OK) /* FT protocol error */
    {
        pFT->bState = SP5FTLM_IDLE;
        *pData = pData[3];
        return 1;
    }

    bDone = FALSE;
    bRC = 0xff;
    switch (pData[2])
    {
    case SP5FT_OR :
        bDone = _T5SP5Ftlm_OnOpenRead (pTL, pFT, pData+4, wUnit, dwTimeStamp, &bRC);
        break;
    case SP5FT_R1 :
        bDone = _T5SP5Ftlm_OnRead_1 (pTL, pFT, pData+4, wUnit, dwTimeStamp, &bRC);
        break;
    case SP5FT_CR :
        bDone = _T5SP5Ftlm_OnCloseRead (pTL, pFT, pData+4, &bRC);
        break;
    case SP5FT_OW :
        bDone = _T5SP5Ftlm_OnOpenWrite (pTL, pFT, pData+4, wUnit, dwTimeStamp, &bRC);
        break;
    case SP5FT_W1 :
        bDone = _T5SP5Ftlm_OnWrite_1 (pTL, pFT, pData+4, wUnit, dwTimeStamp, &bRC);
        break;
    case SP5FT_CW :
        bDone = _T5SP5Ftlm_OnCloseWrite (pTL, pFT, pData+4, &bRC);
        break;
    default :
        bDone = TRUE;
        bRC = SP5FT_ERRFNC;
        return 1;
    }

    if (bDone)
    {
        pFT->bState = SP5FTLM_IDLE;
        *pData = bRC;
        return 1;
    }

    return 0;
}

/****************************************************************************/

static T5_BOOL _T5SP5Ftlm_OnOpenRead (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                      T5_PTBYTE answer, T5_WORD wUnit,
                                      T5_DWORD dwTimeStamp, T5_PTBYTE bRC)
{
    T5_DWORD dwSize, dwSizeLocal, dwOffset, dwCrc;
    T5_BYTE question[20];
    T5_WORD wLen, wSize;

    *bRC = SP5FT_OK;
    T5_COPYFRAMEDWORD (&dwSize, answer);
    T5_COPYFRAMEDWORD (&dwCrc, answer+4);
    pFT->dwTotal = dwSize;
    dwSizeLocal = SP5FF_GETSIZE(pFT->szLocal);

    if (dwCrc != 0 && dwSize >= dwSizeLocal
        && dwCrc == T5SP5Ftls_FileCrcUpTo (pFT->szLocal, dwSizeLocal))
    {
        pFT->dwSize = dwSizeLocal;
        if (pFT->dwSize == pFT->dwTotal)
            return TRUE;
    }

    if (pFT->dwSize == 0L)
        SP5FF_DELETE(pFT->szLocal);

    if (pFT->dwTotal == 0L)
    {
        *bRC = SP5FT_ERRIO;
        return TRUE;
    }

    dwOffset = pFT->dwSize;
    dwSize = (pFT->dwTotal - dwOffset);
    if (dwSize > SP5_MAXFT)
        dwSize = SP5_MAXFT;
    wSize = (T5_WORD)dwSize;

    T5_MEMCPY (question, SP5FT_HEAD, 2);
    question[2] = SP5FT_R1;
    wLen = 3;
    T5_MEMCPY (question+wLen, pFT->szPsw, sizeof (pFT->szPsw));
    wLen += sizeof (pFT->szPsw);
    T5_COPYFRAMEDWORD (question+wLen, &dwOffset);
    wLen += 4;
    T5_COPYFRAMEWORD (question+wLen, &wSize);
    wLen += 2;
    
    if (!T5SP5Tlm_Send (pTL, wUnit, wLen, question, dwTimeStamp))
    {
        *bRC = SP5FT_ERRCOM;
        return TRUE;
    }

    return FALSE;
}

static T5_BOOL _T5SP5Ftlm_OnRead_1 (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                    T5_PTBYTE answer, T5_WORD wUnit,
                                    T5_DWORD dwTimeStamp, T5_PTBYTE bRC)
{
    T5_DWORD dwOffset, dwSize;
    T5_WORD wSize, wLen;
    T5_LONG f;
    T5_PTBYTE question;

    *bRC = SP5FT_OK;
    T5_COPYFRAMEDWORD (&dwOffset, answer);
    T5_COPYFRAMEWORD (&wSize, answer+4);
    answer += 6;

    if (dwOffset != pFT->dwSize)
    {
        *bRC = SP5FT_ERRCOM;
        return TRUE;
    }

    if (pFT->dwSize == 0L)
        f = SP5FF_OPEN (pFT->szLocal, TRUE);
    else
        f = SP5FF_OPENA (pFT->szLocal);
    if (f == 0L || !SP5FF_WRITE (f, answer, (T5_LONG)wSize))
    {
        *bRC = SP5FT_ERRIO;
        return TRUE;
    }
    if (f)
        SP5FF_CLOSE (f);

    question = pTL->dataApp;

    pFT->dwSize += (T5_DWORD)wSize;
    if (pFT->dwSize < pFT->dwTotal)
    {
        dwOffset = pFT->dwSize;
        dwSize = (pFT->dwTotal - dwOffset);
        if (dwSize > SP5_MAXFT)
            dwSize = SP5_MAXFT;
        wSize = (T5_WORD)dwSize;

        T5_MEMCPY (question, SP5FT_HEAD, 2);
        question[2] = SP5FT_R1;
        wLen = 3;
        T5_MEMCPY (question+wLen, pFT->szPsw, sizeof (pFT->szPsw));
        wLen += sizeof (pFT->szPsw);
        T5_COPYFRAMEDWORD (question+wLen, &dwOffset);
        wLen += 4;
        T5_COPYFRAMEWORD (question+wLen, &wSize);
        wLen += 2;
    }
    else
    {
        T5_MEMCPY (question, SP5FT_HEAD, 2);
        question[2] = SP5FT_CR;
        wLen = 3;
        T5_MEMCPY (question+wLen, pFT->szPsw, sizeof (pFT->szPsw));
        wLen += sizeof (pFT->szPsw);
        question[wLen] = (T5_BYTE)T5_STRLEN (pFT->szRemote);
        T5_STRCPY (question+wLen+1, pFT->szRemote);
        wLen += (1 + question[wLen]);
    }
    if (!T5SP5Tlm_Send (pTL, wUnit, wLen, NULL, dwTimeStamp))
    {
        *bRC = SP5FT_ERRCOM;
        return TRUE;
    }

    return FALSE;
}

static T5_BOOL _T5SP5Ftlm_OnCloseRead (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                       T5_PTBYTE answer, T5_PTBYTE bRC)
{
    T5_DWORD dwSize, dwCrc;
    T5_UNUSED(pTL);

    T5_COPYFRAMEDWORD(&dwSize, answer);
    T5_COPYFRAMEDWORD(&dwCrc, answer+4);

    if (dwSize != SP5FF_GETSIZE(pFT->szLocal)
        || dwCrc != T5SP5Ftls_FileCrcUpTo (pFT->szLocal, dwSize))
    {
        *bRC = SP5FT_ERRIO;
        return TRUE;
    }

    *bRC = SP5FT_OK;
    return TRUE;
}

/****************************************************************************/

static T5_BOOL _T5SP5Ftlm_OnOpenWrite (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                       T5_PTBYTE answer, T5_WORD wUnit,
                                       T5_DWORD dwTimeStamp, T5_PTBYTE bRC)
{
    T5_DWORD dwSize, dwCrc, dwOffset;
    T5_WORD wSize, wLen;
    T5_PTBYTE question;
    T5_LONG f;

    *bRC = SP5FT_OK;
    T5_COPYFRAMEDWORD (&dwSize, answer);
    T5_COPYFRAMEDWORD (&dwCrc, answer+4);

    if (dwSize <= pFT->dwTotal
        && T5SP5Ftls_FileCrcUpTo (pFT->szLocal, dwSize) == dwCrc)
    {
        pFT->dwSize = dwSize;
        if (pFT->dwSize == pFT->dwTotal)
        {
            dwSize = SP5FF_GETSIZE (pFT->szLocal);
            dwCrc = T5SP5Ftls_FileCrcUpTo (pFT->szLocal, dwSize);

            question = pTL->dataApp;
            T5_MEMCPY (question, SP5FT_HEAD, 2);
            question[2] = SP5FT_CW;
            wLen = 3;
            T5_MEMCPY (question+wLen, pFT->szPsw, sizeof (pFT->szPsw));
            wLen += sizeof (pFT->szPsw);
            question[wLen] = (T5_BYTE)T5_STRLEN (pFT->szRemote);
            T5_STRCPY (question+wLen+1, pFT->szRemote);
            wLen += (1 + question[wLen]);
            T5_COPYFRAMEDWORD (question+wLen, &dwSize);
            wLen += 4;
            T5_COPYFRAMEDWORD (question+wLen, &dwCrc);
            wLen += 4;

            if (!T5SP5Tlm_Send (pTL, wUnit, wLen, NULL, dwTimeStamp))
            {
                *bRC = SP5FT_ERRCOM;
                return TRUE;
            }
            return FALSE;
        }
    }

    dwOffset = pFT->dwSize;
    dwSize = pFT->dwTotal - dwOffset;
    if (dwSize > SP5_MAXFT)
        dwSize = SP5_MAXFT;
    wSize = (T5_WORD)dwSize;

    pFT->dwSize += dwSize;

    question = pTL->dataApp;
    T5_MEMCPY (question, SP5FT_HEAD, 2);
    question[2] = SP5FT_W1;
    wLen = 3;
    T5_MEMCPY (question+wLen, pFT->szPsw, sizeof (pFT->szPsw));
    wLen += sizeof (pFT->szPsw);
    T5_COPYFRAMEDWORD (question+wLen, &dwOffset);
    wLen += 4;
    T5_COPYFRAMEWORD (question+wLen, &wSize);
    wLen += 2;

    f = SP5FF_OPEN (pFT->szLocal, FALSE);
    if (f ==  0L)
    {
        *bRC = SP5FT_ERRIO;
        return TRUE;
    }

    if ((dwOffset != 0L && !T5SP5Ftls_SeekTo (f, dwOffset))
        || !SP5FF_READ (f, question + wLen, (T5_LONG)wSize))
    {
        SP5FF_CLOSE (f);
        *bRC = SP5FT_ERRIO;
        return TRUE;
    }

    SP5FF_CLOSE (f);

    if (!T5SP5Tlm_Send (pTL, wUnit, (T5_WORD)(wLen+wSize), NULL, dwTimeStamp))
    {
        *bRC = SP5FT_ERRCOM;
        return TRUE;
    }

    return FALSE;
}

static T5_BOOL _T5SP5Ftlm_OnWrite_1 (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                     T5_PTBYTE answer, T5_WORD wUnit,
                                     T5_DWORD dwTimeStamp, T5_PTBYTE bRC)
{
    T5_DWORD dwOffset, dwSize, dwCrc;
    T5_WORD wSize, wLen;
    T5_LONG f;
    T5_PTBYTE question;
    T5_UNUSED(answer);

    *bRC = SP5FT_OK;

    question = pTL->dataApp;
    if (pFT->dwSize == pFT->dwTotal)
    {
        dwSize = SP5FF_GETSIZE (pFT->szLocal);
        dwCrc = T5SP5Ftls_FileCrcUpTo (pFT->szLocal, dwSize);

        T5_MEMCPY (question, SP5FT_HEAD, 2);
        question[2] = SP5FT_CW;
        wLen = 3;
        T5_MEMCPY (question+wLen, pFT->szPsw, sizeof (pFT->szPsw));
        wLen += sizeof (pFT->szPsw);
        question[wLen] = (T5_BYTE)T5_STRLEN (pFT->szRemote);
        T5_STRCPY (question+wLen+1, pFT->szRemote);
        wLen += (1 + question[wLen]);
        T5_COPYFRAMEDWORD (question+wLen, &dwSize);
        wLen += 4;
        T5_COPYFRAMEDWORD (question+wLen, &dwCrc);
        wLen += 4;
    }
    else
    {
        dwOffset = pFT->dwSize;
        dwSize = pFT->dwTotal - dwOffset;
        if (dwSize > SP5_MAXFT)
            dwSize = SP5_MAXFT;
        wSize = (T5_WORD)dwSize;
        pFT->dwSize += dwSize;

        T5_MEMCPY (question, SP5FT_HEAD, 2);
        question[2] = SP5FT_W1;
        wLen = 3;
        T5_MEMCPY (question+wLen, pFT->szPsw, sizeof (pFT->szPsw));
        wLen += sizeof (pFT->szPsw);
        T5_COPYFRAMEDWORD (question+wLen, &dwOffset);
        wLen += 4;
        T5_COPYFRAMEWORD (question+wLen, &wSize);
        wLen += 2;

        f = SP5FF_OPEN (pFT->szLocal, FALSE);
        if (f ==  0L)
        {
            *bRC = SP5FT_ERRIO;
            return TRUE;
        }

        if (!T5SP5Ftls_SeekTo (f, dwOffset)
            || !SP5FF_READ (f, question + wLen, (T5_LONG)wSize))
        {
            SP5FF_CLOSE (f);
            *bRC = SP5FT_ERRIO;
            return TRUE;
        }
        wLen += wSize;
        SP5FF_CLOSE (f);
    }

    if (!T5SP5Tlm_Send (pTL, wUnit, wLen, NULL, dwTimeStamp))
    {
        *bRC = SP5FT_ERRCOM;
        return TRUE;
    }

    return FALSE;
}

static T5_BOOL _T5SP5Ftlm_OnCloseWrite (T5PTR_SP5TL pTL, T5PTR_SP5FTLM pFT,
                                        T5_PTBYTE answer, T5_PTBYTE bRC)
{
    T5_DWORD dwSize, dwCrc;
    T5_UNUSED(pTL);

    T5_COPYFRAMEDWORD (&dwSize, answer);
    T5_COPYFRAMEDWORD (&dwCrc, answer+4);

    if (dwSize != SP5FF_GETSIZE (pFT->szLocal)
        || dwCrc != T5SP5Ftls_FileCrcUpTo (pFT->szLocal, dwSize))
    {
        *bRC = SP5FT_ERRIO;
        return TRUE;
    }

    *bRC = SP5FT_OK;
    return TRUE;
}

/****************************************************************************/

#endif /*T5DEF_SP5*/

/* eof **********************************************************************/
