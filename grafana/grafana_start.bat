REM Comment all if Grafana installed as a service or in an external server
cd c:\oshmi\grafana\bin
call grafana_stop.bat
start \oshmi\bin\hidec grafana-server

