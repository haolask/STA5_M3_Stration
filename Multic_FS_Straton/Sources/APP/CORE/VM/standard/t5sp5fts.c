/*****************************************************************************
T5sp5fts.c : SP5 Safe protocol - file transfer layer - SLAVE

DO NOT ALTER THIS !

(c) COPALP 2007
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_SP5

/****************************************************************************/
/* temp file names */

#ifndef SP5_FTEMPR
#define SP5_FTEMPR  (T5_PTCHAR)"sp5ftr.tmp"
#endif /*SP5_FTEMPR*/

#ifndef SP5_FTEMPW
#define SP5_FTEMPW  (T5_PTCHAR)"sp5ftw.tmp"
#endif /*SP5_FTEMPW*/

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

static void _T5SP5Ftls_SendError (T5PTR_SP5TL pTL, T5_BYTE bFunc,
                                  T5_BYTE bError);
static void _T5SP5Ftls_OpenRead (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                 T5_PTBYTE pQuestion);
static void _T5SP5Ftls_CloseRead (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                  T5_PTBYTE pQuestion);
static void _T5SP5Ftls_Read_1 (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                               T5_PTBYTE pQuestion);
static void _T5SP5Ftls_OpenWrite (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                  T5_PTBYTE pQuestion);
static void _T5SP5Ftls_CloseWrite (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                   T5_PTBYTE pQuestion);
static void _T5SP5Ftls_Write_1 (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                T5_PTBYTE pQuestion);

/****************************************************************************/

T5_BOOL T5SP5Ftls_Open (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                        T5_PTCHAR szPassword, T5_WORD wFlags, T5_WORD wUnit,
                        T5_PTCHAR szConfig)
{
    if (pFT != NULL)
    {
        T5_MEMSET (pFT, 0, sizeof (T5STR_SP5FTLS));
        T5SP5Ftls_SetPassword (pFT, szPassword);
    }

    return T5SP5Tls_Open (pTL, wFlags, wUnit, szConfig);
}

void T5SP5Ftls_SetPassword (T5PTR_SP5FTLS pFT, T5_PTCHAR szPassword)
{
    T5_WORD wLen;

    if (pFT == NULL)
        return;

    T5_MEMSET (pFT->szPsw, 0, sizeof (pFT->szPsw));
    if (szPassword != NULL)
    {
        wLen = (T5_WORD)T5_STRLEN (szPassword);
        if (wLen > sizeof (pFT->szPsw))
            wLen = sizeof (pFT->szPsw);
        T5_MEMCPY (pFT->szPsw, szPassword, wLen);
    }
}

void T5SP5Ftls_Close (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT)
{
    T5_UNUSED(pFT);
    T5SP5Tls_Close (pTL);
}

T5_BOOL T5SP5Ftls_Send (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                        T5_WORD wLen, T5_PTBYTE pData)
{
    T5_UNUSED(pFT);
    return T5SP5Tls_Send (pTL, wLen, pData);
}

void T5SP5Ftls_DontSend (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT)
{
    T5_UNUSED(pFT);
    T5SP5Tls_DontSend (pTL);
}

T5_WORD T5SP5Ftls_Listen (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT, T5_WORD wMaxLen,
                          T5_PTBYTE pData, T5_PTWORD pwUnit)
{
    T5_WORD wRet, wUnit;
    T5_BYTE bFunc;

    wRet = T5SP5Tls_Listen (pTL, wMaxLen, pData, &wUnit);
    if (pwUnit != NULL)
        *pwUnit = wUnit;
    if (pFT == NULL || wRet < 2 || T5_MEMCMP (pData, SP5FT_HEAD, 2) != 0
        || SP5_ISCAST(wUnit))
    {
        return wRet;
    }

    /* message header */
    pData += 2;
    bFunc = *pData++;

    /* password */
    if (T5_MEMCMP (pData, pFT->szPsw, sizeof (pFT->szPsw)) != 0)
    {
        _T5SP5Ftls_SendError (pTL, bFunc, SP5FT_ERRPSW);
        return 0;
    }
    pData += sizeof (pFT->szPsw);

    /* process */
    switch (bFunc)
    {
    case SP5FT_OR :
        _T5SP5Ftls_OpenRead (pTL, pFT, pData);
        break;
    case SP5FT_CR :
        _T5SP5Ftls_CloseRead (pTL, pFT, pData);
        break;
    case SP5FT_R1 :
        _T5SP5Ftls_Read_1 (pTL, pFT, pData);
        break;
    case SP5FT_OW :
        _T5SP5Ftls_OpenWrite (pTL, pFT, pData);
        break;
    case SP5FT_CW :
        _T5SP5Ftls_CloseWrite (pTL, pFT, pData);
        break;
    case SP5FT_W1 :
        _T5SP5Ftls_Write_1 (pTL, pFT, pData);
        break;
    default :
        _T5SP5Ftls_SendError (pTL, bFunc, SP5FT_ERRFNC);
        break;
    }
    return 0;
}

static void _T5SP5Ftls_SendError (T5PTR_SP5TL pTL, T5_BYTE bFunc,
                                  T5_BYTE bError)
{
    T5_BYTE answer[4];

    T5_MEMCPY (answer, SP5FT_HEAD, 2);
    answer[2] = bFunc;
    answer[3] = bError;
    T5SP5Tls_Send (pTL, 4, answer);
}

/****************************************************************************/

static void _T5SP5Ftls_OpenRead (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                 T5_PTBYTE pQuestion)
{
    T5_DWORD dwSize, dwCrc, dwKnownSize;
    T5_BYTE answer[12];

    T5_MEMSET (pFT->szPath, 0, sizeof (pFT->szPath));
    T5_MEMCPY (pFT->szPath, pQuestion+1, *pQuestion);

    if (!SP5FF_EXIST (pFT->szPath)
        || !SP5FF_COPY (pFT->szPath, SP5_FTEMPR))
    {
        T5_MEMSET (pFT->szPath, 0, sizeof (pFT->szPath));
        pFT->bMode = SP5FTLS_IDLE;
        _T5SP5Ftls_SendError (pTL, SP5FT_OR, SP5FT_ERRIO);
        return;
    }

    pQuestion += (*pQuestion + 1);
    T5_COPYFRAMEDWORD (&dwKnownSize, pQuestion);
    dwSize = SP5FF_GETSIZE (SP5_FTEMPR);
    dwCrc = T5SP5Ftls_FileCrcUpTo (SP5_FTEMPR, dwKnownSize);

    pFT->bMode = SP5FTLS_READ;

    T5_MEMCPY (answer, SP5FT_HEAD, 2);
    answer[2] = SP5FT_OR;
    answer[3] = 0;
    T5_COPYFRAMEDWORD (answer+4, &dwSize);
    T5_COPYFRAMEDWORD (answer+8, &dwCrc);
    T5SP5Tls_Send (pTL, 12, answer);
}

static void _T5SP5Ftls_CloseRead (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                  T5_PTBYTE pQuestion)
{
    T5_DWORD dwCrc, dwSize;
    T5_BYTE answer[12];

    if ((pFT->bMode != SP5FTLS_READ)
        || ((T5_WORD)(*pQuestion) != (T5_WORD)T5_STRLEN(pFT->szPath))
        || (T5_MEMCMP (pQuestion + 1, pFT->szPath, *pQuestion) != 0))
    {
        _T5SP5Ftls_SendError (pTL, SP5FT_CR, SP5FT_ERRFNC);
        return;
    }

    dwSize = SP5FF_GETSIZE (SP5_FTEMPR);
    dwCrc = T5SP5Ftls_FileCrcUpTo (SP5_FTEMPR, dwSize);

    SP5FF_DELETE (SP5_FTEMPR);
    T5_MEMSET (pFT->szPath, 0, sizeof (pFT->szPath));
    pFT->bMode = SP5FTLS_IDLE;

    T5_MEMCPY (answer, SP5FT_HEAD, 2);
    answer[2] = SP5FT_CR;
    answer[3] = 0;
    T5_COPYFRAMEDWORD (answer+4, &dwSize);
    T5_COPYFRAMEDWORD (answer+8, &dwCrc);
    T5SP5Tls_Send (pTL, 12, answer);
}

static void _T5SP5Ftls_Read_1 (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                               T5_PTBYTE pQuestion)
{
    T5_DWORD dwSize, dwOffset;
    T5_WORD wLen;
    T5_LONG f;
    T5_PTBYTE answer;

    if (pFT->bMode != SP5FTLS_READ)
    {
        _T5SP5Ftls_SendError (pTL, SP5FT_R1, SP5FT_ERRFNC);
        return;
    }

    dwSize = SP5FF_GETSIZE (SP5_FTEMPR);
    T5_COPYFRAMEDWORD (&dwOffset, pQuestion);
    T5_COPYFRAMEWORD (&wLen, pQuestion+4);
    if ((dwOffset + (T5_DWORD)wLen) > dwSize)
    {
        _T5SP5Ftls_SendError (pTL, SP5FT_R1, SP5FT_ERRARG);
        return;
    }

    f = SP5FF_OPEN (SP5_FTEMPR, FALSE);
    if (!f)
    {
        _T5SP5Ftls_SendError (pTL, SP5FT_R1, SP5FT_ERRIO);
        return;
    }

    if (!SP5FF_SEEKTO (f, dwOffset))
    {
        SP5FF_CLOSE (f);
        _T5SP5Ftls_SendError (pTL, SP5FT_R1, SP5FT_ERRIO);
        return;
    }
    
    answer = pTL->dataApp;
    T5_MEMCPY (answer, SP5FT_HEAD, 2);
    answer[2] = SP5FT_R1;
    answer[3] = 0;
    T5_COPYFRAMEDWORD (answer+4, &dwOffset);
    T5_COPYFRAMEWORD (answer+8, &wLen);

    if (!SP5FF_READ (f, answer+10, (T5_LONG)wLen))
    {
        SP5FF_CLOSE (f);
        _T5SP5Ftls_SendError (pTL, SP5FT_R1, SP5FT_ERRIO);
        return;
    }

    SP5FF_CLOSE (f);
    T5SP5Tls_Send (pTL, (T5_WORD)(wLen+10), NULL); /* app data buffer filled */
}

/****************************************************************************/

static void _T5SP5Ftls_OpenWrite (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                  T5_PTBYTE pQuestion)
{
    T5_DWORD dwSize, dwCrc;
    T5_BYTE answer[12];

    T5_MEMSET (pFT->szPath, 0, sizeof (pFT->szPath));
    T5_MEMCPY (pFT->szPath, pQuestion+1, *pQuestion);

    dwSize = SP5FF_GETSIZE (SP5_FTEMPW);
    dwCrc = T5SP5Ftls_FileCrcUpTo (SP5_FTEMPW, dwSize);

    pFT->bMode = SP5FTLS_WRITE;

    T5_MEMCPY (answer, SP5FT_HEAD, 2);
    answer[2] = SP5FT_OW;
    answer[3] = 0;
    T5_COPYFRAMEDWORD (answer+4, &dwSize);
    T5_COPYFRAMEDWORD (answer+8, &dwCrc);
    T5SP5Tls_Send (pTL, 12, answer);
}

static void _T5SP5Ftls_CloseWrite (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                   T5_PTBYTE pQuestion)
{
    T5_DWORD dwSize, dwCrc;
    T5_BYTE answer[12];

    if ((pFT->bMode != SP5FTLS_WRITE)
        || ((T5_WORD)(*pQuestion) != (T5_WORD)T5_STRLEN(pFT->szPath))
        || (T5_MEMCMP (pQuestion + 1, pFT->szPath, *pQuestion) != 0))
    {
        _T5SP5Ftls_SendError (pTL, SP5FT_CW, SP5FT_ERRFNC);
        return;
    }

    pQuestion += (*pQuestion + 1);
    T5_COPYFRAMEDWORD (&dwSize, pQuestion);
    T5_COPYFRAMEDWORD (&dwCrc, pQuestion + 4);

    if (dwSize != SP5FF_GETSIZE (SP5_FTEMPW)
        || dwCrc != T5SP5Ftls_FileCrcUpTo (SP5_FTEMPW, dwSize))
    {
        _T5SP5Ftls_SendError (pTL, SP5FT_CW, SP5FT_ERRIO);
        return;
    }

    SP5FF_DELETE (pFT->szPath);
    if (!SP5FF_COPY (SP5_FTEMPW, pFT->szPath))
    {
        _T5SP5Ftls_SendError (pTL, SP5FT_CW, SP5FT_ERRIO);
        return;
    }
    dwSize = SP5FF_GETSIZE (pFT->szPath);
    dwCrc = T5SP5Ftls_FileCrcUpTo (pFT->szPath, dwSize);

    T5_MEMSET (pFT->szPath, 0, sizeof (pFT->szPath));
    pFT->bMode = SP5FTLS_IDLE;

    T5_MEMCPY (answer, SP5FT_HEAD, 2);
    answer[2] = SP5FT_CW;
    answer[3] = 0;
    T5_COPYFRAMEDWORD (answer+4, &dwSize);
    T5_COPYFRAMEDWORD (answer+8, &dwCrc);
    T5SP5Tls_Send (pTL, 12, answer);
}

static void _T5SP5Ftls_Write_1 (T5PTR_SP5TL pTL, T5PTR_SP5FTLS pFT,
                                T5_PTBYTE pQuestion)
{
    T5_DWORD dwOffset;
    T5_WORD wSize;
    T5_LONG f;
    T5_BYTE answer[4];

    if (pFT->bMode != SP5FTLS_WRITE)
    {
        _T5SP5Ftls_SendError (pTL, SP5FT_W1, SP5FT_ERRFNC);
        return;
    }

    T5_COPYFRAMEDWORD (&dwOffset, pQuestion);
    T5_COPYFRAMEWORD (&wSize, pQuestion+4);
    pQuestion += 6;

    if (dwOffset == 0L)
        SP5FF_DELETE (SP5_FTEMPW);

    if (dwOffset == 0L)
        f = SP5FF_OPEN (SP5_FTEMPW, TRUE);
    else
        f = SP5FF_OPENA (SP5_FTEMPW);
    if (f == 0L || !SP5FF_WRITE (f, pQuestion, wSize))
    {
        if (f)
            SP5FF_CLOSE (f);
        _T5SP5Ftls_SendError (pTL, SP5FT_W1, SP5FT_ERRIO);
        return;
    }
    if (f)
        SP5FF_CLOSE (f);

    T5_MEMCPY (answer, SP5FT_HEAD, 2);
    answer[2] = SP5FT_W1;
    answer[3] = 0;
    T5SP5Tls_Send (pTL, 4, answer);
}

/****************************************************************************/

T5_BOOL T5SP5Ftls_SeekTo (T5_LONG f, T5_DWORD dwOffset)
{
    T5_DWORD dwPacks, dwRest;
    T5_BYTE dummy[32];

    dwPacks = (dwOffset / 32L);
    dwRest = (dwOffset % 32L);

    while (dwPacks--)
    {
        if (!SP5FF_READ (f, dummy, 32L))
            return FALSE;
    }
    if (dwRest)
    {
        if (!SP5FF_READ (f, dummy, (T5_LONG)dwRest))
            return FALSE;
    }
    return TRUE;
}

T5_DWORD T5SP5Ftls_FileCrcUpTo (T5_PTCHAR szPath, T5_DWORD dwKnownSize)
{
    T5_DWORD dwCrc;
    T5_LONG f;
    T5_BYTE b;

    if (dwKnownSize == 0L)
        return 0L;

    dwCrc = T5SP5Ftls_Crc (0, NULL, 0);
    f = SP5FF_OPEN (szPath, FALSE);
    if (!f)
        return 0L;

    while (dwKnownSize--)
    {
        if (SP5FF_READ (f, &b, 1L))
            dwCrc = T5SP5Ftls_Crc (1, &b, dwCrc);
        else
            dwCrc = 0L;
    }

    SP5FF_CLOSE (f);
    return dwCrc;
}

T5_DWORD T5SP5Ftls_Crc (T5_WORD wLen, T5_PTBYTE pData, T5_DWORD dwCrc)
{
    T5_BYTE i;
    T5_DWORD mem1;

    if (wLen == 0) /* init */
        return 0xffffffffL;

    while (wLen--)
    {
        dwCrc ^= ((T5_DWORD)(*pData) & 0xffL);
        for (i=1; i<= 8; i++)
        {
            mem1 = dwCrc;
            dwCrc /= 2;
            if (mem1 & 1) dwCrc ^= 0xa000a001;
        }
        pData++; 
    }
    return dwCrc;
}

/****************************************************************************/

#endif /*T5DEF_SP5*/

/* eof **********************************************************************/
