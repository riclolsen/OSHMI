//---------------------------------------------------------------------------
// DUMPDB.CPP
// Dump em disco da base de dados.
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

#include <time.h>
#include <list>

#include "dumpdb_u.h"
#include "eventos_u.h"
#include "bcolocal.h"
#include "config.h"
#include "vedados.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmDumpdb *fmDumpdb;
int CntDump=0;

list <String> ListaDumpSQL;

#pragma warn -aus

void TfmDumpdb::ProcessaFilaSQL()
{
int cnt = 0;
int error = 0;
DWORD ini = GetTickCount();
static int transerror = 0;

static cntcall = 0;
Label6->Caption = ++cntcall;

if ( ListaDumpSQL.size() > 0 )
  {
    FILE * fp;
    String fname;
    fname = fname.sprintf( "..\\db\\dumpdb_%u.sql", GetTickCount() );
    fp = fopen( fname.c_str(), "at" );

    if ( fp != NULL )
      {
      String SQL;
      SQL = SQL + (String)"BEGIN DEFERRED TRANSACTION;\n";
      SQL = SQL + (String)"insert or replace into dumpdb (nponto, id, descricao, valor, flags, lims, limi, hister, data, hora, ts, alrin, vlrini, histor, bmorta, periodo, tipo, unidade, eston, estoff, supcmd, timeout, anotacao, estacao, estalm, prior ) values ";
      while (!ListaDumpSQL.empty()) // enquanto houver consulta
        {
        cnt++;
        if ( cnt >= 490 ) //  evita trancar aqui por muito tempo e limite de insert composto em SQLITE é 500
          break;
        String S;
        S = ListaDumpSQL.front();   // pega a primeira da fila
        ListaDumpSQL.pop_front();   // retira-a da fila
        SQL = SQL + (String)"(" + S + (String)"),";
        }
        
      SQL[SQL.Length()] = ';'; // troca a última vírgula por ponto e vírgula
      SQL = SQL + (String) "\n";
      SQL = SQL + (String) "COMMIT;\n";
      fwrite( SQL.c_str(), 1, SQL.Length(), fp );
      fclose( fp );
      }
  }

 if ( cnt != 0 )
     {
      Label3->Caption = ListaDumpSQL.size();
      Label4->Caption = GetTickCount() - ini;
     }
}

//---------------------------------------------------------------------------
// Descarrega Banco Local em uma tabela SQLite
// nponto=0 : descarrega toda a base
// nponto!=0 : descarrega parâmetros somente de nponto
void TfmDumpdb::DumpDB(int nponto, int forcagrv)
{
static Acessa=0;

if (Acessa!=0)
  return;

Acessa=1;

map <int, TPonto> &PontosTR=BL.GetMapaPontos();
map <int, TPonto>::iterator it;
map <int, TPonto>::iterator fim;
int ponto;
char buff[500];
char tipo;

// se ainda não fez integridade, não faz dump, a menos seja forcado 
//if ( BL.GetIntegridade() == 0 && forcagrv == 0 )
//  {
//  Acessa=0;
//  return;
//  }

try
  {
  int i = 1;

  if ( nponto == 0 )
    { // vai fazer todos os pontos
    it  = PontosTR.begin();
    fim = PontosTR.end();
    }
  else
    { // vai fazer só o ponto pedido
    it  = PontosTR.find(nponto);
    fim = it;
    fim++;
    }

  for (; it != fim ; it++)
    {
    ponto = (*it).first; // número do ponto
    TPonto &pto = ((*it).second); // dados do ponto

    tipo = BL.GetTipo(ponto)[1];

    fmVeDados->PulseDumpDB( clRed );

    pto.GetModDescr( buff );

    unsigned short year;
    unsigned short month;
    unsigned short day;
    unsigned short hour;
    unsigned short min;
    unsigned short sec;
    unsigned short msec;

    // verifica se tem horário de verão agora
    TIME_ZONE_INFORMATION tzi;
    DWORD wintz = GetTimeZoneInformation( &tzi );
    int isdst = ( wintz == 2 ) ? 1 : 0;

    TDateTime dt = pto.TagTempoAlarme;
    dt.DecodeDate( &year, &month, &day );
    dt.DecodeTime( &hour, &min, &sec, &msec );

    tm unxtm;
    unxtm.tm_year = year - 1900;
    unxtm.tm_mon = month - 1;
    unxtm.tm_mday = day;
    unxtm.tm_hour = hour;
    unxtm.tm_min = min;
    unxtm.tm_sec = sec;
    unxtm.tm_isdst = isdst;
    time_t unxdate = mktime( &unxtm );
    time_t unxtime = sec + min * 60 + hour * 60 * 60;

    String S;

    S = S.sprintf( "%d,'%s','%s',%f,%d,%f,%f,%f,%d,%d,%d,%d,%f,%d,%f,%d,'%c','%s','%s','%s',%d,%d,'%s','%s',%d,%d",
                   ponto,
                   pto.GetNome(),
                   buff,
                   pto.Valor,
                   pto.Qual.Byte,
                   pto.LimSup,
                   pto.LimInf,
                   pto.Hister,
                   unxdate,
                   unxtime,
                   time( NULL ),
                   (int)pto.AlrIn,
                   pto.ValorNormal,
                   0,
                   0.0,
                   0,
                   tipo,
                   pto.GetUnidade(),
                   pto.GetEstadoOn(),
                   pto.GetEstadoOff(),
                   pto.EhComando() ? pto.GetSupCmd() : 0,
                   (int)pto.Timeout,
                   pto.GetAnotacao(),
                   pto.GetEstacao(),
                   pto.EstadoAlarme,
                   pto.Prioridade
                   );

    ListaDumpSQL.push_back( S );

    if ( nponto != 0 )
      break; // nponto!=0, faz só o primeiro
    }

  if ( nponto == 0 )
    {
    CntDump++;
    fmDumpdb->Label1->Caption = CntDump;
    }
   else
    {
    fmDumpdb->Label2->Caption = nponto;
    }

  }
catch ( Exception &E )
  {
  fmDumpdb->lbErro->Caption = E.Message;
  }

Acessa = 0;
}

//---------------------------------------------------------------------------
int TfmDumpdb::RecuperaInfoDumpDB()
{
// varre a tabela, e passa dados ao banco local
TPonto *pto;
sqlite3_stmt    *res;
const char      *tail;
int error;
int dwncnt = 10;
int cntpt = 0;

error = sqlite3_open( BDFile.c_str(), &sqlite_db );
if (error)
   {
   if ( sqlite_db != NULL )
     sqlite3_close( sqlite_db );
   exit(1);
   }
   
do
  {
  sqlite3_busy_timeout( sqlite_db, 100 );

  error = sqlite3_prepare_v2( sqlite_db,
                              "SELECT NPONTO,ID,DESCRICAO,VALOR,FLAGS,LIMI,LIMS,HISTER,DATA,HORA,TS,ALRIN,VLRINI,HISTOR,BMORTA,PERIODO,TIPO,ESTON,ESTOFF,UNIDADE,SUPCMD,TIMEOUT,ANOTACAO FROM DUMPDB ORDER BY NPONTO",
                              -1, &res, &tail );

  } while ( error != SQLITE_OK || dwncnt-- > 0 );

if ( error != SQLITE_OK )
  {
     exit(0);
  }

bool found=false;
while ( sqlite3_step( res ) == SQLITE_ROW )
  {
  TPonto &pto = BL.GetRefPonto( sqlite3_column_int( res, DBCOL_NPONTO ), found );
  if ( ! found )
    continue;
  if ( pto.NPonto >= NPONTO_SIST_MIN && pto.NPonto <= NPONTO_SIST_MAX )
    continue;

  pto.Acessando = true;

  try
    {
    pto.LimInf = sqlite3_column_double( res, DBCOL_LIMI );
    pto.LimSup = sqlite3_column_double( res, DBCOL_LIMS );
    pto.Hister = sqlite3_column_double( res, DBCOL_HISTER );

    time_t idt = (time_t) sqlite3_column_int( res, DBCOL_DATA );
    tm *tmdt = localtime( &idt );
    try
      {
      if ( tmdt == NULL )
        pto.TagTempoAlarme = 0;
      else
        pto.TagTempoAlarme = EncodeDate( tmdt->tm_year + 1900, tmdt->tm_mon + 1, tmdt->tm_mday ) +
                             EncodeTime( tmdt->tm_hour, tmdt->tm_min, tmdt->tm_sec, 0 ) ;
      if ( pto.TagTempoAlarme < 100 ) // puxa data do alarme para zero se for muito antiga
        pto.TagTempoAlarme = 0;
      }
    catch ( Exception &E )
      {
      pto.TagTempoAlarme = 0;
      }
    pto.AlrIn = sqlite3_column_int( res, DBCOL_ALRIN );
    pto.QualAnt.Byte = sqlite3_column_int( res, DBCOL_FLAGS );

    if ( pto.TipoAD == 'D' )
      pto.QualAnt.Tipo = 0;
    else
      pto.QualAnt.Tipo = 1;

    // se é evento
    if ( pto.TipoAD == 'D' && pto.EstadoAlarme == ESTALM_EVT )
      { // evento: parte off
      pto.Valor = FA_ESTFASIM_OFF;
      pto.QualAnt.Duplo = ESTDUP_OFF;
      }
    else
      { // parte com o valor guardado
      pto.Valor = sqlite3_column_double( res, DBCOL_VALOR );
      }

    pto.QualAnt.Falha = 1; // demais partem com falha

    if ( pto.CodOrigem == 6 )
      { // manuais
      // não permite qualif 00=intermediário ou 11=indeterminado para manuais
      if ( pto.QualAnt.Duplo == ESTDUP_INDETERMINADO )
        pto.QualAnt.Duplo = ESTDUP_ON;
      if ( pto.QualAnt.Duplo == ESTDUP_INVALIDO )
        pto.QualAnt.Duplo = ESTDUP_OFF;
      pto.QualAnt.Falha = 0; // sem falha
      pto.QualAnt.Origem = ORIGEM_MANUAL;
      pto.QualAnt.Subst = 0; // não liga o substituido para os manuais
      }

    pto.Qual.Byte = pto.QualAnt.Byte;

    if ( pto.TipoAD == 'A' || pto.EstadoAlarme == ESTALM_ALL )
      pto.ValorNormal = sqlite3_column_double( res, DBCOL_VLRINI );

    char* p = (char*) sqlite3_column_text( res, DBCOL_ANOTACAO );
    int bytes = sqlite3_column_bytes( res, DBCOL_ANOTACAO );
    if ( p != NULL && bytes > 0 )
      {
      pto.SetAnotacao( p );
      }

    cntpt++;
    }
  catch ( Exception &E )
    {
    lbErro->Caption = "Exceção recuperando dump!";
    }

  pto.Acessando = false;
  }

sqlite3_finalize( res );
sqlite3_close( sqlite_db );
fmDumpdb->Label5->Caption = cntpt;

return cntpt;
}

#pragma warn +aus

//---------------------------------------------------------------------------
__fastcall TfmDumpdb::TfmDumpdb(TComponent* Owner)
        : TForm(Owner)
{
BDFile = (String) "..\\db\\dumpdb.sl3";

// busca dados da tabela de dump
int cntpts = RecuperaInfoDumpDB();
// se não tem nada, já dispara um dump para não ficar com a tabela vazia
// (Visor de Eventos não mostra eventos quando tabela de dump está vazia)
if ( cntpts == 0 )
  DumpDB(0,1);
}

//---------------------------------------------------------------------------
void __fastcall TfmDumpdb::btDumpClick(TObject *Sender)
{
DumpDB(0,1);
}
//---------------------------------------------------------------------------

void __fastcall TfmDumpdb::Timer1Timer(TObject *Sender)
{
if (IHM_EstaFinalizando()) return;
DumpDB(0);
}
//---------------------------------------------------------------------------

void __fastcall TfmDumpdb::Timer2Timer(TObject *Sender)
{
ProcessaFilaSQL();
}
//---------------------------------------------------------------------------


