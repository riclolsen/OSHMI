<?php 
// OSHMI/Open Substation HMI - Copyright 2008-2018 - Ricardo L. Olsen

header('content-type: application/x-javascript; charset=UTF-8');
header("Cache-Control: no-store, must-revalidate");

//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);

require_once 'timezone.php';

@$filtro = $_GET['G']; // parametro G : filtro do tabular por subestacao

@$filmod = $_GET['M']; // parametro F : filtro do tabular por modulo

@$lstSE = $_GET['S']; // parametro S : filtro de subestacao

@$filtalms = $_GET['A']; // parametro A : filtro de alarmados

@$callbackfn = $_GET['B']; // parametro B : nome da funcao de callback

@$pntinfo = $_GET['I']; // parametro P : ponto, tag ou lista de pontos

$order = ' ID ';
if ($filtro == 'TODOS_ANORMAIS')
  {
	$filtro = '%';
    $filtalms = 2;
	$order = ' DATA DESC ';
  }  
  
try {
	$db = new PDO( 'sqlite:../db/hist.sl3' );
	$db->exec ( "PRAGMA synchronous = OFF" );
	$db->exec ( "PRAGMA journal_mode = WAL" );
	$db->exec ( "PRAGMA locking_mode = NORMAL" );
	$db->exec ( "PRAGMA cache_size = 5000" );
	$db->exec ( "PRAGMA temp_store = MEMORY" );
	$db->exec ( "ATTACH DATABASE '../db/dumpdb.sl3' as DBPONTOS" );

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
	  $query = "SELECT DISTINCT ESTACAO AS estacao from dumpdb ORDER BY 1";
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
	  $fitrapormod = "and (substr( d.DESCRICAO, 1, instr( d.DESCRICAO, '~')-1)='$filmod' or substr( d.DESCRICAO, 1, instr( d.DESCRICAO, '~')-1)='$filmodvir')";
	  }
	else
	  $fitrapormod = "";
	  
	$filtraalarmes = "";
	$filtraalarmesh = "";
	if ($filtalms == 2)  
      {
	  $filtraalarmes = " and (d.TIPO='D' and 1-d.VALOR=d.ESTALM and d.SUPCMD=0) ";
	  $filtraalarmesh = " WHERE (TIPO='D' and 1-VALOR=ESTALM and SUPCMD=0) ";
	  }

	if ( $filtro != "" )
	  {
	  $filtro = str_replace( "*", "%", $filtro );    

  	  $query = 
	         "SELECT * FROM 
	          (
	          SELECT -1 as DUMMY,
				  d.NPONTO, 
				  d.ID, 
				  d.ESTACAO,
				  d.DESCRICAO,
				  d.ESTOFF,
				  d.ESTON,
				  d.VALOR,
				  d.FLAGS,		
				  d.TIPO,
				  CASE WHEN d.SUPCMD>0 THEN 7 ELSE 0 END AS ORIGEM, 
				  CASE WHEN d.SUPCMD>0 THEN d.ESTON ELSE '' END AS CMDON, 
				  CASE WHEN d.SUPCMD>0 THEN d.ESTOFF ELSE '' END AS CMDOFF, 
				  substr( d.DESCRICAO, 1, instr( d.DESCRICAO, '~')-1) AS MODULO,
				  d.PRIOR,
				  strftime('%s', 'now') - d.TS as UPD,
				  d.ANOTACAO,
				  d.LIMI,
				  d.LIMS,
				  d.SUPCMD,
				  0 as CMDPNT,
				  d.ESTALM,
				  d.TS as DATA
              from 
                  dumpdb d 
		      where		  
			      d.ESTACAO like '$filtro' $fitrapormod
		          $filtraalarmes
				  and d.tipo='D'
		      UNION		  
		      select * from 
				  (		  
				  SELECT -1 as DUMMY,
					  d.NPONTO, 
					  d.ID, 
					  d.ESTACAO,
					  d.DESCRICAO,
  				      d.ESTOFF,
 				      d.ESTON,
					  h.VALOR,
					  h.FLAGS,		
					  d.TIPO,
 				      CASE WHEN d.SUPCMD>0 THEN 7 ELSE 0 END AS ORIGEM, 
				      CASE WHEN d.SUPCMD>0 THEN d.ESTON ELSE '' END AS CMDON,
				      CASE WHEN d.SUPCMD>0 THEN d.ESTOFF ELSE '' END AS CMDOFF, 
					  substr( d.DESCRICAO, 1, instr( d.DESCRICAO, '~')-1) AS MODULO,
				      d.PRIOR,
					  strftime('%s', 'now') - h.DATA as UPD,
					  d.ANOTACAO,
					  d.LIMI,
					  d.LIMS,
					  d.SUPCMD,
					  0 as CMDPNT,
					  d.ESTALM,
					  h.DATA as DATA					 
				  from 
				  hist h 
				  join 
					  dumpdb d 
					  on d.NPONTO=h.NPONTO and
                         d.ESTACAO like '$filtro' $fitrapormod
				  where 
					  h.DATA >= strftime('%s', 'now') - 48 * 60 * 60 
				  group by d.NPONTO
				  -- order by h.DATA asc
				  )
		      )
              $filtraalarmesh
              GROUP BY 
			    NPONTO
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
		list( $col_lixo, $col_nponto, $col_id, $col_se, $col_descr, $col_estoff, $col_eston, $col_valor, $col_flags, $col_tipo, $col_origem, $cod_cmdon, $col_cmdoff, $col_moddescr, $col_prioridade, $col_time_upd, $col_anot, $col_liminf, $col_limsup, $col_supcmd, $col_cmdkey, $col_almst, $col_data ) = $line;
        
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


