taskkill /F /IM chrome.exe
ping 127.0.0.1 -n 2 -w 1000

del /Q "c:\oshmi\browser-data\default\cache\*.*"
rem rmdir /S /Q "c:\oshmi\browser-data\default\web aplications"
