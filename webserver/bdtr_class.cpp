//---------------------------------------------------------------------------
// BDTR_CLASS.CPP
// Comunicação com o BDTR do Sandro.
// OSHMI - Open Substation HMI
//	Copyright 2008-2018 - Ricardo L. Olsen
//---------------------------------------------------------------------------

#include <stdio.h>
#include <time.h>
#include <math.h>
#include "bdtr_class.h"

BDTR::BDTR()
{
    ORIG_DUAL=255;
    BDTR_ENVIA_COMANDOS=0;
    BDTR_GRUPO_INTEGRIDADE=0;
    PERIODO_INTEGRIDADE=300;
    DELAY_PARA_INTEGRIDADE_APOS_RESET=10;
    SegundosParaGI=0;

    Logar=0;
    LogaEmArq=0;

    IHM_PRINCIPAL = 0; // guarda estado do IHM Principal/Reserva

    EventosAQuitar = 0;
    cntQuitEve = 0;
    BDTR_IPQUITAR = "";

    cntDig = 0;
    cntDigTag = 0;
    cntAna = 0;
    cntTot = 0;
    cntSinc = 0;
    cntOutras = 0;
    cntReset = 0;
    cntErro = 0;
    cntAck = 0;
    cntCmd = 0;

    cntIntgr=0;   // contador de integridades: GI
    cntIniRecIntgr=0;
    cntFimRecIntgr=0;
    cntParcelasIntgr=0;
    IntgrEmAndamento=false;
    cntErroIntgr=0;
}            

BDTR::~BDTR()
{
//fmBDTR->IdUDPServer1->Active=false;
}

int BDTR::EhPrincipal(void)
{
return IHM_PRINCIPAL;
}

// Pede GI (general interrogation) ao BDTR. par=0: somente se principal, par=1: forçada
void BDTR::Sol_Integridade(tipo_integridade tpint)
{
/*
// Pede uma integridade ao BDTR
msg_req m;

m.COD = T_REQ;
m.TIPO = REQ_HORA;
m.ORIG = MEU_END;
m.ID = BDTR_GRUPO_INTEGRIDADE; // grupo zero = pede todos os pontos
m.NPTS = 0;
m.PONTOS[0] = 0;

// pede a hora, para contornar problema de timeout no arp
if ( (IP_BDTR1 != "") && (IHM_PRINCIPAL || tpint == integridade_forcada) )
  {
  try
    {
    envia_udp( (char*)IP_BDTR1.c_str(), BDTR_PORTA_CMD, (char*)&m, sizeof(m) );
    if ( IP_BDTR2 != "" )
      envia_udp( (char*)IP_BDTR2.c_str(), BDTR_PORTA_CMD, (char*)&m, sizeof(m) );
    }
    catch (...)
    { // erro de rede
    msg_status = "Erro de rede ao pedir hora!";
    }
  }

m.TIPO=REQ_GRUPO;

// pede a integridade
if ( (IP_BDTR1 != "") && (IHM_PRINCIPAL || tpint == integridade_forcada) )
  {
  try
    {
    envia_udp( (char*)IP_BDTR1.c_str(), BDTR_PORTA_CMD, (char*)&m, sizeof(m) );
    if (IP_BDTR2!="")
      envia_udp( (char*)IP_BDTR2.c_str(), BDTR_PORTA_CMD, (char*)&m, sizeof(m) );
    }
    catch (...)
    { // erro de rede
    msg_status = "Erro de rede ao pedir integridade!";
    }

  cntIntgr++;
  }

SegundosParaGI = PERIODO_INTEGRIDADE; // contagem para a próxima GI
*/
}

void BDTR::EnviaHora( string IP, unsigned char Origem )
{
if ( IP == "" )
  return;

      msg_sinc m;
      m.COD = T_HORA;
      m.VAGO = 0;
      m.ORIG = Origem;

      time_t timer;
      struct tm *tblock;

      /* gets time of day */
      timer = time(NULL);

      /* converts date/time to a structure */
      tblock = localtime(&timer);

      m.TAG.ANO = tblock->tm_year + 1900;
      m.TAG.MES = tblock->tm_mon + 1;
      m.TAG.DIA = tblock->tm_mday;
      m.TAG.HORA = tblock->tm_hour;
      m.TAG.MINUTO = tblock->tm_min;
      m.TAG.MSEGS = 1000 * tblock->tm_sec + 0;

      if ( Logar ) // se o form é visível, mostra um log
        {
        char linha[1000];
        sprintf( linha, "-> Msg.Hora: %d/%d/%d %02d:%02d:%02d", m.TAG.DIA, m.TAG.MES, m.TAG.ANO, m.TAG.HORA, m.TAG.MINUTO, m.TAG.MSEGS );
        logaln( linha );
        }

      try {
      set_udp_timeout(0);
      envia_udp( (char *)IP.c_str(), BDTR_PORTA, (char*)&m, sizeof(m) );
      }
      catch (...)
      { // erro de rede
      msg_status="Erro de rede ao enviar hora!";
      }
}

int BDTR::EnviaComando(msg_com * mc)
{
/*
  if ( IP_BDTR1 != "" )     // comando, manda quer seja principal ou reserva
    {
    cntCmd++;
    try
      {
      if ( Logar ) // se o form é visível, mostra um log
        {
        char linha[1000];
        sprintf( linha, "-> Msg.Comando: %d %x %x", mc->PONTO.ID, mc->PONTO.VALOR.DIG, mc->PONTO.STATUS );
        logaln( linha );
        }

      envia_udp( (char *)IP_BDTR1.c_str(), BDTR_PORTA_CMD, (char*)mc, sizeof(msg_com) );
      if ( IP_BDTR2 != "" )
      envia_udp( (char *)IP_BDTR2.c_str(), BDTR_PORTA_CMD, (char*)mc, sizeof(msg_com) );
      }
    catch (...)
      { // erro de rede
      msg_status="Erro de rede ao enviar comando!";
      }
    }
*/
  return 0;
}

// bloqueia ou desbloqueia comando (blk=1: bloqueia blk=0:desbloqueia)
int BDTR::BlkComando(int endponto, int blk)
{
/*
if ( BDTR_ENVIA_COMANDOS == 0 )
  return 1;

int tipo = T_DIG;

      msg_com mc;
      mc.COD = T_COM;
      mc.TVAL = tipo;
      mc.ORIG = MEU_END;
      mc.PONTO.ID = endponto;
      if ( blk == 1 )
        {
        mc.PONTO.VALOR.DIG = 0x03;
        mc.PONTO.STATUS = 0x13;
        }
      else
        {
        mc.PONTO.VALOR.DIG = 0x03;
        mc.PONTO.STATUS = 0x03;
        }

      if ( IP_BDTR1 != "" )     // comando, manda quer seja principal ou reserva
        {
        if ( Logar ) // se o form é visível, mostra um log
          {
          char linha[1000];
          sprintf( linha, "-> Msg.Bloq.Comando: %d %x %x", mc.PONTO.ID, mc.PONTO.VALOR.DIG, mc.PONTO.STATUS );
          logaln( linha );
          }

        try
          {
          envia_udp( (char *)IP_BDTR1.c_str(), BDTR_PORTA_CMD, (char*)&mc, sizeof(mc) );
          if ( IP_BDTR2 != "" )
            envia_udp( (char *)IP_BDTR2.c_str(), BDTR_PORTA_CMD, (char*)&mc, sizeof(mc) );
          }
        catch (...)
          { // erro de rede
          msg_status = "Erro de rede ao bloquear comando!";
          }
        }
*/
      return 0;
}

int BDTR::EnviaPontoDig(int endponto, unsigned char qualif, string ip, int porta)
{
/*
if ( ip == "" || porta == 0 || endponto == 0 )
  return 1;

      msg_dig m;
      m.COD = T_DIG;
      m.NRPT = 1;
      m.ORIG = MEU_END;
      m.PONTO[0].ID = endponto;
      m.PONTO[0].STAT = qualif;
      if ( IP_BDTR1 != "" ) // difusão, manda quer seja principal ou reserva
        {
        if ( Logar ) // se o form é visível, mostra um log
          {
          char linha[1000];
          sprintf( linha, "-> Msg.Ponto Digital: %d %x", m.PONTO[0].ID, m.PONTO[0].STAT );
          logaln( linha );
          }

        try
          {
          envia_udp( (char *)ip.c_str(), porta, (char*)&m, sizeof(m) );
          }
        catch (...)
          { // erro de rede
          msg_status = "Erro de rede ao enviar ponto digital!";
          }
        }
*/
      return 0;
}

void BDTR::recebe_udp(char * IP)
{
FILE * fp=NULL;
unsigned char nrpt;
unsigned char orig;
bool Convertido;
bool Espontaneo;
bool Ciclico;
unsigned char Cod;
int szrd;
unsigned char Buff[3000];
char linha[10000]="";

try
  {
  cntTot++;

  // lbIP->Caption=ABinding->PeerIP;

  // a leitura UDP não recebe os 2 primeiros bytes do tamanho!
  szrd=le_udp((char*)Buff, 1500); // le o tamanho da mensagem
  // lbTam->Caption=szrd;

  // loga a mensagem
  if (LogaEmArq)
    fp = fopen("bdtr.log", "a");

  if (fp)
    {
    fprintf(fp, "%04d ", szrd);
    for ( int i=0; i<szrd; i++ )
      fprintf(fp, "%02x ", Buff[i]);
    fprintf(fp, "\n");
    }

  if (Logar) // se o form é visível, mostra um log
    {
    sprintf(linha, "<- %04d ", szrd);
    for ( int i=0; i<szrd; i++ )
      {
      sprintf(linha+strlen(linha), "%02x ", Buff[i]);
      if (i>50)
        {
        sprintf(linha+strlen(linha), " ... ");
        break;
        }
      }
    logaln(linha);
    linha[0]=0;
    }

  Cod = Buff[0] & T_MASC;
  Convertido = Buff[0] & T_CONV;
  Espontaneo = Buff[0] & T_SPONT;
  // Ciclico = Buff[0] & T_CIC;

  switch (Cod) // vai pelo tipo da mensagem (COD)
    {
/*
    case T_DIG :
      {
      msg_dig *m;
      m = (msg_dig*)Buff;
      for (int i=0; i<m->NRPT; i++)
        {
        A_dig *a;
        a = (A_dig *)( Buff + (sizeof(*m)-sizeof(*a)+i*sizeof(*a)) );
        EscrevePonto(a->ID, a->STAT&0x01, a->STAT, 0, !Convertido, 0);

        if (Logar) // se o form é visível
        if (strlen(linha)<5000)
          sprintf(linha+strlen(linha), " - %d %02x", a->ID, a->STAT);
        }
      if (Logar) // se o form é visível, mostra um log
        logaln((string)"T_DIG" + linha);
      cntDig++;
      if (IntgrEmAndamento) cntParcelasIntgr++;
      }
      break;

    case T_DIG_TAG :
      {
      msg_dig_tag *m;
      m = (msg_dig_tag*)Buff;
      for (int i=0; i<m->NRPT; i++)
        {
        A_dig_tag *a;
        a = (A_dig_tag *)( Buff + (sizeof(*m)-sizeof(*a)+i*sizeof(*a)) );

        IncluiEvento(   a->ID,
                        a->UTR,
                        a->STAT,
                        a->TAG.ANO,
                        a->TAG.MES,
                        a->TAG.DIA,
                        a->TAG.HORA,
                        a->TAG.MINUTO,
                        a->TAG.MSEGS/1000,
                        a->TAG.MSEGS%1000
                       );

        EscrevePonto(a->ID, a->STAT&0x01, a->STAT, 0, !Convertido, 1);

        if (Logar) // se o form é visível, mostra um log
        if (strlen(linha)<5000)
          sprintf(linha+strlen(linha), " - pt %d utr %d flg %02x %d:%d:%d %02d%02d%04d", a->ID, a->UTR, a->STAT, a->TAG.HORA, a->TAG.MINUTO,  a->TAG.MSEGS, a->TAG.DIA, a->TAG.MES, a->TAG.ANO );
        }
      if (Logar) // se o form é visível, mostra um log
        logaln((string)"T_DIG_TAG" + linha);
      cntDigTag++;

      if (IHM_PRINCIPAL)
        {
        EventosAQuitar++;
        BDTR_IPQUITAR=IP;
        }
      }
      break;

    case T_ANA :
      {
      msg_ana *m;
      m = (msg_ana*)Buff;
      for (int i=0; i<m->NRPT; i++)
        {
        A_ana *a;
        a = (A_ana *)( Buff + (sizeof(*m)-sizeof(*a)+i*sizeof(*a)) );
        float val = (float)a->VALOR / pow (10, a->STAT&DECBITS);
        int endponto = a->ID;
        EscrevePonto(endponto, val, a->STAT, 0, !Convertido, 0, Espontaneo);
        if (Logar)
          if (strlen(linha)<5000)
            sprintf(linha+strlen(linha), " - %d %d %02x", endponto, a->VALOR, a->STAT);
        }

      if (Logar)
        logaln((string)"T_ANA" + linha);
      cntAna++;
      if (IntgrEmAndamento) cntParcelasIntgr++;
      }
      break;

    case T_NORM :
      {
      msg_ana *m;
      m = (msg_ana*)Buff;
      for (int i=0; i<m->NRPT; i++)
        {
        A_ana *a;
        a = (A_ana *)( Buff + (sizeof(*m)-sizeof(*a)+i*sizeof(*a)) );
        int endponto = a->ID;
        float val = (float)a->VALOR/32767.0;
        EscrevePonto(endponto, val, a->STAT, 0 , !Convertido, 0, Espontaneo );
        if (Logar)
          if (strlen(linha)<5000)
            sprintf(linha+strlen(linha), " - %d %d %02x", endponto, a->VALOR, a->STAT);
        }
      if (Logar)
          logaln((string)"T_NORM" + linha);
      cntAna++;
      if (IntgrEmAndamento) cntParcelasIntgr++;
      }
      break;

    case T_ANA_TAG :
      {
      msg_ana_tag *m;
      m = (msg_ana_tag*)Buff;
      for (int i=0; i<m->NRPT; i++)
        {
        A_ana_tag *a;
        a = (A_ana_tag *)( Buff + (sizeof(*m)-sizeof(*a)+i*sizeof(*a)) );
        int endponto = a->ID;
        float val = (float)a->VALOR / pow (10, a->STAT&DECBITS);
        EscrevePonto(endponto, val, a->STAT, 0, !Convertido, 0, Espontaneo);
        if (Logar)
          if (strlen(linha)<5000)
            sprintf(linha+strlen(linha), " - %d %d %02x", endponto, a->VALOR, a->STAT);
        }
      cntAna++;
      if (IntgrEmAndamento) cntParcelasIntgr++;
      }
      break;

    case T_FLT :  // analógico com ponto flutuante
      {
      msg_float *m;
      m = (msg_float*)Buff;

      for (int i=0; i<m->NRPT; i++)
        {
        int endponto = m->PONTO[i].ID;
        float val = m->PONTO[i].VALOR;
        EscrevePonto(endponto, val, m->PONTO[i].STAT, 0 , !Convertido, 0, Espontaneo);
        if (Logar)
          if (strlen(linha)<5000)
            sprintf(linha+strlen(linha), " - %d %f %02x", endponto, val, m->PONTO[i].STAT);
        }
      if (Logar)
        logaln((string)"T_FLT" + linha);
      cntAna++;
      if (IntgrEmAndamento) cntParcelasIntgr++;
      }
      break;

    case T_INICIO : // inicio de bloco de dados
      {
      msg_req *m;

      m = (msg_req*)Buff;
      if (m->TIPO==REQ_GRUPO && m->ID==0)
        {
        if (Logar)
          logaln((string)"Início da GI.");
        cntIniRecIntgr++;
        }
      cntParcelasIntgr=0;
      IntgrEmAndamento=true;
      }
      break;

    case T_FIM :    // fim de bloco de dados
      {
      msg_req *m;
      m = (msg_req*)Buff;
      if (m->TIPO==REQ_GRUPO && m->ID==0)
        {
        if (Logar)
          logaln((string)"Fim da GI.");
          
        cntFimRecIntgr++;
        RecebeuIntegridade();

        if ((signed) cntParcelasIntgr < m->NPTS)  // se recebeu menos parcelas que o esperado pede nova GI
           {
           if (Logar)
             logaln((string)"Erro de parcelas na GI!");
           cntErroIntgr++;
           Sol_Integridade(integridade_forcada); // pede mesmo se for reserva
           }
        }
      cntParcelasIntgr=0;
      IntgrEmAndamento=false;
      }
      break;

    case T_RESET : // reset do bdtr, vai pedir uma integridade
      if (Logar)
        logaln((string)"RESET DO BDTR!");

      SegundosParaGI=DELAY_PARA_INTEGRIDADE_APOS_RESET;    
      cntReset++;
      break;
*/
    case T_HORA : // mensagem de acerto de hora
      {
      // m->ORIG==255 ORIGEM = 255 identifica mensagem keep-alive do IHM redundante
      // nao se deve usar este endereço a partir do BDTR, apenas no IHM

      msg_sinc * m;
      m = (msg_sinc*)Buff;

      if (Logar)
        {
        sprintf(linha, "Recebida Hora: %02d/%02d/%04d %02d:%02d:%02d,%03d", m->TAG.DIA&0x1F, m->TAG.MES, m->TAG.ANO, m->TAG.HORA, m->TAG.MINUTO, m->TAG.MSEGS/1000, m->TAG.MSEGS%1000);
        logaln(linha);
        }

      RecebeuHora(m);
      cntSinc++;
      }
      break;
/*
    case T_ACK :
      {
      cntAck++;
      msg_ack *m;
      m = (msg_ack*)Buff;
      if ( m->TIPO == T_COM )
        {
        sprintf(linha, "Ack Cmd: pt %d fl %d vl %d", m->COMP, m->ID & 0x80, m->ID & 0x03 );
        logaln(linha);
        SetAckCmd(m->COMP, (m->ID & 0x80) ? 1 : 0, m->ID & 0x03);
        }
      }  
      break;

    case T_COM: // comando, normalmente não se recebe comando, apenas quando há simulação mestre/escravo, vem comando do escravo para o mestre
      {
      msg_com *m;
      m = (msg_com *)Buff;
      if ( m->TVAL == T_DIG )
        CommandIndication(m->PONTO.ID, m->PONTO.VALOR.COM, (float)(m->PONTO.VALOR.COM & 0x03));
      }
      break;

    case T_REQ :
      {
      cntOutras++;
      msg_req *m;
      m = (msg_req *)Buff;
      if ( m->TIPO==REQ_GRUPO && m->ID==0 )
        GenInterrogIndication();
      }
      break;

    case T_NULO : // mensagens não tratadas
    case T_BIN :
    case T_GRUPO :
      cntOutras++;
      break;
*/      
    default:
      cntErro++;
      break;
    }
  }
catch (...)
  { // não conseguiu ler a mensagem
  msg_status="Erro ao ler mensagem!";

  if (fp)
    fprintf(fp, "Exception %s \n", msg_status.c_str());
  }

  if (fp)
    fclose(fp);
}

void BDTR::ontimerseg()
{
/*
// vou quitar os eventos recebidos
if ( EhPrincipal() )
while (EventosAQuitar)
  {
  EventosAQuitar--;
  cntQuitEve++;

  msg_ack ma;
  ma.COD=T_ACK;
  ma.TIPO=T_DIG_TAG;
  ma.ORIG=MEU_END;
  ma.ID=cntQuitEve;
  ma.COMP=0;

  try
    {
    envia_udp( (char*)BDTR_IPQUITAR.c_str(), BDTR_PORTA_CMD, (char*)&ma, sizeof(ma) );
    }
  catch (...)
    { // erro de rede
    msg_status="Erro ao enviar quitaçao de evento!";
    }
  }

// controla contagem regressiva para próxima GI
if (SegundosParaGI>0)
  {
  SegundosParaGI--;

  if (SegundosParaGI==0)
    {
    Sol_Integridade(integridade_normal);  // pede integridade se for principal
    SegundosParaGI = PERIODO_INTEGRIDADE; // reinicia a contagem para a próxima
    }
  }
*/  
}

void BDTR::SetaPeriodoIntegridade(int segundos)
{
    PERIODO_INTEGRIDADE = segundos;
    SegundosParaGI = PERIODO_INTEGRIDADE; // já começa a contagem
}

bool BDTR::VarredorEhConversor()
{
return ( BDTR_PORTA_CMD_LOCAL == BDTR_PORTA_CMD );
}

// mostra conversor IEC104/BDTR, pedindo o grupo 255 para a máquina local na porta de comando
void BDTR::ShowConv104BDTR()
{
/*
if ( !VarredorEhConversor() )
  return;

// Pede uma integridade ao BDTR
msg_req m;

m.COD = T_REQ;
m.TIPO = REQ_GRUPO;
m.ORIG = MEU_END;
m.ID = 255; // grupo 255 = pede para mostrar a janela do conversor
m.NPTS = 0;
m.PONTOS[0] = 0;

envia_udp( "127.0.0.1", BDTR_PORTA_CMD, (char *)&m, sizeof(m) );
*/
}

