# OSHMI Linux autostart

cd ~/.wine/drive_c/oshmi/

# erases chromium cache 
rm -rf ~/.cache/chromium/Default/Cache/*.*

# removes stuff that are not needed to run on linux
rm -rf ~/.wine/drive_c/oshmi/bd/*.bat
rm -rf ~/.wine/drive_c/oshmi/bin/mon_proc.exe
rm -rf ~/.wine/drive_c/oshmi/nginx_php

# SQL data processing
sh ~/.wine/drive_c/oshmi/db/process_dumpdb.sh &
sh ~/.wine/drive_c/oshmi/db/process_soe.sh &
sh ~/.wine/drive_c/oshmi/db/process_hist.sh &

# OSHMI shell
wine ~/.wine/drive_c/oshmi/bin/hmishell.exe &

cd ~/.wine/drive_c/oshmi/bin

# OSHMI realtime database server
wine webserver.exe &

./QTester104 &

# beep iin case wine sound doesn't work
sh ~/.wine/drive_c/oshmi/linux/beep.sh &

# OSHMI iec104 protocol driver via Wine
#wine ~/.wine/drive_c/oshmi/bin/qtester104.exe &

# screens editor Inkscape SAGE
# wine " ~/.wine/drive_c/Program Files (x86)/Inkscape_sage/inkscape.exe" &

# OSHMI pdf manuals
# mupdf ~/.wine/drive_c/oshmi/docs/oshmi_config_manual.pdf &
# mupdf ~/.wine/drive_c/oshmi/docs/oshmi_operation_manual.pdf &

# console for the Wine environment
# wineconsole cmd &


