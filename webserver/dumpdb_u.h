//---------------------------------------------------------------------------

#ifndef dumpdb_uH
#define dumpdb_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include "sqlite3.h"

#define DBCOL_NPONTO 0
#define DBCOL_ID 1
#define DBCOL_DESCRICAO 2
#define DBCOL_VALOR 3
#define DBCOL_FLAGS 4
#define DBCOL_LIMI 5
#define DBCOL_LIMS 6
#define DBCOL_HISTER 7
#define DBCOL_DATA 8
#define DBCOL_HORA 9
#define DBCOL_TS 10
#define DBCOL_ALRIN 11
#define DBCOL_VLRINI 12
#define DBCOL_HISTOR 13
#define DBCOL_BMORTA 14
#define DBCOL_PERIODO 15
#define DBCOL_TIPO 16
#define DBCOL_ESTON 17
#define DBCOL_ESTOFF 18
#define DBCOL_UNIDADE 19
#define DBCOL_SUPCMD 20
#define DBCOL_TIMEOUT 21
#define DBCOL_ANOTACAO 22

//---------------------------------------------------------------------------
class TfmDumpdb : public TForm
{
__published:	// IDE-managed Components
        TTimer *Timer1;
        TButton *btDump;
        TLabel *Label1;
        TLabel *lbErro;
        TLabel *Label2;
        TTimer *Timer2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        void __fastcall btDumpClick(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender);
private:	// User declarations
        String BDFile;
        sqlite3 * sqlite_db;
        void ProcessaFilaSQL();
        int RecuperaInfoDumpDB(); // recupera dump em SQLITE, retorna número de pontos lidos
public:		// User declarations
        __fastcall TfmDumpdb(TComponent* Owner);
        void DumpDB(int nponto, int forcagrv=0); // dump EM SQLITE
};


//---------------------------------------------------------------------------
extern PACKAGE TfmDumpdb *fmDumpdb;
//---------------------------------------------------------------------------
#endif
