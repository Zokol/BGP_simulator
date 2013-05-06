/*! \file Host.cpp
 *  \brief
 *  \details
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      02.05.2013
 */


#include "Host.hpp"
#include "ReportGlobals.hpp"

Host::Host(sc_module_name p_ModuleName, Connection *p_ConnectionConfig):sc_module(p_ModuleName), m_Encoder("Encoder"), m_Decoder("Decoder"), m_MsgBuffer(START)
{
	///StringTools instance for reporting
	StringTools *l_Report = new StringTools(name());
	l_Report->resetReportString();



	//DEBUGGING
	SC_REPORT_INFO(g_DebugID, l_Report->newReportString("elaborates"));

	/// \li define clock period for Router
	m_ClkPeriod = new const sc_time(1, SC_SEC);

	/// \li Allocate clock for the Routers using the previously allocated period
	m_ClkRouter = new sc_clock("CLK", *m_ClkPeriod);


	SC_REPORT_INFO(g_DebugID, l_Report->newReportString("Building the network interfaces"));

	//allocate reference array for network interface modules
	m_NetworkInterface = new Interface*[1];

	//allocate reference array for receiving exports
	export_ReceivingInterface = new sc_export<Interface_If>*[1];

	//allocate reference array for fowarding ports
	port_ForwardingInterface = new sc_port<Interface_If, 1, SC_ZERO_OR_MORE_BOUND>*[1];


	m_NetworkInterface[0] = new Interface(m_Name.getNextName(), p_ConnectionConfig);

	//instantiate hierarchial forwarding port
	port_ForwardingInterface[0] = new sc_port<Interface_If, 1, SC_ZERO_OR_MORE_BOUND>;

	//bind network interface port to host's hierarchial port
	m_NetworkInterface[0]->port_Output.bind(*port_ForwardingInterface[0]);

	//make the hierarchial binding for receiving exports
	export_ReceivingInterface[0] = new sc_export<Interface_If>;
	export_ReceivingInterface[0]->bind(*m_NetworkInterface[0]);

	//bind the clock to the network interface
	m_NetworkInterface[0]->port_Clk(*m_ClkRouter);



	//delete the StringTools object
	delete l_Report;

	SC_THREAD(hostMain);
	sensitive << *m_ClkRouter;

}

Host::~Host()
{


}

void Host::hostMain()
{


	while(true)
	{
		wait();

		//Read frame from the interface if available
		if(m_NetworkInterface[0]->export_ToDataPlane->num_available() > 0)
		{

			m_NetworkInterface[0]->export_ToDataPlane->read(m_Frame);

			//Handle only frames that carry IP
			if(m_Frame.getProtocolType() == TYPE_IP)
			{
				m_Decoder.processFrame(m_Frame);

				appendMsgBuffer(m_Decoder.readIPPacket());
			}

		}

	}

}

void Host::interfaceUp(void)
{
	m_NetworkInterface[0]->interfaceUp();
}

/*! \sa Communication_If
 */
bool Host::sendMessage(string p_DestinationIP, string p_SourceIP, string p_Payload)
{
//	cout << name() << " send a message:" << endl << "Destination: " << p_DestinationIP << endl << "Source: " << p_SourceIP << endl << "Payload: " << p_Payload << endl;
	//build the IP packet
	m_Frame = m_Encoder.buildIPPacket(p_DestinationIP, p_SourceIP, p_Payload);
	//Set the protocol type to IP
	m_Frame.setProtocolType(TYPE_IP);
	//write the frame to the interface
	m_NetworkInterface[0]->write(m_Frame);
	return true;
}

/*! \sa Communication_If
 */
string Host::reaMessageBuffer(void)
{
	m_MsgBufferMutex.lock();
	string l_Temp = m_MsgBuffer;
	m_MsgBufferMutex.unlock();

	return l_Temp + END;

}

/*! \sa Communication_If
 */
void Host::clearMessageBuffer(void)
{				cout << name() << " received a packet." << endl;

	m_MsgBufferMutex.lock();
	m_MsgBuffer = START;
	m_MsgBufferMutex.unlock();
}

void Host::appendMsgBuffer(string p_SubString)
{
	m_MsgBufferMutex.lock();
	m_MsgBuffer += p_SubString;
	m_MsgBufferMutex.unlock();
}

