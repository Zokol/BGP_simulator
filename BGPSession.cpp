/*! \file BGPSession.cpp
 *  \brief     Implementation of BGPSession.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Wed Feb 13 20:36:28 2013
 */


#include "BGPSession.hpp"
#include "ReportGlobals.hpp"

BGPSession::BGPSession(sc_module_name p_ModuleName, BGPSessionParameters * const p_SessionParam):sc_module(p_ModuleName), m_PeerAS(-1), m_PeeringInterface(0), m_Config(p_SessionParam), m_ReSend(false), m_RetransmissonCount(0), m_KeepaliveFlag(false), m_Client(false), m_NewFsmInput(false)
{

	setBGPCurrentState(IDLE);
	m_BGPPreviousState = ACTIVE;
	m_ConnectionCurrentState = SYN;
	m_ConnectionPreviousState = ACK;

    m_KeepaliveMsg.m_OutboundInterface = m_PeeringInterface;
    m_KeepaliveMsg.m_Type = KEEPALIVE;

	m_RTool.setBaseName(name());
    m_RTool.setStampTime(true);
    
    srand(time(NULL));
    m_TCPId = rand()%0xFFFF;

    //Register keepaliveTimer method to the SystemC kernel
    SC_THREAD(keepaliveTimer);
    //    dont_initialize();
    // sensitive << m_BGPKeepalive;

    //Register sessionInvalidation method to the SystemC kernel
    SC_THREAD(sessionInvalidation);
    // dont_initialize();
    // sensitive << m_BGPHoldDown;

    //Register retransmissionTimer method to the SystemC kernel
    SC_THREAD(retransmissionTimer);

    SC_THREAD(fsmRoutine);
    sensitive << port_Clk;


}

BGPSession::BGPSession(sc_module_name p_ModuleName, int p_PeeringInterface, BGPSessionParameters * const p_SessionParam):sc_module(p_ModuleName), m_PeerAS(-1), m_PeeringInterface(p_PeeringInterface), m_Config(p_SessionParam), m_ReSend(false), m_RetransmissonCount(0), m_KeepaliveFlag(false), m_Client(false), m_NewFsmInput(false)
{

	setBGPCurrentState(IDLE);
	m_BGPPreviousState = ACTIVE;
	m_ConnectionCurrentState = SYN;
	m_ConnectionPreviousState = ACK;

	m_BGPOut.m_OutboundInterface = m_PeeringInterface;

    m_KeepaliveMsg.m_OutboundInterface = m_PeeringInterface;
    m_KeepaliveMsg.m_Type = KEEPALIVE;

	m_RTool.setBaseName(name());
    m_RTool.setStampTime(true);


    srand(time(NULL));
    m_TCPId = rand()%0xFFFF;






    //Register keepaliveTimer method to the SystemC kernel
    SC_THREAD(keepaliveTimer);
    //    dont_initialize();
    // sensitive << m_BGPKeepalive;

    //Register sessionInvalidation method to the SystemC kernel
    SC_THREAD(sessionInvalidation);
    // dont_initialize();
    // sensitive << m_BGPHoldDown;

    //Register retransmissionTimer method to the SystemC kernel
    SC_THREAD(retransmissionTimer);

    SC_THREAD(fsmRoutine);
    sensitive << port_Clk;

}

BGPSession::~BGPSession()
{

    
}


void BGPSession::keepaliveTimer(void)
{
    while(true)
        {
            wait(m_BGPKeepalive);

            setKeepaliveFlag(true);


        }

    
}

void BGPSession::setKeepaliveFlag(bool p_Value)
{
	m_KeepaliveFlagMutex.lock();
	m_KeepaliveFlag = p_Value;
	m_KeepaliveFlagMutex.unlock();
}

bool BGPSession::isKeepaliveTime(void)
{
	return m_KeepaliveFlag;
}

void BGPSession::sendKeepalive(void)
{
	port_ToDataPlane->write(m_KeepaliveMsg);

}

void BGPSession::sessionInvalidation(void)
{

    while(true)
        {
            wait(m_BGPHoldDown);

            SC_REPORT_INFO(g_ReportID, m_RTool.newReportString("Hold-down timer expired. Session is invalid."));
            setBGPCurrentState(IDLE);

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

void BGPSession::fsmRoutine(void)
{

	while(true)
	{

		wait();
		if(m_FsmInputBuffer.num_available() > 0)
		{
			m_BGPInPrevious = m_BGPIn;
			m_FsmInputBuffer.read(m_BGPIn);
			if(m_BGPInPrevious == m_BGPIn)
				m_NewFsmInput = false;
			else
				m_NewFsmInput = true;
		}
		else
			m_NewFsmInput = false;

		//BGP FSM starts
		switch (m_BGPCurrentState)
		{
		case IDLE: //IDLE state
			fsmReportRoutineBGP("BGP state: IDLE");

			//make sure that the session is stopped
			sessionStop();
			//if the session interface is up transition to the CONNECT state
			if(port_InterfaceControl->isUp())
			{
				m_RetransmissonCount = 0;
				setBGPCurrentState(CONNECT);
				m_ConnectionCurrentState = SYN;
				m_BeeingHere = false;
				setRetransmissionTimer(TCP_RT_DELAY);

			}
			break;
		case CONNECT:
			fsmReportRoutineBGP("BGP state: CONNECT");
			switch (m_ConnectionCurrentState)
			{
			case SYN:
				if(m_Client) //client does
				{

					fsmReportRoutineConnection("Connection state: CLIENT SYN");
					stopRetransmissionTimer();
					//build the syn message
					m_BGPOut.m_Type = TCP_SYN;
					m_BGPOut.m_AS = m_TCPId;
					port_ToDataPlane->write(m_BGPOut);
					setRetransmissionTimer(TCP_RT_DELAY);
					m_ConnectionCurrentState = ACK;
					m_BeeingHere = true;

				}
				else
				{
					fsmReportRoutineConnection("Connection state: SERVER SYN");

					//handle only if the input is for this session
					if(m_BGPIn.m_OutboundInterface == m_PeeringInterface && m_NewFsmInput)
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
						setBGPCurrentState(ACTIVE);
				}
				break;
			case ACK:
				if(m_Client)
				{
					fsmReportRoutineConnection("Connection state: CLIENT ACK");
					//handle only if the input is for this session
					if(m_BGPIn.m_OutboundInterface == m_PeeringInterface && m_NewFsmInput)
					{
						if(m_BGPIn.m_Type == TCP_SYNACK)
						{
							if (m_BGPIn.m_AS == m_TCPId)
							{
								stopRetransmissionTimer();
								m_BGPOut = m_BGPIn;
								m_BGPOut.m_Type = TCP_ACK;
								port_ToDataPlane->write(m_BGPOut);
								m_ConnectionCurrentState = OPEN_SEND;
							}

						}
					}
					else if(m_ReSend)
						setBGPCurrentState(ACTIVE);

				}
				else
				{
					fsmReportRoutineConnection("Connection state: SERVER ACK");

					if(m_BGPIn.m_OutboundInterface == m_PeeringInterface && m_NewFsmInput)
					{
						if(m_BGPIn.m_Type == TCP_ACK)
						{
							if (m_BGPIn.m_AS == m_TCPId)
							{
								stopRetransmissionTimer();
								m_ConnectionCurrentState = OPEN_SEND;
							}

						}
					}
					else if(m_ReSend)
						setBGPCurrentState(ACTIVE);

				}
				break;
			case OPEN_SEND:
				if(m_Client)
					fsmReportRoutineConnection("Connection state: CLIENT OPEN_SEND");
				else
					fsmReportRoutineConnection("Connection state: SERVER OPEN_SEND");

				//build the open message
				m_BGPOut.m_Type = OPEN;
				m_BGPOut.m_BGPIdentifier = m_Config->getBGPIdentifier();
				m_BGPOut.m_HoldDownTime = m_Config->getHoldDownTime();
				m_BGPOut.m_AS = m_Config->getASNumber();
				m_BGPOut.m_OutboundInterface = m_PeeringInterface;
				//cout << name() << " peering interface is " << m_BGPOut.m_OutboundInterface << endl;
				port_ToDataPlane->write(m_BGPOut);
				setRetransmissionTimer(OPENSEND_RT_DELAY);
				setBGPCurrentState(OPEN_SENT);

				break;
			default:

				break;
			}
			break;
			case ACTIVE:
				if( m_RetransmissonCount == 0)
				{
					m_RetransmissonCount++;
					m_ConnectionCurrentState = SYN;
					setBGPCurrentState(CONNECT);
					setReSend(false);
					if(m_BGPPreviousState == CONNECT)
					{
						fsmReportRoutineBGP("BGP state: ACTIVE after CONNECT");
					}
					else if(m_BGPPreviousState == OPEN_SENT)
					{
						fsmReportRoutineBGP("BGP state: ACTIVE after OPEN_SENT");

					}
					else
					{
						fsmReportRoutineBGP("BGP state: ACTIVE after UNKNOWN");
						setBGPCurrentState(IDLE);
					}
				}
				else
				{
					setBGPCurrentState(IDLE);
				}

				break;
			case OPEN_SENT:
				fsmReportRoutineBGP("BGP state: OPEN_SENT");

				if(m_BGPIn.m_OutboundInterface == m_PeeringInterface && m_NewFsmInput)
				{
					//				cout << name() << ":: received an open message from bgp id:" << m_BGPIn.m_BGPIdentifier << ", type: " << m_BGPIn.m_Type << ":@ "<< sc_time_stamp() << endl;
					if(m_BGPIn.m_Type == OPEN)
					{
						stopRetransmissionTimer();
						//agree on HoldDown time
						if(m_Config->getHoldDownTime() > m_BGPIn.m_HoldDownTime)
							m_Config->setHoldDownTime(m_BGPIn.m_HoldDownTime);
						m_Config->setASNumber(m_BGPIn.m_AS);
						setPeerIdentifier(m_BGPIn.m_BGPIdentifier);
						sendKeepalive();
						setRetransmissionTimer(m_Config->getHoldDownTime());
						setBGPCurrentState(OPEN_CONFIRM);
					}
				}
				else if(m_ReSend)
					setBGPCurrentState(ACTIVE);

				break;
			case OPEN_CONFIRM:
				fsmReportRoutineBGP("BGP state: OPEN_CONFIRM");

				if(m_BGPIn.m_OutboundInterface == m_PeeringInterface && m_NewFsmInput)
				{
					if(m_BGPIn.m_Type == KEEPALIVE)
					{
						stopRetransmissionTimer();
						//cout << name() << " STARTING SESSION" << endl;
						sessionStart();
						setBGPCurrentState(ESTABLISHED);
					}
				}
				else if(m_ReSend)
					setBGPCurrentState(IDLE);

				break;
			case ESTABLISHED:
				fsmReportRoutineBGP("BGP state: ESTABLISHED");

				//verify that the peering interface is up
				if(port_InterfaceControl->isUp())
				{

					//verify that the message is for this session and that it is not a duplicate
					if(m_BGPIn.m_OutboundInterface == m_PeeringInterface && m_NewFsmInput)
					{
						//reset hold-down timer
						resetHoldDown();
						//send updates to RT
						if(m_BGPIn.m_Type == UPDATE)
						{
							port_ToRoutingTable->write(m_BGPIn);

						}
						//transition to IDLE state in case of notification
						else if(m_BGPIn.m_Type == NOTIFICATION)
						{
							setBGPCurrentState(IDLE);
						}
						//in any other case send a notificaton to the peer and transition to the IDLE state
						else
						{
//							m_BGPOut.m_Type = NOTIFICATION;
//							m_BGPOut.m_BGPIdentifier = m_Config->getBGPIdentifier();
//							m_BGPOut.m_AS = m_Config->getASNumber();
//							m_BGPOut.m_OutboundInterface = m_PeeringInterface;
//							port_ToDataPlane->write(m_BGPOut);
//							setBGPCurrentState(IDLE);
						}
					}

					if(isKeepaliveTime())
					{
						sendKeepalive();
						resetKeepalive();
					}


				}
				else
					setBGPCurrentState(IDLE);

				break;
			default:
				break;
		}
	}

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
    SC_REPORT_INFO(g_ReportID, m_RTool.newReportString("Session started."));
    m_SessionValidity = true;
}


void BGPSession::resetKeepalive(void)
{
    m_KeepaliveMutex.lock();
    SC_REPORT_INFO(g_DebugBSID, m_RTool.newReportString("resetting keepalive timer"));
    m_BGPKeepalive.cancel();
    setKeepaliveFlag(false);
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
	m_BGPCurrentStateMutex.lock();
	m_BGPCurrentState = p_State;
	m_BGPCurrentStateMutex.unlock();
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
	m_RetransmissonCount = 0;
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
	if(m_ConnectionCurrentState != m_ConnectionPreviousState && m_PeeringInterface == 0)
		SC_REPORT_INFO(g_ReportID, m_RTool.newReportString(p_Report));
	m_ConnectionPreviousState = m_ConnectionCurrentState;
}
