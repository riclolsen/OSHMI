@ECHO off
:start
cls
echo F: Close OSHMI
echo 1: Execute OSHMI
echo 2: Try to repair tables
echo 3: Empty Hist and SOE tables
echo 4: Empty Dumpdb table
echo 5: Update HMI
echo 6: Change date/time
echo 7: Allow remote access
echo 8: Clean browser cache
echo 9: Ping
echo 0: Close this menu
set choice=
set /p choice="Choose an option > "

if not '%choice%'=='' set choice=%choice:~0,1%
if '%choice%'=='f' goto fecha 
if '%choice%'=='F' goto fecha 
if '%choice%'=='1' goto executa 
if '%choice%'=='2' goto recupera 
if '%choice%'=='3' goto apaga_sde
if '%choice%'=='4' goto apaga_pontos
if '%choice%'=='5' goto atualizar_ihm
if '%choice%'=='6' goto datahora
if '%choice%'=='7' goto remoto
if '%choice%'=='8' goto limpa_cache_chrome
if '%choice%'=='9' goto ping
if '%choice%'=='0' goto sair
ECHO "%choice%" is not valid please try again
ECHO.
goto start

:fecha 
call stop.bat
call cache_clean.bat
goto start

:executa
call stop.bat
call cache_clean.bat
start mon_proc.exe
start hmishell.exe
goto sair

:recupera
rem call stop.bat
echo ... Don't know yet how to repair sqlite files. Try to empty tables ...
pause
goto start

:apaga_sde
echo Y: CONFIRM EMPTY TABLES (SOE e HIST)!
echo N: CANCEL
set choice=
set /p choice="Choose an option > "
if not '%choice%'=='' set choice=%choice:~0,1%
if '%choice%'=='Y' goto confirmou_sde
if '%choice%'=='y' goto confirmou_sde
goto start

:confirmou_sde
call stop.bat
del ..\db\soe.*
del ..\db\hist.*
copy ..\db\db_cold\soe.sl3 ..\db\
copy ..\db\db_cold\hist.sl3 ..\db\
goto start

:apaga_pontos
echo Y: CONFIRM EMPTY TABLE (DUMPDB)!
echo N: CANCEL
set choice=
set /p choice="Choose an option > "
if not '%choice%'=='' set choice=%choice:~0,1%
if '%choice%'=='Y' goto confirmou_pontos
if '%choice%'=='y' goto confirmou_pontos
goto start

:confirmou_pontos
call stop.bat
del ..\db\dumpdb.*
copy ..\db\db_cold\dumpdb.sl3 ..\db\
goto start

:atualizar_ihm
call stop.bat

copy /Y d:\oshmi_update\*.pdf           c:\oshmi\docs\
copy /Y d:\oshmi_update\*.ini           c:\oshmi\conf\
copy /Y d:\oshmi_update\*.txt           c:\oshmi\conf\
copy /Y d:\oshmi_update\*.lua           c:\oshmi\scripts\
copy /Y d:\oshmi_update\*.exe           c:\oshmi\bin\
copy /Y d:\oshmi_update\*.html          c:\oshmi\htdocs\
copy /Y d:\oshmi_update\*.js            c:\oshmi\htdocs\
copy /Y d:\oshmi_update\*.png           c:\oshmi\htdocs\images\
copy /Y d:\oshmi_update\*.jpg           c:\oshmi\htdocs\images\
copy /Y d:\oshmi_update\*.gif           c:\oshmi\htdocs\images\
copy /Y d:\oshmi_update\*.ico           c:\oshmi\htdocs\images\
copy /Y d:\oshmi_update\*.svg           c:\oshmi\svg\
copy /Y d:\oshmi_update\screen_list.js  c:\oshmi\svg\

copy /Y e:\oshmi_update\*.pdf           c:\oshmi\docs\
copy /Y e:\oshmi_update\*.ini           c:\oshmi\conf\
copy /Y e:\oshmi_update\*.txt           c:\oshmi\conf\
copy /Y e:\oshmi_update\*.lua           c:\oshmi\scripts\
copy /Y e:\oshmi_update\*.exe           c:\oshmi\bin\
copy /Y e:\oshmi_update\*.html          c:\oshmi\htdocs\
copy /Y e:\oshmi_update\*.js            c:\oshmi\htdocs\
copy /Y e:\oshmi_update\*.png           c:\oshmi\htdocs\images\
copy /Y e:\oshmi_update\*.jpg           c:\oshmi\htdocs\images\
copy /Y e:\oshmi_update\*.gif           c:\oshmi\htdocs\images\
copy /Y e:\oshmi_update\*.ico           c:\oshmi\htdocs\images\
copy /Y e:\oshmi_update\*.svg           c:\oshmi\svg\
copy /Y e:\oshmi_update\screen_list.js  c:\oshmi\svg\

copy /Y f:\oshmi_update\*.pdf           c:\oshmi\docs\
copy /Y f:\oshmi_update\*.ini           c:\oshmi\conf\
copy /Y f:\oshmi_update\*.txt           c:\oshmi\conf\
copy /Y f:\oshmi_update\*.lua           c:\oshmi\scripts\
copy /Y f:\oshmi_update\*.exe           c:\oshmi\bin\
copy /Y f:\oshmi_update\*.html          c:\oshmi\htdocs\
copy /Y f:\oshmi_update\*.js            c:\oshmi\htdocs\
copy /Y f:\oshmi_update\*.png           c:\oshmi\htdocs\images\
copy /Y f:\oshmi_update\*.jpg           c:\oshmi\htdocs\images\
copy /Y f:\oshmi_update\*.gif           c:\oshmi\htdocs\images\
copy /Y f:\oshmi_update\*.ico           c:\oshmi\htdocs\images\
copy /Y f:\oshmi_update\*.svg           c:\oshmi\svg\
copy /Y f:\oshmi_update\screen_list.js  c:\oshmi\svg\

copy /Y g:\oshmi_update\*.pdf           c:\oshmi\docs\
copy /Y g:\oshmi_update\*.ini           c:\oshmi\conf\
copy /Y g:\oshmi_update\*.txt           c:\oshmi\conf\
copy /Y g:\oshmi_update\*.lua           c:\oshmi\scripts\
copy /Y g:\oshmi_update\*.exe           c:\oshmi\bin\
copy /Y g:\oshmi_update\*.html          c:\oshmi\htdocs\
copy /Y g:\oshmi_update\*.js            c:\oshmi\htdocs\
copy /Y g:\oshmi_update\*.png           c:\oshmi\htdocs\images\
copy /Y g:\oshmi_update\*.jpg           c:\oshmi\htdocs\images\
copy /Y g:\oshmi_update\*.gif           c:\oshmi\htdocs\images\
copy /Y g:\oshmi_update\*.ico           c:\oshmi\htdocs\images\
copy /Y g:\oshmi_update\*.svg           c:\oshmi\svg\
copy /Y g:\oshmi_update\screen_list.js  c:\oshmi\svg\

call cache_clean.bat
shutdown -f -s
goto start

:limpa_cache_chrome
call cache_clean.bat
goto start

:ping
set choice=
set /p choice="Type an IP address to ping > "
ping %choice%
pause
goto start

:datahora
date
time
goto start

:remoto
net start uvnc_service
net start tvnserver
net start termservice
goto start

:sair
exit
