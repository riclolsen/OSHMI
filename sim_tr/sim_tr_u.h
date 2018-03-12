//---------------------------------------------------------------------------

#ifndef sim_tr_uH
#define sim_tr_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <Buttons.hpp>
#include "IdBaseComponent.hpp"
#include "IdComponent.hpp"
#include "IdUDPBase.hpp"
#include "IdUDPClient.hpp"
#include "IdUDPServer.hpp"
#include <ExtCtrls.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TfmSimTR : public TForm
{
__published:	// IDE-managed Components
        TOpenDialog *OpenDialog1;
        TButton *btFileOpen;
        TBitBtn *btPlay;
        TBitBtn *btPause;
        TBitBtn *btFim;
        TIdUDPClient *IdUDPClient1;
        TLabel *Label1;
        TTimer *Timer1;
        TLabel *lbMsg;
        TListBox *lbSim;
        TLabel *lbLine;
        TStringGrid *sgDestinos;
        TLabel *Label3;
        TLabel *lbNomeArq;
        TLabel *Label2;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *lbEstado;
        void __fastcall btFileOpenClick(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall btFimClick(TObject *Sender);
        void __fastcall btPauseClick(TObject *Sender);
        void __fastcall btPlayClick(TObject *Sender);
private:	// User declarations
        void SendBDTR( char * buf, int size );
public:		// User declarations
        __fastcall TfmSimTR(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmSimTR *fmSimTR;
//---------------------------------------------------------------------------
#endif
