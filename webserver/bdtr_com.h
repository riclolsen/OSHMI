//---------------------------------------------------------------------------

#ifndef bdtr_comH
#define bdtr_comH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "IdBaseComponent.hpp"
#include "IdComponent.hpp"
#include "IdUDPBase.hpp"
#include "IdUDPClient.hpp"
#include "IdUDPServer.hpp"
#include <ExtCtrls.hpp>
#include "IdMappedPortUDP.hpp"
#include <NMUDP.hpp>
#include "IdAntiFreeze.hpp"
#include "IdAntiFreezeBase.hpp"
#include "bdtr_class.h"

class MyBDTR : public BDTR
{
public:
char lastMsgRcv[1500]; // última mensagem recebida
int szLastMsgRcv; // tamanho da última mensagem recebida
int CNT_SEG_SEMKA; // conta os segundos sem keep-alive
void envia_udp(char * IP, int porta, char * buf, int sz);
void set_udp_timeout(int timeout);
int le_udp(char * buf, int szmax);
void logaln(string msg);
                   
int  IncluiEvento( unsigned int EndPonto,
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
int SetAckCmd(int EndPonto, int falha, int cmd); // seta ack de comando
void RecebeuIntegridade();
void RecebeuHora(msg_sinc *m);
void CommandIndication(int endcmd, char flags, float valor);
};

//---------------------------------------------------------------------------
class TfmBDTR : public TForm
{
__published:	// IDE-managed Components
        TIdUDPServer *IdUDPServer1;
        TMemo *mmBDTR;
        TLabel *lbCntMsg;
        TLabel *lbCntDig;
        TLabel *lbCntAna;
        TLabel *lbCntOutras;
        TLabel *lbTipo;
        TLabel *lbTam;
        TLabel *lbCntErro;
        TLabel *lbErro;
        TLabel *lbIP;
        TTimer *Timer1;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *lbSolIntgr;
        TLabel *Label11;
        TLabel *lbCntIniIntgr;
        TLabel *lbCntFimIntgr;
        TLabel *Label10;
        TLabel *Label12;
        TCheckBox *cbMonit;
        TCheckBox *cbLoga;
        TButton *btIntgr;
        TLabel *Label13;
        TLabel *lbCntRst;
        TLabel *lbCntSinc;
        TLabel *Label14;
        TLabel *lbAno;
        TLabel *lbMes;
        TLabel *lbDia;
        TLabel *lbHora;
        TLabel *lbMinuto;
        TLabel *lbMSegs;
        TLabel *lbCntDigTag;
        TLabel *Label15;
        TLabel *lbCntQEve;
        TLabel *Label16;
        TLabel *Label17;
        TLabel *lbCntAck;
        TTimer *Timer2;
        TIdAntiFreeze *IdAntiFreeze1;
        TLabel *Label20;
        TLabel *lbMsgCmd;
        TLabel *Label21;
        TLabel *lbErroParc;
        TLabel *lbTmpGI;
        TLabel *lbIP2;
        void __fastcall IdUDPServer1UDPRead(TObject *Sender,
          TStream *AData, TIdSocketHandle *ABinding);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall cbMonitClick(TObject *Sender);
        void __fastcall cbLogaClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfmBDTR(TComponent* Owner);
        void FinalizandoAplicacao();
        MyBDTR bdtr;
        TStream *UDPAData;
        void TagBDTR_HoraAtual(A_tag * ptag);
};

//---------------------------------------------------------------------------
extern PACKAGE TfmBDTR *fmBDTR;
//---------------------------------------------------------------------------
#endif
