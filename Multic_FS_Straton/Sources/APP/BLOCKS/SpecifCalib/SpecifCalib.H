/******************************************************************************
*
*   COPYRIGHT (C) ACTIA  2012
*
*   Toute reproduction de ce document même partielle, est strictement interdite
*   sans autorisation écrite
*
*   Version     : V1.00
*   Date        : 01/06/16
*   Evolution   : BBea Straton Multic S
******************************************************************************/

//*************************************************************************
//   DEFINITION
//*************************************************************************
/* Return codes to apply */
#define Ds32Parameters		(T5_SHORT)-2
#define Ds32Error					(T5_SHORT)-1
#define Ds32Success				(T5_SHORT)0

/* Length of InterpretationCalib0X */
#define Du8LengthNameCalib00		(T5_BYTE)9
#define Du8LengthNameCalib01		(T5_BYTE)14
#define Du8LengthNameCalib02		(T5_BYTE)14
#define Du8LengthNameCalib03		(T5_BYTE)14
#define Du8LengthNameCalib04		(T5_BYTE)14
//*************************************************************************
//   MACRO
//*************************************************************************

//*************************************************************************
//   TYPEDEF
//*************************************************************************
typedef struct 
{
		T5_CHAR NameId[15];
		T5_BYTE NameLength;
		T5_WORD IdCode;
} T5STR_CALIBID;

extern T5STR_CALIBID CalibList[Ceu16WMonitor_eParameterNbr];		/* Dimension should be same as Ceu16WMonitor_eParameterNbr */


//*************************************************************************
//   EXPORT
//*************************************************************************

//*************************************************************************
//   PROTO
//*************************************************************************

T5_DWORD SPECIFCALIB_RD (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
T5_DWORD SPECIFCALIB_WR (T5_WORD wCommand, T5PTR_DB pBase, T5_PTR pClass, T5_PTR pInst, T5_PTWORD pArgs);
