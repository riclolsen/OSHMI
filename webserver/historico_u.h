//---------------------------------------------------------------------------

#ifndef historico_uH
#define historico_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <list>

typedef struct
{
int nponto;
float valor;
int flags;
double tagtempo;
} THistValor;

//---------------------------------------------------------------------------
class TfmHist : public TForm
{
__published:	// IDE-managed Components
        TTimer *Timer1;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label6;
        TLabel *lbErro;
        TButton *btApagaAntigos;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label5;
        TLabel *Label7;
        TLabel *lbWrites;
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall btApagaAntigosClick(TObject *Sender);

private:	// User declarations
        // sqlite3 * sqlite_db;
        std::list <THistValor> ListaValHist;
        // String BDFile;
        void ProcessaFila();

public:		// User declarations
        __fastcall TfmHist(TComponent* Owner);
        void PushVal( int nponto, float valor, int flags, double tagtempo );
};
//---------------------------------------------------------------------------
extern PACKAGE TfmHist *fmHist;
//---------------------------------------------------------------------------
#endif
