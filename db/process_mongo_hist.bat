@ECHO OFF

rem look for process already running, if found get out
wmic process get commandline |find /I "process_mongo_hist.bat"   |find /C /I "cmd" |find /I "2"

if %ERRORLEVEL% EQU 0 GOTO FIM

cd \oshmi\db

FOR /L %%i IN (0,0,0) DO ( 
FORFILES -m mongo_hist_*.js   -c "CMD /c ..\MongoDB\bin\mongo.exe oshmi @FILE && del @FILE & ECHO @FILE" & PING -n 2 127.0.0.1 > nul 
)

:FIM
