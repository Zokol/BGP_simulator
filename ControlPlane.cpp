/*! \file ControlPlane.cpp
 *  \brief     Implementation of ControlPlane.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 12:32:09 2013
 */


#include "ControlPlane.hpp"


ControlPlane::ControlPlane(sc_module_name p_ModName, int p_Sessions):sc_module(p_ModName)
{
  //make the inner bindings
    export_ToControlPlane(m_ReceivingBuffer); //export the receiving
                                             //buffer's input
                                             //interface for the data plane


    //set the session count
    m_Sessions = p_Sessions;

    //initiate the event pointer arrays
    m_BGPSessionHoldDown = new sc_event*[m_Sessions];
    m_BGPSessionKeepalive = new sc_event*[m_Sessions];

    //inititate the session events
    for (int i = 0; i < m_Sessions; ++i)
        {
            m_BGPSessionHoldDown[i] = new sc_event("HoldDown");
            m_BGPSessionKeepalive[i] = new sc_event("Keepalive");
        }


    SC_THREAD(controlPlaneMain);
    sensitive << port_Clk.pos();

    SC_THREAD(timer);
    sensitive << port_Clk.pos();
}

ControlPlane::~ControlPlane()
{

    delete m_BGPSessionHoldDown;
}


void ControlPlane::controlPlaneMain(void)
{
  cout << name() << " starting at time" << sc_time_stamp()  << endl;
    while(true)
    {
        wait();

        //Check if there's messages in the input buffer
      if(m_ReceivingBuffer.num_available() > 0)
          {

              //Handle the message here
          }
      
    
    }

}



void ControlPlane::timer(void)
{
    while(true)
        {

        }

}

