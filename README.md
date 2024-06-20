# OSHMI - Open Substation HMI
A modern SCADA HMI for substations, IoT and other automation applications.

This project exists because we believe that by combining existing open source projects and tools we can create a very capable, mobile and cloud-friendly HMI system that can rival proprietary software. This approach makes it possible to sum the forces of each project (Chromium, SVG/HTML5, PHP, Lua, SQLite, Inkscape, Libiec61850, OpenDNP3, Nginx, Vega, PostgreSQL, Grafana,…) to achieve a great set of open, evergreen, modular and customizable tools for building great HMIs for automation projects. If you also believe this is possible, please give OSHMI a try and collaborate with the project!

- Mobile and Cloud friendly.
- Created for substation automation and control centers, but can be used for IoT and general SCADA HMI automation applications.
- JSON over UDP data and control interface for easy integration with IoT devices and other systems.
- Built with open source, web based technologies and tools: SVG, HTML5, Javascript, PHP, Lua, QT, SQLite, Nginx, Inkscape, Vega.
- Strong focus on UX and High Performance HMI / ISA101 compatible graphics.
- Redundant mode of operation, dual server/ ?n? clients. Unlimited points, clients, monitors and viewers.
- Easy configuration with plain text files.
- IEC61850, IEC60870-5-104 (QTester104), DNP3, MODBUS, OPC UA/DA, Siemens S7, and ICCP client protocol drivers.
- The client web interface can be accessed by modern IOS, Android devices or by an HTML5 browser.
- Please notice that OSHMI does NOT function by itself as a protocol gateway!

Some Features:
- Screen Viewer: full-graphics, vector based, lossless zoom (SVG). Amazing Vega charts engine. Scale great graphics from the cellphone to the video-wall.
- Events Viewer: millisecond resolution; can operate with field time tags or local time tags; 2 stage acknowledgement/elimination; historical mode.
- Tabular Viewer: point list shown filtered by substation and bay.
- Alarms Viewer: displays alarms, with filters by substation and priority.
- Trend Viewer: follow measurements plot in real time.
- Curves Viewer: historical measurement plot.
- Excel Worksheet w/ macros to manage configuration.
- Recording of historical data to a SQLite database.
- Integration with MS PowerBI and Tableau using OData and JSON.
- Lua Scripting language for the server environment. Javascript language for screen (client side) scripts.
- Automation functions can be implemented in LUA (e.g. substation restoration, tap control).
- Distinct treatment for digital states, alarms and protection events. Range check for analog values. Calculation of points.
- Dedicated Shell that can replace the standard Windows Shell and can restrict access only to the HMI functions.
- Excellent Inkscape-based graphics editor to create SVG graphics with SCADA animations.
- Use of Desktop Notifications for breaker opening by protection.
- No need for any browser plugin!

Software languages: english, ukranian and portuguese (BR). Can be easily translated to other languages (UTF-8 user interface).

Contact me here: https://www.linkedin.com/in/ricardo-olsen/

Follow my blog: http://ricolsen1supervc.wordpress.com

See configuration tips on Youtube channel:
https://www.youtube.com/channel/UCgpwj3TdP5jYrfr80_0eHYQ

Articles:
- Use cases: https://goo.gl/5fPSfp
- Installation: https://goo.gl/DRgBzG

Some graphics designed and obtained from Freepik https://www.freepik.com/

This is not a toy project! It's been actually used in dozens of substations up to 230kV level and in control centers.

Feel free to ask any question.
