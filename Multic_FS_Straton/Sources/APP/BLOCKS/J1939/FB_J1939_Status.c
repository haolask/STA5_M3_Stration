////////////////////////////////////////////////////////////////////////////////
//       _________________________________________ ___ ___ ___
//      /  ___      ___  ________ ___ ___        /   /   /   /
//     /  /   |   /  __|/__   __//  //   | (R)  /___/___/___/
//    /  / /| | /  /      /  /  /  // /| |     /   /   /   /
//   /  / __  |(  (___   /  /  /  // __  |    /___/___/___/
//  /  /_/  |_| \_____| /__/  /__//_/  |_|   /   /   /   /
// /________________________________________/___/___/___/
//----------------------------------------------------------------------------
// HEAD : (c) ACTIA Automotive 2015 : " Any copy and re-use of this
//        file without the written consent of ACTIA is forbidden "
//        --------------------------------------------------------------------
//        Classification :  (-)No   (-)Confident. ACTIA   (-)Confident. Client
//        --------------------------------------------------------------------
//        FB_J1939_Status : J1939 message Status Decoder for Straton application
////////////////////////////////////////////////////////////////////////////////
// FILE : FB_J1939_Status.h (CODE)
//----------------------------------------------------------------------------
// DESC : J1939 Functional Block Header file for Straton application 
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 772 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Sources/APP/CORE/VM/custom/T5ACTIA.c $
// Updated the :$Date: 2015-07-22 14:16:15 +0200 (mer., 22 juil. 2015) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
/* FB_J1939.h */
#include "t5vm.h"
#include "t5_J1939.h"

/*----------------------------------------------------------------------------------------
TYPE MEMO
------------------------------------------------------------------------------------------
typedef char            T5_CHAR;    SINT     short signed integer on 8 bits
typedef signed short    T5_SHORT;   INT      signed integer on 16 bits
typedef signed long     T5_LONG;    DINT     signed integer on 32 bits
typedef unsigned char   T5_BOOL;    BOOL     boolean (TRUE / FALSE)
typedef unsigned char   T5_BYTE;    USINT    unsigned interger on 8 bits
typedef unsigned short  T5_WORD;    UINT     unsigned interger on 16 bits
typedef unsigned long   T5_DWORD;   UDINT    unsigned interger on 32 bits
typedef float           T5_REAL;    REAL     single precision floating point on 32 bits
typedef double          T5_LREAL;   LREAL    double precision floating point on 64 bits
----------------------------------------------------------------------------------------*/

/* J1939_Status - Source code (requires t5vm.h include) */
/* Function block */

/* Argument list */

#define _P_STATUS   (*((T5_PTDWORD)(T5GET_DBDATA32(pBase))+pArgs[0]))
#define _P_PORTNOTWIRED   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_INVALIDPORTSETTINGS   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))
#define _P_PGNDATAOUTOFMEMORY   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[3]))
#define _P_RXTIMEOUT   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[4]))
#define _P_SENTFROMREQUEST   (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[5]))
#define _P_PGNREGISTRATIONSTATUS   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[6]))
#define _P_MESSAGELENGTH   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[7]))
#define _P_MESSAGEADDRESS   (*((T5_PTBYTE)(T5GET_DBDATA8(pBase))+pArgs[8]))

/* private block data */

typedef struct
{
    T5_DWORD dwData; /* TODO: replace dwData by the items you need */
} _str_FB_J1939_STATUS;

/* handler */

T5_DWORD J1939_STATUS (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
   _str_FB_J1939_STATUS *pData;

   pData = (_str_FB_J1939_STATUS *)pInst;
   switch (wCommand)
   {
   case T5FBCMD_ACTIVATE :
      /* activates the function block */
      _P_PORTNOTWIRED         = ((TuJ1939_HwStatus*)(&_P_STATUS))->u1ProtocolNotWired;
      _P_INVALIDPORTSETTINGS  = ((TuJ1939_HwStatus*)(&_P_STATUS))->u1InvalidPortSettings;
      _P_PGNDATAOUTOFMEMORY   = ((TuJ1939_HwStatus*)(&_P_STATUS))->u1PgnDataOutOfMemory;
      _P_RXTIMEOUT            = ((TuJ1939_HwStatus*)(&_P_STATUS))->u1RxTimeOut;
      _P_SENTFROMREQUEST      = ((TuJ1939_HwStatus*)(&_P_STATUS))->u1SentFromRequest;
      _P_PGNREGISTRATIONSTATUS= ((TuJ1939_HwStatus*)(&_P_STATUS))->u3PgnRegistrationStatus;
      _P_MESSAGELENGTH        = ((TuJ1939_HwStatus*)(&_P_STATUS))->u8NbOfBytes;
      _P_MESSAGEADDRESS       = ((TuJ1939_HwStatus*)(&_P_STATUS))->u8Address;

      return 0L;

   case T5FBCMD_INITINSTANCE :
      /* initialize private data */
      /* enter your code here... */
      return 0L;

   case T5FBCMD_EXITINSTANCE :
      /* release private data */
      /* enter your code here... */
      return 0L;

   case T5FBCMD_HOTRESTART :
      /* actuate private data for hot restart */
      /* enter your code here... */
      return 0L;

   case T5FBCMD_SIZEOFINSTANCE :
      /* dont change that */
      return (T5_DWORD)sizeof(_str_FB_J1939_STATUS);

   case T5FBCMD_ACCEPTCT :
      /* dont change that */
      return 1L;

   default :
      return 0L;
   }
}

/* Undefine argument list */

#undef _P_STATUS
#undef _P_PORTNOTWIRED
#undef _P_INVALIDPORTSETTINGS
#undef _P_PGNDATAOUTOFMEMORY
#undef _P_RXTIMEOUT
#undef _P_SENTFROMREQUEST
#undef _P_PGNREGISTRATIONSTATUS
#undef _P_MESSAGELENGTH
#undef _P_MESSAGEADDRESS
