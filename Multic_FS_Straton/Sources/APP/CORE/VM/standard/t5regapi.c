/*****************************************************************************
T5RegApi.c : T5 Registry - application level services

DO NOT ALTER THIS !

(c) COPALP 2008
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/

#ifdef T5DEF_REG

/****************************************************************************/

static T5_WORD _T5RegApi_ServeError (T5_PTBYTE pAnswer, T5_WORD wErr);
static T5_WORD _T5RegApi_ServeGet (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                                   T5_PTBYTE pAnswer);
static T5_WORD _T5RegApi_ServeSave (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                                    T5_PTBYTE pAnswer, T5_PTCHAR szPath);
static T5_WORD _T5RegApi_ServeSelect (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                                      T5_PTBYTE pAnswer);
static T5_WORD _T5RegApi_ServeRls (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                                   T5_PTBYTE pAnswer);
static T5_WORD _T5RegApi_ServePut (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                                   T5_PTBYTE pAnswer);

/*****************************************************************************
T5RegApi_Open
Initiate and try to load the registry
Parameters:
    pApi (IN) pointer application registry object
    szPath (IN) null terminated string argument - used for file access
    pCheckEndian (IN) check Endian if TRUE
Return: TRUE if OK (registry loaded and checked)
*****************************************************************************/

T5_BOOL T5RegApi_Open (T5PTR_T5REGAPI pApi, T5_PTCHAR szPath,
                       T5_BOOL bCheckEndian)
{
    T5_DWORD f, dwSize;
    T5_BOOL bRet;

    T5_MEMSET (pApi, 0, sizeof (T5STR_T5REGAPI));
    dwSize = T5RegOem_GetFileSize (T5REGOEM_REG, szPath);
    if (!dwSize)
        return FALSE;

    f = T5RegOem_OpenRead (T5REGOEM_REG, szPath);
    if (!f)
        return FALSE;

    bRet = FALSE;
    pApi->dwram = T5RegOem_AllocRAM (dwSize);
    if (pApi->dwram)
    {
        pApi->pReg = T5RegOem_LinkRAM (pApi->dwram);
        if (pApi->pReg == NULL)
            T5RegOem_FreeRAM (pApi->dwram, NULL);
        else
        {
            bRet = T5RegOem_Read (f, dwSize, (T5_PTBYTE)(pApi->pReg));
            bRet &= T5Reg_CheckRegistry (pApi->pReg, dwSize, bCheckEndian);
            if (!bRet)
            {
                T5RegOem_UnlinkRAM (pApi->dwram, pApi->pReg);
                T5RegOem_FreeRAM (pApi->dwram, pApi->pReg);
            }
        }
    }
    T5RegOem_Close (f);

    if (!bRet)
    {
        T5_MEMSET (pApi, 0, sizeof (T5STR_T5REGAPI));
    }
    else
    {
        T5Reg_UnlockRegistry (pApi->pReg);
    }

    return bRet;
}

/*****************************************************************************
T5RegApi_Close
Release the registry
Parameters:
    pApi (IN) pointer application registry object
*****************************************************************************/

void T5RegApi_Close (T5PTR_T5REGAPI pApi)
{
    if (pApi->pReg)
        T5RegOem_UnlinkRAM (pApi->dwram, pApi->pReg);
    if (pApi->dwram)
        T5RegOem_FreeRAM (pApi->dwram, pApi->pReg);
    T5_MEMSET (pApi, 0, sizeof (T5STR_T5REGAPI));
}

/*****************************************************************************
T5RegApi_Save
Save the registry to bacjup support
Parameters:
    pApi (IN) pointer application registry object
    szPath (IN) null terminated string argument - used for file access
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5RegApi_Save (T5PTR_T5REGAPI pApi, T5_PTCHAR szPath)
{
    T5_DWORD f;
    T5_BOOL bRet;

    bRet = FALSE;
    if (pApi->pReg == NULL)
        return FALSE;

    T5Reg_LockRegistry (pApi->pReg);

#ifdef T5DEF_RTCLOCKSTAMP
    pApi->pReg->dwDate = T5RtClk_GetCurDateStamp ();
    pApi->pReg->dwTime = T5RtClk_GetCurTimeStamp ();
#endif /*T5DEF_RTCLOCKSTAMP*/
    T5Reg_UpdateCRC (pApi->pReg);

    f = T5RegOem_OpenWrite (T5REGOEM_REG, szPath, pApi->pReg->dwFullSize);
    if (f)
    {
        bRet = T5RegOem_Write (f, pApi->pReg->dwFullSize,
                               (T5_PTBYTE)(pApi->pReg));
        T5RegOem_Close (f);
    }

    T5Reg_UnlockRegistry (pApi->pReg);

    return bRet;
}

/*****************************************************************************
T5RegApi_ServeT5CS
Serves a request from T5CS communication server
Parameters:
    pApi (IN) pointer application registry object
    pCS (IN) pointer to T5CS data block
    wCaller (IN) identifier of the client in CS
    szPath (IN) null terminated string argument - used for file access
*****************************************************************************/

#ifndef T5DEF_NOT5CS

void T5RegApi_ServeT5CS (T5PTR_T5REGAPI pApi, T5PTR_CS pCS,
                         T5_WORD wCaller, T5_PTCHAR szPath)
{
    T5_WORD wLen;
    T5_PTBYTE pIn, pOut;

    pIn = (T5_PTBYTE)T5CS_GetRequestFrame (pCS, wCaller);
    pOut = (T5_PTBYTE)T5CS_GetAnswerFrameBuffer (pCS, wCaller);
    wLen = T5RegApi_ServeT5 (pApi, pIn, pOut, szPath);
    T5CS_SendAnswerFrame (pCS, wCaller, wLen);
}

#endif /*T5DEF_NOT5CS*/

/*****************************************************************************
T5RegApi_ServeT5
Serve a request embedded in T5 protocol message
Parameters:
    pApi (IN) pointer application registry object
    pQuestion (IN) pointer to question telegram
    pAnswer (OUT) pointer buffer for answer telegram
    szPath (IN) null terminated string argument - used for file access
Return: length of answer in bytes
*****************************************************************************/

T5_WORD T5RegApi_ServeT5 (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                          T5_PTBYTE pAnswer, T5_PTCHAR szPath)
{
    T5_WORD wLen;

    wLen = T5RegApi_Serve (pApi, pQuestion + 5, pAnswer + 5, szPath) + 1;
    pAnswer[0] = pQuestion[0];
    pAnswer[1] = pQuestion[1];
    T5_COPYFRAMEWORD(pAnswer+2, &wLen);
    pAnswer[4] = pQuestion[4];
    return wLen + 4;
}

/*****************************************************************************
T5RegApi_Serve
Serve a request (T5REG protocol)
Parameters:
    pApi (IN) pointer application registry object
    pQuestion (IN) pointer to question telegram
    pAnswer (OUT) pointer buffer for answer telegram
    szPath (IN) null terminated string argument - used for file access
Return: length of answer in bytes
*****************************************************************************/

T5_WORD T5RegApi_Serve (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                        T5_PTBYTE pAnswer, T5_PTCHAR szPath)
{
    T5_WORD wLen;

    if (pApi == NULL || pApi->pReg == NULL)
        *pQuestion = '?';

    switch (*pQuestion)
    {
    case T5REGFC_GET :
        wLen = _T5RegApi_ServeGet (pApi, pQuestion, pAnswer);
        break;
    case T5REGFC_SAVE :
        wLen = _T5RegApi_ServeSave (pApi, pQuestion, pAnswer, szPath);
        break;
    case T5REGFC_SELECT :
        wLen = _T5RegApi_ServeSelect (pApi, pQuestion, pAnswer);
        break;
    case T5REGFC_RLS :
        wLen = _T5RegApi_ServeRls (pApi, pQuestion, pAnswer);
        break;
    case T5REGFC_PUT :
        wLen = _T5RegApi_ServePut (pApi, pQuestion, pAnswer);
        break;
    default :
        wLen = _T5RegApi_ServeError (pAnswer, T5REGERR_UNKNOWN);
        break;
    }
    return wLen;
}

/*****************************************************************************
_T5RegApi_ServeError
Format an error answer telegram
Parameters:
    pApi (IN) pointer application registry object
    pAnswer (OUT) pointer buffer for answer telegram
    wErr (IN) error code
Return: length of answer in bytes
*****************************************************************************/

static T5_WORD _T5RegApi_ServeError (T5_PTBYTE pAnswer, T5_WORD wErr)
{
    *pAnswer = T5REGFC_ERROR;
    T5_COPYFRAMEWORD (pAnswer+1, &wErr);
    return 3;
}

/*****************************************************************************
_T5RegApi_ServeGet
Serve a "GetSegment" request
Parameters:
    pApi (IN) pointer application registry object
    pQuestion (IN) pointer to question telegram
    pAnswer (OUT) pointer buffer for answer telegram
Return: length of answer in bytes
*****************************************************************************/

static T5_WORD _T5RegApi_ServeGet (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                                   T5_PTBYTE pAnswer)
{
    T5_DWORD dwOffset, dwSize;
    T5_PTBYTE pSeg;

    T5_COPYFRAMEDWORD (&dwOffset, pQuestion+1);
    T5_COPYFRAMEDWORD (&dwSize, pQuestion+5);

    if (dwOffset >= pApi->pReg->dwFullSize)
        return _T5RegApi_ServeError (pAnswer, T5REGERR_BADOFFSET);

    if (dwSize > T5REGMAX_REGGET)
        dwSize = T5REGMAX_REGGET;

    if ((dwOffset+dwSize) >= pApi->pReg->dwFullSize)
        dwSize = pApi->pReg->dwFullSize - dwOffset;

    pSeg = (T5_PTBYTE)(pApi->pReg);

    *pAnswer = T5REGFC_GET;
    T5_COPYFRAMEDWORD (pAnswer+1, &dwOffset);
    T5_COPYFRAMEDWORD (pAnswer+5, &dwSize);
    T5_MEMCPY(pAnswer+9, pSeg + dwOffset, dwSize);
    return (T5_WORD)(9 + dwSize);
}

/*****************************************************************************
_T5RegApi_ServeSave
Serve a "Save" request
Parameters:
    pApi (IN) pointer application registry object
    pQuestion (IN) pointer to question telegram
    pAnswer (OUT) pointer buffer for answer telegram
    szPath (IN) null terminated string argument - used for file access
Return: length of answer in bytes
*****************************************************************************/

static T5_WORD _T5RegApi_ServeSave (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                                    T5_PTBYTE pAnswer, T5_PTCHAR szPath)
{
    T5_UNUSED(pQuestion);
    if (!T5RegApi_Save (pApi, szPath))
        return _T5RegApi_ServeError (pAnswer, T5REGERR_SAVE);

    *pAnswer = T5REGFC_SAVE;
    return 1;
}

/*****************************************************************************
_T5RegApi_ServeSelect
Serve a "Select" request
Parameters:
    pApi (IN) pointer application registry object
    pQuestion (IN) pointer to question telegram
    pAnswer (OUT) pointer buffer for answer telegram
Return: length of answer in bytes
*****************************************************************************/

static T5_WORD _T5RegApi_ServeSelect (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                                      T5_PTBYTE pAnswer)
{
    T5_DWORD dwItem;
    T5_PTCHAR szPassword;
    T5PTR_T5REGITEM pItem;

    pApi->pSelect = NULL;
    pApi->szPassword[0] = '\0';

    T5_COPYFRAMEDWORD (&dwItem, pQuestion+1);
    szPassword = (T5_PTCHAR)(pQuestion + 5);

    pItem = T5Reg_GetItem (pApi->pReg, dwItem);
    if (pItem == NULL)
        return _T5RegApi_ServeError (pAnswer, T5REGERR_BADID);

    if (!T5Reg_CheckPut (pApi->pReg, pItem, szPassword))
        return _T5RegApi_ServeError (pAnswer, T5REGERR_PSW);

    pApi->pSelect = pItem;
    T5_STRCPY (pApi->szPassword, szPassword);

    pAnswer[0] = T5REGFC_SELECT;
    T5_COPYFRAMEDWORD (pAnswer+1, &dwItem);
    return 5;
}

/*****************************************************************************
_T5RegApi_ServeRls
Serve a "Release" request
Parameters:
    pApi (IN) pointer application registry object
    pQuestion (IN) pointer to question telegram
    pAnswer (OUT) pointer buffer for answer telegram
Return: length of answer in bytes
*****************************************************************************/

static T5_WORD _T5RegApi_ServeRls (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                                   T5_PTBYTE pAnswer)
{
    T5_UNUSED(pQuestion);
    pApi->pSelect = NULL;
    pApi->szPassword[0] = '\0';

    pAnswer[0] = T5REGFC_RLS;
    return 1;
}

/*****************************************************************************
_T5RegApi_ServePut
Serve a "Put" request
Parameters:
    pApi (IN) pointer application registry object
    pQuestion (IN) pointer to question telegram
    pAnswer (OUT) pointer buffer for answer telegram
Return: length of answer in bytes
*****************************************************************************/

static T5_WORD _T5RegApi_ServePut (T5PTR_T5REGAPI pApi, T5_PTBYTE pQuestion,
                                   T5_PTBYTE pAnswer)
{
    T5_BOOL bRet;
    T5_DWORD dwItem;
    T5_BYTE bLen;
    T5_PTBYTE data;

    T5_COPYFRAMEDWORD (&dwItem, pQuestion+1);
    bLen = pQuestion[5];
    data = pQuestion + 6;

    if (pApi->pSelect == NULL)
        return _T5RegApi_ServeError (pAnswer, T5REGERR_NOSEL);
    if (pApi->pSelect->dwID != dwItem)
        return _T5RegApi_ServeError (pAnswer, T5REGERR_BADSEL);

    if (pApi->pSelect->bTicType == T5C_STRING)
        bRet = T5Reg_PutString (pApi->pReg, pApi->pSelect,
                                data, pApi->szPassword);
    else
        bRet = T5Reg_PutValue (pApi->pReg, pApi->pSelect,
                                data, pApi->szPassword);

    if (!bRet)
        return _T5RegApi_ServeError (pAnswer, T5REGERR_PUT);

    pApi->pSelect = NULL;
    pApi->szPassword[0] = '\0';

    pAnswer[0] = T5REGFC_PUT;
    T5_COPYFRAMEDWORD (pAnswer+1, &dwItem);
    return 5;
}

/*****************************************************************************
T5RegApi_Read
Read a parameter by string
Parameters:
    pApi (IN) pointer application registry object
    szPath (IN) parameter pathname
    szValue (OUT) filled on output with value as string
Return: TRUE if successful
*****************************************************************************/

T5_BOOL T5RegApi_Read (T5PTR_T5REGAPI pApi, T5_PTCHAR szPath, T5_PTCHAR szValue)
{
    return T5Reg_Read (pApi->pReg, szPath, szValue);
}

/*****************************************************************************
T5RegApi_Write
Write a parameter by string
Parameters:
    pApi (IN) pointer application registry object
    szPath (IN) parameter pathname
    szPassword (IN) password in readable form
    szValue (IN) new value in string format
Return: TRUE if successful
*****************************************************************************/

T5_BOOL T5RegApi_Write (T5PTR_T5REGAPI pApi, T5_PTCHAR szPath,
                        T5_PTCHAR szPassword, T5_PTCHAR szValue)
{
    return T5Reg_Write (pApi->pReg, szPath, szPassword, szValue);
}

/****************************************************************************/

#endif /*T5DEF_REG*/

/* eof **********************************************************************/
