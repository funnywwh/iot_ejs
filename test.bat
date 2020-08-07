@echo off

set CUR=%cd%

cd ..\..\project

set PWD=%cd%
cd %PWD%
set TTS_ENABLE=ON
set JSTEST_ENABLE=ON

call ..\tools\core_launch.bat js

cd %PROJECT_OUT% & cmake ..\.. -G Ninja & ninja & cd ..\..\project


cd %CUR%