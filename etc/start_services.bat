echo This script requires administative rights!
echo Please execute it as administrator.


rem Start services

c:\oshmi\bin\nssm start OSHMI_rtwebsrv 
c:\oshmi\bin\nssm start OSHMI_iec104 
c:\oshmi\bin\nssm start OSHMI_iccp 
