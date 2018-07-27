
rem Starts some OSHMI processes and viewers.

cd c:\oshmi\bin
start webserver.exe
start /wait PING -n 5 127.0.0.1
start "" "%USERPROFILE%\Desktop\OSHMI\Alarms Viewer.lnk"
start /wait PING -n 2 127.0.0.1
start "" "%USERPROFILE%\Desktop\OSHMI\Screen Viewer.lnk"

