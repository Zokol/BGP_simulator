/*! \file ControlPlane.cpp
 *  \brief     Implementation of ControlPlane.
 *  \details
 *  \author    Antti Siiril�, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 12:32:09 2013
 */


#include "ControlPlane.hpp"
#include "ReportGlobals.hpp"


ControlPlane::ControlPlane(sc_module_name p_ModName, ControlPlaneConfig * const p_BGPConfig):sc_module(p_ModName),m_BGPConfig(p_BGPConfig), m_Name("BGP_Session"), m_MsgId(0)
{

	//make the inner bindings
	export_ToControlPlane(m_ReceivingBuffer); //export the receiving
	//buffer's input
	//interface for the data plane



	//initiate the BGPSession pointer arrays
	m_BGPSessions = new BGPSession*[m_BGPConfig->getNumberOfInterfaces()-1];
	//allocate reference array for receiving exports
	export_Session = new sc_export<BGPSession_If>*[m_BGPConfig->getNumberOfInterfaces()-1];
	export_InterfaceControl = new sc_export<Interface_If>*[m_BGPConfig->getNumberOfInterfaces()-1];
	export_RoutingTable = new sc_export<Output_If>*[m_BGPConfig->getNumberOfInterfaces()-1];

	//inititate the sessions
	for (int i = 0; i < m_BGPConfig->getNumberOfInterfaces()-1; i++)
	{
		//create a session
		m_BGPSessions[i] = new BGPSession(m_Name.getNextName(), i, m_BGPConfig);

		//Export the session interface for RT
		export_Session[i] = new sc_export<BGPSession_If>;
		export_Session[i]->bind(*m_BGPSessions[i]);
		m_BGPSessions[i]->port_Clk(port_Clk);
		export_InterfaceControl[i] = new sc_export<Interface_If>;
		export_RoutingTable[i] = new sc_export<Output_If>;

	}
	setUp(true);
	SC_THREAD(controlPlaneMain);
	sensitive << port_Clk.pos();
}

ControlPlane::~ControlPlane()
{

	for (int i = 0; i < m_BGPConfig->getNumberOfInterfaces()-1; i++)
		delete m_BGPSessions[i];
	delete m_BGPSessions;

}


void ControlPlane::controlPlaneMain(void)
{
	srand(time(NULL));


	//The main thread of the control plane starts
	while(true)
	{
		wait();
		if(!isRunning())
			continue;
		//read input message from the buffer if available
		if(m_ReceivingBuffer.num_available() > 0)
		{
			m_ReceivingBuffer.read(m_BGPMsgIn);

//			cout << name() << ": Session: " << m_BGPMsgIn.m_OutboundInterface << " MSG_TYPE: " << m_BGPMsgIn.m_Type << " @ " << sc_time_stamp() << endl;
			//write only if the session is not in IDLE
			if(m_BGPSessions[m_BGPMsgIn.m_OutboundInterface]->getBGPCurrentState() != IDLE)
				m_BGPSessions[m_BGPMsgIn.m_OutboundInterface]->m_FsmInputBuffer.write(m_BGPMsgIn);

		}
	}


}

void ControlPlane::killControlPlane()
{
	setUp(false);
	while(m_ReceivingBuffer.num_available() > 0)
		m_ReceivingBuffer.read(m_BGPMsgIn);

	m_MsgId = 0;
	m_BGPMsgIn.clearMessage();

}

void ControlPlane::reviveControlPlane(void)
{
	setUp(true);
}

void ControlPlane::setUp(bool p_Value)
{
	m_UpMutex.lock();
	m_Up = p_Value;
	m_UpMutex.unlock();
}

bool ControlPlane::isRunning(void)
{
	bool currentV;
	m_UpMutex.lock();
	currentV = m_Up;
	m_UpMutex.unlock();
	return currentV;

}


bool ControlPlane::write(BGPMessage& p_BGPMsg)
{

	//enter to the critical region
	mutex_Write.lock();
	p_BGPMsg.m_MsgId = ++m_MsgId;
	//reset the corresponding keepalive timer
	m_BGPSessions[p_BGPMsg.m_OutboundInterface]->resetKeepalive();
	//write message to the DataPlane
	port_ToDataPlane->write(p_BGPMsg);
	//exit from the critical region
	mutex_Write.unlock();
	return true;

}
