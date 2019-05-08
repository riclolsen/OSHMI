/* This software implements a Modbus driver for OSHMI.
 * Copyright - 2015-2017 - Ricardo Lastra Olsen
 * Derived from libmodbus project:
 * Copyright © 2001-2010 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <conio.h>
#include <vector>
#include <string>
#include <iostream>

#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "../src/modbus.h"
#include "inifile.h"
#include "oshmi_types.h"

//#define MODBUSINI "c:\\oshmi\\conf\\modbus_queue.ini"
#define MODBUSINI "c:\\oshmi\\conf\\modbus_queue.ini"
#define OSHMIINI "c:\\oshmi\\conf\\hmi.ini"
#define MAX_RTU 128
#define MAX_INPREAD 500 // max input register read by type
#define KEEP_ALIVE_TIME 10

#define PKTDIG_MAXPOINTS       250
#define PKTANA_MAXPOINTS       150    
#define PKTFLT_MAXPOINTS       70

// platform detection
#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS

#include <winsock2.h>
#include <Ws2tcpip.h>

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#endif

#if PLATFORM == PLATFORM_WINDOWS
#pragma comment( lib, "wsock32.lib" )
#endif

;
using namespace std;

#define LOOP             1
#define SERVER_ID       17
#define ADDRESS_START    0
#define ADDRESS_END     99

// structures for modbus reading queue
struct mb_read
{
	int mbaddress;
	int numreg;
	int i104addr;
};

struct mb_rturead
{
	modbus_t * ctx;
	string ip;
	int port;
	int delay;
	vector <mb_read> readhr;
	vector <mb_read> readhr_float; // read consecutive 16 bit values as floats (assume 1st byte=exp, 2nd=MSB mant, 3rd=middle mant, 4=LSB mant so intel 2,3,0,1 order) 
	vector <mb_read> readir;
	vector <mb_read> readis;
	vector <mb_read> readcs;
};

vector <mb_rturead> mb_queue;

class OSHMIHandler
{

public:
	int shandle;
	sockaddr_in address;
	sockaddr_in saddress; // socket address
	sockaddr_in saddress_red_driver; // socket address for redundant driver (different port from hmi)
	bool redundant_hmi = false; // there is redundant hmi?
	string IPAddrRed;

	void SendKeepAlive()
	{
		if (redundant_hmi)
		{
			int kasig = MSGKA_SIG;
			int sent_bytes =
				sendto(shandle,
				(const char*)&kasig,
				sizeof(kasig),
				0,
				(sockaddr*)&saddress_red_driver,
				sizeof(sockaddr_in));

			if (sent_bytes != sizeof(kasig))
				printf("failed to send packet\n");
		}
	}

	void SendOSHMI(void * pmsg, int packet_size)
	{
		if (packet_size == 0)
			return;
		int sent_bytes =
			sendto(shandle,
			(const char*)pmsg,
			packet_size,
			0,
			(sockaddr*)&saddress,
			sizeof(sockaddr_in));

		if (sent_bytes != packet_size)
			printf("failed to send packet\n");
		// else
		//	printf("--->>> OSHMI ------>>>\n");
	}

	void SetRedundantHMIAddress(const char * ipaddr)
	{
		if ( strcmp(ipaddr, "") )
		{
			saddress_red_driver.sin_family = AF_INET;
			saddress_red_driver.sin_addr.s_addr = inet_addr(ipaddr);
			saddress_red_driver.sin_port = htons((unsigned short)I104M_LISTENUDPPORT);
			redundant_hmi = true;
		}
	}

	bool Initialize()
	{

		CIniFile hmi_iniFile(OSHMIINI);
		hmi_iniFile.ReadFile();
		IPAddrRed = hmi_iniFile.GetValue("REDUNDANCY", "OTHER_HMI_IP"); // ip address of redundant hmi
		SetRedundantHMIAddress(IPAddrRed.c_str());

#if PLATFORM == PLATFORM_WINDOWS
		WSADATA WsaData;
		///return
			WSAStartup(MAKEWORD(2, 2), &WsaData);
		//	== NO_ERROR;
#endif
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons((unsigned short)I104M_LISTENUDPPORT);

		shandle = socket(AF_INET,
			SOCK_DGRAM,
			IPPROTO_UDP);

		if (shandle <= 0)
		{
			printf("failed to create socket\n");
			exit(21);
		}

		if (bind(shandle,
			(const sockaddr*)&address,
			sizeof(sockaddr_in)) < 0)
		{
			printf("failed to bind socket\n");
			exit(22);
		}

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

		int nonBlocking = 1;
		if (fcntl(handle,
			F_SETFL,
			O_NONBLOCK,
			nonBlocking) == -1)
		{
			printf("failed to set non-blocking\n");
			exit(23);
		}

#elif PLATFORM == PLATFORM_WINDOWS

		DWORD nonBlocking = 1;
		if (ioctlsocket(shandle,
			FIONBIO,
			&nonBlocking) != 0)
		{
			printf("failed to set non-blocking\n");
			exit(23);
		}

#endif

		saddress.sin_family = AF_INET;
		saddress.sin_addr.s_addr = inet_addr("127.0.0.1");
		saddress.sin_port = htons((unsigned short)I104M_WRITEUDPPORT);

		printf("Starting inactive mode...\n");
		if (redundant_hmi) // if redundacy configured, wait to be active (no messages for some time)
			waittobe_active(shandle);

		printf("Entering active mode...\n");
		return true;
	}

	void ShutdownSockets()
	{
#if PLATFORM == PLATFORM_WINDOWS
		WSACleanup();
#endif
	}

	// wait while receiving keep alive messages
	void waittobe_active(int shandle)
	{
		int seconds_wait = (int)(KEEP_ALIVE_TIME * 3.5);
		time_t now;
		time(&now);
		time_t timeant = now;

		std::cout << "x - exits program" << std::endl;

		while (1)
		{
			unsigned char packet_data[1500];
			unsigned int max_packet_size =
				sizeof(packet_data);

#if PLATFORM == PLATFORM_WINDOWS
			typedef int socklen_t;
#endif
			sockaddr_in from;
			socklen_t fromLength = sizeof(from);

			int bytes = recvfrom(shandle,
				(char*)packet_data,
				max_packet_size,
				0,
				(sockaddr*)&from,
				&fromLength);

			if (bytes > 0)
			{
				unsigned int from_address =
					ntohl(from.sin_addr.s_addr);
				unsigned int from_port =
					ntohs(from.sin_port);

				t_msgcmd * pmsg = (t_msgcmd *)packet_data;

				if (pmsg->signature == MSGKA_SIG)
				{ // message from other active driver, keep waiting
					timeant = now;
				}
			}

			time(&now);

			if (difftime(now, timeant) > seconds_wait)
			{ // wait time for messages expired, return to activate 
				return;
			}

			char cmd = 0;
			if (_kbhit())
			{
				cmd = _getch();
			}

			switch (cmd)
			{
			case('x') :
				exit(0);
			}
		}
	}

	int receiveCommands(int * asdu, int * address, int * val, int * slave)
	{
		unsigned char packet_data[1500];
		unsigned int max_packet_size =
			sizeof(packet_data);

#if PLATFORM == PLATFORM_WINDOWS
		typedef int socklen_t;
#endif
		sockaddr_in from;
		socklen_t fromLength = sizeof(from);

		int bytes = recvfrom(shandle,
			(char*)packet_data,
			max_packet_size,
			0,
			(sockaddr*)&from,
			&fromLength);

		if (bytes > 0)
		{
			unsigned long from_address = from.sin_addr.s_addr;
			unsigned int from_port =
				ntohs(from.sin_port);

			unsigned long local = inet_addr("127.0.0.1");
			unsigned long dual = inet_addr("127.0.0.1");
			if (redundant_hmi)
				dual = inet_addr(IPAddrRed.c_str());

			// test origin of packet: it must be local or from redundant address
			if (from_address == local || from_address == dual)
			{
				t_msgcmd * pmsg = (t_msgcmd *)packet_data;

				if (pmsg->signature == MSGCMD_SIG) // test if command message
				{
					switch (pmsg->tipo)
					{
					case 45: // digital single command
					    {
						// returns confirmation of asdu upwards
						t_msgsup msg;
						msg.causa = 0x00;
						msg.endereco = pmsg->endereco;
						msg.tipo = pmsg->tipo;
						msg.prim = 1;
						msg.sec = pmsg->utr;
						msg.signature = MSGSUP_SIG;
						msg.taminfo = 1;
						msg.info[0] = pmsg->onoff;
						int packet_size = sizeof(int) * 7 + 1;
						SendOSHMI(&msg, packet_size);
						
						*asdu = pmsg->tipo;
						*address = pmsg->endereco;
						*val = pmsg->onoff;
						*slave = pmsg->utr;
						return 1;
					    }
					break;
					case 46: // digital double command
					{
						// returns confirmation of asdu upwards
						t_msgsup msg;
						msg.causa = 0x00;
						msg.endereco = pmsg->endereco;
						msg.tipo = pmsg->tipo;
						msg.prim = 1;
						msg.sec = pmsg->utr;
						msg.signature = MSGSUP_SIG;
						msg.taminfo = 1;
						msg.info[0] = pmsg->onoff;
						int packet_size = sizeof(int) * 7 + 1;
						SendOSHMI(&msg, packet_size);

						*asdu = pmsg->tipo;
						*address = pmsg->endereco;
						*val = pmsg->onoff;
						*slave = pmsg->utr;
						return 1;
					}
					break;
					case 48: // setpoint normalized command
					{
						// returns confirmation of asdu upwards
						t_msgsup msg;
						msg.causa = 0x00;
						msg.endereco = pmsg->endereco;
						msg.tipo = pmsg->tipo;
						msg.prim = 1;
						msg.sec = pmsg->utr;
						msg.signature = MSGSUP_SIG;
						msg.taminfo = sizeof(short);
						*(short *)msg.info = (short)pmsg->setpoint;
						int packet_size = sizeof(int) * 7 + sizeof(short);
						SendOSHMI(&msg, packet_size);

						*asdu = pmsg->tipo;
						*address = pmsg->endereco;
						*val = pmsg->setpoint;
						*slave = pmsg->utr;
						return 1;
					}
					break;
					case 49: // setpoint scaled command
					    {
						// returns confirmation of asdu upwards
						t_msgsup msg;
						msg.causa = 0x00;
						msg.endereco = pmsg->endereco;
						msg.tipo = pmsg->tipo;
						msg.prim = 1;
						msg.sec = pmsg->utr;
						msg.signature = MSGSUP_SIG;
						msg.taminfo = sizeof(short);
						*(short *)msg.info = (short)pmsg->setpoint;
						int packet_size = sizeof(int) * 7 + sizeof(short);
						SendOSHMI(&msg, packet_size);

						*asdu = pmsg->tipo;
						*address = pmsg->endereco;
						*val = pmsg->setpoint;
						*slave = pmsg->utr;
						return 1;
					    }
					break;
					} // switch
				}
			}
		}
		return 0; // no command received
	}
};

// send a modbus command according to data received  from OSHMI
void mb_command(int asdu, int address, int val, int slave)
{
	// slave 1 means the first from queue of modbus.ini (RTU_1)
	if (slave <= 0) // default slave is the first
		slave = 1;
	switch (asdu)
		{
		case 45:
			modbus_write_bit(mb_queue[slave - 1].ctx, address, val);
			printf("WRITE modbus_write_bit RTU:%d ADDR=%d VAL=%d\n", slave - 1, address, val);
			break;
		case 46:
			uint8_t src[2];
			src[0] = (val & 0x01) ? 1 : 0;
			src[1] = (val & 0x02) ? 1 : 0;
			modbus_write_bits(mb_queue[slave - 1].ctx, address, 2, src);
			printf("WRITE modbus_write_bit RTU:%d ADDR=%d VAL=%d\n", slave - 1, address, val);
			break;
		case 48:
		case 49:
			modbus_write_register(mb_queue[slave - 1].ctx, address, val);
			printf("WRITE modbus_write_register RTU:%d ADDR=%d VAL=%d\n", slave - 1, address, val);
			break;
		}
}

/* At each loop, the program works in the range ADDRESS_START to
 * ADDRESS_END then ADDRESS_START + 1 to ADDRESS_END and so on.
 */
int main(void)
{
    modbus_t *ctx;
    int rc;

    int nb_fail;
    int nb_loop;
    int addr;
    int nb;
    uint8_t *tab_rq_bits;
    uint8_t *tab_rp_bits;
    uint16_t *tab_rq_registers;
    uint16_t *tab_rw_rq_registers;
    uint16_t *tab_rp_registers;

	cout << "OSHMI Modbus Driver v.1.01" << endl;

	OSHMIHandler oh;
	oh.Initialize();

	CIniFile mb_iniFile(MODBUSINI);
	if ( mb_iniFile.ReadFile() )
	for (int i = 0; i < MAX_RTU; i++)
	{
		string rtun = (string)"RTU_" + std::to_string(i+1);
		if (mb_iniFile.FindKey( rtun ) == CIniFile::noID )
		  break;
		mb_rturead rtu;
		rtu.ip = mb_iniFile.GetValue(rtun, "IP");
		rtu.port = mb_iniFile.GetValueI(rtun, "PORT");
		rtu.delay = mb_iniFile.GetValueI(rtun, "DELAY");
		rtu.ctx = modbus_new_tcp(rtu.ip.c_str(), rtu.port);
		modbus_set_debug(rtu.ctx, TRUE);
		modbus_connect(rtu.ctx);

		for (int j = 0; j < MAX_INPREAD; j++)
		{
			string key = (string)"READHR_" + std::to_string(j+1);
			string aCnCp = mb_iniFile.GetValue(rtun, key);
			if (aCnCp == "")
				break;
			mb_read mbr;
			mbr.i104addr = 0;
			sscanf(aCnCp.c_str(), "%d %d %d", &mbr.mbaddress, &mbr.numreg, &mbr.i104addr);
			if (mbr.i104addr == 0)
				mbr.i104addr = mbr.mbaddress;
			rtu.readhr.push_back(mbr);
		}
		for (int j = 0; j < MAX_INPREAD; j++)
		{
			string key = (string)"READHR_FLOAT_" + std::to_string(j + 1);
			string aCnCp = mb_iniFile.GetValue(rtun, key);
			if (aCnCp == "")
				break;
			mb_read mbr;
			mbr.i104addr = 0;
			sscanf(aCnCp.c_str(), "%d %d %d", &mbr.mbaddress, &mbr.numreg, &mbr.i104addr);
			if (mbr.i104addr == 0)
				mbr.i104addr = mbr.mbaddress;
			rtu.readhr_float.push_back(mbr);
		}
		for (int j = 0; j < MAX_INPREAD; j++)
		{
			string key = (string)"READIR_" + std::to_string(j+1);
			string aCnCp = mb_iniFile.GetValue(rtun, key);
			if (aCnCp == "")
				break;
			mb_read mbr;
			mbr.i104addr = 0;
			sscanf(aCnCp.c_str(), "%d %d %d", &mbr.mbaddress, &mbr.numreg, &mbr.i104addr);
			if (mbr.i104addr == 0)
				mbr.i104addr = mbr.mbaddress;
			rtu.readir.push_back(mbr);
		}
		for (int j = 0; j < MAX_INPREAD; j++)
		{
			string key = (string)"READIS_" + std::to_string(j+1);
			string aCnCp = mb_iniFile.GetValue(rtun, key);
			if (aCnCp == "")
				break;
			mb_read mbr;
			mbr.i104addr = 0;
			sscanf(aCnCp.c_str(), "%d %d %d", &mbr.mbaddress, &mbr.numreg, &mbr.i104addr);
			if (mbr.i104addr == 0)
				mbr.i104addr = mbr.mbaddress;
			rtu.readis.push_back(mbr);
		}
		for (int j = 0; j < MAX_INPREAD; j++)
		{
			string key = (string)"READCS_" + std::to_string(j+1);
			string aCnCp = mb_iniFile.GetValue(rtun, key);
			if (aCnCp == "")
				break;
			mb_read mbr;
			mbr.i104addr = 0;
			sscanf(aCnCp.c_str(), "%d %d %d", &mbr.mbaddress, &mbr.numreg, &mbr.i104addr);
			if (mbr.i104addr == 0)
				mbr.i104addr = mbr.mbaddress;
			rtu.readcs.push_back(mbr);
		}

		mb_queue.push_back(rtu);
	}	

	int max_pointspkt;
	int count;
	int packet_size;

	time_t timeant = 0;

	int asdu, address, val, slave;

	while (!_kbhit())
	for (unsigned  i = 0; i < mb_queue.size(); i++)
	{
		if (oh.redundant_hmi)
		{
			time_t now;
			time(&now);
			if (difftime(now, timeant) > 5)
			{
				timeant = now;
				oh.SendKeepAlive();
				// printf("ka\n");
			}
		}

		// verify if there is commands to execute from OSHMI
		if (oh.receiveCommands(&asdu, &address, &val, &slave))
        	mb_command(asdu, address, val, slave);

		t_msgsupsq msg;
		msg.signature = MSGSUPSQ_SIG;
		msg.prim = 1;
		msg.sec = i + 1; // RTU order or end of IP
		msg.causa = 20;
		msg.tipo = 9;
		msg.taminfo = sizeof(analogico_seq); // value size for the type (not counting the 4 byte address)

		for (unsigned j = 0; j < mb_queue[i].readhr.size(); j++)
		{
			// verify if there is commands to execute from OSHMI
			if (oh.receiveCommands(&asdu, &address, &val, &slave))
				mb_command(asdu, address, val, slave);

			max_pointspkt = PKTANA_MAXPOINTS;

			uint16_t *tabreg = (uint16_t *)malloc(mb_queue[i].readhr[j].numreg * sizeof(uint16_t));
			memset(tabreg, 0, mb_queue[i].readhr[j].numreg * sizeof(uint16_t));
			rc = modbus_read_registers(mb_queue[i].ctx, mb_queue[i].readhr[j].mbaddress, mb_queue[i].readhr[j].numreg, tabreg);

			if (rc != mb_queue[i].readhr[j].numreg) {
				printf("ERROR modbus_read_registers (%d)\n", rc);
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readhr[j].numreg; k++)
			{
				printf("RTU: %s, Input %d = %d\n", mb_queue[i].ip.c_str(), mb_queue[i].readhr[j].mbaddress + k, (short)tabreg[k]);

				unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(analogico_seq)));
				*paddr = mb_queue[i].readhr[j].i104addr + k; // point number

				// value and quality
				analogico_seq * obj = (analogico_seq *)(paddr + 1);
				obj->qds = 0;
				obj->sva = (short)tabreg[k];

				count++;
				if (!((count + 1) % max_pointspkt)) // if next from packet send now (se a proxima eh do proximo pacote, manda agora)
				{
					msg.numpoints = count % max_pointspkt;
					packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(analogico_seq));
					oh.SendOSHMI(&msg, packet_size);
				}
			}

			msg.numpoints = count % max_pointspkt;
			packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(analogico_seq));
			oh.SendOSHMI(&msg, packet_size);

			free(tabreg);
			Sleep(mb_queue[i].delay);
		}

		for (unsigned j = 0; j < mb_queue[i].readir.size(); j++)
		{
			// verify if there is commands to execute from OSHMI
			if (oh.receiveCommands(&asdu, &address, &val, &slave))
				mb_command(asdu, address, val, slave);

			max_pointspkt = PKTANA_MAXPOINTS;

			uint16_t *tabreg = (uint16_t *)malloc(mb_queue[i].readir[j].numreg * sizeof(uint16_t));
			memset(tabreg, 0, mb_queue[i].readir[j].numreg * sizeof(uint16_t));
			rc = modbus_read_input_registers(mb_queue[i].ctx, mb_queue[i].readir[j].mbaddress, mb_queue[i].readir[j].numreg, tabreg);

			if (rc != mb_queue[i].readir[j].numreg) {
				printf("ERROR modbus_read_input_registers (%d)\n", rc);
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readir[j].numreg; k++)
			{
				printf("RTU: %s, Input %d = %d\n", mb_queue[i].ip.c_str(), mb_queue[i].readir[j].mbaddress + k, (short)tabreg[k]);

				unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(analogico_seq)));
				*paddr = mb_queue[i].readir[j].i104addr + k; // point number

				// value and quality
				analogico_seq * obj = (analogico_seq *)(paddr + 1);
				obj->qds = 0;
				obj->sva = (short)tabreg[k];

				count++;
				if (!((count + 1) % max_pointspkt)) // if next from packet send now (se a proxima eh do proximo pacote, manda agora)
				{
					msg.numpoints = count % max_pointspkt;
					packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(analogico_seq));
					oh.SendOSHMI(&msg, packet_size);
				}
			}

			msg.numpoints = count % max_pointspkt;
			packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(analogico_seq));
			oh.SendOSHMI(&msg, packet_size);

			free(tabreg);
			Sleep(mb_queue[i].delay);
		}
		
		msg.signature = MSGSUPSQ_SIG;
		msg.prim = 1;
		msg.sec = i + 1; // RTU order or end of IP
		msg.causa = 20;
		msg.tipo = 13;
		msg.taminfo = sizeof(flutuante_seq); // value size for the type (not counting the 4 byte address)

		for (unsigned j = 0; j < mb_queue[i].readhr_float.size(); j++) // each 2 16 bit holding registers as a float
		{
			// verify if there is commands to execute from OSHMI
			if (oh.receiveCommands(&asdu, &address, &val, &slave))
				mb_command(asdu, address, val, slave);

			max_pointspkt = PKTFLT_MAXPOINTS;

			uint16_t *tabreg = (uint16_t *)malloc(2 * mb_queue[i].readhr_float[j].numreg * sizeof(uint16_t));
			memset(tabreg, 0, 2 * mb_queue[i].readhr_float[j].numreg * sizeof(uint16_t));
			rc = modbus_read_registers(mb_queue[i].ctx, mb_queue[i].readhr_float[j].mbaddress, mb_queue[i].readhr_float[j].numreg * 2, tabreg);

			if (rc != mb_queue[i].readhr_float[j].numreg * 2) {
				printf("ERROR modbus_read_registers (%d)\n", rc);
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readhr_float[j].numreg * 2; k += 2)
			{
				union { float f; unsigned short bt[2]; } u;

				u.bt[1] = tabreg[k];
				u.bt[0] = tabreg[k + 1];

				printf("RTU: %s, Input %d = %f\n", mb_queue[i].ip.c_str(), mb_queue[i].readhr_float[j].mbaddress + k, u.f);

				unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(flutuante_seq)));
				*paddr = mb_queue[i].readhr_float[j].i104addr + k / 2; // point number

				// value and quality
				flutuante_seq * obj = (flutuante_seq *)(paddr + 1);
				obj->qds = 0;
				obj->fr = u.f;

				count++;
				if (!((count + 1) % max_pointspkt)) // if next from packet send now (se a proxima eh do proximo pacote, manda agora)
				{
					msg.numpoints = count % max_pointspkt;
					packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(flutuante_seq));
					oh.SendOSHMI(&msg, packet_size);
				}
			}

			msg.numpoints = count % max_pointspkt;
			packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(flutuante_seq));
			oh.SendOSHMI(&msg, packet_size);

			free(tabreg);
			Sleep(mb_queue[i].delay);
		}

		msg.signature = MSGSUPSQ_SIG;
		msg.prim = 1;
		msg.sec = i + 1; // RTU order or end of IP
		msg.causa = 20;
		msg.tipo = 1;
		msg.taminfo = sizeof(digital_notime_seq); // value size for the type (not counting the 4 byte address)

		for (unsigned j = 0; j < mb_queue[i].readcs.size(); j++)
		{
			// verify if there is commands to execute from OSHMI
			if (oh.receiveCommands(&asdu, &address, &val, &slave))
				mb_command(asdu, address, val, slave);

			max_pointspkt = PKTDIG_MAXPOINTS;

			uint8_t *tabreg = (uint8_t *)malloc(mb_queue[i].readcs[j].numreg * sizeof(uint8_t));
			memset(tabreg, 0, mb_queue[i].readcs[j].numreg * sizeof(uint8_t));
			rc = modbus_read_bits(mb_queue[i].ctx, mb_queue[i].readcs[j].mbaddress, mb_queue[i].readcs[j].numreg, tabreg);

			if (rc != mb_queue[i].readcs[j].numreg) {
				printf("ERROR modbus_read_bits (%d)\n", rc);
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readcs[j].numreg; k++)
			{
				printf("RTU: %s, Input %d = %d\n", mb_queue[i].ip.c_str(), mb_queue[i].readcs[j].mbaddress + k, tabreg[k]);

				unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(digital_notime_seq)));
				*paddr = mb_queue[i].readcs[j].i104addr + k; // point number

				// value and quality
				digital_notime_seq * obj = (digital_notime_seq *)(paddr + 1);
				obj->iq = tabreg[k] ? 1 : 0;

				count++;
				if (!((count + 1) % max_pointspkt)) // if next from packet send now (se a proxima eh do proximo pacote, manda agora)
				{
					msg.numpoints = count % max_pointspkt;
					packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_notime_seq));
					oh.SendOSHMI(&msg, packet_size);
				}
			}

			msg.numpoints = count % max_pointspkt;
			packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_notime_seq));
			oh.SendOSHMI(&msg, packet_size);

			free(tabreg);
			Sleep(mb_queue[i].delay);
		}

		for (unsigned j = 0; j < mb_queue[i].readis.size(); j++)
		{
			// verify if there is commands to execute from OSHMI
			if (oh.receiveCommands(&asdu, &address, &val, &slave))
				mb_command(asdu, address, val, slave);

			max_pointspkt = PKTDIG_MAXPOINTS;

			uint8_t *tabreg = (uint8_t *)malloc(mb_queue[i].readis[j].numreg * sizeof(uint8_t));
			memset(tabreg, 0, mb_queue[i].readis[j].numreg * sizeof(uint8_t));
			rc = modbus_read_input_bits(mb_queue[i].ctx, mb_queue[i].readis[j].mbaddress, mb_queue[i].readis[j].numreg, tabreg);

			if (rc != mb_queue[i].readis[j].numreg) {
				printf("ERROR modbus_read_input_status (%d)\n", rc);
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readis[j].numreg; k++)
			{
				printf("RTU: %s, Input %d = %d\n", mb_queue[i].ip.c_str(), mb_queue[i].readis[j].mbaddress + k, tabreg[k]);

				unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(digital_notime_seq)));
				*paddr = mb_queue[i].readis[j].i104addr + k; // point number

				// value and quality
				digital_notime_seq * obj = (digital_notime_seq *)(paddr + 1);
				obj->iq = tabreg[k] ? 1 : 0;

				count++;
				if (!((count + 1) % max_pointspkt)) // if next from packet send now (se a proxima eh do proximo pacote, manda agora)
				{
					msg.numpoints = count % max_pointspkt;
					packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_notime_seq));
					oh.SendOSHMI(&msg, packet_size);
				}
			}

			msg.numpoints = count % max_pointspkt;
			packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_notime_seq));
			oh.SendOSHMI(&msg, packet_size);

			free(tabreg);
			Sleep(mb_queue[i].delay);
		}
	}

	for (unsigned i = 0; i < mb_queue.size(); i++)
	{
		modbus_close(mb_queue[i].ctx);
		modbus_free(mb_queue[i].ctx);
	}


    // RTU 
/*
    ctx = modbus_new_rtu("/dev/ttyUSB0", 19200, 'N', 8, 1);
    modbus_set_slave(ctx, SERVER_ID);
*/
	/*

    // TCP 
    ctx = modbus_new_tcp("127.0.0.1", 502);
    modbus_set_debug(ctx, TRUE);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }
	

	uint16_t inpreg;
	while (!_kbhit())
	{
		rc = modbus_read_registers(ctx, 30001, 1, &inpreg);
		if (rc != 1) {
			printf("ERROR modbus_read_registers single (%d)\n", rc);
			// printf("Address = %d\n", addr);
			// nb_fail++;
			modbus_flush(ctx);
			modbus_connect(ctx);
		}
		else {
			printf("Input 30001=%d\n", inpreg);
		}
		Sleep(400);
	}

	/*
	// Allocate and initialize the different memory spaces
	nb = ADDRESS_END - ADDRESS_START;

	tab_rq_bits = (uint8_t *) malloc(nb * sizeof(uint8_t));
	memset(tab_rq_bits, 0, nb * sizeof(uint8_t));

	tab_rp_bits = (uint8_t *) malloc(nb * sizeof(uint8_t));
	memset(tab_rp_bits, 0, nb * sizeof(uint8_t));

	tab_rq_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
	memset(tab_rq_registers, 0, nb * sizeof(uint16_t));

	tab_rp_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
	memset(tab_rp_registers, 0, nb * sizeof(uint16_t));

	tab_rw_rq_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
	memset(tab_rw_rq_registers, 0, nb * sizeof(uint16_t));

	nb_loop = nb_fail = 0;
	while (nb_loop++ < LOOP) {
        for (addr = ADDRESS_START; addr <= ADDRESS_END; addr++) {
            int i;

            // Random numbers (short) 
            for (i=0; i<nb; i++) {
                tab_rq_registers[i] = (uint16_t) (65535.0*rand() / (RAND_MAX + 1.0));
                tab_rw_rq_registers[i] = ~tab_rq_registers[i];
                tab_rq_bits[i] = tab_rq_registers[i] % 2;
            }
            nb = ADDRESS_END - addr;

            // WRITE BIT 
            rc = modbus_write_bit(ctx, addr, tab_rq_bits[0]);
            if (rc != 1) {
                printf("ERROR modbus_write_bit (%d)\n", rc);
                printf("Address = %d, value = %d\n", addr, tab_rq_bits[0]);
                nb_fail++;
            } else {
                rc = modbus_read_bits(ctx, addr, 1, tab_rp_bits);
                if (rc != 1 || tab_rq_bits[0] != tab_rp_bits[0]) {
                    printf("ERROR modbus_read_bits single (%d)\n", rc);
                    printf("address = %d\n", addr);
                    nb_fail++;
                }
            }

            // MULTIPLE BITS 
            rc = modbus_write_bits(ctx, addr, nb, tab_rq_bits);
            if (rc != nb) {
                printf("ERROR modbus_write_bits (%d)\n", rc);
                printf("Address = %d, nb = %d\n", addr, nb);
                nb_fail++;
            } else {
                rc = modbus_read_bits(ctx, addr, nb, tab_rp_bits);
                if (rc != nb) {
                    printf("ERROR modbus_read_bits\n");
                    printf("Address = %d, nb = %d\n", addr, nb);
                    nb_fail++;
                } else {
                    for (i=0; i<nb; i++) {
                        if (tab_rp_bits[i] != tab_rq_bits[i]) {
                            printf("ERROR modbus_read_bits\n");
                            printf("Address = %d, value %d (0x%X) != %d (0x%X)\n",
                                   addr, tab_rq_bits[i], tab_rq_bits[i],
                                   tab_rp_bits[i], tab_rp_bits[i]);
                            nb_fail++;
                        }
                    }
                }
            }

            // SINGLE REGISTER 
            rc = modbus_write_register(ctx, addr, tab_rq_registers[0]);
            if (rc != 1) {
                printf("ERROR modbus_write_register (%d)\n", rc);
                printf("Address = %d, value = %d (0x%X)\n",
                       addr, tab_rq_registers[0], tab_rq_registers[0]);
                nb_fail++;
            } else {
                rc = modbus_read_registers(ctx, addr, 1, tab_rp_registers);
                if (rc != 1) {
                    printf("ERROR modbus_read_registers single (%d)\n", rc);
                    printf("Address = %d\n", addr);
                    nb_fail++;
                } else {
                    if (tab_rq_registers[0] != tab_rp_registers[0]) {
                        printf("ERROR modbus_read_registers single\n");
                        printf("Address = %d, value = %d (0x%X) != %d (0x%X)\n",
                               addr, tab_rq_registers[0], tab_rq_registers[0],
                               tab_rp_registers[0], tab_rp_registers[0]);
                        nb_fail++;
                    }
                }
            }

            // MULTIPLE REGISTERS 
            rc = modbus_write_registers(ctx, addr, nb, tab_rq_registers);
            if (rc != nb) {
                printf("ERROR modbus_write_registers (%d)\n", rc);
                printf("Address = %d, nb = %d\n", addr, nb);
                nb_fail++;
            } else {
                rc = modbus_read_registers(ctx, addr, nb, tab_rp_registers);
                if (rc != nb) {
                    printf("ERROR modbus_read_registers (%d)\n", rc);
                    printf("Address = %d, nb = %d\n", addr, nb);
                    nb_fail++;
                } else {
                    for (i=0; i<nb; i++) {
                        if (tab_rq_registers[i] != tab_rp_registers[i]) {
                            printf("ERROR modbus_read_registers\n");
                            printf("Address = %d, value %d (0x%X) != %d (0x%X)\n",
                                   addr, tab_rq_registers[i], tab_rq_registers[i],
                                   tab_rp_registers[i], tab_rp_registers[i]);
                            nb_fail++;
                        }
                    }
                }
            }
            // R/W MULTIPLE REGISTERS 
            rc = modbus_write_and_read_registers(ctx,
                                                 addr, nb, tab_rw_rq_registers,
                                                 addr, nb, tab_rp_registers);
            if (rc != nb) {
                printf("ERROR modbus_read_and_write_registers (%d)\n", rc);
                printf("Address = %d, nb = %d\n", addr, nb);
                nb_fail++;
            } else {
                for (i=0; i<nb; i++) {
                    if (tab_rp_registers[i] != tab_rw_rq_registers[i]) {
                        printf("ERROR modbus_read_and_write_registers READ\n");
                        printf("Address = %d, value %d (0x%X) != %d (0x%X)\n",
                               addr, tab_rp_registers[i], tab_rw_rq_registers[i],
                               tab_rp_registers[i], tab_rw_rq_registers[i]);
                        nb_fail++;
                    }
                }

                rc = modbus_read_registers(ctx, addr, nb, tab_rp_registers);
                if (rc != nb) {
                    printf("ERROR modbus_read_registers (%d)\n", rc);
                    printf("Address = %d, nb = %d\n", addr, nb);
                    nb_fail++;
                } else {
                    for (i=0; i<nb; i++) {
                        if (tab_rw_rq_registers[i] != tab_rp_registers[i]) {
                            printf("ERROR modbus_read_and_write_registers WRITE\n");
                            printf("Address = %d, value %d (0x%X) != %d (0x%X)\n",
                                   addr, tab_rw_rq_registers[i], tab_rw_rq_registers[i],
                                   tab_rp_registers[i], tab_rp_registers[i]);
                            nb_fail++;
                        }
                    }
                }
            }
        }

        printf("Test: ");
        if (nb_fail)
            printf("%d FAILS\n", nb_fail);
        else
            printf("SUCCESS\n");
    }

	// Free the memory 
free(tab_rq_bits);
free(tab_rp_bits);
free(tab_rq_registers);
free(tab_rp_registers);
free(tab_rw_rq_registers);


    // Close the connection 
    modbus_close(ctx);
    modbus_free(ctx);
	*/

    return 0;
}
