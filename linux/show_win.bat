start z:\bin\bash -c "/usr/bin/wmctrl -a %1; echo $? > /tmp/show_win_ret"
ping -n 1
set /p res=<z:\tmp\show_win_ret
exit /b %res%
