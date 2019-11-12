echo This script requires administative rights!
echo Please execute it as administrator.

rem Create services, they will work not interactively and independently of a logged user

c:\oshmi\bin\nssm install OSHMI_rtwebsrv c:\oshmi\bin\webserver.exe
c:\oshmi\bin\nssm install OSHMI_iec104 c:\oshmi\bin\QTester104.exe
rem c:\oshmi\bin\nssm install OSHMI_dnp3 c:\oshmi\bin\dnp3.exe
rem c:\oshmi\bin\nssm install OSHMI_dnp3 c:\oshmi\bin\modbus.exe
rem c:\oshmi\bin\nssm install OSHMI_iccp c:\oshmi\bin\iccp_client.exe
rem c:\oshmi\bin\nssm install OSHMI_opc c:\oshmi\bin\opc_client.exe
rem c:\oshmi\bin\nssm install OSHMI_s7 c:\oshmi\bin\s7client.exe

rem Start services

c:\oshmi\bin\nssm start OSHMI_rtwebsrv
c:\oshmi\bin\nssm start OSHMI_iec104
rem c:\oshmi\bin\nssm start OSHMI_dnp3
rem c:\oshmi\bin\nssm start OSHMI_modbus
rem c:\oshmi\bin\nssm start OSHMI_iccp
rem c:\oshmi\bin\nssm start OSHMI_opc
rem c:\oshmi\bin\nssm start OSHMI_s7

rem Deactivate mon_proc (mon_proc runs the processes interactively with a logged user)
move c:\oshmi\bin\mon_proc.exe c:\oshmi\bin\mon_proc.bak /Y
