<?PHP

// xPlain Power Graphics - Copyright 2016 - Ricardo L. Olsen/DSC Systems
// Script to consult a SQLITE OSHMI database
// GET Parameters:
// FORMAT: JSON (default) or JSVALS (javascript point values)
// STOREDQUERY: index of stored database query 
// or
// QUERY: database query 
// HMAC: query validation based on a secret key and HMAC algorithm

error_reporting(E_ALL);
ini_set('display_errors', 1);

// Use http://www.freeformatter.com/hmac-generator.html to generate hmac validators for queries

// Ex QUERY=SELECT p.nponto, p.id, s.estado, s.data, s.hora FROM dumpdb p join soe s on s.nponto=p.nponto limit 100

  // pre-defined queries, executed by setting STOREDQUERY parameter
  $storedqry = array();
  $storedqry[0] = "select nponto as pnt, valor as y, data*1000 as x from hist where nponto = 43 and data>strftime('%s','now')-60*60 order by data desc";
  $storedqry[1] = "select nponto as pnt, valor as val, data*1000 as date from hist where (nponto = 43 or nponto=44 or nponto=45) and data>strftime('%s','now')-60*60 order by pnt, data desc";
  $storedqry[2] = "select s.nponto as pnt, s.estado as y, s.data*1000 as x, p.descricao as descr, p.estacao as estacao from soe s join dumpdb p on s.nponto=p.nponto where s.data>strftime('%s','now')-60*15 order by s.data desc limit 1000";
  $storedqry[3] = "select d.nponto as pnt, d.id as id, (select h.valor from hist h where h.nponto=d.nponto limit 1) as val, (select h.flags from hist h where h.nponto=d.nponto order by data desc limit 1) as flags from dumpdb as d where d.id like 'OSO2%'";
  
  @$idx = $_GET["STOREDQUERY"];
  @$qry = $_GET["QUERY"];
  @$hmachash = $_GET["HMAC"];
  $secretkey = "$3kur3m3#!";
  @$fmt = $_GET["FORMAT"];
  if ( !isset($fmt) )
     $fmt = "JSON";
   
  if ( isset($idx) )
    { // has STOREDQUERY parameter setted
    if ( $idx < count($storedqry) )
      { // bounds ok
        $qry = $storedqry[$idx];
      }   
    else
      { // out of bounds
      die("");
      }      
    }
  else
  // verify query athenticity
  if ( $qry == "" /*|| $hmachash != hash_hmac("sha1", $qry, $secretkey) */)
    die("");  // not a valid query
  else
    { 
    // validated query but
    // will not accept queries that change the database!
    if ( preg_match("/\binsert\b/i", $qry) ||
         preg_match("/\bdelete\b/i", $qry) ||
         preg_match("/\bdrop\b/i", $qry) ||
         preg_match("/\breplace\b/i", $qry) ||
         preg_match("/\bcreate\b/i", $qry) ||
         preg_match("/\breindex\b/i", $qry) ||
         preg_match("/\bvacuum\b/i", $qry) ||
         preg_match("/\bupdate\b/i", $qry) 
       )
       die(""); // dangerous query!
    }
  
  try 
    {
    // connect to database using PDO
    $dbhist = new PDO( 'sqlite:../db/dumpdb.sl3' );
    $dbhist->exec ( "PRAGMA synchronous = NORMAL" );
    $dbhist->exec ( "PRAGMA journal_mode = WAL" );
    $dbhist->exec ( "PRAGMA locking_mode = NORMAL" );
    $dbhist->exec ( "PRAGMA cache_size = 5000" );
    $dbhist->exec ( "PRAGMA temp_store = MEMORY" );
    
    $dbhist->exec ( "ATTACH '../db/soe.sl3' as soe" );
    $dbhist->exec ( "ATTACH '../db/hist.sl3' as hist" );

    $statement = $dbhist->prepare($qry);
    $statement->execute();
    $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
    // converte para UTF-8, necessario para json_encode
    function walkrec(&$item, $key)
      {
      $item =  mb_convert_encoding($item, "UTF-8", "ISO-8859-1"); // HTML-ENTITIES
      // echo "$key holds $item\n";
      }
    
    if ( $fmt == "JSON" )
      { 
      array_walk_recursive($results, 'walkrec');
      $json = json_encode( $results /*,JSON_PRETTY_PRINT*/ );
      header('Content-Type: application/json');
      echo $json;
      }
    else  
    if ( $fmt == "JSVALS" )
      {
      header('Content-Type: application/javascript');
      foreach ( $results as $res )
        {
        if ( $res["val"] == "" )  
          {
          $res["val"] = 0;
          $res["flags"] = 0x80;
          }
        //echo "V['".$res["pnt"]."']=".$res["val"].";\n";
        //echo "F['".$res["pnt"]."']=".$res["flags"].";\n";
        echo "V['".$res["id"]."']=".$res["val"].";\n";
        echo "F['".$res["id"]."']=".$res["flags"].";\n";
        }
      echo "Sha1Ana='".rand().time()."';\n";
      echo "Sha1Dig='".rand().time()."';\n";
      }
    }
  catch( PDOException $e )
    {
      // echo "Erro: ".$e->getMessage();
    }

?>