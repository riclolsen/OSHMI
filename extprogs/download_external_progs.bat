echo off
echo Download external software.
echo Not included in OSHMI installer because of license or size restrictions.

rem Configure your proxy here if needed
rem SET proxy="-e http-proxy=10.63.1.1:128"
SET proxy=""


echo Downloading some useful tools from Sysinternals...
..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/autoruns.exe
..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/autoruns.chm
..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/procexp.exe
..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/procexp.chm
rem ..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/disk2vhd.exe
rem ..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/disk2vhd.chm
..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/procmon.exe
..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/pskill.exe
..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/psping.exe
..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/tcpvcon.exe
..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/tcpview.exe
..\bin\wget -N --no-check-certificate %proxy% http://live.sysinternals.com/tcpview.chm

cls
echo Downloading Notepad++ to edit text files
..\bin\wget -N --no-check-certificate %proxy% "https://notepad-plus-plus.org/repository/7.x/7.5.6/npp.7.5.6.Installer.exe"

cls
rem SQlite database file browser. Useful to retrive/recover/debug data. (download from http://sqlitebrowser.org/)
rem ..\bin\wget -N --no-check-certificate %proxy% "https://github.com/sqlitebrowser/sqlitebrowser/releases/download/v3.9.1/DB.Browser.for.SQLite-3.9.1-win32.exe"

echo To create certificates and configure https in Nginx
..\bin\wget -N --no-check-certificate %proxy% http://slproweb.com/download/Win32OpenSSL_Light-1_1_0f.exe

rem start /wait npp.7.2.2.Installer.exe
rem start /wait DB.Browser.for.SQLite-3.9.1-win32.exe
