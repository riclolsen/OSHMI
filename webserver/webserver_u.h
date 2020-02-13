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
#include <NMURL.hpp>
#include "IdHTTP.hpp"
#include "IdTCPClient.hpp"
#include "IdTCPConnection.hpp"
//---------------------------------------------------------------------------
class TfmWebServ : public TForm
{
__published:	// IDE-managed Components
        TIdHTTPServer *IdHTTPServer1;
        TNMHTTP *NMHTTP1;
        TTimer *Timer2;
        TButton *btSinc;
        TTimer *Timer3;
        TMemo *mmWeb;
        TCheckBox *cbLogar;
        TLabel *lbOutroIHM;
        TLabel *lbReset;
        TLabel *Label1;
        TIdHTTP *IdHTTP1;
        TNMHTTP *NMHTTP2;
        //void __fastcall NMUDP1DataReceived(TComponent *Sender,
        // int NumberBytes, AnsiString FromIP, int Port);
        //void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall NMHTTP1Success(CmdType Cmd);
        void __fastcall btSincClick(TObject *Sender);
        void __fastcall NMHTTP1Status(TComponent *Sender,
          AnsiString Status);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Timer3Timer(TObject *Sender);
        void __fastcall IdHTTPServer1Exception(TIdPeerThread *AThread,
          Exception *AException);
        void __fastcall NMHTTP2Success(CmdType Cmd);
        void __fastcall NMHTTP2Status(TComponent *Sender,
          AnsiString Status);
private:	// User declarations
        void logaln(String msg);
        int cntDownNoHTTPReq; // count down time with no HTTP Requests, when reached 0 resets server
public:		// User declarations
        __fastcall TfmWebServ(TComponent* Owner);
        unsigned getKeyNum(String s);
};

extern String JANELA_AENCONTRAR;
extern HWND JANELA_ENCONTRADA;
extern int cmdNPonto;
extern int cmdCntAck;
extern int confCmdSimul;

// 4*Timer2->Interval(10s) = 40s
#define NOHTTPREQTIME 4

//---------------------------------------------------------------------------
extern PACKAGE TfmWebServ *fmWebServ;
//---------------------------------------------------------------------------
#endif
