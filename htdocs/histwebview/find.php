<?php

// ----------------------------------------------------------------------------
// Script para implementar um 'SUGEST AS YOU TYPE' 
// busca sugestão para o TAG do ponto na medida em que se digita
// Monta um select em HTML com as opções encontradas para o parâmetro 'q'
// OSHMI/Open Substation HMI - Copyright 2008-2014 - Ricardo L. Olsen

header("Content-Type: text/html; charset=ISO-8859-1");

try 
  {
  $db = new PDO( 'sqlite:../../db/dumpdb.sl3' );
  $db->exec ( "PRAGMA synchronous = NORMAL" );
  }
catch( PDOException $e )
  {
  die("Erro!");
  }
  
$q = $_GET["q"];

$q = trim($q);

if ( ((int)$q)!=0 ) // se for numero de ponto não mostra nada
  die();
  
$q = str_replace ( '*', '%', $q ); // caractere coringa
$q = str_replace ( '?', '%', $q ); // caractere coringa
$q = str_replace ( ' ', '%', $q ); // caractere coringa

$qspl = explode('%', $q);
$qmult = '';
if (count($qspl)>1)
  $qmult=" (traducao_id like '%{$qspl[1]}%' and (i.estacao like '%{$qspl[0]}%'  or estdescr like '%{$qspl[0]}%') ) ";
else  
  $qmult=" traducao_id like '%$q%' ";
  
if (isset($_GET["inp"]))
  $inputname = $_GET["inp"];
else
  $inputname = "";
$limit = 500;

if ($q) 
{
   $query = 
   " SELECT 
       i.nponto, 
       i.id, 
       i.descricao as TRADUCAO_ID,
       i.estacao,
       i.estacao as ESTDESCR
     FROM 
       dumpdb i 
     WHERE 
       i.estalm!=3 and
       i.id != '$q' and
       (
        i.id like '$q%' or
        $qmult
       ) 
     ORDER BY
       i.estacao,
       i.id 
     LIMIT 
       $limit
     ";

   $results = array();
   
   $cnt=0; $cntopt=0;
   $estant='';
   foreach ( $db->query( $query ) as $row )
   {
      $nponto = $row["NPONTO"];
      $id = $row["ID"];
      $trad = $row["TRADUCAO_ID"];
      $est = $row["ESTACAO"];
      $estdescr = $row["ESTDESCR"];
      
      if ($estant!=$est)
        {
        $estant=$est;
        if ($cnt!=0)
          $results[]= "</optgroup>";
        $results[]= "<optgroup label='$est - $estdescr' style='font-family: courier new;'>";          
        $cntopt++;
        }
      
      if ($cnt==0) 
        $sel='selected'; 
      else 
        $sel=''; 

      $itm = sprintf ("%5d: %-22s %-42s",$nponto,$id,$trad);
      $itm = str_replace ( ' ', '&nbsp;', $itm );
      $itm = sprintf ("<option value='%s'>%s</option>", $id, $itm);

      $results[] =  $itm;
      $cnt++;
   }

   $results[]= "</optgroup>";
   
   if ($cnt>0)
   {
   $size=1+$cnt+$cntopt;
   if ($size>15) $size=15;
   echo "<small><select size=$size onchange=document.getElementById(\"$inputname\").value=document.getElementById(\"{$inputname}_sel\").value; onblur=document.getElementById(\"{$inputname}_sel\").style.display=\"none\" id='{$inputname}_sel' name='{$inputname}_sel' style='font-family: courier new;'>";
   echo "<option selected value=''>Choose point</option>";
   echo implode("\n", $results);
   echo "</select></small>";
   }
}

?> 
