//---------------------------------------------------------------------------
// WEBSERVER_U.CPP
// Implementa servidor WEB para dados de tempo real.
//---------------------------------------------------------------------------
/*
OSHMI - Open Substation HMI
	Copyright 2008-2016 - Ricardo L. Olsen

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

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IdBaseComponent"
#pragma link "IdComponent"
#pragma link "IdCustomHTTPServer"
#pragma link "IdHTTPServer"
#pragma link "IdTCPServer"
#pragma resource "*.dfm"

TfmWebServ *fmWebServ;

int cmdNPonto = 0;
int cmdCntAck = 0;
int confCmdSimul = 1;
String JANELA_AENCONTRAR = "";
HWND JANELA_ENCONTRADA = 0;

list <int> lstNPontoDump;
list <String> lstHTTPReq_OutroIHM; // fila de requisições a mandar para o outro IHM

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
void __fastcall TfmWebServ::IdHTTPServer1CommandGet(TIdPeerThread *AThread,
      TIdHTTPRequestInfo *ARequestInfo, TIdHTTPResponseInfo *AResponseInfo)
{
if (IHM_EstaFinalizando()) return;

bool is_dinpg = false; // página dinâmica não vai para o cache
cntDownNoHTTPReq = NOHTTPREQTIME;

// assegura que IP acessante é o local ou do outro IHM, ou da lista de clientes remotos, para outros não responde
if (  WEBSERVER_CLIENTES_REMOTOS[1] != '*' &&
      WEBSERVER_CLIENTES_REMOTOS.Pos( ARequestInfo->RemoteIP ) == 0
   )
  {
  logaln( (String)"R< Unauthorized request: " + ARequestInfo->RemoteIP );
  return;
  }

char SEP = ',';

try
{
// As requisições devem vir no formato abaixo

// Leitura de valor de pontos: http://localhost/pntserver.rjs?P=nponto1,nponto2,...
// Resposta:
//   Data="dd/mm/aaaa hh:mm:ss"; ou Data='Dados Indisponíveis no momento!';
//   V['nponto1']=valor; F['nponto1']=flags;
//   V['nponto2']=valor; F['nponto2']=flags;
//   ...

// Comando: http://localhost/pntserver.rjs?C=nponto,valor
// Leitura de lista de alarmes/eventos http://localhost/pntserver.rjs?E=1

fmVeDados->PulseWeb( clRed );

logaln( (String)"R< " +
         ARequestInfo->RemoteIP + (String)":" +
         IdHTTPServer1->DefaultPort + (String)"/" +
         ARequestInfo->Document +
         (String)" Query=" + ARequestInfo->QueryParams +
         (String)" Post=" + ARequestInfo->FormParams);

if ( ARequestInfo->Document.Pos( SHELLAPI_SCRIPT ) )
{
is_dinpg = true;
if ( ARequestInfo->UnparsedParams.Length() > 0 )
switch ( ARequestInfo->UnparsedParams[1] )
  {
  case 'U': // Mensagem de usuário remoto Login, Logout, Reboot, Shutdown
     {
     String Username = ARequestInfo->Params->Strings[0].SubString(3,100).c_str();
     String Operation = ARequestInfo->Params->Strings[1].SubString(3,100).c_str();
     Loga( (String)"IP=" + ARequestInfo->RemoteIP + (String)" user=" + Username + (String)", oper=" + Operation,  ARQUIVO_LOGUSER );
     AResponseInfo->ContentText = "Ok\n";
     }
     break;
  case 'H':
    if ( HIDE )
      { // only accepts external command to hide/show if configured to hide initially
      if ( ARequestInfo->Params->Strings[0].SubString(3,1) == "2" )
        fmVeDados->Close();
      if ( ARequestInfo->Params->Strings[0].SubString(3,1) == "1" )
        fmVeDados->Hide();
      else
        fmVeDados->Show();
      }
    AResponseInfo->ContentText = "{'error' : 'none'}";
    break;
  case 'Y': // solicitação de desligamento comportado do computador
     {
     // quando rodando como serviço não sai por esta solicitação

     AResponseInfo->ContentText = "{'error' : 'none'}";

     TCHAR username[200 + 1];
     DWORD size = 200 + 1;
     GetUserName((TCHAR*)username, &size);
     // Usuário SYSTEM ou SISTEMA, significa rodando como serviço, neste caso não fecha
     if ( (String)username != (String)"SYSTEM" && (String)username != (String)"SISTEMA" )
       {
       Timer2->Interval = 2000;
       Timer2->Tag = 1; // força fechamento de programa, após o tempo vai executar o TfmVeDados::FormCloseQuery(TObject *Sender, bool &CanClose), onde são feitos os procedimentos de fechamento do programa
       }
     }
     break;
  case 'Z': // Somente silencia o beep, não reconhece nenhum alarme
     BL.SilenciaBeep();
     // se tem outro ihm e não veio dele vou encaminhar a mensagem
     if ( IHMRED_IP_OUTRO_IHM != "" )
     if ( ARequestInfo->RemoteIP != IHMRED_IP_OUTRO_IHM )
         lstHTTPReq_OutroIHM.push_back( ARequestInfo->Document +"?" + ARequestInfo->UnparsedParams );
     break;
  case 'E': // point list
  case 'P': // point list
     AResponseInfo->ContentText = AResponseInfo->ContentText.sprintf("{'beep': %d}", BL.HaBeepAtivo());
     break;
  }
else
  {
  AResponseInfo->ContentText = "{'error' : 'invalid request'}";
  }
}
else
if ( ARequestInfo->Document.Pos( WEBSERVER_SCRIPT ) )
{
is_dinpg = true;
if ( ARequestInfo->UnparsedParams.Length() > 0 )
switch ( ARequestInfo->UnparsedParams[1] )
  {
  case 'x': // executa aplicativo
     if ( ARequestInfo->Params->Strings[0].SubString(3,1)=="1" )
       { // visor de telas
       String indtela;
       if ( ARequestInfo->Params->Count>1 && ARequestInfo->Params->Strings[1].Length()>7 )
          indtela = "?INDTELA=" + ARequestInfo->Params->Strings[1].SubString(3,1);
       ExecExternApp( (VISOR_TELAS + indtela).c_str() );
       }
     else
     if ( ARequestInfo->Params->Strings[0].SubString(3,1)=="2" )
       { // visor de eventos
       JANELA_AENCONTRAR = "Visor de Eventos - OSHMI";
       EnumWindows( (int (__stdcall *)()) enumwndprc, 0 );
       // deixa só uma janela de eventos aberta
       if ( JANELA_ENCONTRADA == 0 )
         ExecExternApp(VISOR_EVENTOS.c_str());
       }
     else
     if ( ARequestInfo->Params->Strings[0].SubString(3,1) == "3" )
       { // visor tabular
       String t = VISOR_TABULAR;
       if ( ARequestInfo->Params->Count > 1 )
       if ( ARequestInfo->Params->Strings[1].SubString(1,2) == (String)"s=" )
         {
         t = t + (String)"?SUBST=" + ARequestInfo->Params->Strings[1].SubString(3,1000);
         }
       if ( ARequestInfo->Params->Strings[2].SubString(1,2) == (String)"b=" )
         {
         t = t + (String)"?BAY=" + ARequestInfo->Params->Strings[2].SubString(3,1000);
         }
       ExecExternApp( t.c_str() );
       }
     else
     if ( ARequestInfo->Params->Strings[0].SubString(3,1) == "4" )
       { // visor de curvas
       ExecExternApp( VISOR_CURVAS.c_str() );
       }
     else
     if ( ARequestInfo->Params->Strings[0].SubString(3,1) == "5" )
       { // visor de eventos
       ExecExternApp((VISOR_EVENTOS+"?MODO=4").c_str());
       }
     else
     if ( ARequestInfo->Params->Strings[0].SubString(3,1) == "6" )
       { // visor tabular
       String t = VISOR_TABULAR;

       JANELA_AENCONTRAR = "Visor de Alarmes - OSHMI";
       EnumWindows( (int (__stdcall *)()) enumwndprc, 0 );
       if ( JANELA_ENCONTRADA == 0 ) // se não achou executa
         ExecExternApp( (t+"?SELMODULO=TODOS_ANORMAIS").c_str() );
       }
     else
     if ( ARequestInfo->Params->Strings[0].SubString(3,1) == "7" )
       { // visor de telas
       if ( ARequestInfo->Params->Count > 1 && ARequestInfo->Params->Strings[1].Length() > 2 )
          {
          String ponto;
          ponto = "?NPONTO=" + ARequestInfo->Params->Strings[1].SubString(3,8);
          ExecExternApp( (VISOR_TENDENCIAS + ponto).c_str() );
          }
       }
     break;

  case 'D': // busca de valores para sincronizar os IHM's
            // responde com todos os dados cujos parâmetros estejam fora do default
            // Anotação, inibição, limites, histerese, tag de tempo alt valor.
     {
     char buf[2000];
     map <int, TPonto> &PontosTR = BL.GetMapaPontos();
     map <int, TPonto>::iterator it;

     AResponseInfo->ContentText = AResponseInfo->ContentText + "SINCRONISMO\n";

     for ( it = PontosTR.begin(); it != PontosTR.end(); it++ )
         {
         TPonto &pto = ((*it).second);

         if (   pto.NPonto != 0 &&                // Não transfere nponto==0
                !pto.EhComando()           // Não transfere estado de comando
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
           AResponseInfo->ContentText = AResponseInfo->ContentText + buf;
           }
         }
     }
     break;

  case 'N': // lista de valores para histórico
     {
     // vai pegar todos os estados de chave e analógicos
     int cnt = 0;
     char buf[100];
     map <int, TPonto> MpPontos;
     map <int, TPonto>::iterator it;

     MpPontos = BL.GetMapaPontos();

     for ( it = MpPontos.begin(); it != MpPontos.end(); it++ )
         {
         // testa se é estado de seccionadora ou disjuntor ou se é analógico
         if ( ((*it).second).TipoAD == 'A' )
           {
           // nponto flags valor casadecimal
           sprintf( buf,"%.3f",((*it).second).Valor );
           AResponseInfo->ContentText = AResponseInfo->ContentText +
                                      (String) (*it).first + (String)" " +
                                      (String) ((*it).second).Qual.Byte + (String)" " +
                                      (String) buf + (String)" " +
                                      (String) ((*it).second).CasaDecimal + (String)" " +
                                      (String)"\n";
           cnt++;
           }
         else
         // testa se é estado de seccionadora ou disjuntor
         if ( ((*it).second).CodOrigem != 6 && ((*it).second).EhComando() && ((*it).second).CodInfo == 0 && (((*it).second).CodTpEq == 27 || ((*it).second).CodTpEq == 28) )
           {
           // nponto flags
           AResponseInfo->ContentText=AResponseInfo->ContentText +
                                      (String) (*it).first + (String)" " +
                                      (String) ((*it).second).Qual.Byte + (String)" " +
                                      (String)"\n";
           cnt++;
           }

         if ( cnt > 8000 )
            break;
         }
     }
     break;

  case 'Y': // solicitação de desligamento comportado do computador
     {
     // quando rodando como serviço não sai por esta solicitação

     AResponseInfo->ContentText = "";

     TCHAR username[200 + 1];
     DWORD size = 200 + 1;
     GetUserName((TCHAR*)username, &size);
     // Usuário SYSTEM ou SISTEMA, significa rodando como serviço, neste caso não fecha
     if ( (String)username != (String)"SYSTEM" && (String)username != (String)"SISTEMA" )
       {
       Timer2->Interval = 2000;
       Timer2->Tag = 1; // força fechamento de programa, após o tempo vai executar o TfmVeDados::FormCloseQuery(TObject *Sender, bool &CanClose), onde são feitos os procedimentos de fechamento do programa
       }
     }
     break;

  case 'H':
    if ( HIDE )
      { // only accepts external command to hide/show if configured to hide initially
      if ( ARequestInfo->Params->Strings[0].SubString(3,1) == "2" )
        fmVeDados->Close();
      if ( ARequestInfo->Params->Strings[0].SubString(3,1) == "1" )
        fmVeDados->Hide();
      else
        fmVeDados->Show();
      }
    AResponseInfo->ContentText = "";
    break;

  case 'S': // solicitação de lista de estações do banco
     {
     char buf[5000];
     BL.GetListaSEs(buf);
     AResponseInfo->ContentText = (String)"\nLISTA_SES='" + (String)buf + (String)"';";

     if ( ARequestInfo->Params->Count > 1)
     if ( ARequestInfo->Params->Strings[1].SubString(1,2) == (String)"B=" )
         {
         AResponseInfo->ContentText = AResponseInfo->ContentText + (String)"\n" +
                                      ARequestInfo->Params->Strings[1].SubString(3,1000)+
                                      (String)"();\n";
         }
     }
     break;

  case 'g': // solicitação de ponto por filtro de SE/Módulo, separado por tabs
     SEP = '\t';
  case 'G': // solicitação de ponto por filtro de SE/Módulo, código javascript
     { // Parameters in URL: ?G=SUBST&M=BAY&A=0&B=callback_fn
     int lista[LIMITE_PONTOS_TABULAR + 1];
     String Qlf;
     int tipo_alm = 0;

     String FilterSubst = ARequestInfo->Params->Strings[0].SubString(3,100).c_str();
     String FilterMod = ARequestInfo->Params->Strings[1].SubString(3,100).c_str();

     if ( ARequestInfo->Params->Strings[2].SubString(3,2) == (String)"2" )
       {
         tipo_alm = 2; // somente persistentes mais eventos não reconhecidos
       }

     int taml = BL.GetListaPontosFiltSubstMod( FilterSubst.c_str(),
                                               FilterMod.c_str(),
                                               lista,
                                               tipo_alm,
                                               0 );

     // coloca a data/hora da resposta
     AResponseInfo->ContentText = "Data='" + FormatDateTime( WEBSERVER_DATE_FMT, Now() ) + "';\n";
     if ( SEP == ',' )
       AResponseInfo->ContentText = AResponseInfo->ContentText+(String)"L=[];\n";  // inicializa array L no Javascript

     float valor;
     TFA_Qual qual;
     double tagalarm = 0;
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
           SVal.sprintf("%.2f",valor);

           if ( SEP == '\t' )
             StringReplaceCh(SVal, '.', ','); // troca os pontos por vírgula (para o excel)

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

     AResponseInfo->ContentText = AResponseInfo->ContentText + Lpto;
     if ( lista[i] != 0 && i >= LIMITE_PONTOS_TABULAR )
        {
        AResponseInfo->ContentText=AResponseInfo->ContentText +
        "L[" + i + (String)"]='0,LIMITE_MAX_PONTOS,HÁ PONTOS NÃO MOSTRADOS,0,0,0,,0';\n";
        // i++;
        }

     // informa se há beep ativo
     if ( BL.HaBeepAtivo() )
       AResponseInfo->ContentText = AResponseInfo->ContentText + "HA_ALARMES=1;\n";
     else
       AResponseInfo->ContentText = AResponseInfo->ContentText + "HA_ALARMES=0;\n";

     AResponseInfo->ContentText = AResponseInfo->ContentText + (String)"NUM_VAR=" + (String)BL.GetNumVar() + (String)";\n";

     // função de callback
     if ( ARequestInfo->Params->Count > 3)
     if ( ARequestInfo->Params->Strings[3].SubString(1,2) == (String)"B=" )
       {
       AResponseInfo->ContentText = AResponseInfo->ContentText + (String)"\n" +
                                    ARequestInfo->Params->Strings[3].SubString(3,1000)+
                                    (String)"();\n";
       }
     }
     break;

  case 'f': // solicitação de ponto por filtro de SE, separado por tabs
     SEP = '\t';
  case 'F': // solicitação de ponto por filtro de SE, código javascript
     {
     int lista[LIMITE_PONTOS_TABULAR + 1];
     String Qlf;
     int tipo_alm = 0;

     String Filter = ARequestInfo->Params->Strings[0].SubString(3,100).c_str();

     if ( ARequestInfo->Params->Strings[1].SubString(3,1) == (String)"2" )
       {
         tipo_alm = 2; // somente persistentes mais eventos não reconhecidos
       }

     int taml = BL.GetListaPontosFilt( Filter.c_str(),
                                       lista,
                                       tipo_alm,
                                       1 );

     // coloca a data/hora da resposta
     AResponseInfo->ContentText = "Data='" + FormatDateTime( WEBSERVER_DATE_FMT, Now() ) + "';\n";
     if ( SEP == ',' )
       AResponseInfo->ContentText = AResponseInfo->ContentText+(String)"L=[];\n";  // inicializa array L no Javascript

     float valor;
     TFA_Qual qual;
     double tagalarm = 0;
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
           SVal.sprintf("%.2f",valor);

           if ( SEP == '\t' )
             StringReplaceCh(SVal, '.', ','); // troca os pontos por vírgula (para o excel)

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

     AResponseInfo->ContentText = AResponseInfo->ContentText + Lpto;
     if ( lista[i] != 0 && i >= LIMITE_PONTOS_TABULAR )
        {
        AResponseInfo->ContentText=AResponseInfo->ContentText +
        "L[" + i + (String)"]='0,LIMITE_MAX_PONTOS,HÁ PONTOS NÃO MOSTRADOS,0,0,0,,0';\n";
        // i++;
        }

     // informa se há beep ativo
     if ( BL.HaBeepAtivo() )
       AResponseInfo->ContentText = AResponseInfo->ContentText + "HA_ALARMES=1;\n";
     else
       AResponseInfo->ContentText = AResponseInfo->ContentText + "HA_ALARMES=0;\n";

     AResponseInfo->ContentText = AResponseInfo->ContentText + (String)"NUM_VAR=" + (String)BL.GetNumVar() + (String)";\n";

     // função de callback
     if ( ARequestInfo->Params->Count > 3)
     if ( ARequestInfo->Params->Strings[3].SubString(1,2) == (String)"B=" )
       {
       AResponseInfo->ContentText = AResponseInfo->ContentText + (String)"\n" +
                                    ARequestInfo->Params->Strings[3].SubString(3,1000)+
                                    (String)"();\n";
       }
     }
     break;

  case 'E': // extended list of points (E=) can come as a post | lista de valores (com POST)
            // this is to allow for a bigger list of points as the url size is limited in most browsers
  case 'P': // point list with http GET | lista de valores (com GET)
     {
     unsigned int ListaPontos[2000];
     // transforma a lista de pontos (string) em vetor de pontos (terminado em zero)
     int inponto = -1;
     if ( ARequestInfo->UnparsedParams[2] == '=' )
       {
       int p2 = 2;
       String nponto_ou_tag;
       String lstpts = ARequestInfo->Params->Strings[0];
       bool reqinfo = false;

       // extended list of points (E=) can come as a post
       if ( ARequestInfo->FormParams.SubString(1,2) == "E=" )
         lstpts = lstpts + ARequestInfo->FormParams.SubString(3,10000000);

       // requisição de informações sobre os pontos 3o parâmetro I=1
       if ( ARequestInfo->Params->Count > 2)
       if ( ARequestInfo->Params->Strings[2].SubString(1,2) == (String)"I=" )
       if ( ARequestInfo->Params->Strings[2].SubString(3,1) == (String)"1" )
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
         if ( nponto_ou_tag!="" && isalpha(nponto_ou_tag[1]) ) // se é identificador (começa por caractere) traduz para nponto
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

       // coloca a data/hora da resposta
       TDateTime agora = Now();
       AResponseInfo->ContentText = (String)"Data='" + agora.FormatString( WEBSERVER_DATE_FMT ) + (String)"';\n";

       float valor;
       TFA_Qual qual;
       double tagalarm = 0;
       bool temponto;
       int alm, tem_anot, alrin, congel, intertrav, possuicmd, persistente;
       String Anot;
       String ResponseDig, ResponseAna;

       // le os pontos do banco local
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
           S.sprintf( "%.2f", valor );

           if ( qual.Tipo )
             {
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
             {
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

       AResponseInfo->ContentText = AResponseInfo->ContentText + ResponseAna;
       AResponseInfo->ContentText = AResponseInfo->ContentText + "Sha1Ana='" + SA + "';\n";

       AResponseInfo->ContentText = AResponseInfo->ContentText + ResponseDig;
       AResponseInfo->ContentText = AResponseInfo->ContentText + "Sha1Dig='" + SD + "';\n";

       if ( BL.HaBeepAtivo() )
         AResponseInfo->ContentText = AResponseInfo->ContentText + "HA_ALARMES=" + (String)BL.HaBeepAtivo() + (String)";\n";
       else
         AResponseInfo->ContentText = AResponseInfo->ContentText + "HA_ALARMES=0;\n";

       AResponseInfo->ContentText = AResponseInfo->ContentText + (String)"NUM_VAR=" + (String)BL.GetNumVar() + (String)";\n";

       // função de callback
       if ( ARequestInfo->Params->Count > 1)
       if ( ARequestInfo->Params->Strings[1].SubString(1,2) == (String)"B=" )
         {
         AResponseInfo->ContentText = AResponseInfo->ContentText + (String)"\n" +
                                      ARequestInfo->Params->Strings[1].SubString(3,1000)+
                                      (String)"();\n";
         }
       break;
       }

     AResponseInfo->ContentText = "alert('Invalid request!');";
     }
     break;

  case 'M': // status do sistema
     {
     if ( BL.HaBeepAtivo() )
       AResponseInfo->ContentText = AResponseInfo->ContentText + "HA_ALARMES=1;\n";
     else
       AResponseInfo->ContentText = AResponseInfo->ContentText + "HA_ALARMES=0;\n";

     AResponseInfo->ContentText = AResponseInfo->ContentText + (String)"NUM_VAR=" + (String)BL.GetNumVar() + (String)";\n";

     // se houver callback especificada, insere a mesma.
     if ( ARequestInfo->Params->Count > 0 )
     if ( ARequestInfo->Params->Strings[1].SubString(1,2) == (String)"B=" )
       {
       AResponseInfo->ContentText = AResponseInfo->ContentText + (String)"\n" +
                                    ARequestInfo->Params->Strings[1].SubString(3,1000)+
                                    (String)"();\n";
       }
     }
     break;

  case 'R': // reconhece evento
     {
     BL.ReconheceAlarmePonto( ARequestInfo->Params->Strings[0].SubString(3,6).ToInt() );

     String sdata = ARequestInfo->Params->Strings[1].SubString(3,12);
     int dd, mm, yy, p1, p2;

     // convert according to date/time format
     // need to be updated to support more date/time formats
     if ( WEBSERVER_DATE_FMT == "dd/mm/yy hh:nn:ss" || WEBSERVER_DATE_FMT == "dd/mm/yyyy hh:nn:ss" )
       {
       dd = sdata.SubString( 1, ( p1 = sdata.Pos("/") ) - 1 ).ToInt();
       sdata[p1] = ' ';
       mm = sdata.SubString( p1 + 1, ( p2 = sdata.Pos("/")) - p1 - 1 ).ToInt();
       sdata[p2] = ' ';
       yy = sdata.SubString( p2 + 1, 5 ).ToInt();
       }
     else // if ( WEBSERVER_DATE_FMT == "yy/mm/dd hh:nn:ss" ||  WEBSERVER_DATE_FMT == "yyyy/mm/dd hh:nn:ss" )
     if ( WEBSERVER_DATE_FMT == "mm/dd/yy hh:nn:ss" ||  WEBSERVER_DATE_FMT == "mm/dd/yyyy hh:nn:ss" )
       {
       mm = sdata.SubString( 1, ( p1 = sdata.Pos("/") ) - 1 ).ToInt();
       sdata[p1] = ' ';
       dd = sdata.SubString( p1 + 1, ( p2 = sdata.Pos("/")) - p1 - 1 ).ToInt();
       sdata[p2] = ' ';
       yy = sdata.SubString( p2 + 1, 5 ).ToInt();
       }
     else // must be: if ( WEBSERVER_DATE_FMT == "yy/mm/dd hh:nn:ss" ||  WEBSERVER_DATE_FMT == "yyyy/mm/dd hh:nn:ss" )
       {
       yy = sdata.SubString( 1, ( p1 = sdata.Pos("/") ) - 1 ).ToInt();
       sdata[p1] = ' ';
       mm = sdata.SubString( p1 + 1, ( p2 = sdata.Pos("/")) - p1 - 1 ).ToInt();
       sdata[p2] = ' ';
       dd = sdata.SubString( p2 + 1, 2 ).ToInt();
       }

     if ( yy < 100 )
       yy = yy + 2000;  

     // logaln( (String)"dd=" + (String)dd + (String)"mm=" + (String)mm + (String)"yy=" + (String)yy );

     // quando agrega reconhece todos alarmes do ponto, senão só o especificado
     if ( ARequestInfo->Params->Strings[4].SubString(3,1).ToInt() == 1 ) // agrega?
       ReconheceEvento( ARequestInfo->Params->Strings[0].SubString(3,6).ToInt(), // NPONTO
                        0,                                                       // ANO
                        0,                                                       // MES
                        0,                                                       // DIA
                        0, // HORA
                        0, // MIN
                        0, // SEG
                        0, // MSEG
                        0
                        );
     else
       ReconheceEvento( ARequestInfo->Params->Strings[0].SubString(3,6).ToInt(), // NPONTO
                        yy,                                                      // ANO
                        mm,                                                      // MES
                        dd,                                                      // DIA
                        ARequestInfo->Params->Strings[2].SubString(3,2).ToInt(), // HORA
                        ARequestInfo->Params->Strings[2].SubString(6,2).ToInt(), // MIN
                        ARequestInfo->Params->Strings[2].SubString(9,2).ToInt(), // SEG
                        ARequestInfo->Params->Strings[3].SubString(3,3).ToInt(), // MSEG
                        0
                        );

     BL.SilenciaBeep();

     // se tem outro ihm e não veio dele vou encaminhar a mensagem
     if ( IHMRED_IP_OUTRO_IHM != "" )
     if ( ARequestInfo->RemoteIP != IHMRED_IP_OUTRO_IHM )
       lstHTTPReq_OutroIHM.push_back( ARequestInfo->Document + "?" + ARequestInfo->UnparsedParams );
     }
     break;

  case 'Q': // quita (apaga) evento da lista
     {
     BL.ReconheceAlarmePonto( ARequestInfo->Params->Strings[0].SubString(3,6).ToInt() );

     String sdata = ARequestInfo->Params->Strings[1].SubString(3,12);
     int dd, mm, yy, p1, p2;

     // convert according to date/time format
     // need to be updated to support more date/time formats
     if ( WEBSERVER_DATE_FMT == "dd/mm/yy hh:nn:ss" || WEBSERVER_DATE_FMT == "dd/mm/yyyy hh:nn:ss" )
       {
       dd = sdata.SubString( 1, ( p1 = sdata.Pos("/") ) - 1 ).ToInt();
       sdata[p1] = ' ';
       mm = sdata.SubString( p1 + 1, ( p2 = sdata.Pos("/")) - p1 - 1 ).ToInt();
       sdata[p2] = ' ';
       yy = sdata.SubString( p2 + 1, 5 ).ToInt();
       }
     else // if ( WEBSERVER_DATE_FMT == "yy/mm/dd hh:nn:ss" ||  WEBSERVER_DATE_FMT == "yyyy/mm/dd hh:nn:ss" )
     if ( WEBSERVER_DATE_FMT == "mm/dd/yy hh:nn:ss" ||  WEBSERVER_DATE_FMT == "mm/dd/yyyy hh:nn:ss" )
       {
       mm = sdata.SubString( 1, ( p1 = sdata.Pos("/") ) - 1 ).ToInt();
       sdata[p1] = ' ';
       dd = sdata.SubString( p1 + 1, ( p2 = sdata.Pos("/")) - p1 - 1 ).ToInt();
       sdata[p2] = ' ';
       yy = sdata.SubString( p2 + 1, 5 ).ToInt();
       }
     else // must be: if ( WEBSERVER_DATE_FMT == "yy/mm/dd hh:nn:ss" ||  WEBSERVER_DATE_FMT == "yyyy/mm/dd hh:nn:ss" )
       {
       yy = sdata.SubString( 1, ( p1 = sdata.Pos("/") ) - 1 ).ToInt();
       sdata[p1] = ' ';
       mm = sdata.SubString( p1 + 1, ( p2 = sdata.Pos("/")) - p1 - 1 ).ToInt();
       sdata[p2] = ' ';
       dd = sdata.SubString( p2 + 1, 2 ).ToInt();
       }

     if ( yy < 100 )
       yy = yy + 2000;  

     // quando agrega reconhece tudos alarmes do ponto, senão só o especificado
     if ( ARequestInfo->Params->Strings[4].SubString(3,1).ToInt() == 1 ) // agrega?
       ReconheceEvento( ARequestInfo->Params->Strings[0].SubString(3,6).ToInt(), // NPONTO
                        0,                                                       // ANO
                        0,                                                       // MES
                        0,                                                       // DIA
                        0, // HORA
                        0, // MIN
                        0, // SEG
                        0, // MSEG
                        1
                        );
     else
       ReconheceEvento( ARequestInfo->Params->Strings[0].SubString(3,6).ToInt(), // NPONTO
                        yy,                                                      // ANO
                        mm,                                                      // MES
                        dd,                                                      // DIA
                        ARequestInfo->Params->Strings[2].SubString(3,2).ToInt(), // HORA
                        ARequestInfo->Params->Strings[2].SubString(6,2).ToInt(), // MIN
                        ARequestInfo->Params->Strings[2].SubString(9,2).ToInt(), // SEG
                        ARequestInfo->Params->Strings[3].SubString(3,3).ToInt(), // MSEG
                        1
                        );

     BL.SilenciaBeep();

     // se tem outro ihm e não veio dele vou encaminhar a mensagem
     if ( IHMRED_IP_OUTRO_IHM != "" )
     if ( ARequestInfo->RemoteIP != IHMRED_IP_OUTRO_IHM )
       lstHTTPReq_OutroIHM.push_back( ARequestInfo->Document + "?" + ARequestInfo->UnparsedParams );
     }
     break;

  case 'Z': // Somente silencia o beep, não reconhece nenhum alarme
     BL.SilenciaBeep();
     // se tem outro ihm e não veio dele vou encaminhar a mensagem
     if ( IHMRED_IP_OUTRO_IHM != "" )
     if ( ARequestInfo->RemoteIP != IHMRED_IP_OUTRO_IHM )
         lstHTTPReq_OutroIHM.push_back( ARequestInfo->Document +"?" + ARequestInfo->UnparsedParams );
     break;

  case 'W': // escreve valores de parâmetros no banco
      {
      int nponto = ARequestInfo->Params->Strings[0].SubString(3,6).ToInt(); // nponto

      bool found;
      TPonto &pt = BL.GetRefPonto( nponto, found );

      if ( found )
        {
        String S; int P;
        float fval;

        try {
          // troca o ponto pela vírgula para converter
          S = ARequestInfo->Params->Strings[1].SubString(4,20);
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
          } catch ( Exception &E ) { logaln( "E: 3-" + E.Message );  }

        try {
          S = ARequestInfo->Params->Strings[2].SubString(4,20);
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
          } catch ( Exception &E ) { logaln( "E: 4-" + E.Message ); }

        try {
          S = ARequestInfo->Params->Strings[3].SubString(4,20);
          if ( (P=S.Pos(",")) !=0 ) S[P]='.';
          if ( S.Trim() != "" )
            {
            fval = fabs(atof(S.c_str()));
            if ( (fval < HUGE_VAL) && (fval > HUGE_VAL) )
              pt.SetHister( fval );
            }
          } catch ( Exception &E ) { logaln( "E: 5-" + E.Message ); }

        try {
            bool alrin = (bool)(ARequestInfo->Params->Strings[4].SubString(4,1).ToInt());

            // register log for alarm inhibited (when changed)
            if ( pt.GetAlrIn() != alrin )
              Loga( (String)"IP=" + ARequestInfo->RemoteIP +
                    (String)", user=" + (String)BL.getUserName() +
                     (String)", point=" + (String)nponto +
                    (String)", id=" + (String)pt.GetNome() +
                    (String)", alm_inhibit=" + (String)((int)alrin),
                    ARQUIVO_LOGALMINHIB );

            pt.SetAlrIn( alrin );
        } catch ( Exception &E ) { logaln( "E: 6-" + E.Message ); }

        try {
          String anot = ARequestInfo->Params->Strings[5].SubString(4,255).Trim();

          // se entrou ou saiu anotação, bloqueia ou libera comando
          if ( ((int)( pt.TemAnotacao() )) ^ ((int)( anot != "" )) )
          if ( ARequestInfo->RemoteIP != IHMRED_IP_OUTRO_IHM ) // se não vem do outro IHM, é porque vem deste, então pode mandar o bloqueio
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
            Loga( (String)"IP=" + ARequestInfo->RemoteIP +
                  (String)", user=" + (String)BL.getUserName() +
                  (String)", point=" + (String)nponto +
                  (String)", id=" + (String)pt.GetNome(),
                  ARQUIVO_LOGANOT );
            Loga( (String)"TEXT=" + (String)anot, ARQUIVO_LOGANOT );
            }
          pt.SetAnotacao( anot.c_str() );

          } catch ( Exception &E ) { logaln( "E: 7-" + E.Message ); }

        //try {
        //  pt.SetValorNormal( ARequestInfo->Params->Strings[6].SubString(4,1).ToInt()?1:0 );
        //  } catch ( Exception &E ) { logaln( "E: 8-" + E.Message ); }

        lstNPontoDump.push_back( nponto ); // vou colocar numa lista para descarregar depois

        // se tem outro ihm e não veio dele vou encaminhar a mensagem
        if ( IHMRED_IP_OUTRO_IHM != "" )
        if ( ARequestInfo->RemoteIP != IHMRED_IP_OUTRO_IHM )
            lstHTTPReq_OutroIHM.push_back( ARequestInfo->Document + "?" + ARequestInfo->UnparsedParams );
        }
      }
      // break;
  case 'I': // informações sobre o ponto
     AResponseInfo->ContentText = BL.ConsultaInfoPonto( ARequestInfo->Params->Strings[0].SubString(3,6).ToInt() ); // NPONTO
     if ( ARequestInfo->Params->Count > 1 )
     if ( ARequestInfo->Params->Strings[1].SubString(1,2) == (String)"B=" )
         {
         AResponseInfo->ContentText = AResponseInfo->ContentText + (String)"\n" +
                                      ARequestInfo->Params->Strings[1].SubString(3,1000)+
                                      (String)"();\n";
         }

     // reconhece o alarme do ponto se for desejado    
     if ( ARequestInfo->Params->Count > 2)
     if ( ARequestInfo->Params->Strings[2].SubString(1,2) == (String)"R=" )
         {
         BL.ReconheceAlarmePonto(ARequestInfo->Params->Strings[0].SubString(3,6).ToInt());
         ReconheceEvento( ARequestInfo->Params->Strings[0].SubString(3,6).ToInt(), // NPONTO
                          0, // ANO
                          0, // MES
                          0, // DIA
                          0, // HORA
                          0, // MIN
                          0, // SEG
                          0, // MSEG
                          0
                          );
         BL.SilenciaBeep();
         }

     break;

  case 'X': // escreve valor do ponto (impõe)
      {
      int nponto = ARequestInfo->Params->Strings[0].SubString(3,6).ToInt(); // nponto
      String V, F;

      try {
        float valor;
        unsigned char flg;
        String tipopt = BL.GetTipo(nponto);

        // troca o ponto pela vírgula para converter
        V = ARequestInfo->Params->Strings[1].SubString(3,10);
        if ( ARequestInfo->Params->Count > 2 )
        if ( ARequestInfo->Params->Strings[2].SubString(1,1) == "F" )
          F = ARequestInfo->Params->Strings[2].SubString(3,5);
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
            ms.PONTO[0].VALOR = valor;
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
        } catch ( Exception &E ) { logaln( "E: 10-" + E.Message ); }

      // se tem outro ihm e não veio dele vou encaminhar a mensagem
      if ( IHMRED_IP_OUTRO_IHM != "" )
      if ( ARequestInfo->RemoteIP != IHMRED_IP_OUTRO_IHM )
         lstHTTPReq_OutroIHM.push_back( ARequestInfo->Document + "?" + ARequestInfo->UnparsedParams );
      AResponseInfo->ContentText = BL.ConsultaInfoPonto( nponto ); // NPONTO
      }

     break;

  case 'K': // comando
     {
     int cnponto, val = 0, tipo, ret = 0;
     float fval = 0;
     cnponto = ARequestInfo->Params->Strings[0].SubString(3,6).ToInt();
     // tipo = ARequestInfo->Params->Strings[2].SubString(3,5).ToInt();

     bool found;
     TPonto &pt = BL.GetRefPonto( cnponto, found );
     if ( pt.EhComandoDigital() )
       val = ARequestInfo->Params->Strings[1].SubString(3,5).ToInt();
     else
       fval = atof(ARequestInfo->Params->Strings[1].SubString(3,20).c_str());

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
           if ( IP_BDTR1 == "" ) // se não tem bdtr, manda pelo 104
             {
             // manda o comando para o protocolo (varredor iec104m)
             if ( pt.EhComandoDigital() )
               {
               ret = fmIEC104M->ComandoIEC_Dig( cnponto, val&0x01?0:1 );
               Loga( (String)"Command Sent(IEC): IP=" + ARequestInfo->RemoteIP + (String)" user=" + (String)BL.getUserName() + (String)", point=" + (String)cnponto + (String)", val=" + (String)val + (String)", id=" + (String)pt.GetNome(), ARQUIVO_LOGCMD );
               }
             else
               {
               ret = fmIEC104M->ComandoIEC_Ana( cnponto, fval );
               BL.IncNumVar(); // reporta variação para que a aplicação atualize o status mais rapidamente
               Loga( (String)"Command Sent(IEC): IP=" + ARequestInfo->RemoteIP + (String)" user=" + (String)BL.getUserName() + (String)", point=" + (String)cnponto + (String)", val=" + (String)fval + (String)", id=" + (String)pt.GetNome(), ARQUIVO_LOGCMD );
               }
             }
           else
             {
             // Testa se o comando é analógico ou digital
             if ( pt.EhComandoDigital() )
               {
               ret = fmBDTR->bdtr.MandaComandoDig( cnponto, val );
               Loga( (String)"Command Sent(BDTR): IP=" + ARequestInfo->RemoteIP + (String)" user=" + (String)BL.getUserName() + (String)", point=" + (String)cnponto + (String)", val=" + (String)val + (String)", id=" + (String)pt.GetNome(), ARQUIVO_LOGCMD );
               }
             else
               {
               ret = fmBDTR->bdtr.MandaComandoAna( cnponto, fval );
               BL.IncNumVar(); // reporta variação para que a aplicação atualize o status mais rapidamente
               Loga( (String)"Command Sent(BDTR): IP=" + ARequestInfo->RemoteIP + (String)" user=" + (String)BL.getUserName() + (String)", point=" + (String)cnponto + (String)", val=" + (String)fval + (String)", id=" + (String)pt.GetNome(), ARQUIVO_LOGCMD );
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
       AResponseInfo->ContentText = "ComandoEnviado='" +
                                   (String)cnponto +
                                   "," +
                                   (String)val +
                                   "';";
     }
     break;

  case 'A': // Ack do comando
     {
     int cnponto, falha, cmd, cnt;
     double hora;

     cnponto = ARequestInfo->Params->Strings[0].SubString(3,6).ToInt();

     cnt = BL.GetAckCmd( cnponto, &falha, &cmd, &hora );

     if ( cnt == -1 )
       {
       AResponseInfo->ContentText = "ComandoAck='Comando não cadastrado (IHM):" + (String)cnponto + "';";
       break;
       }

     if ( BL.GetSimulacao() == SIMULMOD_LOCAL )
       {
       if ( confCmdSimul )
         AResponseInfo->ContentText = "ComandoAck='Comando recusado (BDTR):" + (String)cnponto + "," + (String)cmd + "';";
       else
         AResponseInfo->ContentText = "ComandoAck='Comando aceito (BDTR):" + (String)cnponto + "," + (String)cmd + "';";
       break;
       }

     if ( cnponto == cmdNPonto && cnt == cmdCntAck )
       {
       AResponseInfo->ContentText = "ComandoAck='Comando não confirmado:" + (String)cnponto + "';";
       break;
       }

     if ( cnponto == cmdNPonto && cnt != cmdCntAck )
       {
       if ( falha )
         AResponseInfo->ContentText = "ComandoAck='Comando recusado (BDTR):" + (String)cnponto + "," + (String)cmd + "';";
       else
         AResponseInfo->ContentText = "ComandoAck='Comando aceito (BDTR):" +(String)cnponto + "," + (String)cmd + "';";
       break;
       }

     AResponseInfo->ContentText = "ComandoAck='';";
     }
     break;

  default:
     AResponseInfo->ContentText = "alert('Invalid request');";
     break;
  }
else
  AResponseInfo->ContentText = "alert('Invalid request');";
} // if (ARequestInfo->Document=="/pntserver.rjs")
else
  {
  int iFileHandle;
  int iFileLength;
  int iBytesRead;
  char *pszBuffer;

  is_dinpg = false;

/* document serving disabled: must use NGINX
    try
      {
      AResponseInfo->ContentStream = new TFileStream((String)"." + ARequestInfo->Document, fmOpenRead );
      }
    catch ( Exception &E )
      {
      AResponseInfo->ContentText = "Documento inexistente: " + (String)"." + ARequestInfo->Document;
      }
*/

  AResponseInfo->ContentText = "Can't serve document: " + (String)"." + ARequestInfo->Document;
  }

// Monta e escreve o HTTP Header
if ( !AResponseInfo->HeaderHasBeenWritten )
  {
/*
  if ( ARequestInfo->Document.Pos(".html") || ARequestInfo->Document.Pos(".htm") )
    AResponseInfo->ContentType = "text/html";
  else
  if ( ARequestInfo->Document.Pos(".css") )
    AResponseInfo->ContentType = "text/css";
  else
  if ( ARequestInfo->Document.Pos(".png") )
    AResponseInfo->ContentType = "image/png";
  else
  if ( ARequestInfo->Document.Pos(".jpg") || ARequestInfo->Document.Pos(".jpeg") )
    AResponseInfo->ContentType = "image/jpeg";
  else
  if ( ARequestInfo->Document.Pos(".pdf") )
    AResponseInfo->ContentType = "application/pdf";
  else
  if ( ARequestInfo->Document.Pos(".gif") )
    AResponseInfo->ContentType = "image/gif";
  else
  if (ARequestInfo->Document.Pos(".svg"))
    AResponseInfo->ContentType = "image/svg+xml;charset=UTF-8";
  else
*/
  if (ARequestInfo->Document.Pos(".js") || is_dinpg)
    AResponseInfo->ContentType = "application/x-javascript; charset=ISO-8859-1";
  else
    AResponseInfo->ContentType = "text/plain";

  AResponseInfo->Date = Now();
//  TDateTime lm;
//  if ( is_dinpg )
//    lm = AResponseInfo->Date;
//  else
//    lm = FileDateToDateTime ( FileAge((String)"." + ARequestInfo->Document) );

//  if ( WEBSERVER_NOCACHE || is_dinpg || ( (double)AResponseInfo->Date-(double)lm < 1.0) ) // objeto dinâmico ou recente : evita o cache
//    {

    AResponseInfo->Expires = AResponseInfo->Date;
    AResponseInfo->LastModified = AResponseInfo->Date;
    AResponseInfo->CacheControl = "no-store, no-cache, must-revalidate, post-check=0, pre-check=0";

//    }
//  else
//    { // objeto normal: força o cache
//    AResponseInfo->Expires = AResponseInfo->Date + 10;
//    AResponseInfo->LastModified = lm;
//    AResponseInfo->CacheControl = "public, max-age=86400, must-revalidate";
//    }


  // marca o tamanho no header
  if ( AResponseInfo->ContentText.Length() > 0 )
    AResponseInfo->ContentLength = AResponseInfo->ContentText.Length();

  if ( AResponseInfo->ContentStream )
    if ( AResponseInfo->ContentStream->Size !=0 )
      AResponseInfo->ContentLength = AResponseInfo->ContentStream->Size;

  AResponseInfo->WriteHeader();
  }

}
catch ( Exception &E )
  {
  logaln( "E: 12- " + ARequestInfo->Document + (String)" - " + E.Message );
  }
}

//---------------------------------------------------------------------------
// descarrega parâmetros escritos de ponto no dump a cada x segundos
void __fastcall TfmWebServ::Timer2Timer(TObject *Sender)
{
int nponto;
static int cntHTTPServerReset = 0;

    if ( IHM_EstaFinalizando() )
      return;

    if ( Timer2->Tag == 1 )
      {
      fmVeDados->Close();
      return;
      }

    // esvazia fila de pontos para dump (por escrita de parametros)
    lstNPontoDump.unique();
    while ( !lstNPontoDump.empty() ) // enquanto houver consulta
      {
        nponto = lstNPontoDump.front();   // pega o primeiro da fila
        lstNPontoDump.pop_front();   // retira da fila
        fmDumpdb->DumpDB( nponto, 1 ); // faz dump  do ponto forçando mesmo que ainda não recebeu integridade
      }

    // monitor time without http requests to the server, after some timeout resets the server
    // also resets the server if it is not active  
    cntDownNoHTTPReq--;
    if ( cntDownNoHTTPReq <= 0 || !IdHTTPServer1->Active )
      {
      cntDownNoHTTPReq = NOHTTPREQTIME;
      lbReset->Caption = ++cntHTTPServerReset;
      try
        {
        IdHTTPServer1->Active = false;
        Sleep(100UL);
        IdHTTPServer1->Active = true;
        }
      catch (Exception &E)
        {
        // Error, wait more 10s
        cntDownNoHTTPReq = 1;
        }
      }

    // logaln( (String)"I: HTTPServerActive=" + (String)(int)IdHTTPServer1->Active );
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::NMHTTP1Success(CmdType Cmd)
{
  logaln( "R< GET OK" );

  if ( NMHTTP1->RemoteIP != IHMRED_IP_OUTRO_IHM ) // só aceita resposta do outro IHM
    return;

  if ( Cmd != CmdGET )
    return;
  char * s = NMHTTP1->Body.c_str();

  int nponto, alarme, alrin, ret, cnt = 0;
  float liminf;
  float limsup;
  float hister;
  double tagalarm;
  // float vlnor;
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
  ret = sscanf( s, "%d %d %d %f %f %f %lf", &nponto, &alarme, &alrin, &liminf, &limsup, &hister, &tagalarm );

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
      if ( S != "" )
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
       // NMHTTP1->Host = IHMRED_IP_OUTRO_IHM;
       // NMHTTP1->Port = HTTP_PORTA;
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
logaln( (String)"I: NMHTTP Status - " + Status );
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::FormCreate(TObject *Sender)
{
  try {
      if ( IHMRED_IP_OUTRO_IHM != "" )
        {
        // NMHTTP1->Host = IHMRED_IP_OUTRO_IHM;
        // NMHTTP1->Port = HTTP_PORTA;
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
void __fastcall TfmWebServ::FormDestroy(TObject *Sender)
{
   IdHTTPServer1->Active = false;
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::Timer3Timer(TObject *Sender)
{
  if ( IHM_EstaFinalizando() ) return;

  if ( IHMRED_IP_OUTRO_IHM != "" )
    {
    if ( !lstHTTPReq_OutroIHM.empty() ) // enquanto houver consulta
        {
         String S;
         S = lstHTTPReq_OutroIHM.front();   // pega o primeiro da fila
         lstHTTPReq_OutroIHM.pop_front();   // retira da fila
         try {
             NMHTTP1->Host = IHMRED_IP_OUTRO_IHM;
             NMHTTP1->Port = HTTP_PORTA;
             NMHTTP1->TimeOut = 200;
             String Rq = (String)"http://" +
                         (String)IHMRED_IP_OUTRO_IHM + (String)":" +
                         (String)HTTP_PORTA + (String)"/" +
                         S;
             logaln( (String)"R> Req.HMI " + Rq );
             NMHTTP1->Get( Rq );
             } catch ( Exception &E ) { logaln ( "E: 0-" + E.Message ); }

        Timer3->Interval = 300;
        }
   else
      Timer3->Interval = 2000;
   }
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::btResetServerClick(TObject *Sender)
{
IdHTTPServer1->Active = false;
IdHTTPServer1->Active = true;
}
//---------------------------------------------------------------------------

void __fastcall TfmWebServ::IdHTTPServer1Exception(TIdPeerThread *AThread,
      Exception *AException)
{
logaln( (String)"E: 16-" + AException->Message );
}
//---------------------------------------------------------------------------



