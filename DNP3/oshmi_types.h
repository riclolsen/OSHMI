
#define I104M_LISTENUDPPORT 8098
#define I104M_WRITEUDPPORT 8099

#define MSGKA_SIG 0x5a5a5a5a

#define MSGSUP_SIG 0x53535353
typedef struct {
	unsigned int signature;  // 0x53535353
	unsigned int endereco;
	unsigned int tipo;
	unsigned int prim;
	unsigned int sec;
	unsigned int causa;
	unsigned int taminfo;
	unsigned char info[255];
} t_msgsup;

#define MSGSUPSQ_SIG 0x64646464
typedef struct {
	unsigned int signature;  // 0x64646464
	unsigned int numpoints;
	unsigned int tipo;
	unsigned int prim;
	unsigned int sec;
	unsigned int causa;
	unsigned int taminfo; // value size for the type (not counting the 4 byte address)
	unsigned char info[2000]; // { 4 bytes unsigned int address, point value (taminfo bytes) } ...  Repeat
} t_msgsupsq;

#define MSGCMD_SIG 0x4b4b4b4b
typedef struct
{
	unsigned int signature; // 0x4b4b4b4b
	unsigned int endereco;
	unsigned int tipo;
	union 
	{
		unsigned int onoff;
		float setpoint;
	};
	unsigned int sbo;
	unsigned int qu;
	unsigned int utr;
} t_msgcmd;

#pragma pack(push)
#pragma pack(1) // membros das estruturas alinhados em byte

typedef struct {
        unsigned short nponto; // numero do ponto
        unsigned char nponto3; // 3 byte do numero do ponto
        unsigned char iq;      // informaçao com qualificador
        unsigned short ms;     // milisegundos
        unsigned char min;     // minuto
        unsigned char hora;    // hora
        unsigned char dia;     // dia
        unsigned char mes;
        unsigned char ano;
} digital_w_time7;

typedef struct {
        unsigned char iq;     // informaçao com qualificador
        unsigned short ms;    // milisegundos
        unsigned char min;    // minuto
        unsigned char hora;   // hora
        unsigned char dia;    // dia
        unsigned char mes;
        unsigned char ano;
} digital_w_time7_seq;


typedef struct {
	unsigned char iq;      // informaçao com qualificador
} digital_notime_seq;

typedef struct {
        unsigned short nponto; // numero do ponto
        unsigned char nponto3; // 3 byte do numero do ponto
        short sva;             // valor analogico
        unsigned char qds;     // qualificador do ponto
} analogico;

typedef struct {
        short sva;         // valor analogico
        unsigned char qds; // qualificador do ponto
} analogico_seq;

typedef struct {
        unsigned short nponto; // numero do ponto
        unsigned char nponto3; // 3 byte do numero do ponto
        unsigned char vti;     // valor de tap
        unsigned char qds;     // qualificador
} step;

typedef struct {
        unsigned char vti;   // valor de tap
        unsigned char qds;   // qualificador
} step_seq;

typedef struct {
        unsigned short nponto; // numero do ponto
        unsigned char nponto3; // 3 byte do numero do ponto
        float fr;      	       // valor em ponto flutuante
        unsigned char qds;     // qualificador do ponto
} flutuante;

typedef struct {
        float fr;      		// valor em ponto flutuante
        unsigned char qds; 	// qualificador do ponto
} flutuante_seq;
   
#pragma pack(pop)
