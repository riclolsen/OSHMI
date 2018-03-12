//
//
//  B A N C O  D E  D A D O S  D E  T E M P O  R E A L  -  B D T R
//
// INTERFACE BANCO DE DADOS - PROTOCOLOS
//
// 17-07-2009

//  DEFINICOES GERAIS

#ifndef _BDTR_H_
#define _BDTR_H_

#pragma pack(push,1)

#define far

#ifndef TRUE
#define TRUE -1
#endif
#ifndef FALSE
#define FALSE  0
#endif

//  CONSTANTES VISIVEIS EM OUTRAS APLICAÇÕES
#define PORT_BDTR 65280  // porta de acesso ao BDTR
#define PORT_OLDPCTR 32896 // porta de acesso ao protocolo PCTR pelas aplicações antigas
#define BROADCAST  255 // endereço de Aplicação reservado: broadcast
#define GW_PCTR 254 // endereço de Aplicação reservado: antigas aplicações PCTR
#define TAM_ID_PT  24  // tamanho do texto identificador do ponto

//  DEFINIÇÕES DA BASE DE DADOS

// Mascaras dos bits do campo STAT dos pontos de supervisão
#define FALHA  0X80 // indicador de falha de atualizacao do ponto (0: normal)
#define CONTROLE 0X40 // indicador de ponto de controle (0: supervisão)
#define QBIT 0X40 // indicador de falha na etiqueta tempo p/evento(0: normal)
#define ANALOGICO  0X20 // indicador de ponto analogico (0: digital)
#define IMPOSTO 0X10 // indicador de ponto com valor bloqueado  (0: liberado)
#define CARGA  0X08 // indicador de ponto com valor inicial (0: já foi atualizado)
#define CALCULADO  0X04 // indicador de ponto com valor calculado  (0: adquirido do campo)
#define DECBITS 0X03 // mascara para bits da casa decimal em ponto analogico
#define ESTADO  0X03 // indicador do estado do ponto digital (duplo)
#define INDETR  0X00  // valor para estado digital = indeterminado
#define ABERTO  0X01  // valor para estado digital = aberto
#define FECHADO 0X02  // valor para estado digital = fechado
#define INVALID 0X03  // valor para estado digital = invalido


//  OPERAÇÕES SOBRE PONTOS DE COMANDO
#define CANCEL  0  // operacao CANCELAR
#define OFF 1  // operacao DESLIGAR
#define ON  2  // operacao LIGAR
#define DESCER  1  // operacao DESCER
#define SUBIR  2  // operacao SUBIR
#define PULSO_NDEF 0X00 // pulso de largura NAO DEFINIDA pelo Centro
#define PULSO_CURTO 0X04 // pulso CURTO
#define PULSO_LONGO 0X08 // pulso LONGO
#define LATCH  0XC0 // saida PERSISTENTE

//  CÓDIGOS DAS MENSAGENS DE INTERFACES COM PROTOCOLOS

#define T_CIC  0X80 // bit indica envio ciclico do BDTR
#define T_SPONT 0X20 // bit indica mensagem recebida com causa espontanea
#define T_CONV  0X40 // bit indica codigo já convertido
#define T_TIME  0X10 // bit indica codigo com etiqueta de tempo
#define T_TIPO  0X0F // mascara que isola o tipo específico do dado na mensagem
#define T_MASC  0X1F // mascara para isolar codigo completo do dado
#define T_NULO  0  // tipo invalido ou informação de erro
#define T_DIG  1  // ponto digital sem etiqueta tempo
#define T_DIG_TAG  1+T_TIME // ponto digital com etiqueta tempo
#define T_ANA  2  // ponto analogico convertido sem etiqueta
#define T_NORM  3  // ponto analogico normalizado sem etiqueta
#define T_ANA_TAG  2+T_TIME // ponto analogico com etiqueta tempo
#define T_FLT  4  // ponto analogico em ponto flutuante sem etiqueta tempo
#define T_BIN  5  // sequencia de bits c/interpretação livre
#define T_PAR  6  // alteração de parametro de ponto da base de dados
#define T_DIG_INT  7  // ponto digital sem etiqueta tempo gerados internamente (BDTR ou protocolos)
#define T_REQ  8  // requisição de dados
#define T_COM  9  // requisição de comando
#define T_HORA  10 // informação de acerto de hora
#define T_GRUPO 11 // informação de atualização de grupo PCTR
#define T_RESET 12 // informação de iniciação do centro
#define T_ACK  13 // confirmação de envio
#define T_INICIO 14 // informação de inicio de envio de bloco de dados
#define T_FIM  15 // informação de fim de envio de bloco de dados
// tipos de requisição de dados
#define REQ_GRUPO  1  // todos os pontos de um grupo de dados
#define REQ_PONTO  2  // pontos de uma lista fornecida
#define REQ_EVENT  3  // uma lista de eventos
#define REQ_PCTR 4  // todos os pontos de um grupo no formato PCTR
#define REQ_FALHA  5  // os pontos falhados de um grupo
#define REQ_ANALO  6 // envio de uma lista de pontos analogicos
#define REQ_DIGIT  7 // envio de uma lista de pontos digitais
#define REQ_HORA 8 // envio de data e hora
// tipos de alteração de parametro
#define PAR_ESCALA 32  // termo escala da conversão analogica
#define PAR_OFFSET 33  // termo offset da conversão analogica
#define PAR_CODIGO 34  // codigo de representação do ponto (analogico: casa decimal)
#define PAR_ORIGEM 35  // estação origem do ponto

//  TIPOS DAS GRANDEZAS DO BDTR

// uso este formato para o campo de status do comando digital quando o varredor for sem banco
typedef union
{  // ponto no formato iec
    struct {
        unsigned char dcs : 2;  // estado duplo
        unsigned char qu : 5;  // qualificado de duração
        unsigned char se : 1;  // 1=com select, 0=execute sem select
    };
    unsigned char byte;
} CMDIEC;

typedef union
{ // definição de possíveis valores de um ponto do BDTR
    char  DIG; // valor digital
    short COM; // valor codificado para o comando
    short ANA; // valor inteiro convertido
    short NRM; // valor inteiro normalizado
    char  BST[4]; // valor bitstring 32 bits
    long  CNT; // valor contagem 32 bits
    float FLT; // valor em ponto flutuante

    struct // somente para o comando digital T_DIG de varredor sem banco
    {
        unsigned char COM;
        unsigned char UTR;
        unsigned char ASDU;
        CMDIEC COMIEC;
    } COM_SEMBANCO;

    struct // somente para o comando analógico de varredor sem banco
    {
    union {
        short NRM;
        short ANA;
        float FLT;
    };
        unsigned char UTR;
        unsigned char ASDU;
        CMDIEC COMIEC;
    } COM_SEMBANCOANA;

} valor;

typedef struct
{ // etiqueta de tempo
    unsigned short  ANO; // ano (1980 a 2099)
    unsigned char MES; // mes (1 a 12)
    unsigned char DIA; // dia: bits 0..4: no mes (1 a 31); bits 5..7: na semana (0=inv, 1= domingo ..)
    unsigned char HORA;  // hora (0 a 23)
    unsigned char MINUTO; // minuto (0 a 59)
    unsigned short  MSEGS;  // milisegundo no minuto(0 a 59999)
} A_tag;

typedef struct
{ // ponto digital sem etiqueta de tempo
    unsigned short  ID; // identificador do ponto
    unsigned char STAT;  // qualificador e estado atual (cfe. definicao STATUS)
} A_dig;

typedef struct
{ // ponto digital com etiqueta de tempo
    A_tag TAG; // etiqueta de tempo
    unsigned short  ID; // identificador do evento
    unsigned char UTR; // endereco da origem da informação
    unsigned char STAT;  // qualificador e estado atual (cfe. definicao STATUS)
} A_dig_tag;

typedef struct
{ // ponto analogico sem etiqueta de tempo
    unsigned short  ID; // identificador do ponto
    unsigned char STAT;  // qualificador do ponto (cfe. definicao STATUS)
    short VALOR;  // valor atual da medida
} A_ana;

typedef struct
{ // ponto analogico com etiqueta de tempo
    A_tag TAG; // etiqueta de tempo
    unsigned short  ID; // identificador do evento
    unsigned char UTR; // endereco da origem da informação
    unsigned char STAT;  // qualificador (cfe. definicao STATUS)
    short VALOR;  // valor atual da medida
} A_ana_tag;

typedef struct
{ // ponto analogico em ponto flutuante sem etiqueta de tempo
    unsigned short  ID; // identificador do ponto
    unsigned char STAT;  // qualificador do ponto (cfe. definicao STATUS)
    float VALOR;  // valor atual da medida
} A_float;

typedef struct
{ // ponto de comando
    unsigned short  ID; // identificador do ponto
    unsigned char STATUS; // conjunto de qualificadores
    valor VALOR;  // valor a ser atribuido
} A_com;

typedef struct
{ // sequência de bits c/ interpretação livre
    unsigned short  ID; // identificador do ponto
    unsigned char STAT;  // qualificador do ponto
    unsigned char VALOR[4];  // valor como sequencia de bits
} A_bin;

typedef struct
{ // parâmetros de um ponto da base de dados
    unsigned short  ID; // identificador do ponto
    unsigned short  POS; // indice do ponto na base de dados
    unsigned short  ESCALA; // fator de escala analogica ou inversão digital
    short OFFSET; // offset da conversão analogica
    unsigned char ORIG; // identificador da base de dados que atualiza o ponto
    unsigned char STAT; // qualificadores externos do ponto
} A_reg;

typedef struct
{ // campos variaveis do registro de um ponto da base de dados
    unsigned short  POS; // indice do ponto na base de dados
    unsigned char STAT; // qualificadores externos do ponto
    unsigned char RES; // reservado
    short VALOR; // valor
} A_ponto;

typedef struct
{ // formato de lista de eventos
    int MAXIMO; // numero maximo de eventos na lista
    int TAMANHO;  // numero de eventos armazenados na lista
    int INICIO; // indice do primeiro evento da lista
    A_dig_tag  EVENTOS[1]; // vetor de eventos como lista circular
} A_lista;


//  TIPOS DAS MENSAGENS DE ACESSO AO BDTR

typedef struct
{ // mensagem de digitais s/ etiqueta tempo
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo (T_DIG)
    unsigned char NRPT;  // numero de pontos do tipo acima
    unsigned char ORIG;  // endereço de aplicação da origem dos dados
    A_dig PONTO[1];  // vetor de pontos
} msg_dig;

typedef struct
{ // mensagem de digitais c/ etiqueta tempo
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo (T_DIG_TAG)
    unsigned char NRPT;  // numero de pontos do tipo acima
    unsigned char ORIG;  // endereço de aplicação da origem dos dados
    A_dig_tag PONTO[1];  // vetor de pontos
} msg_dig_tag;

typedef struct
{ // mensagem de analogicos s/ etiqueta tempo
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo (T_ANA ou T_NORM)
    unsigned char NRPT;  // numero de pontos do tipo acima
    unsigned char ORIG;  // endereço de aplicação da origem dos dados
    A_ana PONTO[1];  // vetor de pontos
} msg_ana;

typedef struct
{ // mensagem de analogicos c/ etiqueta tempo
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo acima
    unsigned char NRPT;  // numero de pontos do tipo (T_ANA_TAG)
    unsigned char ORIG;  // endereço de aplicação da origem dos dados
    A_ana_tag PONTO[1];  // vetor de pontos
} msg_ana_tag;

typedef struct
{ // mensagem de analogicos em ponto flutuante s/ etiqueta tempo
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo (T_FLT)
    unsigned char NRPT;  // numero de pontos do tipo acima
    unsigned char ORIG;  // endereço de aplicação da origem dos dados
    A_float  PONTO[1];  // vetor de pontos
} msg_float;

typedef struct
{ // mensagem de sequência de bits
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo (T_BIN)
    unsigned char NRPT;  // numero de pontos do tipo acima
    unsigned char ORIG;  // endereço de aplicação da origem dos dados
    A_bin PONTO[1];  // vetor de pontos
} msg_bin;

typedef struct
{ // mensagem de requisição de comando
    //  unsigned short  TAM; // numero de bytes da mensagem
    unsigned char COD; // identificador do tipo da mensagem (T_COM)
    unsigned char TVAL;  // tipo do valor do comando (T_DIG,T_ANA,T_FLT)
    unsigned char ORIG;  // endereço de aplicação da origem dos dados
    A_com PONTO;  // ponto de comando
} msg_com;

typedef struct
{ // mensagem de alteração de parametro
    //  unsigned short  TAM; // numero de bytes da mensagem
    unsigned char COD; // identificador do tipo da mensagem (T_PAR)
    unsigned char TPAR; // tipo do parametro (PAR_ESCALA,PAR_OFFSET,PAR_ORIGEM)
    unsigned char ORIG; // endereço de aplicação da origem da alteração
    unsigned short  ID; // identificador do ponto
    short VALOR;  // valor do parametro
} msg_par;

typedef struct
{ // mensagem de requisição de dados
    //  unsigned short  TAM; // numero de bytes da mensagem
    unsigned char COD; // identificador do tipo da mensagem (T_REQ)
    unsigned char TIPO;  // tipo da requisição (conforme definição acima)
    unsigned char ORIG;  // endereço de aplicação do centro origem
    unsigned short  ID; // identificador do ponto ou grupo
    unsigned char NPTS;  // numero de pontos do vetor a seguir, número de parcelas da IG em caso de T_FIM
    unsigned short  PONTOS[1]; // valor a ser atribuido
} msg_req;

typedef struct
{ // mensagem atualização de grupo PCTR
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo (T_GRUPO)
    unsigned char DEST;  // endereco de aplicação do destino
    unsigned char ORIG;  // endereço de aplicação da origem dos dados
    unsigned char VAGO[3]; // sem significado (espaço para o PCTR)
    unsigned short  ID; // identificador do grupo
    unsigned short  NBYT;  // numero de bytes no vetor de dados
    unsigned char DADOS[1];  // dados
} msg_grupo;

typedef struct
{ // mensagem de confirmação
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo (T_ACK)
    unsigned char TIPO;  // identificador do tipo confirmado
    unsigned char ORIG; // endereço de aplicação da estação
    unsigned char ID; // identificador da confirmação (a criterio da aplicação)
    unsigned short  COMP; // complemento (a criterio da aplicação)
} msg_ack;

typedef struct
{ // mensagem de sincronização
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo (T_HORA)
    unsigned char VAGO;  // sem significado
    unsigned char ORIG;  // endereço de aplicação da estação origem
    A_tag TAG; // etiqueta de tempo
} msg_sinc;

typedef struct
{ // mensagem genérica
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo
    unsigned char NBYT;  // numero de bytes no vetor de dados
    unsigned char ORIG;  // endereço de aplicação da origem dos dados
    unsigned char INFO[1]; // vetor de dados
} msg_dado;

typedef struct
{ // mensagem identificação de pontos
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo
    unsigned char NRPT; // numero de pontos do tipo acima
    unsigned char ORIG; // endereço de aplicação da origem dos dados
    A_reg PONTO[1]; // vetor de dados
} msg_reg;

typedef struct
{ // mensagem leitura de pontos
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo
    unsigned char NRPT; // numero de pontos do tipo acima
    unsigned char ORIG; // endereço de aplicação da origem dos dados
    A_ponto  PONTO[1]; // vetor de dados
} msg_ponto;

typedef struct
{ // mensagem de identificação de host
    //  unsigned short  TAM; // numero de bytes a seguir
    unsigned char COD; // identificador do tipo
    unsigned char VAGO; // sem significado
    unsigned char ORIG; // endereço de aplicação do host
    unsigned long IPA; // endereço IP do host
} msg_host;

typedef union
{ // ponteiro para mensagem trocada entre BDTR e protocolos
    msg_dig far* DIG; // ponto digital sem etiqueta de tempo
    msg_ana far* ANA; // ponto analogico sem etiqueta de tempo
    msg_float  far* FLT; // ponto analogico em ponto flutuante sem etiqueta de tempo
    msg_bin far* BIN; // sequência de bits sem etiqueta de tempo
    msg_com far* COM; // mensagem de requisição de comando
    msg_dig_tag far* DIGT;  // ponto digital com etiqueta de tempo
    msg_ana_tag far* ANAT;  // ponto analogico com etiqueta de tempo
    msg_grupo  far* GRUPO;  // informação de atualização de grupo
    msg_ack far* CONF;  // mensagem de confirmaçao
    msg_sinc far* SINC;  // mensagem de comando de sincronização
    msg_req far* INTR;  // mensagem de interrogação
    msg_par far* PAR; // mensagem de alteração de parâmetro
    msg_reg far* REG;  // mensagem de identificação de pontos
    msg_ponto  far* PONTO;  // mensagem de leitura de pontos
    msg_host far* HOST; // mensagem de identificação de host
    msg_dado far* DADO;  // mensagem de dados genericos
    void far* GERAL;  // mensagem generica
} msg_bdtr;

typedef struct
{ // grupo de pontos: parametros
    unsigned ID; // identificador do grupo
    char EST[6];  // estacao remota
    unsigned MAXC; // maxima contagem (time_out)
    unsigned NPTOS;  // numero de pontos
} grupo_par;

typedef struct
{ // grupo de pontos: variaveis
    unsigned CONT; // contador de acertos
    unsigned ACUM; // acumulador de tempo
    unsigned FALH; // acumulador de falhas
} grupo_var;

typedef struct
{ // banco de dados: parametros
    unsigned char  ID; // identificador do BDTR (endereço de APLICAÇÃO)
    unsigned char  TOTUTR; // total de remotas cadastradas
    unsigned TOTPTO; // total de pontos
    unsigned TOTTRF; // total de transferencias
    unsigned TOTUSU; // total de usuarios cadastradas
    unsigned TOTGRP; // total de grupos
} bd_par;

// Estrutura para formato A de qualificador de ponto digital e analógico
typedef struct
{
    bool dumb_1:1; // não importa
    bool AcertoHora:1; // indica que houve acerto de hora para trás
    bool Origem1:1; // origem - 0=supervisionado 1=calculado 0=carga ini 1=manual
    bool Origem2:1; //  0 0 1 1
    bool Subst:1;  // indicador de ponto substituido, imposição manual
    bool Tipo:1;  // tipo do ponto 1=analógico, 0=digital
    bool Quest:1;  // bit de questionamento, questiona a validade da medida
    bool Falha:1;  // indicador de falha na atualização, valor antigo
} TFA_Comum;

// Estrutura para formato A de qualificador de ponto digital
typedef struct
{
    bool Estado:1; // estado 0 ou 1
    bool NaoUsado:1;  // não usado
    bool Origem1:1; // origem - 0=supervisionado 1=calculado 0=carga ini 1=manual
    bool Origem2:1; //  0 0 1 1
    bool Subst:1;  // indicador de ponto substituido, imposição manual
    bool Tipo:1;  // tipo do ponto 1=analógico, 0=digital
    bool Quest:1;  // bit de questionamento, questiona a validade da medida
    bool Falha:1;  // indicador de falha na atualização, valor antigo
} TFA_Digital;

// Estrutura para formato A de qualificador de ponto analógico
typedef struct
{
    bool CasaDecimal1:1; // casa decimal: 0, 1, 2 ou 3 casas
    bool CasaDecimal2:1; // a casa decimal não é válida para o ponto no banco local!
    bool Origem1:1; // origem - 0=supervisionado 1=calculado 0=carga ini 1=manual
    bool Origem2:1; //  0 0 1 1
    bool Subst:1;  // indicador de ponto substituido, imposição manual
    bool Tipo:1;  // tipo do ponto 1=analógico, 0=digital
    bool Quest:1;  // bit de questionamento, questiona a validade da medida
    bool Falha:1;  // indicador de falha na atualização, valor antigo
} TFA_Analogico;

// Qualificadores de ponto em formato A
typedef union
{
    TFA_Digital Dig;
    TFA_Analogico Ana;
    TFA_Comum Pto;
    unsigned char Byte;

    // Estrutura para formato A de qualificador de ponto digital
    struct
    {
        bool Estado:1; // estado 0 ou 1
        bool EstadoH:1;  // não usado
        bool Origem1:1; // origem - 0=supervisionado 1=calculado 0=carga ini 1=manual
        bool Origem2:1; //  0 0 1 1
        bool Subst:1;  // indicador de ponto substituido, imposição manual
        bool Tipo:1;  // tipo do ponto 1=analógico, 0=digital
        bool FalhaTag:1;  // falha no tag de tempo
        bool Falha:1;  // indicador de falha na atualização, valor antigo
    };

    // Estrutura para formato A de qualificador de ponto analógico
    struct
    {
        unsigned Duplo:2;  // Estado duplo
        unsigned Origem:2; // origem - 0=supervisionado 1=calculado 2=carga ini 3=manual
        bool _dumb4:1; // indicador de ponto substituido, imposição manual
        bool _dumb5:1; // tipo do ponto 1=analógico, 0=digital
        bool _dumb6:1; // bit de questionamento, questiona a validade da medida
        bool _dumb7:1; // indicador de falha na atualização, valor antigo
    };

    // Estrutura para formato A de qualificador de ponto analógico
    struct
    {
        bool CasaDecimal1:1; // casa decimal: 0, 1, 2 ou 3 casas
        bool CasaDecimal2:1; // a casa decimal não é válida para o ponto no banco local!
        bool dumb2:1; // origem - 0=supervisionado 1=calculado 0=carga ini 1=manual
        bool dumb3:1; //  0 0 1 1
        bool dumb4:1;  // indicador de ponto substituido, imposição manual
        bool dumb5:1;  // tipo do ponto 1=analógico, 0=digital
        bool dumb6:1;  // bit de questionamento, questiona a validade da medida
        bool dumb7:1;  // indicador de falha na atualização, valor antigo
    };

    // Estrutura para formato A de qualificador de ponto analógico
    struct
    {
        bool CasaDecimal:2; // casa decimal: 0, 1, 2 ou 3 casas
        bool dumb8:6;
    };

} TFA_Qual;

#define TFA_TIPODIG 0
#define TFA_TIPOANA 1

#pragma pack(pop)

#endif
