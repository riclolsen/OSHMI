/* This software implements a Modbus driver for OSHMI.
 * Copyright - 2015-2020 - Ricardo Lastra Olsen
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

#define VERSION "OSHMI Modbus Driver v.1.10 - Copyright 2015-2020 Ricardo L. Olsen"

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
#include "cpp/INIReader.h"
#include "oshmi_types.h"

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
#include <time.h>

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
	modbus_t * ctx = nullptr;
	string ip = "";
	int port = 502;
	int slave_id = -1;
	int delay = 0;
	int timeout_ms = 500;
	int endianness = 0;
	string serial_port_name = "";
	int baud_rate = 9600;
	int data_bits = 8;
	int stop_bits = 1;
	char parity = 'N';
	int log_debug = 0;
	vector <mb_read> readhr;
	vector <mb_read> readhr_float; // read consecutive 16 bit values as floats (assume 1st byte=exp, 2nd=MSB mant, 3rd=middle mant, 4=LSB mant so intel 2,3,0,1 order) 
	vector <mb_read> readhr_long; // read consecutive 16 bit values as a long (32 bit integer=DWORD)
	vector <mb_read> readhr_bitstr; // read consecutive 16 bit values as 16 digital statuses to be converted to 16 digital points
	vector <mb_read> readir;
	vector <mb_read> readis;
	vector <mb_read> readcs;
};

vector <mb_rturead> mb_queue;

class OSHMIHandler
{

public:
	int shandle = 0;
	sockaddr_in address;
	sockaddr_in saddress; // socket address
	sockaddr_in saddress_red_driver; // socket address for redundant driver (different port from hmi)
	bool redundant_hmi = false; // there is redundant hmi?
	string IPAddrRed;
	int I104MPort = I104M_LISTENUDPPORT_DEFAULT;

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
			saddress_red_driver.sin_port = htons((unsigned short)I104MPort);
			redundant_hmi = true;
		}
	}

	bool Initialize()
	{

		INIReader reader(OSHMIINI);
		IPAddrRed = reader.Get("REDUNDANCY", "OTHER_HMI_IP", "");
		SetRedundantHMIAddress(IPAddrRed.c_str());

#if PLATFORM == PLATFORM_WINDOWS
		WSADATA WsaData;
		///return
			WSAStartup(MAKEWORD(2, 2), &WsaData);
		//	== NO_ERROR;
#endif
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons((unsigned short)I104MPort);

		shandle = socket(AF_INET,
			SOCK_DGRAM,
			IPPROTO_UDP);

		if (shandle <= 0)
		{
			printf("Failed to create socket!\n");
			exit(21);
		}

		if ( I104MPort != 0 )
		if (bind(shandle,
			(const sockaddr*)&address,
			sizeof(sockaddr_in)) < 0)
		{
			printf("Failed to bind socket!\n");
			exit(22);
		}

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

		int nonBlocking = 1;
		if (fcntl(handle,
			F_SETFL,
			O_NONBLOCK,
			nonBlocking) == -1)
		{
			printf("Failed to set non-blocking!\n");
			exit(23);
		}

#elif PLATFORM == PLATFORM_WINDOWS

		DWORD nonBlocking = 1;
		if (ioctlsocket(shandle,
			FIONBIO,
			&nonBlocking) != 0)
		{
			printf("Failed to set non-blocking!\n");
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

	int receiveCommands(int * asdu, int * address, int * val, int * slave, int * bit )
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
						*val = (int)pmsg->setpoint;
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
						*val = (int)pmsg->setpoint;
						*slave = pmsg->utr;
						return 1;
					    }
					break;
					case 64: // 32bit bitstring command
					   {
						// returns confirmation of asdu upwards
						t_msgsup msg;
						msg.causa = 0x00;
						msg.endereco = pmsg->endereco;
						msg.tipo = pmsg->tipo;
						msg.prim = 1;
						msg.sec = pmsg->utr;
						msg.signature = MSGSUP_SIG;
						msg.taminfo = sizeof(int);
						*(int*)msg.info = pmsg->onoff;
						int packet_size = sizeof(int) * 7 + sizeof(int);
						SendOSHMI(&msg, packet_size);

						*asdu = pmsg->tipo;
						*address = pmsg->endereco;
						*val = pmsg->onoff;
						*slave = pmsg->utr;
						*bit = pmsg->sbo; // bit number in sbo
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
void mb_command(int asdu, int address, int val, int slave, int bit)
{
	// slave 1 means the first from queue of modbus.ini (RTU_1)
	if (slave <= 0) // default slave is the first
		slave = 1;
	switch (asdu)
		{
		case 45: // digital single -> write bit
			modbus_write_bit(mb_queue[slave - 1].ctx, address, val);
			printf("WRITE modbus_write_bit RTU:%d ADDR=%d VAL=%d\n", slave, address, val);
			break;
		case 46: // digital double -> write 2 bits
			uint8_t src[2];
			src[0] = (val & 0x01) ? 1 : 0;
			src[1] = (val & 0x02) ? 1 : 0;
			modbus_write_bits(mb_queue[slave - 1].ctx, address, 2, src);
			printf("WRITE modbus_write_bit RTU:%d ADDR=%d VAL=%d\n", slave, address, val);
			break;
		case 48: // analog 16 bit -> write holding register
		case 49:
			modbus_write_register(mb_queue[slave - 1].ctx, address, val);
			printf("WRITE modbus_write_register RTU:%d ADDR=%d VAL=%d\n", slave, address, val);
			break;
		case 64: // 32bit bitstring (1st bit of bitstring at some bit position) -> read holding register, shift and mask bit and write back to the same holding register
			uint16_t sval, one = 1;
			modbus_read_registers(mb_queue[slave - 1].ctx, address, 1, &sval);
			if (val == 0)
				sval = sval & ~(one << bit); // reset bit
			else
				sval = sval | (one << bit); // raise bit
			modbus_write_register(mb_queue[slave - 1].ctx, address, sval);
			printf("WRITE modbus_write_register bitstring RTU:%d ADDR=%d VAL=%d BIT=%d\n", slave, address, val, bit);
			break;
		}
}

int main(void)
{
    int rc;
	OSHMIHandler oh;

	cout << VERSION << endl;
	cout << "Libmodbus " << LIBMODBUS_VERSION_STRING << endl;

	INIReader reader(MODBUSINI);
	oh.I104MPort = reader.GetInteger("I104M", "UDP_PORT_LISTEN", I104M_LISTENUDPPORT_DEFAULT);
	if (oh.I104MPort == 0)
		cout << "DISABLED I104M UDP PORT FOR LISTEN (NO OSHMI COMMANDS)!" << endl;
	else
		cout << "I104M UDP PORT = " << oh.I104MPort << endl;
	oh.Initialize();

	for (int i = 0; i < MAX_RTU; i++)
	{
		string rtun = (string)"RTU_" + std::to_string(i+1);
		if (!reader.HasSection(rtun))
		  break;
		mb_rturead rtu;
		rtu.ip = reader.GetString(rtun, "IP", "127.0.0.1");
		rtu.port = reader.GetInteger(rtun, "PORT", 502);
		rtu.slave_id = reader.GetInteger(rtun, "SLAVE_ID", -1);
		rtu.timeout_ms = reader.GetInteger(rtun, "TIMEOUT", 500);
		rtu.delay = reader.GetInteger(rtun, "DELAY", 0);
		rtu.endianness = reader.GetInteger(rtun, "ENDIANNESS", 0); // for 32 bit 0=BIG 1=MIDDLE LITTLE

		rtu.serial_port_name = reader.GetString(rtun, "SERIAL_PORT_NAME", "");
		rtu.baud_rate = reader.GetInteger(rtun, "BAUD_RATE", 9600);
		rtu.data_bits = reader.GetInteger(rtun, "DATA_BITS", 8);
		rtu.stop_bits = reader.GetInteger(rtun, "STOP_BITS", 1);
		string sparity = reader.GetString(rtun, "DATA_BITS", "N");
		if (sparity.length() > 0)
			rtu.parity = sparity[0];

		rtu.log_debug = reader.GetInteger(rtun, "LOG_LEVEL", 0);

		if (i > 0 &&  
				(
				rtu.ip!="" && mb_queue[i - 1].ip == rtu.ip && mb_queue[i - 1].port == rtu.port
				||
				rtu.serial_port_name != "" && mb_queue[i - 1].serial_port_name == rtu.serial_port_name
				)
			)
			rtu.ctx = mb_queue[i - 1].ctx; // same previous IP and PORT or serial port, reuse connection
		else
		{
			if (rtu.serial_port_name != "")
				rtu.ctx = modbus_new_rtu(rtu.serial_port_name.c_str(), rtu.baud_rate, rtu.parity, rtu.data_bits, rtu.stop_bits);
			else
			    rtu.ctx = modbus_new_tcp(rtu.ip.c_str(), rtu.port);
			if (rtu.ctx == NULL) {
				cout << "Unable to create the libmodbus context (slave " << i << ")!" << endl;
				exit(1);
			}
			if (rtu.log_debug == 0)
				modbus_set_debug(rtu.ctx, FALSE);
			else
			    modbus_set_debug(rtu.ctx, TRUE);
			modbus_connect(rtu.ctx);
			if (rtu.slave_id != -1)
				modbus_set_slave(rtu.ctx, rtu.slave_id);
			modbus_set_response_timeout(rtu.ctx, rtu.timeout_ms/1000 , (rtu.timeout_ms%1000) * 1000);
			uint32_t old_response_to_sec;
			uint32_t old_response_to_usec;
			modbus_get_response_timeout(rtu.ctx, &old_response_to_sec, &old_response_to_usec);
			printf("-------------------------------\n");
			printf("RTU %d\n", i + 1);
			if (rtu.serial_port_name != "")
				printf("SERIAL PORT %s\n", rtu.serial_port_name.c_str());
			else
			    printf("IP %s PORT %d\n", rtu.ip.c_str(), rtu.port);
			if (rtu.slave_id == -1)
				printf("SLAVE_ID [default]\n");
			else
			    printf("SLAVE_ID %d\n", rtu.slave_id);
			printf("DELAY %d ms\n", rtu.delay);
			printf ("TIMEOUT %d s : %d us \n", old_response_to_sec, old_response_to_usec);
			printf("-------------------------------\n");
		}

		for (int j = 0; j < MAX_INPREAD; j++)
		{
			string key = (string)"READHR_" + std::to_string(j+1);
			string aCnCp = reader.GetString(rtun, key, "");
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
			string aCnCp = reader.GetString(rtun, key, "");
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
			string key = (string)"READHR_LONG_" + std::to_string(j + 1);
			string aCnCp = reader.GetString(rtun, key, "");
			if (aCnCp == "")
				break;
			mb_read mbr;
			mbr.i104addr = 0;
			sscanf(aCnCp.c_str(), "%d %d %d", &mbr.mbaddress, &mbr.numreg, &mbr.i104addr);
			if (mbr.i104addr == 0)
				mbr.i104addr = mbr.mbaddress;
			rtu.readhr_long.push_back(mbr);
		}
		for (int j = 0; j < MAX_INPREAD; j++)
		{
			string key = (string)"READHR_BITSTR_" + std::to_string(j + 1);
			string aCnCp = reader.GetString(rtun, key, "");
			if (aCnCp == "")
				break;
			mb_read mbr;
			mbr.i104addr = 0;
			sscanf(aCnCp.c_str(), "%d %d %d", &mbr.mbaddress, &mbr.numreg, &mbr.i104addr);
			if (mbr.i104addr == 0)
				mbr.i104addr = mbr.mbaddress;
			rtu.readhr_bitstr.push_back(mbr);
		}
		for (int j = 0; j < MAX_INPREAD; j++)
		{
			string key = (string)"READIR_" + std::to_string(j+1);
			string aCnCp = reader.GetString(rtun, key, "");
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
			string aCnCp = reader.GetString(rtun, key, "");
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
			string aCnCp = reader.GetString(rtun, key, "");
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

	int asdu, address, val, slave, bit;

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
		bit = 0;
		if (oh.receiveCommands(&asdu, &address, &val, &slave, &bit))
        	mb_command(asdu, address, val, slave, bit);

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
			bit = 0;
			if (oh.receiveCommands(&asdu, &address, &val, &slave, &bit))
				mb_command(asdu, address, val, slave, bit);

			max_pointspkt = PKTANA_MAXPOINTS;

			if (mb_queue[i].slave_id != -1)
				modbus_set_slave(mb_queue[i].ctx, mb_queue[i].slave_id);
			modbus_set_response_timeout(mb_queue[i].ctx, mb_queue[i].timeout_ms / 1000, (mb_queue[i].timeout_ms % 1000) * 1000);

			uint16_t *tabreg = (uint16_t *)malloc(mb_queue[i].readhr[j].numreg * sizeof(uint16_t));
			memset(tabreg, 0, mb_queue[i].readhr[j].numreg * sizeof(uint16_t));
			rc = modbus_read_registers(mb_queue[i].ctx, mb_queue[i].readhr[j].mbaddress, mb_queue[i].readhr[j].numreg, tabreg);

			if (rc != mb_queue[i].readhr[j].numreg) {
				printf("RTU_%d ERROR modbus_read_registers (%d)\n", i+1, rc);
				printf("%s\n", modbus_strerror(errno));
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_close(mb_queue[i].ctx);
				Sleep(mb_queue[i].delay);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readhr[j].numreg; k++)
			{
				printf("RTU_%d: %s, Port: %d, ID: %d  Input %d = %d\n", i+1, mb_queue[i].ip.c_str(), mb_queue[i].port, mb_queue[i].slave_id, mb_queue[i].readhr[j].mbaddress + k, (short)tabreg[k]);

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
			bit = 0;
			if (oh.receiveCommands(&asdu, &address, &val, &slave, &bit))
				mb_command(asdu, address, val, slave, bit);

			max_pointspkt = PKTANA_MAXPOINTS;

			if (mb_queue[i].slave_id != -1)
				modbus_set_slave(mb_queue[i].ctx, mb_queue[i].slave_id);
			modbus_set_response_timeout(mb_queue[i].ctx, mb_queue[i].timeout_ms / 1000, (mb_queue[i].timeout_ms % 1000) * 1000);

			uint16_t *tabreg = (uint16_t *)malloc(mb_queue[i].readir[j].numreg * sizeof(uint16_t));
			memset(tabreg, 0, mb_queue[i].readir[j].numreg * sizeof(uint16_t));
			rc = modbus_read_input_registers(mb_queue[i].ctx, mb_queue[i].readir[j].mbaddress, mb_queue[i].readir[j].numreg, tabreg);

			if (rc != mb_queue[i].readir[j].numreg) {
				printf("RTU_%d ERROR modbus_read_input_registers (%d)\n", i + 1, rc);
				printf("%s\n", modbus_strerror(errno));
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_close(mb_queue[i].ctx);
				Sleep(mb_queue[i].delay);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readir[j].numreg; k++)
			{
				printf("RTU_%d: %s, Port: %d, ID: %d  Input %d = %d\n", i+1, mb_queue[i].ip.c_str(), mb_queue[i].port, mb_queue[i].slave_id, mb_queue[i].readir[j].mbaddress + k, (short)tabreg[k]);

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
		
		// holding register as 32 bit floats
		msg.signature = MSGSUPSQ_SIG;
		msg.prim = 1;
		msg.sec = i + 1; // RTU order or end of IP
		msg.causa = 20;
		msg.tipo = 13;
		msg.taminfo = sizeof(flutuante_seq); // value size for the type (not counting the 4 byte address)

		for (unsigned j = 0; j < mb_queue[i].readhr_float.size(); j++) // each 2 16 bit holding registers as a float
		{
			// verify if there is commands to execute from OSHMI
			bit = 0;
			if (oh.receiveCommands(&asdu, &address, &val, &slave, &bit))
				mb_command(asdu, address, val, slave, bit);

			max_pointspkt = PKTFLT_MAXPOINTS;

			if (mb_queue[i].slave_id != -1)
				modbus_set_slave(mb_queue[i].ctx, mb_queue[i].slave_id);
			modbus_set_response_timeout(mb_queue[i].ctx, mb_queue[i].timeout_ms / 1000, (mb_queue[i].timeout_ms % 1000) * 1000);

			uint16_t* tabreg = (uint16_t*)malloc(2 * mb_queue[i].readhr_float[j].numreg * sizeof(uint16_t));
			memset(tabreg, 0, 2 * mb_queue[i].readhr_float[j].numreg * sizeof(uint16_t));
			rc = modbus_read_registers(mb_queue[i].ctx, mb_queue[i].readhr_float[j].mbaddress, mb_queue[i].readhr_float[j].numreg * 2, tabreg);

			if (rc != mb_queue[i].readhr_float[j].numreg * 2) {
				printf("RTU_%d ERROR modbus_read_registers (%d)\n", i + 1, rc);
				printf("%s\n", modbus_strerror(errno));
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_close(mb_queue[i].ctx);
				Sleep(mb_queue[i].delay);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readhr_float[j].numreg * 2; k += 2)
			{
				union { float f; unsigned short bt[2]; } u;

				u.bt[1] = tabreg[k];
				u.bt[0] = tabreg[k + 1];

				printf("RTU_%d: %s, Port: %d, ID: %d  Input %d = %f\n", i+1, mb_queue[i].ip.c_str(), mb_queue[i].port, mb_queue[i].slave_id, mb_queue[i].readhr_float[j].mbaddress + k, u.f);

				unsigned int* paddr = (unsigned int*)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(flutuante_seq)));
				*paddr = mb_queue[i].readhr_float[j].i104addr + k / 2; // point number

				// value and quality
				flutuante_seq* obj = (flutuante_seq*)(paddr + 1);
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

		// holding pairs of 16 bit registers as 32 bit integers
		msg.signature = MSGSUPSQ_SIG;
		msg.prim = 1;
		msg.sec = i + 1; // RTU order or end of IP
		msg.causa = 20;
		msg.tipo = 15;
		msg.taminfo = sizeof(integrated_seq); // value size for the type (not counting the 4 byte address)

		for (unsigned j = 0; j < mb_queue[i].readhr_long.size(); j++) // each 2 16 bit holding registers as a 32 bit word
		{
			// verify if there is commands to execute from OSHMI
			bit = 0;
			if (oh.receiveCommands(&asdu, &address, &val, &slave, &bit))
				mb_command(asdu, address, val, slave, bit);

			max_pointspkt = PKTFLT_MAXPOINTS;

			if (mb_queue[i].slave_id != -1)
				modbus_set_slave(mb_queue[i].ctx, mb_queue[i].slave_id);
			modbus_set_response_timeout(mb_queue[i].ctx, mb_queue[i].timeout_ms / 1000, (mb_queue[i].timeout_ms % 1000) * 1000);

			uint16_t* tabreg = (uint16_t*)malloc(2 * mb_queue[i].readhr_long[j].numreg * sizeof(uint16_t));
			memset(tabreg, 0, 2 * mb_queue[i].readhr_long[j].numreg * sizeof(uint16_t));
			rc = modbus_read_registers(mb_queue[i].ctx, mb_queue[i].readhr_long[j].mbaddress, mb_queue[i].readhr_long[j].numreg * 2, tabreg);

			if (rc != mb_queue[i].readhr_long[j].numreg * 2) {
				printf("RTU_%d ERROR modbus_read_registers (%d)\n", i + 1, rc);
				printf("%s\n", modbus_strerror(errno));
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_close(mb_queue[i].ctx);
				Sleep(mb_queue[i].delay);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readhr_long[j].numreg * 2; k += 2)
			{
				union { int32_t i32; unsigned short bt[2]; } u;				 

				switch (mb_queue[i].endianness)
				{
				case 1: // MIDDLE LITTLE ENDIAN
					u.bt[0] = tabreg[k];
					u.bt[1] = tabreg[k + 1];
					break;
				case 0: // BIG ENDIAN
				default:
					u.bt[1] = tabreg[k];
					u.bt[0] = tabreg[k + 1];
					break;
				}

				printf("RTU_%d: %s, Port: %d, ID: %d  Input %d = %ld\n", i + 1, mb_queue[i].ip.c_str(), mb_queue[i].port, mb_queue[i].slave_id, mb_queue[i].readhr_long[j].mbaddress + k, u.i32);

				unsigned int* paddr = (unsigned int*)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(integrated_seq)));
				*paddr = mb_queue[i].readhr_long[j].i104addr + k / 2; // point number

				// value and quality
				integrated_seq* obj = (integrated_seq*)(paddr + 1);
				obj->qds = 0;
				obj->bcr = u.i32;

				count++;
				if (!((count + 1) % max_pointspkt)) // if next from packet send now (se a proxima eh do proximo pacote, manda agora)
				{
					msg.numpoints = count % max_pointspkt;
					packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(integrated_seq));
					oh.SendOSHMI(&msg, packet_size);
				}
			}

			msg.numpoints = count % max_pointspkt;
			packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(integrated_seq));
			oh.SendOSHMI(&msg, packet_size);

			free(tabreg);
			Sleep(mb_queue[i].delay);
		}

		// holding registers as 16 bit bitstrings
		msg.signature = MSGSUPSQ_SIG;
		msg.prim = 1;
		msg.sec = i + 1; // RTU order or end of IP
		msg.causa = 20;
		msg.tipo = 151; // reserved for user ASDU number means 16bit bitstring
		msg.taminfo = sizeof(analogico_seq); // value size for the type (not counting the 4 byte address)

		for (unsigned j = 0; j < mb_queue[i].readhr_bitstr.size(); j++) // each 16 bit holding registers as bitstring
		{
			// verify if there is commands to execute from OSHMI
			bit = 0;
			if (oh.receiveCommands(&asdu, &address, &val, &slave, &bit))
				mb_command(asdu, address, val, slave, bit);

			max_pointspkt = PKTFLT_MAXPOINTS;

			if (mb_queue[i].slave_id != -1)
				modbus_set_slave(mb_queue[i].ctx, mb_queue[i].slave_id);
			modbus_set_response_timeout(mb_queue[i].ctx, mb_queue[i].timeout_ms / 1000, (mb_queue[i].timeout_ms % 1000) * 1000);

			uint16_t* tabreg = (uint16_t*)malloc(mb_queue[i].readhr_bitstr[j].numreg * sizeof(uint16_t));
			memset(tabreg, 0, mb_queue[i].readhr_bitstr[j].numreg * sizeof(uint16_t));
			rc = modbus_read_registers(mb_queue[i].ctx, mb_queue[i].readhr_bitstr[j].mbaddress, mb_queue[i].readhr_bitstr[j].numreg, tabreg);

			if (rc != mb_queue[i].readhr_bitstr[j].numreg) {
				printf("RTU_%d ERROR modbus_read_registers (%d)\n", i + 1, rc);
				printf("%s\n", modbus_strerror(errno));
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_close(mb_queue[i].ctx);
				Sleep(mb_queue[i].delay);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readhr_bitstr[j].numreg; k++)
			{					
				printf("RTU_%d: %s, Port: %d, ID: %d  Input %d = %d\n", i+1, mb_queue[i].ip.c_str(), mb_queue[i].port, mb_queue[i].slave_id, mb_queue[i].readhr_bitstr[j].mbaddress + k, tabreg[k]);

				unsigned int* paddr = (unsigned int*)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(analogico_seq)));
				*paddr = mb_queue[i].readhr_bitstr[j].i104addr + k; // point number

				// value and quality
				analogico_seq* obj = (analogico_seq*)(paddr + 1);
				obj->qds = 0;
				obj->sva = tabreg[k];

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
		msg.tipo = 1;
		msg.taminfo = sizeof(digital_notime_seq); // value size for the type (not counting the 4 byte address)

		for (unsigned j = 0; j < mb_queue[i].readcs.size(); j++)
		{
			// verify if there is commands to execute from OSHMI
			bit = 0;
			if (oh.receiveCommands(&asdu, &address, &val, &slave, &bit))
				mb_command(asdu, address, val, slave, bit);

			max_pointspkt = PKTDIG_MAXPOINTS;

			if (mb_queue[i].slave_id != -1)
				modbus_set_slave(mb_queue[i].ctx, mb_queue[i].slave_id);
			modbus_set_response_timeout(mb_queue[i].ctx, mb_queue[i].timeout_ms / 1000, (mb_queue[i].timeout_ms % 1000) * 1000);

			uint8_t *tabreg = (uint8_t *)malloc(mb_queue[i].readcs[j].numreg * sizeof(uint8_t));
			memset(tabreg, 0, mb_queue[i].readcs[j].numreg * sizeof(uint8_t));
			rc = modbus_read_bits(mb_queue[i].ctx, mb_queue[i].readcs[j].mbaddress, mb_queue[i].readcs[j].numreg, tabreg);

			if (rc != mb_queue[i].readcs[j].numreg) {
				printf("RTU_%d ERROR modbus_read_bits (%d)\n", i + 1, rc);
				printf("%s\n", modbus_strerror(errno));
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_close(mb_queue[i].ctx);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readcs[j].numreg; k++)
			{
				printf("RTU_%d: %s, Port: %d, ID: %d  Input %d = %d\n", i+1, mb_queue[i].ip.c_str(), mb_queue[i].port, mb_queue[i].slave_id, mb_queue[i].readcs[j].mbaddress + k, tabreg[k]);

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

		msg.signature = MSGSUPSQ_SIG;
		msg.prim = 1;
		msg.sec = i + 1; // RTU order or end of IP
		msg.causa = 20;
		msg.tipo = 1;
		msg.taminfo = sizeof(digital_notime_seq); // value size for the type (not counting the 4 byte address)

		for (unsigned j = 0; j < mb_queue[i].readis.size(); j++)
		{
			// verify if there is commands to execute from OSHMI
			bit = 0;
			if (oh.receiveCommands(&asdu, &address, &val, &slave, &bit))
				mb_command(asdu, address, val, slave, bit);

			max_pointspkt = PKTDIG_MAXPOINTS;

			if (mb_queue[i].slave_id != -1)
				modbus_set_slave(mb_queue[i].ctx, mb_queue[i].slave_id);
			modbus_set_response_timeout(mb_queue[i].ctx, mb_queue[i].timeout_ms / 1000, (mb_queue[i].timeout_ms % 1000) * 1000);

			uint8_t *tabreg = (uint8_t *)malloc(mb_queue[i].readis[j].numreg * sizeof(uint8_t));
			memset(tabreg, 0, mb_queue[i].readis[j].numreg * sizeof(uint8_t));
			rc = modbus_read_input_bits(mb_queue[i].ctx, mb_queue[i].readis[j].mbaddress, mb_queue[i].readis[j].numreg, tabreg);

			if (rc != mb_queue[i].readis[j].numreg) {
				printf("RTU_%d ERROR modbus_read_input_bits (%d)\n", i + 1, rc);
				printf("%s\n", modbus_strerror(errno));
				// printf("Address = %d\n", addr);
				// nb_fail++;
				modbus_flush(mb_queue[i].ctx);
				modbus_close(mb_queue[i].ctx);
				modbus_connect(mb_queue[i].ctx);
				break;
			}

			count = 0;
			for (int k = 0; k < mb_queue[i].readis[j].numreg; k++)
			{
				printf("RTU_%d: %s, Port: %d, ID: %d  Input %d = %d\n", i+1, mb_queue[i].ip.c_str(), mb_queue[i].port, mb_queue[i].slave_id, mb_queue[i].readis[j].mbaddress + k, tabreg[k]);

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

    return 0;
}
