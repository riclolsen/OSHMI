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

#ifndef shell_uH
#define shell_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <NMHttp.hpp>
#include <Psock.hpp>

#define MAX_RUN 6 // máximo de programas a executar
#define MAX_ATALHOS_TELAS 10
#define MAX_WIN_TITLE_CTL 100
#define MAX_TELAS 300

//---------------------------------------------------------------------------
class TfmShell : public TForm
{
__published:	// IDE-managed Components
        TToolBar *ToolBar1;
        TToolButton *tbAnormais;
        TImageList *ImageList1;
        TToolButton *tbHist;
        TToolButton *tbEventos;
        TToolButton *tbSilencia;
        TToolButton *tbTabular;
        TToolButton *tbTelas;
        TToolButton *tbCurvas;
        TToolButton *tbSair;
        TToolButton *ToolButton1;
        TToolButton *ToolButton2;
        TToolButton *ToolButton3;
        TPopupMenu *popTelas;
        TTimer *Timer1;
        TTimer *Timer2;
        TNMHTTP *NMHTTP1;
        TTimer *Timer3;
        TTimer *Timer4;
        TTimer *Timer5;
        TTimer *Timer6;
        TTimer *Timer7;
        TTimer *Timer8;
        TPanel *Panel1;
        TLabel *Label2;
        TLabel *Label1;
        TShape *shMove;
        TTimer *Timer9;
        TTimer *Timer10;
        TToolButton *ToolButton4;
        TToolButton *ToolButton5;
        TToolButton *ToolButton6;
        TToolButton *ToolButton7;
        TNMHTTP *NMHTTP2;
        TTimer *Timer11;
        void __fastcall tbSairClick(TObject *Sender);
        void __fastcall tbCurvasClick(TObject *Sender);
        void __fastcall tbEventosClick(TObject *Sender);
        void __fastcall tbHistClick(TObject *Sender);
        void __fastcall tbAnormaisClick(TObject *Sender);
        void __fastcall tbTabularClick(TObject *Sender);
        void __fastcall tbSilenciaClick(TObject *Sender);
        void __fastcall popTelasItemsClick(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall shMoveMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall Timer3Timer(TObject *Sender);
        void __fastcall NMHTTP1Success(CmdType Cmd);
        void __fastcall NMHTTP1Failure(CmdType Cmd);
        void __fastcall Timer4Timer(TObject *Sender);
        void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall Timer5Timer(TObject *Sender);
        void __fastcall tbTelasMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall ToolBar1MouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall tbEventosMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall tbHistMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall tbAnormaisMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall tbTabularMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall tbCurvasMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall tbSilenciaMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall tbSairMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Timer6Timer(TObject *Sender);
        void __fastcall Timer7Timer(TObject *Sender);
        void __fastcall Timer8Timer(TObject *Sender);
        void __fastcall Panel1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall tbTelasClick(TObject *Sender);
        void __fastcall Timer9Timer(TObject *Sender);
        void __fastcall Timer10Timer(TObject *Sender);
        void __fastcall Label2MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall Label1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall Timer11Timer(TObject *Sender);
        void __fastcall lblScreenClick(TObject *Sender);
        void __fastcall lblScreenMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
        void __fastcall FormActivate(TObject *Sender);
private:	// User declarations
        int glAlpha;
        static const int glAlphaMax = 250;
        static const int glAlphaMin = 125;
        int DELAY;
        int INTERVAL;
        int RUN_NUM;
        String RUN[MAX_RUN];
        bool WebServerOk;
        int TRANSPARENCIA;
        int SHRINK;
        String TXT_BOTAO_EVENTOS;
        String TXT_BOTAO_HISTORICO;
        String TXT_BOTAO_ANORMAIS;
        String TXT_BOTAO_TABULAR;
        String TXT_BOTAO_CURVAS;
        String TITULO_VISOR_TELAS;
        String TITULO_VISOR_ANORMALIDADES;
        String TITULO_VISOR_TABULAR;
        String TITULO_VISOR_HISTEVENTOS;
        String TITULO_VISOR_EVENTOS;
        String TITULO_VISOR_CURVAS;
        String TELAS[MAX_ATALHOS_TELAS];
        String WEBSERVER_DATE_FMT;
        int PROC_BEEP;
        int TemBeep; // informa se o alarme bipa
        void SilenciaBeep();
        int HaBeepAtivo();
        void AtivaBeep( int tipo );
        void CheckWinKey();
public:		// User declarations
        TLabel * ScreenItems[MAX_TELAS];
        void ShowSelScreen(void);
        int NumScreens;
        int SelScreen;
        String BROWSER_OPTIONS;
        String VISOR_EVENTOS;
        String VISOR_TABULAR;
        String VISOR_TELAS;
        String VISOR_CURVAS;
        String VISOR_TENDENCIAS;
        String VISOR_DOCS;
        String VISOR_LOGS;
        int lastks;
        __fastcall TfmShell(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmShell *fmShell;
//---------------------------------------------------------------------------

extern String REMOTE_HOST;
extern int REMOTE_PORT;

#define ARQ_CONF "..\\conf\\hmishell.ini"
#define ARQ_CONFI18N "..\\i18n\\messages_i18n.ini"
#define ARQ_CONF_IHM "..\\conf\\hmi.ini"
#define ARQ_LOG "..\\logs\\hmishell.log"

// estados possíveis de beep 
#define BEEP_NENHUM 0
#define BEEP_NORMAL 1
#define BEEP_CRITICO 2

#define CLR_SCR_TXT_OPENED 0x0274020f
#define CLR_SCR_TXT_NOTOPENED 0x02929405 
#define CLR_SCR_BGD_SELECTED 0x02ede5c8
#define CLR_SCR_BGD_NOTSELECTED 0x02fdf5d8
#define CLR_SCR_GRP 0x028fefdf1

extern String SHELLAPIURL;

void ExecExternApp(char * cmd);
void Loga(String msg, String arq=ARQ_LOG );

#endif
