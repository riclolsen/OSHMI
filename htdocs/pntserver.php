<?php 
// OSHMI/Open Substation HMI - Copyright 2008-2014 - Ricardo L. Olsen

header('content-type:text/plain');

@$callbackfn = $_GET['B']; // parâmetro B : nome da função de callback

@$mstatus = $_GET['M']; // parâmetro M : requisição de status
if ( $mstatus == 1 ) 
  {
  print("HA_ALARMES=0;\n");
  print("NUM_VAR=0;\n\n");      
  if ( $callbackfn != '' )
    print("$callbackfn();\n");
  die();  
  }  

$DataConsulta = date("d/m/Y H:i:s");
printf("Data='$DataConsulta';");

$dd=0;$mm=0;$yy=0;
$pontos=array();
$ident=array();
$descr=array();
$unidade=array();
$lista_datas=array();
$lstdt=array();

@$pntinfo = $_GET['I']; // parâmetro P : ponto, tag ou lista de pontos

$par = $_GET['P']; // parâmetro P : ponto, tag ou lista de pontos
$lista_pnt=explode(',',$par);

if ($pntinfo!="")
  array_push($lista_pnt, $pntinfo);  

$lst='';
foreach($lista_pnt as $pnt)
  {
  $pnt=trim($pnt);
  if ($pnt!=0 && $pnt!='')
    $pontos[]=$pnt;
  $lst=$lst."'".$pnt."',";
  }
  
require_once("../comum/lesupsql.php"); 
require_once("../comum/peardb_login.php") ;
$db=&DBLogin("SQL_BANCOTR_CONSULTA");
if (DB::isError($db)) 
   die("Erro de conexão ao banco!");

// Vai pegar dados de tempo real

$lstqry='';
if ($lst!="" && $lst!="'',")
  $lstqry=" and (i.nponto in ($lst 0) or i.id in ($lst 'xxxxxx')) and i.nponto>0 ";

$query = "SELECT i.nponto, v.valor, v.flags, v.data, v.hora, i.id, i.traducao_id,  
           case i.cod_origem when 7 then
             if (p.cod_tipopnt!=0, concat(pt_ocr.cmd_0, '/' , pt_ocr.cmd_1), concat(t.cmd_0, '/' , t.cmd_1))
           else  
           case t.tipo when 'D' then
             if (p.cod_tipopnt!=0, concat(pt_ocr.pres_0, '/' , pt_ocr.pres_1), concat(t.pres_0, '/' , t.pres_1))
             else
               concat( t.unidade )
             end   
             end as descr_tipo,
          i.cod_origem, 
          t.tipo as tipo
          , TRIM(TRAILING '-' FROM SUBSTRING(i.id, 1, 4))
          from id_ponto i 
          join val_tr v on v.nponto=i.nponto
          join id_prot p on i.cod_prot=p.cod_prot
          join id_tipopnt as pt_ocr on p.cod_tipopnt=pt_ocr.cod_tipopnt
          join id_tipos as tp on tp.cod_tpeq=i.cod_tpeq and tp.cod_info=i.cod_info
          join id_tipopnt as t on t.cod_tipopnt=tp.cod_tipopnt           
          where i.cod_tpeq!=95 
          $lstqry 
          order by tipo, nponto";

//die($query);
$result = & $db->query($query);
if (DB::isError($result))
  {
  print ("Data='';\n"); 
  die();   
  }

while ($line = $result->fetchRow()) // percorre as linhas da query
 {
 list($col_nponto,$col_valor,$col_flags, $col_data, $col_hora, $col_id, $col_descr, $col_unid, $col_orige, $col_tipo, $col_estacao)=$line;

 if ($col_tipo=='D')
 {
 if ( $col_flags&0x01 )
   {
   $col_flags= $col_flags & 0xFC;
   $col_flags= $col_flags | 0x01;
   }
 else
   {
   $col_flags= $col_flags & 0xFC;
   $col_flags= $col_flags | 0x02;
   }  
 } 
 
 $col_valor=number_format($col_valor, 2, '.', '');
 
  
 if ($pntinfo!="")
   {
   printf("NPTO='%d';\n", $col_nponto);
   printf("ID='%s';\n", $col_id);
   printf("DESC='%s';\n", $col_descr);
   $estados=explode('/', $col_unid);
   printf("ST_ON='%s';\n",$estados[1]);
   printf("ST_OFF='%s';\n",$estados[0]);
   printf("CNPTO=0;\n");
   printf("ANOT='';\n");
   printf("ALRIN=0;\n");
   printf("VLNOR=%f;\n", $col_valor);
   printf("ESTALM=0;\n");
   printf("LIMI=-999999.9;\n");
   printf("LIMS=999999.9;\n");
   printf("HISTER=0;\n");
   printf("UNIDADE='%s';\n", $col_unid);
   printf("ESTACAO='%s';\n",  $col_estacao);
   }
 else   
   {
   printf("V[%d]=%s;F[%d]=%d;T[%d]=''; \n", $col_nponto, $col_valor, $col_nponto, $col_flags, $col_nponto );
   }
   
 }
   
if ($pntinfo=="")
{
print("HA_ALARMES=0;\n");
printf("Sha1Ana='%s';\n", date("d/m/Y H:i:s"));
printf("Sha1Dig='%s';\n", date("d/m/Y H:i:s")); 
}

if ( $callbackfn != '' )
 print("$callbackfn();\n");

$db->disconnect();   // libera conexão ao banco
?>


