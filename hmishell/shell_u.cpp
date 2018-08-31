//---------------------------------------------------------------------------
// HMI SHELL - Copyright 2008-2018 - Ricardo L. Olsen
//
// Este programa tem por objetivo proporcionar um shell para o Windows
// específico para controlar o IHM. De forma a restringir o acesso
// às demais funcionalidades do Windows.
//
// Deve ser instalado na seguinte chave do registro do windows na conta operador:
// [HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\Winlogon]
// "Shell"="C:\\oshmi\\bin\\hmishell.exe"
//---------------------------------------------------------------------------

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

#include <vcl.h>
#pragma hdrstop

#include <windows.h>
#include <psapi.h>
#include "shell_u.h"
#include "exit_u.h"
#include <stdio.h>
#include <dir.h>
#include <inifiles.hpp>
#include <Filectrl.hpp>
#include "screen_list_u.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmShell *fmShell;

String JANELA_AENCONTRAR = "";
HWND JANELA_ENCONTRADA = 0;

String WNDTITEQ[MAX_WIN_TITLE_CTL];
String WNDTITCT[MAX_WIN_TITLE_CTL];

unsigned TIMER_JANELAS_PROIBIDAS = 1700;
unsigned TIMER_ESPERA_VISOR = 2000;

String REMOTE_HOST;
int REMOTE_PORT = 51909;
String LISTA_TELAS_FILE = "..\\svg\\screen_list.js";
String LISTA_TELAS_FILE_NONUTF8 = "..\\svg\\screen_list-nonutf8.js";
String LISTA_TELAS_WEB = "svg/screen_list.js";

String StrReplace ( String S, String Find, String Repl )
{
int pos = S.Pos( Find );

if ( pos == 0 )
  return S;

return S.SubString( 1, pos - 1 ) + Repl + S.SubString( pos + Find.Length(), S.Length() );
}

//---------------------------------------------------------------------------

void ExecExternApp(char * cmd)
{
  PROCESS_INFORMATION pi;
  STARTUPINFO si;
  memset(&si, 0, sizeof(si));
  memset(&pi, 0, sizeof(pi));
  si.cb = sizeof(STARTUPINFO);
  CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}
//---------------------------------------------------------------------------

// Para o linux:
int Linux_ShowWindow(String WinTitle)
{
// WinExec( ( (String)"cmd /c show_win.bat '" + WinTitle + (String)"'" ).c_str(), SW_SHOWNORMAL );
return 0;
}
//---------------------------------------------------------------------------

// Para o linux:
// > xwininfo -root -tree | grep -c -q -s -i pattern
// retorna 0 se achou e não zero se não achou
int Linux_FindWindow(String WinTitle, String urlexe="")
{
/*
    String cmd;
    int ret;

    cmd = "cmd /c find_win.bat '" + WinTitle + (String)"' " + urlexe;
    ret = WinExec(cmd.c_str(), SW_SHOWNORMAL);
    if ( ret > 31 ) // se conseguiu executar, está no linux, então faz de conta que encontrou para não abrir janela de novo
      {
      JANELA_ENCONTRADA = (void*)1;
      Linux_ShowWindow( WinTitle );
      }
    else
      {
      ExecExternApp(urlexe.c_str());
      }
return ret > 31;
*/

return 0;
}
//---------------------------------------------------------------------------

// Procura se há uma janela de eventos aberta
// No Wine só encontra janelas do wine, não percebe as nativas do linux
// Se encotrar, traz para frente
bool CALLBACK enumwndprc(HWND hWnd, LPARAM lParam)
{
char bufstr[1000];
bufstr[0] = 0;

JANELA_ENCONTRADA = 0;

  GetWindowText( hWnd, bufstr, 998 );
  if ( bufstr[0] != 0 )
  {
  String S = bufstr;
  S = S.UpperCase().Trim();

  if ( S.Pos( JANELA_AENCONTRAR.UpperCase().Trim() ) )
    { // achou
    JANELA_ENCONTRADA = hWnd;
    ShowWindow( hWnd, SW_RESTORE );
    SetForegroundWindow( hWnd );
    return 0;
    }
  }

return 1;
}

// Procura se há uma janela de eventos aberta
// No Wine só encontra janelas do wine, não percebe as nativas do linux
// Se encotrar, não traz para frente
bool CALLBACK enumwndprc2(HWND hWnd, LPARAM lParam)
{
char bufstr[1000];
bufstr[0] = 0;

JANELA_ENCONTRADA = 0;

  GetWindowText( hWnd, bufstr, 998 );
  if ( bufstr[0] != 0 )
  {
  String S = bufstr;
  S = S.UpperCase().Trim();

  if ( S.Pos( JANELA_AENCONTRAR.UpperCase().Trim() ) )
    { // achou
    JANELA_ENCONTRADA = hWnd;
    // ShowWindow(hWnd, SW_RESTORE);
    // SetForegroundWindow( hWnd );
    return 0;
    }
  }

return 1;
}

void __fastcall TfmShell::popTelasItemsClick(TObject *Sender)
{
    TMenuItem *ClickedItem = dynamic_cast<TMenuItem *>(Sender);
    if ( ClickedItem )
      {
      // deixa só o que houver à esquerda do  [, { ou pontos.

      String S = ClickedItem->Caption.Trim();
      if ( S.Pos( "[" ) )
        S = S.SubString( 1, S.Pos( "[" ) - 1 ).Trim();
      if ( S.Pos( "{" ) )
        S = S.SubString( 1, S.Pos( "{" ) - 1 ).Trim();

      while ( S[S.Length()] == '.' )
        S = S.SubString( 1, S.Length() - 1 ).Trim();

      JANELA_AENCONTRAR = S.Trim();

      JANELA_AENCONTRAR = JANELA_AENCONTRAR + (String)" - " + TITULO_VISOR_TELAS;

      EnumWindows( (int (__stdcall *)()) enumwndprc, 0 );
      Linux_FindWindow( JANELA_AENCONTRAR, VISOR_TELAS + (String)"?INDTELA=" + (String)ClickedItem->Tag );
      if ( JANELA_ENCONTRADA == 0 ) // se não achou executa
        {
        ExecExternApp( (VISOR_TELAS + (String)"?INDTELA=" + (String)ClickedItem->Tag).c_str() );
        ToolBar1->Enabled = false;
        Timer1->Interval = TIMER_ESPERA_VISOR;
        }
      }
}
//---------------------------------------------------------------------------

String StringReplace(String Str, String What, String For)
{
int p = Str.Pos(What);
if (p==0)
  return Str;

Str = Str.Delete(p, What.Length());
Str = Str.Insert(For, p);

return Str;
}

__fastcall TfmShell::TfmShell(TComponent* Owner)
        : TForm(Owner)
{
lastks=0;
SelScreen=0;
TemBeep = 0;
PROC_BEEP = 0;
SHRINK = 0;
WebServerOk = false;
glAlpha = 0;
tbTelas->Enabled = false;
tbEventos->Enabled = false;
tbHist->Enabled = false;
tbAnormais->Enabled = false;
tbTabular->Enabled = false;
tbCurvas->Enabled = false;
WEBSERVER_DATE_FMT = "yy/mm/dd hh:nn:ss";

// vai para o diretório do executável
chdir(ExtractFilePath(Application->ExeName).c_str());

// Log user login
// register the system start and (OS) username
#pragma warn -aus
TCHAR UserName[200];
UserName[0]='\0';
DWORD size = sizeof(UserName) - 1;
GetUserName((TCHAR*)UserName, &size);
Loga( (String)"System Started. User=" + (String)UserName );

TIniFile *pIni = new TIniFile(ARQ_CONF_IHM);
if ( pIni != NULL )
  {
  String BROWSER_OPTIONS = pIni->ReadString("RUN","BROWSER_OPTIONS", "--process-per-site --no-sandbox --disable-popup-blocking --no-proxy-server --bwsi --disable-extensions --disable-sync --no-first-run").Trim();
  VISOR_EVENTOS = StringReplace(pIni->ReadString("RUN","EVENTS_VIEWER", "").Trim(), "--bopt", BROWSER_OPTIONS);
  VISOR_TABULAR = StringReplace(pIni->ReadString("RUN","TABULAR_VIEWER", "").Trim(), "--bopt", BROWSER_OPTIONS);
  VISOR_TELAS = StringReplace(pIni->ReadString("RUN","SCREEN_VIEWER", "").Trim(), "--bopt", BROWSER_OPTIONS);
  VISOR_TENDENCIAS = StringReplace(pIni->ReadString("RUN","TREND_VIEWER", "").Trim(), "--bopt", BROWSER_OPTIONS);
  VISOR_CURVAS = StringReplace(pIni->ReadString("RUN","CURVES_VIEWER", "").Trim(), "--bopt", BROWSER_OPTIONS);
  VISOR_DOCS = StringReplace(pIni->ReadString("RUN","DOCS_VIEWER", "").Trim(), "--bopt", BROWSER_OPTIONS);
  VISOR_LOGS = StringReplace(pIni->ReadString("RUN","LOGS_VIEWER", "").Trim(), "--bopt", BROWSER_OPTIONS);

  DELAY = pIni->ReadInteger( "RUN", "DELAY", 15 );
  INTERVAL = pIni->ReadInteger( "RUN", "INTERVAL", 5 );
  RUN_NUM = 0;
  for ( int i = 0; i < MAX_RUN; i++ )
    {
    String s;
    s = s.sprintf( "RUN%02d", i );
    RUN[i] = pIni->ReadString( "RUN", s.c_str(), "" );
    if ( RUN[i] == "" )
      break;
    else
      RUN_NUM = i + 1;
    }

  delete pIni;
  }

pIni = new TIniFile(ARQ_CONF);
if ( pIni != NULL )
  {
  int LAST_SERVER = 2;
  String SERVER1 = "127.0.0.1";
  String SERVER2 = "127.0.0.1";

  // server 1 and 2 addresses
  SERVER1 = pIni->ReadString( "HMISHELL", "SERVER1", SERVER1 ).Trim();
  SERVER2 = pIni->ReadString( "HMISHELL", "SERVER2", SERVER2 ).Trim();

  // server 1 and 2 addresses for a OS logged in user [UserName]
  SERVER1 = pIni->ReadString( "HMISHELL", (String)"SERVER1_" + (String)UserName, SERVER1 ).Trim();
  SERVER2 = pIni->ReadString( "HMISHELL", (String)"SERVER2_" + (String)UserName, SERVER2 ).Trim();

  LAST_SERVER = pIni->ReadInteger( "HMISHELL", "LAST_SERVER", LAST_SERVER );
  PROC_BEEP = pIni->ReadInteger( "HMISHELL", "BEEP", PROC_BEEP );
  if ( LAST_SERVER == 2 )
    {
    LAST_SERVER = 1;
    REMOTE_HOST = SERVER1;
    }
  else
    {
    LAST_SERVER = 2;
    REMOTE_HOST = SERVER2;
    }

  pIni->WriteInteger( "HMISHELL", "LAST_SERVER", LAST_SERVER );

  int red = pIni->ReadInteger( "HMISHELL", "BGCOLOR_RED", -1 );
  int green = pIni->ReadInteger( "HMISHELL", "BGCOLOR_GREEN", -1 );
  int blue = pIni->ReadInteger( "HMISHELL", "BGCOLOR_BLUE", -1 );
  if ( red != -1 && green != -1 && blue != -1 )
    {
    // set desktop background
    int aElements[1] = {COLOR_BACKGROUND};
    DWORD aNewColors[1];
    aNewColors[0] = RGB(red, green, blue);
    SetSysColors(1, aElements, aNewColors);
    }

  delete pIni;
  }

try
  {
  String Rq = (String)"http://" +
              (String)REMOTE_HOST + (String)":" +
              (String)REMOTE_PORT + (String)"/" +
              (String)SHELLAPIURL + (String)"?U=" + (String)UserName + (String)"&O=Login";
  NMHTTP1->TimeOut = 0;
  NMHTTP1->Get( Rq );
  } catch ( Exception &E ) {}

pIni = new TIniFile( ARQ_CONFI18N );
if ( pIni != NULL )
  {
  tbTelas->Caption = pIni->ReadString("HMISHELL", "BUTTON_VIS_SCR", "Screen").Trim();
  tbEventos->Caption = TXT_BOTAO_EVENTOS = pIni->ReadString("HMISHELL", "BUTTON_VIS_SOE", "Events").Trim();
  tbHist->Caption = TXT_BOTAO_HISTORICO = pIni->ReadString("HMISHELL", "BUTTON_VIS_HISTSOE", "Hist").Trim();
  tbTabular->Caption = TXT_BOTAO_TABULAR = pIni->ReadString("HMISHELL", "BUTTON_VIS_TAB", "Tabular").Trim();
  tbAnormais->Caption = TXT_BOTAO_ANORMAIS = pIni->ReadString("HMISHELL", "BUTTON_VIS_ANORM", "Alarms").Trim();
  tbCurvas->Caption = TXT_BOTAO_CURVAS = pIni->ReadString("HMISHELL", "BUTTON_VIS_CURV", "Curves").Trim();
  tbSilencia->Caption = pIni->ReadString("HMISHELL", "BUTTON_SILENT", "Silence").Trim();
  tbSair->Caption = pIni->ReadString("HMISHELL", "BUTTON_EXIT", "Options...").Trim();

  tbTelas->Hint = pIni->ReadString("HMISHELL", "BUTTON_VIS_SCR_HINT", "Screen").Trim();
  tbEventos->Hint = pIni->ReadString("HMISHELL", "BUTTON_VIS_SOE_HINT", "Events").Trim();
  tbHist->Hint = pIni->ReadString("HMISHELL", "BUTTON_VIS_HISTSOE_HINT", "Historic").Trim();
  tbTabular->Hint = pIni->ReadString("HMISHELL", "BUTTON_VIS_TAB_HINT", "Tabular").Trim();
  tbAnormais->Hint = pIni->ReadString("HMISHELL", "BUTTON_VIS_ANORM_HINT", "Alarms").Trim();
  tbCurvas->Hint = pIni->ReadString("HMISHELL", "BUTTON_VIS_CURV_HINT", "Curves").Trim();
  tbSilencia->Hint = pIni->ReadString("HMISHELL", "BUTTON_SILENT_HINT", "Silence").Trim();
  tbSair->Hint = pIni->ReadString("HMISHELL", "BUTTON_EXIT_HINT", "Options...").Trim();

  TITULO_VISOR_TELAS = pIni->ReadString("HMISHELL", "TITLE_VIS_SCR", "Screen Viewer - OSHMI").Trim();
  TITULO_VISOR_EVENTOS = pIni->ReadString("HMISHELL", "TITLE_VIS_SOE", "Events Viewer - OSHMI").Trim();
  TITULO_VISOR_HISTEVENTOS = pIni->ReadString("HMISHELL", "TITLE_VIS_HISTSOE", "Events Viewer - Hist").Trim();
  TITULO_VISOR_TABULAR = pIni->ReadString("HMISHELL", "TITLE_VIS_TAB", "Tabular Viewer - OSHMI").Trim();
  TITULO_VISOR_ANORMALIDADES = pIni->ReadString("HMISHELL", "TITLE_VIS_ANORM", "Alarms Viewer - OSHMI").Trim();
  TITULO_VISOR_CURVAS = pIni->ReadString("HMISHELL", "TITLE_VIS_CURV", "Curves Viewer - OSHMI").Trim();

  for ( int i = 0; i < MAX_WIN_TITLE_CTL; i++ )
    {
    String s;
    s = s.sprintf( "WNDTITEQ%d", i + 1 );
    WNDTITEQ[i] = pIni->ReadString( "HMISHELL", s.c_str(), "" );
    if ( s == "" )
      break;
    }
  for ( int i = 0; i < MAX_WIN_TITLE_CTL; i++ )
    {
    String s;
    s = s.sprintf( "WNDTITCT%d", i + 1 );
    WNDTITCT[i] = pIni->ReadString( "HMISHELL", s.c_str(), "" );
    if ( s == "" )
      break;
    }

  WEBSERVER_DATE_FMT = pIni->ReadString( "WEBSERVER", "DATE_FMT", WEBSERVER_DATE_FMT );

  delete pIni;
  }

// chama o monitor de processos
ExecExternApp( "mon_proc.exe" );

// Limpa cache do navegador
ExecExternApp( "hidec.exe cache_clean.bat" );

// se o servidor for remoto
if ( REMOTE_HOST != "127.0.0.1" )
  {
  // ajusta atalhos para servidor remoto
  VISOR_EVENTOS = StrReplace( VISOR_EVENTOS, "127.0.0.1", REMOTE_HOST );
  VISOR_TABULAR = StrReplace( VISOR_TABULAR, "127.0.0.1", REMOTE_HOST );
  VISOR_TELAS = StrReplace( VISOR_TELAS, "127.0.0.1", REMOTE_HOST );
  VISOR_CURVAS = StrReplace( VISOR_CURVAS, "127.0.0.1", REMOTE_HOST );
  VISOR_TENDENCIAS = StrReplace( VISOR_TENDENCIAS, "127.0.0.1", REMOTE_HOST );
  VISOR_DOCS = StrReplace( VISOR_DOCS, "127.0.0.1", REMOTE_HOST );
  VISOR_LOGS = StrReplace( VISOR_LOGS, "127.0.0.1", REMOTE_HOST );

  // busca o lista telas remoto
  // bool itemp = NMHTTP1->InputFileMode;
  try
     {
     NMHTTP2->InputFileMode =  true;
     NMHTTP2->Body = LISTA_TELAS_FILE;
     NMHTTP2->Header = "";
     NMHTTP2->TimeOut = 2000;
     String Rq = (String)"http://" +
                 (String)REMOTE_HOST + (String)":" +
                 (String)REMOTE_PORT + (String)"/" +
                 (String)LISTA_TELAS_WEB;
     NMHTTP2->Get( Rq );
     // NMHTTP2->InputFileMode =  false;
     } catch ( Exception &E ) {}

  // NMHTTP1->InputFileMode =  itemp;
  }

  // enable timer that process alarm beep
  Timer2->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbSairClick(TObject *Sender)
{
fmSair->Visible = !fmSair->Visible;
FormScreenList->Hide();
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbCurvasClick(TObject *Sender)
{
// tenta encontrar a janela do histview
JANELA_AENCONTRAR = TITULO_VISOR_CURVAS;
EnumWindows( (int (__stdcall *)()) enumwndprc, 0 );
if ( JANELA_ENCONTRADA == 0 ) // se não achou executa
  {
  ExecExternApp( VISOR_CURVAS.c_str() );
  ToolBar1->Enabled = false;
  Timer1->Interval = TIMER_ESPERA_VISOR;
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbEventosClick(TObject *Sender)
{
JANELA_AENCONTRAR = TITULO_VISOR_EVENTOS;
EnumWindows( (int (__stdcall *)()) enumwndprc, 0 );
Linux_FindWindow( JANELA_AENCONTRAR, VISOR_EVENTOS );
if ( JANELA_ENCONTRADA == 0 ) // se não achou executa
  {
  ExecExternApp( VISOR_EVENTOS.c_str() );
  ToolBar1->Enabled = false;
  Timer1->Interval = TIMER_ESPERA_VISOR;
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbHistClick(TObject *Sender)
{
JANELA_AENCONTRAR = TITULO_VISOR_HISTEVENTOS;
EnumWindows( (int (__stdcall *)()) enumwndprc, 0 );
Linux_FindWindow( JANELA_AENCONTRAR, VISOR_EVENTOS + (String)"?MODO=4" );
if ( JANELA_ENCONTRADA == 0 ) // se não achou executa
  {
  ExecExternApp( ( VISOR_EVENTOS + (String)"?MODO=4" ).c_str() );
  ToolBar1->Enabled = false;
  Timer1->Interval = TIMER_ESPERA_VISOR;
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbAnormaisClick(TObject *Sender)
{
JANELA_AENCONTRAR = TITULO_VISOR_ANORMALIDADES;
EnumWindows( (int (__stdcall *)()) enumwndprc, 0);
Linux_FindWindow( JANELA_AENCONTRAR, VISOR_TABULAR + (String)"?SELMODULO=TODOS_ANORMAIS" );
if ( JANELA_ENCONTRADA == 0 ) // se não achou executa
  {
  ExecExternApp( (VISOR_TABULAR + (String)"?SELMODULO=TODOS_ANORMAIS").c_str() );
  ToolBar1->Enabled = false;
  Timer1->Interval = TIMER_ESPERA_VISOR;
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbTabularClick(TObject *Sender)
{
JANELA_AENCONTRAR = TITULO_VISOR_TABULAR;
EnumWindows( (int (__stdcall *)()) enumwndprc, 0 );
Linux_FindWindow( JANELA_AENCONTRAR, VISOR_TABULAR );
if ( JANELA_ENCONTRADA == 0 ) // se não achou executa
  {
  ExecExternApp( VISOR_TABULAR.c_str() );
  ToolBar1->Enabled = false;
  Timer1->Interval = TIMER_ESPERA_VISOR;
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbSilenciaClick(TObject *Sender)
{
SilenciaBeep();
try
  {
  String Rq = (String)"http://" +
              (String)REMOTE_HOST + (String)":" +
              (String)REMOTE_PORT + (String)"/" +
              (String)SHELLAPIURL + (String)"?Z=1";
  NMHTTP1->TimeOut = 0;
  NMHTTP1->Get( Rq );
  } catch ( Exception &E ) {}
ToolBar1->Enabled = false;
Timer1->Interval = TIMER_ESPERA_VISOR;
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::Timer1Timer(TObject *Sender)
{
  static unsigned int cntseg = 0;
  static bool done = false;
  ToolBar1->Enabled = true;

  // após o webserver estar ok espera um tempo e dispara os programas configurados no HMI.ini (RUN00, RUN01, ...)
  if ( WebServerOk && !done )
    {
    cntseg++;

    if ( cntseg <= (unsigned)DELAY + RUN_NUM * INTERVAL )
      {
      for ( int i = 0; i < RUN_NUM; i++ )
        if ( ( cntseg == (unsigned)DELAY + i * INTERVAL) && RUN[i] != "" )
           {
             ExecExternApp( RUN[i].c_str() );
           }
      }
    else
      {
      done = true;
      }
    }

  CheckWinKey();
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::shMoveMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
TPoint pt;
if ( Shift.Contains(ssLeft) )
  {
  pt.x=X; pt.y=Y;
  pt=ClientToScreen(pt);
  Top=pt.y-shMove->Height/2;
  Left=pt.x-shMove->Width/2;

  TIniFile *pIni=new TIniFile(ARQ_CONF);
  if ( pIni != NULL )
    {
    pIni->WriteInteger( "HMISHELL", "POS_Y", Top );
    pIni->WriteInteger( "HMISHELL", "POS_X", Left );
    delete pIni;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
switch (Key)
 {
 case VK_RWIN: // WIN Key
 case VK_LWIN: // WIN Key
   FormScreenList->Visible = !FormScreenList->Visible;
   GetAsyncKeyState(VK_LWIN);
   GetAsyncKeyState(VK_RWIN);
   lastks = 3; 
   break;
 case VK_F1: // ajuda
   WinExec( fmSair->DOC_AJUDA.c_str(), SW_SHOWNORMAL );
   break;
 case VK_F2: // apaga toda a lista eventos
   break;
 case VK_F8: // reconhece tudo
   break;
 case VK_F9: // silencia
   tbSilenciaClick( NULL );
   break;
 case VK_F12: // sair
   FormScreenList->Hide();
   fmSair->Show();
   break;
 case '0':
 case '1':
 case '2':
 case '3':
 case '4':
 case '5':
 case '6':
 case '7':
 case '8':
 case '9':
   String S = TELAS[Key-'0'];
   JANELA_AENCONTRAR = S.SubString( 1, S.Pos( "[" ) - 1 ).Trim() + (String)" - " + TITULO_VISOR_TELAS;
   EnumWindows( (int (__stdcall *)()) enumwndprc, 0);
   Linux_FindWindow( JANELA_AENCONTRAR, VISOR_TELAS + (String)"?INDTELA=" + (String)(Key-'0') );
   if ( JANELA_ENCONTRADA == 0 ) // se não achou executa
     {
     ExecExternApp( (VISOR_TELAS + (String)"?INDTELA=" + (String)(Key-'0')).c_str() );
     ToolBar1->Enabled = false;
     Timer1->Interval = TIMER_ESPERA_VISOR;
     }
   FormScreenList->Hide();
   break;
 }

FormMouseMove(Sender,Shift,0,0);
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::FormCloseQuery(TObject *Sender, bool &CanClose)
{
if ( fmSair->Fechando )
  {
  CanClose=true;
  }
else
if ( !fmSair->PasswdTest( fmSair->edSenha->Text.c_str() ) )
  {
  CanClose=false;
  }
else
  {
  fmShell->tbTelas->Enabled = false;
  fmShell->tbTelas->Enabled = false;
  fmShell->tbEventos->Enabled = false;
  fmShell->tbHist->Enabled = false;
  fmShell->tbAnormais->Enabled = false;
  fmShell->tbTabular->Enabled = false;
  fmShell->tbCurvas->Enabled = false;
  fmShell->tbSilencia->Enabled = false;
  fmShell->Timer1->Enabled = false;
  fmShell->Timer2->Enabled = false;
  fmShell->Timer3->Enabled = false;
  fmShell->Timer4->Enabled = false;
  fmShell->Timer5->Enabled = false;
  fmShell->Timer6->Enabled = false;
  fmShell->Timer7->Enabled = false;
  fmShell->Timer8->Enabled = false;
  fmShell->Timer9->Enabled = false;
  fmShell->Timer10->Enabled = false;
  fmShell->Timer11->Enabled = false;
  Sleep(250);
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::Timer2Timer(TObject *Sender)
{
static int Cnt = 0;

// Processa beep
if ( PROC_BEEP )
if ( ! ( Cnt % 2 ) || ( HaBeepAtivo() == BEEP_CRITICO ) )
  if ( HaBeepAtivo() )
     {
     if ( HaBeepAtivo() == BEEP_CRITICO )
       MessageBeep( MB_ICONASTERISK );
     else
       MessageBeep( MB_OK );
     }

TDateTime nw = Now();
String dttm = FormatDateTime( WEBSERVER_DATE_FMT, nw );
int posspc= dttm.Pos(" ");

String dt = dttm.SubString( 1, posspc );
if ( dt != Label1->Caption )
  Label1->Caption = dt;

Label2->Caption = dttm.SubString( posspc + 1, 99 );

CheckWinKey();

Cnt++;
}

//---------------------------------------------------------------------------

void __fastcall TfmShell::Timer3Timer(TObject *Sender)
{
try
   {
   String Rq = (String)"http://" +
               (String)REMOTE_HOST + (String)":" +
               (String)REMOTE_PORT + (String)"/" +
               (String)SHELLAPIURL + (String)"?P=99999";
   NMHTTP1->TimeOut = 1000;
   NMHTTP1->Get( Rq );
   }
catch ( Exception &E )
   {
   WebServerOk = false;
   tbTelas->Enabled = false;
   tbEventos->Enabled = false;
   tbHist->Enabled = false;
   tbAnormais->Enabled = false;
   tbTabular->Enabled = false;
   tbCurvas->Enabled = false;
   }

CheckWinKey();
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::NMHTTP1Success(CmdType Cmd)
{
WebServerOk = true;
tbTelas->Enabled = true;
tbEventos->Enabled = true;
tbHist->Enabled = true;
tbAnormais->Enabled = true;
tbTabular->Enabled = true;
tbCurvas->Enabled = true;
if ( NMHTTP1->Body.Pos("'beep': 1") )
  AtivaBeep( BEEP_NORMAL );
else
if ( NMHTTP1->Body.Pos("'beep': 2") )
  AtivaBeep( BEEP_CRITICO );
else
  SilenciaBeep();
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::NMHTTP1Failure(CmdType Cmd)
{
WebServerOk = false;
tbTelas->Enabled = false;
tbEventos->Enabled = false;
tbHist->Enabled = false;
tbAnormais->Enabled = false;
tbTabular->Enabled = false;
tbCurvas->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::Timer4Timer(TObject *Sender)
{
TGroupBox *GroupBox=NULL, *grpboxant=NULL;
TLabel *Lbl;
Timer4->Enabled = false;
int topmargin=4;
int leftmargin=4;
int bzsize = 15;
int lblspc = 2;

if ( REMOTE_HOST == "127.0.0.1" )
  fmSair->lbServer->Caption = "";
else
  fmSair->lbServer->Caption = REMOTE_HOST;

// convert screen_list UTF8 encoded to native windows encoding
WinExec("..\\bin\\convert-screenlist-from-utf8.bat", SW_HIDE);
Sleep(500);

FILE * fp;
fp = fopen( LISTA_TELAS_FILE_NONUTF8.c_str(), "rt" );
if (fp == NULL)
  fp = fopen( LISTA_TELAS_FILE.c_str(), "rt" );

if (fp)
  {
  char buff[10000];
  char * rp;
  int index = 1;
  int grpcnt=0, grpindex=0, cumheight=0, maxwidth=0, maxheight=0;

  // lista as telas disponíveis e coloca no menu popup de telas

  popTelas->Items->Clear();
  while ( !feof( fp ) )
    {
      rp = fgets( buff, sizeof(buff) - 1, fp );
      if ( rp == NULL )
        break; // erro ou fim de arquivo

      String S = buff;
      S = S.Trim();
      if ( S == "" )
        continue;  // linha vazia

      S = S.SubString( S.Pos( "opttxt.push('" ) + 13, 299 );
      S = S.SubString( 1, S.Pos( "')" ) - 1 );
      S = S.Trim();

      if ( S == "" ) // não achou tela pelo formato opttxt.push
        {
        // tenta pelo outro formato (option/optgroup)

        S = buff;
        S = S.Trim();
        if ( S.Pos( "<option value=" ) == 0 )
          {
          // new group
          int grppos = S.Pos( "<optgroup label='" );
          if (grppos>0)
            { // detected new group
            if ( grpboxant != NULL )
              cumheight += grpboxant->Height;
            grpboxant = GroupBox;
            GroupBox = new TGroupBox( FormScreenList );
            GroupBox->Parent = FormScreenList;
            GroupBox->Font->Size = 16;
            GroupBox->Color = (TColor)CLR_SCR_GRP;
            GroupBox->Caption = S.SubString(17+grppos, S.Pos("'>")-18);
            GroupBox->Width = GroupBox->Font->Size * 20;
            if ( grpboxant != NULL )
              {
              GroupBox->Left = grpboxant->Left;
              GroupBox->Top = topmargin + grpboxant->Top + grpboxant->Height;
              }
            else
              {
              GroupBox->Left = leftmargin;
              GroupBox->Top = topmargin;
              }

            GroupBox->Visible = true;

            grpindex = 0;
            grpcnt++;
            }

          continue;
          }
        else
          {
          S = S.SubString( S.Pos( ">" ) + 1, 299 );
          S = S.SubString( 1, S.Pos( "<" ) - 1 );
          }

        S = S.Trim();
        if ( S == "" )
          continue;  // não achou tela
        }

      if (GroupBox != NULL)
        {
        Lbl = new TLabel(GroupBox);
        Lbl->Parent = GroupBox;
        Lbl->Font->Color = (TColor)CLR_SCR_TXT_NOTOPENED;
        Lbl->Font->Style = TFontStyles();
        Lbl->Font->Size = 14;
        Lbl->Color = (TColor)CLR_SCR_BGD_NOTSELECTED;
        Lbl->Caption = S + S.StringOfChar(' ', (30-S.Length()>0)?30-S.Length():0 );
        Lbl->Top = Lbl->Height + grpindex*(Lbl->Height+lblspc);
        Lbl->Left = leftmargin + 4;
        Lbl->Cursor = crHandPoint;
        Lbl->Tag = index;
        Lbl->OnClick = lblScreenClick;
        Lbl->OnMouseMove = lblScreenMouseMove;
        GroupBox->Height = Lbl->Top + Lbl->Height + 5 + lblspc;
        if ( (GroupBox->Top + GroupBox->Height) > (FormScreenList->Height - bzsize - topmargin) )
          {
          GroupBox->Left = leftmargin + GroupBox->Left + GroupBox->Width;
          GroupBox->Top = topmargin;
          }
        FormScreenList->Width = leftmargin + GroupBox->Left + GroupBox->Width + bzsize;
        }
      else
        {
        Lbl = new TLabel(FormScreenList);
        Lbl->Parent = FormScreenList;
        Lbl->Font->Color = (TColor)CLR_SCR_TXT_NOTOPENED;
        Lbl->Font->Style = TFontStyles();
        Lbl->Font->Size = 14;
        Lbl->Color = (TColor)CLR_SCR_BGD_NOTSELECTED;
        Lbl->Caption = S + S.StringOfChar(' ', 30-S.Length());
        Lbl->Top = lblspc + (index-1)*(Lbl->Height+lblspc);
        Lbl->Left = leftmargin + 4;
        Lbl->Cursor = crHandPoint;
        Lbl->Tag = index;
        Lbl->OnClick = lblScreenClick;
        Lbl->OnMouseMove = lblScreenMouseMove;
        FormScreenList->Width = leftmargin + Lbl->Width + bzsize;
        }

      if ( Lbl->Top > maxheight )
        maxheight=Lbl->Top;
      ScreenItems[index-1] = Lbl;
      NumScreens=index;

      TMenuItem *NewItem;
      NewItem = new TMenuItem( NULL );
      NewItem->Caption = S;
      if ( index == MAX_ATALHOS_TELAS ) // coloca o atalho de '0' (10o atalho) na primeira tela
        TELAS[0]=S;
      if ( index < MAX_ATALHOS_TELAS )
        TELAS[index] = S;
      NewItem->OnClick = popTelasItemsClick;
      NewItem->Tag = index;
      popTelas->Items->Add( NewItem );

      index++;
      grpindex++;
    }
  FormScreenList->VertScrollBar->Increment=Lbl->Height+lblspc;
  FormScreenList->VertScrollBar->Range=maxheight + Lbl->Height;
  fclose( fp );
  ShowSelScreen();
  ScreenItems[0]->Left = leftmargin + 4;
  }

// Dá um tempo após a inicialização para posicionar a janela.
// Em casos de 2 monitores às vezes as coordenadas mudam após a inicialização do Windows.
TIniFile *pIni = new TIniFile(ARQ_CONF);
if ( pIni != NULL )
  {
  Top=pIni->ReadInteger( "HMISHELL", "POS_Y", 0 );
  Left=pIni->ReadInteger( "HMISHELL", "POS_X", 0 );
  TRANSPARENCIA = pIni->ReadInteger( "HMISHELL", "TRANSPARENCY", 1 );
  // SHRINK = pIni->ReadInteger( "HMISHELL", "SHRINK", 0 );
  delete pIni;
  }

if ( TRANSPARENCIA )
  {
  // faz a janela tranparente (pouco)
  SetWindowLong( Handle, GWL_EXSTYLE, GetWindowLong(Handle, GWL_EXSTYLE) |  WS_EX_LAYERED );

  SetLayeredWindowAttributes( Handle, 0, 0, LWA_ALPHA);
  ShowWindow(Handle, SW_SHOW);
  RedrawWindow(Handle, NULL, NULL, RDW_UPDATENOW);

  for ( glAlpha = 0; glAlpha < glAlphaMax; glAlpha += 20 )
      {
         // Adjust the translucency
         SetLayeredWindowAttributes( Handle, 0, glAlpha, LWA_ALPHA );
         ::Sleep(50); // Wait
      }

  glAlpha = glAlphaMax;
  SetLayeredWindowAttributes( Handle, 0, glAlpha, LWA_ALPHA );
  ShowWindow( Handle, SW_SHOW );
  RedrawWindow( Handle, NULL, NULL, RDW_UPDATENOW );

  Timer5->Interval = 20000;
  Timer5->Enabled = true;
  }
}
//---------------------------------------------------------------------------

// when mouse moved over, will show or sustain toolbar   
void __fastcall TfmShell::FormMouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
if ( Timer9->Enabled || Timer10->Enabled )
  return;

if ( !ToolBar1->Visible )
  Timer9->Enabled = true; // will show toolbar

// faz a janela menos transparente
if ( TRANSPARENCIA && glAlpha != glAlphaMax )
  {
  ShowWindow( Handle, SW_SHOW );
  RedrawWindow( Handle, NULL, NULL, RDW_UPDATENOW );

  for ( ; glAlpha < glAlphaMax; glAlpha += 20 )
      {
         // Adjust the translucency
         SetLayeredWindowAttributes( Handle, 0, glAlpha, LWA_ALPHA );
         ::Sleep(50); // Wait
      }

  glAlpha = glAlphaMax;
  SetLayeredWindowAttributes( Handle, 0, glAlpha, LWA_ALPHA );
  ShowWindow( Handle, SW_SHOW );
  RedrawWindow( Handle, NULL, NULL, RDW_UPDATENOW );
  }

// timeout to shring and make more transparent
Timer5->Enabled = false;
Timer5->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::Timer5Timer(TObject *Sender)
{
if ( fmSair->Visible )
   {
   return;
   }

// fecha o menu popup
PostMessage( popTelas->WindowHandle, WM_LBUTTONDOWN, MK_LBUTTON, 0 );
PostMessage( popTelas->WindowHandle, WM_LBUTTONUP, MK_LBUTTON, 0 );

// faz a janela mais transparente
if ( TRANSPARENCIA )
  {
  ShowWindow( Handle, SW_SHOW );
  RedrawWindow( Handle, NULL, NULL, RDW_UPDATENOW );
  for ( glAlpha = glAlphaMax; glAlpha  > glAlphaMin; glAlpha -= 20 )
      {
         // Adjust the translucency
         SetLayeredWindowAttributes( Handle, 0, glAlpha, LWA_ALPHA );
         ::Sleep(50); // Wait
      }
 glAlpha = glAlphaMin;
 SetLayeredWindowAttributes(Handle, 0, glAlpha, LWA_ALPHA);
 ShowWindow(Handle, SW_SHOW);
 RedrawWindow(Handle, NULL, NULL, RDW_UPDATENOW);
 }

SetWindowPos(Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
 
Timer5->Interval = 10000;
Timer5->Enabled = false;
Timer10->Enabled = true; // shrink toolbar
}
//---------------------------------------------------------------------------

// repassa o evento de mouse move para o form
void __fastcall TfmShell::tbTelasMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::ToolBar1MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbEventosMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbHistMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbAnormaisMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbTabularMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbCurvasMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbSilenciaMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbSairMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::FormCreate(TObject *Sender)
{
// dá um tempo para fazer o posicionamento da janela.
Timer4->Interval = 100;
Timer4->Enabled = true;
}
//---------------------------------------------------------------------------

// ve se há erro de memória virtual, se tiver, fecha navegador
void __fastcall TfmShell::Timer6Timer(TObject *Sender)
{
static int encontrada = 0;

    JANELA_AENCONTRAR = "Memória Virtual";
    EnumWindows( (int (__stdcall *)()) enumwndprc, 0 );
    if ( JANELA_ENCONTRADA ) // se encontrou
      {
      if ( encontrada != 1 )
        {
        ExecExternApp( "taskkill /F /IM chrome.exe" );
        SendMessage( JANELA_ENCONTRADA, WM_CLOSE, 0, 0 );
        }
      encontrada = 1;
      }
    else
      {
      if ( encontrada == 1 )
        encontrada = 0;
      }


    JANELA_AENCONTRAR = "Virtual Memory";
    EnumWindows( (int (__stdcall *)()) enumwndprc, 0);
    if ( JANELA_ENCONTRADA ) // se encontrou
      {
      if ( encontrada != 2 )
        {
        ExecExternApp( "taskkill /F /IM chrome.exe" );
        SendMessage( JANELA_ENCONTRADA, WM_CLOSE, 0, 0 );
        }
      encontrada = 2;
      }
    else
      {
      if ( encontrada == 2 )
        encontrada = 0;
      }

      CheckWinKey();
}
//---------------------------------------------------------------------------

// faz redraw da tela a cada 5s durante o primeiro minuto
// evita problema de ficar preto na inicialização do Windows
void __fastcall TfmShell::Timer7Timer(TObject *Sender)
{
static int cnt = 0;

  Repaint();
  ToolBar1->Repaint();
  tbTelas->Repaint();
  tbEventos->Repaint();
  tbHist->Repaint();
  tbAnormais->Repaint();
  tbTabular->Repaint();
  tbCurvas->Repaint();
  tbSilencia->Repaint();
  tbSair->Repaint();
  ToolButton1->Repaint();
  ToolButton2->Repaint();
  ToolButton3->Repaint();
  ToolButton4->Repaint();
  ToolButton5->Repaint();
  ToolButton6->Repaint();
  ToolButton7->Repaint();
  Panel1->Repaint();
  Label1->Repaint();
  Label2->Repaint();
  shMove->Repaint();

  cnt++;
  if ( cnt >= 12)
    Timer7->Enabled = false;
}
//---------------------------------------------------------------------------

// As duas próximas funções procuram periodicamente uma janela tipo "Abrir" ou "Salvar Como" e a fecham se for encotrada.
// Isto evita que o usuário do navegador possa abrir estes diálogos para executar o desbloqueio da máquina.
// Procura ainda janela de desenvolvedor do chrome e janela solta do chrome (ou chromium).
HWND WINDOW_ENCONTRADA = 0;

// No Wine só encontra janelas do wine, não percebe as nativas do linux
bool CALLBACK enumwndprc1( HWND hWnd, LPARAM lParam )
{
char bufstr[1000];
bufstr[0] = 0;

WINDOW_ENCONTRADA = 0;

  GetWindowText( hWnd, bufstr, 998 );
  if ( bufstr[0] != 0 )
  {
  String S = bufstr;

  bool found = false;
  for ( int i = 0; i < MAX_WIN_TITLE_CTL; i++ )
    {
    if ( S == "" )
      {
      break;
      }
    if ( S == WNDTITEQ[i] )
      {
      found = true;
      break;
      }
    }
  for ( int i = 0; i < MAX_WIN_TITLE_CTL; i++ )
    {
    if ( S == "" )
      {
      break;
      }
    if ( S.Pos( WNDTITCT[i] ) > 0 )
      {
      found = true;
      break;
      }
    }

  if ( found )
    { // achou
      WINDOW_ENCONTRADA = hWnd;
      return 0;
    }
  }

return 1;
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::Timer8Timer(TObject *Sender)
{
String S;

// fecha janelas proibidas, exceto quando estiver com login administrativo (senha válida)
if ( !fmSair )
  return;

if ( !fmSair->PasswdTest( fmSair->edSenha->Text.c_str()) )
  {
  EnumWindows( (int (__stdcall *)()) enumwndprc1, 0 );
  if ( WINDOW_ENCONTRADA != 0 ) // se achou fecha
    SendMessage( WINDOW_ENCONTRADA, WM_CLOSE, 0, 0 );
  }

JANELA_AENCONTRAR = TITULO_VISOR_EVENTOS;
EnumWindows( (int (__stdcall *)()) enumwndprc2, 0);
if ( tbEventos->Marked != (bool)JANELA_ENCONTRADA )
  tbEventos->Marked = (bool)JANELA_ENCONTRADA;
/*
if ( JANELA_ENCONTRADA )
  {
  S =  "[ " + TXT_BOTAO_EVENTOS + " ]";
  }
else
  {
  S =  ". " + TXT_BOTAO_EVENTOS + " .";
  }
if ( S !=  tbEventos->Caption )
  tbEventos->Caption = S;
*/
JANELA_AENCONTRAR = TITULO_VISOR_HISTEVENTOS;
EnumWindows( (int (__stdcall *)()) enumwndprc2, 0);
if ( tbHist->Marked != (bool)JANELA_ENCONTRADA )
  tbHist->Marked = (bool)JANELA_ENCONTRADA;
/*
if ( JANELA_ENCONTRADA )
  {
  S =  "[ " + TXT_BOTAO_HISTORICO + " ]";
  }
else
  {
  S =  ". " + TXT_BOTAO_HISTORICO + " .";
  }
if ( S !=  tbHist->Caption )
  tbHist->Caption = S;
*/
JANELA_AENCONTRAR = TITULO_VISOR_ANORMALIDADES;
EnumWindows( (int (__stdcall *)()) enumwndprc2, 0);
if ( tbAnormais->Marked != (bool)JANELA_ENCONTRADA )
  tbAnormais->Marked = (bool)JANELA_ENCONTRADA;
/*
if ( JANELA_ENCONTRADA )
  {
  S =  "[ " + TXT_BOTAO_ANORMAIS + " ]";
  }
else
  {
  S =   ". " + TXT_BOTAO_ANORMAIS + " .";
  }
if ( S !=  tbAnormais->Caption )
  tbAnormais->Caption = S;
*/
JANELA_AENCONTRAR = TITULO_VISOR_TABULAR;
EnumWindows( (int (__stdcall *)()) enumwndprc2, 0);
if ( tbTabular->Marked != (bool)JANELA_ENCONTRADA )
  tbTabular->Marked = (bool)JANELA_ENCONTRADA;
/*
if ( JANELA_ENCONTRADA )
  {
  S =  "[ " + TXT_BOTAO_TABULAR + " ]";
  }
else
  {
  S =  ". " + TXT_BOTAO_TABULAR + " .";
  }
if ( S !=  tbTabular->Caption )
  tbTabular->Caption = S;
*/
JANELA_AENCONTRAR = TITULO_VISOR_CURVAS;
EnumWindows( (int (__stdcall *)()) enumwndprc2, 0);
if ( tbCurvas->Marked != (bool)JANELA_ENCONTRADA )
  tbCurvas->Marked = (bool)JANELA_ENCONTRADA;
/*
if ( JANELA_ENCONTRADA )
  {
  S =  "[ " + TXT_BOTAO_CURVAS + " ]";
  }
else
  {
  S =  ". " + TXT_BOTAO_CURVAS + " .";
  }
if ( S !=  tbCurvas->Caption )
  tbCurvas->Caption = S;
*/
for ( int i = 0; i < popTelas->Items->Count; i++ )
  {
      String S =  popTelas->Items->Items[i]->Caption.Trim();
      if ( S.Pos( "[" ) )
        S = S.SubString( 1, S.Pos( "[" ) - 1 ).Trim();
      if ( S.Pos( "{" ) )
        S = S.SubString( 1, S.Pos( "{" ) - 1 ).Trim();

      while ( S[S.Length()] == '.' )
        S = S.SubString( 1, S.Length() - 1 ).Trim();

      JANELA_AENCONTRAR = S.Trim();

      JANELA_AENCONTRAR = JANELA_AENCONTRAR + (String)" - " + TITULO_VISOR_TELAS;

      EnumWindows( (int (__stdcall *)()) enumwndprc2, 0 );
      popTelas->Items->Items[i]->Checked = (bool) JANELA_ENCONTRADA;
      if (FormScreenList!=NULL)
        ScreenItems[i]->Font->Color = JANELA_ENCONTRADA? (TColor)CLR_SCR_TXT_OPENED:(TColor)CLR_SCR_TXT_NOTOPENED;
  }

if ( Timer8->Interval != TIMER_JANELAS_PROIBIDAS )
  Timer8->Interval = TIMER_JANELAS_PROIBIDAS;

CheckWinKey();
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::Panel1MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::tbTelasClick(TObject *Sender)
{
// popTelas->Popup(fmShell->Left+tbTelas->Left,fmShell->Top+tbTelas->Top);
FormScreenList->Visible = !FormScreenList->Visible;
}
//---------------------------------------------------------------------------

// animate toolbar growing and shrinking
void __fastcall TfmShell::Timer9Timer(TObject *Sender)
{
static int i = 0;

if ( !SHRINK )
  {
  Timer9->Enabled = false;
  return;
  }

if ( Timer10->Enabled )
  {
  i = 0;
  Timer9->Enabled = false;
  return;
  }

if ( i == 0 )
  {
  if ( ToolBar1->Visible )
    {
    return;
    }

  for ( int j = 0; j < ToolBar1->ButtonCount; j++ )
    {
    ToolBar1->Buttons[j]->Visible = false;
    }
  ToolBar1->Visible = true;
  }

ToolBar1->Buttons[i]->Visible = true;
i++;

if ( i >= ToolBar1->ButtonCount )
  {
  i = 0;
  Timer9->Enabled = false;
  ToolBar1->AutoSize = false;
  ToolBar1->Width = 0;
  ToolBar1->AutoSize = true;
  }

}
//---------------------------------------------------------------------------

void __fastcall TfmShell::Timer10Timer(TObject *Sender)
{
static int i = -1;

if ( !SHRINK )
  {
  Timer10->Enabled = false;
  return;
  }

if ( Timer9->Enabled )
  {
  i = -1;
  Timer10->Enabled = false;
  return;
  }

if ( i == -1 )
  {
  i = ToolBar1->ButtonCount - 1;
  }
else
if ( i == 0 )
  {
  ToolBar1->Buttons[i]->Visible = false;
  ToolBar1->Visible = false;
  i = -1;
  Timer10->Enabled = false;
  return;
  }

ToolBar1->Buttons[i]->Visible = false;
i--;
}
//---------------------------------------------------------------------------

void __fastcall TfmShell::Label2MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------


void __fastcall TfmShell::Label1MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
FormMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void TfmShell::SilenciaBeep()
{
TemBeep = BEEP_NENHUM;
}

int TfmShell::HaBeepAtivo()
{
return TemBeep;
}

void TfmShell::AtivaBeep( int tipo )
{
if ( TemBeep == BEEP_NENHUM || TemBeep == BEEP_NORMAL )
  TemBeep = tipo;
}

// Loga mensagem em arquivo 
void Loga(String msg, String arq )
{
FILE *fp;

  if ( ( fp = fopen( arq.c_str(), "at" ) ) != NULL )
    {
    TDateTime dt;
    dt = dt.CurrentDateTime();
    String S = dt.FormatString( "yy/mm/dd hh:nn:ss(" );
    String Saux;
    S = S + Saux.sprintf( "%03d", GetTickCount() % 1000 );
    S = S + ") - ";
    fputs( S.c_str(), fp );
    fputs( msg.c_str(), fp );
    fputs( "\n", fp );
    fclose( fp );
    }
}

//---------------------------------------------------------------------------

void __fastcall TfmShell::lblScreenClick(TObject *Sender)
{
    TLabel *ClickedItem = dynamic_cast<TLabel *>(Sender);
    if ( ClickedItem )
      {
      // deixa só o que houver à esquerda do  [, { ou pontos.

      String S = ClickedItem->Caption.Trim();
      if ( S.Pos( "[" ) )
        S = S.SubString( 1, S.Pos( "[" ) - 1 ).Trim();
      if ( S.Pos( "{" ) )
        S = S.SubString( 1, S.Pos( "{" ) - 1 ).Trim();

      while ( S[S.Length()] == '.' )
        S = S.SubString( 1, S.Length() - 1 ).Trim();

      JANELA_AENCONTRAR = S.Trim();

      JANELA_AENCONTRAR = JANELA_AENCONTRAR + (String)" - " + TITULO_VISOR_TELAS;

      EnumWindows( (int (__stdcall *)()) enumwndprc, 0 );
      Linux_FindWindow( JANELA_AENCONTRAR, VISOR_TELAS + (String)"?INDTELA=" + (String)ClickedItem->Tag );
      if ( JANELA_ENCONTRADA == 0 ) // se não achou executa
        {
        ExecExternApp( (VISOR_TELAS + (String)"?INDTELA=" + (String)ClickedItem->Tag).c_str() );
        ToolBar1->Enabled = false;
        Timer1->Interval = TIMER_ESPERA_VISOR;
        }
      FormScreenList->Hide();
      }
}

void TfmShell::ShowSelScreen(void)
{
for (int i=0; i<NumScreens; i++)
  {
  if ( i == SelScreen && ScreenItems[i]!=NULL )
    {
    ScreenItems[SelScreen]->Color = (TColor)CLR_SCR_BGD_SELECTED;
    /*
    if ( (ScreenItems[SelScreen]->Top + 3*ScreenItems[i]->Height) >= (FormScreenList->Height + FormScreenList->VertScrollBar->Position) )
        {
        FormScreenList->VertScrollBar->Position = ScreenItems[i]->Top;
        }
    else
    if ( ScreenItems[SelScreen]->Top < FormScreenList->VertScrollBar->Position )
       {
       FormScreenList->VertScrollBar->Position = ScreenItems[SelScreen]->Top;
       }
    */
    }
  else
    ScreenItems[i]->Color = (TColor)CLR_SCR_BGD_NOTSELECTED;
  }
}

void __fastcall TfmShell::lblScreenMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
TLabel *ClickedItem = dynamic_cast<TLabel *>(Sender);
SelScreen = ClickedItem->Tag-1;
ShowSelScreen();
}

void __fastcall TfmShell::Timer11Timer(TObject *Sender)
{
CheckWinKey();
}

// Monitor Win Key to open and close the Screen List
void TfmShell::CheckWinKey()
{
int aks = (GetAsyncKeyState(VK_LWIN)&0x01) || (GetAsyncKeyState(VK_RWIN)&0x01);
if ( lastks==0 && aks!=0 )
  {
  if (FormScreenList!=NULL)
    {
    FormScreenList->Visible = !FormScreenList->Visible;
    }
  }

if (lastks>0)
  lastks--;
else
  lastks = aks;

if ( GetAsyncKeyState(VK_F12) && GetAsyncKeyState(VK_MENU) )
   {
   FormScreenList->Hide();
   fmSair->Show();
   }
}


void __fastcall TfmShell::FormActivate(TObject *Sender)
{
SetFocus();
SetWindowPos(Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
//---------------------------------------------------------------------------

