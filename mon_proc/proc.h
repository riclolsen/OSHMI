//----------------------------------------------------------------------------
//Borland C++Builder
//Copyright (c) 1987, 1998 Borland International Inc. All Rights Reserved.
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#ifndef ProcH
#define ProcH
//----------------------------------------------------------------------------
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
#include <Dialogs.hpp>
#include <Forms.hpp>
#include <Controls.hpp>
#include <Graphics.hpp>
#include <Classes.hpp>
#include <SysUtils.hpp>
#include <Messages.hpp>
#include <Windows.hpp>
#include <System.hpp>
#include <NMUDP.hpp>
//----------------------------------------------------------------------------
class TFormMain : public TForm
{
protected:
    void __fastcall WMQueryEndSession(TMessage &Message);
    void __fastcall WMEndSession(TMessage &Message);
BEGIN_MESSAGE_MAP MESSAGE_HANDLER(WM_QUERYENDSESSION, TMessage, WMQueryEndSession)
                  MESSAGE_HANDLER(WM_ENDSESSION, TMessage, WMEndSession)
END_MESSAGE_MAP(TForm)

__published:
	TPanel *Panel1;
	TBevel *Bevel1;
    TSpeedButton *SpeedButton1;
        TTimer *TimerWatchdog;
  TNMUDP *NMUDP1;
  TPageControl *PageControl1;
  TTabSheet *TabSheet1;
  TListBox *ListBox1;
  TTabSheet *TabSheet2;
  TListBox *lbLog;
  TTabSheet *TabSheet3;
  TMemo *memoMens;
  TTabSheet *TabSheet4;
  TListBox *lbTodosProc;
  TCheckBox *cbWatchdog;
  TLabel *Label1;
  void __fastcall SpeedButton1Click(TObject *Sender);
  void __fastcall TimerWatchdogTimer(TObject *Sender);
  void __fastcall NMUDP1DataReceived(TComponent *Sender, int NumberBytes,
          AnsiString FromIP, int Port);
  void __fastcall ListBox1Click(TObject *Sender);
  void __fastcall cbWatchdogClick(TObject *Sender);
public:
  virtual __fastcall TFormMain(TComponent *Owner);
private:
  int ProcIndex;
  int HIDE;
};
//----------------------------------------------------------------------------
extern TFormMain *FormMain;
//----------------------------------------------------------------------------
#endif	
