/*! \file BGPSession.cpp
 *  \brief     Implementation of BGPSession.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Wed Feb 13 20:36:28 2013
 */


#include "BGPSession.hpp"


BGPSession::BGPSession(sc_module_name p_ModuleName, BGPSessionParameters& p_SessionParam):sc_module(p_ModuleName)
{

    m_Config = p_SessionParam;

    
    //Register sendKeepalive method to the SystemC kernel
    SC_METHOD(sendKeepalive);
    dont_initialize();
    sensitive << m_BGPKeepalive;

    //Register sessionInvalidation method to the SystemC kernel
    SC_METHOD(sessionInvalidation);
    dont_initialize();
    sensitive << m_BGPHoldDown;


}

BGPSession::BGPSession(sc_module_name p_ModuleName, int p_PeeringInterface, BGPSessionParameters& p_SessionParam):sc_module(p_ModuleName)
{
    

    m_Config = p_SessionParam;


    //set the local interface index behind which the peer locates
    m_PeeringInterface = p_PeeringInterface;
    
    //Register sendKeepalive method to the SystemC kernel
    SC_METHOD(sendKeepalive);
    //    dont_initialize();
    sensitive << m_BGPKeepalive;

    //Register sessionInvalidation method to the SystemC kernel
    SC_METHOD(sessionInvalidation);
    //    dont_initialize();
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
            

            cout << name() << " sending keepalive at time " << sc_time_stamp() << endl;
            //write the message to the control plane
            port_ToDataPlane->write(m_KeepaliveMsg);
        }



    //reset keepalive timer
    resetKeepalive();

    //set next_trigger for this method
    next_trigger(m_BGPKeepalive);
    
}


void BGPSession::sessionInvalidation(void)
{
    cout << name() << " session invalid at time " << sc_time_stamp()  << endl;

    sessionStop();
    next_trigger(m_BGPHoldDown);
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
    m_KeepaliveMutex.lock();
    m_BGPKeepalive.cancel();
    m_BGPKeepalive.notify(m_Config.getKeepaliveTime(), SC_SEC);
    m_KeepaliveMutex.unlock();
}

void BGPSession::resetHoldDown(void)
{
    m_BGPHoldDown.cancel();
    m_BGPHoldDown.notify(m_Config.getHoldDownTime(), SC_SEC);
}

bool BGPSession::isSessionValid(void)
{
    return m_SessionValidity;
}

bool BGPSession::isThisSession(sc_int<32> p_BGPIdentifier)
{

    
    return m_BGPIdentifierPeer == p_BGPIdentifier ? true : false;
}

void BGPSession::setPeerIdentifier(sc_int<32> p_BGPIdentifier)
{
    m_BGPIdentifierPeer = p_BGPIdentifier;
}

