<?PHP

//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);


require_once 'timezone.php';

header("Content-Type: application/json; charset=utf-8");
header("Cache-Control: no-store, must-revalidate");
header("Access-Control-Allow-Origin: *");

if ($_SERVER['REQUEST_METHOD'] !== 'GET' && $_SERVER['REQUEST_METHOD'] !== 'POST') {
  die();
}

// Parameters
// FILTER = Tag or point key filter, can be a comma separated list or can be used * in tag to filter
// INITDATE = set a initial date for historical data (if not set will be returned real time data), format: 2010-03-18T06:40:04
// ENDDATE = set a and date for historical data (if not set will be returned all data from INITDATE up to now)
// SAMPLEINTERVAL = interval of data sampling
// DATEFORMAT = 0:unix timestamp >0: local time as text

$realtime = true;
$initdate = 0;
$enddate = 0;
  
try
{
if ( isset($_GET['INITDATE']) )
  {
  if ( $initdate === false || $_GET['INITDATE']=="" )
    $realtime = true;
  else 
    {
    $initdate = strtotime ( $_GET['INITDATE'] );
    $realtime = false;
    }
  }

if ( isset($_GET['ENDDATE']) )
  {
  $enddate = strtotime ( $_GET['ENDDATE'] . ' 23:59:59');
  if ( $enddate === false )
    $enddate = time();
  }
else
  $enddate = time();

$intervalo = 0;
if ( !$realtime )
if ( isset($_GET['INTERVAL']) )
  {
    $intervalo = $_GET['INTERVAL'];
  }

$dtfmt1="";
$dtfmt2="";
$dateformat = 0;
if ( isset($_GET['DATEFORMAT']) )
  {
    $dateformat = $_GET['DATEFORMAT'];
    if ( $dateformat != 0 )
    {
    $dtfmt1=" datetime( ";
    $dtfmt2=" ,'unixepoch','localtime') ";      
    }
  }  

// Instancia o objeto PDO
$pdo = new PDO( 'sqlite:../db/hist.sl3' );
$pdo->exec ( "PRAGMA query_only=TRUE" );
$pdo->exec ( "PRAGMA synchronous=OFF" );
$pdo->exec ( "PRAGMA journal_mode=WAL" );
//$pdo->exec ( "PRAGMA locking_mode=NORMAL" );
//$pdo->exec ( "PRAGMA cache_size=5000" );
$pdo->exec ( "PRAGMA temp_store=MEMORY" );
$pdo->exec ( "ATTACH DATABASE '../db/dumpdb.sl3' as DBPONTOS" );

// define para que o PDO lance exceçoes caso ocorra erros
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
// $pdo->exec("SET SESSION character_set_results = 'UTF8';");
// $pdo->exec("SET NAMES UTF8;");

if ( empty($_GET['FILTER']) )
  {
    $filter = "%";
    $filter2 = "";
  }
else
  {
  $filter = str_replace("*", "%", $_GET['FILTER']);
  $filter2 = str_replace(",", '","', $_GET['FILTER']);
  }

$filter2 = '("'.$filter2.'")';

if ( $realtime )
  {
  $stmt = $pdo->prepare(
           "SELECT 
              d.ID as tag , 
              d.NPONTO as point_key, 
               CASE d.TIPO WHEN 'D' THEN 
			      1 - h.VALOR
			    ELSE
                  h.VALOR 
				END
				as value, 
              CASE d.TIPO WHEN 'D' THEN 
                CASE h.VALOR WHEN 0 THEN 
                d.ESTON ELSE d.ESTOFF END 
              WHEN 'A' THEN 
                 h.VALOR || ' ' || d.UNIDADE END as status, 
              h.FLAGS>127 as failed, 
              $dtfmt1 max(h.DATA) $dtfmt2 as timestamp 
            from 
              hist h 
            join 
              dumpdb d 
              on d.nponto=h.nponto and
                 d.nponto in (select nponto from dumpdb where id like :filter or id in $filter2 or nponto in $filter2 )
            where 
              h.DATA >= :initdate and 
              h.DATA <= :enddate 
            group by d.nponto
            ");
	$stmt->setFetchMode(PDO::FETCH_ASSOC);
	$stmt->execute(array('filter' => $filter,
						 'initdate' => $initdate,
						 'enddate' => $enddate
						));
    }
else
if ( $intervalo == 0 )
    { 
    $stmt = $pdo->prepare(
            "SELECT 
               d.ID as tag , 
               d.NPONTO as point_key, 
               CASE d.TIPO WHEN 'D' THEN 
			      1 - h.VALOR
			    ELSE
                  h.VALOR 
				END
				as value, 
               CASE d.TIPO WHEN 'D' THEN 
                 CASE h.VALOR WHEN 0 THEN 
                 d.ESTON ELSE d.ESTOFF END 
              WHEN 'A' THEN 
                 h.VALOR || ' ' || d.UNIDADE END as status, 
              h.FLAGS>127 as failed, 
              $dtfmt1 h.DATA $dtfmt2 as timestamp 
            from 
              hist h 
            join 
              dumpdb d 
              on d.nponto=h.nponto and
                 d.nponto in (select nponto from dumpdb where id like :filter or id in $filter2 or nponto in $filter2 )
            where 
              h.DATA >= :initdate and 
              h.DATA <= :enddate
            ");
	$stmt->setFetchMode(PDO::FETCH_ASSOC);
	$stmt->execute(array('filter' => $filter,
						 'initdate' => $initdate,
						 'enddate' => $enddate
						));
    }
else
    {
	$stmt = $pdo->prepare(
           "SELECT 
              d.ID as tag , 
              d.NPONTO as point_key, 
              CASE d.TIPO WHEN 'D' THEN 
			      1 - avg(h.VALOR) 
			    ELSE
                  avg(h.VALOR) 
				END
				as value, 
              CASE d.TIPO WHEN 'D' THEN 
                CASE h.VALOR WHEN 0 THEN 
                d.ESTON ELSE d.ESTOFF END 
              WHEN 'A' THEN 
                avg(h.VALOR) || ' ' || d.UNIDADE END as status, 
              max(h.FLAGS>127) as failed, 
              $dtfmt1 (cast(h.DATA as int)/(60*:intervalo))*(60*:intervalo) $dtfmt2 as timestamp 
            from 
              hist h 
            join 
              dumpdb d 
              on d.nponto=h.nponto and
                 d.nponto in (select nponto from dumpdb where id like :filter or id in $filter2 or nponto in $filter2 )
            where 
              (cast(h.DATA as int)/(60*:intervalo))*(60*:intervalo) >= cast(:initdate as int) and 
              h.DATA <= :enddate 
              GROUP BY 
                d.nponto, 
                (cast(h.DATA as int)/(60*:intervalo))*(60*:intervalo)
              ");
	$stmt->setFetchMode(PDO::FETCH_ASSOC);
	$stmt->execute(array('filter' => $filter,
						 'initdate' => $initdate,
						 'enddate' => $enddate,
						 'intervalo' => $intervalo
						));
	}	  

$arr = array(); 
foreach ($stmt as $row)  
  { 
  array_walk($row, 
     function (&$item, $key) { 
       if ($key == "failed")
         $item = $item ? true : false; 
       //else
       //if ( is_string($item) ) 
       //  $item = mb_convert_encoding($item, 'UTF-8', 'ISO-8859-1' ); 
     });  
  $arr[]=$row;
  // print_r ($row);
  // echo json_encode ($row);  
  }

$res = json_encode ($arr, JSON_NUMERIC_CHECK | JSON_UNESCAPED_UNICODE /*| JSON_PRETTY_PRINT*/);  
if ($res===false)
  {
    switch (json_last_error()) {
        case JSON_ERROR_NONE:
            echo ' - No errors';
        break;
        case JSON_ERROR_DEPTH:
            echo ' - Maximum stack depth exceeded';
        break;
        case JSON_ERROR_STATE_MISMATCH:
            echo ' - Underflow or the modes mismatch';
        break;
        case JSON_ERROR_CTRL_CHAR:
            echo ' - Unexpected control character found';
        break;
        case JSON_ERROR_SYNTAX:
            echo ' - Syntax error, malformed JSON';
        break;
        case JSON_ERROR_UTF8:
            echo ' - Malformed UTF-8 characters, possibly incorrectly encoded';
        break;
        default:
            echo ' - Unknown error';
        break;
    }  
  }
else
  echo $res;
}
catch( PDOException $Exception ) 
{
    // json_encode( $Exception );
    die();   
}

$pdo = null;

?>
