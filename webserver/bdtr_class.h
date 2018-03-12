#ifndef bdtr_H
#define bdtr_H

#include <string>
#include "bdtr.h"

using namespace std;

class BDTR
{
public:
    unsigned LogaEmArq;
    unsigned Logar;
    unsigned cntDig;
    unsigned cntDigTag;
    unsigned cntAna;
    unsigned cntTot;
    unsigned cntSinc;
    unsigned cntOutras;
    unsigned cntReset;
    unsigned cntErro;
    unsigned cntAck;
    unsigned cntCmd;

    bool VarredorEhConversor();  // informa se o varredor possui banco (false) ou não (true)  

    int BDTR_ENVIA_COMANDOS;
    int IHM_PRINCIPAL; // guarda estado do IHM Principal/Reserva
    int cntIntgr;   // contador de integridades: GI
    unsigned cntIniRecIntgr;   // conta inicio de GI
    unsigned cntFimRecIntgr;   // conta fim de GI
    unsigned cntParcelasIntgr; // conta as parcelas da GI
    bool IntgrEmAndamento;     // indica andamento de integridade
    unsigned cntErroIntgr;     // conta GIs com erro em parcelas 
    // unsigned TimeoutIntgr;     // timeout para integridade

    unsigned char EventosAQuitar;
    unsigned char cntQuitEve;
    string BDTR_IPQUITAR;
    string IP_BDTR1;
    string IP_BDTR2;
    unsigned short BDTR_PORTA;
    unsigned short BDTR_PORTA_CMD;
    unsigned char MEU_END;
    int BDTR_GRUPO_INTEGRIDADE;
    unsigned int PERIODO_INTEGRIDADE; // em segundos
    unsigned int DELAY_PARA_INTEGRIDADE_APOS_RESET; // em segundos
    unsigned int SegundosParaGI; // indica quanto falta para próxima integridade

    int ORIG_DUAL; // 255: identifica origem IHM DUAL em mensagem no formato BDTR
    string msg_status;

    BDTR();
    ~BDTR();
    int EhPrincipal();
    enum tipo_integridade { integridade_normal, integridade_forcada };
    void Sol_Integridade(tipo_integridade tpint);
    void SetaPeriodoIntegridade(int segundos);
    void EnviaHora( string IP, unsigned char Origem );
    // int EnviaComando(int nponto, int val, int tipo);
    void ShowConv104BDTR();
    int EnviaComando(msg_com * mc);
    int BlkComando(int endponto, int blk);
    int EnviaPontoDig(int endponto, unsigned char qualif, string ip, int porta);
    void ontimerseg(); // timer de segundo, deve ser chamado pela aplicação
    void recebe_udp(char * IP);
    virtual int le_udp(char * buf, int szmax)=0;
    virtual void envia_udp(char * IP, int porta, char * buf, int sz)=0;
    virtual void set_udp_timeout(int timeout){return;};
    virtual void logaln(string msg)=0;
    virtual int EscrevePonto( int nponto,
                              float valor,
                              unsigned char qualif,
                              int calculo=0,
                              int usakconv=0,
                              int temtagtmp=0,
                              int espontaneo=0){return 0;};
    virtual int IncluiEvento( unsigned int ID,
                              unsigned int UTR,
                              unsigned int Flags,
                              unsigned int Ano,
                              unsigned int Mes,
                              unsigned int Dia,
                              unsigned int Hora,
                              unsigned int Minuto,
                              unsigned int Segundo,
                              unsigned int Milisegundo
                              ){return 0;};
    virtual int SetAckCmd(int nponto, int falha, int cmd){return 0;}; // seta ack de comando
    virtual void RecebeuIntegridade(){return;};
    virtual void RecebeuHora(msg_sinc *m){return;};
    virtual void CommandIndication(int nptcmd, char flags, float valor){return;}; // informa que recebu comando pelo bdtr
    virtual void GenInterrogIndication(){return;};
};

#endif





