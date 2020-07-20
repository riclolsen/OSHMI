//---------------------------------------------------------------------------
// BCOLOCAL.CPP
// Classes para manipulação de pontos em tempo real.
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
#include <limits>
#include <inifiles.hpp>
#include <math.h>
#include "bcolocal.h"
#include "vedados.h"
#include "monproc.h"
#include "eventos_u.h"
#include "config.h"
#include "historico_u.h"

// round fp to long integer 
long round_( double x )
{
 if (x >= 0)
    return (long) ( x + 0.5 );
 return (long) ( x - 0.5 );
}

class TMyTimer : public TTimer
{
public:
  TMyTimer();
  unsigned int Cnt;
__published:
  void __fastcall MyOnTimer( TObject *Sender );
};

TMyTimer::TMyTimer(): TTimer(NULL)
{
  Cnt = 0;
  OnTimer = MyOnTimer;
  Interval = 1000;
  Enabled = true;
};

TMyTimer * tmTimeout;

static unsigned CntEventos = 0, CntGrupos = 0;
TBancoLocal BL;

void __fastcall TMyTimer::MyOnTimer(TObject *Sender)
{
if ( IHM_EstaFinalizando() )
  return;
  
if ( ! ( Cnt % 2 ) )
  BL.ProcessaCalculosDigitais();

if ( ! ( Cnt % 3 ) )
  {
  BL.ProcessaCalculosAnalogicos();
  // escreve a hora do sistema em ponto da IHM
  TDateTime dt = Now();
  unsigned short hour; unsigned short min; unsigned short sec; unsigned short msec;
  dt.DecodeTime( &hour, &min, &sec, &msec );
  BL.EscrevePonto( NPONTO_HORA, (double) hour + (double) min / 60.0, 0x20, 0 );
  BL.EscrevePonto( NPONTO_MINUTO, (double) min + (double) sec / 60.0, 0x20, 0 );
  }

// percorre todo o banco e marca falha nos pontos com tag muito antigo
if ( ! ( Cnt % 13 ) )
  BL.ProcessaTimeout();

// em caso de beep ativo, bipa! se for crítico, bipa no dobro da frequência
if ( ! ( Cnt % 2 ) || ( BL.HaBeepAtivo() == BEEP_CRITICO ) )
  if ( BL.HaBeepAtivo() )
     {
     if ( BL.HaBeepAtivo() == BEEP_CRITICO )
       MessageBeep( MB_ICONASTERISK );
     else
       MessageBeep( MB_OK );

     if ( BL.BipaNoSpeaker )
       Beep( BEEP_FRQ, BEEP_DUR ); // bipe do alto falante
     }

Cnt++;
}

TPonto::TPonto()
{
  Acessando = false;
  CntAtu = -1;
  CntAltEst = 0;
  NPonto = 0;
  Qual.Byte = 0x88; // assume no começo falhado e estimado
  QualAnt.Byte = 0x88;
  QualEvt.Byte = 0x88;
  TagTempo = 0;
  TagTempoAltValor = 0;
  TagTempoAlarme = 0;
  TagTempoEvento = 0;
  TickHist = 0;
  BandaMortaHist = 0;
  // timeout de mais 1 minuto (para o caso de falha na difusão) T_INTEGRIDADE
  Timeout = ( T_INTEGRIDADE + 60.0 ) * 1.0 / ( 24.0 * 60.0 * 60.0 );
  TimeoutCongel = ( 60.0 * 15.0 ) * 1.0 / ( 24.0 * 60.0 * 60.0 ); // 15 minutos
  Congelamento = 0;
  Valor = 0;
  ValorNormal = 0;
  ValorHist = 0;
  sValor = 0;
  AlarmeNaoRec = 0;
  EventoAtrasado.TemEventoAtrasado =  false;
  AlarmeTemporizado = 0;
  LimInf = LIMINFMIN;
  LimSup = LIMSUPMAX;
  Hister = 0;
  KConv1 = 1;
  KConv2 = 0;
  Formula = 0;
  for (int i = 0; i < MAX_PARCELAS; i++)
     Parcelas[i] = 0;
  RegiaoAlmLimite = RGLIM_NORMAL;
  AlrIn = false;
  PontoSupCmd = 0;
  PontoCmdSup = 0;
  PontoIntertrav = 0;
  UTR = 0;
  CmdAckCmd = 0;
  CmdAckFalha = 0;
  CmdAckCnt = 0;
  CmdAckTagTempo = 0;
  CmdSBO = 0;
  CmdDuracao = 0;
  CmdASDU = 0;
  CodOrigem = 0;
  CodTpEq = 0;
  CodInfo = 0;
  CasaDecimal = 0;
  Prioridade = 4;
  EstadoAlarme = ESTALM_ALL;
  EventoDigital = false;
  EventoAnalogico = false;
  EnviaComandoPortaModbus = false;
  TipoAD = ' ';
  SetAnotacao("");
  strcpy( Tag, "" );
  strcpy( Unidade, "" );
  strcpy( Estacao, "" );
  strcpy( Modulo, "" );
  strcpy( Descricao, "" );
  strcpy( EstadoOn, "" );
  strcpy( EstadoOff, "" );
};

// returns 0=TRANSIT or analog, 1=OFF, 2=ON, 3=INV
int TPonto::GetDoubleState()
{
if ( EhDigital() )
  {
  return Qual.Byte & ESTDUP_MASCARA;
  }
return 0;
}

void TPonto::Alarmar()
{
 AlarmeNaoRec = 1;
 TagTempoAlarme = (double)Now();
}

void TPonto::AlarmAck()
{
 AlarmeNaoRec = 0;
}

bool TPonto::TemAlarmePersistente()
{
  if ( AlarmeTemporizado > 0 &&
       TagTempoAlarme > 0 &&
       ( (double)Now() - (double)TagTempoAlarme )*24*60*60 <= GetTemporizacaoAlarme()
     )
    return false;

  if ( EhDigital() )
    {
    if ( ( EstadoAlarme == ESTALM_OFF && Qual.Duplo == ESTDUP_OFF )
           ||
         ( EstadoAlarme == ESTALM_ON && Qual.Duplo == ESTDUP_ON  )
        )
      {
      return true;
      }
    }
  else
    {
    if ( RegiaoAlmLimite != RGLIM_NORMAL )
      {
      return true;
      }
    }

  return false;
}

void TPonto::SetTimeAlarm( double timetag )
{
 TagTempoAlarme = timetag;
}

void TPonto::SetLimInf( double val )
{
  LimInf = val;
}

void TPonto::SetLimSup( double val )
{
  LimSup = val;
}

void TPonto::SetHister( float val )
{
  Hister = val;
}

void TPonto::SetValorTipico( double val )
{
  ValorTipico = val;
}

void TPonto::SetTemporizacaoAlarme(unsigned int seconds)
{
AlarmeTemporizado = seconds;
}

// retorna o tipo de beep que o ponto deve acionar
int TPonto::ComoDeveBipar()
{
if ( AlarmeNaoReconhecido() > 0 && // deve estar não reconhecido
     GetPriority() <= LAST_PRIORITY_THAT_BEEPS && // deve ter prioridade que bipa
     TagTempoAlarme > 0 && // tag de tempo não deve estar zerado (ponto inicializado) para bipar
     TemCadastro() && // deve ter cadastro (nome) para bipar
     !(NPonto > NPONTO_SIST_MIN && NPonto < NPONTO_SIST_MAX) && // ponto interno não bipa
     CodOrigem != 6 // manual não bipa
   )
  {
  if ( AlarmeTemporizado == 0 // Se não é temporizado já bipa direto
       ||
       // se for temporizado vai bipar só depois de um tempo
       ( (double)Now() - (double)TagTempoAlarme )*24*60*60 > AlarmeTemporizado
     )
    {
    // se for disjuntor abrindo, sem falha, faz com que bipe numa frequência maior
    if ( CodTpEq == CODTPEQ_DJ &&
         CodInfo == 0 &&
         CodOrigem == 0 &&
         CodOCR == 2 &&
         Qual.Duplo == ESTDUP_OFF &&
         Qual.Subst == false &&
         Qual.Falha == false
       )
      return BEEP_CRITICO;
    else
      return BEEP_NORMAL;
    }
  else
    return BEEP_NENHUM;
  }
else
  return BEEP_NENHUM;
}

void TPonto::SetAlrIn( bool val )
{
  if ( val )
    {
    AlarmAck();
    }
  AlrIn = val;
}

void TPonto::GetModDescr( char * descr )
{
if ( descr != NULL )
  {
  if ( strlen( Modulo ) != 0 )
    sprintf( descr, "%s~%s", Modulo, Descricao );
  else
    sprintf( descr, "%s~%s", Estacao, Descricao );
  }
}

bool TPonto::ValorOk()
{
if ( QualEhFalhado( Qual.Byte ) ||
     QualEhCargaInic( Qual.Byte ) )
  return false;

if ( EhDigital() &&
    ( ( Qual.Duplo ) == ESTDUP_INDETERMINADO ||
      ( Qual.Duplo ) == ESTDUP_INVALIDO
    )
   )
  return false;

return true;
}

int TPonto::GetSupCmd()
{
if ( EhComando() )
  return PontoSupCmd;
else
  return PontoCmdSup;
}

void TPonto::SetAnotacao(char * anot)
{
strncpy( Anotacao, anot, sizeof(Anotacao) - 1 );
Anotacao[sizeof(Anotacao) - 1] = 0;
}

bool TPonto::AlarmeNaoReconhecido()
{
if ( AlarmeNaoRec == 0 )
  return 0;

if ( AlarmeTemporizado == 0 ||
    ((double)Now() - (double)TagTempoAlarme )*24*60*60 > AlarmeTemporizado ||
    TagTempoAlarme == 0
   )
  return AlarmeNaoRec;

return 0;
}

bool TPonto::EhComandoDigital()
{
if ( EhComando() &&
     (
     TipoAD == 'D' ||
     CmdASDU==45 ||
     CmdASDU==46 ||
     CmdASDU==47 ||
     CmdASDU==58 ||
     CmdASDU==59 ||
     CmdASDU==60 ||
     // BACALHAU: enquanto temos muitos comandos de tap cadastrados como analógicos, vou forçar que é digital
     (CodOCR==CODOCR_TAP && CodTpEq==CODTPEQ_TAP && CodInfo==0)
     )
   )
   return true;
else
   return false;     
}

String TPonto::GetTipo()
{
if ( CodOrigem == CODORIGEM_COMANDO ) // forca tipo digital para comandos
  return "D";
else
  return TipoAD;
}

TBancoLocal::TBancoLocal()
{
int nf;
char * pch;

// register the system start and (OS) username
#pragma warn -aus
UserName[0]='\0';
DWORD size = sizeof(UserName) - 1;
GetUserName((TCHAR*)UserName, &size);
Loga( (String)"System Started. User=" + (String)getUserName() + (String)" :-)" );

NumVariacoes = 0;
LastBeepPnt = 0;
ModoSimulacao = SIMULMOD_NAO;
BipaNoSpeaker = true;
CntIntegridade = 0;
UDPActive = false;
SilenciaBeep();
tmTimeout = new TMyTimer();

// carrega lista de pontos
FILE * fp;
fp = fopen( ARQUIVO_LIST1, "rt" );
if ( fp == NULL )
  {
  fp = fopen( ARQUIVO_LIST2, "rt" );
  if ( fp == NULL )
    {
    Loga( "Can't open point list, exiting!" );
    exit(1);
    }
  }

char buff[10000];

TIniFile *pIniIHM = new TIniFile(ARQUIVO_INI);

Pontos.clear();

if (fp)
  {
  char tag[100];
  char alarme[100];
  char descricao[100];
  char tipo, c;
  int nponto, ocr, endereco;
  double f, valor_tipico;
  int nponto_sup;
  int origem;
  int utr, asdu;
  int versao = 1;
  int cntline = 0;

  fgets( buff, sizeof(buff)-1, fp ); // pula 1a linha
  cntline++;

  if ( strncmp( buff, "VERSAO", 6 ) == 0 || strncmp( buff, "VERSION", 5 ) == 0 )
    {
    char aux[50];
    sscanf( buff, "%s %d", aux, &versao );
    fgets( buff, sizeof(buff)-1, fp ); // pula 2a linha
    cntline++;
    }

  while ( !feof(fp) )
    {
    String S;
    char * rp;
    float kconv1, kconv2;
    int prior, estalm;
    int tpeq, info;

    rp = fgets(buff, sizeof(buff)-1, fp);
    if ( rp == NULL )
      break;

    int cnterr = 0;
    nf = sscanf(buff, "%d %d %s %c %s %d %d %d %d %c %d %d %f %f %d %d %d %lf", &nponto, &endereco, tag, &tipo, alarme, &ocr, &tpeq, &info, &origem, &c, &utr, &asdu, &kconv1, &kconv2, &nponto_sup, &estalm, &prior, &valor_tipico);
    cntline++;
    if ( nf < 16 )
       {
       if ( cnterr == 0 )
         Loga( (String)"Error in point list! Line with insufficient parameters: " + cntline );
       cnterr++;
       continue;
       }

    if ( endereco == 0 ) // address=0 means point number = address
      endereco = nponto; // endereco==0 significa que é o endereco físico do protocolo é igual ao nponto

    if ( origem == CODORIGEM_COMANDO && endereco==-1 ) // address=-1 means address=0 (for commands)
      endereco = 0;
                                                                                                   
    // descarta ponto de comando sem supervisionado associado
    if ( origem == CODORIGEM_COMANDO && nponto_sup<=0 )
      {
      Loga( (String)"Error in point list! Command with no related supervised point. Point number " + (String)nponto + (String)", Line " + (String)cntline );
      cnterr++;
      continue;
      }

    // testa se alarme (ou comando digital) tem os estados separados pela '/'
    if ( strchr( alarme, '/' ) == NULL &&
         (
         tipo == 'D'
         ||
         // força comando de tap como digital
         ( origem == CODORIGEM_COMANDO && ocr == CODOCR_TAP && tpeq == CODTPEQ_TAP && info == 0 )
         )
       )
      {
      Loga( (String)"Error in point list! Column alarm without the '/' separator. Point number " + (String)nponto + (String)", Line " + (String)cntline );
      cnterr++;
      continue;
      }

    if ( strchr(buff, '\"') == NULL )
      {
      Loga( (String)"Error in point list! Line with insufficient parameters: " + cntline );
      cnterr++;
      continue;
      }

    // testa tipo A ou D
    if ( tipo != 'D' && tipo != 'A' )
      {
      Loga( (String)"Error in point list! Unidentified type. Line " + cntline );
      cnterr++;
      continue;
      }

    // se tem ponto duplicado, loga erro
    if ( PontoExiste( nponto ) && GetNome(nponto) != "" )
      {
      Loga( (String)"Error in point list! Duplicated Point: " + nponto );
      cnterr++;
      continue;
      }

    // para encontrar o ponto pelo endereço físico e na utr
    // quando programado utr = 0, aceitará o ponto vindo de qualquer utr
    endereco = endereco & 0x00FFFFFF; // endereco com 3 bytes
    int findpt;
    // String SIP_BDTR1 = pIniIHM->ReadString("BDTR","IP_BDTR1", "").Trim();
    //if ( SIP_BDTR1 == "" ) // se não tem bdtr, mapeia pelo endereço e utr
    //  {

    findpt = (utr << 24) | endereco;

    //  }
    //else // se o protocolo é bdtr, mapeia somente pelo endereço
    //  {
    //  findpt = endereco;
    //  }

    if ( NPontoPorEndUTR( endereco, utr ) != 0 &&
         origem != CODORIGEM_COMANDO &&  // command can replicate addreses
         asdu != 151 ) // bistring statuses can have the same addresses
      { // já tem endereço cadastrado
      Loga( (String)"Error in point list! Duplicated address: " + endereco );
      cnterr++;
      continue;
      }

    if( NPontoPorEndUTR( endereco, utr ) == 0 ) // map only the first point
      MapNPontoPorEndUTR[ findpt ] = nponto;

    strncpy( descricao, strchr(buff, '\"') + 1, sizeof(descricao) - 1 );
    char * endquoteptr = strchr(descricao, '\"');
    if ( endquoteptr != NULL ) // tira o último "
       *endquoteptr = 0;

    // convert commas to points in state and description fields   
    char * commapos;
    while ( (commapos = strchr(descricao, ',')) != NULL )
       *commapos = '.';
    while ( (commapos = strchr(alarme, ',')) != NULL )
       *commapos = '.';

    if ( tipo == 'D' || origem == CODORIGEM_COMANDO )
      {
      }
    else
      {
      if ( kconv1 == 0 ) // evita erro de kconv1, o normal é ser 1, zero nunca, a não ser para comandos
        kconv1 = 1;
      }

    if ( tipo != 'D' )
      { // analógico
      if ( origem == CODORIGEM_MANUAL )
        {
        EscrevePonto(nponto, 0, QUAL_TIPO, 1); // manuais partem sem falha
        Pontos[nponto].Qual.Origem = ORIGEM_MANUAL;
        Pontos[nponto].Qual.Falha = 0;
        }
      else
        {
        EscrevePonto(nponto, 0, QUAL_FALHA|QUAL_TIPO, 1); // demais, com falha
        }

      Pontos[nponto].EventoDigital = false;

      if ( estalm >= 10 )
        {
        Pontos[nponto].EventoAnalogico = true;
        Pontos[nponto].CasaDecimal = estalm - 10;
        }
      else
        {
        Pontos[nponto].EventoAnalogico = false;
        Pontos[nponto].CasaDecimal = -estalm;
        }

      strncpy( Pontos[nponto].Unidade, Trim(alarme).c_str(), sizeof(Pontos[nponto].Unidade)-1 );
      Pontos[nponto].Unidade[sizeof(Pontos[nponto].Unidade)-1] = '\0';

      // ajusta banda morta de acordo com tipo ou unidades
      Pontos[nponto].BandaMortaHist = 2.0;
      if ( strcmp( Pontos[nponto].Unidade , "kV" ) == 0 ||
           strcmp( Pontos[nponto].Unidade , "Hz" ) == 0 ||
           tpeq == CODTPEQ_KV ||
           tpeq == CODTPEQ_HZ
         )
        Pontos[nponto].BandaMortaHist = 0.2;
      else
      if ( strcmp( Pontos[nponto].Unidade , "MW" ) == 0 ||
           strcmp( Pontos[nponto].Unidade , "Mvar" ) == 0 ||
           strcmp( Pontos[nponto].Unidade , "MVA" ) == 0 ||
           tpeq == CODTPEQ_MW ||
           tpeq == CODTPEQ_MVAR ||
           tpeq == CODTPEQ_MVA
         )
        Pontos[nponto].BandaMortaHist = 2.5;
      else
      if ( strcmp( Pontos[nponto].Unidade , "oC" ) == 0 ||
           tpeq == CODTPEQ_TEMP
         )
        Pontos[nponto].BandaMortaHist = 4.0;
      else
      if ( strcmp( Pontos[nponto].Unidade , "A" ) == 0 ||
           tpeq == CODTPEQ_AMP
         )
        Pontos[nponto].BandaMortaHist = 4.0;
      else
      if ( strcmp( Pontos[nponto].Unidade , "Pos" ) == 0 ||
           tpeq == 16 
         ) // Tap
         Pontos[nponto].BandaMortaHist = 0.0;

      // ajusta pelo fator configurado   
      Pontos[nponto].BandaMortaHist = Pontos[nponto].BandaMortaHist * (float)HIST_DEADBANDFACTOR/100.0;

      // tipos de medidas que não congelam
      if ( tpeq == 16 ) // TAP
         Pontos[nponto].TimeoutCongel = 0;
      }
    else
      { // digital
      if ( estalm == 3 )
        {
        EscrevePonto(nponto, FA_ESTFASIM_OFF, 0x02, 1);
        Pontos[nponto].Qual.Duplo = ESTDUP_OFF;
        Pontos[nponto].Qual.Falha = 1;
        }
      else
      // ponto manual não falha
      if ( origem == CODORIGEM_MANUAL )
        {
        EscrevePonto(nponto, 0, 0x02, 1);  // manuais partem sem falha
        Pontos[nponto].Qual.Origem = ORIGEM_MANUAL;
        Pontos[nponto].Qual.Falha = 0;
        }
      else
        {
        EscrevePonto(nponto, 0, QUAL_FALHA, 1); // demais, com falha
        }

      if ( estalm == 3 ) // critério para ser evento
        {
        Pontos[nponto].EventoDigital = true;
        }
      else
        {
        Pontos[nponto].EventoDigital = false;
        }

      if ( estalm < 2 ) // se tem valor de alarme, o outro estado é o normal, cuidado formato interno é invertido!
        {
        Pontos[nponto].ValorNormal = estalm ? 1 : 0;
        }
      else
        {
        Pontos[nponto].ValorNormal = 1;
        }

      Pontos[nponto].EstadoAlarme = estalm;
      }

    Pontos[nponto].CodOCR = ocr;

    if ( tipo == 'D' ||
    // força comando de tap como digital
         ( origem == CODORIGEM_COMANDO && ocr == CODOCR_TAP && tpeq == CODTPEQ_TAP && info == 0 )
       )
      {
      char * pchar = strchr(alarme,'/');
      if ( pchar != NULL )
        {
        if ( strlen(pchar + 1) < sizeof(Pontos[nponto].EstadoOn) )
          {
          strcpy( Pontos[nponto].EstadoOn, pchar + 1 );
          S = Trim(alarme);
          S = S.SubString(1,S.Pos("/")-1);
          if ( S.Length() < sizeof(Pontos[nponto].EstadoOff) )
            strcpy(Pontos[nponto].EstadoOff,S.c_str());
          else
            Loga( (String)"Point state OFF message too long! Line: " + cntline );
          }
        else
          {
          Loga( (String)"Point state ON message too long! Line: " + cntline );
          }
        }
      else
        {
        Loga( (String)"Missing '/' in point state message! Line: " + cntline );
        }
      }

      // da versão 3 em diante muda a descrição, onde vem os 3 campos SE~VAO~DESCRICAO separados pelo ~
      pch = strchr(descricao, '~');

      if ( pch != NULL )
        {
        strncpy( Pontos[nponto].Estacao, descricao, pch-descricao );
        Pontos[nponto].Estacao[pch-descricao] = 0;

        *pch = '-';
        char * pch2 = strchr(descricao, '~');

        if (pch2 != NULL)
          {
          strncpy( Pontos[nponto].Modulo, pch+1, pch2-pch );
          Pontos[nponto].Modulo[pch2-pch-1] = 0;

          strncpy( Pontos[nponto].Descricao, pch2+1, sizeof(Pontos[nponto].Descricao) );
          Pontos[nponto].Descricao[sizeof(Pontos[nponto].Descricao)-1] = 0;
          }
        }

    if ( tipo == 'D' && tpeq==CODTPEQ_DJ && info==0 && origem!=CODORIGEM_COMANDO && ocr==2 ) // se é estado de disjuntor
      { // relaciona o estado do DJ com o módulo (para uso do simulador)
      DisjModulo[ (String)Pontos[nponto].Estacao + (String)Pontos[nponto].Modulo ] = nponto;
      }

    ListaSEs.insert( Pontos[nponto].Estacao );

    Pontos[nponto].Endereco = endereco;
    Pontos[nponto].ValorTipico = valor_tipico;
    Pontos[nponto].TipoAD = tipo;
    Pontos[nponto].Prioridade = prior;
    Pontos[nponto].KConv1 = kconv1;
    Pontos[nponto].KConv2 = kconv2;
    Pontos[nponto].CodOrigem = origem;
    Pontos[nponto].CodTpEq = tpeq;
    Pontos[nponto].CodInfo = info;
    strncpy( Pontos[nponto].Tag, Trim(tag).c_str(), sizeof(Pontos[nponto].Tag)-1 );
    Pontos[nponto].Tag[sizeof(Pontos[nponto].Tag)-1] = '\0';
    MapNPontoPorTag[Pontos[nponto].Tag] = nponto;

    Pontos[nponto].UTR = utr;

    switch ( c )
      {
      case 'M': // comando deve ser enviado pela porta especial do driver modbus
         Pontos[nponto].EnviaComandoPortaModbus = true;
         break;

         // configura temoporização de alarme para o ponto (default é não ter)
      case 't':
         Pontos[nponto].SetTemporizacaoAlarme(30);
         break;
      case 'T':
         Pontos[nponto].SetTemporizacaoAlarme(60);
         break;
      case 'u':
         Pontos[nponto].SetTemporizacaoAlarme(5*60);
         break;
      case 'U':
         Pontos[nponto].SetTemporizacaoAlarme(10*60);
         break;
      }

    if ( origem == CODORIGEM_COMANDO )
      {
      Pontos[nponto].CmdSBO = kconv1;
      Pontos[nponto].CmdDuracao = kconv2;
      Pontos[nponto].CmdASDU = asdu;

      Pontos[nponto].PontoSupCmd = nponto_sup;
      Pontos[nponto].PontoIntertrav = valor_tipico;

      // cadastra o ponto de supervisão, se não houver
      if ((String)Pontos[nponto_sup].Tag == (String)"")
         {
         EscrevePonto(nponto_sup, 0, QUAL_FALHA, 1);
         Pontos[nponto_sup].CntAtu--;
         }
      Pontos[nponto_sup].PontoCmdSup = nponto;
      }

    Pontos[nponto].NPonto = nponto;
    }
  fclose(fp);
  }

fp = fopen( ARQUIVO_CALC1, "rt" );
if ( fp == NULL )
  {
  fp = fopen( ARQUIVO_CALC2, "rt" );
  }
if ( fp )
  {
  int nponto = 0, npontoant = 0, cntpar = 0, cod_formula = 0, parcela = 0;
  int parcelas[MAX_PARCELAS];
  int cntline = 0;
  int cnterr = 0;

  fgets( buff, sizeof(buff) - 1, fp ); // pula 1a linha
  cntline++;

  while ( !feof(fp) )
    {
    String S;
    int nf;
    char * rp;

    rp = fgets( buff, sizeof(buff)-1, fp );
    if ( rp == NULL )
      break;
    nf = sscanf(buff, "%d %d %d", &nponto, &parcela, &cod_formula);
    cntline++;
    if ( nf < 3 )
       {
       Loga( (String)"Error in calculation! Line with not enough parameters: " + cntline );
       cnterr++;
       continue;
       }

    if ( nponto != npontoant )
      {
      Pontos[nponto].Formula = cod_formula;
      cntpar = 0;
      }
    else
      {
      cntpar++;
      }

    if ( cntpar < MAX_PARCELAS )
      Pontos[nponto].Parcelas[cntpar] = parcela;

    npontoant = nponto;
    }

  delete pIniIHM;
  fclose(fp);
  }

// create some internal status points

EscrevePonto(NPONTO_BANCO_ATIVO, 0, 0x02, 1);
Pontos[NPONTO_BANCO_ATIVO].TipoAD = 'D';
Pontos[NPONTO_BANCO_ATIVO].Endereco = NPONTO_BANCO_ATIVO;
strcpy(Pontos[NPONTO_BANCO_ATIVO].Estacao,"HMIX");
strcpy(Pontos[NPONTO_BANCO_ATIVO].Tag,"HMIX-RTDB_ST");
strcpy(Pontos[NPONTO_BANCO_ATIVO].EstadoOff,"INACTIVE");
strcpy(Pontos[NPONTO_BANCO_ATIVO].EstadoOn,"ACTIVE");
strcpy(Pontos[NPONTO_BANCO_ATIVO].Descricao,"RTDB Status");
ListaSEs.insert("HMIX");

EscrevePonto(NPONTO_OSHMI_OPER, 0, 0x02, 1);
Pontos[NPONTO_OSHMI_OPER].TipoAD = 'D';
Pontos[NPONTO_OSHMI_OPER].Endereco = NPONTO_OSHMI_OPER;
strcpy(Pontos[NPONTO_OSHMI_OPER].Estacao,"HMIX");
strcpy(Pontos[NPONTO_OSHMI_OPER].Tag,"HMIX-WEBSERVER_ST");
strcpy(Pontos[NPONTO_OSHMI_OPER].EstadoOff,"NOT_OPERATIONAL");
strcpy(Pontos[NPONTO_OSHMI_OPER].EstadoOn,"OPERATIONAL");
strcpy(Pontos[NPONTO_OSHMI_OPER].Descricao,"Webserver Status");

EscrevePonto(NPONTO_OSHMI_MODO, 1, 0x01, 1);
Pontos[NPONTO_OSHMI_MODO].TipoAD = 'D';
Pontos[NPONTO_OSHMI_MODO].Endereco = NPONTO_OSHMI_MODO;
strcpy(Pontos[NPONTO_OSHMI_MODO].Estacao,"HMIX");
strcpy(Pontos[NPONTO_OSHMI_MODO].Tag,"HMIX-WEBSERVER_MODE");
strcpy(Pontos[NPONTO_OSHMI_MODO].EstadoOff,"SECONDARY");
strcpy(Pontos[NPONTO_OSHMI_MODO].EstadoOn,"PRIMARY");
strcpy(Pontos[NPONTO_OSHMI_MODO].Descricao,"Webserver mode");

EscrevePonto(NPONTO_ACERTO_HORA, 1, 0x01, 1);
Pontos[NPONTO_ACERTO_HORA].TipoAD = 'D';
Pontos[NPONTO_ACERTO_HORA].Endereco = NPONTO_ACERTO_HORA;
strcpy(Pontos[NPONTO_ACERTO_HORA].Estacao,"HMIX");
strcpy(Pontos[NPONTO_ACERTO_HORA].Tag,"HMIX-TIME_ADJUST");
strcpy(Pontos[NPONTO_ACERTO_HORA].EstadoOff,"TIME_NOT_SYNC");
strcpy(Pontos[NPONTO_ACERTO_HORA].EstadoOn,"TIME_SYNC");
strcpy(Pontos[NPONTO_ACERTO_HORA].Descricao,"Time Adjust");

//EscrevePonto(NPONTO_COMUNIC_BDTR, 1, 0x01, 1);
//Pontos[NPONTO_COMUNIC_BDTR].TipoAD = 'D';
//Pontos[NPONTO_COMUNIC_BDTR].Endereco = NPONTO_COMUNIC_BDTR;
//strcpy(Pontos[NPONTO_COMUNIC_BDTR].Estacao,"HMIX");
//strcpy(Pontos[NPONTO_COMUNIC_BDTR].Tag,"HMIX-BDTR_COMM_ST");
//strcpy(Pontos[NPONTO_COMUNIC_BDTR].EstadoOff,"NO_COMM");
//strcpy(Pontos[NPONTO_COMUNIC_BDTR].EstadoOn,"NORMAL");
//strcpy(Pontos[NPONTO_COMUNIC_BDTR].Descricao,"BDTR comm status");

EscrevePonto(NPONTO_COMUNIC_I104, 1, 0x01, 1);
Pontos[NPONTO_COMUNIC_I104].TipoAD = 'D';
Pontos[NPONTO_COMUNIC_I104].Endereco = NPONTO_COMUNIC_I104;
strcpy(Pontos[NPONTO_COMUNIC_I104].Estacao,"HMIX");
strcpy(Pontos[NPONTO_COMUNIC_I104].Tag,"HMIX-I104_COMM_ST");
strcpy(Pontos[NPONTO_COMUNIC_I104].EstadoOff,"NO_COMM");
strcpy(Pontos[NPONTO_COMUNIC_I104].EstadoOn,"NORMAL");
strcpy(Pontos[NPONTO_COMUNIC_I104].Descricao,"I104 comm Staus");

EscrevePonto(NPONTO_LOGIN_ADM, 1, 0x01, 1);
Pontos[NPONTO_LOGIN_ADM].TipoAD = 'D';
Pontos[NPONTO_LOGIN_ADM].Endereco = NPONTO_LOGIN_ADM;
strcpy(Pontos[NPONTO_LOGIN_ADM].Estacao,"HMIX");
strcpy(Pontos[NPONTO_LOGIN_ADM].Tag,"HMIX-WEBSERVER_ADMLOGIN");
strcpy(Pontos[NPONTO_LOGIN_ADM].EstadoOff,"NOT_LOGGED");
strcpy(Pontos[NPONTO_LOGIN_ADM].EstadoOn,"LOGGED");
strcpy(Pontos[NPONTO_LOGIN_ADM].Descricao,"Webserver Admin Login");

EscrevePonto(NPONTO_FALHAINTERNA, 1, 0x01, 1);
Pontos[NPONTO_FALHAINTERNA].TipoAD = 'D';
Pontos[NPONTO_FALHAINTERNA].Endereco = NPONTO_FALHAINTERNA;
strcpy(Pontos[NPONTO_FALHAINTERNA].Estacao,"HMIX");
strcpy(Pontos[NPONTO_FALHAINTERNA].Tag,"HMIX-INTERNAL_ST");
strcpy(Pontos[NPONTO_FALHAINTERNA].EstadoOff,"NORMAL");
strcpy(Pontos[NPONTO_FALHAINTERNA].EstadoOn,"DEFECT");
strcpy(Pontos[NPONTO_FALHAINTERNA].Descricao,"Internal status");

EscrevePonto(NPONTO_HORA, 0, 0x20, 1);
Pontos[NPONTO_HORA].TipoAD = 'A';
Pontos[NPONTO_HORA].Endereco = NPONTO_HORA;
strcpy(Pontos[NPONTO_HORA].Estacao,"HMIX");
strcpy(Pontos[NPONTO_HORA].Tag,"HMIX-HOUR");
strcpy(Pontos[NPONTO_HORA].Unidade,"h");
strcpy(Pontos[NPONTO_HORA].Descricao,"Hour");

EscrevePonto(NPONTO_MINUTO, 0, 0x20, 1);
Pontos[NPONTO_MINUTO].TipoAD = 'A';
Pontos[NPONTO_MINUTO].Endereco = NPONTO_MINUTO;
strcpy(Pontos[NPONTO_MINUTO].Estacao,"HMIX");
strcpy(Pontos[NPONTO_MINUTO].Tag,"HMIX-MINUTE");
strcpy(Pontos[NPONTO_MINUTO].Unidade,"min");
strcpy(Pontos[NPONTO_MINUTO].Descricao,"Minute");

EscrevePonto(NPONTO_I104UTRS_OK, 0, 0x20, 1);
Pontos[NPONTO_I104UTRS_OK].TipoAD = 'A';
Pontos[NPONTO_I104UTRS_OK].Endereco = NPONTO_I104UTRS_OK;
strcpy(Pontos[NPONTO_I104UTRS_OK].Estacao,"HMIX");
strcpy(Pontos[NPONTO_I104UTRS_OK].Tag,"HMIX-I104_RTU_OK");
strcpy(Pontos[NPONTO_MINUTO].Unidade,"RTUs");
strcpy(Pontos[NPONTO_I104UTRS_OK].Descricao,"Number of RTUs I104");
}

TBancoLocal::~TBancoLocal()
{
Loga( "System Stopped. :'(" );
delete tmTimeout;
}

// retorna nponto pelo endereço e utr, retorna zero se não encontrado
int TBancoLocal::NPontoPorEndUTR( int endereco, int utr )
{
map <int, int>::iterator it;

// tenta encontrar com utr=0 (aceita qualquer utr) ou com a utr passada

it = MapNPontoPorEndUTR.find( endereco );
if ( it == MapNPontoPorEndUTR.end() )
  {
  // não achou
  it = MapNPontoPorEndUTR.find( (utr << 24) | endereco );
  if ( it == MapNPontoPorEndUTR.end() )
    return 0; // também não achou, retorna zero
  }

// achou, retorna nponto
return (*it).second;
}

unsigned int TBancoLocal::GetNumVar()
{
return NumVariacoes;
}

void TBancoLocal::IncNumVar()
{
NumVariacoes++;
}

bool TBancoLocal::PontoExiste( int nponto )
{
bool found;
map <int, TPonto>::iterator it;

it = Pontos.find( nponto );
if ( it == Pontos.end())
  found = false;
else
  found = true;

return found;
}

TPonto & TBancoLocal::GetRefPontoByTag(String tag, bool &found)
{
map <String, int>::iterator it;
it=MapNPontoPorTag.find(tag);

if (it==MapNPontoPorTag.end())
  {
  return GetRefPonto(-1, found);
  }

return GetRefPonto((*it).second, found);
}

TPonto & TBancoLocal::GetRefPonto(int nponto, bool &found)
{
map <int, TPonto>::iterator it;
it=Pontos.find(nponto);

if (it==Pontos.end())
  found=false;
else
  found=true;

return (*it).second;
}

TPonto & TBancoLocal::GetRefPonto(int nponto)
{
map <int, TPonto>::iterator it;
it=Pontos.find(nponto);

if (it==Pontos.end())
  throw "GetRefPonto: Não encontrou ponto!";

TPonto &ref=(*it).second;
return ref;
}

bool TBancoLocal::ComandoIntertravado( int npontocmd )
{
bool found;

if ( npontocmd == 0 )
  return false;

TPonto &ptcmd = GetRefPonto( npontocmd, found );

if ( ! found )
  return false;

if ( ! ptcmd.EhComando() )
  return false;

if ( ptcmd.PontoIntertrav == 0)
  return false;

TPonto &ptint = GetRefPonto( fabs( ptcmd.PontoIntertrav ), found );
if ( ! found )
  return false;

if ( ptcmd.PontoIntertrav < 0 ) // ponto de intertravamento negativo, inverte a lógica 
  { // ON = LIBERA, OFF = INTERTRAVA
  // qualquer estado que não for ON intertrava
  if ( ptint.Qual.Duplo == ESTDUP_ON )
    return false;
  }
else
  { // OFF = LIBERA, ON = INTERTRAVA
  // qualquer estado que não for OFF intertrava
  if ( ptint.Qual.Duplo == ESTDUP_OFF )
    return false;
  }

return true;
}

bool TBancoLocal::GetPonto(int nponto, double &valor, TFA_Qual &qual, double &tagtempo)
{
map <int, TPonto>::iterator it;
it=Pontos.find(nponto);
if (it==Pontos.end())
  return false; // erro ponto não encontrado

if (((*it).second).Acessando) Sleep(20);
((*it).second).Acessando=true;
valor=((*it).second).Valor;
qual=((*it).second).Qual;
tagtempo=((*it).second).TagTempo;
((*it).second).Acessando=false;

return true;
}

map <int, TPonto> & TBancoLocal::GetMapaPontos(void)
{
map <int, TPonto> & ref=Pontos;
  return ref;
}

int TBancoLocal::EscrevePonto(int nponto, double valor, unsigned char qualif, int calculo, int usakconv, int temtagtmp, int espontaneo)
{
int rg;
int inclui_ev = 0;
bool eh_dupla_transicao = false;
bool mudou_estado = false;
bool mudou_valor = false;
bool mudou_qualif = false;
bool ana_normalizando = false;
TFA_Qual new_qual;
new_qual.Byte = qualif;

map <int, TPonto>::iterator it;

it = Pontos.find(nponto);
if ( it == Pontos.end() )
  {
      // se não achou, insere ponto
      TPonto pt;
      pt.NPonto = nponto;
      Pontos[nponto] = pt;
      it = Pontos.find(nponto);
      ((*it).second).NPonto = nponto;
      ((*it).second).PontoSupCmd = 0;
      ((*it).second).PontoCmdSup = 0;
      ((*it).second).SetAnotacao("");
  }

// não atualiza ponto calculado
if ( ((*it).second).Formula != 0 )
if ( calculo == 0 )
  return 0;

double prev_val = ((*it).second).Valor;

try
{

if (((*it).second).Acessando) Sleep(20);
((*it).second).Acessando = true;

if ( ((*it).second).Qual.Tipo == TIPO_DIGITAL )
  {
  if ( ((*it).second).Qual.Duplo != new_qual.Duplo )
    mudou_estado = true;

  if ( ( ((*it).second).Qual.Duplo == new_qual.Duplo ) &&
       ( ((*it).second).QualEvt.Duplo == new_qual.Duplo ) )
    eh_dupla_transicao = true;
  }

if ( ((*it).second).Qual.Byte != new_qual.Byte )
  mudou_qualif = true;

((*it).second).Qual.Byte = new_qual.Byte;
if (temtagtmp)
  {
  ((*it).second).QualEvt.Byte = new_qual.Byte;
  }

if ( ((*it).second).CodOrigem == CODORIGEM_MANUAL ) // manuais
  {
  ((*it).second).Qual.Origem = ORIGEM_MANUAL; // força origem manual
  ((*it).second).Qual.Subst = 0; // não liga o substituido
  }
else
if ( ((*it).second).CodOrigem == CODORIGEM_CALCULADO ) // calculados
  {
  ((*it).second).Qual.Origem = ORIGEM_CALCULADO;  // força origem calculado
  }

if ( usakconv )
  {
  if ( ((*it).second).Qual.Tipo == TIPO_ANALOGICO )
    {
    valor = ((*it).second).KConv1 * valor + ((*it).second).KConv2; // analógico
    }
  else
    { // digital
    if ( ((*it).second).KConv1 == -1 ) // kconv1==-1 : inverte o estado
      {
      valor = !valor;
      ((*it).second).Qual.Estado = !((*it).second).Qual.Estado;
      ((*it).second).Qual.EstadoH = !((*it).second).Qual.EstadoH;
      }
    }
  }

// gera alarme quando mudou estado mesmo sem tag de tempo
if ( ((*it).second).Qual.Tipo == TIPO_DIGITAL )
if ( !temtagtmp ) // sem tag de tempo
if ( ! ( ((*it).second).EventoDigital && // não pode ser evento com estado que não seja on
         (((*it).second).Qual.Duplo != ESTDUP_ON) ) )
if ( mudou_estado ) // tem que ter mudado o estado
if ( !((*it).second).AlrIn ) // alarme não inibido
if ( ((*it).second).CntAtu > 0 ) // não é primeira atualização
  {
  // kconv2==-1 : gera evento se o valor do ponto digital mudou e não tem tag de tempo e não for a primeira atualização
  if ( ((*it).second).KConv2 == -1 )
    IncluiEvento( nponto,
                  SDE_UTREVGERADO,
                  QUAL_FALHATAG | new_qual.Byte,
                  0,
                  SDE_GERAHORA, // data/hora atual
                  0,
                  0,
                  0,
                  0,
                  0 );
  temtagtmp = true; // digo que tem tag de tempo para poder tocar o alarme sonoro
  }

if ( ((*it).second).Qual.Falha || ((*it).second).TimeoutCongel == 0 ) // se está falhado, não está congelado
  {
  ((*it).second).Congelamento = 0;
  }

 // só gera alarme depois da primeira escrita, ou evento indo para on
 if ( ((*it).second).Qual.Tipo == TIPO_DIGITAL ) // se é digital
 if ( temtagtmp )
 if ( !((*it).second).AlrIn ) // se não está inibido
 if ( ((*it).second).CodOrigem == CODORIGEM_COMANDO || !eh_dupla_transicao || ALARMA_DUPLA_TRANSICAO  ) // só vou alarmar se o valor mudou ou se permite alarmar na dupla transição ou se for retorno de comando
 if ( ( ((*it).second).CntAtu >= 0 && !((*it).second).EventoDigital ) || // não evento
      ( ((*it).second).EventoDigital && valor == FA_ESTFASIM_ON ) || // evento com estado ON
      ( ((*it).second).CodOrigem == CODORIGEM_COMANDO ) // comando (retorno de acionamento de comando pelo BDTR), gera alarme
    )
      {
      ((*it).second).Alarmar();
      IncNumVar();
      int bp = ((*it).second).ComoDeveBipar();
      if ( bp != BEEP_NENHUM )
         {
         AtivaBeep( bp );
         LastBeepPnt = nponto;
         }

/*
      if ( nponto > NPONTO_SIST_MIN && nponto < NPONTO_SIST_MAX )
        { // evento de sistema: não gera alarme
        }
      else
        {
        IncNumVar();

        if  ( ((*it).second).TemCadastro() ) // só bipa se tiver tag cadastrado
          {
          IncNumVar();
          if ( ((*it).second).GetPriority() <= LAST_PRIORITY_THAT_BEEPS )
            { // beep only for priorities from 0 up to LAST_PRIORITY_THAT_BEEPS
            AtivaBeep( BEEP_NORMAL );
            LastBeepPnt = nponto;
            }

          // se for disjuntor abrindo, sem falha, faz com que bipe numa frequência maior
          if ( ((*it).second).CodTpEq == CODTPEQ_DJ &&
               ((*it).second).CodInfo == 0 &&
               ((*it).second).CodOrigem == 0 &&
               ((*it).second).CodOCR == 2 &&
               ((*it).second).Qual.Duplo == ESTDUP_OFF &&
               ((*it).second).Qual.Subst == false &&
               ((*it).second).Qual.Falha == false
             )
            AtivaBeep( BEEP_CRITICO );
          }
        }
*/
      }

// registra hora do alarme, quando muda valor ou na primeira atualização
if ( ((*it).second).Qual.Tipo == TIPO_DIGITAL ) // se é digital
  {
  // se não tem estado de alarme definido e não está alarmado, zera o tag de tempo do alarme
  if ( ((*it).second).EstadoAlarme >= ESTALM_ALL && ((*it).second).SemAlarme() )
    {
    ((*it).second).TagTempoAlarme = 0;
    }
  else
  // marca a hora do alarme
  // se tem estado de alarme definido, testa o estado e marca o tag de acordo
  if ( ((*it).second).Valor != valor || ((*it).second).CntAtu == 0 )
    {
    if ( ( ((*it).second).EstadoAlarme == ESTALM_OFF && new_qual.Duplo == ESTDUP_OFF )
         ||
         ( ((*it).second).EstadoAlarme == ESTALM_ON && new_qual.Duplo == ESTDUP_ON  )
         ||
         ((*it).second).EstadoAlarme == ESTALM_ALL // status
         ||
         ((*it).second).EstadoAlarme == ESTALM_EVT // event
       )
      {
      if ( ((*it).second).CntAtu == 0 && ((*it).second).TagTempoAlarme != 0 )
         { // na primeira atualização, mantém o tag se tiver
         }
      else
         { // atualiza hora do último alarme
         ((*it).second).TagTempoAlarme = (double)Now();
         }
      }
    else
      {
      // quando voltou ao normal, zera a hora do alarme
      // para evento, só zera quando reconhece
      if ( ( ((*it).second).EstadoAlarme == ESTALM_OFF && new_qual.Duplo == ESTDUP_ON )
           ||
           ( ((*it).second).EstadoAlarme == ESTALM_ON && new_qual.Duplo == ESTDUP_OFF  )
         )
        {
        ((*it).second).AlarmAck();
        ((*it).second).TagTempoAlarme = 0;
        }
      }
    }
  }

// Analogic event (e.g.: fault distance of 21 relay)
//   conditions to generate a event:
//        valid value and
//        value changed or spontaneous transmission
// Value is recorded in RTU field of the events table
if ( ((*it).second).Qual.Tipo == TIPO_ANALOGICO )
if ( ((*it).second).EventoAnalogico )
if ( !((*it).second).Qual.Falha )
if ( ( ((*it).second).Valor != valor ) || espontaneo )
if ( !((*it).second).AlrIn )
if ( ((*it).second).CntAtu >= 0 )
   {
    TFA_Qual qlf;
    qlf.Byte = ((*it).second).Qual.Byte;
    qlf.CasaDecimal = ((*it).second).CasaDecimal;
    qlf.EventoAnalogicoVUTR = 1;
    IncluiEvento( nponto,
                  round_( valor * pow10( ((*it).second).CasaDecimal ) ),
                  qlf.Byte,
                  0,
                  SDE_GERAHORA,
                  0,
                  0,
                  0,
                  0,
                  0
                  );

   ((*it).second).Alarmar();
   IncNumVar();
   int bp = ((*it).second).ComoDeveBipar();
   if ( bp != BEEP_NENHUM )
      {
      AtivaBeep( bp );
      LastBeepPnt = nponto;
      }
   }

 if (((*it).second).Valor != valor )
  { // mudou o valor
  mudou_valor = true;

  ((*it).second).Congelamento = 0; // se mudou o valor, não está congelado!
  ((*it).second).TagTempoAltValor = Now(); // marca tempo da última alteração de valor

  if ( ((*it).second).EventoDigital ) // se é evento, já volta para OFF
     {
     valor = FA_ESTFASIM_OFF;
     ((*it).second).Qual.Duplo = ESTDUP_OFF;
     }

  ((*it).second).Valor = valor;
  ((*it).second).sValor = round_( valor ); // vou manter o valor short sem aplicar os kconv

  if ( ((*it).second).CntAtu >=0 )
   ((*it).second).CntAltEst++;

  if ( !((*it).second).AlrIn && // se alarme não está inibido
       ((*it).second).Qual.Tipo == TIPO_ANALOGICO  // se é analógico e
       // && ! ( ((*it).second).LimInf == 0 && ((*it).second).LimSup == 0 )
       // && ! ( ((*it).second).LimInf <= LIMINFMIN && ((*it).second).LimSup >= LIMSUPMAX )
     )
    { // testa limites
    rg = ((*it).second).RegiaoAlmLimite;

    if ( ((*it).second).RegiaoAlmLimite == RGLIM_ALMINF )
      {
      if( valor > ((*it).second).LimSup )
        { rg = RGLIM_ALMSUP; }
      else
      if( valor >= ((*it).second).LimInf + ((*it).second).Hister )
        {
        rg = RGLIM_NORMAL;
        ana_normalizando = true;
        }
      }

    if ( ((*it).second).RegiaoAlmLimite == RGLIM_ALMSUP )
      {
      if( valor < ((*it).second).LimInf )
        { rg = RGLIM_ALMINF; }
      else
      if( valor <= ((*it).second).LimSup - ((*it).second).Hister )
        {
        rg = RGLIM_NORMAL;
        ana_normalizando = true;
        }
      }

    if ( ((*it).second).RegiaoAlmLimite == RGLIM_NORMAL )
      {
      if( valor < ((*it).second).LimInf - ((*it).second).Hister )
        { rg = RGLIM_ALMINF; }
      else
      if( valor > ((*it).second).LimSup + ((*it).second).Hister )
        { rg = RGLIM_ALMSUP; }
      }

    // se limites estão abertos ou zerados, considera região normal  
    if ( ( ((*it).second).LimInf <= LIMINFMIN && ((*it).second).LimSup >= LIMSUPMAX ) ||
         ( ((*it).second).LimInf == 0 && ((*it).second).LimSup == 0) )
      {
      rg = RGLIM_NORMAL;
      }

    // se não esta violando, desliga o alarme
    if ( rg == RGLIM_NORMAL )
      {
      ((*it).second).AlarmAck();
      ((*it).second).TagTempoAlarme = 0;
      }

    // testa troca de regiao de limite, se houver gera alarme
    if ( rg != ((*it).second).RegiaoAlmLimite )
      {
      ((*it).second).RegiaoAlmLimite = rg;

      // se esta violando e houve troca de região, alarma
      if ( rg != RGLIM_NORMAL )
        {
        ((*it).second).Alarmar();
        IncNumVar();
        int bp = ((*it).second).ComoDeveBipar();
        if ( bp != BEEP_NENHUM )
           {
           AtivaBeep( bp );
           LastBeepPnt = nponto;
           }
        }

      // se é temporizado e fora da região, posterga o alarme, caso contrário inclui o evento
      if ( ((*it).second).EhAlarmeTemporizado() )
        {
        if ( rg != RGLIM_NORMAL || (!((*it).second).EventoAtrasado.TemEventoAtrasado && ana_normalizando) )
          { // (quando estiver na região normal só gera evento quando já estiver com alarme persisteste)
          // se o alarme é temporizado, vai incluir só depois de um tempo (em ProcessaTimeout)
          ((*it).second).EventoAtrasado.UTR = SDE_UTREVANALOG;
          ((*it).second).EventoAtrasado.Flags = QUAL_TIPO | rg;
          ((*it).second).EventoAtrasado.TemEventoAtrasado = true;
          }
        else  
        if ( rg == RGLIM_NORMAL )
          ((*it).second).EventoAtrasado.TemEventoAtrasado = false;
        }
      else
        inclui_ev = 1; // vou incluir depois de acessar o ponto para não atrasar o acesso ao mesmo

      IncNumVar();
      }
    }
  }

((*it).second).CntAtu++;
if ( ((*it).second).CntAtu > 0 ) // na primeira (do banco) não atualiza hora
 ((*it).second).TagTempo = Now();

((*it).second).Acessando = false;

if ( inclui_ev )
  {
  IncluiEvento( nponto,
                SDE_UTREVANALOG,
                QUAL_TIPO | rg,
                0,
                SDE_GERAHORA,
                0,
                0,
                0,
                0,
                0
                );
  }
  
// manda para o histórico, se mudou valor ou qualificador, se não estiver inibido, e não for evento                
if ( HIST_RECORD )
if ( fmHist != NULL )
if ( !((*it).second).AlrIn )
if ( !((*it).second).EventoDigital )
  {
  unsigned tick = GetTickCount();
  if ( mudou_estado ||
       mudou_qualif ||
       mudou_valor ||
       ( (tick - ((*it).second).TickHist) > 1000 * 60 * 60 ) // passou mais que uma hora da última gravação
     )
    {
    bool grava = true;

    // não gravaria se medida estivesse dentro da banda morta 
    if ( ((*it).second).TipoAD == 'A' )
      {
      if ( ((*it).second).DentroBandaMortaHist() )
        grava = false;
      }

    // mas se mudou qualificador ou se passou uma hora, grava
    if ( mudou_qualif )
       grava = true;
    else
    if  ( (tick - ((*it).second).TickHist) > 1000 * 60 * 60 )
       grava = true;

    if ( grava )
      {
      fmHist->PushVal( nponto, ((*it).second).Valor, ((*it).second).Qual.Byte, ((*it).second).TagTempo );
      ((*it).second).ValorHist = ((*it).second).Valor;
      ((*it).second).TickHist = tick;
      }
    }
  }
}
catch (Exception &E)
  {
    ((*it).second).Valor = prev_val; // avoid problems with malformed fp value
    ((*it).second).sValor = (short)prev_val;
    ((*it).second).Acessando = 0;
    ((*it).second).Qual.Falha = 1;

    Loga( E.Message + (String)" | Exception for point: " + (String)nponto);
    // dump double float bytes
    unsigned char * pch = (unsigned char *)(&valor);
    Loga( (String)(unsigned int)*pch + (String)" " +
          (String)(unsigned int)*(pch+1) + (String)" " +
          (String)(unsigned int)*(pch+2) + (String)" " +
          (String)(unsigned int)*(pch+3) + (String)" " +
          (String)(unsigned int)*(pch+4) + (String)" " +
          (String)(unsigned int)*(pch+5) + (String)" " +
          (String)(unsigned int)*(pch+6) + (String)" " +
          (String)(unsigned int)*(pch+7) );
  }

return 0; // ok
}

// verifica se ultrapassou a banda morta, considerando o tempo da última gravação
// até 1 minuto = 100% da banda morta
// 2 minutos = 200% da banda morta
bool TPonto::DentroBandaMortaHist()
{
if ( TipoAD == 'D' )
  return false;

if ( ValorHist == Valor )
  return true;

if ( BandaMortaHist == 0 )
  return false;

if ( ValorHist == 0 )
  return false;

// se o valor for baixo, aumenta a banda morta pois normalmente o ruído é maior nesta faixa
float AjusteValorBaixo = 1.0;
if ( Valor < 10.0 )
  AjusteValorBaixo = 2.0;
if ( Valor < 1.0 )
  AjusteValorBaixo = 4.0;

float bm_temporizada = BandaMortaHist * 60000.0 / (float)( 1 + GetTickCount() - TickHist );

if ( bm_temporizada > BandaMortaHist )
  bm_temporizada = BandaMortaHist;

return
 fabs(( 100.0 * ( ValorHist - Valor )) / ValorHist ) < (bm_temporizada * AjusteValorBaixo);
}

int TBancoLocal::GetTagPonto(int nponto, char *tag)
{
map <int, TPonto>::iterator it;
it = Pontos.find( nponto );
if ( it == Pontos.end() )
  {
  strcpy( tag, "" );
  return 1; // ponto não encontrado
  }

strncpy( tag, ((*it).second).Tag, 30 );
tag[30] = 0;

return 0;
}

int TBancoLocal::GetModDescrPonto(int nponto, char * descr)
{
map <int, TPonto>::iterator it;
it = Pontos.find( nponto );
if ( it == Pontos.end() )
  {
  strcpy( descr, "" );
  return 1; // ponto não encontrado
  }

if ( strlen( ((*it).second).Modulo ) != 0 )
  sprintf( descr, "%s~%s", ((*it).second).Modulo, ((*it).second).Descricao );
else
  sprintf( descr, "%s~%s", ((*it).second).Estacao, ((*it).second).Descricao );

return 0;
}

int TBancoLocal::AlarmeNaoReconhecidoPonto(int nponto)
{
map <int, TPonto>::iterator it;

it = Pontos.find(nponto);

if ( it == Pontos.end() )
  {
  return 0; // ponto não encontrado
  }

return ((*it).second).AlarmeNaoReconhecido();
}

// Reconhece o alarme do ponto. Se o ponto for zero, reconhece todos os alarmes
int TBancoLocal::ReconheceAlarmePonto(int nponto)
{
map <int, TPonto>::iterator it;

if ( nponto == 0 )
  {
  for ( it = Pontos.begin(); it != Pontos.end(); it++ )
    {
    if (((*it).second).Acessando) Sleep(20);
    ((*it).second).Acessando = true;
    ((*it).second).AlarmAck();
    // zera a hora do último alarme
    if ( ! ((*it).second).TemAlarmePersistente() )
      {
      ((*it).second).TagTempoAlarme = 0;
      }
    ((*it).second).Acessando = false;
    }
  }
else
  {
  it = Pontos.find(nponto);
  if ( it == Pontos.end() )
    {
    return 1; // ponto não encontrado
    }

  if (((*it).second).Acessando) Sleep(20);
  ((*it).second).Acessando = true;
  ((*it).second).AlarmAck();

  // zera a hora do último alarme
  if ( ! ((*it).second).TemAlarmePersistente() )
    {
    ((*it).second).TagTempoAlarme = 0;
    }

  ((*it).second).Acessando = false;
  }

IncNumVar(); // reporta variação para que a aplicação recarregue a lista de alarmes

return 0; // ok
}

// retorna string em javascript com informações sobre o ponto
String TBancoLocal::ConsultaInfoPonto(int nponto)
{
map <int, TPonto>::iterator it;
String Res;
char buff[1000];
int nponto_cmd;

  it=Pontos.find(nponto);
  if (it==Pontos.end())
    {
    return (String)""; // ponto não encontrado
    }

  sprintf(buff,"NPTO='%d';\n", nponto);
  Res=Res+buff;
  sprintf(buff,"ID='%s';\n", ((*it).second).Tag);
  Res=Res+buff;
  sprintf(buff,"ESTACAO='%s';\n", ((*it).second).Estacao);
  Res=Res+buff;
  sprintf(buff,"MODULO='%s';\n", ((*it).second).Modulo);
  Res=Res+buff;
  sprintf(buff,"DESC='%s~%s';\n", ((*it).second).Modulo, ((*it).second).Descricao);
  Res=Res+buff;
  sprintf(buff,"ST_ON='%s';\n", ((*it).second).EstadoOn);
  Res=Res+buff;
  sprintf(buff,"ST_OFF='%s';\n", ((*it).second).EstadoOff);
  Res=Res+buff;
  sprintf(buff,"CNPTO='%d';\n", 0);
  Res=Res+buff;
  sprintf(buff,"ANOT='%s';\n", ((*it).second).Anotacao);
  Res=Res+buff;
  sprintf(buff,"ALRIN='%d';\n", ((*it).second).AlrIn);
  Res=Res+buff;
  sprintf(buff,"VLNOR='%d';\n", (int)((*it).second).ValorNormal);
  Res=Res+buff;
  sprintf(buff,"ESTALM='%d';\n", (int)((*it).second).EstadoAlarme);
  Res=Res+buff;
  sprintf(buff,"SIMULACAO='%d';\n", GetSimulacao());
  Res=Res+buff;

  if ( ((*it).second).Qual.Tipo==TIPO_ANALOGICO )  // ponto analógico
    {
    sprintf(buff,"LIMI='%1.2f';\n", ((*it).second).LimInf);
    Res=Res+buff;
    sprintf(buff,"LIMS='%1.2f';\n", ((*it).second).LimSup);
    Res=Res+buff;
    sprintf(buff,"HISTER='%1.2f';\n", ((*it).second).Hister);
    Res=Res+buff;
    sprintf(buff,"UNIDADE='%s';\n", ((*it).second).Unidade);
    Res=Res+buff;
    }

  // procura o ponto de comando que tem este supervisionado associado
  nponto_cmd = GetSupCmd(nponto);
  if ( nponto_cmd != 0 )
    {
        sprintf(buff,"CNPTO='%d';\n", nponto_cmd);
        Res=Res+buff;
        sprintf(buff,"CID='%s';\n", Pontos[nponto_cmd].Tag);
        Res=Res+buff;
        sprintf(buff,"CDESC='%s-%s';\n", Pontos[nponto_cmd].Modulo, Pontos[nponto_cmd].Descricao);
        Res=Res+buff;
        sprintf(buff,"CST_ON='%s';\n", Pontos[nponto_cmd].EstadoOn);
        Res=Res+buff;
        sprintf(buff,"CST_OFF='%s';\n", Pontos[nponto_cmd].EstadoOff);
        Res=Res+buff;
     }
return Res;
}

// Retorna lista de subestações separadas por espaços ' '
void TBancoLocal::GetListaSEs(char * lista)
{
set <String>::iterator it;

lista[0]=0;
String Lst="";
String Nome;

  // vai ponto a ponto olhando os tags, comparando com a lista
  for (it = ListaSEs.begin(); it!=ListaSEs.end(); it++)
    {
      Nome= *it;
      if (Nome.Length()>0)
        Lst = Lst + Nome + (String)"-";
    }

strncpy(lista,Lst.c_str(), 4999);
}

// para ordenar lista de pontos colocando os pontos alarmados prioritariamente
// poderia ainda ordenar pelos mais recentes alarmes entre os já reconhecidos
int sort_func( const void *a, const void *b )
{
bool a_alm = BL.AlarmeNaoReconhecidoPonto(*(int *)a);
bool b_alm = BL.AlarmeNaoReconhecidoPonto(*(int *)b);

   if ( a_alm == true && b_alm == false )
     return -1;
   else
   if ( a_alm == false && b_alm == true )
     return 1;
   else
     return 0;
}

// retorna lista de numeros de pontos filtrados por SE e módulo
int TBancoLocal::GetListaPontosFiltSubstMod(char *filtSubst, char *filtMod, int * lista, int tipo_alm, int sem_eventos)
{
map <int, TPonto>::iterator it;
int i = 0;
bool todos_anormais = false;

if ( strcmp(filtSubst, "TODOS_ANORMAIS") == 0 )
  {
  todos_anormais = true;
  }

int limnptos = LIMITE_PONTOS_TABULAR;

if ( todos_anormais )
  limnptos = LIMITE_PONTOS_ANORMAIS;

// trata os '*' no inicio e no fim do filtro

  // vai ponto a ponto olhando os tags, comparando com a lista
  for ( it = Pontos.begin(); it != Pontos.end() && i < limnptos; it++ )
    {
    // se ainda não está na lista acrescenta

    // evita pontos não cadastrados
    if ( (String)((*it).second).Tag == (String)"" || (*it).first == 0 )
      continue;

    // tira os pontos de sistema da lista de alarmes geral
    if ( todos_anormais && ((*it).first) > NPONTO_SIST_MIN && ((*it).first) < NPONTO_SIST_MAX )
      continue;

    if ( tipo_alm == 2 ) // se for tipo 2 (somente persistentes mais eventos não reconhecidos)
      {
      if ( ((*it).second).EhDigital() )
      if ( ! ((*it).second).EhEventoDigital() )
      if ( ((*it).second).GetEstadoAlarme() == ESTALM_ALL ) // tira os digitais com estado que não define alarme persistente
        continue;

      if ( ! ((*it).second).ValorOk() ) // tira os valores ruins
        continue;

      // tira pontos com estado de alarme definido, normalizados
      if (   ((*it).second).EhDigital() &&
             ( ((*it).second).GetEstadoAlarme() == ESTALM_OFF || ((*it).second).GetEstadoAlarme() == ESTALM_ON ) &&
             !((*it).second).TemAlarmePersistente() )
        continue;

      // tira não alarmados sem persistente
      if ( ((*it).second).SemAlarme() && !((*it).second).TemAlarmePersistente() )
        continue;
      }

    // devo excluir eventos ? (sempre deixa os com alarme)
    if ( sem_eventos && ((*it).second).EventoDigital && ((*it).second).SemAlarme() )
      continue;

    if ( todos_anormais )
      {
      // tira os inibidos
      if ( ((*it).second).AlarmeInibido() )
        continue; 

      // tira pontos com estado de alarme definido, normalizados
      if (   ((*it).second).EhDigital() &&
             ( ((*it).second).GetEstadoAlarme() == ESTALM_OFF || ((*it).second).GetEstadoAlarme() == ESTALM_ON ) &&
             ( ((*it).second).Qual.Duplo == ESTDUP_ON || ((*it).second).Qual.Duplo == ESTDUP_OFF ) &&
             !((*it).second).TemAlarmePersistente() )
        continue;

      // tira não alarmados sem persistente
      if ( ((*it).second).SemAlarme() && !((*it).second).TemAlarmePersistente() )
        continue;

      // tira os indeterminados e inválidos se não alarmados
      if ( ((*it).second).EhDigital() &&
           ( ((*it).second).Qual.Duplo == ESTDUP_INDETERMINADO || ((*it).second).Qual.Duplo == ESTDUP_INVALIDO ) &&
           ((*it).second).SemAlarme() )
        continue;

      if ( ((*it).second).CodOrigem == CODORIGEM_COMANDO || ((*it).second).CodOrigem == CODORIGEM_MANUAL )
        continue;

      if ( ((*it).second).PontoCmdSup != 0 )
        lista[i++] = ((*it).second).PontoCmdSup; // coloca o ponto de comando associado, se houver

      lista[i++] = (*it).first;
      }
    else
      { // filter by substation and bay
      if ( strcmp( filtSubst, ((*it).second).GetEstacao() ) == 0 &&
           ( strcmp( filtMod, ((*it).second).GetModulo() ) == 0 ||
             strcmp( filtMod, "" ) == 0 )
         )
        lista[i++] = (*it).first;
      }
    }

  if ( i >= limnptos ) // se extrapolou o limite
    {
    // order: alarmed in front
    qsort( (void *)lista, i, sizeof(lista[0]), sort_func );
    i = limnptos;
    }

  lista[i] = 0;  // marca fim da lista, limitando pelo limnptos
  return i;
}

// retorna lista de numeros de pontos filtrados
int TBancoLocal::GetListaPontosFilt(char *filt, int * lista, int tipo_alm, int sem_eventos)
{
map <int, TPonto>::iterator it;
int i = 0;
bool Inicio;
String Filt;
bool todos_anormais = false;

Filt = filt;

if ( Filt == "TODOS_ANORMAIS" )
  {
  todos_anormais = true;
  }

int limnptos = LIMITE_PONTOS_TABULAR;

if ( todos_anormais )
  limnptos = LIMITE_PONTOS_ANORMAIS;

// trata os '*' no inicio e no fim do filtro

if ( Filt[1] == '*' )
  { // tem * no inicio, vai procurar em qualquer posicao
  Inicio = false;
  Filt = Filt.SubString(2,100);
  }
else
  { // procura só no inicio da string
  Inicio = true;
  if ( Filt[Filt.Length()] == '*' )
    Filt = Filt.SubString(1,Filt.Length()-1);
  if ( Filt.Length() < 4 )
    Filt = Filt + "-";
  }

  // vai ponto a ponto olhando os tags, comparando com a lista
  for ( it = Pontos.begin(); it != Pontos.end() && i < limnptos; it++ )
    {
    // se ainda não está na lista acrescenta

    // evita pontos não cadastrados
    if ( (String)((*it).second).Tag == (String)"" || (*it).first == 0 )
      continue;

    // tira os pontos de sistema da lista de alarmes geral
    if ( todos_anormais && ((*it).first) > NPONTO_SIST_MIN && ((*it).first) < NPONTO_SIST_MAX )
      continue;

    if ( tipo_alm == 2 ) // se for tipo 2 (somente persistentes mais eventos não reconhecidos)
      {
      if ( ((*it).second).EhDigital() )
      if ( ! ((*it).second).EhEventoDigital() )
      if ( ((*it).second).GetEstadoAlarme() == ESTALM_ALL ) // tira os digitais com estado que não define alarme persistente
        continue;

      if ( ! ((*it).second).ValorOk() ) // tira os valores ruins
        continue;

      // tira pontos com estado de alarme definido, normalizados 
      if (   ((*it).second).EhDigital() &&
             ( ((*it).second).GetEstadoAlarme() == ESTALM_OFF || ((*it).second).GetEstadoAlarme() == ESTALM_ON ) &&
             !((*it).second).TemAlarmePersistente() )
        continue;

      // tira não alarmados sem persistente
      if ( ((*it).second).SemAlarme() && !((*it).second).TemAlarmePersistente() )
        continue;
      }

    // devo excluir eventos ? (sempre deixa os com alarme)
    if ( sem_eventos && ((*it).second).EventoDigital && ((*it).second).SemAlarme() )
      continue;

    if ( todos_anormais )
      {
      // tira pontos com estado de alarme definido, normalizados 
      if (   ((*it).second).EhDigital() &&
             ( ((*it).second).GetEstadoAlarme() == ESTALM_OFF || ((*it).second).GetEstadoAlarme() == ESTALM_ON ) &&
             ( ((*it).second).Qual.Duplo == ESTDUP_ON || ((*it).second).Qual.Duplo == ESTDUP_OFF ) &&
             !((*it).second).TemAlarmePersistente() )
        continue;

      // tira não alarmados sem persistente
      if ( ((*it).second).SemAlarme() && !((*it).second).TemAlarmePersistente() )
        continue;

      // tira os indeterminados e inválidos se não alarmados
      if ( ((*it).second).EhDigital() &&
           ( ((*it).second).Qual.Duplo == ESTDUP_INDETERMINADO || ((*it).second).Qual.Duplo == ESTDUP_INVALIDO ) &&
           ((*it).second).SemAlarme() )
        continue;

      if ( ((*it).second).CodOrigem == CODORIGEM_COMANDO || ((*it).second).CodOrigem == CODORIGEM_MANUAL )
        continue;

      if ( ((*it).second).PontoCmdSup != 0 )
        lista[i++] = ((*it).second).PontoCmdSup; // coloca o ponto de comando associado, se houver

      lista[i++] = (*it).first;
      }
    else
    if ( Inicio )
      { // filter by tag at beggining
      if (
           ( Filt.Length()==4 && ((String)((*it).second).Tag).SubString(1,4).Pos(Filt) )
           ||
           ( Filt.Length()!=4 && ((String)((*it).second).Tag).Pos(Filt) )
         )
        lista[i++] = (*it).first;
      }
    else
      { // filter by tag at any position
      if ( ((String)((*it).second).Tag).Pos(Filt) )
        lista[i++] = (*it).first;
      }
    }

  if ( i >= limnptos ) // se extrapolou o limite
    {
    // order: alarmed in front
    qsort( (void *)lista, i, sizeof(lista[0]), sort_func );
    i = limnptos;
    }

  lista[i] = 0;  // marca fim da lista, limitando pelo limnptos
  return i;
}

String TBancoLocal::GetNome(int nponto)
{
return (String)Pontos[nponto].Tag;
}

String TBancoLocal::GetTipo(int nponto)
{
return Pontos[nponto].GetTipo();
}

int TBancoLocal::GetSupCmd(int nponto)
{
if ( Pontos[nponto].CodOrigem == CODORIGEM_COMANDO )
  return Pontos[nponto].PontoSupCmd;
else
  return Pontos[nponto].PontoCmdSup;
}

void TBancoLocal::RecebeuIntegridade()
{
CntIntegridade++;
}

int TBancoLocal::GetIntegridade()
{
return CntIntegridade;
}

bool TBancoLocal::GetAlrIn(int nponto)
{
map <int, TPonto>::iterator it;

  it=Pontos.find(nponto);
  if (it==Pontos.end())
    return false; // ponto não encontrado

  return (*it).second.AlrIn;
}

// percorre todo o banco e marca falha nos pontos com tag muito antigo, verifica congelamento
void TBancoLocal:: ProcessaTimeout()
{
map <int, TPonto>::iterator it;
TDateTime dt;
int bp;

  // nao dá timeout quando tem simulaçao
  // if (HaSimulacao())
  //  return;

  for (it = Pontos.begin(); it!=Pontos.end(); it++)
    {
    // se tem evento atrasado e passou o tempo inclui o evento
    if ( ((*it).second).EventoAtrasado.TemEventoAtrasado &&
         ( (double)Now() - (double)((*it).second).TagTempoAlarme )*24*60*60 > ((*it).second).GetTemporizacaoAlarme()
       )
      {
      IncluiEvento( ((*it).first),
                ((*it).second).EventoAtrasado.UTR,
                ((*it).second).EventoAtrasado.Flags,
                0,
                SDE_GERAHORA,
                0,
                0,
                0,
                0,
                0
                );
      ((*it).second).EventoAtrasado.TemEventoAtrasado = false; // descarta

      // Bipa se for o caso
      bp = ((*it).second).ComoDeveBipar();
      if ( bp != BEEP_NENHUM )
         {
         AtivaBeep( bp );
         }
      }

    if ( ((*it).second).CodOrigem == 6 || ((*it).second).Qual.Subst ) // manuais e substituídos não falham, nem congelam
      continue;

    if ( ((*it).first)>=NPONTO_SIST_MIN        &&  // se for ponto de sistema, não processa timeout
         ((*it).first)<=NPONTO_SIST_MAX        &&
         ((*it).first)!=NPONTO_COMUNIC_I104    &&  // exceto estes do 104, BDTR e acerto de hora
         // ((*it).first)!=NPONTO_COMUNIC_BDTR    &&
         ((*it).first)!=NPONTO_ACERTO_HORA )
      continue;

    if ( ! ((*it).second).Qual.Falha ) 
      { // sem falha
      if (((*it).second).Acessando) Sleep(20);
      ((*it).second).Acessando=true;
      // timeout
      if ( ((*it).second).TagTempo + ((*it).second).Timeout < dt.CurrentDateTime() )
        {
        // para dar falha de comunicação do protocolo quando não é atualizado por tempo maior que a integridade
        if (((*it).first)==NPONTO_COMUNIC_I104 ||
            // ((*it).first)==NPONTO_COMUNIC_BDTR ||
            ((*it).first)==NPONTO_ACERTO_HORA)
          {
          ((*it).second).QualAnt.Byte=((*it).second).Qual.Byte;
          ((*it).second).Qual.Byte=ESTDUP_OFF;
          ((*it).second).Valor=FA_ESTFASIM_OFF;
          }
        else
          { // para dar falha no ponto que não está sendo atualizado há tempo maior que a integridade
          if ( !HaSimulacao() )
            {
            ((*it).second).QualAnt.Byte=((*it).second).Qual.Byte;
            ((*it).second).Qual.Byte|=0x80;
            }
          }
        }
      else
        {
        // congelamento, somente analógicos, só sai do congelamento quando o valor for atualizado
        if ( ((*it).second).Qual.Tipo==TIPO_ANALOGICO )
        if ( ((*it).second).TimeoutCongel >0 )
        if ( fabs(((*it).second).Valor)>=1.0 ) // valor menor que 1 não congela
        if ( ((*it).second).TagTempoAltValor + ((*it).second).TimeoutCongel < dt.CurrentDateTime() )
          ((*it).second).Congelamento=1;
        }
      ((*it).second).Acessando=false;
      }
    }
}

void TBancoLocal::Calcula(int nponto)
{
map <int, TPonto>::iterator it;
double r=0;
int i;
TFA_Qual q;
int temtagtmp = 0;
int usakconv = 0;

it=Pontos.find(nponto);
if (it==Pontos.end())
  return; // ponto não encontrado

q.Byte=(*it).second.Qual.Byte;

try
  {
  switch ((*it).second.Formula)
    {
    case COD_FORMULA_CORRENTE:
        if (Pontos[(*it).second.Parcelas[2]].Valor > 0)
          r = ( 577.35027 *
               sqrt( ( Pontos[(*it).second.Parcelas[0]].Valor * Pontos[(*it).second.Parcelas[0]].Valor ) +
                     ( Pontos[(*it).second.Parcelas[1]].Valor * Pontos[(*it).second.Parcelas[1]].Valor )
                   )
             ) / Pontos[(*it).second.Parcelas[2]].Valor;
        q.Falha = Pontos[(*it).second.Parcelas[0]].Qual.Falha ||
                  Pontos[(*it).second.Parcelas[1]].Qual.Falha ||
                  Pontos[(*it).second.Parcelas[2]].Qual.Falha ;
        q.Tipo = TIPO_ANALOGICO;
        break;
    case COD_FORMULA_PA:
        r = sqrt ((Pontos[(*it).second.Parcelas[0]].Valor *
                   Pontos[(*it).second.Parcelas[0]].Valor) +
                  (Pontos[(*it).second.Parcelas[1]].Valor *
                   Pontos[(*it).second.Parcelas[1]].Valor));
        q.Falha = Pontos[(*it).second.Parcelas[0]].Qual.Falha ||
                  Pontos[(*it).second.Parcelas[1]].Qual.Falha;
        q.Tipo = TIPO_ANALOGICO;
        break;
    case COD_FORMULA_PAI:
         r = (Pontos[(*it).second.Parcelas[0]].Valor * Pontos[(*it).second.Parcelas[1]].Valor * 1.73205080757) / 1000;
         q.Falha = Pontos[(*it).second.Parcelas[0]].Qual.Falha ||
                   Pontos[(*it).second.Parcelas[1]].Qual.Falha;
         q.Tipo = TIPO_ANALOGICO;
        break;
    case COD_FORMULA_E:
         // faz um e dos estados, para ter tag de tempo usar kconv2=-1
         q.Falha = 0;
         q.Duplo = ESTDUP_ON;
         for ( i=0; i<MAX_PARCELAS && (*it).second.Parcelas[i]!=0; i++ )
           {
           if ( Pontos[(*it).second.Parcelas[i]].Qual.Duplo == ESTDUP_OFF )
             q.Duplo = ESTDUP_OFF;
           else
           if ( Pontos[(*it).second.Parcelas[i]].Qual.Duplo == ESTDUP_INDETERMINADO ||
                Pontos[(*it).second.Parcelas[i]].Qual.Duplo == ESTDUP_INVALIDO
              )
             {
             q.Duplo = Pontos[(*it).second.Parcelas[i]].Qual.Duplo;
             q.Falha = 1;
             break;
             }

           q.Falha = q.Falha || Pontos[(*it).second.Parcelas[i]].Qual.Falha;
           }
         r = q.Estado;
         q.Tipo = TIPO_DIGITAL;
         usakconv = 1; // usakconv=1 e temtagtmp=0 para o escreve ponto gerar o tag na transição
        break;
    case COD_FORMULA_OU:
         // faz um ou dos estados, para ter tag de tempo usar kconv2=-1
         q.Falha = 0;
         q.Duplo = ESTDUP_OFF;
         for ( i=0; i<MAX_PARCELAS && (*it).second.Parcelas[i]!=0; i++ )
           {
           if ( Pontos[(*it).second.Parcelas[i]].Qual.Duplo == ESTDUP_ON )
             q.Duplo = ESTDUP_ON;
           else
           if ( Pontos[(*it).second.Parcelas[i]].Qual.Duplo == ESTDUP_INDETERMINADO ||
                Pontos[(*it).second.Parcelas[i]].Qual.Duplo == ESTDUP_INVALIDO
              )
             {
             q.Duplo = Pontos[(*it).second.Parcelas[i]].Qual.Duplo;
             q.Falha = 1;
             break;
             }

           q.Falha = q.Falha || Pontos[(*it).second.Parcelas[i]].Qual.Falha;
           }
         r = q.Estado;
         q.Tipo = TIPO_DIGITAL;
         usakconv = 1; // usakconv=1 e temtagtmp=0 para o escreve ponto gerar o tag na transição
        break;
    case COD_FORMULA_SOMA:
         q.Falha=0;
         for (i=0; i<MAX_PARCELAS && (*it).second.Parcelas[i]!=0; i++)
           {
           r+=Pontos[(*it).second.Parcelas[i]].Valor;
           q.Falha=q.Falha||Pontos[(*it).second.Parcelas[i]].Qual.Falha;
           }
         q.Tipo = TIPO_ANALOGICO;
        break;
    case COD_FORMULA_SOMANEG:
         q.Falha=0;
         for (i=0; i<MAX_PARCELAS && (*it).second.Parcelas[i]!=0; i++)
           {
           r-=Pontos[(*it).second.Parcelas[i]].Valor;
           q.Falha=q.Falha||Pontos[(*it).second.Parcelas[i]].Qual.Falha;
           }
         q.Tipo = TIPO_ANALOGICO;
        break;
    case COD_FORMULA_DIF:
         r = Pontos[(*it).second.Parcelas[0]].Valor - Pontos[(*it).second.Parcelas[1]].Valor;
         q.Falha = Pontos[(*it).second.Parcelas[0]].Qual.Falha ||
                   Pontos[(*it).second.Parcelas[1]].Qual.Falha;
         q.Tipo = TIPO_ANALOGICO;
        break;
    case COD_FORMULA_ESCOLHA_DIG:
        {
        // Escolhe o melhor entre 2 pontos, se ambos estiverem válidos mas diferentes, falha o ponto
        double tag;

        r = Pontos[(*it).second.Parcelas[0]].Valor;
        q.Byte =  Pontos[(*it).second.Parcelas[0]].Qual.Byte;
        q.Falha =  1;

        if ( ! Pontos[(*it).second.Parcelas[0]].Qual.Falha )
           {
           q.Falha =  0;
           tag = Pontos[(*it).second.Parcelas[0]].TagTempoEvento;
           if ( (! Pontos[(*it).second.Parcelas[1]].Qual.Falha) &&
                (q.Duplo != Pontos[(*it).second.Parcelas[1]].Qual.Duplo) )
             q.Falha = 1;
           }
        else
        if ( ! Pontos[(*it).second.Parcelas[1]].Qual.Falha )
           {
           tag = Pontos[(*it).second.Parcelas[1]].TagTempoEvento;
           q.Byte = Pontos[(*it).second.Parcelas[1]].Qual.Byte;
           r = Pontos[(*it).second.Parcelas[1]].Valor;
           }

        // se mudou estado, insere estampa da origem
        if ( (*it).second.Qual.Duplo != q.Duplo )
           if ( !(*it).second.AlarmeInibido() )
              {
              TDateTime dt=tag;

              unsigned short year; unsigned short month; unsigned short day;
              unsigned short hour; unsigned short min; unsigned short sec; unsigned short msec;
              dt.DecodeDate(&year, &month, &day);
              dt.DecodeTime(&hour, &min, &sec, &msec);

              IncluiEvento( nponto,
                            SDE_UTREVGERADO,
                            q.Byte,
                            year,
                            month,
                            day,
                            hour,
                            min,
                            sec,
                            msec
                            );
              temtagtmp = 1;
              }
        }
        break;
    case COD_FORMULA_ESCOLHA_ANA:
        // Escolhe o melhor entre 2 pontos analógicos
        r = Pontos[(*it).second.Parcelas[0]].Valor;
        q.Byte =  Pontos[(*it).second.Parcelas[0]].Qual.Byte;
        if ( ( q.Falha == 1 ) &&
             ( ! Pontos[(*it).second.Parcelas[1]].Qual.Falha ) )
           {
           q.Byte = Pontos[(*it).second.Parcelas[1]].Qual.Byte;
           r = Pontos[(*it).second.Parcelas[1]].Valor;
           }
        break;
    case COD_FORMULA_ESTCOMUNIC:
         // estado de saúde da comunicação, se uma das parcelas estiver sem falha resultado é ON
         q.Byte = 0;
         q.Tipo = TIPO_DIGITAL;
         if ( tmTimeout->Cnt > 120 )
           q.Falha=0;
         else
           q.Falha=1;
         q.Duplo = ESTDUP_OFF;
         for (i=0; i<MAX_PARCELAS && (*it).second.Parcelas[i]!=0; i++)
           {
           if ( Pontos[(*it).second.Parcelas[i]].Qual.Falha == 0 )
             q.Duplo = ESTDUP_ON;
           }
         r = q.Estado;

        // não gera evento quando inibido,
        // não gera evento por um tempo na inicialização   
        if ( !(*it).second.AlarmeInibido() && q.Falha == 0  )
        {

        // quando validar o cálculo após um tempo da inicialização
        // se estiver falhado, gera evento da falha
        // se mudou estado, gera evento
        if ( ( (*it).second.Qual.Duplo != q.Duplo ) ||
             ( (*it).second.Qual.Falha != q.Falha && q.Duplo == ESTDUP_OFF )
           )
          {
            IncluiEvento( nponto,
                          SDE_UTREVGERADO,
                          q.Byte,
                          0,
                          SDE_GERAHORA,
                          0,
                          0,
                          0,
                          0,
                          0 );
            temtagtmp = 1;
            }
        }
        break;
    case COD_FORMULA_DISTMAXMIN:
        {
        float max=-1e30, min=1e30;
        q.Falha=0;
        for (i=0; i<MAX_PARCELAS && (*it).second.Parcelas[i]!=0; i++)
          {
          max = Pontos[(*it).second.Parcelas[i]].Valor > max ? Pontos[(*it).second.Parcelas[i]].Valor : max;
          min = Pontos[(*it).second.Parcelas[i]].Valor < min ? Pontos[(*it).second.Parcelas[i]].Valor : min;
          q.Falha=q.Falha||Pontos[(*it).second.Parcelas[i]].Qual.Falha;
          }
        r = max - min;
        q.Tipo = TIPO_ANALOGICO;
        }
        break;
    case COD_FORMULA_DUPLO:
        {
         // double bit from 2 single bit
        double tag;

        r = Pontos[(*it).second.Parcelas[0]].Valor;
        q.Byte = Pontos[(*it).second.Parcelas[0]].Qual.Byte;

        if (Pontos[(*it).second.Parcelas[0]].Qual.Duplo == ESTDUP_ON &&
            Pontos[(*it).second.Parcelas[1]].Qual.Duplo == ESTDUP_OFF)
           {
           q.Duplo = ESTDUP_ON;
           r = FA_ESTFASIM_ON;
           }
        else
        if (Pontos[(*it).second.Parcelas[0]].Qual.Duplo == ESTDUP_OFF &&
            Pontos[(*it).second.Parcelas[1]].Qual.Duplo == ESTDUP_ON)
           {
           q.Duplo = ESTDUP_OFF;
           r = FA_ESTFASIM_OFF;
           }
        else
        if (Pontos[(*it).second.Parcelas[0]].Qual.Duplo == ESTDUP_ON &&
            Pontos[(*it).second.Parcelas[1]].Qual.Duplo == ESTDUP_ON)
           {
           q.Duplo = ESTDUP_INVALIDO;
           r = FA_ESTFASIM_ON;
           }
        else
        if (Pontos[(*it).second.Parcelas[0]].Qual.Duplo == ESTDUP_OFF &&
            Pontos[(*it).second.Parcelas[1]].Qual.Duplo == ESTDUP_OFF)
           {
           q.Duplo = ESTDUP_INDETERMINADO;
           r = FA_ESTFASIM_OFF;
           }    

        q.Falha =  Pontos[(*it).second.Parcelas[0]].Qual.Falha | Pontos[(*it).second.Parcelas[1]].Qual.Falha;

        // get time from latest
        if (Pontos[(*it).second.Parcelas[0]].TagTempoEvento > Pontos[(*it).second.Parcelas[1]].TagTempoEvento)
          tag = Pontos[(*it).second.Parcelas[0]].TagTempoEvento;
        else
          tag = Pontos[(*it).second.Parcelas[1]].TagTempoEvento;

        // changed state: insert timstamp from source
        if ( (*it).second.Qual.Duplo != q.Duplo )
           if ( !(*it).second.AlarmeInibido() )
              {
              TDateTime dt=tag;

              unsigned short year; unsigned short month; unsigned short day;
              unsigned short hour; unsigned short min; unsigned short sec; unsigned short msec;
              dt.DecodeDate(&year, &month, &day);
              dt.DecodeTime(&hour, &min, &sec, &msec);

              IncluiEvento( nponto,
                            SDE_UTREVGERADO,
                            q.Byte,
                            year,
                            month,
                            day,
                            hour,
                            min,
                            sec,
                            msec
                            );
              temtagtmp = 1;
              }
        }
        break;
    case COD_FORMULA_DIV:
         if (Pontos[(*it).second.Parcelas[1]].Valor == 0 )
           {
           if ( Pontos[(*it).second.Parcelas[0]].Valor > 0)
             r = DBL_MAX;
           else if ( Pontos[(*it).second.Parcelas[0]].Valor < 0)
             r = -DBL_MAX;
           else if ( Pontos[(*it).second.Parcelas[0]].Valor == 0)
             r = 0;
           }
         else
           r = Pontos[(*it).second.Parcelas[0]].Valor / Pontos[(*it).second.Parcelas[1]].Valor;
         q.Falha = Pontos[(*it).second.Parcelas[0]].Qual.Falha |
                   Pontos[(*it).second.Parcelas[1]].Qual.Falha;
         q.Tipo = TIPO_ANALOGICO;
        break;
    default:
        q.Tipo = TIPO_ANALOGICO;
        q.Byte|=QUAL_FALHA;
        break;
    }
  }
catch (Exception &E)
  {
  q.Byte|=QUAL_FALHA;
  }

  // marca ponto como calculado e escreve resultado
  q.Origem1=1;
  q.Origem2=0;
  EscrevePonto(nponto, r, q.Byte, 1, usakconv, temtagtmp);
}

// percorre todo o banco calcula pontos digitais com fórmula
void TBancoLocal:: ProcessaCalculosDigitais()
{
map <int, TPonto>::iterator it;

  for (it = Pontos.begin(); it!=Pontos.end(); it++)
    {
    if ( ((*it).second).TemFormula() )
    if ( ((*it).second).EhDigital() )
      Calcula(((*it).first));
    }
}

// percorre todo o banco calcula pontos analógicos com fórmula
void TBancoLocal:: ProcessaCalculosAnalogicos()
{
map <int, TPonto>::iterator it;

  for (it = Pontos.begin(); it!=Pontos.end(); it++)
    {
    if ( ((*it).second).TemFormula() )
    if ( ((*it).second).EhAnalogico() )
      Calcula(((*it).first));
    }
}

void TBancoLocal::SilenciaBeep()
{
if ( TemBeep ) // quando silencia o beep, avisa variação digital para atualizar os visores
  {
  IncNumVar();
  }

TemBeep = BEEP_NENHUM;
}

int TBancoLocal::HaBeepAtivo()
{
return TemBeep;
}

void TBancoLocal::AtivaBeep( int tipo )
{
if ( TemBeep == BEEP_NENHUM || TemBeep == BEEP_NORMAL )
if ( tipo != BEEP_NENHUM )
  TemBeep = tipo;
}

int TBancoLocal::SetAckCmd(int nponto, int falha, double cmd) // seta ack de comando
{
map <int, TPonto>::iterator it;

  it = Pontos.find(nponto);
  if (it == Pontos.end())
    return false; // ponto não encontrado

 (*it).second.CmdAckCmd = cmd;
 (*it).second.CmdAckFalha = falha;
 (*it).second.CmdAckCnt++;
 (*it).second.CmdAckTagTempo = Now();

 Loga( (String)"Command Ack: point=" + (String)nponto + (String)", fail=" + (String)falha + (String)", val=" + (String)cmd + (String)", id=" + (String)((*it).second.GetNome()), ARQUIVO_LOGCMD );

 return true;
}

int TBancoLocal::GetAckCmd(int nponto, int *falha, double *cmd, double * hora) // testa ack de comando
{
map <int, TPonto>::iterator it;

  it=Pontos.find(nponto);
  if (it==Pontos.end())
    return -1; // not found

  if (cmd!=NULL)
    *cmd=(*it).second.CmdAckCmd;

  if (falha!=NULL)
    *falha=(*it).second.CmdAckFalha;

  if (hora!=NULL)
    *hora=(*it).second.CmdAckTagTempo;

return (*it).second.CmdAckCnt;
}

void TBancoLocal::SetBeepIntSpeaker(bool bipa)          // bipa ou nao no speaker interno
{
BipaNoSpeaker = bipa;
}

// Simulação randômica de valores, estados e eventos para todos os pontos
void TBancoLocal::SetValorTipico( int nponto, double val )
{
map <int, TPonto>::iterator it;

it = Pontos.find( nponto );
if ( it == Pontos.end() )
  return; // ponto não encontrado

(*it).second.ValorTipico = val;
return;
}

// Simulação randômica de valores, estados e eventos para todos os pontos
void TBancoLocal::SetSimulacao( int modsimul )
{
  ModoSimulacao = modsimul;
}

// Retorna o modo de simulação
int TBancoLocal::GetSimulacao()
{
  return ModoSimulacao;
}

// Retorna se existe simulação randômica de valores, estados e eventos para todos os pontos
bool TBancoLocal::HaSimulacao()
{
  return ( ModoSimulacao != SIMULMOD_NAO );
}

// seta o nome do usuário do sistema
char * TBancoLocal::getUserName()
{
return UserName;
}

// retorna o nome do usuário do sistema
void TBancoLocal::setUserName(char * str)
{
strncpy(UserName, str, sizeof(UserName)-1);
UserName[sizeof(UserName)-1]='\0';
}

//------------------------------------------------------------------------------------------------------------------

// recebe escrita de ponto em formato IEC104
int TBancoLocal::EscreveIEC( unsigned int endereco, unsigned int tipo, void *ptinfo, unsigned int taminfo, unsigned int prim, unsigned int sec, unsigned int causa )
{
unsigned char * pdig;
TFA_Qual qual;
double val;
int temtag = 0;

// converte endereço físico para nponto
int nponto = BL.NPontoPorEndUTR( endereco, sec );
if ( nponto == 0 )
  return -1;

// evita pontos inexistentes na base
bool found;
TPonto &pt = BL.GetRefPonto( nponto, found );
if ( !found )
  return -2;

// se foi definida uma UTR na base para o ponto, só atualiza o ponto se vier desta UTR
if ( ( pt.UTR !=  0 ) &&
     ( pt.UTR != (signed) sec ) )
  return -3;

try
  {
  switch (tipo & 0xFF)
      {
      case 1:  // simples sem tag
      case 3:  // duplo sem tag
      case 2:  // simples com tag
      case 4:  // duplo com tag
      case 30: // simples com tag longa
      case 31: // duplo com tag longa
          pdig = (unsigned char *)ptinfo;

          // traduz os qualificadores
          if ( tipo == 3 || tipo == 4 || tipo == 31 )
            {
            qual.Estado = *pdig & 0x01;  // duplo coloca os 2 últimos bits
            qual.EstadoH = (*pdig & 0x02) ? 1 : 0;
            }
          else
            {
            qual.Estado = !( *pdig & 0x01 ); // simples: vai para o evento como se fosse duplo 01 ou 10
            qual.EstadoH = !qual.Estado;
            }

          qual.Subst = *pdig & 0x30; // testa BL e SB
          qual.Origem1 = 0; // origem: supervisionado = 00
          qual.Origem2 = 0;
          qual.Quest = 0;
          qual.Tipo = TIPO_DIGITAL; // 0=digital, 1=analógico
          qual.Falha = *pdig & 0xC0; // testa IV e NT
          val = qual.Estado;

          if ( tipo == 2 || tipo == 4 )  // não tem no 104, mas vou prever assim mesmo
            { // trata tag de tempo curta
            temtag = 1;
            digital_w_time3_seq * dt3 = (digital_w_time3_seq *) ptinfo;

            // completa o tag com a data e hora atual
            TDateTime dt = Now();
            unsigned short year; unsigned short month; unsigned short day;
            unsigned short hour; unsigned short min; unsigned short sec; unsigned short msec;
            dt.DecodeDate( &year, &month, &day );
            dt.DecodeTime( &hour, &min, &sec, &msec );

            qual.Quest = dt3->min & 0x80; // tag invalido ?

            if ( !pt.AlarmeInibido() )
              IncluiEvento( nponto,
                            sec,
                            qual.Byte,
                            year,
                            month,
                            day,
                            hour,
                            dt3->min & 0x7f,
                            dt3->ms / 1000,
                            dt3->ms % 1000
                            );
            }
          if ( tipo == 30 || tipo == 31 )
            { // trata tag de tempo longa
            temtag = 1;
            digital_w_time7_seq * dt7 = (digital_w_time7_seq *) ptinfo;

            qual.Quest = dt7->min & 0x80; // tag invalido ?

            if ( !pt.AlarmeInibido() )
              IncluiEvento( nponto,
                            sec,
                            qual.Byte,
                            2000 + ( dt7->ano & 0x7f ),
                            dt7->mes & 0x0f,
                            dt7->dia & 0x1f,
                            dt7->hora & 0x1f,
                            dt7->min & 0x3f,
                            dt7->ms / 1000,
                            dt7->ms % 1000
                            );
            }
          break;

      case 5: // step position
          {
          step_seq * stp = (step_seq *) ptinfo;
          val = stp->vti & 0x7f;
          qual.CasaDecimal1 = 0;
          qual.CasaDecimal2 = 0;
          qual.Subst = stp->qds & 0x30; // testa BL e SB
          qual.Origem1 = 0; // origem: supervisionado = 00
          qual.Origem2 = 0;
          qual.Quest = 0;
          qual.Tipo = TIPO_ANALOGICO; // 0=digital, 1=analógico
          qual.Falha = (stp->qds & 0xC1) | (stp->vti & 0x80) ; // testa IV, NT e OV e transitório
          }
          break;

      case 9:  // normalized
      case 34: // normalized c/ tag
      case 11: // scaled
      case 35: // scaled c/ tag
          {
          analogico_seq * ana = (analogico_seq *) ptinfo;
          double div;
          if ( tipo == 9 || tipo == 34 )
            div = 32767;
          else
            div = 1;
          val = ana->sva / div;
          qual.CasaDecimal1 = 0;
          qual.CasaDecimal2 = 0;
          qual.Subst = ana->qds & 0x30; // testa BL e SB
          qual.Origem1 = 0; // origem: supervisionado = 00
          qual.Origem2 = 0;
          qual.Quest = 0;
          qual.Tipo = TIPO_ANALOGICO; // 0=digital, 1=analógico
          qual.Falha = ana->qds & 0xC1; // testa IV, NT e OV
          }
          break;
      case 15: // integrated totals
          {
          integrated_seq * integr = (integrated_seq *)ptinfo;
          val = integr->bcr;
          qual.CasaDecimal1 = 0;
          qual.CasaDecimal2 = 0;
          qual.Subst = 0;
          qual.Origem1 = 0; // origem: supervisionado = 00
          qual.Origem2 = 0;
          qual.Quest = 0;
          qual.Tipo = TIPO_ANALOGICO; // 0=digital, 1=analógico
          qual.Falha = integr->qds & 0xC0; // testa IV, leave CA and CY
          }
          break;
      case 13: // ponto flutuante
      case 36: // ponto flutuante c/ tag
          {
          flutuante_seq * flut = (flutuante_seq *) ptinfo;

          /*
          // do not update non-numeric values
          feclearexcept(FE_ALL_EXCEPT);
          val = flut->fr;
          if( fetestexcept(FE_INVALID) ||
              fetestexcept(FE_OVERFLOW) ||
              fetestexcept(FE_DIVBYZERO)
            )
            {
            Loga( (String)"Invalid float format for point: " + (String)nponto + (String)" type: " + tipo );
            return -6;
            }
          */
/*
          val = flut->fr;
          if( _isnan(val) || !_finite(val) )
            {
            Loga( (String)"Invalid float format for point: " + (String)nponto + (String)" type: " + tipo );
            return -6;
            }

          switch ( _fpclass(val) )
            {
            // do not update non-numeric values
            case _FPCLASS_UNSUP: // s = "Unsupported IEEE format"
            case _FPCLASS_SNAN:  // s = "Signaling NaN";
            case _FPCLASS_QNAN:  // s = "Quiet NaN";
            case _FPCLASS_NINF:  // s = "Negative infinity (-INF)";
            case _FPCLASS_PINF:  // s = "Positive infinity (+INF)";
              Loga( (String)"Invalid float format for point: " + (String)nponto + (String)" type: " + tipo );
              return -6;

            // acceptable classes
            //case _FPCLASS_NN:   s = "Negative normalized non-zero";
            //case _FPCLASS_ND:   s = "Negative denormalized";
            //case _FPCLASS_NZ:   s = "Negative zero (-0)";
            //case _FPCLASS_PZ:   s = "Positive 0 (+0)";
            //case _FPCLASS_PD:   s = "Positive denormalized";
            //case _FPCLASS_PN:   s = "Positive normalized non-zero";
            }
*/
          val = flut->fr;
          qual.CasaDecimal1 = 0;
          qual.CasaDecimal2 = 0;
          qual.Subst = flut->qds & 0x30; // testa BL e SB
          qual.Origem1 = 0; // origem: supervisionado = 00
          qual.Origem2 = 0;
          qual.Quest = 0;
          qual.Tipo = TIPO_ANALOGICO; // 0=digital, 1=analógico
          qual.Falha = flut->qds & 0xC1; // testa IV, NT e OV
          }
          break;

      case 151: // 16 bit bitstring (user reserved ASDU)
          {
          analogico_seq * ana = (analogico_seq *) ptinfo;
          short intval = ana->sva;
          for ( int i=0; i<16; i++ )
            {
            TPonto &pto = BL.GetRefPonto( nponto, found );
            if (found && pto.TipoAD == 'D')
              {
              if (intval & 0x0001)
                qual.Byte = 0x02;
              else
                qual.Byte = 0x01;
              EscrevePonto( nponto, qual.Estado, qual.Byte, 0 , 1, temtag, causa == 3 || causa == 11 || causa == 12 );
              }
            else
              {
              return 0;
              }
            intval = intval >> 1;
            nponto++;
            }
          fmVeDados->PulseI104(); // pulse IEC 104 LED
          }
          return 0;
      // retorno (ack/nack) dos comandos

      // digitals
      case 45: // comando simples
      case 46: // comando duplo
      case 47: // comando regulação
      case 58: // comando simples c/ tag
      case 59: // comando duplo c/ tag
      case 60: // comando regulação c/ tag
          SetAckCmd( nponto, causa & 0x40 ? 1 : 0, (double)(*((unsigned char *)ptinfo) & ESTDUP_MASCARA) );
          return -4;
      // analogs
      case 48:
      case 49:
      case 50:
      case 61:
      case 62:
      case 63:
          if (taminfo == 4)
            SetAckCmd( nponto, causa & 0x40 ? 1 : 0, *((float *)ptinfo) );
          if (taminfo == 8)
            SetAckCmd( nponto, causa & 0x40 ? 1 : 0, *((double *)ptinfo) );
          return -4;
      // bitstring    
      case 51:
      case 64:
          if (taminfo == 4)
            SetAckCmd( nponto, causa & 0x40 ? 1 : 0, *((unsigned int *)ptinfo) );
          return -4;
      default: // tipo desconhecido
          return -5;
      }

  EscrevePonto( nponto, val, qual.Byte, 0 , 1, temtag, causa == 3 || causa == 11 || causa == 12 );
  }
catch ( Exception &E )
  {
  if (tipo == 13 || tipo == 36)
    {
    // pt.Valor = 0;
    // pt.sValor = 0;
    }
  pt.Qual.Falha = 1;
  Loga( E.Message + (String)" | Exception for point: " + (String)nponto + (String)" type: " + tipo);
  return -4;
  }

fmVeDados->PulseI104(); // pulse IEC 104 LED
return 0;
}

//------------------------------------------------------------------------------------------------------------------

