
rem Kill all OSHMI processes.
rem If there are services, must be run as administrator!

cd c:\oshmi\bin
start webserver.exe
start /wait PING -n 5 127.0.0.1
start "" "%USERPROFILE%\Desktop\OSHMI\Alarms Viewer.lnk"
start /wait PING -n 1 127.0.0.1
start "" "%USERPROFILE%\Desktop\OSHMI\Screen Viewer.lnk"

