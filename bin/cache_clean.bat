wmic PROCESS WHERE "COMMANDLINE LIKE '%%oshmi\\browser\\chrome.exe%%'" CALL TERMINATE
ping 127.0.0.1 -n 2 -w 1000

del /Q "c:\oshmi\browser-data\Default\cache\*.*"
del /Q "c:\oshmi\browser-data\Default\Media Cache\*.*"
