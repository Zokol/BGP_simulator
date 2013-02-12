/*! \file Interface.cpp
 *  \brief     Implementation of Network Interface.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      25.1.2013
 */


#include "Interface.hpp"


Interface::Interface(sc_module_name p_ModName):sc_module(p_ModName)
{
  //make the inner bindings
    export_FromDataPlane(m_ForwardingBuffer); //export the forwarding buffer's input interface for the protocol engine
    export_ToDataPlane(m_ReceivingBuffer); // //export the forwarding buffer's input interface for the protocol engine

    m_InterfaceState = DOWN;

    SC_THREAD(interfaceMain);
    sensitive << port_Clk.pos();
}

Interface::~Interface()
{
}


void Interface::interfaceMain(void)
{
  cout << name() << " starting at time" << sc_time_stamp()  << endl;
    while(true)
    {
        wait();
	
	if(m_InterfaceState) //only if interface is up
	  //Forward the next packet from the local forwarding queue
	  port_Output->forward(m_ForwardingBuffer.read());
    }
}

bool Interface::forward(Packet p_Packet)
{

  
  if(m_InterfaceState) //only if interface is up
    {
      m_ReceivingBuffer.write(p_Packet);
      return true;
    }
  else
    return false;
}

void Interface::interfaceDown(void)
{
  m_InterfaceState = DOWN;
}

void Interface::interfaceUp(void)
{
  m_InterfaceState = UP;
}
