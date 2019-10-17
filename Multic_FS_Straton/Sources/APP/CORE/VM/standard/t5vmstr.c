/*****************************************************************************
T5VMstr.c :  manage strings

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_STRINGSUPPORTED

/*****************************************************************************
T5VMStr_AscForce
force a string using a char pointer and an explicit length
Parameters:
    pDst (OUT) destination string buffer
    pbValue (IN) pointer to a string
    bStrLen (IN) string length
*****************************************************************************/

T5_RET T5VMStr_AscForce (T5_PTBYTE pDst, T5_PTBYTE pbValue,
                         T5_BYTE bStrLen)
{
    T5_BYTE bLen;

    bLen = bStrLen; /* source current length */
    if (bLen > *pDst) bLen = *pDst; /* adjust to max if too long */
    pDst[1] = bLen; /* copy current length */
    if (bLen)
        T5_MEMCPY (pDst+2, pbValue, bLen); /* copy characters */
    pDst[2+bLen] = 0; /* eos mark */
    return T5RET_OK;
}

/*****************************************************************************
T5VMStr_AscForceC
force a string using string index and null terminated value
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the string variable
    pbValue (IN) pointer to a "C" string
*****************************************************************************/

T5_RET T5VMStr_AscForceC (T5PTR_DB pDB, T5_WORD wIndex, T5_PTCHAR pbValue)
{
    T5_PTBYTE *pStr;
    T5_BYTE bLen;
    T5_PTBYTE pDst;

    pStr = T5GET_DBSTRING(pDB);
    pDst = pStr[wIndex];

    if (pbValue == NULL)
        pbValue = (T5_PTCHAR)"\0";

    bLen = (T5_BYTE)T5_STRLEN((T5_PTCHAR)pbValue); /* source current length */
    if (bLen > *pDst) bLen = *pDst; /* adjust to max if too long */
    pDst[1] = bLen; /* copy current length */
    if (bLen)
        T5_MEMCPY (pDst+2, pbValue, bLen); /* copy characters */
    pDst[2+bLen] = 0; /* eos mark */
    return T5RET_OK;
}

/*****************************************************************************
T5VMStr_Force
force a string using a pointer to a string buffer (len + characters)
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the string variable
    pbValue (IN) pointer to a string buffer)
*****************************************************************************/

T5_RET T5VMStr_Force (T5PTR_DB pDB, T5_WORD wIndex, T5_PTBYTE pbValue)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pDst;
    T5_BYTE bLen;

    pStr = T5GET_DBSTRING(pDB);
    pDst = pStr[wIndex];
    bLen = *pbValue; /* source current length */
    if (bLen > *pDst) bLen = *pDst; /* adjust to max if too long */
    pDst[1] = bLen; /* copy current length */
    if (bLen)
        T5_MEMCPY (pDst+2, pbValue+1, bLen); /* copy characters */
    pDst[2+bLen] = 0; /* eos mark */
    return T5RET_OK;
}

/*****************************************************************************
T5VMStr_DBForce
force a string using a pointer to a string buffer (len + characters)
the source string contains a MOTOROLA WORD for the len + each character
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the string variable
    pbValue (IN) pointer to a string buffer)
*****************************************************************************/

T5_RET T5VMStr_DBForce (T5PTR_DB pDB, T5_WORD wIndex, T5_PTWORD pwValue)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pDst;

    pStr = T5GET_DBSTRING(pDB);
    pDst = pStr[wIndex];

    return T5VMStr_DBForcePtr (pDst, pwValue);
}

/*****************************************************************************
T5VMStr_Copy
copy a string variable to another
Parameters:
    pDB (IN) pointer to the data base
    wDst (IN) index of the destination string variable
    wSrc (IN) index of the source string variable
*****************************************************************************/

T5_RET T5VMStr_Copy (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pSource;

    pStr = T5GET_DBSTRING(pDB);
    pSource = pStr[wSrc];
    return T5VMStr_Force (pDB, wDst, pSource + 1);
}

/*****************************************************************************
T5VMStr_CopyPtr
copy a string variable to another using pointers to string buffers (complete)
Parameters:
    pDst (IN) pointer to destination string variable
    pSrc (IN) pointer to the source string variable
*****************************************************************************/

T5_RET T5VMStr_CopyPtr (T5_PTBYTE pDst, T5_PTBYTE pSrc)
{
    T5_BYTE bLen;

    bLen = pSrc[1];
    if (bLen > pDst[0]) bLen = pDst[0];
    pDst[1] = bLen;
    if (bLen)
        T5_MEMCPY(pDst+2, pSrc+2, bLen);
    pDst[2+bLen] = 0;
    return T5RET_OK;
}

/*****************************************************************************
T5VMStr_DBForcePtr
force string variable with a value from debugger (chars on 2 bytes)
Parameters:
    pDst (IN) pointer to destination string variable
    pwValue (IN) array of characters
*****************************************************************************/

T5_RET T5VMStr_DBForcePtr (T5_PTBYTE pDst, T5_PTWORD pwValue)
{
    T5_BYTE bLen;

    /* source current length */
    bLen = (T5_BYTE)(*pwValue++);
    if (bLen > *pDst) bLen = *pDst; /* adjust to max if too long */
    /* copy current length */
    pDst[1] = bLen;
    /* copy characters */
    pDst += 2;
    while (bLen--)
        *pDst++ = (T5_BYTE)(*pwValue++);
    *pDst = 0; /* eos mark */
    return T5RET_OK;
}

/*****************************************************************************
T5VMStr_Cat
concat strings
Parameters:
    pDB (IN) pointer to the data base
    wDst (IN) index of the destination string variable
    wS1, wS2 (IN) index of the source string variables
*****************************************************************************/

T5_RET T5VMStr_Cat (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wS1, T5_WORD wS2)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pDst, pS1, pS2;
    T5_WORD i, wLen, wLen1;
    T5_BYTE s[257];

    pStr = T5GET_DBSTRING(pDB);
    pDst = pStr[wDst];
    pS1 = pStr[wS1];
    pS2 = pStr[wS2];
    /* get total length */
    wLen = (T5_WORD)(pS1[1]) + (T5_WORD)(pS2[1]);
    if (wLen > (T5_WORD)(*pDst))
        wLen = (T5_WORD)(*pDst);
    /* prepare */
    s[0] = (T5_BYTE)wLen;
    wLen1 = (T5_WORD)(pS1[1]);
    for (i=0; i<wLen; i++)
    {
        if (i < wLen1)
            s[1+i] = pS1[2+i];
        else
            s[1+i] = pS2[2 + i - wLen1];
    }
    s[wLen+1] = 0;
    /* copy */
    return T5VMStr_Force (pDB, wDst, s);
}

/*****************************************************************************
T5VMStr_Compare
compare two strings
Parameters:
    pDB (IN) pointer to the data base
    wS1, wS2 (IN) index of the source string variables
Return: signed integer (s1 - s2)
*****************************************************************************/

T5_SHORT T5VMStr_Compare (T5PTR_DB pDB, T5_WORD wS1, T5_WORD wS2)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS1, pS2;
    T5_SHORT iDiff;
    T5_BYTE bLen1, bLen2;

    pStr = T5GET_DBSTRING(pDB);
    pS1 = pStr[wS1];
    pS2 = pStr[wS2];
    bLen1 = pS1[1];
    bLen2 = pS2[1];
    pS1 += 2;
    pS2 += 2;
    iDiff = 0;
    while (bLen1 && bLen2 && iDiff == 0)
    {
        iDiff = (T5_SHORT)(*pS1++) - (T5_SHORT)(*pS2++);
        bLen1--;
        bLen2--;
    }
    if (iDiff == 0)
    {
        if (bLen1) iDiff = 1;
        else if (bLen2) iDiff = -1;
    }
    return iDiff;
}

/*****************************************************************************
T5VMStr_???toA
number to string conversion
*****************************************************************************/

T5_RET T5VMStr_ItoA (T5PTR_DB pDB, T5_WORD wDst, T5_LONG lValue)
{
    T5_BYTE s[64];

    T5_LTOA (s+1, lValue);
    *s = (T5_BYTE)T5_STRLEN((T5_PTCHAR)(s+1));
    return T5VMStr_Force (pDB, wDst, s);
}

T5_RET T5VMStr_BootoA (T5PTR_DB pDB, T5_WORD wDst, T5_BOOL bValue)
{
    T5_BYTE s[10];

#ifdef T5DEF_IECSTRINGCONV
    if (T5DEF_IECSTRINGCONV)
    {
        T5_STRCPY (s+1, (bValue) ? "TRUE" : "FALSE");
        *s = (T5_BYTE)T5_STRLEN((T5_PTCHAR)(s+1));
    }
    else
#endif /*T5DEF_IECSTRINGCONV*/
    {
        s[0] = 1; /* length */
        s[1] = (bValue) ? '1' : '0';
        s[2] = '\0';
    }
    return T5VMStr_Force (pDB, wDst, s);
}

T5_RET T5VMStr_TmrtoA (T5PTR_DB pDB, T5_WORD wDst, T5_DWORD dwValue)
{
#ifdef T5DEF_IECSTRINGCONV
    T5_BYTE s[64], ms[10];;

    if (T5DEF_IECSTRINGCONV)
    {
        T5_LTOA (s+1, dwValue / 1000);
        T5_LTOA (ms, dwValue % 1000);
        T5_STRCAT (s+1, "s");
        T5_STRCAT (s+1, ms);
        s[0] = (T5_BYTE)T5_STRLEN(s+1);
        return T5VMStr_Force (pDB, wDst, s);
    }
    else
#endif /*T5DEF_IECSTRINGCONV*/
    {
        return T5VMStr_ItoA (pDB, wDst, (T5_LONG)dwValue);
    }
}

T5_RET T5VMStr_UtoA (T5PTR_DB pDB, T5_WORD wDst, T5_DWORD dwValue)
{
    T5_BYTE s[64];
    T5_PTBYTE ptr;
    T5_BYTE bLen;

    if (!dwValue)
    {
        s[0] = 1;
        s[1] = '0';
        s[2] = 0;
        return T5VMStr_Force (pDB, wDst, s);
    }

    s[63] = '\0';
    ptr = &(s[63]);
    bLen = 0;
    while (dwValue)
    {
        ptr--;
        *ptr = (T5_BYTE)((dwValue % 10) + '0');
        dwValue /= 10;
        bLen++;
    }
    ptr--;
    *ptr = bLen;
    return T5VMStr_Force (pDB, wDst, ptr);
}

void T5VMStr_UtoSZ (T5_PTCHAR sz, T5_DWORD dwValue)
{
    T5_PTBYTE ptr;
    T5_WORD wNbDigit;
    T5_DWORD dw;

    if (dwValue == 0L)
    {
        T5_STRCPY (sz, "0");
        return;
    }

    wNbDigit = 0;
    dw = dwValue;
    while (dw)
    {
        wNbDigit += 1;
        dw /= 10;
    }

    sz[wNbDigit] = '\0';
    ptr = (T5_PTBYTE)(&(sz[wNbDigit]));
    while (dwValue)
    {
        ptr--;
        *ptr = (T5_BYTE)((dwValue % 10) + '0');
        dwValue /= 10;
    }
}

/*****************************************************************************
T5VMStr_Ato??
string to number conversion
*****************************************************************************/

T5_LONG T5VMStr_AtoI (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    return (T5_LONG)T5_ATOI(pS + 2);
}

T5_DWORD T5VMStr_AtoUL (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;
    T5_DWORD dw;
    T5_BYTE bLen, i;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    bLen = pS[1];
    dw = 0L;

    i=0;
    while (i<bLen && pS[i + 2] == ' ')
        i++;
    while (i<bLen && pS[i+2] >= '0' && pS[i+2] <= '9')
    {
        dw *= 10L;
        dw += (T5_DWORD)(pS[i+2] - '0');
        i++;
    }
    return dw;
}

/*****************************************************************************
T5VMStr_IsEmpty
Test if a string is empty
Parameters:
    pDB (IN) pointer to the data base
    wSrc (IN) index of the string
Return: TRUE if the string length is 0
*****************************************************************************/

T5_BOOL T5VMStr_IsEmpty (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    return (pS[1] == 0);
}

/*****************************************************************************
T5VMStr_FtoA
float to string conversion
*****************************************************************************/

#ifdef T5DEF_REALSUPPORTED

T5_RET T5VMStr_FtoA (T5PTR_DB pDB, T5_WORD wDst, T5_REAL lValue)
{
    T5_BYTE s[64];

    T5_RTOA (s+1, lValue);
    *s = (T5_BYTE)T5_STRLEN((T5_PTCHAR)(s+1));
    return T5VMStr_Force (pDB, wDst, s);
}

#endif /*T5DEF_REALSUPPORTED*/

/*****************************************************************************
T5VMStr_AtoF
string to float conversion
*****************************************************************************/

#ifdef T5DEF_REALSUPPORTED

T5_REAL T5VMStr_AtoF (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    return (T5_REAL)T5_ATOF(pS + 2);
}

#endif /*T5DEF_REALSUPPORTED*/

/*****************************************************************************
T5VMStr_DtoA
LREAL to string conversion
*****************************************************************************/

#ifdef T5DEF_LREALSUPPORTED

T5_RET T5VMStr_DtoA (T5PTR_DB pDB, T5_WORD wDst, T5_LREAL lValue)
{
    T5_BYTE s[16];

    T5_RTOA (s+1, lValue);
    *s = (T5_BYTE)T5_STRLEN((T5_PTCHAR)(s+1));
    return T5VMStr_Force (pDB, wDst, s);
}

#endif /*T5DEF_LREALSUPPORTED*/

/*****************************************************************************
T5VMStr_AtoD
string to LREAL conversion
*****************************************************************************/

#ifdef T5DEF_LREALSUPPORTED

T5_LREAL T5VMStr_AtoD (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    return (T5_LREAL)T5_ATOF(pS + 2);
}

#endif /*T5DEF_LREALSUPPORTED*/

/*****************************************************************************
T5VMStr_Empty
Reset a string to a null length
Parameters:
    pDB (IN) pointer to the data base
    wSrc (IN) index of the string
Return: 
TRUE if the string length is 0
*****************************************************************************/

T5_RET T5VMStr_Empty (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    pS[1] = 0;
    pS[2] = 0;
    return T5RET_OK;
}

/*****************************************************************************
T5VMStr_Ascii
Get ascii code of a character within a string
Parameters:
    pDB (IN) pointer to the data base
    wSrc (IN) index of the string variable
    lPos (IN) index of the character (0 based)
Return: ascii code or 0 if error
*****************************************************************************/

T5_LONG T5VMStr_Ascii (T5PTR_DB pDB, T5_WORD wSrc, T5_LONG lPos)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    if (lPos >= 0 && lPos < (T5_LONG)(pS[1]))
        return (T5_LONG)(pS[lPos+2]);
    /* 0 if error */
    return 0L;
}

/*****************************************************************************
T5VMStr_Char
builds a one char string
Parameters:
    pDB (IN) pointer to the data base
    wDst (IN) index of the string variable
    lValue (IN) ascii code of the unique character
*****************************************************************************/

T5_RET T5VMStr_Char (T5PTR_DB pDB, T5_WORD wDst, T5_LONG lValue)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pDst;

    pStr = T5GET_DBSTRING(pDB);
    pDst = pStr[wDst];
    if (pDst[0] != 0)
    {
        pDst[1] = 1;
        pDst[2] = (T5_BYTE)(lValue & 0xff);
    }
    return T5RET_OK;
}

/*****************************************************************************
T5VMStr_GetLength
Get current length of a string
Parameters:
    pDB (IN) pointer to the data base
    wSrc (IN) index of the string variable
Return: number of characters in string
*****************************************************************************/

T5_LONG T5VMStr_GetLength (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    return ((T5_LONG)(pS[1]) & 0xff);
}

/*****************************************************************************
T5VMStr_GetMaxLength
get the maximum length (declared) of a string
Parameters:
    pDB (IN) pointer to the data base
    wSrc (IN) index of the string variable
Return: max number of chars in string
*****************************************************************************/

T5_BYTE T5VMStr_GetMaxLength (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    return *pS;
}

/*****************************************************************************
T5VMStr_Left
extract a string on the left
Parameters:
    pDB (IN) pointer to the data base
    wDst (IN) index of the destination string variable
    wSrc (IN) index of the input string variable
    lNb (IN) number of characters wished
*****************************************************************************/

T5_RET T5VMStr_Left (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc, T5_LONG lNb)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;
    T5_BYTE s[257];

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    /* check length */
    if (lNb < 0)
        lNb = 0;
    else if (lNb > (T5_LONG)(pS[1]))
        lNb = (T5_LONG)(pS[1]);
    /* extract */
    if (lNb)
        T5_MEMCPY(s+1, pS+2, lNb);
    *s = (T5_BYTE)lNb;
    /* copy */
    return T5VMStr_Force (pDB, wDst, s);
}

/*****************************************************************************
T5VMStr_Right
extract a string on the right
Parameters:
    pDB (IN) pointer to the data base
    wDst (IN) index of the destination string variable
    wSrc (IN) index of the input string variable
    lNb (IN) number of characters wished
*****************************************************************************/

T5_RET T5VMStr_Right (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc, T5_LONG lNb)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;
    T5_BYTE s[257];

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    /* check length */
    if (lNb < 0)
        lNb = 0;
    else if (lNb > (T5_LONG)(pS[1]))
        lNb = (T5_LONG)(pS[1]);
    /* extract */
    if (lNb)
        T5_MEMCPY(s+1, pS + (2 + pS[1] - (T5_BYTE)lNb), lNb);
    *s = (T5_BYTE)lNb;
    /* copy */
    return T5VMStr_Force (pDB, wDst, s);
}

/*****************************************************************************
T5VMStr_Mid
extract a string from another
Parameters:
    pDB (IN) pointer to the data base
    wDst (IN) index of the destination string variable
    wSrc (IN) index of the input string variable
    lPos (IN) index of the first wished character (0 based)
    lNb (IN) number of characters wished
*****************************************************************************/

T5_RET T5VMStr_Mid (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc,
                    T5_LONG lPos, T5_LONG lNb)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;
    T5_BYTE s[257];

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    /* check pos */
    if (lPos < 0)
        lPos = 0;
    else if (lPos > (T5_LONG)(pS[1]))
        lPos = (T5_LONG)(pS[1]);
    /* check length */
    if (lNb < 0)
        lNb = 0;
    else if (lNb > ((T5_LONG)(pS[1]) - lPos))
        lNb = (T5_LONG)(pS[1]) - lPos;
    /* extract */
    if (lNb)
        T5_MEMCPY(s+1, pS + 2 + lPos, lNb);
    *s = (T5_BYTE)lNb;
    /* copy */
    return T5VMStr_Force (pDB, wDst, s);
}

/*****************************************************************************
T5VMStr_Delete
delete characters in a string
Parameters:
    pDB (IN) pointer to the data base
    wDst (IN) index of the destination string variable
    wSrc (IN) index of the input string variable
    lPos (IN) index of the first character to remove (0 based)
    lNb (IN) number of characters wished
*****************************************************************************/

T5_RET T5VMStr_Delete (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc,
                       T5_LONG lPos, T5_LONG lNb)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;
    T5_BYTE s[257];

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    /* check pos */
    if (lPos < 0)
        lPos = 0;
    else if (lPos > (T5_LONG)(pS[1]))
        lPos = (T5_LONG)(pS[1]);
    /* check length */
    if (lNb < 0)
        lNb = 0;
    else if (lNb > ((T5_LONG)(pS[1]) - lPos))
        lNb = (T5_LONG)(pS[1]) - lPos;
    /* build */
    T5_MEMCPY(s+1, pS+2, lPos);
    T5_MEMCPY(s+(1+lPos), pS+(2+lPos+lNb), (T5_LONG)(pS[1]) - lPos - lNb  );
    *s = pS[1] - (T5_BYTE)lNb;
    /* copy */
    return T5VMStr_Force (pDB, wDst, s);
}

/*****************************************************************************
T5VMStr_Insert
insert characters in a string
Parameters:
    pDB (IN) pointer to the data base
    wDst (IN) index of the destination string variable
    wSrc (IN) index of the string variable
    wInsert (IN) index of the string variable to be inserted
    lPos (IN) wished insert position (0 based)
*****************************************************************************/

T5_RET T5VMStr_Insert (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wSrc,
                                T5_WORD wInsert, T5_LONG lPos)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS, pIns;
    T5_BYTE s[514];
    T5_LONG lLen;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    pIns = pStr[wInsert];
    /* check pos */
    if (lPos < 0)
        lPos = 0;
    else if (lPos > (T5_LONG)(pS[1]))
        lPos = (T5_LONG)(pS[1]);
    /* get and fix number of characters to copy */
    lLen = (T5_LONG)(pIns[1]);
    if (lLen > ((T5_LONG)(pS[0]) - (T5_LONG)(pS[1])))
        lLen = (T5_LONG)(pS[0]) - (T5_LONG)(pS[1]);
    /* build */
    T5_MEMCPY(s+1, pS+2, lPos);
    T5_MEMCPY(s+(1+lPos), pIns+2, lLen);
    T5_MEMCPY(s+(1+lPos+lLen), pS+(2+lPos), (T5_LONG)pS[1] - lPos);
    *s = pS[1] + (T5_BYTE)lLen;
    /* copy */
    return T5VMStr_Force (pDB, wDst, s);
}

/*****************************************************************************
T5VMStr_Find
search for a string in another
Parameters:
    pDB (IN) pointer to the data base
    wSrc (IN) index of the string variable
    wPat (IN) index of the searched string variable
Return: 1 based index of the found position
*****************************************************************************/

T5_LONG T5VMStr_Find (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wPat)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS, pPat;
    T5_LONG lPos, i, lNb;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    pPat = pStr[wPat];
    lPos = 0;
    if (pPat[1] != 0 && pPat[1] <= pS[1])
    {
        lNb = (T5_LONG)(pS[1] - pPat[1]);
        for (i=0, lPos=0; !lPos && i<=lNb; i++)
            if (T5_MEMCMP(pS+(2+i), pPat+2, pPat[1]) == 0) lPos = i + 1;
    }
    return lPos; 
}

/*****************************************************************************
T5VMStr_GetText
Get pointer to the first character of a string
Parameters:
    pDB (IN) pointer to the data base
    wSrc (IN) index of the string variable
Return: can be used as a pointer to a C string
*****************************************************************************/

T5_PTCHAR T5VMStr_GetText (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    return (T5_PTCHAR)(pS + 2);
}

/*****************************************************************************
T5VMStr_AtoH
String to Dint conversion in hexadecimal
Parameters:
    pDB (IN) pointer to the data base
    wSrc (IN) index of the string variable
Return: result
*****************************************************************************/

T5_DWORD T5VMStr_AtoH (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;
    T5_BOOL bOK;
    T5_BYTE i, iNb;
    T5_DWORD dwVal;

    bOK = TRUE;
    dwVal = 0L;
    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    iNb = *pS;
    pS += 2;
    for (i=0; bOK && i<iNb; i++)
    {
        if (pS[i] >= '0' && pS[i] <= '9')
            dwVal = (dwVal << 4) | (((T5_DWORD)(pS[i] - '0')) & 0x0fL);
        else if (pS[i] >= 'A' && pS[i] <= 'F')
            dwVal = (dwVal << 4) | (((T5_DWORD)(pS[i] - 'A' + 10)) & 0x0fL);
        else if (pS[i] >= 'a' && pS[i] <= 'f')
            dwVal = (dwVal << 4) | (((T5_DWORD)(pS[i] - 'a' + 10)) & 0x0fL);
        else
            bOK = FALSE;

    }
    return dwVal;
}

/*****************************************************************************
T5VMStr_HtoA
Dint to String conversion in hexadecimal
Parameters:
    pDB (IN) pointer to the data base
    dwSrc (IN) input value
    wDst (IN) index of the string variable
*****************************************************************************/

void T5VMStr_HtoA (T5PTR_DB pDB, T5_WORD wDst, T5_DWORD dwSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;
    T5_BYTE i;
    T5_DWORD dwDigit;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wDst];

    if (*pS < 8)
    {
        pS[1] = 0; /* reset length */
        pS[2] = 0; /* eos */
    }
    else
    {
        /* initialize with '0' characters */
        pS[1] = 8; /* length */
        for (i=0; i<8; i++) pS[i+2] = '0';
        pS[10] = 0; /* eos */
        /* go */
        i=9;
        while (dwSrc)
        {
            dwDigit = (dwSrc & 0x0fL);
            if (dwDigit < 10)
                pS[i] = '0' + (T5_BYTE)dwDigit;
            else
                pS[i] = 'A' + (T5_BYTE)(dwDigit - 10);
            i--;
            dwSrc = (dwSrc >> 4) & 0x0fffffffL;
        }
        while (pS[1] > 2 && pS[2] == '0' && pS[3] == '0')
        {
            for (i=0; i<6; i++)
                pS[2+i] = pS[4+i];
            pS[1] -= 2;
            pS[2+pS[1]] = 0;
        }

    }
}

/*****************************************************************************
T5VMStr_Crc16
Calculates a CRC16 on all characters of a string
Parameters:
    pDB (IN) pointer to the data base
    wSrc (IN) index of the string variable
Return: CRC16
*****************************************************************************/

T5_WORD T5VMStr_Crc16 (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;
    T5_WORD i, j, wLen;
    T5_WORD wCrc;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    wLen = ((T5_WORD)(pS[1])) & 0x00ff;
    pS += 2;
    /* calculate */
    wCrc = 0xffff;
    for (i=0; i<wLen; i++)
    {
        wCrc ^= (((T5_WORD)(pS[i])) & 0x00ff);
        for (j=0; j<8; j++)
        {
            if (wCrc & 1)
            {
                wCrc >>= 1;
                wCrc ^= 0xa001;
            }
            else
                wCrc >>= 1;
        }
    }
    return wCrc;
}

/*****************************************************************************
LINT to strig conversions
*****************************************************************************/

#ifdef T5DEF_LINTSUPPORTED

T5_LINT T5VMStr_AtoLINT (T5PTR_DB pDB, T5_WORD wSrc)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pS;
    T5_BOOL bOK, bMinus;
    T5_BYTE i, iNb;
    T5_LINT lVal;

    bOK = TRUE;
    lVal = 0;
    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    iNb = *pS;
    pS += 2;

    while (iNb != 0 && *pS == ' ')
    {
        pS++;
        iNb--;
    }

    if (iNb != 0 && *pS == '-')
    {
        bMinus = TRUE;
        pS++;
        iNb--;
    }
    else
        bMinus = FALSE;


    for (i=0; bOK && i<iNb; i++)
    {
        if (pS[i] >= '0' && pS[i] <= '9')
        {
            lVal *= 10;
            lVal += (T5_LINT)(pS[i] - '0');
        }
        else
            bOK = FALSE;
    }

    if (bMinus)
        lVal = -lVal;
    return lVal;
}

T5_LINT T5VMStr_SZtoLINT (T5_PTCHAR sz)
{
    T5_PTBYTE pS;
    T5_BOOL bOK, bMinus;
    T5_BYTE i;
    T5_LINT lVal;

    bOK = TRUE;
    lVal = 0;
    pS = (T5_PTBYTE)sz;

    if (*pS == '-')
    {
        bMinus = TRUE;
        pS++;
    }
    else
        bMinus = FALSE;

    for (i=0; bOK && pS[i] != '\0'; i++)
    {
        if (pS[i] >= '0' && pS[i] <= '9')
        {
            lVal *= 10;
            lVal += (T5_LINT)(pS[i] - '0');
        }
        else
            bOK = FALSE;
    }

    if (bMinus)
        lVal = -lVal;
    return lVal;
}

void T5VMStr_LINTtoA (T5PTR_DB pDB, T5_WORD wDst, T5_LINT lSrc)
{
    T5_CHAR szVal[64];
    T5_WORD wLen;
    T5_LINT lCount, lMod;
    T5_PTBYTE *pStr;
    T5_PTBYTE pDst;
    T5_BYTE bLen;


    wLen = 0;
    if (lSrc < 0)
    {
        *szVal = '-';
        lSrc = -lSrc;
        wLen = 1;
    }

    if (lSrc == 0)
    {
        szVal[wLen] = '0';
        wLen += 1;
    }
    else
    {
        lCount = lSrc;
        while (lCount)
        {
            lCount /= 10;
            wLen += 1;
        }
    }

    szVal[wLen] = '\0';

    while (lSrc && wLen > 0)
    {
        lMod = lSrc / 10;
        lMod *= 10;
        lMod = lSrc - lMod;
        szVal[--wLen] = (T5_CHAR)((lMod/*lSrc % 10*/) + '0');
        lSrc /= 10;
    }

    pStr = T5GET_DBSTRING(pDB);
    pDst = pStr[wDst];
    bLen = (T5_BYTE)T5_STRLEN(szVal);
    if (bLen > *pDst) bLen = *pDst; /* adjust to max if too long */
    pDst[1] = bLen; /* copy current length */
    if (bLen)
        T5_MEMCPY (pDst+2, szVal, bLen); /* copy characters */
    pDst[2+bLen] = 0; /* eos mark */
}

void T5VMStr_LINTtoSZ (T5_PTCHAR sz, T5_LINT lValue)
{
    T5_PTBYTE ptr;
    T5_WORD wNbDigit;
    T5_LINT l;

    if (lValue == 0L)
    {
        T5_STRCPY (sz, "0");
        return;
    }

    if (lValue > 0)
        wNbDigit = 0;
    else
    {
        lValue = - lValue;
        *sz = '-';
        wNbDigit = 1;
    }
    l = lValue;
    while (l)
    {
        wNbDigit += 1;
        l /= 10;
    }

    sz[wNbDigit] = '\0';
    ptr = (T5_PTBYTE)(&(sz[wNbDigit]));
    while (lValue)
    {
        ptr--;
        *ptr = (T5_BYTE)((lValue % 10) + '0');
        lValue /= 10;
    }
}

#endif /*T5DEF_LINTSUPPORTED*/

/****************************************************************************/

#ifdef T5DEF_SINTSUPPORTED

T5_DWORD T5VMStr_ToD8 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wDst,
                       T5_WORD wSizeof)
{
    T5_PTBYTE *pStr, pD8;
    T5_PTBYTE pS;
    T5_WORD wCount;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    wCount = (T5_WORD)(pS[1]) & 0xff;

    pD8 = T5Tools_GetD8Array (pDB, wDst);

    if (wCount > wSizeof)
        wCount = wSizeof;

    if (wCount)
        T5_MEMCPY(pD8, pS+2, wCount);

    return (T5_DWORD)wCount;
}

T5_DWORD T5VMStr_FromD8 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wSizeof,
                         T5_WORD wDst, T5_DWORD dwCount)
{
    T5_PTBYTE *pStr, pD8;
    T5_PTBYTE pS;
    T5_WORD wMax;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wDst];
    wMax = (T5_WORD)(pS[0]) & 0xff;

    pD8 = T5Tools_GetD8Array (pDB, wSrc);

    if (dwCount > (T5_DWORD)wSizeof)
        dwCount = (T5_DWORD)wSizeof;

    if (dwCount > (T5_DWORD)wMax)
        dwCount = (T5_DWORD)wMax;

    if (dwCount)
        T5_MEMCPY(pS+2, pD8, (T5_WORD)dwCount);
    pS[1] = (T5_BYTE)dwCount;

    return dwCount;
}

#endif /*T5DEF_SINTSUPPORTED*/

#ifdef T5DEF_DATA16SUPPORTED

T5_DWORD T5VMStr_ToD16 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wDst,
                        T5_WORD wSizeof)
{
    T5_PTBYTE *pStr, pD16;
    T5_PTBYTE pS;
    T5_WORD wCount;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    wCount = (T5_WORD)(pS[1]) & 0xff;
    wCount /= 2;

    pD16 = (T5_PTBYTE)T5Tools_GetD16Array (pDB, wDst);

    if (wCount > wSizeof)
        wCount = wSizeof;

    if (wCount)
        T5_MEMCPY(pD16, pS+2, wCount * 2);

    return (T5_DWORD)wCount;
}

T5_DWORD T5VMStr_FromD16 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wSizeof,
                          T5_WORD wDst, T5_DWORD dwCount)
{
    T5_PTBYTE *pStr, pD16;
    T5_PTBYTE pS;
    T5_WORD wMax;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wDst];
    wMax = (T5_WORD)(pS[0]) & 0xff;
    wMax /= 2;

    pD16 = (T5_PTBYTE)T5Tools_GetD16Array (pDB, wSrc);

    if (dwCount > (T5_DWORD)wSizeof)
        dwCount = (T5_DWORD)wSizeof;

    if (dwCount > (T5_DWORD)wMax)
        dwCount = (T5_DWORD)wMax;

    T5_MEMCPY(pS+2, pD16, (T5_WORD)(dwCount * 2));
    pS[1] = (T5_BYTE)(dwCount * 2);

    return dwCount;
}

#endif /*T5DEF_DATA16SUPPORTED*/

T5_DWORD T5VMStr_ToD32 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wDst,
                        T5_WORD wSizeof)
{
    T5_PTBYTE *pStr, pD32;
    T5_PTBYTE pS;
    T5_WORD wCount;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wSrc];
    wCount = (T5_WORD)(pS[1]) & 0xff;
    wCount /= 4;

    pD32 = (T5_PTBYTE)T5Tools_GetD32Array (pDB, wDst);

    if (wCount > wSizeof)
        wCount = wSizeof;

    T5_MEMCPY(pD32, pS+2, wCount * 4);

    return (T5_DWORD)wCount;
}

T5_DWORD T5VMStr_FromD32 (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wSizeof,
                          T5_WORD wDst, T5_DWORD dwCount)
{
    T5_PTBYTE *pStr, pD32;
    T5_PTBYTE pS;
    T5_WORD wMax;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wDst];
    wMax = (T5_WORD)(pS[0]) & 0xff;
    wMax /= 4;

    pD32 = (T5_PTBYTE)T5Tools_GetD32Array (pDB, wSrc);

    if (dwCount > (T5_DWORD)wSizeof)
        dwCount = (T5_DWORD)wSizeof;

    if (dwCount > (T5_DWORD)wMax)
        dwCount = (T5_DWORD)wMax;

    T5_MEMCPY(pS+2, pD32, (T5_WORD)(dwCount * 4));
    pS[1] = (T5_BYTE)(dwCount * 4);

    return dwCount;
}

/****************************************************************************/

static void _Fmt2 (T5_PTBYTE pS, T5_DWORD dw)
{
    pS[1] = (T5_BYTE)((dw % 10L) + '0');
    dw /= 10L;
    pS[0] = (T5_BYTE)((dw % 10L) + '0');
}

void T5VMStr_DateFmt (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wFmt,
                      T5_DWORD dwDate, T5_DWORD dwTime)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pDst, pFmt;
    T5_PTBYTE pD, pF;
    T5_BYTE bCount, bNRead;

    pStr = T5GET_DBSTRING(pDB);
    pDst = pStr[wDst];
    pFmt = pStr[wFmt];
    pD = pDst + 2;
    pF = pFmt + 2;
    bCount = bNRead = 0;

    while (bCount < pDst[0] && bNRead < pFmt[1])
    {
        if (*pF != '%')
        {
            *pD++ = *pF++;
            bCount ++;
            bNRead ++;
        }
        else if (bCount >= 254 || (bCount+1) >= pDst[0])
        {
            *pD++ = *pF++;
            bCount ++;
            bNRead ++;
        }
        else
        {
            pF++;
            switch (*pF)
            {
            case 'Y' :
                if (bCount <= 252 && (bCount+4) < pDst[0])
                {
                    _Fmt2 (pD, ((dwDate >> 16) & 0xffffL) / 100L);
                    pD += 2;
                    bCount += 2;
                }
                _Fmt2 (pD, (dwDate >> 16) & 0xffffL);
                break;
            case 'y' :
                _Fmt2 (pD, (dwDate >> 16) & 0xffffL);
                break;
            case 'm' :
                _Fmt2 (pD, (dwDate >> 8) & 0xffL);
                break;
            case 'd' :
                _Fmt2 (pD, dwDate & 0xffL);
                break;
            case 'H' :
                _Fmt2 (pD, dwTime / 3600000L);
                break;
            case 'M' :
                _Fmt2 (pD, (dwTime / 60000L) % 60L);
                break;
            case 'S' :
                _Fmt2 (pD, (dwTime / 1000L) % 60L);
                break;
            default :
                pD[0] = '%';
                pD[1] = *pF;
                break;
            }
            pF++;
            pD += 2;
            bCount += 2;
            bNRead += 2;
        }
    }

    pDst[bCount+2] = '\0';
    pDst[1] = bCount;
}

/****************************************************************************/

T5_BOOL T5VMStr_NumToStr (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wType,
                          T5_WORD wIndex, T5_LONG lWidth, T5_LONG lNbDigit)
{
    T5_PTBYTE *pStr;
    T5_PTBYTE pDst, pDec;
    T5_BYTE i, bWidth, bWLen;
    T5_BOOL bLeft, bError;
    T5_PTR pData;
    T5_BYTE sFmt[257];
#ifdef T5DEF_LREALSUPPORTED
    T5_BOOL bNeg;
    T5_LREAL lr;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_REALSUPPORTED
    T5_REAL r;
#endif /*T5DEF_REALSUPPORTED*/

    bError = FALSE;
    pStr = T5GET_DBSTRING(pDB);
    pDst = pStr[wDst];
    /* check width */
    if (lWidth < 0)
    {
        bLeft = TRUE;
        lWidth = - lWidth;
    }
    else
        bLeft = FALSE;
    if (lWidth == 0)
        bWidth = *pDst;
    else if (lWidth > (T5_LONG)(*pDst))
    {
        bWidth = *pDst;
        bError = TRUE;
    }
    else
        bWidth = (T5_BYTE)(lWidth & 0xffL);
    /* check nb digits */
    if (lNbDigit != 0)
    {
        if (lNbDigit > ((T5_LONG)bWidth) + 2)
            bError = TRUE;
    }
    /* get source data */
    pData = T5Tools_GetAnyParam (pDB, wType, wIndex, NULL, NULL);
    if (pData == NULL)
        bError = TRUE;

    /* avoid re-testing error for all types */
    if (bError)
        wType = 0;

    /* prepare integer part */
    switch (wType)
    {
    case T5C_BOOL :
        T5_LTOA (sFmt, (*(T5_PTBOOL)pData) ? 1L : 0L);
        break;
    case T5C_SINT :
        T5_LTOA (sFmt, (T5_LONG)(*(T5_PTCHAR)pData));
        break;
    case T5C_USINT :
        T5_LTOA (sFmt, (T5_LONG)(*(T5_PTBYTE)pData));
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        T5_LTOA (sFmt, (T5_LONG)(*(T5_PTSHORT)pData));
        break;
    case T5C_UINT :
        T5_LTOA (sFmt, (T5_LONG)(*(T5_PTWORD)pData));
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
        T5_LTOA (sFmt, *(T5_PTLONG)pData);
        break;
    case T5C_UDINT :
        T5VMStr_UtoSZ ((T5_PTCHAR)sFmt, *(T5_PTDWORD)pData);
        break;
    case T5C_TIME :
        T5VMStr_UtoSZ ((T5_PTCHAR)sFmt, *(T5_PTDWORD)pData);
        break;
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        T5VMStr_LINTtoSZ ((T5_PTCHAR)sFmt, *(T5_PTLINT)pData);
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        T5VMStr_LINTtoSZ ((T5_PTCHAR)sFmt, *(T5_PTLINT)pData); /* todo */
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        if (*(T5_PTREAL)pData < 0.0 && (T5_LONG)(*(T5_PTREAL)pData) == 0L)
            T5_STRCPY (sFmt, "-0");
        else
            T5_LTOA (sFmt, (T5_LONG)(*(T5_PTREAL)pData));
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
#ifdef T5DEF_MATHSUPPORTED
    case T5C_LREAL :
        lr = *(T5_PTLREAL)pData;
        bNeg = (lr < 0);
        if (lr >= 0)
            lr = floor (lr);
        else
            lr = ceil (lr);
        if ((((T5_LONG)log (fabs (lr + 1.0))) + 1) > 256)
            bError = TRUE;
        else if( bNeg && lr == 0)
            T5_STRCPY (sFmt, "-0");
        else
            T5_RTOA (sFmt, lr);
        break;
#endif /*T5DEF_MATHSUPPORTED*/
#endif /*T5DEF_LREALSUPPORTED*/
    default : 
        bError = TRUE;
        break;
    }
    if (!bError && T5_STRLEN (sFmt) > bWidth)
        bError = TRUE;
    /* add decimal part */
    if (!bError && lNbDigit > 0)
    {
        if ((T5_STRLEN (sFmt) + lNbDigit + 1) > bWidth)
            bError = TRUE;
        else
        {
            pDec = sFmt;
            while (*pDec)
                pDec++;
            *pDec++ = '.';
            switch (wType)
            {
#ifdef T5DEF_MATHSUPPORTED
#ifdef T5DEF_REALSUPPORTED
            case T5C_REAL :
                r = *(T5_PTREAL)pData;
                r = (T5_REAL)fabs (r);
                r -= (T5_REAL)floor (r);
                while (lNbDigit--)
                {
                    r *= 10.0;
                    T5_LTOA (pDec, (T5_LONG)r);
                    r -= (T5_REAL)floor (fabs (r));
                    pDec++;
                }
                break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
            case T5C_LREAL :
                lr = *(T5_PTLREAL)pData;
                lr = fabs (lr);
                lr -= floor (lr);
                while (lNbDigit--)
                {
                    lr *= 10.0;
                    T5_LTOA (pDec, (T5_LONG)lr);
                    lr -= floor (fabs (lr));
                    pDec++;
                }
                break;
#endif /*T5DEF_LREALSUPPORTED*/
#endif /*T5DEF_MATHSUPPORTED*/
            default :
                while (lNbDigit--)
                    *pDec++ = '0';
                break;
            }
            *pDec = '\0';
        }
    }
    if (!bError && T5_STRLEN (sFmt) > bWidth)
        bError = TRUE;
    /* format and copy */
    if (!bError)
    {
        bWLen = bWidth - (T5_BYTE)T5_STRLEN (sFmt);
        if (lWidth != 0)
            pDst[1] = bWidth;
        else
            pDst[1] = (T5_BYTE)T5_STRLEN (sFmt);
        pDst += 2;
        if (!bLeft && lWidth != 0)
        {
            for (i=0; i<bWLen; i++)
                *pDst++ = ' ';
        }
        for (i=0; sFmt[i] != '\0'; i++)
            *pDst++ = sFmt[i];
        if (bLeft && lWidth != 0)
        {
            for (i=0; i<bWLen; i++)
                *pDst++ = ' ';
        }
        *pDst = 0;
    }
    /* fill with '*' in case of error */
    if (bError)
    {
        pDst[1] = bWidth;
        for (i=0; i<bWidth; i++)
            pDst[i+2] = '*';
        pDst[2+bWidth] = '\0';
    }
    return !bError;
}

/****************************************************************************/

#endif /*T5DEF_STRINGSUPPORTED*/

/****************************************************************************/

T5_BYTE T5VMStr_ICmp (T5_PTBYTE sz1, T5_PTBYTE sz2)
{
    T5_BYTE c1, c2;

    while (*sz1 != '\0' || *sz2 != '\0')
    {
        c1 = *sz1++;
        c2 = *sz2++;
        if (c1 >= 'a' && c1 <= 'z')
            c1 -= ('a' - 'A');
        if (c2 >= 'a' && c2 <= 'z')
            c2 -= ('a' - 'A');
        if (c1 != c2)
            return (c1 - c2);
    }
    return 0;
}

T5_BYTE T5VMStr_NICmp (T5_PTBYTE sz1, T5_PTBYTE sz2, T5_WORD n)
{
    T5_BYTE c1, c2;

    while (n > 0 && (*sz1 != '\0' || *sz2 != '\0'))
    {
        c1 = *sz1++;
        c2 = *sz2++;
        if (c1 >= 'a' && c1 <= 'z')
            c1 -= ('a' - 'A');
        if (c2 >= 'a' && c2 <= 'z')
            c2 -= ('a' - 'A');
        if (c1 != c2)
            return (c1 - c2);
        n -= 1;
    }
    return 0;
}

/* eof **********************************************************************/
