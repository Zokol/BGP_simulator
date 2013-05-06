/*! \file BGPSession.cpp
 *  \brief     Implementation of BGPSession.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Wed Feb 13 20:36:28 2013
 */


#include "BGPSession.hpp"
#include "ReportGlobals.hpp"

BGPSession::BGPSession(sc_module_name p_ModuleName, BGPSessionParameters * const p_SessionParam):sc_module(p_ModuleName), m_PeerAS(-1), m_PeeringInterface(0), m_Config(p_SessionParam), m_ReSend(false), m_RetransmissonCount(0)
{

	m_BGPCurrentState = IDLE;
	m_BGPPreviousState = ACTIVE;
	m_ConnectionCurrentState = SYN;
	m_ConnectionPreviousState = ACK;


	m_RTool.setBaseName(name());
    m_RTool.setStampTime(true);
    
    srand(time(NULL));
    m_TCPId = rand()%0xFFFF;

    //Register sendKeepalive method to the SystemC kernel
    SC_THREAD(sendKeepalive);
    //    dont_initialize();
    // sensitive << m_BGPKeepalive;

    //Register sessionInvalidation method to the SystemC kernel
    SC_THREAD(sessionInvalidation);
    // dont_initialize();
    // sensitive << m_BGPHoldDown;

    //Register retransmissionTimer method to the SystemC kernel
    SC_THREAD(retransmissionTimer);



}

BGPSession::BGPSession(sc_module_name p_ModuleName, int p_PeeringInterface, BGPSessionParameters * const p_SessionParam):sc_module(p_ModuleName), m_PeerAS(-1), m_PeeringInterface(p_PeeringInterface), m_Config(p_SessionParam), m_ReSend(false), m_RetransmissonCount(0)
{

	m_BGPCurrentState = IDLE;
	m_BGPPreviousState = ACTIVE;
	m_ConnectionCurrentState = SYN;
	m_ConnectionPreviousState = ACK;

	m_BGPOut.m_OutboundInterface = m_PeeringInterface;


	m_RTool.setBaseName(name());
    m_RTool.setStampTime(true);


    srand(time(NULL));
    m_TCPId = rand()%0xFFFF;


    //Register sendKeepalive method to the SystemC kernel
    SC_THREAD(sendKeepalive);
    //    dont_initialize();
    // sensitive << m_BGPKeepalive;

    //Register sessionInvalidation method to the SystemC kernel
    SC_THREAD(sessionInvalidation);
    // dont_initialize();
    // sensitive << m_BGPHoldDown;

    //Register retransmissionTimer method to the SystemC kernel
    SC_THREAD(retransmissionTimer);
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

    
}


void BGPSession::sessionInvalidation(void)
{

    while(true)
        {
            wait(m_BGPHoldDown);

            SC_REPORT_INFO(g_ReportID, m_RTool.newReportString("Hold-down timer expired. Session is invalid."));
 
            sessionStop();
        }

}

void BGPSession::retransmissionTimer(void)
{

    while(true)
        {
            wait(m_Retransmission);
            setReSend(true);
        }

}

void BGPSession::fsmRoutine(BGPMessage& p_Input)
{

	m_BGPIn = p_Input;

	//BGP FSM starts
	switch (m_BGPCurrentState)
	{
	case IDLE: //IDLE state
		fsmReportRoutineBGP("BGP state: IDLE");
		//if the session interface is up transition to the CONNECT state
		if(port_InterfaceControl->isUp())
		{
			m_RetransmissonCount = 0;
			m_BGPCurrentState = CONNECT;
			m_ConnectionCurrentState = SYN;
			setRetransmissionTimer(TCP_RT_DELAY);

		}
		break;
	case CONNECT:
		fsmReportRoutineBGP("BGP state: CONNECT");
		switch (m_ConnectionCurrentState)
		{
		case SYN:
			if(m_Config->isClient(m_PeeringInterface)) //client does
			{

					fsmReportRoutineConnection("Connection state: CLIENT SYN");
					stopRetransmissionTimer();
					//build the syn message
					m_BGPOut.m_Type = TCP_SYN;
					m_BGPOut.m_AS = m_TCPId;
					port_ToDataPlane->write(m_BGPOut);
					setRetransmissionTimer(TCP_RT_DELAY);
					m_ConnectionCurrentState = ACK;

			}
			else
			{
				fsmReportRoutineConnection("Connection state: SERVER SYN");

				//handle only if the input is for this session
				if(m_BGPIn.m_OutboundInterface == m_PeeringInterface)
				{
					if(m_BGPIn.m_Type == TCP_SYN)
					{
						stopRetransmissionTimer();
						m_TCPId = m_BGPIn.m_AS;
						m_BGPOut = m_BGPIn;
						m_BGPOut.m_Type = TCP_SYNACK;
						port_ToDataPlane->write(m_BGPOut);
						setRetransmissionTimer(TCP_RT_DELAY);
						m_ConnectionCurrentState = ACK;
					}
				}
				else if(m_ReSend)
					m_BGPCurrentState = ACTIVE;
			}
			break;
		case ACK:
			if(m_Config->isClient(m_PeeringInterface))
			{
				fsmReportRoutineConnection("Connection state: CLIENT ACK");
				//handle only if the input is for this session
				if(m_BGPIn.m_OutboundInterface == m_PeeringInterface)
				{
					if(m_BGPIn.m_Type == TCP_SYNACK)
					{
						if (m_BGPIn.m_AS == m_TCPId)
						{
							stopRetransmissionTimer();
							m_BGPOut = m_BGPIn;
							m_BGPOut.m_Type = TCP_ACK;
							port_ToDataPlane->write(m_BGPOut);
							m_BGPCurrentState = OPEN_SENT;
						}

					}
				}
				else if(m_ReSend)
					m_BGPCurrentState = ACTIVE;

			}
			else
			{
				fsmReportRoutineConnection("Connection state: SERVER ACK");

				if(m_BGPIn.m_OutboundInterface == m_PeeringInterface)
				{
					if(m_BGPIn.m_Type == TCP_ACK)
					{
						if (m_BGPIn.m_AS == m_TCPId)
						{
							stopRetransmissionTimer();
							m_BGPCurrentState = OPEN_SENT;
						}

					}
				}
				else if(m_ReSend)
					m_BGPCurrentState = ACTIVE;

			}
			break;
		default:

			break;
		}
		break;
		case ACTIVE:
			if( m_RetransmissonCount == 0)
			{
				if(m_BGPPreviousState == CONNECT)
				{
					fsmReportRoutineBGP("BGP state: ACTIVE after CONNECT");
					m_RetransmissonCount++;
					m_ConnectionCurrentState = SYN;
					m_BGPCurrentState = CONNECT;
					setReSend(false);
				}
				else if(m_BGPPreviousState == OPEN_SENT)
				{
					fsmReportRoutineBGP("BGP state: ACTIVE after OPEN_SENT");

				}
				else
				{
					fsmReportRoutineBGP("BGP state: ACTIVE after UNKNOWN");
					m_BGPCurrentState = IDLE;
				}
			}
			else
			{
				m_BGPCurrentState = IDLE;
			}

			break;
		case OPEN_SENT:
			fsmReportRoutineBGP("BGP state: OPEN_SENT");

			break;
		case OPEN_CONFIRM:

			break;
		case ESTABLISHED:

			break;
		default:
			break;
	}


}


void BGPSession::sessionStop(void)
{
    m_BGPHoldDown.cancel();
    m_BGPKeepalive.cancel();
    SC_REPORT_INFO(g_ReportID, m_RTool.newReportString("Session stopped."));
    m_SessionValidity = false;
}

void BGPSession::sessionStart(void)
{
    // resetHoldDown();
    resetKeepalive();
    SC_REPORT_INFO(g_ReportID, m_RTool.newReportString("Session started."));
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

void BGPSession::setBGPCurrentState(BGP_States p_State)
{
	m_BGPCurrentState = p_State;
}

void BGPSession::setConnectionCurrentState(TCP_States p_State)
{
	m_ConnectionCurrentState = p_State;
}

void BGPSession::setTCPId(int p_Value)
{
	m_TCPId = p_Value;
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



/*! \sa BGPSession
 */
string BGPSession::getPeerIdentifier(void)
{
    
    //return
    return m_BGPIdentifierPeer;
}

BGP_States BGPSession::getBGPCurrentState()
{
	return m_BGPCurrentState;
}

TCP_States BGPSession::getConnectionCurrentState()
{
	return m_ConnectionCurrentState;
}

int BGPSession::getTCPId()
{
	return m_TCPId;
}

void BGPSession::setRetransmissionTimer(int p_Delay)
{
	m_Retransmission.notify(p_Delay, SC_SEC);
	setReSend(false);
}

void BGPSession::stopRetransmissionTimer(void)
{
	m_Retransmission.cancel();
}
void BGPSession::setReSend(bool p_Value)
{
	m_ReSendMutex.lock();
	m_ReSend = p_Value;
	m_ReSendMutex.unlock();
}

void BGPSession::fsmReportRoutineBGP(string p_Report)
{
	if(m_BGPCurrentState != m_BGPPreviousState)
		SC_REPORT_INFO(g_ReportID, m_RTool.newReportString(p_Report));
	m_BGPPreviousState = m_BGPCurrentState;

}

void BGPSession::fsmReportRoutineConnection(string p_Report)
{
	if(m_ConnectionCurrentState != m_ConnectionPreviousState)
		SC_REPORT_INFO(g_ReportID, m_RTool.newReportString(p_Report));
	m_ConnectionPreviousState = m_ConnectionCurrentState;
}
