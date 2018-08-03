<?php 
// OSHMI/Open Substation HMI - Copyright 2008-2018 - Ricardo L. Olsen

header('content-type: application/x-javascript; charset=UTF-8');
header("Cache-Control: no-store, must-revalidate");

//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);

// require_once 'timezone.php';

if ( @$_GET['G'] ) // filter for alarms
  die();

@$callbackfn = $_GET['B']; // parameter B : callback function

@$lstSE = $_GET['S']; // parâmetro S : filtro de subestação
if ( $lstSE != "" )
  {
  $db = new PDO( 'sqlite:../db/dumpdb.sl3' );
  $db->exec ( "PRAGMA synchronous = OFF" );
  $db->exec ( "PRAGMA journal_mode = WAL" );
  $db->exec ( "PRAGMA locking_mode = NORMAL" );
  $db->exec ( "PRAGMA cache_size = 5000" );
  $db->exec ( "PRAGMA temp_store = MEMORY" );
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
  if ( $callbackfn != '' )
	print("$callbackfn();\n");
  die();
  }

@$mstatus = $_GET['M']; // parameter M: status request
if ( $mstatus == 1 ) 
  { // inform the number of not akcnowledged events, consider alarm state when this number > 0
  $db = new PDO( 'sqlite:../db/soe.sl3' );
  $db->exec ( "PRAGMA synchronous = OFF" );
  $db->exec ( "PRAGMA journal_mode = WAL" );
  $db->exec ( "PRAGMA locking_mode = NORMAL" );
  $db->exec ( "PRAGMA cache_size = 5000" );
  $db->exec ( "PRAGMA temp_store = MEMORY" );
  $sql = "select count(*) from soe where recon<=1";
  $result = $db->prepare($sql);
  $result->execute(array());
  $col_cnt = 0;
  foreach ($result as $line)
    {
    list($col_cnt)=$line;
	}  
  $alm = $col_cnt > 0? 1 : 0;
  print("HA_ALARMES=$alm;\n");
  print("NUM_VAR=$col_cnt;\n\n");      
  if ( $callbackfn != '' )
    print("$callbackfn();\n");
  die();  
  }  

$dd=0;$mm=0;$yy=0;
$pontos=array();
$ident=array();
$descr=array();
$unidade=array();
$lista_datas=array();
$lstdt=array();

@$pntinfo = $_GET['I']; // parameter I : point key or tag list
$DataConsulta = date("d/m/Y H:i:s");
if ( $pntinfo == "" )
  printf("Data='$DataConsulta';\n");

$par = @$_GET['P']; // parameter P :  point key or tag list

$post_par = @$_POST['E']; // parameter E via post : point key or tag list (priority over P via get)
if ( $post_par != "" )
  $par = $post_par;

$lista_pnt=explode(',',$par);

if ( $pntinfo!="" && $pntinfo!=0 && $par!==null )
  { $lista_pnt[]="xxxx"; }
  else
if ( $pntinfo!="" && $pntinfo!=0 && $par=="" )
  array_push($lista_pnt, $pntinfo);  
  
$lst='';
foreach($lista_pnt as $pnt)
  {
  $pnt=trim($pnt);
  if ($pnt!=0 && $pnt!='')
    $pontos[]=$pnt;
  $lst=$lst."'".$pnt."',";
  }

 
try {
	// database connection
	$db = new PDO( 'sqlite:../db/hist.sl3' );
	$db->exec ( "PRAGMA synchronous = OFF" );
	$db->exec ( "PRAGMA journal_mode = WAL" );
	$db->exec ( "PRAGMA locking_mode = NORMAL" );
	$db->exec ( "PRAGMA cache_size = 5000" );
	$db->exec ( "PRAGMA temp_store = MEMORY" );
	$db->exec ( "ATTACH DATABASE '../db/dumpdb.sl3' as DBPONTOS" );

 	// Vai pegar dados de tempo real

	$lstqry='';
	if ($lst!="" && $lst!="'',")
	  $lstqry=" (NPONTO in ($lst 'xxxxxx') or id in ($lst 'xxxxxx')) AND NPONTO>0 ";

	$query = "select * from 
	          (
	          SELECT -1 as DUMMY,
				  d.NPONTO, 
				  d.VALOR,
				  d.FLAGS,		
				  d.DATA,
				  0 as HORA,
				  d.ID, 
				  d.DESCRICAO,
				  CASE WHEN d.TIPO='D' THEN d.ESTOFF || '/' || d.ESTON ELSE d.UNIDADE END as STATES,
				  0 as ORIGEM,
				  d.TIPO,
				  d.ESTACAO,
				  substr( d.DESCRICAO, 1, instr( d.DESCRICAO, '~')-1) as MODULO,
				  d.LIMI,
				  d.LIMS,
				  strftime('%s', 'now') - d.TS as TS,
				  d.ANOTACAO,
				  d.ESTALM,
				  d.VLRINI
              from 
                  dumpdb d 
		      where		  
				  d.NPONTO in (select NPONTO from dumpdb where $lstqry )
				  and d.TIPO='D'
		      UNION		  
		      select * from 
				  (		  
				  SELECT -1 as DUMMY,
					  d.NPONTO, 
					  h.VALOR,
					  h.FLAGS,		
					  h.DATA,
					  0 as HORA,
					  d.ID, 
					  d.DESCRICAO,
					  CASE WHEN d.TIPO='D' THEN d.ESTOFF || '/' || d.ESTON ELSE d.UNIDADE END as STATES,
					  0 as ORIGEM,
					  d.TIPO,
					  d.ESTACAO,
					  substr( d.DESCRICAO, 1, instr( d.DESCRICAO, '~')-1) as MODULO,
					  d.LIMI,
					  d.LIMS,
					  strftime('%s', 'now') - h.DATA AS TS,
					  d.ANOTACAO,
					  d.ESTALM,
					  d.VLRINI
				  from 
				  hist h 
				  join 
					  dumpdb d 
					  on d.nponto=h.nponto and
						 d.nponto in (select nponto from dumpdb where $lstqry )
				  where 
					  h.DATA >= strftime('%s', 'now') - 48 * 60 * 60 
				  group by d.nponto
				  -- order by h.data asc
				  )
		      )
              group by nponto
              ORDER BY 
		          10, 
  			      1
			";
	// echo $query;
	$result= $db->prepare($query);
    // $result->setFetchMode(PDO::FETCH_ASSOC);  
    $result->execute(array());

    foreach ($result as $line)
	 {
	 list($col_lixo,$col_nponto,$col_valor,$col_flags, $col_data, $col_hora, $col_id, $col_descr, $col_unid, $col_orige, $col_tipo, $col_estacao, $col_modulo, $col_liminf, $col_limsup, $col_time_upd, $col_anot, $col_almst, $col_vlrini)=$line;

	 // problem obtaining first column $col_nponto
	 
	 $col_limsup = $col_limsup==null ? 999999.9 : $col_limsup;
	 $col_liminf = $col_liminf==null ? -999999.9 : $col_liminf;
	 
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
	 if ( $col_time_upd < 120 ) // alarm for state changed up to 2min
	   $col_flags= $col_flags | 0x100;	 
	   
     if ( ( $col_almst == 1 && ($col_flags&0x02)==0x02 || $col_almst == 0 && ($col_flags&0x01)==0x01 ) ) 
       $col_flags |= 0x800;
	 } 
	 else
	 { 
	 // analog values, alamed if out of limits
	 if ( $col_valor > $col_limsup || $col_valor < $col_liminf )
	   $col_flags= $col_flags | 0x900;
	 }
	 
	 if ( $col_anot != "" )
	   $col_flags= $col_flags | 0x200;	 
	 
	 $col_valor=number_format($col_valor, 2, '.', '');

	 if ($col_nponto == 99989)
	   {
		 $col_valor = intval(date("i")) + intval(date("s"))/60;
		 $col_flags = 0x20;
	   }
	 if ($col_nponto == 99990)
	   {
		 $col_valor = intval(date("H")) + intval(date("i"))/60; 
		 $col_flags = 0x20;
	   }
	  
	 if ($pntinfo==1 && $par!="")
	   {
	   printf("TAGS[%d]='%s';\n", $col_nponto, $col_id);
	   printf("NPTS['%s']=%d;\n", $col_id, $col_nponto);
	   printf("BAYS[%d]='%s';\n", $col_nponto, $col_modulo);
	   printf("SUBS[%d]='%s';\n", $col_nponto, $col_estacao);
	   printf("DCRS[%d]='%s';\n", $col_nponto, substr($col_descr, strpos($col_descr, "-") + 1));
	   printf("LIMSUPS[%d]=%f;\n", $col_nponto, $col_limsup);
	   printf("LIMINFS[%d]=%f;\n", $col_nponto, $col_liminf);
	   if ($col_tipo == "D")
		 {
		 printf("STONS[%d]='%s';\n", $col_nponto, substr($col_unid, strpos($col_unid, "/") + 1));
		 printf("STOFS[%d]='%s';\n", $col_nponto, substr($col_unid, 0, strpos($col_unid, "/") ));
		 }
	   else
		 {  
		 printf("STONS[%d]='%s';\n", $col_nponto, "");
		 printf("STOFS[%d]='%s';\n", $col_nponto, "");
		 }
	   printf("V[%d]=%s;F[%d]=%d;T[%d]=''; \n", $col_nponto, $col_valor, $col_nponto, $col_flags, $col_nponto);
	   }
	 else  
	 if ($pntinfo!="" && $pntinfo!=0 && $par===null)
	   {
	   printf("NPTO='%d';\n", $col_nponto);
	   printf("ID='%s';\n", $col_id);
	   printf("ESTACAO='%s';\n", $col_estacao);
	   printf("MODULO='%s';\n", $col_modulo);
	   printf("DESC='%s';\n", $col_descr);
	   if ( $col_tipo == 'D' )
	     {
		 $estados=explode('/', $col_unid);
  	     printf("ST_ON='%s';\n",$estados[1]);
	     printf("ST_OFF='%s';\n",$estados[0]);
	     }
	   else	 
	     {
  	     printf("ST_ON='';\n");
	     printf("ST_OFF='';\n");
	     }
	   printf("CNPTO='0';\n");
	   printf("ANOT='%s';\n", $col_anot);
	   printf("ALRIN='0';\n");
	   printf("VLNOR='%f';\n", $col_vlrini);
	   printf("ESTALM='%d';\n", $col_almst);
	   printf("LIMI=%.2f;\n", $col_liminf);
	   printf("LIMS=%.2f;\n", $col_limsup);
	   printf("HISTER=0;\n");
	   printf("UNIDADE='%s';\n", $col_unid);
	   }
	 else   
	   {
	   //if ( $pntinfo!="" )
	   //  printf("TAGS[%d]='%s';\n", $col_nponto, $col_id);

	   printf("V[%d]=%s;F[%d]=%d;T[%d]=''; \n", $col_nponto, $col_valor, $col_nponto, $col_flags, $col_nponto );
	   }
	   
	 }
	   
	if ($pntinfo=="" || $pntinfo==0 )
	{
	print("HA_ALARMES=0;\n");
	printf("Sha1Ana='%s';\n", date("d/m/Y H:i:s"));
	printf("Sha1Dig='%s';\n", date("d/m/Y H:i:s")); 
	}

	if ( $callbackfn != '' )
	 print("$callbackfn();\n");

    }
catch( PDOException $e )
    {
    //echo "Erro: ".$e->getMessage();
    //print_r($dbsde->errorInfo());
    }

?>


