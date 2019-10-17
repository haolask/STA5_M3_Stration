//
// !!!!!!!!!!!! Only for testing purposes to demonstrate flash download speed via J-Link Commander, do *not* take over into DS-5 !!!!!!!!!!!!!
// Forces J-Link to program, even if flash contents already match data to be programmed
//
rx 100
exec SetSkipProgOnCRCMatch=0
//
// Download application into QSPI flash
//
loadbin Release/Multic-S_Straton.bin,0x18280000
verifybin Release/Multic-S_Straton.bin,0x18280000
exit