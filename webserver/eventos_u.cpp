//---------------------------------------------------------------------------
// EVENTOS.CPP
// Registro de eventos.
//---------------------------------------------------------------------------
/*
OSHMI - Open Substation HMI
	Copyright 2008-2018 - Ricardo L. Olsen

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

#include <stdio.h>
#include <mem.h>

#include "eventos_u.h"
#include "bcolocal.h"
#include "vedados.h"
#include "config.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmSDE *fmSDE;

list <String> ListaRecSQL;
list <String> ListaInsSQL;

volatile unsigned Entra;
int retReindex = 123;
unsigned cntEveDescart = 0;

//---------------------------------------------------------------------------
__fastcall TfmSDE::TfmSDE(TComponent* Owner)
        : TForm(Owner)
{
}

//---------------------------------------------------------------------------

// inclui um evento na lista de eventos
// deve ser chamada antes de EscrevePonto
int IncluiEvento( unsigned int NPONTO,
                  unsigned int UTR,
                  unsigned int Flags,
                  unsigned int Ano,
                  unsigned int Mes,
                  unsigned int Dia,
                  unsigned int Hora,
                  unsigned int Minuto,
                  unsigned int Segundo,
                  unsigned int Milisegundo
                  )
{
TEvento ev;
bool found;
int Recon;
TDateTime TS;

  TPonto &pt = BL.GetRefPonto(NPONTO, found);
  if ( !found )
    { // discarded event
    cntEveDescart++;
    return 1;
    }

  // se for evento digital e estado for OFF, nem registra
  if ( pt.EhEventoDigital() && ( ( Flags & QUAL_ESTADO_DUPLO ) == ESTDUP_OFF ) )
    {
    cntEveDescart++;
    return 1;
    }

  if ( Mes == SDE_GERAHORA || Mes == SDE_GERAHORAMS ) // pega a hora atual
    {
    TDateTime dt = Now();
    unsigned short year; unsigned short month; unsigned short day;
    unsigned short hour; unsigned short min; unsigned short sec; unsigned short msec;
    dt.DecodeDate( &year, &month, &day );
    dt.DecodeTime( &hour, &min, &sec, &msec );
    Milisegundo = 0;
    if ( Mes == SDE_GERAHORAMS ) // pega o ms
      Milisegundo = ::GetTickCount() % 1000;
    Ano = year;
    Mes = month;
    Dia = day;
    Hora = hour;
    Minuto = min;
    Segundo = sec;
    }

  if ( fmSDE->Visible )
    {  // show last event
    fmSDE->lbErro->Caption = "";
    fmSDE->lbEvento->Caption =
        (String)NPONTO + " " +
        (String)Ano + (String)"/" + Mes + (String)"/" + Dia + (String)" " +
        (String)Hora + (String)":" + Minuto + (String)":" + Segundo + (String)":" + Milisegundo + " " +
        (String)UTR + " " + Flags;
    }

  TDateTime dtev;
  try
    {
    dtev = EncodeDate( Ano, Mes, Dia ) + EncodeTime( Hora, Minuto, Segundo, Milisegundo );
    pt.TagTempoEvento = dtev;
    }
  catch ( Exception &E )
    { // date conversion error: discarded event 
    cntEveDescart++;
    return 1;
    }

    TS = Now();

    if ( NPONTO > NPONTO_SIST_MIN && NPONTO < NPONTO_SIST_MAX ) // se for evento de sistema
      Recon = 2; // já entra reconhecido e eliminado
    else
      Recon = 0; // entra normal

    TFA_Qual faqual;
    faqual.Byte = Flags;

    // se o estado não mudou e não permite alarmar nesta situação (quando não for comando nem analógico)
    if ( ( pt.EhDigital() ) && ( ! ALARMA_DUPLA_TRANSICAO ) )
      {
      // consulta o estado atual para comparar com o novo

      if ( ( pt.CodOrigem != 7 ) &&
           ( pt.Qual.Duplo == faqual.Duplo ) &&
           ( pt.QualEvt.Duplo == faqual.Duplo )
         )
        {
        Recon = 2; // já entra reconhecido e eliminado
        }
      }

    // evento puro com estado que não ON e falha de valor já entra reconhecido
    if ( faqual.Falha && pt.EhEventoDigital() && ( faqual.Duplo != ESTDUP_ON ) )
       {
       Recon = 2; // já entra reconhecido e eliminado
       }

    // if command status update (executed or blocked as a result of an operator action)
    if ( pt.EhComando() )
       {
       Recon = 1; // event auto acknowledged
       }

    // se tem estado de alarme definido e assumiu estado normal, vai entrar reconhecido
    if ( pt.EhDigital() && pt.EstadoAlarme < ESTALM_ALL )
      {
      if ( ( pt.EstadoAlarme == ESTALM_OFF && faqual.Duplo == ESTDUP_ON )
            ||
           ( pt.EstadoAlarme == ESTALM_ON && faqual.Duplo == ESTDUP_OFF )
         )
           { // event auto acknowledged
           Recon = 1;
           }
      }

    // limit violation returning to normal
    if ( pt.EhAnalogico() && UTR == SDE_UTREVANALOG && faqual.RegiaoAlarme == RGLIM_NORMAL )
       { // event and alarm auto acknowledged
       Recon = 1;
       }

    // verify DST present now     
    TIME_ZONE_INFORMATION tzi;
    DWORD wintz = GetTimeZoneInformation( &tzi );
    int isdst = ( wintz == 2 ) ? 1 : 0;

    tm unxtm;
    unxtm.tm_year = Ano - 1900;
    unxtm.tm_mon = Mes - 1;
    unxtm.tm_mday = Dia;
    unxtm.tm_hour = Hora;
    unxtm.tm_min = Minuto;
    unxtm.tm_sec = Segundo;
    unxtm.tm_isdst = isdst;
    time_t unxdatetime = mktime( &unxtm );
    time_t unxtime = Segundo + Minuto * 60 + Hora * 60 * 60;

    unsigned short year;
    unsigned short month;
    unsigned short day;
    unsigned short hour;
    unsigned short min;
    unsigned short sec;
    unsigned short msec;

    TS.DecodeDate( &year, &month, &day );
    TS.DecodeTime( &hour, &min, &sec, &msec );
    unxtm.tm_year = year - 1900;
    unxtm.tm_mon = month - 1;
    unxtm.tm_mday = day;
    unxtm.tm_hour = hour;
    unxtm.tm_min = min;
    unxtm.tm_sec = sec;
    unxtm.tm_isdst = isdst;
    time_t unxts = mktime( &unxtm );

    String S;
    S = S.sprintf( "%d,%d,%d,%d,%d,%d,%d,%d",
                    NPONTO,
                    unxdatetime,
                    unxtime,
                    Milisegundo,
                    UTR,
                    Flags,
                    Recon,
                    unxts );

    ListaInsSQL.push_back( S );

return 0;
}
//---------------------------------------------------------------------------

int ReconheceEvento( unsigned int NPONTO,
                     unsigned int Ano,
                     unsigned int Mes,
                     unsigned int Dia,
                     unsigned int Hora,
                     unsigned int Minuto,
                     unsigned int Segundo,
                     unsigned int Milisegundo,
                     unsigned int RecApaga // 0= reconhece, 1=apaga
                  )
{
while ( Entra > 0 )
  SleepEx( 50, true ); // se está ocupado, dá um tempo

Entra = 2;
String S;

if ( RecApaga >= 2 )
  RecApaga = 1;

// se nponto, ano, mes, dia, hora, minuto, segundo e milisegundo=0
// reconhece tudo
if ( NPONTO==0 && ( Ano==0 || Ano==2000 ) && Mes==0 && Hora==0 && Minuto==0 && Segundo==0 && Milisegundo==0 )
  {
  S = "update soe set recon=" + (String)(1+RecApaga) + (String)" where recon<2";
  fmSDE->lbRec->Caption=(String)"REC_ALL todos pontos ";
  }
    // se ano, mes, dia, hora, minuto, segundo e milisegundo=0
else// reconhece tudo do ponto
if (  ( Ano==0 || Ano==2000 ) && Mes==0 && Hora==0 && Minuto==0 && Segundo==0 && Milisegundo==0 )
  {
  S = "update soe set recon="+(String)(1+RecApaga)+(String)" where nponto='" + (String)NPONTO + (String)"' and recon<2";
  fmSDE->lbRec->Caption=(String)"REC_ALL ponto " + (String)NPONTO;
  }
else
  {
  // procura evento
  if ( Ano < 100 ) // caso ano for com 2 dígitos
    Ano += 2000;

  // verifica se tem horário de verão agora
  //TIME_ZONE_INFORMATION tzi;
  //DWORD wintz = GetTimeZoneInformation( &tzi );
  //int isdst = ( wintz == 2 ) ? 1 : 0;

  tm unxtm;
  unxtm.tm_year = Ano - 1900;
  unxtm.tm_mon = Mes - 1;
  unxtm.tm_mday = Dia;
  unxtm.tm_hour = Hora;
  unxtm.tm_min = Minuto;
  unxtm.tm_sec = Segundo;
  unxtm.tm_isdst = 1; // isdst;
  time_t unxdatetime = mktime( &unxtm );

  unxtm.tm_year = Ano - 1900;
  unxtm.tm_mon = Mes - 1;
  unxtm.tm_mday = Dia;
  unxtm.tm_hour = Hora;
  unxtm.tm_min = Minuto;
  unxtm.tm_sec = Segundo;
  unxtm.tm_isdst = 0;
  time_t unxdatetime2 = mktime( &unxtm );

  S = "update soe set recon=" +
      (String)(1+RecApaga) +
      (String)" where nponto='" + (String)NPONTO +
      "' and recon<2 " +
      (String)"and ( data='" + (String)unxdatetime +
                  "' or data='" + (String)unxdatetime2 +
         (String)"') ";
  S = S + (String)" and msec=" + (String)Milisegundo;
  fmSDE->lbRec->Caption=(String)"REC ponto " + (String)NPONTO;
  }

ListaRecSQL.push_back( S );

Entra=0;
return 0;
}
//---------------------------------------------------------------------------

void TfmSDE::ProcessaFila()
{
int cnt = 0;
DWORD ini = GetTickCount();

  if ( ListaInsSQL.size() > 0 )
    {
    FILE * fp;
    String fname;
    fname = fname.sprintf( "..\\db\\soe_i%u.sql", GetTickCount() );
    fp = fopen( fname.c_str(), "at" );

    FILE * fppg = NULL;
    if (DB_POSTGRESQL)
      {
      fname = fname.sprintf( "..\\db\\pg_soe_i%u.sql", GetTickCount() );
      fppg = fopen( fname.c_str(), "at" );
      }

    if ( fp != NULL )
      {
      String SQL, PGSQL;
      SQL = SQL + (String)"BEGIN DEFERRED TRANSACTION;\n";
      SQL = SQL + (String)"insert or ignore into soe (nponto, data, hora, msec, utr, estado, recon, ts) values ";
      PGSQL = PGSQL + (String)"START TRANSACTION;\n";
      PGSQL = PGSQL + (String)"insert into soe (nponto, data, hora, msec, utr, estado, recon, ts) values ";
      String EvSQL;

      while ( ! ListaInsSQL.empty() ) // enquanto houver algo
        {
        EvSQL = ListaInsSQL.front(); // pega a primeira da fila
        ListaInsSQL.pop_front(); // retira-a da fila

        // vou usar insert or replace para fazer valer o último estado quando repetir a hora
        // também é útil para não dar erro
        SQL = SQL + (String)"(" + EvSQL + (String)"),";
        PGSQL = PGSQL + (String)"(" + EvSQL + (String)"),";
        if ( cnt++ > 200 ) //  evita trancar aqui por muito tempo e limite de insert composto em SQLITE é 500
          break;
        }

      SQL[SQL.Length()] = ';'; // troca a última vírgula por ponto e vírgula
      SQL = SQL + (String) "\n";
      SQL = SQL + (String) "COMMIT;\n";
      fputs( SQL.c_str(), fp );
      fclose( fp );
      PGSQL[PGSQL.Length()] = ' '; // troca a última vírgula por espaço
      PGSQL = PGSQL + (String) "ON CONFLICT DO NOTHING;\n"; // avoid duplication error to cause failed transaction
      PGSQL = PGSQL + (String) "COMMIT;\n";
      if ( fppg != NULL )
        {
        fputs( PGSQL.c_str(), fppg );
        fclose( fppg );
        }
      }
    }

  if ( ListaRecSQL.size() > 0 )
    {
    FILE * fp;
    String fname;
    fname = fname.sprintf( "..\\db\\soe_r%u.sql", GetTickCount() );
    fp = fopen( fname.c_str(), "at" );

    FILE * fppg = NULL;
    if (DB_POSTGRESQL)
      {
      fname = fname.sprintf( "..\\db\\pg_soe_r%u.sql", GetTickCount() );
      fppg = fopen( fname.c_str(), "at" );
      }

    if ( fp != NULL )
      {
      String SQL = "PRAGMA synchronous = NORMAL;\n", PGSQL;
      SQL = SQL + (String)"PRAGMA journal_mode = WAL;\n";
      SQL = SQL + (String)"BEGIN DEFERRED TRANSACTION;\n";
      PGSQL = PGSQL + (String)"START TRANSACTION;\n";
      String EvSQL;

      while ( ! ListaRecSQL.empty() ) // enquanto houver algo
        {
        EvSQL = ListaRecSQL.front(); // pega a primeira da fila
        ListaRecSQL.pop_front(); // retira-a da fila

        // vou usar insert or replace para fazer valer o último estado quando repetir a hora
        // também é útil para não dar erro
        SQL = SQL + EvSQL + (String)";\n";
        PGSQL = PGSQL + EvSQL + (String)";\n";
        if ( cnt++ > 200 ) //  evita trancar aqui por muito tempo
          break;
        }

      SQL = SQL + (String) "COMMIT;\n";
      fputs( SQL.c_str(), fp );
      fclose( fp );
      PGSQL = PGSQL + (String) "COMMIT;\n";
      if ( fppg != NULL )
        {
        fputs( PGSQL.c_str(), fppg );
        fclose( fppg );
        }
      }
    }

if ( cnt != 0 )
     {
      Label11->Caption = ListaInsSQL.size();
      Label12->Caption = GetTickCount() - ini;
     }
}
//---------------------------------------------------------------------------

void __fastcall TfmSDE::Timer1Timer(TObject *Sender)
{
if (IHM_EstaFinalizando()) return;

TEvento ev;
static int tamfila;
static unsigned int cntTimer1 = 0;

lbDescarte->Caption = cntEveDescart;

cntTimer1++;

if ( Entra > 0 ) return; // se tá ocupado, dá um tempo
Entra = 3;

ProcessaFila();

Timer1->Enabled=false;
if ( tamfila > 10 )
  Timer1->Interval = 200;
else
  Timer1->Interval = 800;

if ( cntTimer1 < 4 )
  Timer1->Interval = 5000;

Timer1->Enabled=true;

  // 1x por dia, às 02:07, apaga eventos antigos para não crescer demais
  {
  unsigned short Hora, Minuto, Seg, Mseg;
  static int Apagou = false;
  Now().DecodeTime( &Hora, &Minuto, &Seg, &Mseg );
  
  // evita apagar mais de uma vez no mesmo minuto
  if ( Hora == 2 && Minuto == 7 && !Apagou )
    {
    btApagaAntigosClick( Sender );
    Apagou = true;
    }
  if ( Hora != 2 || Minuto != 7 ) // libera para a próxima ocorrência
    Apagou = false;
  }

Entra = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfmSDE::btReindexClick(TObject *Sender)
{
    FILE * fp;
    String fname;
    fname = fname.sprintf( "..\\db\\soe_%u.sql", GetTickCount() );
    fp = fopen( fname.c_str(), "at" );
    if ( fp != NULL )
      {
      String SQL = "REINDEX soe;\n";
      fputs( SQL.c_str(), fp );
      fclose( fp );
      }
}
//---------------------------------------------------------------------------

void __fastcall TfmSDE::btPackClick(TObject *Sender)
{
    FILE * fp;
    String fname;
    fname = fname.sprintf( "..\\db\\soe_%u.sql", GetTickCount() );
    fp = fopen( fname.c_str(), "at" );
    if ( fp != NULL )
      {
      String SQL = "VACUUM;\n";
      fputs( SQL.c_str(), fp );
      fclose( fp );
      }
}
//---------------------------------------------------------------------------

void __fastcall TfmSDE::btEmptyClick(TObject *Sender)
{
    FILE * fp;
    String fname;
    fname = fname.sprintf( "..\\db\\soe_%u.sql", GetTickCount() );
    fp = fopen( fname.c_str(), "at" );
    if ( fp != NULL )
      {
      String SQL = "BEGIN IMMEDIATE TRANSACTION;\n";
      SQL = SQL + "delete from soe;\n";
      SQL = SQL + "COMMIT;\n";
      fputs( SQL.c_str(), fp );
      fclose( fp );
      }
}
//---------------------------------------------------------------------------

// apaga registros de mais de n dias
void __fastcall TfmSDE::btApagaAntigosClick(TObject *Sender)
{
    FILE * fp;
    String fname;
    fname = fname.sprintf( "..\\db\\soe_%u.sql", GetTickCount() );
    fp = fopen( fname.c_str(), "at" );
    if ( fp != NULL )
      {
      String SQL = "BEGIN IMMEDIATE TRANSACTION;\n";
      // SQL = SQL + "delete from soe where date(ts, 'unixepoch', 'localtime') < date('now', '-36 days');\n";
      SQL = SQL + (String)"delete from soe where ts < " + (String)( time(NULL) - 60*60*24*HIST_LIFETIME ) + (String)" ;\n";
      SQL = SQL + "COMMIT;\n";
      // Não vou fazer VACUUM/REINDEX pois é muito lento e os espaços vazios tendem a ser ocupados pelos novos dados
      // a tendência é que o tamanho da tabela se mantenha constante após atingir os 36 dias
      fputs( SQL.c_str(), fp );
      fclose( fp );
      }
}
//---------------------------------------------------------------------------



