/*! \file DataPlane.cpp
 *  \brief
 *  \details
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 12:14:35 2013
 */


#include "DataPlane.hpp"
#include "ReportGlobals.hpp"



DataPlane::DataPlane(sc_module_name p_ModuleName, ControlPlaneConfig * const p_Config):sc_module(p_ModuleName), m_Config(p_Config), m_Forwarder("-Forwarder"), m_OutputPort(-1)
{
	setUp(true);
	m_Rpt.setBaseName(name());
	m_InterfaceCount = m_Config->getNumberOfInterfaces();
	SC_THREAD(main);
	sensitive << port_Clk.pos();
}

DataPlane::~DataPlane()
{

}


///This is where the BGP will sit on. Now there is only a simple example, which shows that the connections work.
void DataPlane::main(void)
{

	// SC_REPORT_INFO(g_ReportID, m_Rpt.newReportString("starting "));
	// m_Packet.setProtocolType(TYPE_IP);
	// m_Packet.setIPPayload("1");
	// port_ToInterface[0]->write(m_Packet);

	while(true)
	{
		wait();

		if(!isRunning())
			continue;
		//check all the interfaces
		for (int i = 0; i < m_InterfaceCount; i++)
		{

			if(port_FromInterface[i]->num_available() > 0)
			{

				port_FromInterface[i]->read(m_Packet);

				if(m_Packet.getProtocolType() == TYPE_IP)
				{


					//Check that the packet is valid
					if(m_Forwarder.processFrame(m_Packet))
					{
						//update the TTL field and re-calculate the checksum
						if(m_Forwarder.forward(&m_Packet))//forward only if TTL was not 0
						{

							//resolve route
							m_OutputPort = port_ToRoutingTable->resolveRoute(m_Forwarder.getDestination());

							//check that the route is found
							if(m_OutputPort >= 0)

								port_ToInterface[m_OutputPort]->write(m_Packet);
						}
					}

				}
				else if (m_Packet.getProtocolType() == TYPE_BGP)
				{
					m_BGPMsg = m_Packet.getBGPPayload();
					m_BGPMsg.m_OutboundInterface = i;

					//cout << name() << ": received BGP message: Type " << m_BGPMsg.m_Type << " Interface " << m_BGPMsg.m_OutboundInterface << " @" << sc_time_stamp() << endl;
					port_ToControlPlane->write(m_BGPMsg);
				}
			}


			if (m_BGPForwardingBuffer.num_available() > 0)
			{

				//cout << name() << ":: num_available in session buffer: " << m_BGPForwardingBuffer.num_available() << endl;
				// SC_REPORT_INFO(g_ReportID, m_Rpt.newReportString("Sending BGP message"));
				m_BGPForwardingBuffer.read(m_BGPMsg);
				//cout << name() << "    sending BGP message: AS " << m_BGPMsg.m_AS << " Interface " << m_BGPMsg.m_OutboundInterface << endl;
				m_Packet.setBGPPayload(m_BGPMsg);
				m_Packet.setProtocolType(TYPE_BGP);
				port_ToInterface[m_BGPMsg.m_OutboundInterface]->write(m_Packet);
			}
		}
		//Example how to resolve a route
		//port_ToRoutingTable->resolveRoute(1);

	}
}

void DataPlane::killDataPlane(void)
{
	setUp(false);
	m_Packet.clearPacket();
	m_OutputPort = -1;
	while(m_BGPForwardingBuffer.num_available() > 0)
		m_BGPForwardingBuffer.read(m_BGPMsg);

	m_BGPMsg.clearMessage();


}

void DataPlane::reviveDataPlane(void)
{

	setUp(true);
}

void DataPlane::setUp(bool p_Value)
{
	m_UpMutex.lock();
	m_Up = p_Value;
	m_UpMutex.unlock();
}

bool DataPlane::isRunning(void)
{
	bool currentV;
	m_UpMutex.lock();
	currentV = m_Up;
	m_UpMutex.unlock();
	return currentV;

}

bool DataPlane::write(BGPMessage& p_BGPMsg)
{
	//enter to the critical region
	m_BGPForwardingBufferMutex.lock();
	//write the message to the buffer
	if(isRunning())
		m_BGPForwardingBuffer.write(p_BGPMsg);
	//exit from the critical region
	m_BGPForwardingBufferMutex.unlock();
	return true;
}
