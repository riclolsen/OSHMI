<?php
error_reporting(0);
ini_set('display_errors', 0);

header('Content-Type: application/json');
$feed = new DOMDocument();

$url=trim(urldecode($_GET["URL"]));
if ( strpos($url, "file:") === 0 )
  die();

if ( !$feed->load($url) )
  die("");
  
// echo $feed->saveXML();

$json = array();

$json['title'] = $feed->getElementsByTagName('channel')->item(0)->getElementsByTagName('title')->item(0)->firstChild->nodeValue;
$json['description'] = $feed->getElementsByTagName('channel')->item(0)->getElementsByTagName('description')->item(0)->firstChild->nodeValue;
// $json['link'] = $feed->getElementsByTagName('channel')->item(0)->getElementsByTagName('link')->item(0)->firstChild->nodeValue;

$items = $feed->getElementsByTagName('channel')->item(0)->getElementsByTagName('item');

$rdf = $feed->getElementsByTagName('rdf:RDF');
if ( count($rdf) > 0 )
  $items = $feed->getElementsByTagName('item');  

$json['item'] = array();
$i = 0;

foreach($items as $item) {

   $title = $item->getElementsByTagName('title')->item(0)->firstChild->nodeValue;
   $description = strip_tags($item->getElementsByTagName('description')->item(0)->firstChild->nodeValue);
   //$pubDate = $item->getElementsByTagName('pubDate')->item(0)->firstChild->nodeValue;
   //$guid = $item->getElementsByTagName('guid')->item(0)->firstChild->nodeValue;
   
   $json['item'][$i]['title'] = $title;
   $json['item'][$i]['description'] = $description;
   $i++;
   //$json['item'][$i++]['pubdate'] = $pubDate;
   //$json['item'][$i++]['guid'] = $guid;   
}

echo json_encode($json);
?>