/*! \file Host.cpp
 *  \brief
 *  \details
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      27.1.2013
 */


#include "Host.hpp"
#include "ReportGlobals.hpp"

Host::Host(sc_module_name p_ModuleName, Connection *p_ConnectionConfig):sc_module(p_ModuleName), m_MsgBuffer("")
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

					//bind network interface port to router's hierarchial port
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
			cout << name() << ": Me liky" << endl;

		}

}

void Host::interfaceUp(void)
{
	m_NetworkInterface[0]->interfaceUp();
}
