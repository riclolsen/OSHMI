rem Kill all OSHMI processes.
rem If there are services, must be run as administrator!

cd c:\oshmi\bin
call stop.bat
call c:\oshmi\etc\stop_services.bat
call c:\oshmi\nginx_php\stop_nginx_php.bat
wmic PROCESS WHERE "COMMANDLINE LIKE '%%c:\\oshmi\\bin\\%%'" CALL TERMINATE
wmic PROCESS WHERE "COMMANDLINE LIKE '%%c:\\oshmi\\browser\\%%'" CALL TERMINATE
wmic PROCESS WHERE "COMMANDLINE LIKE '%%c:\\oshmi\\nginx_php\\%%'" CALL TERMINATE
wmic PROCESS WHERE "COMMANDLINE LIKE '%%c:\\oshmi\\db\\%%'" CALL TERMINATE
wmic PROCESS WHERE "COMMANDLINE LIKE '%%c:\\oshmi\\inkscape\\%%'" CALL TERMINATE


