/*
 * This software implements an IEC 60870-5-104 protocol tester.
 * Copyright © 2010,2011,2012 Ricardo L. Olsen
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

#ifndef QIEC104_H
#define QIEC104_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QtNetwork/QTcpSocket>
#include <iec104_class.h>

class QIec104 : public QObject, public iec104_class
{
    Q_OBJECT

public:
    explicit QIec104(QObject *parent = 0);
    ~QIec104();
    int SendCommands; // 1 = allow sending commands, 0 = don't send commands
    int BDTRForcePrimary; // 1 = force primary (cant't stay secondary) , 0 = can be secondary
    QTimer *tmKeepAlive; // 1 second timer
    QTcpSocket *tcps; // socket for iec104 (tcp)
    void terminate();
    void disable_connect();
    void enable_connect();

signals:
    void signal_dataIndication( iec_obj *obj, int numpoints );
    void signal_interrogationActConfIndication();
    void signal_interrogationActTermIndication();
    void signal_tcp_connect();
    void signal_tcp_disconnect();
    void signal_commandActConfIndication(iec_obj *obj);
    void signal_commandActTermIndication(iec_obj *obj);

public slots:
    void slot_tcpdisconnect(); // tcp disconnect for iec104

private slots:
    void slot_tcpconnect(); // tcp connect for iec104
    void slot_tcpreadytoread(); // ready to read data on iec104 tcp socket
    void slot_tcperror( QAbstractSocket::SocketError socketError ); // show errors of tcp
    void slot_keep_alive(); // timer de 1s

private:
    QThread tcpThread;

    // redefine for iec104_class
    void connectTCP();
    void disconnectTCP();
    int readTCP( char * buf, int szmax );
    void sendTCP( char * data, int sz );
    void interrogationActConfIndication();
    void interrogationActTermIndication();
    void commandActConfIndication( iec_obj *obj );
    void commandActTermIndication( iec_obj *obj );
    void dataIndication(iec_obj *obj, int numpoints);
    bool mEnding;
    bool mAllowConnect;
};


#endif // QIEC104_H
