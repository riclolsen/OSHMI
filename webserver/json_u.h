//---------------------------------------------------------------------------

#ifndef json_uH
#define json_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "IdBaseComponent.hpp"
#include "IdComponent.hpp"
#include "IdUDPBase.hpp"
#include "IdUDPServer.hpp"
#include "bcolocal.h"

//---------------------------------------------------------------------------
class TfmJSON : public TForm
{
__published:	// IDE-managed Components
        TMemo *mmJSONLog;
        TIdUDPServer *IdUDPServer1;
        TCheckBox *cbJSONLog;
        void __fastcall IdUDPServer1UDPRead(TObject *Sender,
          TStream *AData, TIdSocketHandle *ABinding);
private:	// User declarations
        void logaln( String msg );
        void process_point( int nponto, char * tagstr, unsigned int address, unsigned int utr, float valor, bool quality_ok, bool is_binary, time_t uxtime, short ms);
public:		// User declarations
        __fastcall TfmJSON(TComponent* Owner);
        int Command( TPonto &pt, double val );
};
//---------------------------------------------------------------------------
extern PACKAGE TfmJSON *fmJSON;
//---------------------------------------------------------------------------
#endif
