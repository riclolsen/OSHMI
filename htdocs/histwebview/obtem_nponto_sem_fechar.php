<!DOCTYPE html>
<html>
<head>
<title>AT-DOES-DSC</title>
<script src="../../flot/jquery.js"></script>
<SCRIPT>

<?php

// OSHMI/Open Substation HMI - Copyright 2008-2014 - Ricardo L. Olsen

header("Content-Type: text/html; charset=ISO-8859-1");

// acerta variáveis globais para várias versões do PHP
extract($_REQUEST, EXTR_PREFIX_ALL|EXTR_REFS|EXTR_SKIP, 'p');

if (!isset($p_Adiciona))
  $p_Adiciona=0;

if ($p_Adiciona==1)
  print "var Adiciona=1;";
else
  print "var Adiciona=0;";

?>

function RetornaPonto(nponto, controlk)
{ 
if ( window.opener.closed )
  window.close();

if (Adiciona)
  {
  var virg='';
  if (window.opener.objeto.value!='')
    virg=',';
  window.opener.objeto.value=window.opener.objeto.value+virg+nponto;
  }
else
  {
  if ( controlk )
    {
    window.opener.objeto.value = nponto;
    window.close();
    }
  else
    {
    if ( window.opener.f1.NPONTO_1.value == "" )
      window.opener.f1.NPONTO_1.value = nponto;
    else
    if ( window.opener.f1.NPONTO_2.value == "" )
      window.opener.f1.NPONTO_2.value = nponto;
    else
    if ( window.opener.f1.NPONTO_3.value == "" )
      window.opener.f1.NPONTO_3.value = nponto;
    else
    if ( window.opener.f1.NPONTO_4.value == "" )
      window.opener.f1.NPONTO_4.value = nponto;
    else
    if ( window.opener.f1.NPONTO_5.value == "" )
      window.opener.f1.NPONTO_5.value = nponto;
    else
    if ( window.opener.f1.NPONTO_6.value == "" )
      {
      window.opener.f1.NPONTO_6.value = nponto;
      window.close();
      }
    }
  }
}

</SCRIPT>
</head>
<body style="font-family:sans-serif;" >
<strong>Obtém número do ponto</strong>
<small>
<P>Use [CLICK] para escolher vários pontos, preenchendo os campos vazios sem fechar esta janela.
<br>Use [CONTROL]+[CLICK] para preencher o valor de um ponto e fechar esta janela.</P>
</small>
<?php

if (!isset($p_tpPto))
  $p_tpPto="";
if (!isset($p_Tipos))
  $p_Tipos="";  
if (!isset($p_Estacao))
  $p_Estacao="";
if (!isset($p_ForaHist))
  $p_ForaHist=0;
if (!isset($p_RetNPonto))
  $p_RetNPonto=0;

try 
  {
  $db = new PDO( 'sqlite:../../db/dumpdb.sl3' );
  $db->exec ( "PRAGMA synchronous = NORMAL" );
  }
catch( PDOException $e )
  {
  die("Erro!");
  }

$Filtro="";   
if ($p_tpPto=='D')
  $Filtro=" and (tpnt.tipo='D') "; 

if ($p_tpPto=='A')
  $Filtro=$Filtro." and (tpnt.tipo='A') "; 
  
if ($p_Tipos)
  $Filtro=$Filtro." and (i.cod_tpeq in ( $p_Tipos )) "; 

$query = "SELECT distinct i.estacao as ESTACAO, i.estacao as DESCRICAO from dumpdb as i
           where i.estalm!=3 and i.estacao!='' order by i.estacao";

print '<FORM METHOD="post">';
echo 'Estação:&nbsp;&nbsp;<select name=Estacao onChange="this.form.submit()">';
print "<option selected VALUE=''> Escolha uma estação!</option>";
foreach ( $db->query( $query ) as $line )
  {
        print "<option ";
        if (strcmp($p_Estacao,$line["ESTACAO"])==0)
          echo 'selected ';
        print "VALUE='{$line["ESTACAO"]}'>{$line["ESTACAO"]} - {$line["DESCRICAO"]}</option>\n";
        each($line);
  }
 
echo '</select>';
print '</FORM>';

if($p_Estacao=="")
   die("</body></html>");

$query = "SELECT i.nponto, i.id, i.descricao as TRADUCAO_ID from dumpdb as i where i.estalm!=3 and i.estacao='$p_Estacao' $Filtro order by i.estacao, i.id, i.nponto";
print "<pre>";
foreach ( $db->query( $query ) as $line )
  {
  if ($p_RetNPonto)
    printf ("<A HREF='javascript:;' onClick='RetornaPonto(\"%s\",window.event.ctrlKey);'>%05d</A> <A HREF='javascript:;' onClick='RetornaPonto(\"%s\",window.event.ctrlKey);'>%-22s</A> %s <BR>", $line["NPONTO"], $line["NPONTO"], $line["NPONTO"], $line["ID"], $line["TRADUCAO_ID"]);
  else   
    printf ("<A HREF='javascript:;' onClick='RetornaPonto(\"%s\",window.event.ctrlKey);'>%05d</A> <A HREF='javascript:;' onClick='RetornaPonto(\"%s\",window.event.ctrlKey);'>%-22s</A> %s <BR>", $line["NPONTO"], $line["NPONTO"], $line["ID"], $line["ID"], $line["TRADUCAO_ID"]);
  }
print "</pre>";

?>
</body></html>
