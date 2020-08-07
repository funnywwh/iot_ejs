@echo off

set CUR=%cd%

cd ..\..\project

set PWD=%cd%
cd %PWD%
set TTS_ENABLE=ON
rem set LOG_TO_UART1=ON
rem set LOG_TO_UART2=ON

call ..\tools\core_launch.bat js

cd %PROJECT_OUT% & cmake ..\.. -G Ninja & ninja & cd ..\..\project


cd %CUR%