/******************************************************************************
*
*   COPYRIGHT (C) ACTIA  2012
*
*   Toute reproduction de ce document même partielle, est strictement interdite
*   sans autorisation écrite
*
*   Version     : V1.00
*   Date        : 17/09/12
*   Evolution   : PCou Straton Multic 2
******************************************************************************/

//*************************************************************************
//   DEFINITION
//*************************************************************************

//*************************************************************************
//   MACRO
//*************************************************************************

//*************************************************************************
//   TYPEDEF
//*************************************************************************
/* Config item for each parameter in the Config Table */
typedef struct
{
   unsigned short usIdent;          // byte   0 & 1
   unsigned short usAddressOffset;  // bytes  2 & 3
   unsigned char  ucSize;           // byte   4
   unsigned char  ucPosition;       // byte   5
   unsigned short usDefaultValue;   // bytes  6 & 7
   unsigned short usMinValue;       // bytes  8 & 9
   unsigned short usMaxValue;       // bytes 10 & 11
}TstParam_AppliParamConfig;
/* check compliance with customdiag.h / .c */
//*************************************************************************
//   EXPORT
//*************************************************************************

//*************************************************************************
//   PROTO
//*************************************************************************

T5_DWORD PARAM_RD  (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
T5_DWORD PARAM_WR  (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
