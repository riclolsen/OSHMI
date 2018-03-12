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
..\bin\wget -N --no-check-certificate %proxy% "https://notepad-plus-plus.org/repository/7.x/7.2.2/npp.7.2.2.Installer.exe"

cls
rem SQlite database file browser. Useful to retrive/recover/debug data. (download from http://sqlitebrowser.org/)
rem ..\bin\wget -N --no-check-certificate %proxy% "https://github.com/sqlitebrowser/sqlitebrowser/releases/download/v3.9.1/DB.Browser.for.SQLite-3.9.1-win32.exe"

cls
echo SCADA SVG Graphics Editor: needed to create and edit screens. 
echo The 0.481/303 version is old but trusty and stable.
echo The 0.91/416 version is new but it's working very nicely as I am testing it.
echo Any intermediate version is not recommended!
..\bin\wget -N --no-check-certificate %proxy% "https://sourceforge.net/projects/sage-scada/files/SAGE v3.03/inkscape0481_sage303.exe"
..\bin\wget -O inkscape091_sage416.exe --no-check-certificate --user-agent="Mozilla/5.0 (Macintosh; Intel Mac OS X 10.8; rv:21.0) Gecko/20100101 Firefox/21.0"  %proxy% "https://integraxor.com/download/sage/?091sage416"

echo Please Install one or another: 
echo inkscape0481_sage303.exe or inkscape091_sage416.exe
rem start /wait inkscape0481_sage303.exe
start /wait inkscape091_sage416.exe

echo To create certificates and configure https in Nginx
..\bin\wget -N --no-check-certificate %proxy% http://slproweb.com/download/Win32OpenSSL_Light-1_1_0f.exe

rem start /wait npp.7.2.2.Installer.exe
rem start /wait DB.Browser.for.SQLite-3.9.1-win32.exe
