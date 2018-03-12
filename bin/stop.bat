
rem Kill most OSHMI processes but nginx and php

taskkill /F /IM chrome.exe
taskkill /F /IM mon_proc.exe
taskkill /F /IM iec104m.exe
taskkill /F /IM qtester104.exe
taskkill /F /IM hmishell.exe
taskkill /F /IM sim_tr.exe
taskkill /F /IM procexp.exe
start /wait close_webserver.vbs

..\db\terminate_soe.bat
..\db\terminate_dumpdb.bat
..\db\terminate_hist.bat

ping 127.0.0.1 -n 2 -w 1000

taskkill /F /IM sqlite3.exe
taskkill /F /IM webserver.exe
