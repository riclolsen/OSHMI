start z:\bin\bash -c "/usr/bin/xwininfo -root -tree  | /bin/grep -c -m 1 -q -s -i %1 ; ret=$? && %2 || :; echo $ret > /tmp/find_win_ret"
ping -n 1
set /p res=<z:\tmp\find_win_ret
exit /b %res%
