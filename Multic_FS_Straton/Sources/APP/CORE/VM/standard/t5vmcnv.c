/*****************************************************************************
T5VMcnv.c :  type conversion functions (TIC)

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/
/* access to 8 bit data */

#define GET_D8(i)           (pData8[i])
#define SET_D8(i,b)         { pData8[i] = b; }

#define GET_SINT(i)         (pSint[i])
#define SET_SINT(i,b)       { pSint[i] = b; }

#define GET_USINT(i)        (pData8[i])
#define SET_USINT(i,b)      { pData8[i] = b; }

/****************************************************************************/
/* access to 16 bit data */

#define GET_D16(i)          (pData16[i])
#define SET_D16(i,w)        { pData16[i] = w; }

#define GET_UINT(i)         (puData16[i])
#define SET_UINT(i,w)       { puData16[i] = w; }

/****************************************************************************/
/* access to 32 bit data */

#define GET_DINT(i)         (pDint[i])
#define SET_DINT(i,d)       { pDint[i] = d; }

#define GET_UDINT(i)        (puDint[i])
#define SET_UDINT(i,d)      { puDint[i] = d; }

#ifdef T5DEF_REALSUPPORTED
#define GET_REAL(i)         (pReal[i])
#define SET_REAL(i,r)       { pReal[i] = r; }
#endif /*T5DEF_REALSUPPORTED*/

/****************************************************************************/
/* access to TIME data */

#define GET_TIME(i)         (pTime[i])
#define SET_TIME(i,d)       { pTime[i] = d; }

/****************************************************************************/
/* access to 64 bit data */

#ifdef T5DEF_LREALSUPPORTED
#define GET_LREAL(i)        (pLReal[i])
#define SET_LREAL(i,r)      { pLReal[i] = r; }
#endif /*T5DEF_LREALSUPPORTED*/

#ifdef T5DEF_LINTSUPPORTED
#define GET_LINT(i)         (pLInt[i])
#define SET_LINT(i,r)       { pLInt[i] = r; }
#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_ULINTSUPPORTED
#define GET_ULINT(i)         (puLInt[i])
#define SET_ULINT(i,r)       { puLInt[i] = r; }
#endif /*T5DEF_ULINTSUPPORTED*/

/****************************************************************************/
/* preparation of data pointers */

#define _P5_BOO     pData8 = T5GET_DBDATA8(pDB);
#define _P5_SINT    pSint = (T5_PTCHAR)T5GET_DBDATA8(pDB);
#define _P5_D16     pData16 = (T5_PTSHORT)T5GET_DBDATA16(pDB);
#define _P5_DINT    pDint = (T5_PTLONG)T5GET_DBDATA32(pDB);
#define _P5_LINT    pLInt = (T5_PTLINT)T5GET_DBDATA64(pDB);
#define _P5_ULINT   puLInt = (T5_PTLWORD)T5GET_DBDATA64(pDB);
#define _P5_REAL    pReal = (T5_PTREAL)T5GET_DBDATA32(pDB);
#define _P5_LREAL   pLReal = (T5_PTLREAL)T5GET_DBDATA64(pDB);
#define _P5_TIME    pTime = T5GET_DBTIME(pDB);
#define _P5_STRING  /* no pointer needed */
#define _P5_USINT   pData8 = T5GET_DBDATA8(pDB);
#define _P5_UINT    puData16 = (T5_PTWORD)T5GET_DBDATA16(pDB);
#define _P5_UDINT   puDint = (T5_PTDWORD)T5GET_DBDATA32(pDB);

/****************************************************************************/
/* casts */

#define CONV_BOOTOBOO(d,s)      _P5_BOO \
                                SET_D8((d), GET_D8(s))
#define CONV_BOOTOSINT(d,s)     _P5_BOO _P5_SINT \
                                SET_SINT((d), (T5_CHAR)GET_D8(s))
#define CONV_BOOTOD16(d,s)      _P5_BOO _P5_D16 \
                                SET_D16((d), (T5_SHORT)GET_D8(s))
#define CONV_BOOTODINT(d,s)     _P5_BOO _P5_DINT \
                                SET_DINT((d), (GET_D8(s)) ? 1L : 0L)
#define CONV_BOOTOLINT(d,s)     _P5_BOO _P5_LINT \
                                SET_LINT((d), (T5_LINT)GET_D8(s))
#define CONV_BOOTOULINT(d,s)    _P5_BOO _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)GET_D8(s))
#define CONV_BOOTOREAL(d,s)     _P5_BOO _P5_REAL \
                                SET_REAL((d), (T5_REAL)((GET_D8(s)) ? 1 : 0))
#define CONV_BOOTOLREAL(d,s)    _P5_BOO _P5_LREAL \
                                SET_LREAL((d), (T5_LREAL)((GET_D8(s))?1:0))
#define CONV_BOOTOTIME(d,s)     _P5_BOO _P5_TIME \
                                SET_TIME((d), (GET_D8(s)) ? 1L : 0L)
#define CONV_BOOTOSTRING(d,s)   _P5_BOO _P5_STRING \
                                T5VMStr_BootoA (pDB, (d), GET_D8(s))
#define CONV_BOOTOUSINT(d,s)    _P5_BOO _P5_USINT \
                                SET_USINT((d), (T5_BYTE)GET_D8(s))
#define CONV_BOOTOUINT(d,s)     _P5_BOO _P5_UINT \
                                SET_UINT((d), (T5_WORD)GET_D8(s))
#define CONV_BOOTOUDINT(d,s)    _P5_BOO _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)GET_D8(s))

#define CONV_SINTTOBOO(d,s)     _P5_SINT _P5_BOO \
                                SET_D8((d), (GET_SINT(s) != 0))
#define CONV_SINTTOSINT(d,s)    _P5_SINT \
                                SET_SINT((d), GET_SINT(s))
#define CONV_SINTTOD16(d,s)     _P5_SINT _P5_D16 \
                                SET_D16((d), (T5_SHORT)GET_SINT(s))
#define CONV_SINTTODINT(d,s)    _P5_SINT _P5_DINT \
                                SET_DINT((d), (T5_LONG)GET_SINT(s))
#define CONV_SINTTOLINT(d,s)    _P5_SINT _P5_LINT \
                                SET_LINT((d), (T5_LINT)GET_SINT(s))
#define CONV_SINTTOULINT(d,s)   _P5_SINT _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)GET_SINT(s))
#define CONV_SINTTOREAL(d,s)    _P5_SINT _P5_REAL \
                                SET_REAL((d), (T5_REAL)GET_SINT(s))
#define CONV_SINTTOLREAL(d,s)   _P5_SINT _P5_LREAL \
                                SET_LREAL((d), (T5_LREAL)GET_SINT(s))
#define CONV_SINTTOTIME(d,s)    _P5_SINT _P5_TIME \
                                SET_TIME((d), (T5_DWORD)GET_SINT(s))
#define CONV_SINTTOSTRING(d,s)  _P5_SINT _P5_STRING \
                                T5VMStr_ItoA (pDB, (d), (T5_LONG)GET_SINT(s))
#define CONV_SINTTOUSINT(d,s)   _P5_SINT _P5_USINT \
                                SET_USINT((d), (T5_BYTE)GET_SINT(s))
#define CONV_SINTTOUINT(d,s)    _P5_SINT _P5_UINT \
                                SET_UINT((d), (T5_WORD)GET_SINT(s))
#define CONV_SINTTOUDINT(d,s)   _P5_SINT _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)GET_SINT(s))

#define CONV_D16TOBOO(d,s)      _P5_D16 _P5_BOO \
                                SET_D8((d), (GET_D16(s) != 0))
#define CONV_D16TOSINT(d,s)     _P5_D16 _P5_SINT \
                                SET_SINT((d), (T5_CHAR)GET_D16(s))
#define CONV_D16TOD16(d,s)      _P5_D16 \
                                SET_D16((d), GET_D16(s))
#define CONV_D16TODINT(d,s)     _P5_D16 _P5_DINT \
                                SET_DINT((d), (T5_LONG)GET_D16(s))
#define CONV_D16TOLINT(d,s)     _P5_D16 _P5_LINT \
                                SET_LINT((d), (T5_LINT)GET_D16(s))
#define CONV_D16TOULINT(d,s)    _P5_D16 _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)GET_D16(s))
#define CONV_D16TOREAL(d,s)     _P5_D16 _P5_REAL \
                                SET_REAL((d), (T5_REAL)GET_D16(s))
#define CONV_D16TOLREAL(d,s)    _P5_D16 _P5_LREAL \
                                SET_LREAL((d), (T5_LREAL)GET_D16(s))
#define CONV_D16TOTIME(d,s)     _P5_D16 _P5_TIME \
                                SET_TIME((d), (T5_DWORD)GET_D16(s))
#define CONV_D16TOSTRING(d,s)   _P5_D16 _P5_STRING \
                                T5VMStr_ItoA (pDB, (d), (T5_LONG)GET_D16(s))
#define CONV_D16TOUSINT(d,s)    _P5_D16 _P5_USINT \
                                SET_USINT((d), (T5_BYTE)GET_D16(s))
#define CONV_D16TOUINT(d,s)     _P5_D16 _P5_UINT \
                                SET_UINT((d), (T5_WORD)GET_D16(s))
#define CONV_D16TOUDINT(d,s)    _P5_D16 _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)GET_D16(s))

#define CONV_DINTTOBOO(d,s)     _P5_DINT _P5_BOO \
                                SET_D8((d), (GET_DINT(s) != 0L))
#define CONV_DINTTOSINT(d,s)    _P5_DINT _P5_SINT \
                                SET_SINT((d), (T5_CHAR)GET_DINT(s))
#define CONV_DINTTOD16(d,s)     _P5_DINT _P5_D16 \
                                SET_D16((d), (T5_SHORT)GET_DINT(s))
#define CONV_DINTTODINT(d,s)    _P5_DINT \
                                SET_DINT((d), GET_DINT(s))
#define CONV_DINTTOLINT(d,s)    _P5_DINT _P5_LINT \
                                SET_LINT((d), (T5_LINT)GET_DINT(s))
#define CONV_DINTTOULINT(d,s)   _P5_DINT _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)GET_DINT(s))
#define CONV_DINTTOREAL(d,s)    _P5_DINT _P5_REAL \
                                SET_REAL((d), (T5_REAL)GET_DINT(s))
#define CONV_DINTTOLREAL(d,s)   _P5_DINT _P5_LREAL \
                                SET_LREAL((d), (T5_LREAL)GET_DINT(s))
#define CONV_DINTTOTIME(d,s)    _P5_DINT _P5_TIME \
                                SET_TIME((d), (T5_DWORD)GET_DINT(s))
#define CONV_DINTTOSTRING(d,s)  _P5_DINT _P5_STRING \
                                T5VMStr_ItoA (pDB, (d), GET_DINT(s))
#define CONV_DINTTOUSINT(d,s)   _P5_DINT _P5_USINT \
                                SET_USINT((d), (T5_BYTE)GET_DINT(s))
#define CONV_DINTTOUINT(d,s)    _P5_DINT _P5_UINT \
                                SET_UINT((d), (T5_WORD)GET_DINT(s))
#define CONV_DINTTOUDINT(d,s)   _P5_DINT _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)GET_DINT(s))

#define CONV_LINTTOBOO(d,s)     _P5_LINT _P5_BOO \
                                SET_D8((d), (GET_LINT(s) != (T5_LINT)0))
#define CONV_LINTTOSINT(d,s)    _P5_LINT _P5_SINT \
                                SET_SINT((d), (T5_CHAR)GET_LINT(s))
#define CONV_LINTTODINT(d,s)    _P5_LINT _P5_DINT \
                                SET_DINT((d), (T5_LONG)GET_LINT(s))
#define CONV_LINTTOTIME(d,s)    _P5_LINT _P5_TIME \
                                SET_TIME((d), (T5_DWORD)GET_LINT(s))
#define CONV_LINTTOD16(d,s)     _P5_LINT _P5_D16 \
                                SET_D16((d), (T5_SHORT)GET_LINT(s))
#define CONV_LINTTOREAL(d,s)    _P5_LINT _P5_REAL \
                                SET_REAL((d), (T5_REAL)GET_LINT(s))
#define CONV_LINTTOLREAL(d,s)   _P5_LINT _P5_LREAL \
                                SET_LREAL((d), (T5_LREAL)GET_LINT(s))
#define CONV_LINTTOSTRING(d,s)  _P5_LINT _P5_STRING \
                                T5VMStr_LINTtoA (pDB, (d), GET_LINT(s))
#define CONV_LINTTOLINT(d,s)    _P5_LINT \
                                SET_LINT((d), GET_LINT(s))
#define CONV_LINTTOULINT(d,s)   _P5_LINT _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)GET_LINT(s))
#define CONV_LINTTOUSINT(d,s)   _P5_LINT _P5_USINT \
                                SET_USINT((d), (T5_BYTE)GET_LINT(s))
#define CONV_LINTTOUINT(d,s)    _P5_LINT _P5_UINT \
                                SET_UINT((d), (T5_WORD)GET_LINT(s))
#define CONV_LINTTOUDINT(d,s)   _P5_LINT _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)GET_LINT(s))

#define CONV_ULINTTOBOO(d,s)    _P5_ULINT _P5_BOO \
                                SET_D8((d), (T5_BYTE)(GET_ULINT(s) != (T5_LWORD)0))
#define CONV_ULINTTOSINT(d,s)   _P5_ULINT _P5_SINT \
                                SET_SINT((d), (T5_CHAR)GET_ULINT(s))
#define CONV_ULINTTODINT(d,s)   _P5_ULINT _P5_DINT \
                                SET_DINT((d), (T5_LONG)GET_ULINT(s))
#define CONV_ULINTTOTIME(d,s)   _P5_ULINT _P5_TIME \
                                SET_TIME((d), (T5_DWORD)GET_ULINT(s))
#define CONV_ULINTTOD16(d,s)    _P5_ULINT _P5_D16 \
                                SET_D16((d), (T5_SHORT)GET_ULINT(s))
#define CONV_ULINTTOREAL(d,s)   _P5_ULINT _P5_REAL \
                                SET_REAL((d), (T5_REAL)GET_ULINT(s))
#define CONV_ULINTTOLREAL(d,s)  _P5_ULINT _P5_LREAL \
                                SET_LREAL((d), (T5_LREAL)GET_ULINT(s))
#define CONV_ULINTTOSTRING(d,s) _P5_ULINT _P5_STRING \
                                T5VMStr_LINTtoA (pDB, (d), GET_ULINT(s)) /*TODO*/
#define CONV_ULINTTOLINT(d,s)   _P5_ULINT _P5_LINT \
                                SET_LINT((d), (T5_LINT)GET_ULINT(s))
#define CONV_ULINTTOULINT(d,s)  _P5_ULINT _P5_LINT \
                                SET_LINT((d), GET_ULINT(s))
#define CONV_ULINTTOUSINT(d,s)  _P5_ULINT _P5_USINT \
                                SET_USINT((d), (T5_BYTE)GET_ULINT(s))
#define CONV_ULINTTOUINT(d,s)   _P5_ULINT _P5_UINT \
                                SET_UINT((d), (T5_WORD)GET_ULINT(s))
#define CONV_ULINTTOUDINT(d,s)  _P5_ULINT _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)GET_ULINT(s))

#define CONV_REALTOBOO(d,s)     _P5_REAL _P5_BOO \
                                SET_D8((d), (GET_REAL(s) != (T5_REAL)0))
#define CONV_REALTOSINT(d,s)    _P5_REAL _P5_SINT \
                                SET_SINT((d), (T5_CHAR)GET_REAL(s))
#define CONV_REALTOD16(d,s)     _P5_REAL _P5_D16 \
                                SET_D16((d), (T5_SHORT)GET_REAL(s))
#define CONV_REALTODINT(d,s)    _P5_REAL _P5_DINT \
                                SET_DINT((d), (T5_LONG)GET_REAL(s))
#define CONV_REALTOLINT(d,s)    _P5_REAL _P5_LINT \
                                SET_LINT((d), (T5_LINT)GET_REAL(s))
#define CONV_REALTOULINT(d,s)   _P5_REAL _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)GET_REAL(s))
#define CONV_REALTOREAL(d,s)    _P5_REAL \
                                SET_REAL((d), GET_REAL(s))
#define CONV_REALTOLREAL(d,s)   _P5_REAL _P5_LREAL \
                                SET_LREAL((d), (T5_LREAL)GET_REAL(s))
#define CONV_REALTOTIME(d,s)    _P5_REAL _P5_TIME \
                                SET_TIME((d), (T5_DWORD)GET_REAL(s))
#define CONV_REALTOSTRING(d,s)  _P5_REAL _P5_STRING \
                                T5VMStr_FtoA (pDB, (d), GET_REAL(s))
#define CONV_REALTOUSINT(d,s)   _P5_REAL _P5_USINT \
                                SET_USINT((d), (T5_BYTE)(T5_LONG)GET_REAL(s))
#define CONV_REALTOUINT(d,s)    _P5_REAL _P5_UINT \
                                SET_UINT((d), (T5_WORD)(T5_LONG)GET_REAL(s))
#define CONV_REALTOUDINT(d,s)   _P5_REAL _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)(T5_LONG)GET_REAL(s))

#define CONV_LREALTOBOO(d,s)    _P5_LREAL _P5_BOO \
                                SET_D8((d), (GET_LREAL(s) != (T5_LREAL)0))
#define CONV_LREALTOSINT(d,s)   _P5_LREAL _P5_SINT \
                                SET_SINT((d), (T5_CHAR)GET_LREAL(s))
#define CONV_LREALTOD16(d,s)    _P5_LREAL _P5_D16 \
                                SET_D16((d), (T5_SHORT)GET_LREAL(s))
#define CONV_LREALTODINT(d,s)   _P5_LREAL _P5_DINT \
                                SET_DINT((d), (T5_LONG)GET_LREAL(s))
#define CONV_LREALTOLINT(d,s)   _P5_LREAL _P5_LINT \
                                SET_LINT((d), (T5_LINT)GET_LREAL(s))
#define CONV_LREALTOULINT(d,s)  _P5_LREAL _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)GET_LREAL(s))
#define CONV_LREALTOLREAL(d,s)  _P5_LREAL \
                                SET_LREAL((d), GET_LREAL(s))
#define CONV_LREALTOREAL(d,s)   _P5_LREAL _P5_REAL \
                                SET_REAL((d), (T5_REAL)GET_LREAL(s))
#define CONV_LREALTOTIME(d,s)   _P5_LREAL _P5_TIME \
                                SET_TIME((d), (T5_DWORD)GET_LREAL(s))
#define CONV_LREALTOSTRING(d,s) _P5_LREAL _P5_STRING \
                                T5VMStr_DtoA (pDB, (d), GET_LREAL(s))
#define CONV_LREALTOUSINT(d,s)  _P5_LREAL _P5_USINT \
                                SET_USINT((d), (T5_BYTE)(T5_LONG)GET_LREAL(s))
#define CONV_LREALTOUINT(d,s)   _P5_LREAL _P5_UINT \
                                SET_UINT((d), (T5_WORD)(T5_LONG)GET_LREAL(s))
#define CONV_LREALTOUDINT(d,s)  _P5_LREAL _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)(T5_LONG)GET_LREAL(s))

#define CONV_TIMETOBOO(d,s)     _P5_TIME _P5_BOO \
                                SET_D8((d), (GET_TIME(s) != 0L))
#define CONV_TIMETOSINT(d,s)    _P5_TIME _P5_SINT \
                                SET_SINT((d), (T5_CHAR)GET_TIME(s))
#define CONV_TIMETOD16(d,s)     _P5_TIME _P5_D16 \
                                SET_D16((d), (T5_SHORT)GET_TIME(s))
#define CONV_TIMETODINT(d,s)    _P5_TIME _P5_DINT \
                                SET_DINT((d), (T5_LONG)GET_TIME(s))
#define CONV_TIMETOLINT(d,s)    _P5_TIME _P5_LINT \
                                SET_LINT((d), (T5_LINT)GET_TIME(s))
#define CONV_TIMETOULINT(d,s)   _P5_TIME _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)GET_TIME(s))
#define CONV_TIMETOREAL(d,s)    _P5_TIME _P5_REAL \
                                SET_REAL((d), (T5_REAL)GET_TIME(s))
#define CONV_TIMETOLREAL(d,s)   _P5_TIME _P5_LREAL \
                                SET_LREAL((d), (T5_LREAL)GET_TIME(s))
#define CONV_TIMETOTIME(d,s)    _P5_TIME \
                                SET_TIME((d), GET_TIME(s))
#define CONV_TIMETOSTRING(d,s)  _P5_TIME _P5_STRING \
                                T5VMStr_TmrtoA (pDB, (d), GET_TIME(s))
#define CONV_TIMETOUSINT(d,s)   _P5_TIME _P5_USINT \
                                SET_USINT((d), (T5_BYTE)GET_TIME(s))
#define CONV_TIMETOUINT(d,s)    _P5_TIME _P5_UINT \
                                SET_UINT((d), (T5_WORD)GET_TIME(s))
#define CONV_TIMETOUDINT(d,s)   _P5_TIME _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)GET_TIME(s))

#define CONV_STRINGTOBOO(d,s)   _P5_STRING _P5_BOO \
                                SET_D8((d), !T5VMStr_IsEmpty (pDB, (s)))
#define CONV_STRINGTOSINT(d,s)  _P5_STRING _P5_SINT \
                                SET_SINT((d), (T5_CHAR)T5VMStr_AtoI(pDB,(s)))
#define CONV_STRINGTOD16(d,s)   _P5_STRING _P5_D16 \
                                SET_D16((d), (T5_SHORT)T5VMStr_AtoI(pDB,(s)))
#define CONV_STRINGTODINT(d,s)  _P5_STRING _P5_DINT \
                                SET_DINT((d), T5VMStr_AtoI(pDB,(s)))
#define CONV_STRINGTOLINT(d,s)  _P5_STRING _P5_LINT \
                                SET_LINT((d), T5VMStr_AtoLINT(pDB,(s)))
#define CONV_STRINGTOULINT(d,s) _P5_STRING _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)T5VMStr_AtoLINT(pDB,(s))) /*TODO*/
#define CONV_STRINGTOREAL(d,s)  _P5_STRING _P5_REAL \
                                SET_REAL((d), T5VMStr_AtoF(pDB,(s)))
#define CONV_STRINGTOLREAL(d,s) _P5_STRING _P5_LREAL \
                                SET_LREAL((d), T5VMStr_AtoD(pDB,(s)))
#define CONV_STRINGTOTIME(d,s)  _P5_STRING _P5_TIME \
                                SET_TIME((d), (T5_DWORD)T5VMStr_AtoI(pDB,(s)))
#define CONV_STRINGTOSTRING(d,s) _P5_STRING \
                                T5VMStr_Copy (pDB, (d), (s))
#define CONV_STRINGTOUSINT(d,s) _P5_STRING _P5_USINT \
                                SET_USINT((d), (T5_BYTE)T5VMStr_AtoI(pDB,(s)))
#define CONV_STRINGTOUINT(d,s)  _P5_STRING _P5_UINT \
                                SET_UINT((d), (T5_WORD)T5VMStr_AtoI(pDB,(s)))
#define CONV_STRINGTOUDINT(d,s) _P5_STRING _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)T5VMStr_AtoUL(pDB,(s)))

#define CONV_USINTTOBOO(d,s)    _P5_USINT _P5_BOO \
                                SET_D8((d), (GET_USINT(s) != 0))
#define CONV_USINTTOSINT(d,s)   _P5_USINT _P5_SINT \
                                SET_SINT((d), (T5_CHAR)GET_USINT(s))
#define CONV_USINTTOD16(d,s)    _P5_USINT _P5_D16 \
                                SET_D16((d), (T5_SHORT)GET_USINT(s))
#define CONV_USINTTODINT(d,s)   _P5_USINT _P5_DINT \
                                SET_DINT((d), (T5_LONG)GET_USINT(s))
#define CONV_USINTTOLINT(d,s)   _P5_USINT _P5_LINT \
                                SET_LINT((d), (T5_LINT)GET_USINT(s))
#define CONV_USINTTOULINT(d,s)  _P5_USINT _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)GET_USINT(s))
#define CONV_USINTTOREAL(d,s)   _P5_USINT _P5_REAL \
                                SET_REAL((d), (T5_REAL)GET_USINT(s))
#define CONV_USINTTOLREAL(d,s)  _P5_USINT _P5_LREAL \
                                SET_LREAL((d), (T5_LREAL)GET_USINT(s))
#define CONV_USINTTOTIME(d,s)   _P5_USINT _P5_TIME \
                                SET_TIME((d), (T5_DWORD)GET_USINT(s))
#define CONV_USINTTOSTRING(d,s) _P5_USINT _P5_STRING \
                                T5VMStr_UtoA (pDB, (d), (T5_DWORD)GET_USINT(s))
#define CONV_USINTTOUSINT(d,s)  _P5_USINT \
                                SET_USINT((d), GET_USINT(s))
#define CONV_USINTTOUINT(d,s)   _P5_USINT _P5_UINT \
                                SET_UINT((d), (T5_WORD)GET_USINT(s))
#define CONV_USINTTOUDINT(d,s)  _P5_USINT _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)GET_USINT(s))

#define CONV_UINTTOBOO(d,s)     _P5_UINT _P5_BOO \
                                SET_D8((d), (GET_UINT(s) != 0))
#define CONV_UINTTOSINT(d,s)    _P5_UINT _P5_SINT \
                                SET_SINT((d), (T5_CHAR)GET_UINT(s))
#define CONV_UINTTOD16(d,s)     _P5_UINT _P5_D16 \
                                SET_D16((d), (T5_SHORT)GET_UINT(s))
#define CONV_UINTTODINT(d,s)    _P5_UINT _P5_DINT \
                                SET_DINT((d), (T5_LONG)GET_UINT(s))
#define CONV_UINTTOLINT(d,s)    _P5_UINT _P5_LINT \
                                SET_LINT((d), (T5_LINT)GET_UINT(s))
#define CONV_UINTTOULINT(d,s)   _P5_UINT _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)GET_UINT(s))
#define CONV_UINTTOREAL(d,s)    _P5_UINT _P5_REAL \
                                SET_REAL((d), (T5_REAL)GET_UINT(s))
#define CONV_UINTTOLREAL(d,s)   _P5_UINT _P5_LREAL \
                                SET_LREAL((d), (T5_LREAL)GET_UINT(s))
#define CONV_UINTTOTIME(d,s)    _P5_UINT _P5_TIME \
                                SET_TIME((d), (T5_DWORD)GET_UINT(s))
#define CONV_UINTTOSTRING(d,s)  _P5_UINT _P5_STRING \
                                T5VMStr_UtoA (pDB, (d), (T5_DWORD)GET_UINT(s))
#define CONV_UINTTOUSINT(d,s)   _P5_UINT _P5_USINT \
                                SET_USINT((d), (T5_BYTE)GET_UINT(s))
#define CONV_UINTTOUINT(d,s)    _P5_UINT \
                                SET_UINT((d), GET_UINT(s))
#define CONV_UINTTOUDINT(d,s)   _P5_UINT _P5_UDINT \
                                SET_UDINT((d), (T5_DWORD)GET_UINT(s))

#define CONV_UDINTTOBOO(d,s)    _P5_UDINT _P5_BOO \
                                SET_D8((d), (GET_UDINT(s) != 0))
#define CONV_UDINTTOSINT(d,s)   _P5_UDINT _P5_SINT \
                                SET_SINT((d), (T5_CHAR)GET_UDINT(s))
#define CONV_UDINTTOD16(d,s)    _P5_UDINT _P5_D16 \
                                SET_D16((d), (T5_SHORT)GET_UDINT(s))
#define CONV_UDINTTODINT(d,s)   _P5_UDINT _P5_DINT \
                                SET_DINT((d), (T5_LONG)GET_UDINT(s))
#define CONV_UDINTTOLINT(d,s)   _P5_UDINT _P5_LINT \
                                SET_LINT((d), (T5_LINT)GET_UDINT(s))
#define CONV_UDINTTOULINT(d,s)  _P5_UDINT _P5_ULINT \
                                SET_ULINT((d), (T5_LWORD)GET_UDINT(s))
#define CONV_UDINTTOREAL(d,s)   _P5_UDINT _P5_REAL \
                                SET_REAL((d), (T5_REAL)GET_UDINT(s))
#define CONV_UDINTTOLREAL(d,s)  _P5_UDINT _P5_LREAL \
                                SET_LREAL((d), (T5_LREAL)GET_UDINT(s))
#define CONV_UDINTTOTIME(d,s)   _P5_UDINT _P5_TIME \
                                SET_TIME((d), (T5_DWORD)GET_UDINT(s))
#define CONV_UDINTTOSTRING(d,s) _P5_UDINT _P5_STRING \
                                T5VMStr_UtoA (pDB, (d), GET_UDINT(s))
#define CONV_UDINTTOUSINT(d,s)  _P5_UDINT _P5_USINT \
                                SET_USINT((d), (T5_BYTE)GET_UDINT(s))
#define CONV_UDINTTOUINT(d,s)   _P5_UDINT _P5_UINT \
                                SET_UINT((d), (T5_WORD)GET_UDINT(s))
#define CONV_UDINTTOUDINT(d,s)  _P5_UDINT \
                                SET_UDINT((d), GET_UDINT(s))

/*****************************************************************************
T5VMCnv_Convert
execute a type conversion instruction
parameters:
    pDB (IN) pointer to the database
    pCode (IN) pointer to the instruction
*****************************************************************************/

void T5VMCnv_Convert (T5PTR_DB pDB, T5_PTWORD pCode)
{
    T5_PTBYTE pData8;
#ifdef T5DEF_SINTSUPPORTED
    T5_PTCHAR pSint;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
    T5_PTSHORT pData16;
    T5_PTWORD puData16;
#endif /*T5DEF_DATA16SUPPORTED*/
    T5_PTLONG pDint;
    T5_PTDWORD puDint = 0;
#ifdef T5DEF_REALSUPPORTED
    T5_PTREAL pReal;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    T5_PTLREAL pLReal;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
    T5_PTLINT pLInt;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    T5_PTLWORD puLInt;
#endif /*T5DEF_ULINTSUPPORTED*/
    T5_PTDWORD pTime;

    switch (pCode[2])
    {
    case T5C_BOOL :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOBOO(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOBOO(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTOBOO(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETOBOO(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTOBOO(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTOBOO(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTOBOO(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TOBOO(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTOBOO(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTOBOO(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTOBOO(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTOBOO(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTOBOO(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
    case T5C_DINT :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTODINT(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTODINT(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTODINT(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETODINT(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTODINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTODINT(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTODINT(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TODINT(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTODINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTODINT(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTODINT(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTODINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTODINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOREAL(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOREAL(*pCode, pCode[1]); break;
        case T5C_REAL  : CONV_REALTOREAL(*pCode, pCode[1]); break;
        case T5C_TIME  : CONV_TIMETOREAL(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTOREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTOREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTOREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TOREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTOREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTOREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTOREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTOREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTOREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_REALSUPPORTED*/
    case T5C_TIME :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOTIME(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOTIME(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTOTIME(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETOTIME(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTOTIME(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTOTIME(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTOTIME(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TOTIME(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTOTIME(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTOTIME(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTOTIME(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTOTIME(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTOTIME(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOLREAL(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOLREAL(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTOLREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETOLREAL(*pCode, pCode[1]); break;
        case T5C_LREAL: CONV_LREALTOLREAL(*pCode, pCode[1]); break;
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTOLREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTOLREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TOLREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTOLREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTOLREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTOLREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTOLREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTOLREAL(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
    case T5C_STRING :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOSTRING(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOSTRING(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTOSTRING(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETOSTRING(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTOSTRING(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
        case T5C_STRING  : CONV_STRINGTOSTRING(*pCode, pCode[1]); break;
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTOSTRING(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TOSTRING(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTOSTRING(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTOSTRING(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTOSTRING(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTOSTRING(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTOSTRING(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_STRINGSUPPORTED*/

#ifdef T5DEF_SINTSUPPORTED
    case T5C_SINT :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOSINT(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOSINT(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTOSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETOSINT(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTOSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTOSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
        case T5C_SINT : CONV_SINTTOSINT(*pCode, pCode[1]); break;
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TOSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTOSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTOSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTOSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTOSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTOSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_SINTSUPPORTED*/

#ifdef T5DEF_DATA16SUPPORTED
    case T5C_INT :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOD16(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOD16(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTOD16(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETOD16(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTOD16(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTOD16(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTOD16(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
        case T5C_INT : CONV_D16TOD16(*pCode, pCode[1]); break;
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTOD16(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTOD16(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTOD16(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTOD16(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTOD16(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_DATA16SUPPORTED*/

#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOLINT(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOLINT(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTOLINT(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETOLINT(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTOLINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTOLINT(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTOLINT(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TOLINT(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
        case T5C_LINT : CONV_LINTTOLINT(*pCode, pCode[1]); break;
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTOLINT(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTOLINT(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTOLINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTOLINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_LINTSUPPORTED*/

#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOULINT(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOULINT(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTOULINT(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETOULINT(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTOULINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTOULINT(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTOULINT(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TOULINT(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTOULINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
        case T5C_ULINT : CONV_ULINTTOULINT(*pCode, pCode[1]); break;
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTOULINT(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTOULINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTOULINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_ULINTSUPPORTED*/

#ifdef T5DEF_USINTSUPPORTED
    case T5C_USINT :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOUSINT(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOUSINT(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTOUSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETOUSINT(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTOUSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTOUSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTOUSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TOUSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTOUSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTOUSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
        case T5C_USINT : CONV_USINTTOUSINT(*pCode, pCode[1]); break;
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTOUSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTOUSINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_USINTSUPPORTED*/

#ifdef T5DEF_UINTSUPPORTED
    case T5C_UINT :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOUINT(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOUINT(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTOUINT(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETOUINT(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTOUINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTOUINT(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTOUINT(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TOUINT(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTOUINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTOUINT(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTOUINT(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
        case T5C_UINT : CONV_UINTTOUINT(*pCode, pCode[1]); break;
#ifdef T5DEF_UDINTSUPPORTED
        case T5C_UDINT : CONV_UDINTTOUINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UDINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_UINTSUPPORTED*/

#ifdef T5DEF_UDINTSUPPORTED
    case T5C_UDINT :
        switch (pCode[3])
        {
        case T5C_BOOL  : CONV_BOOTOUDINT(*pCode, pCode[1]); break;
        case T5C_DINT  : CONV_DINTTOUDINT(*pCode, pCode[1]); break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL  : CONV_REALTOUDINT(*pCode, pCode[1]); break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_TIME  : CONV_TIMETOUDINT(*pCode, pCode[1]); break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL: CONV_LREALTOUDINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_STRINGSUPPORTED
        case T5C_STRING  : CONV_STRINGTOUDINT(*pCode, pCode[1]); break;
#endif /*T5DEF_STRINGSUPPORTED*/
#ifdef T5DEF_SINTSUPPORTED
        case T5C_SINT : CONV_SINTTOUDINT(*pCode, pCode[1]); break;
#endif /*T5DEF_SINTSUPPORTED*/
#ifdef T5DEF_DATA16SUPPORTED
        case T5C_INT : CONV_D16TOUDINT(*pCode, pCode[1]); break;
#endif /*T5DEF_DATA16SUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT : CONV_LINTTOUDINT(*pCode, pCode[1]); break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT : CONV_ULINTTOUDINT(*pCode, pCode[1]); break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_USINTSUPPORTED
        case T5C_USINT : CONV_USINTTOUDINT(*pCode, pCode[1]); break;
#endif /*T5DEF_USINTSUPPORTED*/
#ifdef T5DEF_UINTSUPPORTED
        case T5C_UINT : CONV_UINTTOUDINT(*pCode, pCode[1]); break;
#endif /*T5DEF_UINTSUPPORTED*/
        case T5C_UDINT : CONV_UDINTTOUDINT(*pCode, pCode[1]); break;
        default : break;
        }
        break;
#endif /*T5DEF_UDINTSUPPORTED*/
    default : break;
    }
}

/*****************************************************************************
T5VMCnv_ConvBin
execute a type conversion using binary pointers - STRING not supported!
parameters:
    pDst (IN) pointer to destination buffer
    pSrc (IN) pointer to source buffer
    bDstType (IN) type of destination data
    bSrcType (IN) type of source data
*****************************************************************************/

void T5VMCnv_ConvBin (T5_PTR pDst, T5_PTR pSrc,
                      T5_BYTE bDstType, T5_BYTE bSrcType)
{
    switch (bDstType)
    {
    /* to BOOL *************************************************************/
    case T5C_BOOL :
        switch (bSrcType)
        {
        case T5C_BOOL :
        case T5C_SINT :
        case T5C_USINT :
            *(T5_PTBOOL)pDst = (*(T5_PTBYTE)pSrc != 0) ? 1 : 0;
            break;
        case T5C_INT :
        case T5C_UINT :
            *(T5_PTBOOL)pDst = (*(T5_PTWORD)pSrc != 0) ? 1 : 0;
            break;
        case T5C_DINT :
        case T5C_UDINT :
        case T5C_TIME :
            *(T5_PTBOOL)pDst = (*(T5_PTDWORD)pSrc != 0) ? 1 : 0;
            break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL :
            *(T5_PTBOOL)pDst = (*(T5_PTREAL)pSrc != 0) ? 1 : 0;
            break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL:
            *(T5_PTBOOL)pDst = (*(T5_PTLREAL)pSrc != 0) ? 1 : 0;
            break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT :
            *(T5_PTBOOL)pDst = (*(T5_PTLINT)pSrc != 0) ? 1 : 0;
            break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT :
            *(T5_PTBOOL)pDst = (*(T5_PTLWORD)pSrc != 0) ? 1 : 0;
            break;
#endif /*T5DEF_ULINTSUPPORTED*/
        default : break;
        }
        break;
    /* to SINT *************************************************************/
    case T5C_SINT :
        switch (bSrcType)
        {
        case T5C_BOOL :
        case T5C_SINT :
        case T5C_USINT :
            *(T5_PTCHAR)pDst = *(T5_PTCHAR)pSrc;
            break;
        case T5C_INT :
            *(T5_PTCHAR)pDst = (T5_CHAR)(*(T5_PTSHORT)pSrc);
            break;
        case T5C_UINT :
            *(T5_PTCHAR)pDst = (T5_CHAR)(*(T5_PTWORD)pSrc);
            break;
        case T5C_DINT :
            *(T5_PTCHAR)pDst = (T5_CHAR)(*(T5_PTLONG)pSrc);
            break;
        case T5C_UDINT :
        case T5C_TIME :
            *(T5_PTCHAR)pDst = (T5_CHAR)(*(T5_PTDWORD)pSrc);
            break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL :
            *(T5_PTCHAR)pDst = (T5_CHAR)(*(T5_PTREAL)pSrc);
            break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL:
            *(T5_PTCHAR)pDst = (T5_CHAR)(*(T5_PTLREAL)pSrc);
            break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT :
            *(T5_PTCHAR)pDst = (T5_CHAR)(*(T5_PTLINT)pSrc);
            break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT :
            *(T5_PTCHAR)pDst = (T5_CHAR)(*(T5_PTLWORD)pSrc);
            break;
#endif /*T5DEF_ULINTSUPPORTED*/
        default : break;
        }
        break;
    /* to USINT ************************************************************/
    case T5C_USINT :
        switch (bSrcType)
        {
        case T5C_BOOL :
        case T5C_SINT :
        case T5C_USINT :
            *(T5_PTBYTE)pDst = *(T5_PTBYTE)pSrc;
            break;
        case T5C_INT :
            *(T5_PTBYTE)pDst = (T5_BYTE)(*(T5_PTSHORT)pSrc);
            break;
        case T5C_UINT :
            *(T5_PTBYTE)pDst = (T5_BYTE)(*(T5_PTWORD)pSrc);
            break;
        case T5C_DINT :
            *(T5_PTBYTE)pDst = (T5_BYTE)(*(T5_PTLONG)pSrc);
            break;
        case T5C_UDINT :
        case T5C_TIME :
            *(T5_PTBYTE)pDst = (T5_BYTE)(*(T5_PTDWORD)pSrc);
            break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL :
            *(T5_PTBYTE)pDst = (T5_BYTE)(*(T5_PTREAL)pSrc);
            break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL:
            *(T5_PTBYTE)pDst = (T5_BYTE)(*(T5_PTLREAL)pSrc);
            break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT :
            *(T5_PTBYTE)pDst = (T5_BYTE)(*(T5_PTLINT)pSrc);
            break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT :
            *(T5_PTBYTE)pDst = (T5_BYTE)(*(T5_PTLWORD)pSrc);
            break;
#endif /*T5DEF_ULINTSUPPORTED*/
        default : break;
        }
        break;
    /* to INT **************************************************************/
    case T5C_INT :
        switch (bSrcType)
        {
        case T5C_BOOL :
        case T5C_USINT :
            *(T5_PTSHORT)pDst = (T5_SHORT)(*(T5_PTBYTE)pSrc);
            break;
        case T5C_SINT :
            *(T5_PTSHORT)pDst = (T5_SHORT)(*(T5_PTCHAR)pSrc);
            break;
        case T5C_INT :
        case T5C_UINT :
            *(T5_PTSHORT)pDst = *(T5_PTSHORT)pSrc;
            break;
        case T5C_DINT :
            *(T5_PTSHORT)pDst = (T5_SHORT)(*(T5_PTLONG)pSrc);
            break;
        case T5C_UDINT :
        case T5C_TIME :
            *(T5_PTSHORT)pDst = (T5_SHORT)(*(T5_PTDWORD)pSrc);
            break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL :
            *(T5_PTSHORT)pDst = (T5_SHORT)(*(T5_PTREAL)pSrc);
            break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL:
            *(T5_PTSHORT)pDst = (T5_SHORT)(*(T5_PTLREAL)pSrc);
            break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT :
            *(T5_PTSHORT)pDst = (T5_SHORT)(*(T5_PTLINT)pSrc);
            break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT :
            *(T5_PTSHORT)pDst = (T5_SHORT)(*(T5_PTLWORD)pSrc);
            break;
#endif /*T5DEF_ULINTSUPPORTED*/
        default : break;
        }
        break;
    /* to UINT *************************************************************/
    case T5C_UINT :
        switch (bSrcType)
        {
        case T5C_BOOL :
        case T5C_USINT :
            *(T5_PTWORD)pDst = (T5_WORD)(*(T5_PTBYTE)pSrc);
            break;
        case T5C_SINT :
            *(T5_PTWORD)pDst = (T5_WORD)(*(T5_PTCHAR)pSrc);
            break;
        case T5C_INT :
        case T5C_UINT :
            *(T5_PTWORD)pDst = *(T5_PTWORD)pSrc;
            break;
        case T5C_DINT :
            *(T5_PTWORD)pDst = (T5_WORD)(*(T5_PTLONG)pSrc);
            break;
        case T5C_UDINT :
        case T5C_TIME :
            *(T5_PTWORD)pDst = (T5_WORD)(*(T5_PTDWORD)pSrc);
            break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL :
            *(T5_PTWORD)pDst = (T5_WORD)(*(T5_PTREAL)pSrc);
            break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL:
            *(T5_PTWORD)pDst = (T5_WORD)(*(T5_PTLREAL)pSrc);
            break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT :
            *(T5_PTWORD)pDst = (T5_WORD)(*(T5_PTLINT)pSrc);
            break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT :
            *(T5_PTWORD)pDst = (T5_WORD)(*(T5_PTLWORD)pSrc);
            break;
#endif /*T5DEF_ULINTSUPPORTED*/
        default : break;
        }
        break;
    /* to DINT *************************************************************/
    case T5C_DINT :
        switch (bSrcType)
        {
        case T5C_BOOL :
        case T5C_USINT :
            *(T5_PTLONG)pDst = (T5_LONG)(*(T5_PTBYTE)pSrc);
            break;
        case T5C_SINT :
            *(T5_PTLONG)pDst = (T5_LONG)(*(T5_PTCHAR)pSrc);
            break;
        case T5C_INT :
            *(T5_PTLONG)pDst = (T5_LONG)(*(T5_PTSHORT)pSrc);
            break;
        case T5C_UINT :
            *(T5_PTLONG)pDst = (T5_LONG)(*(T5_PTWORD)pSrc);
            break;
        case T5C_DINT :
        case T5C_UDINT :
        case T5C_TIME :
            *(T5_PTLONG)pDst = *(T5_PTLONG)pSrc;
            break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL :
            *(T5_PTLONG)pDst = (T5_LONG)(*(T5_PTREAL)pSrc);
            break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL:
            *(T5_PTLONG)pDst = (T5_LONG)(*(T5_PTLREAL)pSrc);
            break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT :
            *(T5_PTLONG)pDst = (T5_LONG)(*(T5_PTLINT)pSrc);
            break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT :
            *(T5_PTLONG)pDst = (T5_LONG)(*(T5_PTLWORD)pSrc);
            break;
#endif /*T5DEF_ULINTSUPPORTED*/
        default : break;
        }
        break;
    /* to UDINT or TIME ****************************************************/
    case T5C_UDINT :
    case T5C_TIME :
        switch (bSrcType)
        {
        case T5C_BOOL :
        case T5C_USINT :
            *(T5_PTDWORD)pDst = (T5_DWORD)(*(T5_PTBYTE)pSrc);
            break;
        case T5C_SINT :
            *(T5_PTDWORD)pDst = (T5_DWORD)(*(T5_PTCHAR)pSrc);
            break;
        case T5C_INT :
            *(T5_PTDWORD)pDst = (T5_DWORD)(*(T5_PTSHORT)pSrc);
            break;
        case T5C_UINT :
            *(T5_PTDWORD)pDst = (T5_DWORD)(*(T5_PTWORD)pSrc);
            break;
        case T5C_DINT :
        case T5C_UDINT :
        case T5C_TIME :
            *(T5_PTDWORD)pDst = *(T5_PTDWORD)pSrc;
            break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL :
            *(T5_PTDWORD)pDst = (T5_DWORD)(*(T5_PTREAL)pSrc);
            break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL:
            *(T5_PTDWORD)pDst = (T5_DWORD)(*(T5_PTLREAL)pSrc);
            break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT :
            *(T5_PTDWORD)pDst = (T5_DWORD)(*(T5_PTLINT)pSrc);
            break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT :
            *(T5_PTDWORD)pDst = (T5_DWORD)(*(T5_PTLWORD)pSrc);
            break;
#endif /*T5DEF_ULINTSUPPORTED*/
        default : break;
        }
        break;
    /* to REAL *************************************************************/
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        switch (bSrcType)
        {
        case T5C_BOOL :
        case T5C_USINT :
            *(T5_PTREAL)pDst = (T5_REAL)(*(T5_PTBYTE)pSrc);
            break;
        case T5C_SINT :
            *(T5_PTREAL)pDst = (T5_REAL)(*(T5_PTCHAR)pSrc);
            break;
        case T5C_INT :
            *(T5_PTREAL)pDst = (T5_REAL)(*(T5_PTSHORT)pSrc);
            break;
        case T5C_UINT :
            *(T5_PTREAL)pDst = (T5_REAL)(*(T5_PTWORD)pSrc);
            break;
        case T5C_DINT :
            *(T5_PTREAL)pDst = (T5_REAL)(*(T5_PTLONG)pSrc);
            break;
        case T5C_UDINT :
        case T5C_TIME :
            *(T5_PTREAL)pDst = (T5_REAL)(*(T5_PTDWORD)pSrc);
            break;
        case T5C_REAL :
            *(T5_PTREAL)pDst = *(T5_PTREAL)pSrc;
            break;
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL:
            *(T5_PTREAL)pDst = (T5_REAL)(*(T5_PTLREAL)pSrc);
            break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT :
            *(T5_PTREAL)pDst = (T5_REAL)(*(T5_PTLINT)pSrc);
            break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT :
            *(T5_PTREAL)pDst = (T5_REAL)(*(T5_PTLWORD)pSrc);
            break;
#endif /*T5DEF_ULINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_REALSUPPORTED*/
    /* to LREAL ************************************************************/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL:
        switch (bSrcType)
        {
        case T5C_BOOL :
        case T5C_USINT :
            *(T5_PTLREAL)pDst = (T5_LREAL)(*(T5_PTBYTE)pSrc);
            break;
        case T5C_SINT :
            *(T5_PTLREAL)pDst = (T5_LREAL)(*(T5_PTCHAR)pSrc);
            break;
        case T5C_INT :
            *(T5_PTLREAL)pDst = (T5_LREAL)(*(T5_PTSHORT)pSrc);
            break;
        case T5C_UINT :
            *(T5_PTLREAL)pDst = (T5_LREAL)(*(T5_PTWORD)pSrc);
            break;
        case T5C_DINT :
            *(T5_PTLREAL)pDst = (T5_LREAL)(*(T5_PTLONG)pSrc);
            break;
        case T5C_UDINT :
        case T5C_TIME :
            *(T5_PTLREAL)pDst = (T5_LREAL)(*(T5_PTDWORD)pSrc);
            break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL :
            *(T5_PTLREAL)pDst = (T5_LREAL)(*(T5_PTREAL)pSrc);
            break;
#endif /*T5DEF_REALSUPPORTED*/
        case T5C_LREAL:
            *(T5_PTLREAL)pDst = *(T5_PTLREAL)pSrc;
            break;
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT :
            *(T5_PTLREAL)pDst = (T5_LREAL)(*(T5_PTLINT)pSrc);
            break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT :
            *(T5_PTLREAL)pDst = (T5_LREAL)(*(T5_PTLWORD)pSrc);
            break;
#endif /*T5DEF_ULINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    /* to LINT *************************************************************/
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        switch (bSrcType)
        {
        case T5C_BOOL :
        case T5C_USINT :
            *(T5_PTLINT)pDst = (T5_LINT)(*(T5_PTBYTE)pSrc);
            break;
        case T5C_SINT :
            *(T5_PTLINT)pDst = (T5_LINT)(*(T5_PTCHAR)pSrc);
            break;
        case T5C_INT :
            *(T5_PTLINT)pDst = (T5_LINT)(*(T5_PTSHORT)pSrc);
            break;
        case T5C_UINT :
            *(T5_PTLINT)pDst = (T5_LINT)(*(T5_PTWORD)pSrc);
            break;
        case T5C_DINT :
            *(T5_PTLINT)pDst = (T5_LINT)(*(T5_PTLONG)pSrc);
            break;
        case T5C_UDINT :
        case T5C_TIME :
            *(T5_PTLINT)pDst = (T5_LINT)(*(T5_PTDWORD)pSrc);
            break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL :
            *(T5_PTLINT)pDst = (T5_LINT)(*(T5_PTREAL)pSrc);
            break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL:
            *(T5_PTLINT)pDst = (T5_LINT)(*(T5_PTLREAL)pSrc);
            break;
#endif /*T5DEF_LREALSUPPORTED*/
        case T5C_LINT :
            *(T5_PTLINT)pDst = *(T5_PTLINT)pSrc;
            break;
#ifdef T5DEF_ULINTSUPPORTED
        case T5C_ULINT :
            *(T5_PTLINT)pDst = *(T5_PTLWORD)pSrc;
            break;
#endif /*T5DEF_ULINTSUPPORTED*/
        default : break;
        }
        break;
#endif /*T5DEF_LINTSUPPORTED*/
    /* to ULINT ************************************************************/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        switch (bSrcType)
        {
        case T5C_BOOL :
        case T5C_USINT :
            *(T5_PTLWORD)pDst = (T5_LWORD)(*(T5_PTBYTE)pSrc);
            break;
        case T5C_SINT :
            *(T5_PTLWORD)pDst = (T5_LWORD)(*(T5_PTCHAR)pSrc);
            break;
        case T5C_INT :
            *(T5_PTLWORD)pDst = (T5_LWORD)(*(T5_PTSHORT)pSrc);
            break;
        case T5C_UINT :
            *(T5_PTLWORD)pDst = (T5_LWORD)(*(T5_PTWORD)pSrc);
            break;
        case T5C_DINT :
            *(T5_PTLWORD)pDst = (T5_LWORD)(*(T5_PTLONG)pSrc);
            break;
        case T5C_UDINT :
        case T5C_TIME :
            *(T5_PTLWORD)pDst = (T5_LWORD)(*(T5_PTDWORD)pSrc);
            break;
#ifdef T5DEF_REALSUPPORTED
        case T5C_REAL :
            *(T5_PTLWORD)pDst = (T5_LWORD)(*(T5_PTREAL)pSrc);
            break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
        case T5C_LREAL:
            *(T5_PTLWORD)pDst = (T5_LWORD)(*(T5_PTLREAL)pSrc);
            break;
#endif /*T5DEF_LREALSUPPORTED*/
#ifdef T5DEF_LINTSUPPORTED
        case T5C_LINT :
            *(T5_PTLWORD)pDst = (T5_LWORD)(*(T5_PTLINT)pSrc);
            break;
#endif /*T5DEF_LINTSUPPORTED*/
        case T5C_ULINT :
            *(T5_PTLWORD)pDst = *(T5_PTLWORD)pSrc;
            break;
        default : break;
        }
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
    default : break;
    }
}

#ifdef T5DEF_STRINGSUPPORTED

/*****************************************************************************
T5VMCnv_NumToSZ
converts any variable to a string - outside CTSEG
parameters:
    pDB (IN) pointer to the database
    wType (IN) variable type
    wIndex (IN) variable index
    sz (OUT) converted variable value as a "C" string
*****************************************************************************/

void T5VMCnv_NumToSZ (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_PTCHAR sz)
{
    T5_PTR pData;

    pData = T5Tools_GetAnyParam (pDB, wType, wIndex, NULL, NULL);
    T5VMCnv_NumToSZPtr (pData, wType, sz);
}

void T5VMCnv_NumToSZPtr (T5_PTR pData, T5_WORD wType, T5_PTCHAR sz)
{
    *sz = '\0';
    switch (wType)
    {
    case T5C_BOOL :
    case T5C_SINT :
        T5_LTOA (sz, (T5_LONG)(*(T5_PTCHAR)pData));
        break;
    case T5C_USINT :
        T5VMStr_UtoSZ (sz, (T5_DWORD)(*(T5_PTBYTE)pData));
        break;
    case T5C_INT :
        T5_LTOA (sz, (T5_LONG)(*(T5_PTSHORT)pData));
        break;
    case T5C_UINT :
        T5VMStr_UtoSZ (sz, (T5_DWORD)(*(T5_PTWORD)pData));
        break;
    case T5C_DINT :
        T5_LTOA (sz, *(T5_PTLONG)pData);
        break;
    case T5C_UDINT :
    case T5C_TIME :
        T5VMStr_UtoSZ (sz, *(T5_PTDWORD)pData);
        break;
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        T5VMStr_LINTtoSZ (sz, *(T5_PTLINT)pData);
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        T5VMStr_LINTtoSZ (sz, *(T5_PTLINT)pData); /* todo */
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        T5_RTOA (sz, *(T5_PTREAL)pData);
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        T5_RTOA (sz, *(T5_PTLREAL)pData);
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default : break;
    }
}

/*****************************************************************************
T5VMCnv_SZToNum
force any variable with a value contained in a string - outside CTSEG
parameters:
    pDB (IN) pointer to the database
    wType (IN) variable type
    wIndex (IN) variable index
    sz (IN) value as a "C" string
*****************************************************************************/

void T5VMCnv_SZToNum (T5PTR_DB pDB, T5_WORD wType, T5_WORD wIndex, T5_PTCHAR sz)
{
    T5_PTR pData;

    pData = T5Tools_GetAnyParam (pDB, wType, wIndex, NULL, NULL);
    T5VMCnv_SZToNumPtr (pData, wType, sz);
}

void T5VMCnv_SZToNumPtr (T5_PTR pData, T5_WORD wType, T5_PTCHAR sz)
{
    switch (wType)
    {
    case T5C_BOOL :
        *(T5_PTCHAR)pData = (T5_ATOI (sz)) ? 1 : 0;
    case T5C_SINT :
        *(T5_PTCHAR)pData = (T5_CHAR)(T5_ATOI (sz));
        break;
    case T5C_USINT :
        *(T5_PTBYTE)pData = (T5_BYTE)(T5_ATOI (sz));
        break;
    case T5C_INT :
        *(T5_PTSHORT)pData = (T5_SHORT)(T5_ATOI (sz));
        break;
    case T5C_UINT :
        *(T5_PTWORD)pData = (T5_WORD)(T5_ATOI (sz));
        break;
    case T5C_DINT :
        *(T5_PTLONG)pData = (T5_LONG)(T5_ATOI (sz));
        break;
    case T5C_UDINT :
    case T5C_TIME :
        *(T5_PTDWORD)pData = (T5_DWORD)(T5_ATOI (sz));
        break;
#ifdef T5DEF_LINTSUPPORTED
    case T5C_LINT :
        *(T5_PTLINT)pData = T5VMStr_SZtoLINT (sz);
        break;
#endif /*T5DEF_LINTSUPPORTED*/
#ifdef T5DEF_ULINTSUPPORTED
    case T5C_ULINT :
        *(T5_PTLWORD)pData = (T5_LWORD)T5VMStr_SZtoLINT (sz); /* todo */
        break;
#endif /*T5DEF_ULINTSUPPORTED*/
#ifdef T5DEF_REALSUPPORTED
    case T5C_REAL :
        *(T5_PTREAL)pData = (T5_REAL)(T5_ATOF (sz));
        break;
#endif /*T5DEF_REALSUPPORTED*/
#ifdef T5DEF_LREALSUPPORTED
    case T5C_LREAL :
        *(T5_PTLREAL)pData = (T5_LREAL)(T5_ATOF (sz));
        break;
#endif /*T5DEF_LREALSUPPORTED*/
    default : break;
    }
}

#endif /*T5DEF_STRINGSUPPORTED*/

/* eof **********************************************************************/
