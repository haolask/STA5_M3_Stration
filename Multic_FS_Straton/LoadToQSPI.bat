@echo off
rem  This batch file will load the program code into single quad mode QSPI devices.
rem  - If schematice (PCB popolation) and BOOT loader are configurated to work with
rem    two QSPI flash devices in parallele, 
rem    > please change the option to dual quad mode, by using: R7S721001_DualSPI
rem  Please change the JLINK.EXE path below if it is different in your system. 
rem  
echo LOAD Multic-S_Straton CODE INTO QSPI
echo ===============================================================
echo * Remember to update the JLink.exe path specified in this batch file if using *
echo * a different version than that installed by the RSK+RZA1H_V2 installer.      *
echo *                                                                             *
echo * This batch file was tested and works with Ver 5.00j of the JLink.exe        *
echo *                                                                             *
echo * Please refer to the RSK+RZA1H Tutorial Manual for more information on       * 
echo * generating the required binary files.                                       *
echo ===============================================================
:OPTIONS
echo OPTIONS
echo 1 = Debug   - Load the .bin file from Debug   folder into QSPI and execute
echo 2 = Release - Load the .bin file from Release folder into QSPI and execute
echo 3 = Exit
SET /P REPLY=Choose options (1,2 or 3)
if "%REPLY%"== "1" (goto DEBUG)
if "%REPLY%"== "2" (goto RELEASE)
if "%REPLY%"== "3" (goto END)
if "%REPLY%"== "4" (goto REMOTE)
echo ERROR: Please select 1,2 or 3 only.
goto OPTIONS

:REMOTE
if not exist Debug\Multic-S_Straton.bin goto ERROR4
echo Remove power (12V) to the board before continuing. 
echo BOOT-Mode3 must be selected on the PCB:  1 0 1 = (serial flash booting)
echo Reconnect power (12V) to the board before continuing. 
pause
echo --------------------------------------------------------------
"C:\RENESAS\SEGGER\JLinkARM_V486b\JLink.exe" -speed 12000 -if JTAG -device R7S721001 -CommanderScript LoadToQSPI_RemoteServer.Command
echo --------------------------------------------------------------
echo This is the Multic-S_Straton
echo You may need to turn off the power to the board and turn on 
echo again to run the tutorial.
echo --------------------------------------------------------------
echo That's it.
goto END
:ERROR4
echo --------------------------------------------------------------
echo File "Debug\Multic-S_Straton.bin" does not exist.
echo Please build the project in debug mode first.
echo --------------------------------------------------------------
goto END

:DEBUG
if not exist Debug\Multic-S_Straton.bin goto ERROR1 
echo Remove power (12V) to the board before continuing. 
echo BOOT-Mode3 must be selected on the PCB:  1 0 1 = (serial flash booting)
echo Reconnect power (12V) to the board before continuing. 
pause
echo --------------------------------------------------------------
rem "C:\RENESAS\SEGGER\JLinkARM_V484b\JLink.exe" -speed 12000 -if JTAG -device R7S721001_DualSPI -CommanderScript LoadToQSPI_Debug.Command
rem "C:\RENESAS\SEGGER\JLinkARM_V486b\JLink.exe" -speed 12000 -if JTAG -device R7S721001 -CommanderScript LoadToQSPI_Debug.Command
"C:\RENESAS\SEGGER\JLink_V500j\JLink.exe" -speed 12000 -if JTAG -device R7S721001 -CommanderScript LoadToQSPI_Debug.Command
echo --------------------------------------------------------------
echo This is the Multic-S_Straton
echo You may need to turn off the power to the board and turn on 
echo again to run the tutorial.
echo --------------------------------------------------------------
echo That's it.
goto END
:ERROR1
echo --------------------------------------------------------------
echo File "Debug\Multic-S_Straton.bin" does not exist.
echo Please build the project in debug mode first.
echo --------------------------------------------------------------
goto END

:RELEASE
if not exist Release\Multic-S_Straton.bin goto ERROR2 
echo Remove power (12V) to the board before continuing. 
echo BOOT-Mode3 must be selected on the PCB:  1 0 1 = (serial flash booting)
echo Reconnect power (12V) to the board before continuing. 
pause
echo --------------------------------------------------------------
rem "C:\RENESAS\SEGGER\JLinkARM_V484b\JLink.exe" -speed 12000 -if JTAG -device R7S721001_DualSPI -CommanderScript LoadToQSPI_Release.Command
rem "C:\RENESAS\SEGGER\JLinkARM_V486b\JLink.exe" -speed 12000 -if JTAG -device R7S721001 -CommanderScript LoadToQSPI_Release.Command
"C:\RENESAS\SEGGER\JLink_V500j\JLink.exe" -speed 12000 -if JTAG -device R7S721001 -CommanderScript LoadToQSPI_Release.Command
echo --------------------------------------------------------------
echo This is the Multic-S_Straton
echo You may need to turn off the power to the board and turn on 
echo again to run the tutorial.
echo --------------------------------------------------------------
echo That's it.
goto END
:ERROR2
echo --------------------------------------------------------------
echo File "Release\Multic-S_Straton.bin" does not exist.
echo Please build the project in release mode first.
echo --------------------------------------------------------------
goto END

:END
pause