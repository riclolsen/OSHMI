-- script.lua

-- HMI functions available:
-- Funções do IHM disponíveis para uso:
--
-- hmi_print : imprime valores no diálogo de scripts
--             argumentos: valores separados por vírgula
--             print values on script dialog
--             args: comma separated strings
--
-- hmi_isprimary : retorna true se o ihm está no estado primário, false se secundário
--                 returns true if hmi is in primary state
--
-- hmi_settimer: ajuste o tempo de execução da função ScriptCycle
--               argumento: tempo em ms
--               adjust cycle time for function ScriptCycle
--               args: time in ms
--
-- hmi_activatebeep: liga o beep de alarme sonoro
--                   beeps the alarm sound
--
-- hmi_getpoint: busca informações do ponto
--               argumento: número do ponto
--               retorna: valor analógico (float) ou digital duplo (0,1,2 ou 3)
--                        estado de falha (1=falhado, 0=normal)
--                        tipo do ponto (1=analógico, 0=digital) 
--                        substituído? (1=sim, 0=não)
--                        comando bloqueado? (1=sim, 0=não)
--               obtains point info
--               args: point number
--               returns: analog value (float) or digital double (0,1=OFF,2=ON,3)
--                        fail state (1=failed, 0=normal) 
--                        point type (1=analog, 0=digital) 
--                        substituted? (1=yes, 0=no)
--                        command blocked? (1=yes, 0=no)
--
-- hmi_writepoint: escreve no ponto
--                 argumentos: número do ponto, valor e estado de falha
--                 retorna: 0 se ok, 1 se não encontrou o ponto
--                 writes on point
--                 args: point number, value, fail state
--                 returns: 0 if ok, 1 if point not found
--
-- hmi_blkpoint: bloqueia comando do ponto supervisionado     
--               argumentos: número do ponto e mensagem de bloqueio (anotação)
--               retorna: 0 se ok, 1 se já estava anotado, 2 se não achou o ponto
--               blocks command of supervised point
--               args: point number, description message (annotation)
--               returns: 0 if ok, 1 if point already annotated, 2 if point not found
--
-- hmi_unblkpoint: desbloqueia o comando do ponto supervisionado, somente se a mensagem de anotação for igual
--                 argumentos: nponto e mensagem de anotação
--                 retorna: 0 se ok, 1 se já estava desbloqueado, 2 se não achou o ponto, 3 se a mensagem não for igual
--                 unblocks command of supervised point, only if same annotation message
--                 args: point number,description message (annotation)
--                 returns: 0 if ok, 1 if point already annotated, 2 if point not found
--    
-- hmi_sendcmd: envio de comando 
--              argumentos: número de ponto de comando, valor
--              retorna: 0 se ok, 1 se, 3 se ponto não encontrado, 4 se bloqueado, nil se erro na quantidade de argumentos
--              sends command
--              args: command point number, value
--              returns: 0 if ok, 3 if point not found, 4 if blocked, nil if wrong arg number
--
-- hmi_getnpt:  consulta número do ponto
--              argumento: tag do ponto
--              retorna: 0 se não encontrou, ou número do ponto 
--              query point number by tag
--              argument: point tag
--              retorna: 0 if not found, or point number


-- Exemplo de script Lua para fazer bloqueio de comando das seccionadoras 
-- Example Lua script, blocks command on switch interlock

-- ajusta para rodar a cada 5 segundos 
-- adjusts times for 5 seconds
hmi_settimer( 5000 )

-- mensagem de inicializando
-- initialization message
hmi_print( "Inicializando script.lua!" )

-- Valores para os estados de DJ e SC
-- Switch IEC digital double values
CLOSED = 2
OPEN = 1
TRANSIT = 0 
INVALID = 3

-- Mensagem para a anotação de bloqueio das seccionadoras
-- Block message
msg_intertrav = "Bloqueio automatico por intertravamento"

-- Números dos pontos de supervisão dos estados de DJ e SC
-- Point numbers of breaker and switches
pts52_1 = 16249
pts89_4 = 16250
pts89_6 = 16252
pts89_8 = 16251

-- Começa considerando estado indeterminidado para o DJ
-- Begins with invalid state
v52_1 = INVALID
va52_1 = INVALID

-- Atualiza os valores dos pontos supervisionados
-- get real time values
function GetValues()

  va52_1 = v52_1
  v52_1, falha, tipo, subst = hmi_getpoint(pts52_1)

end  

-- Função principal, chamada automaticamente a cada periodo de timer 
-- Main function, called at each cycle period 
function ScriptCycle() 

  GetValues()

  if va52_1 ~= v52_1 then
 
    if  v52_1 == CLOSED then
      hmi_blkpoint( pts89_4,  msg_intertrav ) 
      hmi_blkpoint( pts89_6,  msg_intertrav ) 
      hmi_print("Blocking")
    end
    
    if  v52_1 == OPEN then
      hmi_unblkpoint( pts89_4, msg_intertrav ) 
      hmi_unblkpoint( pts89_6, msg_intertrav ) 
      hmi_print("Unblocking")
    end
    
  end
  
end

-- Intercepta comandos da IHM (callback), argumentos número do ponto e valor (1=ON, 0=OFF para digitais ou valor analógico), retornar 0 para continuar a execução do comando e 1 para não continuar ao protocolo
-- Callback to intercept HMI commands, args: point number and value (1=ON, 0=OFF or analog value), must return 0 to continue command execution or 1 to stop (will not proceed to protocol level)
function InterceptCommands( pointnumber, commandvalue ) 

  -- if ( pointnumber == 12345 and commandvalue == 1 ) then
  -- do something
  -- end
  
  return 0
end
