<?php
// ----------------------------- PHP ----------------------------------------------------------------------
// Script para gerar um arquivo texto no formato sim_tr do SAGE para um dado período.
// Este mesmo arquivo serve para a IHM OSHMI.
// Parâmetros: nó, ano, mês, dia, hora inicial e hora final.
// O nó de supervisão define o conjunto de pontos que serão simulados.
// Para uma descrição do formato do arquivo Sim_tr, ver o guia: "Desenvolvimento de programas de aplicação para o sistema SAGE" 
// Versão 1.00
// RLO 08/2009

$print_id=1;
$CodNoh=1;
$horaini='00:00:00';
$horafim='23:59:59';
$dt=getdate();
$ano=$dt['year'];
$mes=$dt['mon'];
$dia=$dt['mday'];

$segundo=0;
$segundoant=0;

if (isset($_SERVER['argc']))
    {
    if ($_SERVER['argc']>1)
       $CodNoh = $_SERVER['argv'][1];
    if ($_SERVER['argc']>4)
       {
       $ano = $_SERVER['argv'][2];
       $mes = $_SERVER['argv'][3];
       $dia = $_SERVER['argv'][4];
       }
    if ($_SERVER['argc']>5)
      $horaini = $_SERVER['argv'][5];   
    if ($_SERVER['argc']>6)
      $horafim = $_SERVER['argv'][6];       
    }

// conexão com o banco   
$frel="../comum/peardb_login.php";
$finc=dirname($_SERVER['PHP_SELF'])."/".$frel;
if ( !file_exists($finc) )
   $finc=$frel;
require("$finc") ;

$frel="../comum/lesupsql.php";
$finc=dirname($_SERVER['PHP_SELF'])."/".$frel;
if ( !file_exists($finc) )
   $finc=$frel;
require("$finc") ;

$db=&DBLogin("SQL_BANCOTR_APLIC");
if (DB::isError($db)) 
    {
	    print("\nErro de conexão ao banco!\n");
	    return;
    }
    
printf("> LGO US01               ; NÓ:%d, DATA:%02d/%02d/%d, HORA_INICIAL:%s, HORA_FINAL:%s \n", $CodNoh, $dia, $mes, $ano, $horaini, $horafim);
printf("> INT MED US01\n");
printf("> INT DIG US01\n");

// vou buscar a lista de pontos do nó    

$query="
SELECT 
   i.nponto,
   i.id 
FROM
   id_ponto i 
   join id_ptlog_noh l on l.nponto=i.nponto and l.cod_nohsup=$CodNoh
order by 1    
";

$result =& $db->query($query);
if (!$result)
	{   
		die("\nErro na query!\n");
		return;
	}

$ident=array();
$pontos=array();
$valores=array();

while ( $result->fetchInto($rg, DB_FETCHMODE_ASSOC) )
  {
  $ident[(int)$rg["nponto"]]=$rg["id"];
  $pontos[]=(int)$rg["nponto"];
  $valores[(int)$rg["nponto"]]=999999999999;
  }

// vou intercalar dados de fotos minuto a minuto (sup) e as exceções do mysql

// preparação para pegar fotos com todos os pontos do nó
$consulta= new THistorico;

$month=sprintf("%02d", $mes);

// query para pegar eventos e dados analógicos do histórico mysql 
$query="
SELECT 
   i.nponto,
   i.id,
   TRUNCATE(s.estado,3) as valor, -- garante que a union vai dar float com 3 casas nesta coluna
   s.data,
   s.hora,
   s.msec,
   s.estado as flags,
   TIME_TO_SEC(hora) as segundo
FROM
   id_ponto i 
   join id_ptlog_noh l on l.nponto=i.nponto and l.cod_nohsup=$CodNoh 
   join sde s on s.id=l.nponto
WHERE
   data = '$ano/$mes/$dia' and
   hora >= '$horaini' and
   hora <= '$horafim'      

union

SELECT
   i.nponto,
   i.id,
   s.valor,
   s.data,
   s.hora,
   0 as msec,
   s.flags,
   TIME_TO_SEC(hora) as segundo
FROM
   id_ponto i
   join id_ptlog_noh l on l.nponto=i.nponto and l.cod_nohsup=$CodNoh
   join h{$ano}_{$month} s on s.nponto=l.nponto
WHERE
   data = '$ano/$mes/$dia' and
   hora >= '$horaini' and
   hora <= '$horafim'
   
ORDER BY data, hora, msec  
";

$result =& $db->query($query);
if (!$result)
	{   
		die("\nErro na query!\n");
		return;
	}

$hms=explode(":",$horaini);
$minini=$hms[0]*60+$hms[1];
$cntmin=$minini;
$hms=explode(":",$horafim);
$minfim=$hms[0]*60+$hms[1];

$nponto=0;
$id="";
$flags=0;
$valor=0;
if ( $result->fetchInto($rg, DB_FETCHMODE_ASSOC) )
  {
  }

for (;$cntmin<=$minfim ;$cntmin++)
  {
  $hr=sprintf("%02d:%02d:00",$cntmin/60, $cntmin%60);
  //echo "$cntmin\n";
  
  $segundo=$cntmin*60;
  $data_foto=$dia."/".$mes."/".$ano;
  $ret=$consulta->busca_foto_sup($data_foto, $hr);
    
  // para cada ponto, extrai conjunto de linhas
  while ( list($col_ponto,$linha) = each($ret) )
    {
    if ( in_array($col_ponto, $pontos) ) // testa se o ponto foi solicitado
      {   
      list($col_valor,$col_flags)=$linha;
      if ($col_flags&0x20)
          {
          $col_valor=number_format($col_valor, 3, '.', '');
          if ( $valores[(int)$col_ponto] !=  $col_valor) // só mostra se o ponto variou
            {
            $valores[(int)$col_ponto] =  $col_valor;
            printf("> VAR MED %-22s               VAL %9s  ; %5s -    %s -1\n", $ident[(int)$col_ponto], $col_valor, $col_ponto, $hr );
            }
          }
      else  
          {
          $col_valor=(int)!$col_valor;
          if ( $valores[(int)$col_ponto] !=  $col_valor) // só mostra se o ponto variou
            {
            $valores[(int)$col_ponto] =  $col_valor;
            printf("> VAR DIG %-22s               VAL %9d  ; %5d -    %s -1\n", $ident[(int)$col_ponto], $col_valor, $col_ponto, $hr );
            }
          }
      }
    }
  
  do 
    {
    $nponto= $rg["nponto"];
    $id= $rg["id"];
    $flags=$rg["flags"];
    $segundoant=$segundo;
    $segundo=$rg["segundo"];
    $valor=$rg["valor"];
    $houre=$rg["hora"];
    $ms=$rg["msec"];
    
    if ($segundo > $segundoant)
       printf("> ESP %-5d                                                   ; ---------- %s\n", $segundo-$segundoant, $rg["hora"] );
       
    if ( $segundo >= $cntmin*60 && $segundo < ($cntmin+1)*60)
       {
  
      if ($flags&0x20)
        {
        $valorana=number_format((float)$valor, 3, '.', '');
        if ( $valores[(int)$nponto] !=  $valorana) // só mostra se o ponto variou
          {
          $valores[(int)$nponto] =  $valorana;
          printf("> VAR MED %-22s               VAL %9s  ; %5d -    %s -1\n", $id, $valorana, $nponto, $houre);
          }
        }
      else
        {
        $valordig = ((~(int)$valor)&0x01);
        if ( $valores[(int)$nponto] !=  $valordig) // só mostra se o ponto variou
          {
          $valores[(int)$nponto] =  $valordig;
          printf("> VAR DIG %-22s               VAL %9d  ; %5d -    %s %03d\n", $id, $valordig, $nponto, $houre, $ms);
          }
        }
          
       if ($result->fetchInto($rg, DB_FETCHMODE_ASSOC))
         {
         }
       else
         break;     
       }
     else
       break; 
    
    } while (1);
  
  }  

printf("> LGN US01\n");
printf("> FIM\n");
?>