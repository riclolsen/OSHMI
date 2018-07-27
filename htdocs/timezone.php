<?php

// Default timezone for PHP
//
// See list of supported timezones: http://www.php.net/manual/en/timezones.php
// 
// Change value in c:\oshmi\conf\hmi.ini [webserver]/TIMEZONE=
//

$tz = 'America/Sao_Paulo';
$match = [];
$ini = @file_get_contents("../conf/hmi.ini");
if ($ini === false)
  $ini = file_get_contents("../../conf/hmi.ini");
preg_match('/^TIMEZONE=(.*)$/m', $ini, $match);
if ( count($match) > 0 )
  $tz = $match[1];
date_default_timezone_set($tz); 
?>
