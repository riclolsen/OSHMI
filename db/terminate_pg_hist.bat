@ECHO OFF

rem obtain pid from process, then kill it

set _pid=0

FOR /f "tokens=3 delims=," %%G IN (' wmic process get commandline^,processid /FORMAT:csv ^|find /I "process_pg_hist.bat" ^|find /I "cmd" ^|find /V "find" ^|find /V "%~n0"') DO (
IF %_pid% EQU 0 ( 
  SET _pid=%%G 
  GOTO BRK 
  )
)

:BRK

IF %_pid% EQU 0 GOTO FIM 

TASKKILL /F /PID %_pid%

:FIM
