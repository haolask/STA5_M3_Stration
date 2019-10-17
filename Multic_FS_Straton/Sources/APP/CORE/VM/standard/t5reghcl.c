/*****************************************************************************
T5RegHcl.c : T5 Registry - host client interface

DO NOT ALTER THIS !

(c) COPALP 2008
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/

#ifdef T5DEF_REG

/*****************************************************************************
T5RegHcl_BuildGet
Build a "Get" question telegram
Parameters:
    pQuestion (OUT) pointer to buffer for question telegram
    dwOffset, dwSize (IN) question arguments
Return: length of question in bytes
*****************************************************************************/

T5_WORD T5RegHcl_BuildGet (T5_PTBYTE pQuestion, T5_DWORD dwOffset,
                           T5_DWORD dwSize)
{
    *pQuestion = T5REGFC_GET;
    T5_COPYFRAMEDWORD (pQuestion+1, &dwOffset);
    T5_COPYFRAMEDWORD (pQuestion+5, &dwSize);
    return 9;
}

/*****************************************************************************
T5RegHcl_BuildSave
Build a "Save" question telegram
Parameters:
    pQuestion (OUT) pointer to buffer for question telegram
Return: length of question in bytes
*****************************************************************************/

T5_WORD T5RegHcl_BuildSave (T5_PTBYTE pQuestion)
{
    *pQuestion = T5REGFC_SAVE;
    return 1;
}

/*****************************************************************************
T5RegHcl_BuildSelect
Build a "Select" question telegram
Parameters:
    pQuestion (OUT) pointer to buffer for question telegram
    dwItem, szPassword (IN) question arguments
Return: length of question in bytes
*****************************************************************************/

T5_WORD T5RegHcl_BuildSelect (T5_PTBYTE pQuestion, T5_DWORD dwItem,
                              T5_PTCHAR szPassword)
{
    *pQuestion = T5REGFC_SELECT;
    T5_COPYFRAMEDWORD (pQuestion+1, &dwItem);
    T5_STRCPY (pQuestion+5, szPassword);
    return (T5_WORD)(6 + T5_STRLEN(szPassword));
}

/*****************************************************************************
T5RegHcl_BuildRls
Build a "Release" question telegram
Parameters:
    pQuestion (OUT) pointer to buffer for question telegram
Return: length of question in bytes
*****************************************************************************/

T5_WORD T5RegHcl_BuildRls (T5_PTBYTE pQuestion)
{
    *pQuestion = T5REGFC_RLS;
    return 1;
}

/*****************************************************************************
T5RegHcl_BuildPut
Build a "Put" question telegram
Parameters:
    pQuestion (OUT) pointer to buffer for question telegram
    dwItem, pData (IN) question arguments
    bLen (IN) length of data
Return: length of question in bytes
*****************************************************************************/

T5_WORD T5RegHcl_BuildPut (T5_PTBYTE pQuestion, T5_DWORD dwItem,
                           T5_BYTE bLen, T5_PTBYTE pData)
{
    *pQuestion = T5REGFC_PUT;
    T5_COPYFRAMEDWORD (pQuestion+1, &dwItem);
    pQuestion[5] = bLen;
    T5_MEMCPY(pQuestion+6, pData, bLen);
    return (T5_WORD)(6 + bLen);
}

/*****************************************************************************
T5RegHcl_IsError
Check if an answer telegram is an error report
Parameters:
    pAnswer (IN) pointer to answer telegram
    pwCode (OUT) error code
Return: TRUE if error answer
*****************************************************************************/

T5_BOOL T5RegHcl_IsError (T5_PTBYTE pAnswer, T5_PTWORD pwCode)
{
    if (*pAnswer != T5REGFC_ERROR)
        return FALSE;
    if (pwCode != NULL)
        T5_COPYFRAMEWORD (pwCode, pAnswer+1);
    return TRUE;
}

/*****************************************************************************
T5RegHcl_ParseGet
Check if an answer telegram is a valid "Get" answer
Parameters:
    pAnswer (IN) pointer to answer telegram
    pdwOffset, pdwSize (OUT) answer arguments
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5RegHcl_ParseGet (T5_PTBYTE pAnswer, T5_PTDWORD pdwOffset,
                           T5_PTDWORD pdwSize)
{
    if (*pAnswer != T5REGFC_GET)
        return FALSE;
    if (pdwOffset != NULL)
        T5_COPYFRAMEDWORD (pdwOffset, pAnswer+1);
    if (pdwSize != NULL)
        T5_COPYFRAMEDWORD (pdwSize, pAnswer+5);
    return TRUE;
}

/*****************************************************************************
T5RegHcl_ParseGetData
Get pointer to data in a "Get" answer telegram
Parameters:
    pAnswer (IN) pointer to answer telegram
Return: pointer to data or NULL if fail
*****************************************************************************/

T5_PTBYTE T5RegHcl_ParseGetData (T5_PTBYTE pAnswer)
{
    if (*pAnswer != T5REGFC_GET)
        return NULL;
    return (pAnswer + 9);
}

/*****************************************************************************
T5RegHcl_ParseSave
Check if an answer telegram is a valid "Save" answer
Parameters:
    pAnswer (IN) pointer to answer telegram
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5RegHcl_ParseSave (T5_PTBYTE pAnswer)
{
    return (*pAnswer == T5REGFC_SAVE);
}

/*****************************************************************************
T5RegHcl_ParseSelect
Check if an answer telegram is a valid "Select" answer
Parameters:
    pAnswer (IN) pointer to answer telegram
Return: TRUE if OK
*****************************************************************************/

T5_BOOL T5RegHcl_ParseSelect (T5_PTBYTE pAnswer)
{
    return (*pAnswer == T5REGFC_SELECT);
}

/*****************************************************************************
T5RegHcl_ParseRls
Check if an answer telegram is a valid "Release" answer
Parameters:
    pAnswer (IN) pointer to answer telegram
Return: TRUE if OK
*****************************************************************************/

T5_WORD T5RegHcl_ParseRls (T5_PTBYTE pAnswer)
{
    return (*pAnswer == T5REGFC_RLS);
}

/*****************************************************************************
T5RegHcl_ParsePut
Check if an answer telegram is a valid "Put" answer
Parameters:
    pAnswer (IN) pointer to answer telegram
Return: TRUE if OK
*****************************************************************************/

T5_WORD T5RegHcl_ParsePut (T5_PTBYTE pAnswer)
{
    return (*pAnswer == T5REGFC_PUT);
}

/****************************************************************************/

#endif /*T5DEF_REG*/

/* eof **********************************************************************/
