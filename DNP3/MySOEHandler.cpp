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

// MySOEHandler MySOEHandler::instance;

MySOEHandler::MySOEHandler()
{
	InitializeSockets();

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons((unsigned short)I104M_LISTENUDPPORT);

	if (shandle == 0)
	{
		shandle = socket(AF_INET,
			SOCK_DGRAM,
			IPPROTO_UDP);
		
		if (shandle <= 0)
		{
			std::cout << "Failed to create socket" << std::endl;
			exit(21);
		}

		if (bind(shandle,
			(const sockaddr*)&address,
			sizeof(sockaddr_in)) < 0)
		{
			std::cout << "Failed to bind socket" << std::endl;
			exit(22);
		}

	}
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

	int nonBlocking = 1;
	if (fcntl(handle,
		F_SETFL,
		O_NONBLOCK,
		nonBlocking) == -1)
	{
		std::cout << "Failed to set non-blocking" << std::endl;
		exit(23);
	}

#elif PLATFORM == PLATFORM_WINDOWS

	DWORD nonBlocking = 1;
	if (ioctlsocket(shandle,
		FIONBIO,
		&nonBlocking) != 0)
	{
		std::cout << "Failed to set non-blocking" << std::endl;
		exit(23);
	}

#endif

	saddress.sin_family = AF_INET;
	saddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddress.sin_port = htons((unsigned short)I104M_WRITEUDPPORT);
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


void MySOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<Binary, uint16_t>>& meas)
{
	int packet_size;
	int max_pointspkt = PKTDIG_MAXPOINTS;

	//Print(header, meas, tsmode);

	PrintHeaderInfo(header, tsmode);
	NoDataCntTime = 0; // mantém link vivo

	// monta buffer de mensagem para OSHMI no formato I104M
	t_msgsupsq msg;
	msg.signature = MSGSUPSQ_SIG;
	msg.prim = MasterAddress;
	msg.sec = SlaveAddress;
	msg.causa = ((static_cast<int>(header.type)) == static_cast<int>(GroupVariationType::EVENT)) ? 3 : 20;

	int count = 0;

	if ((static_cast<int>(header.type)) == static_cast<int>(GroupVariationType::STATIC)
		|| 
		(((static_cast<int>(header.type)) == static_cast<int>(GroupVariationType::EVENT)) && (static_cast<int>(header.variation)) == 1) // event without time
		) 
	{
		msg.tipo = 1;
		msg.taminfo = sizeof(digital_notime_seq); // value size for the type (not counting the 4 byte address)
		meas.foreach([&](const IndexedValue<Binary, uint16_t>& pair)
		{
			// acerta endereco do ponto
			unsigned int * paddr = (unsigned int *)(msg.info + (count  % max_pointspkt) * (sizeof(int) + sizeof(digital_notime_seq)));
			*paddr = pair.index + OFFSET_DIGITAL; //considera offset para o grupo

			// valor e qualidade
			digital_notime_seq * obj = (digital_notime_seq *)(paddr + 1);
			obj->iq = xlatequalif(pair.value.quality);
			obj->iq |= pair.value.value? 1 : 0;
			// if (count < 20)
				std::cout << "[" << pair.index << "] : " <<
				ValueToString(pair.value) << " : " <<
				static_cast<int>(pair.value.quality) << " : " <<
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
		if ((static_cast<int>(header.type)) == static_cast<int>(GroupVariationType::EVENT)) // event
	{
		msg.taminfo = sizeof(digital_w_time7_seq); // value size for the type (not counting the 4 byte address)
		meas.foreach([&](const IndexedValue<Binary, uint16_t>& pair)
		{
			max_pointspkt = PKTEVE_MAXPOINTS;
			msg.tipo = 30;

			// acerta endereco do ponto
			unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(digital_w_time7_seq)));
			*paddr = pair.index + OFFSET_DIGITAL; // considera offset para o grupo

			// valor e qualidade
			digital_w_time7_seq * obj = (digital_w_time7_seq *)(paddr + 1);
			obj->iq = xlatequalif(pair.value.quality);
			obj->iq |= pair.value.value ? 1 : 0;

			time_t tmi = pair.value.time / 1000;
			struct tm *unxtm = localtime( &tmi );
			//struct tm *unxtm = gmtime(&tmi);

			obj->ano = unxtm->tm_year % 100;
			obj->mes = unxtm->tm_mon + 1;
			obj->dia = unxtm->tm_mday;
			obj->hora = unxtm->tm_hour;
			obj->min = unxtm->tm_min;
			obj->ms = unxtm->tm_sec * 1000 + pair.value.time % 1000;

			// if (count < 20)
				std::cout << "[" << pair.index << "] : " <<
				ValueToString(pair.value) << " : " <<
				static_cast<int>(pair.value.quality) << " : " <<
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

void MySOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<DoubleBitBinary, uint16_t>>& meas)
{
	int packet_size;
	int max_pointspkt = PKTDIG_MAXPOINTS;

	//Print(header, meas, tsmode);

	PrintHeaderInfo(header, tsmode);
	NoDataCntTime = 0; // mantém link vivo

	// monta buffer de mensagem para OSHMI no formato I104M
	t_msgsupsq msg;
	msg.signature = MSGSUPSQ_SIG;
	msg.prim = MasterAddress;
	msg.sec = SlaveAddress; 
	msg.causa = ((static_cast<int>(header.type)) == static_cast<int>(GroupVariationType::EVENT)) ? 3 : 20;

	int count = 0;

	if ((static_cast<int>(header.type)) == static_cast<int>(GroupVariationType::STATIC)
		||
		(((static_cast<int>(header.type)) == static_cast<int>(GroupVariationType::EVENT)) && (static_cast<int>(header.variation)) == 1) // event without time
		)
	{
		msg.tipo = 3;
		msg.taminfo = sizeof(digital_notime_seq); // value size for the type (not counting the 4 byte address)
		meas.foreach([&](const IndexedValue<DoubleBitBinary, uint16_t>& pair)
		{
			// acerta endereco do ponto
			unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(digital_notime_seq)));
			*paddr = pair.index + OFFSET_DIGITALDBL; // considera offset para o grupo

			// valor e qualidade
			digital_notime_seq * obj = (digital_notime_seq *)(paddr + 1);
			obj->iq = xlatequalif(pair.value.quality);
			obj->iq |= (static_cast<int>(pair.value.value)) ? 1 : 0;

			//if (count < 20)
				std::cout << "[" << pair.index << "] : " <<
				ValueToString(pair.value) << " : " <<
				static_cast<int>(pair.value.quality) << " : " <<
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
		if ((static_cast<int>(header.type)) == static_cast<int>(GroupVariationType::EVENT)) // event
		{
			max_pointspkt = PKTEVE_MAXPOINTS;
			msg.taminfo = sizeof(digital_w_time7_seq); // value size for the type (not counting the 4 byte address)
			meas.foreach([&](const IndexedValue<DoubleBitBinary, uint16_t>& pair)
			{
				msg.tipo = 31;

				// acerta endereco do ponto
				unsigned int * paddr = (unsigned int *)(msg.info + (count % max_pointspkt) * (sizeof(int) + sizeof(digital_w_time7_seq)));
				*paddr = pair.index + OFFSET_DIGITALDBL; // considera offset para o grupo

				// valor e qualidade
				digital_w_time7_seq * obj = (digital_w_time7_seq *)(paddr + 1);
				obj->iq = xlatequalif(pair.value.quality);
				obj->iq |= (static_cast<int>(pair.value.value)) ? 1 : 0;

				time_t tmi = pair.value.time / 1000;
				struct tm *unxtm = localtime(&tmi);
				//struct tm *unxtm = gmtime(&tmi);

				obj->ano = unxtm->tm_year % 100;
				obj->mes = unxtm->tm_mon + 1;
				obj->dia = unxtm->tm_mday;
				obj->hora = unxtm->tm_hour;
				obj->min = unxtm->tm_min;
				obj->ms = unxtm->tm_sec * 1000 + pair.value.time % 1000;

				//if (count < 20)
					std::cout << "[" << pair.index << "] : " <<
					ValueToString(pair.value) << " : " <<
					static_cast<int>(pair.value.quality) << " : " <<
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

void MySOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<Analog, uint16_t>>& meas)
{
	int packet_size;

	//Print(header, meas, tsmode);

	PrintHeaderInfo(header, tsmode);
	NoDataCntTime = 0; // mantém link vivo

	// monta buffer de mensagem para OSHMI no formato I104M
	t_msgsupsq msg;
	msg.signature = MSGSUPSQ_SIG;
	msg.prim = MasterAddress;
	msg.sec = SlaveAddress;
	msg.causa = ((static_cast<int>(header.type)) == static_cast<int>(GroupVariationType::EVENT)) ? 3 : 20;

	int count = 0;

	msg.tipo = 13;
	msg.taminfo = sizeof(flutuante_seq); // value size for the type (not counting the 4 byte address)
	meas.foreach([&](const IndexedValue<Analog, uint16_t>& pair)
	{
		// acerta endereco do ponto
		unsigned int * paddr = (unsigned int *)(msg.info + (count % PKTANA_MAXPOINTS) * (sizeof(int) + sizeof(flutuante_seq)));
		*paddr = pair.index + OFFSET_ANALOG; // considera offset para o grupo

		// valor e qualidade
		flutuante_seq * obj = (flutuante_seq *)(paddr + 1);
		obj->fr = pair.value.value;
		obj->qds = xlatequalif(pair.value.quality);

		// if (count < 20)
			std::cout << "[" << pair.index << "] : " <<
			pair.value.value << " : " <<
			static_cast<int>(pair.value.quality) << " : " <<
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

void MySOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<Counter, uint16_t>>& meas)
{
	Print(header, meas, tsmode);
	NoDataCntTime = 0; // mantém link vivo
}

void MySOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<FrozenCounter, uint16_t>>& meas)
{
	Print(header, meas, tsmode);
	NoDataCntTime = 0; // mantém link vivo
}

void MySOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<BinaryOutputStatus, uint16_t>>& meas)
{
	Print(header, meas, tsmode);
	NoDataCntTime = 0; // mantém link vivo
}

void MySOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<AnalogOutputStatus, uint16_t>>& meas)
{
	Print(header, meas, tsmode);
	NoDataCntTime = 0; // mantém link vivo
}

void MySOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<OctetString, uint16_t>>& meas)
{
	this->PrintHeaderInfo(header, tsmode);
	NoDataCntTime = 0; // mantém link vivo

	meas.foreach([&](const IndexedValue<OctetString, uint16_t>& pair)
	{
		std::cout << "OctetString " <<  " [" << pair.index << "] : Size : " << pair.value.ToReadOnly().Size() << std::endl;
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
