:: Brief	: This is a batch file for invoking the Vivado and initially run a tickle file in it
::			: The batch file is only compatible with Windows PCs
::			: It's been tested in both Windows 7 and Windows 10
:: Note		: User must specify a valid tickle file name
::			: Do not forget to modify the Vivado version also
:: Author 	: Caglayan DOKME, caglayan.dokme@gmail.com
:: Date		: September 13, 2020

@ECHO OFF

:: User specified variables
:: Main tickle file name to be used first in Vivado
set tclfile=BspDevelopment.tcl
set vivadoVersion=2017.2

:: Derived application path
set vivadoPath=C:\Xilinx\Vivado\%vivadoVersion%\bin

:: Check if the given file exist in the current directory
:: %~dp0 represents the current path
if not exist %~dp0\%tclfile% (
	msg "%username%" Given %tclfile% file doesn't exist.
	
	timeout 3 >nul
	exit
) else (
	ECHO Tickle file %tclfile% found successfully
)

if not exist %vivadoPath%\unwrapped\win64.o\vvgl.exe (
	msg "%username%" Cannot find the Vivado executable under given path: %vivadoPath%
	
	timeout 3 >nul
	exit
) 

:: Invokes the Vivado
:: If you have a different version or a different installation path, you should modify the version variable
start %vivadoPath%\unwrapped\win64.o\vvgl.exe ^
%vivadoPath%\vivado.bat ^
-mode tcl ^
-nojournal ^
-nolog ^
-notrace ^
-source %~dp0\%tclfile%

:: Operation was successfull
ECHO Vivado invoked, the program will run the given TCL file
ECHO Caglayan DOKME salutes you :)

timeout 3 >nul
exit
