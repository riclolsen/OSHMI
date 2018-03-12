//---------------------------------------------------------------------------

#ifndef lua_uH
#define lua_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "luasrc/lua.hpp"

//---------------------------------------------------------------------------
class TfmLua : public TForm
{
__published:	// IDE-managed Components
        TMemo *mmSrc;
        TMemo *mmErr;
        TButton *btExec;
        TLabel *lbOut;
        TLabel *lbSrc;
        TTimer *Timer1;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        void __fastcall btExecClick(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall mmErrDblClick(TObject *Sender);
        void __fastcall mmSrcDblClick(TObject *Sender);
private:	// User declarations
        lua_State *L;
        String ScriptCycle;
public:		// User declarations
        __fastcall TfmLua(TComponent* Owner);
        int luaInterceptCommands( int nponto, float val );
};

//---------------------------------------------------------------------------
extern PACKAGE TfmLua *fmLua;
//---------------------------------------------------------------------------
#endif
