REM Reboots the computer
REM Reinicia o sistema operacional

wmic PROCESS WHERE "COMMANDLINE LIKE '%%oshmi\\browser\\chrome.exe%%'" CALL TERMINATE
call c:\oshmi\etc\stop_services.bat
taskkill /F /IM mon_proc.exe
taskkill /F /IM iec104m.exe
taskkill /F /IM qtester104.exe
taskkill /F /IM qtester104.exe
start /wait c:\oshmi\bin\close_webserver.vbs
call c:\oshmi\bin\stop_all.bat

REM para windows que tem shutdown
shutdown /f /r /t 10
IF %ERRORLEVEL% NEQ 0 ..\bin\shutdwn -f -r -t 10

REM para linux em wine
REM /usr/bin/gnome-session-save --shutdown-dialog

call c:\oshmi\nginx_php\stop_nginx_php.bat
taskkill /F /IM nginx.exe
taskkill /F /IM php-cgi.exe

call c:\oshmi\bd\terminate_soe.bat
call c:\oshmi\bd\terminate_dumpdb.bat
call c:\oshmi\bd\terminate_hist.bat

ping 127.0.0.1 -n 2 -w 1000
taskkill /F /IM webserver.exe

