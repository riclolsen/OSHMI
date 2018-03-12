//---------------------------------------------------------------------------
/*
SIM_TR

OSHMI - Open Substation HMI

    Copyright (c) 2008-2014 - Ricardo L. Olsen    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <vcl.h>
#pragma hdrstop
#include <stdio.h>

#include "sim_tr_u.h"

#include "..\\webserver\\bdtr.h"

#define BDTR_PORTA 65280

#define COL_IP 0
#define COL_DESCRICAO 1
#define COL_HABILITA 2

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IdBaseComponent"
#pragma link "IdComponent"
#pragma link "IdUDPBase"
#pragma link "IdUDPClient"
#pragma link "IdUDPServer"
#pragma resource "*.dfm"
TfmSimTR *fmSimTR;

char Buff[10000];
int LinhaExec = 0;
int Rodando = 0;

//---------------------------------------------------------------------------
__fastcall TfmSimTR::TfmSimTR(TComponent* Owner)
        : TForm(Owner)
{
lbNomeArq->Caption = "";
lbEstado->Caption = "Stopped!";

sgDestinos->Cells[COL_IP][0] = "IP Addr.";
sgDestinos->Cells[COL_DESCRICAO][0] = "Descript.";
sgDestinos->Cells[COL_HABILITA][0] = "Enable 1/0";

sgDestinos->Cells[COL_IP][1] = "127.0.0.1";
sgDestinos->Cells[COL_DESCRICAO][1] = "Local Machine";
sgDestinos->Cells[COL_HABILITA][1] = "1";
}

//---------------------------------------------------------------------------
void __fastcall TfmSimTR::btFileOpenClick(TObject *Sender)
{
//OpenDialog1->DefaultExt = "txt";
OpenDialog1->InitialDir = "c:\\oshmi\\etc";
OpenDialog1->FileName = lbNomeArq->Caption;
OpenDialog1->Filter = "Text Files (*.txt)|*.TXT|All (*.*)|*.*";
if ( OpenDialog1->Execute() )
  {
  lbNomeArq->Caption=OpenDialog1->FileName;
  lbSim->Items->LoadFromFile(lbNomeArq->Caption);
  }

LinhaExec = 0;
lbSim->TopIndex = LinhaExec;
}

//---------------------------------------------------------------------------
void __fastcall TfmSimTR::Timer1Timer(TObject *Sender)
{
static  String S;
static  int nf;
static  char ch[100];
static  char comando[100];
static  char tipo[100];
static  char id[100];
static  char subcomando[100];
static  char coment[100];
static  char saux[100];
static  float valor;
static  int nponto;
static  int tempo;
static  int msec;

if( Rodando == 1 )
  lbEstado->Caption = "Executing!";
else
if ( LinhaExec == 0 )
  lbEstado->Caption = "Stopped!";
else
  lbEstado->Caption = "Paused!";

Timer1->Interval = 5;

lbSim->TopIndex = LinhaExec;
lbLine->Caption = LinhaExec;

if ( Rodando == 0 )
  {
  Timer1->Interval = 500;
  return;
  }

for ( ; LinhaExec<lbSim->Items->Count; LinhaExec++ )
  {

  if ( strncmp( lbSim->Items->Strings[LinhaExec].c_str(), "> ESP ", 6 )==0 )
    {
    nf = sscanf( lbSim->Items->Strings[LinhaExec].c_str(), "%s %s %u ", ch, comando, &tempo );
    Timer1->Enabled = false;
    Timer1->Interval = 1000 * tempo;
    Timer1->Enabled = true;
    lbSim->TopIndex = LinhaExec;
    lbLine->Caption = LinhaExec;
    LinhaExec++;
    break;
    }
  else
  if ( strncmp( lbSim->Items->Strings[LinhaExec].c_str(), "> VAR DIG ", 10 ) == 0 )
    {
    nf = sscanf(lbSim->Items->Strings[LinhaExec].c_str(), "%s %s %s %s %s %f %s %d %s %s %d", ch, comando, tipo, id, subcomando, &valor, coment, &nponto, saux, saux, &msec );
    if ( msec == -1 )
      {
      msg_dig ms;
      ms.COD = T_DIG | T_CONV;
      ms.NRPT = 1;
      ms.ORIG = 0;
      ms.PONTO[0].ID = nponto;
      ms.PONTO[0].STAT = (valor)? 0x02 : 0x01;
      SendBDTR( (char*)&ms, sizeof(ms) );
      }
    else
      {
      msg_dig_tag ms;
      TDateTime dt = Now();
      unsigned short year; unsigned short month; unsigned short day;
      unsigned short hour; unsigned short min; unsigned short sec; unsigned short milisg;
      dt.DecodeDate(&year, &month, &day);
      dt.DecodeTime(&hour, &min, &sec, &milisg);

      ms.COD = T_DIG_TAG | T_CONV;
      ms.NRPT = 1;
      ms.ORIG = 0;
      ms.PONTO[0].TAG.ANO = year;
      ms.PONTO[0].TAG.MES = month;
      ms.PONTO[0].TAG.DIA = day;
      ms.PONTO[0].TAG.HORA = hour;
      ms.PONTO[0].TAG.MINUTO = min;
      ms.PONTO[0].TAG.MSEGS = sec * 1000 + msec; // usa o milissegundo do arquivo
      ms.PONTO[0].ID = nponto;
      ms.PONTO[0].UTR = 0;
      ms.PONTO[0].STAT = (valor)? 0x02 : 0x01;
      SendBDTR( (char *)&ms, sizeof(ms) );
      }
    }
  else
  if ( strncmp( lbSim->Items->Strings[LinhaExec].c_str(), "> VAR MED ", 10) == 0 )
    {
    nf = sscanf( lbSim->Items->Strings[LinhaExec].c_str(), "%s %s %s %s %s %f %s %d ", ch, comando, tipo, id, subcomando, &valor, coment, &nponto );
    msg_float ms;
    ms.COD = T_FLT | T_CONV;
    ms.NRPT = 1;
    ms.ORIG = 0;
    ms.PONTO[0].ID = nponto;
    ms.PONTO[0].VALOR = valor;
    ms.PONTO[0].STAT = 0x20;
    SendBDTR( (char *)&ms, sizeof(ms) );
    }

  lbMsg->Caption = nponto;

  if ( !( LinhaExec % 20 ) )
    {
    LinhaExec++;
    break;
    }

  //lbSim->Selected[LinhaExec] = false;
  }

if ( LinhaExec >= lbSim->Items->Count )
  {
  LinhaExec = 0;
  Rodando = 0;
  }

}

//---------------------------------------------------------------------------
void __fastcall TfmSimTR::btFimClick(TObject *Sender)
{
lbEstado->Caption = "Parado!";
LinhaExec = 0;
lbSim->TopIndex = LinhaExec;
Rodando = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfmSimTR::btPauseClick(TObject *Sender)
{
Rodando = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfmSimTR::btPlayClick(TObject *Sender)
{
Rodando = 1;
Timer1->Interval = 100;
Timer1->Enabled = true;
}
//---------------------------------------------------------------------------

// Manda para todos os destinos configurados
void TfmSimTR::SendBDTR( char * buf, int size )
{

  for ( int linha = 1 ; linha < sgDestinos->RowCount ; linha++ )
    {
      if ( sgDestinos->Cells[COL_IP][linha] == "" )
        break;

      if ( sgDestinos->Cells[COL_HABILITA][linha] == "1" )
        {
        IdUDPClient1->Host = fmSimTR->sgDestinos->Cells[COL_IP][linha];
        IdUDPClient1->Port = BDTR_PORTA;
        IdUDPClient1->SendBuffer( buf, size );
        }
    }

}

