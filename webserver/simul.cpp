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

#include <math.hpp>
#include "bdtr_com.h"
#include "simul.h"
#include "bcolocal.h"
#include "vedados.h"
#include "eventos_u.h"
#include "config.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmSimul *fmSimul;
//---------------------------------------------------------------------------
__fastcall TfmSimul::TfmSimul(TComponent* Owner)
        : TForm(Owner)
{

}

//---------------------------------------------------------------------------
void __fastcall TfmSimul::btEscreveClick(TObject *Sender)
{
    int nponto, posvirg;
    TFA_Qual flags;
    float valor;
    bool ehevento = ( Sender == NULL );  // Sender==NULL indica evento

try {
    nponto = atoi(edNPonto->Text.c_str());

    posvirg = edValor->Text.Pos(",");
    if ( posvirg != 0 )
      {
      edValor->Text = edValor->Text.Delete(posvirg,1);
      edValor->Text = edValor->Text.Insert(".",posvirg);
      }

    valor  = atof(edValor->Text.c_str());
    flags.Byte  = strtol(edFlags->Text.c_str(), NULL, 0);

    lbErr->Caption = "";

    if ( nponto == 0 )
      {
      lbErr->Caption = "Número de ponto inválido!";
      }
    else
      {
      if ( BL.GetTipo(nponto) == 'D' )
        { // digital
        valor = flags.Estado;
        if ( flags.Tipo != 0 )
          {
          lbErr->Caption = "Forcing digital flags, as point is digital!";
          flags.Tipo = 0;
          }
        }
      else
        { // analógico
        if ( flags.Tipo != 1 )
          {
          lbErr->Caption = "Forcing analog flags, as point is analog!";
          flags.Tipo = 1;
          }
        BL.SetValorTipico(nponto, valor); // guarda como valor típico para simulação
        }

      if (BL.GetTipo(nponto)=='D' && ehevento) // inclui evento se for digital e Sender==NULL
        {
        IncluiEvento( nponto,
                      SDE_UTREVSIMUL,
                      flags.Byte,
                      0,
                      SDE_GERAHORAMS, // data/hora atual
                      0,
                      0,
                      0,
                      0,
                      0);
        BL.EscrevePonto(nponto, valor, flags.Byte, 0, 0, 1, 1); // avisa que tem tag de tempo
        }
      else
        {
        BL.EscrevePonto(nponto, valor, flags.Byte, 0, 0, 0, 1);
        }

      // se for simulação mestre, vou mandar para os escravos
      if ( BL.GetSimulacao() == SIMULMOD_MESTRE )
        {
        if ( BL.GetTipo(nponto) == 'D' )
          {
          if ( ehevento )
            {
            msg_dig_tag ms;
            ms.COD = T_DIG_TAG|T_CONV;
            ms.NRPT = 1;
            ms.ORIG = 0;
            fmBDTR->TagBDTR_HoraAtual(&ms.PONTO[0].TAG);
            ms.PONTO[0].ID = nponto;
            ms.PONTO[0].UTR = 0;
            ms.PONTO[0].STAT = flags.Byte;
            fmBDTR->EnviaEscravosBDTR((char *)&ms, sizeof(ms) );
            }
          else
            {
            msg_dig ms;
            ms.COD = T_DIG|T_CONV;
            ms.NRPT = 1;
            ms.ORIG = 0;
            ms.PONTO[0].ID = nponto;
            ms.PONTO[0].STAT = flags.Byte;
            fmBDTR->EnviaEscravosBDTR((char *)&ms, sizeof(ms) );
            }
          }
        else
          {
          msg_float ms;
          ms.COD = T_FLT|T_CONV;
          ms.NRPT = 1;
          ms.ORIG = 0;
          ms.PONTO[0].ID = nponto;
          ms.PONTO[0].VALOR = valor;
          ms.PONTO[0].STAT = flags.Byte;
          fmBDTR->EnviaEscravosBDTR((char *)&ms, sizeof(ms) );
          }
        }

      btBuscaClick(NULL);
      fmVeDados->edBusca->Text = nponto;
      fmVeDados->btBuscaPontoClick(NULL);
      }
    }
catch (Exception &E)
    {
    lbErr->Caption = E.Message;
    }

}
//---------------------------------------------------------------------------
void __fastcall TfmSimul::btBuscaClick(TObject *Sender)
{
int nponto;
float valor;
TFA_Qual flags;
double tagtempo;
char str[500];
bool ret;
String S;

nponto = atoi(edNPonto->Text.c_str());
if ( nponto != 0 )
  {
  ret = BL.GetPonto(nponto, valor, flags, tagtempo);
  if ( ret == true )
    { // achou
    if (Sender != NULL)
      lbErr->Caption = "";
    edValor->Text = edValor->Text.sprintf("%0.3f", valor);
    AcertaFlags( flags.Byte );
    S = S.sprintf("0x%02x", flags.Byte);
    if ( S != edFlags->Text )
      edFlags->Text = S;

    BL.GetTagPonto(nponto, str);
    lbID->Caption = str;
    BL.GetModDescrPonto(nponto, str);
    lbDesc->Caption = str;
    }
  else
    lbErr->Caption = "Point not found";
  }
else
  lbErr->Caption = "Invalid point number";
}
//---------------------------------------------------------------------------
void __fastcall TfmSimul::edNPontoEnter(TObject *Sender)
{
lbErr->Caption="";
}
//---------------------------------------------------------------------------
void __fastcall TfmSimul::edValorEnter(TObject *Sender)
{
lbErr->Caption="";
}
//---------------------------------------------------------------------------
void __fastcall TfmSimul::edFlagsEnter(TObject *Sender)
{
lbErr->Caption="";
}
//---------------------------------------------------------------------------
void __fastcall TfmSimul::cbSimulClick(TObject *Sender)
{
if ( cbSimul->Checked )
  {
  if ( !BL.HaSimulacao() ) // se já não está simulado, faz simulação local
    BL.SetSimulacao(SIMULMOD_LOCAL);
  }
else
  {
  BL.SetSimulacao(SIMULMOD_NAO); // desativa simulação
  }         
}
//---------------------------------------------------------------------------

void __fastcall TfmSimul::edFlagsChange(TObject *Sender)
{
unsigned char Byte;
char *end;

edFlags->Text=edFlags->Text.Trim();
unsigned long x=strtol(edFlags->Text.c_str(), &end, 0);

  if (x<256 /* && end >= (char *)edFlags->Text.c_str() + edFlags->Text.Length()-1*/ )
    {
    Byte=(unsigned char)x;
    AcertaFlags(Byte);
    lbErr->Caption="";
    }
  else
    {
    lbErr->Caption="Invalid qualifier (must be integer < 255, < 0xff )!";
    }
}
//---------------------------------------------------------------------------

void TfmSimul::AcertaFlags(unsigned char Byte)
{
bool b;
String S;
TFA_Qual flags;

flags.Byte = Byte;

  b = Byte & 0x80; if ( b != cbQBit7->Checked ) cbQBit7->Checked = b;
  b = Byte & 0x40; if ( b != cbQBit6->Checked ) cbQBit6->Checked = b;
  b = Byte & 0x20; if ( b != cbQBit5->Checked ) cbQBit5->Checked = b;
  b = Byte & 0x10; if ( b != cbQBit4->Checked ) cbQBit4->Checked = b;
  b = Byte & 0x08; if ( b != cbQBit3->Checked ) cbQBit3->Checked = b;
  b = Byte & 0x04; if ( b != cbQBit2->Checked ) cbQBit2->Checked = b;
  b = Byte & 0x02; if ( b != cbQBit1->Checked ) cbQBit1->Checked = b;
  b = Byte & 0x01; if ( b != cbQBit0->Checked ) cbQBit0->Checked = b;
  S = S.sprintf("0x%02x", Byte);
  if (S!=lbFlg->Caption)
     lbFlg->Caption=S;
//  S=S.sprintf("0x%02x", Byte);
//  if (S!=edFlags->Text)
//    edFlags->Text=S;

  if ( flags.Falha )
    lbFalha->Caption = "Failed value";
  else
    lbFalha->Caption = "Value OK";

  if ( flags.Subst )
    lbImposto->Caption = "Imposed";
  else
    lbImposto->Caption = "Not imposed";

  if ( flags.Origem == ORIGEM_SUPERVISIONADO )
    lbOrigem->Caption = "Supervised";
  if ( flags.Origem == ORIGEM_CALCULADO )
    lbOrigem->Caption = "Calculated";
  if ( flags.Origem == ORIGEM_CARGAINI )
    lbOrigem->Caption = "Not Updated";
  if ( flags.Origem == ORIGEM_MANUAL )
    lbOrigem->Caption = "Manual";

  if ( flags.Tipo == TIPO_ANALOGICO )
    {
    lbTipo->Caption = "Analog Point";
    if ( flags.Quest )
      lbQuest->Caption = "Imprecise time tag";
    else
      lbQuest->Caption = "Normal time tag";
    lbEstado->Caption = (String)(unsigned short)flags.CasaDecimal + (String)" Decimal places";
    }
  else
    {
    lbTipo->Caption = "Digital Point";
    if ( flags.Duplo == ESTDUP_INDETERMINADO )
      lbEstado->Caption = "Transit state";
    if ( flags.Duplo == ESTDUP_OFF )
      lbEstado->Caption="OFF State";
    if ( flags.Duplo == ESTDUP_ON )
      lbEstado->Caption = "ON State";
    if ( flags.Duplo == ESTDUP_INVALIDO )
      lbEstado->Caption = "Invalid State";
    if ( flags.Quest)
      lbQuest->Caption = "Imprecise time tag";
    else
      lbQuest->Caption = "Normal time tag";
    }
}

void __fastcall TfmSimul::cbQBit7Click(TObject *Sender)
{
unsigned char Byte = 0;
int Sel;
String S;
  Byte |= cbQBit7->Checked ?0x80 : 0x00;
  Byte |= cbQBit6->Checked ?0x40 : 0x00;
  Byte |= cbQBit5->Checked ?0x20 : 0x00;
  Byte |= cbQBit4->Checked ?0x10 : 0x00;
  Byte |= cbQBit3->Checked ?0x08 : 0x00;
  Byte |= cbQBit2->Checked ?0x04 : 0x00;
  Byte |= cbQBit1->Checked ?0x02 : 0x00;
  Byte |= cbQBit0->Checked ?0x01 : 0x00;

  S = S.sprintf("0x%02x", Byte);
  if ( S != lbFlg->Caption )
    {
    Sel = edFlags->SelStart;
    lbFlg->Caption = S;
    edFlags->Text = edFlags->Text.Trim();
    if ( edFlags->Text.SubString(1,2) == "0x" )
      edFlags->Text = edFlags->Text.sprintf("0x%02x", Byte);
    else
      edFlags->Text = edFlags->Text.sprintf("%2d", Byte);
    edFlags->SelStart = Sel;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfmSimul::btEventoClick(TObject *Sender)
{
if ( BL.GetTipo(atoi(edNPonto->Text.c_str())) == "D" )
   {
   btEscreveClick(NULL); // o NULL vai indicar para dar evento
   }
}
//---------------------------------------------------------------------------

void __fastcall TfmSimul::FormCreate(TObject *Sender)
{
if ( SIMULACAO )
  {
  BL.SetSimulacao(SIMULACAO);
  cbSimul->Checked = true;
  }
}
//---------------------------------------------------------------------------

#pragma warn -aus
// processa a simulação de comando, resultado no ponto supervisionado associado
int TfmSimul::SimulaComando(int nponto, float valor)
{
// só simula se for modo mestre ou local
if ( ( BL.GetSimulacao()!=SIMULMOD_MESTRE ) && ( BL.GetSimulacao()!=SIMULMOD_LOCAL ) )
  return 1;

int nptsup = 0;

bool found;
TPonto &ptcmd = BL.GetRefPonto(nponto, found);
if ( !found )
  return 1;
nptsup = ptcmd.PontoSupCmd;

TPonto &ptsup = BL.GetRefPonto(nptsup, found);
if ( !found )
  return 1;

if ( nptsup != 0 )
  {
   // se for mestre, insere evento do comando, e mando para os escravos
   if ( BL.GetSimulacao() == SIMULMOD_MESTRE )
     {
      IncluiEvento(
        nponto,
        SDE_UTREVSIMUL, // identifica UTR simulacao
        ((unsigned char)valor) | QUAL_FALHATAG,
        0,
        SDE_GERAHORAMS, // data/hora atual
        0,
        0,
        0,
        0,
        0);

      // manda evento do comando
      msg_dig_tag ms;
      ms.COD = T_DIG_TAG|T_CONV;
      ms.NRPT = 1;
      ms.ORIG = 0;
      fmBDTR->TagBDTR_HoraAtual(&ms.PONTO[0].TAG);
      ms.PONTO[0].ID = nponto;
      ms.PONTO[0].UTR = 0;
      ms.PONTO[0].STAT = ((unsigned char)valor) | QUAL_FALHATAG;
      fmBDTR->EnviaEscravosBDTR((char *)&ms, sizeof(ms) );
     }

  TFA_Qual flags;
  flags.Byte = 0;
  if ( ptsup.TipoAD == 'D' )
    {
    flags.Duplo = valor;
    valor = flags.Estado;  // prepara para escrever o valor em formato A
    }
  else
    { // analógico

    // ponto é analógico, mas comando é digital, então é do tipo sobe/desce
    if ( ptcmd.EhComandoDigital() )
      {
      // simula como sobe/desce tap
      if ( valor == 2 )
        valor = ptsup.Valor + 1;
      else
        valor = ptsup.Valor - 1;
      }
    else
      valor = valor; // vai escrever no ponto supervisionado o valor recebido no comando

    flags.Tipo = TIPO_ANALOGICO;
    }

  if ( flags.Tipo == TIPO_ANALOGICO || valor != ptsup.Valor )
    { // só repercute no ponto supervisionado se for analógico (ex.tap), ou vai mudar de estado

    if ( flags.Tipo == TIPO_DIGITAL )  // para o ponto digital, gera o evento
      {
      IncluiEvento(
        nptsup,
        SDE_UTREVSIMUL, // identifica UTR simulacao
        flags.Byte,
        0,
        SDE_GERAHORAMS, // data/hora atual
        0,
        0,
        0,
        0,
        0);

       // se estiver no modo de simulação mestre, manda o valor simulado aos escravos
       if ( BL.GetSimulacao() == SIMULMOD_MESTRE )
        {
        msg_dig_tag ms;
        ms.COD = T_DIG_TAG|T_CONV;
        ms.NRPT = 1;
        ms.ORIG = 0;
        fmBDTR->TagBDTR_HoraAtual(&ms.PONTO[0].TAG);
        ms.PONTO[0].ID = nptsup;
        ms.PONTO[0].UTR = 0;
        ms.PONTO[0].STAT = flags.Byte;
        fmBDTR->EnviaEscravosBDTR((char *)&ms, sizeof(ms) );
        }
      }
    else
      {
       // se estiver no modo de simulação mestre, manda o valor simulado aos escravos
       if ( BL.GetSimulacao() == SIMULMOD_MESTRE )
        {
        msg_float ms;
        ms.COD = T_FLT|T_CONV;
        ms.NRPT = 1;
        ms.ORIG = 0;
        ms.PONTO[0].ID = nptsup;
        ms.PONTO[0].VALOR = valor;
        ms.PONTO[0].STAT = flags.Byte;
        fmBDTR->EnviaEscravosBDTR((char *)&ms, sizeof(ms) );
        }
      }

    BL.EscrevePonto( nptsup, valor, flags.Byte, 0, 0, 1 );
    }
  return 0; // ok
  }

return 1; // erro
}

#define MXANA 150
#pragma warn -aus
void TfmSimul::ProcessaSimulacao(void)
{
if ( (BL.GetSimulacao() != SIMULMOD_LOCAL) && (BL.GetSimulacao() != SIMULMOD_MESTRE) )
  return;

static int first=1;
map <int, TPonto> &PontosTR = BL.GetMapaPontos();
map <int, TPonto>::iterator it;
float valor;
int falha;
float val;
TFA_Qual flg;
double tagtempo;
int cntev = 0;
int nptdj = 0;
int forca = 0;

static int cntsimdig = 0;
cntsimdig++; // para gerar menos eventos digitais

// prepara mensagem analógica
int cntana = 0;
char bufana[2000];
msg_float *pana;
pana = (msg_float *) bufana;
pana->COD = T_FLT|T_CONV;
pana->ORIG = 0;

for ( it = PontosTR.begin(); it != PontosTR.end(); it++ )
  {
  // não interfere nos pontos internos da IHM
  if ( ((*it).first) > NPONTO_SIST_MIN && ((*it).first) < NPONTO_SIST_MAX )
    continue;

  forca=0;
  if ( ((*it).second).CodOrigem == 0 ) // supervisionado?
    {
    if ( ((*it).second).TipoAD == 'A' )
      {
      switch ( ((*it).second).CodTpEq )
        {
        case CODTPEQ_KV:
          valor = ((*it).second).ValorTipico + RandG(0, 2);
          break;
        case CODTPEQ_AMP:
          valor = ((*it).second).ValorTipico + RandG(0, 20);
          break;
        case CODTPEQ_MW:
          valor = ((*it).second).ValorTipico + RandG(0, 5);
          break;
        case CODTPEQ_MVAR:
          valor = ((*it).second).ValorTipico + RandG(0, 2);
          break;
        case CODTPEQ_HZ:
          valor = ((*it).second).ValorTipico + RandG(0, 0.01);
          break;
        case CODTPEQ_VDC:
          valor = ((*it).second).ValorTipico + RandG(0, 2);
          break;
        case CODTPEQ_TAP:
          if ( first ) // no início pega o típico, depois, mantém o valor
            valor = int( ((*it).second).ValorTipico );
          else
            valor = ((*it).second).Valor;
          break;
        default:
          valor = ((*it).second).ValorTipico;
          break;
        }

      switch (((*it).second).CodTpEq)
        {
        case CODTPEQ_KV:
        case CODTPEQ_AMP:
        case CODTPEQ_MW:
        case CODTPEQ_MVAR:
        case CODTPEQ_HZ:
          nptdj = BL.DisjModulo[ (String)((*it).second).Estacao + (String)((*it).second).Modulo ];
          if ( nptdj != 0 )
            {
            BL.GetPonto( nptdj, val, flg, tagtempo );
            if ( val == 1 )  // aberto?
              valor = 0;
            }
          break;
        }

      if ( int(RandG(8, 5)) == 12 )
        falha = QUAL_FALHA;
      else
        falha = 0x00;

      // se o valor vai mudar para zero ou sair de zero, forca
      if ( ((*it).second).Valor == 0 && valor != 0 || ((*it).second).Valor != 0 && valor == 0 )
        forca = 1;

      if ( int(RandG(8, 3)) == 8 || first || forca )
        {
        BL.EscrevePonto(((*it).first), valor, QUAL_TIPO|falha, 0);
        if ( BL.GetSimulacao() == SIMULMOD_MESTRE ) // em modo mestre, não simula digitais
          {
          pana->PONTO[cntana].ID = ((*it).first);
          pana->PONTO[cntana].VALOR = valor;
          pana->PONTO[cntana].STAT = QUAL_TIPO|falha;
          cntana++;
          if ( cntana >= MXANA )
            {
            pana->NRPT = cntana;
            fmBDTR->EnviaEscravosBDTR((char*)pana, sizeof(msg_float)+sizeof(A_float)*(cntana-1));
            cntana = 0;
            }
          }
        }
      } // analógicos
    else
      { // digitais
      if ( cntev >= 1 )
        continue;

      int cmp;

      if ( ((*it).second).CodTpEq == CODTPEQ_DJ && ((*it).second).CodInfo == 0 )
        cmp = 15;
      else
        cmp = 17;

      if ( int(RandG(8, 5)) == cmp || first )
        {
        TFA_Qual flags;
        flags.Byte = 0;
        switch ( int(RandG(2, 2)) )
          {
          default:
          case 2: flags.Duplo = 2; break;
          case 3: flags.Duplo = 1; break;
          case -1: flags.Duplo = 0; break;
          case 6: flags.Duplo = 3; break;
          }

        valor = flags.Estado;
        if ( int(RandG(8, 5)) == 14 )
          flags.Falha = 1;
        else
          flags.Falha = 0;

        BL.GetPonto( ((*it).first), val, flg, tagtempo );

        if ( first )
          { // na primeira só atualiza valores
          if ( ((*it).second).ValorTipico == 0 )
            {
            flg.Duplo = 2;
            val = 0;
            }
          else
            {
            flg.Duplo=1;
            val=1;
            }

          flg.Falha=0;
          BL.EscrevePonto(((*it).first), val, flg.Byte, 0);
          }
        else
          {
          if ( BL.GetSimulacao() == SIMULMOD_MESTRE ) // em modo mestre, não simula eventos digitais
            continue;

          if ( cntsimdig%5 ) // para gerar menos eventos
            continue;

          if (((*it).second).EhComando()) // não gera evento em ponto de comando
            continue;

          if (((*it).second).PontoCmdSup!=0) // não gera evento onde ponto supervisionado tem comando
            continue;

          // para DJ e SC não altera valores
          if ( ((*it).second).CodTpEq==CODTPEQ_DJ && ((*it).second).CodInfo==0 ||
               ((*it).second).CodTpEq==CODTPEQ_SC && ((*it).second).CodInfo==0
             )
            continue;

          if (flg.Byte != flags.Byte)
            {
            cntev++;
            IncluiEvento( ((*it).first),
                  SDE_UTREVSIMUL, // identifica UTR simulacao
                  flags.Byte,
                  0,
                  SDE_GERAHORAMS, // data/hora atual
                  0,
                  0,
                  0,
                  0,
                  0);
            BL.EscrevePonto(((*it).first), valor, flags.Byte, 0, 0, 1);
            }
          }
        }  // rand
      } // digitais
    } // supervisionados
  }

// vê se falta mandar algum dado
if ( cntana > 0 )
  {
  pana->NRPT = cntana;
  fmBDTR->EnviaEscravosBDTR( (char*)pana, sizeof(msg_float)+sizeof(A_float)*(cntana - 1) );
  }

// após a primeira simulação, manda integridade
if ( BL.GetSimulacao() == SIMULMOD_MESTRE && first )
  fmBDTR->EnviaIntegridadeEscravosBDTR();

first=0;
}

void __fastcall TfmSimul::Timer1Timer(TObject *Sender)
{
lbModo->Caption = BL.GetSimulacao();
if ( BL.HaSimulacao() )
  ProcessaSimulacao();
}
//---------------------------------------------------------------------------


