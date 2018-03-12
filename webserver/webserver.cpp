//---------------------------------------------------------------------------
/*
OSHMI - Open Substation HMI
	Copyright 2008-2016 - Ricardo L. Olsen

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
#include <dir.h>
#include <stdio.h>
#include "config.h"
USERES("webserver.res");
USEFORM("webserver_u.cpp", fmWebServ);
USEUNIT("bcolocal.cpp");
USEFORM("bdtr_com.cpp", fmBDTR);
USEFORM("vedados.cpp", fmVeDados);
USEFORM("eventos_u.cpp", fmSDE);
USEFORM("dumpdb_u.cpp", fmDumpdb);
USEFORM("about_u.cpp", AboutForm);
USEUNIT("config.cpp");
USEFORM("simul.cpp", fmSimul);
USEUNIT("sha1.c");
USEUNIT("md5.c");
USEUNIT("bdtr_class.cpp");
USEFORM("lua_u.cpp", fmLua);
USEUNIT("luasrc\lapi.c");
USEUNIT("luasrc\lauxlib.c");
USEUNIT("luasrc\lbaselib.c");
USEUNIT("luasrc\lcode.c");
USEUNIT("luasrc\ldblib.c");
USEUNIT("luasrc\ldebug.c");
USEUNIT("luasrc\ldo.c");
USEUNIT("luasrc\ldump.c");
USEUNIT("luasrc\lfunc.c");
USEUNIT("luasrc\lgc.c");
USEUNIT("luasrc\linit.c");
USEUNIT("luasrc\liolib.c");
USEUNIT("luasrc\llex.c");
USEUNIT("luasrc\lmathlib.c");
USEUNIT("luasrc\lmem.c");
USEUNIT("luasrc\loadlib.c");
USEUNIT("luasrc\lobject.c");
USEUNIT("luasrc\lopcodes.c");
USEUNIT("luasrc\loslib.c");
USEUNIT("luasrc\lparser.c");
USEUNIT("luasrc\lstate.c");
USEUNIT("luasrc\lstring.c");
USEUNIT("luasrc\lstrlib.c");
USEUNIT("luasrc\ltable.c");
USEUNIT("luasrc\ltablib.c");
USEUNIT("luasrc\ltm.c");
USEUNIT("luasrc\lua.c");
USEUNIT("luasrc\lundump.c");
USEUNIT("luasrc\lvm.c");
USEUNIT("luasrc\lzio.c");
USEUNIT("luasrc\print.c");
USEUNIT("luasrc\bit.c");
USEFORM("historico_u.cpp", fmHist);
USEUNIT("sqlite3.c");
USEFORM("i104m_u.cpp", fmIEC104M);
//---------------------------------------------------------------------------
extern String IP_BDTR1;
extern String IP_BDTR2;

//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        char execstr[100];

        try
        {

         ::CreateMutex( NULL, FALSE, "WebServerOSHMI" );
         if ( GetLastError() == ERROR_ALREADY_EXISTS )
           { // outra instancia já está rodando
           }
         else
           {
                 Application->Initialize();

                 DateSeparator = '/';
                 TimeSeparator = ':';
                 ShortDateFormat = "d/M/yyyy";
                 ShortTimeFormat = "hh:mm";
                 LongTimeFormat = "hh:mm:ss";

                 // vai para o diretório do executável
                 chdir( ExtractFilePath(Application->ExeName).c_str() );
                 Application->CreateForm(__classid(TfmVeDados), &fmVeDados);
                 Application->CreateForm(__classid(TfmDumpdb), &fmDumpdb);
                 Application->CreateForm(__classid(TfmSDE), &fmSDE);
                 Application->CreateForm(__classid(TfmWebServ), &fmWebServ);
                 Application->CreateForm(__classid(TfmBDTR), &fmBDTR);
                 Application->CreateForm(__classid(TAboutForm), &AboutForm);
                 Application->CreateForm(__classid(TfmSimul), &fmSimul);
                 Application->CreateForm(__classid(TfmLua), &fmLua);
                 Application->CreateForm(__classid(TfmHist), &fmHist);
                 Application->CreateForm(__classid(TfmIEC104M), &fmIEC104M);
                 if ( IP_BDTR1 == "" && IP_BDTR2 == "" && SIMULACAO == 0 )
                   {
                   // roda varredor IEC104, mas primeiro mata algum que já esteja rodando
                   sprintf( execstr, "taskkill /F /IM %s", RUN_VARREDORIEC104 );
                   ExecExternApp( execstr );
                   Sleep(500);
                   ExecExternApp( RUN_VARREDORIEC104.c_str() );
                   }

                 // executa do servidor nginx / php
                 ExecExternApp( RUN_NGINXPHP.c_str() );
                 if ( HIST_RECORD )
                   {
                   ExecExternApp( RUN_PROCMONHIST.c_str() );
                   ExecExternApp( RUN_PROCHIST.c_str() );
                   }
                 ExecExternApp( RUN_PROCEVENTOS.c_str() );
                 ExecExternApp( RUN_PROCPONTOS.c_str() );
                 Application->Run();
            }

        }
        catch ( Exception &exception )
        {
           Loga( exception.Message );
           return 1;
        }

        ExecExternApp( END_PROCHIST.c_str() );
        ExecExternApp( END_PROCEVENTOS.c_str() );
        ExecExternApp( END_PROCPONTOS.c_str() );

        sprintf( execstr, "taskkill /F /IM %s", RUN_VARREDORIEC104 );
        ExecExternApp( execstr );

        return 0;
}
//---------------------------------------------------------------------------
