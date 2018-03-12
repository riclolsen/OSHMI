//
//
//              B A N C O   D E   D A D O S   D E   T E M P O   R E A L   -   B D T R
//
//                       INTERFACE BANCO DE DADOS - PROTOCOLOS
//
//                                 17-07-2009

//                                     DEFINICOES GERAIS

#ifndef _BDTR_H_
#define _BDTR_H_

#pragma pack(push,1)                        // campos das estruturas contiguos

#ifndef TRUE
#define TRUE        -1
#endif
#ifndef FALSE
#define FALSE       0
#endif

//                                     	CONSTANTES VISIVEIS EM OUTRAS APLICAÇÕES
#define PORT_BDTR   	65280   	// porta de acesso ao BDTR
#define BDTR_PORTA_CMD_LOCAL 65281      // porta de acesso para varredor sem banco local
#define PORT_OLDPCTR    32896          	// porta de acesso ao protocolo PCTR pelas aplicações antigas
#define BROADCAST       255           	// endereço de Aplicação reservado: broadcast	
#define GW_PCTR         254           	// endereço de Aplicação reservado: antigas aplicações PCTR		
#define TAM_ID_PT       24            	// tamanho do texto identificador do ponto  
 
//                                     DEFINIÇÕES DA BASE DE DADOS

                                       	// Mascaras dos bits do campo STAT dos pontos de supervisão
#define FALHA       0X80               	// indicador de falha de atualizacao do ponto	(0: normal) 
#define CONTROLE    0X40          		// indicador de ponto de controle 				(0: supervisão)
#define QBIT        0X40               	// indicador de falha na etiqueta tempo p/evento(0: normal)             
#define ANALOGICO   0X20              	// indicador de ponto analogico 				(0: digital)          
#define IMPOSTO     0X10               	// indicador de ponto com valor bloqueado 		(0: liberado)        
#define CARGA       0X08          		// indicador de ponto com valor inicial         (0: já foi atualizado)
#define CALCULADO   0X04              	// indicador de ponto com valor calculado 		(0: adquirido do campo)        
#define DECBITS     0X03               // mascara para bits da casa decimal em ponto analogico
#define ESTADO      0X03               // indicador do estado do ponto digital (duplo)       
#define INDETR      0X00              //  valor para estado digital = indeterminado         
#define ABERTO      0X01              //  valor para estado digital = aberto         
#define FECHADO     0X02              //  valor para estado digital = fechado        
#define INVALID     0X03              //  valor para estado digital = invalido         


//                                     OPERAÇÕES SOBRE PONTOS DE COMANDO
#define CANCEL      0                  // operacao CANCELAR                         
#define OFF         1                  // operacao DESLIGAR
#define ON          2                  // operacao LIGAR                         
#define DESCER      1                  // operacao DESCER                  
#define SUBIR       2                  // operacao SUBIR                      
#define PULSO_NDEF  0X00               // pulso de largura NAO DEFINIDA pelo Centro
#define PULSO_CURTO 0X04               // pulso CURTO
#define PULSO_LONGO 0X08               // pulso LONGO
#define LATCH       0XC0               // saida PERSISTENTE

//                                     CÓDIGOS DAS MENSAGENS DE INTERFACES COM PROTOCOLOS                        

#define T_CIC       0X80               // bit indica envio ciclico do BDTR 
#define T_SPONT     0X20               // bit indica mensagem recebida com causa espontanea
#define T_CONV      0X40               // bit indica codigo já convertido
#define T_TIME      0X10               // bit indica codigo com etiqueta de tempo
#define T_TIPO      0X0F               // mascara que isola o tipo específico do dado na mensagem
#define T_MASC      0X1F               // mascara para isolar codigo completo do dado
#define T_NULO      0                  //  tipo invalido ou informação de erro
#define T_DIG       1                  //  ponto digital sem etiqueta tempo
#define T_DIG_TAG   1+T_TIME           //  ponto digital com etiqueta tempo
#define T_ANA       2                  //  ponto analogico convertido sem etiqueta
#define T_NORM      3                  //  ponto analogico normalizado sem etiqueta
#define T_ANA_TAG   2+T_TIME           //  ponto analogico com etiqueta tempo
#define T_FLT       4                  //  ponto analogico em ponto flutuante sem etiqueta tempo
#define T_BIN       5                  //  sequencia de bits c/interpretação livre
#define T_PAR       6                  	//  alteração de parametro de ponto da base de dados
#define T_DIG_INT   7                  	//  ponto digital sem etiqueta tempo gerados internamente (BDTR ou protocolos)
#define T_REQ       8                  //  requisição de dados 
#define T_COM       9                  //  requisição de comando 
#define T_HORA      10                 //  informação de acerto de hora
#define T_GRUPO     11                 //  informação de atualização de grupo PCTR
#define T_RESET     12                 //  informação de iniciação do centro
#define T_ACK       13                 //  confirmação de envio 
#define T_INICIO    14                 //  informação de inicio de envio de bloco de dados 
#define T_FIM       15                 //  informação de fim de envio de bloco de dados 
                                       // tipos de requisição de dados
#define REQ_GRUPO   1                  //  todos os pontos de um grupo de dados 
#define REQ_PONTO   2                  //  pontos de uma lista fornecida
#define REQ_EVENT   3                  //  uma lista de eventos
#define REQ_PCTR    4                  //  todos os pontos de um grupo no formato PCTR
#define REQ_FALHA   5                  //  os pontos falhados de um grupo
#define REQ_ANALO   6                  	//  envio de uma lista de pontos analogicos
#define REQ_DIGIT   7                  	//  envio de uma lista de pontos digitais
#define REQ_HORA    8                  	//  envio de data e hora
                                       	// tipos de alteração de parametro
#define PAR_ESCALA  32                 	//  termo escala da conversão analogica  
#define PAR_OFFSET  33                 	//  termo offset da conversão analogica  
#define PAR_CODIGO  34                 	//  codigo de representação do ponto (analogico: casa decimal)             
#define PAR_ORIGEM  35                 	//  estação origem do ponto              

//                                     TIPOS DAS GRANDEZAS DO BDTR

// uso este formato para o campo de status do comando digital quando o varredor for sem banco
typedef union
{                            //  ponto no formato iec
struct {
   unsigned char dcs : 2;       //  estado duplo
   unsigned char qu  : 5;       //  qualificado de duração
   unsigned char se  : 1;       //  1=com select, 0=execute sem select
   };
unsigned char byte;
} CMDIEC;

typedef union
   {                                   // definição de possíveis valores de um ponto do BDTR
   char             DIG;               //  valor digital
   short            COM;               //  valor codificado para o comando
   short            ANA;               //  valor inteiro convertido
   short            NRM;               //  valor inteiro normalizado
   char             BST[4];            //  valor bitstring 32 bits
   long             CNT;               //  valor contagem 32 bits
   float            FLT;               //  valor em ponto flutuante

   struct // somente para o comando de varredor sem banco
     {
     unsigned char COM;
     unsigned char UTR;
     unsigned char ASDU;
     CMDIEC COMIEC;
     } COM_SEMBANCO;

    struct // somente para o comando analógico T_FLT de varredor sem banco
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

typedef  struct
   {                                   // etiqueta de tempo
   unsigned short   ANO;               //  ano (1980 a 2099)               
   unsigned char    MES;               //  mes (1 a 12)                     
   unsigned char    DIA;               //  dia: bits 0..4: no mes (1 a 31); bits 5..7: na semana (0=inv, 1= domingo ..)                     
   unsigned char    HORA;              //  hora (0 a 23)                    
   unsigned char    MINUTO;            //  minuto (0 a 59)                  
   unsigned short   MSEGS;             //  milisegundo no minuto(0 a 59999) 
   } A_tag;         
                    
typedef  struct           
   {                                   // ponto digital sem etiqueta de tempo
   unsigned short   ID;                //  identificador do ponto       
   unsigned char    STAT;              //  qualificador e estado atual (cfe. definicao STATUS)            
   } A_dig;         
                    
typedef  struct           
   {                                   // ponto digital com etiqueta de tempo
   A_tag            TAG;               //  etiqueta de tempo
   unsigned short   ID;                //  identificador do evento       
   unsigned char    UTR;               //  endereco da origem da informação 
   unsigned char    STAT;              //  qualificador e estado atual (cfe. definicao STATUS)            
   } A_dig_tag;     
                    
typedef  struct           
   {                                   // ponto analogico sem etiqueta de tempo
   unsigned short   ID;                //  identificador do ponto       
   unsigned char    STAT;              //  qualificador do ponto (cfe. definicao STATUS)          
   short            VALOR;             //  valor atual da medida
   } A_ana;         
                    
typedef  struct           
   {                                   // ponto analogico com etiqueta de tempo
   A_tag            TAG;               //  etiqueta de tempo
   unsigned short   ID;                //  identificador do evento       
   unsigned char    UTR;               //  endereco da origem da informação
   unsigned char    STAT;              //  qualificador (cfe. definicao STATUS)                                   
   short            VALOR;             //  valor atual da medida
   } A_ana_tag;     
                    
typedef  struct           
   {                                   // ponto analogico em ponto flutuante sem etiqueta de tempo
   unsigned short   ID;                //  identificador do ponto       
   unsigned char    STAT;              //  qualificador do ponto (cfe. definicao STATUS)          
   float            VALOR;             //  valor atual da medida
   } A_float;       
                    
typedef  struct           
   {                                   // ponto de comando
   unsigned short   ID;                //  identificador do ponto       
   unsigned char    STATUS;            //  conjunto de qualificadores
   valor            VALOR;             //  valor a ser atribuido 
   } A_com;         
                    
typedef  struct           
   {                                   // sequência de bits c/ interpretação livre
   unsigned short   ID;                //  identificador do ponto       
   unsigned char    STAT;              //  qualificador do ponto           
   unsigned char    VALOR[4];          //  valor como sequencia de bits
   } A_bin;       

typedef  struct                        	
   {                                   	// parâmetros de um ponto da base de dados
   unsigned short   ID;                	//  identificador do ponto       
   unsigned short   POS;                //  indice do ponto na base de dados
   unsigned short   ESCALA;          	//  fator de escala analogica ou inversão digital
   short            OFFSET;          	//  offset da conversão analogica 
   unsigned char    ORIG;              	//  identificador da base de dados que atualiza o ponto           
   unsigned char    STAT;              	//  qualificadores externos do ponto           
   } A_reg;                            	
                                       	
typedef  struct                        	
   {                                   	// campos variaveis do registro de um ponto da base de dados
   unsigned short   POS;                //  indice do ponto na base de dados
   unsigned char    STAT;              	//  qualificadores externos do ponto           
   unsigned char    RES;              	//  reservado           
   short            VALOR;          	//  valor 
   } A_ponto;                            	
                                       	
typedef struct                         	
   {                                   	// formato de lista de eventos
   int            MAXIMO;              	//  numero maximo de eventos na lista
   int            TAMANHO;             	//  numero de eventos armazenados na lista
   int            INICIO;              	//  indice do primeiro evento da lista
   A_dig_tag      EVENTOS[1];          	//  vetor de eventos como lista circular                                  
   } A_lista;                         	

   
//                                     TIPOS DAS MENSAGENS DE ACESSO AO BDTR                        

typedef struct          
   {                                   // mensagem de digitais s/ etiqueta tempo
//   unsigned short   TAM;               //  numero de bytes a seguir
   unsigned char    COD;               //  identificador do tipo (T_DIG)
   unsigned char    NRPT;              //  numero de pontos do tipo acima
   unsigned char    ORIG;              //  endereço de aplicação da origem dos dados
   A_dig            PONTO[1];          //  vetor de pontos
   } msg_dig;

typedef struct
   {                                   // mensagem de digitais c/ etiqueta tempo
//   unsigned short   TAM;               //  numero de bytes a seguir
   unsigned char    COD;               //  identificador do tipo (T_DIG_TAG)
   unsigned char    NRPT;              //  numero de pontos do tipo acima
   unsigned char    ORIG;              //  endereço de aplicação da origem dos dados
   A_dig_tag        PONTO[1];          //  vetor de pontos
   } msg_dig_tag;

typedef struct
   {                                   // mensagem de analogicos s/ etiqueta tempo
//   unsigned short   TAM;               //  numero de bytes a seguir
   unsigned char    COD;               //  identificador do tipo (T_ANA ou T_NORM)
   unsigned char    NRPT;              //  numero de pontos do tipo acima
   unsigned char    ORIG;              //  endereço de aplicação da origem dos dados
   A_ana            PONTO[1];          //  vetor de pontos
   } msg_ana;

typedef struct
   {                                   // mensagem de analogicos c/ etiqueta tempo
//   unsigned short   TAM;               //  numero de bytes a seguir
   unsigned char    COD;               //  identificador do tipo acima
   unsigned char    NRPT;              //  numero de pontos do tipo (T_ANA_TAG)
   unsigned char    ORIG;              //  endereço de aplicação da origem dos dados
   A_ana_tag        PONTO[1];          //  vetor de pontos
   } msg_ana_tag;

typedef struct
   {                                   // mensagem de analogicos em ponto flutuante s/ etiqueta tempo
//   unsigned short   TAM;               //  numero de bytes a seguir
   unsigned char    COD;               //  identificador do tipo (T_FLT)
   unsigned char    NRPT;              //  numero de pontos do tipo acima
   unsigned char    ORIG;              //  endereço de aplicação da origem dos dados
   A_float          PONTO[1];          //  vetor de pontos
   } msg_float;

typedef struct
   {                                   // mensagem de sequência de bits
//   unsigned short   TAM;               //  numero de bytes a seguir
   unsigned char    COD;               //  identificador do tipo (T_BIN)
   unsigned char    NRPT;              //  numero de pontos do tipo acima
   unsigned char    ORIG;              //  endereço de aplicação da origem dos dados
   A_bin            PONTO[1];          //  vetor de pontos
   } msg_bin;

typedef struct
   {                                   // mensagem de requisição de comando
//   unsigned short   TAM;               //  numero de bytes da mensagem
   unsigned char    COD;               //  identificador do tipo da mensagem (T_COM)
   unsigned char    TVAL;              //  tipo do valor do comando (T_DIG,T_ANA,T_FLT)
   unsigned char    ORIG;              //  endereço de aplicação da origem dos dados
   A_com            PONTO;             //  ponto de comando
   } msg_com;

typedef struct                         	
   {                                   	// mensagem de alteração de parametro
//   unsigned short   TAM;               	//  numero de bytes da mensagem         
   unsigned char    COD;               	//  identificador do tipo da mensagem (T_PAR)      
   unsigned char    TPAR;              	//  tipo do parametro (PAR_ESCALA,PAR_OFFSET,PAR_ORIGEM)                                       
   unsigned char    ORIG;              	//  endereço de aplicação da origem da alteração            
   unsigned short   ID;                	//  identificador do ponto       
   short            VALOR;             	//  valor do parametro                 
   } msg_par;                          	

typedef struct
   {                                   // mensagem de requisição de dados
//   unsigned short   TAM;               //  numero de bytes da mensagem
   unsigned char    COD;               //  identificador do tipo da mensagem (T_REQ)
   unsigned char    TIPO;              //  tipo da requisição (conforme definição acima)
   unsigned char    ORIG;              //  endereço de aplicação do centro origem
   unsigned short   ID;                //  identificador do ponto ou grupo
   unsigned char    NPTS;              //  numero de pontos do vetor a seguir, número de parcelas da IG em caso de T_FIM
   unsigned short   PONTOS[1];         //  valor a ser atribuido
   } msg_req;

typedef struct
   {                                   // mensagem atualização de grupo PCTR
//   unsigned short   TAM;               //  numero de bytes a seguir
   unsigned char    COD;               //  identificador do tipo (T_GRUPO)
   unsigned char    DEST;              //  endereco de aplicação do destino
   unsigned char    ORIG;              //  endereço de aplicação da origem dos dados
   unsigned char    VAGO[3];           //  sem significado (espaço para o PCTR)
   unsigned short   ID;                //  identificador do grupo
   unsigned short   NBYT;              //  numero de bytes no vetor de dados
   unsigned char    DADOS[1];          //  dados
   } msg_grupo;

typedef struct            
   {                                   	// mensagem de confirmação
//   unsigned short   TAM;               //  numero de bytes a seguir
   unsigned char    COD;               	//  identificador do tipo (T_ACK)
   unsigned char    TIPO;             	//  identificador do tipo confirmado
   unsigned char    ORIG;              	//  endereço de aplicação da estação
   unsigned char    ID;               	//  identificador da confirmação (a criterio da aplicação)
   unsigned short   COMP;              	//  complemento (a criterio da aplicação)
   } msg_ack;

typedef struct
   {                                   // mensagem de sincronização
//   unsigned short   TAM;               //  numero de bytes a seguir
   unsigned char    COD;               //  identificador do tipo (T_HORA)
   unsigned char    VAGO;              //  sem significado
   unsigned char    ORIG;              //  endereço de aplicação da estação origem
   A_tag            TAG;               //  etiqueta de tempo
   } msg_sinc;

typedef struct
   {                                   // mensagem genérica
//   unsigned short   TAM;               //  numero de bytes a seguir
   unsigned char    COD;               //  identificador do tipo
   unsigned char    NBYT;              //  numero de bytes no vetor de dados
   unsigned char    ORIG;              //  endereço de aplicação da origem dos dados
   unsigned char    INFO[1];           //  vetor de dados
   } msg_dado;

typedef struct                         	
   {                                   	// mensagem identificação de pontos
//   unsigned short   TAM;               	//  numero de bytes a seguir         
   unsigned char    COD;               	//  identificador do tipo         
   unsigned char    NRPT;              	//  numero de pontos do tipo acima               
   unsigned char    ORIG;              	//  endereço de aplicação da origem dos dados               
   A_reg          	PONTO[1];           //  vetor de dados                 
   } msg_reg;                         	
                                       	
typedef struct                         	
   {                                   	// mensagem leitura de pontos
//   unsigned short   TAM;               	//  numero de bytes a seguir         
   unsigned char    COD;               	//  identificador do tipo         
   unsigned char    NRPT;              	//  numero de pontos do tipo acima               
   unsigned char    ORIG;              	//  endereço de aplicação da origem dos dados               
   A_ponto          PONTO[1];           //  vetor de dados                 
   } msg_ponto;                         	
                                       	
typedef struct                         	
   {                                   	// mensagem de identificação de host
//   unsigned short   TAM;               	//  numero de bytes a seguir         
   unsigned char    COD;               	//  identificador do tipo         
   unsigned char    VAGO;              	//  sem significado        
   unsigned char    ORIG;              	//  endereço de aplicação do host 
   unsigned long    IPA;               	//  endereço IP do host
   } msg_host;                         	

typedef union
   {                                   // ponteiro para mensagem trocada entre BDTR e protocolos
   msg_dig     far* DIG;               //  ponto digital sem etiqueta de tempo
   msg_ana     far* ANA;               //  ponto analogico sem etiqueta de tempo
   msg_float   far* FLT;               //  ponto analogico em ponto flutuante sem etiqueta de tempo
   msg_bin     far* BIN;               //  sequência de bits sem etiqueta de tempo
   msg_com     far* COM;               //  mensagem de requisição de comando
   msg_dig_tag far* DIGT;              //  ponto digital com etiqueta de tempo
   msg_ana_tag far* ANAT;              //  ponto analogico com etiqueta de tempo
   msg_grupo   far* GRUPO;             //  informação de atualização de grupo
   msg_ack     far* CONF;              //  mensagem de confirmaçao
   msg_sinc    far* SINC;              //  mensagem de comando de sincronização
   msg_req     far* INTR;              //  mensagem de interrogação
   msg_par     far* PAR;               	//  mensagem de alteração de parâmetro
   msg_reg     far* REG;            	//  mensagem de identificação de pontos
   msg_ponto   far* PONTO;             	//  mensagem de leitura de pontos
   msg_host    far* HOST;               //  mensagem de identificação de host
   msg_dado    far* DADO;              //  mensagem de dados genericos
   void        far* GERAL;             //  mensagem generica 
   } msg_bdtr; 

typedef struct                         	
   {                                   	// grupo de pontos: parametros
   unsigned       	ID;                 //  identificador do grupo
   char           	EST[6];             //  estacao remota                        
   unsigned       	MAXC;               //  maxima contagem (time_out)            
   unsigned       	NPTOS;              //  numero de pontos                   
   } grupo_par;

typedef struct                         	
   {                                   	// grupo de pontos: variaveis
   unsigned       	CONT;               //  contador de acertos                   
   unsigned       	ACUM;               //  acumulador de tempo                   
   unsigned       	FALH;               //  acumulador de falhas                  
   } grupo_var;

typedef struct                         	
   {                                   	// banco de dados: parametros
   unsigned  char   ID;              	//  identificador do BDTR (endereço de APLICAÇÃO)
   unsigned  char   TOTUTR;          	//  total de remotas cadastradas                    
   unsigned         TOTPTO;          	//  total de pontos                    
   unsigned         TOTTRF;          	//  total de transferencias
   unsigned         TOTUSU;          	//  total de usuarios cadastradas                    
   unsigned         TOTGRP;          	//  total de grupos                   
   } bd_par;


//                                      PRIMITIVAS 

//                                             FUNÇÕES DEFINIDAS NO MODULO BDTR_REQ.C

int BDTR_INIT_req(unsigned long BDTR);
/*       Inicializa a estrutura de acesso ao Banco de Dados em Tempo Real (BDTR).
         Deve ser chamada pelo processo que se comunica com o BDTR, antes de qualquer função de troca de dados.
         PARAMETROS:
         BDTR: endereco de rede da estacao onde roda o BDTR;
         RETORNO:
            =-1: Erro;
            >=0: soquete de acesso ao BDTR;
*/

int BDTR_TERM_req(int SOCKET_BDTR);
/*       Encerra o acesso ao Banco de Dados em Tempo Real (BDTR).
         Deve ser chamada por programas transientes, para devolver recursos do sistema utilizados.
         PARAMETROS:
         SOCKET_BDTR: soquete de acesso ao BDTR;
         RETORNO:
            = 0: Erro;
            = 1: sucesso;
*/

int BDTR_DATA_req(int SOCKET_BDTR, void far* MSG);
/*       Entrega uma mensagem adquirida pelos protocolos ao Banco de Dados em Tempo Real (BDTR).
         PARAMETROS:
         SOCKET_BDTR: soquete de acesso ao BDTR;
         MSG: ponteiro para mensagem no formato reconhecido pelo BDTR;
         RETORNO:
            = 0: Erro:
                 - soquete não iniciado;
                 - falta de recurso do sistema operacional;
                 - defeito da rede;
            = 1: mensagem entregue ao gerenciador do BDTR.                                                        
*/

int BDTR_ACK_req(int SOCKET_BDTR, unsigned char END, unsigned char CODE, unsigned char ID);
/*       Entrega uma mensagem de confirmação pelos protocolos ao Banco de Dados em Tempo Real (BDTR).
         PARAMETROS:
         SOCKET_BDTR: soquete de acesso ao BDTR;
         END: endereco de aplicação da estação que está enviando a confirmação;
         CODE: indicador do tipo de dados que estão sendo confirmados;
         ID: identificador da confirmação (normalmente um numero sequencial);
         RETORNO:
            = 0: Erro:
                 - soquete não iniciado;
                 - falta de recurso do sistema operacional;
                 - defeito da rede;
            = 1: mensagem entregue ao gerenciador do BDTR.                                                        
*/

int BDTR_GENINT_req(int SOCKET_BDTR, char ORIGEM, int GRUPO);
/*       Entrega uma mensagem de requisição de grupo de dados ao Banco de Dados em Tempo Real (BDTR).
         PARAMETROS:
         SOCKET_BDTR: soquete de acesso ao BDTR;
         ORIGEM: endereco de aplicação da estação que solicita os dados;
         GRUPO: identificador do grupo de dados (0: todos dados);
         RETORNO:
            = 0: Erro:
                 - soquete não iniciado;
                 - falta de recurso do sistema operacional;
                 - defeito da rede;
            = 1: mensagem entregue ao gerenciador do BDTR.                                                        
*/

int BDTR_COMMAND_req(int SOCKET_BDTR, int PONTO, char OPER, char ORIGEM);
/*       Entrega uma mensagem de requisição de comando digital ao Banco de Dados em Tempo Real (BDTR).
         PARAMETROS:
         SOCKET_BDTR: soquete de acesso ao BDTR;
         PONTO: identificador do ponto de comando;
         OPER: operação a ser realizada:
                bits 0..1: define o valor atribuido ao ponto digital:
                           = 0: não permitido;
                           = 1: aberto;
                           = 2: fechado;
                           = 3: não permitido;
                bits 2..4: define a largura do pulso:
                           = 0: utilizar definição cadastrada no Banco de Dados;
                           1..7:largura do pulso;
                bit 7:     = 1: define que valor deve ficar bloqueado;
         ORIGEM: endereço de aplicação do centro que enviou o comando;
         RETORNO:
            = 0: Erro:
                 - soquete não iniciado;
                 - falta de recurso do sistema operacional;
                 - defeito da rede;
            = 1: mensagem entregue ao gerenciador do BDTR.                                                        
*/

int BDTR_SETPOINT_req(int SOCKET_BDTR, int PONTO, char OPER, valor VALOR, char ORIGEM);
/*       Entrega uma mensagem de requisição de comando analógico ao Banco de Dados em Tempo Real (BDTR).
         PARAMETROS:
         SOCKET_BDTR: soquete de acesso ao BDTR;
         PONTO: identificador do ponto de comando;
         OPER: operação a ser realizada:
                bits 0..1: define o valor enviado:
                           = 0: setpoint de valor normalizado;
                           = 1: setpoint de valor escalar;
                           = 2: setpoint de valor em ponto flutuante;
                           = 3: setpoint de valor bitsring 32;
                bit 7:     = 1: define se valor deve ficar bloqueado;
         VALOR: valor a ser atribuido;
         ORIGEM: endereço de aplicação do centro que enviou o comando;
         RETORNO:
            = 0: Erro:
                 - soquete não iniciado;
                 - falta de recurso do sistema operacional;
                 - defeito da rede;
            = 1: mensagem entregue ao gerenciador do BDTR.                                                        
*/

int BDTR_LOADPAR_req(int SOCKET_BDTR, unsigned PONTO, unsigned char TIPO, int VALOR, char ORIGEM);
/*       Entrega uma mensagem de requisição de carga de parametro ao Banco de Dados em Tempo Real (BDTR).
         PARAMETROS:
         SOCKET_BDTR: soquete de acesso ao BDTR;
         PONTO: identificador do ponto;
         TIPO: identifica o parâmetro a ser substituído:
               = 1: escala da conversão de ponto analogico ou inversão de ponto digital;
               = 2: offset da conversão de ponto analogico;
               = 3: casa decimal do ponto analógico;
               = 4: endereço da UTR origem do ponto;
         VALOR: valor do parâmetro;
         ORIGEM: endereço do centro ou UTR que requisitou a troca;
         RETORNO:
            = 0: Erro:
                 - soquete não iniciado;
                 - falta de recurso do sistema operacional;
                 - defeito da rede;
            = 1: mensagem entregue ao gerenciador do BDTR.                                                        
*/

int BDTR_CLOCKSYN_req(int SOCKET_BDTR, char ORIGEM);
/*       Entrega uma mensagem de requisição de sincronismo ao Banco de Dados em Tempo Real (BDTR).
         PARAMETROS:
         SOCKET_BDTR: soquete de acesso ao BDTR;
         ORIGEM: endereco de aplicação da estação que solicita o sincronismo;
         RETORNO:
            = 0: Erro:
                 - soquete não iniciado;
                 - falta de recurso do sistema operacional;
                 - defeito da rede;
            = 1: mensagem entregue ao gerenciador do BDTR.                                                        
*/


//                                      FUNÇÕES DE ACESSO PELO AMBIENTE DO SISTEMA OPERACIONAL (local)

int BDTR_OPEN_loc(bd_par far* BD_PAR);
/*
         Devolve os parametros de dimensionamento do Banco de Dados em Tempo Real (BDTR).
         PARAMETROS:
         BD_PAR: area para armazenar os parametros do Banco de Dados.
         RETORNO:
            = 0: Erro. BDTR não instalado;
            = 1: Sucesso. Estrutura preenchida.                                               
*/

int BDTR_GROUP_OPEN_loc(grupo_par far* GRUPOS);
/*
         Devolve a identificação da estrutura de grupos do Banco de Dados em Tempo Real (BDTR).
         PARAMETROS:
         GRUPOS: area para armazenar os grupos.
         RETORNO:
            > 0: numero de grupos;
            = 0: erro. BDTR não responde;                                               
*/

int BDTR_GROUP_READ_loc(grupo_var far* CONTADORES);
/*
         Devolve os contadores indicadores do processamento de grupos no Banco de Dados em Tempo Real (BDTR).
         PARAMETROS:
         CONTADORES: area para armazenar os contadores.
         RETORNO:
            = 0: Erro. BDTR não instalado;
            = 1: Sucesso. Estrutura preenchida.                                               
*/

int BDTR_GROUP_CLEAR_loc(void);
/*
         Zera os contadores indicadores do processamento de grupos no Banco de Dados em Tempo Real (BDTR).
         PARAMETROS:
         RETORNO:
            = 0: Erro. BDTR não instalado;
            = 1: Sucesso. Estrutura preenchida.                                               
*/

int BDTR_POINT_OPEN_loc(int GRUPO, unsigned INICIO, unsigned NPONTOS, A_reg far* BUFFER); 
/*
         Abre operação de leitura de um conjunto de pontos do Banco de Dados em Tempo Real (BDTR).
         O conjunto é definido pelo grupo a que pertence (ou do banco em geral), a posição inicial da lista do grupo e o numero de pontos.
         Fornece parâmetros dos pontos e suas posições no Banco para utilização posterior.
         PARAMETROS:
         GRUPO: 
            =-2: grupo indefinido. Acesso a toda base de dados. INICIO é a posição do primeiro ponto.
            =-1: grupo indefinido. Acesso a toda base de dados. INICIO é identificador do primeiro ponto.
            = 0: grupo indefinido. Lista de pontos em BUFFER. INICIO sem significado.
            > 0: identificador do grupo. INICIO é posição na lista do grupo de inicio da pesquisa;
         INICIO: identificador ou posição, no grupo ou no banco, do primeiro ponto pesquisado;
         NPONTOS: numero de pontos;
         BUFFER: vetor de pontos onde serão colocados os parâmetros.
         RETORNO: 
            Numero de pontos identificados (preenchidos em BUFFER);
*/

int BDTR_POINT_READ_loc(A_ponto far* BUFFER, int NPONTOS);
/*
         Adquire os valores e indicadores de qualidade de uma lista de pontos do Banco de Dados em Tempo Real (BDTR) local,
         a partir de sua posição adquirida previamente.
         PARAMETROS:
         BUFFER: vetor de pontos com identificadores dos pontos fornecidos.
                 Pontos inexistentes serão indicados como falhados;
         NPONTOS: numero de pontos;
         RETORNO:
            = 0: Erro. BDTR não instalado;
            = 1: Sucesso. Estrutura preenchida.                                               
*/

int BDTR_POINT_WRITE_loc(msg_bdtr BUFFER);
/*
         Dispara operação de escrita de uma lista de pontos no Banco de Dados em Tempo Real (BDTR) local.
         PARAMETROS:
         BUFFER: união de ponteiros para uma area organizada conforme o tipo de dados (único para todos pontos).
                 Pontos não identificados serão desconsiderados;
                 Os valores fornecidos (digitais ou analógicos) serão considerados como CALCULADOS;
         RETORNO:
            = 0: Erro. BDTR não instalado ou falta de recurso;
            = 1: Sucesso.                                            
*/

int BDTR_LIST_OPEN_loc(unsigned char USUARIO); 
/*
         Identifica a lista de um usuario para acessos posteriores.
         PARAMETROS:
         USUARIO: endereço da Aplicação que recebe os eventos 
         RETORNO:
            >= 0: identificador da lista para acessos posteriores;
            = -1: usuario não tem lista cadastrada.
*/

int BDTR_LIST_READ_loc(int L, A_dig_tag far* BUFFER, int NPONTOS, int INICIO); 
/*
         Adquire um numero de eventos de uma lista, a partir de uma posição.
         PARAMETROS:
         L: identificador da lista;
         BUFFER: ponteiro para estrutura a ser preenchida.
         NPONTOS: numero de eventos solicitados;
         INICIO: deslocamento para inicio da leitura. Se >= 0 a partir do inicio da lista. Se < 0, a partir do final.
         RETORNO:
            Número de eventos lidos;
*/

int BDTR_LIST_LENGH_loc(int L); 
/*
         Adquire o numero de eventos armazenados numa lista.
         PARAMETROS:
         L: identificador da lista;
         RETORNO:
            Número de eventos da lista. Se lista invalida retorna -1.
*/

int BDTR_SWITCH_loc(char ATIVO);
/*
         Dispara o procedimento de chaveamento entre BDTRs similares, alterando seu modo de operação (ATIVO ou INATIVO).
         PARAMETROS:
         ATIVO: se TRUE, esta maquina assume a condição de ATIVO. Se FALSE, INATIVO;
         RETORNO:
            = FALSE: Erro. BDTR não instalado;
            = TRUE:  Sucesso. procedimento disparado.                                           
*/

int BDTR_CLEAR_loc(unsigned char ID_LISTA);
/*
         Dispara o procedimento de exclusão dos eventos de uma fila.
         PARAMETROS:
         ID_LISTA: identificador da lista;
         RETORNO:
            = FALSE: Erro. BDTR não instalado;
            = TRUE:  Sucesso. procedimento disparado.
*/

#pragma pack(pop)

#endif

