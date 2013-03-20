/*! \file Simulation.cpp 
 *  \brief     Implementation of Simulation module.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      28.1.2013
 */


#include "Simulation.hpp"
#include "ReportGlobals.hpp"
#include "Configuration.hpp"

Simulation::Simulation(sc_module_name p_ModuleName, ServerSocket& p_GUISocket, SimulationConfig& p_SimuConfiguration):sc_module(p_ModuleName), m_GUISocket(p_GUISocket)
{

    //SimulationConfig m_SimuConfiguration = p_SimuConfiguration;
//  //Throws segmentation fault

//  m_NumberOfRouters = m_SimuConfiguration.m_NumberOfRouters;


///Constructor briefly: 

    m_BGPSessionParam.m_HoldDownTime = 180;
    m_BGPSessionParam.m_KeepaliveFraction = 3;


  /// \li Allocate Router pointer array
  m_Router = new Router*[ROUTER_COUNT];

  /// \li Set the base name for the StringTools object
  m_Name.setBaseName("Router");




  /// \li Initiate the Router modules as m_Router
  for(int i = 0; i < ROUTER_COUNT; i++)
    {
      /// \li Generate the routers
      m_Router[i] = new Router(m_Name.getNextName(), INTERFACE_COUNT, m_BGPSessionParam);

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
  if(ROUTER_COUNT > 2)
    {
        m_Name.setBaseName(name());
        SC_REPORT_INFO(g_DebugID, m_Name.newReportString("More than two routers."));
     

      ///connect each router to the next one
        for(int i = 1; i < ROUTER_COUNT-1; i++)
	{

	  m_Router[i]->port_ForwardingInterface[1]->bind(*(m_Router[i+1]->export_ReceivingInterface[0]));
	  m_Router[i+1]->port_ForwardingInterface[0]->bind(*(m_Router[i]->export_ReceivingInterface[1]));
	  m_Router[i]->interfaceUp(1);
	  m_Router[i+1]->interfaceUp(0);

	}
        SC_REPORT_INFO(g_DebugID, m_Name.newReportString("Intermediate routers connected."));

      ///close the ring by connecting the last router to the first
      m_Router[ROUTER_COUNT-1]->port_ForwardingInterface[1]->bind(*(m_Router[0]->export_ReceivingInterface[1]));
      m_Router[0]->port_ForwardingInterface[1]->bind(*(m_Router[ROUTER_COUNT-1]->export_ReceivingInterface[1]));

	  m_Router[0]->interfaceUp(1);
	  m_Router[ROUTER_COUNT-1]->interfaceUp(1);
        SC_REPORT_INFO(g_DebugID, m_Name.newReportString("The last and the first router connected."));



    }


    SC_THREAD(simulationMain);
    sensitive << port_Clk.pos();


}

Simulation::~Simulation()
{

  /// \li Free all the memory
  for(int i = 0; i < ROUTER_COUNT; i++)
    delete m_Router[i];

  delete m_Router;
}


void Simulation::simulationMain(void)
{


}
