echo This script requires administative rights!
echo Please execute it as administrator.

rem Stop services

c:\oshmi\bin\nssm stop OSHMI_rtwebsrv
c:\oshmi\bin\nssm stop OSHMI_iec104
c:\oshmi\bin\nssm stop OSHMI_dnp3
c:\oshmi\bin\nssm stop OSHMI_modbus
c:\oshmi\bin\nssm stop OSHMI_iccp
c:\oshmi\bin\nssm stop OSHMI_iec104_log

rem Remove service without confimation

c:\oshmi\bin\nssm remove OSHMI_rtwebsrv confirm
c:\oshmi\bin\nssm remove OSHMI_iec104 confirm
c:\oshmi\bin\nssm remove OSHMI_dnp3 confirm
c:\oshmi\bin\nssm remove OSHMI_modbus confirm
c:\oshmi\bin\nssm remove OSHMI_iccp confirm
c:\oshmi\bin\nssm remove OSHMI_iec104_log confirm

rem Reactivate mon_proc (mon_proc runs the processes interactively with a logged user)
rem This mode is best for configuration and debugging
move c:\oshmi\bin\mon_proc.bak c:\oshmi\bin\mon_proc.exe /Y