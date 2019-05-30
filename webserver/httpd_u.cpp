//---------------------------------------------------------------------------
// Web server based on libmicrohttpd (via dll).
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
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdlib.h>
#include <string>
#include "httpd_u.h"
#include "microhttpd.h"
#include "bcolocal.h"
#include "vedados.h"
#include "config.h"
#include "dumpdb_u.h"
#include "sha1.h"
#include "webserver_u.h"
#include "eventos_u.h"
#include "bdtr_com.h"
#include "lua_u.h"
#include "simul.h"
#include "i104m_u.h"
#include "json_u.h"

#define POSTBUFFERSIZE  50000
#define MAXNAMESIZE     20
#define MAXANSWERSIZE 512
#define GET  0
#define POST 1

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmMicroHttpd *fmMicroHttpd;

using namespace std;

int sort_function( const void *a, const void *b );
int sort_function_alm_time( const void *a, const void *b );
bool CALLBACK enumwndprc(HWND hWnd, LPARAM lParam);
void StringReplaceCh(String &s, char cf, char cr);

const char* ws = " \t\n\r\f\v";

struct connection_info_struct
{
  int connectiontype;
  char *answerstring;
  struct MHD_PostProcessor *postprocessor;
};

// trim from end of string (right)
inline std::string& rtrim(std::string& s, const char* t = ws)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const char* t = ws)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
inline std::string& trim(std::string& s, const char* t = ws)
{
    return ltrim(rtrim(s, t), t);
}

//---------------------------------------------------------------------------
void TfmMicroHttpd::
request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe)
{
/*
fmMicroHttpd->logln("Request completed");
if (con_info->connectiontype == POST)
  {
    fmMicroHttpd->logln("Request completed free");
    MHD_destroy_post_processor (con_info->postprocessor);
    struct connection_info_struct *con_info = (connection_info_struct *)*con_cls;
    if (NULL != con_info) {
    if (con_info->answerstring) {
      free (con_info->answerstring);
      }
     free (con_info);
     }
  }
  
*con_cls = NULL;
*/

}

//---------------------------------------------------------------------------
int TfmMicroHttpd::
iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
              const char *filename, const char *content_type,
              const char *transfer_encoding, const char *data, uint64_t off,
              size_t size)
{
  //fmMicroHttpd->logln("Iterate post");
  //struct connection_info_struct *con_info = (connection_info_struct *)coninfo_cls;
  //con_info->answerstring = NULL;
/*
  if (0 == strcmp (key, "name"))
    {
      if ((size > 0) && (size <= MAXNAMESIZE))
        {
          char *answerstring;
          answerstring = (char *)malloc (MAXANSWERSIZE);
          if (!answerstring)
            return MHD_NO;

          snprintf (answerstring, MAXANSWERSIZE, "", data);
          con_info->answerstring = answerstring;
        }
      else
        con_info->answerstring = NULL;

      return MHD_NO;
    }
*/
  return MHD_YES;
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::mhd_EnqueueResponse(
  struct MHD_Connection * connection,
  const char * str,
  int mime)
{
struct MHD_Response * response;
response = MHD_create_response_from_buffer(strlen(str), (void *)str, MHD_RESPMEM_MUST_COPY);
switch (mime)
  {
  case MIME_JSON:
    MHD_add_response_header (response, "Content-Type", "application/x-javascript; charset=UTF-8");
    break;
  case MIME_JAVASCRIPT:
    MHD_add_response_header (response, "Content-Type", "application/json");
    break;
  default:
    MHD_add_response_header (response, "Content-Type", "text/plain");
    break;
  }

if ( WEBSERVER_CORS_ORIGIN != "" ) //  add a CORS header to responses if desired
  MHD_add_response_header (response, "Access-Control-Allow-Origin", WEBSERVER_CORS_ORIGIN.c_str());

int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
MHD_destroy_response(response);
return ret;
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandlePointInfoRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
 int nponto = 0;
 const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "I");
 if ( paramval != NULL )
   {
   nponto = atoi(paramval);
   }
 if ( nponto == 0 )
   return MHD_YES;

 String sResp = BL.ConsultaInfoPonto( nponto );

 // insert callback if specified
 paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "B");
 if ( paramval != NULL )
   {
   sResp = sResp + (String)"\n" +
           paramval +
          (String)"();\n";
   }

 // ack alarm if specified
 paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "R");
 if ( paramval != NULL )
   {
     BL.ReconheceAlarmePonto( nponto );
     ReconheceEvento( nponto, // NPONTO
                      0, // YEAR
                      0, // MONTH
                      0, // DAY
                      0, // HOUR
                      0, // MIN
                      0, // SEC
                      0, // MSEC
                      0
                      );
     BL.SilenciaBeep();
   }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleSilenceRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }

  String sResp = "{\"error\": \"none\"}";

  const MHD_ConnectionInfo* info = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
  const sockaddr* client_addr = (info ? info->client_addr : NULL);
  char * client_ip = inet_ntoa( ((sockaddr_in *)client_addr)->sin_addr );

  // Silence beep request, do not ack alarms
  const char * paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "Z");
  if (paramval != NULL)
    {
      sResp = "{\"error\": \"none\"}";
      BL.SilenciaBeep();
      // if there is another redundant machine forward message
      if ( IHMRED_IP_OUTRO_IHM != "" )
      if ( (String)client_ip != IHMRED_IP_OUTRO_IHM )
          BL.lstHTTPReq_OutroIHM.push_back( (String)url + (String)"?Z=1" );
    }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JSON);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleCommandRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }

String sResp;
String sPonto;
String sVal;
String sRealIp;

const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "K");
if ( paramval != NULL )
  sPonto = paramval;

paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "V");
if ( paramval != NULL )
  sVal = paramval;

const MHD_ConnectionInfo* info = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
const sockaddr* client_addr = (info ? info->client_addr : NULL);
char * client_ip = inet_ntoa( ((sockaddr_in *)client_addr)->sin_addr );
const char * realip = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, "X-Real-IP");
if ( realip != NULL )
  sRealIp = realip;

 int cnponto = 0, val = 0, ret = 0;
 double fval = 0;
 bool found = false;
 TPonto pt;

 try {
   cnponto = sPonto.ToInt();
   pt = BL.GetRefPonto( cnponto, found );
 }
 catch (Exception &E){ // invalid conversion to int: command as a tag
   pt = BL.GetRefPontoByTag(sPonto, found);
 }

 if ( found )
   {
   cnponto = pt.NPonto;
   if ( pt.EhComandoDigital() )
     val = atoi(sVal.c_str());
   else
     fval = atof(sVal.c_str());
   }

 if ( found && pt.EhComando() && !BL.ComandoIntertravado( cnponto ) ) // testa se o ponto existe, é de comando e não está intertravado
   {
   // intercepta comando em script lua, se retorna 0 continua ao protocolo/simul
   if ( fmLua->luaInterceptCommands( cnponto, val ) == 0 )
     {
     if ( ( BL.GetSimulacao() == SIMULMOD_MESTRE ) || ( BL.GetSimulacao() == SIMULMOD_LOCAL ) )
       {
       if ( pt.EhComandoDigital() )
         {
         ret = fmSimul->SimulaComando( cnponto, val );
         confCmdSimul = ret;
         }
       else
         {
         ret = fmSimul->SimulaComando( cnponto, fval );
         BL.IncNumVar(); // reporta variação para que a aplicação atualize o status mais rapidamente
         confCmdSimul = ret;
         }
       }
     else
       {
       if ( IP_BDTR1 == "" ) // se não tem bdtr, manda pelo i104m
         {
         // manda o comando para o protocolo (varredor iec104m)
         if ( pt.EhComandoDigital() )
           {
           ret = fmIEC104M->ComandoIEC_Dig( cnponto, val&0x01?0:1 );
           Loga( (String)"Command Sent(IEC): IP=" + (String)client_ip + (String)", RealIP=" + sRealIp + (String)", user=" + (String)BL.getUserName() + (String)", point=" + (String)cnponto + (String)", val=" + (String)val + (String)", id=" + (String)pt.GetNome(), ARQUIVO_LOGCMD );
           }
         else
           {
           ret = fmIEC104M->ComandoIEC_Ana( cnponto, fval );
           BL.IncNumVar(); // reporta variação para que a aplicação atualize o status mais rapidamente
           Loga( (String)"Command Sent(IEC): IP=" + (String)client_ip + (String)", RealIP=" + sRealIp + (String)", user=" + (String)BL.getUserName() + (String)", point=" + (String)cnponto + (String)", val=" + (String)fval + (String)", id=" + (String)pt.GetNome(), ARQUIVO_LOGCMD );
           }
         }
       else
         {
         // Testa se o comando é analógico ou digital
         if ( pt.EhComandoDigital() )
           {
           ret = fmBDTR->bdtr.MandaComandoDig( cnponto, val );
           Loga( (String)"Command Sent(BDTR): IP=" + (String)client_ip + (String)", RealIP=" + sRealIp + (String)", user=" + (String)BL.getUserName() + (String)", point=" + (String)cnponto + (String)", val=" + (String)val + (String)", id=" + (String)pt.GetNome(), ARQUIVO_LOGCMD );
           }
         else
           {
           ret = fmBDTR->bdtr.MandaComandoAna( cnponto, fval );
           BL.IncNumVar(); // reporta variação para que a aplicação atualize o status mais rapidamente
           Loga( (String)"Command Sent(BDTR): IP=" + (String)client_ip + (String)", RealIP=" + sRealIp + (String)", user=" + (String)BL.getUserName() + (String)", point=" + (String)cnponto + (String)", val=" + (String)fval + (String)", id=" + (String)pt.GetNome(), ARQUIVO_LOGCMD );
           }
         }

       if (UDP_JSON_PORT_CMD != 0)
         {
         if ( pt.EhComandoDigital() )
           {
           ret = fmJSON->Command( pt, val&0x01?0:1 ); // val=2 -> ON(true), val=1 -> OFF(false)
           Loga( (String)"Command Sent(JSON): IP=" + (String)client_ip + (String)", RealIP=" + sRealIp + (String)", user=" + (String)BL.getUserName() + (String)", point=" + (String)cnponto + (String)", val=" + (String)val + (String)", id=" + (String)pt.GetNome(), ARQUIVO_LOGCMD );
           }
         else
           {
           ret = fmJSON->Command( pt, fval);
           BL.IncNumVar(); // reporta variação para que a aplicação atualize o status mais rapidamente
           Loga( (String)"Command Sent(JSON): IP=" + (String)client_ip + (String)", RealIP=" + sRealIp + (String)", user=" + (String)BL.getUserName() + (String)", point=" + (String)cnponto + (String)", val=" + (String)fval + (String)", id=" + (String)pt.GetNome(), ARQUIVO_LOGCMD );
           }
         }
       }
     }

   cmdNPonto = cnponto;
   cmdCntAck = BL.GetAckCmd( cnponto, NULL, NULL, NULL );
   }
 else
   ret = -1;

 if ( ret == 0 ) // testa sucesso do comando
   sResp = "ComandoEnviado='" +
           (String)cnponto +
           "," +
           (String)val +
           (String)"';";

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleAckCommandRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
String sResp;
String sPonto;

const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "A");
if ( paramval != NULL )
  sPonto = paramval;

if ( sPonto == "" )
  {
  sResp = "ComandoAck='Invalid command!';";
  return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
  }

 int cnponto, falha, cmd, cnt;
 double hora;

 cnponto = atoi(sPonto.c_str());
 if ( cnponto == 0 )
   {
   sResp = "ComandoAck='Command not found!';";
   return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
   }

 cnt = BL.GetAckCmd( cnponto, &falha, &cmd, &hora );

 if ( cnt == -1 )
   {
   sResp = "ComandoAck='Command Unknown: " + (String)cnponto + "';";
   return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
   }

 if ( BL.GetSimulacao() == SIMULMOD_LOCAL )
   {
   if ( confCmdSimul )
     sResp = "ComandoAck='Command Rejected: " + (String)cnponto + "," + (String)cmd + "';";
   else
     sResp = "ComandoAck='OK: " + (String)cnponto + "," + (String)cmd + "';";
   return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
   }

 if ( cnponto == cmdNPonto && cnt == cmdCntAck )
   {
   sResp = "ComandoAck='Command Unconfirmed: " + (String)cnponto + "';";
   return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
   }

 if ( cnponto == cmdNPonto && cnt != cmdCntAck )
   {
   if ( falha )
     sResp = "ComandoAck='Command Rejected: " + (String)cnponto + "," + (String)cmd + "';";
   else
     sResp = "ComandoAck='OK: " +(String)cnponto + "," + (String)cmd + "';";
   return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
   }

 sResp = "ComandoAck='';";

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleImposePointRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
String sResp;
String sPonto;

const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "X");
if ( paramval != NULL )
  sPonto = paramval;

const MHD_ConnectionInfo* info = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
const sockaddr* client_addr = (info ? info->client_addr : NULL);
char * client_ip = inet_ntoa( ((sockaddr_in *)client_addr)->sin_addr );

int nponto = atoi(sPonto.c_str());
if ( nponto == 0 )
  return MHD_YES;
   
String V, F;

try {
  double valor;
  unsigned char flg;
  String tipopt = BL.GetTipo(nponto);

  paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "V");
  if ( paramval != NULL )
    V = paramval;

  paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "FL");
  if ( paramval != NULL )
    F = paramval;

  // troca o ponto pela vírgula para converter
  if ( tipopt == "D" )
    if ( V != "0" )
      V = "1";
  if ( F == "" )
    {
    if ( tipopt == "D" )
      {
      if ( V == "0" )
        F = (unsigned)0x02;
      else
        F = (unsigned)0x01;
      }
    else
      {
      F = (unsigned)0x20;
      }
    }

  valor = atof( V.c_str() );
  flg = atoi( F.c_str() );
  IncluiEvento(
        nponto,
        SDE_UTREVSIMUL, // identifica UTR simulacao
        flg,
        0,
        SDE_GERAHORAMS, // data/hora atual
        0,
        0,
        0,
        0,
        0);

  BL.EscrevePonto( nponto, valor, flg, 0, 0 );
  if ( tipopt != "D" ) // para analógicos
    BL.SetValorTipico( nponto, valor ); // guarda como valor típico para simulação

  // se tem simulacao mestre, vou encaminha para os escravos
  if ( BL.GetSimulacao() == SIMULMOD_MESTRE )
    {
    if ( tipopt == "A" )
      {
      msg_float ms;
      ms.COD = T_FLT|T_CONV;
      ms.NRPT = 1;
      ms.ORIG = 0;
      ms.PONTO[0].ID = nponto;
      ms.PONTO[0].VALOR = (float)valor;
      ms.PONTO[0].STAT = flg;
      fmBDTR->EnviaEscravosBDTR( (char *)&ms, sizeof(ms) );
      }
    else
      {
      msg_dig_tag ms;
      ms.COD = T_DIG_TAG|T_CONV;
      ms.NRPT = 1;
      ms.ORIG = 0;
      fmBDTR->TagBDTR_HoraAtual( &ms.PONTO[0].TAG );
      ms.PONTO[0].ID = nponto;
      ms.PONTO[0].UTR = 0;
      ms.PONTO[0].STAT = flg;
      fmBDTR->EnviaEscravosBDTR( (char *)&ms, sizeof(ms) );
      }
    }
  }
  catch ( Exception &E )
    { logln( "E: 10-" + E.Message ); }

  // se tem outro ihm e não veio dele vou encaminhar a mensagem
  if ( IHMRED_IP_OUTRO_IHM != "" )
  if ( (String)client_ip != IHMRED_IP_OUTRO_IHM )
    BL.lstHTTPReq_OutroIHM.push_back( (String)url +
                                      (String)"?X=" + nponto +
                                      (String)"&V=" + V +
                                      (String)"&FL=" + F
                                     );

sResp = BL.ConsultaInfoPonto( nponto ); // NPONTO

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleWritePointRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
String sResp = "{\"error\": \"none\"}";
String sPonto;
String sLI, sLS, sHys, sAlrInh, sAnot, sVN;
int nponto;

const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "W");
if ( paramval != NULL )
sPonto = paramval;

paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "LI");
if ( paramval != NULL )
  sLI = paramval;
paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "LS");
if ( paramval != NULL )
  sLS = paramval;
paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "HI");
if ( paramval != NULL )
  sHys = paramval;
paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "AI");
if ( paramval != NULL )
  sAlrInh = paramval;
paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "AN");
if ( paramval != NULL )
  sAnot = paramval;
paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "VN");
if ( paramval != NULL )
  sVN = paramval;

const MHD_ConnectionInfo* info = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
const sockaddr* client_addr = (info ? info->client_addr : NULL);
char * client_ip = inet_ntoa( ((sockaddr_in *)client_addr)->sin_addr );
String sRealIp;
const char * realip = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, "X-Real-IP");
if ( realip != NULL )
  sRealIp = realip;

nponto = atoi(sPonto.c_str());
if ( nponto == 0 )
  return MHD_YES;
  
bool found;
TPonto &pt = BL.GetRefPonto( nponto, found );

if ( found )
  {
  String S; int P;
  double fval;

  try {
    // troca o ponto pela vírgula para converter
    S = sLI;
    if ( (P=S.Pos(",")) !=0 )
      S[P] = '.';
    if ( S.Trim() != "" )
      {
      fval = atof(S.c_str());
      if ( fval < LIMINFMIN )
        pt.SetLimSup( LIMINFMIN );
      else
      if ( fval <= pt.LimSup && fval <= LIMSUPMAX && fval >= LIMINFMIN )
        pt.SetLimInf( fval );
      }
    } catch ( Exception &E ) { logln( "E: 3-" + E.Message );  }

  try {
    S = sLS;
    if ( (P=S.Pos(",")) !=0 ) S[P]='.';
    if ( S.Trim() != "" )
      {
      fval = atof(S.c_str());
      if ( fval > LIMSUPMAX )
        pt.SetLimSup( LIMSUPMAX );
      else
      if ( fval >= pt.LimInf && fval <= LIMSUPMAX && fval >= LIMINFMIN )
        pt.SetLimSup( fval );
      }
    } catch ( Exception &E ) { logln( "E: 4-" + E.Message ); }

  try {
    S = sHys;
    if ( (P=S.Pos(",")) !=0 ) S[P]='.';
    if ( S.Trim() != "" )
      {
      fval = fabs(atof(S.c_str()));
        pt.SetHister( fval );
      }
    } catch ( Exception &E ) { fmMicroHttpd->logln( "E: 5-" + E.Message ); }

  try {
      bool alrin = (bool)(atoi(sAlrInh.c_str()));

      // register log for alarm inhibited (when changed)
      if ( pt.GetAlrIn() != alrin )
        Loga( (String)"IP=" + (String)client_ip +
              (String)", RealIP=" + sRealIp +
              (String)", user=" + (String)BL.getUserName() +
               (String)", point=" + (String)nponto +
              (String)", id=" + (String)pt.GetNome() +
              (String)", alm_inhibit=" + (String)((int)alrin),
              ARQUIVO_LOGALMINHIB );

      pt.SetAlrIn( alrin );
  } catch ( Exception &E ) { logln( "E: 6-" + E.Message ); }

  try {
    String anot = sAnot.Trim();

    // se entrou ou saiu anotação, bloqueia ou libera comando
    if ( ((int)( pt.TemAnotacao() )) ^ ((int)( anot != "" )) )
    if ( (String)client_ip != IHMRED_IP_OUTRO_IHM ) // se não vem do outro IHM, é porque vem deste, então pode mandar o bloqueio
      {
      // vou bloquear ou liberar o comando do ponto no BDTR dependendo se tem ou não anotação
      int cnponto;
      cnponto = pt.GetSupCmd( ); // descobre o ponto de comando para o supervisionado em
      if ( ( IP_BDTR1 != "" ) && ( cnponto != 0 ) ) // se tem bdtr e tem comando associado
        {
        bool found;
        TPonto &ptcmd = BL.GetRefPonto(cnponto, found);

        if ( found )
          {
          if ( anot != "" )
            fmBDTR->bdtr.BlkComando( ptcmd.Endereco, 1 ); // bloqueia comando
          else
            fmBDTR->bdtr.BlkComando( ptcmd.Endereco, 0); // libera comando
          }
        }
      }

    if ( strcmp(pt.GetAnotacao(), anot.c_str() ) )
      { // mudou a anotação: loga
      Loga( (String)"IP=" + (String)client_ip +
            (String)", RealIP=" + sRealIp +
            (String)", user=" + (String)BL.getUserName() +
            (String)", point=" + (String)nponto +
            (String)", id=" + (String)pt.GetNome(),
            ARQUIVO_LOGANOT );
      Loga( (String)"TEXT=" + (String)anot, ARQUIVO_LOGANOT );
      }
    pt.SetAnotacao( anot.c_str() );

    } catch ( Exception &E ) { logln( "E: 7-" + E.Message ); }

  //try {
  //  pt.SetValorNormal( ARequestInfo->Params->Strings[6].SubString(4,1).ToInt()?1:0 );
  //  } catch ( Exception &E ) { logaln( "E: 8-" + E.Message ); }

  BL.lstNPontoDump.push_back( nponto ); // vou colocar numa lista para descarregar depois

  // se tem outro ihm e não veio dele vou encaminhar a mensagem
  if ( IHMRED_IP_OUTRO_IHM != "" )
  if ( (String)client_ip != IHMRED_IP_OUTRO_IHM )
    BL.lstHTTPReq_OutroIHM.push_back( (String)url +
                                      (String)"?W=" + nponto +
                                      (String)"&LI=" + sLI +
                                      (String)"&LS=" + sLS +
                                      (String)"&HI=" + sHys +
                                      (String)"&AI=" + sAlrInh +
                                      (String)"&AN=" + sAnot +
                                      (String)"&VN=" + sVN
                                     );
  }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JSON);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleAckRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
String sResp = "{\"error\": \"none\"}";
String sPonto;
String sData;
String sDataOrig;
String sAgrega;
String sHMS;
String sMsec;
int Remove = 0;

 const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "R"); // ack
 if ( paramval != NULL )
   sPonto = paramval;

 paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "Q"); // remove
 if ( paramval != NULL )
   {
   sPonto = paramval;
   Remove = 1;
   }

 if ( sPonto == "" )
    return MHD_YES;

 int nponto = atoi(sPonto.c_str());

 BL.ReconheceAlarmePonto( nponto );

 paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "D");
 if ( paramval != NULL )
   {
   sData = paramval;
   sDataOrig = sData;
   }

 paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "H");
 if ( paramval != NULL )
   sHMS = paramval;

 paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "M");
 if ( paramval != NULL )
   sMsec = paramval;

 paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "A");
 if ( paramval != NULL )
   sAgrega = paramval;

 int dd, mm, yy, p1, p2;

 try
 {
 // convert according to date/time format
 // need to be updated to support more date/time formats
 if ( WEBSERVER_DATE_FMT == "dd/mm/yy hh:nn:ss" || WEBSERVER_DATE_FMT == "dd/mm/yyyy hh:nn:ss" )
   {
   dd = atoi(sData.SubString( 1, ( p1 = sData.Pos("/") ) - 1 ).c_str());
   sData[p1] = ' ';
   mm = atoi(sData.SubString( p1 + 1, ( p2 = sData.Pos("/")) - p1 - 1 ).c_str());
   sData[p2] = ' ';
   yy = atoi(sData.SubString( p2 + 1, 5 ).c_str());
   }
 else // if ( WEBSERVER_DATE_FMT == "yy/mm/dd hh:nn:ss" ||  WEBSERVER_DATE_FMT == "yyyy/mm/dd hh:nn:ss" )
 if ( WEBSERVER_DATE_FMT == "mm/dd/yy hh:nn:ss" ||  WEBSERVER_DATE_FMT == "mm/dd/yyyy hh:nn:ss" )
   {
   mm = atoi(sData.SubString( 1, ( p1 = sData.Pos("/") ) - 1 ).c_str());
   sData[p1] = ' ';
   dd = atoi(sData.SubString( p1 + 1, ( p2 = sData.Pos("/")) - p1 - 1 ).c_str());
   sData[p2] = ' ';
   yy = atoi(sData.SubString( p2 + 1, 5 ).c_str());
   }
 else // must be: if ( WEBSERVER_DATE_FMT == "yy/mm/dd hh:nn:ss" ||  WEBSERVER_DATE_FMT == "yyyy/mm/dd hh:nn:ss" )
   {
   yy = atoi(sData.SubString( 1, ( p1 = sData.Pos("/") ) - 1 ).c_str());
   sData[p1] = ' ';
   mm = atoi(sData.SubString( p1 + 1, ( p2 = sData.Pos("/")) - p1 - 1 ).c_str());
   sData[p2] = ' ';
   dd = atoi(sData.SubString( p2 + 1, 2 ).c_str());
   }
  }
  catch ( Exception &E )
    { logln( "Exception: AckRequest " + E.Message ); }

 if ( yy < 100 )
   yy = yy + 2000;

  // logln( (String)"dd=" + (String)dd + (String)"mm=" + (String)mm + (String)"yy=" + (String)yy );

  const MHD_ConnectionInfo* info = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
  const sockaddr* client_addr = (info ? info->client_addr : NULL);
  char * client_ip = inet_ntoa( ((sockaddr_in *)client_addr)->sin_addr );

 // quando agrega reconhece todos alarmes do ponto, senão só o especificado
 if ( atoi(sAgrega.c_str()) == 1 ) // agrega?
   {
   ReconheceEvento( nponto, // NPONTO
                    0,                                                       // ANO
                    0,                                                       // MES
                    0,                                                       // DIA
                    0, // HORA
                    0, // MIN
                    0, // SEG
                    0, // MSEG
                    Remove
                    );
   }
 else
   {
   ReconheceEvento( nponto, // NPONTO
                    yy,                                                      // ANO
                    mm,                                                      // MES
                    dd,                                                      // DIA
                    atoi(sHMS.SubString(1,2).c_str()), // HORA
                    atoi(sHMS.SubString(4,2).c_str()), // MIN
                    atoi(sHMS.SubString(7,2).c_str()), // SEG
                    atoi(sMsec.c_str()), // MSEG
                    Remove
                    );
  }

 BL.SilenciaBeep();

 // se tem outro ihm e não veio dele vou encaminhar a mensagem
 if ( IHMRED_IP_OUTRO_IHM != "" )
 if ( (String)client_ip != IHMRED_IP_OUTRO_IHM )
   BL.lstHTTPReq_OutroIHM.push_back( (String)url +
                                     (String)(Remove?"?Q=":"?R=") + (String)nponto +
                                     (String)"&D=" + (String)sDataOrig +
                                     (String)"&H=" + (String)sHMS +
                                     (String)"&M=" + (String)sMsec +
                                     (String)"&A=" + (String)sAgrega
                                    );

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JSON);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleStatusRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
String sResp;

 if ( BL.HaBeepAtivo() )
   sResp = sResp + "HA_ALARMES=1;\n";
 else
   sResp = sResp + "HA_ALARMES=0;\n";

 sResp = sResp + (String)"NUM_VAR=" + (String)BL.GetNumVar() + (String)";\n";

 // insert callback if specified
 const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "B");
 if ( paramval != NULL )
   {
   sResp = sResp + (String)"\n" +
           paramval +
          (String)"();\n";
   }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleStationListRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }    
 String sResp;

 char buf[10000];
 BL.GetListaSEs(buf);
 sResp = (String)"\nLISTA_SES='" + (String)buf + (String)"';";

 // insert callback if specified
 const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "B");
 if ( paramval != NULL )
   {
   sResp = sResp + (String)"\n" +
           paramval +
          (String)"();\n";
   }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleHideRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
String sResp = "{\"error\": \"none\"}";

const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "H");

if ( HIDE )
  { // only accepts external command to hide/show if configured to hide initially
  if ( (String)paramval == (String)"2" )
    fmVeDados->Close();
  if ( (String)paramval == (String)"1" )
    fmVeDados->Hide();
  else
    fmVeDados->Show();
  }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JSON);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleHistListRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
String sResp;

 // list all switches, breakers and analogs
 int cnt = 0;
 char buf[100];
 map <int, TPonto> MpPontos;
 map <int, TPonto>::iterator it;

 MpPontos = BL.GetMapaPontos();

 for ( it = MpPontos.begin(); it != MpPontos.end(); it++ )
     {
     // analog
     if ( ((*it).second).TipoAD == 'A' )
       {
       // nponto flags valor casadecimal
       sprintf( buf,"%.3f",((*it).second).Valor );
       sResp = sResp +
              (String) (*it).first + (String)" " +
              (String) ((*it).second).Qual.Byte + (String)" " +
              (String) buf + (String)" " +
              (String) ((*it).second).CasaDecimal + (String)" " +
              (String)"\n";
       cnt++;
       }
     else
     // switch or breaker
     if ( ((*it).second).CodOrigem != 6 && ((*it).second).EhComando() && ((*it).second).CodInfo == 0 && (((*it).second).CodTpEq == 27 || ((*it).second).CodTpEq == 28) )
       {
       // nponto flags
       sResp = sResp +
              (String) (*it).first + (String)" " +
              (String) ((*it).second).Qual.Byte + (String)" " +
              (String)"\n";
       cnt++;
       }

     if ( cnt > 8000 )
        break;
     }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleExecExtAppRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
String sResp = "{\"error\": \"none\"}";
String sVal;

const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "XX");
if ( paramval != NULL )
  sVal = paramval;

 if ( sVal == (String)"1" )
   { // visor de telas
   String indtela;
   paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "INDTELA");
   if ( paramval != NULL )
      indtela = (String)"?INDTELA=" + (String)paramval;
   ExecExternApp( (VISOR_TELAS + indtela).c_str() );
   }
 else
 if ( sVal == (String)"2" )
   { // events viewer
   JANELA_AENCONTRAR = "Visor de Eventos - OSHMI";
   EnumWindows( (int (__stdcall *)()) enumwndprc, 0 );
   // deixa só uma janela de eventos aberta
   if ( JANELA_ENCONTRADA == 0 )
     ExecExternApp(VISOR_EVENTOS.c_str());
   }
 else
 if ( sVal == (String)"3" )
   { // filtered tabular viewer
   String t = VISOR_TABULAR;
   paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "S");
   if ( paramval != NULL )
     {
     t = t + (String)"?SUBST=" + paramval;
     }
   paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "B");
   if ( paramval != NULL )
     {
     t = t + (String)"?BAY=" + paramval;
     }
   ExecExternApp( t.c_str() );
   }
 else
 if ( sVal == (String)"4" )
   { // curves viewer
   ExecExternApp( VISOR_CURVAS.c_str() );
   }
 else
 if ( sVal == (String)"5" )
   { // events viewer in historical mode
   ExecExternApp((VISOR_EVENTOS+"?MODO=4").c_str());
   }
 else
 if ( sVal == (String)"6" )
   { // tabular viewer
   String t = VISOR_TABULAR;

   JANELA_AENCONTRAR = "Visor de Alarmes - OSHMI";
   EnumWindows( (int (__stdcall *)()) enumwndprc, 0 );
   if ( JANELA_ENCONTRADA == 0 ) // se não achou executa
     ExecExternApp( (t+"?SELMODULO=TODOS_ANORMAIS").c_str() );
   }
 else
 if ( sVal == (String)"7" )
   { // visor de tendencias
   paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "PNT");
   if ( paramval != NULL )
      {
      String ponto;
      ponto = (String)"?NPONTO=" + (String)paramval;
      ExecExternApp( (VISOR_TENDENCIAS + ponto).c_str() );
      }
   }                 

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JSON);
}

int TfmMicroHttpd::ahc_HandleSyncDataRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
String sResp;

// get values to sync redundant HMI
// answer with all data that params are different from default
// Anotation, inhibit, limits, hysteresis, time tag of last change

char buf[2000];
map <int, TPonto> &PontosTR = BL.GetMapaPontos();
map <int, TPonto>::iterator it;

sResp = sResp + "SINCRONISMO\n";

for ( it = PontosTR.begin(); it != PontosTR.end(); it++ )
   {
   TPonto &pto = ((*it).second);

   if (   pto.NPonto != 0 &&  // do not insert nponto==0
          !pto.EhComando()    // do not insert commands
      )
   if ( pto.NPonto < NPONTO_SIST_MIN || pto.NPonto > NPONTO_SIST_MAX ) // tira os pontos internos
   if (   pto.TemAnotacao() ||
          pto.LimInf > LIMINFMIN ||
          pto.LimSup < LIMSUPMAX ||
          pto.Hister != 0.0 ||
          pto.AlarmeNaoReconhecido() ||
          pto.TagTempoAlarme != 0 ||
          // ( pto.EstadoAlarme == ESTALM_ALL && pto.TipoAD == 'D' ) ||
          pto.AlrIn != 0
      )
     {
     String S = (String)"|" + (String)pto.Anotacao;
     // nponto alarme alm_inibido lim_inf lim_sup histerese tag_tempo_alt_valor anotação
     sprintf( buf, "%d %d %d %0.3f %0.3f %0.3f %.8lf %s\n",
                   pto.NPonto,
                   pto.AlarmeNaoReconhecido(),
                   pto.AlrIn,
                   pto.LimInf,
                   pto.LimSup,
                   pto.Hister,
                   pto.TagTempoAlarme,
                   S.c_str() );
     sResp = sResp + buf;
     }
   }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleFilteredListSubstBayRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
String sResp;
char SEP = ',';

// Parameters in URL: ?G=SUBST&M=BAY&A=0&B=callback_fn
int lista[LIMITE_PONTOS_TABULAR + 1];
String Qlf;
int tipo_alm = 0;
String FilterSubst;
String FilterMod;

const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "G");
if ( paramval != NULL )
  FilterSubst = paramval;

paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "GX");
if ( paramval != NULL )
  {
  FilterSubst = paramval;
  SEP = '\t';
  }

paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "M");
if ( paramval != NULL )
  FilterMod = paramval;

paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "A");
if ( paramval != NULL )
if ( (String)paramval == (String)"2" )
 {
   tipo_alm = 2; // only persistent alarms and unacked events
 }

int taml = BL.GetListaPontosFiltSubstMod( FilterSubst.c_str(),
                                          FilterMod.c_str(),
                                          lista,
                                          tipo_alm,
                                          0 );

// coloca a data/hora da resposta
sResp = "Data='" + FormatDateTime( WEBSERVER_DATE_FMT, Now() ) + "';\n";
if ( SEP == ',' )
 sResp = sResp + (String)"L=[];\n";  // will return data in JS array L

double valor;
TFA_Qual qual;
double tagalarm;
bool temponto;
int alm, tem_anot, alrin, congel, ehcomando;
String Anot;
String Lpto;
int i;
char descr[200];

if ( tipo_alm == 2 || FilterSubst == "TODOS_ANORMAIS" )
 {
 qsort( (void *)lista, taml, sizeof(lista[0]), sort_function_alm_time );
 }
else
 {
 qsort( (void *)lista, taml, sizeof(lista[0]), sort_function );
 }

for ( i = 0; lista[i] != 0 && i < LIMITE_PONTOS_TABULAR && i < taml; i++ )
   {
   String Linha;
   TPonto &pt = BL.GetRefPonto( lista[i], temponto );

   if (temponto)
     {
     valor = pt.Valor;
     qual = pt.Qual;
     alm = pt.AlarmeNaoReconhecido();
     Anot = pt.GetAnotacao();
     tem_anot = pt.TemAnotacao();
     alrin = pt.AlarmeInibido();
     tagalarm = pt.GetTimeAlarm();
     congel = pt.Congelado();
     ehcomando = pt.EhComando();

     String SVal, ID, Subest;

     if ( SEP == '\t' )
       { // for excel use full precision
       if ( pt.TipoAD == 'A' )
         { // analog
         SVal.sprintf("%.14g",valor);
         // buggy sprintf printing 00.000xyz or -00.000xyz like values cause errors (turn 00.000 into 0.000)
         int p = SVal.Pos("00.");
         if ( p==1 || p==2 && SVal[1]=='-' )
           SVal = SVal.Delete(p, 1);
         }
       else
         SVal.sprintf("%.0f",valor);
       StringReplaceCh(SVal, '.', ','); // troca os pontos por vírgula (para o excel)
       }
     else
       SVal.sprintf("%.2f",valor);

     ID = (String)pt.GetNome();
     Subest = pt.Estacao;
     //ID.SubString(1,4);
     //if ( Subest.Length() == 4 )
     //if ( Subest[4] == '-' )
     //   Subest.SetLength(3);
     pt.GetModDescr( descr );
     String S = descr;
     StringReplaceCh(S, ',', '.'); // tira as virgulas da descricao
     if ( SEP == ',' )
        Linha = Linha + (String)"L[" + (String)i + (String)"]='";
     Linha = Linha +
        (String)lista[i] + (String)SEP +
        ID + (String)SEP +
        Subest + (String)SEP +
     S + (String)SEP;

     if ( pt.EhComando() )
       {
       Linha = Linha+
         (String)pt.GetEstadoOff() + (String)"/" + (String)pt.GetEstadoOn() + (String)SEP;
       }
     else
     if ( pt.GetTipo() == (String)"D" )
       {
       switch ( qual.Byte & ESTDUP_MASCARA ) // Norma IEC 60870-5-101: 7.2.6.2 Double-point information
         {
         case 0:
           Linha = Linha + (String)WEBSERVER_DP_TRANSIT + (String)SEP;
           break;
         case 1:
           if ( pt.EhEventoDigital() && pt.AlarmeNaoReconhecido() )
             {
             Linha = Linha + (String)pt.GetEstadoOn() +
                             //(String)"/" + (String)pt.GetEstadoOff() +
                             (String)SEP;
             }
           else
             {
             Linha = Linha + (String)pt.GetEstadoOff() + (String)SEP;
             }
           break;
         case 2:
           Linha = Linha + (String)pt.GetEstadoOn() + (String)SEP;
           break;
         case 3:
           Linha = Linha + (String)WEBSERVER_DP_INVALID + (String)SEP;
           break;
         }
       }
     else
       Linha = Linha + SVal + (String)SEP;

     int persistente = pt.TemAlarmePersistente();
     Qlf = (String)pt.GetPriority();
     if ( congel != 0 )
       Qlf = Qlf + (String)"U";
     if ( qual.Byte & 0x80 )
       Qlf = Qlf + (String)"F";
     if ( qual.Byte & 0x10 )
       Qlf = Qlf + (String)"S";
     if ( (qual.Byte & 0x0C) == 0x04 )
        Qlf = Qlf + (String)"C";
     else
     if ( (qual.Byte & 0x0C) == 0x0C )
       Qlf = Qlf + (String)"M";
     else
     if ( (qual.Byte & 0x0C) == 0x08 )
       Qlf = Qlf + (String)"X";
     if ( alm )
       Qlf = Qlf + (String)"L";
     if ( persistente )
       Qlf = Qlf + (String)"P";
     if ( alrin )
       Qlf = Qlf + (String)"I";
     if ( tem_anot )
       Qlf = Qlf + (String)"A";
     if ( pt.GetSupCmd() != 0 )
       {
       Qlf = Qlf + (String)"K";
       if ( BL.ComandoIntertravado( pt.GetSupCmd() ) )
         Qlf = Qlf + (String)"Y";
       }
     if ( ehcomando )
       Qlf = Qlf + (String)"J";

     if ( tagalarm == 0 )
       S = "";
     else
       {
       TDateTime dt = tagalarm;
       S = dt.FormatString( WEBSERVER_DATE_FMT );
       }
     Linha = Linha +
     (String)( qual.Byte | alm << 8 | tem_anot << 9  | alrin << 10 | persistente << 11 | congel << 12 )+ (String)SEP +
     (String)pt.GetSupCmd() + (String)SEP + Qlf +
     (String)SEP + (String)0 + (String)SEP + S;
     if ( SEP == ',' )
       Linha = Linha + (String)"';";
     Linha = Linha + (String)"\n";

     Lpto = Lpto + Linha;
     }
   } // for

sResp = sResp + Lpto;
if ( lista[i] != 0 && i >= LIMITE_PONTOS_TABULAR )
  {
  sResp = sResp +
  "L[" + i + (String)"]='0,LIMITE_MAX_PONTOS,THERE ARE MORE DATA,0,0,0,,0';\n";
  // i++;
  }

// informa se há beep ativo
if ( BL.HaBeepAtivo() )
 sResp = sResp + "HA_ALARMES=1;\n";
else
 sResp = sResp + "HA_ALARMES=0;\n";

sResp = sResp + (String)"NUM_VAR=" + (String)BL.GetNumVar() + (String)";\n";

// função de callback
paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "B");
// função de callback
if ( paramval != NULL )
 {
 sResp = sResp + (String)"\n" +
         paramval +
         (String)"();\n";
 }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleFilteredListRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }
String sResp;
char SEP = ',';
int lista[LIMITE_PONTOS_TABULAR + 1];
String Qlf;
int tipo_alm = 0;
String Filter;

const char * paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "F");
if ( paramval != NULL )
  Filter = paramval;

paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "FX");
if ( paramval != NULL )
  {
  Filter = paramval;
  SEP = '\t';
  }

paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "A");

if ( paramval != NULL )
if ( (String)paramval == (String)"2" )
 {
   tipo_alm = 2; // only persistent alarms and unacked events
 }

int taml = BL.GetListaPontosFilt( Filter.c_str(),
                                 lista,
                                 tipo_alm,
                                 1 );

// insert date/time
sResp = "Data='" + FormatDateTime( WEBSERVER_DATE_FMT, Now() ) + "';\n";
if ( SEP == ',' )
 sResp = sResp + (String)"L=[];\n";  // will return data in JS array L

double valor;
TFA_Qual qual;
double tagalarm;
bool temponto;
int alm, tem_anot, alrin, congel, ehcomando;
String Anot;
String Lpto;
int i;
char descr[200];

if ( tipo_alm == 2 || Filter == "TODOS_ANORMAIS" )
  {
  qsort( (void *)lista, taml, sizeof(lista[0]), sort_function_alm_time );
  }
else
  {
  qsort( (void *)lista, taml, sizeof(lista[0]), sort_function );
  }

for ( i = 0; lista[i] != 0 && i < LIMITE_PONTOS_TABULAR && i < taml; i++ )
   {
   String Linha;
   TPonto &pt = BL.GetRefPonto( lista[i], temponto );

   if (temponto)
     {
     valor = pt.Valor;
     qual = pt.Qual;
     alm = pt.AlarmeNaoReconhecido();
     Anot = pt.GetAnotacao();
     tem_anot = pt.TemAnotacao();
     alrin = pt.AlarmeInibido();
     tagalarm = pt.GetTimeAlarm();
     congel = pt.Congelado();
     ehcomando = pt.EhComando();

     String SVal, ID, Subest;

     if ( SEP == '\t' )
       { // for excel use full precision
       if ( pt.TipoAD == 'A' )
         { // analog
         SVal.sprintf("%.14g",valor);
         // buggy sprintf printing 00.000xyz or -00.000xyz like values cause errors (turn 00.000 into 0.000)
         int p = SVal.Pos("00.");
         if ( p==1 || p==2 && SVal[1]=='-' )
           SVal = SVal.Delete(p, 1);
         }
       else
         SVal.sprintf("%.0f",valor);
       StringReplaceCh(SVal, '.', ','); // troca os pontos por vírgula (para o excel)
       }
     else
       SVal.sprintf("%.2f",valor);

     ID = (String)pt.GetNome();
     Subest = pt.Estacao;
     //if ( Subest.Length() == 4 )
     //if ( Subest[4] == '-' )
     //   Subest.SetLength(3);
     pt.GetModDescr( descr );
     String S = descr;
     StringReplaceCh(S, ',', '.'); // tira as virgulas da descricao
     if ( SEP == ',' )
        Linha = Linha + (String)"L[" + (String)i + (String)"]='";
     Linha = Linha +
        (String)lista[i] + (String)SEP +
        ID + (String)SEP +
        Subest + (String)SEP +
     S + (String)SEP;

     if ( pt.EhComando() )
       {
       Linha = Linha+
         (String)pt.GetEstadoOff() + (String)"/" + (String)pt.GetEstadoOn() + (String)SEP;
       }
     else
     if ( pt.GetTipo() == (String)"D" )
       {
       switch ( qual.Byte & ESTDUP_MASCARA ) // Double-point
         {
         case 0:
           Linha = Linha + (String)WEBSERVER_DP_TRANSIT + (String)SEP;
           break;
         case 1:
           if ( pt.EhEventoDigital() && pt.AlarmeNaoReconhecido() )
             {
             Linha = Linha + (String)pt.GetEstadoOn() + (String)SEP;
             }
           else
             {
             Linha = Linha + (String)pt.GetEstadoOff() + (String)SEP;
             }
           break;
         case 2:
           Linha = Linha + (String)pt.GetEstadoOn() + (String)SEP;
           break;
         case 3:
           Linha = Linha + (String)WEBSERVER_DP_INVALID + (String)SEP;
           break;
         }
       }
     else
       Linha = Linha + SVal + (String)SEP;

     int persistente = pt.TemAlarmePersistente();
     Qlf = (String)pt.GetPriority();
     if ( congel != 0 )
       Qlf = Qlf + (String)"U";
     if ( qual.Byte & 0x80 )
       Qlf = Qlf + (String)"F";
     if ( qual.Byte & 0x10 )
       Qlf = Qlf + (String)"S";
     if ( (qual.Byte & 0x0C) == 0x04 )
        Qlf = Qlf + (String)"C";
     else
     if ( (qual.Byte & 0x0C) == 0x0C )
       Qlf = Qlf + (String)"M";
     else
     if ( (qual.Byte & 0x0C) == 0x08 )
       Qlf = Qlf + (String)"X";
     if ( alm )
       Qlf = Qlf + (String)"L";
     if ( persistente )
       Qlf = Qlf + (String)"P";
     if ( alrin )
       Qlf = Qlf + (String)"I";
     if ( tem_anot )
       Qlf = Qlf + (String)"A";
     if ( pt.GetSupCmd() != 0 )
       {
       Qlf = Qlf + (String)"K";
       if ( BL.ComandoIntertravado( pt.GetSupCmd() ) )
         Qlf = Qlf + (String)"Y";
       }
     if ( ehcomando )
       Qlf = Qlf + (String)"J";

     if ( tagalarm == 0 )
       S = "";
     else
       {
       TDateTime dt = tagalarm;
       S = dt.FormatString( WEBSERVER_DATE_FMT );
       }
     Linha = Linha +
     (String)( qual.Byte | alm << 8 | tem_anot << 9  | alrin << 10 | persistente << 11 | congel << 12 )+ (String)SEP +
     (String)pt.GetSupCmd() + (String)SEP + Qlf +
     (String)SEP + (String)0 + (String)SEP + S;
     if ( SEP == ',' )
       Linha = Linha + (String)"';";
     Linha = Linha + (String)"\n";

     Lpto = Lpto + Linha;
     }
   } // for

sResp = sResp + Lpto;
if ( lista[i] != 0 && i >= LIMITE_PONTOS_TABULAR )
  {
  sResp = sResp +
  "L[" + i + (String)"]='0,LIMITE_MAX_PONTOS,HÁ PONTOS NÃO MOSTRADOS,0,0,0,,0';\n";
  // i++;
  }

// informa se há beep ativo
if ( BL.HaBeepAtivo() )
 sResp = sResp + "HA_ALARMES=1;\n";
else
 sResp = sResp + "HA_ALARMES=0;\n";

sResp = sResp + (String)"NUM_VAR=" + (String)BL.GetNumVar() + (String)";\n";

paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "B");
// função de callback
if ( paramval != NULL )
 {
 sResp = sResp + (String)"\n" +
         paramval +
         (String)"();\n";
 }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandlePointListRequest(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
 static int old_connection_marker;
 if (NULL == *ptr)
    {
    *ptr = &old_connection_marker;
    return MHD_YES;
    }

/*
if (0 == strcmp (method, "POST"))
{
  struct connection_info_struct *con_info = (connection_info_struct *)*ptr;

  if (*upload_data_size != 0)
    {
      fmMicroHttpd->logln(upload_data);
      MHD_post_process(con_info->postprocessor, upload_data,
                       *upload_data_size);
      *upload_data_size = 0;
      return MHD_YES;
    }
  else
  if (NULL == con_info->answerstring)
    {
      return MHD_NO;
    }
}
*/

String lstpts;
const char * paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "P");
if (paramval == NULL)
 {
 paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "E");
 }
lstpts = paramval;

// recover post upload data
// THIS IS A SIMPLIFIED TREATMENT OF POST ON LIBMICROHTTPD!
// (POST method will not be really used anymore from version 5.6 onwards)
if ( strcmp(method, "POST")==0 && upload_data != NULL )
if ( *upload_data_size > 0 )
  {
  if ( *upload_data_size > 2 )
    {
    char buf[POSTBUFFERSIZE];
    int sz = (*upload_data_size > POSTBUFFERSIZE - 1) ? POSTBUFFERSIZE-1 : *upload_data_size;
    strncpy(buf, (const char *)upload_data, sz);
    buf[sz]=0;
    lstpts = buf + 2; // jump over "E="
    sLastDataUpload = buf + 2;
    fmMicroHttpd->logln(lstpts);
    }
  *upload_data_size = 0;
  return MHD_YES;
  }

if ( sLastDataUpload != "" )
  {
  // fmMicroHttpd->logln((String)"Recover post data" + sLastDataUpload);
  lstpts = sLastDataUpload;
  sLastDataUpload = "";
  }

unsigned int ListaPontos[2000];
// transform point/tag list into array (zero terminated)
int inponto = -1;
int p2 = 0;
String nponto_ou_tag;
bool reqinfo = false;
String sResp;

paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "I");
// point info request
if ( (String)paramval == (String)"1" )
 {
   reqinfo = true;
 }

for ( int i = 0; inponto != 0; )
 {
 lstpts = lstpts.SubString(p2+1,lstpts.Length());
 p2 = lstpts.Pos(",");
 if ( p2== 0 )
   p2 = lstpts.Length() + 1;

 nponto_ou_tag = lstpts.SubString(1, p2-1);
 if ( nponto_ou_tag!="" && isalpha(nponto_ou_tag[1]) ) // if ID, convert to number (point key)
   {
   bool found;
   TPonto &pt = BL.GetRefPontoByTag(nponto_ou_tag, found);
   if (found)
     {
     ListaPontos[i] = pt.NPonto;
     if ( ListaPontos[i] != 0 )
       i++;
     }
   }
 else
 if ( nponto_ou_tag != "0" )
   {
   inponto = nponto_ou_tag.ToIntDef(0);
   ListaPontos[i] = inponto;
   i++;
   }
 }

// insert date/time
TDateTime agora = Now();
sResp = (String)"Data='" + agora.FormatString( WEBSERVER_DATE_FMT ) + (String)"';\n";

double valor;
TFA_Qual qual;
double tagalarm;
bool temponto;
int alm, tem_anot, alrin, congel, intertrav, possuicmd, persistente;
String Anot;
String ResponseDig, ResponseAna;

// for each point in database
for ( int i = 0; ListaPontos[i] != 0; i++ )
 {
 TPonto &pt = BL.GetRefPonto(ListaPontos[i], temponto);

 if ( temponto )
   {
   valor = pt.Valor;
   qual = pt.Qual;
   alm = pt.AlarmeNaoReconhecido();
   Anot = pt.GetAnotacao();
   tem_anot = pt.TemAnotacao();
   tagalarm = pt.GetTimeAlarm();
   persistente = pt.TemAlarmePersistente();

   alrin = pt.AlarmeInibido();
   congel = pt.Congelado();
   possuicmd = ( pt.GetSupCmd() != 0 );
   if ( ( !pt.EhComando() ) && possuicmd )
     intertrav = BL.ComandoIntertravado(pt.GetSupCmd());
   else
     intertrav = 0;

   String S;

   if ( qual.Tipo )
     { // analog
     S.sprintf( "%.14g", valor );
     // buggy sprintf printing 00.000xyz or -00.000xyz like values cause errors (turn 00.xxx into 0.xxx)
     int p = S.Pos("00.");
     if ( p==1 || p==2 && S[1]=='-' )
       S = S.Delete(p, 1);

     if ( reqinfo )
       {
       ResponseAna = ResponseAna + (String)"TAGS[" + ListaPontos[i] + (String)"]='" + (String)pt.GetNome() + "';\n";
       ResponseAna = ResponseAna + (String)"NPTS['" + (String)pt.GetNome() + (String)"']=" + (String)ListaPontos[i] + ";\n";
       ResponseAna = ResponseAna + (String)"BAYS[" + ListaPontos[i] + (String)"]='" + (String)pt.GetModulo() + "';\n";
       ResponseAna = ResponseAna + (String)"SUBS[" + ListaPontos[i] + (String)"]='" + (String)pt.GetEstacao() + "';\n";
       ResponseAna = ResponseAna + (String)"DCRS[" + ListaPontos[i] + (String)"]='" + (String)pt.GetDescricao() + "';\n";
       String aux;
       ResponseAna = ResponseAna + (String)"LIMSUPS[" + ListaPontos[i] + (String)"]='" + aux.sprintf("%f", pt.GetLimSup()) + "';\n";
       ResponseAna = ResponseAna + (String)"LIMINFS[" + ListaPontos[i] + (String)"]='" + aux.sprintf("%f", pt.GetLimInf()) + "';\n";
       ResponseAna = ResponseAna + (String)"STONS[" + ListaPontos[i] + (String)"]='';\n";
       ResponseAna = ResponseAna + (String)"STOFS[" + ListaPontos[i] + (String)"]='';\n";
       }
     ResponseAna = ResponseAna +
       "V[" + ListaPontos[i] + "]=" + S + ";F[" + ListaPontos[i] + "]=" + ( qual.Byte | alm<<8 | tem_anot<<9 | alrin<<10 | persistente << 11 | congel<<12 | intertrav<<13 | possuicmd << 14 ) + ";";
     if ( tagalarm != 0 )
       ResponseAna = ResponseAna + "T[" + ListaPontos[i] + "]='" + FormatDateTime( WEBSERVER_DATE_FMT, (TDateTime)tagalarm ) + "';\n";
     else
       ResponseAna = ResponseAna + "T[" + ListaPontos[i] + "]='';\n";
     }
   else
     { // digital
     S.sprintf( "%1.0f", valor );
     if ( reqinfo )
       {
       ResponseDig = ResponseDig + (String)"TAGS[" + ListaPontos[i] + (String)"]='" + (String)pt.GetNome() + "';\n";
       ResponseDig = ResponseDig + (String)"NPTS['" + (String)pt.GetNome() + (String)"']=" + (String)ListaPontos[i] + ";\n";
       ResponseDig = ResponseDig + (String)"BAYS[" + ListaPontos[i] + (String)"]='" + (String)pt.GetModulo() + "';\n";
       ResponseDig = ResponseDig + (String)"SUBS[" + ListaPontos[i] + (String)"]='" + (String)pt.GetEstacao() + "';\n";
       ResponseDig = ResponseDig + (String)"DCRS[" + ListaPontos[i] + (String)"]='" + (String)pt.GetDescricao() + "';\n";
       ResponseDig = ResponseDig + (String)"LIMSUPS[" + ListaPontos[i] + (String)"]='" + (String)LIMSUPMAX + (String)"';\n";
       ResponseDig = ResponseDig + (String)"LIMINFS[" + ListaPontos[i] + (String)"]='" + (String)LIMINFMIN + (String)"';\n";
       ResponseDig = ResponseDig + (String)"STONS[" + ListaPontos[i] + (String)"]='" + (String)pt.GetEstadoOn() + "';\n";
       ResponseDig = ResponseDig + (String)"STOFS[" + ListaPontos[i] + (String)"]='" + (String)pt.GetEstadoOff() + "';\n";
       }
     ResponseDig = ResponseDig +
       "V[" + ListaPontos[i] + "]=" + S + ";F[" + ListaPontos[i] + "]=" + ( qual.Byte | alm<<8 | tem_anot<<9 | alrin<<10 | persistente << 11 | congel<<12 | intertrav<<13 | possuicmd << 14 ) + ";";
     if ( tagalarm != 0 )
       ResponseDig = ResponseDig + "T[" + ListaPontos[i] + "]='" + FormatDateTime( WEBSERVER_DATE_FMT, (TDateTime)tagalarm ) + "';\n";
     else
       ResponseDig = ResponseDig + "T[" + ListaPontos[i] + "]='';\n";
     }
   }
 }

char Sha1Ana[22];
char Sha1Dig[22];

sha1( ResponseAna.c_str(), ResponseAna.Length(), Sha1Ana );
Sha1Ana[20] = 0;
sha1( ResponseDig.c_str(), ResponseDig.Length(), Sha1Dig );
Sha1Dig[20] = 0;

String SA, SD, SX;
for ( int i = 0; i < 20; i++ )
 {
 SA = SA + SX.sprintf( "%02X",(unsigned char)Sha1Ana[i] );
 SD = SD + SX.sprintf( "%02X",(unsigned char)Sha1Dig[i] );
 }

sResp = sResp + ResponseAna;
sResp = sResp + "Sha1Ana='" + SA + "';\n";

sResp = sResp + ResponseDig;
sResp = sResp + "Sha1Dig='" + SD + "';\n";

if ( BL.HaBeepAtivo() )
 sResp = sResp + "HA_ALARMES=" + (String)BL.HaBeepAtivo() + (String)";\n";
else
 sResp = sResp + "HA_ALARMES=0;\n";

sResp = sResp+ (String)"NUM_VAR=" + (String)BL.GetNumVar() + (String)";\n";

// callback parameter
paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "B");
if ( paramval != NULL )
 {
 sResp = sResp + (String)"\n" +
         paramval+
         (String)"();\n";
 }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JAVASCRIPT);
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_echo(void * cls,
		    struct MHD_Connection * connection,
		    const char * url,
		    const char * method,
                    const char * version,
		    const char * upload_data,
		    size_t * upload_data_size,
                    void ** ptr)
{
  // static int dummy;
  struct MHD_Response * response;
  int ret;
  string sPage;

  //if (&dummy != *ptr)
  //{
    //The first time only the headers are valid,
    //   do not respond in the first round...
    // *ptr = &dummy;
    // return MHD_YES;

//  if (NULL == *ptr)
//    {
//    *ptr = &dummy;
    /*
      struct connection_info_struct *con_info;

      con_info = (connection_info_struct *)malloc (sizeof (struct connection_info_struct));
      if (NULL == con_info)
        return MHD_NO;

      con_info->answerstring = NULL;
      if (0 == strcmp (method, "POST"))
        {
          fmMicroHttpd->logln("Create post processor");
          con_info->postprocessor =
            MHD_create_post_processor(connection, POSTBUFFERSIZE, iterate_post, (void *) con_info);

          if (NULL == con_info->postprocessor)
            {
              free (con_info);
              return MHD_NO;
            }

          con_info->connectiontype = POST;
        }
      else
        con_info->connectiontype = GET;

      *ptr = (void *) con_info;
*/
//      return MHD_YES;
//      }
  //}
  // *ptr = NULL; // clear context pointer

  fmMicroHttpd->logln( (String)method + (String)" " + (String)url );

  string sURL = url;
  std::size_t found = sURL.find("/");
  if ( found == std::string::npos  )
    return MHD_NO;

  found = sURL.find(SHELLAPI_SCRIPT.c_str());
  if ( found != std::string::npos )
    {
     return fmMicroHttpd->ahc_HandleShellAPI(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
    }
  else
    {
    found = sURL.find(WEBSERVER_SCRIPT.c_str());
    if ( found != std::string::npos )
       {
       return fmMicroHttpd->ahc_HandleDataAPI(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
       }
    }

  return MHD_NO;
}

//---------------------------------------------------------------------------
__fastcall TfmMicroHttpd::TfmMicroHttpd(TComponent* Owner)
        : TForm(Owner)
{
  // create libmicrohttpd daemon
  d = MHD_start_daemon((WEBSERVER_HTTPD_MODE == 1)? MHD_USE_SELECT_INTERNALLY : MHD_USE_THREAD_PER_CONNECTION,
		       HTTP_PORTA,
		       &apc_AcceptPolicyCallback,
		       NULL,
		       &ahc_echo,
		       "",
                       // MHD_OPTION_NOTIFY_COMPLETED, request_completed, NULL,
		       MHD_OPTION_END);

  if (d == NULL)
    Application->Terminate();
}

//---------------------------------------------------------------------------
void __fastcall TfmMicroHttpd::FormDestroy(TObject *Sender)
{
  MHD_stop_daemon(d);
}

//---------------------------------------------------------------------------
void __fastcall TfmMicroHttpd::Timer2Timer(TObject *Sender)
{
int nponto;

    if ( IHM_EstaFinalizando() )
      return;

    if ( Timer2->Tag == 1 )
      {
      fmVeDados->Close();
      return;
      }

    // check for point in dump queue
    BL.lstNPontoDump.unique();
    while ( !BL.lstNPontoDump.empty() )
      {
       nponto = BL.lstNPontoDump.front(); // take first
       BL.lstNPontoDump.pop_front(); // remove first from queue
       fmDumpdb->DumpDB( nponto, 1 ); // force dump for that point
      }
}

//---------------------------------------------------------------------------
int TfmMicroHttpd::ahc_HandleShellAPI(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
  if (0 != strcmp(method, "GET"))
    return MHD_NO; // unexpected method
  if (0 != *upload_data_size)
    return MHD_NO; // upload data in a GET!?

  struct MHD_Response * response;
  string sResp = "{\"error\": \"invalid request\"}";
  const MHD_ConnectionInfo* info = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
  const sockaddr* client_addr = (info ? info->client_addr : NULL);
  char * client_ip = inet_ntoa( ((sockaddr_in *)client_addr)->sin_addr );

  // log user operation request
  const char * paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "U");
  if (paramval != NULL)
    {
    string username = paramval;
    paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "O");
    string operation = paramval;
    const char * realip = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, "X-Real-IP");
    Loga( (String)"IP=" + (String)client_ip + (String)", RealIP=" + (String)realip + (String)", user=" + username.c_str() + (String)", oper=" + operation.c_str(),  ARQUIVO_LOGUSER );
    sResp = "{\"error\": \"none\"}";
    }

  // hide/show request
  paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "H");
  if (paramval != NULL)
    { // only accepts external command to hide/show if configured to hide initially
    if ( HIDE )
      {
      string value = paramval;
      if ( value == "2" )
        fmVeDados->Close();
      if ( value == "1" )
        fmVeDados->Hide();
      else
        fmVeDados->Show();
      }
    sResp = "{\"error\": \"none\"}";
    }

  // shutdown request
  paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "Y");
  if (paramval != NULL)
    {
      // when running as a service, ignore this request
      sResp = "{\"error\": \"none\"}";

      TCHAR username[200 + 1];
      DWORD size = 200 + 1;
      GetUserName((TCHAR*)username, &size);
      // user SYSTEM ou SISTEMA, means running as a service
      if ( (String)username != (String)"SYSTEM" && (String)username != (String)"SISTEMA" )
        {
        Timer2->Interval = 2000;
        Timer2->Tag = 1; // forca program close, after timeout wil exec TfmVeDados::FormCloseQuery(TObject *Sender, bool &CanClose)
        }
    }

  // Silence beep request, do not ack alarms
  paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "Z");
  if (paramval != NULL)
    {
      sResp = "{\"error\": \"none\"}";
      BL.SilenciaBeep();
      // if there is another redundant machine forward message
      if ( IHMRED_IP_OUTRO_IHM != "" )
      if ( (String)client_ip != IHMRED_IP_OUTRO_IHM )
          BL.lstHTTPReq_OutroIHM.push_back( (String)url + (String)"?Z=1" );
    }

  paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "E");
  if (paramval != NULL)
    {
      char buf[20];
      itoa(BL.HaBeepAtivo(), buf, 10);
      sResp = (string)"{\"beep\": " + buf + (string)", \"error\": \"none\"}";
    }

  paramval = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "P");
  if (paramval != NULL)
    {
      char buf[20];
      itoa(BL.HaBeepAtivo(), buf, 10);
      sResp = (string)"{\"beep\": " + buf + (string)", \"error\": \"none\"}";
    }

return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JSON);
}

//---------------------------------------------------------------------------
// log a line to a form memo
void TfmMicroHttpd::logln(String msg)
{
if ( cbLog->Checked )
  {
  if (mmLog->Lines->Count>1500)
    for (int i=0; i<50; i++)
    mmLog->Lines->Delete(0);

  TDateTime dt;
  mmLog->Lines->Add( dt.CurrentDateTime().FormatString( "hh:nn:ss " ) + msg );
  }
}

//---------------------------------------------------------------------------
// check access rights
int TfmMicroHttpd::apc_AcceptPolicyCallback(void * cls,
  const struct sockaddr * addr,
  socklen_t addrlen)
{
  char * client_ip = inet_ntoa( ((sockaddr_in *)addr)->sin_addr );

  if ( WEBSERVER_CLIENTES_REMOTOS[1] != '*'
       &&
       WEBSERVER_CLIENTES_REMOTOS.Pos( client_ip ) == 0
     ){
    fmMicroHttpd->logln( (String)"R< Unauthorized request from: " + (String)client_ip );
    return MHD_NO;
  }

  return MHD_YES;
}
//---------------------------------------------------------------------------

int TfmMicroHttpd::ahc_HandleDataAPI(void * cls,
  struct MHD_Connection * connection,
  const char * url,
  const char * method,
  const char * version,
  const char * upload_data,
  size_t * upload_data_size,
  void ** ptr)
{
  if (0 != strcmp(method, "GET") && 0 != strcmp(method, "POST"))
    return MHD_NO; // unexpected method
  if (0 == strcmp(method, "GET") && 0 != *upload_data_size)
    return MHD_NO; // upload data in a GET!?

  const char * paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "E");
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "P");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandlePointListRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "I");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandlePointInfoRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "G");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleFilteredListSubstBayRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "GX");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleFilteredListSubstBayRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "F");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleFilteredListRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "FX");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleFilteredListRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "XX");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleExecExtAppRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
  paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "N");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleHistListRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
   }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "R"); // ack event request
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleAckRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "Q"); // remove event request
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleAckRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "H");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleHideRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "S");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleStationListRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "M");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleStatusRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "Z"); // remove event request
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleSilenceRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "W"); // remove event request
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleWritePointRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "X");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleImposePointRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "K");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleCommandRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "A");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleAckCommandRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
  if ( paramval == NULL )
  {
    paramval = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "D");
    if (paramval != NULL)
      {
      return fmMicroHttpd->ahc_HandleSyncDataRequest(cls, connection, url, method, version, upload_data, upload_data_size, ptr);
      }
  }
string sResp = "{\"error\": \"invalid request\"}";
return mhd_EnqueueResponse(connection, sResp.c_str(), MIME_JSON);
}

//---------------------------------------------------------------------------

void __fastcall TfmMicroHttpd::btGetSyncClick(TObject *Sender)
{
fmWebServ->btSincClick(Sender);
}
//---------------------------------------------------------------------------

