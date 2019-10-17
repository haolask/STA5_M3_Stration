/*****************************************************************************
T5rtclk.c : real time clock / calendar - CAN BE CHANGED FOR PORT PURPOSE
(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/*****************************************************************************
T5RtClk_GetString
read the current date/time and stores it in a string
Parameters:
    lType: wished format:
        0: current date: 'YYYY/MM/DD'
        1: current time: 'HH:MM:SS'
        2: day of the week
    pString (OUT) buffer where to store the string
return: output string length
*****************************************************************************/

#ifdef T5DEF_RTCLOCKSUPPORTED

T5_BYTE T5RtClk_GetString (T5_LONG lType, T5_PTCHAR pString)
{
    return 0;
}

#endif /*T5DEF_RTCLOCKSUPPORTED*/

/*****************************************************************************
T5RtClk_GetCurDateStamp
returns the current date stamp
return: current date expressed on a 32 bit word:
    bits 0..7   day of the month
    bits 8..15  month of the year
    bits 16..31 year
*****************************************************************************/

#ifdef T5DEF_RTCLOCKSTAMP

T5_DWORD T5RtClk_GetCurDateStamp (void)
{
    return 0L;
}

#endif /*T5DEF_RTCLOCKSTAMP*/

/*****************************************************************************
T5RtClk_GetCurTimeStamp
returns the current time stamp
return: current time of the day: number of milliseconds elapsed since 00:00:00
*****************************************************************************/

#ifdef T5DEF_RTCLOCKSTAMP

T5_DWORD T5RtClk_GetCurTimeStamp (void)
{
    return 0L;
}

#endif /*T5DEF_RTCLOCKSTAMP*/

/*****************************************************************************
T5RtClk_GetCurDateTimeStamp
get advanced current date/time information
Parameters:
    bLocal (IN) TRUE if local time wished (FALSE for GMT)
    pdwDate (OUT) date stamp on 32 bits:
        bits 0..7   day of the month
        bits 8..15  month of the year
        bits 16..31 year
    pdwTime (OUT) time stamp: number of milliseconds elapsed since 00:00:00
    pbDST (OUT) TRUE if Daylight Saving is ON
*****************************************************************************/

#ifdef T5DEF_RTCLOCKSTAMPENH

void T5RtClk_GetCurDateTimeStamp (T5_BOOL bLocal, T5_PTDWORD pdwDate,
                                  T5_PTDWORD pdwTime, T5_PTBOOL pbDST)
{
    if (pdwDate != NULL)
    {
        *pdwDate = 0L;
    }

    if (pdwTime != NULL)
    {
        *pdwTime = 0L;
    }

    if (pbDST != NULL)
    {
        *pbDST = FALSE;
    }
}

#endif /*T5DEF_RTCLOCKSTAMPENH*/

/* eof **********************************************************************/
