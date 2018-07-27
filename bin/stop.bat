
rem Kill most OSHMI processes but nginx and php

wmic PROCESS WHERE "COMMANDLINE LIKE '%%oshmi\\browser\\chrome.exe%%'" CALL TERMINATE
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
..\db\terminate_monit_hist_files.bat

ping 127.0.0.1 -n 2 -w 1000

taskkill /F /IM sqlite3.exe
taskkill /F /IM webserver.exe
wmic PROCESS WHERE "COMMANDLINE LIKE '%%oshmi\\bin\\sqlite3.exe%%'" CALL TERMINATE

