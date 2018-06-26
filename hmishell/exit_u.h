//---------------------------------------------------------------------------
// HMI SHELL - Copyright (c) 2008-2018 - Ricardo L. Olsen
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

#ifndef exit_uH
#define exit_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TfmSair : public TForm
{
__published:	// IDE-managed Components
        TBitBtn *btDesligar;
        TBitBtn *btReiniciar;
        TEdit *edSenha;
        TBitBtn *btLogout;
        TButton *btExplorer;
        TLabel *lbVersion;
        TButton *btPos;
        TButton *btRecup;
        TButton *btProcExp;
        TButton *btCache;
        TEdit *edSenha2;
        TLabel *lbSenha;
        TLabel *lbConfSenha;
        TButton *btNovaSenha;
        TLabel *lbServer;
        TButton *btAjuda;
        TButton *btNotes;
        void __fastcall btDesligarClick(TObject *Sender);
        void __fastcall btReiniciarClick(TObject *Sender);
        void __fastcall btLogoutClick(TObject *Sender);
        void __fastcall edSenhaChange(TObject *Sender);
        void __fastcall btExplorerClick(TObject *Sender);
        void __fastcall btPosClick(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall btRecupClick(TObject *Sender);
        void __fastcall btProcExpClick(TObject *Sender);
        void __fastcall btCacheClick(TObject *Sender);
        void __fastcall btNovaSenhaClick(TObject *Sender);
        void __fastcall edSenha2Change(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall btAjudaClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall btNotesClick(TObject *Sender);
private:	// User declarations
        DWORD VersaoWinwdows;
        bool EntrandoNovaSenha;
        String SENHA_ADM;
        int HIDEPASSWD;
        void PasswdReadDefault();
        void PasswdWrite( char * pswd );
        void PasswdToHexStr( char * pswd, char * hexres );
public:		// User declarations
        bool PasswdTest( char * pswd );
        // senha padrão: hash já em hexa
        String DOC_AJUDA;
        String LOG_AJUDA;
        String msgAltSenha;
        String msgSalvaSenha;
        bool Fechando;
        __fastcall TfmSair(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmSair *fmSair;
//---------------------------------------------------------------------------

#define ALTURA_MENOR 200
#define ALTURA_MAIOR 282

#endif
