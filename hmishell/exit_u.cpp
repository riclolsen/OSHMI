//---------------------------------------------------------------------------
// HMI SHELL - Copyright 2008-2016 - Ricardo L. Olsen
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
#include <inifiles.hpp>
#include <stdio.h>

#include "exit_u.h"
#include "shell_u.h"
#include "sha1.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmSair *fmSair;

void TfmSair::PasswdReadDefault()
{
TIniFile *pIni = new TIniFile( ARQ_CONF );

if ( pIni != NULL )
  {
  SENHA_ADM  = pIni->ReadString( "HMISHELL", "PASSWORD", "" );
  HIDEPASSWD = pIni->ReadInteger( "HMISHELL", "HIDEPASSWD", HIDEPASSWD );
  }

if ( SENHA_ADM == "" )
  {
  SENHA_ADM = "43C753D610B45C98BC986DAC30B340BD274A0E59"; /* "oshmi" */
  }
}

void TfmSair::PasswdToHexStr( char * pswd, char * hexres )
{
  char shares[22];
  sha1( pswd, strlen(pswd), shares );
  shares[20]=0;
  hexres[0]=0;

  for ( int i=0; i<20; i++ )
    sprintf( hexres+strlen(hexres), "%02X", (unsigned char)shares[i] );
}

void TfmSair::PasswdWrite( char * pswd )
{
TIniFile *pIni = new TIniFile( ARQ_CONF );
if ( pIni != NULL )
  {
  char hex[50];
  PasswdToHexStr( pswd, hex );
  SENHA_ADM = hex;
  pIni->WriteString( "HMISHELL", "PASSWORD", hex );
  delete pIni;
  }
}

// Testa senha comparando com a gravada, retorna verdadeiro se igual
bool TfmSair::PasswdTest( char * pswd )
{
  char res[50];

  if ( strcmp ( pswd, "" ) == 0 )  // sem senha digitada dá falso
     return false;

  PasswdToHexStr( pswd, res ); // faz o hash

  return ( SENHA_ADM == res ); // compara com o hash
}

//---------------------------------------------------------------------------
__fastcall TfmSair::TfmSair(TComponent* Owner)
        : TForm(Owner)
{
DOC_AJUDA = "hidec.exe listdocs.bat";
LOG_AJUDA = "hidec.exe listlogs.bat";
HIDEPASSWD = 0;

TIniFile *pIni = new TIniFile( ARQ_CONFI18N );
if ( pIni != NULL )
  {
  fmSair->Caption = pIni->ReadString("HMISHELL", "BUTTON_EXIT", "Options...").Trim();
  btDesligar->Caption = pIni->ReadString("HMISHELL", "BUTTON_TURNOFF", "Turn Off").Trim();
  btReiniciar->Caption = pIni->ReadString("HMISHELL", "BUTTON_RESTART", "Restart").Trim();
  btLogout->Caption = pIni->ReadString("HMISHELL", "BUTTON_LOGOFF", "Logoff").Trim();
  btRecup->Caption = pIni->ReadString("HMISHELL", "BUTTON_RECOVER", "Recover").Trim();
  btAjuda->Caption = pIni->ReadString("HMISHELL", "BUTTON_HELP", "Help").Trim();
  btCache->Caption = pIni->ReadString("HMISHELL", "BUTTON_CACHE", "Limpar Cache").Trim();
  btNotes->Caption = pIni->ReadString("HMISHELL", "BUTTON_NOTES", "Sticky Notes").Trim();
  msgAltSenha = pIni->ReadString("HMISHELL", "BUTTON_PASSCHG", "Change Passwd.").Trim();
  btNovaSenha->Caption = msgAltSenha;
  msgSalvaSenha = pIni->ReadString("HMISHELL", "BUTTON_SAVEPASS", "Save Passwd.").Trim();
  lbSenha->Caption = pIni->ReadString("HMISHELL", "LABEL_NEWPASS", "Passwd.").Trim();
  lbConfSenha->Caption = pIni->ReadString("HMISHELL", "LABEL_CONFPASS", "Passwd.Conf.").Trim();

  delete pIni;
  }

PasswdReadDefault();
EntrandoNovaSenha = false;
Fechando = false;
OSVERSIONINFO VersionInformation;
GetVersionEx( &VersionInformation );
VersaoWinwdows = VersionInformation.dwMajorVersion;

// Just enable Sticky Notes button if detected
if ( GetFileAttributes("c:\\windows\\sysnative\\StikyNot.exe") !=  0xffffffff ||
     GetFileAttributes("c:\\windows\\system32\\StikyNot.exe") !=  0xffffffff ||
     GetFileAttributes("c:\\oshmi\\StickyNotes\\StikyNot.exe") !=  0xffffffff
   )
  btNotes->Visible=true;

}
//---------------------------------------------------------------------------

void __fastcall TfmSair::btDesligarClick(TObject *Sender)
{
Fechando = true;
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

// Log user logout
// register the system start and (OS) username
#pragma warn -aus
TCHAR UserName[200];
UserName[0]='\0';
DWORD size = sizeof(UserName) - 1;
GetUserName((TCHAR*)UserName, &size);
Loga( (String)"System Shutdown. User=" + (String)UserName );

try
  {
  String Rq = (String)"http://" +
              (String)REMOTE_HOST + (String)":" +
              (String)REMOTE_PORT + (String)"/" +
              (String)"htdocs/pntserver.rjs?U=" + (String)UserName + (String)"&O=Shutdown";
  fmShell->NMHTTP1->TimeOut = 0;
  fmShell->NMHTTP1->Get( Rq );
  } catch ( Exception &E ) {}

Sleep(100);

try
  {
  fmShell->NMHTTP1->Host = "127.0.0.1";
  fmShell->NMHTTP1->TimeOut = 0;
  fmShell->NMHTTP1->Get( (String)"/htdocs/pntserver.rjs?Y=1" );
  } catch ( Exception &E ) {}

WinExec( "hidec.exe do_turnoff.bat", SW_HIDE );
Close();
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::btReiniciarClick(TObject *Sender)
{
Fechando = true;
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

// Log user logout
// register the system start and (OS) username
#pragma warn -aus
TCHAR UserName[200];
UserName[0]='\0';
DWORD size = sizeof(UserName) - 1;
GetUserName((TCHAR*)UserName, &size);
Loga( (String)"System Reboot. User=" + (String)UserName );

try
  {
  String Rq = (String)"http://" +
              (String)REMOTE_HOST + (String)":" +
              (String)REMOTE_PORT + (String)"/" +
              (String)"htdocs/pntserver.rjs?U=" + (String)UserName + (String)"&O=Reboot";
  fmShell->NMHTTP1->TimeOut = 0;
  fmShell->NMHTTP1->Get( Rq );
  } catch ( Exception &E ) {}

Sleep(100);

try
  {
  fmShell->NMHTTP1->Host = "127.0.0.1";
  fmShell->NMHTTP1->TimeOut = 0;
  fmShell->NMHTTP1->Get( (String)"/htdocs/pntserver.rjs?Y=1" );
  } catch ( Exception &E ) {}

WinExec( "hidec.exe do_reboot.bat", SW_HIDE );
Close();
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::btLogoutClick(TObject *Sender)
{
Fechando = true;
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

// Log user logout
// register the system start and (OS) username
#pragma warn -aus
TCHAR UserName[200];
UserName[0]='\0';
DWORD size = sizeof(UserName) - 1;
GetUserName((TCHAR*)UserName, &size);
Loga( (String)"System Logoff. User=" + (String)UserName );

try
  {
  String Rq = (String)"http://" +
              (String)REMOTE_HOST + (String)":" +
              (String)REMOTE_PORT + (String)"/" +
              (String)"htdocs/pntserver.rjs?U=" + (String)UserName + (String)"&O=Logoff";
  fmShell->NMHTTP1->TimeOut = 0;
  fmShell->NMHTTP1->Get( Rq );
  } catch ( Exception &E ) {}

Sleep(100);
  
try
  {
  fmShell->NMHTTP1->Host = "127.0.0.1";
  fmShell->NMHTTP1->TimeOut = 0;
  fmShell->NMHTTP1->Get( (String)"/htdocs/pntserver.rjs?Y=1" );
  } catch ( Exception &E ) {}

WinExec( "hidec.exe do_logout.bat", SW_HIDE );
Close();
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::edSenhaChange(TObject *Sender)
{
    if (  EntrandoNovaSenha  )
      {
      if ( edSenha->Text != "" )
        {
        if ( edSenha->Text == edSenha2->Text )
          btNovaSenha->Enabled = true;
        else
          btNovaSenha->Enabled = false;
        }

      return;
      }

    if ( PasswdTest( edSenha->Text.c_str() ) )
      {
      btExplorer->Visible = true;
      btPos->Visible = true;
      btProcExp->Visible = true;
      btNovaSenha->Visible = true;
      fmShell->shMove->Visible = true;
      btCache->Visible = true;
      Height = ALTURA_MAIOR;

      try
        {
        fmShell->NMHTTP1->Host = "127.0.0.1";
        fmShell->NMHTTP1->TimeOut = 0;
        fmShell->NMHTTP1->Get( (String)"/htdocs/pntserver.rjs?H=0" );
        } catch ( Exception &E ) {}
      }
    else
      {
      btExplorer->Visible = false;
      btPos->Visible = false;
      btProcExp->Visible = false;
      fmShell->shMove->Visible = false;
      btNovaSenha->Visible = false;
      btCache->Visible = true;
      Height = ALTURA_MENOR;

      try
        {
        fmShell->NMHTTP1->Host = "127.0.0.1";
        fmShell->NMHTTP1->TimeOut = 0;
        fmShell->NMHTTP1->Get( (String)"/htdocs/pntserver.rjs?H=1" );
        } catch ( Exception &E ) {}

      }
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::btExplorerClick(TObject *Sender)
{
char buf[1000];

  GetWindowsDirectory( buf, 800 );
  strcat( buf, "\\explorer.exe" );
  ExecExternApp( buf );
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::btPosClick(TObject *Sender)
{
  fmShell->Top = 0;
  fmShell->Left = 0;
}
//---------------------------------------------------------------------------

// trata teclas no form
// testa esc para sair
// testa sequencia de teclas para apresentar o campo de senha
// é necessário digitar a hora e os minutos (HHMM) com as teclas abaixo dos números
// exemplo quando for 12:56, digitar QWTY
void __fastcall TfmSair::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
static int status = 1;

// keys below numbers 0123456789
char qwerty[] = "PQWERTYUIO";

if ( Key == VK_ESCAPE )
  Close();

// if password field hidden, show it when hour and mins entered on keys below numbers
// e.g. when 12:56, type QWTY to show the password field
if ( HIDEPASSWD )
  {
  TDateTime dt = dt.CurrentDateTime();
  String ds = dt.FormatString("hhnn");

  if ( (unsigned) qwerty[ds[status]-'0'] == (unsigned) Key )
    {
    if ( status == 4 )
      {
      status = 1;
      edSenha->Visible = !edSenha->Visible;
      lbSenha->Visible = edSenha->Visible;

      if ( edSenha->Visible )
        {
        edSenha->SetFocus();
        }
      }
    status++;
    }
  else
    status = 1;
  }  
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::btRecupClick(TObject *Sender)
{
ExecExternApp("cmd /c menu.bat");
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::btProcExpClick(TObject *Sender)
{
ExecExternApp("..\\extprogs\\procexp.exe");
}
//---------------------------------------------------------------------------


void __fastcall TfmSair::btCacheClick(TObject *Sender)
{
ExecExternApp("cmd /c cache_clean.bat");
}      
//---------------------------------------------------------------------------

void __fastcall TfmSair::btNovaSenhaClick(TObject *Sender)
{
if ( !EntrandoNovaSenha )
  {
  EntrandoNovaSenha = true;
  lbConfSenha->Visible = true;
  edSenha2->Visible = true;
  btNovaSenha->Enabled = false;
  btNovaSenha->Caption = msgSalvaSenha;
  edSenha->Text = "";
  edSenha2->Text = "";
  }
else
  { // OK, Grava Senha
  if ( edSenha->Text == edSenha2->Text )
    {
    EntrandoNovaSenha = false;
    PasswdWrite( edSenha->Text.c_str() );
    btNovaSenha->Caption = msgAltSenha;
    lbConfSenha->Visible = false;
    edSenha2->Visible = false;
    edSenha->Text = "";
    edSenha2->Text = "";
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::edSenha2Change(TObject *Sender)
{
  edSenhaChange( Sender );
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::FormCreate(TObject *Sender)
{
      Height = ALTURA_MENOR;
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::FormHide(TObject *Sender)
{
    EntrandoNovaSenha = false;
    btNovaSenha->Caption = msgAltSenha;
    lbConfSenha->Visible = false;
    edSenha2->Visible = false;
    edSenha2->Text = "";
    btNovaSenha->Visible = false;
    Height = ALTURA_MENOR;
//  lbSenha->Visible = false;
//  edSenha->Visible = false;
    edSenha->Text = "";
    btCache->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::FormShow(TObject *Sender)
{
if ( HIDEPASSWD )
  {
  edSenha->Visible = false;
  lbSenha->Visible = false;
  }
else
  {
  edSenha->Visible = true;
  lbSenha->Visible = true;
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::btAjudaClick(TObject *Sender)
{
  if ( GetAsyncKeyState(VK_LSHIFT) )
    ExecExternApp( fmShell->VISOR_LOGS.c_str() );
    // WinExec( LOG_AJUDA.c_str(), SW_SHOWNORMAL );
  else
    ExecExternApp( fmShell->VISOR_DOCS.c_str() );
     // WinExec( DOC_AJUDA.c_str(), SW_SHOWNORMAL );
}
//---------------------------------------------------------------------------

void __fastcall TfmSair::btNotesClick(TObject *Sender)
{
  if ( GetFileAttributes("c:\\windows\\sysnative\\StikyNot.exe") !=  0xffffffff )
    ExecExternApp( "c:\\windows\\sysnative\\StikyNot.exe" ); // para Windows 7 64
  else
  if ( GetFileAttributes("c:\\windows\\system32\\StikyNot.exe") !=  0xffffffff )
    ExecExternApp( "c:\\windows\\system32\\StikyNot.exe" ); // para Windows 7 32
  else  
  if ( GetFileAttributes("c:\\oshmi\\StickyNotes\\StikyNot.exe") !=  0xffffffff )
    ExecExternApp( "c:\\oshmi\\StickyNotes\\StikyNot.exe" ); // para instalação dedicada
  else
    ExecExternApp( "StikyNot" ); // para Windows 7 32
}
//---------------------------------------------------------------------------

