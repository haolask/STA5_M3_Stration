/*****************************************************************************
T5VMbsmp.c : digital sampling trace

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_BSAMPLING

/****************************************************************************/
/* static services */

static void _T5VMBsmp_UpdateStamp (T5PTR_DBBSAMPLING pDef);

/****************************************************************************/
/* flags */

/* flags defined in tic.h
T5BSMP_ACTIVE       main activation flag
T5BSMP_AUTOSTART    start immediately
T5BSMP_ONESHOT      stop on buffer full
*/

#define T5BSMP_RUNNING  0x0100
#define T5BSMP_FULL     0x0200
#define T5BSMP_STARTED  0x0400

/****************************************************************************/
/* settings: offset in TIC code sequence */

#define T5SMPSET_FLAGS      0
#define T5SMPSET_PERIOD_H   1
#define T5SMPSET_PERIOD_L   2
#define T5SMPSET_ONTRIGGER  3
#define T5SMPSET_OFFTRIGGER 4
#define T5SMPSET_ONDELAY    5
#define T5SMPSET_OFFDELAY   6
#define T5SMPSET_NBVAR      7
#define T5SMPSET_VARS       8

/*****************************************************************************
T5VMBsmp_ResetContent
fully resets the definition of a sampling trace
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the trace (IGNORED - RESERVED FOR EXTENSIONS)
*****************************************************************************/

T5_RET T5VMBsmp_ResetContent (T5PTR_DB pDB, T5_WORD wIndex/*ignored*/)
{
    return T5VMBsmp_Set (pDB, wIndex, NULL);
}

/*****************************************************************************
T5VMBsmp_Set
sets the definition of a sampling trace
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the trace (IGNORED - RESERVED FOR EXTENSIONS)
    pCode (IN) tic style definition of the sampling trace
Return: OK or error
*****************************************************************************/

T5_RET T5VMBsmp_Set (T5PTR_DB pDB, T5_WORD wIndex/*ignored*/,
                     T5_PTWORD pCode)
{
    T5PTR_DBBSAMPLING pDef;
    T5_WORD i;
    T5PTR_DBSTATUS pStatus;
    T5_PTBYTE pData8;
    T5_UNUSED(wIndex);

    pDef = T5GET_DBBSAMPLING(pDB);
    pStatus = T5GET_DBSTATUS(pDB);
    pData8 = T5GET_DBDATA8(pDB);
    /* simply reset contents if pCode is NULL */
    T5_MEMSET (pDef, 0, sizeof (T5STR_DBBSAMPLING));
    if (pCode == NULL)
        return T5RET_OK;

    /* store settings */
    pDef->wFlags = pCode[T5SMPSET_FLAGS]
                   & ~(T5BSMP_RUNNING | T5BSMP_FULL | T5BSMP_STARTED);

    T5_COPYWORDSTOLONG ((T5_PTBYTE)(&(pDef->dwPeriod)),
                        (T5_PTBYTE)(pCode+T5SMPSET_PERIOD_H));
    pDef->wOnTrigger = pCode[T5SMPSET_ONTRIGGER];
    pDef->wOffTrigger = pCode[T5SMPSET_OFFTRIGGER];
    pDef->wOnDelay = pCode[T5SMPSET_ONDELAY];
    pDef->wOffDelay = pCode[T5SMPSET_OFFDELAY];
    for (i=0; i< 8 && i<pCode[T5SMPSET_NBVAR]; i++)
        pDef->wIndex[i] = pCode[T5SMPSET_VARS+i];

    if (pDef->wOnTrigger)
        pDef->bOnPrev = pData8[pDef->wOnTrigger];
    if (pDef->wOffTrigger)
        pDef->bOffPrev = pData8[pDef->wOffTrigger];

    T5TRACE ("Setting sampling trace:");
    T5TRACEDW ("  Flags", pDef->wFlags);
    T5TRACEDW ("  Period", pDef->dwPeriod);
    T5TRACEDW ("  OnTrigger", pDef->wOnTrigger);
    T5TRACEDW ("  OffTrigger", pDef->wOffTrigger);
    T5TRACEDW ("  OnDelay", pDef->wOnDelay);
    T5TRACEDW ("  OffDelay", pDef->wOffDelay);
    for (i=0; i<8 && pDef->wIndex[i] != 0; i++)
        T5TRACEDW ("  Spied", pDef->wIndex[i]);

    /* autostart */
    if ((pDef->wFlags & T5BSMP_ACTIVE) != 0
        && (pDef->wFlags & T5BSMP_AUTOSTART) != 0)
    {
        pDef->wFlags |= T5BSMP_RUNNING;
        pDef->wFlags |= T5BSMP_STARTED;
    }

    /* store current stamping */
    if (pDef->dwPeriod != 0L)
        pDef->dwLastDate = pStatus->dwTimStamp;

    return T5RET_OK;
}

/*****************************************************************************
T5VMBsmp_Ctl
Strats or stops a sampling trace
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the trace (IGNORED - RESERVED FOR EXTENSIONS)
    wCtl (IN) TRUE=start / FALSE=stop
*****************************************************************************/

void T5VMBsmp_Ctl (T5PTR_DB pDB, T5_WORD wIndex/*ignored*/, T5_WORD wCtl)
{
    T5PTR_DBBSAMPLING pDef;
    T5PTR_DBSTATUS pStatus;
    T5_UNUSED(wIndex);

    pDef = T5GET_DBBSAMPLING(pDB);
    pStatus = T5GET_DBSTATUS(pDB);
    /* return if no sampling defined */
    if ((pDef->wFlags & T5BSMP_ACTIVE) == 0)
        return;
    /* start */
    if (wCtl && (pDef->wFlags & T5BSMP_STARTED) == 0)
    {
        pDef->wFlags |= T5BSMP_RUNNING;
        pDef->wFlags |= T5BSMP_STARTED;
        T5TRACE("BSmp starts!");
        if (pDef->dwPeriod != 0L)
            pDef->dwLastDate = pStatus->dwTimStamp;
    }
    /* or stop */
    else if (!wCtl && (pDef->wFlags & T5BSMP_RUNNING) != 0)
    {
        pDef->wFlags &= ~T5BSMP_RUNNING;
        _T5VMBsmp_UpdateStamp (pDef);
        T5TRACE("BSmp stops!");
    }
}

/*****************************************************************************
T5VMBsmp_Activate
activates a sampling trace (supposed to be called on each cycle)
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the trace (IGNORED - RESERVED FOR EXTENSIONS)
*****************************************************************************/

void T5VMBsmp_Activate (T5PTR_DB pDB, T5_WORD wIndex/*ignored*/)
{
    T5PTR_DBBSAMPLING pDef;
    T5_BYTE bSample, bMask, bVar;
    T5_PTBYTE pData8;
    T5_WORD i;
    T5PTR_DBSTATUS pStatus;
    T5_DWORD dwElapsed;
    T5_UNUSED(wIndex);

    pDef = T5GET_DBBSAMPLING(pDB);
    pStatus = T5GET_DBSTATUS(pDB);
    pData8 = T5GET_DBDATA8(pDB);
    /* return if no sampling defined */
    if ((pDef->wFlags & T5BSMP_ACTIVE) == 0)
        return;

    /* manage start condition */
    if (pDef->wOnTrigger != 0 && (pDef->wFlags & T5BSMP_STARTED) == 0)
    {
        bVar = pData8[pDef->wOnTrigger];
        if ((pDef->wFlags & T5BSMP_RUNNING) == 0
            && bVar != pDef->bOnPrev
            && ((bVar != 0 && (pDef->wFlags & T5BSMP_ONFALL) == 0)
                || (bVar == 0 && (pDef->wFlags & T5BSMP_ONFALL) != 0)))
        {
            if (pDef->wOnDelay != 0)
            {
                pDef->dwDelay = (T5_DWORD)(pDef->wOnDelay);
                if (pDef->dwPeriod != 0L)
                    pDef->dwDelay *= pDef->dwPeriod;
                else pDef->dwDelay += 1L; /* for this cycle */
            }
            else
            {
                pDef->wFlags |= T5BSMP_RUNNING;
                T5TRACE("BSmp starts!");
            }
            if (pDef->dwPeriod != 0L)
                pDef->dwLastDate = pStatus->dwTimStamp;
            /* start only once! */
            pDef->wFlags |= T5BSMP_STARTED;
        }
        pDef->bOnPrev = bVar;
    }

    /* manage start delay */
    if ((pDef->wFlags & T5BSMP_RUNNING) == 0
        && pDef->dwDelay != 0L)
    {
        if (pDef->dwPeriod == 0)
            pDef->dwDelay -= 1L;
        else
        {
            dwElapsed = T5_BOUNDTIME(pStatus->dwTimStamp - pDef->dwLastDate);
            pDef->dwLastDate = pStatus->dwTimStamp;
            if (dwElapsed > pDef->dwDelay)
                pDef->dwDelay -= dwElapsed;
            else
                pDef->dwDelay = 0L;
        }
        if (pDef->dwDelay == 0L)
        {
            pDef->wFlags |= T5BSMP_RUNNING;
            if (pDef->dwPeriod != 0L)
                pDef->dwLastDate = pStatus->dwTimStamp;
            T5TRACE("BSmp starts!");
        }
    }

    /* manage active sampling */
    if ((pDef->wFlags & T5BSMP_RUNNING) == 0)
        return;
    if (pDef->dwPeriod == 0L)
        pDef->dwLastDate += 1L;  /* time unit is the cycle */
    else
    {
        dwElapsed = T5_BOUNDTIME(pStatus->dwTimStamp - pDef->dwLastDate);
        if (dwElapsed >= pDef->dwPeriod)
            pDef->dwLastDate = pStatus->dwTimStamp;
        else
            return; /* wait for next tick */
    }

    /* sampling counter on 64 bits */
    pDef->dwCountL += 1L;
    if (!pDef->dwCountL)
        pDef->dwCountH += 1L;

    /* builds the sample */
    bSample = 0;
    bMask = 0x01;
    for (i=0; i<8 && pDef->wIndex[i] != 0; i++)
    {
        if (pData8[pDef->wIndex[i]])
            bSample |= bMask;
        bMask <<= 1;
    }
    /* register the sample in rolling buffer */
    pDef->bData[pDef->wpNext] = bSample;
    pDef->wpNext += 1;


    /*{
        char str[255], s[3];
        int i;

        T5TRACE ("NEW SAMPLE RECORDED");
        T5TRACEDW (" dwCountL", pDef->dwCountL);
        T5TRACEDW (" wpNext", pDef->wpNext);
        *str = '\0';
        for (i=0; i<20; i++)
        {
            sprintf (s, ".%02X", pDef->bData[i]);
            strcat (str, s);
        }
        T5TRACE (str);
    }*/

    if (pDef->wpNext >= T5MAX_BSAMPLING)
    {
        pDef->wFlags |= T5BSMP_FULL;
        if ((pDef->wFlags & T5BSMP_STOPONFULL) != 0)
            pDef->wFlags &= ~T5BSMP_RUNNING;
        else
            pDef->wpNext = 0;
    }
    
    /* manage stop delay */
    if (pDef->dwDelay != 0L)
    {
        if (pDef->dwPeriod == 0)
            pDef->dwDelay -= 1L;
        else
        {
            dwElapsed = T5_BOUNDTIME(pStatus->dwTimStamp - pDef->dwLastDate);
            if (dwElapsed > pDef->dwDelay)
                pDef->dwDelay -= dwElapsed;
            else
                pDef->dwDelay = 0L;
        }
        if (pDef->dwDelay == 0L)
        {
            pDef->wFlags &= ~T5BSMP_RUNNING;
            _T5VMBsmp_UpdateStamp (pDef);
            T5TRACE("BSmp stops!");
        }
    }
    /* manage stop condition */
    else if (pDef->wOffTrigger != 0)
    {
        bVar = pData8[pDef->wOffTrigger];
        if (bVar != pDef->bOffPrev
            && ((bVar != 0 && (pDef->wFlags & T5BSMP_OFFFALL) == 0)
                || (bVar == 0 && (pDef->wFlags & T5BSMP_OFFFALL) != 0)))
        {
            if (pDef->wOffDelay != 0)
            {
                pDef->dwDelay = (T5_DWORD)(pDef->wOffDelay);
                if (pDef->dwPeriod != 0L)
                    pDef->dwDelay *= pDef->dwPeriod;
            }
            else
            {
                pDef->wFlags &= ~T5BSMP_RUNNING;
                _T5VMBsmp_UpdateStamp (pDef);
                T5TRACE("BSmp stops!");
            }
            if (pDef->dwPeriod != 0L)
                pDef->dwLastDate = pStatus->dwTimStamp;
            /* start only once! */
        }
        pDef->bOffPrev = bVar;
    }
}

/*****************************************************************************
T5VMBsmp_GetSetting / GetSettingSize
fills a comm buffer with current settings of a sampling trace
and/or returns the frame length
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the trace (IGNORED - RESERVED FOR EXTENSIONS)
    pFrame (OUT) buffer where to copy the settings
Return: size of the frame
Frame:
    T5_WORD  wLen;
    T5_WORD  wFlags;
    T5_DWORD dwPeriod;              
    T5_WORD  wOnDelay;              
    T5_WORD  wOffDelay;             
    T5_WORD  wOnTrigger;            
    T5_WORD  wOffTrigger;           
    T5_WORD  wIndex[T5DBMAX_SIGNAL];
*****************************************************************************/

#define _T5SIZE (4 + ((6+T5DBMAX_SIGNAL) * 2))

T5_WORD T5VMBsmp_GetSettingsSize (T5PTR_DB pDB, T5_WORD wIndex/*ignored*/)
{
    T5_UNUSED(pDB);
    T5_UNUSED(wIndex);
    return _T5SIZE;
}

T5_WORD T5VMBsmp_GetSettings (T5PTR_DB pDB, T5_WORD wIndex/*ignored*/,
                              T5_PTR pFrame)
{
    T5_PTBYTE pDst;
    T5PTR_DBBSAMPLING pDef;
    T5_WORD i, wLen;
    T5_UNUSED(wIndex);

    T5TRACE (">>Srv gets BSMP settings");

    pDef = T5GET_DBBSAMPLING(pDB);
    pDst = (T5_PTBYTE)pFrame;

    wLen = _T5SIZE - 2;
    T5_COPYFRAMEWORD(pDst, &wLen);
    pDst += 2;
    T5_COPYFRAMEWORD(pDst, &(pDef->wFlags));
    pDst += 2;
    T5_COPYFRAMEDWORD(pDst, &(pDef->dwPeriod));
    pDst += 4;
    T5_COPYFRAMEWORD(pDst, &(pDef->wOnDelay));
    pDst += 2;
    T5_COPYFRAMEWORD(pDst, &(pDef->wOffDelay));
    pDst += 2;
    T5_COPYFRAMEWORD(pDst, &(pDef->wOnTrigger));
    pDst += 2;
    T5_COPYFRAMEWORD(pDst, &(pDef->wOffTrigger));
    pDst += 2;
    for (i=0; i<T5DBMAX_SIGNAL; i++)
    {
        T5_COPYFRAMEWORD(pDst, &(pDef->wIndex[i]));
        pDst += 2;
    }

    return _T5SIZE;
}

#undef _T5SIZE

/*****************************************************************************
T5VMBsmp_GetData / GetDataSize
fills a comm buffer with the contents of a sampling trace
and/or returns the frame length
Parameters:
    pDB (IN) pointer to the data base
    wIndex (IN) index of the trace (IGNORED - RESERVED FOR EXTENSIONS)
    pFrame (OUT) buffer where to copy the contents of the trace
Return: size of the frame
Frame:
    T5_WORD  wLen;
    T5_DWORD dwCountH;
    T5_DWORD dwCountL;
    T5_WORD  wFlags;
    T5_WORD  wpNext;
    T5_DWORD dwDateStamp;
    T5_DWORD dwTimeStamp;
    T5_BYTE  bData[T5MAX_BSAMPLING];
*****************************************************************************/

#define _T5SIZE (22 + T5MAX_BSAMPLING)

T5_WORD T5VMBsmp_GetDataSize (T5PTR_DB pDB, T5_WORD wIndex/*ignored*/)
{
    T5_UNUSED(pDB);
    T5_UNUSED(wIndex);
    return _T5SIZE;
}

T5_WORD T5VMBsmp_GetData (T5PTR_DB pDB, T5_WORD wIndex/*ignored*/,
                          T5_PTR pFrame)
{
    T5_PTBYTE pDst;
    T5PTR_DBBSAMPLING pDef;
    T5_WORD wLen;
    T5_UNUSED(wIndex);

    pDef = T5GET_DBBSAMPLING(pDB);
    pDst = (T5_PTBYTE)pFrame;

    if ((pDef->wFlags & T5BSMP_RUNNING) != 0)
        _T5VMBsmp_UpdateStamp (pDef);

    wLen = _T5SIZE - 2;
    T5_COPYFRAMEWORD(pDst, &wLen);
    pDst += 2;
    T5_COPYFRAMEDWORD(pDst, &(pDef->dwCountH));
    pDst += 4;
    T5_COPYFRAMEDWORD(pDst, &(pDef->dwCountL));
    pDst += 4;
    T5_COPYFRAMEWORD(pDst, &(pDef->wFlags));
    pDst += 2;
    T5_COPYFRAMEWORD(pDst, &(pDef->wpNext));
    pDst += 2;
    T5_COPYFRAMEDWORD(pDst, &(pDef->dwDateStamp));
    pDst += 4;
    T5_COPYFRAMEDWORD(pDst, &(pDef->dwTimeStamp));
    pDst += 4;
    T5_MEMCPY (pDst, pDef->bData, T5MAX_BSAMPLING);

    return _T5SIZE;
}

#undef _T5SIZE

/*****************************************************************************
_T5VMBsmp_UpdateStamp
update the date/time stamp of a sampling trace
Parameters:
    pDef (IN) pointer to the sampling trace buffer
Notes:
The time stamp is updated:
    - when the sampling trace is defined (reset to 0)
    - when the trace stops (stamp of the last sample)
    - each time sampling data is read if the trace is active
*****************************************************************************/

static void _T5VMBsmp_UpdateStamp (T5PTR_DBBSAMPLING pDef)
{
#ifdef T5DEF_RTCLOCKSTAMP
    pDef->dwDateStamp = T5RtClk_GetCurDateStamp ();
    pDef->dwTimeStamp = T5RtClk_GetCurTimeStamp ();
#endif /*T5DEF_RTCLOCKSTAMP*/
}

/****************************************************************************/

#endif /*T5DEF_BSAMPLING*/

/* eof **********************************************************************/
