<?PHP

// OSHMI/Open Substation HMI - Copyright 2008-2014 - Ricardo L. Olsen

// Parâmetros:
//
// Eventos em tempo real:
// &E=MAXEVENTOS &A=AGREGAR &G=HORAGPS &F=FILTRO &B=CALLBACK
// MAXEVENTOS : número máximo de alarmes
// AGREGAR : 1=agrega eventos do mesmo ponto, 0=não agrega
// HORAGPS : 1=trabalha com a hora do campo (GPS) para os eventos, 0=hora local
// FILTRO : Filtro por subestação (lista separada por vírgula) 
// CALLBACK : nome da função de callback
//
// OU
//
// Eventos históricos:
// &V=MAXEVENTOS &H=HORAINI &D=DATAINI &G=HORAGPS &F=FILTRO &T=DATEFMT &B=CALLBACK 
// MAXEVENTOS : número máximo de alarmes
// HORAINI : Hora inicial dos eventos
// DATAINI : Data inicial dos eventos
// HORAGPS : 1=trabalha com a hora do campo (GPS) para os eventos, 0=hora local
// FILTRO : Filtro por ESTAÇÃO ou ID ou DESCRIÇÃO  
// DATEFMT : deprecated parameter, not needed anymore
// CALLBACK : nome da função de callback

// Show no errors
error_reporting(0);

header("Content-type: text/javascript; charset=UTF-8");
header("Content-Disposition: inline; filename=\"eventserver.js\"");
header("Cache-Control: no-store, must-revalidate");
    
require_once 'timezone.php';

// internationalization messages
$i18n = parse_ini_file( '../i18n/messages_i18n.ini', true );

extract($_REQUEST, EXTR_PREFIX_ALL|EXTR_SKIP, 'p');

// consulta deve ser modo Tempo Real "E" ou Histórico "V":
if ( ! isset( $p_E ) && ! isset( $p_V ) )
  return;

$MODO_TR = 0; 
$MODO_HIST = $MODO_TR + 1; 
$MODO = $MODO_TR;
$MAXEVENTOS = 500;

if ( isset( $p_E ) ) 
  { $MAXEVENTOS = intval($p_E); }
else
if ( isset( $p_V ) ) 
  { $MAXEVENTOS = intval($p_V); $MODO = $MODO_HIST; }

if ( isset( $p_H ) ) $HORAINI =  $p_H; else $HORAINI = "00:00:00";
if ( isset( $p_D ) ) $DATAINI =  $p_D; else $DATAINI = "";

if ( isset( $p_A ) ) $AGREGAR =  intval($p_A); else $AGREGAR = 0; // 0:não agrega eventos, 1:agrega, 2:modo pânico
if ( isset( $p_G ) ) $HORAGPS =  intval($p_G); else $HORAGPS = 1;
if ( isset( $p_F ) ) $FILTRO = $p_F; else $FILTRO = "";
if ( isset( $p_B ) ) $CALLBACK = $p_B; else $CALLBACK = "";

$ini_array = parse_ini_file( '../conf/hmi.ini' ); 

$NUMMAX_ALARMES = @intval( $ini_array["MAX_EVENTS"] );
if ( $NUMMAX_ALARMES < 50 )
   {
   $NUMMAX_ALARMES = 50; 
   }
if ( $NUMMAX_ALARMES > 5000 )
   {
   $NUMMAX_ALARMES = 5000; 
   }

$NUMMAX_ALARMES = max( $MAXEVENTOS, $NUMMAX_ALARMES );

$PRIOR_MOSTRA_PANICO = intval( @$ini_array["PANIC_PRIORITY"] );
if ( $PRIOR_MOSTRA_PANICO > 9 || $PRIOR_MOSTRA_PANICO < 0 || !array_key_exists("PANIC_PRIORITY", $ini_array) )
  {
  $PRIOR_MOSTRA_PANICO = 1;
  }
  
$FILTRO_RECONHECIDOS = " s.RECON<=1 ";

$FILTDH = strtotime("-2 days");
$FILTDHFIM = "";
$ORDER = " DESC ";
$ORDER2 = " DESC ";
if ( $MODO == $MODO_HIST )
  {
  $AGREGAR=0;
  $FILTRO_RECONHECIDOS = " s.RECON>=0 ";
  $ORDER = " ASC ";
  $FILTDH = strtotime($DATAINI." ".$HORAINI);
  $FILTDHFIM = strtotime($DATAINI." 23:59:59.9999");
  }

if ( $AGREGAR == 0 )
  {
  $SQL_COUNTNPONTO = " 1 AS CNT ";
  $SQL_GROUPBY = "";
  }
else
  {
  $SQL_COUNTNPONTO = " COUNT(NPT) AS CNT ";
  $SQL_GROUPBY = " GROUP BY NPT ";
  $ORDER2 = " ASC ";
  }   

$TIPO_ANALOGICO = 1;
$TIPO_DIGITAL = 0;
$QUAL_CASADEC = 0x03; // casa decimal no ponto analógico
$QUAL_TIPO = 0x20; // flag para o tipo do ponto Analógico/Digital
$QUAL_FALHATAG = 0x40; // flag para falha em tag de tempo de eventos
$QUAL_FALHA = 0x80; // flag para a invalidade do ponto
$QUAL_ORIGEM = 0x0C;
$QUAL_ESTADO = 0x01;
$QUAL_ESTADO_DUPLO = 0x03;
$QUAL_SUBST = 0x10;
$QUAL_EVENTOANALOGICO = 0x10; // flag de registro de evento analógico com valor no cmapo UTR
$ORG_SUPERVISIONADO = 0x00; // origem normal: supervisionado
$ORG_CALCULADO = 0x04; // ponto com origem em cálculo
$ORG_CARGAINIC = 0x08; // Ponto nunca atualizado
$ORG_MANUAL = 0x0C; // ponto de origem manual
$ORG_MASCARA = 0x0C; // mascara de origem 

echo "WSCons=1;L=[];\n";

try {
    $lstpnt = array();
    $strpnt = "";
    $sderows = array();
    $nponto = 0;

    $dbsde = new PDO( 'sqlite:../db/soe.sl3' );
    $dbsde->exec ( "PRAGMA synchronous = NORMAL" );
    $dbsde->exec ( "PRAGMA journal_mode = WAL" );
    $dbsde->exec ( "PRAGMA locking_mode = NORMAL" );
    $dbsde->exec ( "PRAGMA cache_size = 5000" );
    $dbsde->exec ( "PRAGMA temp_store = MEMORY" );
    //$dbsde->exec ( "PRAGMA mmap_size=268435456" );
    $dbsde->exec ( "ATTACH DATABASE '../db/dumpdb.sl3' as DBPONTOS" );

    $filtclause = "";
    $FILTRODATAHORA = "";
    if ( $MODO == $MODO_TR )
      {  
      $FILTRO = trim( $FILTRO, "," );
      if ( $FILTRO != "" )
        $filtclause = " AND p.ESTACAO IN (".$FILTRO.") ";
      }
    else  
    if ( $MODO == $MODO_HIST )
      {
      $FILTRO = str_replace("*", "%", $FILTRO);
      $filtclause = " AND ( p.ESTACAO LIKE '$FILTRO%' OR p.ID LIKE '%$FILTRO%' OR p.DESCRICAO LIKE '%$FILTRO%' ) ";
      if ( $HORAGPS == 0 )
        $FILTRODATAHORA = " AND s.TS >= $FILTDH ".($FILTDHFIM==""? "" : " AND s.TS <= $FILTDHFIM ");
      else
        $FILTRODATAHORA = " AND s.DATA >= $FILTDH ".($FILTDHFIM==""? "" : " AND s.DATA <= $FILTDHFIM ");
      }
    
    if ( $HORAGPS == 0 )
      // usa hora da IHM
      $sql = "SELECT *, $SQL_COUNTNPONTO FROM (SELECT s.NPONTO as NPT, s.TS as DATAH, s.HORA, 0 as MSEC, s.UTR, s.ESTADO, s.RECON, p.NPONTO, p.ID, p.DESCRICAO, p.FLAGS, p.ESTON, p.ESTOFF, p.UNIDADE, p.TIPO, p.SUPCMD, p.ANOTACAO, p.ESTACAO, p.ESTALM, p.PRIOR, s.DATA as GPSDATAH, s.MSEC as GPSMSEC ".
             " FROM SOE s ".
             " JOIN DUMPDB p ON p.NPONTO = s.NPONTO ".
             " WHERE $FILTRO_RECONHECIDOS ".
             " AND p.ID !='' $filtclause ".
             $FILTRODATAHORA.
             (($SQL_GROUPBY=="")?"":" ORDER BY DATAH $ORDER2 ").
             " ) ". 
             $SQL_GROUPBY.
             " ORDER BY DATAH $ORDER, GPSDATAH $ORDER, GPSMSEC $ORDER ".
             " LIMIT $NUMMAX_ALARMES+1"; 
    else
      // usa hora do campo
      $sql = "SELECT *, $SQL_COUNTNPONTO FROM (SELECT s.NPONTO as NPT, s.DATA as DATAH, s.HORA, s.MSEC, s.UTR, s.ESTADO, s.RECON, p.NPONTO, p.ID, p.DESCRICAO, p.FLAGS, p.ESTON, p.ESTOFF, p.UNIDADE, p.TIPO, p.SUPCMD, p.ANOTACAO, p.ESTACAO, p.ESTALM, p.PRIOR ".
             " FROM SOE s ".
             " JOIN DUMPDB p ON p.NPONTO = s.NPONTO ".
             " WHERE $FILTRO_RECONHECIDOS ".
             " AND p.ID !='' $filtclause ".
             $FILTRODATAHORA.
             (($SQL_GROUPBY=="")?"":" ORDER BY DATAH $ORDER2, s.MSEC $ORDER2 ").
             " ) ".
             $SQL_GROUPBY.
             " ORDER BY DATAH $ORDER, MSEC $ORDER ".
             " LIMIT $NUMMAX_ALARMES+1"; 
                
//  echo $sql;            
    $cnt = 0;
    $tmini = 0;
    foreach ( $dbsde->query( $sql ) as $row )
      {
      $nponto = $row['NPT'];  
      $flags = $row['ESTADO'];
      $id = $row["ID"];
      $se = $row["ESTACAO"];
      $descricao = str_ireplace( ",", ".", $row["DESCRICAO"] );
      $estalm = $row["ESTALM"];
      $prioridade = $row["PRIOR"];

      if (   $AGREGAR == 0 ||
           ( $AGREGAR == 1 ) ||
           ( $AGREGAR == 2 && $estalm == 3 && ( ($flags & 0x03) == 2 )  && $prioridade <= $PRIOR_MOSTRA_PANICO ) ||
           ( $AGREGAR == 2 && $estalm == 2 && $prioridade <= $PRIOR_MOSTRA_PANICO ) ||
           ( $AGREGAR == 2 && $estalm == 1 && ( ($flags & 0x01) == 0 ) && $prioridade <= $PRIOR_MOSTRA_PANICO ) ||
           ( $AGREGAR == 2 && $estalm == 0 && ( ($flags & 0x01) == 1 ) && $prioridade <= $PRIOR_MOSTRA_PANICO )
        )
       {
        $estado="";
        switch ( $flags & 0x33 )
           {
           case 0x10:
           case 0x00:
             $estado = $i18n["EVENTS"]["IEC_DP_TRANSIT"];
             break;
           case 0x11:
           case 0x01:
             $estado = $row["ESTOFF"];
             break;
           case 0x12:
           case 0x02:
             $estado = $row["ESTON"];
             break;
           case 0x13:
             if ( $row["SUPCMD"] != 0  )
               $estado = $i18n["EVENTS"]["CMD_BLOCKED"];
             else
               $estado = $i18n["EVENTS"]["IEC_DP_INVALID"];
             break;
           case 0x03:
             if ( $row["SUPCMD"] != 0 )
               $estado = $i18n["EVENTS"]["CMD_UNBLOCKED"];
             else
               $estado = $i18n["EVENTS"]["IEC_DP_INVALID"];
             break;
           case 0x20:
             $estado = $i18n["EVENTS"]["LIMIT_NORMAL"];
             break;
           case 0x21:
             $estado = $i18n["EVENTS"]["LIMIT_INF_VIOLATED"];
             break;
           case 0x22:
             $estado = $i18n["EVENTS"]["LIMIT_SUP_VIOLATED"];
             break;
           case 0x23:
             $estado = "INVALID_REGION";
             break;
           case 0x30:
             $estado = sprintf( "%.0f %s", $row['UTR'], $row["UNIDADE"] );
             break;
           case 0x31:
             $estado = sprintf( "%.1f %s", $row['UTR'] / 10.0, $row["UNIDADE"] );
             break;
           case 0x32:
             $estado = sprintf( "%.2f %s", $row['UTR'] / 100.0, $row["UNIDADE"] );
             break;
           case 0x33:
             $estado = sprintf( "%.3f %s", $row['UTR'] / 1000.0, $row["UNIDADE"] );
             break;
           default:
             $estado = "";
           break;
           }
           
         $Qlf = "" . $prioridade;
         if ( $row["SUPCMD"] != 0 )
           $Qlf = $Qlf . "K";
         if ( $flags & $QUAL_FALHA )
           $Qlf = $Qlf . "F";
         if ( $flags & $QUAL_FALHATAG )
           $Qlf = $Qlf . "T";
    
         // não mostra o qualificador substituído para eventos analógicos
         if ( ( $flags & $QUAL_TIPO ) == $TIPO_DIGITAL )
         if ( $flags & $QUAL_SUBST )
           $Qlf = $Qlf . "S";
    
         if ( ( $flags & $ORG_MASCARA ) == $ORG_CALCULADO )
            $Qlf = $Qlf . "C";
         else
         if ( ( $flags & $ORG_MASCARA ) == $ORG_MANUAL )
           $Qlf = $Qlf . "M";
         else
         if ( ( $flags & $ORG_MASCARA ) == $ORG_CARGAINIC ) // esta situação não deve ocorrer para eventos
           $Qlf = $Qlf . "X";
    
         if ( $row['RECON'] == 0 )
           $Qlf = $Qlf . "L";
         if ( $row["ANOTACAO"] != "" )
           $Qlf = $Qlf . "A";
             
        // $dt = getdate( $row['DATAH'] );
        
        // para mostrar o número de repetições de eventos sobre o mesmo ponto
        $conteve = intval( $row['CNT'] );
        $strnumeve = "";
        if ( $AGREGAR > 0 && $conteve > 1 )
           $strnumeve = "+".($conteve-1);  
  
        if ( $cnt >= $NUMMAX_ALARMES )
          {
          printf( "L[%d]=',,,,MAX_EVT,,...,,,,';\n", $cnt ); 
          break;
          }
        else
          {
          if ( $cnt == 0 )
            $tmini = $row['DATAH'];
          $tm = $tmini;
          $tm = abs (60 + $tm - ( $tm % 60 ) - $row['DATAH'] );
          // linha do evento para o javascript
          printf( "L[%d]='%s,%s,%03d,%d,%s,%s,%s,%s,%d,%s%s,%d';\n", 
                 $cnt, 
                 date( $i18n["WEBSERVER"]["DATE_FMT_EVENT"], $row['DATAH'] ), 
                 date( $i18n["WEBSERVER"]["TIME_FMT_EVENT"], $row['DATAH'] ), 
                 $row['MSEC'], 
                 $nponto, 
                 $id,
                 $se,
                 $descricao,
                 $estado,
                 $flags,               
                 $Qlf,
                 $strnumeve,
                 $tm
              );
          }     
        $cnt++;
        }
      }    
    
    }
catch( PDOException $e )
    {
    // echo "Erro: ".$e->getMessage();
    // print_r($dbsde->errorInfo());
    }

echo "WSCons=2;\n";
if ( $CALLBACK != "" )
   echo "$CALLBACK();\n";          
    
?>
