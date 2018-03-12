<?php
// acerta variáveis globais para várias versões do PHP
extract($_REQUEST, EXTR_PREFIX_ALL|EXTR_SKIP, 'p');

$im = imagecreatefrompng ("$p_filename");
imagepng ($im);
?>

