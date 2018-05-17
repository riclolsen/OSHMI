//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "screen_list_u.h"
#include "shell_u.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormScreenList *FormScreenList;
//---------------------------------------------------------------------------
__fastcall TFormScreenList::TFormScreenList(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormScreenList::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
switch (Key)
 {
 case VK_RWIN: // WIN Key
 case VK_LWIN: // WIN Key
   FormScreenList->Visible = false;
   GetAsyncKeyState(VK_LWIN);
   GetAsyncKeyState(VK_RWIN);
   fmShell->lastks=3;
   break;
 case VK_UP:
 case VK_LEFT:
   if (fmShell->SelScreen > 0)
     fmShell->SelScreen--;
   fmShell->ShowSelScreen();
   break;
 case VK_TAB:
 case VK_DOWN:
 case VK_RIGHT:
   if (fmShell->SelScreen < fmShell->NumScreens - 1)
     fmShell->SelScreen++;
   fmShell->ShowSelScreen();
   break;
 case VK_RETURN:
   fmShell->lblScreenClick(fmShell->ScreenItems[fmShell->SelScreen]);
   break;
 default: // pass keystrokes to main form
   fmShell->FormKeyDown(Sender, Key, Shift);
   break;
 }
}
//---------------------------------------------------------------------------

void __fastcall TFormScreenList::FormMouseWheelDown(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
   if (fmShell->SelScreen < fmShell->NumScreens - 1)
     fmShell->SelScreen++;
   fmShell->ShowSelScreen();
}
//---------------------------------------------------------------------------

void __fastcall TFormScreenList::FormMouseWheelUp(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
   if (fmShell->SelScreen > 0)
     fmShell->SelScreen--;
   fmShell->ShowSelScreen();
}
//---------------------------------------------------------------------------

void __fastcall TFormScreenList::FormShow(TObject *Sender)
{
ShowWindow( Handle, SW_RESTORE );
SetForegroundWindow(Handle);
}
//---------------------------------------------------------------------------

