<?PHP

//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);

require_once 'timezone.php';

function url_origin( $s, $use_forwarded_host = false )
{
    $ssl      = ( ! empty( $s['HTTPS'] ) && $s['HTTPS'] == 'on' );
    $sp       = strtolower( $s['SERVER_PROTOCOL'] );
    $protocol = substr( $sp, 0, strpos( $sp, '/' ) ) . ( ( $ssl ) ? 's' : '' );
    $port     = $s['SERVER_PORT'];
    $port     = ( ( ! $ssl && $port=='80' ) || ( $ssl && $port=='443' ) ) ? '' : ':'.$port;
    $host     = ( $use_forwarded_host && isset( $s['HTTP_X_FORWARDED_HOST'] ) ) ? $s['HTTP_X_FORWARDED_HOST'] : ( isset( $s['HTTP_HOST'] ) ? $s['HTTP_HOST'] : null );
    $host     = isset( $host ) ? $host : $s['SERVER_NAME'] . $port;
    return $protocol . '://' . $host;
}

function full_url( $s, $use_forwarded_host = false )
{
    return url_origin( $s, $use_forwarded_host ) . $s['REQUEST_URI'];
}

$url = full_url( $_SERVER );
$arrurl = explode('?', $url, 2);
$url = $arrurl[0];

// Parameters
// FILTER = Tag or point key filter, can be a comma separated list or can be used * in tag to filter
// INITDATE = set a initial date for historical data (if not set will be returned real time data), format: 2010-03-11T06:40:04
// ENDDATE = set a and date for historical data (if not set will be returned all data from INITDATE up to now)
// SAMPLEINTERVAL = interval of data sampling

if ( isset($_GET['METADATA']) )
{
header("Content-Type: application/xml;charset=utf-8");
 
echo 
'<?xml version="1.0" encoding="utf-8"?>
<edmx:Edmx Version="1.0" xmlns:edmx="http://schemas.microsoft.com/ado/2007/06/edmx">
  <edmx:DataServices m:DataServiceVersion="3.0" m:MaxDataServiceVersion="3.0" xmlns:m="http://schemas.microsoft.com/ado/2007/08/dataservices/metadata"> 
    <Schema Namespace="OSHMI.ServiceContracts.OData" xmlns="http://schemas.microsoft.com/ado/2009/11/edm">
      <EntityType Name="realtime">
        <Key>
          <PropertyRef Name="TAG" />
        </Key>
        <Property Name="TAG" Type="Edm.String" Nullable="false" />
        <Property Name="POINT_KEY" Type="Edm.Int64" />
        <Property Name="VALUE" Type="Edm.Single" />
        <Property Name="STATUS" Type="Edm.String" />
        <Property Name="FAILED" Type="Edm.Boolean" />
        <Property Name="TIMESTAMP" Type="Edm.DateTime" />
      </EntityType>
      <EntityContainer Name="RealTimeDataService">
        <EntitySet Name="realtimedata" EntityType="OSHMI.ServiceContracts.OData.realtime"></EntitySet>
      </EntityContainer>      
    </Schema>
  </edmx:DataServices>
</edmx:Edmx>';
  
die();  
}

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
  $enddate = strtotime ( $_GET['ENDDATE'] . ' 23:59:59' );
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

header("Content-Type: application/atom+xml;type=feed;charset=utf-8");

echo '<?xml version="1.0" encoding="utf-8"?>
<feed xml:base="'.$url.'" xmlns="http://www.w3.org/2005/Atom" xmlns:d="http://schemas.microsoft.com/ado/2007/08/dataservices" xmlns:m="http://schemas.microsoft.com/ado/2007/08/dataservices/metadata" xmlns:georss="http://www.georss.org/georss" xmlns:gml="http://www.opengis.net/gml" m:context="'.$url.'/$metadata#realtimedata">
  <id>'.$url.'</id>
  <title type="text">realtimedata</title>';
echo "\n<updated>".date(DATE_ATOM)."</updated>\n"; // 2017-02-25T00:35:44Z
echo '<link rel="self" title="realtimedata" href="realtimedata" />';
echo "\n";

// Instancia o objeto PDO
$pdo = new PDO( 'sqlite:../db/hist.sl3' );
$pdo->exec ( "PRAGMA synchronous = OFF" );
$pdo->exec ( "PRAGMA journal_mode = WAL" );
$pdo->exec ( "PRAGMA locking_mode = NORMAL" );
$pdo->exec ( "PRAGMA cache_size = 5000" );
$pdo->exec ( "PRAGMA temp_store = MEMORY" );
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
  $stmt = $pdo->prepare(
           "SELECT 
              d.ID as TAG, 
              d.NPONTO as POINT_KEY, 
              h.VALOR as VALUE, 
              CASE d.TIPO WHEN 'D' THEN 
                CASE h.VALOR WHEN 0 THEN 
                d.ESTON ELSE d.ESTOFF END 
              WHEN 'A' THEN 
                d.UNIDADE END as STATUS, 
              h.FLAGS>127 as FAILED, 
              max(h.DATA) as TIMESTAMP 
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
else
if ( $intervalo == 0 )
  $stmt = $pdo->prepare(
           "SELECT 
              d.ID as TAG, 
              d.NPONTO as POINT_KEY, 
              h.VALOR as VALUE, 
              CASE d.TIPO WHEN 'D' THEN 
                CASE h.VALOR WHEN 0 THEN 
                d.ESTON ELSE d.ESTOFF END 
              WHEN 'A' THEN 
                d.UNIDADE END as STATUS, 
              h.FLAGS>127 as FAILED, 
              h.DATA as TIMESTAMP 
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
else
  $stmt = $pdo->prepare(
           "SELECT 
              d.ID as TAG, 
              d.NPONTO as POINT_KEY, 
              avg(h.VALOR) as VALUE, 
              CASE d.TIPO WHEN 'D' THEN 
                CASE h.VALOR WHEN 0 THEN 
                d.ESTON ELSE d.ESTOFF END 
              WHEN 'A' THEN 
                d.UNIDADE END as STATUS, 
              h.FLAGS>127 as FAILED, 
              (cast(h.DATA as int)/(60*$intervalo))*(60*$intervalo) as TIMESTAMP 
            from 
              hist h 
            join 
              dumpdb d 
              on d.nponto=h.nponto and
                 d.nponto in (select nponto from dumpdb where id like :filter or id in $filter2 or nponto in $filter2 )
            where 
              h.DATA >= :initdate and 
              h.DATA <= :enddate
            GROUP BY 
              d.nponto, 
              (cast(h.DATA as int)/(60*$intervalo))*(60*$intervalo)
           ");

$stmt->setFetchMode(PDO::FETCH_ASSOC);
  
$stmt->execute(array('filter' => $filter,
                     'initdate' => $initdate,
                     'enddate' => $enddate,                     
                    ));
  
foreach ($stmt as $row)  
  {
  echo "<entry>\n";
  echo  "<id>realtimedata('{$row['TAG']}')</id>\n";
  echo '<category term="OSHMI.ServiceContracts.OData.realtime" scheme="http://schemas.microsoft.com/ado/2007/08/dataservices/scheme" />'."\n";
  echo "<link rel=\"edit\" title=\"realtime\" href=\"realtimedata('{$row['TAG']}')\" />\n";
  echo "<title />\n";
  echo "<updated>".date(DATE_ATOM, $row['TIMESTAMP'])."</updated>\n";
  echo "<author>\n";
  echo "    <name />\n";
  echo "</author>\n";
  echo '  <content type="application/xml">'."\n";
  echo "    <m:properties>\n";

  $status = mb_convert_encoding($row['STATUS'], 'UTF-8', 'ISO-8859-1' ); 

  echo "      <d:TAG m:type=\"Edm.String\">{$row['TAG']}</d:TAG>\n";
  echo "      <d:POINT_KEY m:type=\"Edm.Int64\">{$row['POINT_KEY']}</d:POINT_KEY>\n";
  echo "      <d:VALUE m:type=\"Edm.Single\">{$row['VALUE']}</d:VALUE>\n";
  echo "      <d:STATUS m:type=\"Edm.String\">{$status}</d:STATUS>\n";
  $failed = $row['FAILED'] == 0 ? "false" : "true";
  echo "      <d:FAILED m:type=\"Edm.Boolean\">$failed</d:FAILED>\n";
  $edmdate = date(DATE_ATOM, $row['TIMESTAMP']);
  echo "      <d:TIMESTAMP m:type=\"Edm.DateTime\">{$edmdate}</d:TIMESTAMP>\n";

  echo "    </m:properties>\n";
  echo "  </content>\n";
  echo "</entry>\n";
  }
  
echo '</feed>';
}
catch( PDOException $Exception ) 
{
    echo $Exception.Message;
    die();   
}

$pdo = null;

?>
