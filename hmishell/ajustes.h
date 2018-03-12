//---------------------------------------------------------------------------
// HMI SHELL - Copyright (c) 2008-2014 - Ricardo L. Olsen
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

#ifndef ajustesH
#define ajustesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TfmAjustes : public TForm
{
__published:	// IDE-managed Components
        TEdit *edNPonto;
        TEdit *edKConv1;
        TEdit *edKConv2;
        TButton *btAjusta;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TEdit *edConf;
        TLabel *Label4;
        TLabel *lbErro;
        TButton *btCancel;
        void __fastcall btAjustaClick(TObject *Sender);
        void __fastcall btCancelClick(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
public:		// User declarations
        __fastcall TfmAjustes(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmAjustes *fmAjustes;
//---------------------------------------------------------------------------
#endif
