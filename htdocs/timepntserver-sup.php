<?PHP

// OSHMI/Open Substation HMI - Copyright 2008-2014 - Ricardo L. Olsen

// Parâmetros:
//
// Valores de foto ou série histórica:
// &P=LISTAPONTOS &H=HORA &D=DATA &T=$DATEFMT &U=UNIXTIME &F=FUNCAO B=CALLBACK
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

// Show no errors
error_reporting(E_ALL);

header("Content-type: text/javascript; charset=ISO-8859-1");
header("Content-Disposition: inline; filename=\"timepntserver.js\"");
header("Cache-Control: no-cache");
header("Pragma: no-cache");
    
require_once 'timezone.php';
// @date_default_timezone_set( date_default_timezone_get() );  // date_default_timezone_set('UTC');      

extract($_REQUEST, EXTR_PREFIX_ALL|EXTR_SKIP, 'p');

if ( ! isset( $p_P ) )
  return;

$ARRPNTS = explode( ",", trim($p_P ,",") );
if ( isset( $p_H ) ) $HORA = $p_H; else $HORA = "00:00:00";
if ( isset( $p_T ) ) $DATEFMT = $p_T; else $DATEFMT = "DD/MM/YYYY";
if ( isset( $p_D ) ) $DATA = $p_D; else $DATA = "";
if ( isset( $p_U ) ) $UNIXTIME = $p_U; else $UNIXTIME = 0;
if ( isset( $p_B ) ) $CALLBACK = $p_B; else $CALLBACK = "";
if ( isset( $p_F ) ) $FUNCAO = $p_F; else $FUNCAO = 'F';

if ( $UNIXTIME != 0 )
  {
  $FILTDH = $UNIXTIME;   
  $dt = getdate( $UNIXTIME );
  $DATA = sprintf( "%02d/%02d/%02d" , $dt["mday"], $dt["mon"], $dt["year"] );
  $HORA = sprintf( "%02d:%02d:%02d" , $dt["hours"], $dt["minutes"], $dt["seconds"] );
  }
else
  {
  if ( $DATEFMT == "DD/MM/YYYY" )
    $DATAF = str_replace ("/", "-", $DATA);  
  $FILTDH = strtotime($DATAF." ".$HORA);   
  }

if ( $FUNCAO == "F" )
  echo "Data='".$DATA." ".$HORA."';\n";
  
require_once("../comum/lesupsql.php"); 
/*
require_once("../comum/peardb_login.php") ;
$db=&DBLogin("SQL_BANCOTR_CONSULTA");
if (DB::isError($db)) 
   die("Erro de conexão ao banco!");
*/
//try 
  //{
    //$dbsde = new PDO( 'sqlite:..\oshmi\db\hist.sl3' );
    //$dbsde->exec ( "PRAGMA synchronous = NORMAL" );
    //$dbsde->exec ( "PRAGMA journal_mode = WAL" );
    //$dbsde->exec ( "PRAGMA locking_mode = NORMAL" );

    if ( $FUNCAO == "S" )
      printf( "var hvalues = [];\n" );
              
    $cntpnt = 0;
    $pontos = array();
//    while ( 1 )
//    {    
    $sql = "";
    $uni = "";
    $cntsql = 0;
    for( ; $cntpnt < count($ARRPNTS); $cntpnt++ )
      {
      $pnt = $ARRPNTS[$cntpnt];
      
      $pontos[$cntpnt] =  $pnt;
      
      //$sql = $sql . $uni . "SELECT NPONTO, VALOR, FLAGS, max(DATA) as DATA from hist where nponto = $pnt and data<$FILTDH ";
//      if ( $FUNCAO == "S" )
//        $sql = $sql . $uni . "SELECT NPONTO, VALOR, FLAGS, DATA as DATA from hist where nponto = $pnt and data > $FILTDH " ;
//      else
//        $sql = $sql . $uni . "SELECT NPONTO, VALOR, FLAGS, DATA as DATA from hist where nponto = $pnt and data = (select data from hist where nponto=$pnt and data<=$FILTDH  order by data desc limit 1) ";
//      $uni = "UNION ";
//      $cntsql++;
//      if ( $cntsql > 50 )
//       break;
      }    
      
// print_r ($ARRPNTS);
    
    $consulta= new THistorico;
    
    foreach ($ARRPNTS as $pt)
      {
          printf( "F[$pt]|=0x80;\n");
          $consulta->inclui_ponto($pt);
      }
    $ret=$consulta->busca_foto_sup($DATA, $HORA);    // busca no histórico
      
    // para cada ponto, extrai conjunto de linhas
    while ( list($col_ponto,$linha) = each($ret) )
      {
      if ( in_array($col_ponto, $ARRPNTS) || count($ARRPNTS)==0 ) // testa se o ponto foi solicitado
        {   
        list($col_valor,$col_flags)=$linha;
        $col_valor=number_format($col_valor, 3, ',', '');
        
        $nponto = $col_ponto;
        if ( $nponto == "" )
          continue;
        $flags = $col_valor == 0 ? 0x02 : 0x01;
        $flags |= $col_flags;
        $valor = $col_valor; 
        // $unxtime = $row['DATA'] * 1000; 
        
        if ( $FUNCAO == "S" )
          printf( "hvalues.push( { 'nponto':$nponto,'valor':$valor,'flags':$flags,'unxtime':$unxtime } );\n" );
        else
          printf( "V[$nponto]=%f;F[$nponto]=%d;T[$nponto]='$DATA $HORA';\n", $valor, $flags );
        }
      }
      
//    if ( $cntpnt >= count($ARRPNTS) - 1 )  
//      break;
//    }
//  }
//catch( PDOException $e )
//  {
    // echo "Erro: ".$e->getMessage();
//  }

if ( $FUNCAO == "F" )
  {
  echo "HA_ALARMES=0;\n";
  //$t = gettimeofday();
  //echo "Sha1Dig='" . ( $t["sec"] % 10000 ) . "';\n";
  //echo "NUM_VAR=" . ( $t["sec"] % 10000 ) . ";\n";
  echo "Sha1Dig='" . $FILTDH . "';\n";
  echo "Sha1Ana='" . $FILTDH . "';\n";
  echo "NUM_VAR=" . $FILTDH . ";\n";
  }
  
if ( $CALLBACK != "" )
   echo "$CALLBACK();\n";          
    
?>
