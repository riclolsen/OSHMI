/*
   mon_proc.exe

   Copyright 2008-2014 - Ricardo L. Olsen

   Esta aplicação serve para monitorar processos especificados
   no arquivo MON_PROC.INI.
   Estes processos são monitorados e disparados quando não
   estiverem rodado.
   O monitor pode aceitar mensagens pela porta UDP (8081),
   assim pode ter certeza que o processo não está pendurado.
   Se ficar sem receber mensagem de algum processo (configurado
   para tal) fecha o mesmo. O reinício do processo é automático.

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

//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <inifiles.hpp>
#include <tlhelp32.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <psapi.h>
#include "proc.h"

#define FILE_LOG "..\\logs\\mon_proc.log"
#define FILE_INI "..\\conf\\mon_proc.ini"

using namespace std;

bool ProcessoExecutando( String ExeFile, DWORD &id );

#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;

vector <String> Processos;
vector <String> Args;
vector <String> Exec;
vector <int> Modo;
vector <int> WDCnt;
vector <int> WDCntToRestart;  // x TimerWatchdog.Interval/1000 = seconds to restart hanged processes

void Loga(String msg)
{
FILE *fp;

  if ( ( fp = fopen(FILE_LOG, "at")) != NULL )
    {
    TDateTime dt;
    dt = dt.CurrentDateTime();
    String S = dt.FormatString("dd/mm/yy hh:nn:ss(");
    S = S + GetTickCount() % 1000;
    S = S + ") - " + msg;
    if ( FormMain->lbLog->Items->Count > 150 )
      {
      FormMain->lbLog->Items->Delete(0);
      }
    FormMain->lbLog->Items->Add(S);
    fprintf( fp, S.c_str() );
    fprintf( fp, "\n" );
    fclose( fp );
    }
}

//---------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent *Owner)
  : TForm(Owner)
{
ProcIndex = -1;
HIDE = 0;
Caption = "Process Monitor";

TIniFile *Ini = new TIniFile( FILE_INI );
if ( Ini == NULL )
  {
  exit(1);
  }

NMUDP1->LocalPort = Ini->ReadInteger( "CONF", "Port", 8081 );

for ( int i = 1; i < 20; i++ )
  {
  String str;
  String section = "PROC";
  section = section + i;
  str = Ini->ReadString( section, "File", "" );
  // remove as plicas "" e outros caracteres do caminho
  str = str.Trim();
  if ( str != "" )
    {
    Processos.push_back( str ); // caminho completo do executável
    Args.push_back( Ini->ReadString(section, "Args", "") ); // argumentos de linha de comando
    Modo.push_back( Ini->ReadInteger(section, "Mode", 0) ); // modo
    Exec.push_back( Ini->ReadString(section, "Exec", "") ); // linha de comando completa para execução (caminho + argumentos), se não existir executa PROC+ARGS
    WDCnt.push_back(0); // contagem inicial zerada
    WDCntToRestart.push_back( Ini->ReadInteger(section, "RestartCount", 12) ); // tempo para restart
    }
  }

 HIDE = Ini->ReadInteger( "CONF", "Hide", 0 );
}

//---------------------------------------------------------------------------

bool ProcessoExecutando(String ExeFile, DWORD &id)
{
bool achou = false;
HANDLE snap;
PROCESSENTRY32 pre;
bool ret;

DWORD idProcess[200];
DWORD cbNeeded;

EnumProcesses(
  idProcess, // array of process identifiers
  sizeof(idProcess), // size of array
  &cbNeeded // number of bytes returned
  );

unsigned i;
for ( i = 0; i < (int)cbNeeded / sizeof(DWORD); i++ )
  {
  HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, idProcess[i] );
  if ( hProcess != NULL )
    {
    HMODULE hmp[100];
    DWORD dwSize;
    int ret = EnumProcessModules( hProcess, hmp, sizeof(hmp), &dwSize );
    if ( ret != 0 )
      {
      char buf[1000];
      GetModuleFileNameEx( hProcess, hmp[0], buf, sizeof(buf) - 1 );

      if( ExeFile.AnsiCompareIC(buf) == 0 )
        {
        id = idProcess[i];
        achou = true;
        CloseHandle( hProcess );
        break;
        }
      }
    CloseHandle( hProcess );
    }
  }

return achou;
}

void MostraTodos(void)
{
bool ret;

FormMain->lbTodosProc->Items->Clear();

DWORD idProcess[200];
DWORD cbNeeded;

EnumProcesses(
  idProcess,  // array of process identifiers
  sizeof(idProcess),            // size of array
  &cbNeeded      // number of bytes returned
  );

unsigned i;
for ( i = 0; i < cbNeeded / sizeof(DWORD); i++ )
  {
  HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, idProcess[i] );
  if ( hProcess != NULL)
    {
    HMODULE hmp[100];
    DWORD dwSize;
    int ret = EnumProcessModules( hProcess, hmp, sizeof(hmp), &dwSize );
    if ( ret != 0 )
      {
      char buf[1000];
      GetModuleFileNameEx( hProcess, hmp[0], buf, sizeof(buf) - 1 );
      FormMain->lbTodosProc->Items->Add( buf );
      }
    CloseHandle( hProcess );
    }
  }

}

void __fastcall TFormMain::TimerWatchdogTimer(TObject *Sender)
{
DWORD id;

  if ( HIDE && FormMain->Visible )
    {
    Hide();
    }

   static int cnt = 0;
   switch ( (cnt++) % 8 )
     {
     case 0: cbWatchdog->Caption = "wAtchdog"; break;
     case 1: cbWatchdog->Caption = "waTchdog"; break;
     case 2: cbWatchdog->Caption = "watChdog"; break;
     case 3: cbWatchdog->Caption = "watcHdog"; break;
     case 4: cbWatchdog->Caption = "watchDog"; break;
     case 5: cbWatchdog->Caption = "watchdOg"; break;
     case 6: cbWatchdog->Caption = "watchdoG"; break;
     case 7: cbWatchdog->Caption = "Watchdog"; break;
     }


   MostraTodos();

   int sel = ListBox1->ItemIndex;
   ListBox1->Enabled = true;
   ListBox1->Clear();

   FormMain->ListBox1->Items->Clear();
   for ( unsigned i = 0 ; i < Processos.size(); i++ )
     {
     if ( ProcessoExecutando( Processos[i], id ) )
       { // processo existe
       if ( Modo[i] == 1 ) // se é para usar o Watchdog
         {
         WDCnt[i] = WDCnt[i] + 1; // incrementa Watchdog counter do processo
         if ( WDCnt[i] >= WDCntToRestart[i] ) // 6 contagens dá um minuto parado
           { // este processso está parado, deve ser restartado
           HANDLE ps = OpenProcess( 1, false, id );
           if(ps)
             {
             if( !TerminateProcess(ps,-9))
               {
               Loga( (AnsiString)"Unable to terminate process " + Processos[i] );
               }
             else
               {
               Loga( (AnsiString)"Process finished " + Processos[i] );
               }

             CloseHandle( ps );
             }
           else
             {
             Loga( (AnsiString)"Unable to open process " + Processos[i] );
             }

           WDCnt[i] = 0;
           }
         }

       FormMain->ListBox1->Items->Add( Processos[i]+ (AnsiString)" " + WDCnt[i] + (AnsiString)"/" + WDCntToRestart[i] + (AnsiString)" ---> OK!" );
       }
     else
       {
       FormMain->ListBox1->Items->Add( Processos[i]+ " ---> Not found!" );
       String cmdline = Processos[i]+" ";
       cmdline = cmdline + Args[i];
       if ( Exec[i] != "" )
         { // se tiver a linha exec definida, ignora PROC e ARGS, executa EXEC
         cmdline = Exec[i];
         }
       WinExec( cmdline.c_str(), SW_SHOWNORMAL );  // executa
       Loga( (AnsiString)"Process created " + Processos[i] );
       }
     }

ListBox1->ItemIndex = sel;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::NMUDP1DataReceived(TComponent *Sender,
      int NumberBytes, AnsiString FromIP, int Port)
{
char * buf = (char*)malloc(5000);
int bytesread = 0;
NMUDP1->ReadBuffer( buf, 5000, bytesread );

char *p = strtok( buf, "\n" );
String proc = p;
proc = proc.Trim();

// procura processo ( a primeira linha da mensagem é o nome do processo )
for ( unsigned i = 0; i < Processos.size(); i++ )
  if ( proc.AnsiCompareIC(Processos[i]) == 0 )
    { // achou, resseta watchdog
    WDCnt[i] = 0;

    if ( i == (unsigned)ProcIndex ) // mostra se for o processo selecionado
      {
      memoMens->Lines->Clear();
      memoMens->Lines->Add( proc );
      while ( ( p = strtok(NULL, "\n")) != NULL )
         {
         memoMens->Lines->Add(p);
         }
      }
    break;
    }

free(buf);
}
//----------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButton1Click(TObject *Sender)
{
DWORD id;
int i = ProcIndex;

if ( i == -1 )
  {
  return;
  }

Loga( (AnsiString)"User tried to close process " + Processos[i] );

if ( ProcessoExecutando( Processos[i], id ))
  {
  HANDLE ps = OpenProcess( 1, false, id );

  if( ps )
    {
    if( !TerminateProcess( ps, -9 ) )
      Loga( (AnsiString)"Unable to terminate process " + Processos[i] );
    else
      {
      FormMain->ListBox1->Items->Strings[i] = Processos[i] + (AnsiString)" " + WDCnt[i] + (AnsiString)" ---> Terminou!";
      Loga( (AnsiString)"Terminated process " + Processos[i] );
      }
    CloseHandle(ps);
    }
  else
    Loga( (AnsiString)"Unable to open process " + Processos[i] );
  }
}

//---------------------------------------------------------------------------

void __fastcall TFormMain::ListBox1Click(TObject *Sender)
{
ProcIndex = ListBox1->ItemIndex;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::cbWatchdogClick(TObject *Sender)
{
if ( cbWatchdog->Checked )
  {
  TimerWatchdog->Enabled = true;
  }
else
  {
  TimerWatchdog->Enabled = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::WMQueryEndSession(TMessage & Message)
{
Message.Result = true;

/*
//ShowMessage(Message.LParam);

if ( Message.LParam & 0x80000000 ) // logoff
  {
  //  ExitWindowsEx(EWX_FORCEIFHUNG|EWX_LOGOFF, -1);
  }
else
  {
  //ExitWindowsEx(EWX_FORCEIFHUNG|EWX_REBOOT, -1);
  //WinExec("shutdown -f -s", SW_SHOWMINNOACTIVE);
  }
*/
}

void __fastcall TFormMain::WMEndSession(TMessage & Message)
{
/*
if ( Message.LParam & 0x80000000 ) // logoff
  WinExec("shutdown.exe -l", SW_SHOWNORMAL);
else
  WinExec("shutdown.exe -f -r -t 20", SW_SHOWNORMAL);
*/  
Close();
}

