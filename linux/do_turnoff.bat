REM Turn off the computer
REM Desliga a máquina

start z:\usr\bin\killall chromium-browser
start z:\usr\bin\killall QTester104
start z:\bin\bash -c "/usr/bin/curl http://127.0.0.1:51908/pntserver.rjs?Y=1"
start z:\usr\bin\killall hmishell.exe
start z:\bin\ping 127.0.0.1 -c 4 -w 1000
start z:\usr\bin\killall webserver.exe

start z:\usr\bin\pkill -f process_dumpdb.sh
start z:\usr\bin\pkill -f process_soe.sh
start z:\usr\bin\pkill -f process_hist.sh
start z:\bin\ping 127.0.0.1 -c 2 -w 1000

rem start z:\bin\bash -c "/usr/bin/xfce4-session-logout --logout"
start z:\bin\bash -c "/usr/bin/xfce4-session-logout --halt"
rem start z:\bin\bash -c "/usr/bin/xfce4-session-logout --reboot"
