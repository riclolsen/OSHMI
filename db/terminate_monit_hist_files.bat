@ECHO OFF

rem obtain pid from process, then kill it

set _pid=0

FOR /f "tokens=3 delims=," %%G IN (' wmic process get commandline^,processid /FORMAT:csv ^|find /I "monit_hist_files.bat" ^|find /I "cmd" ^|find /V "find" ') DO SET _pid=%%G

IF %_pid% EQU 0 GOTO FIM 

TASKKILL /F /PID %_pid%

:FIM
