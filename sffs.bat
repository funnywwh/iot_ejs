set HEXPATH=%1
set SFFS_ROOT=%cd:\=/%/
set SFFS_JSON=sffs.json
set SFFSIMG=%HEXPATH:\=/%/sffs.img
set SFFSCMD=dtools.exe fbdevgen

cd %SFFS_ROOT%

.\bin2carray\bin2carray.exe fsdep.c js\lib\iot.js js\lib\test.js
%SFFSCMD% %SFFS_JSON% %SFFSIMG%
