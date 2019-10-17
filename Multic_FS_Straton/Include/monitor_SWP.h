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
//        SWP XX protocol API
////////////////////////////////////////////////////////////////////////////////
// FILE : monitor_SWPxx.h (HEADER)
//----------------------------------------------------------------------------
// DESC : SWP XX protocol API
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 1290 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_MONITOR/Sources/API/monitor_SWP.h $
// Updated the :$Date: 2016-05-13 00:46:55 +0800 (周五, 13 五月 2016) $
// By 			:$Author: ssprung $
////////////////////////////////////////////////////////////////////////////////

#ifndef SWP_API_INCLUDE
#define SWP_API_INCLUDE

//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////
#include "std_type.h"


////////////////////////////////////////////////////////////////////////////////
// TYPEDEF
////////////////////////////////////////////////////////////////////////////////
#define CucSWP_iNbSWPMax  4

typedef struct{				// Size HEX DEC
    TU8 Specific_SWPXX_C0;	// 1	0	0
    TU8 Specific_SWPXX_C1;	// 1	1	1
    TU8 Specific_SWPXX_C2;	// 1	2	2
    TU8 Specific_SWPXX_C3;	// 1	3	3
    TU8 Specific_SWPXX_C4;	// 1	4	4
    TU8 Specific_SWPXX_C5;	// 1	5	5
    TU8 Specific_SWPXX_C6;	// 1	6	6
    TU8 Specific_SWPXX_C7;	// 1	7	7
    TU8 Specific_SWPXX_C8;	// 1	8	8
    TU8 Specific_SWPXX_C9;	// 1	9	9
    TU8 Specific_SWPXX_C10;	// 1	A	A
    TU8 Specific_SWPXX_C11;	// 1	B	B
    TU8 Specific_SWPXX_C12;	// 1	C	C
    TU8 Specific_SWPXX_C13;	// 1	D	D
    TU8 Specific_SWPXX_C14;	// 1	E	E
    TU8 Specific_SWPXX_C15;	// 1	F	F
    TU8 Specific_SWPXX_C16;	// 1	10	16
    TU8 Specific_SWPXX_C17;	// 1	11	17
    TU8 Specific_SWPXX_C18;	// 1	12	18
    TU8 Specific_SWPXX_C19;	// 1	13	19
    TU8 Specific_SWPXX_C20;	// 1	14	20
    TU8 Specific_SWPXX_C21;	// 1	15	21
    TU8 Specific_SWPXX_C22;	// 1	16	22
    TU8 Specific_SWPXX_C23;	// 1	17	23
    TU8 Specific_SWPXX_C24;	// 1	18	24
    TU8 Specific_SWPXX_C25;	// 1	19	25
    TU8 Specific_SWPXX_C26;	// 1	1A	26
    TU8 Specific_SWPXX_C27;	// 1	1B	27
    TU8 Specific_SWPXX_C28;	// 1	1C	28
    TU8 Specific_SWPXX_C29;	// 1	1D	29
    TU8 Specific_SWPXX_C30;	// 1	1E	30
    TU8 Specific_SWPXX_C31;	// 1	1F	31
    TU8 Specific_SWPXX_C32;	// 1	20	32
    TU8 Specific_SWPXX_C33;	// 1	21	33
    TU8 Specific_SWPXX_C34;	// 1	22	34
    TU8 Specific_SWPXX_C35;	// 1	23	35
    TU8 Specific_SWPXX_C36;	// 1	24	36
    TU8 Specific_SWPXX_C37;	// 1	25	37
    TU8 Specific_SWPXX_C38;	// 1	26	38
    TU8 Specific_SWPXX_C39;	// 1	27	39
    TU8 Specific_SWPXX_C40;	// 1	28	40
    TU8 Specific_SWPXX_C41;	// 1	29	41
    TU8 Specific_SWPXX_C42;	// 1	2A	42
    TU8 Specific_SWPXX_C43;	// 1	2B	43
    TU8 Specific_SWPXX_C44;	// 1	2C	44
    TU8 Specific_SWPXX_C45;	// 1	2D	45
    TU8 Specific_SWPXX_C46;	// 1	2E	46
    TU8 Specific_SWPXX_C47;	// 1	2F	47
    TU8 Specific_SWPXX_C48;	// 1	30	48
    TU8 Specific_SWPXX_C49;	// 1	31	49
    TU8 Specific_SWPXX_C50;	// 1	32	50
    TU8 Specific_SWPXX_C51;	// 1	33	51
    TU8 Specific_SWPXX_WK;	// 1	34	52
    TU8 Specific_SWPXX_WK0;	// 1	35	53
    TU8 Specific_SWPXX_WK1;	// 1	36	54
    TU8 Specific_SWPXX_WK2;	// 1	37	55
    TU8 Specific_SWPXX_WK3;	// 1	38	56
    TU8 Specific_SWPXX_Input_spare[6];	// 6	3D	62
    TU8 Specific_SWPXX_RxFrame_Timeout;	// 1	3E	63
    TU8 Specific_SWPXX_LED0;	// 1	3F	64
    TU8 Specific_SWPXX_LED1;	// 1	40	65
    TU8 Specific_SWPXX_LED2;	// 1	41	66
    TU8 Specific_SWPXX_LED3;	// 1	42	67
    TU8 Specific_SWPXX_LED4;	// 1	43	68
    TU8 Specific_SWPXX_LED5;	// 1	44	69
    TU8 Specific_SWPXX_LED6;	// 1	45	70
    TU8 Specific_SWPXX_LED7;	// 1	46	71
    TU8 Specific_SWPXX_LED8;	// 1	47	72
    TU8 Specific_SWPXX_LED9;	// 1	48	73
    TU8 Specific_SWPXX_LED10;	// 1	49	74
    TU8 Specific_SWPXX_LED11;	// 1	4A	75
    TU8 Specific_SWPXX_LED12;	// 1	4B	76
    TU8 Specific_SWPXX_LED13;	// 1	4C	77
    TU8 Specific_SWPXX_LED14;	// 1	4D	78
    TU8 Specific_SWPXX_LED15;	// 1	4E	79
    TU8 Specific_SWPXX_LED16;	// 1	4F	80
    TU8 Specific_SWPXX_LED17;	// 1	51	81
    TU8 Specific_SWPXX_LED18;	// 1	52	82
    TU8 Specific_SWPXX_LED19;	// 1	53	83
    TU8 Specific_SWPXX_LED20;	// 1	54	84
    TU8 Specific_SWPXX_LED21;	// 1	55	85
    TU8 Specific_SWPXX_LED22;	// 1	56	86
    TU8 Specific_SWPXX_LED23;	// 1	57	87
    TU8 Specific_SWPXX_LED24;	// 1	58	88
    TU8 Specific_SWPXX_LED25;	// 1	59	89
    TU8 Specific_SWPXX_LED26;	// 1	5A	90
    TU8 Specific_SWPXX_LED27;	// 1	5B	91
    TU8 Specific_SWPXX_LED28;	// 1	5C	92
    TU8 Specific_SWPXX_LED29;	// 1	5D	93
    TU8 Specific_SWPXX_LED30;	// 1	5E	94
    TU8 Specific_SWPXX_LED31;	// 1	5F	95
    TU8 Specific_SWPXX_LED32;	// 1	60	96
    TU8 Specific_SWPXX_LED33;	// 1	61	97
    TU8 Specific_SWPXX_LED34;	// 1	62	98
    TU8 Specific_SWPXX_LED35;	// 1	63	99
    TU8 Specific_SWPXX_LED36;	// 1	64	100
    TU8 Specific_SWPXX_LED37;	// 1	65	101
    TU8 Specific_SWPXX_LED38;	// 1	66	102
    TU8 Specific_SWPXX_LED39;	// 1	67	103
    TU8 Specific_SWPXX_LED40;	// 1	68	104
    TU8 Specific_SWPXX_LED41;	// 1	69	105
    TU8 Specific_SWPXX_LED42;	// 1	6A	106
    TU8 Specific_SWPXX_LED43;	// 1	6B	107
    TU8 Specific_SWPXX_LED44;	// 1	6C	108
    TU8 Specific_SWPXX_LED45;	// 1	6D	109
    TU8 Specific_SWPXX_LED46;	// 1	6E	110
    TU8 Specific_SWPXX_LED47;	// 1	6F	111
    TU8 Specific_SWPXX_IL0;		// 1	70	112
    TU8 Specific_SWPXX_IL1;		// 1	71	113
    TU8 Specific_SWPXX_IL2;		// 1	72	114
    TU8 Specific_SWPXX_IL3;		// 1	73	115
    TU8 Specific_SWPXX_IL4;		// 1	74	116
    TU8 Specific_SWPXX_IL5;		// 1	75	117
    TU8 Specific_SWPXX_IL6;		// 1	76	118
    TU8 Specific_SWPXX_IL7;		// 1	77	119
    TU8 Specific_SWPXX_IL8;		// 1	78	120
    TU8 Specific_SWPXX_IL9;		// 1	79	121
    TU8 Specific_SWPXX_BUZZ;	// 1	7A	122
    TU8 Specific_SWPXX_BKL;		// 1	7B	123
    TU16 Specific_SWPXX_LCD;	// 1	7E	125
    TU16 Specific_SWPXX_Output_spare;	// 1	7F	127
}TstMAI_Specific_SWP_IOCard;

typedef struct{						// Size HEX DEC
	TU8 Generic_SWPXX_M_IN0;		// 1	0	0
	TU8 Generic_SWPXX_M_IN1;		// 1	1	1
	TU8 Generic_SWPXX_M_IN2;		// 1	2	2
	TU8 Generic_SWPXX_M_IN3;		// 1	3	3
	TU8 Generic_SWPXX_M_IN4;		// 1	4	4
	TU8 Generic_SWPXX_M_IN5;		// 1	5	5
	TU8 Generic_SWPXX_M_IN6;		// 1	6	6
	TU8 Generic_SWPXX_M_IN7;		// 1	7	7
	TU8 Generic_SWPXX_M_IN8;		// 1	8	8
	TU8 Generic_SWPXX_M_IN9;		// 1	9	9
	TU8 Generic_SWPXX_M_IN10;		// 1	A	10
	TU8 Generic_SWPXX_M_IN11;		// 1	B	11
	TU8 Generic_SWPXX_S1_IN0;		// 1	C	12
	TU8 Generic_SWPXX_S1_IN1;		// 1	D	13
	TU8 Generic_SWPXX_S1_IN2;		// 1	E	14
	TU8 Generic_SWPXX_S1_IN3;		// 1	F	15
	TU8 Generic_SWPXX_S1_IN4;		// 1	10	16
	TU8 Generic_SWPXX_S1_IN5;		// 1	11	17
	TU8 Generic_SWPXX_S1_IN6;		// 1	12	18
	TU8 Generic_SWPXX_S1_IN7;		// 1	13	19
	TU8 Generic_SWPXX_S1_IN8;		// 1	14	20
	TU8 Generic_SWPXX_S1_IN9;		// 1	15	21
	TU8 Generic_SWPXX_S1_IN10;		// 1	16	22
	TU8 Generic_SWPXX_S1_IN11;		// 1	17	23
	TU8 Generic_SWPXX_S2_IN0;		// 1	18	24
	TU8 Generic_SWPXX_S2_IN1;		// 1	19	25
	TU8 Generic_SWPXX_S2_IN2;		// 1	1A	26
	TU8 Generic_SWPXX_S2_IN3;		// 1	1B	27
	TU8 Generic_SWPXX_S2_IN4;		// 1	1C	28
	TU8 Generic_SWPXX_S2_IN5;		// 1	1D	29
	TU8 Generic_SWPXX_S2_IN6;		// 1	1E	30
	TU8 Generic_SWPXX_S2_IN7;		// 1	1F	31
	TU8 Generic_SWPXX_S2_IN8;		// 1	20	32
	TU8 Generic_SWPXX_S2_IN9;		// 1	21	33
	TU8 Generic_SWPXX_S2_IN10;		// 1	22	34
	TU8 Generic_SWPXX_S2_IN11;		// 1	23	35
	TU8 Generic_SWPXX_S3_IN0;		// 1	24	36
	TU8 Generic_SWPXX_S3_IN1;		// 1	25	37
	TU8 Generic_SWPXX_S3_IN2;		// 1	26	38
	TU8 Generic_SWPXX_S3_IN3;		// 1	27	39
	TU8 Generic_SWPXX_S3_IN4;		// 1	28	40
	TU8 Generic_SWPXX_S3_IN5;		// 1	29	41
	TU8 Generic_SWPXX_S3_IN6;		// 1	2A	42
	TU8 Generic_SWPXX_S3_IN7;		// 1	2B	43
	TU8 Generic_SWPXX_S3_IN8;		// 1	2C	44
	TU8 Generic_SWPXX_S3_IN9;		// 1	2D	45
	TU8 Generic_SWPXX_S3_IN10;		// 1	2E	46
	TU8 Generic_SWPXX_S3_IN11;		// 1	2F	47
	TU16 Generic_SWPXX_WKO_SWP;		// 2	31	49
	TU8 Generic_SWPXX_IN1;			// 1	32	50
	TU8 Generic_SWPXX_IN3;			// 1	33	51
	TU8 Generic_SWPXX_IN5;			// 1	34	52
	TU8 Generic_SWPXX_IN7;			// 1	35	53
	TU8 Generic_SWPXX_IN9;			// 1	36	54
	TU8 Generic_SWPXX_IN11;			// 1	37	55
	TU8 Generic_SWPXX_CAN;			// 1	38	56
	TU8 Generic_SWPXX_spare[3];		// 3	3B	59
	TU8 Generic_SWPXX_S1_LINErr;		// 1	3C	60
	TU8 Generic_SWPXX_S2_LINErr;		// 1	3D	61
	TU8 Generic_SWPXX_S3_LINErr;		// 1	3E	62
	TU8 Generic_SWPXX_RxFrameTimeout;	// 1	3F	63
	TU8 Generic_SWPXX_M_OUT0;		// 1	40	64
	TU8 Generic_SWPXX_M_OUT1;		// 1	41	65
	TU8 Generic_SWPXX_M_OUT2;		// 1	42	66
	TU8 Generic_SWPXX_M_OUT3;		// 1	43	67
	TU8 Generic_SWPXX_M_OUT4;		// 1	44	68
	TU8 Generic_SWPXX_M_OUT5;		// 1	45	69
	TU8 Generic_SWPXX_M_OUT6;		// 1	46	70
	TU8 Generic_SWPXX_M_OUT7;		// 1	47	71
	TU8 Generic_SWPXX_M_OUT8;		// 1	48	72
	TU8 Generic_SWPXX_M_OUT9;		// 1	49	73
	TU8 Generic_SWPXX_M_OUT10;		// 1	4A	74
	TU8 Generic_SWPXX_M_OUT11;		// 1	4B	75
	TU8 Generic_SWPXX_S1_OUT0;		// 1	4C	76
	TU8 Generic_SWPXX_S1_OUT1;		// 1	4D	77
	TU8 Generic_SWPXX_S1_OUT2;		// 1	4E	78
	TU8 Generic_SWPXX_S1_OUT3;		// 1	4F	79
	TU8 Generic_SWPXX_S1_OUT4;		// 1	50	80
	TU8 Generic_SWPXX_S1_OUT5;		// 1	51	81
	TU8 Generic_SWPXX_S1_OUT6;		// 1	52	82
	TU8 Generic_SWPXX_S1_OUT7;		// 1	53	83
	TU8 Generic_SWPXX_S1_OUT8;		// 1	54	84
	TU8 Generic_SWPXX_S1_OUT9;		// 1	55	85
	TU8 Generic_SWPXX_S1_OUT10;		// 1	56	86
	TU8 Generic_SWPXX_S1_OUT11;		// 1	57	87
	TU8 Generic_SWPXX_S2_OUT0;		// 1	58	88
	TU8 Generic_SWPXX_S2_OUT1;		// 1	59	89
	TU8 Generic_SWPXX_S2_OUT2;		// 1	5A	90
	TU8 Generic_SWPXX_S2_OUT3;		// 1	5B	91
	TU8 Generic_SWPXX_S2_OUT4;		// 1	5C	92
	TU8 Generic_SWPXX_S2_OUT5;		// 1	5D	93
	TU8 Generic_SWPXX_S2_OUT6;		// 1	5E	94
	TU8 Generic_SWPXX_S2_OUT7;		// 1	5F	95
	TU8 Generic_SWPXX_S2_OUT8;		// 1	60	96
	TU8 Generic_SWPXX_S2_OUT9;		// 1	61	97
	TU8 Generic_SWPXX_S2_OUT10;		// 1	62	98
	TU8 Generic_SWPXX_S2_OUT11;		// 1	63	99
	TU8 Generic_SWPXX_S3_OUT0;		// 1	64	100
	TU8 Generic_SWPXX_S3_OUT1;		// 1	65	101
	TU8 Generic_SWPXX_S3_OUT2;		// 1	66	102
	TU8 Generic_SWPXX_S3_OUT3;		// 1	67	103
	TU8 Generic_SWPXX_S3_OUT4;		// 1	68	104
	TU8 Generic_SWPXX_S3_OUT5;		// 1	69	105
	TU8 Generic_SWPXX_S3_OUT6;		// 1	6A	106
	TU8 Generic_SWPXX_S3_OUT7;		// 1	6B	107
	TU8 Generic_SWPXX_S3_OUT8;		// 1	6C	108
	TU8 Generic_SWPXX_S3_OUT9;		// 1	6D	109
	TU8 Generic_SWPXX_S3_OUT10;		// 1	6E	110
	TU8 Generic_SWPXX_S3_OUT11;		// 1	6F	111
	TU16 Generic_SWPXX_Cab_Illumination;// 1	71	113
	TU8 Generic_SWPXX_spare0[9];		// 9	7A	122
	TU8 Generic_SWPXX_BKL;				// 1	7B	123
	TU8 Generic_SWPXX_spare1[3];		// 3	7E	126
	TU8 Generic_SWP1_SLEEP;				// 1	7F	127
}TstMAI_Generic_SWP_IOCard;

// RAM / ZED definition
//----------------------
typedef struct
{
    TU8 DulAPI_Addr_SWP_Backlight;
    TU8 Spare0;
    TU16 DulAPI_Addr_SWP_Gene_BacklightVal;
    TstMAI_Specific_SWP_IOCard Specific_SWP_IOCard[CucSWP_iNbSWPMax];
	TstMAI_Generic_SWP_IOCard  Generic_SWP_IOCard[CucSWP_iNbSWPMax];
}TstWZed_eSWP;

////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////
#define DpstWZed_eSWP ((TstWZed_eSWP*)DU32ZED_Addr_SWP_Start)

////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////

#endif	/*	SWP_API_INCLUDE	*/


