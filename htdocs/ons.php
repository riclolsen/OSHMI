<?php
// Serve como proxy para carregar conteÃºdo dinÃ¢mico de origem diferente em javascript

error_reporting(0);
ini_set('display_errors', 0);

function get_fcontent( $url,  $javascript_loop = 0, $timeout = 15 ) {
    $url = str_replace( "&amp;", "&", urldecode(trim($url)) );

    $cookie = tempnam ("/tmp", "CURLCOOKIE");
    $ch = curl_init();
    curl_setopt( $ch, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 5.1; rv:1.7.3) Gecko/20041001 Firefox/0.10.1" );
    curl_setopt( $ch, CURLOPT_URL, $url );
    curl_setopt( $ch, CURLOPT_COOKIEJAR, $cookie );
    curl_setopt( $ch, CURLOPT_FOLLOWLOCATION, true );
    curl_setopt( $ch, CURLOPT_ENCODING, "" );
    curl_setopt( $ch, CURLOPT_RETURNTRANSFER, true );
    curl_setopt( $ch, CURLOPT_AUTOREFERER, true );
    curl_setopt( $ch, CURLOPT_SSL_VERIFYPEER, false );    # required for https urls
    curl_setopt( $ch, CURLOPT_CONNECTTIMEOUT, $timeout );
    curl_setopt( $ch, CURLOPT_TIMEOUT, $timeout );
    curl_setopt( $ch, CURLOPT_MAXREDIRS, 10 );
    $content = curl_exec( $ch );
    $response = curl_getinfo( $ch );
    curl_close ( $ch );

    if ($response['http_code'] == 301 || $response['http_code'] == 302) {
        ini_set("user_agent", "Mozilla/5.0 (Windows; U; Windows NT 5.1; rv:1.7.3) Gecko/20041001 Firefox/0.10.1");

        if ( $headers = get_headers($response['url']) ) {
            foreach( $headers as $value ) {
                if ( substr( strtolower($value), 0, 9 ) == "location:" )
                    return get_fcontent( trim( substr( $value, 9, strlen($value) ) ) );
            }
        }
    }

    if (    ( preg_match("/>[[:space:]]+window\.location\.replace\('(.*)'\)/i", $content, $value) || preg_match("/>[[:space:]]+window\.location\=\"(.*)\"/i", $content, $value) ) && $javascript_loop < 5) {
        return get_fcontent( $value[1], $javascript_loop+1 );
    } else {
        return array( $content, $response );
    }
}



$url=trim(urldecode($_GET["URL"]));
if ( stripos($url, "file:") === 0 )
  die();
$lurl=get_fcontent($url);
$content = $lurl[0];
$type = "application/json";
foreach ($lurl[1] as $value) {
    if (preg_match('/^Content-Type:/i', $value)) {
        // Successful match
        header($value,true);  
    }
}

if ($type!="")    
  header('Content-Type: '.$type, true);

$jsonct = json_decode ( $content, true );
$rws = $jsonct["rows"];

$vals=array();
$tm =  strtotime("today");

function process($item, $key)
{
  static $i=0;
  global $vals, $tm;
  
  // echo "$key holds $item $i\n";
  if ( $i % 2 )    
  {
    $vals[($i-1)/2]["x"] = 1000*($tm + 60*($i-1)/2);
    $vals[($i-1)/2]["y"] = $item/1000;
  }
    
  $i++;
}

array_walk_recursive($rws, 'process');

// print_r($vals);

echo json_encode($vals);
// echo mb_convert_encoding($content, 'UTF-8', mb_detect_encoding($content, 'UTF-8, ISO-8859-1', true));

?>