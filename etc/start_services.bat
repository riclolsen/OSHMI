echo This script requires administative rights!
echo Please execute it as administrator.


rem Start services

c:\oshmi\bin\nssm start OSHMI_rtwebsrv 
c:\oshmi\bin\nssm start OSHMI_iec104 
c:\oshmi\bin\nssm start OSHMI_iccp 
rem c:\oshmi\bin\nssm start OSHMI_dnp3
rem c:\oshmi\bin\nssm start OSHMI_modbus
rem c:\oshmi\bin\nssm start OSHMI_s7
rem c:\oshmi\bin\nssm start OSHMI_opc

