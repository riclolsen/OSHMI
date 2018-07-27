<?PHP
// OSHMI/Open Substation HMI - Copyright 2008-2014 - Ricardo L. Olsen
// Generate database insert for events to be synchronized

// Show no errors
error_reporting(0);

try {

    $dbsde = new PDO( 'sqlite:../db/soe.sl3' );
    $dbsde->exec ( "PRAGMA synchronous = NORMAL" );
    $dbsde->exec ( "PRAGMA journal_mode = WAL" );
    $dbsde->exec ( "PRAGMA locking_mode = NORMAL" );

    printf("PRAGMA synchronous = NORMAL;\n");
    printf("PRAGMA journal_mode = WAL;\n");
    printf("BEGIN DEFERRED TRANSACTION;\n");
    
    $sql = "SELECT NPONTO, DATA, HORA, MSEC, UTR, ESTADO, RECON, TS ".
            " FROM SOE ".
            " WHERE NPONTO < 99980 or NPONTO > 100000 ".
            " LIMIT 10000"; 
 
    $cnt = 0;
    $stmt = $dbsde->query( $sql );
    $it = new IteratorIterator($stmt);
    $numrows = iterator_count($it);
    $stmt->execute();
    foreach ( $stmt as $row )
      {
      if ( ! ($cnt % 30) ) 
        printf( "INSERT OR IGNORE INTO soe (NPONTO,DATA,HORA,MSEC,UTR,ESTADO,RECON,TS) VALUES \n");

      printf( "(%d,%d,%d,%d,%d,%d,%d,%d)", 
              $row['NPONTO'], 
              $row['DATA'], 
              $row['HORA'], 
              $row['MSEC'], 
              $row['UTR'], 
              $row['ESTADO'],
              $row['RECON'],
              $row['TS']
              );
      $cnt++;
      if ( ! ($cnt % 30) ) 
        printf(";\n");
      else  
      if ( $cnt != $numrows )
        printf(",\n");
      }
    
    printf(";\n");
    printf("COMMIT;\n");
      
    }
catch( PDOException $e )
    {
    echo "Error: ".$e->getMessage();
    }

?>
