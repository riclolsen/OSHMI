
#define I104M_LISTENUDPPORT_DEFAULT 8098
#define I104M_WRITEUDPPORT 8099

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
    uint8_t info[255];
} t_msgsup;

#define MSGSUPSQ_SIG 0x64646464
typedef struct {
    uint32_t signature; // 0x64646464
    uint32_t numpoints;
    uint32_t tipo;
    uint32_t prim;
    uint32_t sec;
    uint32_t causa;
    uint32_t taminfo;         // value size for the type (not counting the 4 byte address)
    uint8_t info[2000]; // { 4 bytes unsigned int address, point value (taminfo bytes) } ...  Repeat
} t_msgsupsq;

#define MSGCMD_SIG 0x4b4b4b4b
typedef struct
{
    uint32_t signature; // 0x4b4b4b4b
    uint32_t endereco;
    uint32_t tipo;
	union 
	{
        uint32_t onoff;
		float setpoint;
        int32_t setpoint_i32;
		int16_t setpoint_i16;
	};
    uint32_t sbo;
    uint32_t qu;
    uint32_t utr;
} t_msgcmd;

#pragma pack(push)
#pragma pack(1) // membros das estruturas alinhados em byte

typedef struct {
        uint8_t iq;     // informaçao com qualificador
        uint16_t ms;    // milisegundos
        uint8_t min;    // minuto
        uint8_t hora;   // hora
        uint8_t dia;    // dia
        uint8_t mes;
        uint8_t ano;
} digital_w_time7_seq;


typedef struct {
    uint8_t iq; // informaçao com qualificador
} digital_notime_seq;


typedef struct {
        int16_t sva;         // valor analogico
        uint8_t qds; // qualificador do ponto
} analogico_seq;

typedef struct {
        uint8_t vti;        // valor de tap
        uint8_t qds; // qualificador
} step_seq;

typedef struct {
        float fr;      		// valor em ponto flutuante
        uint8_t qds; // qualificador do ponto
} flutuante_seq;

typedef struct {
    float fr;          // valor em ponto flutuante
    uint8_t qds;       // qualificador do ponto
    uint16_t ms;       // milisegundos
    uint8_t min;       // minuto
    uint8_t hora;      // hora
    uint8_t dia;       // dia
    uint8_t mes;       // mes
    uint8_t ano;       // ano
} flutuante_w_time7_seq;

typedef struct {
    uint32_t bcr;       // valor binary counter reading
    uint8_t qds;        // qualificador do ponto
} integrated_seq;

#pragma pack(pop)
