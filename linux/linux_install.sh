#!/bin/sh

# OSHMI modifications necessary to run on linux, please run after OSHMI install or updates.
# Please issue commands one by one in the following order paying attention to possible errors.
# Most actions require an active internet connection.

cd ~/.wine/drive_c/oshmi/linux/

# allow install of wine 1.6 and up
sudo add-apt-repository ppa:ubuntu-wine/ppa 

# no need for apache, port 80 must be free
sudo apt-get remove apache2*

# install required packages
sudo apt-get update
sudo apt-get install xinit xfce4 slim chromium-browser nginx php5 php5-cgi php5-fpm php5-sqlite php5-gd default-jre wmctrl libcap2-bin xterm mupdf zenity geany software-properties-common x11-utils wget sqlite3 nmap ttf-mscorefonts-installer lsof
sudo apt-get install wine1.6 winetricks
# update all packages
sudo apt-get dist-upgrade 

# get and install inkscape sage (screen editor)
wget "http://sourceforge.net/projects/sage-scada/files/SAGE v4.07/inkscape0484_sage407.exe"
wine inkscape0484_sage407.exe

# allow "Wine" to open resctricted ports and set time
# need to re-run this every time "Wine" is updated
sudo setcap cap_sys_time,cap_net_admin,cap_net_bind_service+ep /usr/bin/wine-preloader

# removes stuff that are not needed to run on linux
rm -rf ../db/*.bat
rm -rf ../bin/mon_proc.exe
rm -rf ../nginx_php

sudo cp process_dumpdb.sh ../db/
sudo cp process_soe.sh ../db/
sudo cp process_hist.sh ../db/

sudo cp nginx.conf /etc/nginx/
sudo cp php.ini /etc/php5/fpm/
sudo cp www.conf /etc/php5/fpm/pool.d/
sudo cp xorg.conf /etc/X11/

sudo service nginx restart
sudo service php5-fpm restart

# makes autostart executable
chmod +x autostart.sh

# makes GlipsGraffiti JAR files executable
#chmod +x ../GlipsGraffiti/*.jar

# adds user www-data to group oshmi
sudo usermod -a -G oshmi www-data

# detect usb 3g modem
# sudo modprobe option 
# route for usb 3g modem
# sudo route add default gw <IP>

# Additional fonts install (optional)
 
# install additional fonts for wine (no sudo)
# winetricks consolas corefonts lucida tahoma

# use the windows fonts in lunux too (with sudo)
# sudo cp ~/.wine/drive_c/windows/Fonts/*.* /usr/share/fonts/truetype/msttcorefonts/

# list ports opened (debug)
# nmap 127.0.0.1 -p1-65535
#
# expected results:
# Starting Nmap 6.00 ( http://nmap.org ) at 2013-02-27 14:01 BRT
# mass_dns: warning: Unable to determine any DNS servers. Reverse DNS is disabled. Try using --system-dns or specify valid servers with --dns-servers
# Nmap scan report for localhost (127.0.0.1)
# Host is up (0.00013s latency).
# Not shown: 65532 closed ports
# PORT      STATE SERVICE
# 9000/tcp  open  cslistener
# 51908/tcp open  http
# 51909/tcp open  unknown
# Nmap done: 1 IP address (1 host up) scanned in 0.82 seconds

# list ports opened by services (debug)
# sudo lsof -i -P
#
# expected results:
# nginx      841 www-data    6u  IPv4   8363      0t0  TCP *:51909 (LISTEN)
# nginx      842 www-data    6u  IPv4   8363      0t0  TCP *:51909 (LISTEN)
# nginx      843 www-data    6u  IPv4   8363      0t0  TCP *:51909 (LISTEN)
# php5-fpm   853     root    6u  IPv4   7427      0t0  TCP localhost:9000 (LISTEN)
# php5-fpm   854 www-data    0u  IPv4   7427      0t0  TCP localhost:9000 (LISTEN)
# php5-fpm   855 www-data    0u  IPv4   7427      0t0  TCP localhost:9000 (LISTEN)
# webserver 1111 operador    6u  IPv4  10257      0t0  UDP *:8082 
# webserver 1111 operador   16u  IPv4   9409      0t0  UDP *:8099 
# webserver 1111 operador   17u  IPv4   9413      0t0  UDP *:65280 
# webserver 1111 operador   21u  IPv4   9411      0t0  TCP *:51908 (LISTEN)
# webserver 1111 operador   24u  IPv4   9413      0t0  UDP *:65280 
# webserver 1111 operador   25u  IPv4   9411      0t0  TCP *:51908 (LISTEN)
# wineserve 1125 operador   29u  IPv4  10257      0t0  UDP *:8082 
# wineserve 1125 operador  119u  IPv4   9409      0t0  UDP *:8099 
# wineserve 1125 operador  121u  IPv4   9411      0t0  TCP *:51908 (LISTEN)
# wineserve 1125 operador  123u  IPv4   9413      0t0  UDP *:65280 

# erases chromium cache 
# rm -rf ~/.cache/chromium/Default/Cache/*.*
