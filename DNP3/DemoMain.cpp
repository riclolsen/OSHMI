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

#define DRIVER_VERSION "OSHMI DNP3 DRIVER V0.4 - Copyright 2016-2018 - Ricardo Lastra Olsen"

#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/PrintingSOEHandler.h>
#include <asiodnp3/ConsoleLogger.h>
#include <asiodnp3/DefaultMasterApplication.h>
#include <asiodnp3/BlockingCommandCallback.h>

#include <asiopal/UTCTimeSource.h>

#include <opendnp3/LogLevels.h>
#include <opendnp3/app/ControlRelayOutputBlock.h>

#include <thread>
#include <vector>
#include <string>
#include <iostream>
#include <conio.h>

#include "IniFile.h"
#include "MySOEHandler.h"

#define DNP3INI "c:\\oshmi\\conf\\dnp3.ini"
#define OSHMIINI "c:\\oshmi\\conf\\hmi.ini"

#define KEEP_ALIVE_TIME 5
#define MAX_SLAVES 50

using namespace std;
using namespace openpal;
using namespace asiopal;
using namespace asiodnp3;
using namespace opendnp3;

// wait while receiving keep alive messages
void waittobe_active(int shandle)
{
	int seconds_wait = KEEP_ALIVE_TIME * 3.5;
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
		{ // passou o tempo sem receber mensagens, retorna para ativar
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

int main(int argc, char* argv[])
{
	vector <asiodnp3::IChannel *> pChannelVec;
	MasterStackConfig stackConfigVec[MAX_SLAVES];
	MySOEHandler soehVec[MAX_SLAVES];
	vector <asiodnp3::IMaster *> pMasterVec;
	vector <opendnp3::MasterScan> integrityScanVec;
	vector <opendnp3::MasterScan> exceptionScanVec;
	vector <opendnp3::MasterScan> rangeScan1Vec;
	vector <opendnp3::MasterScan> rangeScan2Vec;
	vector <opendnp3::MasterScan> rangeScan3Vec;
	vector <opendnp3::MasterScan> rangeScan4Vec;
	vector <opendnp3::ICommandProcessor *> pCommandProcessorVec;
	int masterAddress, slaveAddress;

	std::cout << DRIVER_VERSION << std::endl;

	CIniFile hmi_iniFile(OSHMIINI);
	hmi_iniFile.ReadFile();
	string IPAddrRed = hmi_iniFile.GetValue("REDUNDANCY", "OTHER_HMI_IP"); // ip address of redundant hmi
	
	CIniFile iniFile(DNP3INI);
	iniFile.ReadFile();

	masterAddress = iniFile.GetValueI("MASTER", "LINK_ADDRESS");
	if (masterAddress == 0)
		exit(1);

	std::cout << "Starting inactive mode..." << std::endl;
	if (IPAddrRed != "") // se tiver redundancia, espera ficar ativo (sem receber mensagem de keep alive por um tempo)
		waittobe_active(soehVec[0].shandle);

	std::cout << "Entering active mode..." << std::endl;

	// Specify what log levels to use. NORMAL is warning and above
	// You can add all the comms logging by uncommenting below
	const uint32_t FILTERS = levels::NORMAL | levels::ALL_COMMS;

	// This is the main point of interaction with the stack
	DNP3Manager manager(std::thread::hardware_concurrency());

	// send log messages to the console
	manager.AddLogSubscriber(&ConsoleLogger::Instance());

	int cntslaves = 0;
	char buffer[20];
	string IPAddrAnt;
	string IPAddr;
	string range_scan1;
	string range_scan2;
	string range_scan3;
	string range_scan4;

	while ( (slaveAddress = iniFile.GetValueI( ((string)"SLAVE") + itoa(1+cntslaves, buffer, 10), "LINK_ADDRESS")) != 0 )
	{
		IPAddr = iniFile.GetValue(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "IP_ADDRESS");
		int IPPort = iniFile.GetValueI(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "IP_PORT", 20000);
		int enable_unsolicited = iniFile.GetValueI(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "ENABLE_UNSOLICITED", 1);
		int integrity_scan = iniFile.GetValueI(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "INTEGRITY_SCAN", 180);
		int class1_scan = iniFile.GetValueI(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "CLASS1_SCAN", 5);
		int class2_scan = iniFile.GetValueI(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "CLASS2_SCAN", 17);
		int class3_scan = iniFile.GetValueI(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "CLASS3_SCAN", 29);
		int response_timeout = iniFile.GetValueI(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "RESPONSE_TIMEOUT", 2);
		int time_sync = iniFile.GetValueI(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "TIME_SYNC", 1);
		int nodata_timeout = iniFile.GetValueI(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "NODATA_TIMEOUT", 300);
		string range_scan1 = iniFile.GetValue(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "RANGE_SCAN_1");
		string range_scan2 = iniFile.GetValue(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "RANGE_SCAN_2");
		string range_scan3 = iniFile.GetValue(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "RANGE_SCAN_3");
		string range_scan4 = iniFile.GetValue(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "RANGE_SCAN_4");

		if (IPAddr == "")
			exit(2);

		// um canal por IP
		if (IPAddr == IPAddrAnt) // enquanto repetir o ip, mantém o mesmo canal na lista. Os escravos de mesmo IP devem estar em sequência no ini!
		{
			pChannelVec.push_back( pChannelVec[cntslaves - 1] );
		}
		else
		{	
			// Connect via a TCPClient socket to a outstation	
			pChannelVec.push_back(manager.AddTCPClient("tcpclient", FILTERS, TimeDuration::Seconds(2), TimeDuration::Seconds(5), IPAddr.c_str(), IPPort));
			// para serial usaria AddSerial
		}

		IPAddrAnt = IPAddr;  // guarda ip anterior

		// The master config object for a master. The default are
		// useable, but understanding the options are important.

		// you can override application layer settings for the master here
		// in this example, we've change the application layer timeout to 2 seconds
		stackConfigVec[cntslaves].master.responseTimeout = TimeDuration::Seconds(response_timeout);
		stackConfigVec[cntslaves].master.disableUnsolOnStartup = !enable_unsolicited;
		stackConfigVec[cntslaves].master.timeSyncMode = (time_sync > 0) ? TimeSyncMode::SerialTimeSync : TimeSyncMode::None;

		// You can override the default link layer settings here
		// in this example we've changed the default link layer addressing
		stackConfigVec[cntslaves].link.LocalAddr = masterAddress;
		stackConfigVec[cntslaves].link.RemoteAddr = slaveAddress;

		soehVec[cntslaves].SetSlaveNo(cntslaves);
		soehVec[cntslaves].SetNoDataTimeout(nodata_timeout);
		soehVec[cntslaves].SetMasterAddress(masterAddress);
		soehVec[cntslaves].SetSlaveAddress(slaveAddress);
		if (IPAddrRed != "")
		  soehVec[cntslaves].SetRedundantHMIAddress(IPAddrRed.c_str());

		// Um mestre para cada escravo

		// Create a new master on a previously declared port, with a
		// name, log level, command acceptor, and config info. This
		// returns a thread-safe interface used for sending commands.
		pMasterVec.push_back(pChannelVec[cntslaves]->AddMaster(
								"master", // id for logging
								soehVec[cntslaves], // callback for data processing                
								asiodnp3::DefaultMasterApplication::Instance(),	// master application instance
								stackConfigVec[cntslaves] // stack configuration
							));

		// do an integrity poll (Class 3/2/1/0) every N seconds
		if (integrity_scan == 0)
          integrity_scan = 0x7FFFFFFF;
		integrityScanVec.push_back( pMasterVec[cntslaves]->AddClassScan(ClassField::AllClasses(), TimeDuration::Seconds(integrity_scan)) );
		
		// do a Class 1 exception poll every X seconds
		if (class1_scan == 0)
			class1_scan = 0x7FFFFFFF;
        exceptionScanVec.push_back(pMasterVec[cntslaves]->AddClassScan(ClassField(ClassField::CLASS_1), TimeDuration::Seconds(class1_scan)));

		// do a Class 2 exception poll every Y seconds
		if (class2_scan == 0)
			class2_scan = 0x7FFFFFFF;
		exceptionScanVec.push_back(pMasterVec[cntslaves]->AddClassScan(ClassField(ClassField::CLASS_2), TimeDuration::Seconds(class2_scan)));

		// do a Class 3 exception poll every X seconds
        if (class3_scan == 0)
		  class3_scan = 0x7FFFFFFF;
		exceptionScanVec.push_back(pMasterVec[cntslaves]->AddClassScan(ClassField(ClassField::CLASS_3), TimeDuration::Seconds(class3_scan)));

		if (range_scan1 != "")
		{
			stringstream ss(range_scan1);
			string sub;
			getline(ss, sub, ','); int group = std::stoi(sub);
			getline(ss, sub, ','); int variation = std::stoi(sub);
			getline(ss, sub, ','); int start = std::stoi(sub);
			getline(ss, sub, ','); int stop = std::stoi(sub);
			getline(ss, sub, ','); int period = std::stoi(sub);
			if (period == 0)
				period = 0x7FFFFFFF;
			rangeScan1Vec.push_back(pMasterVec[cntslaves]->AddRangeScan(GroupVariationID(group, variation), start, stop, TimeDuration::Seconds(period)));
		}
		else
			rangeScan1Vec.push_back(pMasterVec[cntslaves]->AddRangeScan(GroupVariationID(1, 0), 0, 0, TimeDuration::Seconds(0x7FFFFFFF)));

		if (range_scan2 != "")
		{
			stringstream ss(range_scan2);
			string sub;
			getline(ss, sub, ','); int group = std::stoi(sub);
			getline(ss, sub, ','); int variation = std::stoi(sub);
			getline(ss, sub, ','); int start = std::stoi(sub);
			getline(ss, sub, ','); int stop = std::stoi(sub);
			getline(ss, sub, ','); int period = std::stoi(sub);
			if (period == 0)
				period = 0x7FFFFFFF;
			rangeScan2Vec.push_back(pMasterVec[cntslaves]->AddRangeScan(GroupVariationID(group, variation), start, stop, TimeDuration::Seconds(period)));
		}
		else
			rangeScan2Vec.push_back(pMasterVec[cntslaves]->AddRangeScan(GroupVariationID(1, 0), 0, 0, TimeDuration::Seconds(0x7FFFFFFF)));

		if (range_scan3 != "")
		{
			stringstream ss(range_scan3);
			string sub;
			getline(ss, sub, ','); int group = std::stoi(sub);
			getline(ss, sub, ','); int variation = std::stoi(sub);
			getline(ss, sub, ','); int start = std::stoi(sub);
			getline(ss, sub, ','); int stop = std::stoi(sub);
			getline(ss, sub, ','); int period = std::stoi(sub);
			if (period == 0)
				period = 0x7FFFFFFF;
			rangeScan3Vec.push_back(pMasterVec[cntslaves]->AddRangeScan(GroupVariationID(group, variation), start, stop, TimeDuration::Seconds(period)));
		}
		else
			rangeScan3Vec.push_back(pMasterVec[cntslaves]->AddRangeScan(GroupVariationID(1, 0), 0, 0, TimeDuration::Seconds(0x7FFFFFFF)));

		if (range_scan4 != "")
		{
			stringstream ss(range_scan4);
			string sub;
			getline(ss, sub, ','); int group = std::stoi(sub);
			getline(ss, sub, ','); int variation = std::stoi(sub);
			getline(ss, sub, ','); int start = std::stoi(sub);
			getline(ss, sub, ','); int stop = std::stoi(sub);
			getline(ss, sub, ','); int period = std::stoi(sub);
			if (period == 0)
				period = 0x7FFFFFFF;
			rangeScan4Vec.push_back(pMasterVec[cntslaves]->AddRangeScan(GroupVariationID(group, variation), start, stop, TimeDuration::Seconds(period)));
		}
		else
			rangeScan4Vec.push_back(pMasterVec[cntslaves]->AddRangeScan(GroupVariationID(1, 0), 0, 0, TimeDuration::Seconds(0x7FFFFFFF)));

		// Enable the master. This will start communications.
		pMasterVec[cntslaves]->Enable();

		pCommandProcessorVec.push_back( pMasterVec[cntslaves]->GetCommandProcessor() );

		// Optionally, you can bind listeners to the channel to get state change notifications
		// This listener just prints the changes to the console
		pChannelVec[cntslaves]->AddStateListener([cntslaves, slaveAddress, masterAddress, &soehVec](ChannelState state)
		{
			std::cout << "Channel slave state: " << cntslaves << " - " << ChannelStateToString(state) << std::endl;
			soehVec[cntslaves].SendOSHMIChannelStatePoint(state == ChannelState::OPEN);
		});

		cntslaves++;
		
		// when no more slaves found
		if (iniFile.GetValueI(((string)"SLAVE") + itoa(1 + cntslaves, buffer, 10), "LINK_ADDRESS") == 0)
		{
			time_t timeant = 0;

			// keep receiving UDP packets
			while (true)
			{
				char cmd = 0;
				if (_kbhit())
				{
					std::cout << "Enter a command" << std::endl;
					std::cout << "x - exits program" << std::endl;
					std::cout << "a - about program" << std::endl;
					std::cout << "i - integrity demand scan" << std::endl;
					std::cout << "e - exception demand scan" << std::endl;
					std::cout << "r - restart connections" << std::endl;
					cmd = _getch();
				}

				switch (cmd)
				{
				case('x'):
					// C++ destructor on DNP3Manager cleans everything up for you
					return 0;
				case('a'):
					std::cout << DRIVER_VERSION << std::endl;
					break;
				case('i'):
					for (int i = 0; i < cntslaves; i++)
						integrityScanVec[i].Demand();
					break;
				case('e'):
					for (int i = 0; i < cntslaves; i++)
						exceptionScanVec[i].Demand();
					break;
				case('r'):
					for (int i = 0; i < cntslaves; i++)
						pMasterVec[i]->Disable();
					Sleep(2000);
					for (int i = 0; i < cntslaves; i++)
						pMasterVec[i]->Enable();
					break;
				case 0:
					break;
				default:
					std::cout << "Unknown action: " << cmd << std::endl;
					break;
				}

				time_t now;
				time(&now);
				if (difftime(now, timeant) > 5)
				{
					timeant = now;
					if (IPAddrRed != "")
						soehVec[0].SendKeepAlive();
					for (int i = 0; i < cntslaves; i++)
					{
						soehVec[i].NoDataTimeoutCnt(5);
						if (soehVec[i].NoDataState)
						{
							pMasterVec[i]->Disable();
							Sleep(500);
    						pMasterVec[i]->Enable();
						}
					}
				}

			    unsigned char packet_data[1500];
				unsigned int max_packet_size =	sizeof(packet_data);

#if PLATFORM == PLATFORM_WINDOWS
				typedef int socklen_t;
#endif
				sockaddr_in from;
				socklen_t fromLength = sizeof(from);

				int bytes = recvfrom(soehVec[0].shandle,
					(char*)packet_data,
					max_packet_size,
					0,
					(sockaddr*)&from,
					&fromLength);

				if (bytes <= 0)
					continue;
				unsigned int from_address =
					ntohl(from.sin_addr.s_addr);
				unsigned int from_port =
					ntohs(from.sin_port);

				t_msgcmd * pmsg = (t_msgcmd *)packet_data;

				if (pmsg->signature == MSGKA_SIG)
				{ // message from other active driver, should not happen, exit
					std::cout << "Message from other active driver! Exiting..." << std::endl;
					exit(1);
				}
				else
				if (pmsg->signature == MSGCMD_SIG)
				switch (pmsg->tipo)
				{
				case 45:; // digital single command
					{
						ControlCode cc = (ControlCode)pmsg->qu;
						switch (cc)
						{
						case ControlCode::NUL:
						case ControlCode::LATCH_ON:
						case ControlCode::LATCH_OFF:
						default:
							if (pmsg->onoff)
								cc = ControlCode::LATCH_ON;
							else
								cc = ControlCode::LATCH_OFF;
							break;
						case ControlCode::PULSE_CLOSE:
						case ControlCode::PULSE_TRIP:
							if (pmsg->onoff)
								cc = ControlCode::PULSE_CLOSE;
							else
								cc = ControlCode::PULSE_TRIP;
							break;
						case ControlCode::PULSE:
								cc = ControlCode::PULSE;
							break;
						}
						for (int i = 0; i < cntslaves; i++)
						{
							if (stackConfigVec[i].link.RemoteAddr == pmsg->utr) // encontra a utr
							{
								ControlRelayOutputBlock crob(cc);
								BlockingCommandCallback handler;
								int index = pmsg->endereco - OFFSET_DIGITAL_OUT;

								if (index == 3999) // on demand integrity scan by a command
								{
									integrityScanVec[i].Demand();
								}
								else
								if (index == 3998) // on demand range 1 scan by a command
								{
									rangeScan1Vec[i].Demand();
								}
								else
								if (index == 3997) // on demand range 2 scan by a command
								{
									rangeScan2Vec[i].Demand();
								}
								else
								if (index == 3996) // on demand range 3 scan by a command
								{
									rangeScan3Vec[i].Demand();
								}
								else
								if (index == 3995) // on demand range 4 scan by a command
								{
									rangeScan4Vec[i].Demand();
								}
								else
								{
									if (pmsg->sbo)
										pCommandProcessorVec[i]->SelectAndOperate(crob, index, handler);
									else
										pCommandProcessorVec[i]->DirectOperate(crob, index, handler);
									auto response = handler.WaitForResult();
									std::cout << "OSHMI Cmd-> RTU:" << pmsg->utr << " Addr:" << index << " " << ControlCodeToString(cc) << " SBO:" << pmsg->sbo
										<< " Res:" << CommandResultToString(response.GetResult()) <<
										" Sts:" << CommandStatusToString(response.GetStatus()) << std::endl;

									// retorna confirmação como asdu de comando para cima
									t_msgsup msg;
									msg.causa = (response.GetStatus() == CommandStatus::SUCCESS) ? 0x00 : 0x40;
									msg.endereco = pmsg->endereco;
									msg.tipo = pmsg->tipo;
									msg.prim = stackConfigVec[i].link.LocalAddr;
									msg.sec = stackConfigVec[i].link.RemoteAddr;
									msg.signature = MSGSUP_SIG;
									msg.taminfo = 1;
									msg.info[0] = pmsg->onoff;
									int packet_size = sizeof(int) * 7 + 1;
									soehVec[i].SendOSHMI(&msg, packet_size);
									break;
								}
							}
						}

					}
						break;
					case 49: // comando se setpoint escalado (scaled)
						for (int i = 0; i < cntslaves; i++)
						{
							if (stackConfigVec[i].link.RemoteAddr == pmsg->utr) // encontra a utr
							{
								AnalogOutputInt16 ao(pmsg->setpoint);
								BlockingCommandCallback handler;
								int index = pmsg->endereco - OFFSET_ANALOG_OUT;
								if (pmsg->sbo)
									pCommandProcessorVec[i]->SelectAndOperate(ao, index, handler);
								else
									pCommandProcessorVec[i]->DirectOperate(ao, index, handler);
								auto response = handler.WaitForResult();
								std::cout << "OSHMI Cmd-> RTU:" << pmsg->utr << " Addr:" << index << " " << (short)pmsg->setpoint << " SBO:" << pmsg->sbo
									<< " Res:" << CommandResultToString(response.GetResult()) <<
									" Sts:" << CommandStatusToString(response.GetStatus()) << std::endl;

								// retorna confirmação como asdu de comando para cima
								t_msgsup msg;
								msg.causa = (response.GetStatus() == CommandStatus::SUCCESS) ? 0x00 : 0x40;
								msg.endereco = pmsg->endereco;
								msg.tipo = pmsg->tipo;
								msg.prim = stackConfigVec[i].link.LocalAddr;
								msg.sec = stackConfigVec[i].link.RemoteAddr;
								msg.signature = MSGSUP_SIG;
								msg.taminfo = sizeof(short);
								*(short *)msg.info = (short)pmsg->setpoint;
								int packet_size = sizeof(int) * 7 + sizeof(short);
								soehVec[i].SendOSHMI(&msg, packet_size);
								break;
							}
						}
						break;
					case 50: // comando de setpoint ponto flutuante
						for (int i = 0; i < cntslaves; i++)
						{
							if (stackConfigVec[i].link.RemoteAddr == pmsg->utr) // encontra a utr
							{
								AnalogOutputFloat32 ao(pmsg->setpoint);
								BlockingCommandCallback handler;
								int index = pmsg->endereco - OFFSET_ANALOG_OUT;
								if (pmsg->sbo)
									pCommandProcessorVec[i]->SelectAndOperate(ao, index, handler);
								else
									pCommandProcessorVec[i]->DirectOperate(ao, index, handler);
								auto response = handler.WaitForResult();
								std::cout << "OSHMI Cmd-> RTU:" << pmsg->utr << " Addr:" << index << " " << pmsg->setpoint << " SBO:" << pmsg->sbo
									<< " Res:" << CommandResultToString(response.GetResult()) <<
									" Sts:" << CommandStatusToString(response.GetStatus()) << std::endl;

								// retorna confirmação como asdu de comando para cima
								t_msgsup msg;
								msg.causa = (response.GetStatus() == CommandStatus::SUCCESS) ? 0x00 : 0x40;
								msg.endereco = pmsg->endereco;
								msg.tipo = pmsg->tipo;
								msg.prim = stackConfigVec[i].link.LocalAddr;
								msg.sec = stackConfigVec[i].link.RemoteAddr;
								msg.signature = MSGSUP_SIG;
								msg.taminfo = sizeof(float);
								*(float *)msg.info = pmsg->setpoint;
								int packet_size = sizeof(int) * 7 + sizeof(float);
								soehVec[i].SendOSHMI(&msg, packet_size);
								break;
							}
						}
						break;
				} // switch
			} // while
		}
	}

	return 0;
}

/*
int main2(int argc, char* argv[])
{
	CIniFile iniFile("./dnp3.ini");
	iniFile.ReadFile();

	// Specify what log levels to use. NORMAL is warning and above
	// You can add all the comms logging by uncommenting below
	const uint32_t FILTERS = levels::NORMAL; // | levels::ALL_COMMS;

	// This is the main point of interaction with the stack
	DNP3Manager manager(std::thread::hardware_concurrency());

	// send log messages to the console
	manager.AddLogSubscriber(&ConsoleLogger::Instance());

	// Connect via a TCPClient socket to a outstation	
	auto pChannel = manager.AddTCPClient("tcpclient", FILTERS, TimeDuration::Seconds(2), TimeDuration::Seconds(5), "127.0.0.1", 20000);

	// Optionally, you can bind listeners to the channel to get state change notifications
	// This listener just prints the changes to the console
	pChannel->AddStateListener([](ChannelState state)
	{
		std::cout << "channel state: " << ChannelStateToString(state) << std::endl;
	});

	// The master config object for a master. The default are
	// useable, but understanding the options are important.
	MasterStackConfig stackConfig;

	// you can override application layer settings for the master here
	// in this example, we've change the application layer timeout to 2 seconds
	stackConfig.master.responseTimeout = TimeDuration::Seconds(2);
	stackConfig.master.disableUnsolOnStartup = false;

	// You can override the default link layer settings here
	// in this example we've changed the default link layer addressing
	stackConfig.link.LocalAddr = 1;
	stackConfig.link.RemoteAddr = 10;

	MySOEHandler &soeh = MySOEHandler::Instance();
	soeh.SetMasterAddress(stackConfig.link.LocalAddr);
	soeh.SetSlaveAddress(stackConfig.link.RemoteAddr);

	// Create a new master on a previously declared port, with a
	// name, log level, command acceptor, and config info. This
	// returns a thread-safe interface used for sending commands.
	auto pMaster = pChannel->AddMaster(
		"master",										// id for logging
		soeh,					    // callback for data processing                
		asiodnp3::DefaultMasterApplication::Instance(),	// master application instance
		stackConfig										// stack configuration
		);

	// do an integrity poll (Class 3/2/1/0) once per minute
	auto integrityScan = pMaster->AddClassScan(ClassField::AllClasses(), TimeDuration::Minutes(1));

	// do a Class 1 exception poll every 5 seconds
	auto exceptionScan = pMaster->AddClassScan(ClassField(ClassField::CLASS_1), TimeDuration::Seconds(5));

	// Enable the master. This will start communications.
	pMaster->Enable();

	auto pCommandProcessor = pMaster->GetCommandProcessor();

	do
	{

		std::cout << "Enter a command" << std::endl;
		std::cout << "x - exits program" << std::endl;
		std::cout << "i - integrity demand scan" << std::endl;
		std::cout << "e - exception demand scan" << std::endl;
		std::cout << "c - send crob" << std::endl;

		char cmd;
		std::cin >> cmd;
		switch (cmd)
		{
		case('x') :
			// C++ destructor on DNP3Manager cleans everything up for you
			return 0;
		case('i') :
			integrityScan.Demand();
			break;
		case('e') :
			exceptionScan.Demand();
			break;
		case('c') :
		{
			// This is an example of synchronously doing a control operation
			ControlRelayOutputBlock crob(ControlCode::LATCH_ON);
			BlockingCommandCallback handler;
			pCommandProcessor->SelectAndOperate(crob, 0, handler);
			auto response = handler.WaitForResult();
			std::cout << "Result: " << CommandResultToString(response.GetResult()) <<
				" Status: " << CommandStatusToString(response.GetStatus()) << std::endl;
			break;
		}
		default:
			std::cout << "Unknown action: " << cmd << std::endl;
			break;
		}
	} while (true);

return 0;
}
*/

