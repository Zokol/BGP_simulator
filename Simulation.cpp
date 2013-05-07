/*! \file Simulation.cpp 
 *  \brief     Implementation of Simulation module.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      28.1.2013
 */


#include "Simulation.hpp"
#include "ReportGlobals.hpp"
#include "GUIProtocolTags.hpp"

Simulation::Simulation(sc_module_name p_ModuleName, ServerSocket& p_GUISocket, SimulationConfig * const p_SimuConfiguration):sc_module(p_ModuleName), m_GUISocket(p_GUISocket), m_SimuConfiguration(p_SimuConfiguration)
{



	//  DEBUGGING
	m_Name.appendReportString("Router count: ");
	SC_REPORT_INFO(g_DebugID, m_Name.appendReportString(m_SimuConfiguration->getNumberOfRouters()));

	m_Name.appendReportString("Interface count: ");
	SC_REPORT_INFO(g_DebugID, m_Name.appendReportString(m_SimuConfiguration->getRouterConfiguration(0).getNumberOfInterfaces()));


	/// \li Allocate Router pointer array
	m_Router = new Router*[m_SimuConfiguration->getNumberOfRouters()];
	m_Host = new Host*[m_SimuConfiguration->getNumberOfRouters()];

	/// \li Set the base name for the StringTools object
	m_Name.setBaseName("Router");


	StringTools *l_HostName = new StringTools("Host");
	/// \li Initiate the Router modules as m_Router
	for(int i = 0; i < m_SimuConfiguration->getNumberOfRouters(); i++)
	{
		/// \li Generate the routers
		m_Router[i] = new Router(m_Name.getNextName(), m_SimuConfiguration->getRouterConfigurationPtr(i));
		m_Host[i] = new Host(l_HostName->getNextName(), m_SimuConfiguration->getHostConfigurationPtr(i));

	}


	///Build the network
	RouterConfig *l_Handle;
	///connect all the routers
	for (int i = 0; i < m_SimuConfiguration->getNumberOfRouters(); ++i)
	{
		///get handle to the routers configuration object
		l_Handle = m_SimuConfiguration->getRouterConfigurationPtr(i);

		///connect all the interfaces
		for (int j = 0; j < l_Handle->getNumberOfInterfaces(); ++j)
		{

			///Connect only those interfaces that have been configured
			if (l_Handle->isConnection(j))
			{
				///Connect the interfaces
				if(l_Handle->getNeighborRouterId(j) == 0x7FFFFFFF)
				{
					m_Router[i]->connectInterface(m_Host[i], j);
				}
				else if(m_Router[i]->connectInterface(m_Router[l_Handle->getNeighborRouterId(j)], j, l_Handle->getNeighborInterfaceId(j)))
				{

				}
			}
		}


	}

	m_SimuConfiguration->ifModes();
	//cout << "Simulation elaboration finished" << endl;
	SC_THREAD(simulationMain);
	sensitive << port_Clk.pos();


}

Simulation::~Simulation()
{

	/// \li Free all the memory
	for(int i = 0; i < m_SimuConfiguration->getNumberOfRouters(); i++)
		delete m_Router[i];

	delete m_Router;
}


void Simulation::simulationMain(void)
{

	m_Name.resetReportString();
	m_Name.setBaseName(name());
	SC_REPORT_INFO(g_DebugID, m_Name.newReportString("Starts"));
//cout << "Simulation main starts" << endl;
#ifdef _GUI_TEST

	bool state = true;
#elif defined (_GUI)
	enum_State = ACTIVE;
	prev_State = TERMINATE;


#else

#endif
	bool run = true;
	// m_GUISocket.set_non_blocking(true);
	// RouterConfig *testC = m_SimuConfiguration->getRouterConfigurationPtr(0);
	//  int t = 20;
	while(run)
	{
		wait();

#ifdef _GUI_TEST

		m_Word = "";

		if(state)
		{
			try
			{
				m_GUISocket >> m_Word;
			}
			catch(SocketException e)
			{
				////cout << e.description() << endl;
				continue;
			}

			if(m_Word != "")
			{
				//cout << "Received: " << m_Word << endl;
				m_GUISocket << m_Word;
				if(m_Word.compare("STOP") == 0)
					sc_stop();
			}
		}

		if(!(m_GUISocket.is_valid()))
			//cout << "socket not valid" << endl;

#elif defined (_GUI)

		if(prev_State != enum_State)
//			//cout << "Current state is " << enum_State << endl;

		prev_State = enum_State;
		///START:FSM of the socket server
		switch(enum_State)
		{
		case ACTIVE:
			if(m_GUISocket.is_valid())
				enum_State = receiveRoutine()?PROCESS:ACTIVE;
			else
				enum_State = TERMINATE;
			break;
		case PROCESS:

			socketRoutine();
			break;
		case SEND:

			if(m_GUISocket.is_valid())
				enum_State = sendRoutine()?ACTIVE:SEND;
			else
				enum_State = TERMINATE;
			break;
		case TERMINATE:

			m_Word = STOP;
			if(m_GUISocket.is_valid())
				sendRoutine();
			run = false;
			break;

		default:
			SC_REPORT_INFO(g_ReportID, m_Name.newReportString("Server state failure.\nSimulation ends."));

			run = false;
			break;
		}
		///END:FSM of the socket server

#else

#endif
	}//end of process loop

#if defined (_GUI) || defined (_GUI_TEST)

	sc_stop();

#endif
}


void Simulation::socketRoutine(void)
{
	bool l_Param = true;
	try
	{
		//separate the command and the parameters from the received string
		if(m_Word.find("<CMD>", 0, 5) == string::npos)
			m_Cmd = "UNKNOWN";
		else if(m_Word.find("</CMD>",0) == string::npos)
			m_Cmd = "UNKNOWN";
		else
		{

			m_Word = m_Word.substr(5);
			m_Word = m_Word.substr(0,m_Word.find("</CMD>",0));

			//cout << "Word without tags: " << m_Word << endl;
			unsigned l_Pos = m_Word.find(",",0);

			if(l_Pos == string::npos)
			{
				m_Cmd = m_Word;
				m_Fields = "";
				l_Param = false;
			}
			else
			{
				m_Cmd = m_Word.substr(0,l_Pos);
				m_Fields = m_Word.substr(l_Pos+1);
			}
		}

		///The command cases
		if(m_Cmd.compare(STOP) == 0) ///STOP
		{


			//set the next server state to TERMINATE
			enum_State = TERMINATE;
		}
		else if (m_Cmd.compare(RESET_ROUTER) == 0 && l_Param) ///RESET_ROUTER
		{

			//get the router ID
			fieldRoutine(1);
			//reset router
			m_Router[m_IntBuffer[0]]->resetRouter();
			//send acknowledgement
			m_Word = ACK;
			//set next server state to ACTIVE
			enum_State = SEND;
		}
		else if (m_Cmd.compare(KILL_ROUTER) == 0 && l_Param) ///KILL_ROUTER
		{

			//get the router ID
			fieldRoutine(1);
			//kill the router
			m_Router[m_IntBuffer[0]]->killRouter();
			//send acknowledgement
			m_Word = ACK;
			//set next server state to SEND
			enum_State = SEND;

		}
		else if (m_Cmd.compare(REVIVE_ROUTER) == 0 && l_Param) ///REVIVE_ROUTER
		{

			//get the router ID
			fieldRoutine(1);
			//kill the router
			m_Router[m_IntBuffer[0]]->reviveRouter();
			//send acknowledgement
			m_Word = ACK;
			//set next server state to SEND
			enum_State = SEND;
		}
		else if (m_Cmd.compare(READ_PACKET) == 0 && l_Param) ///READ_PACKET
		{
			//get the router ID
			fieldRoutine(1);
			//TODO: add readMessages() method into Router module
			//cout << "Reading packet" << endl;
			m_Word = m_Host[m_IntBuffer[0]]->reaMessageBuffer();
			//set next server state to SEND
			enum_State = SEND;

		}
		else if (m_Cmd.compare(CLEAR_PACKET) == 0 && l_Param) ///CLEAR_PACKET
		{
			//get the router ID
			fieldRoutine(1);

			//TODO: add clearMessages() method into Router module
			m_Host[m_IntBuffer[0]]->clearMessageBuffer();

			//send acknowledgement
			m_Word = ACK;


			//set next server state to SEND
			enum_State = SEND;

		}
		else if (m_Cmd.compare(SEND_PACKET) == 0 && l_Param) ///SEND_PACKET
		{
			//get the router ID
			fieldRoutine(3);

			if(m_Host[m_IntBuffer[1]]->sendMessage(m_FieldBuffer[0],m_SimuConfiguration->getRouterConfiguration(m_IntBuffer[1]).getIPAsString(), m_FieldBuffer[2]))
				m_Word = ACK;
			else
				m_Word = NACK;

			//set next server state to SEND
			enum_State = SEND;

		}
		else if (m_Cmd.compare(CONNECT) == 0 && l_Param) ///CONNECT
		{
			//get the router ID
			fieldRoutine(2);
			//kill the router
			m_Router[m_IntBuffer[0]]->connectInterface(m_IntBuffer[1]);
			//send acknowledgement
			m_Word = ACK;
			//set next server state to SEND
			enum_State = SEND;

		}
		else if (m_Cmd.compare(DISCONNECT) == 0 && l_Param) ///DISCONNECT
		{
			//get the router ID
			fieldRoutine(2);
			//kill the router
			m_Router[m_IntBuffer[0]]->disconnectInterface(m_IntBuffer[1]);
			//send acknowledgement
			m_Word = ACK;
			//set next server state to SEND
			enum_State = SEND;

		}
		else if (m_Cmd.compare(READ_TABLE) == 0 && l_Param) ///READ_TABLE
		{
			//get the router ID
			fieldRoutine(1);

			//call the mehtod that returns the routing table
			m_Word = m_Router[m_IntBuffer[0]]->getRoutingTable();
			//set next server state to SEND
			enum_State = SEND;

		}
		else if (m_Cmd.compare(READ_RAW_TABLE) == 0 && l_Param) ///READ_RAW_TABLE
		{
			//get the router ID
			fieldRoutine(1);

			//call the mehtod that returns the raw routing table
			m_Word = m_Router[m_IntBuffer[0]]->getRawRoutingTable();
			//set next server state to SEND
			enum_State = SEND;

		}
		else if (m_Cmd.compare(SET_LOCAL_PREF) == 0 && l_Param) ///SET_LOCAL_PREF
		{
			//get the router ID
			fieldRoutine(3);


			//call method that set the local pref for a given AS ID
			m_Router[m_IntBuffer[0]]->setPreferredAS(m_IntBuffer[1], m_IntBuffer[2]);

			//send acknowledgement
			m_Word = ACK;

			//set next server state to SEND
			enum_State = SEND;

		}
		else if (m_Cmd.compare(SET_KEEPALIVE) == 0 && l_Param) ///SET_KEEPALIVE
		{
			//get the router ID
			fieldRoutine(2);
			//get pointer to the correct configuration object
			//cout << "0: " << m_IntBuffer[0] << "\n1: " << m_IntBuffer[1] << endl;

			RouterConfig *l_RConfig = m_SimuConfiguration->getRouterConfigurationPtr(m_IntBuffer[0]);
			l_RConfig->setKeepaliveTime(m_IntBuffer[1]);
			//send acknowledgement
			m_Word = ACK;
			//set next server state to SEND
			enum_State = SEND;

		}
		else if (m_Cmd.compare(SET_HOLDDOWN_MULT) == 0 && l_Param) ///SET_HOLDDOWN_MULT
		{
			//get the router ID
			fieldRoutine(2);
			//get pointer to the correct configuration object
			RouterConfig *l_RConfig = m_SimuConfiguration->getRouterConfigurationPtr(m_IntBuffer[0]);
			l_RConfig->setHoldDownTimeFactor(m_IntBuffer[1]);
			//send acknowledgement
			m_Word = ACK;
			//set next server state to SEND
			enum_State = SEND;

		}
		else if (m_Cmd.compare(SHOW_IF) == 0 && l_Param) /// SHOW_IF
		{
			//TODO: to be implemented if extra time is left
		}
		else
		{
			cout << m_Word << endl;
			m_Word = NACK;
			enum_State = SEND;
		}
	}
	catch(...)
	{
		m_Word = NACK;
		enum_State = SEND;
	}


}

bool Simulation::sendRoutine(void)
{

	try
	{
		m_GUISocket << m_Word;
		return true;
	}
	catch(SocketException e)
	{
		return  false;
	}

}

bool Simulation::receiveRoutine()
{

	try
	{
		m_GUISocket >> m_Word;
		return true;
	}
	catch(SocketException e)
	{
		return  false;
	}

}

bool Simulation::fieldRoutine(int p_NumOfFields)
{
	bool l_Parse = true;
	int i = 0;
	unsigned l_Pre = 0, l_Post = 0;

	if(m_Fields.compare("") == 0)
		return false;

	while (l_Parse)
	{
		l_Post = m_Fields.find(",",l_Pre);
		if(l_Post == string::npos)
		{
			m_FieldBuffer[i] = m_Fields.substr(l_Pre);
			l_Parse = false;
		}
		else
		{
			m_FieldBuffer[i] = m_Fields.substr(l_Pre, l_Post-l_Pre);
			l_Pre = l_Post+1;
		}
		i++;

	}

	bufferToInt(p_NumOfFields);

	//cout << i << " field(s) was(were) read." << endl;
	return true;
}

void Simulation::bufferToInt(int p_NumberOfElements)
{
	for (int var = 0; var < p_NumberOfElements; var++)
	{
		m_IntBuffer[var] = StringTools().sToI(m_FieldBuffer[var]);
	}

}
