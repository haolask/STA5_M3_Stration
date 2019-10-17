/******************************************************************************
*
*   COPYRIGHT (C) ACTIA  2012
*
*   Toute reproduction de ce document même partielle, est strictement interdite
*   sans autorisation écrite
*
*   Version     : V1.00
*   Date        : 01/03/16
*   Evolution   : BBea Straton Multic S
******************************************************************************/

//*************************************************************************
//   DEFINITION
//*************************************************************************
/* Return codes to apply */
#define Ds16NotProcessed	(T5_SHORT)-3
#define Ds16Parameters		(T5_SHORT)-2
#define Ds16Error					(T5_SHORT)-1
#define Ds16Success				(T5_SHORT)0
#define Ds16StatusIsInactive				(T5_SHORT)2

/* Missing return codes from monitor (inherit from bsp_core.h) - To be removed/updated once available */
#define Ds32WCore_eSuccessActiveStatus		 1

//*************************************************************************
//   MACRO
//*************************************************************************

//*************************************************************************
//   TYPEDEF
//*************************************************************************

//*************************************************************************
//   EXPORT
//*************************************************************************

//*************************************************************************
//   PROTO
//*************************************************************************

T5_DWORD RTC_RD (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
T5_DWORD RTC_WR (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

T5_DWORD RTC_TIMER_RD (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
T5_DWORD RTC_TIMER_WR (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
T5_DWORD RTC_TIMER_CLR (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);

T5_DWORD RTC_ALARM_RD (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
T5_DWORD RTC_ALARM_WR (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
T5_DWORD RTC_ALARM_CLR (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
