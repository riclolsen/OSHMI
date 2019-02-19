<?PHP

header("Cache-Control: no-store, must-revalidate");
header('Access-Control-Allow-Origin: *');

//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);

require_once 'timezone.php';

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
    //$dtfmt1=" to_char( TIMESTAMP 'epoch' + ";
    //$dtfmt2=" * INTERVAL '1 second', 'YYYY-MM-DD HH24:MI:SS' ) ";      
    $dtfmt1=" to_char( to_timestamp( ";
    $dtfmt2=" ), 'YYYY-MM-DD HH24:MI:SS' ) ";      
    }
  }  

header("Content-Type: application/json; charset=utf-8");

// Instancia o objeto PDO
$pdo = new PDO( 'pgsql:host=127.0.0.1;port=5432;dbname=oshmi;user=grafana;password=oshmi;sslmode=disable;' );

// define para que o PDO lance exceçoes caso ocorra erros
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
// $pdo->exec("SET SESSION character_set_results = 'UTF8';");
// $pdo->exec("SET NAMES UTF8;");

if ( empty($_GET['FILTER']) )
  {
    $filter = "%";
    $filter2 = "0";
    $filter2int = "0";
  }
else
  {
  $filter = str_replace("*", "%", $_GET['FILTER']);
  $filter2 = str_replace(",", "','", $_GET['FILTER']);
  $arrf = explode(',', $_GET['FILTER']);
  $filter2int = "-1";
  foreach ($arrf as $arrv)
    {
    if (is_numeric($arrv))
  	  $filter2int = $filter2int . "," . $arrv;      
    }
  }

$filter2 = "('".$filter2."')";
$filter2int = "(".$filter2int.")";

if ( $realtime )
    {
	$stmt = $pdo->prepare(
           "
		   SELECT 
		     TAG as tag,
			 POINT_KEY as point_key,
			 VALUE as value,
			 STATUS as status,
			 FAILED as failed,
			 $dtfmt1 UNIX_TIMESTAMP $dtfmt2 as timestamp
		   FROM
		     realtime_data
		   WHERE
             (TAG like :filter or TAG in $filter2 or POINT_KEY in $filter2int)
		   ");
	$stmt->setFetchMode(PDO::FETCH_ASSOC);
	$stmt->execute(array('filter' => $filter));
	}	   
else
if ( $intervalo == 0 )
    { 
    $stmt = $pdo->prepare(
		   "
		   SELECT 
		     TAG as tag,
			 POINT_KEY as point_key,
			 VALUE as value,
			 STATUS as status,
			 FAILED as failed,
			 $dtfmt1 UNIX_TIMESTAMP $dtfmt2 as timestamp
		   FROM
		     historical_data
		   WHERE
             (TAG like :filter or TAG in $filter2 or POINT_KEY in $filter2int) and
              UNIX_TIMESTAMP >= :initdate and 
              UNIX_TIMESTAMP <= :enddate
			ORDER BY 
			  UNIX_TIMESTAMP
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
		   "
		   SELECT 
		     last(TAG) as tag,
			 POINT_KEY as point_key,
			 avg(VALUE) as value, -- ver caso digital
			 CASE WHEN last(TYPE_ANADIG)='D' THEN last(STATUS) ELSE avg(VALUE) || ' ' || last(UNIT) END as status,
			 max(FAILED) as failed,
			 $dtfmt1 min((UNIX_TIMESTAMP/(60*:intervalo))*(60*:intervalo)) $dtfmt2 as timestamp
		   FROM
		     historical_data
		   WHERE
             (TAG like :filter or TAG in $filter2 or POINT_KEY in $filter2int) and
             (UNIX_TIMESTAMP/(60*:intervalo))*(60*:intervalo) >= :initdate and 
             UNIX_TIMESTAMP <= :enddate
           GROUP BY 
             POINT_KEY, 
             (UNIX_TIMESTAMP/(60*:intervalo))*(60*:intervalo)
   		   ORDER BY 6
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
