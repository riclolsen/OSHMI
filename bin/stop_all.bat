
rem Kill all OSHMI processes.
rem If there are services, must be run as administrator!

cd c:\oshmi\bin
call stop.bat
call ..\etc\stop_services.bat
start /wait ..\nginx_php\stop_nginx_php.bat
taskkill /F /IM cmd.exe /T