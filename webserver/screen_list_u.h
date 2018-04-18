//---------------------------------------------------------------------------

#ifndef screen_list_uH
#define screen_list_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFormScreenList : public TForm
{
__published:	// IDE-managed Components
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormMouseWheelDown(TObject *Sender,
          TShiftState Shift, TPoint &MousePos, bool &Handled);
        void __fastcall FormMouseWheelUp(TObject *Sender,
          TShiftState Shift, TPoint &MousePos, bool &Handled);
private:	// User declarations
public:		// User declarations
        __fastcall TFormScreenList(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormScreenList *FormScreenList;
//---------------------------------------------------------------------------
#endif
