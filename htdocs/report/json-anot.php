<?PHP
header("Content-Type: application/json; charset=utf-8");

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

//require_once '../timezone.php';

try
{

// PDO object
$pdo = new PDO( 'sqlite:../../db/dumpdb.sl3' );
$pdo->exec ( "PRAGMA synchronous = OFF" );
$pdo->exec ( "PRAGMA journal_mode = WAL" );
$pdo->exec ( "PRAGMA locking_mode = NORMAL" );
$pdo->exec ( "PRAGMA cache_size = 5000" );
$pdo->exec ( "PRAGMA temp_store = MEMORY" );
$pdo->exec ( "ATTACH DATABASE '../../db/notes.sl3' as NTS; " );

$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
// $pdo->exec("SET SESSION character_set_results = 'UTF8';");
// $pdo->exec("SET NAMES UTF8;");
// $_GET['PARAMETER'] $sql = "... = :parameter ... "; $stmt->execute( array('parameter' => $_GET['PARAMETER'] ) );


// CHANGE THIS QUERY TO CREATE OTHER REPORTS
$sql=  "select nponto as nponto, id, descricao, anotacao, 'blocante' as tipo from dumpdb where anotacao != '' " .
       "union ".
       "select n.pointnum as nponto, d.id as id, d.descricao as descricao, n.content as anotacao, 'documental' as tipo from notes n join dumpdb d on d.nponto=n.pointnum where n.erased = 0 ".
       "order by id";       

$stmt = $pdo->prepare($sql);
$stmt->setFetchMode(PDO::FETCH_ASSOC);
$stmt->execute(array());
      
$arr = array(); 
foreach ($stmt as $row) 
  { 
  array_walk($row, 
     function (&$item, $key) { 
       if ($key == "FAILED")
         $item = $item ? true : false; 
       else
       if ( is_string($item) ) 
	   if ( mb_detect_encoding($item, 'UTF-8', true ) === false ) // detect if not UTF8 encoded
         $item = mb_convert_encoding($item, 'UTF-8', 'ISO-8859-1' ); // convert from ISO latin1 to UTF8
     });  
  $arr[]=$row;
  }

$res = array();
$res = json_encode ($arr, JSON_NUMERIC_CHECK | JSON_UNESCAPED_UNICODE| JSON_PRETTY_PRINT);  
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
    die (json_encode( $Exception ));
}

$pdo = null;

?>
