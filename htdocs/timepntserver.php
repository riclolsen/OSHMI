<?PHP

// OSHMI/Open Substation HMI - Copyright 2008-2018 - Ricardo L. Olsen

// Parâmetros:
//
// Valores de foto ou série histórica:
// &P=LISTAPONTOS &H=HORA &D=DATA INICIAL [&E=DATA FINAL] [&I=INTERVALO EM min] &T=$DATEFMT &U=UNIXTIME &F=FUNCAO B=CALLBACK
//
// HORA : hora no formato hh:mm:ss
// DATA : data no formato dd/mm/yyyy ou mm/dd/yyyy
// DATEFMT: formato da data "DD/MM/YYYY" ou "MM/DD/YYYY"
// ou
// UNIXTIME: unix time 
//
// FUNCAO : 'F'=Foto histórica, 'S'=Série histórica desde a hora especificada até a hora corrente
// CALLBACK : nome da função de callback
//

header("Content-type: text/javascript; charset=UTF-8");
header("Content-Disposition: inline; filename=\"timepntserver.js\"");
header("Cache-Control: no-store, must-revalidate");
header('Access-Control-Allow-Origin: *');

// Show no errors
// error_reporting(0);
    
require_once 'timezone.php';
// @date_default_timezone_set( date_default_timezone_get() );

extract($_REQUEST, EXTR_PREFIX_ALL|EXTR_SKIP, 'p');

if ( ! isset( $p_P ) )
  return;

$ARRPNTS = explode( ",", trim($p_P ,",") );
if ( isset( $p_H ) ) $HORA = $p_H; else $HORA = "00:00:00";
if ( isset( $p_T ) ) $DATEFMT = $p_T; else $DATEFMT = "DD/MM/YYYY";
if ( isset( $p_D ) ) $DATAINI = $p_D; else $DATAINI = "";
if ( isset( $p_E ) ) $DATAFIM = $p_E; else $DATAFIM = time();
if ( isset( $p_I ) ) $INTERVALO = $p_I; else $INTERVALO = 1;
if ( isset( $p_U ) ) $UNIXTIME = $p_U; else $UNIXTIME = 0;
if ( isset( $p_B ) ) $CALLBACK = $p_B; else $CALLBACK = "";
if ( isset( $p_F ) ) $FUNCAO = $p_F; else $FUNCAO = 'F';

if ( $UNIXTIME != 0 )
  {
  $FILTDH = $UNIXTIME;   
  $dt = getdate( $UNIXTIME );
  $DATAINI = sprintf( "%02d/%02d/%02d" , $dt["mday"], $dt["mon"], $dt["year"] );
  $HORA = sprintf( "%02d:%02d:%02d" , $dt["hours"], $dt["minutes"], $dt["seconds"] );
  }
else
  {
  if ( $DATEFMT == "DD/MM/YYYY" )
    {
    $DATAF = str_replace ("/", "-", $DATAINI);
    }  
  $FILTDH = strtotime($DATAF." ".$HORA);
  }

if ( $FUNCAO == "F" )
  {
  echo "Data='".$DATAINI." ".$HORA."';\n";
  }

try 
  {
    $dbhist = new PDO( 'sqlite:../db/hist.sl3' );
    $dbhist->exec ( "PRAGMA query_only=TRUE" );
    $dbhist->exec ( "PRAGMA synchronous=OFF" );
    $dbhist->exec ( "PRAGMA journal_mode=WAL" );
    //$dbhist->exec ( "PRAGMA locking_mode=NORMAL" );
    //$dbhist->exec ( "PRAGMA cache_size=5000" );
    $dbhist->exec ( "PRAGMA temp_store=MEMORY" );
    $dbhist->exec ( "ATTACH DATABASE '../db/dumpdb.sl3' as DBPONTOS" );
    //$dbhist->exec ( "PRAGMA mmap_size=268435456" );

    if ( $FUNCAO == "S" )
      {
      printf( "hvalues = [];\n" );
      }

    $cntpnt = 0;
    $maxtm = 0;
    $mintm = time();
    while ( 1 )
    {
    $sql = "";
    $uni = "";
    $cntsql = 0;
    for( ; $cntpnt < count($ARRPNTS) ; $cntpnt++ )
      {
      $pnt = $ARRPNTS[$cntpnt];

      if ( $FUNCAO == "S" )
        {
        if ( $INTERVALO != 0 )
          $sql = $sql . $uni . "SELECT 
                                    NPONTO, 
                                    avg(VALOR) as VALOR, 
                                    max(FLAGS) as FLAGS, 
                                    (cast(data as int)/(60*$INTERVALO))*(60*$INTERVALO) as DATA 
                                FROM 
                                    hist 
                                WHERE 
                                    (nponto = '$pnt' or 
                                     nponto in (select nponto from dumpdb where id='$pnt')) and 
                                    data > $FILTDH and data <  $DATAFIM 
                                GROUP BY 
                                    (cast(data as int)/(60*$INTERVALO))*(60*$INTERVALO) ";
        else
          $sql = $sql . $uni . "SELECT 
                                    NPONTO, 
                                    VALOR, 
                                    FLAGS, 
                                    DATA as DATA 
                                FROM hist 
                                WHERE 
                                    (nponto = '$pnt' or 
                                     nponto in (select nponto from dumpdb where id='$pnt')) and 
                                    data > $FILTDH and 
                                    data < $DATAFIM ";
        }
      else
        {
        $sql = $sql . $uni . "SELECT 
                                    NPONTO, 
                                    VALOR, 
                                    FLAGS, 
                                    DATA as DATA 
                              FROM
                                    hist 
                              WHERE
                                    (nponto = '$pnt' or 
                                     nponto in (select nponto from dumpdb where id='$pnt')) and 
                                    data = (select data from hist where nponto=$pnt and data<=$FILTDH  
                              ORDER BY
                                    data desc limit 1) ";
        }
      $uni = " UNION ";
      $cntsql++;
      if ( $cntsql > 50 )
       break;
      }
    //  echo $sql;  
    if ( $sql != "" )
    foreach ( $dbhist->query( $sql ) as $row )
      {
      $nponto = $row['NPONTO'];
      if ( $nponto == "" )
        continue;
      $flags = $row['FLAGS'];
      $valor = $row['VALOR']; 
      $unxtime = $row['DATA'] * 1000;

      if ( $row['DATA'] > $maxtm )
         {
         $maxtm = $row['DATA'];
         }
      if ( $row['DATA'] < $mintm )
         {
         $mintm = $row['DATA'];	
         }

      if ( $FUNCAO == "S" )
        {
        printf( "hvalues.push( { 'nponto':'$pnt','valor':$valor,'flags':$flags,'unxtime':$unxtime } );\n" );
        }
      else
        {
        $dt = getdate( $unxtime/1000 );
        $DATAPT = sprintf( "%02d/%02d/%02d" , $dt["mday"], $dt["mon"], $dt["year"] );
        $HORAPT = sprintf( "%02d:%02d:%02d" , $dt["hours"], $dt["minutes"], $dt["seconds"] );
        printf( "V[$nponto]=%f;F[$nponto]=%d;T[$nponto]='$DATAPT $HORAPT';\n", $valor, $flags );
        }
      }    
    if ( $cntpnt >= count($ARRPNTS) - 1 )  
      break;
    }
  /*
  $dt = getdate( $maxtm );  
  $DATAMAX = sprintf( "%02d/%02d/%02d" , $dt["mday"], $dt["mon"], $dt["year"] );
  $HORAMAX = sprintf( "%02d:%02d:%02d" , $dt["hours"], $dt["minutes"], $dt["seconds"] );
  $dt = getdate( $mintm );  
  $DATAMIN = sprintf( "%02d/%02d/%02d" , $dt["mday"], $dt["mon"], $dt["year"] );
  $HORAMIN = sprintf( "%02d:%02d:%02d" , $dt["hours"], $dt["minutes"], $dt["seconds"] );
  printf( "// $DATAMIN $HORAMIN \n");
  printf( "// $DATAMAX $HORAMAX \n");
  */
  // if data too old, presents an invalid, unknown date
  if ( intval($FILTDH) - $maxtm > 60 * 5 )
    {
	  echo "Data='??/??/???? ??:??:??';\n";
	}
 	
  if ( $FUNCAO == "S" )
    {
    $dbpnt = new PDO( 'sqlite:../db/dumpdb.sl3' );
    $dbpnt->exec ( "PRAGMA synchronous = NORMAL" );
    $dbpnt->exec ( "PRAGMA journal_mode = WAL" );
    $dbpnt->exec ( "PRAGMA locking_mode = NORMAL" );
    $sql = "SELECT NPONTO, LIMS, LIMI from dumpdb where nponto = $pnt" ;
    foreach ( $dbpnt->query( $sql ) as $row )
      {
      $nponto = $row['NPONTO'];
      printf( "LIMSUPS[$nponto]=%f;LIMINFS[$nponto]=%f;\n", $row['LIMS'], $row['LIMI'] );
      }
    }        
    
  }
catch( PDOException $e )
  {
    // echo "Erro: ".$e->getMessage();
  }

if ( $FUNCAO == "F" )
  {
  echo "HA_ALARMES=0;\n";
  echo "Sha1Dig='" . $FILTDH . "';\n";
  echo "Sha1Ana='" . $FILTDH . "';\n";
  echo "NUM_VAR=" . time() . ";\n";
  }
  
if ( $CALLBACK != "" )
  {
  echo "$CALLBACK();\n";
  }          
?>
