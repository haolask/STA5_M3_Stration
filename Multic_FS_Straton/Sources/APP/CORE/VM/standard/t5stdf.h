/*****************************************************************************
T5stdf.h :   standard functions and function blocks

DO NOT ALTER THIS !

(c) COPA-DATA France 2002
*****************************************************************************/

#ifndef _T5STDF_H_INCLUDED_
#define _T5STDF_H_INCLUDED_

/****************************************************************************/
/* standard function blocks - numbering */

#define T5FBL_SIG_GEN           1
#define T5FBL_RS                2
#define T5FBL_SR                3
#define T5FBL_R_TRIG            4
#define T5FBL_F_TRIG            5
#define T5FBL_CTU               6
#define T5FBL_CTD               7
#define T5FBL_CTUD              8
#define T5FBL_TON               9
#define T5FBL_TOF               10
#define T5FBL_TP                11
#define T5FBL_SEMA              12
#define T5FBL_CMP               13
#define T5FBL_STACKINT          14
#define T5FBL_DERIVATE          15
#define T5FBL_HYSTER            16
#define T5FBL_AVERAGE           17
#define T5FBL_LIM_ALRM          18
#define T5FBL_BLINK             19
#define T5FBL_INTEGRAL          20
#define T5FBL_SERIO             21
#define T5FBL_UNPACK8           22
#define T5FBL_TOFR              23
#define T5FBL_TPR               24
#define T5FBL_PLS               25
#define T5FBL_FLIPFLOP          26
#define T5FBL_RAMP              27
#define T5FBL_BLINKA            28
#define T5FBL_DTAT              29
#define T5FBL_DTEVERY           30
#define T5FBL_FIFO              31
#define T5FBL_LIFO              32
#define T5FBL_ALARM_M           33
#define T5FBL_ALARM_A           34
#define T5FBL_LOGFILECSV        35
#define T5FBL_TMU               36
#define T5FBL_TMD               37
#define T5FBL_MBSUDP            38
#define T5FBL_MBSRTU            39
#define T5FBL_SIGPLAY           40
#define T5FBL_ISFCSTEP          41
#define T5FBL_ISFCINITSTEP      42
#define T5FBL_AVERAGEL          43
#define T5FBL_CURVELIN          44
#define T5FBL_SURFLIN           45
#define T5FBL_DTCURDATETIME     46
#define T5FBL_MBSUDPEX          47
#define T5FBL_MBSRTUEX          48
#define T5FBL_CANRCV            49
#define T5FBL_MBMTCP            50
#define T5FBL_MBMRTU            51
#define T5FBL_TMUSEC            52
#define T5FBL_MBSLAVETCP        53
#define T5FBL_HYSTERACC         54
#define T5FBL_SERIO_B           55
#define T5FBL_MBSRTUEXD         56
#define T5FBL_MBSLAVETCPEX      57

/* REMEMBER K5MWTEXT */

/****************************************************************************/
/* standard functions - numbering */

#define T5FNC_ABS               1
#define T5FNC_ACOS              2
#define T5FNC_ASIN              3
#define T5FNC_ATAN              4
#define T5FNC_COS               5
#define T5FNC_EXPT              6
#define T5FNC_LOG               7
#define T5FNC_SIN               8
#define T5FNC_SQRT              9
#define T5FNC_TAN               10
#define T5FNC_TRUNC             11
#define T5FNC_POW               12
#define T5FNC_VSISTAMPGMT       13
#define T5FNC_UDPSENDTOARRAY    14
#define T5FNC_UDPRCVFROMARRAY   15
#define T5FNC_FR_READ           16
#define T5FNC_FR_WRITE          17
#define T5FNC_F_FLUSH           18
#define T5FNC_F_SEEK            19
#define T5FNC_AND_MASK          20
#define T5FNC_LIMIT             21
#define T5FNC_MAX               22
#define T5FNC_MIN               23
#define T5FNC_MOD               24
#define T5FNC_MUX4              25
#define T5FNC_MUX8              26
#define T5FNC_NOT_MASK          27
#define T5FNC_OR_MASK           28
#define T5FNC_RAND              29
#define T5FNC_ROL               30
#define T5FNC_ROR               31
#define T5FNC_SEL               32
#define T5FNC_SHL               33
#define T5FNC_SHR               34
#define T5FNC_XOR_MASK          35
#define T5FNC_ODD               36
#define T5FNC_ASCII             40
#define T5FNC_CHAR              41
#define T5FNC_DELETE            42
#define T5FNC_FIND              43
#define T5FNC_INSERT            44
#define T5FNC_LEFT              45
#define T5FNC_MID               46
#define T5FNC_MLEN              47
#define T5FNC_REPLACE           48
#define T5FNC_RIGHT             49
#define T5FNC_ARCREATE          60
#define T5FNC_ARREAD            61
#define T5FNC_ARWRITE           62
#define T5FNC_DAY_TIME          63
#define T5FNC_F_ROPEN           70
#define T5FNC_F_WOPEN           71
#define T5FNC_FA_READ           72
#define T5FNC_FM_READ           73
#define T5FNC_FA_WRITE          74
#define T5FNC_FM_WRITE          75
#define T5FNC_F_CLOSE           76
#define T5FNC_F_EOF             77

#define T5FNC_BIN_TO_BCD        78
#define T5FNC_BCD_TO_BIN        79
#define T5FNC_PRINTF            80
#define T5FNC_ROLW              81
#define T5FNC_RORW              82
#define T5FNC_SHLW              83
#define T5FNC_SHRW              84
#define T5FNC_ROLB              85
#define T5FNC_RORB              86
#define T5FNC_SHLB              87
#define T5FNC_SHRB              88
#define T5FNC_ATOH              89
#define T5FNC_HTOA              90
#define T5FNC_USEDEGREES        91
#define T5FNC_ATAN2             92
#define T5FNC_CRC16             93

#define T5FNC_LOGMESSAGE        94
#define T5FNC_MOVEBLOCK         95

#define T5FNC_ANDWORD           96
#define T5FNC_ORWORD            97
#define T5FNC_XORWORD           98
#define T5FNC_NOTWORD           99

#define T5FNC_ANDBYTE           100
#define T5FNC_ORBYTE            101
#define T5FNC_XORBYTE           102
#define T5FNC_NOTBYTE           103

#define T5FNC_SERIALIZEOUT      104
#define T5FNC_SERIALIZEIN       105

#define T5FNC_COSL              106
#define T5FNC_SINL              107
#define T5FNC_TANL              108
#define T5FNC_ACOSL             109
#define T5FNC_ASINL             110
#define T5FNC_ATANL             111
#define T5FNC_ATAN2L            112

#define T5FNC_GETSYSINFO        113

#define T5FNC_POWL              114
#define T5FNC_SQRTL             115
#define T5FNC_ABSL              116
#define T5FNC_TRUNCL            117

#define T5FNC_DTCURTIME         118
#define T5FNC_DTCURDATE         119
#define T5FNC_DTDAY             120
#define T5FNC_DTMONTH           121
#define T5FNC_DTYEAR            122
#define T5FNC_DTSEC             123
#define T5FNC_DTMIN             124
#define T5FNC_DTHOUR            125
#define T5FNC_DTMS              126

#define T5FNC_ASIREADPP         127
#define T5FNC_ASIWRITEPP        128
#define T5FNC_ASISENDPAR        129
#define T5FNC_ASIREADPI         130
#define T5FNC_ASISTOREPI        131

#define T5FNC_MODR              132
#define T5FNC_MODLR             133

#define T5FNC_MBSHIFT           134

#define T5FNC_FATALSTOP         135
#define T5FNC_CYCLESTOP         136

#define T5FNC_STRINGTOARRAY     137
#define T5FNC_ARRAYTOSTRING     138
#define T5FNC_STRINGTOARRAY32   139
#define T5FNC_ARRAYTOSTRING32   140
#define T5FNC_STRINGTOARRAY16   141
#define T5FNC_ARRAYTOSTRING16   142

#define T5FNC_GETVARBYNAME      143
#define T5FNC_GETVARBYADDR      144
#define T5FNC_GETVARSYMBOL      145
#define T5FNC_GETVARPROFILE     146
#define T5FNC_GETVARTYPENAME    147

#define T5FNC_TCPLISTEN         148
#define T5FNC_TCPACCEPT         149
#define T5FNC_TCPCLOSE          150
#define T5FNC_TCPSEND           151
#define T5FNC_TCPRECEIVE        152
#define T5FNC_TCPCONNECT        153
#define T5FNC_TCPISCONNECTED    154
#define T5FNC_TCPISVALID        155

#define T5FNC_ENABLEEVENTS      156
#define T5FNC_SCALELIN          157
#define T5FNC_RCPAPPLY          158

#define T5FNC_SERGETSTRING      159
#define T5FNC_SERPUTSTRING      160

#define T5FNC_COUNTOF           161

#define T5FNC_LOBYTE            162
#define T5FNC_HIBYTE            163
#define T5FNC_LOWORD            164
#define T5FNC_HIWORD            165
#define T5FNC_MAKEWORD          166
#define T5FNC_MAKEDWORD         167
#define T5FNC_PACK8             168
#define T5FNC_SETBIT            169
#define T5FNC_TESTBIT           170
#define T5FNC_DTFORMAT          171

#define T5FNC_FB_READ           172
#define T5FNC_FB_WRITE          173
#define T5FNC_F_AOPEN           174

#define T5FNC_STRINGTABLE       175
#define T5FNC_LOADSTRING        176
#define T5FNC_SIGSCALE          177

#define T5FNC_F_EXIST           178
#define T5FNC_F_GETSIZE         179
#define T5FNC_F_COPY            180
#define T5FNC_F_DELETE          181
#define T5FNC_F_RENAME          182

#define T5FNC_UDPCREATE         183
#define T5FNC_UDPADDRMAKE       184
#define T5FNC_UDPSENDTO         185
#define T5FNC_UDPRCVFROM        186

#define T5FNC_VSIGETBIT         187
#define T5FNC_VSISETBIT         188
#define T5FNC_VSIGETDATE        189
#define T5FNC_VSISETDATE        190
#define T5FNC_VSIGETTIME        191
#define T5FNC_VSISETTIME        192
#define T5FNC_VSISTAMP          193

#define T5FNC_REGPARGET         194
#define T5FNC_REGPARPUT         195

#define T5FNC_NUMTOSTRING       196
#define T5FNC_F_ISREADY         197

#define T5FNC_F_SETCSVOPT       198
#define T5FNC_F_CANSND          199

#define T5FNC_F_LN              200
#define T5FNC_F_EXP             201
#define T5FNC_F_LNL             202
#define T5FNC_F_EXPL            203
#define T5FNC_F_LOGL            204
#define T5FNC_F_LEN             205

#define T5FNC_ROL64             206
#define T5FNC_ROR64             207
#define T5FNC_SHL64             208
#define T5FNC_SHR64             209
#define T5FNC_AND64             210
#define T5FNC_OR64              211
#define T5FNC_XOR64             212
#define T5FNC_NOT64             213
#define T5FNC_TCPSNDARRAY       214
#define T5FNC_TCPRCVARRAY       215

#define T5FNC_F_SAVERETAIN      216
#define T5FNC_F_LOADRETAIN      217

#define T5FNC_MBSLAVEIDENT      218
#define T5FNC_SWAB              219
#define T5FNC_VSICOPY           220

/* REMEMBER K5MWTEXT */

/****************************************************************************/

#endif /*_T5STDF_H_INCLUDED_*/

/* eof **********************************************************************/
