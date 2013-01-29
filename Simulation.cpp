/*! \file Simulation.cpp 
 *  \brief     Implementation of Simulation module.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      28.1.2013
 */


#include "Simulation.hpp"


Simulation::Simulation(sc_module_name p_ModuleName):sc_module(p_ModuleName)
{
///Constructor briefly: 



  /// \li Allocate Router pointer array
  m_Router = new Router*[ROUTER_COUNT];

  /// \li Set the base name for the router modules
  m_Name = "Router_";



  /// \li Initiate the Router modules as m_Router
  for(int i = 0; i < ROUTER_COUNT; i++)
    {
      cout << "Building " << appendName(m_Name, i) << endl;
      /// \li Generate the routers
      m_Router[i] = new Router(appendName(m_Name, i), INTERFACE_COUNT);
      cout << appendName(m_Name, i) << " built." << endl;
    }
  

  ///Build the network TO-DO: clean up the binding process -> make a connection method in the router

  ///connect the 0-interfaces of router 0 and router 1
  m_Router[0]->port_ForwardingInterface[0]->bind(*(m_Router[1]->export_ReceivingInterface[0]));

  m_Router[1]->port_ForwardingInterface[0]->bind(*(m_Router[0]->export_ReceivingInterface[0]));

  ///set the those interfaces up
  m_Router[0]->interfaceUp(0);
  m_Router[1]->interfaceUp(0);


 
  ///build a ring if there is more than two routers in the simulation
  if(ROUTER_COUNT > 2)
    {
      cout << "More than two routers." << endl;

      ///connect each router to the next one
      for(int i = 1; i < ROUTER_COUNT-1; i++)
	{

	  m_Router[i]->port_ForwardingInterface[1]->bind(*(m_Router[i+1]->export_ReceivingInterface[0]));
	  m_Router[i+1]->port_ForwardingInterface[0]->bind(*(m_Router[i]->export_ReceivingInterface[1]));
	  m_Router[i]->interfaceUp(1);
	  m_Router[i+1]->interfaceUp(0);

	}

      ///close the ring by connecting the last router to the first
      m_Router[ROUTER_COUNT-1]->port_ForwardingInterface[1]->bind(*(m_Router[0]->export_ReceivingInterface[1]));
      m_Router[0]->port_ForwardingInterface[1]->bind(*(m_Router[ROUTER_COUNT-1]->export_ReceivingInterface[1]));

	  m_Router[0]->interfaceUp(1);
	  m_Router[ROUTER_COUNT-1]->interfaceUp(1);

    }

  cout << name() << ": Simulation starts for " << SIMULATION_DURATION << " ns" << endl;
  /// \li Start the simulation 
  sc_start(SIMULATION_DURATION, SC_NS);




}

Simulation::~Simulation()
{

  /// \li Free all the memory
  for(int i = 0; i < ROUTER_COUNT; i++)
    delete m_Router[i];

  delete m_Router;
}

const char* Simulation::appendName(string p_Name, int p)
{
  stringstream ss;
  ss << p;
  p_Name += ss.str();
  return p_Name.c_str();
}
