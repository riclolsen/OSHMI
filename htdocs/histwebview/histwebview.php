<?php
    header("Content-Type: text/html; charset=UTF-8");
?>

<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8"/>
<meta name="application-name" content="OSHMI-Open Substation HMI"/>
<meta name="description" content="HistWebView"/>
<meta name="author" content="Ricardo L.Olsen"/>
<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1"/>    
<link rel=stylesheet href="estilo.css" type="text/css"/>
<link rel="stylesheet" href="../lib/ui-1.8.20/themes/base/jquery.ui.all.css"/>
<script src="../../i18n/messages_i18n.js"></script>
<script src="../images.js"></script>
<script src="../util.js"></script>
<!--[if IE]><script src="../lib/flot-0.7/excanvas.js"></script><![endif]-->
<script src="../lib/jquery-1.5.1.js"></script>
<script src="../lib/flot-0.7/jquery.flot.js"></script>
<script src="../lib/flot-0.7/jquery.flot.crosshair.js"></script>
<script src="../lib/flot-0.7/jquery.flot.selection.js"></script>
<script src="../lib/flot-0.7/jquery.flot.navigate.js"></script>
<script src="../lib/ui-1.8.20/jquery.ui.core.js"></script>
<script src="../lib/ui-1.8.20/jquery.ui.datepicker.js"></script>
<script src="../lib/ui-1.8.20/i18n/jquery.ui.datepicker-pt-br.js"></script>
<script>

// ----------------------------------------------------------------------------
// HistWebView- Curves Viewer
// Consulta aos históricos
// OSHMI/Open Substation HMI - Copyright 2008-2018 - Ricardo L. Olsen

$(document).ready(function() { 

  // desabilita o botão direito 
  document.oncontextmenu = function() {return false;};

  document.title = "."; 
  document.title = Msg.NomeVisorCurvas + " - " + Msg.NomeProduto + " - " + Msg.VersaoProduto;
  
  LoadFavicon( Imgs.FAVICONCURVAS );
  
  $( "#DATA_1" ).datepicker(); 
  $( "#DATA_1" ).datepicker( "option", "yearRange", "1997" );
  $( "#DATA_1" ).datepicker( "option", "changeYear", true );
  
  $( "#DATA_2" ).datepicker();
  $( "#DATA_2" ).datepicker( "option", "yearRange", "1997" );
  $( "#DATA_2" ).datepicker( "option", "changeYear", true );

  $( "#DATA_3" ).datepicker();
  $( "#DATA_3" ).datepicker( "option", "yearRange", "1997" );
  $( "#DATA_3" ).datepicker( "option", "changeYear", true );

  $( "#DATA_4" ).datepicker();
  $( "#DATA_4" ).datepicker( "option", "yearRange", "1997" );
  $( "#DATA_4" ).datepicker( "option", "changeYear", true );

  $( "#DATA_5" ).datepicker();
  $( "#DATA_5" ).datepicker( "option", "yearRange", "1997" );
  $( "#DATA_5" ).datepicker( "option", "changeYear", true );

  $( "#DATA_6" ).datepicker();
  $( "#DATA_6" ).datepicker( "option", "yearRange", "1997" );
  $( "#DATA_6" ).datepicker( "option", "changeYear", true );
});

function ObtemPontoSemFechar(objeto)
{
 url="./obtem_nponto_sem_fechar.php";
 janela="_blank";
 window.objeto=objeto;
 window.open(url,janela,"toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=no,top=20,left=20,screenY=500,screenX=680,width=600,height=350");
}

// Script para implementar um 'SUGEST AS YOU TYPE' 
// busca sugestão para o TAG do ponto na medida em que se digita

var req = null;
var inp = null;

function isInt(myNum) {
         // get the modulus: if it's 0, then it's an integer
         var myMod = myNum % 1;

         if (myMod == 0) {
                 return true;
         } else {
                 return false;
         }
}

function findPosX(obj)
{
	var curleft = 0;
	if (obj.offsetParent)
	{
		while (obj.offsetParent)
		{
			curleft += obj.offsetLeft
			obj = obj.offsetParent;
		}
	}
	else if (obj.x)
		curleft += obj.x;
	return curleft;
}

function findPosY(obj)
{
	var curtop = 0;
	if (obj.offsetParent)
	{
		while (obj.offsetParent)
		{
			curtop += obj.offsetTop
			obj = obj.offsetParent;
		}
	}
	else if (obj.y)
		curtop += obj.y;
	return curtop;
}

function loadXMLDoc(inputn) {

   inp=inputn; 
   
   if (isInt(inp))
     return;
   
   url='find.php?q='+inp.value+'&inp='+inp.id;
    
   // Internet Explorer
   try { req = new ActiveXObject("Msxml2.XMLHTTP"); }
   catch(e) {
      try { req = new ActiveXObject("Microsoft.XMLHTTP"); }
      catch(oc) { req = null; }
   }

   // Mozailla/Safari
   if (req == null && typeof XMLHttpRequest != "undefined") {
      req = new XMLHttpRequest();
   }
   // Call the processChange() function when the page has loaded
   if (req != null) {
      req.onreadystatechange = processChange;
      req.open("GET", url, true);
      req.send(null);
   }
}

function processChange(evt) {
   // The page has loaded and the HTTP status code is 200 OK
   if (req.readyState == 4) {
      if (req.status == 200) {
      // Write the contents of this URL to the searchResult layer
      getObject("lista_op").style.left = findPosX(inp) + "px"; 
      getObject("lista_op").style.top = (findPosY(inp) + inp.offsetHeight) + "px";
      getObject("lista_op").innerHTML = req.responseText;
      }
   }
}

function getObject(name) {
   return document.getElementById(name);
}

function createCookie(name,value,days) {
	if (days) {
		var date = new Date();
		date.setTime(date.getTime()+(days*24*60*60*1000));
		var expires = "; expires="+date.toGMTString();
	}
	else var expires = "";
	document.cookie = name+"="+value+expires+"; path=/";
}

function readCookie(name) {
	var nameEQ = name + "=";
	var ca = document.cookie.split(';');
	for(var i=0;i < ca.length;i++) {
		var c = ca[i];
		while (c.charAt(0)==' ') c = c.substring(1,c.length);
		if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
	}
	return null;
}

function eraseCookie(name) {
	createCookie(name,"",-1);
}
// ----------------------------------------------------------------------------
</script>

<title>Curves Viewer - OSHMI</title>
</head>

<?php

    function zerofill ($num,$zerofill)
    {
       while (strlen($num)<$zerofill)
       {
             $num = "0".$num;
       }
    return $num;
    } 
    
    // acerta variáveis globais para várias versões do PHP
    extract($_REQUEST, EXTR_PREFIX_ALL|EXTR_SKIP, 'p');
    
    $p_ORIGEM = 2;
        
    if (isset($p_NPONTO_1) && !isset($p_DATA_1))
      @$p_DATA_1 = date("d/m/Y");
    if (isset($p_NPONTO_2) && !isset($p_DATA_2))
      @$p_DATA_2 = date("d/m/Y");
    if (isset($p_NPONTO_3) && !isset($p_DATA_3))
      @$p_DATA_3 = date("d/m/Y");
    if (isset($p_NPONTO_4) && !isset($p_DATA_4))
      @$p_DATA_4 = date("d/m/Y");
    if (isset($p_NPONTO_5) && !isset($p_DATA_5))
      @$p_DATA_5 = date("d/m/Y");
    if (isset($p_NPONTO_6) && !isset($p_DATA_6))
      @$p_DATA_6 = date("d/m/Y");

    if (!isset($p_USUARIO))
      $p_USUARIO="";
    else
      $p_USUARIO=strtoupper($p_USUARIO);  
    if (!isset($p_CONFIG))
      $p_CONFIG="";
    else
      $p_CONFIG=strtoupper($p_CONFIG);  
      
   if ( isset($_COOKIE['consulta_historico_user']) )
      $_COOKIE['consulta_historico_user']=strtoupper($_COOKIE['consulta_historico_user']);    

    $NPONTO=array();
    $NUMPONTO=array();
    $DATA=array();
    $TIPO=array();
    $PRES_0=array();
    $PRES_1=array();
    $UNIDADE=array();
    $COR=array();
    $COR[0]=0xccff99;
    $COR[1]=0xccffff;
    $COR[2]=0xccccff;
    $COR[3]=0x99cccc;
    $COR[4]=0x66cc99;
    $COR[5]=0x669999;
    $MAX_PONTOS=6;
    
    if (!isset($p_INTERVALO))
      $p_INTERVALO=0;     

    if (!isset($p_ORIGEM))
      $p_ORIGEM=0;     

echo "<body style='user-select:none;' onLoad='";
    $NPontos=0;
    $fimdospontos=false;
    for ($i=1; $i<=$MAX_PONTOS; $i++)
      {
	    $DATA[$i-1]="";
	    $NPONTO[$i-1]="";
      eval ("if (!isset(\$p_NPONTO_$i)) \$p_NPONTO_$i=0;");
      eval ("if (\$p_NPONTO_$i!='' && \$p_NPONTO_$i!='0') {\$NPONTO[$i-1]=\$p_NPONTO_$i; \$DATA[$i-1]=trim(\$p_DATA_$i);} else echo \"SelHoje(document.f1.DATA_$i);\"; ");
      eval ("if (!isset(\$p_cbInv$i)) \$p_cbInv$i='';");
      if ($NPONTO[$i-1] && !$fimdospontos ) 
        $NPontos=$i;
      else
        $fimdospontos=true;  
      if ($fimdospontos)
        {
        $NPONTO[$i-1]='';
        $pnpt="p_NPONTO_$i";
        $$pnpt='';
        }
      }
      
echo "'>";
?>

<div id="main">


	<div class="tabsystem keepheading">

		<div class="tabpage tdefault">
            <H3>Query</H3>

<FORM NAME=f1 METHOD="post";>
  <a href="#" title="Find point." OnClick="ObtemPontoSemFechar(document.f1.NPONTO_1);">Point 1:</a>
  <INPUT TYPE="TEXT" NAME="NPONTO_1" ID="NPONTO_1" onkeyup="loadXMLDoc(this)" autocomplete="off" TITLE="Enter point number or tag" STYLE="font-family: monospace" VALUE="<?php print "$NPONTO[0]";?>" SIZE="23">&nbsp;
  Date:
  <input type=text ID="DATA_1" name="DATA_1" size=10 VALUE="<?php print "$DATA[0]";?>">
  <IMG ID="HOJE1" ONCLICK="SelHoje(document.f1.DATA_1);" TITLE="Today." align=middle width=24 height=24 STYLE="cursor:hand" src="icons/reload.png">
  <IMG ID="COPIADATA1" ONCLICK="document.f1.DATA_6.value=document.f1.DATA_5.value=document.f1.DATA_4.value=document.f1.DATA_3.value=document.f1.DATA_2.value=document.f1.DATA_1.value;" TITLE="Repeat date on fiels below." align=middle width=24 height=24 STYLE="cursor:hand" src="icons/down.png">
  Inv.:<INPUT TYPE="CHECKBOX" ID="cbInv1" NAME="cbInv1" title="Invert curve" <?php if (strval($p_cbInv1)!='') print "CHECKED";?>>
<BR><BR>
  <a href="#" title="Find point." OnClick="ObtemPontoSemFechar(document.f1.NPONTO_2);">Point 2:</a>
  <INPUT TYPE="TEXT" NAME="NPONTO_2" ID="NPONTO_2" onkeyup="loadXMLDoc(this)" autocomplete="off"  TITLE="Enter point number or tag" STYLE="font-family: monospace" VALUE="<?php print "$NPONTO[1]";?>" SIZE="23">&nbsp;
  Date:
  <input type=text ID="DATA_2" name="DATA_2" size=10 VALUE="<?php print "$DATA[1]";?>">
  <IMG ID="HOJE2" ONCLICK="SelHoje(document.f1.DATA_2);" TITLE="Today." align=middle width=24 height=24 STYLE="cursor:hand" src="icons/reload.png">
  <IMG ID="COPIADATA2" ONCLICK="document.f1.DATA_6.value=document.f1.DATA_5.value=document.f1.DATA_4.value=document.f1.DATA_3.value=document.f1.DATA_2.value;" TITLE=Repeat date on fiels below." align=middle width=24 height=24 STYLE="cursor:hand" src="icons/down.png">
  Inv.:<INPUT TYPE="CHECKBOX" ID="cbInv2" NAME="cbInv2" title="Invert curve" <?php if (strval($p_cbInv2)!='') print "CHECKED";?>>
<BR><BR>
  <a href="#" title="Find point." OnClick="ObtemPontoSemFechar(document.f1.NPONTO_3);">Point 3:</a>
  <INPUT TYPE="TEXT" NAME="NPONTO_3" ID="NPONTO_3" onkeyup="loadXMLDoc(this)" autocomplete="off"  TITLE="Enter point number or tag" STYLE="font-family: monospace" VALUE="<?php print "$NPONTO[2]";?>" SIZE="23">&nbsp;
  Date:
  <input type=text ID="DATA_3" name="DATA_3" size=10 VALUE="<?php print "$DATA[2]";?>">
  <IMG ID="HOJE3" ONCLICK="SelHoje(document.f1.DATA_3);" TITLE="Today." align=middle width=24 height=24 STYLE="cursor:hand" src="icons/reload.png">
  <IMG ID="COPIADATA3" ONCLICK="document.f1.DATA_6.value=document.f1.DATA_5.value=document.f1.DATA_4.value=document.f1.DATA_3.value;" TITLE="Repeat date on fiels below." align=middle width=24 height=24 STYLE="cursor:hand" src="icons/down.png">
  Inv.:<INPUT TYPE="CHECKBOX" ID="cbInv3" NAME="cbInv3" title="Invert curve" <?php if (strval($p_cbInv3)!='') print "CHECKED";?>>
<BR><BR>
  <a href="#" title="Find point." OnClick="ObtemPontoSemFechar(document.f1.NPONTO_4);">Point 4:</a>
  <INPUT TYPE="TEXT" NAME="NPONTO_4" ID="NPONTO_4" onkeyup="loadXMLDoc(this)" autocomplete="off"  TITLE="Enter point number or tag" STYLE="font-family: monospace" VALUE="<?php print "$NPONTO[3]";?>" SIZE="23">&nbsp;
  Date:
  <input type=text ID="DATA_4" name="DATA_4" size=10 VALUE="<?php print "$DATA[3]";?>">
  <IMG ID="HOJE4" ONCLICK="SelHoje(document.f1.DATA_4);" TITLE="Today." align=middle width=24 height=24 STYLE="cursor:hand" src="icons/reload.png">
  <IMG ID="COPIADATA4" ONCLICK="document.f1.DATA_6.value=document.f1.DATA_5.value=document.f1.DATA_4.value;" TITLE="Repeat date on fiels below." align=middle width=24 height=24 STYLE="cursor:hand" src="icons/down.png">
  Inv.:<INPUT TYPE="CHECKBOX" ID="cbInv4" NAME="cbInv4" title="Invert curve" <?php if (strval($p_cbInv4)!='') print "CHECKED";?>>
<BR><BR>
  <a href="#" title="Find point." OnClick="ObtemPontoSemFechar(document.f1.NPONTO_5);">Point 5:</a>
  <INPUT TYPE="TEXT" NAME="NPONTO_5" ID="NPONTO_5" onkeyup="loadXMLDoc(this)" autocomplete="off"  TITLE="Enter point number or tag" STYLE="font-family: monospace" VALUE="<?php print "$NPONTO[4]";?>" SIZE="23">&nbsp;
  Date:
  <input type=text ID="DATA_5" name="DATA_5" size=10 VALUE="<?php print "$DATA[4]";?>">
  <IMG ID="HOJE5" ONCLICK="SelHoje(document.f1.DATA_5);" TITLE="Today." align=middle width=24 height=24 STYLE="cursor:hand" src="icons/reload.png">
  <IMG ID="COPIADATA5" ONCLICK="document.f1.DATA_6.value=document.f1.DATA_5.value;" TITLE="Repeat date on fiels below." align=middle width=24 height=24 STYLE="cursor:hand" src="icons/down.png">
  Inv.:<INPUT TYPE="CHECKBOX" ID="cbInv5" NAME="cbInv5" title="Invert curve" <?php if (strval($p_cbInv5)!='') print "CHECKED";?>>
<BR><BR>
  <a href="#" title="Find point." OnClick="ObtemPontoSemFechar(document.f1.NPONTO_6);">Point 6:</a>
  <INPUT TYPE="TEXT" NAME="NPONTO_6" ID="NPONTO_6" onkeyup="loadXMLDoc(this)" autocomplete="off"  TITLE="Enter point number or tag" STYLE="font-family: monospace" VALUE="<?php print "$NPONTO[5]";?>" SIZE="23">&nbsp;
Date:
  <input type=text ID="DATA_6" name="DATA_6" size=10 VALUE="<?php print "$DATA[5]";?>">
  <IMG ID="HOJE6" ONCLICK="SelHoje(document.f1.DATA_6);" TITLE="Today." align=middle width=24 height=24 STYLE="cursor:hand" src="icons/reload.png">
  <IMG ID="COPIADATA6" TITLE="Repeat date on fiels below." align=middle width=24 height=24 STYLE="cursor:hand;visibility:hidden" src="icons/down.png">
  Inv.:<INPUT TYPE="CHECKBOX" ID="cbInv6" NAME="cbInv6" title="Invert curve" <?php if (strval($p_cbInv6)!='') print "CHECKED";?>>
<BR><BR>
<div id='lista_op' style='position:absolute'></div>
<?php
if ( !isset($p_HORAINICIAL) )
  $p_HORAINICIAL='00:00:00';
if ( !isset($p_HORAFINAL) )
  $p_HORAFINAL='23:59:59';
?>  
<DIV ID="secavanc" style="display:none">
Initial hour:<INPUT TYPE="TEXT" ID="HORAINICIAL" NAME="HORAINICIAL" VALUE="<?php print "$p_HORAINICIAL";?>" SIZE="8">&nbsp;
Final hour:<INPUT TYPE="TEXT" ID="HORAFINAL" NAME="HORAFINAL" VALUE="<?php print "$p_HORAFINAL";?>" SIZE="8">
Interval:<select ID="INTERVALO" NAME="INTERVALO" TITLE="Sample interval.">
<option id='INTERVALO0'  <?php if ($p_INTERVALO==0)  print "SELECTED"; ?> VALUE="0">All</option>
<option id='INTERVALO1'  <?php if ($p_INTERVALO==1)  print "SELECTED"; ?> VALUE="1">1 min</option>
<option id='INTERVALO2'  <?php if ($p_INTERVALO==2)  print "SELECTED"; ?> VALUE="2">2 min</option>
<option id='INTERVALO5'  <?php if ($p_INTERVALO==5)  print "SELECTED"; ?> VALUE="5">5 min</option>
<option id='INTERVALO10' <?php if ($p_INTERVALO==10) print "SELECTED"; ?> VALUE="10">10 min</option>
<option id='INTERVALO15' <?php if ($p_INTERVALO==15) print "SELECTED"; ?> VALUE="15">15 min</option>
<option id='INTERVALO20' <?php if ($p_INTERVALO==20) print "SELECTED"; ?> VALUE="20">20 min</option>
<option id='INTERVALO30' <?php if ($p_INTERVALO==30) print "SELECTED"; ?> VALUE="30">30 min</option>
<option id='INTERVALO60' <?php if ($p_INTERVALO==60) print "SELECTED"; ?> VALUE="60">60 min</option>
</select>
</DIV>
<INPUT ID="EXECUTAR" TYPE="SUBMIT" NAME="Submit1" VALUE="EXEC"  STYLE="cursor:hand">
<select ID='ORIGEM' name="ORIGEM" TITLE="Selecione uma origem para os dados." style="display:none">
<option id='ORIGEM0' <?php if ($p_ORIGEM==0) print "SELECTED"; ?> VALUE="0">Automático (padrão)</option>
<option id='ORIGEM1' <?php if ($p_ORIGEM==1) print "SELECTED"; ?> VALUE="1">Arquivos SUP</option>
<option id='ORIGEM2' <?php if ($p_ORIGEM==2) print "SELECTED"; ?> VALUE="2">MySQL (=Monitora)</option>
<option id='ORIGEM3' <?php if ($p_ORIGEM==3) print "SELECTED"; ?> VALUE="3">SAGE (PostgreSQL)</option>
<option id='ORIGEM4' <?php if ($p_ORIGEM==4) print "SELECTED"; ?> VALUE="4">ESTIMADOR SAGE</option>
</select>
<BR>
<SMALL>
<DIV style="display:none">
<strong>Save Config</strong><BR>
User: 
  <INPUT TYPE="TEXT" 
         ID="USUARIO" 
         NAME="USUARIO" 
         onchange="document.getElementById('USUARIO').value=document.getElementById('USUARIO').value.replace(/[^\w^ ]/g,'-'); createCookie('consulta_historico_user',  document.getElementById('USUARIO').value,365*5);"
         VALUE="<?PHP echo@ ($_COOKIE['consulta_historico_user']); ?>" 
         TITLE="User name to save a config. Use only alpha, numeric, dashes and spaces chars."
         SIZE="18" 
         >
<BR>
Config: 
  <INPUT TYPE="TEXT" 
         ID="CONFIG"  
         NAME="CONFIG"  
         onchange="document.getElementById('CONFIG').value=document.getElementById('CONFIG').value.replace(/[^\w^ ]/g,'-'); document.getElementById('CB_APAGACONF').checked='';document.getElementById('DIVAPAGAR').style.display='none';"  
         VALUE="<?PHP echo@ $p_CONFIG; /*($_COOKIE['consulta_historico_config']);*/ ?>" 
         TITLE="Name to identify a saved config. Use only alpha, numeric, dashes and spaces chars." 
         SIZE="25" >&nbsp; <span id='DIVAPAGAR' style='display:none'>Erase: <INPUT TYPE='CHECKBOX' ID='CB_APAGACONF' NAME='CB_APAGACONF' title='Mark to erase this saved config.'></span>
<BR>
</DIV>
<?php

$dir = "";

if (isset($_COOKIE['consulta_historico_user']))
  {
  $dir = "confighist/" . $_COOKIE['consulta_historico_user'];
  if (!is_dir( $dir ) )
     mkdir($dir);
  if ($_COOKIE['consulta_historico_user']!='' && $p_CONFIG!='')
    {      
      $handle=fopen($dir.'/'.$p_CONFIG.'.JS', "w");
      if ($handle)
      {
      for ( $i=1; $i<=$MAX_PONTOS; $i++ )
        {
        if ($NPONTO[$i-1]!="")
          {
          $pnpt="p_NPONTO_$i";
          fprintf($handle, "document.getElementById('NPONTO_$i').value='%s'; \n", $$pnpt);
          fprintf($handle, "document.getElementById('DATA_$i').value='%s'; \n", $DATA[$i-1]);
          $cbInv="p_cbInv$i";
          fprintf($handle, "document.getElementById('cbInv$i').checked='%s'; \n", $$cbInv);
          }
        else  
          {
          fprintf($handle, "document.getElementById('NPONTO_$i').value=''; \n");
          fprintf($handle, "document.getElementById('cbInv$i').checked=''; \n");
          }
          
        }  
      fprintf($handle, "document.getElementById('HORAINICIAL').value='%s'; \n", $p_HORAINICIAL);
      fprintf($handle, "document.getElementById('HORAFINAL').value='%s'; \n", $p_HORAFINAL);
      fprintf($handle, "document.getElementById('INTERVALO%d').selected=true; \n", $p_INTERVALO);
      fprintf($handle, "document.getElementById('ORIGEM%d').selected=true; \n", $p_ORIGEM);
      fclose($handle);
      }
    }
  }

if ( is_dir( $dir ) && $_COOKIE['consulta_historico_user']!='')
  { 
    $files = scandir($dir);
    
    if (count($files) > 2)
      {   
	  echo "<BR>Carregar: \n";
	  echo "<select ID='SELCONFIG' NAME='SELCONFIG' TITLE='Choose a saved config to load.' ";
	  echo "onchange='$.getScript(\"$dir/\"+document.getElementById(\"SELCONFIG\").options[document.getElementById(\"SELCONFIG\").selectedIndex].value+\".JS\");document.getElementById(\"CONFIG\").value=document.getElementById(\"SELCONFIG\").options[document.getElementById(\"SELCONFIG\").selectedIndex].text;document.getElementById(\"SELCONFIG\").selectedIndex=0;document.getElementById(\"ORIGEM\").focus();document.getElementById(\"DIVAPAGAR\").style.display=\"\";document.getElementById(\"CB_APAGACONF\").checked=\"\";'";
	  echo ">\n";
      echo "<option id='SELCONFIGESC' SELECTED VALUE=0>Choose a saved config.</option>\n";
      }
 
    foreach ($files as $ind => $nome)
    {
        if (preg_match('/(.*?)\.(JS)$/', $nome, $base))
          {  
            if ( isset($p_CB_APAGACONF) && strval( $p_CB_APAGACONF ) && $base[1] == $p_CONFIG )
               { 
                  unlink("$dir/$nome"); 
                  echo "<script>document.getElementById('CONFIG').value='';</script>";
               }
            else  
              {
                  echo " <option id='SELCONFIG{$ind}' VALUE=\"{$base[1]}\">{$base[1]}</option>";
              }
          }       
    }
 
    if (count($files) > 0)
    {
	  echo "</select>\n";
      echo "<BR> \n";
    }
  }
		
echo '</SMALL></FORM></div>';
    if ($NPONTO[0]=='')
      die("</body></html>");

echo '		<div class="tabpage">';
echo '			<h3>Sheet</h3>';

    print '<BUTTON id="btcopy" onClick="CopyClipboard(holdtext);">Copy to clipboard (Excel)</BUTTON>&nbsp;&nbsp;&nbsp;<BR><BR>';
    print '<BUTTON id="btcopy" onClick="CopyClipboard(holdtext2);">Copy to clipboard (Excel), alternative format</BUTTON>&nbsp;&nbsp;&nbsp;';
    print "\n<BR><BR><span style='font-size: 7pt'>Qualifier: unity for analogs ou state for digitals followed by:<br>F=Failed, E=Estimated, S=Substituted, C=Calculated, M=Manual, H=Time Set G=Large Error V=Variation</span><BR><BR>";

    try 
      {
      $db = new PDO( 'sqlite:../../db/dumpdb.sl3' );
      $db->exec ( "PRAGMA synchronous = NORMAL" );
      }
    catch( PDOException $e )
      {
      die("Erro!");
      }

    // Monta lista de pontos e id para consulta
    $lstpnt = "";
    for ( $indpt=0 ; $indpt < 6 ; $indpt++ )
      {
      $npt = (int) $NPONTO[$indpt];
      if ( $npt != 0 )
        {
        $lstpnt = $lstpnt . $npt . ",";
        $NPONTO[$indpt] = $npt;
        }
      }
     $lstpnt = trim($lstpnt, ",");
     $fltpnt = "";
     if ( $lstpnt != "" )
        $fltpnt = "nponto in ( $lstpnt )  or ";

    $lstid = "";
    for ( $indpt=0 ; $indpt < 6 ; $indpt++ )
      {
      if ( $NPONTO[$indpt] != "" )
        {
        $lstid = $lstid . "'" . $NPONTO[$indpt] . "',";
        }
      }
     $lstid = trim($lstid, ",");
     $fltid = "";
     if ( $lstid != "" )
        $fltid = "id in ( $lstid ) ";
    
    $query = 
"SELECT nponto, estacao, descricao, id, tipo, estoff, eston, unidade 
 FROM dumpdb
 WHERE 
       $fltpnt
       $fltid 
";

//  echo $query;
    
    $Ident="";   
    $i=0;
    $temdig=0;
    $temana=0;
    foreach ( $db->query( $query ) as $row )
    {
         print "<strong>{$row['NPONTO']}: {$row['ESTACAO']}-{$row['DESCRICAO']} ({$row['ID']})</strong>\n";
         print "<BR>";
         for ($i=0;$i<$MAX_PONTOS;$i++)
           {
           if ($NPONTO[$i]==$row['NPONTO'] || $NPONTO[$i]==$row['ID'])
             {
             $NUMPONTO[$i]=$row['NPONTO'];
             $TIPO[$i]=$row['TIPO'];
             $PRES_0[$i]=$row['ESTOFF'];
             $PRES_1[$i]=$row['ESTON'];
             $UNIDADE[$i]=$row['UNIDADE'];
             }
           }
         $Ident.=$row['NPONTO']."  ";
         $Ident.=$row['ESTACAO']."  ";
         $Ident.=$row['DESCRICAO']."  ";
         $Ident.="\n";
         $Identific[$row['NPONTO']]=$row['ESTACAO'].":".$row['DESCRICAO']." (".$row['ID'].")" ;
    }

    require ("lesupsql.php");
    set_time_limit ( 180 );
	
    $txtparaclipboard="";
    $txtparaclipboard2="";
    // print_r ($NUMPONTO);
    print '<TABLE name="Valores" WIDTH="auto" BGCOLOR="white">';
    print "\n<tr><th>Hora</th>";
    $ultimalin=0;
    
    for ($i=0; $i<$NPontos; $i++) // para cada ponto
      {
      $xdata[$i]=array(); $ydata[$i]=array();
      
      $consulta= new THistorico;
      $consulta->origem=$p_ORIGEM;         // origem dos dados
      $consulta->intervalo=$p_INTERVALO;  
      $consulta->valores=TRUE;
      $consulta->stats=TRUE;
      $consulta->horainicial=$p_HORAINICIAL;
      $consulta->horafinal=$p_HORAFINAL;
      $consulta->define_data($DATA[$i]);
      $consulta->inclui_ponto($NPONTO[$i]);
      $ret[$i]=$consulta->busca_hist(); // get historical data from PostgreSQL (1st try) or SQLite (2nd try)

	  // pega as linhas (valores)
      //list($ponto[$i], $linhas[$i]) = each($ret[$i]); 
  	  $ponto[$i] = key($ret[$i]);
	  $linhas[$i] = current($ret[$i]);

      $ultlini[$i]=count($linhas[$i]);
      if ($ultlini[$i]>$ultimalin)
         $ultimalin=$ultlini[$i];
      $cli[$i]=0;
      if ( isset($NUMPONTO[$i]) )
        {
        print "<th>$NUMPONTO[$i]<BR><span style='font-size: 6pt'>";
        print "$DATA[$i]</span></th><th>Qualif.</th>";
        }
      }
    print "</tr>";

    for ($cl=0;$cl<5;$cl++)
    {
      if ($cl%2)
        print "<tr BGCOLOR='#F0E0F0'>\n";
      else
        print "<tr BGCOLOR='#E8D8E8'>\n";
      for ($i=0; $i<$NPontos; $i++) // para cada ponto
      {
          $cli[$i]++;

          if  ($cl >= count($linhas[$i]) ) // evita erro quando não encontrou arquivo ou ponto
          {
            print "\t\t<td style=\"text-align:right;\"> </td>\t\t<td style=\"text-align:right;\"> </td>\n";
            continue;
          }

          $linha = $linhas[$i][$cl];
          $hora  = $linha[0];
          $val=sprintf("%.2f", (double)$linha[1]);
          $qual = $linha[2];
		  
          if ($i==0)
          {
            print "\t\t<td>$qual</td>\n";
            $txtparaclipboard.="$qual\t";
          }

          print "\t\t<td style=\"text-align:right;\">$val</td>\n";
          $txtparaclipboard.=strtr($val,".",",")."\t";

          print "\t\t<td>$hora</td>\n";
          $txtparaclipboard.="$hora\t";
      }

      print "\t</tr>\n";
      $txtparaclipboard.="\n";
    }

    $hora=array();
    $val=array();
    $qual=array();
    
    for ($cl=5;;$cl++)
    {
      $minhora="24:00:00";
      $fim=$NPontos;
      for ($i=0; $i<$NPontos; $i++) $fim=$i+1;    
      
      for ($i=0; $i<$NPontos!=0; $i++) // para cada ponto
        {
        if ($cli[$i] < $ultlini[$i]) // ve se não chegou no fim das linhas deste ponto
          {
          if ($TIPO[$i]=='D')
            $temdig=1;
          else  
            $temana=1;

          $linha = $linhas[$i][$cli[$i]];
          $hora[$i]  = $linha[0];
          $val[$i]=sprintf("%.2f", (double)$linha[1]);
          $qual[$i] = $linha[2];

          if (strcmp($hora[$i], $minhora) < 0)
            $minhora=$hora[$i];
          }
        else
          $fim--;
        }
      if ($fim==0)
        break;

      if ($cl%2)
        print "<tr BGCOLOR='#F0F0F0'>";
      else
        print "<tr BGCOLOR='#E8E8E8'>";

      print "<td>$minhora</td>";
      $txtparaclipboard.="$minhora\t";
      $txtparaclipboard2.="$minhora\t";
      $branco=16777215;
      for ($i=0; $i<$NPontos; $i++) // para cada ponto
      {
        $color=sprintf("#%06x", $COR[$i]-0x111111*($cl%2));
        if ( isset($hora[$i]) )
        if ($hora[$i] == $minhora)
        {
          //$cor=($cl%2)*4;
          //$cor=sprintf("#%02x%02x%02x", 255-$cor-10*$i, 255-$cor-5*$i, 255-$cor);
          print "<td style='text-align:right' BGCOLOR=$color>$val[$i]</td>";
          $txtparaclipboard.=strtr($val[$i],".",",")."\t";
          $vfmt = sprintf("%0.2f", $val[$i]);
          $txtparaclipboard2.=strtr($vfmt,".",",")."\t";

            $tok = strtok ($hora[$i],":"); $segundos=$tok*3600;
            $tok = strtok (":");           $segundos+=$tok*60;
            $tok = strtok (":");           $segundos+=$tok;

            $xdata[$i][$cli[$i]-5]= $segundos/3600;
            $ydata[$i][$cli[$i]-5]= $val[$i];

            $S="";
            
            if ($TIPO[$i]=="D")  
              {
              if ($val[$i]==0)
                $S.=$PRES_1[$i]." ";
              else  
                $S.=$PRES_0[$i]." ";
              } 
            else
              {
              $S.=$UNIDADE[$i]." ";
              }

            $S.=(($qual[$i]&0x80)?"F":""); // falha
            $S.=(($qual[$i]&0x10)?"S":""); // substituído
            // $S.=(($qual[$i]&0x02)?"H":""); // acerto de hora
            // $S.=(($qual[$i]&0x20)&&($qual[$i]&0x01)?"G":""); // analógico, erro grosseiro
            // $S.=(($qual[$i]&0x40)?"V":""); // variacao
            //switch ($qual[$i]&0x0C)
            //  {
            //  case 0x04: $S.="C"; break; // calculado
            //  case 0x08: $S.="E"; break; // estimado
            //  case 0x0C: $S.="M"; break; // manual
            //  }
            // $S.=" ".$qual[$i]; // analógico, erro grosseiro
              
          print "<td BGCOLOR=$color>$S&nbsp;</td>";
          $txtparaclipboard.="$S\t";

          $cli[$i]++;
        }
        else
        {
           print "<td BGCOLOR=$color>&nbsp;</td><td BGCOLOR=$color>&nbsp;</td>";
          $txtparaclipboard.="\t\t";
          $txtparaclipboard2.="0,00\t";
        }
      }
      print "</tr>\n";
      $dataHoje = date("d/m/Y");
      $horaAtual= date("H:i:00");
      $ok=false;
      if ($minhora==$horaAtual)
      {
        $ok=true;
        for ($j=0; $j<$NPontos; $j++)
        {
          $part_data = explode("/",$DATA[$j]);
          $d = $part_data[0];
          $m = $part_data[1];
          $a = $part_data[2];
          $DAT = zerofill($d,2)."/".zerofill($m,2)."/".$a;
          if ($DAT<>$dataHoje)
          {
            $ok = false;
            $j=$NPontos+1;
          }  
        }
      }  
      if ($ok===true)
      {
          $txtparaclipboard2.="\n";  
          $ult_hora = explode(":",$minhora);
          $minut = $ult_hora[1]+1;
          IF ($minut==60)
          {
            $hor   = $ult_hora[0]+1;
            $minut = 0;
          }  
          else
            $hor = $ult_hora[0];
          $min1  = $minut;
          for ($ih= $hor;$ih<24;$ih++){  
            for ($im= $min1;$im<60;$im++){
                $txtparaclipboard2.=zerofill($ih,2).":".zerofill($im,2).":00\t";
                $txtparaclipboard2.="\t";
                for ($j=1; $j<($NPontos-1); $j++) // para cada ponto
                  $txtparaclipboard2.="\t";
                $txtparaclipboard2.="\t\n";
            }    
            $min1=0;
          }
      }
      $txtparaclipboard.="\n";
      $txtparaclipboard2.="\n";
   }
   print "</table>\n";

   print '<TEXTAREA name="holdtext" id="holdtext" cols=120 rows=1460 STYLE="display:none;">';
   print "\n$Ident\n";
   print "Hora";
   for ($i=0; $i<$NPontos; $i++) // para cada ponto
     print "\t$NUMPONTO[$i] ($DATA[$i])\tQualificador";
   print "\n";
   print $txtparaclipboard;
   print "</TEXTAREA>\n";

   print '<TEXTAREA name="holdtext2" id="holdtext2" cols=120 rows=1460  STYLE="display:none;">';
   print "\n\nPlanilha de Comportamento de Medidas\n\n";
   for ($i=0; $i<$NPontos; $i++) // para cada ponto
     print "\t$DATA[$i]";
   print "\n";
   for ($i=0; $i<$NPontos; $i++) // para cada ponto
     print "\tPto: $NUMPONTO[$i]";
   print "\n";
   print $txtparaclipboard2;
   print "</TEXTAREA>\n";
   
?>
		</div>

		<div class="tabpage">

			<h3 onclick='$("input.plotreset").click();' >Plot</h3>

<table border="0">
<tr>
<td>
    <div id="placeholder" style="font-family:sans-serif;width:800px;height:500px;"></div>
    <p id="overviewLegend" style="margin-left:10px"></p>

</td>
<td>
    <input class="yaxisleft"    id="yaxisleft"    type="button" value="&uarr;" title='Show lower values' ><br />
    <input class="yaxisright"   id="yaxisright"   type="button" value="&darr;" title='Show bigger values'><br />
    <input class="yaxiszoomout" id="yaxiszoomout" type="button" value="z-"     title='Less Zoom'><br />
    <input class="yaxiszoomin"  id="yaxiszoomin"  type="button" value="z+"     title='More Zoom'><br />
    <input class="yaxisminus"   id="yaxisminus"   type="button" value="<"      title='Shorter plot'><br />
    <input class="yaxisplus"    id="yaxisplus"    type="button" value=">"      title='Taller plot'><br /><br />
    <input class="yaxiscolor"   id="yaxiscolor"   type="button" value="Cor"    title='Change color'><br />
    <input class="yaxispoints"  id="yaxispoints"  type="button" value="*"      title='Show/hide ticks'><br />
    <input class="plotreset"    id="plotreset"    type="button" value="RST"    title='Reset'><br />
</td>
</tr>
<tr>
<td>
    <input class="xaxisleft"    id="xaxisleft"    type="button" value="&larr;" title='Backwards time'>
    <input class="xaxisright"   id="xaxisright"   type="button" value="&rarr;" title='Forward time'>
    <input class="xaxiszoomout" id="xaxiszoomout" type="button" value="z-"     title='Less zoom'>
    <input class="xaxiszoomin"  id="xaxiszoomin"  type="button" value="z+"     title='More zoom'>
    <input class="xaxisminus"   id="xaxisminus"   type="button" value="<"      title='Smaller plot'>
    <input class="xaxisplus"    id="xaxisplus"    type="button" value=">"      title='Larger plot'>
    <P>Mouse: mouse over to see values; double click to Zoom+; roll to +/- Zoom; click+drag to move.</P>
</td>
<td>
</td>
</tr>
<tr>
<td>
</td>
<td>
</td>
</table>  
  
<input class="dataUpdate" type="button" value="Poll for data" style='display:none' >
    
<script>
var series = [];

<?PHP
for ($pt=0; $pt<$NPontos; $pt++) // para cada ponto
{
$tamx=count($xdata[$pt]);

if ($tamx > 0)
  {
  $cbInv="p_cbInv".($pt+1);
  $inv="";
  if ( $$cbInv!="" )
    {
    $inv="(INV.)";
    foreach ($ydata[$pt] as $k => $v)
      $ydata[$pt][$k]= -$v;
    }

  if ($TIPO[$pt]=='D')
    foreach ($ydata[$pt] as $k => $v)
      $ydata[$pt][$k] = $v==0?0:$v;

  echo "series.push( { label: '', data: [] } );\n";
  echo "series[$pt].label = '{$DATA[$pt]}-{$NUMPONTO[$pt]}-{$Identific[$NUMPONTO[$pt]]} $inv = -----------';\n";
  // echo "series[$pt].color = 'rgb(230, 0, 0)';\n";
  echo "series[$pt].shadowSize=3;\n";
  
  if ($tamx>300)
    echo "$('#yaxispoints').css('display', 'none');\n";
  
  for ($n=0; $n<$tamx; $n++)
    {
    	echo  "series[$pt].data.push([".($xdata[$pt][$n]*1000*60*60).",".$ydata[$pt][$n]."]);\n" ;
    }
  
  }

}
?>
     //  { label: "sin(x sin(x))", data: d }
// opções para o gráfico
var options = {
    legend: { show: true, container: $("#overviewLegend") },  //backgroundOpacity: 0.5 },
		crosshair: { mode: "x" },
		// selection: { mode: "xy" },
        lines: { show: true },
        points: { show: false, radius:1 },
        grid: { hoverable: true, autoHighlight: false },
        xaxis: { min: 0 , 
                 max: 23.599*1000*60*60 , 
                 mode: "time", 
                 timeFormat: "%H:%M:%S" },
        yaxis: { /*tickDecimals: 0 , tickSize: 1000*/  
               },
        zoom: {
            interactive: true
        },
        pan: {
            interactive: true
        }
    };

var cor_cnt = 0;
var iteration = 0;
var plot;
var placeholder = $("#placeholder");

var isIE = navigator.appName.indexOf("Microsoft") != -1;
var el = document.getElementById("placeholder");
if(isIE){
  G_vmlCanvasManager.initElement(el);//method only loaded for IE
  var ctx = el.getContext('2d');//dont know if this really is needed
}	

$(function () {
	
    plot=$.plot($("#placeholder"), series, options);

/*
    // faz o zoom pela seleção
    placeholder.bind("plotselected", function (event, ranges) {
        //$("#selection").text(ranges.xaxis.from.toFixed(1) + " to " + ranges.xaxis.to.toFixed(1));

        options.xaxis.min = ranges.xaxis.from;
        options.xaxis.max = ranges.xaxis.to;
        plot = $.plot(placeholder, series, options);
    });

    placeholder.bind("plotunselected", function (event) {
        $("#selection").text("");
    });
*/

    var legends = $("#overviewLegend .legendLabel");
    legends.each(function () {
        // fix the widths so they don't jump around
        $(this).css('width', $(this).width());
    });
     
    var updateLegendTimeout = null;
    var latestPosition = null;
    
    function updateLegend() {
        updateLegendTimeout = null;
        var pos = latestPosition;
        
        var axes = plot.getAxes();
        if (pos.x < axes.xaxis.min || pos.x > axes.xaxis.max ||
            pos.y < axes.yaxis.min || pos.y > axes.yaxis.max)
            return;
 
        var i, j, dataset = plot.getData();
        for (i = 0; i < dataset.length; ++i) {
            var sseries = dataset[i];
 
            // find the nearest points, x-wise
            for (j = 0; j < sseries.data.length; ++j)
                if (sseries.data[j][0] > pos.x)
                    break;
            
            // now interpolate
            var y, p1 = sseries.data[j - 1], p2 = sseries.data[j];
            if (p1 == null && p2 == null)
              return;
            
            if (p1 == null)
                y = p2[1];
            else if (p2 == null)
                y = p1[1];
            else
                y = p1[1] + (p2[1] - p1[1]) * (pos.x - p1[0]) / (p2[0] - p1[0]);
            legends = $("#overviewLegend .legendLabel");
            //legends = $("#placeholder .legendLabel");
            legends.eq(i).text(sseries.label.replace(/=.*/, "= " + y.toFixed(2))  /*+ ' ' + UNIDADE*/);
            series[i].label=legends.eq(i).text();
        }
    }

    // atualiza legenda 
    $("#placeholder").bind("plothover",  function (event, pos, item) {
        latestPosition = pos;
        if (!updateLegendTimeout)
            updateLegendTimeout = setTimeout(updateLegend, 400);
    });    

    // acerta o tamanho dos botões e prepara as funções para tratar o click
    $("input.xaxisleft").width(50);
    $("input.xaxisleft").click(function () {
        segue_tempo=0;
		options.xaxis.min = options.xaxis.min - 10*60*1000;
		options.xaxis.max = options.xaxis.max - 10*60*1000;
		plot = $.plot(placeholder, series, options);
	});
	
    $("input.xaxisright").width(50);
    $("input.xaxisright").click(function () {
        segue_tempo=0;
		options.xaxis.min = options.xaxis.min + 10*60*1000;
		options.xaxis.max = options.xaxis.max + 10*60*1000;
		plot = $.plot(placeholder, series, options);
	});

    $("input.xaxiszoomin").width(50);
    $("input.xaxiszoomin").click(function () {
		if ( options.xaxis.min < options.xaxis.max - 10*60*1000)
		{
        segue_tempo=0;
		options.xaxis.min = options.xaxis.min + 10*60*1000;
		options.xaxis.max = options.xaxis.max - 10*60*1000;
		plot = $.plot(placeholder, series, options);
	    }
	});

    $("input.xaxiszoomout").width(50);
    $("input.xaxiszoomout").click(function () {
        segue_tempo=0;
		options.xaxis.min = options.xaxis.min - 10*60*1000;
		options.xaxis.max = options.xaxis.max + 10*60*1000;
		plot = $.plot(placeholder, series, options);
	});

    $("input.xaxisplus").width(50);
    $("input.xaxisplus").click(function () {
        placeholder.width(placeholder.width()+100);
		plot = $.plot(placeholder, series, options);
	});

    $("input.xaxisminus").width(50);
    $("input.xaxisminus").click(function () {
		if (placeholder.width()>400)
		     placeholder.width(placeholder.width()-100);
		plot = $.plot(placeholder, series, options);
	});

    $("input.yaxisleft").width(50);
    $("input.yaxisleft").click(function () {
		options.yaxis.min = plot.getAxes().yaxis.min - (plot.getAxes().yaxis.max-plot.getAxes().yaxis.min)/2;
		options.yaxis.max = plot.getAxes().yaxis.max - (plot.getAxes().yaxis.max-plot.getAxes().yaxis.min)/2;
		plot = $.plot(placeholder, series, options);
	});

    $("input.yaxisright").width(50);
    $("input.yaxisright").click(function () {
		options.yaxis.min = plot.getAxes().yaxis.min + (plot.getAxes().yaxis.max-plot.getAxes().yaxis.min)/2;
		options.yaxis.max = plot.getAxes().yaxis.max + (plot.getAxes().yaxis.max-plot.getAxes().yaxis.min)/2;
		plot = $.plot(placeholder, series, options);
	});

    $("input.yaxiszoomin").width(50);
    $("input.yaxiszoomin").click(function () {
		options.yaxis.min = plot.getAxes().yaxis.min + (plot.getAxes().yaxis.max-plot.getAxes().yaxis.min)/4;
		options.yaxis.max = plot.getAxes().yaxis.max - (plot.getAxes().yaxis.max-plot.getAxes().yaxis.min)/4;
		plot = $.plot(placeholder, series, options);
	});

    $("input.yaxiszoomout").width(50);
    $("input.yaxiszoomout").click(function () {
		options.yaxis.min = plot.getAxes().yaxis.min - (plot.getAxes().yaxis.max-plot.getAxes().yaxis.min)/2;
		options.yaxis.max = plot.getAxes().yaxis.max + (plot.getAxes().yaxis.max-plot.getAxes().yaxis.min)/2;
		plot = $.plot(placeholder, series, options);
	});

    $("input.yaxisplus").width(50);
    $("input.yaxisplus").click(function () {
        placeholder.height(placeholder.height()+50);
		plot = $.plot(placeholder, series, options);
	});

    $("input.yaxisminus").width(50);
    $("input.yaxisminus").click(function () {
		if (placeholder.height()>100)
		     placeholder.height(placeholder.height()-50)
		plot = $.plot(placeholder, series, options);
	});

    $("input.yaxiscolor").width(50);
    $("input.yaxiscolor").click(function () {
	    for (var s=0; s<series.length; s++)
		  series[s].color = ++cor_cnt%20;
		plot = $.plot(placeholder, series, options);
	});
	
    $("input.yaxispoints").width(50);
    $("input.yaxispoints").click(function () {
	    if ( !options.points.show && options.lines.show )
	      {
	      options.points.show = true;
        options.lines.show = true;
        }
	    else  
	    if ( options.points.show && options.lines.show )
	      {
	      options.points.show = true;
        options.lines.show = false;
        }
	    else  
	    if ( options.points.show && !options.lines.show )
	      {
	      options.points.show = false;
        options.lines.show = true;
        }        
		plot = $.plot(placeholder, series, options);
	});

    $("input.plotreset").width(50);
    $("input.plotreset").click(function () { 
        segue_tempo=1;
        options.xaxis.min = 0;
        options.xaxis.max = 23.599*1000*60*60 ;
        options.yaxis.min = undefined;
        options.yaxis.max = undefined;        
		plot = $.plot(placeholder, series, options);
    });
 
  // vou criar os tabs dinamicamente porque dá problema com o IE, quando carregado antes da renderização do gráfico (não renderiza direito)
  $.getScript("../lib/tabsystem-4.01/behaviour/listener.js",  
       function () 
         { $.getScript("../lib/tabsystem-4.01/behaviour/tabs.js", 
            function () 
            {
	           setTimeout(tabSystem.construct, 500);
	        } ); 
         } );  
  
  });
    
</script>

		</div>

	</div>

</div>

</body>
</html>
