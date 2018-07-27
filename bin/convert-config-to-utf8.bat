@echo off
c:\oshmi\bin\iconv.exe -f ISO8859-1 -t UTF-8 c:\oshmi\conf\point_list.txt > nul
if %ERRORLEVEL% == 0 (
copy c:\oshmi\conf\point_list.txt c:\oshmi\conf\point_list-non-utf8.bak
c:\oshmi\bin\iconv.exe -f ISO8859-1 -t UTF-8 c:\oshmi\conf\point_list.txt > c:\oshmi\conf\plutf8.temp.txt
copy c:\oshmi\conf\plutf8.temp.txt c:\oshmi\conf\point_list.txt
del c:\oshmi\conf\plutf8.temp.txt
echo CONVERTION OK POINT_LIST
) ELSE (
echo CANNOT CONVERT POINT_LIST
)

c:\oshmi\bin\iconv.exe -f ISO8859-1 -t UTF-8 c:\oshmi\conf\sage_id.txt > nul
if %ERRORLEVEL% == 0 (
copy c:\oshmi\conf\sage_id.txt c:\oshmi\conf\sage_id-non-utf8.bak
c:\oshmi\bin\iconv.exe -f ISO8859-1 -t UTF-8 c:\oshmi\conf\sage_id.txt > c:\oshmi\conf\plutf8.temp.txt
copy c:\oshmi\conf\plutf8.temp.txt c:\oshmi\conf\sage_id.txt
del c:\oshmi\conf\plutf8.temp.txt
echo CONVERTION OK SAGE_ID
) ELSE (
echo CANNOT CONVERT SAGE_ID
)

c:\oshmi\bin\iconv.exe -f ISO8859-1 -t UTF-8 c:\oshmi\svg\screen_list.js > nul
if %ERRORLEVEL% == 0 (
copy c:\oshmi\svg\screen_list.js c:\oshmi\svg\screen_list-nonutf8.js
c:\oshmi\bin\iconv.exe -f ISO8859-1 -t UTF-8 c:\oshmi\svg\screen_list.js > c:\oshmi\svg\sl.temp.js
copy c:\oshmi\svg\sl.temp.js c:\oshmi\svg\screen_list.js
del c:\oshmi\svg\sl.temp.js
echo CONVERTION OK SCREEN_LIST
) ELSE (
echo CANNOT CONVERT SCREEN_LIST
)


