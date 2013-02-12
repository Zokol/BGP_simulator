/*! \file Router.cpp
 *  \brief     
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      27.1.2013
 */


#include "Router.hpp"

Router::Router(sc_module_name p_ModuleName, int p_InterfaceCount):sc_module(p_ModuleName), m_InterfaceCount(p_InterfaceCount)
{

  
  /// \li define clock period for Router
      m_ClkPeriod = new const sc_time(10, SC_NS);

  /// \li Allocate clock for the Routers using the previously allocated period
    m_ClkRouter = new sc_clock("CLK", *m_ClkPeriod);

  
  //TO-DO
  //bind the ports here
    m_Bgp = new DataPlane("Engine", m_InterfaceCount);
    m_Bgp->port_Clk(*m_ClkRouter);

  
  m_Name = "Interface_";

  //allocate reference array for network interface modules
  m_NetworkInterface = new Interface*[m_InterfaceCount];

  //allocate reference array for receiving exports
  export_ReceivingInterface = new sc_export<Interface_If>*[m_InterfaceCount];

  //allocate reference array for fowarding ports
  port_ForwardingInterface = new sc_port<Interface_If, 1, SC_ZERO_OR_MORE_BOUND>*[m_InterfaceCount];

  //instantiate the network interface modules
  for(int i = 0; i < m_InterfaceCount; i++)
    {
      
      //instantiate an interface
      m_NetworkInterface[i] = new Interface(appendName(m_Name, i));

      //instantiate hierarchial forwarding port
      port_ForwardingInterface[i] = new sc_port<Interface_If, 1, SC_ZERO_OR_MORE_BOUND>;
      
      //bind network interface port to router's hierarchial port
      m_NetworkInterface[i]->port_Output.bind(*port_ForwardingInterface[i]);
      
      //make the hierarchial binding for receiving exports
      export_ReceivingInterface[i] = new sc_export<Interface_If>;
      export_ReceivingInterface[i]->bind(*m_NetworkInterface[i]);

      //bind the clock to the network interface
      m_NetworkInterface[i]->port_Clk(*m_ClkRouter);      





      m_Bgp->port_FromInterface(m_NetworkInterface[i]->export_ToDataPlane);//bind the receiving buffer's output to the protcol engine's output
      m_Bgp->port_ToInterface(m_NetworkInterface[i]->export_FromDataPlane);//bind the protocol engine's output to the forwarding buffer's input

    }

}

Router::~Router()
{

  for(int i = 0; i < m_InterfaceCount; i++)
    {
      delete export_ReceivingInterface[i];
      delete port_ForwardingInterface[i];
      delete m_NetworkInterface[i];
    }

  
  delete export_ReceivingInterface;
  delete port_ForwardingInterface;
  delete m_NetworkInterface;

  delete m_Bgp;
  delete m_ClkPeriod;
  delete m_ClkRouter;
}

void Router::interfaceUp(int p_InterfaceId)
{
  m_NetworkInterface[p_InterfaceId]->interfaceUp();
  cout << m_NetworkInterface[p_InterfaceId]->name() << " set up." << endl;
}


const char* Router::appendName(string p_Name, int p)
{
  stringstream ss;
  ss << p;
  p_Name += ss.str();
  return p_Name.c_str();
}
