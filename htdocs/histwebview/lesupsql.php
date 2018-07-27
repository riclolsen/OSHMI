<?php

//----------------------------------------------------------------------------

// Classe para acesso ao Histórico de Supervisão, SQL e SUP
// Ver exemplo de uso ao final
// OSHMI/Open Substation HMI - Copyright 2008-2018 - Ricardo L. Olsen

header("Content-Type: text/html; charset=UTF-8");

//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);

require_once '../timezone.php';

function GetByte($filehandle) 
  { 
	  return ord(fread($filehandle, 1)); 
  }
  
function GetInt($filehandle) // pega um inteiro de 16 bits, sem sinal, little endian do arquivo
  {  
	  return current(unpack("v", fread($filehandle, 2)));
  }
  
function EhHorarioVerao($data = "")
{
if ($data=="")
  $data = date("d/m/Y");
$data = mktime(0,0,0, substr($data,3,2),substr($data,0,2),substr($data,6,4));
if ( $data >= mktime(0,0,0,10, 3,1999) && $data < mktime(0,0,0, 2,27,2000) )
  return TRUE;
if ( $data >= mktime(0,0,0,10, 8,2000) && $data < mktime(0,0,0, 2,18,2001) )
  return TRUE;
if ( $data >= mktime(0,0,0,10,14,2001) && $data < mktime(0,0,0, 2,17,2002) )
  return TRUE;
if ( $data >= mktime(0,0,0,11, 3,2002) && $data < mktime(0,0,0, 2,16,2003) )
  return TRUE;
if ( $data >= mktime(0,0,0,10, 3,2003) && $data < mktime(0,0,0, 2,16,2004) )
  return TRUE;
if ( $data >= mktime(0,0,0,11, 2,2004) && $data < mktime(0,0,0, 2,20,2005) )
  return TRUE;
if ( $data >= mktime(0,0,0,11, 5,2006) && $data < mktime(0,0,0, 2,25,2007) )
  return TRUE;
if ( $data >= mktime(0,0,0,10, 14,2007) && $data < mktime(0,0,0,2,17,2008) )
  return TRUE;  
return FALSE;
}

//----------------------------------------------------------------------------

class THistorico
{
 var $CaminhoSup="/hstsup/niv01/";
 var $lista_pontos; // lista com número dos pontos
 var $lista_id;     // lista com id dos pontos
 var $lista_tipo;   // lista com o tipo dos pontos
 var $horainicial;
 var $horafinal;
 var $dia;
 var $mes;
 var $ano;
 var $intervalo; // 0=tudo , 1=1 minuto, 5=5 minutos, ...
 var $stats;
 var $valores;
 var $origem;    // 0=tanto faz, 1=arquivos SUP, 2=MySQL, 3=SAGE

//----------------------------------------------------------------------------

 function THistorico()
 {
 // @date_default_timezone_set( date_default_timezone_get() );  // date_default_timezone_set('UTC');
 
 $this->define_data(date("d/m/Y"));
 $this->horainicial='00:00:00';
 $this->horafinal='23:59:59';
 $this->intervalo=1;
 $this->lista_pontos=array();
 $this->lista_id=array();
 $this->lista_tipo=array();
 $this->stats=TRUE;
 $this->valores=TRUE;
 $this->origem=0; // origem qualquer
 
 
 // acerta caminho de inclusão, para funcionar pele linha de comando e WEB    
 $this->CaminhoSup = "./"; 
 }

//----------------------------------------------------------------------------

 function inclui_ponto($pto)
 {
 array_push($this->lista_pontos, $pto);
 }

function ajusta_chaves()
 {
 $dbb = new PDO( 'sqlite:../../db/dumpdb.sl3' );
 $dbb->exec ( "PRAGMA synchronous = OFF" );
 $dbb->exec ( "PRAGMA journal_mode = WAL" );
 $dbb->exec ( "PRAGMA locking_mode = NORMAL" );

 for ( $i = 0; $i < count( $this->lista_pontos ); $i++ ) 
   {
       
     $query = 
" 
SELECT 
  nponto, id, tipo
FROM 
  dumpdb
WHERE 
  (id='{$this->lista_pontos[$i]}' or nponto='{$this->lista_pontos[$i]}')
";

     $found = 0;
     foreach ( $dbb->query( $query ) as $row )
       {
       $this->lista_pontos[$i] = $row["NPONTO"];
       $this->lista_id[$i] = $row["ID"];
       $this->lista_tipo[$i] = $row["TIPO"];
       $found = 1;
       break;
       }
       
     if ( $found ==0 )  
       {
       array_splice($this->lista_pontos,$i,1);  // não encontrado: remove ponto da lista
       }
   }
 }

//----------------------------------------------------------------------------

 function define_data($dta)
 {
 $tok = strtok ($dta,"/"); $this->dia = sprintf("%02d", $tok);
 $tok = strtok ("/");      $this->mes = sprintf("%02d", $tok);
 $tok = strtok ("/");      $this->ano = $tok;
//print " ---> AQUI!!! $this->dia  $this->mes   $this->ano";
 }

//----------------------------------------------------------------------------

 // busca direcionada ao SUP
 function busca_sup($lst_ptos=array(), $data="")
  {

  if (count($lst_ptos))
    $this->lista_pontos=$lst_ptos;

 $this->ajusta_chaves();

 if ($data!="")
    $this->define_data($data);

  $retorno=array();

  if ($this->ano>=2000)
    {
    $OL_Arquivo=sprintf ("%s%04d_%02d/H%02d%02d%02dA.SUP",
                          $this->CaminhoSup, $this->ano, $this->mes,
                          $this->dia, $this->mes, $this->ano%100);
    }
  else
    {
    $OL_Arquivo=sprintf ("%s%02d_%02d/H%02d%02d%02dA.SUP",
                          $this->CaminhoSup, $this->mes, $this->ano%100,
                          $this->dia, $this->mes, $this->ano%100);
    }

  if ( ( $OL_Hand = fopen( $OL_Arquivo, "rb" ) )
       == FALSE )
    {
      return $retorno;
    }

  // le cabecalho fixo
  $Versao     = GetInt($OL_Hand);       // bytes  0 -  1
  $Dia        = GetByte($OL_Hand);      // byte   2
  $Mes        = GetByte($OL_Hand);      // byte   3
  $Ano        = GetInt($OL_Hand);       // bytes  4 -  5
  $ProximaSeq = GetByte($OL_Hand);      // byte   6
  $nada       = GetByte($OL_Hand);      // byte   7
  $NumPtosDig = GetInt($OL_Hand);       // bytes  8 -  9
  $NumPtosAna = GetInt($OL_Hand);       // bytes 10 - 11
  $PrimMinuto = GetInt($OL_Hand);       // bytes 12 - 13
  $UltiMinuto = GetInt($OL_Hand);       // bytes 14 - 15
  $lixo       = fread($OL_Hand, 8);     // bytes 16 - 23

  $TFD =  2*intval(($NumPtosDig+7)/8);         // tamanho foto digital
  $TFA = 17*intval(($NumPtosAna+7)/8);         // tamanho foto analogica
  $TFT = $TFD + $TFA;
  $TCT = 1464 + 2*$NumPtosDig + 3*$NumPtosAna; // tamanho cab total

  for ($i=0; $i<1440; $i++)
    {
    $StatusGrav[$i] = GetByte( $OL_Hand );    // bytes 24 - 1463
    }

  for ($i=0; $i<$NumPtosDig; $i++)
    {
    $Digitais[$i] = GetInt( $OL_Hand );    // bytes 24 - 1463
    }

  for ($i=0; $i<$NumPtosAna; $i++)
    {
    $Analogicos[$i] = GetInt( $OL_Hand );    // bytes 24 - 1463
    $CasaDec[$i] = GetByte( $OL_Hand );    // bytes 24 - 1463
    }

  foreach ($this->lista_pontos as $pto)  // para cada ponto da lista, busca valor no sup
    {
    $posdig=NULL;
    $posana=array_search ($pto, $Analogicos, FALSE);
    if (gettype($posana)!="integer")
      {
      $posdig=array_search ($pto, $Digitais, FALSE);
      if (gettype($posdig)!="integer")
        continue;
      }

    $hora=$this->horainicial;
    $valor=0;
    $falha=128;
    $minini=substr($this->horainicial,0,2)*60+substr($this->horainicial,3,2);
    $minfim=substr($this->horafinal,0,2)*60+substr($this->horafinal,3,2);
    if ($minfim>$UltiMinuto)
      $minfim=$UltiMinuto;
    if ($this->stats)
      {
      $hmin="";
      $hmax="";
      $min=9999999;
      $max=-9999999;
      $med=0;
      $cont_falhas=0;
      }

    $linhas=array();
    
    $interv=$this->intervalo;
    if ($interv==0)
      $interv=1;
    for ($i=$minini; $i<=$minfim; $i+=$interv)
      {
      if ($i==0 && $StatusGrav[$i]==0)
         continue;

      $posfoto = $TCT + $i*$TFT;
      $hora=sprintf("%02d:%02d:00", $i/60, $i%60);

      if (gettype($posdig)!="integer")
         { // analogico
         if ($StatusGrav[$i]) // testa se o minuto foi gravado
           { // sim, o minuto foi gravado
           $pos = $posfoto + $TFD + 17*intval($posana/8);
           fseek($OL_Hand, $pos);
           $falha=(GetByte($OL_Hand)&(0x01<<$posana%8))?128:0;
           $falha|=0x20;
           fseek($OL_Hand, 2*($posana%8), SEEK_CUR);
           $valor=GetInt($OL_Hand);

           if ($valor>32767)
             $valor=$valor-65536;

           switch ($CasaDec[$posana])
             {
             case 1:  $valor/=10;   break;
             case 2:  $valor/=100;  break;
             case 3:  $valor/=1000; break;
             default:               break;
             }
           }
         else
           $falha=128|0x20; // o minuto não foi gravado, mantém o valor do anterior com falha
         }
      else
         { // digital
         if ($StatusGrav[$i]) // testa se o minuto foi gravado
           { // sim, o minuto foi gravado
           $pos = $posfoto + 2*intval($posdig/8);
           fseek($OL_Hand, $pos);
           $falha=(GetByte($OL_Hand)&(0x01<<$posdig%8))?128:0;
           $valor=(GetByte($OL_Hand)&(0x01<<$posdig%8))?1:0;
           }
         else
           $falha=128; // o minuto não foi gravado, mantém o valor do anterior com falha

         }

         if ($this->stats)
           {
           if ($valor>$max)
             {
             $max=$valor;
             $hmax=$hora;
             }
           if ($valor<$min)
             {
             $min=$valor;
             $hmin=$hora;
             }
           $med+=$valor;
           if ($falha>=128)
             $cont_falhas++;
           }

         if ( $this->valores )
           {
           $colunas=array();
           array_push($colunas,$hora);
           array_push($colunas,$valor);
           array_push($colunas,$falha);
           array_push($linhas, $colunas);
           }
      }

    if ($this->stats && $i!=0)
      {
      $colunas=array();
      array_push($colunas,"--:--:--");
      array_push($colunas,intval(10000*($i-$cont_falhas)/$i)/100);
      array_push($colunas,"Availability (%)");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,"--:--:--");
      array_push($colunas,intval(100*$med/60)/100);
      array_push($colunas,"Integration (hour)");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,"--:--:--");
      array_push($colunas,intval(100*$med/$i)/100);
      array_push($colunas,"Avg");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,$hmin);
      array_push($colunas,$min);
      array_push($colunas,"Min");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,$hmax);
      array_push($colunas,$max);
      array_push($colunas,"Max");
      array_unshift($linhas, $colunas);
      }

    $retorno[$pto]=$linhas;
    }

  fclose($OL_Hand);

  return $retorno;
  }

//----------------------------------------------------------------------------

// busca direcionada ao SQLITE
function busca_sql($lst_ptos=array(), $data="")
  {
  if (count($lst_ptos))
    $this->lista_pontos=$lst_ptos;

  $this->ajusta_chaves();

  try 
    {
    $db = new PDO( 'sqlite:../../db/hist.sl3' );
    $db->exec ( "PRAGMA synchronous = OFF" );
    $db->exec ( "PRAGMA journal_mode = WAL" );
    $db->exec ( "PRAGMA locking_mode = NORMAL" );
    }
  catch( PDOException $e )
    {
    die("Erro!");
    }
  
  if ($data!="")
    $this->define_data($data);

  $retorno=array();

  $cnt=0;
  foreach ( $this->lista_pontos as $pto )
    {
    $tipo=$this->lista_tipo[$cnt];
    $cnt++;

    if ($this->intervalo==0 || $tipo=='D')
      $filtra_hora="";
    else
      $filtra_hora="and hora like '%%:%%:00'";
    $DATAINI = sprintf("%02d/%02d/%04d", $this->dia, $this->mes, $this->ano);
    $DATEFMT = "DD/MM/YYYY";
    if ( $DATEFMT == "DD/MM/YYYY" )
      $DATAINI = str_replace ("/", "-", $DATAINI);
    $FILTDH = strtotime($DATAINI." "."00:00:00");    
    
    // pego a última gravação antes da data inicial como valor inicial do dia solicitado
//    $query = "SELECT data as DATA, valor as VALOR, flags as FLAGS, 1 as LIXO from hist where nponto=$pto and data>=$FILTDH and data<= ($FILTDH + 60*60*24) ".
//             "union SELECT $FILTDH as DATA, valor as VALOR, flags as FLAGS, max(data) as LIXO from hist where nponto=$pto and data<$FILTDH order by data ";
    $query = "SELECT data as DATA, valor as VALOR, flags as FLAGS from hist where nponto=$pto and data>=$FILTDH and data<= ($FILTDH + 60*60*24) ".
             "union SELECT $FILTDH DATA, valor as VALOR, flags as FLAGS from hist where nponto=$pto and data=(select data from hist where nponto=$pto and data<$FILTDH order by data desc limit 1 ) order by data ";

    $i=0;
    if ($this->stats)
      {
      $hmin="";
      $hmax="";
      $min=9999999;
      $max=-9999999;
      $med=0;
      $cnt_med=0;
      $cont_falhas=0;
      }
      
    $linhas = array();  

    foreach ( $db->query( $query ) as $row )
      {
          $i++;

          $colunas=array();
          $dt = getdate( $row['DATA'] );
          $hora = sprintf( "%02d:%02d:%02d", $dt["hours"], $dt["minutes"], $dt["seconds"] );
          array_push($colunas,$hora);
          array_push($colunas,$valor=$row["VALOR"]);
          $flags=(int)$row["FLAGS"];
          if ($tipo=='A')
            $flags&=0xFE;  
          
          // apaga o bit Q, será usado para Integridade/Variacao
          $flags&=0xBF;
                    
          array_push($colunas,$flags);

          if ($this->stats)
            {
            if ($flags>=128)
              $cont_falhas++;
//          else // só considera valores ok para máximo e mínimo
              {
              if ($valor>$max)
                {
                $max=$valor;
                $hmax=$hora;
                }
              if ($valor<$min)
                {
                $min=$valor;
                $hmin=$hora;
                }
              }

            if ($hora[6]=='0' && $hora[7]=='0') // só o minuto redondo para cálculo da hora
              {
              $med+=$valor;
              $cnt_med++;
              }
            }

          if ( $this->valores )
            array_push($linhas, $colunas);
      }

    if ($this->stats && $i!=0)
      {
      $colunas=array();
      array_push($colunas,"--:--:--");
      // array_push($colunas,intval(10000*($i-$cont_falhas)/$i)/100);
      array_push($colunas,0);
      // array_push($colunas,"Availability (%)");
      array_push($colunas,"-");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,"--:--:--");
      // array_push($colunas,intval(100*$med/60)/100);
      array_push($colunas,0);
      //array_push($colunas,"Integration (hour)");
      array_push($colunas,"-");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,"--:--:--");
      //if ( $cnt_med > 0 )
      //  array_push($colunas,intval(100*$med/$cnt_med)/100);
      //else  
      //  array_push($colunas,0);
      array_push($colunas,0);
      // array_push($colunas,"Média");
      array_push($colunas,"-");
      
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,$hmin);
      array_push($colunas,$min);
      array_push($colunas,"Min");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,$hmax);
      array_push($colunas,$max);
      array_push($colunas,"Max");
      array_unshift($linhas, $colunas);
      }
    $retorno[$pto]=$linhas;
    }

  return $retorno;
  }

//----------------------------------------------------------------------------

// busca direcionada ao OSHMI POSTGRESQL

function busca_pgsql($lst_ptos=array(), $data="")
  {
  if (count($lst_ptos))
    $this->lista_pontos=$lst_ptos;

  $this->ajusta_chaves();

  try 
    {
      $db = new PDO( 'pgsql:host=127.0.0.1;port=5432;dbname=oshmi;user=grafana;password=oshmi;sslmode=disable;' );
	  if ($data!="")
		$this->define_data($data);

	  $retorno=array();

	  $cnt=0;
	  foreach ( $this->lista_pontos as $pto )
		{
		$tipo=$this->lista_tipo[$cnt];
		$cnt++;

		if ($this->intervalo==0 || $tipo=='D')
		  $filtra_hora="";
		else
		  $filtra_hora="and hora like '%%:%%:00'";
		$DATAINI = sprintf("%02d/%02d/%04d", $this->dia, $this->mes, $this->ano);
		$DATEFMT = "DD/MM/YYYY";
		if ( $DATEFMT == "DD/MM/YYYY" )
		  $DATAINI = str_replace ("/", "-", $DATAINI);
		$FILTDH = strtotime($DATAINI." "."00:00:00");    
		
		// pego a última gravação antes da data inicial como valor inicial do dia solicitado
		$query =  "
		          SELECT 
				     UNIX_TIMESTAMP as data,
					 VALUE as valor,
					 FLAGS as flags
				  FROM 
				     historical_data
				  WHERE 
				    POINT_KEY=$pto and 
				    UNIX_TIMESTAMP>=$FILTDH and 
					UNIX_TIMESTAMP<=($FILTDH + 60*60*24)
				     
				  UNION
                  SELECT				  
				     $FILTDH as data,
					 VALUE as valor,
					 FLAGS as flags
				  FROM 
				     historical_data
				  WHERE 
				    POINT_KEY=$pto and 
					UNIX_TIMESTAMP=(select UNIX_TIMESTAMP from historical_data where POINT_KEY=$pto and UNIX_TIMESTAMP<$FILTDH order by UNIX_TIMESTAMP desc limit 1 )
				  order by data
		          ";

		$i=0;
		if ($this->stats)
		  {
		  $hmin="";
		  $hmax="";
		  $min=9999999;
		  $max=-9999999;
		  $med=0;
		  $cnt_med=0;
		  $cont_falhas=0;
		  }
      
        }

    $linhas = array();  
    foreach ( $db->query( $query ) as $row )
      {
          $i++;

          $colunas=array();
          $dt = getdate( $row['data'] );
          $hora = sprintf( "%02d:%02d:%02d", $dt["hours"], $dt["minutes"], $dt["seconds"] );
          array_push($colunas,$hora);
          array_push($colunas,$valor=$row["valor"]);
          $flags=(int)$row["flags"];
          if ($tipo=='A')
            $flags&=0xFE;  
          
          // apaga o bit Q, será usado para Integridade/Variacao
          $flags&=0xBF;
                    
          array_push($colunas,$flags);

          if ($this->stats)
            {
            if ($flags>=128)
              $cont_falhas++;
//          else // só considera valores ok para máximo e mínimo
              {
              if ($valor>$max)
                {
                $max=$valor;
                $hmax=$hora;
                }
              if ($valor<$min)
                {
                $min=$valor;
                $hmin=$hora;
                }
              }

            if ($hora[6]=='0' && $hora[7]=='0') // só o minuto redondo para cálculo da hora
              {
              $med+=$valor;
              $cnt_med++;
              }
            }

          if ( $this->valores )
            array_push($linhas, $colunas);
      }

    if ($this->stats && $i!=0)
      {
      $colunas=array();
      array_push($colunas,"--:--:--");
      // array_push($colunas,intval(10000*($i-$cont_falhas)/$i)/100);
      array_push($colunas,0);
      // array_push($colunas,"Availability (%)");
      array_push($colunas,"-");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,"--:--:--");
      // array_push($colunas,intval(100*$med/60)/100);
      array_push($colunas,0);
      //array_push($colunas,"Integration (hour)");
      array_push($colunas,"-");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,"--:--:--");
      //if ( $cnt_med > 0 )
      //  array_push($colunas,intval(100*$med/$cnt_med)/100);
      //else  
      //  array_push($colunas,0);
      array_push($colunas,0);
      // array_push($colunas,"Média");
      array_push($colunas,"-");
      
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,$hmin);
      array_push($colunas,$min);
      array_push($colunas,"Min");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,$hmax);
      array_push($colunas,$max);
      array_push($colunas,"Max");
      array_unshift($linhas, $colunas);
      }
    $retorno[$pto]=$linhas;
    }
  catch( PDOException $e )
    {
	// echo "Erro: ".$e->getMessage();
	// print_r($dbsde->errorInfo());
	}

  return $retorno;
  }

//----------------------------------------------------------------------------

 // busca genérica no histórico: pega do SUP ou do SQL, o que for melhor
 function busca_hist($lst_ptos=array(), $data="")
  {
  $retorno=array();

  if (count($lst_ptos))
    $this->lista_pontos=$lst_ptos;

  if ( $this->intervalo == 0 ) // o intervalo é zero (tudo): SQL
    {
    foreach ( $this->lista_pontos as $pto )
      {
      $lst=array();
      array_push($lst, $pto);
      
      switch ($this->origem)
        {
          case 0: // qualquer origem
            default:  
            $ret=$this->busca_pgsql($lst, $data);
            break;
          case 1: // arquivos SUP
            $ret=$this->busca_sup($lst, $data);
            break;
          case 2: // histórico OSHMI sqlite
            $ret=$this->busca_sql($lst, $data);
            break;
          case 3: // histórico SAGE
            $ret=$this->busca_sage_sql($lst, $data, 0);
            break;
          case 4: // histórico SAGE: Valores Estimados
            $ret=$this->busca_sage_sql($lst, $data, 1);
            break;
          case 5: // histórico OHSMI POSTGRESQL
            $ret=$this->busca_pgsql($lst, $data, 1);
            break;
        }
      
      // list($ponto, $linhas)=each($ret);
	  $ponto = key($ret);
	  $linhas = current($ret);
      if ( count($linhas)<=5 && $this->origem==0 ) // se não achou no SQL, busca no SUP
        {
        $ret=$this->busca_sql($lst, $data);
        // list($ponto, $linhas)=each($ret);
	    $ponto = key($ret);
        $linhas = current($ret);
        
        if ( count($linhas)<=5 ) // se não achou no SUP, busca no SAGE
            {
            $ret=$this->busca_sage_sql($lst, $data);
            // list($ponto, $linhas)=each($ret);
            $ponto = key($ret);
	        $linhas = current($ret);
            }
        }
      $retorno[$ponto]=$linhas;
      }
    }
  else
    {
    foreach ( $this->lista_pontos as $pto )
      {
      $lst=array();
      array_push($lst, $pto);
      switch ($this->origem)
        {
          case 0: // qualquer origem
          default:  
            $ret=$this->busca_pgsql($lst, $data);
            break;
          case 1: // arquivos SUP
            $ret=$this->busca_sup($lst, $data);
            break;
          case 2: // histórico OSHMI sqlite
            $ret=$this->busca_sql($lst, $data);
            break;
          case 3: // histórico SAGE
            $ret=$this->busca_sage_sql($lst, $data);
            break;
          case 4: // histórico SAGE: Valores Estimados
            $ret=$this->busca_sage_sql($lst, $data, 1);
            break;
          case 5: // histórico OHSMI POSTGRESQL
            $ret=$this->busca_pgsql($lst, $data, 1);
            break;
        }
      // list($ponto, $linhas)=each($ret);
  	  $ponto = key($ret);
	  $linhas = current($ret);

      if ( count($linhas)<=5 && $this->origem==0 ) // se não achou no SUP, busca no SQL
        {
        $ret=$this->busca_sql($lst, $data);
        // list($ponto, $linhas)=each($ret);
        $ponto = key($ret);
	    $linhas = current($ret);
        if ( count($linhas)<=5 ) // se não achou no MySQL, busca no SAGE
            {
            $ret=$this->busca_sage_sql($lst, $data);
            //list($ponto, $linhas)=each($ret);
			$ponto = key($ret);
            $linhas = current($ret);
            }
        }
      $retorno[$ponto]=$linhas;
      }
    }

  return $retorno;
  }

//----------------------------------------------------------------------------

/* Busca de foto no arquivo SUP
* 
*  Parâmetros: data e hora, sem a data busca na data atual e sem a hora devolve o último minuto gravado
* 
*  Retorno da função busca_foto_sup
*  
*                            colunas
* 
*  Retorno -+- NPonto - valor, flags, identific
*              NPonto - valor, flags, identific
*              ...
*              NPonto - valor, flags, identific
*/
 function busca_foto_sup(&$data, &$hora)
  {
  if ($data!="") // se foi recebida uma data, usa esta. Senão usa data atual.
    {
	$this->define_data($data);
	}

  // para retornar a data atual ou manter a fornecida
  $data=sprintf("%02d/%02d/%02d", $this->dia, $this->mes, $this->ano);

  $retorno=array();

  if ($this->ano>=2000)
    {
    $OL_Arquivo=sprintf ("%s%04d_%02d/H%02d%02d%02dA.SUP",
                          $this->CaminhoSup, $this->ano, $this->mes,
                          $this->dia, $this->mes, $this->ano%100);
    }
  else
    {
    $OL_Arquivo=sprintf ("%s%02d_%02d/H%02d%02d%02dA.SUP",
                          $this->CaminhoSup, $this->mes, $this->ano%100,
                          $this->dia, $this->mes, $this->ano%100);
    }

  if ( ( $OL_Hand = fopen( $OL_Arquivo, "rb" ) )
       == FALSE )
    {
      return $retorno;
    }

  // le cabecalho fixo
  $Versao     = GetInt($OL_Hand);       // bytes  0 -  1
  $Dia        = GetByte($OL_Hand);      // byte   2
  $Mes        = GetByte($OL_Hand);      // byte   3
  $Ano        = GetInt($OL_Hand);       // bytes  4 -  5
  $ProximaSeq = GetByte($OL_Hand);      // byte   6
  $nada       = GetByte($OL_Hand);      // byte   7
  $NumPtosDig = GetInt($OL_Hand);       // bytes  8 -  9
  $NumPtosAna = GetInt($OL_Hand);       // bytes 10 - 11
  $PrimMinuto = GetInt($OL_Hand);       // bytes 12 - 13
  $UltiMinuto = GetInt($OL_Hand);       // bytes 14 - 15
  $lixo       = fread($OL_Hand, 8);     // bytes 16 - 23

  $TFD =  2*intval(($NumPtosDig+7)/8);         // tamanho foto digital
  $TFA = 17*intval(($NumPtosAna+7)/8);         // tamanho foto analogica
  $TFT = $TFD + $TFA;
  $TCT = 1464 + 2*$NumPtosDig + 3*$NumPtosAna; // tamanho cab total

  if ($hora=="")
  { // se não foi fornecida a hora, pega do último minuto gravado
    $min=$UltiMinuto;
    $hora=sprintf("%02d:%02d:00", $min/60, $min%60);
	}
  else	
    {
    $doispontos=strpos($hora, ":");
    $min=substr($hora,0,$doispontos)*60+substr($hora,$doispontos+1,2);
    }
  
  for ($i=0; $i<1440; $i++)
    {
    $StatusGrav[$i] = GetByte( $OL_Hand );    // bytes 24 - 1463
    }

  $posfoto = $TCT + $min*$TFT;
	
  for ($i=0; $i<$NumPtosDig; $i++)
    {
    $Digitais[$i] = GetInt( $OL_Hand );    // bytes 24 - 1463
    }
	
  for ($i=0; $i<$NumPtosAna; $i++)
    {
    $Analogicos[$i] = GetInt( $OL_Hand );    // bytes 24 - 1463
    $CasaDec[$i] = GetByte( $OL_Hand );    // bytes 24 - 1463
    }

  $tem_lista = count ($this->lista_pontos);
  
  for ($i=0; $i<$NumPtosDig; $i++)
    {
    $pos = $posfoto + 2*intval($i/8);
    fseek($OL_Hand, $pos);
    $falha=(GetByte($OL_Hand)&(0x01<<$i%8))?128:0;
    $valor=(GetByte($OL_Hand)&(0x01<<$i%8))?1:0;
    if (!$tem_lista || in_array($Digitais[$i], $this->lista_pontos))
      {
      $colunas=array();
	  array_push($colunas,$valor);
      array_push($colunas,$falha);
      $retorno[$Digitais[$i]]=$colunas;
      }
    }

  for ($i=0; $i<$NumPtosAna; $i++)
    {
    $valor=0;
    $falha=128;
    $pos = $posfoto + $TFD + 17*intval($i/8);
    fseek($OL_Hand, $pos);
    $falha=(GetByte($OL_Hand)&(0x01<<$i%8))?128:0;
    $falha|=0x20; // sinaliza ponto analógico
    fseek($OL_Hand, 2*($i%8), SEEK_CUR);
    $valor=GetInt($OL_Hand);

    if ($valor>32767)
      $valor=$valor-65536;

    switch ($CasaDec[$i])
      {
      case 1:  $valor/=10;   break;
      case 2:  $valor/=100;  break;
      case 3:  $valor/=1000; break;
      default:               break;
      }

   if (!$tem_lista || in_array($Analogicos[$i], $this->lista_pontos))
      {
      $colunas=array();
      array_push($colunas,$valor);
      array_push($colunas,$falha);
      $retorno[$Analogicos[$i]]=$colunas;
      }
    }

  fclose($OL_Hand);
  return $retorno;
  }
  
function busca_foto(&$data, &$hora)
  {
  $rt=$this->busca_foto_sup($data, $hora);
  
  require_once("peardb_login.php") ;
  $db=&DBLogin("SQL_BANCOTR_CONSULTA");
  if (DB::isError($db)) 
     die("Erro de conexão ao banco Bancotr Consulta em ".$data. " com erro :".$db->getMessage());

  if ($data!="")
    $this->define_data($data);

  $retorno=array();

  $query = "SELECT nponto, valor, flags from h{$this->ano}_$this->mes where data='$this->ano/$this->mes/$this->dia' and hora='{$hora}'";

    $result = & $db->query($query);

    if (DB::isError($result))
       return;
       
    if ($result->numRows()==0)
       return;
       
        while( $result->fetchInto($line) )
          {
          $colunas=array();
          array_push($colunas,$valor=$line[1]);
          $flags=(int)$line[2];
          // apaga o bit Q, será usado para Integridade/Variacao
          $flags&=0xBF;
          array_push($colunas,$flags);
          $rt[$line[0]]=$colunas;
          }

  $db->disconnect();

  return $rt;
  }
//----------------------------------------------------------------------------

/*
*  Retorno da função busca_foto_sup
*  
*                            colunas
* 
*  Retorno -+- NPonto - valor, flags, identific
*              NPonto - valor, flags, identific
*              ...
*              NPonto - valor, flags, identific
* 
* 
*  Retorno das funcoes de busca: busca_hist, busca_sup, busca_sql
* 
*  array com chave ponto e valores que são array de linhas
*  linha=array : hora, valor, flags
* 
* 
*              linhas         colunas
* 
*  Retorno -+- NPonto - hora, valor, flags
*           |         - hora, valor, flags
*           |         ... 
*           |         - hora, valor, flags
*           |
*           +- NPonto - hora, valor, flags
*           |         - hora, valor, flags
*           |         ... 
*           |         - hora, valor, flags
*           .
*           .
*           .
*           |
*           +- NPonto - hora, valor, flags
*                     - hora, valor, flags
*                     ... 
*                     - hora, valor, flags
*/

//----------------------------------------------------------------------------

 // busca direcionada ao SUP para valores integrados
 function & busca_intsup($lst_ptos=array(), $data="")
  {
  if (count($lst_ptos))
    $this->lista_pontos=$lst_ptos;

  if ($data!="")
    $this->define_data($data);

  $weekday=date( "w", mktime(0,0,0, $this->mes, $this->dia, $this->ano) );
  if ( $weekday == 6 || $weekday == 0 )
    {
    $ini_ponta=0;
    $fim_ponta=0;
    }
  else
      if (EhHorarioVerao(sprintf ("%02d%02d%04d",$this->dia, $this->mes, $this->ano)))
        {
        $ini_ponta=19*60;
        $fim_ponta=22*60;
        }
      else
        {
        $ini_ponta=18*60;
        $fim_ponta=21*60;
        }


  $retorno=array();

  if ($this->ano>=2000)
    {
    $OL_Arquivo=sprintf ("%s%04d_%02d/H%02d%02d%02dA.SUP",
                          $this->CaminhoSup, $this->ano, $this->mes,
                          $this->dia, $this->mes, $this->ano%100);
    }
  else
    {
    $OL_Arquivo=sprintf ("%s%02d_%02d/H%02d%02d%02dA.SUP",
                          $this->CaminhoSup, $this->mes, $this->ano%100,
                          $this->dia, $this->mes, $this->ano%100);
    }

  $err_rep = ini_get ('error_reporting');
  ini_set ('error_reporting', $err_rep & ~E_WARNING); // não reporta warnings
  if ( ( $OL_Hand = fopen( $OL_Arquivo, "rb" ) )
       == FALSE )
    {
      ini_set ('error_reporting', $err_rep);
      return $retorno;
    }

  ini_set ('error_reporting', $err_rep);

  // le cabecalho fixo
  $Versao     = GetInt($OL_Hand);       // bytes  0 -  1
  $Dia        = GetByte($OL_Hand);      // byte   2
  $Mes        = GetByte($OL_Hand);      // byte   3
  $Ano        = GetInt($OL_Hand);       // bytes  4 -  5
  $ProximaSeq = GetByte($OL_Hand);      // byte   6
  $nada       = GetByte($OL_Hand);      // byte   7
  $NumPtosDig = GetInt($OL_Hand);       // bytes  8 -  9
  $NumPtosAna = GetInt($OL_Hand);       // bytes 10 - 11
  $PrimMinuto = GetInt($OL_Hand);       // bytes 12 - 13
  $UltiMinuto = GetInt($OL_Hand);       // bytes 14 - 15
  $lixo       = fread($OL_Hand, 8);     // bytes 16 - 23

  $TFD =  2*intval(($NumPtosDig+7)/8);         // tamanho foto digital
  $TFA = 17*intval(($NumPtosAna+7)/8);         // tamanho foto analogica
  $TFT = $TFD + $TFA;
  $TCT = 1464 + 2*$NumPtosDig + 3*$NumPtosAna; // tamanho cab total

  for ($i=0; $i<1440; $i++)
    {
    $StatusGrav[$i] = GetByte( $OL_Hand );    // bytes 24 - 1463
    }

  for ($i=0; $i<$NumPtosDig; $i++)
    {
    $Digitais[$i] = GetInt( $OL_Hand );    // bytes 24 - 1463
    }

  for ($i=0; $i<$NumPtosAna; $i++)
    {
    $Analogicos[$i] = GetInt( $OL_Hand );    // bytes 24 - 1463
    $CasaDec[$i] = GetByte( $OL_Hand );    // bytes 24 - 1463
    }

  foreach ($this->lista_pontos as $pto)  // para cada ponto da lista, busca valor no sup
    {
    $posdig=NULL;
    $posana=array_search ($pto, $Analogicos, FALSE);
    if (gettype($posana)!="integer")
      {
      $posdig=array_search ($pto, $Digitais, FALSE);
      if (gettype($posdig)!="integer")
        continue;
      }

    $hora=$this->horainicial;
    $valor=0;
    $falha=128;
    $minini=substr($this->horainicial,0,2)*60+substr($this->horainicial,3,2);
    $minfim=substr($this->horafinal,0,2)*60+substr($this->horafinal,3,2);
    if ($minfim>$UltiMinuto)
      $minfim=$UltiMinuto;
    if ($this->stats)
      {
      $hmin="";
      $hmax="";
      $min=1e30;
      $max=-1e30;
      $pthmin="";
      $pthmax="";
      $ptmin=1e30;
      $ptmax=-1e30;
      $med=0;
      $cont_falhas=0;
      }

    $linhas=array();
    $acc=0;
    $temfalha=0;
    for ($i=$minini; $i<=$minfim; $i++)
      {
      if ($i==0 && $StatusGrav[$i]==0)
         continue;

      $posfoto = $TCT + $i*$TFT;
      $hora=sprintf("%02d:%02d:00", ($i+1)/60, ($i+1)%60);

      if (gettype($posdig)!="integer")
         { // analogico
         if ($StatusGrav[$i]) // testa se o minuto foi gravado
           {  
           $pos = $posfoto + $TFD + 17*intval($posana/8);
           fseek($OL_Hand, $pos);
           $falha=(GetByte($OL_Hand)&(0x01<<$posana%8))?128:0;
           $falha|=0x20;
           fseek($OL_Hand, 2*($posana%8), SEEK_CUR);
           $valor=GetInt($OL_Hand);

           if ($valor>32767)
             $valor=$valor-65536;

           switch ($CasaDec[$posana])
             {
             case 1:  $valor/=10;   break;
             case 2:  $valor/=100;  break;
             case 3:  $valor/=1000; break;
             default:               break;
             }
           }
         else
           {
           $falha=128|0x20;
           }
         }
      else
         { // digital
         if ($StatusGrav[$i]) // testa se o minuto foi gravado
           {
           $pos = $posfoto + 2*intval($posdig/8);
           fseek($OL_Hand, $pos);
           $falha=(GetByte($OL_Hand)&(0x01<<$posdig%8))?128:0;
           $valor=(GetByte($OL_Hand)&(0x01<<$posdig%8))?1:0;
           }
         else
           {
           $falha=128;
           }
         }

         if ($falha==128)
           $temfalha=128;

         $acc+=$valor;
         if ( !(($i+1) % $this->intervalo) )
           {
           $vint=intval(100*$acc/$this->intervalo)/100;
           if ($this->valores)
             {
             $colunas=array();
             array_push($colunas,$hora);
             array_push($colunas,$vint);
             array_push($colunas,$temfalha);
             array_push($linhas, $colunas);
             }
           $acc=0;
           $temfalha=0;

             if ($this->stats)
               {
               if ($i>$ini_ponta && $i<$fim_ponta)
                 { // horário de ponta
                   if ($vint>$ptmax)
                     {
                     $ptmax=$vint;
                     $pthmax=$hora;
                     }
                   if ($vint<$ptmin)
                     {
                     $ptmin=$vint;
                     $pthmin=$hora;
                     }
                 }
               else
                 { // fora da ponta
                   if ($vint>$max)
                     {
                     $max=$vint;
                     $hmax=$hora;
                     }
                   if ($vint<$min)
                     {
                     $min=$vint;
                     $hmin=$hora;
                     }
                 }
               $med+=$vint;
               if ($falha>=128)
                 $cont_falhas++;
               }
           }
      }

    if ($this->stats && $i!=0)
      {
      $colunas=array();
      array_push($colunas,"--:--:--");
      array_push($colunas,intval(1000*($i-$cont_falhas)/$i)/10);
      array_push($colunas,"Disponibilidade (%)");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,"--:--:--");
      array_push($colunas,intval(100*$med/60)/100);
      array_push($colunas,"Integral (hora)");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,"--:--:--");
      array_push($colunas,intval(100*$this->intervalo*$med/$i)/100);
      array_push($colunas,"Média");
      array_unshift($linhas, $colunas);

      if ($max==-1e30)
        {
        $max="";
        $hmax="--:--:--";
        }
      $colunas=array();
      array_push($colunas,$hmax);
      array_push($colunas,$max);
      array_push($colunas,"Máximo Fora Ponta");
      array_unshift($linhas, $colunas);

      if ($ptmax==-1e30)
        {
        $ptmax="";
        $pthmax="--:--:--";
        }
      $colunas=array();
      array_push($colunas,$pthmax);
      array_push($colunas,$ptmax);
      array_push($colunas,"Máximo Ponta");
      array_unshift($linhas, $colunas);
      }

    $retorno[$pto]=$linhas;
    }

  fclose($OL_Hand);

  return $retorno;
  }

 // busca direcionada ao POSTGRESQL HISTÓRICO SAGE
 function busca_sage_sql($lst_ptos=array(), $data="", $Estimados=0)
  {
  require_once("peardb_login.php") ;
  $db=&DBLogin("SQL_BH_SAGE");
  if (DB::isError($db)) 
     die("Erro de conexão ao banco Sage!");
     
  if (count($lst_ptos))
    $this->lista_pontos=$lst_ptos;

  $this->ajusta_chaves();

  if ($data!="")
    $this->define_data($data);

  $retorno=array();
  
  $cnt=0;
  foreach ($this->lista_pontos as $pto)
    {
    
    $id=$this->lista_id[$cnt];
    $tipo=$this->lista_tipo[$cnt];
    $cnt++;
    
    
    if ($tipo=='A')
      {
      if ($Estimados)
        {$tabhis="pas";$colval="valest";$colflag="flagest";$colflagest="flagest";}
      else
        {$tabhis="pas";$colval="valor";$colflag="flag";$colflagest="flagest";}
      }
    else  
      {$tabhis="pds";$colval="estado";$colflag="estado";$colflagest="0";}
    
    if ($this->intervalo==0 || $tipo=='D')
      $filtra_hora="";
    else
      $filtra_hora="and to_char(bh_dthr, 'HH24:MI:SS') like '%%:%%:00'";

    $query = 
    " select 
         to_char(bh_dthr, 'HH24:MI:SS'), $colval, $colflag, $colflagest, bh_variacao
      from 
         bh{$tabhis}_h_{$this->ano}_{$this->mes}_{$this->dia}
      where 
         bh_chave= 
          (
          select bh_chave 
           from 
             {$tabhis}_r 
           where 
             id='$id' 
           order by 
             bh_dthr_ini desc 
           limit 1
          )
         $filtra_hora
      and to_char(bh_dthr, 'HH24:MI:SS')>='$this->horainicial' and to_char(bh_dthr, 'HH24:MI:SS')<='$this->horafinal' 
      order by bh_dthr         
     ";

    $result = & $db->query($query);
    if (DB::isError($result))
       continue;
       
    if ($result->numRows()==0)
       continue;

       
//    if ($result==FALSE)
//      continue;

    $i=0;
    if ($this->stats)
      {
      $hmin="";
      $hmax="";
      $min=9999999;
      $max=-9999999;
      $med=0;
      $cnt_med=0;
      $cont_falhas=0;
      }

    $linhas=array();
    $horaant="";
        
        while( $result->fetchInto($line) )
          {
          $i++;
          
          $colunas=array();
          
          $hora=$line[0];
          if ($hora!=$horaant)
            $horaant=$hora;
          else 
            continue;  
          
          array_push($colunas,$hora);
          
          $valor=$line[1];
          if ($tipo=='D')  
            $valor=(int)(!($valor&0x01));
          array_push($colunas,$valor);

          // traduz flags do SAGE para formato A
          // ver STI_dcatbh.html gerado pelo histórico do SAGE
          $flags_sage=(int)$line[2];
          $flagest=(int)$line[3];
          $flags=0;

          $variacao=(int)$line[4];

          // falha
          if ($flags_sage & 4 || $flags_sage & 8 || $flags_sage & 64 || $flags_sage & 4096)
            $flags|=0x80;
          // substituído  
          if ($flags_sage & 16 || $flags_sage & 32 )
            $flags|=0x10;
          // manual (não supervisionado)
          if ($flags_sage & 256)
            { $flags|=0x0C; }
          if ($tipo=='A')  
            {
            // estimado  
            if ($flags_sage & 524288)
              { $flags|=0x08; $flags&=0xFB; }

            if ($flags_sage & 1 || $flagest==1) // erro grosseiro detectado na estimação
              $flags|=0x01; 
                          
            if ($Estimados)
              {
              // FLAGS do ESTIMADOR
              //Nome  	Valor  	 Descrição
              //K_EE_FLG_NORM 	0 	Flag -> Medida estimada normalmente
              //K_EE_FLG_ERREE 	1 	Flag -> Erro grosseiro de estimacao do estado
              //K_EE_FLG_NAOEST 2 	Flag -> Medida nao faz parte do escopo do estimador de estado
              //K_EE_FLG_EXCL 	3 	Flag -> Medida excluida do processo de estimacao de estado
              //K_EE_FLG_RLOC 	4 	Flag -> Redundancia local insuficiente (Limite de confianca baixo)
              //K_EE_FLG_REST 	5 	Flag -> Medida e` um resultado de calculo com restricao
              //K_EE_FLG_ANORM 	6 	Flag -> Estimacao de Estado Anormal              
              $flags=0x08;
              if ($flags_sage == 1) // erro grosseiro detectado na estimação
                 $flags|=0x01;
              if ($flags_sage>1) // falha a medida estimada
                 $flags|=0x80;
              } 
              
            if ($variacao)
              $flags|=0x40;          
              
            $flags|=0x20; // analógico          
            }  
          else
            { // digital
            if ($variacao)
              $flags|=0x40;          

            $flags|=$valor&0x01;
            }  
            
          array_push($colunas,$flags);

          if ($this->stats)
            {
            if ($flags>=128)
              $cont_falhas++;
//          else // só considera valores ok para máximo e mínimo
              {
              if ($valor>$max)
                {
                $max=$valor;
                $hmax=$hora;
                }
              if ($valor<$min)
                {
                $min=$valor;
                $hmin=$hora;
                }
              }

            if ($hora[6]=='0' && $hora[7]=='0') // só o minuto redondo para cálculo da hora
              {
              $med+=$valor;
              $cnt_med++;
              }
            }

          if ( $this->valores )
            array_push($linhas, $colunas);
          }

    if ($this->stats && $i!=0)
      {
      $colunas=array();
      array_push($colunas,"--:--:--");
      array_push($colunas,intval(1000*($i-$cont_falhas)/$i)/10);
      array_push($colunas,"Disponibilidade (%)");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,"--:--:--");
      array_push($colunas,intval(100*$med/60)/100);
      array_push($colunas,"Integral (hora)");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,"--:--:--");
      array_push($colunas,intval(100*$med/$cnt_med)/100);
      array_push($colunas,"Média");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,$hmin);
      array_push($colunas,$min);
      array_push($colunas,"Mínimo");
      array_unshift($linhas, $colunas);

      $colunas=array();
      array_push($colunas,$hmax);
      array_push($colunas,$max);
      array_push($colunas,"Máximo");
      array_unshift($linhas, $colunas);
      }
    $retorno[$pto]=$linhas;
    
    }
  
  $db->disconnect();
  return $retorno;
  }
//----------------------------------------------------------------------------

 // busca de foto direcionada ao POSTGRESQL HISTÓRICO SAGE
 function busca_foto_sage_sql($lst_ptos=array(), $data="", $Estimados=0)
  {
  require_once("peardb_login.php") ;
  $db=&DBLogin("SQL_BH_SAGE");
  if (DB::isError($db)) 
     die("Erro de conexão ao banco Sage!");
     
  if (count($lst_ptos))
    $this->lista_pontos=$lst_ptos;

  $this->ajusta_chaves();

  if ($data!="")
    $this->define_data($data);

  $retorno=array();
  
  if ($Estimados)
    {$colval="valest";$colflag="flagest";$colflagest="flagest";}
  else
    {$colval="valor";$colflag="flag";$colflagest="flagest";}

  $lstpto='';
  $cnt=0;
  foreach ($this->lista_pontos as $pto)
    {
    if ($pto != '')
      $lstpto=$lstpto."'".$this->lista_id[$cnt]."',";
    
    $cnt++;
    }
  $lstpto=$lstpto."-1";

    // cuidado que a segunda coluna ao unir vira tudo tipo float, portanto para o tipo digital deve ser usado só a coluna de flags 
    $query = 
    " 
      select 
         to_char(bh_dthr, 'HH24:MI:SS'), $colval, $colflag, $colflagest, bh_variacao, id, 'A' as tipo
      from 
         pas_h_r_v
      where 
         id in ($lstpto)
      union         
      select 
         to_char(bh_dthr, 'HH24:MI:SS'), estado, estado, 0, bh_variacao, id, 'D' as tipo
      from 
         pds_h_r_v
      where 
         id in ($lstpto)
     ";
    
    // echo $query;    

    $result = & $db->query($query);
    if (DB::isError($result))
       return;

    if ($result->numRows()==0)
       return $retorno;

    $pto=0;
        
     while( $result->fetchInto($line) )
          {
          $linhas=array();

          $id=$line[5];
          
          for ($cnt=0;$cnt<count($this->lista_id); $cnt++)
            {
            if ($id==$this->lista_id[$cnt]) // faltou um ponto, pula na lista
               {
               $pto=$this->lista_pontos[$cnt];
               break;
               }
            }
          
          $tipo=$line[6];

          $colunas=array();
          
          $hora=$line[0];
          
          array_push($colunas,$hora);
          
          $valor=$line[1];
          // traduz flags do SAGE para formato A
          // ver STI_dcatbh.html gerado pelo histórico do SAGE
          $flags_sage=(int)$line[2];
          $flagest=(int)$line[3];

          if ($tipo=='D')  
            $valor=(int)(!($flags_sage&0x01)); // para digital pega a coluna de flags
          array_push($colunas,$valor);

          $variacao=(int)$line[4];

          $flags=0;
          // falha
          if ($flags_sage & 4 || $flags_sage & 8 || $flags_sage & 64 || $flags_sage & 4096)
            $flags|=0x80;
          // substituído  
          if ($flags_sage & 16 || $flags_sage & 32 )
            $flags|=0x10;
          // manual (não supervisionado)
          if ($flags_sage & 256)
            { $flags|=0x0C; }
          if ($tipo=='A')  
            {
            // estimado  
            if ($flags_sage & 524288)
              { $flags|=0x08; $flags&=0xFB; }

            if ($flags_sage & 1 || $flagest==1) // erro grosseiro detectado na estimação
              $flags|=0x01; 
                          
            if ($Estimados)
              {
              // FLAGS do ESTIMADOR
              //Nome  	Valor  	 Descrição
              //K_EE_FLG_NORM 	0 	Flag -> Medida estimada normalmente
              //K_EE_FLG_ERREE 	1 	Flag -> Erro grosseiro de estimacao do estado
              //K_EE_FLG_NAOEST 2 	Flag -> Medida nao faz parte do escopo do estimador de estado
              //K_EE_FLG_EXCL 	3 	Flag -> Medida excluida do processo de estimacao de estado
              //K_EE_FLG_RLOC 	4 	Flag -> Redundancia local insuficiente (Limite de confianca baixo)
              //K_EE_FLG_REST 	5 	Flag -> Medida e` um resultado de calculo com restricao
              //K_EE_FLG_ANORM 	6 	Flag -> Estimacao de Estado Anormal              
              $flags=0x08;
              if ($flags_sage == 1) // erro grosseiro detectado na estimação
                 $flags|=0x01;
              if ($flags_sage>1) // falha a medida estimada
                 $flags|=0x80;
              } 
              
            if ($variacao)
              $flags|=0x40;          
              
            $flags|=0x20; // analógico          
            }  
          else
            { // digital
            if ($variacao)
              $flags|=0x40;          

            $flags|=$valor&0x01;
            }  
            
          array_push($colunas,$flags);
          if ( $this->valores )
            array_push($linhas, $colunas);
      
          $retorno[$pto]=$linhas;
          }
    
  $db->disconnect();
  return $retorno;
  }

//----------------------------------------------------------------------------
} // THistorico

//----------------------------------------------------------------------------

/*

// Exemplo de uso da classe THistorico

$consulta= new THistorico;
$consulta->intervalo=1;
$consulta->valores=FALSE;
$consulta->stats=TRUE;
$consulta->horainicial="00:00:00";
$consulta->horafinal="23:59:59";
$consulta->define_data("05/05/2002");
$consulta->inclui_ponto(8056);
$ret=$consulta->busca_sup();


//$consulta->define_data("27/04/2002");
//$ret=$consulta->busca_sql();

//$consulta->intervalo=0;
//$consulta->define_data("28/04/2002");
//$ret=$consulta->busca_sql();


//$ret=$consulta->busca_sql();

  // para cada ponto, extrai conjunto de linhas
  while (list($ponto, $linhas) = each($ret) )
  {
  foreach ($linhas as $linha) // para cada linha
    {
    print "$ponto ";
    foreach ($linha as $coluna) // para cada coluna: hora, valor e flags
      print "$coluna ";
    print "<BR>";
    }
  }
*/

?>

