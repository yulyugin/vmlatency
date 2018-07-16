@ECHO OFF

IF NOT EXIST C:\WinDDK\7600.16385.1 GOTO UNKNOWN_WDK

CALL C:\WinDDK\7600.16385.1\bin\setenv.bat C:\WinDDK\7600.16385.1\ fre x64 WIN7 2>&1 > NUL
CD /d %~dp0/win
build /F /w /c
CD /d %~dp0

GOTO EXIT

:UNKNOWN_WDK
ECHO *** Can't find useful Windows Driver Kit. ***
ECHO *** Only Windows Driver Kit Version 7.1.0 is supported. ***

:EXIT
