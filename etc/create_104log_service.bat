c:\oshmi\bin\nssm install OSHMI_iec104_log "\Arquivos de Programas\Wireshark\tshark.exe" -f \"tcp portrange 2404-2405\" -w C:\oshmi\logs\iec104.log -b filesize:100000 -b files:10
c:\oshmi\bin\nssm start OSHMI_iec104_log

