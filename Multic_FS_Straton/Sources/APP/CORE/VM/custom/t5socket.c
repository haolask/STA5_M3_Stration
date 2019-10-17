/*****************************************************************************
T5Socket.c : socket interface - CAN BE CHANGED WHEN PORTING
(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"
#include "monitor.h"
#ifdef T5DEF_ETHERNET

/*************************** DEFINES ***************************/
//#define Ds32T5Socket_iEthernetPort			1100
#define Du32T5Socket_iMaxConnexion			T5MAX_PORT

// Connexion status
enum
{
    Ceu8T5Socket_iFree = 0,
    Ceu8T5Socket_iTCPListen,
    Ceu8T5Socket_iTCPAccept,
    Ceu8T5Socket_iTCPKernelAccepted
};

typedef struct
{
    void *pvSocketHdl;
    TU8   u8ConnState;
}TstT5Socket_iConnEntry;

/********************* GLOBAL VARIABLES ************************/
TstT5Socket_iConnEntry stT5Socket_iConnEntry[Du32T5Socket_iMaxConnexion];
TpvWEthIf_eTCPPcbHandle       pvT5Socket_iTCPConnPCB;				//TCP connected socket
TpvWEthIf_eUDPPcbHandle      *pvT5Socket_iUDPSocket;				//UDP socket
//TU8                           u8T5Socket_iConnectInProgress;
TS8                    s8T5Socket_iReceiveFlag[Du32T5Socket_iMaxConnexion];
TU8                    u8T5Socket_iFragNotPushed[Du32T5Socket_iMaxConnexion];
TstWEthIf_ePBuf       *pstT5Socket_iBuffReassembly[Du32T5Socket_iMaxConnexion];
TU16                   u16T5Socket_iUDPPort;
TU8                    pu8T5Socket_iRecvBuff[Du32T5Socket_iMaxConnexion][T5MAX_FRAME];
TU16                   u16T5Socket_iRecvStart[Du32T5Socket_iMaxConnexion];
TU16                   u16T5Socket_iRecvEnd[Du32T5Socket_iMaxConnexion];


/********************** ACTIA FUNCTIONS ************************/
// This function returns index corresponding to given socket entry
TS32 s32T5Socket_iSearchEntry( void *FpvSocket )
{
    TU32 s32Index;
    
    s32Index = 0;
    
    if( FpvSocket != 0)
    {
        // Look for a socket entry
        while(   (stT5Socket_iConnEntry[s32Index].pvSocketHdl != FpvSocket) && (s32Index<Du32T5Socket_iMaxConnexion) )
        {
            s32Index++;
        }
    }
    else
    {
        // Look for a socket entry
        while(   ((stT5Socket_iConnEntry[s32Index].pvSocketHdl != FpvSocket) || (stT5Socket_iConnEntry[s32Index].u8ConnState != Ceu8T5Socket_iFree))
        && (s32Index<Du32T5Socket_iMaxConnexion) )
        {
            s32Index++;
        }
    }
    
    if( s32Index == Du32T5Socket_iMaxConnexion )
    {
        s32Index = -1;
    }
    else{} //empty ELSE
    
    return s32Index;
}

// This function handles initiated TCP connection.
TS32 s32T5Socket_iTcpOpenHandler_Exe( void *FpvSocket )
{
    TS32 s32Entry;
    
    s32Entry = 0;
    
    // Look for the entry
    while((stT5Socket_iConnEntry[s32Entry].u8ConnState != Ceu8T5Socket_iTCPListen) && (s32Entry<Du32T5Socket_iMaxConnexion) )
    {
    	s32Entry++;
    }
    
    if(s32Entry == Du32T5Socket_iMaxConnexion)
    {
        s32Entry = -1;
    }
    
    if( s32Entry >= 0 )
    {
        if(FpvSocket != 0) 
        {
            // Store the connection entry
            stT5Socket_iConnEntry[s32Entry].pvSocketHdl = FpvSocket;
            stT5Socket_iConnEntry[s32Entry].u8ConnState = Ceu8T5Socket_iTCPAccept;
            
            s32Entry = Ces32WEthIf_eSuccess;
        }
        else
        {
            s32Entry = Ces32WEthIf_eError;
        }
    }
    else
    {
        s32Entry = Ces32WEthIf_eError;
    }
    
    return s32Entry;
}

//This function provides access to the received data.
void vT5Socket_iCopyRcvData_Exe( void *FpvData, TU16 Fu16Size, TU8 Fu8Sock )
{
    TU16 u16Count;
    TU32 u32Index;
    TstWEthIf_ePBuf *pstBuf;
    
    if(Fu8Sock <Du32T5Socket_iMaxConnexion)
    {
        // Set-up context
        u16Count = 0U;
        u32Index = 0U;
        pstBuf = (TstWEthIf_ePBuf *)FpvData;
        
        // Read from pbuf
        while(   (pstBuf != 0)
              && (u16Count<Fu16Size) )
        {
            pu8T5Socket_iRecvBuff[Fu8Sock][u16T5Socket_iRecvEnd[Fu8Sock]] = ((TU8 *)pstBuf->pvPayload)[u32Index];
            u16T5Socket_iRecvEnd[Fu8Sock] = (u16T5Socket_iRecvEnd[Fu8Sock] + 1) % T5MAX_FRAME;
            u32Index++;
            u16Count++;
            // Go next fragment
            if( u32Index == pstBuf->u16Len )
            {
                pstBuf = pstBuf->pstNext;
                u32Index = 0U;
            }
            else{} //empty ELSE
        }
    }
    else{} //empty ELSE
}

// This function is called when a TCP packet is transmitted and acknowledged by remote side
TS32 s32T5Socket_iTcpSent_Exe( void *pvArg, TpvWEthIf_eTCPPcbHandle FpvPcb, TU16 Fu16Len )
{
    if(pvArg);
    if(FpvPcb);
    if(Fu16Len);
    
    return Ces32WEthIf_eSuccess;
}

// This function is called when a TCP packet is received
TS32 s32T5Socket_iTcpRecv_Exe( void *pvArg, TpvWEthIf_eTCPPcbHandle FpvPcb, TstWEthIf_ePBuf *FpstRcvBuff, TS32 Fs32Error )
{
    TS32 s32Ret = Ces32WEthIf_eSuccess;
    TS32 s32TcpEntry = -1;
    
    if(   (Fs32Error == Ces32WEthIf_eSuccess)
       && (FpstRcvBuff != 0) )
    {
        //Look for socket
        s32TcpEntry = s32T5Socket_iSearchEntry( FpvPcb );
        
        if(s32TcpEntry >= 0)
        {
            // If the queue is empty
            if( pstT5Socket_iBuffReassembly[s32TcpEntry] == 0 )
            {
                // Store the first element
                pstT5Socket_iBuffReassembly[s32TcpEntry] = FpstRcvBuff;
            }
            else
            {
                // Add the element in the queue
                DstWMonitor_eApplication_Function.pvWEthIf_ePBufCat_Exe( pstT5Socket_iBuffReassembly[s32TcpEntry], FpstRcvBuff );
            }
            
            // Check if TCP packet is completed
            if( FpstRcvBuff->u8Flags & Du8WEthIf_ePBUF_FLAG_PUSH )
            {
                //Read data
                vT5Socket_iCopyRcvData_Exe( pstT5Socket_iBuffReassembly[s32TcpEntry], pstT5Socket_iBuffReassembly[s32TcpEntry]->u16Tot_len, (TU8)s32TcpEntry );
                
                // We can receive more data now
                DstWMonitor_eApplication_Function.pvWEthIf_eTcpRcvd_Exe( FpvPcb, pstT5Socket_iBuffReassembly[s32TcpEntry]->u16Tot_len );
                
                (void)DstWMonitor_eApplication_Function.pu8WEthIf_ePBufFree_Exe( pstT5Socket_iBuffReassembly[s32TcpEntry] );
                
                pstT5Socket_iBuffReassembly[s32TcpEntry] = 0;
            }
            else {} //Empty else
        }
        else
        {
            // Close connection
            DstWMonitor_eApplication_Function.ps32WEthIf_eTcpClose_Exe( (void *)FpvPcb );
            s32Ret = Ces32WEthIf_eError;
        }
    }
    else
    {
        // Close connection
        DstWMonitor_eApplication_Function.ps32WEthIf_eTcpClose_Exe( (void *)FpvPcb );
        s32Ret = Ces32WEthIf_eError;
    }
    
    return s32Ret;
}

// This function is called when a TCP connection on the listening port is initiated
TS32 s32T5Socket_iTcpAccept_Exe( void *pvArg, TpvWEthIf_eTCPPcbHandle FpvNewPcb, void *FpvAddr, TU16 Fu16Port, TS32 Fs32Error )
{
    TS32 s32Ret;
    
    if( Fs32Error == Ces32WEthIf_eSuccess )
    {
        s32Ret = s32T5Socket_iTcpOpenHandler_Exe( (void *)FpvNewPcb );
        
        if( s32Ret == Ces32WEthIf_eSuccess )
        {
            // Disable Nagle Algorithm
            DstWMonitor_eApplication_Function.pvWEthIf_eSetFlagPCB_Exe( FpvNewPcb, (TU8)Du8WEthIf_eNODELAY );
            
            // Set-up TCP call-backs
            DstWMonitor_eApplication_Function.pvWEthIf_eTcpSetUp_Exe( FpvNewPcb, s32T5Socket_iTcpSent_Exe, s32T5Socket_iTcpRecv_Exe );
            
            s32Ret = Ces32WEthIf_eSuccess;
        }
        else
        {
            s32Ret = Ces32WEthIf_eError;
        }
    }
    else
    {
        s32Ret = Ces32WEthIf_eError;
    }
    
    return s32Ret;
}

#ifdef T5DEF_TCPCLIENT

// This function is called when a TCP connection on the listening port is initiated
TS32 s32T5Socket_iTcpConnect_Exe( void *pvArg, TpvWEthIf_eTCPPcbHandle FpvNewPcb, TS32 Fs32Error )
{
    TS32 s32Ret;
    
    if( Fs32Error == Ces32WEthIf_eSuccess )
    {
       // Disable Nagle Algorithm
        DstWMonitor_eApplication_Function.pvWEthIf_eSetFlagPCB_Exe( FpvNewPcb, (TU8)Du8WEthIf_eNODELAY );
        
        DstWMonitor_eApplication_Function.pvWEthIf_eTcpSetUp_Exe( FpvNewPcb, s32T5Socket_iTcpSent_Exe, s32T5Socket_iTcpRecv_Exe );
        
        pvT5Socket_iTCPConnPCB = FpvNewPcb;
        
        u8T5Socket_iConnectInProgress = 2;
        
        s32Ret = Ces32WEthIf_eSuccess;
    }
    else
    {
        u8T5Socket_iConnectInProgress = 3;
        
        s32Ret = Ces32WEthIf_eError;
    }
    
    return s32Ret;
}

#endif /*T5DEF_TCPCLIENT*/

/*****************************************************************************
T5Socket_Initialize
Initialize the Socket interface
*****************************************************************************/

T5_RET T5Socket_Initialize (T5HND_CS pfCallback)
{
    TU16 u16Index;

    // Initialize environment
    
    for( u16Index=0U; u16Index<Du32T5Socket_iMaxConnexion; u16Index++ )
    {
        pstT5Socket_iBuffReassembly[u16Index] = 0;
        u8T5Socket_iFragNotPushed[u16Index] = 0;
        u16T5Socket_iRecvStart[u16Index] = 0;
        u16T5Socket_iRecvEnd[u16Index] = 0;
    }

    u16T5Socket_iUDPPort = 0;

    return T5RET_OK;
}

/*****************************************************************************
T5Socket_Terminate
Release the socket interface
*****************************************************************************/

void T5Socket_Terminate (void)
{
}

/*****************************************************************************
T5Socket_CreateListeningSocket
Create a non blocking listening socket
Parameters:
    wPort (IN) ethernet port number
    wMaxCnx (IN) max number of allowed connections
    pSocket (OUT) created socket if OK
return: OK or ERROR
*****************************************************************************/

T5_RET T5Socket_CreateListeningSocket (T5_WORD wPort, T5_WORD wMaxCnx,
                                       T5_PTSOCKET pSocket,
                                       T5HND_CS pfCallback)
{
    TS32 s32Ret;
    TS32 s32Entry;
    
    // Look for an empty entry
    s32Entry = s32T5Socket_iSearchEntry( 0 );
    if(s32Entry >= 0)
    {
        // Listen on TCP
        s32Ret = DstWMonitor_eApplication_Function.ps32WEthIf_eTcpListen_Exe( 0, wPort, s32T5Socket_iTcpAccept_Exe );
        
        if( s32Ret != Ces32WEthIf_eSuccess )
        {
            *pSocket = T5_INVSOCKET;
            s32Ret = T5RET_ERROR;
        }
        else
        {
            stT5Socket_iConnEntry[s32Entry].u8ConnState = Ceu8T5Socket_iTCPListen;
            *pSocket = s32Entry;
        }
    }
    else
    {
        // Max connection
        *pSocket = T5_INVSOCKET;
        s32Ret = T5RET_ERROR;
    }
    
    return ((T5_RET)s32Ret);
}

#ifdef T5DEF_SOCKETV2

T5_RET T5Socket_CreateListeningSocketOn (T5_WORD wPort, T5_WORD wMaxCnx,
                                         T5_PTCHAR szAddr,
                                         T5_PTSOCKET pSocket,
                                         T5HND_CS pfCallback)
{
    return T5RET_ERROR;
}

#endif /*T5DEF_SOCKETV2*/

/*****************************************************************************
T5Socket_CloseSocket
Close a socket
Parameters:
    sock (IN) socket to be closed
*****************************************************************************/

void T5Socket_CloseSocket (T5_SOCKET sock)
{
    TpvWEthIf_eTCPPcbHandle pvPcb = 0;
    
    if(sock < Du32T5Socket_iMaxConnexion)
    {
        //Look for PCB handler
        pvPcb = stT5Socket_iConnEntry[sock].pvSocketHdl;
        
        if(pvPcb != 0)
        {
            stT5Socket_iConnEntry[sock].pvSocketHdl = 0;
            stT5Socket_iConnEntry[sock].u8ConnState = Ceu8T5Socket_iTCPListen;
            DstWMonitor_eApplication_Function.ps32WEthIf_eTcpClose_Exe( pvPcb );
        }
    }
}

/*****************************************************************************
T5Socket_Accept
Accept non blokcing sockets
Parameters:
    sockListen (IN) handle of the listening socket
return: accepted socket or T5_INVSOCKET if nothing
*****************************************************************************/

T5_SOCKET T5Socket_Accept (T5_SOCKET sockListen, T5HND_CS pfCallback)
{
    if(sockListen < Du32T5Socket_iMaxConnexion)
    {
        if(stT5Socket_iConnEntry[sockListen].u8ConnState == Ceu8T5Socket_iTCPAccept)
        {
            stT5Socket_iConnEntry[sockListen].u8ConnState = Ceu8T5Socket_iTCPKernelAccepted;
            return (sockListen);
        }
        else
        {
            return T5_INVSOCKET;
        }
    }
    else
    {
        return T5_INVSOCKET;
    }
}

/*****************************************************************************
T5Socket_Send
Send data on a non blocking client socket
Parameters:
    sock (IN) client socket
    wSize (IN) number of bytes to send
    pData (IN) pointer to data to send
    pbFail (OUT) set to TRUE on error: socket must be closed
return: number of bytes actually sent
*****************************************************************************/

T5_WORD T5Socket_Send (T5_SOCKET sock, T5_WORD wSize, T5_PTR pData,
                       T5_PTBOOL pbFail)
{
    TpvWEthIf_eTCPPcbHandle pvPcb;
    TS32 s32Ret;
    TU16 u16Size;
    
    if(sock < Du32T5Socket_iMaxConnexion)
    {
        //Look for PCB handler
        pvPcb = stT5Socket_iConnEntry[sock].pvSocketHdl;
        
        if(pvPcb != 0)
        {
            s32Ret = DstWMonitor_eApplication_Function.ps32WEthIf_eTcpSend_Exe( pvPcb, (void *)pData, wSize );
            if( s32Ret == Ces32WEthIf_eSuccess )
            {
                *pbFail = FALSE;
                u16Size = wSize;
            }
            else
            {
               *pbFail = TRUE;
               u16Size = 0;
            }
        }
        else
        {
            *pbFail = TRUE;
            u16Size = 0;
        }
    }
    else
    {
        *pbFail = TRUE;
        u16Size = 0;
    }
    
    return (u16Size);
}

/*****************************************************************************
T5Socket_Receive
Receive data on a non blocking client socket
Parameters:
    sock (IN) client socket
    wSize (IN) number of bytes to read
    pData (OUT) buffer where to store received data
    pbFail (OUT) set to TRUE on error: socket must be closed
return: number of bytes actually received
*****************************************************************************/

T5_WORD T5Socket_Receive (T5_SOCKET sock, T5_WORD wSize, T5_PTR pData,
                          T5_PTBOOL pbFail)
{
    TpvWEthIf_eTCPPcbHandle pvPcb;
    TU16 u16Index;
    TU16 u16ReceiveSize;
    
    if(sock < Du32T5Socket_iMaxConnexion)
    {
        //Look for PCB handler
        pvPcb = stT5Socket_iConnEntry[sock].pvSocketHdl;
        
        if(pvPcb != 0)
        {
            if(u16T5Socket_iRecvStart[sock] <= u16T5Socket_iRecvEnd[sock])
            {
                u16ReceiveSize = u16T5Socket_iRecvEnd[sock] - u16T5Socket_iRecvStart[sock];
            }
            else
            {
                u16ReceiveSize = T5MAX_FRAME + u16T5Socket_iRecvEnd[sock] - u16T5Socket_iRecvStart[sock];
            }
            
            if(u16ReceiveSize < wSize)
            {
                u16Index = 0;
            }
            else
            {
                for( u16Index = 0; u16Index < wSize; u16Index++ )
                {
                    ((TU8 *)pData)[u16Index] = pu8T5Socket_iRecvBuff[sock][u16T5Socket_iRecvStart[sock]];
                    u16T5Socket_iRecvStart[sock] = (u16T5Socket_iRecvStart[sock] + 1) % T5MAX_FRAME;
                }
            }
            
            //No error
            *pbFail = FALSE;
            
            return (u16Index);
        }
        else
        {
            *pbFail = TRUE;
            return 0;
        }
    }
    else
    {
        *pbFail = TRUE;
        return 0;
    }
}

/*****************************************************************************
T5Socket_CreateConnectedSocket
Create a non blocking socket connected to a remote server
- USED FOR MODBUS I/Os -
Parameters:
    szAddr (IN) address of the server
    wPort (IN) ethernet port number of the server
    pSocket (OUT) created socket if OK
    pbWait (OUT) set to TRUE if connect is not fully performed
return: OK or ERROR
*****************************************************************************/

#ifdef T5DEF_TCPCLIENT

T5_RET T5Socket_CreateConnectedSocket (T5_PTCHAR szAddr, T5_WORD wPort,
                                       T5_PTSOCKET pSocket,
                                       T5_PTBOOL pbWait,
                                       T5HND_CS pfCallback)
{
    TS32 s32Ret;
    
    if( (pvT5Socket_iTCPConnPCB == 0) && (u8T5Socket_iConnectInProgress == 0) )
    {
        // Connect to a port
        s32Ret = DstWMonitor_eApplication_Function.ps32WEthIf_eTcpConnect_Exe( ((TU8 *)szAddr), wPort, s32T5Socket_iTcpConnect_Exe );
    
        if( s32Ret != Ces32WEthIf_eSuccess )
        {
            *pSocket = T5_INVSOCKET;
            u8T5Socket_iConnectInProgress = 1;
            s32Ret = T5RET_ERROR;
        }
        else
        {
             *pSocket = DiT5Socket_iConnectedsocket;
        }
    }
    else
    {
        // Only one connection accepted
        *pSocket = T5_INVSOCKET;
        s32Ret = T5RET_ERROR;
    }
    
    return ((T5_RET)s32Ret);
}

#endif /*T5DEF_TCPCLIENT*/

/*****************************************************************************
T5Socket_CheckPendingConnect
Check pending connection a non blocking socket
The socket was created by T5Socket_CreateConnectedSocket()
- USED FOR MODBUS I/Os -
Parameters:
    sock (IN) client socket
    pbFail (OUT) set to TRUE on error: socket must be closed
return: OK if connection complete / ERROR if fail or still pending
*****************************************************************************/

#ifdef T5DEF_TCPCLIENT

T5_RET T5Socket_CheckPendingConnect (T5_SOCKET sock, T5_PTBOOL pbFail)
{
    if(u8T5Socket_iConnectInProgress == 2)
    {
        // Return OK if ready to write
        u8T5Socket_iConnectInProgress = 0;
        *pbFail = FALSE;
        return T5RET_OK;
    }
    else if(u8T5Socket_iConnectInProgress == 3)
    {
        // Error
        u8T5Socket_iConnectInProgress = 0;
        *pbFail = TRUE;
        return T5RET_ERROR;
    }
    else
    {
        // Still pending
        *pbFail = FALSE;
        return T5RET_ERROR;
    }
}

#endif /*T5DEF_TCPCLIENT*/

/****************************************************************************/
/* UDP management functions */

#ifdef T5DEF_UDP

// This function is called when a UDP packet is received
 void vT5Socket_iUdpReceive_Exe( void *pvArg, TpvWEthIf_eUDPPcbHandle FpvPcb, TstWEthIf_ePBuf *FpstRcvBuff, void *FpvAddr, TU16 Fu16Port )
{

}

/*****************************************************************************
T5Socket_UdpCreate
Create a UDP socket - must be non blocking socket
Parameters:
    wPort (IN) ethernet port number - bind required if not 0
    pSocket (OUT) created socket if OK
return: OK or ERROR
*****************************************************************************/

T5_RET T5Socket_UdpCreate (T5_WORD wPort, T5_PTSOCKET pSocket)
{
    TS32 s32Ret;
    
    if(pvT5Socket_iUDPSocket == 0)
    {
        pvT5Socket_iUDPSocket = DstWMonitor_eApplication_Function.ppstWEthIf_eUdpCreate_Exe( vT5Socket_iUdpReceive_Exe );
        
        if(pvT5Socket_iUDPSocket != 0)
        {
            *pSocket = DiT5Socket_iUDPSocket;
            
            if(wPort != 0)
            {
                s32Ret = DstWMonitor_eApplication_Function.ps32WEthIf_eUdpBind_Exe( pvT5Socket_iUDPSocket, 0, wPort );
                
                if(s32Ret != Ces32WEthIf_eSuccess)
                {
                    DstWMonitor_eApplication_Function.pvWEthIf_eUdpRemove_Exe( pvT5Socket_iUDPSocket );
                    pvT5Socket_iUDPSocket = 0;
                    *pSocket = T5_INVSOCKET;
                    return T5RET_ERROR;
                }
            }
            
            u16T5Socket_iUDPPort = wPort;
            return T5RET_OK
        }
        else
        {
            //Error
            *pSocket = T5_INVSOCKET;
            return T5RET_ERROR;
        }
    }
    else
    {
        // Only one accepted
        *pSocket = T5_INVSOCKET;
        return T5RET_ERROR;
    }
}

#ifdef T5DEF_SOCKETV2

T5_RET T5Socket_AcceptBroadcast (T5_SOCKET sock)
{
    return T5RET_ERROR;
}

#endif /*T5DEF_SOCKETV2*/

/*****************************************************************************
T5Socket_UdpSendTo
Send data on a UDP socket
Parameters:
    sock (IN) UDP socket
    pAddr (IN) pointer to the socket address
    pData (IN) pointer to data to send
    wSize (IN) number of bytes to send
return: OK or ERROR
*****************************************************************************/

T5_RET T5Socket_UdpSendTo (T5_SOCKET sock, T5_PTR pAddr,
                           T5_PTR pData, T5_WORD wSize)
{
    TstWEthIf_ePBuf *pstBuf;
    TU32 u32Index;
    
    // Allocate a pbuf
    pstBuf = DstWMonitor_eApplication_Function.ppstWEthIf_ePBufAlloc_Exe( wSize );
    if( pstBuf != 0 )
    {
        // Copy data in pbuf
        for( u32Index=0U; u32Index<Fu16Size; u32Index++ )
        {
        	((TU8 *)pstBuf->pvPayload)[u32Index] = pData[u32Index];
        }
        
        // send data over UDP
        (void)DstWMonitor_eApplication_Function.ps32WEthIf_eUdpSendTo_Exe( pvT5Socket_iUDPSocket, pstBuf, pAddr, Fu16Port );
        
        // Free the sent buffer
        (void)DstWMonitor_eApplication_Function.pu8WEthIf_ePBufFree_Exe( pstBuf );
    }
    else{} //empty ELSE
	
    return T5RET_ERROR;
}

/*****************************************************************************
T5Socket_UdpRecvFrom
Receive data on a UDP socket
Parameters:
    sock (IN) UDP socket
    pData (OUT) pointer to buffer where to copy received data
    wMax (IN) size of the buffer where to copy received data
    pAddr (OUT) pointer to the socket address
return: number or received bytes (0 if nothing or error)
*****************************************************************************/

T5_WORD T5Socket_UdpRecvFrom (T5_SOCKET sock, T5_PTR pData, T5_WORD wMax,
                              T5_PTR pAddr)
{
    return 0;
}

/*****************************************************************************
T5Socket_UdpAddrMake
Fill a socket address structure
Parameters:
    pAddr (OUT) pointer to the socket address to construct
    szAddr (IN) IP address
    wPort (IN) ETHERNET port number
return: OK or ERROR
*****************************************************************************/

void T5Socket_UdpAddrMake (T5_PTR pAddr, T5_PTCHAR szAddr, T5_WORD wPort)
{
}

/****************************************************************************/

#endif /*T5DEF_UDP*/

/****************************************************************************/

#endif /*T5DEF_ETHERNET*/

/* eof **********************************************************************/
