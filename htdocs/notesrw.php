<?PHP

// Show no errors
error_reporting(E_ALL);

header("Content-Type: application/json; charset=utf-8");
header("Cache-Control: no-cache");
header("Pragma: no-cache");
    
require_once 'timezone.php';

try {
    $db = new PDO( 'sqlite:../db/notes.sl3' );
    $db->exec ( "PRAGMA synchronous = NORMAL" );
    $db->exec ( "PRAGMA journal_mode = WAL" );
    $db->exec ( "PRAGMA locking_mode = NORMAL" );
    $db->exec ( "PRAGMA cache_size = 5000" );
    $db->exec ( "PRAGMA temp_store = MEMORY" );
    //$db->exec ( "PRAGMA mmap_size=268435456" );
    //$db->exec ( "ATTACH DATABASE '../db/dumpdb.sl3' as DBPONTOS" );

    $screenfile = $_GET["screenfile"];
/*
    $qry = sprintf( "SELECT id, content, x, y, bgcolor, textcolor, opened, user, tscreate from notes where screenfile='%s' and erased=0;", $screenfile);
    // echo $qry;
    $ret = $db->query($qry);
    $o = $ret->fetchAll(PDO::FETCH_ASSOC);
*/    
    $stmt = $db->prepare("SELECT id, screenfile, content, x, y, bgcolor, textcolor, opened, user, tscreate from notes where screenfile=:screenfile and erased=0;");
    $stmt->bindValue(':screenfile', $screenfile);
    $stmt->execute();
    $o = $stmt->fetchAll(PDO::FETCH_ASSOC);
    
    // array_walk_recursive($o, 'walkrec');
    echo json_encode($o);

    }
catch( PDOException $e )
    {
     echo "Erro: ".$e->getMessage();
     print_r($db->errorInfo());
    }

?>
