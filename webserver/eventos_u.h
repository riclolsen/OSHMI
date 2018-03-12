//---------------------------------------------------------------------------

#ifndef eventos_uH
#define eventos_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <list>
#include <map>

#define SDE_GERAHORAMS   98
#define SDE_GERAHORA     99
#define SDE_UTREVGERADO 253
#define SDE_UTREVSIMUL  254
#define SDE_UTREVANALOG   0
#define MAX_EVENTOS_LISTA 3000

typedef struct
 {
 unsigned int NPONTO;
 unsigned int UTR;
 unsigned int Flags;
 TDateTime Data;
 TDateTime Hora;
 unsigned int Milisegundo;
 TDateTime TS;
 int Recon;
 int Tipo; // tipo = 0 : insert, tipo 1 = update recon where ID
 } TEvento;

//---------------------------------------------------------------------------
class TfmSDE : public TForm
{
__published:	// IDE-managed Components
        TLabel *lbErro;
        TLabel *lbRecCnt;
        TButton *btEmpty;
        TLabel *lbDescarte;
        TLabel *lbEvento;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *lbRec;
        TTimer *Timer1;
        TButton *btReindex;
        TButton *btPack;
        TButton *btApagaAntigos;
        TLabel *Label11;
        TLabel *Label12;
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall btReindexClick(TObject *Sender);
        void __fastcall btPackClick(TObject *Sender);
        void __fastcall btApagaAntigosClick(TObject *Sender);
        void __fastcall btEmptyClick(TObject *Sender);
private:	// User declarations
        void ProcessaFila();
public:		// User declarations
        __fastcall TfmSDE(TComponent* Owner);
};

int IncluiEvento( unsigned int NPONTO,
                  unsigned int UTR,
                  unsigned int Flags,
                  unsigned int Ano,
                  unsigned int Mes,
                  unsigned int Dia,
                  unsigned int Hora,
                  unsigned int Minuto,
                  unsigned int Segundo,
                  unsigned int Milisegundo
                  );

int ReconheceEvento( unsigned int NPONTO,
                     unsigned int Ano,
                     unsigned int Mes,
                     unsigned int Dia,
                     unsigned int Hora,
                     unsigned int Minuto,
                     unsigned int Segundo,
                     unsigned int Milisegundo,
                     unsigned int RecApaga // 0= reconhece, 1=apaga
                  );

//---------------------------------------------------------------------------
extern PACKAGE TfmSDE *fmSDE;
//---------------------------------------------------------------------------
#endif
