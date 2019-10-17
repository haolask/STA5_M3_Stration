/*****************************************************************************
T5Serial.c : serial interface - TO BE FILLED WHEN PORTING
(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"

#ifdef T5DEF_SERIAL

#include "std_type.h"
#include "monitor.h"

/*

structure T5_SERIAL:

int handle; //0 close / 1 open

*/
//#define T5SERIAL_DBG 1

/* Availables UART Baudrates   */                   
#define	Du32WUart_eBaudrate_230400		230400U
#define	Du32WUart_eBaudrate_115200		115200U
#define	Du32WUart_eBaudrate_57600		57600U
#define	Du32WUart_eBaudrate_38400		38400U
#define	Du32WUart_eBaudrate_19200		19200U
#define	Du32WUart_eBaudrate_14400		14400U
#define	Du32WUart_eBaudrate_10400		10400U
#define	Du32WUart_eBaudrate_9600		9600U
#define	Du32WUart_eBaudrate_4800		4800U
#define	Du32WUart_eBaudrate_2400		2400U
#define	Du32WUart_eBaudrate_1200		1200U

/*****************************************************************************
T5Serial_Initialize
Initialize a comm port descriptor (reset to invalid state)
Parameters:
    pSerial (OUT) pointer to the port descriptor
*****************************************************************************/

void T5Serial_Initialize (T5_PTSERIAL pSerial)
{
#ifdef T5SERIAL_DBG
#else
   pSerial->handle = 0;
   //iDUart_eInit_Exe (CeuiDUart_eCtrlrA,1,0);
#endif
}

/*****************************************************************************
T5Serial_IsValid
Test if a comm port descriptor is valid (port is open)
Warning: data may be loaded on Hot Restart. Check if not null is not enough!
Parameters:
    pSerial (IN) pointer to the port descriptor
Return: TRUE if the port is open
*****************************************************************************/

T5_BOOL T5Serial_IsValid (T5_PTSERIAL pSerial)
{
#ifdef T5SERIAL_DBG
    return FALSE; // debug pcou
#else
    T5_BOOL bRetVal = FALSE;
    if(pSerial->handle)
    {
       bRetVal = TRUE;
    }
    return (bRetVal);
#endif
}

/*****************************************************************************
T5Serial_Open
Open a serial port
Parameters:
    pSerial (IN/OUT) pointer to the port descriptor
    szConfig (IN) configuration strings (comm settings)
Return: TRUE if the port is open successfully
*****************************************************************************/

T5_BOOL T5Serial_Open (T5_PTSERIAL pSerial, T5_PTCHAR szConfig)
{
#ifdef T5SERIAL_DBG
    return FALSE; // debug pcou
#else
   pSerial->handle = 1;
    DstWMonitor_eApplication_Function.pvPActiDebug_eConfigBaudate_Exe(Du32WUart_eBaudrate_115200);
    return TRUE;
#endif
}

/*****************************************************************************
T5Serial_Close
Close a serial port
Parameters:
    pSerial (IN/OUT) pointer to the port descriptor
*****************************************************************************/

void T5Serial_Close (T5_PTSERIAL pSerial)
{
#ifdef T5SERIAL_DBG
#else
   pSerial->handle = 0;
   DstWMonitor_eApplication_Function.pvPActiDebug_eClose_Exe();
#endif
}

/*****************************************************************************
T5Serial_Send
Send data to a serial port
Parameters:
    pSerial (IN/OUT) pointer to the port descriptor
    wSize (IN) number of bytes to send
    pData (IN) pointer to data to send
Return: number of bytes sent
*****************************************************************************/

static T5_BYTE buf[1024];  // debug pcou
static int c = 0;          // debug pcou


T5_WORD T5Serial_Send (T5_PTSERIAL pSerial, T5_WORD wSize, T5_PTR pData)
{
#ifdef T5SERIAL_DBG
    return 0; // debug pcou
#else
   T5_WORD wSendBytes = 0;

   DstWMonitor_eApplication_Function.piPActiDebug_eTXBuffer_Exe((unsigned char *)pData, (unsigned short)wSize); //(unsigned char *FpucTXBuffer, unsigned short FucLenght)
   wSendBytes = wSize;

   T5_MEMSET (buf, 0, sizeof (buf));   // debug pcou
   c = 0;                              // debug pcou

   return (wSendBytes);
#endif
}

/*****************************************************************************
T5Serial_Receive
Receive data on a serial port
Parameters:
    pSerial (IN/OUT) pointer to the port descriptor
    wSize (IN) wished number of bytes (maximum read)
    pData (IN) pointer to buffer where to store received bytes
Return: number of bytes received
*****************************************************************************/

T5_WORD T5Serial_Receive (T5_PTSERIAL pSerial, T5_WORD wSize, T5_PTR pData)
{
#ifdef T5SERIAL_DBG
    return 0; // debug pcou
#else
   T5_WORD wRecBytes; // debug pcou  = 0;

   T5_MEMSET (buf, 0, sizeof (buf));   // debug pcou
   c = 0;                              // debug pcou
   wRecBytes = 0;                              // debug pcou

   while(wRecBytes <wSize)
   {
//    //if( 0 == iDUart_eGetByte_Exe (CeuiDUart_eCtrlrA,(&(((unsigned char *)pData)[wRecBytes]))))
      if( 0 == DstWMonitor_eApplication_Function.piPActiDebug_eReadOneByte_Exe(&(((unsigned char *)pData)[wRecBytes])))
      {
         buf[c++] = ((unsigned char *)pData)[wRecBytes];
         wRecBytes++;
      }
      else
      {
        int istop = 0;
        break;
      }
    }
//
   if(wSize<=wRecBytes)
   {
      int istop = 0;
    }

   return (wRecBytes);
#endif
}

/****************************************************************************/

#endif /*T5DEF_SERIAL*/

/* eof **********************************************************************/
