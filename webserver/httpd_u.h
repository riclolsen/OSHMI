//---------------------------------------------------------------------------

#ifndef httpd_uH
#define httpd_uH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "microhttpd.h"
#include <ExtCtrls.hpp>

#define MIME_TEXT 0
#define MIME_JSON 1
#define MIME_JAVASCRIPT 2
//---------------------------------------------------------------------------
class TfmMicroHttpd : public TForm
{
__published:	// IDE-managed Components
        TLabel *lbLog;
        TMemo *mmLog;
        TTimer *Timer2;
        TCheckBox *cbLog;
        TButton *btGetSync;
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall btGetSyncClick(TObject *Sender);
private:	// User declarations
        void logln(String msg);
        String sLastDataUpload;
struct MHD_Daemon * d;

public:		// User declarations
        __fastcall TfmMicroHttpd(TComponent* Owner);
        static int apc_AcceptPolicyCallback(void * cls,
                const struct sockaddr * addr,
                socklen_t addrlen);
        static int iterate_post (
                void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
                const char *filename, const char *content_type,
                const char *transfer_encoding, const char *data, uint64_t off,
                size_t size);
        static void request_completed
                (void *cls, struct MHD_Connection *connection,
                void **con_cls, enum MHD_RequestTerminationCode toe);
        int mhd_EnqueueResponse(
                struct MHD_Connection * connection,
                const char * str,
                int mime = MIME_JSON);
        static int ahc_echo(void * cls,
		struct MHD_Connection * connection,
		const char * url,
		const char * method,
                const char * version,
		const char * upload_data,
		size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleShellAPI(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleDataAPI(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandlePointListRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleFilteredListRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleFilteredListSubstBayRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleSyncDataRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleExecExtAppRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleHistListRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleHideRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleStationListRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleStatusRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleAckRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleSilenceRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleWritePointRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleImposePointRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleCommandRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandleAckCommandRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);
        int ahc_HandlePointInfoRequest(void * cls,
                struct MHD_Connection * connection,
                const char * url,
                const char * method,
                const char * version,
                const char * upload_data,
                size_t * upload_data_size,
                void ** ptr);

};
//---------------------------------------------------------------------------
extern PACKAGE TfmMicroHttpd *fmMicroHttpd;
//---------------------------------------------------------------------------
#endif
