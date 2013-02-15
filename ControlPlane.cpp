/*! \file ControlPlane.cpp
 *  \brief     Implementation of ControlPlane.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 12:32:09 2013
 */


#include "ControlPlane.hpp"


ControlPlane::ControlPlane(sc_module_name p_ModName, int p_Sessions, BGPSessionParameters p_BGPParameters):sc_module(p_ModName)
{

  //make the inner bindings
    export_ToControlPlane(m_ReceivingBuffer); //export the receiving
                                             //buffer's input
                                             //interface for the data plane


    //set the session count
    m_SessionCount = p_Sessions;

    //initiate the BGPSession pointer arrays
    m_BGPSessions = new BGPSession*[m_SessionCount];

    //inititate the sessions
    for (int i = 0; i < m_SessionCount; ++i)
        {
            //create a session 
            m_BGPSessions[i] = new BGPSession("BGP_Session", p_BGPParameters);
        }
    
    SC_THREAD(controlPlaneMain);
    sensitive << port_Clk.pos();
}

ControlPlane::~ControlPlane()
{

    for (int i = 0; i < m_SessionCount; ++i)
        delete m_BGPSessions[i];
    delete m_BGPSessions;
}


void ControlPlane::controlPlaneMain(void)
{
  cout << name() << " starting at time" << sc_time_stamp()  << endl;



  //The main thread of the control plane starts
    while(true)
    {
        wait();

        //Check if there's messages in the input buffer
      if(m_ReceivingBuffer.num_available() > 0)
          {
              m_ReceivingBuffer.read(m_BGPMsg);
              
              //check whether the session is valid     
              if (m_BGPSessions[m_BGPMsg.m_OutboundInterface]->isThisSession(m_BGPMsg.m_BGPIdentifier)) 
                  {
                      //if the session is valid do the required routies
                   
                  }
              //if the session was not valid but this is an OPEN message
              else if (m_BGPMsg.m_Type = OPEN)
                  {

                      //start new session for the session index
                      //corresponding the interface index to which the
                      //peer is connected
                      m_BGPSessions[m_BGPMsg.m_OutboundInterface]->setPeerIdentifier(m_BGPMsg.m_BGPIdentifier);

                      //start the session
                      m_BGPSessions[m_BGPMsg.m_OutboundInterface]->sessionStart();

                  }
              //Ohterwise
              else
                  {
                      //drop

                  }





              //To send a message to data plane
              port_ToDataPlane->write(m_BGPMsg);


              
              
              //Handle the message here
          }



        
    
    }
}

