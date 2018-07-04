#!/bin/sh

while : ; do 

NUMBEEP="$(/usr/bin/curl -s http://127.0.0.1:51909/htdocs/shellapi.rjs?P=99999 |grep beep|cut -d: -f2|cut -b2)"
echo $NUMBEEP

if [ $NUMBEEP = '1' ]
then 
  /usr/bin/aplay -q -D default ~/oshmi/linux/receive.wav
fi

if [ $NUMBEEP = '2' ]
then 
  /usr/bin/aplay -q -D default ~/oshmi/linux/alert.wav
fi

if [ $NUMBEEP = '1' ]
then 
  sleep 2
else
  sleep 1
fi

done

