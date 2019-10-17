/*****************************************************************************
T5VMstf.c :  standard functions

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/
/* PART ONE : Helpers */

/****************************************************************************/
/* access to outputs */

#define ISLOCK8(i)      ((pLock[i] & T5LOCK_DATA8) != 0)
#define ISLOCK16(i)     ((pLock[i] & T5LOCK_DATA16) != 0)
#define ISLOCK32(i)     ((pLock[i] & T5LOCK_DATA32) != 0)
#define ISLOCK64(i)     ((pLock[i] & T5LOCK_DATA64) != 0)
#define ISLOCKTIME(i)   ((pLock[i] & T5LOCK_TIME) != 0)
#define ISLOCKSTRING(i) ((pLock[i] & T5LOCK_STRING) != 0)

#ifdef T5DEF_DONTFORCELOCKEDVARS

#define IFNOT_ISD8LOCKED(i)     if (!ISLOCK8(i))
#define IFNOT_ISD16LOCKED(i)    if (!ISLOCK16(i))
#define IFNOT_ISD32LOCKED(i)    if (!ISLOCK32(i))
#define IFNOT_ISD64LOCKED(i)    if (!ISLOCK64(i))
#define IFNOT_ISTIMELOCKED(i)   if (!ISLOCKTIME(i))
#define IFNOT_ISSTRINGLOCKED(i) if (!ISLOCKSTRING(i))

#else /*T5DEF_DONTFORCELOCKEDOUTPUTS*/

#define IFNOT_ISD8LOCKED(i)     /*nothing*/
#define IFNOT_ISD16LOCKED(i)    /*nothing*/
#define IFNOT_ISD32LOCKED(i)    /*nothing*/
#define IFNOT_ISD64LOCKED(i)    /*nothing*/
#define IFNOT_ISTIMELOCKED(i)   /*nothing*/
#define IFNOT_ISSTRINGLOCKED(i) /*nothing*/

#endif /*T5DEF_DONTFORCELOCKEDOUTPUTS*/

/****************************************************************************/
/* access to 8 bit data */

#define GET_D8IN(i)     (pData8[pArgs[i]])
#define SET_D8OUT(b)    { IFNOT_ISD8LOCKED(*pArgs) pData8[*pArgs] = b; }

/****************************************************************************/
/* access to 16 bit data */

#define GET_D16IN(i)    (pData16[pArgs[i]])
#define SET_D16OUT(w)   { IFNOT_ISD16LOCKED(*pArgs) pData16[*pArgs] = w; }

/****************************************************************************/
/* access to 32 bit data */

#define GET_DINTIN(i)    (pDint[pArgs[i]])
#define SET_DINTOUT(d)   { IFNOT_ISD32LOCKED(*pArgs) pDint[*pArgs] = d; }

#ifdef T5DEF_REALSUPPORTED
#define GET_REALIN(i)    (pReal[pArgs[i]])
#define SET_REALOUT(r)   { IFNOT_ISD32LOCKED(*pArgs) pReal[*pArgs] = r; }
#endif /*T5DEF_REALSUPPORTED*/

#ifdef T5DEF_LREALSUPPORTED
#define GET_LREALIN(i)   ((T5_LREAL)(pReal[pArgs[i]]))
#else /*T5DEF_LREALSUPPORTED*/
#define GET_LREALIN(i)   ((T5_REAL)pReal[pArgs[i]])
#endif /*T5DEF_LREALSUPPORTED*/

/****************************************************************************/
/* access to 64 bit data */

#ifdef T5DEF_LREALSUPPORTED
#define GET_REAL64IN(i)  (pReal64[pArgs[i]])
#define SET_REAL64OUT(d) { IFNOT_ISD64LOCKED(*pArgs) pReal64[*pArgs] = d; }
#endif /*T5DEF_LREALSUPPORTED*/

#ifdef T5DEF_DATA64SUPPORTED
#define GET_D64IN(i)  (pD64[pArgs[i]])
#define SET_D64OUT(d) { IFNOT_ISD64LOCKED(*pArgs) pD64[*pArgs] = d; }
#endif /*T5DEF_DATA64SUPPORTED*/

/****************************************************************************/
/* PART TWO : declaration of static services */

static T5_BOOL _MoveBlock (T5PTR_DB pDB, T5_WORD wType,
                           T5_WORD wSrc, T5_WORD wSrcSizeof,
                           T5_WORD wDst, T5_WORD wDstSizeof,
                           T5_DWORD dwPosSrc, T5_DWORD dwPosDst,
                           T5_DWORD dwNb);

#ifdef T5DEF_SERIALIZEDATA

static T5_LONG _SerializeOut (T5PTR_DB pDB, T5_WORD wType,
                              T5_WORD wDst, T5_WORD wDstSizeof,
                              T5_WORD wSrc, T5_DWORD dwPos, T5_BOOL bBig);

static T5_LONG _SerializeIn (T5PTR_DB pDB, T5_WORD wType,
                             T5_WORD wSrc, T5_WORD wSrcSizeof,
                             T5_WORD wDst, T5_DWORD dwPos, T5_BOOL bBig);

#ifdef T5DEF_STRINGSUPPORTED

static T5_LONG _SerGetString (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wDstSizeof,
                              T5_WORD wDst, T5_DWORD dwPos, T5_DWORD dwMaxLen,
                              T5_BOOL bEos, T5_BOOL bHeaded);

static T5_LONG _SerPutString (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wDstSizeof,
                              T5_WORD wSrc, T5_DWORD dwPos, T5_DWORD dwMaxLen,
                              T5_BOOL bEos, T5_BOOL bHeaded);

#endif /*T5DEF_STRINGSUPPORTED*/

static void _CopyBytes (T5_PTBYTE pDst, T5_PTBYTE pSrc,
                        T5_WORD wSizeof, T5_BOOL bBig);

#endif /*T5DEF_SERIALIZEDATA*/

static T5_DWORD _GetSysInfo (T5PTR_DB pDB, T5_DWORD dwID);

static T5_BOOL _MBShift (T5PTR_DB pDB, T5_WORD wType,
                         T5_WORD wBase, T5_WORD wSizeof, T5_DWORD dwPos,
                         T5_DWORD dwNbByte, T5_DWORD dwNbShift,
                         T5_BOOL bRight, T5_BOOL bRotate, T5_BOOL bInBit);

#ifdef T5DEF_RECIPES

static T5_BOOL _ApplyRcpColumn (T5PTR_DB pDB, T5_LONG lRcp, T5_LONG lCol);
static void _ApplyRcpCell (T5PTR_DB pDB, T5PTR_CODERCPV pCell,
                           T5_WORD wCol, T5_PTBYTE pValue);

#endif /*T5DEF_RECIPES*/

static void _SetBit (T5PTR_DB pDB, T5_WORD wType, T5_WORD wSrc,
                     T5_WORD wDst, T5_DWORD dwBitNo, T5_BYTE bVal);
static T5_BOOL _GetBit (T5PTR_DB pDB, T5_WORD wType,
                        T5_WORD wSrc, T5_DWORD dwBitNo);

#ifdef T5DEF_STRINGSUPPORTED
#ifdef T5DEF_STRINGTABLE

static T5_BOOL _StringTable (T5PTR_DB pDB, T5_DWORD dwIndex);
static void _LoadString (T5PTR_DB pDB, T5_WORD wDst, T5_DWORD dwIndex);

#endif /*T5DEF_STRINGTABLE*/
#endif /*T5DEF_STRINGSUPPORTED*/

#ifdef T5DEF_SIGNAL
#ifdef T5DEF_REALSUPPORTED

static T5_REAL _SigScale (T5PTR_DB pDB, T5_DWORD dwID, T5_WORD wTime);

#endif /*T5DEF_REALSUPPORTED*/
#endif /*T5DEF_SIGNAL*/

#ifdef T5DEF_UDP
#ifdef T5DEF_STRINGSUPPORTED
#ifdef T5DEF_ETHERNET

static T5_DWORD _Udp_Create (T5PTR_DB pDB, T5_DWORD dwPort);
static T5_BOOL _Udp_AddrMake (T5PTR_DB pDB, T5_WORD wStr,
                              T5_DWORD dwPort, T5_WORD pAdd, T5_WORD wSizeof);
static T5_BOOL _Udp_SendTo (T5PTR_DB pDB, T5_DWORD dwSock, T5_DWORD dwNb,
                            T5_WORD pAdd, T5_WORD wSizeof, T5_WORD wStr);
static T5_DWORD _Udp_RcvFrom (T5PTR_DB pDB, T5_DWORD dwSock, T5_DWORD dwNb,
                              T5_WORD pAdd, T5_WORD wSizeof, T5_WORD wStr);

static T5_BOOL _Udp_SendToArr (T5PTR_DB pDB, T5_DWORD dwSock, T5_DWORD dwNb,
                               T5_WORD pAdd, T5_WORD wSizeof, T5_WORD pAddArray, T5_WORD wArraySize);
static T5_DWORD _Udp_RcvFromArr (T5PTR_DB pDB, T5_DWORD dwSock, T5_DWORD dwNb,
                                 T5_WORD pAdd, T5_WORD wSizeof, T5_WORD pAddArray, T5_WORD wArraySize);

#endif /*T5DEF_ETHERNET*/
#endif /*T5DEF_STRINGSUPPORTED*/
#endif /*T5DEF_UDP*/

#ifdef T5DEF_REG
#ifdef T5DEF_STRINGSUPPORTED

static void _RegParGet (T5PTR_DB pDB, T5_WORD wType, T5_PTCHAR szPath,
                        T5_WORD wDst, T5_WORD wSrc);
static T5_BOOL _RegParPut (T5PTR_DB pDB, T5_WORD wType, T5_PTCHAR szPath,
                           T5_WORD wSrc);

#endif /*T5DEF_STRINGSUPPORTED*/
#endif /*T5DEF_REG*/

static T5_BOOL _SetCsvOpt (T5PTR_DB pDB, T5_PTCHAR szSep, T5_PTCHAR szDec);

/****************************************************************************/
/* PART THREE : ALL FUNCTION HANDLERS */

/* math functions ***********************************************************/

static T5_BOOL _STFS_ABS (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    T5_REAL r1;

    r1 = GET_REALIN(1);
    if (r1 >= T5_ZEROREAL)
    {
        SET_REALOUT(r1);
    }
    else
    {
        SET_REALOUT(-r1);
    }
    return TRUE;
#else /*T5DEF_MATHSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MATHSUPPORTED*/
}

static T5_BOOL _STFS_EXPT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL lr1, lr2; /* for fast access */
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL lr1, lr2; /* for fast access */
#endif /*T5DEF_LREALSUPPORTED*/
    T5_LONG l1;

    lr1 = T5_ONELREAL;
    lr2 = GET_LREALIN(1); /* input value */
    l1 = (T5_LONG)GET_DINTIN(2); /* integer exponent */
    if (l1 > 0) 
    {
        while (l1-- && lr1 < T5_MAXLREAL && lr1 > -T5_MAXLREAL) lr1 *= lr2;
        if (lr1 > T5_MAXLREAL) lr1 = T5_MAXLREAL;
        else if (lr1 < -T5_MAXLREAL) lr1 = -T5_MAXLREAL;
    }
    else if (l1 < 0 && lr2 == 0)
        lr1 = T5_MAXLREAL;
    else if (l1 < 0) 
    {
        while (l1++ && (lr1<-T5_MINLREAL || lr1>T5_MINLREAL)) lr1 /= lr2;
        if (lr1 > 0 && lr1 < T5_MINLREAL) lr1 = T5_MINLREAL;
        else if (lr1 < 0 && lr1 > -T5_MINLREAL) lr1 = -T5_MINLREAL;
    }
    else lr1 = T5_ONELREAL;
    SET_REALOUT((T5_REAL)lr1);
    return TRUE;
#else /*T5DEF_MATHSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MATHSUPPORTED*/
}

static T5_BOOL _STFS_POW (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL lr1, lr2; /* for fast access */
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL lr1, lr2; /* for fast access */
#endif /*T5DEF_LREALSUPPORTED*/

    lr1 = GET_LREALIN(1); /* value */
    lr2 = GET_LREALIN(2); /* exponent */
    if (lr1 == T5_ZEROLREAL && lr2 == T5_ZEROLREAL)
    {
        SET_REALOUT(T5_ZEROREAL);
    }
    else
    {
        SET_REALOUT((T5_REAL)pow (lr1, lr2));
    }
    return TRUE;
#else /*T5DEF_MATHSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MATHSUPPORTED*/
}

static T5_BOOL _STFS_LOG (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL lr1; /* for fast access */
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL lr1; /* for fast access */
#endif /*T5DEF_LREALSUPPORTED*/

    lr1 = GET_REALIN(1); /* value */
    if (lr1 <= T5_ZEROLREAL)
    {
        SET_REALOUT(-T5_MAXREAL);
    }
    else
    {
        SET_REALOUT((T5_REAL)log10 (lr1));
    }
    return TRUE;
#else /*T5DEF_MATHSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MATHSUPPORTED*/
}

static T5_BOOL _STFS_LN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL lr1; /* for fast access */
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL lr1; /* for fast access */
#endif /*T5DEF_LREALSUPPORTED*/

    lr1 = GET_REALIN(1); /* value */
    if (lr1 <= T5_ZEROLREAL)
    {
        SET_REALOUT(-T5_MAXREAL);
    }
    else
    {
        SET_REALOUT((T5_REAL)log (lr1));
    }
    return TRUE;
#else /*T5DEF_MATHSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MATHSUPPORTED*/
}

static T5_BOOL _STFS_EXP (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);

    SET_REALOUT((T5_REAL)exp (GET_REALIN(1)));
    return TRUE;
#else /*T5DEF_MATHSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MATHSUPPORTED*/
}

static T5_BOOL _STFS_SQRT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL lr1; /* for fast access */
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL lr1; /* for fast access */
#endif /*T5DEF_LREALSUPPORTED*/

    lr1 = GET_REALIN(1); /* value */
    if (lr1 <= T5_ZEROLREAL)
    {
        SET_REALOUT(T5_ZEROREAL);
    }
    else
    {
        SET_REALOUT((T5_REAL)sqrt (lr1));
    }
    return TRUE;
#else /*T5DEF_MATHSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MATHSUPPORTED*/
}

static T5_BOOL _STFS_TRUNC (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL lr1; /* for fast access */
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL lr1; /* for fast access */
#endif /*T5DEF_LREALSUPPORTED*/

    lr1 = GET_REALIN(1); /* value */
    if (lr1 >= T5_ZEROLREAL)
    {
        SET_REALOUT((T5_REAL)floor (lr1));
    }
    else
    {
        SET_REALOUT((T5_REAL)ceil (lr1));
    }
    return TRUE;
#else /*T5DEF_MATHSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MATHSUPPORTED*/
}

static T5_BOOL _STFS_MODR (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    T5_REAL r1;

    r1 = GET_REALIN(2);
    if (r1 <= T5_ZEROREAL)
    {
        SET_REALOUT(-T5_ONEREAL);
    }
    else
    {
        SET_REALOUT((T5_REAL)fmod (GET_REALIN(1), r1));
    }
    return TRUE;
#else /*T5DEF_MATHSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MATHSUPPORTED*/
}

static T5_BOOL _STFS_SCALELIN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    T5_REAL r1, rMin, rMax, roMin, roMax;


    r1 = GET_REALIN(1);
    rMin = GET_REALIN(2);
    rMax = GET_REALIN(3);
    roMin = GET_REALIN(4);
    roMax = GET_REALIN(5);
    if (rMin >= rMax)
    {
        SET_REALOUT(r1);
    }
    else if (r1 < rMin)
    {
        SET_REALOUT(roMin);
    }
    else if (r1 > rMax)
    {
        SET_REALOUT(roMax);
    }
    else
    {
        SET_REALOUT((roMin * rMax - roMax * rMin) / (rMax - rMin)
                    + r1 * (roMax - roMin) / (rMax - rMin));
    }
    return TRUE;
#else /*T5DEF_MATHSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MATHSUPPORTED*/
}

#ifdef T5DEF_LREALSUPPORTED
#ifdef T5DEF_MATHSUPPORTED
#define T5DEF_MATHLREAL
#endif /*T5DEF_MATHSUPPORTED*/
#endif /*T5DEF_LREALSUPPORTED*/

static T5_BOOL _STFS_ABSL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5_LREAL lr1;

    lr1 = GET_REAL64IN(1);
    if (lr1 >= T5_ZEROLREAL)
    {
        SET_REAL64OUT(lr1);
    }
    else
    {
        SET_REAL64OUT(-lr1);
    }
    return TRUE;
#else /*T5DEF_MATHLREAL*/
    return FALSE;
#endif /*T5DEF_MATHLREAL*/
}

static T5_BOOL _STFS_POWL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5_LREAL lr1, lr2;

    lr1 = GET_REAL64IN(1); /* value */
    lr2 = GET_REAL64IN(2); /* exponent */
    if (lr1 == T5_ZEROLREAL && lr2 == T5_ZEROLREAL)
    {
        SET_REAL64OUT(T5_ZEROREAL);
    }
    else
    {
        SET_REAL64OUT(pow (lr1, lr2));
    }
    return TRUE;
#else /*T5DEF_MATHLREAL*/
    return FALSE;
#endif /*T5DEF_MATHLREAL*/
}

static T5_BOOL _STFS_SQRTL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5_LREAL lr1;

    lr1 = GET_REAL64IN(1); /* value */
    if (lr1 <= T5_ZEROLREAL)
    {
        SET_REAL64OUT(T5_ZEROREAL);
    }
    else
    {
        SET_REAL64OUT(sqrt (lr1));
    }
    return TRUE;
#else /*T5DEF_MATHLREAL*/
    return FALSE;
#endif /*T5DEF_MATHLREAL*/
}

static T5_BOOL _STFS_TRUNCL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5_LREAL lr1;

    lr1 = GET_REAL64IN(1); /* value */
    if (lr1 >= T5_ZEROLREAL)
    {
        SET_REAL64OUT(floor (lr1));
    }
    else
    {
        SET_REAL64OUT(ceil (lr1));
    }
    return TRUE;
#else /*T5DEF_MATHLREAL*/
    return FALSE;
#endif /*T5DEF_MATHLREAL*/
}

static T5_BOOL _STFS_MODLR (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5_LREAL lr1;

    lr1 = GET_REAL64IN(2);
    if (lr1 <= T5_ZEROLREAL)
    {
        SET_REAL64OUT(-T5_ONELREAL);
    }
    else
    {
        SET_REAL64OUT(fmod (GET_REAL64IN(1), lr1));
    }
    return TRUE;
#else /*T5DEF_MATHLREAL*/
    return FALSE;
#endif /*T5DEF_MATHLREAL*/
}

static T5_BOOL _STFS_LNL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5_LREAL lr1;

    lr1 = GET_REAL64IN(1); /* value */
    if (lr1 <= T5_ZEROLREAL)
    {
        SET_REAL64OUT(-T5_MAXLREAL);
    }
    else
    {
        SET_REAL64OUT(log (lr1));
    }
    return TRUE;
#else /*T5DEF_MATHLREAL*/
    return FALSE;
#endif /*T5DEF_MATHLREAL*/
}

static T5_BOOL _STFS_LOGL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5_LREAL lr1;

    lr1 = GET_REAL64IN(1); /* value */
    if (lr1 <= T5_ZEROLREAL)
    {
        SET_REAL64OUT(-T5_MAXLREAL);
    }
    else
    {
        SET_REAL64OUT(log10 (lr1));
    }
    return TRUE;
#else /*T5DEF_MATHLREAL*/
    return FALSE;
#endif /*T5DEF_MATHLREAL*/
}

static T5_BOOL _STFS_EXPL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MATHLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);

        SET_REAL64OUT(exp (GET_REAL64IN(1)));
    return TRUE;
#else /*T5DEF_MATHLREAL*/
    return FALSE;
#endif /*T5DEF_MATHLREAL*/
}

/* trigo functions **********************************************************/

static T5_BOOL _STFS_USEDEGREES (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);

    SET_D8OUT((pStatus->wFlags & T5FLAG_DEGREES) != 0);
    if (GET_D8IN(1))
        pStatus->wFlags |= T5FLAG_DEGREES;
    else
        pStatus->wFlags &= ~T5FLAG_DEGREES;
    return TRUE;
}

#define T5_DEGTORAD(f)  ((f) / (T5_REAL)57.29578)
#define T5_RADTODEG(f)  ((f) * (T5_REAL)57.29578)

static T5_BOOL _STFS_COS (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);

    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
    {
        SET_REALOUT((T5_REAL)cos (T5_DEGTORAD (GET_REALIN(1))));
    }
    else
    {
        SET_REALOUT((T5_REAL)cos (GET_REALIN(1)));
    }
    return TRUE;
#else /*T5DEF_TRIGOSUPPORTED*/
    return FALSE;
#endif /*T5DEF_TRIGOSUPPORTED*/
}

static T5_BOOL _STFS_SIN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);

    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
    {
        SET_REALOUT((T5_REAL)sin (T5_DEGTORAD (GET_REALIN(1))));
    }
    else
    {
        SET_REALOUT((T5_REAL)sin (GET_REALIN(1)));
    }
    return TRUE;
#else /*T5DEF_TRIGOSUPPORTED*/
    return FALSE;
#endif /*T5DEF_TRIGOSUPPORTED*/
}

static T5_BOOL _STFS_TAN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL lr1; /* for fast access */
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL lr1; /* for fast access */
#endif /*T5DEF_LREALSUPPORTED*/

    lr1 = GET_REALIN(1); /* value */
    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
        lr1 = T5_DEGTORAD(lr1);
    if (cos(lr1) > -T5_MINLREAL && cos(lr1) < T5_MINLREAL)
    {
        SET_REALOUT(T5_MAXREAL);
    }
    else
    {
        SET_REALOUT((T5_REAL)tan (lr1));
    }
    return TRUE;
#else /*T5DEF_TRIGOSUPPORTED*/
    return FALSE;
#endif /*T5DEF_TRIGOSUPPORTED*/
}

static T5_BOOL _STFS_ACOS (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL lr1; /* for fast access */
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL lr1; /* for fast access */
#endif /*T5DEF_LREALSUPPORTED*/

    lr1 = GET_REALIN(1); /* value */
    if (lr1 < -T5_ONELREAL || lr1 > T5_ONELREAL)
        lr1 = T5_ZEROLREAL;
    else lr1 = (T5_REAL)acos (lr1);
    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
        lr1 = T5_RADTODEG(lr1);
    SET_REALOUT((T5_REAL)lr1);
    return TRUE;
#else /*T5DEF_TRIGOSUPPORTED*/
    return FALSE;
#endif /*T5DEF_TRIGOSUPPORTED*/
}

static T5_BOOL _STFS_ASIN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL lr1; /* for fast access */
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL lr1; /* for fast access */
#endif /*T5DEF_LREALSUPPORTED*/

    lr1 = GET_REALIN(1); /* value */
    if (lr1 < -T5_ONELREAL || lr1 > T5_ONELREAL)
        lr1 = T5_ZEROLREAL;
    else lr1 = (T5_REAL)asin (lr1);
    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
        lr1 = T5_RADTODEG(lr1);
    SET_REALOUT((T5_REAL)lr1);
    return TRUE;
#else /*T5DEF_TRIGOSUPPORTED*/
    return FALSE;
#endif /*T5DEF_TRIGOSUPPORTED*/
}

static T5_BOOL _STFS_ATAN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL lr1; /* for fast access */
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL lr1; /* for fast access */
#endif /*T5DEF_LREALSUPPORTED*/

    lr1 = (T5_REAL)atan (GET_REALIN(1));
    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
        lr1 = T5_RADTODEG(lr1);
    SET_REALOUT((T5_REAL)lr1);
    return TRUE;
#else /*T5DEF_TRIGOSUPPORTED*/
    return FALSE;
#endif /*T5DEF_TRIGOSUPPORTED*/
}

static T5_BOOL _STFS_ATAN2 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);
#ifdef T5DEF_LREALSUPPORTED
    T5_LREAL lr1; /* for fast access */
#else /*T5DEF_LREALSUPPORTED*/
    T5_REAL lr1; /* for fast access */
#endif /*T5DEF_LREALSUPPORTED*/

    if (GET_REALIN(1) == T5_ZEROREAL && GET_REALIN(2) == T5_ZEROREAL)
        lr1 = T5_ZEROREAL;
    else
        lr1 = (T5_REAL)atan2 (GET_REALIN(1), GET_REALIN(2));
    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
        lr1 = T5_RADTODEG(lr1);
    SET_REALOUT((T5_REAL)lr1);
    return TRUE;
#else /*T5DEF_TRIGOSUPPORTED*/
    return FALSE;
#endif /*T5DEF_TRIGOSUPPORTED*/
}

#undef T5_DEGTORAD
#undef T5_RADTODEG

#define T5_DEGTORAD(f)  ((f) / 57.29577951308)
#define T5_RADTODEG(f)  ((f) * 57.29577951308)

#ifdef T5DEF_LREALSUPPORTED
#ifdef T5DEF_TRIGOSUPPORTED
#define T5DEF_TRIGOLREAL
#endif /*T5DEF_TRIGOSUPPORTED*/
#endif /*T5DEF_LREALSUPPORTED*/

static T5_BOOL _STFS_COSL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);

    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
    {
        SET_REAL64OUT(cos (T5_DEGTORAD (GET_REAL64IN(1))));
    }
    else
    {
        SET_REAL64OUT(cos (GET_REAL64IN(1)));
    }
    return TRUE;
#else /*T5DEF_TRIGOLREAL*/
    return FALSE;
#endif /*T5DEF_TRIGOLREAL*/
}

static T5_BOOL _STFS_SINL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);

    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
    {
        SET_REAL64OUT(sin (T5_DEGTORAD (GET_REAL64IN(1))));
    }
    else
    {
        SET_REAL64OUT(sin (GET_REAL64IN(1)));
    }
    return TRUE;
#else /*T5DEF_TRIGOLREAL*/
    return FALSE;
#endif /*T5DEF_TRIGOLREAL*/
}

static T5_BOOL _STFS_TANL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);
    T5_LREAL lr1;

    lr1 = GET_REAL64IN(1);
    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
        lr1 = T5_DEGTORAD(lr1);
    if (cos(lr1) > -T5_MINLREAL && cos(lr1) < T5_MINLREAL)
    {
        SET_REAL64OUT(T5_MAXLREAL);
    }
    else
    {
        SET_REAL64OUT(tan (lr1));
    }
    return TRUE;
#else /*T5DEF_TRIGOLREAL*/
    return FALSE;
#endif /*T5DEF_TRIGOLREAL*/
}

static T5_BOOL _STFS_ACOSL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);
    T5_LREAL lr1;

    lr1 = GET_REAL64IN(1);
    if (lr1 < -T5_ONELREAL || lr1 > T5_ONELREAL)
        lr1 = T5_ZEROLREAL;
    else lr1 = acos (lr1);
    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
        lr1 = T5_RADTODEG(lr1);
    SET_REAL64OUT(lr1);
    return TRUE;
#else /*T5DEF_TRIGOLREAL*/
    return FALSE;
#endif /*T5DEF_TRIGOLREAL*/
}

static T5_BOOL _STFS_ASINL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);
    T5_LREAL lr1;

    lr1 = GET_REAL64IN(1);
    if (lr1 < -T5_ONELREAL || lr1 > T5_ONELREAL)
        lr1 = T5_ZEROLREAL;
    else lr1 = asin (lr1);
    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
        lr1 = T5_RADTODEG(lr1);
    SET_REAL64OUT(lr1);
    return TRUE;
#else /*T5DEF_TRIGOLREAL*/
    return FALSE;
#endif /*T5DEF_TRIGOLREAL*/
}

static T5_BOOL _STFS_ATANL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);
    T5_LREAL lr1;

    lr1 = atan (GET_REAL64IN(1));
    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
        lr1 = T5_RADTODEG(lr1);
    SET_REAL64OUT(lr1);
    return TRUE;
#else /*T5DEF_TRIGOLREAL*/
    return FALSE;
#endif /*T5DEF_TRIGOLREAL*/
}

static T5_BOOL _STFS_ATAN2L (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_TRIGOLREAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);
    T5_LREAL lr1;

    if (GET_REAL64IN(1) == T5_ZEROLREAL && GET_REAL64IN(2) == T5_ZEROLREAL)
        lr1 = T5_ZEROLREAL;
    else
        lr1 = atan2 (GET_REAL64IN(1), GET_REAL64IN(2));
    if ((pStatus->wFlags & T5FLAG_DEGREES) != 0)
        lr1 = T5_RADTODEG(lr1);
    SET_REAL64OUT(lr1);
    return TRUE;
#else /*T5DEF_TRIGOLREAL*/
    return FALSE;
#endif /*T5DEF_TRIGOLREAL*/
}

#undef T5_DEGTORAD
#undef T5_RADTODEG

/* random *******************************************************************/

static T5_BOOL _STFS_RAND (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RANDOMSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    if (GET_DINTIN(1) == 0)
    {
        SET_DINTOUT(0);
    }
    else
    {
        SET_DINTOUT(rand () % GET_DINTIN(1));
    }
    return TRUE;
#else /*T5DEF_RANDOMSUPPORTED*/
    return FALSE;
#endif /*T5DEF_RANDOMSUPPORTED*/
}

/* arithmetics **************************************************************/

static T5_BOOL _STFS_MAX (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1, dw2;

    dw1 = GET_DINTIN(1);
    dw2 = GET_DINTIN(2);
    if (dw1 > dw2)
    {
        SET_DINTOUT(dw1);
    }
    else
    {
        SET_DINTOUT(dw2);
    }
    return TRUE;
}

static T5_BOOL _STFS_MIN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1, dw2;

    dw1 = GET_DINTIN(1);
    dw2 = GET_DINTIN(2);
    if (dw1 < dw2)
    {
        SET_DINTOUT(dw1);
    }
    else
    {
        SET_DINTOUT(dw2);
    }
    return TRUE;
}

static T5_BOOL _STFS_MOD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw2;

    dw2 = GET_DINTIN(2);
    if (dw2 <= 0)
    {
        SET_DINTOUT(-1);
    }
    else
    {
        SET_DINTOUT(GET_DINTIN(1) % dw2);
    }
    return TRUE;
}

static T5_BOOL _STFS_ODD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT((GET_DINTIN(1) & 1) != 0);
    return TRUE;
}

static T5_BOOL _STFS_LIMIT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1, dw2, dw3;

    dw1 = GET_DINTIN(1);
    dw2 = GET_DINTIN(2);
    dw3 = GET_DINTIN(3);
    if (dw2 < dw1)
    {
        SET_DINTOUT(dw1);
    }
    else if (dw2 > dw3)
    {
        SET_DINTOUT(dw3);
    }
    else
    {
        SET_DINTOUT(dw2);
    }
    return TRUE;
}

/* selectors ****************************************************************/

static T5_BOOL _STFS_SEL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    if (GET_D8IN(1))
    {
        SET_DINTOUT(GET_DINTIN(3));
    }
    else
    {
        SET_DINTOUT(GET_DINTIN(2));
    }
    return TRUE;
}

static T5_BOOL _STFS_MUX4 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    switch (GET_DINTIN(1))
    {
    case 0 : SET_DINTOUT(GET_DINTIN(2)); break;
    case 1 : SET_DINTOUT(GET_DINTIN(3)); break;
    case 2 : SET_DINTOUT(GET_DINTIN(4)); break;
    case 3 : SET_DINTOUT(GET_DINTIN(5)); break;
    default: SET_DINTOUT(0); break;
    }
    return TRUE;
}

static T5_BOOL _STFS_MUX8 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    switch (GET_DINTIN(1))
    {
    case 0 : SET_DINTOUT(GET_DINTIN(2)); break;
    case 1 : SET_DINTOUT(GET_DINTIN(3)); break;
    case 2 : SET_DINTOUT(GET_DINTIN(4)); break;
    case 3 : SET_DINTOUT(GET_DINTIN(5)); break;
    case 4 : SET_DINTOUT(GET_DINTIN(6)); break;
    case 5 : SET_DINTOUT(GET_DINTIN(7)); break;
    case 6 : SET_DINTOUT(GET_DINTIN(8)); break;
    case 7 : SET_DINTOUT(GET_DINTIN(9)); break;
    default: SET_DINTOUT(0); break;
    }
    return TRUE;
}

/* bitwise masks ************************************************************/

static T5_BOOL _STFS_NOTMASK (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT(~GET_DINTIN(1));
    return TRUE;
}

static T5_BOOL _STFS_ANDMASK (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT(GET_DINTIN(1) & GET_DINTIN(2));
    return TRUE;
}

static T5_BOOL _STFS_ORMASK (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT(GET_DINTIN(1) | GET_DINTIN(2));
    return TRUE;
}

static T5_BOOL _STFS_XORMASK (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT(GET_DINTIN(1) ^ GET_DINTIN(2));
    return TRUE;
}

static T5_BOOL _STFS_NOTWORD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);

    SET_D16OUT(~GET_D16IN(1));
    return TRUE;
#else /*T5DEF_DATA16SUPPORTED*/
    return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
}

static T5_BOOL _STFS_ANDWORD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);

    SET_D16OUT(GET_D16IN(1) & GET_D16IN(2));
    return TRUE;
#else /*T5DEF_DATA16SUPPORTED*/
    return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
}

static T5_BOOL _STFS_ORWORD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);

    SET_D16OUT(GET_D16IN(1) | GET_D16IN(2));
    return TRUE;
#else /*T5DEF_DATA16SUPPORTED*/
    return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
}

static T5_BOOL _STFS_XORWORD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);

    SET_D16OUT(GET_D16IN(1) ^ GET_D16IN(2));
    return TRUE;
#else /*T5DEF_DATA16SUPPORTED*/
    return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
}

static T5_BOOL _STFS_NOTBYTE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = (T5_PTBYTE)T5GET_DBDATA8(pDB);

    SET_D8OUT(~GET_D8IN(1));
    return TRUE;
}

static T5_BOOL _STFS_ANDBYTE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = (T5_PTBYTE)T5GET_DBDATA8(pDB);

    SET_D8OUT(GET_D8IN(1) & GET_D8IN(2));
    return TRUE;
}

static T5_BOOL _STFS_ORBYTE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = (T5_PTBYTE)T5GET_DBDATA8(pDB);

    SET_D8OUT(GET_D8IN(1) | GET_D8IN(2));
    return TRUE;
}

static T5_BOOL _STFS_XORBYTE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = (T5_PTBYTE)T5GET_DBDATA8(pDB);

    SET_D8OUT(GET_D8IN(1) ^ GET_D8IN(2));
    return TRUE;
}

static T5_BOOL _STFS_NOT64 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ULINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLWORD pD64 = (T5_PTLWORD)T5GET_DBDATA64(pDB);

    SET_D64OUT(~GET_D64IN(1));
    return TRUE;
#else /*T5DEF_ULINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_ULINTSUPPORTED*/
}

static T5_BOOL _STFS_AND64 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ULINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLWORD pD64 = (T5_PTLWORD)T5GET_DBDATA64(pDB);

    SET_D64OUT(GET_D64IN(1) & GET_D64IN(2));
    return TRUE;
#else /*T5DEF_ULINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_ULINTSUPPORTED*/
}

static T5_BOOL _STFS_OR64 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ULINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLWORD pD64 = (T5_PTLWORD)T5GET_DBDATA64(pDB);

    SET_D64OUT(GET_D64IN(1) | GET_D64IN(2));
    return TRUE;
#else /*T5DEF_ULINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_ULINTSUPPORTED*/
}

static T5_BOOL _STFS_XOR64 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ULINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLWORD pD64 = (T5_PTLWORD)T5GET_DBDATA64(pDB);

    SET_D64OUT(GET_D64IN(1) ^ GET_D64IN(2));
    return TRUE;
#else /*T5DEF_ULINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_ULINTSUPPORTED*/
}

/* swab *********************************************************************/

static T5_BOOL _STFS_SWAB (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    switch (pArgs[1]) /* type */
    {
    case T5C_BOOL :
    case T5C_SINT :
    case T5C_USINT :
        {
            T5_PTBYTE pData8 = (T5_PTBYTE)T5GET_DBDATA8(pDB);
            SET_D8OUT (GET_D8IN(2));
        }
        break;
    case T5C_INT :
    case T5C_UINT :
#ifdef T5DEF_DATA16SUPPORTED
        {
            T5_WORD w1, w2;
            T5_PTWORD pData16 = (T5_PTWORD)T5GET_DBDATA16(pDB);
            w1 = GET_D16IN(2);
            w2 = ((w1 << 8) & 0xff00) | ((w1 >> 8) & 0x00ff);
            SET_D16OUT (w2);
        }
        break;
#else /*T5DEF_DATA16SUPPORTED*/
        return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_UDINT :
        {
            T5_DWORD dw1, dw2;
            T5_PTDWORD pDint = (T5_PTDWORD)T5GET_DBDATA32(pDB);
            dw1 = GET_DINTIN(2);
            dw2 = ((dw1 << 24) & 0xff000000L)
                | ((dw1 << 8 ) & 0x00ff0000L)
                | ((dw1 >> 8)  & 0x0000ff00L)
                | ((dw1 >> 24) & 0x000000ffL);
            SET_DINTOUT (dw2);
        }
        break;
    case T5C_LINT : 
    case T5C_ULINT :
#ifdef T5DEF_DATA64SUPPORTED
        {
            T5_PTLWORD pD64 = (T5_PTLWORD)T5GET_DBDATA64(pDB);
            SET_D64OUT (GET_D64IN(2));
        }
        break;
#else /*T5DEF_DATA64SUPPORTED*/
        return FALSE;
#endif /*T5DEF_DATA64SUPPORTED*/
    case T5C_REAL :
#ifdef T5DEF_REALSUPPORTED
        {
            T5_PTREAL pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
            SET_REALOUT (GET_REALIN(2));
        }
        break;
#else /*T5DEF_REALSUPPORTED*/
        return FALSE;
#endif /*T5DEF_REALSUPPORTED*/
        break;
    case T5C_LREAL :
#ifdef T5DEF_LREALSUPPORTED
        {
            T5_PTLREAL pReal64 = (T5_PTLREAL)T5GET_DBDATA64(pDB);
            SET_REAL64OUT (GET_REAL64IN(2));
        }
        break;
#else /*T5DEF_REALSUPPORTED*/
        return FALSE;
#endif /*T5DEF_REALSUPPORTED*/
        break;
    case T5C_TIME :
        {
            T5_PTDWORD pTime = T5GET_DBTIME(pDB);
            pTime[pArgs[0]] = pTime[pArgs[2]];
        }
        break;
    case T5C_STRING :
#ifdef T5DEF_STRINGSUPPORTED
        T5VMStr_Copy (pDB, pArgs[0], pArgs[2]);
        break;
#else /*T5DEF_STRINGSUPPORTED*/
        return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
        break;
    default : break;
    }
    return TRUE;
}

/* shift registers **********************************************************/

static T5_BOOL _STFS_ROL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1, dw2;

    dw1 = GET_DINTIN(1); /* value */
    dw2 = GET_DINTIN(2) & 0x1fL; /* number of rotations */
    while (dw2--)
    {
        if (dw1 & 0x80000000)
            dw1 = ((dw1 << 1) & 0xfffffffe) | 0x00000001;
        else
            dw1 = ((dw1 << 1) & 0xfffffffe);
    }
    SET_DINTOUT(dw1);
    return TRUE;
}

static T5_BOOL _STFS_ROR (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1, dw2;

    dw1 = GET_DINTIN(1); /* value */
    dw2 = GET_DINTIN(2) & 0x1fL; /* number of rotations */
    while (dw2--)
    {
        if (dw1 & 0x00000001)
            dw1 = ((dw1 >> 1) & 0x7fffffff) | 0x80000000;
        else
            dw1 = ((dw1 >> 1) & 0x7fffffff);
    }
    SET_DINTOUT(dw1);
    return TRUE;
}

static T5_BOOL _STFS_SHL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1, dw2;

    dw1 = GET_DINTIN(1); /* value */
    dw2 = GET_DINTIN(2) & 0x1fL; /* number of rotations */
    while (dw2--)
        dw1 = ((dw1 << 1) & 0xfffffffe);
    SET_DINTOUT(dw1);
    return TRUE;
}

static T5_BOOL _STFS_SHR (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS (pDB);
    T5_LONG dw1, dw2;

    dw1 = GET_DINTIN(1); /* value */
    dw2 = GET_DINTIN(2) & 0x1fL; /* number of rotations */
    while (dw2--)
    {
        if ((pStatus->dwFlagsEx & T5FLAGEX_USHR) == 0 && dw1 & 0x80000000)
            dw1 = ((dw1 >> 1) & 0x7fffffff) | 0x80000000;
        else
            dw1 = ((dw1 >> 1) & 0x7fffffff);
    }
    SET_DINTOUT(dw1);
    return TRUE;
}

static T5_BOOL _STFS_ROLW (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);
    T5_WORD w1, w2;

    w1 = GET_D16IN(1); /* value */
    w2 = GET_D16IN(2) & 0x0f; /* number of rotations */
    while (w2--)
    {
        if (w1 & 0x8000)
            w1 = ((w1 << 1) & 0xfffe) | 0x0001;
        else
            w1 = ((w1 << 1) & 0xfffe);
    }
    SET_D16OUT(w1);
    return TRUE;
#else /*T5DEF_DATA16SUPPORTED*/
    return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
}

static T5_BOOL _STFS_RORW (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);
    T5_WORD w1, w2;

    w1 = GET_D16IN(1); /* value */
    w2 = GET_D16IN(2) & 0x0f; /* number of rotations */
    while (w2--)
    {
        if (w1 & 0x0001)
            w1 = ((w1 >> 1) & 0x7fff) | 0x8000;
        else
            w1 = ((w1 >> 1) & 0x7fff);
    }
    SET_D16OUT(w1);
    return TRUE;
#else /*T5DEF_DATA16SUPPORTED*/
    return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
}

static T5_BOOL _STFS_SHLW (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);
    T5_WORD w1, w2;

    w1 = GET_D16IN(1); /* value */
    w2 = GET_D16IN(2) & 0x0f; /* number of rotations */
    while (w2--)
        w1 = ((w1 << 1) & 0xfffe);
    SET_D16OUT(w1);
    return TRUE;
#else /*T5DEF_DATA16SUPPORTED*/
    return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
}

static T5_BOOL _STFS_SHRW (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS (pDB);
    T5_WORD w1, w2;

    w1 = GET_D16IN(1); /* value */
    w2 = GET_D16IN(2) & 0x0f; /* number of rotations */
    while (w2--)
    {
        if ((pStatus->dwFlagsEx & T5FLAGEX_USHR) == 0 && w1 & 0x8000)
            w1 = ((w1 >> 1) & 0x7fff) | 0x8000;
        else
            w1 = ((w1 >> 1) & 0x7fff);
    }
    SET_D16OUT(w1);
    return TRUE;
#else /*T5DEF_DATA16SUPPORTED*/
    return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
}

static T5_BOOL _STFS_ROLB (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_SINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_BYTE b1, b2;

    b1 = GET_D8IN(1); /* value */
    b2 = GET_D8IN(2) & 0x07; /* number of rotations */
    while (b2--)
    {
        if (b1 & 0x80)
            b1 = ((b1 << 1) & 0xfe) | 0x01;
        else
            b1 = ((b1 << 1) & 0xfe);
    }
    SET_D8OUT(b1);
    return TRUE;
#else /*T5DEF_SINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_SINTSUPPORTED*/
}

static T5_BOOL _STFS_RORB (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_SINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_BYTE b1, b2;

    b1 = GET_D8IN(1); /* value */
    b2 = GET_D8IN(2) & 0x07; /* number of rotations */
    while (b2--)
    {
        if (b1 & 0x01)
            b1 = ((b1 >> 1) & 0x7f) | 0x80;
        else
            b1 = ((b1 >> 1) & 0x7f);
    }
    SET_D8OUT(b1);
    return TRUE;
#else /*T5DEF_SINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_SINTSUPPORTED*/
}

static T5_BOOL _STFS_SHLB (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_SINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_BYTE b1, b2;

    b1 = GET_D8IN(1); /* value */
    b2 = GET_D8IN(2) & 0x07; /* number of rotations */
    while (b2--)
        b1 = ((b1 << 1) & 0xfe);
    SET_D8OUT(b1);
    return TRUE;
#else /*T5DEF_SINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_SINTSUPPORTED*/
}

static T5_BOOL _STFS_SHRB (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_SINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS (pDB);
    T5_BYTE b1, b2;

    b1 = GET_D8IN(1); /* value */
    b2 = GET_D8IN(2) & 0x07; /* number of rotations */
    while (b2--)
    {
        if ((pStatus->dwFlagsEx & T5FLAGEX_USHR) == 0 && b1 & 0x80)
            b1 = ((b1 >> 1) & 0x7f) | 0x80;
        else
            b1 = ((b1 >> 1) & 0x7f);
    }
    SET_D8OUT(b1);
    return TRUE;
#else /*T5DEF_SINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_SINTSUPPORTED*/
}

static T5_BOOL _STFS_ROL64 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ULINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLWORD pD64 = (T5_PTLWORD)T5GET_DBDATA64(pDB);
    T5_LWORD lw1, lw2;

    lw1 = GET_D64IN(1); /* value */
    lw2 = GET_D64IN(2) & 0x3f; /* number of rotations */
    while (lw2--)
    {
        if (lw1 & T5_ULL80)
            lw1 = ((lw1 << 1) & T5_ULLFE) | 0x01;
        else
            lw1 = ((lw1 << 1) & T5_ULLFE);
    }
    SET_D64OUT(lw1);
    return TRUE;
#else /*T5DEF_ULINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_ULINTSUPPORTED*/
}

static T5_BOOL _STFS_ROR64 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ULINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLWORD pD64 = (T5_PTLWORD)T5GET_DBDATA64(pDB);
    T5_LWORD lw1, lw2;

    lw1 = GET_D64IN(1); /* value */
    lw2 = GET_D64IN(2) & 0x3f; /* number of rotations */
    while (lw2--)
    {
        if (lw1 & 0x01)
            lw1 = ((lw1 >> 1) & T5_ULL7F) | T5_ULL80;
        else
            lw1 = ((lw1 >> 1) & T5_ULL7F);
    }
    SET_D64OUT(lw1);
    return TRUE;
#else /*T5DEF_ULINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_ULINTSUPPORTED*/
}

static T5_BOOL _STFS_SHL64 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ULINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLWORD pD64 = (T5_PTLWORD)T5GET_DBDATA64(pDB);
    T5_LWORD lw1, lw2;

    lw1 = GET_D64IN(1); /* value */
    lw2 = GET_D64IN(2) & 0x3f; /* number of rotations */
    while (lw2--)
        lw1 = ((lw1 << 1) & T5_ULLFE);
    SET_D64OUT(lw1);
    return TRUE;
#else /*T5DEF_ULINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_ULINTSUPPORTED*/
}

static T5_BOOL _STFS_SHR64 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ULINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLWORD pD64 = (T5_PTLWORD)T5GET_DBDATA64(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS (pDB);
    T5_LWORD lw1, lw2;

    lw1 = GET_D64IN(1); /* value */
    lw2 = GET_D64IN(2) & 0x3f; /* number of rotations */
    while (lw2--)
    {
        if ((pStatus->dwFlagsEx & T5FLAGEX_USHR) == 0 && lw1 & T5_ULL80)
            lw1 = ((lw1 >> 1) & T5_ULL7F) | T5_ULL80;
        else
            lw1 = ((lw1 >> 1) & T5_ULL7F);
    }
    SET_D64OUT(lw1);
    return TRUE;
#else /*T5DEF_ULINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_ULINTSUPPORTED*/
}

/* strings functions ********************************************************/

static T5_BOOL _STFS_ASCII (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT(T5VMStr_Ascii (pDB, pArgs[1], GET_DINTIN(2) - 1));
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_CHAR (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    IFNOT_ISSTRINGLOCKED(*pArgs)
    {
        T5VMStr_Char (pDB, *pArgs, GET_DINTIN(1));
    }
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_MLEN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT(T5VMStr_GetLength (pDB, pArgs[1]));
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_F_LEN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);

    SET_D16OUT((T5_SHORT)T5VMStr_GetLength (pDB, pArgs[1]));
    return TRUE;
#else /*T5DEF_DATA16SUPPORTED*/
    return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_LEFT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    IFNOT_ISSTRINGLOCKED(*pArgs)
    {
        T5VMStr_Left (pDB, *pArgs, pArgs[1], GET_DINTIN(2));
    }
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_RIGHT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    IFNOT_ISSTRINGLOCKED(*pArgs)
    {
        T5VMStr_Right (pDB, *pArgs, pArgs[1], GET_DINTIN(2));
    }
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_MID (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    IFNOT_ISSTRINGLOCKED(*pArgs)
    {
        T5VMStr_Mid (pDB, *pArgs, pArgs[1], GET_DINTIN(3) - 1, GET_DINTIN(2));
    }
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_DELETE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    IFNOT_ISSTRINGLOCKED(*pArgs)
    {
        T5VMStr_Delete (pDB, *pArgs, pArgs[1], GET_DINTIN(3) - 1, GET_DINTIN(2));
    }
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_INSERT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    IFNOT_ISSTRINGLOCKED(*pArgs)
    {
        T5VMStr_Insert (pDB, *pArgs, pArgs[1], pArgs[2], GET_DINTIN(3) - 1);
    }
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_FIND (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT(T5VMStr_Find (pDB, pArgs[1], pArgs[2]));
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_REPLACE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    IFNOT_ISSTRINGLOCKED(*pArgs)
    {
        T5VMStr_Delete (pDB, *pArgs, pArgs[1],
                        GET_DINTIN(4) - 1, GET_DINTIN(3));
        T5VMStr_Insert (pDB, *pArgs, *pArgs, pArgs[2],
                                        GET_DINTIN(4) - 1);
    }
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_ATOH (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT(T5VMStr_AtoH (pDB, pArgs[1]));
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_HTOA (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    IFNOT_ISSTRINGLOCKED(*pArgs)
    {
        T5VMStr_HtoA (pDB, *pArgs, GET_DINTIN(1));
    }
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_PRINTF (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    IFNOT_ISD8LOCKED(*pArgs)
    {
#ifdef T5DEF_PRINTFSUPPORTED
        T5_PRINTF (T5VMStr_GetText (pDB, pArgs[1]),
                GET_DINTIN(2), GET_DINTIN(3), GET_DINTIN(4), GET_DINTIN(5));
#endif /*T5DEF_PRINTFSUPPORTED*/
        SET_D8OUT(0);
    }
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_CRC16 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);

    IFNOT_ISD16LOCKED(*pArgs)
    {
        SET_D16OUT(T5VMStr_Crc16 (pDB, pArgs[1]));
    }
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_STRINGTOARRAY (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
#ifdef T5DEF_SINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (T5VMStr_ToD8 (pDB, pArgs[1], pArgs[2], pArgs[3]));
    return TRUE;
#else /*T5DEF_SINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_SINTSUPPORTED*/
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_ARRAYTOSTRING (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
#ifdef T5DEF_SINTSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (T5VMStr_FromD8 (pDB, pArgs[1], pArgs[2], pArgs[3], GET_DINTIN(4)));
    return TRUE;
#else /*T5DEF_SINTSUPPORTED*/
    return FALSE;
#endif /*T5DEF_SINTSUPPORTED*/
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_SRINGTOARRAY16 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (T5VMStr_ToD16 (pDB, pArgs[1], pArgs[2], pArgs[3]));
    return TRUE;
#else /*T5DEF_DATA16SUPPORTED*/
    return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_ARRAYTOSTRING16 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (T5VMStr_FromD16 (pDB, pArgs[1], pArgs[2], pArgs[3], GET_DINTIN(4)));
    return TRUE;
#else /*T5DEF_DATA16SUPPORTED*/
    return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_SRINGTOARRAY32 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (T5VMStr_ToD32 (pDB, pArgs[1], pArgs[2], pArgs[3]));
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_ARRAYTOSTRING32 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (T5VMStr_FromD32 (pDB, pArgs[1], pArgs[2], pArgs[3], GET_DINTIN(4)));
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

static T5_BOOL _STFS_NUMTOSTRING (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    IFNOT_ISSTRINGLOCKED(*pArgs)
    {
        T5VMStr_NumToStr (pDB, *pArgs, pArgs[1], pArgs[2], GET_DINTIN(3), GET_DINTIN(4));
    }
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

/* file access **************************************************************/

static T5_BOOL _T5Stf_SkipFileFunc (T5PTR_DB pDB)
{
#ifdef T5DEF_NOIECFILEFUNC
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS (pDB);
    if ((pStatus->dwFlagsEx & T5FLAGEX_NOFILEFUNC) == 0)
    {
        T5_HOOKNOIECFILEFUNC (pDB);

        T5_PRINTF ("File functions are not supported");
        pStatus->dwFlagsEx |= T5FLAGEX_NOFILEFUNC;
    }
    return TRUE;
#endif /*T5DEF_NOIECFILEFUNC*/
    return FALSE;
}

#ifdef T5DEF_FILESUPPORTED
#ifdef T5DEF_STRINGSUPPORTED
#define T5DEF_FILESTRING
#ifdef T5DEF_FILEV2
#define T5DEF_FILESTRINGV2
#endif /*T5DEF_FILEV2*/
#endif /*T5DEF_STRINGSUPPORTED*/
#endif /*T5DEF_FILESUPPORTED*/

static T5_BOOL _STFS_F_ROPEN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1 = 0;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        pRsc = T5VMRsc_GetFreeItem (pDB);
        if (pRsc != T5_PTNULL)
            dw1 = T5File_Open (T5VMStr_GetText (pDB, pArgs[1]), FALSE);
        if (dw1)
        {
            pRsc->dwObject = (T5_DWORD)dw1;
            pRsc->wType = T5SYSRSC_FILE;
            pRsc->wUsed = 1;
        }
    }
    SET_DINTOUT(dw1);
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_F_WOPEN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1 = 0L;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        pRsc = T5VMRsc_GetFreeItem (pDB);
        if (pRsc != T5_PTNULL)
            dw1 = T5File_Open (T5VMStr_GetText (pDB, pArgs[1]), TRUE);
        if (dw1)
        {
            pRsc->dwObject = (T5_DWORD)dw1;
            pRsc->wType = T5SYSRSC_FILE;
            pRsc->wUsed = 1;
        }
    }
    SET_DINTOUT(dw1);
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_F_AOPEN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1 = 0L;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        pRsc = T5VMRsc_GetFreeItem (pDB);
        if (pRsc != T5_PTNULL)
            dw1 = T5File_OpenAppend (T5VMStr_GetText (pDB, pArgs[1]));
        if (dw1)
        {
            pRsc->dwObject = (T5_DWORD)dw1;
            pRsc->wType = T5SYSRSC_FILE;
            pRsc->wUsed = 1;
        }
    }
    SET_DINTOUT(dw1);
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_F_CLOSE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc = T5_PTNULL;
    T5_LONG dw1 = 0;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        dw1 = GET_DINTIN(1);
        if (dw1 != 0)
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        if (pRsc != T5_PTNULL)
        {
            T5File_Close (dw1);
            pRsc->dwObject = 0L;
            pRsc->wType = 0;
            pRsc->wUsed = 0;
        }
    }
    SET_D8OUT(pRsc != T5_PTNULL);
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_F_EOF (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_BOOL bOK = FALSE;;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        dw1 = GET_DINTIN(1);
        if (dw1 == 0)
            pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        bOK = (pRsc != T5_PTNULL && T5File_Eof (dw1));
    }
    SET_D8OUT(bOK);
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_F_ISREADY (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_BOOL bOK = FALSE;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        dw1 = GET_DINTIN(1);
        if (dw1 == 0) pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
#ifdef T5DEF_F_ISREADY
        bOK = (pRsc != T5_PTNULL && T5File_IsReady (dw1));
#else /*T5DEF_F_ISREADY*/
        bOK = (pRsc != T5_PTNULL);
#endif /*T5DEF_F_ISREADY*/
    }
    SET_D8OUT(bOK);
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_FA_READ (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1, dw2 = 0;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        dw1 = GET_DINTIN(1);
        if (dw1 == 0) pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        if (pRsc == T5_PTNULL) dw2 = 0L;
        else if (!T5File_Read (dw1, &dw2, sizeof (T5_LONG))) dw2 = 0L;
    }
    SET_DINTOUT(dw2);
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_FA_WRITE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1, dw2;
    T5_BOOL bOK = FALSE;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        dw1 = GET_DINTIN(1);
        dw2 = GET_DINTIN(2);
        if (dw1 == 0) pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        bOK = (pRsc != T5_PTNULL
                && T5File_Write (dw1, &dw2, sizeof (T5_LONG)));
    }
    SET_D8OUT(bOK);
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_FB_READ (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_BOOL bOK = FALSE, bLockFB;
    T5_PTR pDataFB;
    T5_WORD wSizeofFB;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        dw1 = GET_DINTIN(2);
        if (dw1 == 0 || pArgs[1] == T5C_STRING)
            pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        if (pRsc == T5_PTNULL)
            bOK = FALSE;
        else
        {
            pDataFB = T5Tools_GetAnyParam (pDB, pArgs[1], pArgs[3], &bLockFB, &wSizeofFB);
            if (bLockFB)
                bOK = TRUE; /*skipped*/
            else
                bOK = T5File_Read (dw1, pDataFB, (T5_LONG)wSizeofFB);
        }
    }
    SET_D8OUT(bOK);
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_FB_WRITE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_BOOL bOK = FALSE;
    T5_PTR pDataFB;
    T5_WORD wSizeofFB;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        dw1 = GET_DINTIN(2);
        if (dw1 == 0 /*|| pArgs[1] == T5C_STRING*/)
            pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        if (pRsc == T5_PTNULL)
            bOK = FALSE;
        else
        {
            pDataFB = T5Tools_GetAnyParam (pDB, pArgs[1], pArgs[3], NULL, &wSizeofFB);
            if (pArgs[1] != T5C_STRING)
                bOK = T5File_Write (dw1, pDataFB, (T5_LONG)wSizeofFB);
            else
            {
#ifdef T5DEF_STRINGSUPPORTED
                T5_PTBYTE pStr = (T5_PTBYTE)pDataFB;
                bOK = T5File_Write (dw1, pStr+2, (T5_LONG)(pStr[1]));
#else /*T5DEF_STRINGSUPPORTED*/
                bOK = FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
            }
        }
    }
    SET_D8OUT(bOK);
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_FR_READ (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRINGV2
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_BOOL bLockFB;
    T5_PTBYTE pDataFB;
    T5_WORD wSizeofFB, wSize, wType, wNb, wOff, wQ;

    wQ = 0;
    if (!_T5Stf_SkipFileFunc (pDB))
    {
        wType = pArgs[1]; /*data type*/
        dw1 = GET_DINTIN(2); /*file id*/
        if (dw1 == 0 || wType == T5C_STRING)
            pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        if (pRsc != T5_PTNULL)
        {
            wSize = pArgs[4];
            pDataFB = (T5_PTBYTE)T5Tools_GetAnyArray (pDB, wType, pArgs[3]);
            T5Tools_GetAnyParam (pDB, wType, pArgs[3], &bLockFB, &wSizeofFB);
            wOff = (T5_WORD)GET_DINTIN (5);
            wNb = (T5_WORD)GET_DINTIN (6);
            if ((wOff + wNb) <= wSize && pDataFB != NULL)
            {
                pDataFB += (wSizeofFB * wOff);
                wQ = (T5_WORD)T5File_Fread (dw1, (T5_DWORD)wSizeofFB, (T5_DWORD)wNb, (T5_PTR)pDataFB);
            }
        }
    }
    SET_DINTOUT((T5_DWORD)wQ);
    return TRUE;
#else /*T5DEF_FILESTRINGV2*/
    return FALSE;
#endif /*T5DEF_FILESTRINGV2*/
}

static T5_BOOL _STFS_FR_WRITE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRINGV2
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_BOOL bLockFB;
    T5_PTBYTE pDataFB;
    T5_WORD wSizeofFB, wSize, wType, wNb, wOff, wQ;

    wQ = 0;
    if (!_T5Stf_SkipFileFunc (pDB))
    {
        wType = pArgs[1]; /*data type*/
        dw1 = GET_DINTIN(2); /*file id*/
        if (dw1 == 0 || wType == T5C_STRING)
            pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        if (pRsc != T5_PTNULL)
        {
            wSize = pArgs[4];
            pDataFB = (T5_PTBYTE)T5Tools_GetAnyArray (pDB, wType, pArgs[3]);
            T5Tools_GetAnyParam (pDB, wType, pArgs[3], &bLockFB, &wSizeofFB);
            wOff = (T5_WORD)GET_DINTIN (5);
            wNb = (T5_WORD)GET_DINTIN (6);
            if ((wOff + wNb) <= wSize && pDataFB != NULL)
            {
                pDataFB += (wSizeofFB * wOff);
                wQ = (T5_WORD)T5File_Fwrite (dw1, (T5_DWORD)wSizeofFB, (T5_DWORD)wNb, (T5_PTR)pDataFB);
            }
        }
    }
    SET_DINTOUT((T5_DWORD)wQ);
    return TRUE;
#else /*T5DEF_FILESTRINGV2*/
    return FALSE;
#endif /*T5DEF_FILESTRINGV2*/
}

static T5_BOOL _STFS_FM_WRITE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_BOOL bOK = FALSE;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        dw1 = GET_DINTIN(1);
        if (dw1 == 0) pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        bOK = (pRsc != T5_PTNULL
                && T5File_WriteLine (dw1, T5VMStr_GetText (pDB, pArgs[2])));
    }
    SET_D8OUT(bOK);
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_FM_READ (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_BYTE s[257];

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        dw1 = GET_DINTIN(1);
        if (dw1 == 0) pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        if (pRsc != T5_PTNULL)
        {
            *s = (T5_BYTE)T5VMStr_GetMaxLength (pDB, *pArgs);
            *s = T5File_ReadLine (dw1, (T5_PTCHAR)(s+1), *s);
            IFNOT_ISSTRINGLOCKED(*pArgs)
                T5VMStr_Force (pDB, *pArgs, s);
        }
    }
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_F_FLUSH (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRINGV2
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_BOOL bOK = FALSE;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        dw1 = GET_DINTIN(1); /*file id*/
        if (dw1 == 0)
            pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        if (pRsc != T5_PTNULL)
            bOK = T5File_Fflush (dw1);
    }
    SET_D8OUT(bOK);
    return TRUE;
#else /*T5DEF_FILESTRINGV2*/
    return FALSE;
#endif /*T5DEF_FILESTRINGV2*/
}

static T5_BOOL _STFS_F_SEEK (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRINGV2
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_BOOL bOK = FALSE;

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        dw1 = GET_DINTIN(1); /*file id*/
        if (dw1 == 0)
            pRsc = T5_PTNULL;
        else
        {
            pRsc = T5VMRsc_FindData (pDB, (T5_DWORD)dw1);
            if (pRsc != T5_PTNULL && pRsc->wType != T5SYSRSC_FILE)
                pRsc = T5_PTNULL;
        }
        if (pRsc != T5_PTNULL)
            bOK = T5File_Fseek (dw1, GET_DINTIN(3), GET_DINTIN(2));
    }
    SET_D8OUT(bOK);
    return TRUE;
#else /*T5DEF_FILESTRINGV2*/
    return FALSE;
#endif /*T5DEF_FILESTRINGV2*/
}

/* file management **********************************************************/

#ifdef T5DEF_FILESTRING
#ifdef T5DEF_FILEMGT
#define T5DEF_FILESTRINGMGT
#endif /*T5DEF_FILEMGT*/
#endif /*T5DEF_FILESTRING*/

static T5_BOOL _STFS_F_EXIST (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRINGMGT
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        SET_D8OUT (T5File_Exist (T5VMStr_GetText (pDB, pArgs[1])));
    }
    else
    {
        SET_D8OUT (FALSE);
    }
    return TRUE;
#else /*T5DEF_FILESTRINGMGT*/
    return FALSE;
#endif /*T5DEF_FILESTRINGMGT*/
}

static T5_BOOL _STFS_F_GETSIZE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRINGMGT
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        SET_DINTOUT (T5File_GetSize (T5VMStr_GetText (pDB, pArgs[1])));
    }
    else
    {
        SET_DINTOUT (0);
    }
    return TRUE;
#else /*T5DEF_FILESTRINGMGT*/
    return FALSE;
#endif /*T5DEF_FILESTRINGMGT*/
}

static T5_BOOL _STFS_F_COPY (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRINGMGT
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        SET_D8OUT (T5File_Copy (T5VMStr_GetText (pDB, pArgs[1]),
                            T5VMStr_GetText (pDB, pArgs[2])));
    }
    else
    {
        SET_D8OUT (FALSE);
    }
    return TRUE;
#else /*T5DEF_FILESTRINGMGT*/
    return FALSE;
#endif /*T5DEF_FILESTRINGMGT*/
}

static T5_BOOL _STFS_F_DELETE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRINGMGT
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        SET_D8OUT (T5File_Delete (T5VMStr_GetText (pDB, pArgs[1])));
    }
    else
    {
        SET_D8OUT (FALSE);
    }
    return TRUE;
#else /*T5DEF_FILESTRINGMGT*/
    return FALSE;
#endif /*T5DEF_FILESTRINGMGT*/
}

static T5_BOOL _STFS_F_RENAME (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRINGMGT
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        SET_D8OUT (T5File_Rename (T5VMStr_GetText (pDB, pArgs[1]),
                                  T5VMStr_GetText (pDB, pArgs[2])));
    }
    else
    {
        SET_D8OUT (FALSE);
    }
    return TRUE;
#else /*T5DEF_FILESTRINGMGT*/
    return FALSE;
#endif /*T5DEF_FILESTRINGMGT*/
}

/* files and RETAIN variables ***********************************************/

static T5_BOOL _STFS_F_SAVERETAIN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        SET_D8OUT (T5VMWarm_SaveToFile (pDB, T5VMStr_GetText (pDB, pArgs[1])));
    }
    else
    {
        SET_D8OUT (FALSE);
    }
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

static T5_BOOL _STFS_F_LOADRETAIN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_FILESTRING
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

    if (!_T5Stf_SkipFileFunc (pDB))
    {
        SET_D8OUT (T5VMWarm_LoadFromFile (pDB, T5VMStr_GetText (pDB, pArgs[1])));
    }
    else
    {
        SET_D8OUT (FALSE);
    }
    return TRUE;
#else /*T5DEF_FILESTRING*/
    return FALSE;
#endif /*T5DEF_FILESTRING*/
}

/* TCP functions ************************************************************/

static T5_BOOL _STFS_TCPLISTEN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_LIBSOCK
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_SOCKET sock;

    pRsc = T5VMRsc_GetFreeItem (pDB);
    if (pRsc == T5_PTNULL)
        dw1 = (T5_LONG)T5_INVSOCKET;
    else if (T5Socket_CreateListeningSocket ((T5_WORD)GET_DINTIN(1),
                                                (T5_WORD)GET_DINTIN(2),
                                                &sock, NULL) != T5RET_OK)
        dw1 = (T5_LONG)T5_INVSOCKET;
    else
    {
        dw1 = (T5_LONG)sock;
        pRsc->dwObject = dw1;
        pRsc->wType = T5SYSRSC_SOCKET;
        pRsc->wUsed = 1;
    }
    SET_DINTOUT(dw1);
    return TRUE;
#else /*T5DEF_LIBSOCK*/
    return FALSE;
#endif /*T5DEF_LIBSOCK*/
}

static T5_BOOL _STFS_TCPACCEPT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_LIBSOCK
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1, dw2;

    dw1 = GET_DINTIN(1);
    pRsc = T5VMRsc_GetFreeItem (pDB);
    if (pRsc == T5_PTNULL || !T5VMRsc_FindSocket (pDB, dw1))
        dw2 = (T5_LONG)T5_INVSOCKET;
    else
    {
        dw2 = (T5_LONG)T5Socket_Accept ((T5_SOCKET)dw1, NULL);
        if (dw2 != (T5_LONG)T5_INVSOCKET)
        {
            pRsc->dwObject = dw2;
            pRsc->wType = T5SYSRSC_SOCKET;
            pRsc->wUsed = 1;
        }
    }
    SET_DINTOUT(dw2);
    return TRUE;
#else /*T5DEF_LIBSOCK*/
    return FALSE;
#endif /*T5DEF_LIBSOCK*/
}

static T5_BOOL _STFS_TCPCONNECT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_LIBSOCK
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_SOCKET sock;
    T5_BOOL bSock;

    pRsc = T5VMRsc_GetFreeItem (pDB);
    if (pRsc == T5_PTNULL)
        dw1 = (T5_LONG)T5_INVSOCKET;
    else if (T5Socket_CreateConnectedSocket (T5VMStr_GetText (pDB, pArgs[1]),
                                        (T5_WORD)GET_DINTIN(2), &sock,
                                        &bSock, NULL) != T5RET_OK)
        dw1 = (T5_LONG)T5_INVSOCKET;
    else if (sock == T5_INVSOCKET)
        dw1 = (T5_LONG)T5_INVSOCKET;
    else
    {
        dw1 = (T5_LONG)sock;
        pRsc->dwObject = dw1;
        pRsc->wType = T5SYSRSC_SOCKET;
        pRsc->wUsed = 1;
    }
    SET_DINTOUT(dw1);
    return TRUE;
#else /*T5DEF_LIBSOCK*/
    return FALSE;
#endif /*T5DEF_LIBSOCK*/
}

static T5_BOOL _STFS_TCPCLOSE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_LIBSOCK
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;

    dw1 = GET_DINTIN(1);
    pRsc = T5VMRsc_FindSocket (pDB, dw1);
    if (pRsc == T5_PTNULL)
    {
        SET_D8OUT(FALSE);
    }
    else
    {
        if (dw1 != (T5_LONG)T5_INVSOCKET)
            T5Socket_CloseSocket ((T5_SOCKET)dw1);
        pRsc->dwObject = 0L;
        pRsc->wType = 0;
        pRsc->wUsed = 0;
        SET_D8OUT(TRUE);
    }
    return TRUE;
#else /*T5DEF_LIBSOCK*/
    return FALSE;
#endif /*T5DEF_LIBSOCK*/
}

static T5_BOOL _STFS_TCPSEND (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_LIBSOCK
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1, dw2;
    T5_BOOL bSock;
    T5_BYTE bLen;

    dw1 = GET_DINTIN(1);
    if ((pRsc = T5VMRsc_FindSocket (pDB, dw1)) == NULL || dw1 == (T5_LONG)T5_INVSOCKET)
        dw2 = 0;
    else
    {
        bLen = (T5_BYTE)T5VMStr_GetMaxLength (pDB, pArgs[3]);
        if ((T5_DWORD)bLen > (T5_DWORD)GET_DINTIN(2))
            bLen = (T5_BYTE)GET_DINTIN(2);
        dw2 = (T5_LONG)T5Socket_Send ((T5_SOCKET)dw1, (T5_WORD)bLen,
                                        T5VMStr_GetText (pDB, pArgs[3]),
                                        &bSock);
        if (bSock) /* fail */
        {
            T5Socket_CloseSocket ((T5_SOCKET)dw1);
            pRsc->dwObject = 0L;
            pRsc->wType = 0;
            pRsc->wUsed = 0;
            dw2 = 0;
        }
    }
    SET_DINTOUT(dw2);
    return TRUE;
#else /*T5DEF_LIBSOCK*/
    return FALSE;
#endif /*T5DEF_LIBSOCK*/
}

static T5_BOOL _STFS_TCPRECEIVE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_LIBSOCK
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1, dw2;
    T5_BOOL bSock;
    T5_BYTE s[257];

    dw1 = GET_DINTIN(1);
    if ((pRsc = T5VMRsc_FindSocket (pDB, dw1)) == NULL
        || dw1 == (T5_LONG)T5_INVSOCKET)
        dw2 = 0;
    else
    {
        *s = (T5_BYTE)T5VMStr_GetMaxLength (pDB, pArgs[3]);
        if ((T5_DWORD)(*s) > (T5_DWORD)GET_DINTIN(2))
            *s = (T5_BYTE)GET_DINTIN(2);

        dw2 = (T5_LONG)T5Socket_Receive ((T5_SOCKET)dw1, (T5_WORD)(*s),
                                            s+1, &bSock);
        if (!bSock)
        {
            *s = (T5_BYTE)dw2;
            IFNOT_ISSTRINGLOCKED(pArgs[3])
                T5VMStr_Force (pDB, pArgs[3], s);
        }
        else /* fail */
        {
            T5Socket_CloseSocket ((T5_SOCKET)dw1);
            pRsc->dwObject = 0L;
            pRsc->wType = 0;
            pRsc->wUsed = 0;
            dw2 = 0;
        }
    }
    SET_DINTOUT(dw2);
    return TRUE;
#else /*T5DEF_LIBSOCK*/
    return FALSE;
#endif /*T5DEF_LIBSOCK*/
}

static T5_BOOL _STFS_TCPSENDARRAY (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_LIBSOCK
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1, dw2;
    T5_BOOL bSock;
    T5_WORD wLen;

    dw1 = GET_DINTIN(1);
    if ((pRsc = T5VMRsc_FindSocket (pDB, dw1)) == NULL
        || dw1 == (T5_LONG)T5_INVSOCKET)
        dw2 = 0;
    else
    {
        wLen = pArgs[4];
        if ((T5_DWORD)wLen > (T5_DWORD)GET_DINTIN(2))
            wLen = (T5_WORD)GET_DINTIN(2);

        dw2 = (T5_LONG)T5Socket_Send ((T5_SOCKET)dw1, wLen,
                                        T5Tools_GetD8Array (pDB, pArgs[3]),
                                        &bSock);
        if (bSock) /* fail */
        {
            T5Socket_CloseSocket ((T5_SOCKET)dw1);
            pRsc->dwObject = 0L;
            pRsc->wType = 0;
            pRsc->wUsed = 0;
            dw2 = 0;
        }
    }
    SET_DINTOUT(dw2);
    return TRUE;
#else /*T5DEF_LIBSOCK*/
    return FALSE;
#endif /*T5DEF_LIBSOCK*/
}

static T5_BOOL _STFS_TCPRCVARRAY (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_LIBSOCK
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1, dw2;
    T5_BOOL bSock;
    T5_WORD wLen;

    dw1 = GET_DINTIN(1);
    if ((pRsc = T5VMRsc_FindSocket (pDB, dw1)) == NULL
        || dw1 == (T5_LONG)T5_INVSOCKET)
        dw2 = 0;
    else
    {
        wLen = pArgs[4];
        if ((T5_DWORD)wLen > (T5_DWORD)GET_DINTIN(2))
            wLen = (T5_WORD)GET_DINTIN(2);

        dw2 = (T5_LONG)T5Socket_Receive ((T5_SOCKET)dw1, wLen,
                                            T5Tools_GetD8Array (pDB, pArgs[3]),
                                            &bSock);
        if (bSock) /* fail */
        {
            T5Socket_CloseSocket ((T5_SOCKET)dw1);
            pRsc->dwObject = 0L;
            pRsc->wType = 0;
            pRsc->wUsed = 0;
            dw2 = 0;
        }
    }
    SET_DINTOUT(dw2);
    return TRUE;
#else /*T5DEF_LIBSOCK*/
    return FALSE;
#endif /*T5DEF_LIBSOCK*/
}

static T5_BOOL _STFS_TCPISCONNECTED (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_LIBSOCK
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_DBSYSRSC pRsc;
    T5_LONG dw1;
    T5_BOOL bSock;

    dw1 = GET_DINTIN(1);
    if ((pRsc = T5VMRsc_FindSocket (pDB, dw1)) == NULL
        || dw1 == (T5_LONG)T5_INVSOCKET)
        bSock = FALSE;
    else if (T5Socket_CheckPendingConnect ((T5_SOCKET)dw1, &bSock) == T5RET_OK)
        bSock = TRUE;
    else
    {
        if (bSock) /* fail */
        {
            T5Socket_CloseSocket ((T5_SOCKET)dw1);
            pRsc->dwObject = 0L;
            pRsc->wType = 0;
            pRsc->wUsed = 0;
        }
        bSock = FALSE;
    }
    SET_D8OUT(bSock);
    return TRUE;
#else /*T5DEF_LIBSOCK*/
    return FALSE;
#endif /*T5DEF_LIBSOCK*/
}

static T5_BOOL _STFS_TCPISVALID (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_LIBSOCK
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;
    T5_BOOL bSock;

    dw1 = GET_DINTIN(1);
    bSock = (T5VMRsc_FindSocket (pDB, dw1)
                && dw1 != (T5_LONG)T5_INVSOCKET);
    SET_D8OUT(bSock);
    return TRUE;
#else /*T5DEF_LIBSOCK*/
    return FALSE;
#endif /*T5DEF_LIBSOCK*/
}

/* UDP functions ************************************************************/

static T5_BOOL _STFS_UDPCREATE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_UDP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT(_Udp_Create (pDB, GET_DINTIN(1)));
    return TRUE;
#else /*T5DEF_UDP*/
    return FALSE;
#endif /*T5DEF_UDP*/
}

static T5_BOOL _STFS_UDPADDRMAKE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_UDP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT(_Udp_AddrMake (pDB, pArgs[1], GET_DINTIN(2), pArgs[3], pArgs[4]));
    return TRUE;
#else /*T5DEF_UDP*/
    return FALSE;
#endif /*T5DEF_UDP*/
}

static T5_BOOL _STFS_UDPSENDTO (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_UDP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT(_Udp_SendTo (pDB, GET_DINTIN(1), GET_DINTIN(2), pArgs[3], pArgs[4], pArgs[5]));
    return TRUE;
#else /*T5DEF_UDP*/
    return FALSE;
#endif /*T5DEF_UDP*/
}

static T5_BOOL _STFS_UDPSENDTOARRAY (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_UDP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT(_Udp_SendToArr (pDB, GET_DINTIN(1), GET_DINTIN(2), pArgs[3], pArgs[4], pArgs[5], pArgs[6]));
    return TRUE;
#else /*T5DEF_UDP*/
    return FALSE;
#endif /*T5DEF_UDP*/
}

static T5_BOOL _STFS_UDPRCVFROM (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_UDP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT(_Udp_RcvFrom (pDB, GET_DINTIN(1), GET_DINTIN(2), pArgs[3], pArgs[4], pArgs[5]));
    return TRUE;
#else /*T5DEF_UDP*/
    return FALSE;
#endif /*T5DEF_UDP*/
}

static T5_BOOL _STFS_UDPRCVFROMARRAY (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_UDP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT(_Udp_RcvFromArr (pDB, GET_DINTIN(1), GET_DINTIN(2), pArgs[3], pArgs[4], pArgs[5], pArgs[6]));
    return TRUE;
#else /*T5DEF_UDP*/
    return FALSE;
#endif /*T5DEF_UDP*/
}

/* dynamic memory allocation ************************************************/

static T5_BOOL _STFS_ARCREATE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MALLOCSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;

    dw1 = T5VMRsc_CreateArray (pDB, GET_DINTIN(1) + 1L, GET_DINTIN(2), sizeof (T5_LONG));
    SET_DINTOUT(dw1);
    return TRUE;
#else /*T5DEF_MALLOCSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MALLOCSUPPORTED*/
}

static T5_BOOL _STFS_ARREAD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MALLOCSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;

    if (!T5VMRsc_ReadArray (pDB, GET_DINTIN(1) + 1L, GET_DINTIN(2), sizeof (T5_LONG), &dw1))
        dw1 = 0L;
    SET_DINTOUT(dw1);
    return TRUE;
#else /*T5DEF_MALLOCSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MALLOCSUPPORTED*/
}

static T5_BOOL _STFS_ARWRITE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MALLOCSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1, dw2;

    dw1 = GET_DINTIN(3);
    dw2 = T5VMRsc_WriteArray (pDB, GET_DINTIN(1) + 1L, GET_DINTIN(2),
                                sizeof (T5_LONG), &dw1);
    SET_DINTOUT(dw2);
    return TRUE;
#else /*T5DEF_MALLOCSUPPORTED*/
    return FALSE;
#endif /*T5DEF_MALLOCSUPPORTED*/
}

/* BCD conversion ***********************************************************/

static T5_BOOL _STFS_BIN_TO_BCD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1, dw2, dw3;

    dw1 = GET_DINTIN(1);
    dw2 = 0L;
    dw3 = 1L;
    if ((T5_LONG)dw1 > 0)
    {
        while (dw1)
        {
            dw2 += ((dw1 % 10L) * dw3);
            dw1 /= 10L;
            dw3 *= 16L;
        }
    }
    SET_DINTOUT(dw2);
    return TRUE;
}

static T5_BOOL _STFS_BCD_TO_BIN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1, dw2, dw3;

    dw1 = GET_DINTIN(1);
    dw2 = 0L;
    dw3 = 1L;
    if ((T5_LONG)dw1 > 0)
    {
        while (dw1)
        {
            if ((dw1 % 16L) > 9L)
            {
                dw1 = 0L; /* invalid: break */
                dw2 = 0L;
            }
            else
            {
                dw2 += ((dw1 % 16L) * dw3);
                dw1 /= 16L;
                dw3 *= 10L;
            }
        }
    }
    SET_DINTOUT(dw2);
    return TRUE;
}

/* move operations **********************************************************/

static T5_BOOL _STFS_MOVEBLOCK (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT (_MoveBlock (pDB, pArgs[1], pArgs[2], pArgs[3], pArgs[4],
               pArgs[5], GET_DINTIN(6), GET_DINTIN(7), GET_DINTIN(8)));
    return TRUE;
}

static T5_BOOL _STFS_MBSHIFT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT (_MBShift (pDB, pArgs[1], pArgs[2], pArgs[3],
                            GET_DINTIN(4), GET_DINTIN(5), GET_DINTIN(6),
                            GET_D8IN(7), GET_D8IN(8), GET_D8IN(9)));
    return TRUE;
}

static T5_BOOL _STFS_COUNTOF (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT((T5_LONG)(pArgs[3]) & 0xffffL);
    return TRUE;
}

/* data serialization *******************************************************/

static T5_BOOL _STFS_SERIALIZEOUT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_SERIALIZEDATA
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (_SerializeOut (pDB, pArgs[1], pArgs[2], pArgs[3],
                                pArgs[4], GET_DINTIN(5), GET_D8IN(6)));
    return TRUE;
#else /*T5DEF_SERIALIZEDATA*/
    return FALSE;
#endif /*T5DEF_SERIALIZEDATA*/
}

static T5_BOOL _STFS_SERIALIZEIN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_SERIALIZEDATA
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (_SerializeIn (pDB, pArgs[1], pArgs[2], pArgs[3],
                               pArgs[4], GET_DINTIN(5), GET_D8IN(6)));
    return TRUE;
#else /*T5DEF_SERIALIZEDATA*/
    return FALSE;
#endif /*T5DEF_SERIALIZEDATA*/
}

static T5_BOOL _STFS_SERGETSTRING (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_SERIALIZEDATA
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

#ifdef T5DEF_STRINGSUPPORTED
    SET_DINTOUT (_SerGetString (pDB, pArgs[1], pArgs[2], pArgs[3],
                     GET_DINTIN(4), GET_DINTIN(5), GET_D8IN(6), GET_D8IN(7)));
#endif /*T5DEF_STRINGSUPPORTED*/
    return TRUE;
#else /*T5DEF_SERIALIZEDATA*/
    return FALSE;
#endif /*T5DEF_SERIALIZEDATA*/
}

static T5_BOOL _STFS_SERPUTSTRING (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_SERIALIZEDATA
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

#ifdef T5DEF_STRINGSUPPORTED
    SET_DINTOUT (_SerPutString (pDB, pArgs[1], pArgs[2], pArgs[3],
                     GET_DINTIN(4), GET_DINTIN(5), GET_D8IN(6), GET_D8IN(7)));
#endif /*T5DEF_STRINGSUPPORTED*/
    return TRUE;
#else /*T5DEF_SERIALIZEDATA*/
    return FALSE;
#endif /*T5DEF_SERIALIZEDATA*/
}

/* Date and time ************************************************************/

static T5_BOOL _STFS_DAY_TIME (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RTCLOCKSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_BYTE s[257];

#ifdef T5DEF_STRINGSUPPORTED
    IFNOT_ISSTRINGLOCKED(*pArgs)
    {
        *s = T5RtClk_GetString (GET_DINTIN(1), (T5_PTCHAR)(s+1));
        T5VMStr_Force (pDB, *pArgs, s);
    }
#endif /*T5DEF_STRINGSUPPORTED*/
    return TRUE;
#else /*T5DEF_RTCLOCKSUPPORTED*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSUPPORTED*/
}

static T5_BOOL _STFS_DTFORMAT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RTCLOCKSTAMP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);

#ifdef T5DEF_STRINGSUPPORTED
    IFNOT_ISSTRINGLOCKED(*pArgs)
    {
        T5VMStr_DateFmt (pDB, pArgs[0], pArgs[1],
                            T5RtClk_GetCurDateStamp (),
                            T5RtClk_GetCurTimeStamp ());
    }
#endif /*T5DEF_STRINGSUPPORTED*/
    return TRUE;
#else /*T5DEF_RTCLOCKSTAMP*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSTAMP*/
}

static T5_BOOL _STFS_DTCURTIME (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RTCLOCKSTAMP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (T5RtClk_GetCurTimeStamp ());
    return TRUE;
#else /*T5DEF_RTCLOCKSTAMP*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSTAMP*/
}

static T5_BOOL _STFS_DTCURDATE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RTCLOCKSTAMP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (T5RtClk_GetCurDateStamp ());
    return TRUE;
#else /*T5DEF_RTCLOCKSTAMP*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSTAMP*/
}

static T5_BOOL _STFS_DTDAY (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RTCLOCKSTAMP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;

    dw1 = GET_DINTIN(1);
    SET_DINTOUT(dw1 & 0xffL);
    return TRUE;
#else /*T5DEF_RTCLOCKSTAMP*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSTAMP*/
}

static T5_BOOL _STFS_DTMONTH (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RTCLOCKSTAMP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;

    dw1 = GET_DINTIN(1);
    SET_DINTOUT((dw1 >> 8) & 0xffL);
    return TRUE;
#else /*T5DEF_RTCLOCKSTAMP*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSTAMP*/
}

static T5_BOOL _STFS_DTYEAR (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RTCLOCKSTAMP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;

    dw1 = GET_DINTIN(1);
    SET_DINTOUT((dw1 >> 16) & 0xffffL);
    return TRUE;
#else /*T5DEF_RTCLOCKSTAMP*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSTAMP*/
}

static T5_BOOL _STFS_DTSEC (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RTCLOCKSTAMP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;

    dw1 = GET_DINTIN(1);
    SET_DINTOUT((dw1 / 1000L) % 60L);
    return TRUE;
#else /*T5DEF_RTCLOCKSTAMP*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSTAMP*/
}

static T5_BOOL _STFS_DTMIN (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RTCLOCKSTAMP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;

    dw1 = GET_DINTIN(1);
    SET_DINTOUT((dw1 / 60000L) % 60L);
    return TRUE;
#else /*T5DEF_RTCLOCKSTAMP*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSTAMP*/
}

static T5_BOOL _STFS_DTHOUR (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RTCLOCKSTAMP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;

    dw1 = GET_DINTIN(1);
    SET_DINTOUT(dw1 / 3600000L);
    return TRUE;
#else /*T5DEF_RTCLOCKSTAMP*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSTAMP*/
}

static T5_BOOL _STFS_DTMS (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RTCLOCKSTAMP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;

    dw1 = GET_DINTIN(1);
    SET_DINTOUT(dw1 % 1000L);
    return TRUE;
#else /*T5DEF_RTCLOCKSTAMP*/
    return FALSE;
#endif /*T5DEF_RTCLOCKSTAMP*/
}

/* ASi **********************************************************************/

static T5_BOOL _STFS_ASIREADPP (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ASI
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;
    T5PTR_ASI pASI;

    pASI = T5GET_ASI(pDB);
    if (pASI == NULL)
    {
        SET_DINTOUT(0);
    }
    else
    {
        dw1 = (T5_DWORD)T5Asi_ReadPP (pASI, (T5_WORD)GET_DINTIN(1),
                                        (T5_WORD)GET_DINTIN(2));
        SET_DINTOUT(dw1 & 0xff);
    }
    return TRUE;
#else /*T5DEF_ASI*/
    return FALSE;
#endif /*T5DEF_ASI*/
}

static T5_BOOL _STFS_ASIWRITEPP (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ASI
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_ASI pASI;

    pASI = T5GET_ASI(pDB);
    if (pASI == NULL)
    {
        SET_D8OUT(0);
    }
    else
    {
        SET_D8OUT (T5Asi_WritePP (pASI, (T5_WORD)GET_DINTIN(1),
                                    (T5_WORD)GET_DINTIN(2),
                                    (T5_BYTE)GET_DINTIN(3)));
    }
    return TRUE;
#else /*T5DEF_ASI*/
    return FALSE;
#endif /*T5DEF_ASI*/
}

static T5_BOOL _STFS_ASISENDPAR (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ASI
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_ASI pASI;

    pASI = T5GET_ASI(pDB);
    if (pASI == NULL)
    {
        SET_D8OUT(0);
    }
    else
    {
        SET_D8OUT (T5Asi_SendParameter (pASI, (T5_WORD)GET_DINTIN(1),
                                        (T5_WORD)GET_DINTIN(2),
                                        (T5_BYTE)GET_DINTIN(3)));
    }
    return TRUE;
#else /*T5DEF_ASI*/
    return FALSE;
#endif /*T5DEF_ASI*/
}

static T5_BOOL _STFS_ASIREADPI (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ASI
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_LONG dw1;
    T5PTR_ASI pASI;

    pASI = T5GET_ASI(pDB);
    if (pASI == NULL)
    {
        SET_DINTOUT(0);
    }
    else
    {
        dw1 = (T5_DWORD)T5Asi_ReadPI (pASI, (T5_WORD)GET_DINTIN(1),
                                        (T5_WORD)GET_DINTIN(2));
        SET_DINTOUT(dw1 & 0xff);
    }
    return TRUE;
#else /*T5DEF_ASI*/
    return FALSE;
#endif /*T5DEF_ASI*/
}

static T5_BOOL _STFS_ASISTOREPI (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_ASI
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5PTR_ASI pASI;

    pASI = T5GET_ASI(pDB);
    if (pASI == NULL)
    {
        SET_D8OUT(0);
    }
    else
    {
        SET_D8OUT (T5Asi_StorePI (pASI, (T5_WORD)GET_DINTIN(1)));
    }
    return TRUE;
#else /*T5DEF_ASI*/
    return FALSE;
#endif /*T5DEF_ASI*/
}

/* System *******************************************************************/

static T5_BOOL _STFS_LOGMESSAGE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    T5VMLog_Push (pDB, (T5_WORD)GET_DINTIN(1), (T5_WORD)GET_DINTIN(2), GET_DINTIN(3));
    SET_D8OUT(1);
    return TRUE;
}

static T5_BOOL _STFS_GETSYSINFO (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (_GetSysInfo (pDB, GET_DINTIN(1)));
    return TRUE;
}

static T5_BOOL _STFS_CYCLESTOP (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);

    if (GET_D8IN(1))
    {
        pStatus->wFlags &= ~T5FLAG_RUN;
        pStatus->wFlags &= ~T5FLAG_PROGSTEP;
    }
    SET_D8OUT(1);
    return TRUE;
}

static T5_BOOL _STFS_FATALSTOP (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

    if (GET_D8IN(1))
        return FALSE;
    else
    {
        SET_D8OUT (FALSE);
    }
    return TRUE;
}

static T5_BOOL _STFS_ENABLEEVENTS (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5PTR_DBSTATUS pStatus = T5GET_DBSTATUS(pDB);

    if (GET_D8IN(1))
    {
        pStatus->wFlags &= ~T5FLAG_EAFREEZE;
        SET_D8OUT(TRUE);
    }
    else
    {
        pStatus->wFlags |= T5FLAG_EAFREEZE;
        SET_D8OUT(FALSE);
    }
    return TRUE;
}

static T5_BOOL _STFS_SETCSVOPT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

    SET_D8OUT (_SetCsvOpt (pDB, T5VMStr_GetText (pDB, pArgs[1]),
                            T5VMStr_GetText (pDB, pArgs[2])));
    return TRUE;
#else /*T5DEF_STRINGSUPPORTED*/
    return FALSE;
#endif /*T5DEF_STRINGSUPPORTED*/
}

/* MAP functions ************************************************************/

static T5_BOOL _STFS_GETVARBYNAME (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

#ifdef T5DEF_STRINGSUPPORTED
    SET_DINTOUT((T5_DWORD)T5Map_GetBySymbol (pDB,
                T5VMStr_GetText (pDB, pArgs[1])));
#endif /*T5DEF_STRINGSUPPORTED*/
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_GETVARBYADDR (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT((T5_DWORD)T5Map_GetByIndex (pDB, pArgs[1], pArgs[2]));
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_GETVARSYMBOL (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

#ifdef T5DEF_STRINGSUPPORTED
    if (!T5Map_CheckPointer (pDB, (T5PTR_DBMAP)GET_DINTIN(1)))
        T5VMStr_AscForceC (pDB, pArgs[0], (T5_PTCHAR)"\0");
    else
        T5VMStr_AscForceC (pDB, pArgs[0],
                    T5Map_GetSymbol ((T5PTR_DBMAP)GET_DINTIN(1)));
#endif /*T5DEF_STRINGSUPPORTED*/
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_GETVARPROFILE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

#ifdef T5DEF_STRINGSUPPORTED
    if (!T5Map_CheckPointer (pDB, (T5PTR_DBMAP)GET_DINTIN(1)))
        T5VMStr_AscForceC (pDB, pArgs[0], (T5_PTCHAR)"\0");
    else
        T5VMStr_AscForceC (pDB, pArgs[0],
                    T5Map_GetProfileName ((T5PTR_DBMAP)GET_DINTIN(1)));
#endif /*T5DEF_STRINGSUPPORTED*/
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_GETVARTYPENAME (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

#ifdef T5DEF_STRINGSUPPORTED
    if (!T5Map_CheckPointer (pDB, (T5PTR_DBMAP)GET_DINTIN(1)))
        T5VMStr_AscForceC (pDB, pArgs[0], (T5_PTCHAR)"\0");
    else
        T5VMStr_AscForceC (pDB, pArgs[0],
                    T5Map_GetTypeName ((T5PTR_DBMAP)GET_DINTIN(1), pDB));
#endif /*T5DEF_STRINGSUPPORTED*/
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

/* Bytes and words - data assembly ******************************************/

static T5_BOOL _STFS_LOBYTE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);

    SET_D8OUT ((T5_BYTE)(GET_D16IN(1) & 0x00ff));
    return TRUE;
}

static T5_BOOL _STFS_HIBYTE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);

    SET_D8OUT ((T5_BYTE)((GET_D16IN(1) >> 8) & 0x00ff));
    return TRUE;
}

static T5_BOOL _STFS_MAKEWORD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);

    SET_D16OUT ((((T5_WORD)GET_D8IN(1) << 8) & 0xff00)
                | ((T5_WORD)GET_D8IN(2) & 0x00ff));
    return TRUE;
}

static T5_BOOL _STFS_LOWORD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D16OUT ((T5_WORD)(GET_DINTIN(1) & 0x0000ffffL));
    return TRUE;
}

static T5_BOOL _STFS_HIWORD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D16OUT ((T5_WORD)((GET_DINTIN(1) >> 16) & 0x0000ffffL));
    return TRUE;
}

static T5_BOOL _STFS_MAKEDWORD (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTWORD pData16 = T5GET_DBDATA16(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT ((((T5_LONG)GET_D16IN(1) << 16) & 0xffff0000L)
                 | ((T5_LONG)GET_D16IN(2) & 0x0000ffffL));
    return TRUE;
}

static T5_BOOL _STFS_PACK8 (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_BYTE i, b1, b2;

    b1 = 0;
    b2 = 0x01; /*mask*/
    for (i=1; i<=8; i++)
    {
        if (pData8[pArgs[i]])
            b1 |= b2;
        b2 <<= 1;
    }
    SET_D8OUT (b1);
    return TRUE;
}

static T5_BOOL _STFS_SETBIT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    _SetBit (pDB, pArgs[1], pArgs[2], pArgs[0],
                (T5_DWORD)GET_DINTIN(3), GET_D8IN(4));
    return TRUE;
}

static T5_BOOL _STFS_TESTBIT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT(_GetBit (pDB, pArgs[1], pArgs[2], (T5_DWORD)GET_DINTIN(3)));
    return TRUE;
}

/* Recipes, string tables and signals ***************************************/

static T5_BOOL _STFS_RCPAPPLY (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_RECIPES
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT (_ApplyRcpColumn (pDB, GET_DINTIN(1), GET_DINTIN(2)));
    return TRUE;
#else /*T5DEF_RECIPES*/
    return FALSE;
#endif /*T5DEF_RECIPES*/
}

static T5_BOOL _STFS_STRINGTABLE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGTABLE
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT (_StringTable (pDB, GET_DINTIN(1)));
    return TRUE;
#else /*T5DEF_STRINGTABLE*/
    return FALSE;
#endif /*T5DEF_STRINGTABLE*/
}

static T5_BOOL _STFS_LOADSTRING (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_STRINGTABLE
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    if (!ISLOCKSTRING(*pArgs))
    {
        _LoadString (pDB, *pArgs, GET_DINTIN(1));
    }
    return TRUE;
#else /*T5DEF_STRINGTABLE*/
    return FALSE;
#endif /*T5DEF_STRINGTABLE*/
}

static T5_BOOL _STFS_SIGSCALE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_SIGNAL
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
    T5_PTREAL pReal = (T5_PTREAL)pDint;

    if (!ISLOCKSTRING(*pArgs))
    {
        SET_REALOUT (_SigScale (pDB, GET_DINTIN(1), pArgs[2]));
    }
    return TRUE;
#else /*T5DEF_SIGNAL*/
    return FALSE;
#endif /*T5DEF_SIGNAL*/
}

/* VSI functions ************************************************************/

static T5_BOOL _STFS_VSIGETBIT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT (T5Map_GetVSIBit (pDB, pArgs[1], pArgs[2], GET_DINTIN(3)));
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_VSISETBIT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT (T5Map_SetVSIBit (pDB, pArgs[1], pArgs[2], GET_DINTIN(3), GET_D8IN(4)));
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_VSIGETDATE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (T5Map_GetVSIDate (pDB, pArgs[1], pArgs[2]));
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_VSISETDATE (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT (T5Map_SetVSIDate (pDB, pArgs[1], pArgs[2], GET_DINTIN(3)));
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_VSIGETTIME (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_DINTOUT (T5Map_GetVSITime (pDB, pArgs[1], pArgs[2]));
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_VSISETTIME (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    SET_D8OUT (T5Map_SetVSITime (pDB, pArgs[1], pArgs[2], GET_DINTIN(3)));
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_VSISTAMP (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

    SET_D8OUT (T5Map_StampVSI (pDB, pArgs[1], pArgs[2]));
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_VSISTAMPGMT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

    SET_D8OUT (T5Map_StampVSIGMT (pDB, pArgs[1], pArgs[2]));
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

static T5_BOOL _STFS_VSICOPY (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_VARMAP
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    SET_D8OUT (T5Map_CopyVSI (pDB, pArgs[1], pArgs[2], pArgs[3], GET_D8IN(4), GET_D8IN(5)));
    return TRUE;
#else /*T5DEF_VARMAP*/
    return FALSE;
#endif /*T5DEF_VARMAP*/
}

/* Registry functions *******************************************************/

static T5_BOOL _STFS_REGPARGET (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_REG
    _RegParGet (pDB, pArgs[1], T5VMStr_GetText (pDB, pArgs[2]), pArgs[0], pArgs[3]);
    return TRUE;
#else /*T5DEF_REG*/
    return FALSE;
#endif /*T5DEF_REG*/
}

static T5_BOOL _STFS_REGPARPUT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_REG
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

    SET_D8OUT (_RegParPut (pDB, pArgs[1], T5VMStr_GetText (pDB, pArgs[2]), pArgs[3]));
    return TRUE;
#else /*T5DEF_REG*/
    return FALSE;
#endif /*T5DEF_REG*/
}

/* CAN functions ************************************************************/

static T5_BOOL _STFS_CANSND (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_CANBUS
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);
    T5_PTLONG pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);

    if (GET_DINTIN(3) > (T5_LONG)(pArgs[5]))
    {
        SET_D8OUT (FALSE);
    }
    else
    {
        SET_D8OUT (T5VMCan_SendAppMsg (pDB, T5VMStr_GetText (pDB, pArgs[1]),
                                        (T5_DWORD)GET_DINTIN(2), GET_DINTIN(3),
                                        T5Tools_GetD8Array (pDB, pArgs[4]),
                                        GET_D8IN(6)));
    }
    return TRUE;
#else /*T5DEF_CANBUS*/
    return FALSE;
#endif /*T5DEF_CANBUS*/
}

/* MODBUS functions *********************************************************/

static T5_BOOL _STFS_MBSLAVEIDENT (T5PTR_DB pDB, T5_PTWORD pArgs)
{
#ifdef T5DEF_MODBUSIOS
    T5_PTBYTE pLock = T5GET_DBLOCK(pDB);
    T5_PTBYTE pData8 = T5GET_DBDATA8(pDB);

#ifdef T5DEF_STRINGSUPPORTED
    SET_D8OUT (T5MBSrv_SetDevIdent (T5VMStr_GetText (pDB, pArgs[1]),
                                    T5VMStr_GetText (pDB, pArgs[2]),
                                    T5VMStr_GetText (pDB, pArgs[3]),
                                    T5Tools_GetD16Array (pDB, pArgs[4]),
                                    pArgs[5]));
#endif /*T5DEF_STRINGSUPPORTED*/
    return TRUE;
#else /*T5DEF_MODBUSIOS*/
    return FALSE;
#endif /*T5DEF_MODBUSIOS*/
}

/****************************************************************************/
/* PART FOUR : THE TABLE OF FUNCTION HANDLERS AND THE MAIN ENTRY POINT */

static const T5HND_STF _STFS[] = {
/*0                       */    NULL,
/*T5FNC_ABS               1*/   _STFS_ABS,
/*T5FNC_ACOS              2*/   _STFS_ACOS,
/*T5FNC_ASIN              3*/   _STFS_ASIN,
/*T5FNC_ATAN              4*/   _STFS_ATAN,
/*T5FNC_COS               5*/   _STFS_COS,
/*T5FNC_EXPT              6*/   _STFS_EXPT,
/*T5FNC_LOG               7*/   _STFS_LOG,
/*T5FNC_SIN               8*/   _STFS_SIN,
/*T5FNC_SQRT              9*/   _STFS_SQRT,
/*T5FNC_TAN               10*/  _STFS_TAN,
/*T5FNC_TRUNC             11*/  _STFS_TRUNC,
/*T5FNC_POW               12*/  _STFS_POW,
/*VSISTAMPGMT             */    _STFS_VSISTAMPGMT,
/*UDPSENDTOARRAY          */    _STFS_UDPSENDTOARRAY,
/*UDPRCVFROMARRAY         */    _STFS_UDPRCVFROMARRAY,
/*T5FNC_FR_READ           */    _STFS_FR_READ,
/*T5FNC_FR_WRITE          */    _STFS_FR_WRITE,
/*T5FNC_F_FLUSH           */    _STFS_F_FLUSH,
/*T5FNC_F_SEEK            */    _STFS_F_SEEK,
/*T5FNC_AND_MASK          20*/  _STFS_ANDMASK,
/*T5FNC_LIMIT             21*/  _STFS_LIMIT,
/*T5FNC_MAX               22*/  _STFS_MAX,
/*T5FNC_MIN               23*/  _STFS_MIN,
/*T5FNC_MOD               24*/  _STFS_MOD,
/*T5FNC_MUX4              25*/  _STFS_MUX4,
/*T5FNC_MUX8              26*/  _STFS_MUX8,
/*T5FNC_NOT_MASK          27*/  _STFS_NOTMASK,
/*T5FNC_OR_MASK           28*/  _STFS_ORMASK,
/*T5FNC_RAND              29*/  _STFS_RAND,
/*T5FNC_ROL               30*/  _STFS_ROL,
/*T5FNC_ROR               31*/  _STFS_ROR,
/*T5FNC_SEL               32*/  _STFS_SEL,
/*T5FNC_SHL               33*/  _STFS_SHL,
/*T5FNC_SHR               34*/  _STFS_SHR,
/*T5FNC_XOR_MASK          35*/  _STFS_XORMASK,
/*T5FNC_ODD               36*/  _STFS_ODD,
/*37                      */    NULL,
/*38                      */    NULL,
/*39                      */    NULL,
/*T5FNC_ASCII             40*/  _STFS_ASCII,
/*T5FNC_CHAR              41*/  _STFS_CHAR,
/*T5FNC_DELETE            42*/  _STFS_DELETE,
/*T5FNC_FIND              43*/  _STFS_FIND,
/*T5FNC_INSERT            44*/  _STFS_INSERT,
/*T5FNC_LEFT              45*/  _STFS_LEFT,
/*T5FNC_MID               46*/  _STFS_MID,
/*T5FNC_MLEN              47*/  _STFS_MLEN,
/*T5FNC_REPLACE           48*/  _STFS_REPLACE,
/*T5FNC_RIGHT             49*/  _STFS_RIGHT,
/*50                      */    NULL,
/*51                      */    NULL,
/*52                      */    NULL,
/*53                      */    NULL,
/*54                      */    NULL,
/*55                      */    NULL,
/*56                      */    NULL,
/*57                      */    NULL,
/*58                      */    NULL,
/*59                      */    NULL,
/*T5FNC_ARCREATE          60*/  _STFS_ARCREATE,
/*T5FNC_ARREAD            61*/  _STFS_ARREAD,
/*T5FNC_ARWRITE           62*/  _STFS_ARWRITE,
/*T5FNC_DAY_TIME          63*/  _STFS_DAY_TIME,
/*64                      */    NULL,
/*65                      */    NULL,
/*66                      */    NULL,
/*67                      */    NULL,
/*68                      */    NULL,
/*69                      */    NULL,
/*T5FNC_F_ROPEN           70*/  _STFS_F_ROPEN,
/*T5FNC_F_WOPEN           71*/  _STFS_F_WOPEN,
/*T5FNC_FA_READ           72*/  _STFS_FA_READ,
/*T5FNC_FM_READ           73*/  _STFS_FM_READ,
/*T5FNC_FA_WRITE          74*/  _STFS_FA_WRITE,
/*T5FNC_FM_WRITE          75*/  _STFS_FM_WRITE,
/*T5FNC_F_CLOSE           76*/  _STFS_F_CLOSE,
/*T5FNC_F_EOF             77*/  _STFS_F_EOF,
/*T5FNC_BIN_TO_BCD        78*/  _STFS_BIN_TO_BCD,
/*T5FNC_BCD_TO_BIN        79*/  _STFS_BCD_TO_BIN,
/*T5FNC_PRINTF            80*/  _STFS_PRINTF,
/*T5FNC_ROLW              81*/  _STFS_ROLW,
/*T5FNC_RORW              82*/  _STFS_RORW,
/*T5FNC_SHLW              83*/  _STFS_SHLW,
/*T5FNC_SHRW              84*/  _STFS_SHRW,
/*T5FNC_ROLB              85*/  _STFS_ROLB,
/*T5FNC_RORB              86*/  _STFS_RORB,
/*T5FNC_SHLB              87*/  _STFS_SHLB,
/*T5FNC_SHRB              88*/  _STFS_SHRB,
/*T5FNC_ATOH              89*/  _STFS_ATOH,
/*T5FNC_HTOA              90*/  _STFS_HTOA,
/*T5FNC_USEDEGREES        91*/  _STFS_USEDEGREES,
/*T5FNC_ATAN2             92*/  _STFS_ATAN2,
/*T5FNC_CRC16             93*/  _STFS_CRC16,
/*T5FNC_LOGMESSAGE        94*/  _STFS_LOGMESSAGE,
/*T5FNC_MOVEBLOCK         95*/  _STFS_MOVEBLOCK,
/*T5FNC_ANDWORD           96*/  _STFS_ANDWORD,
/*T5FNC_ORWORD            97*/  _STFS_ORWORD,
/*T5FNC_XORWORD           98*/  _STFS_XORWORD,
/*T5FNC_NOTWORD           99*/  _STFS_NOTWORD,
/*T5FNC_ANDBYTE           100*/ _STFS_ANDBYTE,
/*T5FNC_ORBYTE            101*/ _STFS_ORBYTE,
/*T5FNC_XORBYTE           102*/ _STFS_XORBYTE,
/*T5FNC_NOTBYTE           103*/ _STFS_NOTBYTE,
/*T5FNC_SERIALIZEOUT      104*/ _STFS_SERIALIZEOUT,
/*T5FNC_SERIALIZEIN       105*/ _STFS_SERIALIZEIN,
/*T5FNC_COSL              106*/ _STFS_COSL,
/*T5FNC_SINL              107*/ _STFS_SINL,
/*T5FNC_TANL              108*/ _STFS_TANL,
/*T5FNC_ACOSL             109*/ _STFS_ACOSL,
/*T5FNC_ASINL             110*/ _STFS_ASINL,
/*T5FNC_ATANL             111*/ _STFS_ATANL,
/*T5FNC_ATAN2L            112*/ _STFS_ATAN2L,
/*T5FNC_GETSYSINFO        113*/ _STFS_GETSYSINFO,
/*T5FNC_POWL              114*/ _STFS_POWL,
/*T5FNC_SQRTL             115*/ _STFS_SQRTL,
/*T5FNC_ABSL              116*/ _STFS_ABSL,
/*T5FNC_TRUNCL            117*/ _STFS_TRUNCL,
/*T5FNC_DTCURTIME         118*/ _STFS_DTCURTIME,
/*T5FNC_DTCURDATE         119*/ _STFS_DTCURDATE,
/*T5FNC_DTDAY             120*/ _STFS_DTDAY,
/*T5FNC_DTMONTH           121*/ _STFS_DTMONTH,
/*T5FNC_DTYEAR            122*/ _STFS_DTYEAR,
/*T5FNC_DTSEC             123*/ _STFS_DTSEC,
/*T5FNC_DTMIN             124*/ _STFS_DTMIN,
/*T5FNC_DTHOUR            125*/ _STFS_DTHOUR,
/*T5FNC_DTMS              126*/ _STFS_DTMS,
/*T5FNC_ASIREADPP         127*/ _STFS_ASIREADPP,
/*T5FNC_ASIWRITEPP        128*/ _STFS_ASIWRITEPP,
/*T5FNC_ASISENDPAR        129*/ _STFS_ASISENDPAR,
/*T5FNC_ASIREADPI         130*/ _STFS_ASIREADPI,
/*T5FNC_ASISTOREPI        131*/ _STFS_ASISTOREPI,
/*T5FNC_MODR              132*/ _STFS_MODR,
/*T5FNC_MODLR             133*/ _STFS_MODLR,
/*T5FNC_MBSHIFT           134*/ _STFS_MBSHIFT,
/*T5FNC_FATALSTOP         135*/ _STFS_FATALSTOP,
/*T5FNC_CYCLESTOP         136*/ _STFS_CYCLESTOP,
/*T5FNC_STRINGTOARRAY     137*/ _STFS_STRINGTOARRAY,
/*T5FNC_ARRAYTOSTRING     138*/ _STFS_ARRAYTOSTRING,
/*T5FNC_STRINGTOARRAY32   139*/ _STFS_SRINGTOARRAY32,
/*T5FNC_ARRAYTOSTRING32   140*/ _STFS_ARRAYTOSTRING32,
/*T5FNC_STRINGTOARRAY16   141*/ _STFS_SRINGTOARRAY16,
/*T5FNC_ARRAYTOSTRING16   142*/ _STFS_ARRAYTOSTRING16,
/*T5FNC_GETVARBYNAME      143*/ _STFS_GETVARBYNAME,
/*T5FNC_GETVARBYADDR      144*/ _STFS_GETVARBYADDR,
/*T5FNC_GETVARSYMBOL      145*/ _STFS_GETVARSYMBOL,
/*T5FNC_GETVARPROFILE     146*/ _STFS_GETVARPROFILE,
/*T5FNC_GETVARTYPENAME    147*/ _STFS_GETVARTYPENAME,
/*T5FNC_TCPLISTEN         148*/ _STFS_TCPLISTEN,
/*T5FNC_TCPACCEPT         149*/ _STFS_TCPACCEPT,
/*T5FNC_TCPCLOSE          150*/ _STFS_TCPCLOSE,
/*T5FNC_TCPSEND           151*/ _STFS_TCPSEND,
/*T5FNC_TCPRECEIVE        152*/ _STFS_TCPRECEIVE,
/*T5FNC_TCPCONNECT        153*/ _STFS_TCPCONNECT,
/*T5FNC_TCPISCONNECTED    154*/ _STFS_TCPISCONNECTED,
/*T5FNC_TCPISVALID        155*/ _STFS_TCPISVALID,
/*T5FNC_ENABLEEVENTS      156*/ _STFS_ENABLEEVENTS,
/*T5FNC_SCALELIN          157*/ _STFS_SCALELIN,
/*T5FNC_RCPAPPLY          158*/ _STFS_RCPAPPLY,
/*T5FNC_SERGETSTRING      159*/ _STFS_SERGETSTRING,
/*T5FNC_SERPUTSTRING      160*/ _STFS_SERPUTSTRING,
/*T5FNC_COUNTOF           161*/ _STFS_COUNTOF,
/*T5FNC_LOBYTE            162*/ _STFS_LOBYTE,
/*T5FNC_HIBYTE            163*/ _STFS_HIBYTE,
/*T5FNC_LOWORD            164*/ _STFS_LOWORD,
/*T5FNC_HIWORD            165*/ _STFS_HIWORD,
/*T5FNC_MAKEWORD          166*/ _STFS_MAKEWORD,
/*T5FNC_MAKEDWORD         167*/ _STFS_MAKEDWORD,
/*T5FNC_PACK8             168*/ _STFS_PACK8,
/*T5FNC_SETBIT            169*/ _STFS_SETBIT,
/*T5FNC_TESTBIT           170*/ _STFS_TESTBIT,
/*T5FNC_DTFORMAT          171*/ _STFS_DTFORMAT,
/*T5FNC_FB_READ           172*/ _STFS_FB_READ,
/*T5FNC_FB_WRITE          173*/ _STFS_FB_WRITE,
/*T5FNC_F_AOPEN           174*/ _STFS_F_AOPEN,
/*T5FNC_STRINGTABLE       175*/ _STFS_STRINGTABLE,
/*T5FNC_LOADSTRING        176*/ _STFS_LOADSTRING,
/*T5FNC_SIGSCALE          177*/ _STFS_SIGSCALE,
/*T5FNC_F_EXIST           178*/ _STFS_F_EXIST,
/*T5FNC_F_GETSIZE         179*/ _STFS_F_GETSIZE,
/*T5FNC_F_COPY            180*/ _STFS_F_COPY,
/*T5FNC_F_DELETE          181*/ _STFS_F_DELETE,
/*T5FNC_F_RENAME          182*/ _STFS_F_RENAME,
/*T5FNC_UDPCREATE         183*/ _STFS_UDPCREATE,
/*T5FNC_UDPADDRMAKE       184*/ _STFS_UDPADDRMAKE,
/*T5FNC_UDPSENDTO         185*/ _STFS_UDPSENDTO,
/*T5FNC_UDPRCVFROM        186*/ _STFS_UDPRCVFROM,
/*T5FNC_VSIGETBIT         187*/ _STFS_VSIGETBIT,
/*T5FNC_VSISETBIT         188*/ _STFS_VSISETBIT,
/*T5FNC_VSIGETDATE        189*/ _STFS_VSIGETDATE,
/*T5FNC_VSISETDATE        190*/ _STFS_VSISETDATE,
/*T5FNC_VSIGETTIME        191*/ _STFS_VSIGETTIME,
/*T5FNC_VSISETTIME        192*/ _STFS_VSISETTIME,
/*T5FNC_VSISTAMP          193*/ _STFS_VSISTAMP,
/*T5FNC_REGPARGET         194*/ _STFS_REGPARGET,
/*T5FNC_REGPARPUT         195*/ _STFS_REGPARPUT,
/*T5FNC_NUMTOSTRING       196*/ _STFS_NUMTOSTRING,
/*T5FNC_F_ISREADY         197*/ _STFS_F_ISREADY,
/*T5FNC_F_SETCSVOPT       198*/ _STFS_SETCSVOPT,
/*T5FNC_F_CANSND          199*/ _STFS_CANSND,
/*T5FNC_F_LN              200*/ _STFS_LN,
/*T5FNC_F_EXP             201*/ _STFS_EXP,
/*T5FNC_F_LNL             202*/ _STFS_LNL,
/*T5FNC_F_EXPL            203*/ _STFS_EXPL,
/*T5FNC_F_LOGL            204*/ _STFS_LOGL,
/*T5FNC_F_LEN             205*/ _STFS_F_LEN,
/*T5FNC_ROL64             206*/ _STFS_ROL64,
/*T5FNC_ROR64             207*/ _STFS_ROR64,
/*T5FNC_SHL64             208*/ _STFS_SHL64,
/*T5FNC_SHR64             209*/ _STFS_SHR64,
/*T5FNC_AND64             210*/ _STFS_AND64,
/*T5FNC_OR64              211*/ _STFS_OR64,
/*T5FNC_XOR64             212*/ _STFS_XOR64,
/*T5FNC_NOT64             213*/ _STFS_NOT64,
/*T5FNC_TCPSNDARRAY       214*/ _STFS_TCPSENDARRAY,
/*T5FNC_TCPRCVARRAY       215*/ _STFS_TCPRCVARRAY,
/*T5FNC_F_SAVERETAIN      216*/ _STFS_F_SAVERETAIN,
/*T5FNC_F_LOADRETAIN      217*/ _STFS_F_LOADRETAIN,
/*T5FNC_MBSLAVEIDENT      218*/ _STFS_MBSLAVEIDENT,
/*T5FNC_SWAB              219*/ _STFS_SWAB,
/*T5FNC_VSICOPY           220*/ _STFS_VSICOPY,
NULL };

/*****************************************************************************
T5VMStf_Execute
execute a standard function
generates a log error in case of not supported function
Parameters:
    pDB (IN) pointer to the data base
    pArgs (IN) pointer to the list of arguments
arguments are: <function ID> <nbPar> <ParOut> <ParInpList>
return: TRUE if OK - FALSE in case of not supported function
*****************************************************************************/

T5_BOOL T5VMStf_Execute (T5PTR_DB pDB, T5_PTWORD pArgs)
{
    T5_BOOL bOK;

    bOK = FALSE;
    if (*pArgs < (sizeof (_STFS) / sizeof (T5HND_STF)) && _STFS[*pArgs] != NULL)
    {
        /* skip function ID and nb of arguments */
        bOK = _STFS[*pArgs] (pDB, pArgs + 2);
        /* never outupt a log message for FatalStop() */
        if (!bOK && *pArgs == T5FNC_FATALSTOP)
            return FALSE;
    }
    /* unknown or not implemented function -> fatal error */
    if (!bOK)
        T5VMLog_Push (pDB, T5RET_UNKNOWNSTF, *pArgs, 0);
    return bOK;
}

/****************************************************************************/
/* PART FIVE : implementation of static services */

/****************************************************************************/
/* MoveBlock standard function */

static T5_BOOL _MoveBlock (T5PTR_DB pDB, T5_WORD wType,
                           T5_WORD wSrc, T5_WORD wSrcSizeof,
                           T5_WORD wDst, T5_WORD wDstSizeof,
                           T5_DWORD dwPosSrc, T5_DWORD dwPosDst,
                           T5_DWORD dwNb)
{
    T5_PTBYTE pSrc, pDst;
    T5_DWORD dwSize;

    /* Check input parameters */
    if (dwNb == 0)
        return FALSE;
    if ((dwPosSrc + dwNb) > (T5_DWORD)wSrcSizeof)
        return FALSE;
    if ((dwPosDst + dwNb) > (T5_DWORD)wDstSizeof)
        return FALSE;

    /* get pointers and item size */
    switch (wType)
    {
    case T5C_BOOL  :
    case T5C_SINT :
    case T5C_USINT :
        pSrc = T5Tools_GetD8Array (pDB, wSrc);
        pSrc += dwPosSrc;
        pDst = T5Tools_GetD8Array (pDB, wDst);
        pDst += dwPosDst;
        dwSize = 1;
        break;
    case T5C_INT :
    case T5C_UINT :
#ifdef T5DEF_DATA16SUPPORTED
        pSrc = (T5_PTBYTE)T5Tools_GetD16Array (pDB, wSrc);
        pSrc += (2 * dwPosSrc);
        pDst = (T5_PTBYTE)T5Tools_GetD16Array (pDB, wDst);
        pDst += (2 * dwPosDst);
        dwSize = 2;
        break;
#else /*T5DEF_DATA16SUPPORTED*/
        return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/

    case T5C_DINT  :
    case T5C_UDINT :
    case T5C_REAL  :
        pSrc = (T5_PTBYTE)T5Tools_GetD32Array (pDB, wSrc);
        pSrc += (4 * dwPosSrc);
        pDst = (T5_PTBYTE)T5Tools_GetD32Array (pDB, wDst);
        pDst += (4 * dwPosDst);
        dwSize = 4;
        break;

    case T5C_TIME  :
        pSrc = (T5_PTBYTE)T5Tools_GetTimeArray (pDB, wSrc);
        pSrc += (4 * dwPosSrc);
        pDst = (T5_PTBYTE)T5Tools_GetTimeArray (pDB, wDst);
        pDst += (4 * dwPosDst);
        dwSize = 4;
        break;

    case T5C_LREAL:
    case T5C_LINT :
    case T5C_ULINT :
#ifdef T5DEF_DATA64SUPPORTED
        pSrc = (T5_PTBYTE)T5Tools_GetD64Array (pDB, wSrc);
        pSrc += (8 * dwPosSrc);
        pDst = (T5_PTBYTE)T5Tools_GetD64Array (pDB, wDst);
        pDst += (8 * dwPosDst);
        dwSize = 8;
        break;
#else /*T5DEF_DATA64SUPPORTED*/
        return FALSE;
#endif /*T5DEF_DATA64SUPPORTED*/

    case T5C_STRING  :
        return FALSE;

    default :
        return FALSE;
    }

    if (pDst < pSrc || (pDst >= (pSrc+(dwNb * dwSize))))
        T5_MEMCPY (pDst, pSrc, dwNb * dwSize);
    else
    {
        while (dwNb--)
            T5_MEMCPY (pDst + (dwNb * dwSize), pSrc + (dwNb * dwSize), dwSize);
    }
    return TRUE;
}

/****************************************************************************/
/* data serialization standard functions */

#ifdef T5DEF_SERIALIZEDATA

static T5_LONG _SerializeOut (T5PTR_DB pDB, T5_WORD wType,
                              T5_WORD wDst, T5_WORD wDstSizeof,
                              T5_WORD wSrc, T5_DWORD dwPos, T5_BOOL bBig)
{
    T5_PTBYTE pData8, pDst;
    T5_PTDWORD pData32, pTime;
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pData16;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 pData64;
#endif /*T5DEF_DATA16SUPPORTED*/

    /* initialize local pointers */
    pData8 = T5GET_DBDATA8(pDB);
    pData32 = T5GET_DBDATA32(pDB);
    pTime = T5GET_DBTIME(pDB);
#ifdef T5DEF_DATA16SUPPORTED
    pData16 = T5GET_DBDATA16(pDB);
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
    pData64 = T5GET_DBDATA64(pDB);
#endif /*T5DEF_DATA64SUPPORTED*/

    pDst = T5Tools_GetD8Array (pDB, wDst);

    switch (wType)
    {
    case T5C_BOOL  :
    case T5C_SINT :
    case T5C_USINT :
        if (wDstSizeof < 1 || dwPos > ((T5_DWORD)wDstSizeof - 1))
            return 0;
        pDst[dwPos] = pData8[wSrc];
        return dwPos + 1;

    case T5C_INT :
    case T5C_UINT :
#ifdef T5DEF_DATA16SUPPORTED
        if (wDstSizeof < 2 || dwPos > ((T5_DWORD)wDstSizeof - 2))
            return 0;
        _CopyBytes (pDst+dwPos, (T5_PTBYTE)(pData16+wSrc), 2, bBig);
        return dwPos + 2;
#else /*T5DEF_DATA16SUPPORTED*/
        return 0;
#endif /*T5DEF_DATA16SUPPORTED*/

    case T5C_DINT  :
    case T5C_UDINT :
    case T5C_REAL  :
        if (wDstSizeof < 4 || dwPos > ((T5_DWORD)wDstSizeof - 4))
            return 0;
        _CopyBytes (pDst+dwPos, (T5_PTBYTE)(pData32+wSrc), 4, bBig);
        return dwPos + 4;

    case T5C_TIME  :
        if (wDstSizeof < 4 || dwPos > ((T5_DWORD)wDstSizeof - 4))
            return 0;
        _CopyBytes (pDst+dwPos, (T5_PTBYTE)(pTime+wSrc), 4, bBig);
        return dwPos + 4;

    case T5C_LREAL:
    case T5C_LINT :
    case T5C_ULINT :
#ifdef T5DEF_DATA64SUPPORTED
        if (wDstSizeof < 8 || dwPos > ((T5_DWORD)wDstSizeof - 8))
            return 0;
        _CopyBytes (pDst+dwPos, (T5_PTBYTE)(pData64+wSrc), 8, bBig);
        return dwPos + 8;
#else /*T5DEF_DATA64SUPPORTED*/
        return 0;
#endif /*T5DEF_DATA64SUPPORTED*/
    default :
        return 0;
    }
}

static T5_LONG _SerializeIn (T5PTR_DB pDB, T5_WORD wType,
                             T5_WORD wSrc, T5_WORD wSrcSizeof,
                             T5_WORD wDst, T5_DWORD dwPos, T5_BOOL bBig)
{
    T5_PTBYTE pLock;
    T5_PTBYTE pData8, pSrc;
    T5_PTDWORD pData32, pTime;
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pData16;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 pData64;
#endif /*T5DEF_DATA16SUPPORTED*/

    /* initialize local pointers */
    pLock = T5GET_DBLOCK(pDB);
    pData8 = T5GET_DBDATA8(pDB);
    pData32 = T5GET_DBDATA32(pDB);
    pTime = T5GET_DBTIME(pDB);
#ifdef T5DEF_DATA16SUPPORTED
    pData16 = T5GET_DBDATA16(pDB);
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_DATA64SUPPORTED
    pData64 = T5GET_DBDATA64(pDB);
#endif /*T5DEF_DATA64SUPPORTED*/

    pSrc = T5Tools_GetD8Array (pDB, wSrc);

    switch (wType)
    {
    case T5C_BOOL  :
    case T5C_SINT :
    case T5C_USINT :
        if (dwPos > ((T5_DWORD)wSrcSizeof - 1))
            return 0;
        IFNOT_ISD8LOCKED(wDst)
        {
            pData8[wDst] = pSrc[dwPos];
        }
        return dwPos + 1;

    case T5C_INT :
    case T5C_UINT :
#ifdef T5DEF_DATA16SUPPORTED
        if (dwPos > ((T5_DWORD)wSrcSizeof - 2))
            return 0;
        IFNOT_ISD16LOCKED(wDst)
        {
            _CopyBytes ((T5_PTBYTE)(pData16+wDst), pSrc+dwPos, 2, bBig);
        }
        return dwPos + 2;
#else /*T5DEF_DATA16SUPPORTED*/
        return 0;
#endif /*T5DEF_DATA16SUPPORTED*/

    case T5C_DINT  :
    case T5C_UDINT :
    case T5C_REAL  :
        if (dwPos > ((T5_DWORD)wSrcSizeof - 4))
            return 0;
        IFNOT_ISD32LOCKED(wDst)
        {
            _CopyBytes ((T5_PTBYTE)(pData32+wDst), pSrc+dwPos, 4, bBig);
        }
        return dwPos + 4;

    case T5C_TIME  :
        if (dwPos > ((T5_DWORD)wSrcSizeof - 4))
            return 0;
        IFNOT_ISTIMELOCKED(wDst)
        {
            _CopyBytes ((T5_PTBYTE)(pTime+wDst), pSrc+dwPos, 4, bBig);
        }
        return dwPos + 4;

    case T5C_LREAL:
    case T5C_LINT :
    case T5C_ULINT :
#ifdef T5DEF_DATA64SUPPORTED
        if (dwPos > ((T5_DWORD)wSrcSizeof - 8))
            return 0;
        IFNOT_ISD64LOCKED(wDst)
        {
            _CopyBytes ((T5_PTBYTE)(pData64+wDst), pSrc+dwPos, 8, bBig);
        }
        return dwPos + 8;
#else /*T5DEF_DATA64SUPPORTED*/
        return 0;
#endif /*T5DEF_DATA64SUPPORTED*/
    default :
        return 0;
    }
}

static void _CopyBytes (T5_PTBYTE pDst, T5_PTBYTE pSrc,
                        T5_WORD wSizeof, T5_BOOL bBig)
{
    T5_BOOL bInvert;
    T5_WORD i;

#ifdef T5DEF_BIGENDIAN
    bInvert = !bBig;
#else /*T5DEF_BIGENDIAN*/
    bInvert = bBig;
#endif /*T5DEF_BIGENDIAN*/

    if (bInvert)
    {
        for (i=0; i<(wSizeof / 2); i++)
        {
            pDst[i] = pSrc[wSizeof-i-1];
            pDst[wSizeof-i-1] = pSrc[i];
        }
    }
    else
        T5_MEMCPY(pDst, pSrc, wSizeof);
}

#ifdef T5DEF_STRINGSUPPORTED

static T5_LONG _SerGetString (T5PTR_DB pDB, T5_WORD wSrc, T5_WORD wSrcSizeof,
                              T5_WORD wDst, T5_DWORD dwPos, T5_DWORD dwMaxLen,
                              T5_BOOL bEos, T5_BOOL bHeaded)
{
    T5_BYTE bLen;
    T5_PTBYTE pSrc;
    T5_PTBYTE *pStr;
    T5_PTBYTE pDst;

    pSrc = T5Tools_GetD8Array (pDB, wSrc);
    pStr = T5GET_DBSTRING(pDB);
    pDst = pStr[wDst];

    /*fixed length */
    if (dwMaxLen)
    {
        /* check array length */
        if ((dwPos + dwMaxLen) > (T5_DWORD)wSrcSizeof)
            return 0L;
        /* get actual length (O terminated) */
        bLen = 0;
        while ((T5_DWORD)bLen < dwMaxLen && pSrc[dwPos + (T5_DWORD)bLen] != 0)
            bLen += 1;
        /* copy */
        T5VMStr_AscForce (pDst, pSrc + dwPos, bLen);
        /* return next pos */
        return dwPos + dwMaxLen;
    }
    /* null terminated */
    if (bEos)
    {
        /* check position */
        if (dwPos >= (T5_DWORD)wSrcSizeof)
            return 0L;
        /* get actual length (O terminated) */
        dwMaxLen = (T5_DWORD)wSrcSizeof - dwPos;
        bLen = 0;
        while ((T5_DWORD)bLen < dwMaxLen && pSrc[dwPos + (T5_DWORD)bLen] != 0)
            bLen += 1;
        /* copy */
        T5VMStr_AscForce (pDst, pSrc + dwPos, bLen);
        /* return next pos */
        return (dwPos + (T5_DWORD)bLen + 1L);
    }
    /* headed with length */
    if (bHeaded)
    {
        /* check position */
        if (dwPos >= (T5_DWORD)wSrcSizeof)
            return 0L;
        /* read length */
        bLen = pSrc[dwPos];
        /* check array length */
        if ((dwPos + (T5_DWORD)bLen + 1L) > (T5_DWORD)wSrcSizeof)
            return 0L;
        /* copy */
        T5VMStr_AscForce (pDst, pSrc + dwPos + 1, bLen);
        /* return next pos */
        return (dwPos + (T5_DWORD)bLen + 1L);
    }
    /* error - bad arguments */
    return 0;
}

static T5_LONG _SerPutString (T5PTR_DB pDB, T5_WORD wDst, T5_WORD wDstSizeof,
                              T5_WORD wSrc, T5_DWORD dwPos, T5_DWORD dwMaxLen,
                              T5_BOOL bEos, T5_BOOL bHeaded)
{
    T5_BYTE bLen;
    T5_PTBYTE pSrc;
    T5_PTBYTE *pStr;
    T5_PTBYTE pDst;

    pDst = T5Tools_GetD8Array (pDB, wDst);
    pStr = T5GET_DBSTRING(pDB);
    pSrc = pStr[wSrc];

    /* fixed length */
    if (dwMaxLen)
    {
        /* check array length */
        if ((dwPos + dwMaxLen) > (T5_DWORD)wDstSizeof)
            return 0L;
        /* set null bytes */
        T5_MEMSET (pDst + dwPos, 0, dwMaxLen);
        /* get actual copy size; */
        bLen = pSrc[1];
        if ((T5_DWORD)bLen > dwMaxLen)
            bLen = (T5_BYTE)dwMaxLen;
        /* copy */
        T5_MEMCPY (pDst + dwPos, pSrc + 2, bLen);
        /* return next pos */
        return dwPos + dwMaxLen;
    }
    /* null terminated */
    if (bEos)
    {
        /* check array length */
        if ((dwPos + (T5_DWORD)(pSrc[1]) + 1L) > (T5_DWORD)wDstSizeof)
            return 0L;
        /* copy */
        T5_MEMCPY (pDst + dwPos, pSrc + 2, pSrc[1]);
        pDst[dwPos + (T5_DWORD)(pSrc[1])] = 0;
        /* return next pos */
        return (dwPos + (T5_DWORD)(pSrc[1]) + 1L);
    }
    /* headed with length */
    if (bHeaded)
    {
        /* check array length */
        if ((dwPos + (T5_DWORD)(pSrc[1]) + 1L) > (T5_DWORD)wDstSizeof)
            return 0L;
        /* copy */
        T5_MEMCPY (pDst + dwPos, pSrc + 1, pSrc[1] + 1);
        /* return next pos */
        return (dwPos + (T5_DWORD)(pSrc[1]) + 1L);
    }
    /* error - bad arguments */
    return 0;
}

#endif /*T5DEF_STRINGSUPPORTED*/

#endif /*T5DEF_SERIALIZEDATA*/

/****************************************************************************/
/* get system info */

static T5_DWORD _GetSysInfo (T5PTR_DB pDB, T5_DWORD dwID)
{
    T5PTR_DBSTATUS pStatus;
    T5PTR_DBPRIVATE pPrivate;
    T5_DWORD i, nb;

    pStatus = T5GET_DBSTATUS(pDB);
    pPrivate = T5GET_DBPRIVATE(pDB);
    switch (dwID)
    {
    /* 0: trigger cycle time in microsec */
    case 0 : return pStatus->dwTimTrigger;
    /* 1: trigger cycle time in millisec */
    case 1 : return pStatus->dwTimTrigger / 1000L;
    /* 2: last cycle duration in microsec */
    case 2 : return pStatus->dwTimCycle;
    /* 3: last cycle duration in millisec */
    case 3 : return pStatus->dwTimCycle / 1000L;
    /* 4: maximum cycle duration in microsec */
    case 4 : return pStatus->dwTimMax;
    /* 5: maximum cycle duration in millisec */
    case 5 : return pStatus->dwTimMax / 1000L;
    /* 6: cycle begin time stamp in ms */
    case 6 : return pStatus->dwTimStamp;
    /* 7: number of cycle overflows */
    case 7 : return pStatus->dwOverflow;
    /* 8: number of cycles performed */
    case 8 : return pStatus->dwCycleCount;
    /* 9: application version number */
    case 9 : return pStatus->dwAppVersion;
    /* 10: application date stamp */
    case 10: return pStatus->dwAppDateStamp;
    /* 11: application CRC (the code) */
    case 11: return pStatus->dwAppCodeCRC;
    /* 12: application CRC (data map) */
    case 12: return pStatus->dwAppDataCRC;
    /* 13: error flag */
    case 13: return (T5_DWORD)((pStatus->wFlags & T5FLAG_ERROR) != 0);
    /* 14: application heap free size */
    case 14:
        if (pPrivate->pHeap != NULL)
            return T5Heap_GetFreeSpace (pPrivate->pHeap);
        return 0L;
    /* 15: VMDB size */
    case 15: return T5VM_GetTotalDBSize (pDB);
    /* 16: seconds elapsed */
    case 16: return pStatus->dwElapsed;
    /* 17: cycle after an on line change */
    case 17: return ((pStatus->dwFlagsEx & T5FLAGEX_CHGCYC) != 0) ? 1 : 0;
    /* 18: application start in warm mode */
    case 18: return ((pStatus->dwFlagsEx & T5FLAGEX_WARM) != 0) ? 1 : 0;
    /* 19: number of locked variables */
    case 19: return pStatus->dwNbLock;
    /* 20: number of breakpoints */
    case 20:
        nb = 0;
        for (i=0; i<T5MAX_BKP; i++)
        {
            if (pStatus->bkp[i].wStyle != T5BKP_DELETED && pStatus->bkp[i].wStyle != T5BKP_VIEW)
                nb += 1;
        }
        return nb;
    /* 21: big endian */
    case 21 :
#ifdef T5DEF_BIGENDIAN
        return 1L;
#else /*T5DEF_BIGENDIAN*/
        return 0L;
#endif /*T5DEF_BIGENDIAN*/

    /* unknown: return 0 */
    default : break;
    }
    return 0L;
}

/****************************************************************************/
/* MBShift standard function */

static T5_BOOL _MBShift (T5PTR_DB pDB, T5_WORD wType,
                         T5_WORD wBase, T5_WORD wSizeof, T5_DWORD dwPos,
                         T5_DWORD dwNbByte, T5_DWORD dwNbShift,
                         T5_BOOL bRight, T5_BOOL bRotate, T5_BOOL bInBit)
{
    T5_PTBYTE pBuffer;
    T5_BOOL bIn;
    T5_DWORD i;

    /* check type and get base address */
    switch (wType)
    {
    case T5C_SINT :
    case T5C_USINT :
        pBuffer = T5Tools_GetD8Array (pDB, wBase);
        break;
    case T5C_INT :
    case T5C_UINT :
#ifdef T5DEF_DATA16SUPPORTED
        pBuffer = (T5_PTBYTE)T5Tools_GetD16Array (pDB, wBase);
        wSizeof *= sizeof (T5_WORD);
        dwPos *= sizeof (T5_WORD);
        break;
#else /*T5DEF_DATA16SUPPORTED*/
        return FALSE;
#endif /*T5DEF_DATA16SUPPORTED*/

    case T5C_DINT  :
    case T5C_UDINT :
        pBuffer = (T5_PTBYTE)T5Tools_GetD32Array (pDB, wBase);
        wSizeof *= sizeof (T5_DWORD);
        dwPos *= sizeof (T5_DWORD);
        break;
    case T5C_LINT :
    case T5C_ULINT :
#ifdef T5DEF_DATA64SUPPORTED
        pBuffer = (T5_PTBYTE)T5Tools_GetD64Array (pDB, wBase);
        wSizeof *= sizeof (T5_DATA64);
        dwPos *= sizeof (T5_DATA64);
        break;
#else /*T5DEF_DATA64SUPPORTED*/
        return FALSE;
#endif /*T5DEF_DATA64SUPPORTED*/
    default :
        return FALSE;
    }
    /* check number of bits and offset buffer*/
    if (dwNbByte == 0 || (dwNbByte + dwPos) > (T5_DWORD)wSizeof)
        return FALSE;
    pBuffer += dwPos;
    /* check nb shift */
    if (dwNbShift > 255)
        return FALSE;
    /* shift */
    while (dwNbShift--)
    {
        if (bRight)
        {
            if (bRotate)
                bInBit = ((pBuffer[dwNbByte-1] & 0x01) != 0) ? TRUE : FALSE;
            for (i=dwNbByte-1; (T5_LONG)i>=0; i--)
            {
                pBuffer[i] >>= 1;
                if (i == 0)
                    bIn = bInBit;
                else
                    bIn = ((pBuffer[i-1] & 0x01) != 0) ? TRUE : FALSE;
                if (bIn)
                    pBuffer[i] |= 0x80;
                else
                    pBuffer[i] &= 0x7f;
            }
        }
        else
        {
            if (bRotate)
                bInBit = ((pBuffer[0] & 0x80) != 0) ? TRUE : FALSE;
            for (i=0; i<dwNbByte; i++)
            {
                pBuffer[i] <<= 1;
                if (i == (dwNbByte - 1))
                    bIn = bInBit;
                else
                    bIn = ((pBuffer[i+1] & 0x80) != 0) ? TRUE : FALSE;
                if (bIn)
                    pBuffer[i] |= 0x01;
                else
                    pBuffer[i] &= 0xfe;
            }
        }
    }
    return TRUE;
}

/****************************************************************************/
/* embedded recipes */

#ifdef T5DEF_RECIPES

static T5_BOOL _ApplyRcpColumn (T5PTR_DB pDB, T5_LONG lRcp, T5_LONG lCol)
{
    T5PTR_DBPRIVATE pPrivate;
    T5PTR_CODEHEADER pHeader;
    T5PTR_CODERCPL pRcpl;
    T5_PTDWORD pRcpOffset, pVarOffset;
    T5PTR_CODERCP pRcp;
    T5PTR_CODERCPV pRcpVar;
    T5_WORD iVar;
    T5_PTBYTE pValue;

    if (lRcp < 0 || lCol < 0)
        return FALSE;

    pPrivate = T5GET_DBPRIVATE (pDB);
    pHeader = (T5PTR_CODEHEADER)(pPrivate->pCode);
    pRcpl = T5VMCode_GetRecipes (pPrivate->pCode);
    if (pRcpl == NULL || lRcp >= (T5_LONG)(pRcpl->wCount))
        return FALSE;

    pRcpOffset = &(pRcpl->dwRcp1);
    pRcpOffset += lRcp;
    pRcp = (T5PTR_CODERCP)T5_OFF(pHeader, *pRcpOffset);
    if (lCol >= (T5_LONG)pRcp->wNbCol)
        return FALSE;

    pVarOffset = &(pRcp->dwVar1);
    for (iVar=0; iVar<pRcp->wNbVar; iVar++, pVarOffset++)
    {
        pRcpVar = (T5PTR_CODERCPV)T5_OFF(pHeader, *pVarOffset);
        pValue = pRcpVar->bValue;
        _ApplyRcpCell (pDB, pRcpVar, (T5_WORD)lCol, pValue);
    }
    return TRUE;
}

static void _ApplyRcpCell (T5PTR_DB pDB, T5PTR_CODERCPV pCell,
                           T5_WORD wCol, T5_PTBYTE pValue)
{
    T5_PTBYTE pVar;
#ifdef T5DEF_DONTFORCELOCKEDVARS
    T5_PTBYTE pLock;
#endif /*T5DEF_DONTFORCELOCKEDVARS*/
#ifdef T5DEF_XV
    T5_PTBYTE *pXV;
#endif /*T5DEF_XV*/

    /* check type - get pointer to the variable */
    pVar = NULL;
    if (pCell->wType & T5C_EXTERN)
    {
#ifdef T5DEF_XV
        pXV = T5GET_DBXV(pDB);
        pVar = (T5_PTBYTE)(pXV[pCell->wOffset]);
#endif /*T5DEF_XV*/
    }
    else if (pCell->dwCTOffset == 0)
    {
        switch (pCell->wType & ~T5C_EXTERN)
        {
        case T5C_BOOL :
        case T5C_SINT :
        case T5C_USINT :
            pVar = T5GET_DBDATA8(pDB);
            pVar += pCell->wOffset;
            break;
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT :
        case T5C_UINT :
            pVar = (T5_PTBYTE)(T5GET_DBDATA16(pDB) + pCell->wOffset);
            break;
#endif /*T5DEF_DATA16SUPPORTED*/
        case T5C_DINT :
        case T5C_UDINT :
        case T5C_REAL :
            pVar = (T5_PTBYTE)(T5GET_DBDATA32(pDB) + pCell->wOffset);
            break;
#ifdef T5DEF_DATA64SUPPORTED
        case T5C_LINT :
        case T5C_ULINT :
        case T5C_LREAL :
            pVar = (T5_PTBYTE)(T5GET_DBDATA64(pDB) + pCell->wOffset);
            break;
#endif /*T5DEF_DATA64SUPPORTED*/
        case T5C_TIME :
            pVar = (T5_PTBYTE)(T5GET_DBTIME(pDB) + pCell->wOffset);
            break;
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING :
            pVar = *(T5GET_DBSTRING(pDB) + pCell->wOffset);
            break;
#endif /*T5DEF_STRINGSUPPORTED*/
        default : break;
        }
    }
#ifdef T5DEF_CTSEG
    else
    {
        pVar = (T5_PTBYTE)(pDB[T5TB_CTSEG].pData);
        if (pVar != NULL)
            pVar += pCell->dwCTOffset;
    }
#endif /*T5DEF_CTSEG*/

    if (pVar == NULL)
        return;

#ifdef T5DEF_DONTFORCELOCKEDVARS
    /* check lock flag */
    if (pCell->dwCTOffset == 0)
    {
        pLock = T5GET_DBLOCK(pDB);
        pLock += pCell->wOffset;
        switch (pCell->wType)
        {
        case T5C_BOOL :
        case T5C_SINT :
        case T5C_USINT :
            if (*pLock & T5LOCK_DATA8)
                return;
            break;
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT :
        case T5C_UINT :
            if (*pLock & T5LOCK_DATA16)
                return;
            break;
#endif /*T5DEF_DATA16SUPPORTED*/
        case T5C_DINT :
        case T5C_UDINT :
        case T5C_REAL :
            if (*pLock & T5LOCK_DATA32)
                return;
            break;
#ifdef T5DEF_DATA64SUPPORTED
        case T5C_LINT :
        case T5C_ULINT :
        case T5C_LREAL :
            if (*pLock & T5LOCK_DATA64)
                return;
            break;
#endif /*T5DEF_DATA64SUPPORTED*/
        case T5C_TIME :
            if (*pLock & T5LOCK_TIME)
                return;
            break;
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING :
            if (*pLock & T5LOCK_STRING)
                return;
            break;
#endif /*T5DEF_STRINGSUPPORTED*/
        default : break;
        }
    }
#endif /*T5DEF_DONTFORCELOCKEDVARS*/

    /* reach column and copy */
    switch (pCell->wType & ~T5C_EXTERN)
    {
    case T5C_BOOL :
    case T5C_SINT :
    case T5C_USINT :
        pValue += wCol;
        *pVar = *pValue;
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
    case T5C_UINT :
        pValue += (wCol * sizeof (T5_WORD));
        T5_MEMCPY (pVar, pValue, sizeof (T5_WORD));
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_UDINT :
    case T5C_REAL :
    case T5C_TIME :
        pValue += (wCol * sizeof (T5_DWORD));
        T5_MEMCPY (pVar, pValue, sizeof (T5_DWORD));
        break;
#ifdef T5DEF_DATA64SUPPORTED
    case T5C_LINT :
    case T5C_ULINT :
    case T5C_LREAL :
        pValue += (wCol * sizeof (T5_DATA64));
        T5_MEMCPY (pVar, pValue, sizeof (T5_DATA64));
        break;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
    case T5C_STRING :
        while (wCol--)
            pValue += (*pValue + 1);
        pVar[1] = pValue[0];
        if (pVar[1] > pVar[0])
            pVar[1] = pVar[0];
        T5_MEMCPY(pVar+2, pValue+1, pVar[1]);
        pVar[pVar[1] + 2] = '\0';
        break;
#endif /*T5DEF_STRINGSUPPORTED*/
    default : break;
    }
}

#endif /*T5DEF_RECIPES*/

/****************************************************************************/
/* test / set bit */

static void _SetBit (T5PTR_DB pDB, T5_WORD wType, T5_WORD wSrc,
                     T5_WORD wDst, T5_DWORD dwBitNo, T5_BYTE bVal)
{
    T5_PTBYTE pData8;
    T5_BYTE bMask;
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pData16;
    T5_WORD wMask;
#endif /*T5DEF_DATA16SUPPORTED*/
    T5_PTDWORD pData32;
    T5_DWORD dwMask;
#ifdef T5DEF_DATA64SUPPORTED
    T5_PTDATA64 pData64;
#endif /*T5DEF_DATA64SUPPORTED*/

    switch (wType)
    {
    case T5C_BOOL :
    case T5C_SINT :
    case T5C_USINT :
        pData8 = T5GET_DBDATA8(pDB);
        if (dwBitNo < 8)
        {
            bMask = (T5_BYTE)(1 << (T5_WORD)dwBitNo);
            if (bVal)
                pData8[wDst] = pData8[wSrc] | bMask;
            else
                pData8[wDst] = pData8[wSrc] & ~bMask;
        }
        else
            pData8[wDst] = pData8[wSrc];
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
    case T5C_UINT :
        pData16 = T5GET_DBDATA16(pDB);
        if (dwBitNo < 16)
        {
            wMask = (T5_WORD)(1 << (T5_WORD)dwBitNo);
            if (bVal)
                pData16[wDst] = pData16[wSrc] | wMask;
            else
                pData16[wDst] = pData16[wSrc] & ~wMask;
        }
        else
            pData16[wDst] = pData16[wSrc];
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_UDINT :
        pData32 = T5GET_DBDATA32(pDB);
        if (dwBitNo < 32)
        {
            dwMask = (T5_DWORD)(1UL << dwBitNo);
            if (bVal)
                pData32[wDst] = pData32[wSrc] | dwMask;
            else
                pData32[wDst] = pData32[wSrc] & ~dwMask;
        }
        else
            pData32[wDst] = pData32[wSrc];
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        pData32 = T5GET_DBDATA32(pDB);
        pData32[wDst] = pData32[wSrc];
        break;
#endif /*T5DEF_REALSUPPORTED*/
    case T5C_TIME :
        pData32 = T5GET_DBTIME(pDB);
        pData32[wDst] = pData32[wSrc];
        break;
#ifdef T5DEF_DATA64SUPPORTED
    case T5C_LINT :
    case T5C_ULINT :
    case T5C_LREAL :
        pData64 = T5GET_DBDATA64(pDB);
        pData64[wDst] = pData64[wSrc];
        break;
#endif /*T5DEF_DATA64SUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
    case T5C_STRING :
        T5VMStr_Copy (pDB, wDst, wSrc);
        break;
#endif /*T5DEF_STRINGSUPPORTED*/
    default : break;
    }
}

static T5_BOOL _GetBit (T5PTR_DB pDB, T5_WORD wType,
                        T5_WORD wSrc, T5_DWORD dwBitNo)
{
    T5_BOOL bRet;
    T5_PTBYTE pData8;
    T5_BYTE bMask;
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTWORD pData16;
    T5_WORD wMask;
#endif /*T5DEF_DATA16SUPPORTED*/
    T5_PTDWORD pData32;
    T5_DWORD dwMask;

    bRet = FALSE;
    switch (wType)
    {
    case T5C_BOOL :
    case T5C_SINT :
    case T5C_USINT :
        pData8 = T5GET_DBDATA8(pDB);
        if (dwBitNo < 8)
        {
            bMask = (T5_BYTE)(1 << (T5_WORD)dwBitNo);
            if (pData8[wSrc] & bMask)
                bRet = TRUE;
        }
        break;
#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
    case T5C_UINT :
        pData16 = T5GET_DBDATA16(pDB);
        if (dwBitNo < 16)
        {
            wMask = (T5_WORD)(1 << (T5_WORD)dwBitNo);
            if (pData16[wSrc] & wMask)
                bRet = TRUE;
        }
        break;
#endif /*T5DEF_DATA16SUPPORTED*/
    case T5C_DINT :
    case T5C_UDINT :
        pData32 = T5GET_DBDATA32(pDB);
        if (dwBitNo < 32)
        {
            dwMask = (T5_DWORD)(1UL << dwBitNo);
            if (pData32[wSrc] & dwMask)
                bRet = TRUE;
        }
        break;
    default : break;
    }
    return bRet;
}

/* string tables ************************************************************/

#ifdef T5DEF_STRINGSUPPORTED
#ifdef T5DEF_STRINGTABLE

T5_BOOL _StringTable (T5PTR_DB pDB, T5_DWORD dwIndex)
{
    T5PTR_DBPRIVATE pPrivate;
    T5_PTR pTable;

    pPrivate = T5GET_DBPRIVATE (pDB);
    pTable = T5VMCode_GetStringTable (pPrivate->pCode, dwIndex);
    if (pTable == NULL)
        return FALSE;

    pPrivate->dwStrTable = dwIndex;
    pPrivate->pStrTable = pTable;
    return TRUE;
}

void _LoadString (T5PTR_DB pDB, T5_WORD wDst, T5_DWORD dwIndex)
{
    T5PTR_DBPRIVATE pPrivate;
    T5_PTDWORD pdwTable;
    T5_PTBYTE pString;

    pPrivate = T5GET_DBPRIVATE (pDB);
    pdwTable = (T5_PTDWORD)(pPrivate->pStrTable);
    if (pdwTable == NULL || dwIndex < pdwTable[0] || dwIndex > pdwTable[1])
        pString = NULL;
    else
    {
        dwIndex -= pdwTable[0];
        pdwTable += 2; /* skip min-max */
        pdwTable += dwIndex;
        if (*pdwTable == 0L)
            pString = NULL;
        else
        {
            pString = (T5_PTBYTE)(pPrivate->pStrTable);
            pString += *pdwTable;
        }
    }

    if (pString == NULL)
        T5VMStr_Force (pDB, wDst, (T5_PTBYTE)("\0"));
    else
        T5VMStr_Force (pDB, wDst, pString);
}

#endif /*T5DEF_STRINGTABLE*/
#endif /*T5DEF_STRINGSUPPORTED*/

/* signals ******************************************************************/

#ifdef T5DEF_SIGNAL
#ifdef T5DEF_REALSUPPORTED

static T5_REAL _SigScale (T5PTR_DB pDB, T5_DWORD dwID, T5_WORD wTime)
{
    T5_PTDWORD pTime;
    T5_PTR pSignal;
    T5_DWORD dwTime;

    pSignal = T5stfb_GetSignal (pDB, dwID, &dwTime);
    if (pSignal == NULL)
        return T5_ZEROREAL;

    pTime = T5GET_DBTIME(pDB);
    return T5stfb_GetSigVal (pSignal, pTime[wTime]);
}

#endif /*T5DEF_REALSUPPORTED*/
#endif /*T5DEF_SIGNAL*/

/* UDP **********************************************************************/

#ifdef T5DEF_UDP
#ifdef T5DEF_STRINGSUPPORTED
#ifdef T5DEF_ETHERNET

static T5_DWORD _Udp_Create (T5PTR_DB pDB, T5_DWORD dwPort)
{
    T5PTR_DBSYSRSC pRsc;
    T5_SOCKET sock;
    T5_DWORD dwSock;

    pRsc = T5VMRsc_GetFreeItem (pDB);
    if (T5VMRsc_GetFreeItem (pDB) == T5_PTNULL)
        return (T5_DWORD)T5_INVSOCKET;

    if (T5Socket_UdpCreate ((T5_WORD)dwPort, &sock) != T5RET_OK)
        return (T5_DWORD)T5_INVSOCKET;

    dwSock = (T5_DWORD)sock;
    pRsc->dwObject = dwSock;
    pRsc->wType = T5SYSRSC_SOCKET;
    pRsc->wUsed = 1;
    return dwSock;
}

static T5_BOOL _Udp_AddrMake (T5PTR_DB pDB, T5_WORD wStr,
                              T5_DWORD dwPort, T5_WORD pAdd, T5_WORD wSizeof)
{
    T5_PTBYTE pUAdd;

    pUAdd = T5Tools_GetD8Array (pDB, pAdd);
    if (wSizeof < sizeof (T5_UDPADDR))
    {
        T5_PRINTF ("UDP address must be USINT[%u]", sizeof (T5_UDPADDR));
        return FALSE;
    }

    T5Socket_UdpAddrMake (pUAdd, T5VMStr_GetText (pDB, wStr), (T5_WORD)dwPort);
    return TRUE;
}

static T5_BOOL _Udp_SendTo (T5PTR_DB pDB, T5_DWORD dwSock, T5_DWORD dwNb,
                            T5_WORD pAdd, T5_WORD wSizeof, T5_WORD wStr)
{
    T5_PTBYTE pUAdd;
    T5_RET ret;
    T5_PTBYTE *pStr, pS;

    pUAdd = T5Tools_GetD8Array (pDB, pAdd);
    if (wSizeof < sizeof (T5_UDPADDR))
    {
        T5_PRINTF ("UDP address must be USINT[%u]", sizeof (T5_UDPADDR));
        return 0L;
    }

    if (T5VMRsc_FindSocket (pDB, dwSock) == NULL
        || dwSock == (T5_DWORD)T5_INVSOCKET)
        return 0L;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wStr];
    if (dwNb > (T5_DWORD)(pS[1]))
        dwNb = (T5_DWORD)(pS[1]);

    ret = T5Socket_UdpSendTo ((T5_SOCKET)dwSock, pUAdd, pS+2, (T5_WORD)dwNb);
    return (ret == T5RET_OK);
}

static T5_BOOL _Udp_SendToArr (T5PTR_DB pDB, T5_DWORD dwSock, T5_DWORD dwNb,
                               T5_WORD pAdd, T5_WORD wSizeof, T5_WORD pAddArray, T5_WORD wArraySize)
{
    T5_PTBYTE pUAdd, pArr;
    T5_RET ret;

    pUAdd = T5Tools_GetD8Array (pDB, pAdd);
    pArr = T5Tools_GetD8Array (pDB, pAddArray);
    if (wSizeof < sizeof (T5_UDPADDR))
    {
        T5_PRINTF ("UDP address must be USINT[%u]", sizeof (T5_UDPADDR));
        return 0L;
    }

    if (T5VMRsc_FindSocket (pDB, dwSock) == NULL
        || dwSock == (T5_DWORD)T5_INVSOCKET)
        return 0L;

    if (dwNb > (T5_DWORD)wArraySize)
        dwNb = (T5_DWORD)wArraySize;

    ret = T5Socket_UdpSendTo ((T5_SOCKET)dwSock, pUAdd, pArr, (T5_WORD)dwNb);
    return (ret == T5RET_OK);
}

static T5_DWORD _Udp_RcvFrom (T5PTR_DB pDB, T5_DWORD dwSock, T5_DWORD dwNb,
                              T5_WORD pAdd, T5_WORD wSizeof, T5_WORD wStr)
{
    T5_PTBYTE pUAdd;
    T5_PTBYTE *pStr, pS;
    T5_WORD wRet;

    pUAdd = T5Tools_GetD8Array (pDB, pAdd);
    if (wSizeof < sizeof (T5_UDPADDR))
    {
        T5_PRINTF ("UDP address must be USINT[%u]", sizeof (T5_UDPADDR));
        return 0L;
    }

    if (T5VMRsc_FindSocket (pDB, dwSock) == NULL
        || dwSock == (T5_DWORD)T5_INVSOCKET)
        return 0L;

    pStr = T5GET_DBSTRING(pDB);
    pS = pStr[wStr];
    if (dwNb > (T5_DWORD)(pS[0]))
        dwNb = (T5_DWORD)(pS[0]);

    wRet = T5Socket_UdpRecvFrom ((T5_SOCKET)dwSock, pS+2, (T5_WORD)dwNb, pUAdd);
    if (wRet > (T5_WORD)(pS[0]))
        wRet = (T5_WORD)(pS[0]);
    pS[1] = (T5_BYTE)wRet;
    pS[pS[1]+2] = 0;

    return wRet;
}

static T5_DWORD _Udp_RcvFromArr (T5PTR_DB pDB, T5_DWORD dwSock, T5_DWORD dwNb,
                                 T5_WORD pAdd, T5_WORD wSizeof, T5_WORD pAddArray, T5_WORD wArraySize)
{
    T5_PTBYTE pUAdd, pArr;
    T5_WORD wRet;

    pUAdd = T5Tools_GetD8Array (pDB, pAdd);
    pArr = T5Tools_GetD8Array (pDB, pAddArray);
    if (wSizeof < sizeof (T5_UDPADDR))
    {
        T5_PRINTF ("UDP address must be USINT[%u]", sizeof (T5_UDPADDR));
        return 0L;
    }

    if (T5VMRsc_FindSocket (pDB, dwSock) == NULL
        || dwSock == (T5_DWORD)T5_INVSOCKET)
        return 0L;

    if (dwNb > (T5_DWORD)wArraySize)
        dwNb = (T5_DWORD)wArraySize;

    wRet = T5Socket_UdpRecvFrom ((T5_SOCKET)dwSock, pArr, (T5_WORD)dwNb, pUAdd);
    return wRet;
}


#endif /*T5DEF_ETHERNET*/
#endif /*T5DEF_STRINGSUPPORTED*/
#endif /*T5DEF_UDP*/

/* registry parameters ******************************************************/

#ifdef T5DEF_REG
#ifdef T5DEF_STRINGSUPPORTED

static void _RegParGet (T5PTR_DB pDB, T5_WORD wType, T5_PTCHAR szPath,
                        T5_WORD wDst, T5_WORD wSrc)
{
    T5_PTR pReg;
    T5PTR_T5REGITEM pItem;
    T5_PTR pSrc, pDst;
    T5_NUMBUFFER buffer;
    T5_BOOL bLock;
    T5_WORD wSizeof;

    pReg = T5VM_GetReg (pDB);
    if (pReg == NULL)
        pItem = NULL;
    else
        pItem = T5Reg_FindItem (pReg, szPath);

    if (pItem == NULL || ((wType == T5C_STRING) != (pItem->bTicType == T5C_STRING)))
    {
        if (wType == T5C_STRING)
            T5VMStr_Copy (pDB, wDst, wSrc);
        else
        {
            pSrc = T5Tools_GetAnyParam (pDB, wType, wSrc, NULL, NULL);
            pDst = T5Tools_GetAnyParam (pDB, wType, wDst, &bLock, &wSizeof);
            if (!bLock)
                T5_MEMCPY (pDst, pSrc, wSizeof);
        }
    }
    else
    {
        if (wType == T5C_STRING)
            T5VMStr_AscForceC (pDB, wDst, (T5_PTCHAR)T5Reg_GetString (pReg, pItem));
        else
        {
            T5Reg_GetValue (pReg, pItem, &buffer, 8);
            pDst = T5Tools_GetAnyParam (pDB, wType, wDst, &bLock, &wSizeof);
            T5VMCnv_ConvBin (pDst, buffer.b, (T5_BYTE)wType, pItem->bTicType);
        }
    }
}

static T5_BOOL _RegParPut (T5PTR_DB pDB, T5_WORD wType, T5_PTCHAR szPath,
                           T5_WORD wSrc)
{
    T5_PTR pReg;
    T5PTR_T5REGITEM pItem;
    T5_PTR pSrc;
    T5_NUMBUFFER buffer;

    pReg = T5VM_GetReg (pDB);
    if (pReg == NULL)
        return FALSE;
    pItem = T5Reg_FindItem (pReg, szPath);
    if (pItem == NULL || ((wType == T5C_STRING) != (pItem->bTicType == T5C_STRING)))
        return FALSE;

    if (wType == T5C_STRING)
        T5Reg_PutStringA (pReg, pItem, (T5_PTBYTE)T5VMStr_GetText (pDB, wSrc));
    else
    {
        pSrc = T5Tools_GetAnyParam (pDB, wType, wSrc, NULL, NULL);
        T5VMCnv_ConvBin (buffer.b, pSrc, pItem->bTicType, (T5_BYTE)wType);
        T5Reg_PutValueA (pReg, pItem, &buffer);
    }
    return TRUE;
}

#endif /*T5DEF_STRINGSUPPORTED*/
#endif /*T5DEF_REG*/

/* set CSV formatting options ***********************************************/

static T5_BOOL _SetCsvOpt (T5PTR_DB pDB, T5_PTCHAR szSep, T5_PTCHAR szDec)
{
    T5PTR_DBSTATUS pStatus;

    if (T5_STRLEN(szSep) != 1 || T5_STRLEN(szDec) != 1)
        return FALSE;

    pStatus = T5GET_DBSTATUS(pDB);
    pStatus->cCsvSep = *szSep;
    pStatus->cCsvDec = *szDec;
    return TRUE;
}

/* eof **********************************************************************/
