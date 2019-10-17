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
//        Power XX protocol API
////////////////////////////////////////////////////////////////////////////////
// FILE : monitor_powerxx.h (HEADER)
//----------------------------------------------------------------------------
// DESC : Power XX protocol API
//----------------------------------------------------------------------------
// HIST :
// Version 		:$Revision: 581 $
// Url			:$HeadURL: https://forge2.actia.fr/bgse/8153_92_multic_s/trunk/PROJECTS/Multic-S_WORKSHOP/Multic-S_Straton/Include/monitor_powerxx.h $
// Updated the :$Date: 2015-05-13 01:17:17 +0800 (周三, 13 五月 2015) $
// By 			:$Author: pcouronn $
////////////////////////////////////////////////////////////////////////////////
#ifndef POWER_API_INCLUDE
#define POWER_API_INCLUDE

//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////
#include "std_type.h"


////////////////////////////////////////////////////////////////////////////////
// TYPEDEF
////////////////////////////////////////////////////////////////////////////////
#define CucPower_iNbPowerMax  10

// PowerXX Failures
//----------------------
typedef union{
   TU16 uiValue;
   struct{
      TU16 WakeUp                 :1;   // bit 0
      TU16 StatusMissing          :1;   // bit 1
      TU16 Unexpected             :1;   // bit 2
      TU16 WrongAppVariant        :1;   // bit 3
      TU16 WrongParamCRC          :1;   // bit 4
      TU16 WriteParamFail         :1;   // bit 5
      TU16 CANLink                :1;   // bit 6
      TU16 Restarted              :1;   // bit 7
      TU16 PowersWithSameAddress  :1;   // bit 8
      TU16 Inhibited              :1;   // bit 9
      TU16 Reseted                :1;   // bit 10
      TU16 b11                    :1;
      TU16 b12                    :1;
      TU16 b13                    :1;
      TU16 b14                    :1;
      TU16 b15                    :1;
   }Fail;
}TuMAI_Power_Failure;

// PowerXX IO Card
//---------------------------------------------------------

typedef struct{                  // Size HEX DEC
   TU32 FREQ1_TOFF;              // 4    0   0
   TU32 FREQ1_TON;               // 4    4   4
   TU32 FREQ2_TOFF;              // 4    8   8
   TU32 FREQ2_TON;               // 4    C   12
   TU16 Ana_OIL_DV;              // 2    10  16
   TU16 Ana_UIN1;                // 2    12  18
   TU16 Ana_UIN2;                // 2    14  20
   TU16 Ana_UIN3;                // 2    16  22
   TU16 Ana_UIN4;                // 2    18  24
   TU16 Ana_UIN5;                // 2    1A  26
   TU16 Ana_UIN6;                // 2    1C  28
   TU16 Ana_UIN7;                // 2    1E  30
   TU16 INA1;                    // 2    20  32
   TU16 INA2;                    // 2    22  34
   TU16 Current_BRIDGE1;         // 2    24  36
   TU16 Current_BRIDGE2;         // 2    26  38
   TU16 Current_INOUT09;         // 2    28  40
   TU16 Current_INOUT10;         // 2    2A  42
   TU16 Current_INOUT11;         // 2    2C  44
   TU16 Current_INOUT12;         // 2    2E  46
   TU16 Current_INOUT13;         // 2    30  48
   TU16 Current_INOUT14;         // 2    32  50
   TU16 Current_INOUT15;         // 2    34  52
   TU16 Current_INOUT16;         // 2    36  54
   TU16 Current_INOUT17;         // 2    38  56
   TU16 Current_INOUT18;         // 2    3A  58
   TU16 Current_OUT01;           // 2    3C  60
   TU16 Current_OUT02;           // 2    3E  62
   TU16 Current_OUT03;           // 2    40  64
   TU16 Current_OUT04;           // 2    42  66
   TU16 Current_OUT05;           // 2    44  68
   TU16 Current_OUT06;           // 2    46  70
   TU16 Current_OUT07;           // 2    48  72
   TU16 Current_OUT08;           // 2    4A  74
   TU16 Current_OUT19;           // 2    4C  76
   TU16 Current_OUT20;           // 2    4E  78
   TU16 Voltage_INOUT09;         // 2    50  80
   TU16 Voltage_INOUT10;         // 2    52  82
   TU16 Voltage_INOUT11;         // 2    54  84
   TU16 Voltage_INOUT12;         // 2    56  86
   TU16 Voltage_INOUT13;         // 2    58  88
   TU16 Voltage_INOUT14;         // 2    5A  90
   TU16 Voltage_INOUT15;         // 2    5C  92
   TU16 Voltage_INOUT16;         // 2    5E  94
   TU16 Voltage_INOUT17;         // 2    60  96
   TU16 Voltage_INOUT18;         // 2    62  98
   TU8  Bool_INA1;               // 1    64  100
   TU8  Bool_INA2;               // 1    65  101
   TU8  Bool_FREQ1;              // 1    66  102
   TU8  Bool_FREQ2;              // 1    67  103
   TU8  Bool_INOUT09;            // 1    68  104
   TU8  Bool_INOUT10;            // 1    69  105
   TU8  Bool_INOUT11;            // 1    6A  106
   TU8  Bool_INOUT12;            // 1    6B  107
   TU8  Bool_INOUT13;            // 1    6C  108
   TU8  Bool_INOUT14;            // 1    6D  109
   TU8  Bool_INOUT15;            // 1    6E  110
   TU8  Bool_INOUT16;            // 1    6F  111
   TU8  Bool_INOUT17;            // 1    70  112
   TU8  Bool_INOUT18;            // 1    71  113
   TU8  Bool_UIN1;               // 1    72  114
   TU8  Bool_UIN2;               // 1    73  115
   TU8  Bool_UIN3;               // 1    74  116
   TU8  Bool_UIN4;               // 1    75  117
   TU8  Bool_UIN5;               // 1    76  118
   TU8  Bool_UIN6;               // 1    77  119
   TU8  Bool_UIN7;               // 1    78  120
   TU8  Fail_OUT01;              // 1    79  121
   TU8  Fail_OUT02;              // 1    7A  122
   TU8  Fail_OUT03;              // 1    7B  123
   TU8  Fail_OUT04;              // 1    7C  124
   TU8  Fail_OUT05;              // 1    7D  125
   TU8  Fail_OUT06;              // 1    7E  126
   TU8  Fail_OUT07;              // 1    7F  127
   TU8  Fail_OUT08;              // 1    80  128
   TU8  Fail_OUT19;              // 1    81  129
   TU8  Fail_OUT20;              // 1    82  130
   TU8  FailTyp_OUT01;           // 1    83  131
   TU8  FailTyp_OUT02;           // 1    84  132
   TU8  FailTyp_OUT03;           // 1    85  133
   TU8  FailTyp_OUT04;           // 1    86  134
   TU8  FailTyp_OUT05;           // 1    87  135
   TU8  FailTyp_OUT06;           // 1    88  136
   TU8  FailTyp_OUT07;           // 1    89  137
   TU8  FailTyp_OUT08;           // 1    8A  138
   TU8  FailTyp_OUT19;           // 1    8B  139
   TU8  FailTyp_OUT20;           // 1    8C  140
   TU8  Fail_INOUT09;            // 1    8D  141
   TU8  Fail_INOUT10;            // 1    8E  142
   TU8  Fail_INOUT11;            // 1    8F  143
   TU8  Fail_INOUT12;            // 1    90  144
   TU8  Fail_INOUT13;            // 1    91  145
   TU8  Fail_INOUT14;            // 1    92  146
   TU8  Fail_INOUT15;            // 1    93  147
   TU8  Fail_INOUT16;            // 1    94  148
   TU8  Fail_INOUT17;            // 1    95  149
   TU8  Fail_INOUT18;            // 1    96  150
   TU8  Fail_SENS_SUP1;          // 1    97  151
   TU8  Fail_SENS_SUP2;          // 1    98  152
   TU8  Fail_WIPER_Outputs;      // 1    99  153
   TU8  Fail_WIPER_Park;         // 1    9A  154
   TU8  Status_INA1;             // 1    9B  155
   TU8  Status_INA2;             // 1    9C  156
   TU8  Status_FREQ1;            // 1    9D  157
   TU8  Status_FREQ2;            // 1    9E  158
   TU8  Status_UIN1;             // 1    9F  159
   TU8  Status_UIN2;             // 1    A0  160
   TU8  Status_UIN3;             // 1    A1  161
   TU8  Status_UIN4;             // 1    A2  162
   TU8  Status_UIN5;             // 1    A3  163
   TU8  Status_UIN6;             // 1    A4  164
   TU8  Status_UIN7;             // 1    A5  165
   TU8  WK1;                     // 1    A6  166
   TU8  WK2;                     // 1    A7  167
   TU16 Cmd_SENS_SUP1;           // 2    A8  168
   TU16 Cmd_SENS_SUP2;           // 2    AA  170
   TU16 DutyCycle_INOUT09;       // 2    AC  172
   TU16 DutyCycle_INOUT10;       // 2    AE  174
   TU16 DutyCycle_INOUT11;       // 2    B0  176
   TU16 DutyCycle_INOUT12;       // 2    B2  178
   TU16 DutyCycle_INOUT13;       // 2    B4  180
   TU16 DutyCycle_INOUT14;       // 2    B6  182
   TU16 DutyCycle_INOUT15;       // 2    B8  184
   TU16 DutyCycle_INOUT16;       // 2    BA  186
   TU16 DutyCycle_INOUT17;       // 2    BC  188
   TU16 DutyCycle_INOUT18;       // 2    BE  190
   TU16 DutyCycle_OUT01;         // 2    C0  192
   TU16 DutyCycle_OUT02;         // 2    C2  194
   TU16 DutyCycle_OUT03;         // 2    C4  196
   TU16 DutyCycle_OUT04;         // 2    C6  198
   TU16 DutyCycle_OUT05;         // 2    C8  200
   TU16 DutyCycle_OUT06;         // 2    CA  202
   TU16 DutyCycle_OUT07;         // 2    CC  204
   TU16 DutyCycle_OUT08;         // 2    CE  206
   TU16 DutyCycle_OUT19;         // 2    D0  208
   TU16 DutyCycle_OUT20;         // 2    D2  210
   TU16 Cmd_WIPER;               // 2    D4  212
   TU8  Cmd_INOUT09;             // 1    D6  214
   TU8  Cmd_INOUT10;             // 1    D7  215
   TU8  Cmd_INOUT11;             // 1    D8  216
   TU8  Cmd_INOUT12;             // 1    D9  217
   TU8  Cmd_INOUT13;             // 1    DA  218
   TU8  Cmd_INOUT14;             // 1    DB  219
   TU8  Cmd_INOUT15;             // 1    DC  220
   TU8  Cmd_INOUT16;             // 1    DD  221
   TU8  Cmd_INOUT17;             // 1    DE  222
   TU8  Cmd_INOUT18;             // 1    DF  223
   TU8  Cmd_OUT01;               // 1    E0  224
   TU8  Cmd_OUT02;               // 1    E1  225
   TU8  Cmd_OUT03;               // 1    E2  226
   TU8  Cmd_OUT04;               // 1    E3  227
   TU8  Cmd_OUT05;               // 1    E4  228
   TU8  Cmd_OUT06;               // 1    E5  229
   TU8  Cmd_OUT07;               // 1    E6  230
   TU8  Cmd_OUT08;               // 1    E7  231
   TU8  Cmd_OUT19;               // 1    E8  232
   TU8  Cmd_OUT20;               // 1    E9  233
   TU8  Cmd_SLEEP;               // 1    EA  234
   TU8  Dir_OUT01;               // 1    EB  235
   TU8  Dir_OUT02;               // 1    EC  236
   TU8  Dir_OUT03;               // 1    ED  237
   TU8  Dir_OUT04;               // 1    EE  238
   TU8  Cmd_OIL;                 // 1    EF  239   Power 66A:           Power 66B:
   TU8  OIL_Available;           // 1    F0  240   OIL_Available        0
   TU8  Fail_VBAT;               // 1    F1  241   Fail_VBAT            Fail_V2
   TU8  Fail_V1;                 // 1    F2  242   0                    Fail_V1
   TU8  Fail_VAMS;               // 1    F3  243   Fail_VAMS            0
   TU8  spare_F4;                // 1    F4  244
   TU8  spare_F5;                // 1    F5  245
   TU8  spare_F6;                // 1    F6  246
   TU8  CANLinkFailure;          // 1    F7  247
   TU16 ParameterCRC;            // 2    F8  248
   TU8  ApplicationVariante;     // 1    FA  250
   TU8  ApplicationVersion;      // 1    FB  251
   TU8  HardwareConf;            // 1    FC  252
   TU8  HardwareType;            // 1    FD  253
   TU8  WakeUpOrigin;            // 1    FE  254
   TU8  spare_FF;                // 1    FF  255 =MAX! (256 bytes in total)
}TstMAI_Power33_IOCard;

// RAM / ZED definition
//----------------------
typedef struct
{
	TuMAI_Power_Failure		Power15_Failures[CucPower_iNbPowerMax];						// 20		DulAPI_Addr_PowerXX_Failures
	TuMAI_Power_Failure		Power33_Failures[CucPower_iNbPowerMax];						// 20		DulAPI_Addr_PowerXX_Failures
	TuMAI_Power_Failure		Power66_Failures[CucPower_iNbPowerMax];						// 20		DulAPI_Addr_PowerXX_Failures
	TU8						Power15_NbOnNetwork;					// 1		DulAPI_Addr_PowerXX_NbOnNetwork
	TU8						Power33_NbOnNetwork;					// 1		DulAPI_Addr_PowerXX_NbOnNetwork
	TU8						Power66_NbOnNetwork;					// 1		DulAPI_Addr_PowerXX_NbOnNetwork
	TU8						Spare;									// 1		DulAPI_Addr_PowerXX_NbOnNetwork
	TstMAI_Power33_IOCard	Power33_IOCard[CucPower_iNbPowerMax];	// 2560		DulAPI_Addr_Power33_IOCard
	TstMAI_Power33_IOCard	Power15_IOCard[CucPower_iNbPowerMax];	// 2560		DulAPI_Addr_Power15_IOCard
	TstMAI_Power33_IOCard	Power66A_IOCard[CucPower_iNbPowerMax];	// 2560		DulAPI_Addr_Power66A_IOCard
	TstMAI_Power33_IOCard	Power66B_IOCard[CucPower_iNbPowerMax];	// 2560		DulAPI_Addr_Power66B_IOCard
}TstWZed_ePowerXX;

////////////////////////////////////////////////////////////////////////////////
// DEFINITION
////////////////////////////////////////////////////////////////////////////////
#define DpstWZed_ePowerXX ((TstWZed_ePowerXX*)DU32ZED_Addr_PowerXX_Start)

////////////////////////////////////////////////////////////////////////////////
// EXPORT
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PROTO
////////////////////////////////////////////////////////////////////////////////

#endif	/*	POWER_API_INCLUDE	*/
