; oshmi.nsi
; OSHMI installer script
; Copyright 2008-2020 - Ricardo L. Olsen

; NSIS (Nullsoft Scriptable Install System) - http://nsis.sourceforge.net/Main_Page

RequestExecutionLevel user

!include "TextFunc.nsh"
!include "WordFunc.nsh"

;--------------------------------

!define VERSION "v.6.17"
!define VERSION_ "6.17.0.0"

Function .onInit
 System::Call 'keexrnel32::CreateMutexA(i 0, i 0, t "MutexOshmiInstall") i .r1 ?e'
 Pop $R0
 StrCmp $R0 0 +3
   MessageBox MB_OK|MB_ICONEXCLAMATION "Installer already executing!"
   Abort
FunctionEnd
 
;--------------------------------

!ifdef HAVE_UPX
!packhdr tmp.dat "upx\upx -9 tmp.dat"
!endif

!ifdef NOCOMPRESS
SetCompress off
!endif

;--------------------------------

!define /date DATEBAR "%d/%m/%Y"
Name "OSHMI"
Caption "OSHMI (Open Substation HMI) Installer ${VERSION} ${DATEBAR}"
Icon "..\icons\oshmi-o-logo255.ico"

!define /date DATE "%d_%m_%Y"
OutFile "oshmi_setup_${VERSION}.exe"

VIProductVersion ${VERSION_}
VIAddVersionKey ProductName "OSHMI (Open Substation HMI)"
VIAddVersionKey Comments "SCADA HMI Software"
VIAddVersionKey CompanyName "Ricardo Olsen"
VIAddVersionKey LegalCopyright "Copyright 2008-2020 Ricardo L. Olsen"
VIAddVersionKey FileDescription "OSHMI Installer"
VIAddVersionKey FileVersion ${VERSION}
VIAddVersionKey ProductVersion ${VERSION}
VIAddVersionKey InternalName "OSHMI Installer"
VIAddVersionKey LegalTrademarks "OSHMI"
VIAddVersionKey OriginalFilename "oshmi_setup_${VERSION}.exe"

SetDateSave on
SetDatablockOptimize on
CRCCheck on
SilentInstall normal
BGGradient 000000 800000 FFFFFF
InstallColors FF8080 000030
XPStyle on

InstallDir "c:\oshmi"
InstallDirRegKey HKLM "Software\OSHMI" "Install_Dir"

CheckBitmap "${NSISDIR}\Contrib\Graphics\Checks\classic-cross.bmp"

LicenseText "OSHMI Release Notes"
LicenseData "release_notes.txt"

; Must be admin
RequestExecutionLevel admin

;--------------------------------

Page license
;Page components
;Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

; LoadLanguageFile "${NSISDIR}\Contrib\Language files\PortugueseBR.nlf"

;--------------------------------

AutoCloseWindow false
ShowInstDetails show

;--------------------------------

Section "" ; empty string makes it hidden, so would starting with -

; Closes all OSHMI processes
  nsExec::Exec 'net stop OSHMI_rtwebsrv'
  nsExec::Exec 'net stop OSHMI_iec104'
  nsExec::Exec 'net stop OSHMI_iccp'
  nsExec::Exec 'net stop OSHMI_dnp3'
  nsExec::Exec 'net stop OSHMI_modbus'
  nsExec::Exec 'net stop OSHMI_opc'
  nsExec::Exec 'net stop OSHMI_s7'
  nsExec::Exec 'c:\oshmi\bin\stop_all.bat'
  nsExec::Exec 'taskkill /F /IM mon_proc.exe'
  nsExec::Exec 'taskkill /F /IM procexp.exe'
  nsExec::Exec 'taskkill /F /IM iccp_client.exe'
  nsExec::Exec 'taskkill /F /IM s7client.exe'
  nsExec::Exec 'taskkill /F /IM opc_client.exe'
  nsExec::Exec 'taskkill /F /IM QTester104.exe'
  nsExec::Exec 'taskkill /F /IM dnp3.exe'
  nsExec::Exec 'taskkill /F /IM modbus.exe'
  nsExec::Exec 'taskkill /F /IM hmishell.exe'
  nsExec::Exec 'taskkill /F /IM webserver.exe'
  nsExec::Exec 'taskkill /F /IM sqlite3.exe'
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\bin\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\browser\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\nginx_php\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\inkscape\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\PostgreSQL\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\grafana\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\db\\%'" CALL TERMINATE`

  SetOverwrite on

  var /GLOBAL NAVWINCMD
  var /GLOBAL NAVLINCMD
  var /GLOBAL NAVDATDIR
  var /GLOBAL NAVPREOPT
  var /GLOBAL NAVPOSOPT
  var /GLOBAL NAVVISABO
  var /GLOBAL NAVVISEVE
  var /GLOBAL NAVVISHEV
  var /GLOBAL NAVVISTAB
  var /GLOBAL NAVVISANO
  var /GLOBAL NAVVISTEL
  var /GLOBAL NAVVISTRE
  var /GLOBAL NAVVISCUR
  var /GLOBAL NAVVISOVW
  var /GLOBAL NAVVISDOC
  var /GLOBAL NAVVISLOG
  var /GLOBAL NAVGRAFAN
  var /GLOBAL HTTPSRV
    
  StrCpy $HTTPSRV   "http://127.0.0.1:51909"
 #StrCpy $HTTPSRV   "https://127.0.0.1"
  StrCpy $NAVWINCMD "browser\chrome.exe"
  StrCpy $NAVLINCMD "/usr/bin/chromium-browser"
  StrCpy $NAVDATDIR "--user-data-dir=$INSTDIR\browser-data"
  StrCpy $NAVPREOPT "--process-per-site --no-sandbox"
  StrCpy $NAVPOSOPT "--disable-popup-blocking --no-proxy-server --bwsi --disable-extensions --disable-sync --no-first-run"
  StrCpy $NAVVISABO "/htdocs/about.html"
  StrCpy $NAVVISEVE "/htdocs/events.html"
  StrCpy $NAVVISHEV "/htdocs/events.html?MODO=4"
  StrCpy $NAVVISTAB "/htdocs/tabular.html"
  StrCpy $NAVVISANO "/htdocs/tabular.html?SELMODULO=TODOS_ANORMAIS"
  StrCpy $NAVVISTEL "/htdocs/screen.html"
  StrCpy $NAVVISTRE "/htdocs/trend.html"
  StrCpy $NAVVISCUR "/htdocs/histwebview/histwebview.php"
  StrCpy $NAVVISOVW "/htdocs/overview.html"
  StrCpy $NAVVISDOC "/htdocs/listdocs.php"
  StrCpy $NAVVISLOG "/htdocs/listlogs.php"
  StrCpy $NAVGRAFAN "/grafana"

  ; write reg info
  WriteRegStr HKLM SOFTWARE\OSHMI "Install_Dir" "$INSTDIR"

  ; write uninstall strings
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OSHMI" "DisplayName" "OSHMI (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OSHMI" "UninstallString" '"$INSTDIR\bt-uninst.exe"'

  ; erases all of the old chromium
  RMDir /r "$INSTDIR\browser" 
  RMDir /r "$INSTDIR\browser-data" 

  ; erases all of the old Inkscape but the share dir
  ; New files will replace old.
  RMDir /r "$INSTDIR\inkscape\doc" 
  RMDir /r "$INSTDIR\inkscape\etc" 
  RMDir /r "$INSTDIR\inkscape\lib"  
  Delete "$INSTDIR\inkscape\*.*"  
  
  CreateDirectory "$INSTDIR\bin"
  CreateDirectory "$INSTDIR\bin\platforms"
  CreateDirectory "$INSTDIR\browser"
  CreateDirectory "$INSTDIR\browser-data"
  CreateDirectory "$INSTDIR\charts"
  CreateDirectory "$INSTDIR\conf"
  CreateDirectory "$INSTDIR\conf_templates"
  CreateDirectory "$INSTDIR\db"
  CreateDirectory "$INSTDIR\db\db_cold"
  CreateDirectory "$INSTDIR\docs"
  CreateDirectory "$INSTDIR\etc"
  CreateDirectory "$INSTDIR\extprogs"
  CreateDirectory "$INSTDIR\htdocs"
  CreateDirectory "$INSTDIR\htdocs\images"
  CreateDirectory "$INSTDIR\i18n"
  CreateDirectory "$INSTDIR\inkscape"
  CreateDirectory "$INSTDIR\fonts"
  CreateDirectory "$INSTDIR\linux"
  CreateDirectory "$INSTDIR\linux\iptables"
  CreateDirectory "$INSTDIR\linux\nginx"
  CreateDirectory "$INSTDIR\linux\nginx\conf"
  CreateDirectory "$INSTDIR\linux\nginx\conf\sites-available"
  CreateDirectory "$INSTDIR\linux\php"
  CreateDirectory "$INSTDIR\linux\php\7.2"
  CreateDirectory "$INSTDIR\linux\php\7.2\fpm"  
  
  CreateDirectory "$INSTDIR\logs"
  CreateDirectory "$INSTDIR\nginx_php"
  CreateDirectory "$INSTDIR\scripts"
  CreateDirectory "$INSTDIR\svg"
  CreateDirectory "$INSTDIR\grafana"
  CreateDirectory "$INSTDIR\PostgreSQL"
  CreateDirectory "$INSTDIR\PowerBI"
  CreateDirectory "$INSTDIR\Opc.Ua.CertificateGenerator"

  SetOutPath $INSTDIR

  File /a "..\installer\release_notes.txt"
  File /a "..\installer\gpl.txt"
  File /a "..\icons\favicon.ico"
  File /a "..\index.html"

  SetOutPath $INSTDIR\bin
  File /a "..\bin\*.exe"
  File /a "..\bin\*.dll"
  File /a "..\bin\*.vbs"
  File /a "..\bin\*.bat"
  ;File /a "..\bin\*.json"

  SetOutPath $INSTDIR\bin\platforms
  File /a "..\bin\platforms\*.dll"

  SetOutPath $INSTDIR\etc
  File /a "..\etc\dyn_sheet.xlsx"
  File /a "..\etc\webserver_query.iqy"  
  File /a "..\etc\simtr_example.txt"
  File /a "..\etc\*.reg"

  SetOutPath $INSTDIR\extprogs
  File /a "..\extprogs\download_external_progs.bat"
  
  SetOutPath $INSTDIR\nginx_php
  File /r /x *.log "..\nginx_php\*.*" 
  SetOutPath $INSTDIR\nginx_php\php
  File /a "..\conf_templates\php.ini"
  SetOutPath $INSTDIR\nginx_php\conf
  File /a "..\conf_templates\nginx.conf"

  SetOutPath $INSTDIR\linux
  File /a "..\linux\*.*"
  File /a "..\linux\QTester104"
  SetOutPath $INSTDIR\linux\iptables
  File /a "..\linux\iptables\rules.v4"
  SetOutPath $INSTDIR\linux\nginx\conf
  File /a "..\linux\nginx\conf\nginx.conf"
  SetOutPath $INSTDIR\linux\nginx\conf\sites-available
  File /a "..\linux\nginx\conf\sites-available\default"
  SetOutPath $INSTDIR\linux\php\7.2\fpm
  File /a "..\linux\php\7.2\fpm\php.ini"

  SetOutPath $INSTDIR\db\db_cold
  File /a "..\db\db_cold\*.*"
  
  SetOutPath $INSTDIR\db
  File /a "..\db\db_maintenance.bat"
  File /a "..\db\monit_hist_files.bat"
  File /a "..\db\process_hist.bat"
  File /a "..\db\process_soe.bat"
  File /a "..\db\process_dumpdb.bat"
  File /a "..\db\terminate_hist.bat"
  File /a "..\db\terminate_soe.bat"
  File /a "..\db\terminate_dumpdb.bat"
  File /a "..\db\pragmas_hist.sql"
  File /a "..\db\pragmas_hist_fast.sql"
  File /a "..\db\pragmas_hist_safe.sql"
  File /a "..\db\pragmas_soe.sql"
  File /a "..\db\pragmas_dumpdb.sql"
  File /a "..\db\process_pg_hist.bat"
  File /a "..\db\process_pg_soe.bat"
  File /a "..\db\process_pg_dumpdb.bat"
  File /a "..\db\terminate_pg_hist.bat"
  File /a "..\db\terminate_pg_soe.bat"
  File /a "..\db\terminate_pg_dumpdb.bat"
  File /a "..\db\terminate_monit_hist_files.bat"
  File /a "..\db\process_mongo_hist.bat"
  File /a "..\db\process_mongo_dumpdb.bat"
  File /a "..\db\terminate_mongo_hist.bat"
  File /a "..\db\terminate_mongo_dumpdb.bat"

  SetOutPath $INSTDIR\i18n
  File /a "..\i18n\*.*"

  SetOutPath $INSTDIR\conf_templates

  File /a "..\conf_templates\*.*"

  SetOutPath $INSTDIR\htdocs
  File /a "..\htdocs\about.html"
  File /a "..\htdocs\dlgcomando.html"
  File /a "..\htdocs\dlginfo.html"
  File /a "..\htdocs\events.html"
  File /a "..\htdocs\trend.html"
  File /a "..\htdocs\tabular.html"
  File /a "..\htdocs\screen.html"
  File /a "..\htdocs\almbox.html"
  File /a "..\htdocs\overview.html"
  File /a "..\htdocs\images.js"
  File /a "..\htdocs\util.js"
  File /a "..\htdocs\fan.js"
  File /a "..\htdocs\websage.js"
  File /a "..\htdocs\pntserver.js"
  File /a "..\htdocs\timepntserver.php"
  File /a "..\htdocs\timezone.php"
  File /a "..\htdocs\legacy_options.js"
  File /a "..\htdocs\eventserver.php"
  File /a "..\htdocs\eventsync.php"
  File /a "..\htdocs\annotation.php"
  File /a "..\htdocs\annotation_readonly.php"
  File /a "..\htdocs\config_viewers_default.js"
  File /a "..\htdocs\rss.php"
  File /a "..\htdocs\proxy.php"
  File /a "..\htdocs\ons.php"
  File /a "..\htdocs\vega_websage.js"  
  File /a "..\htdocs\listlogs.php"
  File /a "..\htdocs\listdocs.php"
  File /a "..\htdocs\json.php"
  File /a "..\htdocs\odata.php"
  File /a "..\htdocs\csv.php"
  File /a "..\htdocs\csv-soe.php"
; File /a "..\htdocs\simulator.html"
; File /a "..\htdocs\wdc-simulator.js"
; File /a "..\htdocs\vis.css"
  File /a "..\htdocs\tagfind.php"
  File /a "..\htdocs\tableau_wdc.php"
  File /a "..\htdocs\tableau_wdc.js"
  File /a "..\htdocs\dialogstyle.css"
  File /a "..\htdocs\symbol_webreflection.js"
  File /a "..\htdocs\sqlite_pntserver.php"
  File /a "..\htdocs\sqlite_tabserver.php"
  File /a "..\htdocs\pgsql_json.php"
  File /a "..\htdocs\pgsql_odata.php"
  File /a "..\htdocs\pgsql_eventserver.php"
  File /a "..\htdocs\pgsql_pntserver.php"
  File /a "..\htdocs\pgsql_tabserver.php"
  File /a "..\htdocs\pgsql_timepntserver.php"
  File /a "..\htdocs\pntserver_gateway.js"
; File /a "..\htdocs\"
    
  SetOutPath $INSTDIR\htdocs\histwebview
  File /a /r "..\htdocs\histwebview\*.*"

  SetOutPath $INSTDIR\htdocs\lib
  File /a /r "..\htdocs\lib\*.*"

  SetOutPath $INSTDIR\htdocs\report
  File /a /r "..\htdocs\report\*.*"

  SetOutPath "$INSTDIR\htdocs\images"
  File /a "..\htdocs\images\*.*"

  SetOutPath $INSTDIR\extprogs
  File /a "..\extprogs\vcredist_x86.exe"
  File /a "..\extprogs\vcredist_x86-2012.exe"
  File /a "..\extprogs\vcredist_x86-2013.exe"
  ;File /a "..\extprogs\vcredist_x86-2015.exe"
  ;File /a "..\extprogs\vcredist_x86-2017.exe"
  File /a "..\extprogs\vcredist_x86-15-17-19.exe"

  SetOutPath $INSTDIR\browser
  File /a /r "..\browser\*.*"

  SetOutPath $INSTDIR\browser-data
  File /a /r "..\browser-data\*.*"

  ; Inkscape custom built
  SetOutPath $INSTDIR\inkscape
  File /a /r "..\inkscape\*.*"

  ; Inkscape additional symbols
  SetOutPath $INSTDIR\inkscape\share\symbols
  File /a /r "..\inkscape_symbols\*.*"

  SetOutPath $INSTDIR\docs
  File /a "..\docs\oshmi_operation_manual-pt_br.odt"
  File /a "..\docs\oshmi_operation_manual-pt_br.pdf"
  File /a "..\docs\oshmi_operation_manual-en_us.odt"
  File /a "..\docs\oshmi_operation_manual-en_us.pdf"
  File /a "..\docs\oshmi_configuration_manual-en_us.odt"
  File /a "..\docs\oshmi_configuration_manual-en_us.pdf"
  File /a "..\docs\oshmi_dnp3_config-en_us.odt"
  File /a "..\docs\oshmi_dnp3_config-en_us.pdf"
  File /a "..\docs\oshmi_modbus_config-en_us.odt"
  File /a "..\docs\oshmi_modbus_config-en_us.pdf"
  File /a "..\docs\oshmi_opc_client_config-en_us.odt"
  File /a "..\docs\oshmi_opc_client_config-en_us.pdf"
  File /a "..\docs\oshmi_s7_client_config-en_us.odt"
  File /a "..\docs\oshmi_s7_client_config-en_us.pdf"
  File /a "..\docs\lua_reference_manual.pdf"
  File /a "..\docs\inkscape-shortcuts1.svg"
  File /a "..\docs\inkscape-shortcuts2.svg"
  File /a "..\docs\inkscape-shortcuts1.pdf"
  File /a "..\docs\inkscape-shortcuts2.pdf"

  SetOutPath $INSTDIR\fonts
  File /a /r "..\fonts\*.*"  

  SetOutPath $INSTDIR\grafana
  File /a /r "..\grafana\grafana_start.bat"  

  SetOutPath $INSTDIR\PostgreSQL
  File /a /r "..\PostgreSQL\install.txt"  
  File /a /r "..\PostgreSQL\postgresql_start.bat"  

  SetOutPath $INSTDIR\PowerBI
  File /a /r "..\PowerBI\powerbi-oshmi-demo.pbix"  
  
  SetOutPath $INSTDIR\Opc.Ua.CertificateGenerator
  File /a /r "..\Opc.Ua.CertificateGenerator\*.*"  

  ; backup old (possibly incompatible) files to allow for the new ones to be overwritten
  Rename "$INSTDIR\conf\nginx_http.conf" "$INSTDIR\conf\nginx_http.pre_${VERSION}.conf.bak"
  Rename "$INSTDIR\conf\nginx_https.conf" "$INSTDIR\conf\nginx_https.pre_${VERSION}.conf.bak"
  Rename "$INSTDIR\conf\oshmi_config_manager.xlsm" "$INSTDIR\conf\oshmi_config_manager.pre_${VERSION}.xlsm.bak"
  SetOutPath $INSTDIR\conf
  File /a "..\conf_templates\oshmi_config_manager.xlsm"
  File /a "..\conf_templates\nginx_http.conf"  
  File /a "..\conf_templates\nginx_https.conf"  
  
  SetOverwrite off

  SetOutPath $INSTDIR\charts
  File /a "..\charts\*.json"  

  SetOutPath $INSTDIR\etc
  File /a "..\etc\*.bat"

  SetOutPath $INSTDIR\conf
  File /a "..\conf_templates\config_viewers.js"
  File /a "..\conf_templates\point_list.txt"
  File /a "..\conf_templates\point_calc.txt"
  File /a "..\conf_templates\qtester104.ini"  
  File /a "..\conf_templates\dnp3.ini"  
  File /a "..\conf_templates\modbus_queue.ini"
  File /a "..\conf_templates\opc_client.conf"
  File /a "..\conf_templates\s7client.ini"
  File /a "..\conf_templates\hmi.ini"
  File /a "..\conf_templates\hmishell.ini"
  File /a "..\conf_templates\mon_proc.ini"
  File /a "..\conf_templates\nginx_access_control.conf"  

  SetOutPath "$INSTDIR\db"
  File /a "..\db\db_cold\*.sl3"
  File /a "..\db\.pgpass"
  File /a "..\db\pgpass.conf"
  
  IfFileExists "$SYSDIR\forfiles.exe" JaExisteForFiles
  File /a "..\db\forfiles.exe"

  JaExisteForFiles:

  SetOutPath "$INSTDIR\svg"
  File /a "..\svg\kaw2.svg"
  File /a "..\svg\kik3.svg"
  File /a "..\svg\knh2.svg"
  File /a "..\svg\kor1.svg"
  File /a "..\svg\brasil.svg"
  File /a "..\svg\office.svg"
  File /a "..\conf_templates\screen_list.js"

; Visual C redist: necessario para executar o PHP
  nsExec::Exec '"$INSTDIR\extprogs\vcredist_x86.exe" /q'
  nsExec::Exec '"$INSTDIR\extprogs\vcredist_x86-2012.exe" /q'
  nsExec::Exec '"$INSTDIR\extprogs\vcredist_x86-2013.exe" /q'
  ;nsExec::Exec '"$INSTDIR\extprogs\vcredist_x86-2015.exe" /q'
  ;nsExec::Exec '"$INSTDIR\extprogs\vcredist_x86-2017.exe" /q'
  nsExec::Exec '"$INSTDIR\extprogs\vcredist_x86-15-17-19.exe" /q'

;  MessageBox MB_YESNO "Wish to substitute Windows Shell by the HMIShell? \nWARNING: ANSWERING YES WILL BLOCK THE MACHINE FOR THE OPERATOR" IDNO InstFim 
; LabelShell:
; registry key to change Windows shell
;  WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\Winlogon" "Shell" "c:\\oshmi\\bin\\hmishell.exe"
; registry key to disable task manager
;  WriteRegDword HKCU "Software\Microsoft\Windows\CurrentVersion\Policies\System" "DisableTaskMgr" 0x01
; InstFim:

; Escreve chaves para definir atalhos do chrome no hmi.ini

; Evita reescrever atalhos ja configurados manualmente para https
  ReadINIStr $0 "$INSTDIR\conf\hmi.ini"  "RUN" "SCREEN_VIEWER"
  StrCpy $0 $0 5
  StrCmp $0 "https" viewer_shortcuts_end

; chaves para o windows   
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" "EVENTS_VIEWER"     '"$INSTDIR\$NAVWINCMD $NAVDATDIR --bopt --app=$HTTPSRV$NAVVISEVE"'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" "TABULAR_VIEWER"    '"$INSTDIR\$NAVWINCMD $NAVDATDIR --bopt --app=$HTTPSRV$NAVVISTAB"'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" "SCREEN_VIEWER"     '"$INSTDIR\$NAVWINCMD $NAVDATDIR --bopt --app=$HTTPSRV$NAVVISTEL"'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" "TREND_VIEWER"      '"$INSTDIR\$NAVWINCMD $NAVDATDIR --bopt --app=$HTTPSRV$NAVVISTRE"'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" "CURVES_VIEWER"     '"$INSTDIR\$NAVWINCMD $NAVDATDIR --bopt --app=$HTTPSRV$NAVVISCUR"'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" "DOCS_VIEWER"       '"$INSTDIR\$NAVWINCMD $NAVDATDIR --bopt --app=$HTTPSRV$NAVVISDOC"'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" "LOGS_VIEWER"       '"$INSTDIR\$NAVWINCMD $NAVDATDIR --bopt --app=$HTTPSRV$NAVVISLOG"'
 
; chaves para o linux   
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" ";EVENTS_VIEWER"    '$NAVLINCMD --bopt --app=$HTTPSRV$NAVVISEVE'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" ";TABULAR_VIEWER"   '$NAVLINCMD --bopt --app=$HTTPSRV$NAVVISTAB'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" ";SCREEN_VIEWER"    '$NAVLINCMD --bopt --app=$HTTPSRV$NAVVISTEL'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" ";TREND_VIEWER"     '$NAVLINCMD --bopt --app=$HTTPSRV$NAVVISTRE'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" ";CURVES_VIEWER"    '$NAVLINCMD --bopt --app=$HTTPSRV$NAVVISCUR'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" ";DOCS_VIEWER"      '$NAVLINCMD --bopt --app=$HTTPSRV$NAVVISDOC'
;  WriteINIStr "$INSTDIR\conf\hmi.ini"  "RUN" ";LOGSS_VIEWER"     '$NAVLINCMD --bopt --app=$HTTPSRV$NAVVISLOG'

 viewer_shortcuts_end:    

; vou colocar aqui todos os atalhos no Desktop, apagando os antigos
  Delete "$DESKTOP\OSHMI\*.*"
  CreateDirectory "$DESKTOP\OSHMI"

; Cria atalhos para os aplicativos
  CreateShortCut "$DESKTOP\OSHMI\_Start_OSHMI.lnk"                "$INSTDIR\bin\start_hmi.bat"  
  CreateShortCut "$DESKTOP\OSHMI\_Config_Manager.lnk"             "$INSTDIR\conf\oshmi_config_manager.xlsm"
  CreateShortCut "$DESKTOP\OSHMI\Stop_All.lnk"                    "$INSTDIR\bin\stop_all.bat"  
  CreateShortCut "$DESKTOP\OSHMI\HMIShell.lnk"                    "$INSTDIR\bin\hmishell.exe"  
  CreateShortCut "$DESKTOP\OSHMI\QTester104.lnk"                  "$INSTDIR\bin\QTester104.exe"  
  CreateShortCut "$DESKTOP\OSHMI\WebServer.lnk"                   "$INSTDIR\bin\webserver.exe"  
  CreateShortCut "$DESKTOP\OSHMI\Recovery Menu.lnk"               "$INSTDIR\bin\menu.bat"  
  CreateShortCut "$DESKTOP\OSHMI\Clean Browser Cache.lnk"         "$INSTDIR\bin\cache_clean.bat"  
  CreateShortCut "$DESKTOP\OSHMI\Process Explorer.lnk"            "$INSTDIR\extprogs\procexp.exe"  

  CreateShortCut "$DESKTOP\OSHMI\Chromium Browser.lnk"            "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT $NAVPOSOPT"
  CreateShortCut "$DESKTOP\OSHMI\About.lnk"                       "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT --app=$HTTPSRV$NAVVISABO $NAVPOSOPT" "$INSTDIR\htdocs\images\oshmi.ico" 
  CreateShortCut "$DESKTOP\OSHMI\Screen Viewer.lnk"               "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT --app=$HTTPSRV$NAVVISTEL $NAVPOSOPT" "$INSTDIR\htdocs\images\tela.ico" 
  CreateShortCut "$DESKTOP\OSHMI\Events Viewer.lnk"               "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT --app=$HTTPSRV$NAVVISEVE $NAVPOSOPT" "$INSTDIR\htdocs\images\chrono.ico" 
  CreateShortCut "$DESKTOP\OSHMI\Historical Events.lnk"           "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT --app=$HTTPSRV$NAVVISHEV $NAVPOSOPT" "$INSTDIR\htdocs\images\calendar.ico" 
  CreateShortCut "$DESKTOP\OSHMI\Tabular Viewer.lnk"              "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT --app=$HTTPSRV$NAVVISTAB $NAVPOSOPT" "$INSTDIR\htdocs\images\tabular.ico" 
  CreateShortCut "$DESKTOP\OSHMI\Alarms Viewer.lnk"               "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT --app=$HTTPSRV$NAVVISANO $NAVPOSOPT" "$INSTDIR\htdocs\images\firstaid.ico" 
  CreateShortCut "$DESKTOP\OSHMI\Curves Viewer.lnk"               "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT --app=$HTTPSRV$NAVVISCUR $NAVPOSOPT" "$INSTDIR\htdocs\images\plot.ico" 
  CreateShortCut "$DESKTOP\OSHMI\Grafana.lnk"                     "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT --app=$HTTPSRV$NAVGRAFAN $NAVPOSOPT" "$INSTDIR\htdocs\images\grafana.ico" 
; CreateShortCut "$DESKTOP\OSHMI\Overview.lnk"                    "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT --app=$HTTPSRV$NAVVISOVW $NAVPOSOPT" "$INSTDIR\htdocs\images\oshmi.ico" 

  CreateShortCut "$DESKTOP\OSHMI\Operation Manual.lnk"            "$INSTDIR\bin\operation_manual.bat"
  CreateShortCut "$DESKTOP\OSHMI\Configuration Manual.lnk"        "$INSTDIR\bin\configuration_manual.bat"
  CreateShortCut "$DESKTOP\OSHMI\Docs Viewer.lnk"                 "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT --app=$HTTPSRV$NAVVISDOC $NAVPOSOPT" "$INSTDIR\htdocs\images\help2.ico"
  CreateShortCut "$DESKTOP\OSHMI\Logs Viewer.lnk"                 "$INSTDIR\$NAVWINCMD" " $NAVDATDIR $NAVPREOPT --app=$HTTPSRV$NAVVISLOG $NAVPOSOPT" "$INSTDIR\htdocs\images\info.ico"

  CreateShortCut "$DESKTOP\OSHMI\Inkscape SAGE.lnk"               "$INSTDIR\inkscape\inkscape.exe"
; CreateShortCut "$DESKTOP\OSHMI\Config Files - PSPad.lnk"        "$PROGRAMFILES\PSPad Editor\pspad.exe" "$INSTDIR\conf\hmi.ini $INSTDIR\conf\hmishell.ini $INSTDIR\svg\screen_list.js $INSTDIR\conf\config_viewers.js $INSTDIR\conf\mon_proc.ini $INSTDIR\conf\qtester104.ini $INSTDIR\conf\point_list.txt $INSTDIR\conf\point_calc.txt $INSTDIR\conf\nginx_access_control.conf $INSTDIR\conf\nginx_https.conf $INSTDIR\conf\nginx_http.conf"
  CreateShortCut "$DESKTOP\OSHMI\Config Files - Notepad++.lnk"    "$PROGRAMFILES\Notepad++\notepad++.exe" "$INSTDIR\conf\hmi.ini $INSTDIR\conf\hmishell.ini $INSTDIR\svg\screen_list.js $INSTDIR\conf\config_viewers.js $INSTDIR\conf\mon_proc.ini $INSTDIR\conf\qtester104.ini $INSTDIR\conf\point_list.txt $INSTDIR\conf\point_calc.txt $INSTDIR\conf\nginx_https.conf $INSTDIR\conf\nginx_http.conf "
  CreateShortCut "$DESKTOP\OSHMI\Nginx and PHP Start.lnk"         "$INSTDIR\nginx_php\start_nginx_php.bat"

; apaga o cache do chrome
  Delete "$INSTDIR\browser-data\Default\Cache\*.*"
  RMDir /r "$INSTDIR\browser-data\Default\Web Aplications"

; cria regras de firewall

; Add an application to the firewall exception list - All Networks - All IP Version - Enabled
  SimpleFC::AddApplication "OSHMI Webserver" "$INSTDIR\bin\webserver.exe" 0 2 "" 1
  Pop $0 ; return error(1)/success(0)

  SimpleFC::AddApplication "OSHMI Shell" "$INSTDIR\bin\hmishell.exe" 0 2 "" 1
  Pop $0 ; return error(1)/success(0)

  SimpleFC::AddApplication "OSHMI Mon_Proc" "$INSTDIR\bin\mon_proc.exe" 0 2 "" 1
  Pop $0 ; return error(1)/success(0)

  SimpleFC::AddApplication "OSHMI QTester104" "$INSTDIR\bin\QTester104.exe" 0 2 "" 1
  Pop $0 ; return error(1)/success(0)

  SimpleFC::AddApplication "OSHMI DNP3" "$INSTDIR\bin\dnp3.exe" 0 2 "" 1
  Pop $0 ; return error(1)/success(0)

  SimpleFC::AddApplication "OSHMI MODBUS" "$INSTDIR\bin\modbus.exe" 0 2 "" 1
  Pop $0 ; return error(1)/success(0)

  SimpleFC::AddApplication "OSHMI ICCP" "$INSTDIR\bin\iccp_client.exe" 0 2 "" 1
  Pop $0 ; return error(1)/success(0)

  SimpleFC::AddApplication "OSHMI NGINX" "$INSTDIR\nginx_php\nginx.exe" 0 2 "" 1
  Pop $0 ; return error(1)/success(0)

  SimpleFC::AddApplication "OSHMI PHP-CGI" "$INSTDIR\nginx_php\php\php-cgi.exe" 0 2 "" 1
  Pop $0 ; return error(1)/success(0)

  SimpleFC::AddPort 65280 "OSHMI Webserver" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 65281 "OSHMI Webserver" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 8082 "OSHMI Webserver" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 8099 "OSHMI Webserver" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 51909 "OSHMI Shell" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 8081 "OSHMI Mon_Proc" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 51908 "OSHMI Webserver" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 2404 "OSHMI QTester104" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 65280 "OSHMI QTester104" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 65281 "OSHMI QTester104" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 51909 "OSHMI NGINX" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 80 "OSHMI NGINX" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 443 "OSHMI NGINX" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 9000 "OSHMI PHP-CGI" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 8098 "OSHMI ICCP" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 8097 "OSHMI MODBUS" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 8098 "OSHMI MODBUS" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 8098 "OSHMI DNP3" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  SimpleFC::AddPort 8096 "OSHMI DNP3" 256 0 2 "" 1
  Pop $0 ; return error(1)/success(0)
  
  ; Verify system locale to set HMI language
  !define LOCALE_ILANGUAGE '0x1' ;System Language Resource ID     
  !define LOCALE_SLANGUAGE '0x2' ;System Language & Country [Cool]
  !define LOCALE_SABBREVLANGNAME '0x3' ;System abbreviated language
  !define LOCALE_SNATIVELANGNAME '0x4' ;System native language name [Cool]
  !define LOCALE_ICOUNTRY '0x5' ;System country code     
  !define LOCALE_SCOUNTRY '0x6' ;System Country
  !define LOCALE_SABBREVCTRYNAME '0x7' ;System abbreviated country name
  !define LOCALE_SNATIVECTRYNAME '0x8' ;System native country name [Cool]
  !define LOCALE_IDEFAULTLANGUAGE '0x9' ;System default language ID
  !define LOCALE_IDEFAULTCOUNTRY  '0xA' ;System default country code
  !define LOCALE_IDEFAULTCODEPAGE '0xB' ;System default oem code page

  System::Call 'kernel32::GetSystemDefaultLangID() i .r0'
  System::Call 'kernel32::GetLocaleInfoA(i 1024, i ${LOCALE_SNATIVELANGNAME}, t .r1, i ${NSIS_MAX_STRLEN}) i r0'
  System::Call 'kernel32::GetLocaleInfoA(i 1024, i ${LOCALE_SNATIVECTRYNAME}, t .r2, i ${NSIS_MAX_STRLEN}) i r0'
  System::Call 'kernel32::GetLocaleInfoA(i 1024, i ${LOCALE_SLANGUAGE}, t .r3, i ${NSIS_MAX_STRLEN}) i r0'
  ;MessageBox MB_OK|MB_ICONINFORMATION "Your System LANG Code is: $0. $\r$\nYour system language is: $1. $\r$\nYour system language is: $2. $\r$\nSystem Locale INFO: $3."
  IntOp $R0 $0 & 0xFFFF
  ;MessageBox MB_OK|MB_ICONINFORMATION "$R0"
  IntCmp $R0 1046 lang_portuguese
  IntCmp $R0 1033 lang_english

  ; default is english - us
  Goto lang_english
  
lang_portuguese:
;  MessageBox MB_OK|MB_ICONINFORMATION "Portuguese"
  nsExec::Exec '$INSTDIR\i18n\go-pt_br.bat'
  Goto lang_end
  
lang_english:
;  MessageBox MB_OK|MB_ICONINFORMATION "English"
  nsExec::Exec '$INSTDIR\i18n\go-en_us.bat'
  Goto lang_end

  lang_end:    

  WriteUninstaller "bt-uninst.exe"

  MessageBox MB_OK "OSHMI Installed! To quickly run the system after installed: Open the OSHMI desktop folder and execute the '_Start OSHMI' shortcut."
  
SectionEnd

; Uninstaller

UninstallText "OSHMI Uninstall. All files will be removed from $INSTDIR !"
UninstallIcon "${NSISDIR}\Contrib\Graphics\Icons\nsis1-uninstall.ico"

Section "Uninstall"

; Fecha processos

  ; SetOutPath $INSTDIR\bin
  nsExec::Exec 'net stop OSHMI_rtwebsrv'
  nsExec::Exec 'net stop OSHMI_iec104'
  nsExec::Exec 'net stop OSHMI_iccp'
  nsExec::Exec 'net stop OSHMI_dnp3'
  nsExec::Exec 'net stop OSHMI_modbus'
  nsExec::Exec 'net stop OSHMI_opc'
  nsExec::Exec 'net stop OSHMI_s7'
  nsExec::Exec 'c:\oshmi\bin\stop_all.bat'
  nsExec::Exec 'c:\oshmi\nginx_php\stop_nginx_php.bat'
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\bin\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\browser\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\nginx_php\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\inkscape\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\PostgreSQL\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\grafana\\%'" CALL TERMINATE`
  nsExec::Exec `wmic PROCESS WHERE "COMMANDLINE LIKE '%c:\\oshmi\\db\\%'" CALL TERMINATE`

; Remove an application from the firewall exception list
  SimpleFC::RemoveApplication "$INSTDIR\webserver.exe"
  Pop $0 ; return error(1)/success(0)
  SimpleFC::RemoveApplication "$INSTDIR\hmishell.exe"
  Pop $0 ; return error(1)/success(0)
  SimpleFC::RemoveApplication "$INSTDIR\mon_proc.exe"
  Pop $0 ; return error(1)/success(0)
  SimpleFC::RemoveApplication "$INSTDIR\QTester104.exe"
  Pop $0 ; return error(1)/success(0)
  SimpleFC::RemoveApplication "$INSTDIR\dnp3.exe"
  Pop $0 ; return error(1)/success(0)
  SimpleFC::RemoveApplication "$INSTDIR\modbus.exe"
  Pop $0 ; return error(1)/success(0)
  SimpleFC::RemoveApplication "$INSTDIR\iccp_client.exe"
  Pop $0 ; return error(1)/success(0)
  SimpleFC::RemoveApplication  "$INSTDIR\nginx_php\nginx.exe"
  Pop $0 ; return error(1)/success(0)
  SimpleFC::RemoveApplication "$INSTDIR\nginx_php\php\php-cgi.exe"
  Pop $0 ; return error(1)/success(0)
  SimpleFC::RemoveApplication "$INSTDIR\opc_client.exe"
  Pop $0 ; return error(1)/success(0)
  SimpleFC::RemoveApplication "$INSTDIR\s7client.exe"
  Pop $0 ; return error(1)/success(0)

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OSHMI"
  DeleteRegKey HKLM "SOFTWARE\OSHMI"
  WriteRegStr  HKCU "Software\Microsoft\Windows NT\CurrentVersion\Winlogon" "Shell" "explorer.exe"
  WriteRegDword HKCU "Software\Microsoft\Windows\CurrentVersion\Policies\System" "DisableTaskMgr" 0x00
  
  nsExec::Exec '$INSTDIR\etc\remove_services.bat'
  
  Delete "$INSTDIR\*.*"
  Delete "$INSTDIR\bin\*.*"
  Delete "$INSTDIR\bin\platforms\*.*"
  Delete "$INSTDIR\charts\*.*"
  Delete "$INSTDIR\conf_templates\*.*"
  Delete "$INSTDIR\conf\*.*"
  Delete "$INSTDIR\db\db_cold\*.*"
  Delete "$INSTDIR\db\*.*"
  Delete "$INSTDIR\docs\*.*"
  Delete "$INSTDIR\etc\*.*"
  Delete "$INSTDIR\extprogs\*.*"
  Delete "$INSTDIR\htdocs\*.*"
  Delete "$INSTDIR\htdocs\images\*.*"
  Delete "$INSTDIR\i18n\*.*"
  Delete "$INSTDIR\fonts\*.*"
  Delete "$INSTDIR\linux\*.*"
  Delete "$INSTDIR\logs\*.*"
  Delete "$INSTDIR\scripts\*.*"
  Delete "$INSTDIR\svg\*.*"
  RMDir /r "$INSTDIR\bin" 
  RMDir /r "$INSTDIR\bin\platforms" 
  RMDir /r "$INSTDIR\browser" 
  RMDir /r "$INSTDIR\browser-data" 
  RMDir /r "$INSTDIR\conf" 
  RMDir /r "$INSTDIR\db" 
  RMDir /r "$INSTDIR\docs" 
  RMDir /r "$INSTDIR\etc" 
  RMDir /r "$INSTDIR\extprogs" 
  RMDir /r "$INSTDIR\htdocs"
  RMDir /r "$INSTDIR\i18n"
  RMDir /r "$INSTDIR\linux"
  RMDir /r "$INSTDIR\logs"
  RMDir /r "$INSTDIR\nginx_php"
  RMDir /r "$INSTDIR\scripts"
  RMDir /r "$INSTDIR\svg"
  RMDir /r "$INSTDIR"
  RMDir /r "$DESKTOP\OSHMI"

SectionEnd
                                                                                                                                            
