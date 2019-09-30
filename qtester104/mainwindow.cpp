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

#include <QClipboard>
#include <QDir>
#include <QCloseEvent>
#include <QDateTime>
#include <QItemSelectionModel>
#include <string>
#include <sstream>
#include <time.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

#define QTESTER_VERSION "v2.0"
#define QTESTER_COPYRIGHT "Copyright © 2010-2019 Ricardo Lastra Olsen"
#define CURDIRINIFILENAME "/qtester104.ini"
#define CONFDIRINIFILENAME "../conf/qtester104.ini"

//-------------------------------------------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    I104M_Logar = 1;
    i104.mLog.deactivateLog();

    // look for the ini file in the application dir, if not found use the conf dir
    QString ininame = QCoreApplication::applicationDirPath() + CURDIRINIFILENAME;
    if (!QFile(ininame).exists())
    {
      ininame = CONFDIRINIFILENAME;
    }

    if ( QCoreApplication::arguments().count() > 1 )
    {
      ininame = QCoreApplication::arguments().at(1);
    }

    // busca configuracoes no arquivo ini
    QSettings settings( ininame, QSettings::IniFormat );

    i104.setPrimaryAddress( settings.value( "IEC104/PRIMARY_ADDRESS", 1 ).toInt() );
    i104.ForcePrimary = settings.value( "I104M/FORCE_PRIMARY", 0 ).toInt();
    i104.setSecondaryAddress( settings.value( "RTU1/SECONDARY_ADDRESS", 1 ).toInt() );
    i104.SendCommands = settings.value( "RTU1/ALLOW_COMMANDS", 0 ).toInt();

    QString IPEscravo;
    IPEscravo = settings.value( "RTU1/IP_ADDRESS_BACKUP", "" ).toString();
    i104.setSecondaryIP_backup( const_cast<char *>(IPEscravo.toStdString().c_str()) );
    IPEscravo = settings.value( "RTU1/IP_ADDRESS", "" ).toString();
    i104.setSecondaryIP( const_cast<char *>(IPEscravo.toStdString().c_str()) );
    i104.setPortTCP( settings.value( "RTU1/TCP_PORT", i104.getPortTCP() ).toUInt() );
    i104.setGIPeriod( settings.value( "RTU1/GI_PERIOD", 330 ).toUInt() );

    // this is for using with the OSHMI HMI in a dual architecture
    QSettings settings_oshmi( "../conf/hmi.ini", QSettings::IniFormat );
    I104M_host_dual.setAddress(settings_oshmi.value( "REDUNDANCY/OTHER_HMI_IP", "0.0.0.0" ).toString());
    I104M_host.setAddress("127.0.0.1");
    I104M_CntDnToBePrimary = I104M_CntToBePrimary;

    ui->setupUi( this );

    // this is for hiding the window when runnig
    Hide = settings_oshmi.value( "RUN/HIDE", "" ).toInt();

    this->setWindowTitle( tr("QTester104 IEC60870-5-104") );

    QIntValidator * validator = new QIntValidator( 0, 65535, this );
    ui->leLinkAddress->setValidator( validator );
    QIntValidator * validator1 = new QIntValidator( 0, 65535, this );
    ui->lePort->setValidator( validator1 );
    QIntValidator * validator2 = new QIntValidator( 0, 255, this );
    ui->leMasterAddress->setValidator( validator2 );

    QRegExp rx( "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b" );
    QValidator *valip = new QRegExpValidator( rx, this );
    ui->leIPRemoto->setValidator( valip );

    udps = new QUdpSocket();
    udps->bind( I104M_porta_escuta );
    udps->open( QIODevice::ReadWrite );

    QString qs;
    QTextStream( &qs ) << i104.getPortTCP();
    ui->lePort->setText( qs );
    ui->leIPRemoto->setText( IPEscravo );
    qs = "";
    QTextStream( &qs ) << i104.getSecondaryAddress();
    ui->leLinkAddress->setText( qs );
    qs = "";
    QTextStream( &qs ) << i104.getPrimaryAddress();
    ui->leMasterAddress->setText( qs );

    ui->leIPRemoto->setText( IPEscravo );

    tmLogMsg = new QTimer();
    tmI104M_kamsg = new QTimer();

    connect( udps, SIGNAL(readyRead()), this, SLOT(slot_I104M_ready_to_read()) );
    connect( tmLogMsg, SIGNAL(timeout()), this, SLOT(slot_timer_logmsg()) );
    connect( tmI104M_kamsg, SIGNAL(timeout()), this, SLOT(slot_timer_I104M_kamsg()) );
    connect( &i104, SIGNAL(signal_dataIndication(iec_obj *, unsigned)), this, SLOT(slot_dataIndication(iec_obj *, unsigned)) );
    connect( &i104, SIGNAL(signal_interrogationActConfIndication()), this, SLOT(slot_interrogationActConfIndication()) );
    connect( &i104, SIGNAL(signal_interrogationActTermIndication()), this, SLOT(slot_interrogationActTermIndication()) );
    connect( &i104, SIGNAL(signal_tcp_connect()), this, SLOT(slot_tcpconnect()) );
    connect( &i104, SIGNAL(signal_tcp_disconnect()), this, SLOT(slot_tcpdisconnect()) );
    connect( &i104, SIGNAL(signal_commandActRespIndication(iec_obj *)), this, SLOT(slot_commandActRespIndication(iec_obj *)) );

    ui->pbGI->setEnabled(false);
    ui->pbSendCommandsButton->setEnabled( false );

    ui->twPontos->clear();
    ui->twPontos->setSortingEnabled ( false );
    ui->twPontos->setColumnCount( 8 );
    ui->twPontos->sortByColumn( 0, Qt::AscendingOrder );

    if ( IPEscravo != "" )
      on_pbConnect_clicked();

    QStringList colunas;
    colunas << "Address" << "CA" << "Value" << "ASDU" << "Cause" << "Flags" << "Count" << "TimeTag";
    ui->twPontos->setHorizontalHeaderLabels( colunas );

    tmLogMsg->start(500);

    if ( I104M_HaveDualHost() )
    {
        tmI104M_kamsg->start( I104M_seconds_kamsg * 1000 );
        isPrimary = false;
        i104.disable_connect();
        ui->lbMode->setText( "<font color='red'>Secondary</font>" );
    }
    else
    {
        isPrimary = true;
        i104.enable_connect();
        ui->lbMode->setText( "<font color='green'>Primary</font>" );
    }

    ui->lbCopyright->setText( QString(QTESTER_VERSION) + QString(" - ") + QString(QTESTER_COPYRIGHT) );

    QFont font = QFont("Consolas");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(9);
    font.setFixedPitch(true);
    ui->lwLog->setFont(font);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete tmLogMsg;
    delete tmI104M_kamsg;
}

void MainWindow::on_pbGI_clicked()
{
    i104.solicitGI();
}

void MainWindow::on_pbConnect_clicked()
{
    if ( i104.tmKeepAlive->isActive() )
    {
        i104.tmKeepAlive->stop();
        i104.tcps->close();
        i104.slot_tcpdisconnect();
    }
    else
    {
        i104.setSecondaryIP( const_cast<char *>(ui->leIPRemoto->text().toStdString().c_str()) );
        i104.setPortTCP( ui->lePort->text().toUInt() );
        i104.setSecondaryAddress( ui->leLinkAddress->text().toInt() );
        i104.setPrimaryAddress( ui->leMasterAddress->text().toInt() );

        QString qs;
        ui->leIPRemoto->setText( i104.getSecondaryIP() );
        QTextStream( &qs ) << ui->leLinkAddress->text().toInt();
        ui->leLinkAddress->setText( qs );
        qs="";
        QTextStream( &qs ) << ui->leMasterAddress->text().toInt();
        ui->leMasterAddress->setText( qs );

        ui->lePort->setEnabled( false );
        ui->leIPRemoto->setEnabled( false );
        ui->leLinkAddress->setEnabled( false );
        ui->leMasterAddress->setEnabled( false );

        ui->pbConnect->setText( "Give up..." );
        ui->lbStatus->setText( "<font color='green'>TRYING TO CONNECT!</font>" );

        mapPtItem_ColAddress.clear();
        mapPtItem_ColCommonAddress.clear();
        mapPtItem_ColValue.clear();
        mapPtItem_ColType.clear();
        mapPtItem_ColCause.clear();
        mapPtItem_ColFlags.clear();
        mapPtItem_ColCount.clear();
        mapPtItem_ColTimeTag.clear();
        ui->twPontos->clearContents();
        ui->twPontos->setRowCount ( 0 );
        // ui->lwLog->clear();
        i104.tmKeepAlive->start(1000);
    }
}

// receive data/commands from OSHMI
void MainWindow::slot_I104M_ready_to_read()
{
    char buf[5000];

    unsigned char br[2000]; // buffer de recepcao
    int bytesrec;

    while ( udps->hasPendingDatagrams() )
    {
    QHostAddress address;
    quint16 port;
    bytesrec = int(udps->readDatagram( reinterpret_cast<char *>(br), sizeof(br), &address, &port ));

    if ( bytesrec <= 0)
       return;

    // I104M message must be local or from redundant computer, consider ipv4 & ipv6
    if ( address.toString()!="127.0.0.1" &&
         address.toString()!="::ffff:127.0.0.1" &&
         address.toString()!=I104M_host_dual.toString() &&
         address.toString()!= (QString("::ffff:") + I104M_host_dual.toString())
       )
       {
       I104M_Loga( QString("R--> I104M: Message from invalid origin ") + address.toString());
       return;
       }

    // I104M message
    t_msgcmd * pmsg = reinterpret_cast<t_msgcmd *>(br);

    sprintf( buf, "%3d: I104M: ", bytesrec );
    for ( int i=0; i<bytesrec; i++ )
        sprintf (buf+strlen(buf), "%02x ", br[i]);
    I104M_Loga( buf );

    if (pmsg->signature != MSGCMD_SIG)
        I104M_Loga( "R--> I104M: Invalid Message!" );

    iec_obj obj;
    obj.cause = iec104_class::ACTIVATION;
    obj.address = pmsg->endereco;
    obj.ca = static_cast<unsigned short>(pmsg->utr);
    obj.qu = static_cast<unsigned char>(pmsg->qu);
    obj.se = static_cast<unsigned char>(pmsg->sbo);
    obj.type = static_cast<unsigned char>(pmsg->tipo);

    switch ( pmsg->tipo )
        {
        case I104M_ASDU_SPECIAL_CMD: // special command
            if (pmsg->endereco == I104M_SPECIAL_CMD_ADDR_REQ_GI) // request general interrogation
            {
              I104M_Loga( "R--> I104M: REQ GI" );
              i104.solicitGI();
            }
            else
            if (pmsg->endereco == I104M_SPECIAL_CMD_ADDR_KEEP_ALIVE &&
                (address.toString()!=I104M_host_dual.toString() || address.toString()!= (QString("::ffff:") + I104M_host_dual.toString())) &&
                i104.ForcePrimary==0) // keep alive
            {
              I104M_Loga( "R--> I104M: KEEP ALIVE FROM REDUNDANT COMPUTER" );
              if (isPrimary)
              {
                I104M_Loga( "     I104M: BECOMMING SECONDARY!" );
                ui->lbMode->setText("<font color='red'></font>");
              }
              isPrimary = false;
              i104.disable_connect();
              I104M_CntDnToBePrimary = I104M_CntToBePrimary; // restart count to be primary
            }
            break;
        case iec104_class::C_SC_TA_1: // single command with time tag
        case iec104_class::C_SC_NA_1: // single command
            sprintf(buf, "R--> I104M: Single Command %s", pmsg->onoff?"on":"off");
            I104M_Loga(buf);
            obj.sp = static_cast<unsigned char>(pmsg->onoff);
            i104.sendCommand( &obj );
            LastCommandAddress = obj.address;
            break;
        case iec104_class::C_DC_TA_1: // double command with time tag
        case iec104_class::C_DC_NA_1: // double command
            sprintf(buf, "R--> I104M: Double Command %s", pmsg->onoff?"on":"off");
            I104M_Loga(buf);
            obj.dp = pmsg->onoff?2:1;
            i104.sendCommand( &obj );
            LastCommandAddress = obj.address;
            break;
        case iec104_class::C_SE_TA_1: // set-point normalised command with time tag
        case iec104_class::C_SE_NA_1: // set-point normalised command
            sprintf(buf, "R--> I104M: set-point normalised command %f", double(pmsg->setpoint));
            I104M_Loga(buf);
            obj.value = pmsg->setpoint;
            i104.sendCommand( &obj );
            LastCommandAddress = obj.address;
            break;
        case iec104_class::C_SE_TB_1: // set-point scaled command with time tag
        case iec104_class::C_SE_NB_1: // set-point scaled command
            sprintf(buf, "R--> I104M: set-point scaled command %f", double(pmsg->setpoint));
            I104M_Loga(buf);
            obj.value = pmsg->setpoint;
            i104.sendCommand( &obj );
            LastCommandAddress = obj.address;
            break;
        case iec104_class::C_SE_TC_1: // set-point short floating point command with time tag
        case iec104_class::C_SE_NC_1: // set-point short floating point command
            sprintf(buf, "R--> I104M: set-point short floating command %f", double(pmsg->setpoint));
            I104M_Loga(buf);
            obj.value = pmsg->setpoint;
            i104.sendCommand( &obj );
            LastCommandAddress = obj.address;
            break;
        case iec104_class::C_RC_TA_1: // regulating step command with time tag
        case iec104_class::C_RC_NA_1: // regulating step command
            sprintf(buf, "R--> I104M: regulating step command %f", double(pmsg->setpoint));
            I104M_Loga(buf);
            obj.rcs = static_cast<unsigned char>(pmsg->setpoint);
            i104.sendCommand( &obj );
            LastCommandAddress = obj.address;
            break;
        case iec104_class::C_BO_TA_1: // bitstring command with time tag
        case iec104_class::C_BO_NA_1: // bitstring command
            sprintf(buf, "R--> I104M: bitstring command %u", pmsg->setpoint_i32);
            I104M_Loga(buf);
            obj.value = pmsg->setpoint_i32;
            i104.sendCommand( &obj );
            LastCommandAddress = obj.address;
            break;
        }
    }
}

// Envio de comando
void MainWindow::on_pbSendCommandsButton_clicked()
{
    iec_obj obj;
    obj.type = static_cast<unsigned char>(ui->cbCmdAsdu->currentText().left(ui->cbCmdAsdu->currentText().indexOf(':')).toUInt());

    // reset process must have value set (qrp)
    if ( obj.type == iec104_class::C_RP_NA_1 )
    {
        if ( ui->leCmdValue->text().trimmed() == "" )
            return;
        obj.address = 0;
        obj.value = ui->leCmdValue->text().toFloat();
    }
    else
    // test command parameters if not sync command or test command (that don't have parameters)
    if (obj.type != iec104_class::C_CS_NA_1 && obj.type != iec104_class::C_TS_TA_1)
    {
        if ( ui->leCmdValue->text().trimmed() == "" || ui->leCmdAddress->text().trimmed() == "" )
            return;
        if ( ui->leCmdAddress->text().toInt() == 0 )
            return;

        obj.address = ui->leCmdAddress->text().toUInt();
        obj.value = ui->leCmdValue->text().toFloat();
    }

    obj.ca = ui->leASDUAddr->text().toUShort();
    i104.setSecondaryASDUAddress( obj.ca );
    QDateTime current = QDateTime::currentDateTime();

    switch ( obj.type )
    {
    case iec104_class::C_SC_NA_1:
    case iec104_class::C_SC_TA_1:
        obj.scs = static_cast<unsigned char>(ui->leCmdValue->text().toUInt());
        break;
    case iec104_class::C_DC_NA_1:
    case iec104_class::C_DC_TA_1:
        obj.dcs = static_cast<unsigned char>(ui->leCmdValue->text().toUInt());
        break;
    case iec104_class::C_RC_NA_1:
    case iec104_class::C_RC_TA_1:
        obj.rcs = static_cast<unsigned char>(ui->leCmdValue->text().toUInt());
        break;
    case iec104_class::C_SE_NA_1:
    case iec104_class::C_SE_TA_1:
        obj.value = ui->leCmdValue->text().toInt();
        break;
    case iec104_class::C_SE_NB_1:
    case iec104_class::C_SE_TB_1:
        obj.value = ui->leCmdValue->text().toInt();
        break;
    case iec104_class::C_SE_NC_1:
    case iec104_class::C_SE_TC_1:
        obj.value = ui->leCmdValue->text().toFloat();
        break;
    case iec104_class::C_CS_NA_1:
        obj.timetag.year = static_cast<unsigned char>(current.date().year()%100);
        obj.timetag.month = static_cast<unsigned char>(current.date().month());
        obj.timetag.mday = static_cast<unsigned char>(current.date().day());
        obj.timetag.hour = static_cast<unsigned char>(current.time().hour());
        obj.timetag.min = static_cast<unsigned char>(current.time().minute());
        obj.timetag.msec = static_cast<unsigned short>(current.time().second()*1000 + current.time().msec());
        obj.timetag.iv = 0;
        obj.timetag.su = 0;
        obj.timetag.wday = 0;
        obj.timetag.res1 = 0;
        obj.timetag.res2 = 0;
        obj.timetag.res3 = 0;
        obj.timetag.res4 = 0;
        break;
    case iec104_class::C_TS_TA_1:
        obj.timetag.year = static_cast<unsigned char>(current.date().year()%100);
        obj.timetag.month = static_cast<unsigned char>(current.date().month());
        obj.timetag.mday = static_cast<unsigned char>(current.date().day());
        obj.timetag.hour = static_cast<unsigned char>(current.time().hour());
        obj.timetag.min = static_cast<unsigned char>(current.time().minute());
        obj.timetag.msec = static_cast<unsigned short>(current.time().second()*1000 + current.time().msec());
        obj.timetag.iv = 0;
        obj.timetag.su = 0;
        obj.timetag.wday = 0;
        obj.timetag.res1 = 0;
        obj.timetag.res2 = 0;
        obj.timetag.res3 = 0;
        obj.timetag.res4 = 0;
        break;
    case iec104_class::C_RP_NA_1:
        break;
    }
    obj.qu = static_cast<unsigned char>(ui->cbCmdDuration->currentText().left(1).toUInt());
    obj.se = static_cast<unsigned char>(ui->cbSBO->isChecked());

    i104.sendCommand( &obj );
    LastCommandAddress = obj.address;
}

void MainWindow::I104M_Loga( QString str, int id )
{
    if  (I104M_Logar && id == 0 )
    {
        i104.mLog.pushMsg( const_cast<char*>(str.toStdString().c_str()), 0 );
    }
}

void MainWindow::slot_dataIndication( iec_obj *obj, unsigned numpoints )
{
    char buf[1000];
    char buftt[1000];
    int rw = -1;
    bool inserted = false;
    QTableWidgetItem *pitem;
    static const char* dblmsg[] = { "tra ","off ","on ","ind " };

    I104M_processPoints( obj, numpoints );

    if ( ui->cbPointMap->isChecked() )
    {
        for (unsigned i=0; i< numpoints; i++, obj++)
        {
            pitem = nullptr;
            pitem = mapPtItem_ColAddress[std::make_pair(obj->ca,obj->address)];
            if ( pitem == nullptr )
            {
                sprintf( buf, "%06u", obj->address );

                // insere
                rw = ui->twPontos->rowCount();
                ui->twPontos->insertRow( rw );
                QTableWidgetItem *newItem = new QTableWidgetItem(buf);
                newItem->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
                ui->twPontos->setItem( rw, 0, newItem );
                newItem->setFlags( Qt::ItemIsSelectable );
                mapPtItem_ColAddress[std::make_pair(obj->ca,obj->address)] = newItem;

                newItem = new QTableWidgetItem();
                newItem->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
                ui->twPontos->setItem( rw, 1, newItem );
                newItem->setFlags( Qt::ItemIsSelectable );
                mapPtItem_ColCommonAddress[std::make_pair(obj->ca,obj->address)] = newItem;

                newItem = new QTableWidgetItem();
                newItem->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
                ui->twPontos->setItem( rw, 2, newItem );
                newItem->setFlags( Qt::ItemIsSelectable );
                mapPtItem_ColValue[std::make_pair(obj->ca,obj->address)] = newItem;

                newItem = new QTableWidgetItem();
                newItem->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
                ui->twPontos->setItem( rw, 3, newItem );
                newItem->setFlags( Qt::ItemIsSelectable );
                mapPtItem_ColType[std::make_pair(obj->ca,obj->address)] = newItem;

                newItem = new QTableWidgetItem();
                newItem->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
                ui->twPontos->setItem( rw, 4, newItem );
                newItem->setFlags( Qt::ItemIsSelectable );
                mapPtItem_ColCause[std::make_pair(obj->ca,obj->address)] = newItem;

                newItem = new QTableWidgetItem();
                newItem->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
                ui->twPontos->setItem( rw, 5, newItem );
                newItem->setFlags( Qt::ItemIsSelectable );
                mapPtItem_ColFlags[std::make_pair(obj->ca,obj->address)] = newItem;

                newItem = new QTableWidgetItem("0");
                newItem->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
                ui->twPontos->setItem( rw, 6, newItem );
                newItem->setFlags( Qt::ItemIsSelectable );
                mapPtItem_ColCount[std::make_pair(obj->ca,obj->address)] = newItem;

                newItem = new QTableWidgetItem();
                ui->twPontos->setItem( rw, 7, newItem );
                newItem->setFlags( Qt::ItemIsSelectable );
                mapPtItem_ColTimeTag[std::make_pair(obj->ca,obj->address)] = newItem;

                inserted = true;
            }

            sprintf( buf, "%9.3f", double(obj->value) );
            mapPtItem_ColValue[std::make_pair(obj->ca,obj->address)]->setText( buf );
            sprintf( buf, "%u", obj->ca );
            mapPtItem_ColCommonAddress[std::make_pair(obj->ca,obj->address)]->setText( buf );
            sprintf( buf, "%d", obj->type );
            mapPtItem_ColType[std::make_pair(obj->ca,obj->address)]->setText( buf );
            sprintf( buf, "%d", obj->cause );
            mapPtItem_ColCause[std::make_pair(obj->ca,obj->address)]->setText( buf );
            sprintf( buf, "%d", 1+mapPtItem_ColCount[std::make_pair(obj->ca,obj->address)]->text().toInt() );
            mapPtItem_ColCount[std::make_pair(obj->ca,obj->address)]->setText( buf );

            QDateTime current = QDateTime::currentDateTime();
            sprintf(buftt, "Local: %s", current.toString("yyyy/MM/dd hh:mm:ss.zzz").toStdString().c_str());

            switch (obj->type)
              {
              case iec104_class::M_SP_TB_1: // 30
                  sprintf( buftt, "Field: %02d/%02d/%02d %02d:%02d:%02d.%03d %s", obj->timetag.year, obj->timetag.month, obj->timetag.mday, obj->timetag.hour, obj->timetag.min, obj->timetag.msec/1000, obj->timetag.msec%1000, obj->timetag.iv?"iv":"ok" );
                  [[clang::fallthrough]];
              case iec104_class::M_SP_NA_1: // 1
                  sprintf( buf, "%s%s%s%s%s", obj->scs?"on ":"off ", obj->iv?"iv ":"", obj->bl?"bl ":"", obj->sb?"sb ":"", obj->nt?"nt ":"" );
                  break;

              case iec104_class::M_DP_TB_1: // 31
                  sprintf( buftt, "Field: %02d/%02d/%02d %02d:%02d:%02d.%03d %s", obj->timetag.year, obj->timetag.month, obj->timetag.mday, obj->timetag.hour, obj->timetag.min, obj->timetag.msec/1000, obj->timetag.msec%1000, obj->timetag.iv?"iv":"ok" );
                  [[clang::fallthrough]];
              case iec104_class::M_DP_NA_1: // 3
                  sprintf( buf, "%s%s%s%s%s", dblmsg[obj->dcs], obj->iv?"iv ":"", obj->bl?"bl ":"", obj->sb?"sb ":"", obj->nt?"nt ":"" );
                  break;

              case iec104_class::M_ST_TB_1: // 32
                  sprintf( buftt, "Field: %02d/%02d/%02d %02d:%02d:%02d.%03d %s", obj->timetag.year, obj->timetag.month, obj->timetag.mday, obj->timetag.hour, obj->timetag.min, obj->timetag.msec/1000, obj->timetag.msec%1000, obj->timetag.iv?"iv":"ok" );
                  [[clang::fallthrough]];
              case iec104_class::M_ST_NA_1: // 5
                  sprintf( buf, "%s%s%s%s%s%s", obj->ov?"ov ":"", obj->iv?"iv ":"", obj->bl?"bl ":"", obj->sb?"sb ":"", obj->nt?"nt ":"", obj->t?"t ":"" );
                  break;

              case iec104_class::M_IT_TB_1: // 37
                  sprintf( buftt, "Field: %02d/%02d/%02d %02d:%02d:%02d.%03d %s", obj->timetag.year, obj->timetag.month, obj->timetag.mday, obj->timetag.hour, obj->timetag.min, obj->timetag.msec/1000, obj->timetag.msec%1000, obj->timetag.iv?"iv":"ok" );
                  [[clang::fallthrough]];
              case iec104_class::M_IT_NA_1: // 15
                  sprintf( buf, "%s%s%s%s%u", obj->iv?"iv ":"", obj->cadj?"ca ":"", obj->cy?"cy ":"", "sq=", obj->sq );
                  break;

              case iec104_class::M_BO_TB_1: // 33
                  sprintf( buftt, "Field: %02d/%02d/%02d %02d:%02d:%02d.%03d %s", obj->timetag.year, obj->timetag.month, obj->timetag.mday, obj->timetag.hour, obj->timetag.min, obj->timetag.msec/1000, obj->timetag.msec%1000, obj->timetag.iv?"iv":"ok" );
                  [[clang::fallthrough]];
              case iec104_class::M_BO_NA_1: // 7
                  sprintf( buf, "%s%s%s%s%s", obj->ov?"ov ":"", obj->iv?"iv ":"", obj->bl?"bl ":"", obj->sb?"sb ":"", obj->nt?"nt ":"" );
                  break;

              case iec104_class::M_ME_TD_1: // 34
              case iec104_class::M_ME_TE_1: // 35
              case iec104_class::M_ME_TF_1: // 36
                  sprintf( buftt, "Field: %02d/%02d/%02d %02d:%02d:%02d.%03d %s", obj->timetag.year, obj->timetag.month, obj->timetag.mday, obj->timetag.hour, obj->timetag.min, obj->timetag.msec/1000, obj->timetag.msec%1000, obj->timetag.iv?"iv":"ok" );
                  [[clang::fallthrough]];
              case iec104_class::M_ME_NA_1: // 9
              case iec104_class::M_ME_NB_1: // 11
              case iec104_class::M_ME_NC_1: // 13
                  sprintf( buf, "%s%s%s%s%s", obj->ov?"ov ":"", obj->iv?"iv ":"", obj->bl?"bl ":"", obj->sb?"sb ":"", obj->nt?"nt ":"" );
                  break;
              }

            mapPtItem_ColFlags[std::make_pair(obj->ca,obj->address)]->setText( buf );
            mapPtItem_ColTimeTag[std::make_pair(obj->ca,obj->address)]->setText( buftt );
        }

    if ( inserted )
        ui->twPontos->sortItems ( 0 );
    }
}

void MainWindow::slot_timer_logmsg()
{
    static int count = 0;
    static int rowant = 0;
    static const int logBufSize = 30000;
    static int cntLogMsgs = 0; // index for circular buffer of log messages

    if ( Hide )
      if ( this->isVisible() )
         this->setVisible( false );

    // adjust size of rows and columns
    if ( ! ( ++count%15 ) )
        if ( rowant < ui->twPontos->rowCount() )
        {
        rowant = ui->twPontos->rowCount();
        ui->twPontos->resizeRowsToContents();
        ui->twPontos->resizeColumnsToContents();
        }

    // if ( !i104.mLog.haveMsg() && i104.tmKeepAlive->isActive() )
    //  i104.mLog.pushMsg( "." );

    if ( i104.mLog.haveMsg() )
    {
      //if (ui->lwLog->count() > logBufSize )
      //{
      //    ui->lwLog->clear();
      //    ui->lwLog->addItem( "*** Message list auto cleaned!" );
      //}

      while ( i104.mLog.haveMsg() )
      {
          if ( ui->lwLog->count() < logBufSize )
            { // buffer not filled: create new lines
              ui->lwLog->addItem( i104.mLog.pullMsg().c_str() );
            }
          else
            { // buffer filled: rewrite lines
              ui->lwLog->item( cntLogMsgs % logBufSize )->setText( i104.mLog.pullMsg().c_str() );
              // Marks end of circular buffer
              ui->lwLog->item( (cntLogMsgs + 1) % logBufSize )->setText( "=================================================" );
              ui->lwLog->item( (cntLogMsgs + 1) % logBufSize )->setForeground(Qt::green);
              ui->lwLog->item( (cntLogMsgs + 1) % logBufSize )->setBackground(Qt::yellow);
            }

          if ( ui->lwLog->item( cntLogMsgs % logBufSize )->text().indexOf("I104M") >= 0 )
             {
              ui->lwLog->item( cntLogMsgs % logBufSize )->setForeground(Qt::lightGray);
              ui->lwLog->item( cntLogMsgs % logBufSize )->setBackground(Qt::transparent);
             }
          else
          if ( ui->lwLog->item( cntLogMsgs % logBufSize )->text().indexOf("COMMAND") >=0 )
             {
              ui->lwLog->item( cntLogMsgs % logBufSize )->setForeground(Qt::black);
              ui->lwLog->item( cntLogMsgs % logBufSize )->setBackground(Qt::lightGray);
             }
          else
          if ( ui->lwLog->item( cntLogMsgs % logBufSize )->text().indexOf("[") >= 0 )
             {
              ui->lwLog->item( cntLogMsgs % logBufSize )->setForeground(Qt::red);
              ui->lwLog->item( cntLogMsgs % logBufSize )->setBackground(Qt::transparent);
             }
          else
             {
              ui->lwLog->item( cntLogMsgs % logBufSize )->setForeground(Qt::black);
              ui->lwLog->item( cntLogMsgs % logBufSize )->setBackground(Qt::transparent);
             }
          cntLogMsgs++;
      }

      if ( ui->cbAutoScroll->isChecked() )
        {
          ui->lwLog->scrollToItem( ui->lwLog->item( (cntLogMsgs - 1) % logBufSize ), QAbstractItemView::PositionAtBottom );
        }
    }
}

void  MainWindow::slot_interrogationActConfIndication()
{
}

void  MainWindow::slot_interrogationActTermIndication()
{
}

void MainWindow::slot_tcpconnect()
{
    ui->leIPRemoto->setText( i104.tcps->peerAddress().toString() );
    ui->lbStatus->setText( "<font color='green'> TCP CONNECTED!</font>" );
    ui->pbGI->setEnabled( true );
    ui->pbSendCommandsButton->setEnabled( true );
    ui->pbConnect->setText( "Disconnect" );
}

void MainWindow::slot_tcpdisconnect()
{
    if ( I104M_HaveDualHost() && isPrimary == true )
    {
       I104M_CntDnToBePrimary = I104M_CntToBePrimary + 1; // wait a little more time to be primary again to allow for the secondary to assume
       isPrimary = false;
       i104.disable_connect();
       I104M_Loga( " --- I104M: BECOMING SECONDARY BY DISCONNECTION" );
       ui->lbMode->setText( "<font color='red'>Secondary</font>" );
    }

    ui->lbStatus->setText( "<font color='red'> TCP DISCONNECTED!</font>" );
    ui->pbGI->setEnabled( false );
    ui->pbSendCommandsButton->setEnabled( false );

    if ( i104.tmKeepAlive->isActive() )
    {
        ui->pbConnect->setText( "Give up" );
        ui->lePort->setEnabled( false );
        ui->leIPRemoto->setEnabled( false );
        ui->leLinkAddress->setEnabled( false );
        ui->leMasterAddress->setEnabled( false );
    }
    else
    {
        ui->pbConnect->setText( "Connect" );
        ui->lePort->setEnabled( true );
        ui->leIPRemoto->setEnabled( true );
        ui->leLinkAddress->setEnabled( true );
        ui->leMasterAddress->setEnabled( true );
    }
}

void MainWindow::slot_commandActRespIndication( iec_obj *obj )
{
bool is_select = false;

    if ( obj->cause == iec104_class::REQUEST || obj->cause == iec104_class::ACTIVATION || obj->cause == iec104_class::ACTCONFIRM )
    if ( LastCommandAddress == obj->address )
    {
        i104.mLog.pushMsg("     COMMAND CONF INDICATION");
        is_select = ( obj->se == iec104_class::SELECT );

        // if confirmed select, execute
        if ( is_select && obj->pn == iec104_class::POSITIVE )
        {
            // if defined ASDU address on UI, use it
            // else will set to zero and use slave address (send Command will substitute zero to slave address)
            obj->ca = ui->leASDUAddr->text().toUShort();
            obj->se = iec104_class::EXECUTE;
            i104.sendCommand( obj );
        }

        // respond to I104M only if it's not a select or if its a negative response
        if ( is_select == false || obj->pn == iec104_class::NEGATIVE )
        {
            t_msgsup I104M_msg;
            I104M_msg.signature = MSGSUP_SIG;
            I104M_msg.tipo = obj->type;
            I104M_msg.endereco = obj->address;
            I104M_msg.sec = obj->ca;
            I104M_msg.prim = unsigned(i104.getPrimaryAddress());
            // mask cause, and p/n result to bit 6 1=NEG 0=POS
            I104M_msg.causa = unsigned(obj->cause | ( ((obj->pn==iec104_class::NEGATIVE)?1:0) << 6));

            switch ( obj->type )
            {
            case iec104_class::C_SC_NA_1:
            case iec104_class::C_SC_TA_1:
                I104M_msg.taminfo = 1;
                I104M_msg.info[0] = obj->scs;
                break;
            case iec104_class::C_DC_NA_1:
            case iec104_class::C_DC_TA_1:
                I104M_msg.taminfo = 1;
                I104M_msg.info[0] = obj->dcs;
                break;
            case iec104_class::C_RC_NA_1:
            case iec104_class::C_RC_TA_1:
                I104M_msg.taminfo = 1;
                I104M_msg.info[0] = obj->rcs;
                break;
            case iec104_class::C_SE_NA_1:
            case iec104_class::C_SE_TA_1:
                I104M_msg.taminfo = 4;
                *(reinterpret_cast<float *>(&I104M_msg.info)) = obj->value;
                break;

            case iec104_class::C_SE_NB_1:
            case iec104_class::C_SE_TB_1:
                I104M_msg.taminfo = 4;
                *(reinterpret_cast<float *>(&I104M_msg.info)) = obj->value;
                break;

            case iec104_class::C_SE_NC_1:
            case iec104_class::C_SE_TC_1:
                I104M_msg.taminfo = 4;
                *(reinterpret_cast<float *>(&I104M_msg.info)) = obj->value;
                break;

            case iec104_class::C_BO_NA_1:
            case iec104_class::C_BO_TA_1:
                I104M_msg.taminfo = 4;
                *(reinterpret_cast<uint32_t *>(&I104M_msg.info)) = uint32_t(obj->value);
                break;
            }
            if ( obj->pn == iec104_class::NEGATIVE )
            {
                I104M_Loga( "T<-- I104M: COMMAND REJECTED BY IEC104 SLAVE" );
            }
            else
            {
                I104M_Loga( "T<-- I104M: COMMAND ACCEPTED BY IEC104 SLAVE" );
            }
            udps->writeDatagram ( reinterpret_cast<char *>(&I104M_msg), sizeof(I104M_msg), I104M_host, I104M_porta );
            if ( I104M_HaveDualHost() )
                udps->writeDatagram ( reinterpret_cast<char *>(&I104M_msg), sizeof(I104M_msg), I104M_host_dual, I104M_porta );
        }
    }
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    i104.terminate();
    event->accept();
}

void MainWindow::slot_timer_I104M_kamsg()
{
    if ( ! isPrimary )
    {
       if ( I104M_CntDnToBePrimary <= 0 )
          {
          isPrimary = true;
          i104.enable_connect();
          i104.tmKeepAlive->start();
          I104M_CntDnToBePrimary = I104M_CntToBePrimary;
          I104M_Loga( " --- I104M: BECOMING PRIMARY BY TIMEOUT" );
          ui->lbMode->setText( "<font color='green'>Primary</font>" );
          }
       else
          I104M_CntDnToBePrimary--;
    }

    if ( isPrimary )
    {   // send keepalive message to the dual host
        t_msgcmd i104M_msg;
        i104M_msg.signature = MSGCMD_SIG;
        i104M_msg.tipo = I104M_ASDU_SPECIAL_CMD;
        i104M_msg.endereco = I104M_SPECIAL_CMD_ADDR_KEEP_ALIVE;
        i104M_msg.setpoint_i32 = 0;
        i104M_msg.sbo = 0;
        i104M_msg.qu = 0;
        i104M_msg.utr = 0;
        udps->writeDatagram( reinterpret_cast<char *>(&i104M_msg), sizeof(i104M_msg), I104M_host_dual, I104M_porta_escuta );
    }
}

void MainWindow::on_cbLog_clicked()
{
    if ( ui->cbLog->isChecked() )
        {
        i104.mLog.activateLog();
        QDate dt = QDate::currentDate();
        QString str = dt.toString() + QString(" - ") + QString(QTESTER_VERSION);
        i104.mLog.pushMsg(str.toStdString().c_str());
        }
    else
        i104.mLog.deactivateLog();
}

void MainWindow::on_pbCopyClipb_clicked()
{
    int itemsCount = ui->lwLog->count();
    QStringList strings;
    for (int i = 0; i < itemsCount; ++i)
       strings << ui->lwLog->item(i)->text();

    QApplication::clipboard()->setText(strings.join("\n"));
}

void MainWindow::on_pbCopyVals_clicked()
{
    QString text = "Address\tCA\tValue\tASDU\tCause\tFlags\tCount\tTimeTag\n";

    for (int i=0; i<ui->twPontos->rowCount();  i++)
    {
        for (int j=0; j<7; j++)
          {
            text = text + ui->twPontos->item(i, j)->text() + "\t" ;
          }
        text = text + "\n";
    }

    QApplication::clipboard()->setText(text);
}

void MainWindow::I104M_processPoints( iec_obj *obj, unsigned numpoints )
{
    static t_msgsupsq msg;

    switch ( obj->type )
      {
      case iec104_class::M_DP_TB_1: // double state with time tag
        {
        msg.signature = MSGSUPSQ_SIG;
        msg.tipo = obj->type;
        msg.prim = static_cast<uint32_t>(i104.getPrimaryAddress());
        msg.sec = obj->ca;
        msg.causa = obj->cause;
        msg.taminfo = sizeof(digital_w_time7_seq);
        msg.numpoints = static_cast<uint32_t>(numpoints);

        for ( unsigned count=0; count < numpoints; count++, obj++ )
          {
          uint32_t * paddr = reinterpret_cast<uint32_t *>(msg.info + count * (sizeof(int32_t) + sizeof(digital_w_time7_seq)));
          *paddr = obj->address;

          // value and qualifier
          digital_w_time7_seq * i104mobj = reinterpret_cast<digital_w_time7_seq *>(paddr + 1);
          i104mobj->iq = static_cast<unsigned char>(obj->dp | (obj->bl << 4) | (obj->sb << 5) |(obj->nt << 6) | (obj->iv << 7));
          i104mobj->ano = obj->timetag.year;
          i104mobj->mes = obj->timetag.month;
          i104mobj->dia = obj->timetag.mday;
          i104mobj->hora = obj->timetag.hour;
          i104mobj->min = obj->timetag.min;
          i104mobj->ms = obj->timetag.msec;
          }

        SendOSHMI( reinterpret_cast<char *>(&msg), sizeof(int32_t) * 7 + msg.numpoints * (sizeof(int32_t) + sizeof(digital_w_time7_seq)) );
        }
        break;
      case iec104_class::M_SP_TB_1: // single state with time tag
        {
        msg.signature = MSGSUPSQ_SIG;
        msg.tipo = obj->type;
        msg.prim = static_cast<uint32_t>(i104.getPrimaryAddress());
        msg.sec = obj->ca;
        msg.causa = obj->cause;
        msg.taminfo = sizeof(digital_w_time7_seq);
        msg.numpoints = static_cast<uint32_t>(numpoints);

        for ( unsigned count=0; count < numpoints; count++, obj++ )
          {
          uint32_t * paddr = reinterpret_cast<uint32_t *>(msg.info + count * (sizeof(int32_t) + sizeof(digital_w_time7_seq)));
          *paddr = obj->address;

          // value and qualifier
          digital_w_time7_seq * i104mobj = reinterpret_cast<digital_w_time7_seq *>(paddr + 1);
          i104mobj->iq = static_cast<unsigned char>(obj->sp | (obj->bl << 4) | (obj->sb << 5) |(obj->nt << 6) | (obj->iv << 7));
          i104mobj->ano = obj->timetag.year;
          i104mobj->mes = obj->timetag.month;
          i104mobj->dia = obj->timetag.mday;
          i104mobj->hora = obj->timetag.hour;
          i104mobj->min = obj->timetag.min;
          i104mobj->ms = obj->timetag.msec;
          }

        SendOSHMI( reinterpret_cast<char *>(&msg), sizeof(int32_t) * 7 + msg.numpoints * (sizeof(int32_t) + sizeof(digital_w_time7_seq)) );
        }
        break;
      case iec104_class::M_DP_NA_1: // double state without time tag
        {
        msg.signature = MSGSUPSQ_SIG;
        msg.tipo = obj->type;
        msg.prim = static_cast<uint32_t>(i104.getPrimaryAddress());
        msg.sec = obj->ca;
        msg.causa = obj->cause;
        msg.taminfo = sizeof(digital_notime_seq);
        msg.numpoints = static_cast<uint32_t>(numpoints);

        for ( unsigned count=0; count < numpoints; count++, obj++ )
          {
          uint32_t * paddr = reinterpret_cast<uint32_t *>(msg.info + count * (sizeof(int32_t) + sizeof(digital_notime_seq)));
          *paddr = obj->address;

          // value and qualifier
          digital_notime_seq * i104mobj = reinterpret_cast<digital_notime_seq *>(paddr + 1);
          i104mobj->iq = static_cast<unsigned char>(obj->dp | (obj->bl << 4) | (obj->sb << 5) |(obj->nt << 6) | (obj->iv << 7));
          }

        SendOSHMI( reinterpret_cast<char *>(&msg), sizeof(int32_t) * 7 + msg.numpoints * (sizeof(int32_t) + sizeof(digital_notime_seq)) );
        }
        break;
      case iec104_class::M_SP_NA_1: // single state without time tag
        {
        msg.signature = MSGSUPSQ_SIG;
        msg.tipo = obj->type;
        msg.prim = static_cast<uint32_t>(i104.getPrimaryAddress());
        msg.sec = obj->ca;
        msg.causa = obj->cause;
        msg.taminfo = sizeof(digital_notime_seq);
        msg.numpoints = static_cast<uint32_t>(numpoints);

        for ( unsigned count=0; count < numpoints; count++, obj++ )
          {
          uint32_t * paddr = reinterpret_cast<uint32_t *>(msg.info + count * (sizeof(int) + sizeof(digital_notime_seq)));
          *paddr = obj->address;

          // value and qualifier
          digital_notime_seq * i104mobj = reinterpret_cast<digital_notime_seq *>(paddr + 1);
          i104mobj->iq = static_cast<unsigned char>(obj->sp | (obj->bl << 4) | (obj->sb << 5) |(obj->nt << 6) | (obj->iv << 7));
          }

        SendOSHMI( reinterpret_cast<char *>(&msg), sizeof(int32_t) * 7 + msg.numpoints * (sizeof(int32_t) + sizeof(digital_notime_seq)) );
        }
        break;

      case iec104_class::M_ST_TB_1: // 32 = step with time tag (will ignore time)
      case iec104_class::M_ST_NA_1: // 5 = step without time tag
        {
        msg.signature = MSGSUPSQ_SIG;
        msg.tipo = iec104_class::M_ST_NA_1;
        msg.prim = static_cast<uint32_t>(i104.getPrimaryAddress());
        msg.sec = obj->ca;
        msg.causa = obj->cause;
        msg.taminfo = sizeof(step_seq);
        msg.numpoints = static_cast<uint32_t>(numpoints);

        for ( unsigned count=0; count < numpoints; count++, obj++ )
          {
          uint32_t * paddr = reinterpret_cast<uint32_t *>(msg.info + count * (sizeof(int32_t) + sizeof(step_seq)));
          *paddr = obj->address;

          // value and qualifier
          step_seq * i104mobj = reinterpret_cast<step_seq *>(paddr + 1);
          i104mobj->qds = static_cast<unsigned char>(obj->ov | (obj->bl << 4) | (obj->sb << 5) |(obj->nt << 6) | (obj->iv << 7));
          i104mobj->vti = static_cast<unsigned char>(obj->value) | static_cast<unsigned char>(obj->t << 7);
          }

        SendOSHMI( reinterpret_cast<char *>(&msg), sizeof(int32_t) * 7 + msg.numpoints * (sizeof(int32_t) + sizeof(step_seq)) );
        }
        break;

      case iec104_class::M_ME_TD_1: // 34 = normalized with time tag
      case iec104_class::M_ME_NA_1: // 9 = normalized without time tag
        {
        msg.signature = MSGSUPSQ_SIG;
        msg.tipo = iec104_class::M_ME_NA_1;
        msg.prim = static_cast<uint32_t>(i104.getPrimaryAddress());
        msg.sec = obj->ca;
        msg.causa = obj->cause;
        msg.taminfo = sizeof(analogico_seq);
        msg.numpoints = static_cast<uint32_t>(numpoints);

        for ( unsigned count=0; count < numpoints; count++, obj++ )
          {
          uint32_t * paddr = reinterpret_cast<uint32_t *>(msg.info + count * (sizeof(int32_t) + sizeof(analogico_seq)));
          *paddr = obj->address;

          // value and qualifier
          analogico_seq * i104mobj = reinterpret_cast<analogico_seq *>(paddr + 1);
          i104mobj->qds = static_cast<unsigned char>(obj->ov | (obj->bl << 4) | (obj->sb << 5) |(obj->nt << 6) | (obj->iv << 7));
          i104mobj->sva = static_cast<short>(obj->value);
          }

        SendOSHMI( reinterpret_cast<char *>(&msg), sizeof(int32_t) * 7 + msg.numpoints * (sizeof(int32_t) + sizeof(analogico_seq)) );
        }
        break;

      case iec104_class::M_ME_TE_1: // 35 = scaled with time tag
      case iec104_class::M_ME_NB_1: // 11 = scaled without time tag
        {
        msg.signature = MSGSUPSQ_SIG;
        msg.tipo = iec104_class::M_ME_NB_1;
        msg.prim = static_cast<uint32_t>(i104.getPrimaryAddress());
        msg.sec = obj->ca;
        msg.causa = obj->cause;
        msg.taminfo = sizeof(analogico_seq);
        msg.numpoints = static_cast<uint32_t>(numpoints);

        for ( unsigned count=0; count < numpoints; count++, obj++ )
          {
          uint32_t * paddr = reinterpret_cast<uint32_t *>(msg.info + count * (sizeof(int32_t) + sizeof(analogico_seq)));
          *paddr = obj->address;

          // value and qualifier
          analogico_seq * i104mobj = reinterpret_cast<analogico_seq *>(paddr + 1);
          i104mobj->qds = static_cast<unsigned char>(obj->ov | (obj->bl << 4) | (obj->sb << 5) |(obj->nt << 6) | (obj->iv << 7));
          i104mobj->sva = static_cast<short>(obj->value);
          }

        SendOSHMI( reinterpret_cast<char *>(&msg), sizeof(int32_t) * 7 + msg.numpoints * (sizeof(int32_t) + sizeof(analogico_seq)) );
        }
        break;

      case iec104_class::M_ME_TF_1: // 36 = float with time tag
      case iec104_class::M_ME_NC_1: // 13 = float without time tag
        {
        msg.signature = MSGSUPSQ_SIG;
        msg.tipo = iec104_class::M_ME_NC_1;
        msg.prim = static_cast<uint32_t>(i104.getPrimaryAddress());
        msg.sec = obj->ca;
        msg.causa = obj->cause;
        msg.taminfo = sizeof(flutuante_seq);
        msg.numpoints = static_cast<uint32_t>(numpoints);

        for ( unsigned count=0; count < numpoints; count++, obj++ )
          {
          uint32_t * paddr = reinterpret_cast<uint32_t *>(msg.info + count * (sizeof(int32_t) + sizeof(flutuante_seq)));
          *paddr = obj->address;

          // value and qualifier
          flutuante_seq * i104mobj = reinterpret_cast<flutuante_seq *>(paddr + 1);
          i104mobj->qds = static_cast<unsigned char>(obj->ov | (obj->bl << 4) | (obj->sb << 5) |(obj->nt << 6) | (obj->iv << 7));
          i104mobj->fr = obj->value;
          }

        SendOSHMI( reinterpret_cast<char *>(&msg), sizeof(int32_t) * 7 + msg.numpoints * (sizeof(int32_t) + sizeof(flutuante_seq)) );
        }
        break;

    case iec104_class::M_IT_TB_1: // 37 = integrated totals with time tag
    case iec104_class::M_IT_NA_1: // 15 = integrated totals without time tag
      {
      msg.signature = MSGSUPSQ_SIG;
      msg.tipo = iec104_class::M_IT_NA_1;
      msg.prim = static_cast<uint32_t>(i104.getPrimaryAddress());
      msg.sec = obj->ca;
      msg.causa = obj->cause;
      msg.taminfo = sizeof(integrated_seq);
      msg.numpoints = static_cast<uint32_t>(numpoints);

      for ( unsigned count=0; count < numpoints; count++, obj++ )
        {
        uint32_t * paddr = reinterpret_cast<uint32_t *>(msg.info + count * (sizeof(int32_t) + sizeof(integrated_seq)));
        *paddr = obj->address;

        // value and qualifier
        integrated_seq * i104mobj = reinterpret_cast<integrated_seq *>(paddr + 1);
        // map carry to overflow
        i104mobj->qds = static_cast<unsigned char>((obj->cy << 7) | (obj->cadj << 6) | (obj->iv << 7));
        i104mobj->bcr = obj->bcr;
        }

      SendOSHMI( reinterpret_cast<char *>(&msg), sizeof(int32_t) * 7 + msg.numpoints * (sizeof(int32_t) + sizeof(integrated_seq)) );
      }
      break;
    case iec104_class::M_BO_TB_1: // 33 = bitstring of 32 bits with time tag (will send as counter)
    case iec104_class::M_BO_NA_1: // 7 = bitstring of 32 bits (will send as counter)
      {
      // note: bitstring could also possibly be interpreted as 32 single binary states in sequence
      // I opt to use counter to avoid possible address conflict for bitstrings in sequence of addresses
      msg.signature = MSGSUPSQ_SIG;
      msg.tipo = iec104_class::M_IT_NA_1;
      msg.prim = static_cast<uint32_t>(i104.getPrimaryAddress());
      msg.sec = obj->ca;
      msg.causa = obj->cause;
      msg.taminfo = sizeof(integrated_seq);
      msg.numpoints = static_cast<uint32_t>(numpoints);

      for ( unsigned count=0; count < numpoints; count++, obj++ )
        {
        uint32_t * paddr = reinterpret_cast<uint32_t *>(msg.info + count * (sizeof(int32_t) + sizeof(integrated_seq)));
        *paddr = obj->address;

        // value as counter and no qualifier
        integrated_seq * i104mobj = reinterpret_cast<integrated_seq *>(paddr + 1);
        i104mobj->qds = 0;
        i104mobj->bcr = obj->bsi;
        }

      SendOSHMI( reinterpret_cast<char *>(&msg), sizeof(int32_t) * 7 + msg.numpoints * (sizeof(int32_t) + sizeof(integrated_seq)) );
      }
      break;
    default:
      i104.mLog.pushMsg( "R--> IEC104 UNSUPPORTED TYPE, NOT FORWARDED TO I104M/OSHMI" );
      break;
    }
}

void MainWindow::SendOSHMI(char * msg, uint32_t packet_size)
{
    udps->writeDatagram ( reinterpret_cast<const char *>(msg), packet_size, I104M_host, I104M_porta );
    if ( I104M_HaveDualHost() )
      udps->writeDatagram ( reinterpret_cast<const char *>(msg), packet_size, I104M_host_dual, I104M_porta );
}
