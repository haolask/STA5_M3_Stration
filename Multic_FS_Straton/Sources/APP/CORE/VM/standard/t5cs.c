/*****************************************************************************
T5CS.c :     communication server

DO NOT ALTER THIS !

(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/
/* protocol */

/*

frame header = 4 bytes - followed by data - no terminator

+-----+-----+---------+---------+---...---+
| 'T' | '5' | Nb (HB) | Nb (LB) | Data    |
+-----+-----+---------+---------+---...---+

'T' = 84 (0x54)
'5' = 53 (0x35)

Data format: ReqType (1 byte) + Request data (Nb-1 bytes)

*/

/****************************************************************************/
/* dummy callback */

void T5CS_DefaultCallback (void) {}

/****************************************************************************/
/* static services */

static void _T5CS_StartSending (T5PTR_CS pData, T5_WORD wCaller,
                                T5_WORD wSize);
static T5_BOOL _T5CS_FilterReq (T5_WORD wReq, T5PTR_CS pData, T5_WORD wCaller);

#ifdef T5DEF_FT

static void _T5CSFT_Crc (T5_WORD wLen, T5_PTBYTE pData,
                         T5_PTDWORD pdwCalcCrc);

#endif /*T5DEF_FT*/

#ifdef T5DEF_CANRAW

static void _ServeCanGateway (T5PTR_CS pData, T5_WORD wCaller);

#endif /*T5DEF_CANRAW*/

#ifdef T5DEF_CSDISC

static void _T5CS_Discovery (T5_SOCKET sock, T5_WORD wMainPort, T5_BOOL bM5);

#endif /*T5DEF_CSDISC*/

/*****************************************************************************
T5CS_Open / T5CS_OpenM5
Open the communication server
Parameters:
    pData (IN) pointer to the communication server database
    pSettings (IN) pointer to the communication settings
    pfCallback (IN) application defined callback procedure
*****************************************************************************/

#ifdef T5DEF_CSM5

void T5CS_OpenM5 (T5PTR_CS pData, T5PTR_CSSET pSettings)
{
    T5CS_Open (pData, pSettings, NULL);
    pData->dwFlags |= T5CS_M5;
}

#endif /*T5DEF_CSM5*/

void T5CS_Open (T5PTR_CS pData, T5PTR_CSSET pSettings, T5HND_CS pfCallback)
{
    T5_WORD wMaxPort;
    T5_RET wRet;

    wRet = T5RET_OK;
    /* reset everything (all IDs to 0) */
    T5_MEMSET (pData, 0, sizeof (T5STR_CS));
    wMaxPort = T5MAX_PORT;
    /* set callback */
    if (pData->pfCallback != T5_PTNULL)
        pData->pfCallback = pfCallback;
    else
        pData->pfCallback = T5CS_DefaultCallback;

    /* safe protocol */
#ifdef T5DEF_SP5
    if (pSettings->wUnitSP5 != 0
        && pSettings->szSerialConf != T5_PTNULL
        && T5SP5Ftls_Open (&(pData->sp5), &(pData->sp5ft),
                           T5CS_SP5PASSWORD,
                           0, pSettings->wUnitSP5,
                           pSettings->szSerialConf))
    {
        pData->csPort[0].wFlags = T5CS_READY | T5CS_SP5;
        pData->csPort[0].wID = 1;
    }
#endif /*T5DEF_SP5*/

    /* serial */
#ifdef T5DEF_SERIAL
    if (pSettings->wUnitSP5 == 0
        && pSettings->szSerialConf != T5_PTNULL)
    {
        T5CSSer_Open (pData, pSettings->szSerialConf);
        wMaxPort -= 1;
    }
#endif /*T5DEF_SERIAL*/

    /* sockets */
#ifdef T5DEF_ETHERNET
    T5Socket_Initialize (pfCallback);
    if (pSettings->wEthernetPort != 0)
    {
        wRet = T5Socket_CreateListeningSocket (pSettings->wEthernetPort,
                                               wMaxPort, &(pData->sock),
                                               pfCallback);
        if (wRet != T5RET_OK)
            pData->sock = T5_INVSOCKET;
    }
    pData->wcsPort = pSettings->wEthernetPort;
#endif /*T5DEF_ETHERNET*/

#ifdef T5DEF_CSDISC
    if (T5Socket_UdpCreate (T5CS_DISCPORT, &(pData->sockDisc)) != T5RET_OK)
        pData->sockDisc = T5_INVSOCKET;
    else
        T5Socket_AcceptBroadcast (pData->sockDisc);
#endif /*T5DEF_CSDISC*/
}

#ifdef T5DEF_SP5

/*****************************************************************************
T5CS_SetSP5Password
Change the SP5 password
Parameters:
    pData (IN) pointer to the communication server database
    szPassword (IN) new password
*****************************************************************************/

void T5CS_SetSP5Password (T5PTR_CS pData, T5_PTCHAR szPassword)
{
    T5SP5Ftls_SetPassword (&(pData->sp5ft), szPassword);
}

#endif /*T5DEF_SP5*/

/*****************************************************************************
T5CS_CheckSilentSockets
Check and close possible silent sockets
Parameters:
    pData (IN) pointer to the communication server database
    dwTimeStamp (IN) current time stamp
    dwTimeout (IN) timeout
*****************************************************************************/

void T5CS_CheckSilentSockets (T5PTR_CS pData, T5_DWORD dwTimeStamp,
                              T5_DWORD dwTimeout)
{
#ifdef T5DEF_ETHERNET
    T5CSSock_CheckSilentSocketsEx (pData, dwTimeStamp, dwTimeout);
#endif /*T5DEF_ETHERNET*/
}

/*****************************************************************************
T5CS_GetNbHostTCP
Get number of connected hosts
Parameters:
    pData (IN) pointer to the communication server database
Return: number of connected hosts
*****************************************************************************/

T5_DWORD T5CS_GetNbHostTCP (T5PTR_CS pData)
{
#ifdef T5DEF_ETHERNET
    T5_DWORD i, nb=0;

    for (i=0; i<T5MAX_PORT; i++)
    {
        if ((pData->csPort[i].wFlags & T5CS_SOCKET) != 0
            && pData->csPort[i].sock != T5_INVSOCKET)
        {
            nb++;
        }
    }
    return nb;
#else /*T5DEF_ETHERNET*/
    return 0;
#endif /*T5DEF_ETHERNET*/
}

/*****************************************************************************
T5CS_Close
Close the communication server
Parameters:
    pData (IN) pointer to the communication server database
*****************************************************************************/

void T5CS_Close (T5PTR_CS pData)
{
#ifdef T5DEF_ETHERNET
    T5CSSock_CloseAllSockets (pData);
    T5Socket_Terminate ();
#endif /*T5DEF_ETHERNET*/

#ifdef T5DEF_CSDISC
    if (pData->sockDisc != T5_INVSOCKET)
    {
        T5Socket_CloseSocket (pData->sockDisc);
        pData->sockDisc = T5_INVSOCKET;
    }
#endif /*T5DEF_CSDISC*/

    /* safe protocol */
#ifdef T5DEF_SP5
    if ((pData->csPort[0].wFlags & T5CS_SP5) != 0)
        T5SP5Ftls_Close (&(pData->sp5), &(pData->sp5ft));
#endif /*T5DEF_SP5*/

#ifdef T5DEF_SERIAL
    if ((pData->csPort[0].wFlags & T5CS_SERIAL) != 0)
        T5CSSer_Close (pData);
#endif /*T5DEF_SERIAL*/

    T5_MEMSET (pData, 0, sizeof (T5STR_CS));
}

/*****************************************************************************
T5CS_Activate...
Activate the communication: accept clients, send and receive data
Parameters:
    pData (IN) pointer to the communication server database
*****************************************************************************/

#ifdef T5DEF_CSM5

void T5CS_ActivateM5 (T5PTR_CS pData, T5_DWORD dwTimeStamp)
{
    pData->dwStamp = dwTimeStamp;
    T5CS_Activate (pData);
}

#endif /*T5DEF_CSM5*/

void T5CS_ActivateStamp (T5PTR_CS pData, T5_DWORD dwTimeStamp)
{
    pData->dwStamp = dwTimeStamp;
    T5CS_Activate (pData);
}

void T5CS_Activate (T5PTR_CS pData)
{
#ifdef T5DEF_SP5
    T5_WORD wLen;
#endif /*T5DEF_SP5*/

#ifdef T5DEF_CSDISC
    if (pData->sockDisc != T5_INVSOCKET)
        _T5CS_Discovery (pData->sockDisc, pData->wcsPort,
                         (pData->dwFlags & T5CS_M5) != 0);
#endif /*T5DEF_CSDISC*/

    /* ethernet */
#ifdef T5DEF_ETHERNET
    T5CSSock_Accept (pData);
    T5CSSock_ActivateAllSockets (pData);
    T5CSSock_ActivateAllSockets (pData);
#endif /*T5DEF_ETHERNET*/

    /* safe protocol */
#ifdef T5DEF_SP5
    if ((pData->csPort[0].wFlags & T5CS_SP5) != 0)
    {
        if ((pData->csPort[0].wFlags & T5CS_DATAOUT) != 0
            && T5SP5Ftls_Send (&(pData->sp5), &(pData->sp5ft),
                               pData->csPort[0].wPakSizeOut,
                               pData->csPort[0].bfOut))
        {
            pData->csPort[0].wFlags &= ~T5CS_SENDING;
            pData->csPort[0].wFlags &= ~T5CS_DATAOUT;
            pData->csPort[0].wPakSizeOut = 0;
        }
        if ((pData->csPort[0].wFlags & T5CS_DATAIN) == 0
            && (wLen = T5SP5Ftls_Listen (&(pData->sp5), &(pData->sp5ft),
                                         T5MAX_FRAME,
                                         pData->csPort[0].bfIn, NULL)) != 0)
        {
            pData->csPort[0].wPakSizeIn = wLen;
            pData->csPort[0].wFlags &= ~T5CS_RECEIVING;
            pData->csPort[0].wFlags |= T5CS_DATAIN;
        }
    }
#endif /*T5DEF_SP5*/

    /* serial */
#ifdef T5DEF_SERIAL
    if ((pData->csPort[0].wFlags & T5CS_SERIAL) != 0)
    {
        T5CSSer_Activate (pData);
        T5CSSer_Activate (pData);
    }
#endif /*T5DEF_SERIAL*/
}

/*****************************************************************************
T5CS_GetTaskNo
Get task number attached to a question (M5 protocol only)
Parameters:
    wCaller (IN) ID of the requesting client (if any)
Return: task number (1 based - 0 if unknown)
*****************************************************************************/

T5_WORD T5CS_GetTaskNo (T5PTR_CS pData, T5_WORD wCaller)
{
#ifdef T5DEF_CSM5
    /* check caller ID - must be index + 1 */
    if (wCaller < 1 || wCaller > T5MAX_PORT)
        return 1;
    wCaller -= 1;
    return pData->csPort[wCaller].wTaskNo;
#else /*T5DEF_CSM5*/
    return 1;
#endif /*T5DEF_CSM5*/
}

/*****************************************************************************
T5CS_GetQuestion
Scan if there is a pending request not served
Parameters:
    pData (IN) pointer to the communication server database
    pwCaller (OUT) ID of the requesting client (if any)
Return: type of the request or 0 if nothing requested
*****************************************************************************/

#define T5_ISMBAP(b)    (!T5CS_ISHEAD(b) || (b[2]==0 && b[3]==0))

T5_WORD T5CS_GetQuestion (T5PTR_CS pData, T5_PTWORD pwCaller)
{
    T5_WORD i;
    T5_BOOL bFound;
    T5_WORD wReq;
    T5_PTBYTE pBuffer;

    do
    {
        wReq = 0;
        for (i=0, bFound=FALSE; !bFound && i<T5MAX_PORT; i++)
        {
            if ((pData->csPort[i].wFlags & T5CS_READY) != 0
                && (pData->csPort[i].wFlags & T5CS_DATAIN) != 0)
            {
                *pwCaller = i + 1;
                pBuffer = pData->csPort[i].bfIn;
                if (!T5_ISMBAP(pBuffer))
                    wReq = (T5_WORD)(pBuffer[4]) & 0x00ff;
                else if ((pData->csPort[i].wFlags & T5CS_SOCKET) != 0)
                    wReq = T5CSRQ_MODBUSTCP;
                else
                    wReq = T5CSRQ_MODBUSRTU;
                bFound = TRUE;
            }
        }
    } while (_T5CS_FilterReq (wReq, pData, *pwCaller));

    return wReq;
}

/*****************************************************************************
T5CS_ReleaseQuestion
Release a pending request and free a port for another request
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (OUT) ID of the client
*****************************************************************************/

void T5CS_ReleaseQuestion (T5PTR_CS pData, T5_WORD wCaller)
{
    /* check caller ID - must be index + 1 */
    if (wCaller < 1 || wCaller > T5MAX_PORT)
        return;
    wCaller -= 1;
    /* check the port is OK */
    if ((pData->csPort[wCaller].wFlags & T5CS_READY) == 0)
        return;
    /* reset received data */
    pData->csPort[wCaller].wFlags &= ~T5CS_DATAIN;
    pData->csPort[wCaller].wPakSizeIn = 0;
    pData->csPort[wCaller].wSizeIn = 0;
}

/*****************************************************************************
T5CS_GetVMRequest
Get pointer to a request for the VM
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of the client
Return: pointer to the request or NULL if no request for this client
*****************************************************************************/

T5_PTR T5CS_GetVMRequest (T5PTR_CS pData, T5_WORD wCaller)
{
    /* check caller ID - must be index + 1 */
    if (wCaller < 1 || wCaller > T5MAX_PORT)
        return T5_PTNULL;
    wCaller -= 1;
    /* check there is data in */
    if ((pData->csPort[wCaller].wFlags & T5CS_READY) == 0
        || (pData->csPort[wCaller].wFlags & T5CS_DATAIN) == 0)
        return T5_PTNULL;
    /* check this is a VM request */
    if (pData->csPort[wCaller].bfIn[4] != T5CSRQ_VMREQ)
        return T5_PTNULL;
    /* extract pointer to VM request */
    return (&(pData->csPort[wCaller].bfIn[5]));
}

/*****************************************************************************
T5CS_GetRequestFrame
Get pointer to a request (full frame)
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of the client
Return: pointer to the request or NULL if no request for this client
*****************************************************************************/

T5_PTR T5CS_GetRequestFrame (T5PTR_CS pData, T5_WORD wCaller)
{
    /* check caller ID - must be index + 1 */
    if (wCaller < 1 || wCaller > T5MAX_PORT)
        return T5_PTNULL;
    wCaller -= 1;
    /* check there is data in */
    if ((pData->csPort[wCaller].wFlags & T5CS_READY) == 0
        || (pData->csPort[wCaller].wFlags & T5CS_DATAIN) == 0)
        return T5_PTNULL;
    /* request frame */
    return (pData->csPort[wCaller].bfIn);
}

/*****************************************************************************
T5CS_SendRC
Send an answer with just a return check
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of client
    wRC (IN) return value to send
*****************************************************************************/

void T5CS_SendRC (T5PTR_CS pData, T5_WORD wCaller, T5_WORD wRC)
{
    T5_PTBYTE pFrame;

    /* check caller ID - must be index + 1 */
    if (wCaller < 1 || wCaller > T5MAX_PORT)
        return;
    wCaller -= 1;
    /* for MODBUS request: dont answer */
    if (!T5CS_ISHEAD(pData->csPort[wCaller].bfIn))
        return;
    /* check port is ready */
    if ((pData->csPort[wCaller].wFlags & T5CS_READY) == 0
        || (pData->csPort[wCaller].wFlags & T5CS_SENDING) != 0)
        return;
    /* builds the frame */
    pFrame = pData->csPort[wCaller].bfOut;
    pFrame[0] = pData->csPort[wCaller].bfIn[0];
    pFrame[1] = pData->csPort[wCaller].bfIn[1];
    pFrame[2] = 0; /* len - hibyte */
    pFrame[3] = 3; /* len - lobyte */
    pFrame[4] = pData->csPort[wCaller].bfIn[4]; /* echo req type */
    T5_COPYFRAMEWORD(pFrame+5, &wRC);
    /* mark data to send */
    _T5CS_StartSending (pData, wCaller, 7);
}

/*****************************************************************************
T5CS_GetVMAnswerBuffer
Get the pointer to the answer buffer for a VM request
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of client
Return: pointer to the answer buffer or NULL if invalid client
*****************************************************************************/

T5_PTR T5CS_GetVMAnswerBuffer (T5PTR_CS pData, T5_WORD wCaller)
{
    /* check caller ID - must be index + 1 */
    if (wCaller < 1 || wCaller > T5MAX_PORT)
        return T5_PTNULL;
    wCaller -= 1;
    /* check port is ready */
    if ((pData->csPort[wCaller].wFlags & T5CS_READY) == 0
        || (pData->csPort[wCaller].wFlags & T5CS_SENDING) != 0
        || (pData->csPort[wCaller].wFlags & T5CS_DATAOUT) != 0)
        return T5_PTNULL;
    /* get buffer address */
    return (&(pData->csPort[wCaller].bfOut[5]));
}

/*****************************************************************************
T5CS_GetAnswerFrameBuffer
Get the pointer to the answer buffer (full frame)
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of client
Return: pointer to the answer buffer or NULL if invalid client
*****************************************************************************/

T5_PTR T5CS_GetAnswerFrameBuffer (T5PTR_CS pData, T5_WORD wCaller)
{
    /* check caller ID - must be index + 1 */
    if (wCaller < 1 || wCaller > T5MAX_PORT)
        return T5_PTNULL;
    wCaller -= 1;
    /* check port is ready */
    if ((pData->csPort[wCaller].wFlags & T5CS_READY) == 0
        || (pData->csPort[wCaller].wFlags & T5CS_SENDING) != 0
        || (pData->csPort[wCaller].wFlags & T5CS_DATAOUT) != 0)
        return T5_PTNULL;
    /* get buffer address */
    return (pData->csPort[wCaller].bfOut);
}

/*****************************************************************************
T5CS_SendVMAnswer
Sends and answer from the VM
the contents of the answer has been copied to the send buffer
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of client
    wLen (IN) length of the VM answer
*****************************************************************************/

void T5CS_SendVMAnswer (T5PTR_CS pData, T5_WORD wCaller, T5_WORD wLen)
{
    T5_PTBYTE pFrame;
    T5_WORD wFrameLen;

    /* check caller ID - must be index + 1 */
    if (wCaller < 1 || wCaller > T5MAX_PORT)
        return;
    wCaller -= 1;
    /* check port is ready */
    if ((pData->csPort[wCaller].wFlags & T5CS_READY) == 0
        || (pData->csPort[wCaller].wFlags & T5CS_SENDING) != 0)
        return;
    /* builds the frame */
    pFrame = pData->csPort[wCaller].bfOut;
    wFrameLen = wLen + 1;
    pFrame[0] = pData->csPort[wCaller].bfIn[0];
    pFrame[1] = pData->csPort[wCaller].bfIn[1];
    T5_COPYFRAMEWORD(pFrame+2, &wFrameLen);
    pFrame[4] = T5CSRQ_VMREQ;
    /* VM data is asssumed to be copied by the caller */
    /* mark data to send */
    _T5CS_StartSending (pData, wCaller, (T5_WORD)(5 + wLen));
}

/*****************************************************************************
T5CS_SendAnswerFrame
Sends and answer (for MODBUS and private frames)
the contents of the answer has been copied to the send buffer
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of client
    wLen (IN) length of the VM answer
*****************************************************************************/

void T5CS_SendAnswerFrame (T5PTR_CS pData, T5_WORD wCaller, T5_WORD wLen)
{
    /* null length means: dont answer */
    if (wLen == 0)
        return;
    /* check caller ID - must be index + 1 */
    if (wCaller < 1 || wCaller > T5MAX_PORT)
        return;
    wCaller -= 1;
    /* check port is ready */
    if ((pData->csPort[wCaller].wFlags & T5CS_READY) == 0
        || (pData->csPort[wCaller].wFlags & T5CS_SENDING) != 0)
        return;
    /* mark data to send */
    _T5CS_StartSending (pData, wCaller, wLen);
}

/*****************************************************************************
_T5CS_StartSending
Start sending answer
the contents of the answer is assmed to be complete
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of client
    wSize (IN) number of bytes to send
*****************************************************************************/

static void _T5CS_StartSending (T5PTR_CS pData, T5_WORD wCaller,
                                T5_WORD wSize)
{
    pData->csPort[wCaller].wPakSizeOut = wSize;
    pData->csPort[wCaller].wSizeOut = wSize;
    pData->csPort[wCaller].wFlags |= T5CS_DATAOUT;
}

/*****************************************************************************
T5CS_GetLoadInfo
Extract info from a download request
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of client
    pLoad (OUT) where to store frame information
Return: TRUE if OK - BAD if invalid request
*****************************************************************************/

T5_BOOL T5CS_GetLoadInfo (T5PTR_CS pData, T5_WORD wCaller,
                          T5PTR_CSLOAD pLoad)
{
    T5_PTCHAR sz;
    T5_WORD wMax;

    /* check caller ID - must be index + 1 */
    if (wCaller < 1 || wCaller > T5MAX_PORT)
        return FALSE;
    wCaller -= 1;
    /* check there is data in */
    if ((pData->csPort[wCaller].wFlags & T5CS_READY) == 0
        || (pData->csPort[wCaller].wFlags & T5CS_DATAIN) == 0)
        return FALSE;
    /* check this is a load request */
    switch (pData->csPort[wCaller].bfIn[4])
    {
    case T5CSRQ_BEGINLOAD :
    case T5CSRQ_LOAD :
    case T5CSRQ_ENDLOAD :
        break;
    default :
        return FALSE;
    }
    /* extract pointer to VM request */
    pLoad->bType = pData->csPort[wCaller].bfIn[5];
    T5_COPYFRAMEDWORD (&(pLoad->dwSize), &(pData->csPort[wCaller].bfIn[6]));
    sz = (T5_PTCHAR)(&(pData->csPort[wCaller].bfIn[10]));
    pLoad->szName = sz;
    wMax = 255;
    while (wMax-- && (*sz != '\0')) sz++;
    pLoad->pData = sz + 1;
    return TRUE;
}

/*****************************************************************************
T5CS_GetStartInfo
Extract info from a download request
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of client
    pStart (OUT) start parameters
Return: TRUE if OK - BAD if invalid request
*****************************************************************************/

T5_BOOL T5CS_GetStartInfo (T5PTR_CS pData, T5_WORD wCaller,
                           T5PTR_CSSTART pStart)
{
    /* check caller ID - must be index + 1 */
    if (wCaller < 1 || wCaller > T5MAX_PORT)
        return FALSE;
    wCaller -= 1;
    /* check there is data in */
    if ((pData->csPort[wCaller].wFlags & T5CS_READY) == 0
        || (pData->csPort[wCaller].wFlags & T5CS_DATAIN) == 0)
        return FALSE;
    /* check this is a start request */
    switch (pData->csPort[wCaller].bfIn[4])
    {
    case T5CSRQ_START :
    case T5CSRQ_HOTSTART :
    case T5CSRQ_STOP :
    case T5CSRQ_CHANGE :
        break;
    default :
        return FALSE;
    }
    /* extract pointer to VM request */
    pStart->bWarm = (pData->csPort[wCaller].bfIn[5] != 0);
    pStart->bCC = (pData->csPort[wCaller].bfIn[6] != 0);
    pStart->szName = (T5_PTCHAR)(&(pData->csPort[wCaller].bfIn[7]));
    return TRUE;
}

/*****************************************************************************
T5CSFT_Serve
serve a file transfer request
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of client
*****************************************************************************/

#ifdef T5DEF_FT

void T5CSFT_Serve (T5PTR_CS pData, T5_WORD wCaller)
{
    T5_PTBYTE pQ, pA;
    T5_WORD wRC, wLen;
    T5_DWORD dwSize;

    pQ = (T5_PTBYTE)T5CS_GetRequestFrame (pData, wCaller);
    pA = (T5_PTBYTE)T5CS_GetAnswerFrameBuffer (pData, wCaller);

    wLen = 0;
    switch (pQ[5])
    {
    case T5FT_OW :
        T5_COPYFRAMEDWORD (&dwSize, pQ+6);
        if (pData->dwFTHandle != 0L)
            T5FT_Close (pData->dwFTHandle);
        pData->dwFTCrc = 0xffffffffL;
        pData->dwFTHandle = T5FT_OpenWrite (dwSize, (T5_PTCHAR)(pQ+10));
        if (pData->dwFTHandle == 0L)
            wRC = T5FTERR_OPENWRITE;
        else
            wRC = T5RET_OK;
        break;
    case T5FT_OR :
        if (pData->dwFTHandle != 0L)
            T5FT_Close (pData->dwFTHandle);
        pData->dwFTCrc = 0xffffffffL;
        pData->dwFTHandle = T5FT_OpenRead ((T5_PTCHAR)(pQ+6), &dwSize);
        if (pData->dwFTHandle == 0L)
            wRC = T5FTERR_OPENREAD;
        else
        {
            T5_COPYFRAMEDWORD (pA+8, &dwSize);
            wLen = 4;
            wRC = T5RET_OK;
        }
        break;
    case T5FT_CW :
    case T5FT_CR :
        if (pData->dwFTHandle == 0L)
            wRC = T5FTERR_CLOSE;
        else
        {
            T5FT_Close (pData->dwFTHandle);
            pData->dwFTHandle = 0L;
            T5_COPYFRAMEDWORD (pA+8, &(pData->dwFTCrc));
            wLen = 4;
            wRC = T5RET_OK;
        }
        break;
    case T5FT_XW :
        T5_COPYFRAMEWORD (&wLen, pQ+6);
        if (pData->dwFTHandle == 0L)
            wRC = T5FTERR_WRITE;
        else
        {
            T5FT_Write (pData->dwFTHandle, wLen, pQ+8);
            _T5CSFT_Crc (wLen, pQ+8, &(pData->dwFTCrc));
            wRC = T5RET_OK;
        }
        wLen = 0;
        break;
    case T5FT_XR :
        if (pData->dwFTHandle == 0L)
            wRC = T5FTERR_READ;
        else
        {
            wLen = T5FT_Read (pData->dwFTHandle, T5FT_PAKSIZE, pA+10);
            if (wLen)
                _T5CSFT_Crc (wLen, pA+10, &(pData->dwFTCrc));
            T5_COPYFRAMEWORD (pA+8, &wLen);
            wLen += 2;
            wRC = T5RET_OK;
        }
        break;
    case T5FT_RM :
        if (pData->dwFTHandle != 0L)
        {
            T5FT_Close (pData->dwFTHandle);
            pData->dwFTHandle = 0L;
        }
        if (!T5FT_Delete ((T5_PTCHAR)(pQ+6)))
            wRC = T5FTERR_DELETE;
        else
            wRC = T5RET_OK;
        break;

    case T5FT_OD :
        if (pData->dwFTDirHandle != 0L)
            T5FT_CloseDir (pData->dwFTDirHandle);
        pData->dwFTDirHandle = T5FT_OpenDir ();
        if (pData->dwFTDirHandle == 0L)
            wRC = T5FTERR_OPENDIR;
        else
            wRC = T5RET_OK;
        break;
    case T5FT_CD :
        if (pData->dwFTDirHandle == 0L)
            wRC = T5FTERR_CLOSEDIR;
        else
        {
            T5FT_CloseDir (pData->dwFTDirHandle);
            pData->dwFTDirHandle = 0L;
            wRC = T5RET_OK;
        }
        break;
    case T5FT_XD :
        if (pData->dwFTDirHandle == 0L)
            wRC = T5FTERR_GETDIR;
        else
        {
            if (!T5FT_GetDirEntry (pData->dwFTDirHandle, pA+8))
                pA[8] = 0;
            wLen = (T5_WORD)(T5_STRLEN(pA+8) + 1);
            wRC = T5RET_OK;
        }
        break;
    default :
        wRC = T5FTERR_UNKNOWN;
        break;
    }

    wLen += 4;
    pA[0] = pQ[0];
    pA[1] = pQ[1];
    T5_COPYFRAMEWORD (pA+2, &wLen);
    pA[4] = pQ[4]; /* echo req type */
    T5_COPYFRAMEWORD(pA+5, &wRC);
    pA[7] = pQ[5]; /* echo FT code */
    /* data at pA+8 */
    T5CS_SendAnswerFrame (pData, wCaller, (T5_WORD)(4+wLen));
    T5CS_ReleaseQuestion (pData, wCaller);
}

#endif /*T5DEF_FT*/

/*****************************************************************************
_T5CSFT_Crc
calculate file CRC32
Parameters:
    wLen (IN) number of byte
    pData (IN) pointer to bytes
    pdwCalcCrc (IN/OUT) pointer to CRC32
*****************************************************************************/

#ifdef T5DEF_FT

static void _T5CSFT_Crc (T5_WORD wLen, T5_PTBYTE pData,
                         T5_PTDWORD pdwCalcCrc)
{
    T5_BYTE i;
    T5_DWORD mem1;

    while (wLen--)
    {
        *pdwCalcCrc ^= ((T5_DWORD)(*pData) & 0xffL);
        for (i=1; i<= 8; i++)
        {
            mem1 = *pdwCalcCrc;
            *pdwCalcCrc /= 2;
            if (mem1 & 1) *pdwCalcCrc ^= 0xa000a001;
        }
        pData++; 
    }
}

#endif /*T5DEF_FT*/

/*****************************************************************************
_T5CS_FilterReq
Filter requests handled by CS
Parameters:
    wReq (IN) : incoming request
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of client
Return: type of the request or 0 if nothing requested
*****************************************************************************/

static T5_BOOL _T5CS_FilterReq (T5_WORD wReq, T5PTR_CS pData, T5_WORD wCaller)
{
    T5_PTBYTE pQ;
    T5_WORD wCode, wRC;
    T5_DWORD dwOld, dwNew;

#ifdef T5DEF_CANRAW
    if (wReq == T5CSRQ_CAN)
    {
        _ServeCanGateway (pData, wCaller);
        return TRUE;
    }
    else if (wReq != 0 && pData->dwCan)
    {
        T5CANRaw_Close (pData->dwCan);
        pData->dwCan = 0L;
    }
#endif /*T5DEF_CANRAW*/

#ifdef T5DEF_FT
    if (wReq == T5CSRQ_FT)
    {
        T5CSFT_Serve (pData, wCaller);
        return TRUE;
    }
#endif /*T5DEF_FT*/

    if (wReq == T5CSRQ_CS)
    {
        pQ = (T5_PTBYTE)T5CS_GetRequestFrame (pData, wCaller);
        T5_COPYFRAMEWORD (&wCode, pQ + 5);
        pQ += 7;

        wRC = T5RET_ERROR;
        switch (wCode)
        {
        case T5CCS_SETPSWLOAD :
            T5_COPYFRAMEDWORD (&dwOld, pQ);
            T5_COPYFRAMEDWORD (&dwNew, pQ+4);
            if (T5_SETDOWNLOADPASSWORD (dwOld, dwNew))
                wRC = T5RET_OK;
            break;
        default : break;
        }
        T5CS_SendRC (pData, wCaller, wRC);
        T5CS_ReleaseQuestion (pData, wCaller);
        return TRUE;
    }

    return FALSE;
}

/* CAN gateway protocol *****************************************************/

/*

Open:
Q= 'O' - baudrate(w) - flags(b) - settings(str)
A= 'O' - 1|0

Close:
Q= 'C'
A= 'C' - 1|0

Exchange:
Q= 'X' - nbsnd(b) { ID(dw) - flags(b) - len(b) - data(8) }
Q= 'X' - nbrcv(b) { ID(dw) - flags(b) - len(b) - data(8) }

*/

#ifdef T5DEF_CANRAW

/*****************************************************************************
_ServeCanGateway
Serve CAN Gateway request
Parameters:
    pData (IN) pointer to the communication server database
    wCaller (IN) ID of client
*****************************************************************************/

static void _ServeCanGateway (T5PTR_CS pData, T5_WORD wCaller)
{
    T5_PTBYTE pQ, pA, pRet;
    T5_WORD wLen, wRate;
    T5_BYTE bFlags, nb, bLen;
    T5_DWORD dwID;
    T5_BYTE data[8];

    pQ = (T5_PTBYTE)T5CS_GetRequestFrame (pData, wCaller);
    pA = (T5_PTBYTE)T5CS_GetAnswerFrameBuffer (pData, wCaller);
    pA[0] = pQ[0];
    pA[1] = pQ[1];
    pA[4] = pQ[4]; /* echo req type */
    pA[5] = pQ[5]; /* echo CAN code */

    pQ += 5;
    pRet = pA + 6;
    wLen = 2;
    switch (*pQ++)
    {
    case 'O' :
        T5_COPYFRAMEWORD(&wRate, pQ);
        bFlags = pQ[2];
        pData->dwCan = T5CANRaw_Open (wRate, (bFlags & T5CSCAN_2B) != 0, (T5_PTCHAR)(pQ+3));
        *pRet = (pData->dwCan != 0L) ? 1 : 0;
        wLen++;
        break;
    case 'C' :
        if (pData->dwCan)
        {
            T5CANRaw_Close (pData->dwCan);
            pData->dwCan = 0L;
        }
        *pRet = 1;
        wLen++;
        break;
    case 'X' :
        *pRet = 0;
        wLen++;
        pRet++;
        nb = *pQ++;
        while (nb--)
        {
            T5_COPYFRAMEDWORD(&dwID, pQ);
            bFlags = pQ[4];
            T5CANRaw_Send (pData->dwCan, dwID, pQ[5], pQ+6,
                 (bFlags & T5CSCAN_2B) != 0, (bFlags & T5CSCAN_RTR) != 0);
            pQ += 14;
        }
        nb = 0;
        T5_MEMSET (data, 0, sizeof (data));
        while (nb < 50 && T5CANRaw_PopRcv (pData->dwCan, &dwID, &bLen, data))
        {
            nb++;
            pA[6] = nb;
            T5_COPYFRAMEDWORD(pRet, &dwID);
            pRet[4] = 0;
            pRet[5] = bLen;
            T5_MEMCPY (pRet+6, data, 8);
            wLen += 14;
            pRet += 14;
            T5_MEMSET (data, 0, sizeof (data));
        }
        break;
    default :
        *pRet = 0;
        wLen++;
        break;
    }
    T5_COPYFRAMEWORD (pA+2, &wLen);
    T5CS_SendAnswerFrame (pData, wCaller, (T5_WORD)(4+wLen));
    T5CS_ReleaseQuestion (pData, wCaller);
}

#endif /*T5DEF_CANRAW*/

/* node discovery protocol **************************************************/

#ifdef T5DEF_CSDISC

/*****************************************************************************
_T5CS_Discovery
Discovery protocol server
Parameters:
    sock (IN) 11173 socket
    wMainPort (IN) CS main port number
    bM5 (IN) TRUE if CS server if for multiple VMs
*****************************************************************************/

static void _T5CS_Discovery (T5_SOCKET sock, T5_WORD wMainPort, T5_BOOL bM5)
{
    T5_WORD wSize;
    T5_UDPADDR addr;
    T5_CHAR frame[256], szPort[10];

    T5_MEMSET (&addr, 0, sizeof (addr));
    T5_MEMSET (frame, 0, sizeof (frame));
    wSize = T5Socket_UdpRecvFrom (sock, frame, sizeof (frame), &addr);
    if (wSize == 4 && T5_MEMCMP (frame, "disc", 4) == 0)
    {
        T5_STRCPY (frame, "disc:");
        T5_LTOA (szPort, wMainPort);
        T5_STRCAT (frame, szPort);
        T5_STRCAT (frame, ",");
#ifdef T5DEF_EVENTSERVER
        T5_LTOA (szPort, T5DEF_TRACEPORT);
        T5_STRCAT (frame, szPort);
#else /*T5DEF_EVENTSERVER*/
        T5_STRCAT (frame, "0");
#endif /*T5DEF_EVENTSERVER*/
        if (bM5)
            T5_STRCAT (frame, ",M5");
        else
            T5_STRCAT (frame, ",T5");

        T5Socket_UdpSendTo (sock, &addr, frame, T5_STRLEN(frame));
    }
}

#endif /*T5DEF_CSDISC*/

/* eof **********************************************************************/
