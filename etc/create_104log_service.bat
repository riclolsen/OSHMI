c:\oshmi\bin\nssm install OSHMI_iec104_log "%PROGRAMFILES%\Wireshark\tshark.exe" -f \"tcp portrange 2404-2405\" -w C:\oshmi\logs\iec104.log -b filesize:100000 -b files:10
rem use %PROGRAMFILES(x86)% for 32 bit software in 64 bit Windows
c:\oshmi\bin\nssm start OSHMI_iec104_log

