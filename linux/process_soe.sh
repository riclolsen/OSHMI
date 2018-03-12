#!/bin/sh

# process sql files in a sqlite database then removes the files

cd ~/.wine/drive_c/oshmi/db/
   
# exit when flock fails
set -e 

(
# locks to avoid multiples of this process running
flock -x -w 10 9

# avoids exit in case of errors
# evita sair em caso de erros
set +e 

while [ 1 ]; do

# pega todos os arquivos sql de eventos
# gets all sql event files
  for file in soe_*.sql; do
  if [ "$file" != "soe_*.sql" ]; then
  
# process sql file into the database
# processa o arquivo no sqlite
    res=`/usr/bin/sqlite3 -init pragmas_soe.sql soe.sl3 < "$file" `

    if [ "$?" = "0" ]; then

# if ok, deletes the sql file
# se não deu erro, apaga o arquivo
      rm -f "$file"
    
    else

# error detected: retry
# quando der erro, faz nova tentativa
      sleep 2
      res=`/usr/bin/sqlite3 -init pragmas_soe.sql soe.sl3 < "$file" `
      rm -f "$file"
    fi  
  
  fi
  done 

  sleep 1
  echo "$file"
  
done

) 9>/var/lock/process_soe.exclusivelock
