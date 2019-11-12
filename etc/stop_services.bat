echo This script requires administative rights!
echo Please execute it as administrator.

rem Stop services

c:\oshmi\bin\nssm stop OSHMI_rtwebsrv
c:\oshmi\bin\nssm stop OSHMI_iec104
c:\oshmi\bin\nssm stop OSHMI_dnp3
c:\oshmi\bin\nssm stop OSHMI_modbus
c:\oshmi\bin\nssm stop OSHMI_iccp
c:\oshmi\bin\nssm stop OSHMI_s7
c:\oshmi\bin\nssm stop OSHMI_opc
