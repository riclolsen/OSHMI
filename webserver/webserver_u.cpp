//---------------------------------------------------------------------------
// WEBSERVER_U.CPP
// Implementa servidor WEB para dados de tempo real.
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
#include <list>
#include <winsock2.h>
#include <ctype.h>
#include "config.h"
#include "webserver_u.h"
#include "bcolocal.h"
#include "vedados.h"
#include "eventos_u.h"
#include "dumpdb_u.h"
#include "sha1.h"
#include "md5.h"
#include "bdtr_com.h"
#include "simul.h"
#include "i104m_u.h"
#include "lua_u.h"
#include "json_u.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IdBaseComponent"
#pragma link "IdComponent"
#pragma link "IdCustomHTTPServer"
#pragma link "IdHTTPServer"
#pragma link "IdTCPServer"
#pragma link "IdHTTP"
#pragma link "IdTCPClient"
#pragma link "IdTCPConnection"
#pragma resource "*.dfm"

String JANELA_AENCONTRAR;
HWND JANELA_ENCONTRADA = 0;

TfmWebServ *fmWebServ;

int cmdNPonto = 0;
int cmdCntAck = 0;
int confCmdSimul = 1;

// order point list by id
int sort_function( const void *a, const void *b )
{
   return strcmp( BL.GetNome(*(int *)a).c_str(), BL.GetNome(*(int *)b).c_str() );
}

// order point list by time
int sort_function_alm_time( const void *a, const void *b )
{
   bool temponto;
   TPonto &p1 = BL.GetRefPonto( *(int *)a, temponto );
   if ( !temponto )
     return 0;
   TPonto &p2 = BL.GetRefPonto( *(int *)b, temponto );
   if ( !temponto )
     return 0;
   return  p1.GetTimeAlarm() < p2.GetTimeAlarm() ? 1 : -1;
}

// substitui caractere cf por caractere cr em s (ansi string)
void StringReplaceCh(String &s, char cf, char cr)
{
int p;
while ( (p=s.Pos(cf)) !=0 )
  s[p] = cr;
}

// procura se há uma janela de eventos aberta
bool CALLBACK enumwndprc(HWND hWnd, LPARAM lParam)
{
char bufstr[1000];

JANELA_ENCONTRADA = 0;

if ( IsWindowVisible(hWnd) )
  {
  GetWindowText( hWnd, bufstr, 999 );
  if ( strstr(bufstr,JANELA_AENCONTRAR.c_str()) != NULL )
    { // achou
    JANELA_ENCONTRADA = hWnd;
    ShowWindow( hWnd, SW_RESTORE );
    SetForegroundWindow( hWnd );
    return 0;
    }
  }

return 1;
}

//---------------------------------------------------------------------------
void TfmWebServ::logaln(String msg)
{
if ( cbLogar->Checked )
  {
  if (mmWeb->Lines->Count>1500)
    for (int i=0; i<50; i++)
      mmWeb->Lines->Delete(0);

  TDateTime dt;
  mmWeb->Lines->Add( dt.CurrentDateTime().FormatString( "hh:nn:ss " ) + msg );
  }
}

//---------------------------------------------------------------------------
__fastcall TfmWebServ::TfmWebServ(TComponent* Owner)
        : TForm(Owner)
{
cntDownNoHTTPReq = NOHTTPREQTIME; 
NMHTTP1->Port = HTTP_PORTA;
lbOutroIHM->Caption = (String)"Other HMI IP = " + IHMRED_IP_OUTRO_IHM;
}

//---------------------------------------------------------------------------
// descarrega parâmetros escritos de ponto no dump a cada x segundos
void __fastcall TfmWebServ::Timer2Timer(TObject *Sender)
{
int nponto;

    if ( IHM_EstaFinalizando() )
      return;

    if ( Timer2->Tag == 1 )
      {
      fmVeDados->Close();
      return;
      }

    // esvazia fila de pontos para dump (por escrita de parametros)
    BL.lstNPontoDump.unique();
    while ( !BL.lstNPontoDump.empty() ) // enquanto houver consulta
      {
        nponto = BL.lstNPontoDump.front();   // pega o primeiro da fila
        BL.lstNPontoDump.pop_front();   // retira da fila
        fmDumpdb->DumpDB( nponto, 1 ); // faz dump  do ponto forçando mesmo que ainda não recebeu integridade
      }
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::NMHTTP1Success(CmdType Cmd)
{
  logaln( "R< NMHTTP1 GET OK" );

  if ( NMHTTP1->RemoteIP != IHMRED_IP_OUTRO_IHM ) // só aceita resposta do outro IHM
    return;

  if ( Cmd != CmdGET )
    return;
  char * s = NMHTTP1->Body.c_str();

  int nponto, alarme, alrin, ret, cnt = 0;
  double liminf;
  double limsup;
  float hister;
  double tagalarm;
  // double vlnor;
  String anotacao;
  char buf[2000];
  s = s - 1;

  do
  {
  nponto = 0;
  s = s + 1;
  if ( strlen(s) < 10 )
    break;

  cnt++;
  if ( cnt == 1 )
    { // a primeira linha deve ser "SINCRONISMO"
    strcpy( buf, "" );
    ret = sscanf( s, "%s", buf );
    if ( ret == 0 || ((String)buf) != (String)"SINCRONISMO" )
       return;
    logaln( "   SYNC" );
    s = strchr( s, '\n' );

    fmVeDados->PulseWeb( clGreen );

    // recebeu dados para sincronismo, vou colocar todos os pontos na configuração default para depois ajustar pelos dados recebidos
    BL.ReconheceAlarmePonto(0);
    map <int, TPonto> &MpPontos = BL.GetMapaPontos();
    map <int, TPonto>::iterator it;
    for ( it = MpPontos.begin(); it != MpPontos.end(); it++ )
         {
         ((*it).second).SetAnotacao("");
         ((*it).second).LimInf = LIMINFMIN;
         ((*it).second).LimSup = LIMSUPMAX;
         ((*it).second).Hister = 0.0;
         //((*it).second).TagTempoAlarme = 0.0;
         //if ( (*it).second.TipoAD == 'A' || (*it).second.EstadoAlarme == ESTALM_ALL )
         //  ((*it).second).ValorNormal = 1.0;
         ((*it).second).AlrIn = 0;
         }

    continue;
    }

  nponto = 0; alarme = 0; alrin = 0; liminf = LIMINFMIN; limsup = LIMSUPMAX;
  hister = 0.0; tagalarm = 0.0; strcpy( buf, "" );
  ret = sscanf( s, "%d %d %d %lf %lf %f %lf", &nponto, &alarme, &alrin, &liminf, &limsup, &hister, &tagalarm );

  if ( nponto == 0 || ret < 7 )
    break;

  try
    {
    bool temponto;
    TPonto &pt = BL.GetRefPonto( nponto, temponto );

    int sz = strchr( s, '\n' ) - strchr( s, '|' ); // fim da linha menos início do texto
    if ( sz > sizeof(buf) )
      sz = sizeof(buf) - 1;
    strncpy( buf, strchr(s, '|') + 1, sz );
    buf[sz] = 0;

    String S = buf;
    S = S.Trim();

    // logaln( "   Sync point " + (String)nponto );
    if ( temponto )
      {
      if ( liminf > LIMINFMIN )
        pt.SetLimInf( liminf );
      if ( limsup < LIMSUPMAX )
        pt.SetLimSup( limsup );
      if ( hister != 0.0 )
        pt.SetHister( hister );
      if ( alrin != 0 )
        pt.SetAlrIn( alrin );
      //if ( S != "" ) // will erase when empy on redundant machine.
      pt.SetAnotacao( S.c_str() );
      if ( alarme != 0 )
        {
        pt.Alarmar();
        pt.SetTimeAlarm( tagalarm );
        }
      }
    }
  catch ( Exception &E )
    {
    logaln( "E: 13-" + E.Message );
    }

  s = strchr( s, '\n' );

  } while ( s != NULL );

fmVeDados->PulseWeb( clGreen );
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::btSincClick(TObject *Sender)
{
if ( IHMRED_IP_OUTRO_IHM != "" )
  {
  // Ao iniciar devo buscar os últimos XXX eventos do IHM redundante
  // Peço os dados de eventos
  // http://127.0.0.1/htdocs/pntserver.rjs?V=5000&H=00:00:00&D=16/09/2008&F=
  // TODO: AINDA NÃO FEITO!!!

  // Devo ainda replicar o estado atual da base de dados do IHM Redudante
  // Para cada ponto:
  // http://127.0.0.1:51908/htdocs/pntserver.rjs?D=1
  try {
       NMHTTP1->TimeOut = 8000;
       String Rq = (String)"http://" +
                   (String)IHMRED_IP_OUTRO_IHM + (String)":" +
                   (String)HTTP_PORTA + (String)"/" +
                   (String)WEBSERVER_SCRIPT + (String)"?D=1";
       logaln( (String)"S> Req.Sync. " +
                Rq
                );
       NMHTTP1->Get( Rq );
      } catch ( Exception &E ) { logaln ( "E: 14-" + E.Message ); }
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::NMHTTP1Status(TComponent *Sender,
      AnsiString Status)
{
logaln( (String)"I: NMHTTP1 Status - " + Status );
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::FormCreate(TObject *Sender)
{
  try {
      if ( IHMRED_IP_OUTRO_IHM != "" )
        {
        NMHTTP1->TimeOut = 1;
        String Rq = (String)"http://" +
                    (String)IHMRED_IP_OUTRO_IHM + (String)":" +
                    (String)HTTP_PORTA + (String)"/" +
                    (String)WEBSERVER_SCRIPT + (String)"?P=99999";
        logaln( (String)"S> Req.Point. " +
                (String)Rq
                );

        NMHTTP1->Get( Rq );
        }
      } catch ( Exception &E ) { logaln ( "E: 15-" + E.Message ); }

   btSinc->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::Timer3Timer(TObject *Sender)
{
  if ( IHM_EstaFinalizando() ) return;

  if ( IHMRED_IP_OUTRO_IHM != "" )
    {
    if ( !BL.lstHTTPReq_OutroIHM_writepoint.empty() ) // se houver consulta
        {
         String S;
         S = BL.lstHTTPReq_OutroIHM_writepoint.front();   // pega o primeiro da fila
         BL.lstHTTPReq_OutroIHM_writepoint.pop_front();   // retira da fila
         try {
             /*
             IdHTTP1->Host = IHMRED_IP_OUTRO_IHM;
             IdHTTP1->Port = HTTP_PORTA;
             IdHTTP1->ReadTimeout = 200;
             String Rq = (String)"http://" +
                         (String)IHMRED_IP_OUTRO_IHM + (String)":" +
                         (String)HTTP_PORTA + (String)"/" +
                         S;
             logaln( (String)"R> Req.HMI WP " + Rq );
             IdHTTP1->Get( Rq );
             */
             NMHTTP2->Host = IHMRED_IP_OUTRO_IHM;
             NMHTTP2->Port = HTTP_PORTA;
             NMHTTP2->TimeOut = 300;
             String Rq = (String)"http://" +
                         (String)IHMRED_IP_OUTRO_IHM + (String)":" +
                         (String)HTTP_PORTA + (String)"/" +
                         S;
             logaln( (String)"R> Req.HMI WP " + Rq );
             NMHTTP2->Get( Rq );
             } catch ( Exception &E ) { logaln ( "E: 0-" + E.Message ); }

        Timer3->Interval = 400;
        }
    else
    if ( !BL.lstHTTPReq_OutroIHM.empty() ) // se houver consulta
        {
         String S;
         S = BL.lstHTTPReq_OutroIHM.front();   // pega o primeiro da fila
         BL.lstHTTPReq_OutroIHM.pop_front();   // retira da fila
         try {
             NMHTTP1->Host = IHMRED_IP_OUTRO_IHM;
             NMHTTP1->Port = HTTP_PORTA;
             NMHTTP1->TimeOut = 300;
             String Rq = (String)"http://" +
                         (String)IHMRED_IP_OUTRO_IHM + (String)":" +
                         (String)HTTP_PORTA + (String)"/" +
                         S;
             logaln( (String)"R> Req.HMI " + Rq );
             NMHTTP1->Get( Rq );
             } catch ( Exception &E ) { logaln ( "E: 0-" + E.Message ); }

        Timer3->Interval = 400;
        }
   else
        {
        Timer3->Interval = 1000;
        }
   }
   else
   {
      Timer3->Enabled = false;
   }
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::IdHTTPServer1Exception(TIdPeerThread *AThread,
      Exception *AException)
{
logaln( (String)"E: 16-" + AException->Message );
}
//---------------------------------------------------------------------------




void __fastcall TfmWebServ::NMHTTP2Success(CmdType Cmd)
{
  logaln( "R< NMHTTP2 GET OK" );
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::NMHTTP2Status(TComponent *Sender,
      AnsiString Status)
{
logaln( (String)"I: NMHTTP1 Status - " + Status );
}
//---------------------------------------------------------------------------

