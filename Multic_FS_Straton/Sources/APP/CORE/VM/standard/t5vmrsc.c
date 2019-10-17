/*****************************************************************************
T5VMrsc.c :  manage system resources

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/
/* static services */

static T5_BOOL _T5Param_FindSection (T5_LONG f, T5_PTCHAR szSection);
static T5_BOOL _T5Param_FindEntry (T5_LONG f, T5_PTCHAR szEntry, T5_PTCHAR szBuffer);

/*****************************************************************************
T5VMRsc_GetFreeItem
Get a free system resource structure from the heap
Parameters:
    pDB (IN) pointer to the data base
return:
pointer to the free system resource structure - NULL if not found
*****************************************************************************/

T5PTR_DBSYSRSC T5VMRsc_GetFreeItem (T5PTR_DB pDB)
{
    T5_WORD i;
    T5PTR_DBSYSRSC pRsc, pFound;

    pRsc = T5GET_DBSYSRSC(pDB);
    pDB = &pDB[T5TB_SYSRSC];
    pFound = T5_PTNULL;
    for (i=0; pFound==T5_PTNULL && i<pDB->wNbItemAlloc; i++, pRsc++)
        if (pRsc->wUsed == 0) pFound = pRsc;
    return pFound;
}

/*****************************************************************************
T5VMRsc_FindData
find a system resource using its data handler
Parameters:
    pDB (IN) pointer to the data base
    dwObject (IN) searched data
return:
pointer to the system resource structure - NULL if not found
*****************************************************************************/

T5PTR_DBSYSRSC T5VMRsc_FindData (T5PTR_DB pDB, T5_DWORD dwObject)
{
    T5_WORD i;
    T5PTR_DBSYSRSC pRsc, pFound;

    pRsc = T5GET_DBSYSRSC(pDB);
    pDB = &pDB[T5TB_SYSRSC];
    pFound = T5_PTNULL;
    for (i=0; pFound==T5_PTNULL && i<pDB->wNbItemAlloc; i++, pRsc++)
        if (pRsc->wUsed && pRsc->dwObject == dwObject) pFound = pRsc;
    return pFound;
}

/*****************************************************************************
T5VMRsc_Exist
test if some system resource(s) is allocated
Parameters:
    pDB (IN) pointer to the data base
return:
TRUE if at least one resource defined
*****************************************************************************/

T5_BOOL T5VMRsc_Exist (T5PTR_DB pDB)
{
    T5_WORD i;
    T5PTR_DBSYSRSC pRsc;
    T5_BOOL bFound;

    pRsc = T5GET_DBSYSRSC(pDB);
    pDB = &pDB[T5TB_SYSRSC];
    bFound = FALSE;
    for (i=0; !bFound && i<pDB->wNbItemAlloc; i++, pRsc++)
        if (pRsc->wUsed) bFound = TRUE;
    return bFound;
}

/*****************************************************************************
T5VMRsc_FindArray
find a system resource using its data handler
Parameters:
    pDB (IN) pointer to the data base
    dwID (IN) non zero array ID
return:
pointer to the system resource structure - NULL if not found
*****************************************************************************/

T5PTR_DBSYSRSC T5VMRsc_FindArray (T5PTR_DB pDB, T5_LONG lID)
{
    T5_WORD i;
    T5PTR_DBSYSRSC pRsc, pFound;

    pRsc = T5GET_DBSYSRSC(pDB);
    pDB = &pDB[T5TB_SYSRSC];
    pFound = T5_PTNULL;
    for (i=0; pFound==T5_PTNULL && i<pDB->wNbItemAlloc; i++, pRsc++)
        if (pRsc->wUsed == (T5_WORD)lID && pRsc->wType == T5SYSRSC_ARRAY)
            pFound = pRsc;
    return pFound;
}

/*****************************************************************************
T5VMRsc_FindSocket
find a system resource using its data handler
Parameters:
    pDB (IN) pointer to the data base
    dwID (IN) non zero array ID
return:
pointer to the system resource structure - NULL if not found
*****************************************************************************/

T5PTR_DBSYSRSC T5VMRsc_FindSocket (T5PTR_DB pDB, T5_DWORD dwID)
{
    T5_WORD i;
    T5PTR_DBSYSRSC pRsc, pFound;

    pRsc = T5GET_DBSYSRSC(pDB);
    pDB = &pDB[T5TB_SYSRSC];
    pFound = T5_PTNULL;
    for (i=0; pFound==T5_PTNULL && i<pDB->wNbItemAlloc; i++, pRsc++)
        if (pRsc->wUsed != 0
            && pRsc->dwObject == dwID && pRsc->wType == T5SYSRSC_SOCKET)
            pFound = pRsc;
    return pFound;
}

/*****************************************************************************
T5VMRsc_FreeAll
free all allocated resources (called on shutdown)
Parameters:
    pDB (IN) pointer to the data base
*****************************************************************************/

void T5VMRsc_FreeAll (T5PTR_DB pDB)
{
    T5_WORD i;
    T5PTR_DBSYSRSC pRsc;

    pRsc = T5GET_DBSYSRSC(pDB);
    pDB = &pDB[T5TB_SYSRSC];
    for (i=0; i<pDB->wNbItemAlloc; i++, pRsc++)
    {
        if (pRsc->wUsed)
        {
            if (pRsc->wType == T5SYSRSC_MALLOC)
            {
#ifdef T5DEF_FILESUPPORTED
                T5_FREE((T5_PTR)(pRsc->dwObject));
#endif /*T5DEF_FILESUPPORTED*/
            }
            else if (pRsc->wType == T5SYSRSC_FILE)
            {
#ifdef T5DEF_FILESUPPORTED
                T5File_Close (pRsc->dwObject);
#endif /*T5DEF_FILESUPPORTED*/
            }
            else if (pRsc->wType == T5SYSRSC_SOCKET)
            {
#ifdef T5DEF_LIBSOCK
#ifdef T5DEF_ETHERNET
                if ((T5_SOCKET)(pRsc->dwObject) != T5_INVSOCKET)
                    T5Socket_CloseSocket ((T5_SOCKET)(pRsc->dwObject));
#endif /*T5DEF_ETHERNET*/
#endif /*T5DEF_LIBSOCK*/
            }
            pRsc->dwObject = 0L;
            pRsc->dwSize = 0L;
            pRsc->wType = 0;
            pRsc->wUsed = 0;
        }
    }
}

/*****************************************************************************
T5VMRsc_CreateArray
allocates an array of typed data
Parameters:
    pDB (IN) pointer to the data base
    lID (IN) ID of the array (1 based)
    lSize (IN) number of data items to be allocated
    wSizeOf (IN) size of an item in bytes
Return:
    1=OK
    2=bad array id
    3=bad size
    4=not enough memory
*****************************************************************************/

#ifdef T5DEF_MALLOCSUPPORTED

T5_LONG T5VMRsc_CreateArray (T5PTR_DB pDB, T5_LONG lId,
                             T5_LONG lSize, T5_WORD wSizeOf)
{
    T5PTR_DBSYSRSC pRsc;

    /* check array ID */
    if (lId < 0 || lId >= T5MAX_SYSRSC
        || T5VMRsc_FindArray (pDB, lId) != T5_PTNULL)
        return 2L;
    /* chec size */
    if (lSize <= 0)
        return 3L;
    /* get free cell in heap */
    pRsc = T5VMRsc_GetFreeItem (pDB);
    if (pRsc == T5_PTNULL)
        return 2L;
    /* try to allocate memory */
    pRsc->dwObject = (T5_DWORD)T5_MALLOC(lSize * wSizeOf);
    if (pRsc->dwObject == 0L)
        return 4L;
    /* ok - done */
    pRsc->wUsed = (T5_WORD)lId;
    pRsc->wType = T5SYSRSC_ARRAY;
    pRsc->dwSize = (T5_DWORD)(lSize * wSizeOf);
    return 1L;
}

#endif /*T5DEF_MALLOCSUPPORTED*/

/*****************************************************************************
T5VMRsc_ReadArray
read an item from an array of typed data
Parameters:
    pDB (IN) pointer to the data base
    lID (IN) ID of the array (1 based)
    lIndex (In) index of the item (0 based)
    wSizeOf (IN) size of an item in bytes
    pData (IN) pointer to the buffer where to copy the read data
Return: TRUE if ok
*****************************************************************************/

#ifdef T5DEF_MALLOCSUPPORTED

T5_BOOL T5VMRsc_ReadArray (T5PTR_DB pDB, T5_LONG lId, T5_LONG lIndex,
                           T5_WORD wSizeOf, T5_PTR pData)
{
    T5PTR_DBSYSRSC pRsc;

    /* check array ID */
    pRsc = T5VMRsc_FindArray (pDB, lId);
    if (pRsc == T5_PTNULL)
        return FALSE;
    /* chek index and array size */
    if (pRsc->dwSize < ((T5_DWORD)(lIndex+1) * (T5_DWORD)wSizeOf))
        return FALSE;
    /* ok - get data */
    T5_MEMCPY(pData,
              (T5_PTR)(pRsc->dwObject + (T5_DWORD)lIndex*(T5_DWORD)wSizeOf),
              wSizeOf);
    return TRUE;
}

#endif /*T5DEF_MALLOCSUPPORTED*/

/*****************************************************************************
T5VMRsc_WriteArray
write an item in an array of typed data
Parameters:
    pDB (IN) pointer to the data base
    lID (IN) ID of the array (1 based)
    lIndex (In) index of the item (0 based)
    wSizeOf (IN) size of an item in bytes
    pData (IN) pointer to the buffer containing the value
Return:
    1=OK
    2=bad array id
    3=bad index
*****************************************************************************/

#ifdef T5DEF_MALLOCSUPPORTED

T5_LONG T5VMRsc_WriteArray (T5PTR_DB pDB, T5_LONG lId, T5_LONG lIndex,
                            T5_WORD wSizeOf, T5_PTR pData)
{
    T5PTR_DBSYSRSC pRsc;

    /* check array ID */
    pRsc = T5VMRsc_FindArray (pDB, lId);
    if (pRsc == T5_PTNULL)
        return 2L;
    /* chek index and array size */
    if (pRsc->dwSize < ((T5_DWORD)(lIndex+1) * (T5_DWORD)wSizeOf))
        return 3L;
    /* ok - get data */
    T5_MEMCPY((T5_PTR)(pRsc->dwObject + (T5_DWORD)lIndex*(T5_DWORD)wSizeOf),
              pData, wSizeOf);
    return 1L;
}

#endif /*T5DEF_MALLOCSUPPORTED*/

/*****************************************************************************
T5Param_Get
Search for either a registry parameter or an entry in an INI file
Registry parameter is searched if szEntry begins with '/'
WARNING: the registry parameter must be declared with type STRING!
Parameters:
    pDB (IN) pointer to the data base
    szFile (IN) INI file pathname - unused for reistry
    szSection (In) INI file section - unused for reistry
    szEntry (IN) INI entry name or registry parameter path if begin with '/'
    szBuffer (OUT) filled on output with found value
                   filled with szEntry if not found
    wBufSize (IN) size of szBuffer
Return: TRUE if found
*****************************************************************************/

T5_BOOL T5Param_Get (T5PTR_DB pDB, T5_PTCHAR szFile, T5_PTCHAR szSection,
                     T5_PTCHAR szEntry, T5_PTCHAR szBuffer, T5_WORD wBufSize)
{
    T5_BOOL bRet = FALSE;
#ifdef T5DEF_STRINGSUPPORTED
    T5_PTCHAR szValue = szEntry;
    T5_CHAR szRead[256];
    T5_WORD wLen = 0;
#ifdef T5DEF_REG
    T5_PTR pReg = NULL;
#endif /*T5DEF_REG*/

    *szRead = '\0';
#ifdef T5DEF_REG
    /* search in registry */
    pReg = T5VM_GetReg (pDB);
    if (pReg != NULL && *szEntry == '/')
    {
        T5PTR_T5REGITEM pItem = T5Reg_FindItem (pReg, szEntry);
        if (pItem != NULL && pItem->bTicType == T5C_STRING)
        {
            T5_STRCPY (szRead, (T5_PTCHAR)T5Reg_GetString (pReg, pItem));
            szValue = szRead;
            bRet = TRUE;
        }
    }
#endif /*T5DEF_REG*/

#ifdef T5DEF_FILESUPPORTED
    /* search in file */
    if (!bRet)
    {
        T5_LONG f = T5File_Open (szFile, FALSE);
        if (f)
        {
            if (_T5Param_FindSection (f, szSection) && _T5Param_FindEntry (f, szEntry, szRead))
            {
                szValue = szRead;
                bRet = TRUE;
            }
            T5File_Close (f);
        }
    }

#endif /*T5DEF_FILESUPPORTED*/

    /* safe copy value to buffer */
    if (wBufSize > 0)
    {
        wLen = (T5_WORD)T5_STRLEN(szValue);
        if (wLen >= wBufSize)
            wLen = wBufSize - 1;
        T5_MEMCPY (szBuffer, szValue, wLen);
        szBuffer[wLen] = '\0';
    }
#endif /*T5DEF_STRINGSUPPOTED*/
    return bRet;
}

/*****************************************************************************
_T5Param_FindSection
search for a section in an INI file
Parameters:
    f (IN) INI file open for read
    szSection (IN) searched section name
Return: TRUE if found
*****************************************************************************/

static T5_BOOL _T5Param_FindSection (T5_LONG f, T5_PTCHAR szSection)
{
    T5_BOOL bRet = FALSE;
#ifdef T5DEF_FILESUPPORTED
    T5_CHAR szLine[256];
    T5_WORD wLen = T5_STRLEN (szSection);

    while (!bRet && !T5File_Eof (f))
    {
        T5File_ReadLine (f, szLine, 255);
        if (*szLine == '[' && T5VMStr_NICmp ((T5_PTBYTE)(szLine+1), (T5_PTBYTE)(szSection), wLen) == 0
            && szLine[wLen+1] == ']')
        {
            bRet = TRUE;
        }
    }
#endif /*T5DEF_FILESUPPORTED*/
    return bRet;
}

/*****************************************************************************
_T5Param_FindEntry
search for a entry in an INI section
Parameters:
    f (IN) INI file open for read
    szEntry (IN) searched entry name
    szBuffer (OUT) filled on output with entry value if found
Return: TRUE if found
*****************************************************************************/

static T5_BOOL _T5Param_FindEntry (T5_LONG f, T5_PTCHAR szEntry, T5_PTCHAR szBuffer)
{
#ifdef T5DEF_FILESUPPORTED
    T5_CHAR szLine[512];
    T5_PTCHAR szValue;
    T5_WORD wLen = T5_STRLEN (szEntry);

    while (!T5File_Eof (f))
    {
        T5File_ReadLine (f, szLine, 255);
        if (*szLine == '[')
            return FALSE;
        if (T5VMStr_NICmp ((T5_PTBYTE)szLine, (T5_PTBYTE)szEntry, wLen) == 0 && szLine[wLen] == '=')
        {
            szValue = &(szLine[wLen+1]);
            wLen = T5_STRLEN (szValue);
            if (wLen > 255)
            {
                szValue[255] = '\0';
                wLen -= 1;
            }
            if (wLen > 0 && szValue[wLen-1] == '\r')
                szValue[wLen-1] = '\0';
            T5_STRCPY (szBuffer, szValue);
            return TRUE;
        }
    }
#endif /*T5DEF_FILESUPPORTED*/
    return FALSE;
}

/* eof **********************************************************************/
