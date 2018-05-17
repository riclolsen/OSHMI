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

#include <vcl.h>
#pragma hdrstop
USERES("hmishell.res");
USEFORM("shell_u.cpp", fmShell);
USEFORM("exit_u.cpp", fmSair);
USELIB("c:\program files (x86)\borland\cbuilder5\Lib\PSDK\psapi.lib");
USEUNIT("sha1.c");
USEFORM("..\webserver\screen_list_u.cpp", FormScreenList);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

 ::CreateMutex(NULL, FALSE, "HMIShellOSHMI");
 if ( GetLastError() == ERROR_ALREADY_EXISTS )
   { // outra instancia já está rodando
   }
 else
   {   
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TfmShell), &fmShell);
                 Application->CreateForm(__classid(TfmSair), &fmSair);
                 Application->CreateForm(__classid(TFormScreenList), &FormScreenList);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
   }
        
 return 0;
}
//---------------------------------------------------------------------------
