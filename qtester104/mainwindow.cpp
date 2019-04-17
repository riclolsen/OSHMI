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

#define QTESTER_VERSION "v1.26"
#define QTESTER_COPYRIGHT "Copyright © 2010-2019 Ricardo Lastra Olsen"
#define CURDIRINIFILENAME "/qtester104.ini"
#define CONFDIRINIFILENAME "../conf/qtester104.ini"

//-------------------------------------------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    BDTR_Logar = 1;
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
    i104.BDTRForcePrimary = settings.value( "BDTR/FORCE_PRIMARY", 0 ).toInt();
    i104.setSecondaryAddress( settings.value( "RTU1/SECONDARY_ADDRESS", 1 ).toInt() );
    i104.SendCommands = settings.value( "RTU1/ALLOW_COMMANDS", 0 ).toInt();

    QString IPEscravo;
    IPEscravo = settings.value( "RTU1/IP_ADDRESS_BACKUP", "" ).toString();
    i104.setSecondaryIP_backup( (char *)IPEscravo.toStdString().c_str() );
    IPEscravo = settings.value( "RTU1/IP_ADDRESS", "" ).toString();
    i104.setSecondaryIP( (char *)IPEscravo.toStdString().c_str() );
    i104.setPortTCP( settings.value( "RTU1/TCP_PORT", i104.getPortTCP() ).toInt() );
    i104.setGIPeriod( settings.value( "RTU1/GI_PERIOD", 330 ).toInt() );

    // this is for using with the OSHMI HMI in a dual architecture
    QSettings settings_bdtr( "../conf/hmi.ini", QSettings::IniFormat );
    BDTR_host_dual = settings_bdtr.value( "REDUNDANCY/OTHER_HMI_IP", "0.0.0.0" ).toString();
    BDTR_host = "127.0.0.1";
    BDTR_CntDnToBePrimary = BDTR_CntToBePrimary;

    ui->setupUi( this );

    // this is for hiding the window when runnig
    Hide = settings_bdtr.value( "RUN/HIDE", "" ).toInt();

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
    udps->bind( BDTR_porta_escuta );
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
    tmBDTR_kamsg = new QTimer();

    connect( udps, SIGNAL(readyRead()), this, SLOT(slot_BDTR_pronto_para_ler()) );
    connect( tmLogMsg, SIGNAL(timeout()), this, SLOT(slot_timer_logmsg()) );
    connect( tmBDTR_kamsg, SIGNAL(timeout()), this, SLOT(slot_timer_BDTR_kamsg()) );
    connect( &i104, SIGNAL(signal_dataIndication(iec_obj *, int)), this, SLOT(slot_dataIndication(iec_obj *, int)) );
    connect( &i104, SIGNAL(signal_interrogationActConfIndication()), this, SLOT(slot_interrogationActConfIndication()) );
    connect( &i104, SIGNAL(signal_interrogationActTermIndication()), this, SLOT(slot_interrogationActTermIndication()) );
    connect( &i104, SIGNAL(signal_tcp_connect()), this, SLOT(slot_tcpconnect()) );
    connect( &i104, SIGNAL(signal_tcp_disconnect()), this, SLOT(slot_tcpdisconnect()) );
    connect( &i104, SIGNAL(signal_commandActConfIndication(iec_obj *)), this, SLOT(slot_commandActConfIndication(iec_obj *)) );
    connect( &i104, SIGNAL(signal_commandActTermIndication(iec_obj *)), this, SLOT(slot_commandActTermIndication(iec_obj *)) );

    ui->pbGI->setEnabled(false);
    ui->pbSendCommandsButton->setEnabled( false );

    ui->twPontos->clear();
    ui->twPontos->setSortingEnabled ( false );
    ui->twPontos->setColumnCount( 8 );
    ui->twPontos->sortByColumn( 0 );

    if ( IPEscravo != "" )
      on_pbConnect_clicked();

    QStringList colunas;
    colunas << "Address" << "CA" << "Value" << "ASDU" << "Cause" << "Flags" << "Count" << "TimeTag";
    ui->twPontos->setHorizontalHeaderLabels( colunas );

    tmLogMsg->start(500);

    if ( BDTR_HaveDualHost() )
    {
        tmBDTR_kamsg->start( BDTR_seconds_kamsg * 1000 );
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

    ui->lbCopyright->setText( (QString)QTESTER_VERSION + (QString)" - " + (QString)QTESTER_COPYRIGHT );

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
    delete tmBDTR_kamsg;
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
        i104.setSecondaryIP( (char*)ui->leIPRemoto->text().toStdString().c_str() );
        i104.setPortTCP( ui->lePort->text().toInt() );
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

// recebimento de informacoes pelo BDTR
void MainWindow::slot_BDTR_pronto_para_ler()
{
    char buf[5000];
    char bufOut[1600];  // buffer para mensagem bdtr

    unsigned char br[2000]; // buffer de recepcao
    int bytesrec;

    while ( udps->hasPendingDatagrams() )
    {
    QHostAddress address;
    quint16 port;
    bytesrec = udps->readDatagram ( (char *)br, sizeof(br), &address, &port );

    if ( bytesrec <= 0)
       return;

    sprintf( buf, "%3d: BDTR: ", bytesrec );
    for ( int i=0; i< bytesrec; i++ )
        sprintf (buf+strlen(buf), "%02x ", br[i]);
    BDTR_Loga( buf );

    int Tipo = br[0] & T_MASC;

    switch ( Tipo )
    {
    case T_REQ:
        {
            msg_req * msg;
            msg = (msg_req*)br;

            if ( msg->TIPO == REQ_GRUPO && msg->ID == 0 ) // GI
            {
                BDTR_Loga( "R--> BDTR: REQ GI" );
                i104.solicitGI();
            }
            if ( msg->TIPO == REQ_GRUPO && msg->ID == 255 ) // request group 255: show form
            {
                Hide = ! Hide;
                if ( ! Hide )
                  this->setVisible( true );
            }
            else
            {
                if ( msg->TIPO == REQ_HORA )
                  BDTR_Loga( "R--> BDTR: IGNORED (TIME REQ)" );
                else
                  BDTR_Loga( "R--> BDTR: IGNORED (REQ ?)" );
            }
        }
        break;
    case T_HORA:
        // if BDTRForcePrimary==1 don't become secondary when received keep alive messages from other machine
        // if BDTRForcePrimary==0 become secondary when received keep alive messages from other machine
        if ( address == BDTR_host_dual && i104.BDTRForcePrimary == 0 )
          {
          BDTR_Loga( "R--> BDTR: KEEPALIVE RECEIVED FROM DUAL MACHINE(TIME)");
          if ( isPrimary )
            {
              BDTR_Loga( " BECOMING SECONDARY" );
              ui->lbMode->setText( "<font color='red'>Secondary</font>" );
            }
          isPrimary = false;
          i104.disable_connect();
          BDTR_CntDnToBePrimary = BDTR_CntToBePrimary; // restart count to be primary
          }
        else
          BDTR_Loga( "R--> BDTR: IGNORED (TIME)" );
        break;
    case T_COM: // COMANDO
        {
            msg_com * msg;
            msg = (msg_com*)br;
            bool enviar = false;
            stringstream oss;

            oss.str("");
            oss << "--> BDTR COM : TVAL="
                << (unsigned)msg->TVAL
                << " STATUS="
                << (unsigned)msg->PONTO.STATUS
                << " ID="
                << (unsigned) msg->PONTO.ID;
            BDTR_Loga((char*)oss.str().c_str());

            if ( msg->TVAL == T_DIG ) // DIGITAL
            {
                BDTR_Loga("    BDTR DIGITAL");
                msg_ack *ms;
                ms = (msg_ack*)bufOut;
                // status bits 11 and 00 are used for command blocking
                // forward only commands for status = 10 (ON) or = 01 (OFF)
                if ( (msg->PONTO.STATUS & ESTADO) != 3 && (msg->PONTO.STATUS & ESTADO) != 0 )
                {
                    iec_obj obj;
                    obj.cause = iec104_class::ACTIVATION;
                    obj.address = msg->PONTO.ID;
                    obj.ca = msg->PONTO.VALOR.COM_SEMBANCO.UTR;
                    obj.qu = msg->PONTO.VALOR.COM_SEMBANCO.COMIEC.qu;
                    obj.se = msg->PONTO.VALOR.COM_SEMBANCO.COMIEC.se;

                    switch ( msg->PONTO.VALOR.COM_SEMBANCO.ASDU )
                      {
                      case 0: // if ASDU not defined, use single command
                        msg->PONTO.VALOR.COM_SEMBANCO.ASDU = iec104_class::C_SC_NA_1;
                      case iec104_class::C_SC_NA_1:
                      case iec104_class::C_SC_TA_1: // single
                        obj.type = msg->PONTO.VALOR.COM_SEMBANCO.ASDU;
                        obj.scs = !(msg->PONTO.VALOR.COM_SEMBANCO.COMIEC.dcs & 0x01);
                        enviar=true;
                        break;

                      case iec104_class::C_DC_NA_1:
                      case iec104_class::C_DC_TA_1: // double
                        obj.type = msg->PONTO.VALOR.COM_SEMBANCO.ASDU;
                        obj.dcs = msg->PONTO.VALOR.COM_SEMBANCO.COMIEC.dcs;
                        enviar=true;
                        break;

                      case iec104_class::C_RC_NA_1:
                      case iec104_class::C_RC_TA_1: // reg. step
                        obj.type = msg->PONTO.VALOR.COM_SEMBANCO.ASDU;
                        obj.rcs = msg->PONTO.VALOR.COM_SEMBANCO.COMIEC.dcs;
                        enviar=true;
                        break;

                      default:
                        enviar=false;
                        break;
                      }

                 if (enviar)
                    {
                    // forward command to IEC104
                    i104.sendCommand( &obj );
                    LastCommandAddress = obj.address;
                    // Vai enviar ack pelo BDTR ao receber o activation em nivel de 104
                    }
                else
                    { // REJECT COMMAND (ASDU not supported)
                    ms->COD = T_ACK;
                    ms->TIPO = T_COM;
                    ms->ORIG = BDTR_orig;
                    ms->ID = 0x80 | msg->PONTO.VALOR.COM_SEMBANCO.COMIEC.dcs;
                    ms->COMP = msg->PONTO.ID;
                    udps->writeDatagram ( (const char *) bufOut, sizeof( msg_ack ), BDTR_host, BDTR_porta );
                    if ( BDTR_HaveDualHost() )
                      udps->writeDatagram ( (const char *) bufOut, sizeof( msg_ack ), BDTR_host_dual, BDTR_porta );

                    BDTR_Loga( "T<-- BDTR: COMMAND REJECTED, UNSUPPORTED ASDU" );
                    }
                }
            }
            else
            if ( msg->TVAL == T_FLT || msg->TVAL == T_NORM || msg->TVAL == T_ANA ) // ANALOGICOS
            {
                BDTR_Loga("    BDTR ANALOG");
                msg_ack *ms;
                ms = (msg_ack*)bufOut;
                iec_obj obj;
                obj.cause = iec104_class::ACTIVATION;
                obj.address = msg->PONTO.ID;
                obj.ca = msg->PONTO.VALOR.COM_SEMBANCOANA.UTR;
                obj.qu = 0;
                obj.se = msg->PONTO.VALOR.COM_SEMBANCOANA.COMIEC.se;

                switch ( msg->PONTO.VALOR.COM_SEMBANCOANA.ASDU )
                  {
                  case iec104_class::C_SE_NA_1: // analogico normalizado
                  case iec104_class::C_SE_TA_1: // analogico normalizado com time tag
                    obj.value = msg->PONTO.VALOR.COM_SEMBANCOANA.NRM;
                    obj.type = msg->PONTO.VALOR.COM_SEMBANCOANA.ASDU;
                    enviar=true;
                    break;

                  case iec104_class::C_SE_NB_1: // analogico escalado
                  case iec104_class::C_SE_TB_1: // analogico escalado com time tag
                    obj.value = msg->PONTO.VALOR.COM_SEMBANCOANA.ANA;
                    obj.type = msg->PONTO.VALOR.COM_SEMBANCOANA.ASDU;
                    enviar=true;
                    break;

                  case 0: // 0: use floating point, without time
                    msg->PONTO.VALOR.COM_SEMBANCO.ASDU = iec104_class::C_SE_NC_1;
                  case iec104_class::C_SE_NC_1: // analogico float
                  case iec104_class::C_SE_TC_1: // analogico float com time tag
                    obj.value = msg->PONTO.VALOR.COM_SEMBANCOANA.FLT;
                    obj.type = msg->PONTO.VALOR.COM_SEMBANCOANA.ASDU;
                    enviar=true;
                    break;

                  default:
                    enviar=false;
                    break;
                  }

             if (enviar)
                {
                // forward command to IEC104
                i104.sendCommand( &obj );
                LastCommandAddress = obj.address;
                // Vai enviar ack pelo BDTR ao receber o activation em nivel de 104
                }
            else
                { // REJECT COMMAND (ASDU not supported)
                ms->COD = T_ACK;
                ms->TIPO = T_COM;
                ms->ORIG = BDTR_orig;
                ms->ID = 0x80 | msg->PONTO.VALOR.COM_SEMBANCOANA.COMIEC.dcs;
                ms->COMP = msg->PONTO.ID;
                udps->writeDatagram ( (const char *) bufOut, sizeof( msg_ack ), BDTR_host, BDTR_porta );
                if ( BDTR_HaveDualHost() )
                  udps->writeDatagram ( (const char *) bufOut, sizeof( msg_ack ), BDTR_host_dual, BDTR_porta );

                BDTR_Loga( "T<-- BDTR: COMMAND REJECTED, UNSUPPORTED ASDU" );
                }
            }
        }
        break;
    case T_DIG:
        BDTR_Loga( "R--> BDTR: IGNORED MSG (DIGITAL)" );
        break;
    default:
        BDTR_Loga( "R--> BDTR: IGNORED MSG" );
        break;
    }
    }
}

void MainWindow::BDTR_processPoints( iec_obj *obj, int numpoints )
{

    TFA_Qual qfa;
    int tam_msg;

    // Atencao, vou deixar o bit T_CONV do c�digo da mensagem para sinalizar varredor sem banco

    switch ( obj->type )
      {
      case iec104_class::M_DP_TB_1: // duplo com tag
      case iec104_class::M_SP_TB_1: // simples com tag
        {
        msg_dig_tag *msgdigtag;

        tam_msg = sizeof( A_dig_tag ) * numpoints + sizeof( msg_dig_tag ) - sizeof( A_dig_tag );
        msgdigtag = (msg_dig_tag*)malloc( tam_msg );
        msgdigtag->COD = T_DIG_TAG;
        if ( obj->cause == iec104_class::CYCLIC )
          msgdigtag->COD |= T_CIC;
        if ( obj->cause == iec104_class::SPONTANEOUS )
          msgdigtag->COD |= T_SPONT;
        msgdigtag->NRPT = numpoints;
        msgdigtag->ORIG = BDTR_orig;

        for ( int cntpnt=0; cntpnt < numpoints; cntpnt++, obj++ )
          {
          // converte o qualificador do IEC para formato A do PABD
          qfa.Byte = 0;
          qfa.Subst = obj->bl || obj->sb;
          qfa.Tipo = TFA_TIPODIG;
          qfa.Falha = obj->iv || obj->nt;
          qfa.FalhaTag = obj->timetag.iv;

          if ( obj->type == iec104_class::M_DP_TB_1 || obj->type == iec104_class::M_DP_NA_1 )
            {
            qfa.Duplo = obj->dp;
            }
          else
            { // simples para duplo
            qfa.Estado = !obj->sp;
            qfa.EstadoH = obj->sp;
            }

          msgdigtag->PONTO[cntpnt].ID = obj->address;
          msgdigtag->PONTO[cntpnt].UTR = obj->ca;
          msgdigtag->PONTO[cntpnt].STAT = qfa.Byte;
          msgdigtag->PONTO[cntpnt].TAG.ANO = 2000+obj->timetag.year;
          msgdigtag->PONTO[cntpnt].TAG.MES = obj->timetag.month;
          msgdigtag->PONTO[cntpnt].TAG.DIA = obj->timetag.mday;
          msgdigtag->PONTO[cntpnt].TAG.HORA = obj->timetag.hour;
          msgdigtag->PONTO[cntpnt].TAG.MINUTO = obj->timetag.min;
          msgdigtag->PONTO[cntpnt].TAG.MSEGS = obj->timetag.msec;
          }

        udps->writeDatagram ( (const char *) msgdigtag, tam_msg, BDTR_host, BDTR_porta );
        if ( BDTR_HaveDualHost() )
          udps->writeDatagram ( (const char *) msgdigtag, tam_msg, BDTR_host_dual, BDTR_porta );

        free( msgdigtag );
        }
        break;

      case iec104_class::M_DP_NA_1: // duplo sem tag
      case iec104_class::M_SP_NA_1: // simples sem tag
        {
        msg_dig *msgdig;
        tam_msg = sizeof(A_dig) * numpoints + sizeof( msg_dig ) - sizeof( A_dig );
        msgdig = (msg_dig*)malloc( tam_msg );
        msgdig->COD = T_DIG;
        if ( obj->cause == iec104_class::CYCLIC )
          msgdig->COD |= T_CIC;
        if ( obj->cause == iec104_class::SPONTANEOUS )
          msgdig->COD |= T_SPONT;
        msgdig->NRPT = numpoints;
        msgdig->ORIG = BDTR_orig;

        for (int cntpnt=0; cntpnt < numpoints; cntpnt++, obj++)
          {
          // converte o qualificador do IEC para formato A do PABD
          qfa.Byte = 0;
          qfa.Subst = obj->bl || obj->sb;
          qfa.Tipo = TFA_TIPODIG;
          qfa.Falha = obj->iv || obj->nt;
          if ( obj->type == iec104_class::M_DP_TB_1 || obj->type == iec104_class::M_DP_NA_1 )
            {
            qfa.Duplo = obj->dp;
            }
          else
            { // simples para duplo
            qfa.Estado = !obj->sp;
            qfa.EstadoH = obj->sp;
            }

          msgdig->PONTO[cntpnt].ID = obj->address;
          msgdig->PONTO[cntpnt].STAT = qfa.Byte;
          }

        udps->writeDatagram ( (const char *) msgdig, tam_msg, BDTR_host, BDTR_porta );
        if ( BDTR_HaveDualHost() )
            udps->writeDatagram ( (const char *) msgdig, tam_msg, BDTR_host_dual, BDTR_porta );
        free(msgdig);
        }
        break;

      case iec104_class::M_ST_NA_1: // 5 = step
      case iec104_class::M_ME_NA_1: // 9 = normalized
      case iec104_class::M_ME_NB_1: // 11 = scaled
      case iec104_class::M_ST_TB_1: // 32 = step with time tag
      case iec104_class::M_ME_TD_1: // 34 = normalized with time tag
      case iec104_class::M_ME_TE_1: // 35 = scaled with time tag
        {
        msg_ana *msgana;

        tam_msg = sizeof( A_ana ) * numpoints + sizeof( msg_ana ) - sizeof( A_ana );
        msgana = (msg_ana*) malloc( tam_msg );
        if ( obj->type == iec104_class::M_ME_NA_1 || obj->type == iec104_class::M_ME_TD_1 )
          msgana->COD = T_NORM;
        else
          msgana->COD = T_ANA;
        if ( obj->cause == iec104_class::CYCLIC )
          msgana->COD |= T_CIC;
        if ( obj->cause == iec104_class::SPONTANEOUS )
          msgana->COD |= T_SPONT;
        msgana->NRPT = numpoints;
        msgana->ORIG = BDTR_orig;

        for ( int cntpnt=0; cntpnt < numpoints; cntpnt++, obj++ )
          {
          // converte o qualificador do IEC para formato A do PABD
          qfa.Byte = 0;
          qfa.Subst = obj->bl || obj->sb;
          qfa.Tipo = TFA_TIPOANA;
          qfa.Falha = obj->iv || obj->nt || obj->ov;
          if ( obj->type == iec104_class::M_ST_NA_1 || obj->type == iec104_class::M_ST_TB_1 ) // tap
             qfa.Falha =  qfa.Falha || obj->t; // transient = falha

          msgana->PONTO[cntpnt].ID = obj->address;
          msgana->PONTO[cntpnt].STAT = qfa.Byte;
          msgana->PONTO[cntpnt].VALOR = obj->value;
          }

        udps->writeDatagram ( (const char *) msgana, tam_msg, BDTR_host, BDTR_porta );
        if ( BDTR_HaveDualHost() )
           udps->writeDatagram ( (const char *) msgana, tam_msg, BDTR_host_dual, BDTR_porta );
        free(msgana);
        }
        break;

      case iec104_class::M_ME_NC_1: // 13
      case iec104_class::M_ME_TF_1: // 36
        {
        msg_float *msgflt;

        tam_msg = sizeof(A_float) * numpoints + sizeof(msg_float) - sizeof(A_float);
        msgflt = (msg_float*)malloc( tam_msg );
        msgflt->COD = T_FLT;
        if ( obj->cause == iec104_class::CYCLIC )
          msgflt->COD |= T_CIC;
        if ( obj->cause == iec104_class::SPONTANEOUS )
          msgflt->COD |= T_SPONT;

        msgflt->NRPT = numpoints;
        msgflt->ORIG = BDTR_orig;

        for ( int cntpnt=0; cntpnt < numpoints; cntpnt++, obj++ )
          {
          // qualifier converte o qualificador do IEC para formato A do PABD/BDTR
          qfa.Byte = 0;
          qfa.Subst = obj->bl || obj->sb;
          qfa.Tipo = TFA_TIPOANA;
          qfa.Falha = obj->iv || obj->nt || obj->ov;

          msgflt->PONTO[cntpnt].ID = obj->address;
          msgflt->PONTO[cntpnt].STAT = qfa.Byte;
          msgflt->PONTO[cntpnt].VALOR = obj->value;
          }

        udps->writeDatagram ( (const char *) msgflt, tam_msg, BDTR_host, BDTR_porta );
        if ( BDTR_HaveDualHost() )
            udps->writeDatagram ( (const char *) msgflt, tam_msg, BDTR_host_dual, BDTR_porta );
        free( msgflt );
        }
        break;

      default:
         i104.mLog.pushMsg( "R--> IEC104 UNSUPPORTED TYPE, NOT FORWARDED TO BDTR" );
         break;
      }
}

// Envio de comando
void MainWindow::on_pbSendCommandsButton_clicked()
{
    iec_obj obj;
    obj.type = ui->cbCmdAsdu->currentText().left(ui->cbCmdAsdu->currentText().indexOf(':')).toInt();

    // test command parameters if not sync command (that don't have parameters)
    if (obj.type != iec104_class::C_CS_NA_1)
    {
        if ( ui->leCmdValue->text().trimmed() == "" || ui->leCmdAddress->text().trimmed() == "" )
            return;
        if ( ui->leCmdAddress->text().toInt() == 0 )
            return;

        obj.address = ui->leCmdAddress->text().toInt();
        obj.value = ui->leCmdValue->text().toInt();
    }

    obj.ca = ui->leASDUAddr->text().toInt();
    i104.setSecondaryASDUAddress( obj.ca );

    switch ( obj.type )
    {
    case iec104_class::C_SC_NA_1:
    case iec104_class::C_SC_TA_1:
        obj.scs = ui->leCmdValue->text().toInt();
        break;
    case iec104_class::C_DC_NA_1:
    case iec104_class::C_DC_TA_1:
        obj.dcs = ui->leCmdValue->text().toInt();
        break;
    case iec104_class::C_RC_NA_1:
    case iec104_class::C_RC_TA_1:
        obj.rcs = ui->leCmdValue->text().toInt();
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
        QDateTime current = QDateTime::currentDateTime();
        obj.timetag.year = current.date().year()%100;
        obj.timetag.month = current.date().month();
        obj.timetag.mday = current.date().day();
        obj.timetag.hour = current.time().hour();
        obj.timetag.min = current.time().minute();
        obj.timetag.msec = current.time().second()*1000 + current.time().msec();
        obj.timetag.iv = 0;
        obj.timetag.su = 0;
        obj.timetag.wday = 0;
        obj.timetag.res1 = 0;
        obj.timetag.res2 = 0;
        obj.timetag.res3 = 0;
        obj.timetag.res4 = 0;
        break;
    }
    obj.qu = ui->cbCmdDuration->currentText().left(1).toInt();
    obj.se = (int)ui->cbSBO->isChecked();

    i104.sendCommand( &obj );
    LastCommandAddress = obj.address;
}

void MainWindow::BDTR_Loga( QString str, int id )
{
    if  (BDTR_Logar && id == 0 )
    {
        i104.mLog.pushMsg( (char*) str.toStdString().c_str(), 0 );
    }
}

void MainWindow::slot_dataIndication( iec_obj *obj, int numpoints )
{
    char buf[1000];
    char buftt[1000];
    int rw = -1;
    bool inserted = false;
    QTableWidgetItem *pitem;
    static const char* dblmsg[] = { "tra ","off ","on ","ind " };

    BDTR_processPoints( obj, numpoints );

    if ( ui->cbPointMap->isChecked() )
    {
        for (int i=0; i< numpoints; i++, obj++)
        {
            pitem = NULL;
            pitem = mapPtItem_ColAddress[std::make_pair(obj->ca,obj->address)];
            if ( pitem == NULL )
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

            sprintf( buf, "%9.3f", obj->value );
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
              case iec104_class::M_SP_NA_1: // 1
                  sprintf( buf, "%s%s%s%s%s", obj->scs?"on ":"off ", obj->iv?"iv ":"", obj->bl?"bl ":"", obj->sb?"sb ":"", obj->nt?"nt ":"" );
                  break;

              case iec104_class::M_DP_TB_1: // 31
                  sprintf( buftt, "Field: %02d/%02d/%02d %02d:%02d:%02d.%03d %s", obj->timetag.year, obj->timetag.month, obj->timetag.mday, obj->timetag.hour, obj->timetag.min, obj->timetag.msec/1000, obj->timetag.msec%1000, obj->timetag.iv?"iv":"ok" );
              case iec104_class::M_DP_NA_1: // 3
                  sprintf( buf, "%s%s%s%s%s", dblmsg[obj->dcs], obj->iv?"iv ":"", obj->bl?"bl ":"", obj->sb?"sb ":"", obj->nt?"nt ":"" );
                  break;

              case iec104_class::M_ST_TB_1: // 32
                sprintf( buftt, "Field: %02d/%02d/%02d %02d:%02d:%02d.%03d %s", obj->timetag.year, obj->timetag.month, obj->timetag.mday, obj->timetag.hour, obj->timetag.min, obj->timetag.msec/1000, obj->timetag.msec%1000, obj->timetag.iv?"iv":"ok" );
              case iec104_class::M_ST_NA_1: // 5
                  sprintf( buf, "%s%s%s%s%s%s", obj->ov?"ov ":"", obj->iv?"iv ":"", obj->bl?"bl ":"", obj->sb?"sb ":"", obj->nt?"nt ":"", obj->t?"t ":"" );
                  break;

              case iec104_class::M_ME_TD_1: // 34
              case iec104_class::M_ME_TE_1: // 35
              case iec104_class::M_ME_TF_1: // 36
                sprintf( buftt, "Field: %02d/%02d/%02d %02d:%02d:%02d.%03d %s", obj->timetag.year, obj->timetag.month, obj->timetag.mday, obj->timetag.hour, obj->timetag.min, obj->timetag.msec/1000, obj->timetag.msec%1000, obj->timetag.iv?"iv":"ok" );
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

          if ( ui->lwLog->item( cntLogMsgs % logBufSize )->text().indexOf("BDTR") >= 0 )
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
msg_req m;
m.COD = T_INICIO;
m.TIPO = REQ_GRUPO;
m.ORIG = BDTR_orig;
m.ID = 0;
m.NPTS = 0;
m.PONTOS[0] = 0;
udps->writeDatagram ( (const char *) &m, sizeof( msg_req ), BDTR_host, BDTR_porta );
if ( BDTR_HaveDualHost() )
  udps->writeDatagram ( (const char *) &m, sizeof( msg_req ), BDTR_host_dual, BDTR_porta );
BDTR_Loga( "T<-- BDTR: INTERROGATION BEGIN" );
}

void  MainWindow::slot_interrogationActTermIndication()
{
msg_req m;
m.COD = T_FIM;
m.TIPO = REQ_GRUPO;
m.ORIG = BDTR_orig;
m.ID = 0;
m.NPTS = 0;
m.PONTOS[0] = 0;
udps->writeDatagram ( (const char *) &m, sizeof( msg_req ), BDTR_host, BDTR_porta );
if ( BDTR_HaveDualHost() )
    udps->writeDatagram ( (const char *) &m, sizeof( msg_req ), BDTR_host_dual, BDTR_porta );
BDTR_Loga( "T<-- BDTR: INTERROGATION END" );
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
    if ( BDTR_HaveDualHost() && isPrimary == true )
    {
       BDTR_CntDnToBePrimary = BDTR_CntToBePrimary + 1; // wait a little more time to be primary again to allow for the secondary to assume
       isPrimary = false;
       i104.disable_connect();
       BDTR_Loga( " --- BDTR: BECOMING SECONDARY BY DISCONNECTION" );
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

void MainWindow::slot_commandActConfIndication( iec_obj *obj )
{
bool is_select = false;

    if ( LastCommandAddress == obj->address )
    {
        i104.mLog.pushMsg("     COMMAND ACT CONF INDICATION");
        is_select = ( obj->se == iec104_class::SELECT );

        // if confirmed select, execute
        if ( obj->se == iec104_class::SELECT && obj->pn == iec104_class::POSITIVE )
        {
            // if defined ASDU address on UI, use it
            // else will set to zero and use slave address (send Command will substitute zero to slave address)
            obj->ca = ui->leASDUAddr->text().toInt();
            obj->se = iec104_class::EXECUTE;
            i104.sendCommand( obj );
        }

        // respond to BDTR only if it's not a select or if its a negative response
        if ( is_select == false || obj->pn == iec104_class::NEGATIVE )
        {
            char bufOut[1600];  // buffer for bdtr response
            msg_ack *ms;
            ms=(msg_ack*)bufOut;
            // ack msg for BDTR
            ms->ID = 0;
            ms->COD = T_ACK;
            ms->TIPO = T_COM;
            ms->ORIG = BDTR_orig;
            ms->COMP = obj->address;
            switch ( obj->type )
            {
            case iec104_class::C_SC_NA_1:
            case iec104_class::C_SC_TA_1:
                ms->ID = ( obj->scs == 1 ) ? 2 : 1;
                break;
            case iec104_class::C_DC_NA_1:
            case iec104_class::C_DC_TA_1:
                ms->ID = obj->dcs;
                break;
            case iec104_class::C_RC_NA_1:
            case iec104_class::C_RC_TA_1:
                ms->ID = obj->rcs;
                break;
            }
            if ( obj->pn == iec104_class::NEGATIVE )
            {
                ms->ID |= 0x80;
                BDTR_Loga( "T<-- BDTR: COMMAND REJECTED BY IEC104 SLAVE" );
            }
            else
            {
                BDTR_Loga( "T<-- BDTR: COMMAND ACCEPTED BY IEC104 SLAVE" );
            }
            udps->writeDatagram ( (const char *) bufOut, sizeof(msg_ack), BDTR_host, BDTR_porta );
            if ( BDTR_HaveDualHost() )
                udps->writeDatagram ( (const char *) bufOut, sizeof(msg_ack), BDTR_host_dual, BDTR_porta );
        }
    }
}

void MainWindow::slot_commandActTermIndication( iec_obj *obj )
{
    if ( LastCommandAddress == obj->address )
      i104.mLog.pushMsg("     COMMAND ACT TERM INDICATION");
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    i104.terminate();
    event->accept();
}

void MainWindow::slot_timer_BDTR_kamsg()
{
    if ( ! isPrimary )
    {
       if ( BDTR_CntDnToBePrimary <= 0 )
          {
          isPrimary = true;
          i104.enable_connect();
          i104.tmKeepAlive->start();
          BDTR_CntDnToBePrimary = BDTR_CntToBePrimary;
          BDTR_Loga( " --- BDTR: BECOMING PRIMARY BY TIMEOUT" );
          ui->lbMode->setText( "<font color='green'>Primary</font>" );
          }
       else
          BDTR_CntDnToBePrimary--;
    }

    if ( isPrimary )
    {   // send keepalive message to the dual host

        msg_sinc m;
        m.COD = T_HORA;
        m.VAGO = 0;
        m.ORIG = BDTR_orig;

        time_t timer;
        struct tm *tblock;
        timer = time(NULL); // gets time of day
        tblock = localtime(&timer); // converts date/time to a structure

        m.TAG.ANO = tblock->tm_year+1900;
        m.TAG.MES = tblock->tm_mon+1;
        m.TAG.DIA = tblock->tm_mday;
        m.TAG.HORA = tblock->tm_hour;
        m.TAG.MINUTO = tblock->tm_min;
        m.TAG.MSEGS = 1000*tblock->tm_sec+0;

        udps->writeDatagram ( (const char *) &m, sizeof(m), BDTR_host_dual, BDTR_porta_escuta );
    }
}

void MainWindow::on_cbLog_clicked()
{
    if ( ui->cbLog->isChecked() )
        {
        i104.mLog.activateLog();
        QDate dt = QDate::currentDate();
        QString str = dt.toString() + (QString)" - " + (QString)QTESTER_VERSION;
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
