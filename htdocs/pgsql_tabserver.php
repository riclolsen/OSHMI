<?php 
// OSHMI/Open Substation HMI - Copyright 2008-2018 - Ricardo L. Olsen

header('content-type: application/x-javascript; charset=UTF-8');
header("Cache-Control: no-cache");
header("Pragma: no-cache");

//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);

require_once 'timezone.php';

@$filtro = $_GET['G']; // parâmetro G : filtro do tabular por subestação

@$filmod = $_GET['M']; // parâmetro F : filtro do tabular por módulo

@$lstSE = $_GET['S']; // parâmetro S : filtro de subestação

@$filtalms = $_GET['A']; // parâmetro A : filtro de alarmados

@$callbackfn = $_GET['B']; // parâmetro B : nome da função de callback

@$pntinfo = $_GET['I']; // parâmetro P : ponto, tag ou lista de pontos

$order = " r.tag ";

if ($filtro == 'TODOS_ANORMAIS')
  {
	$filtro = '%';
    $filtalms = 2;
	$order = " r.UNIX_TIMESTAMP DESC ";
  }

try {
	$db = new PDO( 'pgsql:host=127.0.0.1;port=5432;dbname=oshmi;user=grafana;password=oshmi;sslmode=disable;' );
	// Vai pegar dados de tempo real

	// read date/time format
	$dfmt='d/m/y'; $tfmt='H:i:s';
	$match = [];
	$ini = @file_get_contents("../i18n/messages_i18n.ini");
	preg_match('/^DATE_FMT_EVENT=(.*)$/m', $ini, $match);
	if ( count($match) > 0 )
	  $dfmt = trim($match[1], " '\"\t\n\r");
	$match = [];
	preg_match('/^TIME_FMT_EVENT=(.*)$/m', $ini, $match);
	if ( count($match) > 0 )
	  $tfmt = trim($match[1], " '\"\t\n\r");

    if ( $lstSE != "" )
	  {
	  $query = "SELECT DISTINCT GROUP_MAIN AS estacao from CATALOG ORDER BY 1";
      $result = $db->prepare($query);
      // $result->setFetchMode(PDO::FETCH_ASSOC);  
      $result->execute(array());
    
	  print("\nLISTA_SES='");        

	  foreach ($result as $line)
		{
		list($col_se)=$line;
		printf("%s-", $col_se);        
		}
	  print("';\n");        
	  }

	if ( $filmod!= "" )
	 // $fitrapormod = "and m.descricao = '$filmod'";
	  {
	  $filmodvir = str_replace( ".", ",", $filmod );
	  $fitrapormod = "and (r.GROUP_SECONDARY='$filmod' or r.GROUP_SECONDARY='$filmodvir')";
	  }
	else
	  $fitrapormod = "";
	  
	$filtraalarmes = "";
	if ($filtalms == 2)  
	  $filtraalarmes = " and r.VALUE=r.ALARM_ST and r.TYPE_ANADIG='D' and r.SUPCMD=0 ";

	if ( $filtro != "" )
	  {
	  $filtro = str_replace( "*", "%", $filtro );    
	  $query =  "SELECT
					r.POINT_KEY, 
					r.TAG, 
					r.GROUP_MAIN,
					r.DESCR_SHORT, 
					r.STATE_OFF,
					r.STATE_ON,
					r.VALUE,
					r.FLAGS,
					r.TYPE_ANADIG,
					CASE WHEN r.IS_CMD=1 THEN 7 ELSE 0 END, 
					CASE WHEN r.IS_CMD=1 THEN r.STATE_ON ELSE '' END,
					CASE WHEN r.IS_CMD=1 THEN r.STATE_OFF ELSE '' END,
					r.GROUP_SECONDARY,
					r.PRIORITY,
					extract(epoch from now()) - r.UNIX_TIMESTAMP,
  				    r.COMMENTS,
					r.LO_LIM,
					r.HI_LIM,
					r.SUPCMD,
					c.POINT_KEY as CMD_KEY,
					r.ALARM_ST,
					r.UNIX_TIMESTAMP as DATA
			    FROM 
				    realtime_data r
				LEFT OUTER JOIN
				    catalog c on c.SUPCMD = r.POINT_KEY					
	 		    WHERE 
				    -- IS_CMD = 0 AND 
				    r.GROUP_MAIN like '$filtro' $fitrapormod
				    $filtraalarmes
			    ORDER BY 
			        $order 
                ";

      $result = $db->prepare($query);
      // $result->setFetchMode(PDO::FETCH_ASSOC);  
      $result->execute(array());

	  
	  $DataConsulta = date("d/m/Y H:i:s");
	  print("Data='$DataConsulta';\n");
	  print("L=[];\n");
	  $cnt=0;
	  foreach ($result as $line)
		{
		list( $col_nponto, $col_id, $col_se, $col_descr, $col_estoff, $col_eston, $col_valor, $col_flags, $col_tipo, $col_origem, $cod_cmdon, $col_cmdoff, $col_moddescr, $col_prioridade, $col_time_upd, $col_anot, $col_liminf, $col_limsup, $col_supcmd, $col_cmdkey, $col_almst, $col_data ) = $line;
        
		$flags = $col_flags;  

		$col_limsup = $col_limsup==null ? 999999.9 : $col_limsup;
	    $col_liminf = $col_liminf==null ? -999999.9 : $col_liminf;

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
		  $estval = sprintf("%0.2f", $col_valor);
		  }  

		$Qlf = "";
		$Qlf=$Qlf.$col_prioridade;
		if ($filtalms == 2)
		  $Qlf=$Qlf."L";   
        else
		if ( $col_tipo=='D' && $col_time_upd < 120 ) // alarm for state changed up to 2min
		  {
   		  $Qlf=$Qlf."L"; 
          $flags |= 0x100;		  
		  }
        else
        if ( $col_tipo=='A' && ($col_valor > $col_limsup || $col_valor < $col_liminf) )
		  $Qlf=$Qlf."LP";   
		
		if ( $col_flags & 0x80 )
		  $Qlf=$Qlf."F";

		if ( $col_origem == 1 )
		  $Qlf=$Qlf."C";

		if ( $col_origem == 6 )
		  $Qlf=$Qlf."M";

		if ( $col_supcmd > 0 )
		  $Qlf=$Qlf."KJ";
	    else
		if ( $col_cmdkey > 0 )
		  $Qlf=$Qlf."K";

    	if ( $col_tipo=='D' && 
		     ( $col_almst == 1 && ($flags&0x02)==0x02 || $col_almst == 0 && ($flags&0x01)==0x01 )
		   ) 
		  {
		  $Qlf=$Qlf."P";		
		  $flags |= 0x800;
		  }

	/*      
		if ( $col_origem == 7 )
		  {
		  $estval = $col_cmdoff . "/" . $col_cmdon;          
		  }
	*/
		$estnorm = 0;
		$taganorm = '';
		
		$supcmd = $col_supcmd;
		if ($supcmd ==0 && $col_cmdkey != 0)
		  $supcmd = $col_cmdkey;
		printf("L[$cnt]='%d,%s,%s,%s,%s,%d,%d,%s,%d,%s';\n", $col_nponto, $col_id, $col_se, $col_descr, $estval, $flags, $supcmd, $Qlf, $estnorm, date( $dfmt.' '.$tfmt, $col_data) );        
		$cnt++;
		}
	  print("\n");        
	  }

	if ( $callbackfn != '' )
	 print("$callbackfn();\n");
}
catch( PDOException $e )
    {
    //echo "Erro: ".$e->getMessage();
    //print_r($db->errorInfo());
    }


?>


