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
#include "bdtr.h"
#include "iec104_class.h"
#include "qiec104.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent( QCloseEvent *event );

public:
    MainWindow( QWidget *parent = 0 );
    ~MainWindow();

private slots:
    void on_cbLog_clicked(); // Check box for log messages changed
    void on_pbSendCommandsButton_clicked(); // Send Command pressed
    void on_pbConnect_clicked(); // connect button pressed
    void on_pbGI_clicked(); // GI button pressed
    void slot_timer_logmsg(); // timer for log messages
    void slot_timer_BDTR_kamsg(); // timer for sending keepalive BDTR messages
    void slot_BDTR_pronto_para_ler();  // BDTR: sinal para leitura de dados no tcp do BDTR
    void slot_dataIndication( iec_obj *obj, int numpoints );
    void slot_interrogationActConfIndication();
    void slot_interrogationActTermIndication();
    void slot_tcpconnect();         // tcp connect for iec104
    void slot_tcpdisconnect();      // tcp disconnect for iec104
    void slot_commandActConfIndication( iec_obj *obj );
    void slot_commandActTermIndication( iec_obj *obj );

    void on_pbCopyClipb_clicked(); // copy log messages to clipboard
    void on_pbCopyVals_clicked(); // copy values table to clipboard

private:
    std::map <std::pair<int,int>, QTableWidgetItem *> mapPtItem_ColAddress; // map of points to cells of table
    std::map <std::pair<int,int>, QTableWidgetItem *> mapPtItem_ColCommonAddress;
    std::map <std::pair<int,int>, QTableWidgetItem *> mapPtItem_ColValue;
    std::map <std::pair<int,int>, QTableWidgetItem *> mapPtItem_ColType;
    std::map <std::pair<int,int>, QTableWidgetItem *> mapPtItem_ColCause;
    std::map <std::pair<int,int>, QTableWidgetItem *> mapPtItem_ColFlags;
    std::map <std::pair<int,int>, QTableWidgetItem *> mapPtItem_ColCount;
    std::map <std::pair<int,int>, QTableWidgetItem *> mapPtItem_ColTimeTag;

    Ui::MainWindow *ui;
    QTimer *tmLogMsg; // timer to show log messages
    QIec104 i104;

    unsigned LastCommandAddress;
    int SendCommands;             // 1 = allow sending commands, 0 = don't send commands
    int Hide;

    // BDTR Related
    void BDTR_Loga( QString str, int id=0 ); // BDTR: log messages
    void BDTR_processPoints( iec_obj *obj, int numpoints ); // BDTR: process points
    inline bool BDTR_HaveDualHost() { return ( BDTR_host_dual != (QHostAddress)"0.0.0.0"); };
    bool isPrimary; // define se modo prim�rio ou secund�rio (o secund�rio permanece desconectado pelo IEC104)
    static const int BDTR_CntToBePrimary = 3; // counts necessary to be primary when not receiving keepalive messages
    int BDTR_CntDnToBePrimary; // countdown to be primary when not receiving keepalive messages
    int BDTR_Logar; // controla log das mensagens BDTR
    QTimer *tmBDTR_kamsg; // timer to send keep alive messages to the dual host
    static const int BDTR_seconds_kamsg = 7;
    QUdpSocket *udps; // BDTR: udp socket
    QHostAddress BDTR_host; // endere�o IP do cliente BDTR (normalmente a pr�pria m�quina)
    QHostAddress BDTR_host_dual; // endere�o IP do cliente BDTR dual
    static const int BDTR_porta = 65280; // porta UDP para envio de dados aos clientes BDTR
    static const int BDTR_porta_escuta = 65281; // porta UDP para envio de dados aos clientes BDTR
    static const unsigned char BDTR_orig = 0; // endere�o de origem BDTR

};

#endif // MAINWINDOW_H
