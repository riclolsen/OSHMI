OSHMI - Open Substation HMI
    Copyright 2008-2020 - Ricardo L. Olsen 
    (https://www.linkedin.com/in/ricardo-olsen/)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    
The system will install to "c:\oshmi\".

Acknowledgements:

  Inkscape - https://inkscape.org
  Inkscape + SAGE - https://sourceforge.net/projects/sage-scada
  Chromium - by "The Chromium Authors"
  Chromium x86 binaries - http://chromium.woolyss.com
  Libiec6150 - https://github.com/mz-automation/libiec61850
  ICCP Client by Fernando Covatti - https://github.com/fcovatti/iccp
  Opendnp3 - https://www.automatak.com/opendnp3/
  Libmodbus - http://libmodbus.org/
  H-OPC - https://github.com/hylasoft-usa/h-opc
  SNAP7/SHARP7 Project by Davide Nardella - http://snap7.sourceforge.net/
  QT - http://www.qt.io
  Lua Language - https://www.lua.org
  GNU Libmicrohttpd: https://www.gnu.org/software/libmicrohttpd
  The Indy Project - http://www.indyproject.org
  FIPS-180-1 compliant SHA-1 implementation by Paul Bakker http://polarssl.org
  NSIS (Nullsoft Scriptable Install System) - http://nsis.sourceforge.net/Main_Page
  Open Sans Fonts by Steve Matteson
  Source Sans Pro by Adobe Systems Incorporated. License - http://scripts.sil.org/OFL
  Google Fonts - https://fonts.google.com/
  JQuery - http://jquery.com
  JQuery UI - http://jqueryui.com
  Shortcut - http://www.openjs.com/scripts/events/keyboard_shortcuts
  Core JS - http://www.sitepoint.com
  Modernzr - http://www.modernizr.com
  Flot - http://www.flotcharts.org
  Snap SVG - http://snapsvg.io
  Pergola - http://www.dotuscomus.com/pergola
  X3DOM - http://www.x3dom.org
  Chroma.js - https://github.com/gka/chroma.js
  TabSystem - http://www.developer-x.com
  CSS Animation Cheat Sheet - http://www.justinaguilar.com/animations
  D3JS - http://d3js.org
  Radar-chart-d3 - https://github.com/alangrafu/radar-chart-d3
  Vega A Visualization Grammar - https://vega.github.io/vega
  NSSM - The Non-Sucking Service Manager: http://nssm.cc
  NGINX - http://nginx.org
  APACHE - http://apache.org
  PHP - http://php.net
  WGet - http://www.gnu.org/software/wget
  SQLite - https://www.sqlite.org
  PostgreSQL - https://www.postgresql.org/
  BIGSQL - https://www.openscg.com/bigsql/package-manager.jsp/
  Grafana - https://grafana.com/
  MongoDB - https://github.com/mongodb/mongo
  Some icons from - http://keyamoon.com/icomoon and http://raphaeljs.com/icons
  CSS Loaders from Luke Haas - https://projects.lukehaas.me/css-loaders/
  JSMN - https://github.com/zserge/jsmn
  
-------------------------------------------------------------------

QUICKSTART:

To quickly run the system after installed, open the OSHMI desktop folder and:

  - On the OSHMI desktop folder: execute _Start_OSHMI.
 OR 
  - Execute the "Webserver" program.
  - Next open the Screen Viewer (and other viewers) using the icons on the OSHMI folder.
 OR
  - Execute the "HMIShell" program (intended method for production/locked HMIs).
  - Next open the Screen Viewer (and other viewers) using the HMIShell.

The default password for HMIShell is "oshmi".

The system is preconfigured to run a simulation with an example point list and screens.
To simulate a command, click on a breaker and push the "Command" button then choose 
  an action like "open" or "close" and push the action button.
To stop the system click on the "Stop_All" icon in the OSHMI folder.
To edit and create new SVG screens, use the customized Inkscape+SAGE 
  (shortcut in the OSHMI folder). 
Screens are in "c:\oshmi\svg\".
OSHMI configuration files are in "c:\oshmi\conf\".
For more info about configuration please read the OSHMI Configuration Manual.

By default, the system is configured to allow HTTP access only by the local machine.
To allow other IP addresses edit the "c:\oshmi\conf\nginx_access_control.conf" file.
To configure safe remote client access, configure IP address access control, HTTPS, 
  client certificates and user authentication directly in the Nginx configuration files.

-------------------------------------------------------------------

The customized Inkscape SVG graphics editor is a derivative of the Inkscape Project
 work and of the Ecava SAGE work, it is not in any way associated with the Inkscape 
 Project nor Ecava.
We respect and intend to comply with the branding policy of the Inkscape Project
  https://inkscape.org/en/about/branding/
In our view, the modifications introduced are not substantial and do not intend 
  to create a competitive product.
The modifications introduced are meant only to allow the use of the software as a 
  SCADA Synoptic Editor.
We respect the GPL license of the upstream work by making the source code of the 
  modifications to the original Inkscape and SAGE code available at:

Source Code:
  https://sourceforge.net/p/oshmiopensubstationhmi/mercurial/ci/default/tree/inkscape_sage_src/

-------------------------------------------------------------------

Recommended utils (can be downloaded running the "download_external_progs.bat" script that 
  is located inside the "extprogs" folder):
 - Alternative (older) screen editor Inkscape 0.481 + SAGE 3.03. (To edit/create SVG screens)
     https://sourceforge.net/projects/sage-scada/files/SAGE%20v3.03/inkscape0481_sage303.exe/download
 - SysInternals (useful systems utilities)
     http://technet.microsoft.com/en-us/sysinternals/default
     http://live.sysinternals.com/
 - Notepad++ (text editor): http://notepad-plus-plus.org/
 - Visual Studio Code: https://code.visualstudio.com/
     
Recommended Operating Systems:
 - Windows 10 Pro/Home/Enterprise, 32 or 64 bits.
 - Windows 8/8.1 Pro/Home, 32 or 64 bits.
 - Windows 7 Pro/Home, 32 or 64 bits.
 - Modern Windows server OSs.

Please use an updated OS.

If you encounter errors related to Windows Universal C Runtime (CRT), update the Windows OS or see:
  https://support.microsoft.com/help/2999226/update-for-universal-c-runtime-in-windows

Can also run on:
 - Linux Mint (under Wine, experimental), follow instructions from the OSHMI configuration manual.
 - Other Linux distros (no specific instructions provided).
 - See configuration manual.

Not anymore compatible Operating Systems:
 - Windows Vista 32/64 bits (require an old version of Chromium not included).
 - Windows XP 32/64 bits (require an old version of Chromium not included).
   On XP and Vista it is possible to use OSHMI Version 3.17 with Chromium version 49:
     https://sourceforge.net/projects/oshmiopensubstationhmi/files/oshmi_setup_v.3.17.exe/download
     https://sourceforge.net/projects/oshmiopensubstationhmi/files/chromium_browser_for_win_xp.zip/download
 
Client user interface can be used on:
 - Modern IOS and Android devices.
 - Any device with an updated HTML5 browser like Chrome/Chromium, Firefox, Opera, Edge or Safari.
 - Edge currently does not support SVG SMIL animations.
 - Internet Explorer is not supported.

-------------------------------------------------------------------

BUILD REQUIREMENTS.

To compile the source code on Windows you will need the following environments:

Webserver, HMIShell, Mon_Proc and Sim_TR:
 - Borland C++ Builder 5.
 - Indy 9 Components (for C++ Builder 5)- http://www.indyproject.org/download/Files/Indy9.html.

QTester104:
 - QT 5.13 or newer.

IEC61850 Driver:
 - Visual Studio Community (2022 or newer).
 - Libiec6150 library https://github.com/mz-automation/libiec61850.
 - DOTNET 6.0 or later.

DNP3 and Modbus Drivers:
 - Visual Studio Community (2015 or newer).
 - OpenDNP3 and Libmodbus libraries.

OPC Driver:
 - Visual Studio Community (2015 or newer).
 - Forked H-OPC library https://github.com/riclolsen/h-opc.
 - DOTNET framework 4.7.2 or later.

S7 Driver:
 - Visual Studio Community (2019 or newer).
 - DOTNET Core 3.1 framework.

Inkscape+SAGE:
 - See http://wiki.inkscape.org/wiki/index.php/Compiling_Inkscape_on_Windows_with_MSYS2.

-------------------------------------------------------------------

Notes for version 6.29:

IEC61850:
 - IEC61850 Client driver updated to version 0.3.

-------------------------------------------------------------------

Notes for version 6.28:

IEC61850:
 - New IEC61850 Client driver.

-------------------------------------------------------------------

Notes for version 6.27:

Modbus:
 - Added read of pair of consecutive input registers as floats.

Chromium:
 - Updated to version 108.0.5359.125.

SQLite:
 - Updated to version 3.40.0.

-------------------------------------------------------------------

Notes for version 6.26:

Screen Viewer:
 - Fixed problem with Time Machine.

Chromium:
 - Updated to version 98.0.4758.102.

SQLite:
 - Updated to version 3.37.2.

-------------------------------------------------------------------

Notes for version 6.25:

Chromium:
 - Updated to version 94.0.4606.61.

SQLite:
 - Updated to version 3.35.4.

HMIShell:
 - Updated to version 6.25, fixed problems with per OS user config.

-------------------------------------------------------------------

Notes for version 6.24:

Chromium:
 - Updated to version 91.0.4472.212.

-------------------------------------------------------------------

Notes for version 6.23:

Webserver.exe:
 - Updated to version 6.23.
 - Record to hist table values even when alarm is inhibited for a point.

HMI Shell:
 - Possibility to specify configurations per OS user (USER_USERNAME section).

SQLite:
 - Updated to version 3.35.4.

Chromium:
 - Updated to version 90.0.4430.72.

-------------------------------------------------------------------

Notes for version 6.22:

QTester104:
 - Updated to version 2.4.
 - Fixed packet read problem.

Chromium:
 - Updated to version 88.0.4324.150.

-------------------------------------------------------------------

Notes for version 6.21:

HMI Shell:
 - Options to set HTTP port and URL for Shell API in ini file.

Modbus:
 - Support for RS232 and RS485.

-------------------------------------------------------------------

Notes for version 6.20:

WARNING! Due to hist.sl3 table format change, old hist.sl3 tables must be replaced by the table with new format!
After upgrading version, replace c:\oshmi\db\hist.sl3 by c:\oshmi\db\db_cold\hist.sl3.
Alternatively delete c:\oshmi\db\hist.sl3 before upgrading to 6.20 version.

Webserver:
 - Updated to version 6.20.
 - Changed hist.sl3 table format to add FIELDTS column to record field timestamp.

Screen Viewer:
 - Fixed PHP/SQLite error related to documental annotations.

-------------------------------------------------------------------

Notes for version 6.19:

DNP3 Client Driver:
 - Updated to version 0.77.
 - Send timestamp data for analog points when available (via I104M).

Webserver:
 - Updated to version 6.19.
 - Added formula 56 (ON for 2 single points for a double state ok and valid).
 - Added formula 57 (ON for a double state point ok and valid).

-------------------------------------------------------------------

Notes for version 6.18:

DNP3 Client Driver:
 - Updated to version 0.76.
 - Support for Serial Port connections.

SQLite:
 - Updated to version 3.33.0.

-------------------------------------------------------------------

Notes for version 6.17:

Webserver.exe:
 - New formula (54) for creating a double digital point from 2 single digitals.
 - New formula (55) for division of 2 parcels (P1/P2).

DNP3 Client Driver:
 - Updated to version 0.75.
 - Fixed problem with double bit binary.

SQLite:
 - Updated to version 3.32.3.

-------------------------------------------------------------------

Notes for version 6.16:

Events Viewer:
 - Avoid table display problem on large width (ultra-wide) monitors.

DNP3 Client Driver:
 - Updated to version 0.74.
 - Fixed problem disabling unsolicited.
 - Fixed problem with digital with relative time SOE (group 2 var 3).

SQLite:
 - Updated to version 3.32.2.

-------------------------------------------------------------------

Notes for version 6.15:

Screen Viewer:
 - Better error handling for documental annotations.

Webserver.exe:
 - Changed component that do some GET HTTP requests (redundancy only).

-------------------------------------------------------------------

Notes for version 6.14:

HMI Shell:
 - Updated to version 6.14.
 - Do not try to close Webserver directly when logoff/restart/turnoff (leave to etc/do_*.bat scripts).

Modbus client:
 - Updated to version 1.09.
 - Fixed memory leak.
 - New config option to change/disable I104M listen TCP port.
 - Can be combined with other protocol drivers in monitor only mode (no commands).
 
DNP3 client:
 - Updated to version 0.72.
 - OpenDNP3 library updated to version 2.3.2.
 - Better ini file reader.
 - New config option to change/disable I104M listen TCP port.
 - Can be combined with other protocol drivers in monitor only mode (no commands).

Chromium:
 - Updated to version 80.0.3987.87.

PHP:
 - Updated to version 7.4.2 x86 NTS.

SQLite:
 - Updated to version 3.31.1.

-------------------------------------------------------------------

Notes for version 6.13:

Modbus client driver:
 - Updated to version 1.07.
 - Libmodbus updated to version 3.1.6.
 - Shared connection to repeated IP/Port.
 - Fixed problem with RTU reconnection.
 - Supported setting of Slave ID.
 - Supported holding registers as 16 bit bitstrings of digital statuses for read and write.
 - Supported setting of timeouts for each slave.
 - Better ini file reader.
 - Better logs.
 - Built with Visual Studio 2019. 

Webserver.exe:
 - I104M transport now supports ASDU 151(16 bit bitstring) and 64 (32 bit bitstring command).
 - Allows to write commands to address 0 (zero) of MODBUS using address = "-1" (minus one).

Events Viewer:
 - Fixed timeout and retries on Ajax call of events list.

-------------------------------------------------------------------

Notes for version 6.12:

Screen Viewer:
 - Pinned annotations (#PIN on documental annotations).
 - Fixed timeout problem writing annotations to redundant machine when down.

PHP:
 - Updated to version 7.4.1 x86 NTS.

Linux:
 - Added example of init.d script to run OSHMI as a Linux Daemon (background service).

Chromium:
 - Updated to version 79.0.3945.88 x86.

-------------------------------------------------------------------

Notes for version 6.11:

QTester104:
 - Updated to version 2.1.
 - Support for parameters commands (ASDUs 110-113) in the QTester104 UI.
 - Register commands results from RTUs in the Point Map panel.

S7 Client:
 - Rebuilt with DOTNET Core 3.1.

Screen Viewer:
 - Fixed "exec_on_update" event from Script Tab.
 - Fixed !TMP and !ALM special markup codes. 
 - (Re)Addressed problem starting SMIL animations (Animate function) with Chrome 78.
 - Mouse wheel events declared "passive".
 
Script.lua.
 - Fixed example script.lua file with existing point numbers.

PHP:
 - Updated to version 7.4.0 x86 NTS.
 - This PHP version embeds SQLite3 3.30.1.
 - Updated MongoDB PHP extension.
 - Updated php.ini to newest template.

Events Viewer:
 - Fixed order of aggregated events for updated PHP 7.4 w/SQLite >= 3.30.

Chromium:
 - Updated to version 78.0.3904.108.

-------------------------------------------------------------------

Notes for version 6.10:

Screen Viewer:
 - Addressed problem starting SMIL animations (Animate function) with Chrome 78.
 - Show preview window at the opposite corner of mouse coordinates.

-------------------------------------------------------------------

Notes for version 6.9:

S7 Client:
 - Initial version of S7 client protocol driver (for Siemens PLCs).

Webserver:
 - Fixed limits checking when limits are fully open.
 - Do not check limits when zeroed limits superior and inferior (variable always normal).

Events Viewer.
 - Fixed timezone setting problem.
 - Changed default timezone to "America/Recife" for PHP scripts.

SQLite:
 - Updated to version 3.30.1.

-------------------------------------------------------------------

Notes for version 6.8:

OPC Client:
 - Updated to version 0.2.
 - Client can now specify the application certificate file and password.
 - Client can now set the application name.
 - Added Certificate Generator Tool and example of self-signed certificate creation.
 - Uses now a forked customized version of h-opc.
 - Fixed application URI problem with certificate validation.
 - Fixed OPC DA type identification.
 - Tag data type parameter is now considered optional, leave empty to let the driver 
   detect the type automatically.
 - Updated driver documentation.

QTester104:
 - Updated to version 2.0.
 - Changed underlying transport protocol from BDTR to I104M.
 - Removed limitation of addresses > 65535 when forwarding 
   to OSHMI webserver.exe.
 - Added ASDU types bitstring and integrated totals.
 - Added Test Command with time tag.
 - Added Reset Process Command.
 - Compiled with QT 5.13/MingGW 32 bits.
 - Support for 64 bit compilation.
 - Fixed most compiler warnings.

Webserver:
 - Removed support for the old BDTR adapter protocol.
 - Removed section BDTR from hmi.ini config file.
 - Fixed blocking annotation synchronization.
 - Blocking annotations limited to 999 characters.
 - Documental annotations limited to 4000 characters.

Installer:
 - Do not rewrite browser configuration in "hmi.ini".

Chromium:
 - Updated to version 77.0.3865.75.
 - Much better notification display with more room and
   options to expand/contract/block notifications.

Nginx:
 - Updated to version 1.16.1.

-------------------------------------------------------------------

Notes for version 6.7:

Webserver.exe:
 - Modified treatment of floating exception to just fail the point and preserve the old value.

SQLite:
 - Updated to version 3.29.0.

-------------------------------------------------------------------

Notes for version 6.6:

Screen Viewer:
 - "tag" variable made available for read in scripts of "Color" XSAC processing.

Webserver.exe:
 - Fixed problem when correcting AnsiString.sprintf %g double values 
   (that prints 00.0 or -00.0).

-------------------------------------------------------------------

Notes for version 6.5:

OPC Driver:
 - New OPC UA/DA (TCP/IP binary) client driver version 0.1.

Chromium:
 - Updated to version 74.0.3729.131.

PHP:
 - Updated to version 7.2.18 x86 NTS.
 - The version was downgraded due to SQLite table locking problems with PHP 7.3.

Nginx:
 - Updated to version 1.16.0.

-------------------------------------------------------------------

Notes for version 6.4:

Screen Viewer:
 - Vega JSON scripts can be directly embedded in the screen file.
 - Fixed use of variables shortcuts (%n, %m, ...) in !EVAL expressions.
 - Vega updated to version 5.3.5.
 - Vega-lite updated to version 3.1.0.
 - Current value in faceplate highlighted in cyan.

Screen, Tabular and Events Viewers, webserver.exe:
 - Fixed manual value imposing for redundant HMI.

Power BI integration:
 - Added Power BI example file for integration with OSHMI using the OData Protocol.

Tableau integration:
 - Added missing php file that find tags for Tableau web data connector.
 - Fixed data retrieval problem in Tableau web data connector. 
 - Tableau WDC API lib updated to version 2.3.

OData and JSON data exporters:
 - Added CORS headers.
 - Added query_only SQLite pragma.

PHP:
 - Updated to version 7.3.4 x86 NTS.

SQLite:
 - Updated to version 3.28.0.

Other:
 - Better scripts to stop processes.

-------------------------------------------------------------------

Notes for version 6.3:

Events Viewer:
 - Better timeout handling for SQLite SOE table.
 - One automatic retry for AJAX queries.

Webserver.exe:
 - Repositioned I104M window that was opening out of the screen bounds.

QTester104.ini:
 - Updated to version 1.26.
 - The ini file (qtester104.ini) is now first looked for in the same executable directory, 
   if not found look in the "../conf" dir or the file name can be passed via a command line argument.

Linux:
 - Update install instructions for Linux installation.

-------------------------------------------------------------------

Notes for version 6.2:

Screen Editor:
 - Updated upstream Inkscape to version 0.92.4.
   See https://inkscape.org/releases/0.92.4/.
 - Vega JSON specifications can be entered also in the "Script" tab.
 - Added "exec_once" and "exec_on_update" to the "Script" tab.

Screen Viewer:
 - Updated to execute the new "Script" tab markup.

Alarms Viewer:
 - Expandable area of station filters.
 - Persistency of filters saved in localStorage area of the browser.
 - Filters loaded for all stations in alphabetical order.
 - Legibility enhancements.
 - Select All and Unselect All buttons for filters.

Chromium:
 - Updated to version 73.0.3683.75.

-------------------------------------------------------------------

Notes for version 6.1:

Webserver:
 - Configurable HTTP port for event and annotation synchronization in redundant mode.

Screen Viewer:
 - Protection of local variables when executing scripts in the SVG file.

SQLite:
 - Updated to version 3.27.2.

Linux:
 - Updated config files for Nginx and PHP.

-------------------------------------------------------------------

Notes for version 6.0:

This release introduces many performance optimizations under the hood.
Some scripted displays may break due to changes in the the way the SVG file is now attached to the page.
The example displays were updated, if you are updating OSHMI over a previous version
please copy them (*.svg) from the config_templates folder to SVG folder.

Data Client Mode:
 - Now OSHMI can be configured as a data client to access an OSHMI server, the client requests only realtime data. 
   In this mode the local installation must have all SVG files.
   This mode makes possible extremely low bandwidth communications with the remote server.
   This is ideal for satellite, GPRS and other applications that require very low bandwidth comms.
   Data can be encrypted (using https/client certificates) and compressed by the Nginx remote server.
   The old Client Mode that requests everything to the server is still available.

Webserver:
 - Replaced Indy HTTP server with a new server based on the GNU Libmicrohttpd (DLL).
 - Possibility of starting microhttpd daemon in single thread mode or thread per connection.
 - Added http header "Access-Control-Allow-Origin" for CORS. Default origin is "*".
 - Significant performance optimizations of real time data queries.
 - Fixed JSON command sent to invalid secondary address (redundant mode only).
 - Fixed problem when received malformed float value.

Screen Viewer:
 - Modified insertion of the SVG file in the page (from inside an <object> to direct <svg>).
 - Changed point values queries to HTTP GET method instead of POST.
 - SVG file loaded with fetch().
 - Faster loading of displays with less DOM style recalculations (reflows).
 - Fixed redundant processing of SAGE/XSAC tags for SVG TEXT elements.
 - Make Animate, RemoveAnimate, ShowHideTranslate and ShowImage functions available through WebSAGE/$W object.
 - Vega updated to version 4.4.0.
 - Vega-Lite updated to version 2.6.0. 
 - No more support for older browsers like I.E. as Vega 4 requires ES6.
 - Better error management in vega_websage.js.
 - Elements linked to invalid or unavailable tags are now made invisible and it is logged to the 
   console the tag and element id.
 - D3JS number formatting support for "Get" directive.
 - Color templates shortcuts (-clr-nnn) available also for Vega visualization specifications.

Events, Tabular & Screen Viewers:
 - New functions for requesting scripts and JSON.
 - Defined a PDO timeout of 3s for query of events in SQLite table.

QTester104:
 - Updated to version 1.25.
 - Can set TCP port in the UI.
 - Point mapping can be set on or off by a checkbox (when disabled the performance is better).
 - Show Common Address of ASDU in Point Map.
 - Much improved TCP/IP performance.

Linux:
 - Updated Nginx configuration files.

Dynamic Excel Sheet (c:\oshmi\etc\dyn_sheet.xlsx):
 - Updated web queries to work with new microhttpd.

SQLite:
 - Updated to version 3.27.1.

Chromium:
 - Updated to version 71.0.3578.98.

PHP:
 - Updated to version 7.3.1 x86 NTS.
 - PHP.ini updated to production template of version 7.3.

Nginx:
 - Configured with upstream directive with keepalive for PHP FAST-CGI and the realtime webserver.
 - Configured with tcp_nodelay, tcp_nopush and sendfile (all on for linux, all off for windows).
 - Configured to allow bigger URL queries.
 - Updated to version 1.15.8.
 - Configure timeout of 10s for PHP scripts.

Uninstaller:
 - Fixed unnecessary confirmation message for removal of IEC104 log service.

-------------------------------------------------------------------

Notes for version 5.5:

Webserver:
 - Reverted floating point treatment from version 5.3 due to performance loss.

SQLite:
 - Updated to version 3.25.3.

-------------------------------------------------------------------

Notes for version 5.4:

Webserver:
 - Fixed limited precision in serving values.

DNP3 driver:
 - Updated to version 0.71.
 - Removed limitation of address zero for master and slaves.

-------------------------------------------------------------------

Notes for version 5.3:

Webserver:
 - New JSON UDP protocol driver interface. Now it is easy to feed data to 
   OSHMI using simple JSON UDP messages. See the Configuration Manual.   
 - Can now export files with realtime data to MongoDB.
 - Fixed setting of hysteresis for out of range verification of analog values.
 - Try to avoid errors due to malformed float values.
 - Restricted I104M protocol driver UDP messages to trusted origins.

QTester104:
 - Updated to version 1.24.
 - Allowed different common addresses in the same connection.

MongoDB:
 - Available as an addon for 64 bit Windows systems.

PHP:
 - MongoDB extension enabled.

Chromium:
 - Updated to version 69.0.3497.81.

Nginx:
 - Allowed access by default to htdocs/eventsync.php in nginx_http.conf .

SQLite:
 - Updated to version 3.25.2.

-------------------------------------------------------------------

Notes for version 5.2:

Tabular:
 - Fixed bay detection for select box when the number of bays is 2.

DNP3 driver.
 - Updated to 0.7 based on a newer version of Opendnp3 (2.2.0).
 - Compilation on Visual Studio 2017.
 - Fixed channel creation (multiple slaves on same IP/Port generate only one channel).
 - Better logs.
 - Non-blocking execution of commands.

Modbus driver.
 - Compilation on Visual Studio 2017.

HMIShell:
 - Browser command-line arguments changed for opening viewers.
 - BROWSER_OPTIONS key in hmi.ini created (applied to all viewers).
 - Possibility of pointing to OSHMI servers according to OS logged-in user name.

Nginx configuration:
 - Changed position of the include of nginx_access_control.conf in nginx_http.conf 
   and nginx_https.conf. The effects of IP access directives now extends to 
   the full server sections.

-------------------------------------------------------------------

Notes for version 5.1:

Webserver.exe:
 - Point values are now stored internally as 64bit floats.
 - Now the system can handle very big values down to historian level.
 - Fixed reading of UTF-8 config files with BOM.

Screen Viewer:
 - Support for plot graphics by tag.
 - Simpler animation for changed values (just underline or overline).

DNP3 driver:
 - Updated to version 0.6.
 - Added support for Class 0 Scan.
 - Added support for 32 bits Analog Outputs.
 - Added support for 32 bits Counters and Frozen Counters.
 - Added support for Analog Outputs Status.
 - Added support for Digital Outputs Status.

Trend Viewer:
 - Fixed div tag missing a '>'.

Other:
 - Added directive "Cache Control: no-cache, must-revalidate" to Nginx configuration for SVG
   displays and some other files. Allowing for a better cache validation in client browsers.
 - Support for direct commands by tag.
 - Remove redundant set of mime type header for SVG files in nginx_http(s).conf.

-------------------------------------------------------------------

Notes for version 5.0:

The full web interface was converted to UTF-8 encoding for better internationalization and
better integration with the modern sotfware ecosystem.
Point database txt files now must be encoded in UTF-8. Old files that contains 
international characters must be converted to or recreated in UTF-8. 
This can be easily accomplished using Notepad++, iconv (see below) or some other tool.

This version leverages the power of Timescale PostgreSQL database extension that 
optimizes insertion and query of time series data. The preconfigured add-on can run 
on the OSHMI servers or data can be uploaded to an external server installed on 
any PostgreSQL/Timescale supported OS.
Grafana is available as an add-on to allow for easy construction of customized 
dashboards and alerts.
Now OSHMI is capable of handling historical databases of very large installations 
such as big power plants and control centers.
The historical data storage and analytics capabilities of OSHMI now match or 
supplant that of expensive commercial SCADA systems.

There in now the possibility to provide a Mobile/Intranet/Internet Gateway.
This can be used to OSHMI viewers remotely via a modern web browser.
The access via gateway is for monitoring only (cannot control devices or ack alarms).
The database backend for the gateway can be PostgreSQL (strongly recommended) or SQLite.
The enabling of the gateway is configured at the end of c:\oshmi\nginx_http.conf file.

Integration of analytics capabilities now include: 
 - PostgreSQL (fully featured open source database)
 - Timescale (time series extension for PostgreSQL)
 - Grafana (via PostgreSQL)
 - Power BI (via OData, JSON and PostgreSQL) 
 - Tableau (via OData, JSON and PostgreSQL)
 - Excel (via web query and PostgreSQL/ODBC)

PostgreSQL/Timescale, Grafana:
 - PostgreSQL/Timescale and Grafana now available as addons for 64bit Windows only.
 - PostgreSQL/Timescale and Grafana can be installed on other platforms manually.
 - New scripts to process the SQL text files compatible w/ Postgresql/Timescale.

Screen Viewer:
 - Better compatibility with other web browsers.
 - Animation of Arc function (donut graphics).
 - Better handling of mouse events.
 - Correction of some specific historical graphic plot type.
 - Support for (current) round time period for Vega 3 historical plot.
 - New markup for "get" attribute for digital points (OFF|ON) messages.
 - Fixed problem with center of rotation (for "rotate" attribute).
 - Fixed problem with positioning of radar charts.
 - !EVAL expressions now have access to the SVG DOM reference (thisobj) for the 
   container of the markup object.

Events Viewer:
 - Fixed problem showing silent button when acknowledging alarms.
 - Fixed problem of error when setting table cell width in IE.
 - Readability improvements.

Tabular/Alarms Viewers:
 - Fixed problem of error when setting table cell width in IE.

Webserver.exe:
 - Do not load command points with SUPCMD=0.
 - Implemented watchdog timeout for absence of HTTP requests
   as the server sometimes stops listening under Wine/Linux.
 - Can now output files SQL text files compatible w/ Postgresql/Timescale.
 - Configuration of timezone moved to conf/hmi.ini | [webserver] | TIMEZONE.
 - Limited number of parcels in calculated points.
 - Safer annotation code, sync.
 - Fixed reading of point_list.txt last field after end quote.
 - Tuning of historical files generation.
 - Commas in description or states of point are converted to points.

HMIShell:
 - Do not search for already opened Viewers on Linux.
 - Fixed problem with screen_list.js strings size.
 - Fixed remote beep issue.

Linux: 
 - New scripts for running on Linux.
 - Reference Linux distribution is now Mint 19.0 XFCE 32bits.
 - QTester104 v.1.23 natively compiled.
 - Script to beep for alarms out of Wine.

DNP3 driver:
 - Updated to version 0.5.
 - Added support for binary counters and frozen counters.

Documentation:
 - New instructions for installation on Linux Mint.
 - Instructions to install PostgreSQL/Timescale and Grafana add-ons.

Docs Viewer:
 - Moved to htdocs/listdocs.php.

Report generator:
 - New tool to create interactive reports by changing a database query.
 - Can export CSV files.

SQLite:
 - Updated to version 3.24.0.
 - Reduced delay time for events processing from 2 to 1 second.

PHP:
 - Updated to version 7.2.7 NTS.
 - Fixed unnecessary kill of cmd.exe when stopping php/NGinx services.
 - PostgreSQL PDO driver enabled.

Nginx:
 - Updated to version 1.15.

Other:
 - Included utility convert-config-to-utf8.bat to convert old config files 
   encoded as ISO8859-1 (Latin1) to UTF8.
 - Fixed problem with quotes in annotations.
 - Improvements to OData and JSON exporters for SQLite.
 - New OData and JSON exporters for PostgreSQL.
 - More precise scripts to stop OSHMI process.
 - Included binaries for 'iconv' command line tool that can convert text files
   to and from various encoding schemes (including UTF-8 and ISO8859-1).
   Example, convert from ISO8859-1 to UTF-8: 
     ..\bin\iconv  -f ISO8859-1 -t UTF-8 point_list.txt > point_list_utf8.txt

-------------------------------------------------------------------

Notes for version 4.15:

Screen Viewer:
Webserver.exe:
 - Changed from HTTP GET to HTTP POST the request for point list to allow larger requests.
   This overcomes browser limitation for URL size.
   Now the number of objects in screens have no more arbitrary limits.
 - Changed API URLs used by HMIShell.
 - Vega updated to version 3.3.1.
 - Vega-Lite updated to version 2.4.3. 
 - Added Polyfill for requestAnimationFrame for better compatibility with older browsers.

Events Viewer:
 - Fixed misalignment of column text when resized.
 
HMI Shell:
 - Use new API URL.
 - Get the keyboard focus when opened the screen list.

Config:
 - Updated nginx_http.conf and nginx_https.conf in the conf_templates folder.
 - Old conf\nginx_http.conf overwritten and backup to conf\nginx_http.conf.bak.

Chromium:
 - Updated to version 66.0.3359.117.
 - Redefined Chromium configurations to allow notifications.

-------------------------------------------------------------------

Notes for version 4.14:

Screen Editor:
 - Updated Inkscape to version 0.92.3.
   Various bug fixes and performance improvements (much faster startup, etc.).
   See https://inkscape.org/releases/0.92.3/.

Screen Viewer:   
 - Change mouse cursor to pointer for mouse event handling.

-------------------------------------------------------------------

Notes for version 4.13:

HMIShell:
 - New improved menu to choose screens started by Windows-Key globally.
 - Global handling of ALT+F12 shortcut to show options menu.

Screen Viewer:
 - Removed creation of objects that can identify relations for faster loading.
 - Snap SVG updated to 0.5.1.
 - Fixed creation of unnecessary objects.

Webserver.exe:
 - Altered code to generate events and beeps for state changed (KConv2=-1) 
   even when the invalid state qualifier is true.
 - New formula to calculate distance from max to min of parcels.

Modbus:   
 - Can now read floating point values (as a combination of 2 16bit holding registers).

SQLite:
 - Updated to version 3.23.1.

Fonts:
 - Available Open Sans v15 (various) and Roboto v18 (Regular).

Chromium:
 - Updated config data profile to match version 64.0.3282.140.

Other:
 - Included htpasswd.exe and htdigest.exe binaries for managing Nginx user authentication.
 - Added .reg files to implement chromium policy for auto selection of client certificates.

-------------------------------------------------------------------

Notes for version 4.12:

Export tools for historical data:
 - OData and JSON: optimized queries, now produces results much faster.
 - New tools for CSV export of historical and SOE data.

Webserver.exe:
 - Fixed problem of historical recording of calculation of communication state.

Screen Viewer:
 - Recover HIDETB (hide toolbar) parameter when reloading page.

-------------------------------------------------------------------

Notes for version 4.11:

Events Viewer:
 - Better sorting of events order when field timetags are disabled.
 - Significantly optimized queries when events are not aggregated.

Screen Viewer:
 - Fixed qualifier flags treatment for some rare cases.

SQLite:
 - Updated to version 3.22.0.

Chromium:
 - Updated to version 64.0.3282.140.

PHP:
 - Updated to version 7.2.2.

-------------------------------------------------------------------

Notes for version 4.10:

Screen Editor:
 - New embedded Screen Editor: Inkscape 0.92.2 + SAGE 4.15 32bit custom built for OSHMI.
   Startable by the Inkscape SAGE icon on Desktop.
   Separate installation from other Inkscape or Inkscape+SAGE versions.
   Installed to c:\oshmi\inkscape directory.
   Other Inkscape SAGE versions 0.48+3.03 or 0.91+4.16 still compatible.
   Sources added to the Mercurial repository.

Screen Viewer:
 - Possibility to tag screens using prefixes, so the same screen can be used with data 
   from different origins (same tag suffix, but different prefix).
 - Updated Radar Chart library.
 - Corrected bug drawing some Vega-Lite charts.
 - Vega: updated to version 3.0.8.
 - Vega-Lite: updated to version 2.0.3.
 - Updated code to handle dynamic embedding option.
 - Possibility to obtain more point info for Vega markup.
 - Possibility to embed Vega/Vega-Lite JSON specification in the SVG file.
   Just put the code directly in the "prompt" field of the dialog instead of a URL.

Curves Viewer:
 - Better copy to clipboard function.

Tabular, Alarms and Events Viewers:
 - New button to copy data to clipboard (suitable to Excel).

DNP3 Driver:
 - Updated to version 0.4.
 - Can now restart integrity poll queues when detect absence of data in a connection.

-------------------------------------------------------------------

Notes for version 4.9:

Modbus driver:
 - Integrated in the installer.
 - Supports Modbus/TCP, multiple IEDs, digital and setpoint commands.
 - Source code kept in a separate project: https://modbus-driver-for-oshmi.sourceforge.io/.

Configuration Manual:
 - Updated install instructions.

Webserver:
 - Updated to support command sending to optional Modbus driver UDP port.

Other:
 - Scripts for creating an IEC60870-5-104 circular buffer logs using Wireshark as Windows service.
 - Included MSVC++ Redistributable 2017.

-------------------------------------------------------------------

Notes for version 4.8:

Config Manager Worksheet v1.0 (c:\ohsmi\conf\oshmi_config_manager.xlsm):
 - New Excel worksheet with macros to manage configuration.
 - Can import, manipulate and export point_list.txt file.

QTester104:
 - Updated to version 1.23.
 - Source code changed to UTF-8.
 - Now logs up to 30000 lines to a circular buffer.
 - More detailed log of received values.
 - Now logs sent frames.
 - Use of colors in log windows.
 - Use Consolas font when available.

Viewers:
 - Use of Meta tag for better compatibility with IE/Edge browsers.

Chromium:
 - Updated to version 62.0.3202.94 32bit, no sync, no WebRTC, no Widevine.    

Demo screens:
 - New demo screen Brasil, with Vega charts, RSS, JSON import, Internet data integrations.
 - New demo screen Office with isometric perspective, Vega charts and animations.

-------------------------------------------------------------------

Notes for version 4.7:

DNP3 Driver:
 - Incoporated in the installer.
 - Source code kept in a separate project: https://dnp3-driver-for-oshmi.sourceforge.io/.

QTester104: 
 - Updated to version 1.22, buit with QT 5.9.2.
 - Not anymore compatible with XP or Vista.

Screen Viewer:
 - Added support for Vega version 3 and Vega-Lite version 2 visualizations.
 - Fixed SHIFT+ENTER shortcut that allows to save screenshot as SVG.
 - New default colors for inhibited alarm tags (gray/lightgray).
 - Inverted priority for annotation tags when both conditions are present: command blocked > inhibited alarm.
 - Fixed incompatibility with old version Chromium that runs on XP/Vista.
 - Changed the default color of measurements.
 - Fixed Tooltips problem with parent object.
 - Fixed problem with backslashes on annotations.

Events Viewer:
 - Highlight (bold) for priority zero acknowledged events.
 - Now is shown the priority color (faded) for acknowledged alarms.

Point Info Dialog (faceplate):
 - New non-blocking annotation field.
 - Redesigned dialog.

Point Command Dialog:
 - Redesigned dialog.

SQLite:
 - Updated to version 3.21.0.
  
Historical Database:
 - Processing of SQL files for historical data now dinamically changes the usage of write cache to speedup 
   database inserts when the performance is slow enough to occur accumulation of SQL files.
 - Script for database files maintenance (\oshmi\db\db_maintenance.bat).
    
Chromium:
 - Updated to version 60.0.3112.101 32b, no sync, no WebRTC, no Widevine.    
  
PHP:
 - Enabled PHP Zend Extension OpCache for better performance.
  
Webserver.exe:
 - Better handling of range limits settings.
 - Better handling of delayed range alarms.
 
HMIShell:
 - Can now set the desktop background color (does not work in Windows 10 without Win Shell).
 - New button to start desktop Sticky Notes (Win7) or installed externally as c:\oshmi\stickynotes\stikynot.exe.
 - Fixed opening of Docs Viewer from client HMI.
 
Webserver.exe:
 - Log (un)inhibited (disable/enable) alarms for points (c:\oshmi\logs\alm_inhibit.log).

-------------------------------------------------------------------

Notes for version 4.6:

Data Export:
  - Connectivity to Tableau and MS PowerBI via OData.
  - Connectivity to Tableau Desktop/Tableau Public Edition via Web Data Connector (WDC).
  - Connectivity to MS PowerBI via JSON.
  See configuration manual.

OSHMI Desktop Folder:
  - Corrected link to Logs Viewer.
  - New link to easily start OSHMI (starts Webserver, Screen and Alarms Viewers).
  - Removed link to open configuration files in PSPad.

Config Files:
  - Included examples of self-signed certificate files nginx.cnf and nginx.key that can be used to enable secure https browsing.
    Command to regenerate (recommended!) those certificate files: 
    c:\OpenSSL-Win32\bin\openssl req -config "c:\OpenSSL-Win32\openssl.cnf" -x509 -nodes -sha256 -days 36500 -newkey rsa:4096 -keyout /oshmi/conf/nginx.key -out /oshmi/conf/nginx.crt -extensions v3_req
    Example openssl.cnf is also included.
    Install OpenSSL for Windows to use this functionality.
    To browse securely in Chromium export certificate as p7b file and import as "Trusted Root Certificate Authorities".
    See: http://www.nullalo.com/en/chrome-how-to-install-self-signed-ssl-certificates/.
  - New config files c:\oshmi\conf\nginx_http.conf and  c:\oshmi\conf\nginx_https.conf for easing edit and preservation of configuration on updates.
  - https disabled by default.
    
HMIShell:
  - Compiled for Pentium Pro instruction set instead of plain 386.
  - Avoid errors on initialization when used as Windows Shell.

Curves Viewer:
 - Removed incompatibilities with PHP7.
 - Set correct charset headers.
 
Events Viewer:
 - Prioritity zero events not shown in italic anymore, just colored bold for better readability.

Alarms Viewer:
 - Prioritity zero alarms not shown in italic anymore, just colored bold for better readability.

Trend Viewer:
 - Better and faster initial curve complete from historical and real time data.

Viewers:
 - Added "mobile-web-app-capable" meta tag.
 - Altered shortcuts to other viewers in Point Info Dialog.

PHP:
 - Updated to 7.1.6.

Nginx:
 - Updated to 1.12.0.

-------------------------------------------------------------------

Notes for version 4.5:

Webserver:
  - Solved bug reading HIST section of hmi.ini.

SQLite:
  - Updated to version 3.19.3.

Screen Viewer:
  - Fixed bug on opacity handling of alarmed blinking objects.
  - Better image map for the pan control.

Chromium:
  - Updated to version 59.0.3071.109 32b,no sync,no WebRTC,no Widevine.

-------------------------------------------------------------------

Notes for version 4.4:

Screen Viewer:
  - New function to make objects draggable.
  - Direct commands in screen with no confirmation.
  - Take advantage of requestAnimationFrame() browser functionality when available.

Webserver:
  - Notification of values variation after analog command issued for faster Viewer's updates.
  - Temporized alarms for analog points limits violations.
  - Safer point list reading.
  - Compiled for Pentium Pro instruction set instead of plain 386.
  
Events Viewer:
  - New icons for modes.
  - Don't show lines side by side anymore when the width is big enough.

Tabular Viewer:
  - Corrected alignment of digital alarms next to analog alarms.
  
Viewers:
  - Log of commands in the localStorage of the browser.
  
Other:
  - Logout/Turnoff/Reboot scripts redirected to etc dir, will not be rewritten on updates.
  - Logout script in etc dir can be configured to work correctly with services configuration.

-------------------------------------------------------------------

Notes for version 4.3:

Webserver:
  - Support for analog commands.
  - Corrected bug importing units of measure.
  - Configurable lifetime for historical data.
  - Log of IP address origin of commands.
  - Logging of annotations.
  - Logging of remote user login/logout.
  
Screen, Tabular and Events Viewers, Command Dialog:
  - Support for analog commands.
  - Timeout for commands resets when option selected or analog value changed.

QTester104:
  - Updated to version 1.21.
  - Corrected bug of wrong month set on commands with time.
  - Support for analog commands comming from OSHMI Webserver BDTR converter.

SQLite:
  - Updated to version 3.18.0.
  
HMIShell:
  - Added log files viewer.
  - Login/Logout log of user.
  
OSHMI Shortcuts Folder:
  - Added shortcuts to Docs and Logs Viewers.

Chromium:
  - Updated to version 57.0.2987.133.

-------------------------------------------------------------------

Notes for version 4.2:

QTester104:
  - Updated to version 1.19.
  - Corrected bug on select before operate commands present in OSHMI 4.0 and 4.1.
  - Removed resctriction of slave address up to 255: now it can be up to 65535.

SQLite:
  - Updated to version 3.17.0.

-------------------------------------------------------------------

Notes for version 4.1:

Events Viewer:
  - New mechanism to configure a relationship between notifications and screens.
    When the user clicks a trip notification it can be opened the respective screen of the occurrence location.
    It is necessary to configure a function in conf/config_viewers.js like EventsViewer_NotificationClick on config_viewers_default.js.

Screen Viewer:
  - Loading effect shown while loading and preprocessing SVG file.
  - Some behind the scenes changes on window event processing.
  - F10 shortcut to animated hide/show toolbar. 
    Mouse-over small visible piece of hidden toolbar to show it again.

Tabular Viewer:
  - Analog values aligned to the right.

SQLite:
  - Updated to version 3.16.2 (better performance and some bugs corrected).

-------------------------------------------------------------------

Notes for version 4.0:

Screen Viewer:
  - Added the descriptive charts library Vega (version 2.6.3) that opens unlimited real time charts capability.
  - Arc graphic.
  - D3JS Updated to version 3.5.17.
  - No more compatibility with old style screens, the only editor now supported is Inkscape+SAGE.
  - Faster screen loading and processing due to the previous item.
  - Point faceplate annotation field with more space.

QTester104:
  - Updated to version 1.17.
  - Support for setting ASDU address for commands.

SQLite:
  - Updated to version 3.15.2.

-------------------------------------------------------------------

Notes for version 3.17:

QTester104:
  - Updated to version 1.16.
  - Copy of values to clipboard.
  - Better logs.
  - Support for Clock Sync Command.
  - Split bar for Log/Points Widgets.
  - Show timetag values.

SQLite:
  - Updated to version 3.15.

Chromium:
  - Updated to version 56.0.2890.0.

-------------------------------------------------------------------

Notes for version 3.16:

QTester104:
  - Updated to version 1.15.
  - Support for analog commands.

SQLite:
  - Updated to version 3.14.2.

-------------------------------------------------------------------

Notes for version 3.15:

Webserver.exe:
  - New formula for evaluating the communication state with a station using point statuses.

-------------------------------------------------------------------

Notes for version 3.14:

Webserver.exe:
  - Corrected bug affecting the lua stack that was causing problems after many commands issued.

-------------------------------------------------------------------

Notes for version 3.13:

Webserver.exe:
  - Tag link suppport for screens.
  - By default beeps only for alarms up to priority 2 (0, 1, and 2).
  - Interception of commands and tag addressing in Lua scripts.
  - Corrected bug related to tag to point number conversion.

Screen Viewer:
  - Support for tag linking (in addition to number linking).
  - Tooltips with point details on mouseover objects.
  - Restored broken Time Machine function.
  - Do not show inhibited alarms on alarm box.
  - Included chroma.js library.

QTester104 Updated to version 1.14:
  - Better redundancy control.
  - Configurable periodic General Interrogation.

Browser:
  - Modified configuration to allow desktop notifications.
  
Events Viewer:
  - Modified for restoring broken Desktop Notifications.
  - Avoid feezing when the events database becomes unresponsive.
  
Tabular Viewer:
  - Shows points with inhibited alarm in a distinct color (brightened) text.
  
SQLite:
  - Updated to version 3.13.
  
NGINX:
  - Configured to allow access and auto listing files to "docs" folder. 
    Links to documents in this folder can now be created in screens.
    
HMIShell:
  - Button of "Help" now shows list of PDF documents from "docs" folder.
  
Installer:
  - Removed install of pdf viewer, now it's only used the internal Chromium PDF Viewer.

-------------------------------------------------------------------

Notes for version 3.12:

Screen Viewer:
  - Expanded the processing of SVG tags to include circle, use, line, polyline, polygon.

-------------------------------------------------------------------

Notes for version 3.11:

HMIShell:
  - Corrected problem when starting HMIShell.
  
MonProc:
  - Added option to specify a complete command line to execute a process.

-------------------------------------------------------------------

Notes for version 3.10:

QTester104:
  - Updated to version 1.13. Added copy of log to clipboard.

-------------------------------------------------------------------

Notes for version 3.9:

Webserver:
  - Removed limit of the number of synchonized alarms for redundant HMI's.

Screen Viewer:
  - Color interpolation on animations.
  - Screenshot to SVG file (SHIFT+ENTER).
  - Better mouse event handlers.
  - More options to plot measures.
  - Avoid showing some errors from alarm box not yet created.

Alarms and Events Viewers:
  - New form of highlighting priority 0 alarms.

Chromium:
  - Updated to version 50.0.2628.0.
  
SQLite:
  - Updated to version 3.10.2

-------------------------------------------------------------------

Notes for version 3.8:

ICCP Client:
  - Corrected bug related to redundancy control.

-------------------------------------------------------------------

Notes for version 3.7:

QTester104:
  - Updated to version 1.12. Added the possibility to stay simultaneously connected to 2 redundant RTU's.

Tabular Viewer:
  - Corrected shortcut [F8] to ack all alarms.
  - Scrollbar to show many substation alarm filters

-------------------------------------------------------------------

Notes for version 3.6:

QTester104:
  - Updated to version 1.11. Added the possibility of scannig a redundant RTU.
  
SQLite:
  - Updated to version 3.8.11.1.
  
Webserver:
  - Avoid closing by request on logout when running as a service.
  
Installer:
  - Changed to avoid overwriting bat files on "bin" and "etc" folders.

-------------------------------------------------------------------

Notes for version 3.5:

HMIShell:
  - Can now be configured to beep. Useful for running webserver as a service
    or for a client HMI (with no webserver running).

-------------------------------------------------------------------

Notes for version 3.4:

QTester104:
  - Updated to version 1.10.

-------------------------------------------------------------------

Notes for version 3.3:

QTester104:
  - Updated to version 1.08.

-------------------------------------------------------------------

Notes for version 3.2:

QTester104:
  - Updated to version 1.07.

-------------------------------------------------------------------

Notes for the version 3.1:

Tabular Visor:
  - Now includes event points.
  
ICCP Client:
  - Updated to version 1.8.1.
  
QTester104:
  - Compiled with QT 5.4.
  
SQLite:
  - Updated to version 3.8.10.2

-------------------------------------------------------------------

Notes for version 3.0 (first public release):

Includes:
 - Chromium 42.0.2309.0.
 - NGINX 1.0.11.
 - PHP 5.4.17.
 - SQLite 3.8.8.2
 - NSSM 2.24
