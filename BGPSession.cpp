/*! \file BGPSession.cpp
 *  \brief     Implementation of BGPSession.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Wed Feb 13 20:36:28 2013
 */


#include "BGPSession.hpp"


BGPSession(sc_module_name p_ModuleName, int p_PeeringInterface, BGPSessionParameters p_SessionParam):sc_module(p_ModuleName)
{

    //assign the session parameters
    setSessionParameters(p_SessionParam);
    
    //set the local interface index behind which the peer locates
    m_PeeringInterface = p_PeeringInterface;
    
    //Register sendKeepalive method to the SystemC kernel
    SC_METHOD(sendKeepalive);
    sensitive << m_BGPKeepalive;

    //Register sessionInvalidation method to the SystemC kernel
    SC_METHOD(sessionInvalidation);
    sensitive << m_BGPHoldDown;
}

BGPSession::~BGPSession()
{

    
}


void BGPSession::sendKeepalive(void)
{
    //send keepalives only if the session is valid
    if (m_SessionValidity)
        {
        
    

            //TODO build the message
    


            //write the message to the control plane
            port_ToDataPlane.write(m_KeepaliveMsg);
        }



    //reset keepalive timer
    resetKeepalive();

    //set next_trigger for this method
    next_trigger(m_Keepalive);
    
}


void BGPSession::sessionInvalidation(void)
{

    sessionStop();

}

void BGPSession::sessionStop(void)
{
    m_BGPHoldDown.cancel();
    m_BGPKeepalive.cancel();
    m_SessionValidity = false;
}

void BGPSession::sessionStart(void)
{
    resetHoldDown();
    resetKeepalive();
    m_SessionValidity = true;
}


void BGPSession::resetKeepalive(void)
{
    m_Keepalive.cancel();
    m_Keepalive.notify(m_KeepaliveTime, SC_SEC);
}

void BGPSession::resetHoldDown(void)
{
    m_HoldDownTime.cancel();
    m_HoldDownTime.notify(m_HoldDownTime, SC_SEC);
}
