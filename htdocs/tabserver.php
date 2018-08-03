<?php 
// OSHMI/Open Substation HMI - Copyright 2008-2018 - Ricardo L. Olsen
header('content-type:text/plain');
header("Cache-Control: no-store, must-revalidate");

@$filtro = $_GET['F']; // parâmetro F : filtro do tabular

@$lstSE = $_GET['S']; // parâmetro S : filtro de subestação

@$callbackfn = $_GET['B']; // parâmetro B : nome da função de callback

@$pntinfo = $_GET['I']; // parâmetro P : ponto, tag ou lista de pontos

require_once("../comum/lesupsql.php"); 
require_once("../comum/peardb_login.php") ;
$db=&DBLogin("SQL_BANCOTR_CONSULTA");
if (DB::isError($db)) 
   die("Erro de conexão ao banco!");

// Vai pegar dados de tempo real

if ( $lstSE != "" )
  {
  $query =  "SELECT distinct e.estacao from id_ponto i
             join id_nops n on i.cod_nops=n.cod_nops
             join id_modulos m on m.cod_modulo=n.cod_modulo
             join id_estacao e on e.cod_estacao=m.cod_estacao
             join val_tr v on v.nponto=i.nponto ";
      
  $result = & $db->query($query);
  if (DB::isError($result))
    {
    print ("Data='';\n"); 
    die();   
    }

  print("LISTA_SES='");        

  while ($line = $result->fetchRow()) // percorre as linhas da query
    {
    list($col_se)=$line;
    printf("%s-", $col_se);        
    }
  print("'\n");        
  }

if ( $filtro != "" )
  {
  $filtro = str_replace( "*", "%", $filtro );    
  $query =  "SELECT i.nponto, 
                    i.id, 
                    e.estacao, 
                    i.traducao_id, 
                    tpnt.pres_0, 
                    tpnt.pres_1, 
                    v.valor, 
                    v.flags, 
                    tpnt.tipo, 
                    i.cod_origem, 
                    tpnt.cmd_1, 
                    tpnt.cmd_0
             from id_ponto i
             join id_nops n on i.cod_nops=n.cod_nops
             join id_modulos m on m.cod_modulo=n.cod_modulo
             join id_estacao e on e.cod_estacao=m.cod_estacao
             join val_tr v on i.nponto=v.nponto 
             join id_tipos tp on i.cod_tpeq=tp.cod_tpeq and i.cod_info=tp.cod_info
             join id_tipopnt as tpnt on tpnt.cod_tipopnt=tp.cod_tipopnt
             where i.id like '$filtro%' 
             order by nponto";
      
  $result = & $db->query($query);
  if (DB::isError($result))
    {
    print ("Data='';\n"); 
    die();   
    }

  $DataConsulta = date("d/m/Y H:i:s");
  print("Data='$DataConsulta';\n");
  print("L=[];\n");
  $cnt=0;
  while ($line = $result->fetchRow()) // percorre as linhas da query
    {
    list( $col_nponto, $col_id, $col_se, $col_descr, $col_estoff, $col_eston, $col_valor, $col_flags, $col_tipo, $col_origem, $cod_cmdon, $col_cmdoff ) = $line;

    $col_descr = str_replace( ",", ".", $col_descr );
    $col_flags = (int) $col_flags;
    
    if ( $col_tipo == "D" )
      {
      if ( ($col_flags & 0x01) == 0x00 )
        $estval = $col_eston;
      if ( ($col_flags & 0x01) == 0x01 )
        $estval = $col_estoff;
      }
    else
      {
      $estval = sprintf("%0.3f", $col_valor);
      }  

    $Qlf = "";
    if ( $col_flags & 0x80 )
      $Qlf=$Qlf."F";

    if ( $col_origem == 1 )
      $Qlf=$Qlf."C";

    if ( $col_origem == 6 )
      $Qlf=$Qlf."M";

/*      
    if ( $col_origem == 7 )
      {
      $estval = $col_cmdoff . "/" . $col_cmdon;          
      }
*/
    $flags = $col_flags;  
    $sup_cmd = 0;
    $estnorm = 0;
    $taganorm = '';
    printf("L[$cnt]='%d,%s,%s,%s,%s,%d,%d,%s,%d,%s';\n", $col_nponto, $col_id, $col_se, $col_descr, $estval, $flags, $sup_cmd, $Qlf, $estnorm, $tagnorm );        
    $cnt++;
    }
  print("\n");        
  }

if ( $callbackfn != '' )
 print("$callbackfn();\n");

$db->disconnect();   // libera conexão ao banco
?>


