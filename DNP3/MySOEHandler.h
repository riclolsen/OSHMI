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
 * 
 * Modified and integrated into OSHMI by Ricardo Lastra Olsen (2016-2023).
 *
 */

#ifndef ASIODNP3_MYSOEHANDLER_H
#define ASIODNP3_MYSOEHANDLER_H

#include <opendnp3/master/ISOEHandler.h>

#include <iostream>
#include <sstream>

#define DNP3INI "c:\\oshmi\\conf\\dnp3.ini"
#define OSHMIINI "c:\\oshmi\\conf\\hmi.ini"

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

#include "oshmi_types.h"

#define OFFSET_DIGITAL        1000
#define OFFSET_DIGITALDBL    50000
#define OFFSET_ANALOG        60000
#define OFFSET_COUNTER       80000
#define OFFSET_FROZENCOUNTER 85000
#define OFFSET_DIGITAL_OUT   90000
#define OFFSET_ANALOG_OUT    94000
#define OFFSET_DIGITAL_OUTST 95000
#define OFFSET_ANALOG_OUTST  99000

#define PKTDIG_MAXPOINTS       250
#define PKTEVE_MAXPOINTS       100
#define PKTANA_MAXPOINTS       150      



namespace opendnp3
{

class MySOEHandler : public ISOEHandler
{

public:
	static int shandle; // handle para udp socket
	sockaddr_in saddress; // socket address
	sockaddr_in saddress_red; // socket address for redundant hmi
	sockaddr_in saddress_red_driver; // socket address for redundant driver (different port from hmi)
	bool redundant_hmi = false; // there is redundant hmi?
	int SlaveAddress = 0;
	int MasterAddress = 0;
	int NoDataCntTime = 0;
	bool NoDataState = false;
	int NoDataTimeout = 0;
	int SlaveNo = -1;
    int I104MListenPort = I104M_LISTENUDPPORT_DEFAULT;
	
	void SetSlaveNo(int no)
	{
		SlaveNo = no;
	}

	void SetNoDataTimeout(int seconds)
	{
		NoDataTimeout = seconds;
	}

	void NoDataTimeoutCnt(int seconds) 
	{
		if (NoDataTimeout == 0) // timeout 0: do not control no data timeout
			return;

		NoDataCntTime += seconds;
		if (NoDataCntTime > NoDataTimeout)
		{
			NoDataState = true;
			SendOSHMIChannelStatePoint(false);
			std::cout << "channel state: " << SlaveNo +  " - no data comming from RTU" << std::endl;
		}
		else
		{
			if (NoDataState == true)
			{
			SendOSHMIChannelStatePoint(true);
			std::cout << "channel state: " << SlaveNo + " - data is now comming from RTU" << std::endl;
		    }

			NoDataState = false;
		}
	}

	void SetSlaveAddress(int addr)
	{
		SlaveAddress = addr;
	}

	void SetMasterAddress(int addr)
	{
		MasterAddress = addr;
	}

	void SetRedundantHMIAddress(const char * ipaddr)
	{
		saddress_red.sin_family = AF_INET;
		saddress_red.sin_addr.s_addr = inet_addr(ipaddr);
		saddress_red.sin_port = htons((unsigned short)I104M_WRITEUDPPORT);

		saddress_red_driver.sin_family = AF_INET;
		saddress_red_driver.sin_addr.s_addr = inet_addr(ipaddr);
		saddress_red_driver.sin_port = htons((unsigned short)I104MListenPort);

		redundant_hmi = true;
	}
   
	void Process(const HeaderInfo& info, const ICollection<Indexed<Binary>>& values) override;
	void Process(const HeaderInfo& info, const ICollection<Indexed<DoubleBitBinary>>& values) override;
	void Process(const HeaderInfo& info, const ICollection<Indexed<Analog>>& values) override;
	void Process(const HeaderInfo& info, const ICollection<Indexed<Counter>>& values) override;
	void Process(const HeaderInfo& info, const ICollection<Indexed<FrozenCounter>>& values) override;
	void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryOutputStatus>>& values) override;
	void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogOutputStatus>>& values) override;
	void Process(const HeaderInfo& info, const ICollection<Indexed<OctetString>>& values) override;

	void Process(const HeaderInfo& info, const ICollection<Indexed<TimeAndInterval>>& values) override {};
    void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryCommandEvent>>& values) override{};
    void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogCommandEvent>>& values) override{};
	// void Process(const HeaderInfo& info, const ICollection<Indexed<SecurityStat>>& values) override;
    void Process(const HeaderInfo& info, const ICollection<DNPTime>& values) override {};

		void BeginFragment(const ResponseInfo& info) override
    {
        // std::cout << "Begin receiving measurement data for outstation: " << SlaveAddress << std::endl;
    }

    void EndFragment(const ResponseInfo& info) override
    {
        // std::cout << "End receiving measurement data for outstation: " << SlaveAddress << std::endl;
    }

    void Init();
    unsigned char xlatequalif(int qdnp);
	void SendKeepAlive();
	void SendOSHMI(void * pmsg, int packet_size);
	void SendOSHMIChannelStatePoint(bool channel_ok);

	MySOEHandler();

protected:

	virtual void Start() {}
    virtual void End() {}

private:

	void PrintHeaderInfo(const HeaderInfo& header)
	{
        std::cout << "Header: "
                  << " Group" << GroupVariationSpec::to_human_string(header.gv);
        std::cout << QualifierCodeSpec::to_human_string(header.qualifier)
                  << " isEvent: " << header.isEventVariation
                  << " tsQuality: " << TimestampQualitySpec::to_human_string(header.tsquality)
			      << std::endl;
	}

	template <class T>
    std::string ValueToString(const T& meas)
	{
		std::ostringstream oss;
		oss << meas.value;
		return oss.str();
	}

};

}


#endif
