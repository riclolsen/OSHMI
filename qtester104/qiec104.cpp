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

#include <QtWidgets/QApplication>
#include "qiec104.h"

QIec104::QIec104( QObject *parent ) :
    QObject( parent )
{
mEnding = false;
mAllowConnect = true;
SendCommands = 0;
BDTRForcePrimary = 0;
mLog.activateLog();
mLog.doLogTime();

tcps = new QTcpSocket();
tmKeepAlive = new QTimer();

connect( tcps, SIGNAL(readyRead()), this, SLOT(slot_tcpreadytoread()) );
connect( tcps, SIGNAL(connected()), this, SLOT(slot_tcpconnect()) );
connect( tcps, SIGNAL(disconnected()), this, SLOT(slot_tcpdisconnect()) );
connect( tcps, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_tcperror(QAbstractSocket::SocketError)),Qt::DirectConnection );
connect( tmKeepAlive, SIGNAL(timeout()), this, SLOT(slot_keep_alive()) );

tcps->moveToThread( &tcpThread );
tcpThread.start( QThread::TimeCriticalPriority );
}

QIec104::~QIec104()
{
delete tmKeepAlive;
delete tcps;
}

void QIec104::dataIndication( iec_obj *obj, int numpoints )
{
    emit signal_dataIndication( obj, numpoints );
}

void QIec104::connectTCP()
{
static int cnt = 0;
char buf[100];

    tcps->close();
    if ( !mEnding && mAllowConnect )
      { // alternate main and backup UTR IP address, if configured
      if ( (++cnt) % 2 || strcmp(getSecondaryIP_backup(), "") == 0 )
        {
        tcps->connectToHost( getSecondaryIP(), getPortTCP(), QIODevice::ReadWrite );
        sprintf( buf, "Try to connect IP: %s", getSecondaryIP() );
        mLog.pushMsg( (const char *)buf );
        }
      else
        {
        tcps->connectToHost( getSecondaryIP_backup(), getPortTCP(), QIODevice::ReadWrite );
        sprintf( buf, "Try to connect IP: %s", getSecondaryIP_backup() );
        mLog.pushMsg( (const char *)buf );
        }
      }
}

void QIec104::disconnectTCP()
{
     tcps->close();
}

void QIec104::slot_tcperror( QAbstractSocket::SocketError socketError )
{
  if ( socketError != QAbstractSocket::SocketTimeoutError )
    {
    char buf[100];
    sprintf( buf, "SocketError: %d", socketError );
    mLog.pushMsg( (const char *)buf );
    }
}

int QIec104::readTCP( char * buf, int szmax )
{
    if (!mEnding)
      return tcps->read( buf, szmax );
    else
      return 0;
}

// send tcp data, user provided
void QIec104::sendTCP( char * data, int sz )
{
    if ( tcps->state() == QAbstractSocket::ConnectedState )
    if ( !mEnding )
      tcps->write( data, sz );
}

void QIec104::slot_tcpconnect()
{
    tcps->setSocketOption( QAbstractSocket::LowDelayOption, 1 );
    onConnectTCP();
    emit signal_tcp_connect();
}

void QIec104::slot_tcpdisconnect()
{
    onDisconnectTCP();
    emit signal_tcp_disconnect();
}

void QIec104::slot_keep_alive()
{
    static unsigned int cnts = 1;

    if ( !mEnding )
      {
        cnts++;

        if ( ! (cnts % 5) )
            if ( tcps->state() != QAbstractSocket::ConnectedState && mAllowConnect )
            {
                mLog.pushMsg("!!!!!TRY TO CONNECT!");
                connectTCP();
            }

        onTimerSecond();
      }
}

void  QIec104::interrogationActConfIndication()
{
    emit signal_interrogationActConfIndication();
}

void  QIec104::interrogationActTermIndication()
{
    emit signal_interrogationActTermIndication();
}

void QIec104::commandActConfIndication( iec_obj *obj )
{
    emit signal_commandActConfIndication( obj );
}

void QIec104::commandActTermIndication( iec_obj *obj )
{
    emit signal_commandActTermIndication( obj );
}

void QIec104::terminate()
{
mEnding = true;
tcps->close();
tcpThread.quit();
tcpThread.wait( 1000 );
if ( tcpThread.isRunning() )
  tcpThread.terminate();
if ( tcpThread.isRunning() )
  tcpThread.wait( 2000 );
}

void QIec104::slot_tcpreadytoread()
{
if ( tcps->bytesAvailable() < 6 )
  return;

packetReadyTCP();

int cnt = 0;
// espera para ver se chega mais alguma coisa pela rede
while ( (tcps->bytesAvailable() > 5) && (cnt++ < 10) )
  {
  tcps->waitForReadyRead( 10 );
  packetReadyTCP();
  }
}

void QIec104::disable_connect()
{
    mAllowConnect = false;
    if ( tcps->state() == QAbstractSocket::ConnectedState )
      disconnectTCP();
}

void QIec104::enable_connect()
{
    mAllowConnect = true;
}
