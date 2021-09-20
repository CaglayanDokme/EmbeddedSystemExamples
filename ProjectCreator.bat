:: Brief	: This is a batch file for invoking the Vivado and initially run a tickle file in it
::		    : The batch file is only compatible with Windows PCs
::		    : It's been tested in both Windows 7 and Windows 10
:: Note		: User must specify a valid tickle file name and a Vivado version
::		    : It can also be done using the command line input parameters
:: Usage	: You can either specift the Vivado version or not
::		    : If you already modified the batch script, then just run it
::		    : > .\Magic.bat
::		    : >	.\Magic.bat BspDevelopment.tcl
::		    : > .\Magic.bat BspDevelopment.tcl 2020.1
:: Author 	: Caglayan DOKME, caglayandokme@gmail.com
:: Date		: September 13, 2021 -> Created.
::			: September 19, 2021 -> Input argument parsing feature added.

@ECHO OFF

:: User specified variables
:: Main tickle file name to be used first in Vivado
set tclfile=System.tcl
set vivadoVersion=2017.2

:: Parse the program arguments
:: First input is the tickle file name
:: Second input is the Vivado version
if not "%1" == "" (
	set tclFile=%1
	
	if not "%2" == "" (
		set vivadoVersion=%2
		
		if not "%3" == "" (
			msg Too many input arguments given.
			
			timeout 3 >nul
			exit
		)
	)
) 

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

timeout 5 >nul
exit
