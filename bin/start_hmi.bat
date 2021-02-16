
rem Starts some OSHMI processes and viewers.

cd c:\oshmi\bin
start webserver.exe

start /b /wait PING -n 4 127.0.0.1 | rem
timeout 4

rem Choose a protocol driver
start qtester104.exe

start /b /wait PING -n 5 127.0.0.1 | rem
timeout 5
start "" "%USERPROFILE%\Desktop\OSHMI\Alarms Viewer.lnk"

start /b /wait PING -n 2 127.0.0.1 | rem
timeout 2
start "" "%USERPROFILE%\Desktop\OSHMI\Events Viewer.lnk"

start /b /wait PING -n 2 127.0.0.1 | rem
timeout 2
rem start "" "%USERPROFILE%\Desktop\OSHMI\Screen Viewer.lnk"
start "" "c:\oshmi\browser\chrome" --user-data-dir=c:\oshmi\browser-data --process-per-site --no-sandbox --disable-popup-blocking --no-proxy-server --bwsi --disable-extensions --disable-sync --no-first-run --app=http://127.0.0.1:51909/htdocs/screen.html?SELTELA=../svg/KAW2.svg

