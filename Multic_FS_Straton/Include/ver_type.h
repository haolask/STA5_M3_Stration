////////////////////////////////////////////////////////////////////////////////
//
//      O       OOOO  OOOOOOO   O       O         ## ## ##
//     O O     O         O      O      O O
//    O   O    O         O      O     O   O      ## ## ##
//   OOOOOOO   O         O      O    OOOOOOO
//  O       O   OOOO     O      O   O       O   ## ## ##
//
//----------------------------------------------------------------------------
// HEAD : (c) ACTIA Automotive 2011 : " Any copy and re-use of this
//        file without the written consent of ACTIA is forbidden "
//        --------------------------------------------------------------------
//        Classification :  (-)No   (-)Confident. ACTIA   (-)Confident. Client
//        --------------------------------------------------------------------
//        VER_TYPE : SW banner Version type definition Export
////////////////////////////////////////////////////////////////////////////////
// FILE : ver_type.h (HEADER)
//----------------------------------------------------------------------------
// DESC : SW banner Version type definition Export
//----------------------------------------------------------------------------
// LIST :
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 515 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Include/ver_type.h $
// Updated the :$Date: 2015-04-16 01:55:11 +0800 (周四, 16 四月 2015) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////
#ifndef VER_TYPE_INCLUDE
#define VER_TYPE_INCLUDE

//----------------------------------------------------------------------------//
#include "std_type.h"

//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// TYPEDEF
////////////////////////////////////////////////////////////////////////////////
/*	Field size definition	*/
#define Du16WVersion_eVRefSize			10U
#define Du16WVersion_eVNameSize			20U
#define Du16WVersion_eVCopyRightSize	10U
#define Du16WVersion_eDateSize			12U
#define Du16WVersion_eTimeSize			10U

/*	Software identification	*/
typedef struct
{
	TU8	u8V0;                       		  //Software version
	TU8	u8V1;                       		  //Software version
	TU8	u8V2;                       		  //Software version
	TU8	u8V3;                       		  //Software version
	TU8	tu8VRef[Du16WVersion_eVRefSize];
	TU8	tu8VName[Du16WVersion_eVNameSize];
	TU8	tu8VCopyRight[Du16WVersion_eVCopyRightSize];
	TU8	tu8VDate[Du16WVersion_eDateSize];
	TU8	tu8VTime[Du16WVersion_eTimeSize];
} TstWVersion_eSWIdent;


////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////


#endif	/*	VER_TYPE_INCLUDE	*/
