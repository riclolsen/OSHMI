//---------------------------------------------------------------------------
// BDTR.CPP
// Comunicação com o BDTR do Sandro.
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
#include <time.h>
#include "config.h"
#include "bdtr_com.h"
#include "bcolocal.h"
#include "bdtr.h"
#include "eventos_u.h"
#include "vedados.h"
#include "simul.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IdBaseComponent"
#pragma link "IdComponent"
#pragma link "IdUDPBase"
#pragma link "IdUDPServer"
#pragma link "IdMappedPortUDP"
#pragma link "IdAntiFreeze"
#pragma link "IdAntiFreezeBase"
#pragma resource "*.dfm"
TfmBDTR *fmBDTR;

void MyBDTR::CommandIndication(int endcmd, char flags, float valor)
{
/*
// converte endereço físico para nponto
int nptcmd = BL.NPontoPorEndUTR( endcmd, 0 );
if ( nptcmd == 0 )
  return;

// se estiver no modo simulação mestre, simula o comando
if ( BL.GetSimulacao() == SIMULMOD_MESTRE )
  {
  // envia ack do comando
  msg_ack ma;
  ma.COD = T_ACK;
  ma.TIPO = T_COM;
  ma.ID = valor;
  ma.COMP = nptcmd;
  fmBDTR->EnviaEscravosBDTR( (char *)&ma, sizeof(ma) );

  // manda evento do comando
  msg_dig_tag ms;
  ms.COD = T_DIG_TAG|T_CONV;
  ms.NRPT = 1;
  ms.ORIG = 0;
  fmBDTR->TagBDTR_HoraAtual(&ms.PONTO[0].TAG);
  ms.PONTO[0].ID = nptcmd;
  ms.PONTO[0].UTR = 0;
  ms.PONTO[0].STAT = ((unsigned char) valor) | QUAL_FALHATAG;
  fmBDTR->EnviaEscravosBDTR( (char *)&ms, sizeof(ms) );

  // simula o comando
  fmSimul->SimulaComando(nptcmd, valor);
  }
*/
}

void MyBDTR::envia_udp(char * IP, int porta, char * buf, int sz)
{
fmBDTR->IdUDPServer1->SendBuffer( IP, porta, buf, sz );
}

void MyBDTR::set_udp_timeout(int timeout)
{
fmBDTR->IdUDPServer1->ReceiveTimeout=timeout;
}

int MyBDTR::le_udp(char * buf, int szmax)
{
int bytesread = fmBDTR->UDPAData->Read(buf, szmax);
memcpy(lastMsgRcv, buf, bytesread);
szLastMsgRcv = bytesread;
return bytesread;
}

void MyBDTR::logaln(string msg)
{
if ( Logar )
  {
  if ( fmBDTR->mmBDTR->Lines->Count > 1500 )
   for ( int i = 0; i < 50; i++ )
      fmBDTR->mmBDTR->Lines->Delete(0);

  fmBDTR->mmBDTR->Lines->Add( msg.c_str() );
  }
}

int MyBDTR::SetAckCmd(int EndPonto, int falha, int cmd) // seta ack de comando
{
// converte endereço físico para nponto
int nponto = BL.NPontoPorEndUTR( EndPonto, 0 );
if ( nponto == 0 )
  return 0;

return BL.SetAckCmd(nponto, falha, cmd);
}                                

int MyBDTR::IncluiEvento( unsigned int EndPonto,
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
fmVeDados->PulseSDE( clRed );

// converte endereço físico para nponto
int nponto = BL.NPontoPorEndUTR( EndPonto, 0 );
if ( nponto == 0 )
  return 0;

if ( !BL.GetAlrIn( nponto ) ) // só registra se não estiver inibido o alarme
  return ::IncluiEvento( nponto, UTR, Flags, Ano, Mes, Dia, Hora, Minuto, Segundo, Milisegundo );

return 1;  
}

void MyBDTR::RecebeuIntegridade()
{
BL.RecebeuIntegridade();
}

void MyBDTR::RecebeuHora(msg_sinc *m)
{
char linha[5000] = "";

      SYSTEMTIME st; // estrutura de data/hora do windows

      if (m->ORIG==ORIG_DUAL && IHMRED_IP_OUTRO_IHM!="") // chegou keep-alive do outro IHM: fico na reserva
        {
        if (IHM_PRINCIPAL>0) // se estava principal, marca que foi para reserva
          {
          ::IncluiEvento( NPONTO_OSHMI_MODO,
              255,
              0x01,
              0,
              SDE_GERAHORA,
              0,
              0,
              0,
              0,
              0 );
          EscrevePonto(NPONTO_OSHMI_MODO, 1, 0x01);
          }

        IHM_PRINCIPAL=0; // sou reserva
        CNT_SEG_SEMKA=0; // resseta contagem de tempo do keep-alive
        fmVeDados->PulseBDTR( clGreen );
        }

      if ( (BDTR_ACERTO_HORA && m->ORIG!=ORIG_DUAL) || (m->ORIG==ORIG_DUAL && IHMRED_ACERTO_HORA) ) // se aceita acerto de hora
        { // tenta acertar a hora do micro
        st.wYear=m->TAG.ANO;
        st.wMonth=m->TAG.MES;
        st.wDay=m->TAG.DIA&0x1F; // cuidado o dia do mes vem nos 5 primeiros bits
        st.wDayOfWeek==(m->TAG.DIA>>5)+1; // este parâmetro é ignorado em SetSystemTime
        st.wHour=m->TAG.HORA;
        st.wMinute=m->TAG.MINUTO;
        st.wSecond=m->TAG.MSEGS/1000;
        st.wMilliseconds=m->TAG.MSEGS%1000;

        // vou ver a hora atual e só vou acertar se tiver mais que 1 s de diferença
        TDateTime stAgora=Now();
        TDateTime dtData(st.wYear,st.wMonth,st.wDay);
        TDateTime dtHora(st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
        TDateTime dtRec=dtData+dtHora;

        // 24h=1, 1s=1/(60*60*24)
        if ( fabs((double)dtRec - (double)stAgora) > (double)1.0/(86400) )
          {

          if ( !SetLocalTime(&st) )
             { // erro, não conseguiu ajustar a hora
             msg_status="Não foi possível acertar a hora do sistema.";
             sprintf(linha, "Não foi possível acertar a hora do sistema.");
             if (Logar)
                logaln(linha);
             }
          else
             { // ajustou a hora com sucesso
             ::IncluiEvento( NPONTO_ACERTO_HORA,
                           255,
                           0x02,
                           st.wYear,
                           st.wMonth,
                           st.wDay,
                           st.wHour,
                           st.wMinute,
                           st.wSecond,
                           st.wMilliseconds
                         );
             EscrevePonto(NPONTO_ACERTO_HORA, 0, 0x02);

             sprintf(linha, "Hora do sistema ajustada com sucesso!");
             if (Logar)
                logaln(linha);
             }
          }
        else
          {
             sprintf(linha, "Hora recebida, mas não foi necessário acertar!");
             if (Logar)
                logaln(linha);
           }
        }

      // loga a hora do sistema
      if (Logar)
        {
        GetLocalTime(&st);
        sprintf(linha, "Hora Sistema : %02d/%02d/%04d %02d:%02d:%02d,%03d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        logaln(linha);
        }
}

//---------------------------------------------------------------------------
__fastcall TfmBDTR::TfmBDTR(TComponent* Owner)
        : TForm(Owner)
{
bdtr.Logar = 0;
bdtr.IP_BDTR1 = IP_BDTR1.c_str();
bdtr.IP_BDTR2 = IP_BDTR2.c_str();
bdtr.MEU_END = END_IHM;
bdtr.BDTR_PORTA = BDTR_PORTA;
bdtr.BDTR_PORTA_CMD = BDTR_PORTA_CMD;
bdtr.BDTR_GRUPO_INTEGRIDADE = BDTR_GRUPO_INTEGRIDADE;
bdtr.CNT_SEG_SEMKA = 0;
bdtr.BDTR_ENVIA_COMANDOS = ENVIA_COMANDOS;

IncluiEvento( NPONTO_OSHMI_MODO,
              255,
              0x02,
              0,
              SDE_GERAHORA,
              0,
              0,
              0,
              0,
              0 );

if ( IHMRED_IP_OUTRO_IHM == "" ) // se não tem outro IHM, assume principal
  {
  bdtr.IHM_PRINCIPAL = 1;
  IncluiEvento( NPONTO_OSHMI_MODO,
                255,
                0x02,
                0,
                SDE_GERAHORA,
                0,
                0,
                0,
                0,
                0 );
  BL.EscrevePonto(NPONTO_OSHMI_MODO, 0, 0x02);
  }
else
  {
  bdtr.IHM_PRINCIPAL = 0;
  IncluiEvento( NPONTO_OSHMI_MODO,
                255,
                0x01,
                0,
                SDE_GERAHORA,
                0,
                0,
                0,
                0,
                0 );
  BL.EscrevePonto(NPONTO_OSHMI_MODO, 1, 0x01);
  }

fmBDTR->IdUDPServer1->ReceiveTimeout = 0;

if ( IP_BDTR1 != "" || IHMRED_IP_OUTRO_IHM != "" )
  fmBDTR->IdUDPServer1->Active = true;
else
  fmBDTR->IdUDPServer1->Active = false;

// atualiza o ponto de operação local
//if ( PONTO_OPERACAO != 0 )
//  {
//  BL.EscrevePonto(PONTO_OPERACAO, FA_ESTFASIM_ON, ESTDUP_ON);
//  // envia estado ON aos BDTR e ao outro IHM, se existirem
//  bdtr.EnviaPontoDig(PONTO_OPERACAO, ESTDUP_ON, bdtr.IP_BDTR1, BDTR_PORTA_CMD);
//  bdtr.EnviaPontoDig(PONTO_OPERACAO, ESTDUP_ON, bdtr.IP_BDTR2, BDTR_PORTA_CMD);
//  bdtr.EnviaPontoDig(PONTO_OPERACAO, ESTDUP_ON, IHMRED_IP_OUTRO_IHM.c_str(), BDTR_PORTA);
//  }

// prepara as primeira
Timer1->Interval = 10000;
Timer1->Enabled = true;
}
//---------------------------------------------------------------------------

// Tratamento da integridade
// Força uma no inicio (10s) mesmo se reserva e depois segue pelo tempo configurado
void __fastcall TfmBDTR::Timer1Timer(TObject *Sender)
{
//  bdtr.Sol_Integridade(BDTR::integridade_forcada); // faz a primeira integridade, forcada (mesmo se reserva)
//  bdtr.SetaPeriodoIntegridade(T_INTEGRIDADE);
  Timer1->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfmBDTR::Timer2Timer(TObject *Sender)
{
static unsigned int cntsec = 0;

if ( IHM_EstaFinalizando() ) return;

cntsec++;

bdtr.ontimerseg();

/*
// vê se passou 1 minuto desde a última chamada
if ( !(cntsec % 60) )
  {
  // envia estado operacional do IHM
  if ( PONTO_OPERACAO != 0 )
    { // envia estado ON aos BDTR e ao outro IHM, se existirem
    bdtr.EnviaPontoDig(PONTO_OPERACAO, ESTDUP_ON, bdtr.IP_BDTR1, BDTR_PORTA_CMD);
    bdtr.EnviaPontoDig(PONTO_OPERACAO, ESTDUP_ON, bdtr.IP_BDTR2, BDTR_PORTA_CMD);
    bdtr.EnviaPontoDig(PONTO_OPERACAO, ESTDUP_ON, IHMRED_IP_OUTRO_IHM.c_str(), BDTR_PORTA);
    }
  }
*/

// envio periódico de acerto de hora ao ihm redundante
if ( bdtr.EhPrincipal() )
if ( IHMRED_PERIODO_ENVIO_ACERTO_HORA && IHMRED_IP_OUTRO_IHM != "" )
if ( ! (cntsec % IHMRED_PERIODO_ENVIO_ACERTO_HORA) )
  bdtr.EnviaHora(IHMRED_IP_OUTRO_IHM.c_str(), bdtr.ORIG_DUAL); // aviso ao outro

/*
// envio periódico de acerto de hora ao BDTR
if ( BDTR_PERIODO_ENVIO_ACERTO_HORA > 0 )
  if ( ! (cntsec % BDTR_PERIODO_ENVIO_ACERTO_HORA) )
    { // envia hora aos 2 BDTR's
    bdtr.EnviaHora(bdtr.IP_BDTR1, END_IHM);
    bdtr.EnviaHora(bdtr.IP_BDTR2, END_IHM);

    // se for simulação mestre, manda a hora para os escravos
    if ( BL.GetSimulacao() == SIMULMOD_MESTRE )
      EnviaHoraEscravosBDTR();
    }

if ( BL.GetSimulacao() == SIMULMOD_MESTRE )
if ( !(cntsec % 300) )
  EnviaIntegridadeEscravosBDTR();
*/

// conta tempo sem keep-alive para ficar principal
bdtr.CNT_SEG_SEMKA++;

// asssume principal, se passar mais que 2x o tempo de keep-alive mais 2s
if ( bdtr.EhPrincipal()==0 )
if ( IHMRED_PERIODO_ENVIO_ACERTO_HORA && IHMRED_IP_OUTRO_IHM != "" )
if ( bdtr.CNT_SEG_SEMKA > (IHMRED_PERIODO_ENVIO_ACERTO_HORA * 2 + 2) )
  {
  bdtr.IHM_PRINCIPAL = 1; // fico principal
  bdtr.EnviaHora(IHMRED_IP_OUTRO_IHM.c_str(), bdtr.ORIG_DUAL); // aviso ao outro
  if ( bdtr.cntIniRecIntgr == 0 ) // se ainda não recebeu nenhuma integridade
    bdtr.Sol_Integridade(BDTR::integridade_normal); // faz integridade
  // marca que foi para principal
  IncluiEvento( NPONTO_OSHMI_MODO,
              255,
              0x02,
              0,
              SDE_GERAHORA,
              0,
              0,
              0,
              0,
              0 );
  BL.EscrevePonto(NPONTO_OSHMI_MODO, 0, 0x02);
  }

  if (fmBDTR->Visible)
    {
    fmBDTR->lbErro->Caption=bdtr.msg_status.c_str();
    lbCntMsg->Caption=bdtr.cntTot;
    lbCntDig->Caption=bdtr.cntDig;
    lbCntDigTag->Caption=bdtr.cntDigTag;
    lbCntAna->Caption=bdtr.cntAna;
    lbCntIniIntgr->Caption=bdtr.cntIniRecIntgr;
    lbCntFimIntgr->Caption=bdtr.cntFimRecIntgr;
    lbCntRst->Caption=bdtr.cntReset;
    lbCntSinc->Caption=bdtr.cntSinc;
    lbCntOutras->Caption=bdtr.cntOutras;
    lbCntErro->Caption=bdtr.cntErro;
    lbCntQEve->Caption=bdtr.cntQuitEve;
    lbCntAck->Caption=bdtr.cntAck;
    lbSolIntgr->Caption=bdtr.cntIntgr;
    lbErroParc->Caption=bdtr.cntErroIntgr;
    lbTmpGI->Caption=bdtr.SegundosParaGI;
    lbMsgCmd->Caption=bdtr.cntCmd;
    }
}
//---------------------------------------------------------------------------

void TfmBDTR::FinalizandoAplicacao()
{
/*
// envia estado operacional do IHM (saindo)
if ( PONTO_OPERACAO != 0 )
  { // envia estado ON aos BDTR e ao outro IHM, se existirem
  bdtr.EnviaPontoDig(PONTO_OPERACAO, ESTDUP_OFF, bdtr.IP_BDTR1, BDTR_PORTA_CMD);
  bdtr.EnviaPontoDig(PONTO_OPERACAO, ESTDUP_OFF, bdtr.IP_BDTR2, BDTR_PORTA_CMD);
  bdtr.EnviaPontoDig(PONTO_OPERACAO, ESTDUP_OFF, IHMRED_IP_OUTRO_IHM.c_str(), BDTR_PORTA);
  }
*/
}
//---------------------------------------------------------------------------

void __fastcall TfmBDTR::FormDestroy(TObject *Sender)
{
fmBDTR->IdUDPServer1->Active = false;
}
//---------------------------------------------------------------------------

// Interpreta as mensagens vindas do BDTR
void __fastcall TfmBDTR::IdUDPServer1UDPRead(TObject *Sender,
      TStream *AData, TIdSocketHandle *ABinding)
{
UDPAData = AData;

if (IHM_EstaFinalizando()) return;

// vê se o endereço é conhecido
if (
     //strcmp ( ABinding->PeerIP.c_str(), bdtr.IP_BDTR1.c_str() ) == 0 ||
     //strcmp ( ABinding->PeerIP.c_str(), bdtr.IP_BDTR2.c_str() ) == 0  ||
     //strcmp ( ABinding->PeerIP.c_str(), "127.0.0.1" ) == 0 ||
     strcmp ( ABinding->PeerIP.c_str(), IHMRED_IP_OUTRO_IHM.c_str() ) == 0 ||
     BL.HaSimulacao() // aceito os dados quando há simulação, podem estar vindo em broadcast, inclusive da própria máquina
   )
  {
  // em modo mestre, descarta se vier da própria máquina (o IP_BDTR1 deve ser configurado como o da própria máquina)
  if ( BL.GetSimulacao() == SIMULMOD_MESTRE )
  if ( strcmp ( ABinding->PeerIP.c_str(), bdtr.IP_BDTR1.c_str() ) == 0 )
     return;

  fmVeDados->PulseBDTR(clRed);

/*
  try
    {
    // sinaliza que a comunicação com o BDTR está ok
    if ( ABinding->PeerIP != IHMRED_IP_OUTRO_IHM )
      {
      // se estava falhada comunicação com bdtr e normalizou, dispara GI) testa quantas
      if (BL.GetRefPonto(NPONTO_COMUNIC_BDTR).Qual.Duplo==ESTDUP_OFF && bdtr.cntIntgr>1)
        {
        BL.EscrevePonto(NPONTO_COMUNIC_BDTR, FA_ESTFASIM_ON, ESTDUP_ON);
        bdtr.Sol_Integridade(BDTR::integridade_normal);
        }
      else
        BL.EscrevePonto(NPONTO_COMUNIC_BDTR, FA_ESTFASIM_ON, ESTDUP_ON);
      }
    }
  catch (char * p)
    {
    }
*/

  bdtr.recebe_udp(ABinding->PeerIP.c_str());
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmBDTR::cbMonitClick(TObject *Sender)
{
bdtr.Logar = cbMonit->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfmBDTR::cbLogaClick(TObject *Sender)
{
bdtr.LogaEmArq = cbLoga->Checked;
}
//---------------------------------------------------------------------------

void TfmBDTR::TagBDTR_HoraAtual(A_tag * ptag)
{
TDateTime dt = Now();
unsigned short year; unsigned short month; unsigned short day;
unsigned short hour; unsigned short min; unsigned short sec; unsigned short milisg;

  dt.DecodeDate(&year, &month, &day);
  dt.DecodeTime(&hour, &min, &sec, &milisg);

  ptag->ANO = year;
  ptag->MES = month;
  ptag->DIA = day;
  ptag->HORA = hour;
  ptag->MINUTO = min;
  ptag->MSEGS = sec * 1000 + (::GetTickCount() % 1000);
}



