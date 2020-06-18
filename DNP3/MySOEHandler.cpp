/**
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */

#include <iostream>
#include <sstream>
#include <time.h> 
#include "MySOEHandler.h"
#include "cpp/INIReader.h"

using namespace std;

bool InitializeSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
	WSADATA WsaData;
	return WSAStartup(MAKEWORD(2, 2),
		&WsaData)
		== NO_ERROR;
#else
	return true;
#endif
}

void ShutdownSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
	WSACleanup();
#endif
}

namespace opendnp3
{
MySOEHandler::MySOEHandler()
{
	saddress.sin_family = AF_INET;
	saddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddress.sin_port = htons((unsigned short)I104M_WRITEUDPPORT);

	if (shandle == 0)
        Init();
}

void MySOEHandler::Init() 
{
    InitializeSockets();

	sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short)I104MListenPort);

    if (shandle == 0)
    {
        INIReader reader(DNP3INI);
        I104MListenPort = reader.GetInteger("I104M", "UDP_PORT_LISTEN", I104M_LISTENUDPPORT_DEFAULT);
        if (I104MListenPort == 0)
            cout << "DISABLED I104M UDP PORT FOR LISTEN (NO OSHMI COMMANDS)!" << endl;
        else
            cout << "I104M LISTEN UDP PORT (OSHMI COMMANDS) = " << I104MListenPort << endl;

		shandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (shandle <= 0)
        {
            std::cout << "Failed to create socket" << std::endl;
            exit(21);
        }

        if (I104MListenPort != 0)
            if (bind(shandle, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0)
            {
                std::cout << "Failed to bind socket" << std::endl;
                exit(22);
            }
    }
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

    int nonBlocking = 1;
    if (fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1)
    {
        std::cout << "Failed to set non-blocking" << std::endl;
        exit(23);
    }

#elif PLATFORM == PLATFORM_WINDOWS

    DWORD nonBlocking = 1;
    if (ioctlsocket(shandle, FIONBIO, &nonBlocking) != 0)
    {
        std::cout << "Failed to set non-blocking" << std::endl;
        exit(23);
    }

#endif
}

unsigned char MySOEHandler::xlatequalif(int qdnp)
{
	unsigned char q104 = 0;
	q104 |= (qdnp & 0x04) ? 0x80 : 0x00; // communication lost
	q104 |= (qdnp & 0x01) ? 0x00 : 0x80; // online
	q104 |= (qdnp & 0x08) ? 0x20 : 0x00; // remote forced data
	q104 |= (qdnp & 0x10) ? 0x20 : 0x00; // local forced data
	q104 |= (qdnp & 0x20) ? 0x80 : 0x00; // over-range
	return q104;
}

void MySOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<Binary>>& values)
{
	int packet_size;
	int max_pointspkt = PKTDIG_MAXPOINTS;

	PrintHeaderInfo(info);
	NoDataCntTime = 0; // mantém link vivo

	// monta buffer de mensagem para OSHMI no formato I104M
	t_msgsupsq msg;
	msg.signature = MSGSUPSQ_SIG;
	msg.prim = MasterAddress;
	msg.sec = SlaveAddress;
	msg.causa = info.isEventVariation ? 3 : 20;

	int count = 0;
	
	if (info.gv != GroupVariation::Group2Var2 && info.gv != GroupVariation::Group2Var3) // event without time
	{
		msg.tipo = 1;
		msg.taminfo = sizeof(digital_notime_seq); // value size for the type (not counting the 4 byte address)
		values.ForeachItem([&](const Indexed<Binary>& pair)
		{
			// acerta endereco do ponto
			unsigned int * paddr = (unsigned int *)(msg.info + (count  % max_pointspkt) * (sizeof(int) + sizeof(digital_notime_seq)));
			*paddr = pair.index + OFFSET_DIGITAL; //considera offset para o grupo

			// valor e qualidade
			digital_notime_seq * obj = (digital_notime_seq *)(paddr + 1);
			obj->iq = xlatequalif(pair.value.flags.value);
			obj->iq |= pair.value.value? 1 : 0;

			std::cout << "[" << pair.index << "] : " <<
				ValueToString(pair.value) << " : " <<
				static_cast<int>(pair.value.flags.value) << " : " <<
				pair.value.time << " RTU:" << msg.sec << std::endl;

			count++;
			if (!((count + 1) % max_pointspkt)) // se a próxima é do próximo pacote, manda agora
			{
				msg.numpoints = count % max_pointspkt;
				packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_notime_seq));
				SendOSHMI(&msg, packet_size);
			}

		});

	msg.numpoints = count % max_pointspkt;
	packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_notime_seq));
	SendOSHMI(&msg, packet_size);
	}
	else
	{
		msg.taminfo = sizeof(digital_w_time7_seq); // value size for the type (not counting the 4 byte address)
		values.ForeachItem([&](const Indexed<Binary>& pair)
		{
			max_pointspkt = PKTEVE_MAXPOINTS;
			msg.tipo = 30;

			// acerta endereco do ponto
			unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(digital_w_time7_seq)));
			*paddr = pair.index + OFFSET_DIGITAL; // considera offset para o grupo

			// valor e qualidade
			digital_w_time7_seq * obj = (digital_w_time7_seq *)(paddr + 1);
			obj->iq = xlatequalif(pair.value.flags.value);
			obj->iq |= pair.value.value ? 1 : 0;

			time_t tmi = pair.value.time / 1000;
			struct tm *unxtm = localtime( &tmi );
			//struct tm *unxtm = gmtime(&tmi);

			obj->ano = unxtm->tm_year % 100;
			obj->mes = unxtm->tm_mon + 1;
			obj->dia = unxtm->tm_mday;
			obj->hora = unxtm->tm_hour;
			obj->min = unxtm->tm_min;
			obj->ms = (unsigned short)(unxtm->tm_sec * 1000 + pair.value.time % 1000);

			std::cout << "[" << pair.index << "] : " <<
				ValueToString(pair.value) << " : " <<
				static_cast<int>(pair.value.flags.value) << " : " <<
				pair.value.time << " RTU:" << msg.sec << std::endl;

			count++;
			if (!((count + 1) % max_pointspkt)) // se a próxima é do próximo pacote, manda agora
			{
				msg.numpoints = count % max_pointspkt;
				packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_w_time7_seq));
				SendOSHMI(&msg, packet_size);
			}

		});

	msg.numpoints = count % max_pointspkt;
	packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_w_time7_seq));
	SendOSHMI(&msg, packet_size);
	}
}

void MySOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<DoubleBitBinary>>& values)
{
	int packet_size;
	int max_pointspkt = PKTDIG_MAXPOINTS;

	PrintHeaderInfo(info);
	NoDataCntTime = 0; // mantém link vivo

	// monta buffer de mensagem para OSHMI no formato I104M
	t_msgsupsq msg;
	msg.signature = MSGSUPSQ_SIG;
	msg.prim = MasterAddress;
	msg.sec = SlaveAddress; 
	msg.causa = info.isEventVariation ? 3 : 20;

	int count = 0;

	if (info.gv != GroupVariation::Group4Var2 && info.gv != GroupVariation::Group4Var3) // event without time
	{
		msg.tipo = 3;
		msg.taminfo = sizeof(digital_notime_seq); // value size for the type (not counting the 4 byte address)
		values.ForeachItem([&](const Indexed<DoubleBitBinary>& pair)
		{
			// acerta endereco do ponto
			unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(digital_notime_seq)));
			*paddr = pair.index + OFFSET_DIGITALDBL; // considera offset para o grupo

			// valor e qualidade
			digital_notime_seq * obj = (digital_notime_seq *)(paddr + 1);
			obj->iq = xlatequalif(pair.value.flags.value);
			obj->iq |= (static_cast<int>(pair.value.value)) ? 1 : 0;

			std::cout << "[" << pair.index << "] : " <<
				ValueToString(pair.value) << " : " <<
				static_cast<int>(pair.value.flags.value) << " : " <<
				pair.value.time << " RTU:" << msg.sec << std::endl;

			count++;
			if (!((count + 1) % max_pointspkt)) // se a próxima é do próximo pacote, manda agora
			{
				msg.numpoints = count % max_pointspkt;
				packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_notime_seq));
				SendOSHMI(&msg, packet_size);
			}

		});

	msg.numpoints = count % max_pointspkt;
	packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_notime_seq));
	SendOSHMI(&msg, packet_size);		
	}
	else
		{
			max_pointspkt = PKTEVE_MAXPOINTS;
			msg.taminfo = sizeof(digital_w_time7_seq); // value size for the type (not counting the 4 byte address)
			values.ForeachItem([&](const Indexed<DoubleBitBinary>& pair)
			{
				msg.tipo = 31;

				// acerta endereco do ponto
				unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(digital_w_time7_seq)));
				*paddr = pair.index + OFFSET_DIGITALDBL; // considera offset para o grupo

				// valor e qualidade
				digital_w_time7_seq * obj = (digital_w_time7_seq *)(paddr + 1);
				obj->iq = xlatequalif(pair.value.flags.value);
				obj->iq |= (static_cast<int>(pair.value.value)) ? 1 : 0;

				time_t tmi = pair.value.time / 1000;
				struct tm *unxtm = localtime(&tmi);
				//struct tm *unxtm = gmtime(&tmi);

				obj->ano = unxtm->tm_year % 100;
				obj->mes = unxtm->tm_mon + 1;
				obj->dia = unxtm->tm_mday;
				obj->hora = unxtm->tm_hour;
				obj->min = unxtm->tm_min;
				obj->ms = (unsigned short)(unxtm->tm_sec * 1000 + pair.value.time % 1000);

     			std::cout << "[" << pair.index << "] : " <<
					ValueToString(pair.value) << " : " <<
					static_cast<int>(pair.value.flags.value) << " : " <<
					pair.value.time << " RTU:" << msg.sec << std::endl;

				count++;
				if (!((count + 1) % max_pointspkt)) // se a próxima é do próximo pacote, manda agora
				{
					msg.numpoints = count % max_pointspkt;
					packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_w_time7_seq));
					SendOSHMI(&msg, packet_size);
				}

			});
		
		msg.numpoints = count % max_pointspkt;
		packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_w_time7_seq));
		SendOSHMI(&msg, packet_size);
		}

}

void MySOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<Analog>>& values)
{
	int packet_size;

	PrintHeaderInfo(info);
	NoDataCntTime = 0; // mantém link vivo

	// monta buffer de mensagem para OSHMI no formato I104M
	t_msgsupsq msg;
	msg.signature = MSGSUPSQ_SIG;
	msg.prim = MasterAddress;
	msg.sec = SlaveAddress;
	msg.causa = info.isEventVariation ? 3 : 20;

	int count = 0;

	msg.tipo = 13;
	msg.taminfo = sizeof(flutuante_seq); // value size for the type (not counting the 4 byte address)
	values.ForeachItem([&](const Indexed<Analog>& pair)
	{
		// acerta endereco do ponto
		unsigned int * paddr = (unsigned int *)(msg.info + (count % PKTANA_MAXPOINTS) * (sizeof(int) + sizeof(flutuante_seq)));
		*paddr = pair.index + OFFSET_ANALOG; // considera offset para o grupo

		// valor e qualidade
		flutuante_seq * obj = (flutuante_seq *)(paddr + 1);
		obj->fr = (float)pair.value.value;
		obj->qds = xlatequalif(pair.value.flags.value);

		std::cout << "[" << pair.index << "] : " <<
			pair.value.value << " : " <<
			static_cast<int>(pair.value.flags.value) << " : " <<
			pair.value.time << " RTU:" << msg.sec << std::endl;

		count++;

		if ( !((count + 1) % PKTANA_MAXPOINTS)) // se a próxima é do próximo pacote, manda agora
		{
			msg.numpoints = count % PKTANA_MAXPOINTS;
			packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(flutuante_seq));
			SendOSHMI(&msg, packet_size);
		}

	});

	msg.numpoints = count % PKTANA_MAXPOINTS;
	packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(flutuante_seq));
	SendOSHMI(&msg, packet_size);
}

void MySOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<Counter>>& values)
{
	int packet_size;

	PrintHeaderInfo(info);
	NoDataCntTime = 0; // mantém link vivo

    // monta buffer de mensagem para OSHMI no formato I104M
	t_msgsupsq msg;
	msg.signature = MSGSUPSQ_SIG;
	msg.prim = MasterAddress;
	msg.sec = SlaveAddress;
	msg.causa = info.isEventVariation ? 3 : 20;

	int count = 0;

	msg.tipo = 15;
	msg.taminfo = sizeof(integrated_seq); // value size for the type (not counting the 4 byte address)
	values.ForeachItem([&](const Indexed<Counter>& pair)
	{
		// acerta endereco do ponto
		unsigned int * paddr = (unsigned int *)(msg.info + (count % PKTANA_MAXPOINTS) * (sizeof(int) + sizeof(integrated_seq)));
		*paddr = pair.index + OFFSET_COUNTER; // considera offset para o grupo

		// valor e qualidade
		integrated_seq * obj = (integrated_seq *)(paddr + 1);
		obj->bcr = pair.value.value;
		obj->qds = xlatequalif(pair.value.flags.value);

		std::cout << "[" << pair.index << "] : " <<
			pair.value.value << " : " <<
			static_cast<int>(pair.value.flags.value) << " : " <<
			pair.value.time << " RTU:" << msg.sec << std::endl;

		count++;

		if (!((count + 1) % PKTANA_MAXPOINTS)) // se a próxima é do próximo pacote, manda agora
		{
			msg.numpoints = count % PKTANA_MAXPOINTS;
			packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(integrated_seq));
			SendOSHMI(&msg, packet_size);
		}

	});

	msg.numpoints = count % PKTANA_MAXPOINTS;
	packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(flutuante_seq));
	SendOSHMI(&msg, packet_size);
}

void MySOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<FrozenCounter>>& values)
{
	int packet_size;

	PrintHeaderInfo(info);
	NoDataCntTime = 0; // mantém link vivo

	// monta buffer de mensagem para OSHMI no formato I104M
	t_msgsupsq msg;
	msg.signature = MSGSUPSQ_SIG;
	msg.prim = MasterAddress;
	msg.sec = SlaveAddress;
	msg.causa = info.isEventVariation ? 3 : 20;

	int count = 0;

	msg.tipo = 15;
	msg.taminfo = sizeof(integrated_seq); // value size for the type (not counting the 4 byte address)
	values.ForeachItem([&](const Indexed<FrozenCounter>& pair)
	{
		// acerta endereco do ponto
		unsigned int * paddr = (unsigned int *)(msg.info + (count % PKTANA_MAXPOINTS) * (sizeof(int) + sizeof(integrated_seq)));
		*paddr = pair.index + OFFSET_FROZENCOUNTER; // considera offset para o grupo

		// valor e qualidade
		integrated_seq * obj = (integrated_seq *)(paddr + 1);
		obj->bcr = pair.value.value;
		obj->qds = xlatequalif(pair.value.flags.value);

		std::cout << "[" << pair.index << "] : " <<
			pair.value.value << " : " <<
			static_cast<int>(pair.value.flags.value) << " : " <<
			pair.value.time << " RTU:" << msg.sec << std::endl;

		count++;

		if (!((count + 1) % PKTANA_MAXPOINTS)) // se a próxima é do próximo pacote, manda agora
		{
			msg.numpoints = count % PKTANA_MAXPOINTS;
			packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(flutuante_seq));
			SendOSHMI(&msg, packet_size);
		}

	});

	msg.numpoints = count % PKTANA_MAXPOINTS;
	packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(flutuante_seq));
	SendOSHMI(&msg, packet_size);
}

void MySOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<BinaryOutputStatus>>& values)
{
	int packet_size;
	int max_pointspkt = PKTDIG_MAXPOINTS;

	PrintHeaderInfo(info);
	NoDataCntTime = 0; // mantém link vivo

	// monta buffer de mensagem para OSHMI no formato I104M
	t_msgsupsq msg;
	msg.signature = MSGSUPSQ_SIG;
	msg.prim = MasterAddress;
	msg.sec = SlaveAddress;
	msg.causa = info.isEventVariation ? 3 : 20;

	int count = 0;

	if (info.gv != GroupVariation::Group11Var2) // event without time
	{
		msg.tipo = 1;
		msg.taminfo = sizeof(digital_notime_seq); // value size for the type (not counting the 4 byte address)
		values.ForeachItem([&](const Indexed<BinaryOutputStatus>& pair)
		{
			// acerta endereco do ponto
			unsigned int * paddr = (unsigned int *)(msg.info + (count  % max_pointspkt) * (sizeof(int) + sizeof(digital_notime_seq)));
			*paddr = pair.index + OFFSET_DIGITAL_OUTST; //considera offset para o grupo

			// valor e qualidade
			digital_notime_seq * obj = (digital_notime_seq *)(paddr + 1);
			obj->iq = xlatequalif(pair.value.flags.value);
			obj->iq |= pair.value.value ? 1 : 0;

			std::cout << "[" << pair.index << "] : " <<
				ValueToString(pair.value) << " : " <<
				static_cast<int>(pair.value.flags.value) << " : " <<
				pair.value.time << " RTU:" << msg.sec << std::endl;

			count++;
			if (!((count + 1) % max_pointspkt)) // se a próxima é do próximo pacote, manda agora
			{
				msg.numpoints = count % max_pointspkt;
				packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_notime_seq));
				SendOSHMI(&msg, packet_size);
			}

		});

		msg.numpoints = count % max_pointspkt;
		packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_notime_seq));
		SendOSHMI(&msg, packet_size);
	}
	else
		{
			msg.taminfo = sizeof(digital_w_time7_seq); // value size for the type (not counting the 4 byte address)
			values.ForeachItem([&](const Indexed<BinaryOutputStatus>& pair)
			{
				max_pointspkt = PKTEVE_MAXPOINTS;
				msg.tipo = 30;

				// acerta endereco do ponto
				unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(digital_w_time7_seq)));
				*paddr = pair.index + OFFSET_DIGITAL_OUTST; // considera offset para o grupo

				// valor e qualidade
				digital_w_time7_seq * obj = (digital_w_time7_seq *)(paddr + 1);
				obj->iq = xlatequalif(pair.value.flags.value);
				obj->iq |= pair.value.value ? 1 : 0;

				time_t tmi = pair.value.time / 1000;
				struct tm *unxtm = localtime(&tmi);
				//struct tm *unxtm = gmtime(&tmi);

				obj->ano = unxtm->tm_year % 100;
				obj->mes = unxtm->tm_mon + 1;
				obj->dia = unxtm->tm_mday;
				obj->hora = unxtm->tm_hour;
				obj->min = unxtm->tm_min;
				obj->ms = (unsigned short)(unxtm->tm_sec * 1000 + pair.value.time % 1000);

				std::cout << "[" << pair.index << "] : " <<
					ValueToString(pair.value) << " : " <<
					static_cast<int>(pair.value.flags.value) << " : " <<
					pair.value.time << " RTU:" << msg.sec << std::endl;

				count++;
				if (!((count + 1) % max_pointspkt)) // se a próxima é do próximo pacote, manda agora
				{
					msg.numpoints = count % max_pointspkt;
					packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_w_time7_seq));
					SendOSHMI(&msg, packet_size);
				}

			});

			msg.numpoints = count % max_pointspkt;
			packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(digital_w_time7_seq));
			SendOSHMI(&msg, packet_size);
		}
}

void MySOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<AnalogOutputStatus>>& values)
{
	int packet_size;

	PrintHeaderInfo(info);
	NoDataCntTime = 0; // mantém link vivo

    // monta buffer de mensagem para OSHMI no formato I104M
	t_msgsupsq msg;
	msg.signature = MSGSUPSQ_SIG;
	msg.prim = MasterAddress;
	msg.sec = SlaveAddress;
	msg.causa = info.isEventVariation ? 3 : 20;

	int count = 0;

	msg.tipo = 13;
	msg.taminfo = sizeof(flutuante_seq); // value size for the type (not counting the 4 byte address)
	values.ForeachItem([&](const Indexed<AnalogOutputStatus>& pair)
	{
		// acerta endereco do ponto
		unsigned int * paddr = (unsigned int *)(msg.info + (count % PKTANA_MAXPOINTS) * (sizeof(int) + sizeof(flutuante_seq)));
		*paddr = pair.index + OFFSET_ANALOG_OUTST; // considera offset para o grupo

        // valor e qualidade
		flutuante_seq * obj = (flutuante_seq *)(paddr + 1);
		obj->fr = (float)pair.value.value;
		obj->qds = xlatequalif(pair.value.flags.value);

		std::cout << "[" << pair.index << "] : " <<
			pair.value.value << " : " <<
			static_cast<int>(pair.value.flags.value) << " : " <<
			pair.value.time << " RTU:" << msg.sec << std::endl;

		count++;

		if (!((count + 1) % PKTANA_MAXPOINTS)) // se a próxima é do próximo pacote, manda agora
		{
			msg.numpoints = count % PKTANA_MAXPOINTS;
			packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(flutuante_seq));
			SendOSHMI(&msg, packet_size);
		}

	});

	msg.numpoints = count % PKTANA_MAXPOINTS;
	packet_size = sizeof(int) * 7 + msg.numpoints * (sizeof(int) + sizeof(flutuante_seq));
	SendOSHMI(&msg, packet_size);
}

void MySOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<OctetString>>& values)
{
	this->PrintHeaderInfo(info);
	NoDataCntTime = 0; // mantém link vivo

	values.ForeachItem([&](const Indexed<OctetString>& pair)
	{
		std::cout << "OctetString " << " [" << pair.index << "] : Size : " << pair.value.Size() << std::endl;
	});
}

void MySOEHandler::SendKeepAlive()
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
		{
			std::cout << "Failed to send packet " << std::endl;
		}
	}
}

void MySOEHandler::SendOSHMI(void * pmsg, int packet_size)
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
	{
		std::cout << "Failed to send packet " << std::endl;
	}

	if (redundant_hmi)
	{
		sent_bytes =
			sendto(shandle,
			(const char*)pmsg,
				packet_size,
				0,
				(sockaddr*)&saddress_red,
				sizeof(sockaddr_in));

		if (sent_bytes != packet_size)
		{
			std::cout << "Failed to send packet " << std::endl;
		}
		else 
		{
			std::cout << "--->>> OSHMI ------>>>" << std::endl;
		}
	}

}

void MySOEHandler::SendOSHMIChannelStatePoint(bool channel_ok)
{
	// monta buffer de mensagem para OSHMI no formato I104M
	t_msgsupsq msg;
	msg.signature = MSGSUPSQ_SIG;
	msg.prim = MasterAddress;
	msg.sec = SlaveAddress;
	msg.causa = 3;
	msg.tipo = 1;
	msg.taminfo = sizeof(digital_notime_seq); // value size for the type (not counting the 4 byte address)
											  // acerta endereco do ponto
	unsigned int * paddr = (unsigned int *)(msg.info);
	*paddr = OFFSET_DIGITAL - 1; // pega o ponto OFFSET - 1 para armazenar o estado do link
								 // valor e qualidade
	digital_notime_seq * obj = (digital_notime_seq *)(paddr + 1);
	obj->iq = 0;
	obj->iq |= (channel_ok) ? 1 : 0;
	msg.numpoints = 1;
	int packet_size = sizeof(int) * 7 + 1 * (sizeof(int) + sizeof(digital_notime_seq));
	SendOSHMI(&msg, packet_size);
}

int MySOEHandler::shandle = 0;

}
