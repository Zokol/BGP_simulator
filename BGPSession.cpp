/*! \file BGPSession.cpp
 *  \brief     Implementation of BGPSession.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Wed Feb 13 20:36:28 2013
 */


#include "BGPSession.hpp"
#include "ReportGlobals.hpp"

BGPSession::BGPSession(sc_module_name p_ModuleName, BGPSessionParameters * const p_SessionParam):sc_module(p_ModuleName), m_PeerAS(-1), m_PeeringInterface(0), m_Config(p_SessionParam)
{


    m_RTool.setBaseName(name());
    m_RTool.setStampTime(true);
    
    //Register sendKeepalive method to the SystemC kernel
    SC_THREAD(sendKeepalive);
    //    dont_initialize();
    // sensitive << m_BGPKeepalive;

    //Register sessionInvalidation method to the SystemC kernel
    SC_THREAD(sessionInvalidation);
    // dont_initialize();
    // sensitive << m_BGPHoldDown;
 


}

BGPSession::BGPSession(sc_module_name p_ModuleName, int p_PeeringInterface, BGPSessionParameters * const p_SessionParam):sc_module(p_ModuleName), m_PeerAS(-1)
{
    
    m_RTool.setBaseName(name());
    m_RTool.setStampTime(true);

    m_Config = p_SessionParam;


    //set the local interface index behind which the peer locates
    m_PeeringInterface = p_PeeringInterface;

    //Register sendKeepalive method to the SystemC kernel
    SC_THREAD(sendKeepalive);
    //    dont_initialize();
    // sensitive << m_BGPKeepalive;

    //Register sessionInvalidation method to the SystemC kernel
    SC_THREAD(sessionInvalidation);
    // dont_initialize();
    // sensitive << m_BGPHoldDown;


}

BGPSession::~BGPSession()
{

    
}


void BGPSession::sendKeepalive(void)
{
    while(true)
        {
            wait(m_BGPKeepalive);

            //send keepalives only if the session is valid
            if (m_SessionValidity)
                {

                    //TODO build the message
                    m_KeepaliveMsg.m_OutboundInterface = m_PeeringInterface;            
                    m_KeepaliveMsg.m_Type = KEEPALIVE;
                    SC_REPORT_INFO(g_DebugBSID, m_RTool.newReportString("sending keepalive at time"));
                    //write the message to the control plane
                    port_ToDataPlane->write(m_KeepaliveMsg);
                }



            //reset keepalive timer
            resetKeepalive();

        }
    //set next_trigger for this method
    // next_trigger(m_BGPKeepalive);
    
}


void BGPSession::sessionInvalidation(void)
{

    while(true)
        {
            wait(m_BGPHoldDown);

            SC_REPORT_INFO(g_DebugBSID, m_RTool.newReportString("session invalid at time"));

            sessionStop();
        }
    // next_trigger(m_BGPHoldDown);
}

void BGPSession::sessionStop(void)
{
    m_BGPHoldDown.cancel();
    m_BGPKeepalive.cancel();
    m_SessionValidity = false;
}

void BGPSession::sessionStart(void)
{
    // resetHoldDown();
    resetKeepalive();
    m_SessionValidity = true;
}


void BGPSession::resetKeepalive(void)
{
    m_KeepaliveMutex.lock();
    SC_REPORT_INFO(g_DebugBSID, m_RTool.newReportString("resetting keepalive timer"));
    m_BGPKeepalive.cancel();
    m_BGPKeepalive.notify(m_Config->getKeepaliveTime(), SC_SEC);
    m_KeepaliveMutex.unlock();
}

void BGPSession::resetHoldDown(void)
{
    m_BGPHoldDown.cancel();
    m_BGPHoldDown.notify(m_Config->getHoldDownTime(), SC_SEC);
    SC_REPORT_INFO(g_DebugBSID, m_RTool.newReportString("resetting hold-down timer"));
}

bool BGPSession::isSessionValid(void)
{
    return m_SessionValidity;
}

bool BGPSession::isThisSession(string p_BGPIdentifier)
{

    
    return m_BGPIdentifierPeer.compare(p_BGPIdentifier) == 0 ? true: false;
}

void BGPSession::setPeerIdentifier(string p_BGPIdentifier)
{
    m_BGPIdentifierPeer = p_BGPIdentifier;
}


/*! \sa BGPSession
 */
string BGPSession::getPeerAS(void)
{
    
    //return
    return m_RTool.iToS(m_PeerAS);
}

/*! \sa BGPSession
 */
int BGPSession::getPeeringInterface(void)
{
    return m_PeeringInterface;
}

/*! \sa BGPSession
 */
void BGPSession::setPeeringInterface(int p_Interface)
{
    
    //store the arguments
    m_PeeringInterface = p_Interface;
    
}

/*! \sa BGPSession
 */
void BGPSession::setPeerAS(int p_PeerAS)
{
    
    //store the arguments
    m_PeerAS = p_PeerAS;
    
}


