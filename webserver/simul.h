//---------------------------------------------------------------------------

#ifndef simulH
#define simulH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfmSimul : public TForm
{
__published:	// IDE-managed Components
        TEdit *edNPonto;
        TEdit *edValor;
        TEdit *edFlags;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TButton *btEscreve;
        TButton *btBusca;
        TLabel *lbFlg;
        TCheckBox *cbSimul;
        TCheckBox *cbQBit6;
        TCheckBox *cbQBit5;
        TCheckBox *cbQBit4;
        TCheckBox *cbQBit3;
        TCheckBox *cbQBit2;
        TCheckBox *cbQBit1;
        TCheckBox *cbQBit0;
        TButton *btEvento;
        TLabel *lbID;
        TLabel *lbDesc;
        TLabel *lbErr;
        TLabel *Label4;
        TLabel *lbEstado;
        TLabel *lbQuest;
        TLabel *lbTipo;
        TLabel *lbImposto;
        TLabel *lbOrigem;
        TCheckBox *cbQBit7;
        TLabel *lbFalha;
        TBevel *Bevel1;
        TBevel *Bevel2;
        TBevel *Bevel3;
        TBevel *Bevel5;
        TBevel *Bevel7;
        TBevel *Bevel8;
        TBevel *Bevel4;
        TTimer *Timer1;
        TLabel *lbModo;
        void __fastcall btEscreveClick(TObject *Sender);
        void __fastcall btBuscaClick(TObject *Sender);
        void __fastcall edNPontoEnter(TObject *Sender);
        void __fastcall edValorEnter(TObject *Sender);
        void __fastcall edFlagsEnter(TObject *Sender);
        void __fastcall cbSimulClick(TObject *Sender);
        void __fastcall edFlagsChange(TObject *Sender);
        void __fastcall cbQBit7Click(TObject *Sender);
        void __fastcall btEventoClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
private:	// User declarations
public:		// User declarations
        void AcertaFlags(unsigned char Byte);
        __fastcall TfmSimul(TComponent* Owner);
        int SimulaComando(int nponto, float valor);
        void ProcessaSimulacao(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmSimul *fmSimul;
//---------------------------------------------------------------------------
#endif
