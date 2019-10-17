/*****************************************************************************
T5BArray.h : block collection - arrays - definitions

DO NOT ALTER THIS !

(c) COPALP 2011
*****************************************************************************/

#ifdef T5DEF_BARRAY

#define _T5BARRAYDECLAREHANDLE(f) \
    T5_DWORD f (T5_WORD wCommand, T5PTR_DB pBase, \
    T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

_T5BARRAYDECLAREHANDLE(ARCOUNT)
_T5BARRAYDECLAREHANDLE(ARFIND)
_T5BARRAYDECLAREHANDLE(ARFILL)
_T5BARRAYDECLAREHANDLE(ARMIN)
_T5BARRAYDECLAREHANDLE(ARMAX)
_T5BARRAYDECLAREHANDLE(ARSORT)

#define T5BARRAY_FBLIST_STATIC \
    { "ARCOUNT", ARCOUNT }, \
    { "ARFIND", ARFIND }, \
    { "ARFILL", ARFILL }, \
    { "ARMIN", ARMIN }, \
    { "ARMAX", ARMAX }, \
    { "ARSORT", ARSORT },

#endif /*T5DEF_BARRAY*/

/* eof **********************************************************************/
