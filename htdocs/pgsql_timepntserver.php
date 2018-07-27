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

//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);

header('content-type: application/x-javascript; charset=UTF-8');
header("Cache-Control: no-cache");
header("Pragma: no-cache");
    
require_once 'timezone.php';

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
	$db = new PDO( 'pgsql:host=127.0.0.1;port=5432;dbname=oshmi;user=grafana;password=oshmi;sslmode=disable;' );

    if ( $FUNCAO == "S" )
      {
      printf( "var hvalues = [];\n" );
      }

    $cntpnt = 0;
    $maxtm = 0;
    $mintm = time();
    while ( 1 )
    {
    $sql = "";
    $uni = "";
    $cntsql = 0;
    for( ; $cntpnt < count($ARRPNTS); $cntpnt++ )
      {
      $pnt = $ARRPNTS[$cntpnt];
      if ( $FUNCAO == "S" )
        {
        if ( $INTERVALO != 0 )
          $sql = $sql . $uni . "SELECT 
	                              POINT_KEY as nponto,
								  avg(VALUE) as valor,
								  max(flags) as flags,                            								  
	                              (UNIX_TIMESTAMP/(60*$INTERVALO))*(60*$INTERVALO) as data,
								  max(HI_LIM) as lims,
								  min(LO_LIM) as limi
	                            FROM 
								  historical_data
								WHERE 
								  POINT_KEY=$pnt and UNIX_TIMESTAMP > $FILTDH and UNIX_TIMESTAMP < $DATAFIM 
	                            GROUP BY POINT_KEY,(UNIX_TIMESTAMP/(60*$INTERVALO))*(60*$INTERVALO) 
                                ORDER BY 4								
	                            ";
        else
          $sql = $sql . $uni . "SELECT 
	                              POINT_KEY as nponto, 
								  VALUE as valor, 
								  FLAGS as flags, 
								  UNIX_TIMESTAMP as data,
								  HI_LIM as lims,
								  LO_LIM as limi
								from 
								  historical_data 
								where 
								  POINT_KEY = $pnt and UNIX_TIMESTAMP > $FILTDH and UNIX_TIMESTAMP < $DATAFIM 
                                ORDER BY 4								
								";
        }
      else
        {
        $sql = $sql . $uni . "select nponto, valor, flags, data, lims, limi from (
		                        SELECT 
		                        POINT_KEY as nponto, 
								CASE WHEN TYPE_ANADIG = 'A' THEN VALUE ELSE 1-VALUE END as valor, 
								FLAGS as flags, 
								UNIX_TIMESTAMP as data,
 						        HI_LIM as lims,
								LO_LIM as limi
						      from 
							    historical_data 
							  where 
							    POINT_KEY = $pnt and
								UNIX_TIMESTAMP < $FILTDH and ( (TYPE_ANADIG = 'A' and UNIX_TIMESTAMP > $FILTDH - 24*60*60) or (TYPE_ANADIG = 'D' and UNIX_TIMESTAMP > $FILTDH - 24*60*60) ) order by UNIX_TIMESTAMP desc limit 1 
						      ) as SEL
							  ";
        }
      $uni = "UNION ";
      $cntsql++;
      if ( $cntsql > 25 )
       break;
      }
    $result = $db->prepare($sql);
    $result->setFetchMode(PDO::FETCH_ASSOC);  
    $result->execute(array());
	
	$lims = 99999;
	$limi = -99999;
    if ( $sql != "" )
    foreach ( $result as $row )
      {
      $nponto = $row['nponto'];
      if ( $nponto == "" )
        continue;
      $flags = $row['flags'];
      $valor = $row['valor']; 
      $unxtime = $row['data'] * 1000;
	  $lims=$row['lims'];
	  $limi=$row['limi'];

      if ( $row['data'] > $maxtm )
         {
         $maxtm = $row['data'];
         }
      if ( $row['data'] < $mintm )
         {
         $mintm = $row['data'];	
         }

      if ( $FUNCAO == "S" )
        {
        printf( "hvalues.push( { 'nponto':$nponto,'valor':%.3f,'flags':$flags,'unxtime':$unxtime } );\n", $valor );
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
      printf( "LIMSUPS[$pnt]=%f;LIMINFS[$pnt]=%f;\n", $lims, $limi );
    }        
    
  }
catch( PDOException $e )
  {
    //echo "Erro: ".$e->getMessage();
    //print_r($db->errorInfo());
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
