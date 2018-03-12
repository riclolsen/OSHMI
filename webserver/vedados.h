//---------------------------------------------------------------------------

#ifndef vedadosH
#define vedadosH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <NMUDP.hpp>

#pragma warn -8027

//---------------------------------------------------------------------------
class TfmVeDados : public TForm
{
protected:
    void __fastcall WMQueryEndSession(TMessage &Message);
    void __fastcall WMEndSession(TMessage &Message);
BEGIN_MESSAGE_MAP MESSAGE_HANDLER(WM_QUERYENDSESSION, TMessage, WMQueryEndSession)
                  MESSAGE_HANDLER(WM_ENDSESSION, TMessage, WMEndSession)
END_MESSAGE_MAP(TForm)

__published:	// IDE-managed Components
        TStringGrid *sgPontos;
        TEdit *edBusca;
        TButton *btBuscaPonto;
        TTimer *Timer1;
        TButton *btBDTR;
        TButton *btSDE;
        TButton *btWebServ;
        TEdit *edtPasswd;
        TButton *btDumpdb;
        TImage *Image2;
        TShape *shBDTR;
        TLabel *lbTime;
        TShape *shSDE;
        TShape *shWeb;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TButton *btClose;
        TShape *shI104;
        TShape *shDumpDB;
        TButton *btSimul;
        TButton *btScript;
        TNMUDP *NMUDPWD;
        TButton *btIEC104;
        TLabel *Label1;
        TButton *btHist;
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall btBuscaPontoClick(TObject *Sender);
        void __fastcall btBDTRClick(TObject *Sender);
        void __fastcall btSDEClick(TObject *Sender);
        void __fastcall btWebServClick(TObject *Sender);
        void __fastcall edBuscaKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall edtPasswdChange(TObject *Sender);
        void __fastcall FormDblClick(TObject *Sender);
        void __fastcall btDumpdbClick(TObject *Sender);
        void __fastcall Image2DblClick(TObject *Sender);
        void __fastcall btCloseClick(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall btSimulClick(TObject *Sender);
        void __fastcall btScriptClick(TObject *Sender);
        void __fastcall btIEC104Click(TObject *Sender);
        void __fastcall btHistClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfmVeDados(TComponent* Owner);
        int EncontraPonto;
        void PulseBDTR(TColor cor = clRed);
        void PulseI104(TColor cor = clRed);
        void PulseSDE(TColor cor = clRed);
        void PulseWeb(TColor cor = clRed);
        void PulseDumpDB(TColor cor = clRed);
        void ShowButtons( bool ShowBt );
};

//---------------------------------------------------------------------------
extern PACKAGE TfmVeDados *fmVeDados;
//---------------------------------------------------------------------------
#endif
