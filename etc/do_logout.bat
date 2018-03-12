REM Logout user
REM Desloga o usuário

REM uncomment for services configuration
rem goto services

taskkill /F /IM chrome.exe
taskkill /F /IM mon_proc.exe
taskkill /F /IM iec104m.exe
taskkill /F /IM qtester104.exe
start /wait ..\bin\close_webserver.vbs

REM para windows que tem shutdown
shutdown /l /f
IF %ERRORLEVEL% NEQ 0 ..\bin\shutdwn -f -l -t 10

REM para linux em wine
REM /usr/bin/gnome-session-save --shutdown-dialog

taskkill /F /IM nginx.exe
taskkill /F /IM php-cgi.exe

..\bd\terminate_soe.bat
..\bd\terminate_dumpdb.bat
..\bd\terminate_hist.bat

ping 127.0.0.1 -n 2 -w 1000
taskkill /F /IM webserver.exe


:services

taskkill /F /IM chrome.exe
taskkill /F /IM mon_proc.exe

REM para windows que tem shutdown
shutdown /l /f
IF %ERRORLEVEL% NEQ 0 ..\bin\shutdwn -f -l -t 10

