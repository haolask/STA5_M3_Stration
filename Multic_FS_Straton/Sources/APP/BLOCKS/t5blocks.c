/*****************************************************************************
T5Blocks.c :custom functions and FBs - TO BE COMPLETED WHEN PORTING
(c) COPALP 2002

Implementation of blocks and functions of the "Plus!" library
Functions and FBs of the Plus! library are pre-declared in the Workbench
and supported by the STRATON simulator:

Counters with edge detection: CTUr, CTDr, CTUDr

*****************************************************************************/

#include "t5vm.h"
#include "std_type.h"
#include "monitor.h"
#include "SleepCmd.h"
#include "Param.h"
#include "Dtc.h"
#include "Lin.h"
#include "FB_J1939.h"
#include "Rtc.H"
#include "SpecifCalib.H"
#include "BeepCmd.H"

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

/****************************************************************************/
/* list of registered functions and blocks */

typedef struct _s_pfb
{
    T5_PTCHAR szName;
    T5HND_FB pFb;
}
str_pfb;

static str_pfb PFB[] = {
    { (T5_PTCHAR)"PARAM_RD", PARAM_RD },
    { (T5_PTCHAR)"PARAM_WR", PARAM_WR },

    { (T5_PTCHAR)"DTC_WR", DTC_WR },
    { (T5_PTCHAR)"DTC_RD", DTC_RD },
    { (T5_PTCHAR)"DTC_LIST", DTC_LIST },
    { (T5_PTCHAR)"DTC_CLR", DTC_CLR },

    { (T5_PTCHAR)"SLEEP", SLEEP },

    { (T5_PTCHAR)"LIN_INIT", LIN_INIT },
    { (T5_PTCHAR)"LIN_TX", LIN_TX },
    { (T5_PTCHAR)"LIN_RQFRAME", LIN_RQFRAME },
    { (T5_PTCHAR)"LIN_RDDATA", LIN_RDDATA },

    { (T5_PTCHAR)"J1939_STATUS", J1939_STATUS },
    { (T5_PTCHAR)"RTC_RD", RTC_RD },
    { (T5_PTCHAR)"RTC_WR", RTC_WR },
    { (T5_PTCHAR)"SPECIFCALIB_RD", SPECIFCALIB_RD },
    { (T5_PTCHAR)"SPECIFCALIB_WR", SPECIFCALIB_WR },
    
    
    { (T5_PTCHAR)"RTC_TIMER_RD", RTC_TIMER_RD }, 
    { (T5_PTCHAR)"RTC_TIMER_WR", RTC_TIMER_WR },    
    { (T5_PTCHAR)"RTC_TIMER_CLR", RTC_TIMER_CLR },
    { (T5_PTCHAR)"RTC_ALARM_RD", RTC_ALARM_RD },
    { (T5_PTCHAR)"RTC_ALARM_WR", RTC_ALARM_WR },
    { (T5_PTCHAR)"RTC_ALARM_CLR", RTC_ALARM_CLR },
    { (T5_PTCHAR)"BEEP_PLAYBEEP", BEEP_PlayBeep},
    { (T5_PTCHAR)"BEEP_VOL", BEEP_VOL },
    { (T5_PTCHAR)"BEEP_STOP", BEEP_STOP },
    T5BARRAY_FBLIST_STATIC
    /* continue with your custom blocks and functions... */

{ T5_PTNULL, T5_PTNULL } };


/*****************************************************************************
T5Blocks_GetFirstID
start enumerating custom blocks
Parameters:
    pdwID (OUT) ID of the first block
return: TRUE if ok
*****************************************************************************/

T5_BOOL T5Blocks_GetFirstID (T5_PTDWORD pdwID)
{
    *pdwID = 0L;

    return (PFB[*pdwID].pFb != T5_PTNULL);
}

/*****************************************************************************
T5Blocks_GetNextID
enumerate custom blocks
Parameters:
    pdwID (IN/OUT) ID of the current block - set to next ID on output
return: TRUE if ok
*****************************************************************************/

T5_BOOL T5Blocks_GetNextID (T5_PTDWORD pdwID)
{
    *pdwID += 1L;

    if (*pdwID >= (sizeof (PFB) / sizeof (str_pfb)))
        return FALSE;

    return (PFB[*pdwID].pFb != T5_PTNULL);
}

/*****************************************************************************
T5Blocks_GetName
return the name of a custom block
Parameters:
    dwID (IN) ID of the block
return: pointer to block name (UPPERCASE) or NULL if bad ID
*****************************************************************************/

T5_PTCHAR T5Blocks_GetName (T5_DWORD dwID)
{
    if (dwID >= (sizeof (PFB) / sizeof (str_pfb)))
        return T5_PTNULL;

    return PFB[dwID].szName;
}

/*****************************************************************************
T5Blocks_GetProc
return the handler procedure of a custom block
Parameters:
    dwID (IN) ID of the block
return: pointer to block handler or NULL if bad ID
*****************************************************************************/

T5HND_FB T5Blocks_GetProc (T5_DWORD dwID)
{
    if (dwID >= (sizeof (PFB) / sizeof (str_pfb)))
        return T5_PTNULL;

    return PFB[dwID].pFb;
}

/* eof **********************************************************************/
