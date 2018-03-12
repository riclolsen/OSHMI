//---------------------------------------------------------------------------

#ifndef webserver_uH
#define webserver_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "IdBaseComponent.hpp"
#include "IdComponent.hpp"
#include "IdCustomHTTPServer.hpp"
#include "IdHTTPServer.hpp"
#include <NMUDP.hpp>
#include <ExtCtrls.hpp>
#include <NMHttp.hpp>
#include <Psock.hpp>
#include "IdTCPServer.hpp"
//---------------------------------------------------------------------------
class TfmWebServ : public TForm
{
__published:	// IDE-managed Components
        TIdHTTPServer *IdHTTPServer1;
        TNMHTTP *NMHTTP1;
        TTimer *Timer2;
        TButton *btSinc;
        TTimer *Timer3;
        TButton *btResetServer;
        TMemo *mmWeb;
        TCheckBox *cbLogar;
        TLabel *lbOutroIHM;
        void __fastcall IdHTTPServer1CommandGet(TIdPeerThread *AThread,
          TIdHTTPRequestInfo *ARequestInfo,
          TIdHTTPResponseInfo *AResponseInfo);
        //void __fastcall NMUDP1DataReceived(TComponent *Sender,
        // int NumberBytes, AnsiString FromIP, int Port);
        //void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall NMHTTP1Success(CmdType Cmd);
        void __fastcall btSincClick(TObject *Sender);
        void __fastcall NMHTTP1Status(TComponent *Sender,
          AnsiString Status);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall Timer3Timer(TObject *Sender);
        void __fastcall btResetServerClick(TObject *Sender);
        void __fastcall IdHTTPServer1Exception(TIdPeerThread *AThread,
          Exception *AException);
private:	// User declarations
        void logaln(String msg);
public:		// User declarations
        __fastcall TfmWebServ(TComponent* Owner);
        unsigned getKeyNum(String s);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmWebServ *fmWebServ;
//---------------------------------------------------------------------------
#endif
