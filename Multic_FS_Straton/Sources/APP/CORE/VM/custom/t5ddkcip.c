/*****************************************************************************
T5Memory.c : system memory management - TO BE COMPLETED WHEN PORTING
(c) COPALP 2002
*****************************************************************************/

#include "t5vm.h"
#include "t5ddkc.h"

#ifdef  T5DEF_DDKC

/****************************************************************************/

void T5DDKC_IpInit (void)
{
}

void T5DDKC_IpExit (void)
{
}

/****************************************************************************/

//T5DDK_DECLAREHANDLER (DDK_ACTIA)


T5HDN_DDKC T5DDKC_IpGetHandler (T5_PTCHAR szDriverName)
{
//    T5DDK_LINKHANDLER ("ACTIA", DDK_ACTIA);

    return NULL;
}

/****************************************************************************/

#endif /*T5DEF_DDKC*/

/* eof **********************************************************************/
