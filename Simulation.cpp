/*! \file Simulation.cpp 
 *  \brief     Implementation of Simulation module.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      28.1.2013
 */


#include "Simulation.hpp"
#include "ReportGlobals.hpp"


Simulation::Simulation(sc_module_name p_ModuleName, ServerSocket& p_GUISocket, SimulationConfig& p_SimuConfiguration):sc_module(p_ModuleName), m_GUISocket(p_GUISocket), m_SimuConfiguration(p_SimuConfiguration)
{

//  //Throws segmentation fault

//  DEBUGGING
m_Name.appendReportString("Router count: ");
SC_REPORT_INFO(g_DebugID, m_Name.appendReportString(m_SimuConfiguration.getNumberOfRouters()));

m_Name.appendReportString("Interface count: ");
 SC_REPORT_INFO(g_DebugID, m_Name.appendReportString(m_SimuConfiguration.getRouterConfiguration(0).getNumberOfInterfaces()));


  /// \li Allocate Router pointer array
  m_Router = new Router*[m_SimuConfiguration.getNumberOfRouters()];

  /// \li Set the base name for the StringTools object
  m_Name.setBaseName("Router");




  /// \li Initiate the Router modules as m_Router
  for(int i = 0; i < m_SimuConfiguration.getNumberOfRouters(); i++)
    {
      /// \li Generate the routers
        m_Router[i] = new Router(m_Name.getNextName(), m_SimuConfiguration.getRouterConfiguration(i));

    }
  

  ///Build the network TO-DO: clean up the binding process -> make a connection method in the router

  ///connect the 0-interfaces of router 0 and router 1
  m_Router[0]->port_ForwardingInterface[0]->bind(*(m_Router[1]->export_ReceivingInterface[0]));

  m_Router[1]->port_ForwardingInterface[0]->bind(*(m_Router[0]->export_ReceivingInterface[0]));
        SC_REPORT_INFO(g_DebugID, m_Name.newReportString("Two first routers connected."));

  ///set the those interfaces up
  m_Router[0]->interfaceUp(0);
  m_Router[1]->interfaceUp(0);


 
  ///build a ring if there is more than two routers in the simulation
  if(m_SimuConfiguration.getNumberOfRouters() > 2)
    {
        m_Name.setBaseName(name());
        SC_REPORT_INFO(g_DebugID, m_Name.newReportString("More than two routers."));
     

      ///connect each router to the next one
        for(int i = 1; i < m_SimuConfiguration.getNumberOfRouters()-1; i++)
	{

	  m_Router[i]->port_ForwardingInterface[1]->bind(*(m_Router[i+1]->export_ReceivingInterface[0]));
	  m_Router[i+1]->port_ForwardingInterface[0]->bind(*(m_Router[i]->export_ReceivingInterface[1]));
	  m_Router[i]->interfaceUp(1);
	  m_Router[i+1]->interfaceUp(0);

	}
        SC_REPORT_INFO(g_DebugID, m_Name.newReportString("Intermediate routers connected."));

      ///close the ring by connecting the last router to the first
      m_Router[m_SimuConfiguration.getNumberOfRouters()-1]->port_ForwardingInterface[1]->bind(*(m_Router[0]->export_ReceivingInterface[1]));
      m_Router[0]->port_ForwardingInterface[1]->bind(*(m_Router[m_SimuConfiguration.getNumberOfRouters()-1]->export_ReceivingInterface[1]));

	  m_Router[0]->interfaceUp(1);
	  m_Router[m_SimuConfiguration.getNumberOfRouters()-1]->interfaceUp(1);
        SC_REPORT_INFO(g_DebugID, m_Name.newReportString("The last and the first router connected."));



    }


    SC_THREAD(simulationMain);
    sensitive << port_Clk.pos();


}

Simulation::~Simulation()
{

  /// \li Free all the memory
  for(int i = 0; i < m_SimuConfiguration.getNumberOfRouters(); i++)
    delete m_Router[i];

  delete m_Router;
}


void Simulation::simulationMain(void)
{
    bool state = true;
    // m_GUISocket.set_non_blocking(true);
    while(true)
        {
            wait();
            m_Word = "";

            if(state)
                {
                    try
                        {
                            m_GUISocket >> m_Word;
                        }
                    catch(SocketException e)
                        {
                            //cout << e.description() << endl;
                        }
                    
                    if(m_Word != "")
                        {
                            cout << "Received: " << m_Word << endl;
                            if(m_Word.compare("Request") == 0)
                                {
                                    m_GUISocket << "Response";
                                }
                        }
                }
            
            if(!(m_GUISocket.is_valid()))
                cout << "socket not valid" << endl;
        }

}
