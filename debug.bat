@echo off
start devenv %*

goto :eof


REM Functions

:initvs
	if defined VCINSTALLDIR goto :eof
	call "%*vsvars32.bat" 1>nul
	if not errorlevel 1 goto :eof
	popd
	echo.%cmdcmdline% | findstr /C:"%SystemRoot%\system32\cmd.exe" 1>nul
	if not errorlevel 1  pause	
	EXIT
	

:vsvars32
	if defined VCINSTALLDIR goto :eof
	if defined VS150COMNTOOLS call :initvs %VS150COMNTOOLS%
	if defined VS140COMNTOOLS call :initvs %VS140COMNTOOLS%
	if defined VS130COMNTOOLS call :initvs %VS130COMNTOOLS%
	if defined VS120COMNTOOLS call :initvs %VS120COMNTOOLS%
	if defined VS110COMNTOOLS call :initvs %VS110COMNTOOLS%
	if defined VS100COMNTOOLS call :initvs %VS100COMNTOOLS%
	if defined VS90COMNTOOLS call :initvs %VS90COMNTOOLS%
	if defined VS80COMNTOOLS call :initvs %VS80COMNTOOLS%
	if defined VS71COMNTOOLS call :initvs %VS71COMNTOOLS% & set extraflags=/Og /Wp64 /MT
	if defined VS70COMNTOOLS call :initvs %VS70COMNTOOLS% & set extraflags=/Og /Wp64 /MT
	if defined VS60COMNTOOLS call :initvs %VS60COMNTOOLS%
	if defined VCINSTALLDIR goto :eof
	echo Visual Studio is not installed. Aborting.
	popd
	echo.%cmdcmdline% | findstr /C:"%SystemRoot%\system32\cmd.exe" 1>nul
	if not errorlevel 1  pause	
	EXIT
