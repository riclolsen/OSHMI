//---------------------------------------------------------------------------
// Communication I104M ( DNP3.EXE, MODBUS.EXE, ICCP_CLIENT.EXE, IEC104M.EXE )
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
#include <math.h>

#include "i104m_u.h"
#include "config.h"
#include "bcolocal.h"
#include "eventos_u.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IdBaseComponent"
#pragma link "IdComponent"
#pragma link "IdUDPBase"
#pragma link "IdUDPServer"
#pragma resource "*.dfm"
TfmIEC104M *fmIEC104M;

//---------------------------------------------------------------------------
__fastcall TfmIEC104M::TfmIEC104M(TComponent* Owner)
        : TForm(Owner)
{
I104_cntIntgr = 0;
cntPackt = 0;
cntPacktDisc = 0;
}

//---------------------------------------------------------------------------

// Envia comando digital ao varredor iec104m
// onoff : 1=ON, 0=OFF
int TfmIEC104M::ComandoIEC_Dig( unsigned int nponto, unsigned int onoff )
{
bool found;
TPonto &pt = BL.GetRefPonto( nponto, found );
if ( !found && nponto!=0 )
  return 1;

static t_msgcmd msgcmd;

msgcmd.signature = MSGCMD_SIG;
msgcmd.onoff = onoff;

if ( nponto == 0 ) // GI
  {
  I104_cntIntgr++;
  msgcmd.tipo = 1001;
  msgcmd.endereco = 0;
  msgcmd.sbo = 0;
  msgcmd.qu = 0;
  msgcmd.utr = 0;
  }
else
  {
  msgcmd.endereco = pt.Endereco;
  msgcmd.tipo = pt.CmdASDU;
  msgcmd.sbo = pt.CmdSBO;
  msgcmd.qu = pt.CmdDuracao;
  msgcmd.utr = pt.UTR;
  }

int tambf = sizeof( msgcmd );
int port = I104M_REMOTEPORT;
if ( pt.EnviaComandoPortaModbus )
   port = I104M_REMOTEPORTMODBUS;

IdUDPServer1->SendBuffer( "127.0.0.1", port, (char *)&msgcmd, tambf );

// manda para o varredor 104 redundante
if ( IHMRED_IP_OUTRO_IHM != "" )
  {
  IdUDPServer1->SendBuffer( IHMRED_IP_OUTRO_IHM, port, (char *)&msgcmd, tambf );
  }

// registra evento do envio do comando
if ( msgcmd.tipo != 1001 ) // não registra pedido de GI
IncluiEvento( nponto,
              msgcmd.utr,
              onoff ? 0x02 : 0x01,
              0,
              SDE_GERAHORA,
              0,
              0,
              0,
              0,
              0 );

if ( cbLog->Checked )
   logaln( (String)"< CMD Type:" + (String)msgcmd.tipo +
           (String)" Address:" + (String)msgcmd.endereco +
           (String)" RTU:" + (String)msgcmd.utr +
           (String)" SBO:" + (String)msgcmd.sbo +
           (String)" QU:" + (String)msgcmd.qu +
           (String)" VAL:" + (String)msgcmd.onoff
         );

return 0;
}

//---------------------------------------------------------------------------

// Envia comando analógico ao varredor iec104m
int TfmIEC104M::ComandoIEC_Ana( unsigned int nponto, double val )
{
bool found;
double temp;
TPonto &pt = BL.GetRefPonto( nponto, found );
if ( !found && nponto!=0 )
  return 1;

static t_msgcmd msgcmd;

msgcmd.signature = MSGCMD_SIG;
msgcmd.onoff = 0;

if ( nponto == 0 ) // GI
  {
  I104_cntIntgr++;
  msgcmd.tipo = 1001;
  msgcmd.endereco = 0;
  msgcmd.sbo = 0;
  msgcmd.qu = 0;
  msgcmd.utr = 0;
  }
else
  {
  msgcmd.endereco = pt.Endereco;
  msgcmd.tipo = pt.CmdASDU;
  msgcmd.sbo = pt.CmdSBO;
  msgcmd.qu = pt.CmdDuracao;
  msgcmd.utr = pt.UTR;

  switch ( pt.CmdASDU )
    {
    case 48: // C_SE_NA_1 Set-point normalized
    case 61: // C_SE_TA_1 Set-point normalized w/ time
      if ( pt.KConv1 !=0 )
        {
        temp = ((val - pt.KConv2) / pt.KConv1) * 32767;
        // limit possible values
        if ( temp > 32767 )
          temp = 32767;
        if ( temp < -32768 )
          temp = -32768;
        msgcmd.setpoint = temp;
        }
      else
        return 5;
      break;

    case 49: // C_SE_NB_1 Set-point scaled
    case 62: // C_SE_TB_1 Set-point scaled w/ time
      temp = val * pow10 (pt.CasaDecimal);
      // limit possible values
      if ( temp > 32767 )
        temp = 32767;
      if ( temp < -32768 )
        temp = -32768;
      msgcmd.setpoint =  temp;
      break;
    case 51: // C_BO_NA_1 Bitstring of 32 bits
      msgcmd.setpoint_i32 = (int)val;
      break;
    default:
    case 50: // C_SE_NC_1 Set-point short floating point
    case 63: // C_SE_TC_1 Set-point short floating point w/ time
      msgcmd.setpoint =  val;
      break;
    }           
  }

int tambf = sizeof( msgcmd );
int port = I104M_REMOTEPORT;
if ( pt.EnviaComandoPortaModbus )
   port = I104M_REMOTEPORTMODBUS;

IdUDPServer1->SendBuffer( "127.0.0.1", port, (char *)&msgcmd, tambf );

// manda para o varredor 104 redundante
if ( IHMRED_IP_OUTRO_IHM != "" )
  {
  IdUDPServer1->SendBuffer( IHMRED_IP_OUTRO_IHM, port, (char *)&msgcmd, tambf );
  }

// registra evento do envio do comando
if ( msgcmd.tipo != 1001 ) // não registra pedido de GI
  {
  TFA_Qual qlf;
  qlf.Byte = 0;
  qlf.Tipo = TIPO_ANALOGICO;
  qlf.EventoAnalogicoVUTR = 1;
  qlf.CasaDecimal = pt.CasaDecimal;
  IncluiEvento( nponto,
                round_( val * pow10( pt.CasaDecimal ) ),
                qlf.Byte,
                0,
                SDE_GERAHORA,
                0,
                0,
                0,
                0,
                0
                );
  }

if ( cbLog->Checked )
   logaln( (String)"< CMD Type:" + (String)msgcmd.tipo +
           (String)" Address:" + (String)msgcmd.endereco +
           (String)" RTU:" + (String)msgcmd.utr +
           (String)" VAL:" + (String)val
         );

return 0;
}

//---------------------------------------------------------------------------

void __fastcall TfmIEC104M::Timer1Timer(TObject *Sender)
{
if ( IHM_EstaFinalizando() ) return;

// se não tem bdtr nem simulacao, manda o primeiro pedido de integridade pelo iec104m
if ( IP_BDTR1 == "" && !BL.HaSimulacao() )
  {
  // pede uma GI para o iec104m
  ComandoIEC_Dig( 0, 0 );
  BL.RecebeuIntegridade(); // sinaliza integridade
  }

Timer1->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfmIEC104M::Timer2Timer(TObject *Sender)
{
if ( Visible )
  {
  lbPackt->Caption = (String)"Packets ok: " + cntPackt;
  lbPacktNok->Caption = (String)"Packets nok: " + cntPacktDisc;
  }
}
//---------------------------------------------------------------------------

void TfmIEC104M::logaln( String msg )
{
if ( cbLog->Checked )
  {
  if ( mmLOG->Lines->Count > 1500 )
   for ( int i = 0; i < 50; i++ )
      mmLOG->Lines->Delete(0);

  mmLOG->Lines->Add( msg.c_str() );
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmIEC104M::IdUDPServer1UDPRead(TObject *Sender,
      TStream *AData, TIdSocketHandle *ABinding)
{
if ( IHM_EstaFinalizando() ) return;

bool msg_valid = false;
unsigned char buffer[3000];
String s;

t_msgsup * pmsgsup = (t_msgsup *)buffer;
t_msgsupsq * pmsgsupsq = (t_msgsupsq *)buffer;

if (  WEBSERVER_CLIENTES_REMOTOS[1] != '*' &&
      WEBSERVER_CLIENTES_REMOTOS.Pos( ABinding->PeerIP ) == 0
   )
  {
  logaln( (String)"> Unauthorized request from: " + ABinding->PeerIP );
  return;
  }


try
{
  int bytesread = AData->Read( buffer, sizeof(buffer) );

  if ( pmsgsup->signature == MSGSUP_SIG )
    {
    msg_valid = true;
    cntPackt++;
    int v = BL.EscreveIEC(
                   pmsgsup->endereco,
                   pmsgsup->tipo,
                   pmsgsup->info,
                   pmsgsup->taminfo,
                   pmsgsup->prim,
                   pmsgsup->sec,
                   pmsgsup->causa );

    if ( cbLog->Checked )
       {
       logaln( (String)"> From:" + ABinding->PeerIP );
       logaln( (String)"> Type:" + (String)pmsgsup->tipo +
               (String)" Address:" + (String)pmsgsup->endereco +
               (String)" Sec:" + (String)pmsgsup->sec +
               (String)" Cause:" + (String)pmsgsup->causa +
               (String)" WrDB:" + (String)v
             );
       }
    }
  else
  if ( pmsgsupsq->signature == MSGSUPSQ_SIG )
    {
    int incinfo;
    switch( pmsgsupsq->tipo )
      {
        case 1:  // simples sem tag
        case 3:  // duplo sem tag
          incinfo = 4 + 1;
          break;
        case 2:  // simples com tag
        case 4:  // duplo com tag
          incinfo = 4 + 1 + 3;
          break;
        case 30: // simples com tag longa
        case 31: // duplo com tag longa
          incinfo = 4 + 1 + 7;
          break;
        case 5:
          incinfo = 4 + 2;
          break;
        case 9:  // normalized
        case 11: // scaled
          incinfo = 4 + 3;
          break;
        case 34: // normalized c/ tag
        case 35: // scaled c/ tag
          incinfo = 4 + 3 + 7;
          break;
        case 13: // ponto flutuante
          incinfo = 4 + 5;
          break;
        case 36: // ponto flutuante c/ tag
          incinfo = 4 + 5 + 7;
          break;
        case 15:
          incinfo = 4 + 5;
          break;
        default:
          cntPacktDisc++;
          return;
      }

    msg_valid = true;
    cntPackt++;

    if ( cbLog->Checked )
       {
       logaln( (String)"> Type:" + (String)pmsgsupsq->tipo +
               (String)" Points:" + (String)pmsgsupsq->numpoints +
               (String)" Sec:" + (String)pmsgsupsq->sec +
               (String)" Cause:" + (String)pmsgsupsq->causa +
               (String)" Inc:" + (String)incinfo
             );
       }

    s = "";
    for ( unsigned int i = 0; i < pmsgsupsq->numpoints; i++ )
      {
      unsigned int addr = *(unsigned int *) ( ((unsigned char *)pmsgsupsq->info) + (i * incinfo) );
      void * pinfo = ((unsigned char *)pmsgsupsq->info) + (4 + i * incinfo);

      // test malformed packet with more supposed info than bytes read
      if ( (i * incinfo) > (bytesread - sizeof(unsigned int) * 7) )
        {
        cntPacktDisc++;
        return;
        }

      int v = BL.EscreveIEC(
                     addr,
                     pmsgsupsq->tipo,
                     pinfo,
                     pmsgsupsq->taminfo,
                     pmsgsupsq->prim,
                     pmsgsupsq->sec,
                     pmsgsupsq->causa );

      if ( cbLog->Checked )
        {
        s = s + (String)" " + (String)addr;
        if ( pmsgsupsq->tipo == 1 || pmsgsupsq->tipo == 3 || pmsgsupsq->tipo == 30 || pmsgsupsq->tipo == 31 )
           s = s + (String)"/" + (String)*((unsigned char *)pinfo);
        if ( pmsgsupsq->tipo == 13 )
           s = s + (String)"/" + (String)*((float *)pinfo);
        s = s + (String)"/" + (String)v;
        }

      }
    if ( cbLog->Checked )
      {
      logaln( s );
      } 
    }
}
catch( Exception &E ){ return; };

if ( msg_valid )
  {
  try
    {
    // sinaliza que a comunicação com o varredor 104 está ok
    BL.EscrevePonto( NPONTO_COMUNIC_I104, FA_ESTFASIM_ON, ESTDUP_ON );

    // se estava falhada comunicação com I104 e normalizou, dispara GI, testa quantas
    if ( BL.GetRefPonto(NPONTO_COMUNIC_I104).Qual.Duplo == ESTDUP_OFF && I104_cntIntgr > 1 )
      {
      ComandoIEC_Dig( 0, 0 ); // GI
      }
    }
  catch ( char * p )
    {
    }
  }
else
  {
  cntPacktDisc++;
  }
}
//---------------------------------------------------------------------------


