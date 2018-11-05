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
#include <time.h>
#include "config.h"

#include "historico_u.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmHist *fmHist;

//---------------------------------------------------------------------------
__fastcall TfmHist::TfmHist(TComponent* Owner)
        : TForm(Owner)
{
}

void TfmHist::PushVal( int nponto, double valor, int flags, double tagtempo )
{
THistValor hvl;
hvl.nponto = nponto;
hvl.valor = valor;
hvl. flags = flags;
hvl.tagtempo = tagtempo;
ListaValHist.push_back( hvl );
}

//---------------------------------------------------------------------------

void TfmHist::ProcessaFila()
{
static int tickant = GetTickCount();
static int cntFileWrites=0;
int cnt = 0;
DWORD tickini = GetTickCount();
const int LIM_INSERT = 250; // o limite teórico de insert composto é de 500, vou usar menos

static unsigned int cntcall = 0;
Label6->Caption = ++cntcall;
Label3->Caption = ListaValHist.size();
Label4->Caption = tickini - tickant;

  // espera juntar LIM_INSERT ou passar ao menos 1.5s para gravar menos
  if ( ( ListaValHist.size() >= (2 * LIM_INSERT) ) ||
       ( ListaValHist.size() > 0 && ( (tickini - tickant) > 1500 ) )
     )
    {
    cntFileWrites++;
    tickant = tickini;
    FILE * fp;
    String fname;
    fname = fname.sprintf( "..\\db\\hist_%u.sql", GetTickCount() );
    fp = fopen( fname.c_str(), "at" );

    FILE * fppg = NULL;
    if (DB_POSTGRESQL)
      {
      fname = fname.sprintf( "..\\db\\pg_hist_%u.sql", GetTickCount() );
      fppg = fopen( fname.c_str(), "at" );
      }

    FILE * fpmg = NULL;
    if (DB_MONGODB)
      {
      fname = fname.sprintf( "..\\db\\mongo_hist_%u.js", GetTickCount() );
      fpmg = fopen( fname.c_str(), "at" );
      }

    if ( fp != NULL )
      {
      String SQL, PGSQL, MONGODB;
      SQL = (String)"BEGIN DEFERRED TRANSACTION;\n";
      PGSQL = (String)"START TRANSACTION;\n";
      MONGODB = (String)"var bulk=db.realtime_data.initializeUnorderedBulkOp();\n";
      // vou usar insert or replace para fazer valer o último estado quando repetir a hora
      // também é útil para não dar erro
      String S;
      tm unxtm;
      time_t unxts;
      THistValor hvl;
      unsigned short year;
      unsigned short month;
      unsigned short day;
      unsigned short hour;
      unsigned short min;
      unsigned short sec;
      unsigned short msec;
      TDateTime dt;
      bool ins = true;

      // verifica se tem horário de verão agora
      TIME_ZONE_INFORMATION tzi;
      DWORD wintz = GetTimeZoneInformation( &tzi );

      while ( ! ListaValHist.empty() ) // enquanto houver algo
        {
        if ( ins )
           {
           SQL = SQL + (String)"insert or replace into hist (nponto, valor, flags, data) values ";
           PGSQL = PGSQL + (String)"insert into hist (nponto, valor, flags, data) values ";
           ins = false;
           }

        hvl = ListaValHist.front(); // pega a primeira da fila
        ListaValHist.pop_front(); // retira-a da fila

        dt = hvl.tagtempo;
        dt.DecodeDate( &year, &month, &day );
        dt.DecodeTime( &hour, &min, &sec, &msec );

        int isdst = ( wintz == 2 ) ? 1 : 0;
        unxtm.tm_year = year - 1900;
        unxtm.tm_mon = month - 1;
        unxtm.tm_mday = day;
        unxtm.tm_hour = hour;
        unxtm.tm_min = min;
        unxtm.tm_sec = sec;
        unxtm.tm_isdst = isdst; // deixa para o sistema determinar se está ou não em Horário de Verão
        unxts = mktime( &unxtm );

        S = S.sprintf( "(%u,%.14g,%u,%u),", hvl.nponto, hvl.valor, hvl.flags, unxts );
        SQL = SQL + S;
        PGSQL = PGSQL + S;

        if (DB_MONGODB && fpmg != NULL)
          {
          S = S.sprintf( "bulk.find({_id:%u}).upsert().updateOne({$set:{value:%.14g,flags:%u,timestamp:%u}});\n", hvl.nponto, hvl.valor, hvl.flags, unxts );
          // buggy sprintf printing 00.000xyz or -00.000xyz like values cause errors inserting on MongoDB (turn 00.000 into 0.000)
          int p = S.Pos(":00.");
          if ( p )
            {
            S = S.Delete(p+1, 1);
            }
          p = S.Pos(":-00.");
          if ( p )
            {
            S = S.Delete(p+2, 1);
            }

          MONGODB = MONGODB + S;
          }

        if ( ++cnt > ( LIM_INSERT * 2.5 ) ) // evita trancar aqui por muito tempo
          {
          break;
          }

        if ( ! (++cnt % LIM_INSERT) ) // limite de insert composto em SQLITE é 500, vou aplicar menos
          {
          SQL[SQL.Length()] = ';'; // troca a última vírgula por ponto e vírgula
          SQL = SQL + (String) "\n";
          PGSQL[PGSQL.Length()] = ';'; // troca a última vírgula por ponto e vírgula
          PGSQL = PGSQL + (String) "\n";
          ins = true;
          continue;
          }
        }

      SQL[SQL.Length()] = ';'; // troca a última vírgula por ponto e vírgula
      SQL = SQL + (String) "\n";
      SQL = SQL + (String) "COMMIT;\n";
      fputs( SQL.c_str(), fp );
      fclose( fp );
      PGSQL[PGSQL.Length()] = ' '; // troca a última vírgula por espaço
      PGSQL = PGSQL + (String) "ON CONFLICT (nponto, data) DO UPDATE SET valor=EXCLUDED.valor, flags=EXCLUDED.flags;\n"; // UPSERT: force update to latest values
      PGSQL = PGSQL + (String) "COMMIT;\n";
      MONGODB = MONGODB + "bulk.execute();\n";
      if ( fppg != NULL )
        {
        fputs( PGSQL.c_str(), fppg );
        fclose( fppg );
        }
      if ( fpmg != NULL )
        {
        fputs( MONGODB.c_str(), fpmg );
        fclose( fpmg );
        }
      }
    }

    lbWrites->Caption = cntFileWrites;
}

void __fastcall TfmHist::Timer1Timer(TObject *Sender)
{
  // 1x por dia, às 03:07, apaga eventos antigos para não crescer demais
  unsigned short Hora, Minuto, Seg, Mseg;
  static int Apagou = false;
  Now().DecodeTime( &Hora, &Minuto, &Seg, &Mseg );

  // evita apagar mais de uma vez no mesmo minuto
  if ( Hora == 3 && Minuto == 7 && !Apagou )
    {
    btApagaAntigosClick( Sender );
    Apagou = true;
    return;
    }

  if ( Hora != 3 || Minuto != 7 ) // libera para a próxima ocorrência
    Apagou = false;

  ProcessaFila();
}

//---------------------------------------------------------------------------
void __fastcall TfmHist::btApagaAntigosClick(TObject *Sender)
{
    FILE * fp;
    String fname;
    fname = fname.sprintf( "..\\db\\hist_%u.sql", GetTickCount() );
    fp = fopen( fname.c_str(), "at" );
    if ( fp != NULL )
      {
      String SQL = "BEGIN IMMEDIATE TRANSACTION;\n";
      SQL = SQL + (String)"delete from hist where data < " + (String)( time(NULL) - 60*60*24*HIST_LIFETIME ) + (String)" ;\n";
      SQL = SQL + "COMMIT;\n";
      // Não vou fazer VACUUM/REINDEX pois é muito lento e os espaços vazios tendem a ser ocupados pelos novos dados
      // a tendência é que o tamanho da tabela se mantenha constante após atingir os 36 dias
      fputs( SQL.c_str(), fp );
      fclose( fp );
      }

}
//---------------------------------------------------------------------------

