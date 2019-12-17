/*
 * This software implements an IEC 60870-5-104 protocol tester.
 * Copyright © 2010-2017 Ricardo L. Olsen
 *
 * Disclaimer
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QPushButton>
#include <QTimer>
#include <QSettings>
#include <QtWidgets/QTableWidgetItem>
#include <map>
#include "iec104_class.h"
#include "qiec104.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 protected:
  void closeEvent(QCloseEvent* event);

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 private slots:
  void on_cbLog_clicked(); // Check box for log messages changed
  void on_pbSendCommandsButton_clicked(); // Send Command pressed
  void on_pbConnect_clicked(); // connect button pressed
  void on_pbGI_clicked(); // GI button pressed
  void slot_timer_logmsg(); // timer for log messages
  void slot_timer_I104M_kamsg(); // timer for sending keepalive I104M messages
  void slot_I104M_ready_to_read();  // I104M: slot to read data from OSHMI UDP
  void slot_dataIndication(iec_obj* obj, unsigned numpoints);
  void slot_interrogationActConfIndication();
  void slot_interrogationActTermIndication();
  void slot_tcpconnect();         // tcp connect for iec104
  void slot_tcpdisconnect();      // tcp disconnect for iec104
  void slot_commandActRespIndication(iec_obj* obj);

  void on_pbCopyClipb_clicked(); // copy log messages to clipboard
  void on_pbCopyVals_clicked(); // copy values table to clipboard

 private:
  std::map <std::pair<int, int>, QTableWidgetItem*> mapPtItem_ColAddress; // map of points to cells of table
  std::map <std::pair<int, int>, QTableWidgetItem*> mapPtItem_ColCommonAddress;
  std::map <std::pair<int, int>, QTableWidgetItem*> mapPtItem_ColValue;
  std::map <std::pair<int, int>, QTableWidgetItem*> mapPtItem_ColType;
  std::map <std::pair<int, int>, QTableWidgetItem*> mapPtItem_ColCause;
  std::map <std::pair<int, int>, QTableWidgetItem*> mapPtItem_ColFlags;
  std::map <std::pair<int, int>, QTableWidgetItem*> mapPtItem_ColCount;
  std::map <std::pair<int, int>, QTableWidgetItem*> mapPtItem_ColTimeTag;

  Ui::MainWindow* ui;
  QTimer* tmLogMsg; // timer to show log messages
  QIec104 i104;

  unsigned LastCommandAddress;
  int SendCommands;             // 1 = allow sending commands, 0 = don't send commands
  int Hide;

  // I104M Related
  void I104M_Loga(QString str, int id = 0); // I104M: log messages
  void I104M_processPoints(iec_obj* obj, unsigned numpoints);   // I104M: process points
  inline bool I104M_HaveDualHost() { return (I104M_host_dual != QHostAddress("0.0.0.0")); }
  QHostAddress I104M_host; // IP address from OSHMI main machine
  QHostAddress I104M_host_dual; // OSHMI dual host address (the other machine)
  static const int I104M_porta = 8099; // UDP port to send data to OSHMI
  static const int I104M_porta_escuta = 8098; // udp port to receive commands from OSHMI
  static const int I104M_CntToBePrimary = 3; // counts necessary to be primary when not receiving keepalive messages
  static const int I104M_seconds_kamsg = 7; // period of keepalive messages
  int I104M_CntDnToBePrimary; // countdown to be primary when not receiving keepalive messages
  int I104M_Logar; // controls log of I104M messages
  bool isPrimary; // primary or secondary redundant mode
  QUdpSocket* udps; // I104M: udp socket
  QTimer* tmI104M_kamsg; // timer to send keep alive messages to the dual host
  void SendOSHMI(char* msg, unsigned int packet_size);
  void fmtCP56Time(char*, cp56time2a*);
};


#pragma pack(push)
#pragma pack(1) // byte aligned structures

#define I104M_LISTENUDPPORT 8098
#define I104M_WRITEUDPPORT 8099

#define I104M_ASDU_SPECIAL_CMD 1001
#define I104M_SPECIAL_CMD_ADDR_REQ_GI 0
#define I104M_SPECIAL_CMD_ADDR_KEEP_ALIVE 1

#define PKTDIG_MAXPOINTS       250
#define PKTEVE_MAXPOINTS       100
#define PKTANA_MAXPOINTS       150

#define MSGKA_SIG 0x5a5a5a5a

#define MSGSUP_SIG 0x53535353
typedef struct {
  uint32_t signature;  // 0x53535353
  uint32_t endereco;
  uint32_t tipo;
  uint32_t prim;
  uint32_t sec;
  uint32_t causa;
  uint32_t taminfo;
  unsigned char info[255];
} t_msgsup;

#define MSGSUPSQ_SIG 0x64646464
typedef struct {
  uint32_t signature;  // 0x64646464
  uint32_t numpoints;
  uint32_t tipo;
  uint32_t prim;
  uint32_t sec;
  uint32_t causa;
  uint32_t taminfo; // value size for the type (not counting the 4 byte address)
  unsigned char info[2000]; // { 4 bytes uint32_t address, point value (taminfo bytes) } ...  Repeat
} t_msgsupsq;

#define MSGCMD_SIG 0x4b4b4b4b
typedef struct {
  uint32_t signature; // 0x4b4b4b4b
  uint32_t endereco;
  uint32_t tipo;
  union {
    uint32_t onoff;
    float setpoint;
    int32_t setpoint_i32;
    short int setpoint_i16;
  };
  uint32_t sbo;
  uint32_t qu;
  uint32_t utr;
} t_msgcmd;

typedef struct {
  unsigned short nponto; // point address 1st & 2nd bytes
  unsigned char nponto3; // point address 3rd byte
  unsigned char iq;      // state & qualifier
  unsigned short ms;     // milli seconds
  unsigned char min;     // minute
  unsigned char hora;    // hour
  unsigned char dia;     // day
  unsigned char mes;
  unsigned char ano;
} digital_w_time7;

typedef struct {
  unsigned char iq;     // state & qualifier
  unsigned short ms;    // milli seconds
  unsigned char min;    // minute
  unsigned char hora;   // hour
  unsigned char dia;    // day
  unsigned char mes;
  unsigned char ano;
} digital_w_time7_seq;


typedef struct {
  unsigned char iq;      // state & qualifier
} digital_notime_seq;

typedef struct {
  unsigned short nponto; // point address 1st & 2nd bytes
  unsigned char nponto3; // point address 3rd byte
  short sva;             // analog value 16bit integer
  unsigned char qds;     // qualifier
} analogico;

typedef struct {
  short sva;         // analog value 16bit integer
  unsigned char qds; // qualifier
} analogico_seq;

typedef struct {
  unsigned short nponto; // point address 1st & 2nd bytes
  unsigned char nponto3; // point address 3rd byte
  unsigned char vti;     // step pos
  unsigned char qds;     // qualifier
} step;

typedef struct {
  unsigned char vti;   // step pos
  unsigned char qds;   // qualifier
} step_seq;

typedef struct {
  unsigned short nponto; // point address 1st & 2nd bytes
  unsigned char nponto3; // point address 3rd byte
  float fr;              // analog value 4byte float
  unsigned char qds;     // qualifier
} flutuante;

typedef struct {
  float fr;         // analog value 4byte float
  unsigned char qds;  // qualifier
} flutuante_seq;

typedef struct {
  unsigned short nponto; // point address 1st & 2nd bytes
  unsigned char nponto3; // point address 3rd byte
  uint32_t bcr;      // valor binary counter reading
  unsigned char qds;     // qualifier
} integrated;

typedef struct {
  uint32_t bcr;   // value binary counter reading
  unsigned char qds;  // qualifies
} integrated_seq;

#pragma pack(pop)

#endif // MAINWINDOW_H
