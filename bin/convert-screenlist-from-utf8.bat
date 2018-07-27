@echo off
c:\oshmi\bin\iconv.exe -f UTF-8 -t ISO8859-1 c:\oshmi\svg\screen_list.js > nul
if %ERRORLEVEL% == 0 (
c:\oshmi\bin\iconv.exe -f UTF-8 -t ISO8859-1 c:\oshmi\svg\screen_list.js > c:\oshmi\svg\screen_list-nonutf8.js
echo CONVERTION OK
) ELSE (
echo CANNOT CONVERT
)

