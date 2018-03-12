//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("sim_tr.res");
USEFORM("sim_tr_u.cpp", fmSimTR);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TfmSimTR), &fmSimTR);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
