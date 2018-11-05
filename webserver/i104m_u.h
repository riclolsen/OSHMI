//---------------------------------------------------------------------------

#ifndef i104m_uH
#define i104m_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "IdBaseComponent.hpp"
#include "IdComponent.hpp"
#include "IdUDPBase.hpp"
#include "IdUDPServer.hpp"

//---------------------------------------------------------------------------

#define I104M_REMOTEPORT 8098
#define I104M_REMOTEPORTMODBUS 8097

#define MSGSUP_SIG 0x53535353
typedef struct {
unsigned int signature;  // 0x53535353
unsigned int endereco;
unsigned int tipo;
unsigned int prim;
unsigned int sec;
unsigned int causa;
unsigned int taminfo;
unsigned char info[255];
} t_msgsup;

#define MSGSUPSQ_SIG 0x64646464
typedef struct {
unsigned int signature;  // 0x64646464
unsigned int numpoints;
unsigned int tipo;
unsigned int prim;
unsigned int sec;
unsigned int causa;
unsigned int taminfo; // value size for the type (not counting the 4 byte address)
unsigned char info[2000]; // { 4 bytes unsigned int address, point value (taminfo bytes) } ...  Repeat
} t_msgsupsq;

#define MSGCMD_SIG 0x4b4b4b4b
typedef struct
{
	unsigned int signature; // 0x4b4b4b4b
	unsigned int endereco;
	unsigned int tipo;
	union
	{
		unsigned int onoff;
		float setpoint;
                int setpoint_i32;
                short int setpoint_i16;
	};
	unsigned int sbo;
	unsigned int qu;
	unsigned int utr;
} t_msgcmd;

//---------------------------------------------------------------------------
class TfmIEC104M : public TForm
{
__published:	// IDE-managed Components
        TTimer *Timer1;
        TLabel *lbPackt;
        TTimer *Timer2;
        TLabel *lbPacktNok;
        TMemo *mmLOG;
        TCheckBox *cbLog;
        TIdUDPServer *IdUDPServer1;
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall IdUDPServer1UDPRead(TObject *Sender,
          TStream *AData, TIdSocketHandle *ABinding);
private:	// User declarations
        void logaln( String msg );
public:		// User declarations
        __fastcall TfmIEC104M(TComponent* Owner);
        int ComandoIEC_Dig( unsigned int nponto, unsigned int onoff );
        int ComandoIEC_Ana( unsigned int nponto, double val );
        int I104_cntIntgr;
        int cntPackt;
        int cntPacktDisc;
};
//---------------------------------------------------------------------------
extern PACKAGE TfmIEC104M *fmIEC104M;
//---------------------------------------------------------------------------

#endif
