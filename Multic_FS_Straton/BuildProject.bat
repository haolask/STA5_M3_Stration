@echo off
rem  This batch file will build the Project code.
rem  Please change the (Toolchain and GNU tools) path below if it is different in your system. 
rem  
rem  Use make option (-f "D:\ACTIx\MULTIC-S\SW_trunk\PROJECTS\Multic-S_WORKSHOP\Multic-S_Straton\xxxxx\makefileXX") to specify another makefile
rem
rem  [WIN7] Increase the size of the command prompt window (cmd.exe)
rem  - Firstly you should open a command prompt:
rem  - Start > Run > type "cmd" and press Enter
rem  - Open a command prompt by clicking the Start button, type cmd and press Enter to validate.
rem  - Right click on the title bar and select Properties.
rem  - Select the Layout tab and enter your custom values in the "Windows Size" Section.
rem    e.g. window size       - Width  = 160
rem    e.g. scree buffer size - Height = 9999
rem  => This configuration will be saved and used for later command prompt's
rem  
echo BUILD Multic-S_Straton program 
echo ===============================================================
:OPTIONS
echo OPTIONS
echo 1 = Debug   - will build the project in FOLDER: \Debug
echo 2 = Release - will build the project in FOLDER: \Release
echo 3 = Exit
SET /P REPLY=Choose options (1,2 or 3)
if "%REPLY%"== "1" (goto DEBUG)
if "%REPLY%"== "2" (goto RELEASE)
if "%REPLY%"== "3" (goto END)
echo ERROR: Please select 1 or 2 or 3 only.
goto OPTIONS

:DEBUG
if not exist ..\..\compile\yagarto-tools-20121018\bin\make.exe goto ERROR1 
if not exist Debug\makefile goto ERROR2
echo --------------------------------------------------------------
cd "Debug"
..\..\..\compile\yagarto-tools-20121018\bin\make.exe clean
..\..\..\compile\yagarto-tools-20121018\bin\make.exe all --debug -j4
echo That's it.
goto END

:RELEASE
if not exist ..\..\compile\yagarto-tools-20121018\bin\make.exe goto ERROR1 
if not exist Release\makefile goto ERROR3
echo --------------------------------------------------------------
cd "Release"
..\..\..\compile\yagarto-tools-20121018\bin\make.exe clean
..\..\..\compile\yagarto-tools-20121018\bin\make.exe all --debug -j4
echo That's it.
goto END

:ERROR1
echo --------------------------------------------------------------
echo File "..\..\compile\yagarto-tools-20121018\bin\make.exe" does not exist.
echo --------------------------------------------------------------
goto END

:ERROR2
echo --------------------------------------------------------------
echo File "Debug\makefile" does not exist.
echo --------------------------------------------------------------
goto END

:ERROR3
echo --------------------------------------------------------------
echo File "Release\makefile" does not exist.
echo --------------------------------------------------------------
goto END

:END
pause
