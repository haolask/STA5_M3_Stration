/*****************************************************************************
T5BArray.c : block collection - arrays

DO NOT ALTER THIS !

(c) COPALP 2011
*****************************************************************************/

#include "t5vm.h"
#include "t5barray.h"

#ifdef T5DEF_BARRAY

/****************************************************************************/
/* useful macros */

#define _F_BEGIN \
    switch (wCommand) \
    { \
    case T5FBCMD_ACTIVATE :

#define _F_END \
        return 0L; \
    case T5FBCMD_ACCEPTCT : \
        return 1L; \
    default : \
        return 0L; \
    }

T5_SHORT _ARSTRCMP (T5_PTBYTE pS1, T5_PTBYTE pS2)
{
    T5_SHORT iDiff;
    T5_BYTE bLen1, bLen2;

    bLen1 = pS1[0];
    bLen2 = pS2[0];
    pS1 += 1;
    pS2 += 1;
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
/****************************************************************************/
/* ArCount  */

static T5_LONG _ARCOUNT_D8 (T5_PTBYTE pArray, T5_WORD wStart, T5_WORD wDim, T5_BYTE bPattern)
{
    T5_WORD i;
    T5_LONG lCount;

    lCount = 0L;
    for (i=wStart; i<wDim; i++)
    {
        if (pArray[i] == bPattern)
            lCount++;
    }
    return lCount;
}

static T5_LONG _ARCOUNT_D16 (T5_PTWORD pArray, T5_WORD wStart, T5_WORD wDim, T5_WORD wPattern)
{
    T5_WORD i;
    T5_LONG lCount;

    lCount = 0L;
    for (i=wStart; i<wDim; i++)
    {
        if (pArray[i] == wPattern)
            lCount++;
    }
    return lCount;
}

static T5_LONG _ARCOUNT_D32 (T5_PTDWORD pArray, T5_WORD wStart, T5_WORD wDim, T5_DWORD dwPattern)
{
    T5_WORD i;
    T5_LONG lCount;

    lCount = 0L;
    for (i=wStart; i<wDim; i++)
    {
        if (pArray[i] == dwPattern)
            lCount++;
    }
    return lCount;
}

#ifdef T5DEF_DATA64SUPPORTED

static T5_LONG _ARCOUNT_D64 (T5_PTDATA64 pArray, T5_WORD wStart, T5_WORD wDim, T5_DATA64 lPattern)
{
    T5_WORD i;
    T5_LONG lCount;

    lCount = 0L;
    for (i=wStart; i<wDim; i++)
    {
        if (pArray[i] == lPattern)
            lCount++;
    }
    return lCount;
}

#endif /*T5DEF_DATA64SUPPORTED*/

#ifdef T5DEF_STRINGSUPPORTED

static T5_LONG _ARCOUNT_STRING (T5_PTBYTE pArray, T5_WORD wStart, T5_WORD wDim, T5_PTBYTE pPattern)
{
    T5_WORD i, wLen;
    T5_LONG lCount;

    wLen = (T5_WORD)pArray[0] + 3;
    pArray += (wStart * wLen);
    lCount = 0L;
    for (i=wStart; i<wDim; i++, pArray += wLen)
    {
        if (T5_MEMCMP(pArray + 1, pPattern + 1, pArray[1] + 1) == 0)
            lCount++;
    }
    return lCount;
}

#endif /*T5DEF_STRINGSUPPORTED*/

#define _P_ANYTYPE      pArgs[0]
#define _P_ARR          pArgs[1]
#define _P_ARR_SIZE     pArgs[2]
#define _P_PATTERN      pArgs[3]
#define _P_STARTINDEX   (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[4]))
#define _P_COUNT        (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[5]))

T5_DWORD ARCOUNT (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
    T5_PTR pArray, pPattern;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        pArray = T5Tools_GetAnyArray (pBase, _P_ANYTYPE, _P_ARR);
        pPattern = T5Tools_GetAnyParam (pBase, _P_ANYTYPE, _P_PATTERN, NULL, NULL);
        _P_COUNT = 0;
        if (pArray != NULL && pPattern != NULL
            && _P_STARTINDEX >= 0 && _P_STARTINDEX < (T5_LONG)_P_ARR_SIZE)
        {
            switch (_P_ANYTYPE)
            {
            case T5C_BOOL :
            case T5C_SINT :
            case T5C_USINT :
                _P_COUNT = _ARCOUNT_D8 ((T5_PTBYTE)pArray, (T5_WORD)_P_STARTINDEX,
                                        _P_ARR_SIZE, *(T5_PTBYTE)pPattern);
                break;
            case T5C_INT :
            case T5C_UINT :
                _P_COUNT = _ARCOUNT_D16 ((T5_PTWORD)pArray, (T5_WORD)_P_STARTINDEX,
                                         _P_ARR_SIZE, *(T5_PTWORD)pPattern);
                break;
            case T5C_DINT :
            case T5C_UDINT :
            case T5C_REAL :
            case T5C_TIME :
                _P_COUNT = _ARCOUNT_D32 ((T5_PTDWORD)pArray, (T5_WORD)_P_STARTINDEX,
                                         _P_ARR_SIZE, *(T5_PTDWORD)pPattern);
                break;
            case T5C_LINT :
            case T5C_ULINT :
            case T5C_LREAL :
#ifdef T5DEF_DATA64SUPPORTED
                _P_COUNT = _ARCOUNT_D64 ((T5_PTDATA64)pArray, (T5_WORD)_P_STARTINDEX,
                                         _P_ARR_SIZE, *(T5_PTDATA64)pPattern);
#endif /*T5DEF_DATA64SUPPORTED*/
                break;
            case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
                _P_COUNT = _ARCOUNT_STRING ((T5_PTBYTE)pArray, (T5_WORD)_P_STARTINDEX,
                                            _P_ARR_SIZE, (T5_PTBYTE)pPattern);
#endif /*T5DEF_STRINGSUPPORTED*/
                break;
            default : break;
            }
        }
    }
    _F_END
}

#undef _P_ANYTYPE
#undef _P_ARR
#undef _P_ARR_SIZE
#undef _P_PATTERN
#undef _P_STARTINDEX
#undef _P_COUNT

/****************************************************************************/
/* ArFind */

static T5_LONG _ARFIND_D8 (T5_PTBYTE pArray, T5_WORD wStart, T5_WORD wDim, T5_BYTE bPattern)
{
    T5_WORD i;
    T5_LONG lCount;

    lCount = 0L;
    for (i=wStart; i<wDim; i++)
    {
        if (pArray[i] == bPattern)
            return (T5_LONG)i;
    }
    return -1;
}

static T5_LONG _ARFIND_D16 (T5_PTWORD pArray, T5_WORD wStart, T5_WORD wDim, T5_WORD wPattern)
{
    T5_WORD i;
    T5_LONG lCount;

    lCount = 0L;
    for (i=wStart; i<wDim; i++)
    {
        if (pArray[i] == wPattern)
            return (T5_LONG)i;
    }
    return -1;
}

static T5_LONG _ARFIND_D32 (T5_PTDWORD pArray, T5_WORD wStart, T5_WORD wDim, T5_DWORD dwPattern)
{
    T5_WORD i;
    T5_LONG lCount;

    lCount = 0L;
    for (i=wStart; i<wDim; i++)
    {
        if (pArray[i] == dwPattern)
            return (T5_LONG)i;
    }
    return -1;
}

#ifdef T5DEF_DATA64SUPPORTED

static T5_LONG _ARFIND_D64 (T5_PTDATA64 pArray, T5_WORD wStart, T5_WORD wDim, T5_DATA64 lPattern)
{
    T5_WORD i;
    T5_LONG lCount;

    lCount = 0L;
    for (i=wStart; i<wDim; i++)
    {
        if (pArray[i] == lPattern)
            return (T5_LONG)i;
    }
    return -1;
}

#endif /*T5DEF_DATA64SUPPORTED*/

#ifdef T5DEF_STRINGSUPPORTED

static T5_LONG _ARFIND_STRING (T5_PTBYTE pArray, T5_WORD wStart, T5_WORD wDim, T5_PTBYTE pPattern)
{
    T5_WORD i, wLen;
    T5_LONG lCount;

    wLen = (T5_WORD)pArray[0] + 3;
    pArray += (wStart * wLen);
    lCount = 0L;
    for (i=wStart; i<wDim; i++, pArray += wLen)
    {
        if (T5_MEMCMP(pArray + 1, pPattern + 1, pArray[1] + 1) == 0)
            return (T5_LONG)i;
    }
    return -1;
}

#endif /*T5DEF_STRINGSUPPORTED*/

#define _P_ANYTYPE      pArgs[0]
#define _P_ARR          pArgs[1]
#define _P_ARR_SIZE     pArgs[2]
#define _P_PATTERN      pArgs[3]
#define _P_STARTINDEX   (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[4]))
#define _P_INDEX        (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[5]))

T5_DWORD ARFIND (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
    T5_PTR pArray, pPattern;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        pArray = T5Tools_GetAnyArray (pBase, _P_ANYTYPE, _P_ARR);
        pPattern = T5Tools_GetAnyParam (pBase, _P_ANYTYPE, _P_PATTERN, NULL, NULL);
        _P_INDEX = -1;
        if (pArray != NULL && pPattern != NULL
            && _P_STARTINDEX >= 0 && _P_STARTINDEX < (T5_LONG)_P_ARR_SIZE)
        {
            switch (_P_ANYTYPE)
            {
            case T5C_BOOL :
            case T5C_SINT :
            case T5C_USINT :
                _P_INDEX = _ARFIND_D8 ((T5_PTBYTE)pArray, (T5_WORD)_P_STARTINDEX,
                                       _P_ARR_SIZE, *(T5_PTBYTE)pPattern);
                break;
            case T5C_INT :
            case T5C_UINT :
                _P_INDEX = _ARFIND_D16 ((T5_PTWORD)pArray, (T5_WORD)_P_STARTINDEX,
                                        _P_ARR_SIZE, *(T5_PTWORD)pPattern);
                break;
            case T5C_DINT :
            case T5C_UDINT :
            case T5C_REAL :
            case T5C_TIME :
                _P_INDEX = _ARFIND_D32 ((T5_PTDWORD)pArray, (T5_WORD)_P_STARTINDEX,
                                        _P_ARR_SIZE, *(T5_PTDWORD)pPattern);
                break;
            case T5C_LINT :
            case T5C_ULINT :
            case T5C_LREAL :
#ifdef T5DEF_DATA64SUPPORTED
                _P_INDEX = _ARFIND_D64 ((T5_PTDATA64)pArray, (T5_WORD)_P_STARTINDEX,
                                        _P_ARR_SIZE, *(T5_PTDATA64)pPattern);
#endif /*T5DEF_DATA64SUPPORTED*/
                break;
            case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
                _P_INDEX = _ARFIND_STRING ((T5_PTBYTE)pArray, (T5_WORD)_P_STARTINDEX,
                                           _P_ARR_SIZE, (T5_PTBYTE)pPattern);
#endif /*T5DEF_STRINGSUPPORTED*/
                break;
            default : break;
            }
        }
    }
    _F_END
}

#undef _P_ANYTYPE
#undef _P_ARR
#undef _P_ARR_SIZE
#undef _P_PATTERN
#undef _P_STARTINDEX
#undef _P_INDEX

/****************************************************************************/
/* ArFill */

static void _ARFILL_D8 (T5_PTBYTE pArray, T5_WORD wDim, T5_BYTE bPattern)
{
    T5_WORD i;

    for (i=0; i<wDim; i++)
        pArray[i] = bPattern;
}

static void _ARFILL_D16 (T5_PTWORD pArray, T5_WORD wDim, T5_WORD wPattern)
{
    T5_WORD i;

    for (i=0; i<wDim; i++)
        pArray[i] = wPattern;
}

static void _ARFILL_D32 (T5_PTDWORD pArray, T5_WORD wDim, T5_DWORD dwPattern)
{
    T5_WORD i;

    for (i=0; i<wDim; i++)
        pArray[i] = dwPattern;
}

#ifdef T5DEF_DATA64SUPPORTED

static void _ARFILL_D64 (T5_PTDATA64 pArray, T5_WORD wDim, T5_DATA64 lPattern)
{
    T5_WORD i;

    for (i=0; i<wDim; i++)
        pArray[i] = lPattern;
}

#endif /*T5DEF_DATA64SUPPORTED*/

#ifdef T5DEF_STRINGSUPPORTED

static void _ARFILL_STRING (T5_PTBYTE pArray, T5_WORD wDim, T5_PTBYTE pPattern)
{
    T5_WORD i, wLen, wStep;

    wStep = (T5_WORD)pArray[0] + 3;
    wLen = (T5_WORD)pPattern[1];
    if (wLen > (T5_WORD)(pArray[0]))
        wLen = (T5_WORD)(pArray[0]);

    for (i=0; i<wDim; i++, pArray += wStep)
    {
        pArray[1] = (T5_BYTE)wLen;
        T5_MEMCPY(pArray + 2, pPattern + 2, wLen + 1);
    }
}

#endif /*T5DEF_STRINGSUPPORTED*/

#define _P_ANYTYPE      pArgs[0]
#define _P_ARR          pArgs[1]
#define _P_ARR_SIZE     pArgs[2]
#define _P_VALUE        pArgs[3]
#define _P_OK           (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[4]))

T5_DWORD ARFILL (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
    T5_PTR pArray, pPattern;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        pArray = T5Tools_GetAnyArray (pBase, _P_ANYTYPE, _P_ARR);
        pPattern = T5Tools_GetAnyParam (pBase, _P_ANYTYPE, _P_VALUE, NULL, NULL);
        if (pArray == NULL || pPattern == NULL)
            _P_OK = FALSE;
        else
        {
            _P_OK = TRUE;
            switch (_P_ANYTYPE)
            {
            case T5C_BOOL :
            case T5C_SINT :
            case T5C_USINT :
                _ARFILL_D8 ((T5_PTBYTE)pArray, _P_ARR_SIZE, *(T5_PTBYTE)pPattern);
                break;
            case T5C_INT :
            case T5C_UINT :
                _ARFILL_D16 ((T5_PTWORD)pArray, _P_ARR_SIZE, *(T5_PTWORD)pPattern);
                break;
            case T5C_DINT :
            case T5C_UDINT :
            case T5C_REAL :
            case T5C_TIME :
                _ARFILL_D32 ((T5_PTDWORD)pArray, _P_ARR_SIZE, *(T5_PTDWORD)pPattern);
                break;
            case T5C_LINT :
            case T5C_ULINT :
            case T5C_LREAL :
#ifdef T5DEF_DATA64SUPPORTED
                _ARFILL_D64 ((T5_PTDATA64)pArray, _P_ARR_SIZE, *(T5_PTDATA64)pPattern);
#endif /*T5DEF_DATA64SUPPORTED*/
                break;
            case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
                _ARFILL_STRING ((T5_PTBYTE)pArray, _P_ARR_SIZE, (T5_PTBYTE)pPattern);
#endif /*T5DEF_STRINGSUPPORTED*/
                break;
            default : break;
            }
        }
    }
    _F_END
}

#undef _P_ANYTYPE
#undef _P_ARR
#undef _P_ARR_SIZE
#undef _P_VALUE
#undef _P_OK

/****************************************************************************/
/* ArMin */

static T5_BYTE _ARMIN_USINT (T5_PTBYTE pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_BYTE b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] < b)
            b = pArray[i];
    }
    return b;
}

static T5_CHAR _ARMIN_SINT (T5_PTCHAR pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_CHAR b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] < b)
            b = pArray[i];
    }
    return b;
}

static T5_SHORT _ARMIN_INT (T5_PTSHORT pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_SHORT b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] < b)
            b = pArray[i];
    }
    return b;
}

static T5_WORD _ARMIN_UINT (T5_PTWORD pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_WORD b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] < b)
            b = pArray[i];
    }
    return b;
}

static T5_LONG _ARMIN_DINT (T5_PTLONG pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_LONG b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] < b)
            b = pArray[i];
    }
    return b;
}

static T5_DWORD _ARMIN_DWORD (T5_PTDWORD pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_DWORD b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] < b)
            b = pArray[i];
    }
    return b;
}

#ifdef T5DEF_REALSUPPORTED

static T5_REAL _ARMIN_REAL (T5_PTREAL pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_REAL b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] < b)
            b = pArray[i];
    }
    return b;
}

#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_LINTSUPPORTED

static T5_LINT _ARMIN_LINT (T5_PTLINT pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_LINT b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] < b)
            b = pArray[i];
    }
    return b;
}

#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_ULINTSUPPORTED

static T5_LWORD _ARMIN_ULINT (T5_PTLWORD pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_LWORD b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] < b)
            b = pArray[i];
    }
    return b;
}

#endif /*T5DEF_ULINTSUPPORTED*/

#ifdef T5DEF_LREALSUPPORTED

static T5_LREAL _ARMIN_LREAL (T5_PTLREAL pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_LREAL b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] < b)
            b = pArray[i];
    }
    return b;
}

#endif /*T5DEF_LREALSUPPORTED*/

#ifdef T5DEF_STRINGSUPPORTED

void _ARMIN_STRING (T5_PTBYTE pArray, T5_WORD wDim, T5_PTBYTE pValue)
{
    T5_WORD i, wLen;
    T5_BYTE buffer[257];

    T5_MEMCPY (buffer, pArray + 1, pArray[1] + 2);
    wLen = (T5_WORD)(pArray[0]) + 3;
    for (i=0; i<wDim; i++, pArray += wLen)
    {
        if (_ARSTRCMP (pArray + 1, buffer) < 0)
            T5_MEMCPY (buffer, pArray + 1, pArray[1] + 2);
    }
    if (buffer[0] > pValue[0])
        buffer[0] = pValue[0];
    T5_MEMCPY (pValue + 1, buffer, *buffer + 2);
}

#endif /*T5DEF_STRINGSUPPORTED*/

#define _P_ANYTYPE   pArgs[0]
#define _P_ARR       pArgs[1]
#define _P_ARR_SIZE  pArgs[2]
#define _P_Q         pArgs[3]

T5_DWORD ARMIN (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
    T5_PTR pArray, pValue;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        pArray = T5Tools_GetAnyArray (pBase, _P_ANYTYPE, _P_ARR);
        pValue = T5Tools_GetAnyParam (pBase, _P_ANYTYPE, _P_Q, NULL, NULL);
        if (pArray != NULL && pValue != NULL)
        {
            switch (_P_ANYTYPE)
            {
            case T5C_BOOL :
            case T5C_USINT :
                *(T5_PTBYTE)pValue = _ARMIN_USINT ((T5_PTBYTE)pArray, _P_ARR_SIZE);
                break;
            case T5C_SINT :
                *(T5_PTCHAR)pValue = _ARMIN_SINT ((T5_PTCHAR)pArray, _P_ARR_SIZE);
                break;
            case T5C_INT :
                *(T5_PTSHORT)pValue = _ARMIN_INT ((T5_PTSHORT)pArray, _P_ARR_SIZE);
                break;
            case T5C_UINT :
                *(T5_PTWORD)pValue = _ARMIN_UINT ((T5_PTWORD)pArray, _P_ARR_SIZE);
                break;
            case T5C_DINT :
                *(T5_PTLONG)pValue = _ARMIN_DINT ((T5_PTLONG)pArray, _P_ARR_SIZE);
                break;
            case T5C_UDINT :
            case T5C_TIME :
                *(T5_PTDWORD)pValue = _ARMIN_DWORD ((T5_PTDWORD)pArray, _P_ARR_SIZE);
                break;
            case T5C_REAL :
#ifdef T5DEF_REALSUPPORTED
                *(T5_PTREAL)pValue = _ARMIN_REAL ((T5_PTREAL)pArray, _P_ARR_SIZE);
#endif /*T5DEF_REALSUPPORTED*/
                break;
            case T5C_LINT :
#ifdef T5DEF_LINTSUPPORTED
                *(T5_PTLINT)pValue = _ARMIN_LINT ((T5_PTLINT)pArray, _P_ARR_SIZE);
#endif /*T5DEF_LINTSUPPORTED*/
                break;
            case T5C_ULINT :
#ifdef T5DEF_ULINTSUPPORTED
                *(T5_PTLWORD)pValue = _ARMIN_ULINT ((T5_PTLWORD)pArray, _P_ARR_SIZE);
#endif /*T5DEF_ULINTSUPPORTED*/
            case T5C_LREAL :
#ifdef T5DEF_LREALSUPPORTED
                *(T5_PTLREAL)pValue = _ARMIN_LREAL ((T5_PTLREAL)pArray, _P_ARR_SIZE);
#endif /*T5DEF_LREALSUPPORTED*/
                break;
            case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
                _ARMIN_STRING ((T5_PTBYTE)pArray, _P_ARR_SIZE, (T5_PTBYTE)pValue);
#endif /*T5DEF_STRINGSUPPORTED*/
                break;
            default : break;
            }
        }
    }
    _F_END
}

#undef _P_ANYTYPE
#undef _P_ARR
#undef _P_ARR_SIZE
#undef _P_Q

/****************************************************************************/
/* ArMax */

static T5_BYTE _ARMAX_USINT (T5_PTBYTE pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_BYTE b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] > b)
            b = pArray[i];
    }
    return b;
}

static T5_CHAR _ARMAX_SINT (T5_PTCHAR pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_CHAR b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] > b)
            b = pArray[i];
    }
    return b;
}

static T5_SHORT _ARMAX_INT (T5_PTSHORT pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_SHORT b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] > b)
            b = pArray[i];
    }
    return b;
}

static T5_WORD _ARMAX_UINT (T5_PTWORD pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_WORD b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] > b)
            b = pArray[i];
    }
    return b;
}

static T5_LONG _ARMAX_DINT (T5_PTLONG pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_LONG b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] > b)
            b = pArray[i];
    }
    return b;
}

static T5_DWORD _ARMAX_DWORD (T5_PTDWORD pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_DWORD b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] > b)
            b = pArray[i];
    }
    return b;
}

#ifdef T5DEF_REALSUPPORTED

static T5_REAL _ARMAX_REAL (T5_PTREAL pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_REAL b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] > b)
            b = pArray[i];
    }
    return b;
}

#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_LINTSUPPORTED

static T5_LINT _ARMAX_LINT (T5_PTLINT pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_LINT b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] > b)
            b = pArray[i];
    }
    return b;
}

#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_ULINTSUPPORTED

static T5_LWORD _ARMAX_ULINT (T5_PTLWORD pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_LWORD b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] > b)
            b = pArray[i];
    }
    return b;
}

#endif /*T5DEF_ULINTSUPPORTED*/

#ifdef T5DEF_LREALSUPPORTED

static T5_LREAL _ARMAX_LREAL (T5_PTLREAL pArray, T5_WORD wDim)
{
    T5_WORD i;
    T5_LREAL b;

    b = *pArray;
    for (i=1; i<wDim; i++)
    {
        if (pArray[i] > b)
            b = pArray[i];
    }
    return b;
}

#endif /*T5DEF_LREALSUPPORTED*/

#ifdef T5DEF_STRINGSUPPORTED

void _ARMAX_STRING (T5_PTBYTE pArray, T5_WORD wDim, T5_PTBYTE pValue)
{
    T5_WORD i, wLen;
    T5_BYTE buffer[257];

    T5_MEMSET (buffer, 0, sizeof (buffer));
    wLen = (T5_WORD)(pArray[0]) + 3;
    for (i=0; i<wDim; i++, pArray += wLen)
    {
        if (_ARSTRCMP (pArray + 1, buffer) > 0)
            T5_MEMCPY (buffer, pArray + 1, pArray[1] + 2);
    }
    if (buffer[0] > pValue[0])
        buffer[0] = pValue[0];
    T5_MEMCPY (pValue + 1, buffer, *buffer + 2);
}

#endif /*T5DEF_STRINGSUPPORTED*/

#define _P_ANYTYPE   pArgs[0]
#define _P_ARR       pArgs[1]
#define _P_ARR_SIZE  pArgs[2]
#define _P_Q         pArgs[3]

T5_DWORD ARMAX (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
    T5_PTR pArray, pValue;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        pArray = T5Tools_GetAnyArray (pBase, _P_ANYTYPE, _P_ARR);
        pValue = T5Tools_GetAnyParam (pBase, _P_ANYTYPE, _P_Q, NULL, NULL);
        if (pArray != NULL && pValue != NULL)
        {
            switch (_P_ANYTYPE)
            {
            case T5C_BOOL :
            case T5C_USINT :
                *(T5_PTBYTE)pValue = _ARMAX_USINT ((T5_PTBYTE)pArray, _P_ARR_SIZE);
                break;
            case T5C_SINT :
                *(T5_PTCHAR)pValue = _ARMAX_SINT ((T5_PTCHAR)pArray, _P_ARR_SIZE);
                break;
            case T5C_INT :
                *(T5_PTSHORT)pValue = _ARMAX_INT ((T5_PTSHORT)pArray, _P_ARR_SIZE);
                break;
            case T5C_UINT :
                *(T5_PTWORD)pValue = _ARMAX_UINT ((T5_PTWORD)pArray, _P_ARR_SIZE);
                break;
            case T5C_DINT :
                *(T5_PTLONG)pValue = _ARMAX_DINT ((T5_PTLONG)pArray, _P_ARR_SIZE);
                break;
            case T5C_UDINT :
            case T5C_TIME :
                *(T5_PTDWORD)pValue = _ARMAX_DWORD ((T5_PTDWORD)pArray, _P_ARR_SIZE);
                break;
            case T5C_REAL :
#ifdef T5DEF_REALSUPPORTED
                *(T5_PTREAL)pValue = _ARMAX_REAL ((T5_PTREAL)pArray, _P_ARR_SIZE);
#endif /*T5DEF_REALSUPPORTED*/
                break;
            case T5C_LINT :
#ifdef T5DEF_LINTSUPPORTED
                *(T5_PTLINT)pValue = _ARMAX_LINT ((T5_PTLINT)pArray, _P_ARR_SIZE);
#endif /*T5DEF_LINTSUPPORTED*/
                break;
            case T5C_ULINT :
#ifdef T5DEF_ULINTSUPPORTED
                *(T5_PTLWORD)pValue = _ARMAX_ULINT ((T5_PTLWORD)pArray, _P_ARR_SIZE);
#endif /*T5DEF_ULINTSUPPORTED*/
            case T5C_LREAL :
#ifdef T5DEF_LREALSUPPORTED
                *(T5_PTLREAL)pValue = _ARMAX_LREAL ((T5_PTLREAL)pArray, _P_ARR_SIZE);
#endif /*T5DEF_LREALSUPPORTED*/
                break;
            case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
                _ARMAX_STRING ((T5_PTBYTE)pArray, _P_ARR_SIZE, (T5_PTBYTE)pValue);
#endif /*T5DEF_STRINGSUPPORTED*/
                break;
            default : break;
            }
        }
    }
    _F_END
}

#undef _P_ANYTYPE
#undef _P_ARR
#undef _P_ARR_SIZE
#undef _P_Q

/****************************************************************************/
/* ArSort */

static void _ARSORT_USINT (T5_PTBYTE pArray, T5_WORD wDim)
{
    T5_WORD i, j;
    T5_BYTE t;

    for (i=0; i<wDim-1; i++)
    {
        for (j=i+1; j<wDim; j++)
        {
            if (pArray[i] > pArray[j])
            {
                t = pArray[i];
                pArray[i] = pArray[j];
                pArray[j] = t;
            }
        }
    }
}

static void _ARSORT_SINT (T5_PTCHAR pArray, T5_WORD wDim)
{
    T5_WORD i, j;
    T5_CHAR t;

    for (i=0; i<wDim-1; i++)
    {
        for (j=i+1; j<wDim; j++)
        {
            if (pArray[i] > pArray[j])
            {
                t = pArray[i];
                pArray[i] = pArray[j];
                pArray[j] = t;
            }
        }
    }
}

static void _ARSORT_INT (T5_PTSHORT pArray, T5_WORD wDim)
{
    T5_WORD i, j;
    T5_WORD t;

    for (i=0; i<wDim-1; i++)
    {
        for (j=i+1; j<wDim; j++)
        {
            if (pArray[i] > pArray[j])
            {
                t = pArray[i];
                pArray[i] = pArray[j];
                pArray[j] = t;
            }
        }
    }
}

static void _ARSORT_UINT (T5_PTWORD pArray, T5_WORD wDim)
{
    T5_WORD i, j;
    T5_WORD t;

    for (i=0; i<wDim-1; i++)
    {
        for (j=i+1; j<wDim; j++)
        {
            if (pArray[i] > pArray[j])
            {
                t = pArray[i];
                pArray[i] = pArray[j];
                pArray[j] = t;
            }
        }
    }
}

static void _ARSORT_DINT (T5_PTLONG pArray, T5_WORD wDim)
{
    T5_WORD i, j;
    T5_LONG t;

    for (i=0; i<wDim-1; i++)
    {
        for (j=i+1; j<wDim; j++)
        {
            if (pArray[i] > pArray[j])
            {
                t = pArray[i];
                pArray[i] = pArray[j];
                pArray[j] = t;
            }
        }
    }
}

static void _ARSORT_DWORD (T5_PTDWORD pArray, T5_WORD wDim)
{
    T5_WORD i, j;
    T5_DWORD t;

    for (i=0; i<wDim-1; i++)
    {
        for (j=i+1; j<wDim; j++)
        {
            if (pArray[i] > pArray[j])
            {
                t = pArray[i];
                pArray[i] = pArray[j];
                pArray[j] = t;
            }
        }
    }
}

#ifdef T5DEF_REALSUPPORTED

static void _ARSORT_REAL (T5_PTREAL pArray, T5_WORD wDim)
{
    T5_WORD i, j;
    T5_REAL t;

    for (i=0; i<wDim-1; i++)
    {
        for (j=i+1; j<wDim; j++)
        {
            if (pArray[i] > pArray[j])
            {
                t = pArray[i];
                pArray[i] = pArray[j];
                pArray[j] = t;
            }
        }
    }
}

#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_LINTSUPPORTED

static void _ARSORT_LINT (T5_PTLINT pArray, T5_WORD wDim)
{
    T5_WORD i, j;
    T5_LINT t;

    for (i=0; i<wDim-1; i++)
    {
        for (j=i+1; j<wDim; j++)
        {
            if (pArray[i] > pArray[j])
            {
                t = pArray[i];
                pArray[i] = pArray[j];
                pArray[j] = t;
            }
        }
    }
}

#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_ULINTSUPPORTED

static void _ARSORT_ULINT (T5_PTLWORD pArray, T5_WORD wDim)
{
    T5_WORD i, j;
    T5_LWORD t;

    for (i=0; i<wDim-1; i++)
    {
        for (j=i+1; j<wDim; j++)
        {
            if (pArray[i] > pArray[j])
            {
                t = pArray[i];
                pArray[i] = pArray[j];
                pArray[j] = t;
            }
        }
    }
}

#endif /*T5DEF_ULINTSUPPORTED*/

#ifdef T5DEF_LREALSUPPORTED

static void _ARSORT_LREAL (T5_PTLREAL pArray, T5_WORD wDim)
{
    T5_WORD i, j;
    T5_LREAL t;

    for (i=0; i<wDim-1; i++)
    {
        for (j=i+1; j<wDim; j++)
        {
            if (pArray[i] > pArray[j])
            {
                t = pArray[i];
                pArray[i] = pArray[j];
                pArray[j] = t;
            }
        }
    }
}

#endif /*T5DEF_LREALSUPPORTED*/

#ifdef T5DEF_STRINGSUPPORTED

void _ARSORT_STRING (T5_PTBYTE pArray, T5_WORD wDim)
{
    T5_WORD i, j, wLen;
    T5_BYTE t[257];

    wLen = (T5_WORD)(pArray[0]) + 3;
    for (i=0; i<wDim-1; i++)
    {
        for (j=i+1; j<wDim; j++)
        {
            if (_ARSTRCMP (pArray + i * wLen + 1, pArray + j * wLen + 1) > 0)
            {
                T5_MEMCPY (t, pArray + i * wLen + 1, pArray[i * wLen + 1] + 2);
                T5_MEMCPY (pArray + i * wLen + 1, pArray + j * wLen + 1, pArray[j * wLen + 1] + 2);
                T5_MEMCPY (pArray + j * wLen + 1, t, t[0] + 2);
            }
        }
    }
}

#endif /*T5DEF_STRINGSUPPORTED*/

#define _P_ANYTYPE      pArgs[0]
#define _P_ARR          pArgs[1]
#define _P_ARR_SIZE     pArgs[2]
#define _P_OK           (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))

T5_DWORD ARSORT (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
    T5_PTR pArray;
    T5_UNUSED(pInst);
    T5_UNUSED(pClass);

    _F_BEGIN
    {
        pArray = T5Tools_GetAnyArray (pBase, _P_ANYTYPE, _P_ARR);
        if (pArray == NULL)
            _P_OK = FALSE;
        else if (_P_ARR_SIZE < 2)
            _P_OK = TRUE;
        else
        {
            _P_OK = TRUE;
            switch (_P_ANYTYPE)
            {
            case T5C_BOOL :
            case T5C_USINT :
                _ARSORT_USINT ((T5_PTBYTE)pArray, _P_ARR_SIZE);
                break;
            case T5C_SINT :
                _ARSORT_SINT ((T5_PTCHAR)pArray, _P_ARR_SIZE);
                break;
            case T5C_INT :
                _ARSORT_INT ((T5_PTSHORT)pArray, _P_ARR_SIZE);
                break;
            case T5C_UINT :
                _ARSORT_UINT ((T5_PTWORD)pArray, _P_ARR_SIZE);
                break;
            case T5C_DINT :
                _ARSORT_DINT ((T5_PTLONG)pArray, _P_ARR_SIZE);
                break;
            case T5C_UDINT :
            case T5C_TIME :
                _ARSORT_DWORD ((T5_PTDWORD)pArray, _P_ARR_SIZE);
                break;
            case T5C_REAL :
#ifdef T5DEF_REALSUPPORTED
                _ARSORT_REAL ((T5_PTREAL)pArray, _P_ARR_SIZE);
#endif /*T5DEF_REALSUPPORTED*/
                break;
            case T5C_LINT :
#ifdef T5DEF_LINTSUPPORTED
                _ARSORT_LINT ((T5_PTLINT)pArray, _P_ARR_SIZE);
#endif /*T5DEF_LINTSUPPORTED*/
                break;
            case T5C_ULINT :
#ifdef T5DEF_ULINTSUPPORTED
                _ARSORT_ULINT ((T5_PTLWORD)pArray, _P_ARR_SIZE);
#endif /*T5DEF_ULINTSUPPORTED*/
            case T5C_LREAL :
#ifdef T5DEF_LREALSUPPORTED
                _ARSORT_LREAL ((T5_PTLREAL)pArray, _P_ARR_SIZE);
#endif /*T5DEF_LREALSUPPORTED*/
                break;
            case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
                _ARSORT_STRING ((T5_PTBYTE)pArray, _P_ARR_SIZE);
#endif /*T5DEF_STRINGSUPPORTED*/
                break;
            default : break;
            }
        }
    }
    _F_END
}

/* Undefine argument list */

#undef _P_ANYTYPE
#undef _P_ARR
#undef _P_ARR_SIZE
#undef _P_OK


/****************************************************************************/

#endif /*T5DEF_BARRAY*/

/* eof **********************************************************************/
