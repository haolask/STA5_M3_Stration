/*****************************************************************************
T5MBFB.c :   MODBUS master function blocks (TCP and RTU)

DO NOT ALTER THIS !

(c) COPA-DATA France 2010
*****************************************************************************/

#include "t5vm.h"

/****************************************************************************/
/* MBSlaveTCP */

#ifdef T5DEF_MBSTCP
#ifdef T5DEF_MODBUSIOS
#ifdef T5DEF_ETHERNET

#define _P_IN       (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_PORT     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_OK       (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))

typedef struct
{
    T5STR_CS cs;
    T5_BOOL bOpen;
} _str_FB_MBSLAVETCP;

T5_DWORD MBSLAVETCP (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass,
                     T5_PTR pInst, T5_PTWORD pArgs)
{
    _str_FB_MBSLAVETCP *pData;
    T5PTR_DBSTATUS pStatus;
    T5STR_CSSET settings;
    T5_WORD wReq, wCaller, wLen;
    T5_PTBYTE pIn, pOut;

    pData = (_str_FB_MBSLAVETCP *)pInst;
    switch (wCommand)
    {
    case T5FBCMD_ACTIVATE :
        if (_P_IN && !(pData->bOpen))
        {
            T5_MEMSET (&settings, 0, sizeof (settings));
            settings.wEthernetPort = (T5_WORD)_P_PORT;
            T5CS_Open (&(pData->cs), &settings, NULL);
            pData->bOpen = (pData->cs.sock != T5_INVSOCKET);
        }
        else if (!_P_IN && pData->bOpen)
        {
            T5CS_Close (&(pData->cs));
            pData->bOpen = FALSE;
        }
        else
        {
            T5CS_Activate (&(pData->cs));
            if ((wReq = T5CS_GetQuestion (&(pData->cs), &wCaller)) != 0)
            {
                if (wReq == T5CSRQ_MODBUSTCP)
                {
                    pIn = (T5_PTBYTE)T5CS_GetRequestFrame (&(pData->cs), wCaller);
                    pOut = (T5_PTBYTE)T5CS_GetAnswerFrameBuffer (&(pData->cs), wCaller);
                    if (pIn != NULL && pOut != NULL)
                    {
                        wLen = T5MB_ServeEx (T5GET_MBC(pBase), pIn, pOut, TRUE, 0);
                        T5CS_SendAnswerFrame (&(pData->cs), wCaller, wLen);
                    }
                }
                T5CS_ReleaseQuestion (&(pData->cs), wCaller);
            }
        }
        _P_OK = pData->bOpen;
        return 0L;

    case T5FBCMD_INITINSTANCE :
        T5_MEMSET (pData, 0, sizeof(_str_FB_MBSLAVETCP));
        return 0L;
    case T5FBCMD_EXITINSTANCE :
        if (pData->bOpen)
        {
            T5CS_Close (&(pData->cs));
            pData->bOpen = FALSE;
        }
        return 0L;
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        if ((pStatus->dwFlagsEx & T5FLAGEX_CHGCYC) == 0)
        {
            T5_MEMSET (pData, 0, sizeof(_str_FB_MBSLAVETCP));
        }
        return 0L;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FB_MBSLAVETCP);
    case T5FBCMD_ACCEPTCT :
        return 1L;
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_MBSLAVETCP;
    default :
        return 0L;
    }
}

#undef _P_IN
#undef _P_PORT
#undef _P_OK

#define _P_IN       (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[0]))
#define _P_PORT     (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[1]))
#define _P_SLAVE    (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[2]))
#define _P_SRVID    (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[3]))
#define _P_OK       (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[4]))

T5_DWORD MBSLAVETCPEX (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass,
    T5_PTR pInst, T5_PTWORD pArgs)
{
    _str_FB_MBSLAVETCP *pData;
    T5PTR_DBSTATUS pStatus;
    T5STR_CSSET settings;
    T5_WORD wReq, wCaller, wLen;
    T5_PTBYTE pIn, pOut;

    pData = (_str_FB_MBSLAVETCP *)pInst;
    switch (wCommand)
    {
    case T5FBCMD_ACTIVATE:
        if (_P_IN && !(pData->bOpen))
        {
            T5_MEMSET (&settings, 0, sizeof (settings));
            settings.wEthernetPort = (T5_WORD)_P_PORT;
            T5CS_Open (&(pData->cs), &settings, NULL);
            pData->bOpen = (pData->cs.sock != T5_INVSOCKET);
        }
        else if (!_P_IN && pData->bOpen)
        {
            T5CS_Close (&(pData->cs));
            pData->bOpen = FALSE;
        }
        else
        {
            T5CS_Activate (&(pData->cs));
            if ((wReq = T5CS_GetQuestion (&(pData->cs), &wCaller)) != 0)
            {
                if (wReq == T5CSRQ_MODBUSTCP)
                {
                    pIn = (T5_PTBYTE)T5CS_GetRequestFrame (&(pData->cs), wCaller);
                    pOut = (T5_PTBYTE)T5CS_GetAnswerFrameBuffer (&(pData->cs), wCaller);
                    if (pIn != NULL && pOut != NULL)
                    {
                        wLen = T5MB_ServeExID (T5GET_MBC (pBase), pIn, pOut, TRUE, _P_SLAVE, _P_SRVID+2);
                        T5CS_SendAnswerFrame (&(pData->cs), wCaller, wLen);
                    }
                }
                T5CS_ReleaseQuestion (&(pData->cs), wCaller);
            }
        }
        _P_OK = pData->bOpen;
        return 0L;

    case T5FBCMD_INITINSTANCE:
        T5_MEMSET (pData, 0, sizeof (_str_FB_MBSLAVETCP));
        return 0L;
    case T5FBCMD_EXITINSTANCE:
        if (pData->bOpen)
        {
            T5CS_Close (&(pData->cs));
            pData->bOpen = FALSE;
        }
        return 0L;
    case T5FBCMD_HOTRESTART:
        pStatus = T5GET_DBSTATUS (pBase);
        if ((pStatus->dwFlagsEx & T5FLAGEX_CHGCYC) == 0)
        {
            T5_MEMSET (pData, 0, sizeof (_str_FB_MBSLAVETCP));
        }
        return 0L;
    case T5FBCMD_SIZEOFINSTANCE:
        return (T5_DWORD)sizeof (_str_FB_MBSLAVETCP);
    case T5FBCMD_ACCEPTCT:
        return 1L;
    case T5FBCMD_GETCLASSID:
        return (T5_DWORD)T5FBL_MBSLAVETCPEX;
    default:
        return 0L;
    }
}

#undef _P_IN
#undef _P_PORT
#undef _P_SLAVE
#undef _P_SRVID
#undef _P_OK

#endif /*T5DEF_ETHERNET */
#endif /*T5DEF_MODBUSIOS*/
#endif /*T5DEF_MBSTCP*/

/****************************************************************************/

#ifdef T5DEF_DATA16SUPPORTED

/****************************************************************************/
/* common for TCP and RTU */

#define _MB_OFF         0   /* off line */
#define _MB_CNX         1   /* connecting */
#define _MB_IDLE        2   /* on line - no request */
#define _MB_SND         3   /* sending */
#define _MB_RCV         4   /* receiving */

#define _P_ANYTYPE      pArgs[0]
#define _P_CNX          (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[1]))
#define _P_SND          (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[2]))
#define _P_SLAVE        (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[3]))
#define _P_FCODE        (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[4]))
#define _P_ADDR         (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[5]))
#define _P_NBR          (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[6]))
#define _P_ADDW         (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[7]))
#define _P_NBW          (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[8]))
#define _P_DATA         pArgs[9] /*any */
#define _P_DATA_SIZE    pArgs[10]
#define _P_TIMEOUT      (*((T5_PTDWORD)(T5GET_DBTIME(pBase))+pArgs[11]))

typedef struct
{
    T5_DWORD dwStamp;
    T5_WORD wTransID;
    T5_WORD wTotalSize;
    T5_WORD wCurSize;
    T5_WORD wNbItem;
    T5_BYTE bStatus;
    T5_BYTE bError;
    T5_BOOL bReqDone;
    T5_BOOL bCnxPrev;
    T5_BOOL bSndPrev;
    T5_BYTE res1;
    T5_BYTE frame[256+6];
#ifdef T5DEF_TCPCLIENT
    T5_SOCKET sock;
#endif /*T5DEF_TCPCLIENT*/
#ifdef T5DEF_SERIAL
    T5_SERIAL ser;
#endif /*T5DEF_SERIAL*/
} _str_FB_MBMASTER;

static T5_RET _MB_CheckArgs (T5PTR_DB pBase, T5_PTWORD pArgs)
{
    T5_WORD wAnyType;

    /* slave number */
    if (_P_SLAVE < 0 || _P_SLAVE > 255)
        return 200;
    /* get type of DATA array */
    wAnyType = _P_ANYTYPE;
    if (wAnyType == T5C_UINT)
        wAnyType = T5C_INT;
    /* function code, type and nb of items */
    switch (_P_FCODE)
    {
    case T5MBF_REX :
    case T5MBF_RID :
        if (wAnyType != T5C_SINT && wAnyType != T5C_USINT)
            return 203;
        break;
    case T5MBF_RWR :
        if (_P_ADDR < 1 ||_P_ADDR > 65536 || _P_ADDW < 1 ||_P_ADDW > 65536)
            return 201;
        if (wAnyType != T5C_INT && wAnyType != T5C_UINT)
            return 203;
        if (_P_NBW < 1 || _P_NBW > _P_DATA_SIZE || _P_NBW > 121
            || _P_NBR < 1 || _P_NBR > _P_DATA_SIZE || _P_NBR > 125)
            return 204;
        break;
    case T5MBF_RIB :
    case T5MBF_RCB :
        if (_P_ADDR < 1 ||_P_ADDR > 65536)
            return 201;
        if (wAnyType != T5C_BOOL)
            return 203;
        if (_P_NBR < 1 || _P_NBR > _P_DATA_SIZE || _P_NBR > 2000)
            return 204;
        break;
    case T5MBF_RIW :
    case T5MBF_RHW :
        if (_P_ADDR < 1 ||_P_ADDR > 65536)
            return 201;
        if (wAnyType != T5C_INT && wAnyType != T5C_UINT)
            return 203;
        if (_P_NBR < 1 || _P_NBR > _P_DATA_SIZE || _P_NBR > 125)
            return 204;
        break;
    case T5MBF_W1B :
        if (_P_ADDW < 1 ||_P_ADDW > 65536)
            return 201;
        if (wAnyType != T5C_BOOL)
            return 203;
        if (_P_NBW != 1)
            return 204;
        break;
    case T5MBF_W1W :
        if (_P_ADDW < 1 ||_P_ADDW > 65536)
            return 201;
        if (wAnyType != T5C_INT && wAnyType != T5C_UINT)
            return 203;
        if (_P_NBW != 1)
            return 204;
        break;
    case T5MBF_WNB :
        if (_P_ADDW < 1 ||_P_ADDW > 65536)
            return 201;
        if (wAnyType != T5C_BOOL)
            return 203;
        if (_P_NBW < 1 || _P_NBW > _P_DATA_SIZE || _P_NBW > 1968)
            return 204;
        break;
    case T5MBF_WNW :
        if (_P_ADDW < 1 ||_P_ADDW > 65536)
            return 201;
        if (wAnyType != T5C_INT && wAnyType != T5C_UINT)
            return 203;
        if (_P_NBW < 1 || _P_NBW > _P_DATA_SIZE || _P_NBW > 120)
            return 204;
        break;
    default :
        return T5MBERR_FUNC;
    }
    return T5RET_OK;
}

static T5_WORD _MB_Crc16 (T5_PTBYTE pS, T5_WORD wLen)
{
    T5_WORD i, j;
    T5_WORD wCRC;

    wCRC = 0xffff;
    for (i=0; i<wLen; i++)
    {
        wCRC ^= (((T5_WORD)(pS[i])) & 0x00ff);
        for (j=0; j<8; j++)
        {
            if (wCRC & 1)
            {
                wCRC >>= 1;
                wCRC ^= 0xa001;
            }
            else
                wCRC >>= 1;
        }
    }
    wCRC = ((wCRC << 8) & 0xff00)
         | ((wCRC >> 8) & 0x00ff);
    return wCRC;
}

static void _MB_BuildFrame (T5PTR_DB pBase, _str_FB_MBMASTER *pData,
                            T5_PTWORD pArgs, T5_BOOL bTCP)
{
    T5_PTBYTE pMBAP, pFrame, pD8;
    T5_WORD i, w, wLen, wCRC;
    T5_PTWORD pD16;

    pData->wNbItem = (T5_WORD)_P_NBR; /* for reading bits */

    pFrame = pData->frame;
    if (bTCP)
    {
        pMBAP = pFrame;
        pFrame += 6;
    }
    else
        pMBAP = NULL;

    memset (pFrame, 0, 256);

    *pFrame++ = (T5_BYTE)_P_SLAVE;
    *pFrame++ = (T5_BYTE)_P_FCODE;
    wLen = 2;

    switch (_P_FCODE)
    {
    case T5MBF_REX :
    case T5MBF_RID:
        break;
    case T5MBF_RWR :
        w = (T5_WORD)_P_ADDR - 1;
        T5_COPYFRAMEWORD(pFrame, &w);
        w = (T5_WORD)_P_NBR;
        T5_COPYFRAMEWORD(pFrame+2, &w);
        w = (T5_WORD)_P_ADDW - 1;
        T5_COPYFRAMEWORD(pFrame+4, &w);
        w = (T5_WORD)_P_NBW;
        T5_COPYFRAMEWORD(pFrame+6, &w);
        w *= 2;
        pFrame[8] = (T5_BYTE)w; /* byte count */
        wLen += 9;
        pFrame += 9;
        pD16 = T5Tools_GetD16Array (pBase, _P_DATA);
        for (i=0; i<_P_NBW; i++)
        {
            T5_COPYFRAMEWORD(pFrame, pD16);
            pFrame += 2;
            pD16++;
        }
        wLen += w;
        break;
    case T5MBF_RIW :
    case T5MBF_RHW :
    case T5MBF_RIB :
    case T5MBF_RCB :
        w = (T5_WORD)_P_ADDR - 1;
        T5_COPYFRAMEWORD(pFrame, &w);
        w = (T5_WORD)_P_NBR;
        T5_COPYFRAMEWORD(pFrame+2, &w);
        wLen += 4;
        break;
    case T5MBF_W1B :
        w = (T5_WORD)_P_ADDW - 1;
        T5_COPYFRAMEWORD(pFrame, &w);
        if (*T5Tools_GetD8Array (pBase, _P_DATA))
            pFrame[2] = 0xff;
        else
            pFrame[2] = 0x00;
        pFrame[3] = 0x00;
        wLen += 4;
        break;
    case T5MBF_W1W :
        w = (T5_WORD)_P_ADDW - 1;
        T5_COPYFRAMEWORD(pFrame, &w);
        w = *T5Tools_GetD16Array (pBase, _P_DATA);
        T5_COPYFRAMEWORD(pFrame+2, &w);
        wLen += 4;
        break;
    case T5MBF_WNB :
        w = (T5_WORD)_P_ADDW - 1;
        T5_COPYFRAMEWORD(pFrame, &w);
        w = (T5_WORD)_P_NBW;
        T5_COPYFRAMEWORD(pFrame+2, &w);
        w = (T5_WORD)_P_NBW / 8;
        if (_P_NBW % 8)
            w += 1;
        pFrame[4] = (T5_BYTE)w; /* byte count */
        wLen += 5;
        pFrame += 5;
        *pFrame = 0;
        pD8 = T5Tools_GetD8Array (pBase, _P_DATA);
        for (i=0; i<_P_NBW; i++)
        {
            if (i > 0 && (i % 8) == 0)
            {
                pFrame++;
                *pFrame = 0;
            }
            if (*pD8)
                *pFrame |= (1 << (i % 8));
            pD8++;
        }
        wLen += w;
        break;
    case T5MBF_WNW :
        w = (T5_WORD)_P_ADDW - 1;
        T5_COPYFRAMEWORD(pFrame, &w);
        w = (T5_WORD)_P_NBW;
        T5_COPYFRAMEWORD(pFrame+2, &w);
        w *= 2;
        pFrame[4] = (T5_BYTE)w; /* byte count */
        wLen += 5;
        pFrame += 5;
        pD16 = T5Tools_GetD16Array (pBase, _P_DATA);
        for (i=0; i<_P_NBW; i++)
        {
            T5_COPYFRAMEWORD(pFrame, pD16);
            pFrame += 2;
            pD16++;
        }
        wLen += w;
        break;
    default :
        return;
    }
    pData->wTotalSize = wLen;
    pData->wCurSize = 0;
    if (bTCP)
    {
        pData->wTransID += 1;
        if (pData->wTransID == 0 || pData->wTransID == 0x5435)
            pData->wTransID = 1;
        T5_COPYFRAMEWORD (pMBAP, &(pData->wTransID));
        pMBAP[2] = pMBAP[3] = 0;
        T5_COPYFRAMEWORD (pMBAP+4, &wLen);
        pData->wTotalSize += 6;
    }
    else
    {
        pFrame = pData->frame;
        wCRC = _MB_Crc16 (pFrame, wLen);
        T5_COPYFRAMEWORD(pFrame+wLen, &wCRC);
        pData->wTotalSize += 2;
    }
}

static void _MB_ParseFrame (T5PTR_DB pBase, _str_FB_MBMASTER *pData,
                            T5_PTWORD pArgs, T5_BOOL bTCP)
{
    T5_PTBYTE pFrame, pD8;
    T5_BYTE bFCode;
    T5_WORD wAnyType, i, wNb, wCRC;
    T5_PTWORD pArray;

    pFrame = pData->frame;
    if (bTCP)
        pFrame += 6;

    if (!bTCP)
    {
        T5_COPYFRAMEWORD(&wCRC, pFrame + pData->wTotalSize - 2);
        if (wCRC != _MB_Crc16 (pFrame, pData->wTotalSize - 2))
        {
            pData->bError = T5MBERR_CRC;
            return;
        }
    }

    pFrame++; /*slave*/
    bFCode = *pFrame++;

    if (bFCode & 0x80)
    {
        pData->bError = bFCode & 0x7f;
        return;
    }

    wAnyType = _P_ANYTYPE;
    if (wAnyType == T5C_UINT)
        wAnyType = T5C_INT;

    switch (bFCode)
    {
    case T5MBF_REX :
        if (wAnyType != T5C_SINT && wAnyType != T5C_USINT)
        {
            pData->bError = 203;
            return;
        }
        pD8 = T5Tools_GetD8Array (pBase, _P_DATA);
        *pD8 = *pFrame;
        break;
    case T5MBF_RID:
        if (wAnyType != T5C_SINT && wAnyType != T5C_USINT)
        {
            pData->bError = 203;
            return;
        }
        wNb = *pFrame++;
        pD8 = T5Tools_GetD8Array (pBase, _P_DATA);
        if (wNb < 2 || wNb > _P_DATA_SIZE)
        {
            pData->bError = 202;
            return;
        }
        T5_MEMCPY (pD8, pFrame, wNb);
        break;
    case T5MBF_RIW :
    case T5MBF_RHW :
    case T5MBF_RWR :
        if (wAnyType != T5C_INT && wAnyType != T5C_UINT)
        {
            pData->bError = 203;
            return;
        }
        wNb = *pFrame++;
        if (wNb < 2 || wNb > (_P_DATA_SIZE * 2))
        {
            pData->bError = 202;
            return;
        }
        wNb /= 2;
        pArray = T5Tools_GetD16Array (pBase, _P_DATA);
        while (wNb--)
        {
            T5_COPYFRAMEWORD (pArray, pFrame);
            pArray += 1;
            pFrame += 2;
        }
        break;
    case T5MBF_RIB :
    case T5MBF_RCB :
        if (wAnyType != T5C_BOOL)
        {
            pData->bError = 203;
            return;
        }
        wNb = pData->wNbItem / 8;
        if (pData->wNbItem % 8)
            wNb += 1;
        if (wNb < *pFrame || pData->wNbItem > _P_DATA_SIZE)
        {
            pData->bError = 202;
            return;
        }
        pFrame++;
        pD8 = T5Tools_GetD8Array (pBase, _P_DATA);
        for (i=0; i<pData->wNbItem; i++)
        {
            *pD8 = (pFrame[i/8] & (1 << (i % 8))) ? 1 : 0;
            pD8++;
        }
        break;
    case T5MBF_W1B :
    case T5MBF_W1W :
    case T5MBF_WNB :
    case T5MBF_WNW :
        break;
    default :
        pData->bError = T5MBERR_FUNC;
        break;
    }
}

static void _MB_StartRCV (_str_FB_MBMASTER *pData, T5_DWORD dwStamp,
                          T5_BOOL bTCP)
{
    pData->wCurSize = pData->wTotalSize = 0;
    pData->dwStamp = dwStamp;
    if (!bTCP && pData->frame[0] == 0) /* broadcast */
    {
        pData->bReqDone = TRUE;
        pData->bStatus = _MB_IDLE;
    }
    else
        pData->bStatus = _MB_RCV;
}

/****************************************************************************/
/* MBMasterTCP */

#ifdef T5DEF_TCPCLIENT

#define _P_IP           (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[12]))
#define _P_PORT         (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[13]))
#define _P_READY        (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[14]))
#define _P_OK           (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[15]))
#define _P_ERROR        (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[16]))

static void _MB_TCPAbort (_str_FB_MBMASTER *pData, T5_BYTE wError)
{
    T5Socket_CloseSocket (pData->sock);
    pData->sock = T5_INVSOCKET;
    pData->bStatus = _MB_OFF;
    pData->bError = wError;
}

T5_DWORD MBMASTERTCP (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
    _str_FB_MBMASTER *pData;
    T5_RET wRet;
    T5_BOOL bWait, bFail;
    T5PTR_DBSTATUS pStatus;
    T5_WORD wMBAP;
    T5_UNUSED(pClass);

    pData = (_str_FB_MBMASTER *)pInst;
    switch (wCommand)
    {
    case T5FBCMD_ACTIVATE :
        pStatus = T5GET_DBSTATUS(pBase);
        if (!_P_CNX && pData->bStatus != _MB_OFF)
            _MB_TCPAbort (pData, 0);
        switch (pData->bStatus)
        {
        case _MB_OFF :
            pData->bReqDone = FALSE;
            if (!pData->bCnxPrev && _P_CNX)
            {
                pData->bError = 0;
                wRet = T5Socket_CreateConnectedSocket (
                    _P_IP + 2, (T5_WORD)_P_PORT,
                    &(pData->sock), &bWait, NULL
                    );
                if (wRet != T5RET_OK)
                    pData->bError = T5MBERR_COMM;
                else if (bWait)
                    pData->bStatus = _MB_CNX;
                else
                    pData->bStatus = _MB_IDLE;
            }
            break;
        case _MB_CNX :
            pData->bReqDone = FALSE;
            wRet = T5Socket_CheckPendingConnect (pData->sock, &bFail);
            if (bFail)
                _MB_TCPAbort (pData, T5MBERR_COMM);
            else if (wRet == T5RET_OK)
                    pData->bStatus = _MB_IDLE;
            break;
        case _MB_IDLE :
            if (!pData->bSndPrev && _P_SND)
            {
                pData->bReqDone = FALSE;
                wRet = _MB_CheckArgs (pBase, pArgs);
                if (wRet != T5RET_OK)
                    pData->bError = (T5_BYTE)wRet;
                else
                {
                    pData->bError = 0;
                    _MB_BuildFrame (pBase, pData, pArgs, TRUE);
                    pData->wCurSize = T5Socket_Send (pData->sock,
                                                     pData->wTotalSize,
                                                     pData->frame, &bFail);
                    if (bFail)
                        _MB_TCPAbort (pData, T5MBERR_COMM);
                    else if (pData->wCurSize == pData->wTotalSize)
                        _MB_StartRCV (pData, pStatus->dwTimStamp, TRUE);
                    else
                        pData->bStatus = _MB_SND;
                }
            }
            break;
        case _MB_SND :
            pData->wCurSize += T5Socket_Send (
                pData->sock,
                pData->wTotalSize - pData->wCurSize,
                pData->frame + pData->wCurSize,
                &bFail);
            if (bFail)
                _MB_TCPAbort (pData, T5MBERR_COMM);
            else if (pData->wCurSize == pData->wTotalSize)
                _MB_StartRCV (pData, pStatus->dwTimStamp, TRUE);
            break;
        case _MB_RCV :
            if (T5_BOUNDTIME(pStatus->dwTimStamp - pData->dwStamp) > _P_TIMEOUT)
            {
                pData->bReqDone = TRUE;
                pData->bError = T5MBERR_TIMEOUT;
                pData->bStatus = _MB_IDLE;
            }
            else
            {
                bFail = FALSE;
                if (pData->wCurSize < 6)
                {
                    pData->wCurSize += T5Socket_Receive (
                        pData->sock,
                        (6 - pData->wCurSize),
                        pData->frame + pData->wCurSize,
                        &bFail);
                    if (bFail)
                        _MB_TCPAbort (pData, T5MBERR_COMM);
                    else if (pData->wCurSize == 6)
                    {
                        T5_COPYFRAMEWORD (&(pData->wTotalSize), pData->frame + 4);
                        pData->wTotalSize += 6;
                    }
                }
                if (!bFail && pData->wTotalSize > 0)
                {
                    pData->wCurSize += T5Socket_Receive (
                        pData->sock,
                        (pData->wTotalSize - pData->wCurSize),
                        pData->frame + pData->wCurSize,
                        &bFail);
                    if (bFail)
                        _MB_TCPAbort (pData, T5MBERR_COMM);
                    else if (pData->wCurSize == pData->wTotalSize)
                    {
                        T5_COPYFRAMEWORD (&wMBAP, pData->frame);
                        if (wMBAP != pData->wTransID)
                        {
                            pData->wTotalSize = 0;
                            pData->wCurSize = 0;
                        }
                        else
                        {
                            _MB_ParseFrame (pBase, pData, pArgs, TRUE);
                            pData->bReqDone = TRUE;
                            pData->bStatus = _MB_IDLE;
                        }
                    }
                }
            }
            break;
        default : break;
        }
        pData->bCnxPrev = _P_CNX;
        pData->bSndPrev = _P_SND;
        _P_READY = (pData->bStatus != _MB_OFF && pData->bStatus != _MB_CNX);
        _P_OK = (pData->bReqDone && pData->bError == 0);
        _P_ERROR = (T5_LONG)(pData->bError) & 0xff;
        return 0L;

    case T5FBCMD_INITINSTANCE :
        pData->sock = T5_INVSOCKET;
        pData->bStatus = _MB_OFF;
        return 0L;
    case T5FBCMD_EXITINSTANCE :
        if (pData->sock != T5_INVSOCKET)
        {
            T5Socket_CloseSocket (pData->sock);
            pData->sock = T5_INVSOCKET;
        }
        return 0L;
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        if ((pStatus->dwFlagsEx & T5FLAGEX_CHGCYC) == 0)
        {
            pData->sock = T5_INVSOCKET;
            pData->bStatus = _MB_OFF;
        }
        return 0L;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FB_MBMASTER);
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_MBMTCP;
    case T5FBCMD_ACCEPTCT :
        return 1L;
    default :
        return 0L;
    }
}

#undef _P_IP
#undef _P_PORT
#undef _P_READY
#undef _P_OK
#undef _P_ERROR

#endif /*T5DEF_TCPCLIENT*/

/****************************************************************************/
/* MBMasterRTU */

#ifdef T5DEF_SERIAL

#define _P_PORT         (*((T5_PTCHAR *)(T5GET_DBSTRING(pBase))+pArgs[12]))
#define _P_READY        (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[13]))
#define _P_OK           (*((T5_PTBOOL)(T5GET_DBDATA8(pBase))+pArgs[14]))
#define _P_ERROR        (*((T5_PTLONG)(T5GET_DBDATA32(pBase))+pArgs[15]))

static void _MB_RTUAbort (_str_FB_MBMASTER *pData, T5_BYTE wError)
{
    T5Serial_Close (&(pData->ser));
    pData->bStatus = _MB_OFF;
    pData->bError = wError;
}

static void _MB_FlushSerial (_str_FB_MBMASTER *pData)
{
    while (T5Serial_Receive (&(pData->ser), 256, pData->frame))
        ; /* flush */
}

T5_DWORD MBMASTERRTU (
    T5_WORD wCommand,
    T5PTR_DB pBase,
    T5_PTR pClass,
    T5_PTR pInst,
    T5_PTWORD pArgs)
{
    _str_FB_MBMASTER *pData;
    T5PTR_DBSTATUS pStatus;
    T5_RET wRet;
    T5_UNUSED(pClass);

    pData = (_str_FB_MBMASTER *)pInst;
    switch (wCommand)
    {
    case T5FBCMD_ACTIVATE :
        pStatus = T5GET_DBSTATUS(pBase);
        if (!_P_CNX && pData->bStatus != _MB_OFF)
            _MB_RTUAbort (pData, 0);
        switch (pData->bStatus)
        {
        case _MB_OFF :
            pData->bReqDone = FALSE;
            if (!pData->bCnxPrev && _P_CNX)
            {
                pData->bError = 0;
                if (!T5Serial_Open (&(pData->ser), _P_PORT + 2))
                    pData->bError = T5MBERR_COMM;
                else
                    pData->bStatus = _MB_IDLE;
            }
            break;
        case _MB_IDLE :
            if (!pData->bSndPrev && _P_SND)
            {
                pData->bReqDone = FALSE;
                wRet = _MB_CheckArgs (pBase, pArgs);
                if (wRet != T5RET_OK)
                    pData->bError = (T5_BYTE)wRet;
                else
                {
                    pData->bError = 0;
                    _MB_BuildFrame (pBase, pData, pArgs, FALSE);
                    pData->wCurSize = T5Serial_Send (&(pData->ser),
                                                     pData->wTotalSize,
                                                     pData->frame);
                    if (pData->wCurSize == pData->wTotalSize)
                    {
                        _MB_FlushSerial (pData);
                        _MB_StartRCV (pData, pStatus->dwTimStamp, FALSE);
                    }
                    else
                        pData->bStatus = _MB_SND;
                }
            }
            break;
        case _MB_SND :
            pData->wCurSize += T5Serial_Send (
                &(pData->ser),
                pData->wTotalSize - pData->wCurSize,
                pData->frame + pData->wCurSize);
            if (pData->wCurSize == pData->wTotalSize)
            {
                _MB_FlushSerial (pData);
                _MB_StartRCV (pData, pStatus->dwTimStamp, FALSE);
            }
            break;
        case _MB_RCV :
            if (T5_BOUNDTIME(pStatus->dwTimStamp - pData->dwStamp) > _P_TIMEOUT)
            {
                pData->bReqDone = TRUE;
                pData->bError = T5MBERR_TIMEOUT;
                pData->bStatus = _MB_IDLE;
            }
            else
            {
                if (pData->wCurSize < 3)
                {
                    pData->wCurSize += T5Serial_Receive (
                        &(pData->ser),
                        (3 - pData->wCurSize),
                        pData->frame + pData->wCurSize);
                    if (pData->wCurSize == 3)
                    {
                        switch (pData->frame[1])
                        {
                        case T5MBF_REX :
                            /* read exception : sl+fc+data+crc(2) */
                            pData->wTotalSize = 5;
                            break;
                        case T5MBF_RIB :
                        case T5MBF_RCB :
                        case T5MBF_RIW :
                        case T5MBF_RHW :
                        case T5MBF_RID :
                        case T5MBF_RWR :
                            /* read requests: sl+fc+nb+data(nb)+crc(2) */
                            pData->wTotalSize = ((T5_WORD)(pData->frame[2])
                                                           & 0x00ff) + 5;
                            break;
                        case T5MBF_W1B :
                        case T5MBF_WNB :
                        case T5MBF_W1W :
                        case T5MBF_WNW :
                            /* write requests: sl+fc+data(4)+crc(2) */
                            pData->wTotalSize = 8;
                            break;
                        default :
                            /* exceptions: sl+fc+code+crc(2) */
                            if ((pData->frame[1] & 0x80) != 0)
                                pData->wTotalSize = 5;
                            /* unknown or not supported function codes: skip */
                            else
                            {
                                pData->bError = T5MBERR_FUNC;
                                pData->bReqDone = TRUE;
                                pData->bStatus = _MB_IDLE;
                            }
                            break;
                        }
                    }
                }
                if (!pData->bReqDone && pData->wTotalSize > 0)
                {
                    pData->wCurSize += T5Serial_Receive (
                        &(pData->ser),
                        (pData->wTotalSize - pData->wCurSize),
                        pData->frame + pData->wCurSize);
                    if (pData->wCurSize == pData->wTotalSize)
                    {
                        _MB_ParseFrame (pBase, pData, pArgs, FALSE);
                        pData->bReqDone = TRUE;
                        pData->bStatus = _MB_IDLE;
                    }
                }
            }
            break;
        default : break;
        }
        pData->bCnxPrev = _P_CNX;
        pData->bSndPrev = _P_SND;
        _P_READY = (pData->bStatus != _MB_OFF && pData->bStatus != _MB_CNX);
        _P_OK = (pData->bReqDone && pData->bError == 0);
        _P_ERROR = (T5_LONG)(pData->bError) & 0xff;
        return 0L;
    case T5FBCMD_INITINSTANCE :
        T5Serial_Initialize (&(pData->ser));
        pData->bStatus = _MB_OFF;
        return 0L;
    case T5FBCMD_EXITINSTANCE :
        T5Serial_Close (&(pData->ser));
        return 0L;
    case T5FBCMD_HOTRESTART :
        pStatus = T5GET_DBSTATUS(pBase);
        if ((pStatus->dwFlagsEx & T5FLAGEX_CHGCYC) == 0)
        {
            T5Serial_Initialize (&(pData->ser));
            if (pData->bStatus != _MB_OFF)
            {
                if (!T5Serial_Open (&(pData->ser), _P_PORT + 2))
                {
                    pData->bError = T5MBERR_COMM;
                    pData->bStatus = _MB_OFF;
                }
                else
                {
                    pData->bStatus = _MB_IDLE;
                }
            }
        }
        return 0L;
    case T5FBCMD_SIZEOFINSTANCE :
        return (T5_DWORD)sizeof(_str_FB_MBMASTER);
    case T5FBCMD_ACCEPTCT :
        return 1L;
    case T5FBCMD_GETCLASSID :
        return (T5_DWORD)T5FBL_MBMRTU;
    default :
        return 0L;
    }
}

#undef _P_PORT
#undef _P_READY
#undef _P_OK
#undef _P_ERROR

#endif /*T5DEF_SERIAL*/

/****************************************************************************/

#undef _P_ANYTYPE
#undef _P_CNX
#undef _P_SND
#undef _P_SLAVE
#undef _P_FCODE
#undef _P_ADDR
#undef _P_NBR
#undef _P_ADDW
#undef _P_NBW
#undef _P_DATA
#undef _P_DATA_SIZE
#undef _P_TIMEOUT

/****************************************************************************/

#endif /*T5DEF_DATA16SUPPORTED*/

/* eof **********************************************************************/
