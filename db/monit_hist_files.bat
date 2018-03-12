rem Monitor the number of hist*.sql files and choose the pragmas file
rem for speed when there are many or safety when there are a few

CD \oshmi\db


SET pragmas_file=pragmas_hist
SET safepgm=1
SET safepgmant=1
SET maxfiles=3

COPY /Y %pragmas_file%_safe.sql %pragmas_file%.sql

:infloop

FOR /f %%A IN ('DIR /A-D /B hist*.sql ^| FIND /c /v "~~~"') DO SET numfl=%%A
IF %numfl% GEQ %maxfiles% (SET safepgm=0) ELSE (SET safepgm=1)

ECHO %safepgm%

IF %safepgm% NEQ %safepgmant% (IF %safepgm% NEQ 0 (COPY /Y %pragmas_file%_safe.sql %pragmas_file%.sql) ELSE (COPY /Y %pragmas_file%_fast.sql %pragmas_file%.sql))

set safepgmant=%safepgm%

PING -n 15 127.0.0.1 > nul

GOTO infloop
