//---------------------------------------------------------------------------
// CONFIG.CPP
// Carrega as configurações do aplicativo.
//---------------------------------------------------------------------------
/*
OSHMI - Open Substation HMI
	Copyright 2008-2019 - Ricardo L. Olsen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <vcl.h>
#pragma hdrstop
#include <inifiles.hpp>
#include <stdio.h>
#include "config.h"
#include "sha1.h"

int T_INTEGRIDADE = 5 * 60;
int BDTR_PORTA = 65280;
int BDTR_PORTA_CMD = 65281;
int BDTR_GRUPO_INTEGRIDADE = 0;
int HTTP_PORTA_OLD = 51907;
int HTTP_PORTA = 51908;
String IP_BDTR1 = "";
String IP_BDTR2 = "";
int END_BDTR = 201;
int END_IHM = 208;
int ENVIA_COMANDOS = 1;
int BDTR_ACERTO_HORA = 0;
int BDTR_PERIODO_ENVIO_ACERTO_HORA = 0;
int PONTO_OPERACAO = 0;

// Protocol driver JSON UDP interface
int UDP_JSON_PORT = 9100;
int UDP_JSON_PORT_CMD = 9101;
String UDP_JSON_ENDPOINT1 = "127.0.0.1";
String UDP_JSON_ENDPOINT2 = "";

String IHMRED_IP_OUTRO_IHM = "";
int IHMRED_ACERTO_HORA = 0;
int IHMRED_PERIODO_ENVIO_ACERTO_HORA = 5;
int IHMRED_HTTP_PORT = 51909;

int PRIORIDADE_PANICO = 4;
int NUMMAX_ALARMES = 1000;
int LAST_PRIORITY_THAT_BEEPS = 2;

// int WEBSERVER_NOCACHE = 1;
int ALARMA_DUPLA_TRANSICAO = 0;
String WEBSERVER_CLIENTES_REMOTOS = "";
String WEBSERVER_SCRIPT = "pntserver.rjs";
String SHELLAPI_SCRIPT = "shellapi.rjs";
String SIMPLEJSON_PATH = "/simplejson/";
// date time format according to TDateTime::FormatString
String WEBSERVER_DATE_FMT = "yy/mm/dd hh:nn:ss";
String WEBSERVER_DP_TRANSIT = "TRANSIT"; // message for iec digital double 00
String WEBSERVER_DP_INVALID = "INVALID"; // message for iec digital double 11
String WEBSERVER_CORS_ORIGIN = "*"; // for the http header Access-Control-Allow-Origin
int WEBSERVER_HTTPD_MODE = 1; // microhttpd daemon start mode (1=single thread, 2=thread per connection)

String VISOR_EVENTOS;
String VISOR_TABULAR;
String VISOR_TELAS;
String VISOR_CURVAS;
String VISOR_TENDENCIAS;
int BEEP_INTSPEAKER = 0;

int HIST_RECORD = 1;
int HIST_DEADBANDFACTOR = 100;
int HIST_LIFETIME = 36;

int DB_POSTGRESQL=0; // enable write postgresql files
int DB_MONGODB=0; // enable write mongodb files

int HIDE = 0;
int IHM_FINALIZANDO = 0;
int SIMULACAO = 0;
String LISTA_ESCRAVOS = "";
String LISTA_ESCRAVOS_SIMUL[MAX_ESCRAVOS_SIMUL+1];

int RUN_LINUX = 0;

String RUN_VARREDORIEC104 = "qtester104.exe";
String RUN_NGINXPHP = "hidec.exe ..\\nginx_php\\start_nginx_php.bat";
String RUN_PROCHIST = "hidec.exe ..\\db\\process_hist.bat";
String RUN_PROCMONHIST = "hidec.exe ..\\db\\monit_hist_files.bat";
String RUN_PROCEVENTOS = "hidec.exe ..\\db\\process_soe.bat";
String RUN_PROCPONTOS = "hidec.exe ..\\db\\process_dumpdb.bat";
String END_PROCHIST = "hidec.exe ..\\db\\terminate_hist.bat";
String END_PROCEVENTOS = "hidec.exe ..\\db\\terminate_soe.bat";
String END_PROCPONTOS = "hidec.exe ..\\db\\terminate_dumpdb.bat";

String PG_START = "hidec.exe ..\\PostgreSQL\\postgresql_start.bat";
String RUN_PGPROCHIST = "hidec.exe ..\\db\\process_pg_hist.bat";
String RUN_PGPROCEVENTOS = "hidec.exe ..\\db\\process_pg_soe.bat";
String RUN_PGPROCPONTOS = "hidec.exe ..\\db\\process_pg_dumpdb.bat";
String END_PGPROCHIST = "hidec.exe ..\\db\\terminate_pg_hist.bat";
String END_PGPROCEVENTOS = "hidec.exe ..\\db\\terminate_pg_soe.bat";
String END_PGPROCPONTOS = "hidec.exe ..\\db\\terminate_pg_dumpdb.bat";


String MONGO_START = "hidec.exe ..\\mongodb\\mongodb_start.bat";
String RUN_MONGOPROCHIST = "hidec.exe ..\\db\\process_mongo_hist.bat";
//String RUN_MONGOPROCEVENTOS = "hidec.exe ..\\db\\process_mongo_soe.bat";
String RUN_MONGOPROCPONTOS = "hidec.exe ..\\db\\process_mongo_dumpdb.bat";
String END_MONGOPROCHIST = "hidec.exe ..\\db\\terminate_mongo_hist.bat";
//String END_MONGOPROCEVENTOS = "hidec.exe ..\\db\\terminate_mongo_soe.bat";
String END_MONGOPROCPONTOS = "hidec.exe ..\\db\\terminate_mongo_dumpdb.bat";

String GRAFANA_START = "hidec.exe ..\\grafana\\grafana_start.bat";

int Config_Le( void )
{
// Lê o arquivo INI
TIniFile *pIni = new TIniFile( ARQUIVO_INI );

if ( pIni == NULL )
  return 1;

T_INTEGRIDADE = pIni->ReadInteger( "BDTR", "INTEGRITY_PERIOD", T_INTEGRIDADE );
//BDTR_PORTA = pIni->ReadInteger( "BDTR", "BDTR_PORT", BDTR_PORTA );
//BDTR_PORTA_CMD = pIni->ReadInteger( "BDTR", "BDTR_PORT_CMD", BDTR_PORTA_CMD );
//BDTR_GRUPO_INTEGRIDADE = pIni->ReadInteger( "BDTR", "INTEGRITY_GROUP", BDTR_GRUPO_INTEGRIDADE );
//IP_BDTR1 = pIni->ReadString( "BDTR", "IP_BDTR1", IP_BDTR1 ).Trim();
//IP_BDTR2 = pIni->ReadString( "BDTR", "IP_BDTR2", IP_BDTR2 ).Trim();
//END_BDTR = pIni->ReadInteger( "BDTR", "ADDR_BDTR", END_BDTR );
//END_IHM = pIni->ReadInteger( "BDTR", "ADDR_IHM", END_IHM );
//ENVIA_COMANDOS = pIni->ReadInteger( "BDTR", "SEND_COMMANDS", ENVIA_COMANDOS );
//BDTR_ACERTO_HORA = pIni->ReadInteger( "BDTR", "ACCEPT_TIME", BDTR_ACERTO_HORA );
//BDTR_PERIODO_ENVIO_ACERTO_HORA = pIni->ReadInteger( "BDTR", "SEND_TIME_PERIOD", BDTR_PERIODO_ENVIO_ACERTO_HORA );
// PONTO_OPERACAO = pIni->ReadInteger("BDTR", "OPERATION_POINT", PONTO_OPERACAO );

IHMRED_IP_OUTRO_IHM = pIni->ReadString( "REDUNDANCY", "OTHER_HMI_IP", IHMRED_IP_OUTRO_IHM ).Trim();
IHMRED_ACERTO_HORA =  pIni->ReadInteger( "REDUNDANCY", "ACCEPT_TIME", IHMRED_ACERTO_HORA );
IHMRED_PERIODO_ENVIO_ACERTO_HORA = pIni->ReadInteger( "REDUNDANCY", "SEND_TIME_PERIOD", IHMRED_PERIODO_ENVIO_ACERTO_HORA );
IHMRED_HTTP_PORT = pIni->ReadInteger( "REDUNDANCY", "HTTP_PORT", IHMRED_HTTP_PORT );

UDP_JSON_PORT = pIni->ReadInteger( "JSON", "UDP_JSON_PORT", UDP_JSON_PORT );
UDP_JSON_PORT_CMD = pIni->ReadInteger( "JSON", "UDP_JSON_PORT_CMD", UDP_JSON_PORT_CMD );
UDP_JSON_ENDPOINT1 = pIni->ReadString( "JSON", "UDP_JSON_ENDPOINT1", "127.0.0.1" );
UDP_JSON_ENDPOINT2 = pIni->ReadString( "JSON", "UDP_JSON_ENDPOINT2", IHMRED_IP_OUTRO_IHM );

NUMMAX_ALARMES = pIni->ReadInteger( "WEBSERVER", "MAX_EVENTS", NUMMAX_ALARMES );
PRIORIDADE_PANICO = pIni->ReadInteger( "WEBSERVER", "PANIC_PRIORITY", PRIORIDADE_PANICO );
LAST_PRIORITY_THAT_BEEPS = pIni->ReadInteger( "WEBSERVER", "LAST_PRIORITY_THAT_BEEPS", LAST_PRIORITY_THAT_BEEPS );
WEBSERVER_CLIENTES_REMOTOS = pIni->ReadString( "WEBSERVER", "REMOTE_CLIENTS", WEBSERVER_CLIENTES_REMOTOS ).Trim();
// adiciona a própria máquina e o ihm redundante à lista de clientes, vou autorizar as requisições HTTP por esta lista
WEBSERVER_CLIENTES_REMOTOS = WEBSERVER_CLIENTES_REMOTOS + ",127.0.0.1," + IHMRED_IP_OUTRO_IHM;
ALARMA_DUPLA_TRANSICAO = pIni->ReadInteger( "WEBSERVER", "DOUBLE_TRANSITION_ALARM", ALARMA_DUPLA_TRANSICAO );
WEBSERVER_CORS_ORIGIN = pIni->ReadString( "WEBSERVER", "WEBSERVER_CORS_ORIGIN", WEBSERVER_CORS_ORIGIN );
WEBSERVER_HTTPD_MODE = pIni->ReadInteger( "WEBSERVER", "HTTPD_MODE", WEBSERVER_HTTPD_MODE );

HIST_RECORD = pIni->ReadInteger( "HIST", "RECORD", HIST_RECORD );
HIST_DEADBANDFACTOR = pIni->ReadInteger( "HIST", "DEADBAND_FACTOR", HIST_DEADBANDFACTOR );
HIST_LIFETIME = pIni->ReadInteger( "HIST", "LIFETIME", HIST_LIFETIME );
DB_POSTGRESQL = pIni->ReadInteger( "HIST", "DB_POSTGRESQL", DB_POSTGRESQL );
DB_MONGODB = pIni->ReadInteger( "HIST", "DB_MONGODB", DB_MONGODB );

HIDE = pIni->ReadInteger( "RUN", "HIDE", 0 );
SIMULACAO = pIni->ReadInteger( "RUN", "SIMULATION", 0 );
LISTA_ESCRAVOS = pIni->ReadString( "RUN", "SLAVE_LIST", LISTA_ESCRAVOS ).Trim();
String s = LISTA_ESCRAVOS;
int cntslv=0;
for (int i = 1, ini = 1; i <= s.Length() && cntslv < MAX_ESCRAVOS_SIMUL; i++ )
  {
  if ( s[i] == ',' )
    {
    LISTA_ESCRAVOS_SIMUL[cntslv] = s.SubString( ini, i - ini ).Trim().c_str();
    ini = i + 1;
    cntslv++;
    }
  else
  if (i==s.Length())
      {
      LISTA_ESCRAVOS_SIMUL[cntslv] = s.SubString( ini, i - ini + 1).Trim().c_str();
      cntslv++;
      }
  }
LISTA_ESCRAVOS_SIMUL[cntslv] = "";

BEEP_INTSPEAKER = pIni->ReadInteger( "RUN","BEEP_INTSPEAKER", BEEP_INTSPEAKER );

VISOR_EVENTOS = pIni->ReadString( "RUN","EVENTS_VIEWER", "" );
VISOR_TABULAR = pIni->ReadString( "RUN","TABULAR_VIEWER", "" );
VISOR_TELAS = pIni->ReadString( "RUN","SCREEN_VIEWER", "" );
VISOR_CURVAS = pIni->ReadString( "RUN","CURVES_VIEWER", "" );
VISOR_TENDENCIAS = pIni->ReadString( "RUN","TREND_VIEWER", "" );

RUN_LINUX = pIni->ReadInteger( "RUN","LINUX", RUN_LINUX );
RUN_VARREDORIEC104 = pIni->ReadString( "RUN","PROTOCOL_DRIVER", RUN_VARREDORIEC104 );

delete pIni;

// read internationalization options
pIni = new TIniFile( ARQUIVO_INI_I18N );
if ( pIni == NULL )
  return 1;

WEBSERVER_DATE_FMT = pIni->ReadString( "WEBSERVER", "DATE_FMT", WEBSERVER_DATE_FMT );

return 0;
}

void IHM_VaiFinalizar(void)
{
IHM_FINALIZANDO = 1;
}

int IHM_EstaFinalizando(void)
{
return IHM_FINALIZANDO;
}

void PasswdToHexStr( char * pswd, char * hexres )
{
  char shares[22];
  sha1( pswd, strlen(pswd), shares );
  shares[20] = 0;
  hexres[0] = 0;

  for ( int i = 0; i < 20; i++ )
    {
    sprintf( hexres + strlen( hexres ), "%02X", (unsigned char)shares[i] );
    }
}

// Testa senha comparando com a gravada, retorna verdadeiro se igual
bool PasswdTest( char * pswd )
{
  char res[50];
  String SenhaAdm;

  TIniFile *pIni = new TIniFile( ARQUIVO_INI_SHELL );
  if ( pIni == NULL )
    {
    return false;
    }
  else
    {
    SenhaAdm  = pIni->ReadString( "IHMSHELL", "PASSWORD", SENHA_ADM );
    }
  
  if ( strcmp ( pswd, "" ) == 0 )  // sem senha digitada dá falso
     return false; 

  PasswdToHexStr( pswd, res ); // faz o hash

  return ( SenhaAdm == res ); // compara com o hash
}

// Loga mensagem em arquivo 
void Loga(String msg, String arq )
{
FILE *fp;

  if ( ( fp = fopen( arq.c_str(), "at" ) ) != NULL )
    {
    TDateTime dt;
    dt = dt.CurrentDateTime();
    String S = dt.FormatString( "yy/mm/dd hh:nn:ss(" );
    String Saux;
    S = S + Saux.sprintf( "%03d", GetTickCount() % 1000 );
    S = S + ") - ";
    fputs( S.c_str(), fp );
    fputs( msg.c_str(), fp );
    fputs( "\n", fp );
    fclose( fp );
    }
}

void ExecExternApp(char * cmd)
{
  PROCESS_INFORMATION pi;
  STARTUPINFO si;
  memset(&si, 0, sizeof(si));
  memset(&pi, 0, sizeof(pi));
  si.cb = sizeof(STARTUPINFO);
  CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}

