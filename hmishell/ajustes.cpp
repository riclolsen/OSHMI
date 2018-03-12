//---------------------------------------------------------------------------
// HMI SHELL - Copyright 2008-2014 - Ricardo L. Olsen
/*
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

// aceita parametros de ponto, testa se são dados válidos e escreve no arquivo ajustes.ini
// Para ajuste de kconv pelo operador (orientado)

#include <vcl.h>
#pragma hdrstop
#include <inifiles.hpp>
#include <math.h>
#include "ajustes.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmAjustes *fmAjustes;
//---------------------------------------------------------------------------
__fastcall TfmAjustes::TfmAjustes(TComponent* Owner)
        : TForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TfmAjustes::btAjustaClick(TObject *Sender)
{
try
  {
  int nponto, conf, P;
  float kconv1, kconv2, fval;
  String S;

  nponto = edNPonto->Text.ToInt();

  S = edKConv1->Text;
  if ( ( P = S.Pos(",") ) !=0 )
    {
    S[P] = '.';
    }
  fval = atof(S.c_str());
  if ( fval >= HUGE_VAL || fval <= -HUGE_VAL || fval == 0 )
     {
     throw Exception("Invalid kconv1.");
     }
  kconv1 = fval;

  S = edKConv2->Text;
  if ( ( P = S.Pos(",") ) != 0 )
    {
    S[P] = '.';
    }
  fval = atof(S.c_str());
  if ( fval >= HUGE_VAL || fval <= -HUGE_VAL )
    {
    throw Exception("Invalid kconv2.");
    }
  kconv2 = fval;

  conf = edConf->Text.ToInt();

  if ( nponto <= 0 )
    {
    throw Exception("Invalid point number.");
    }

  if ( ( int (nponto + kconv1 + kconv2) % 11) +
       ( int (nponto + kconv1 + kconv2) % 17)
       != conf )
    {
    throw Exception("Invalid Data.");
    }

  TIniFile *pIni = new TIniFile( ".\\conf\\adjusts.ini" );
  pIni->WriteFloat( nponto, "KCONV1", kconv1 );
  pIni->WriteFloat( nponto, "KCONV2", kconv2 );
  delete pIni;

  lbErro->Caption = "OK!";
  }
catch ( Exception &E )
  {
  lbErro->Caption = "ERROR! " + E.Message;
  }

}
//---------------------------------------------------------------------------
void __fastcall TfmAjustes::btCancelClick(TObject *Sender)
{
Hide();
}
//---------------------------------------------------------------------------
void __fastcall TfmAjustes::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
if ( Key == VK_ESCAPE )
  {
  Hide();
  }
}
//---------------------------------------------------------------------------
