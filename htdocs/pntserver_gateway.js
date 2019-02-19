// identify script names that access data servers (INTERNET GATEWAY NORMALLY WITH NO ACCESS TO CONTROLS)

// data server path (leave empty for local server) e.g.: 'http://192.168.1.1:51909/htdocs/'
var ServerPath = '';

// identify the script name that access realtime point data
// var PNTServer = 'pntserver.rjs'; // pntserver.rjs has access to controls!
var PNTServer = ServerPath + 'pgsql_pntserver.php'; // via PostgreSQL (no access to controls)
// var PNTServer = 'sqlite_pntserver.php'; // via SQLite (no access to controls)

// identify the script name that access historic data
// var TimePNTServer = "timepntserver.php"; // via SQLite
var TimePNTServer = ServerPath +  "pgsql_timepntserver.php"; // via PostgreSQL

// identify the script name that access tabular data
// var TabPNTServer = "pntserver.rjs"; // pntserver.rjs has access to controls!
var TabPNTServer = ServerPath + "pgsql_tabserver.php"; // via PostgreSQL
// var TabPNTServer = "sqlite_tabserver.php"; // via SQLite

// identify the script name that access events data
// var EventServer = 'eventserver.php'; // via SQLite
var EventServer = ServerPath + 'pgsql_eventserver.php'; // via PostgreSQL

// identify the script name that access point documental annotation data
// var DocAnnotationServer = 'annotation.php'; // via SQLite (can write annotations)
var DocAnnotationServer = ServerPath + 'annotation_readonly.php'; // via SQLite read only

