/*! \file Packet.cpp
 *  \brief     Implementation of Packet class.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   2.0
 *  \date      11.2.2013
 */


#include "Packet.hpp"
#include "StringTools.hpp"





Packet::Packet(void)
{
    initPDU();
}

Packet::~Packet(void)
{
}

Packet::Packet(const Packet& p_Packet)
{
    initPDU();
    *this = p_Packet;
}


Packet::Packet(BGPMessage& p_BGPPayload, int p_ProtocolType)
{
    initPDU();
    m_BGPPayload = p_BGPPayload;
    m_ProtocolType = p_ProtocolType;


}

bool Packet::setProtocolType(int p_ProtocolType)
{
    m_ProtocolType = p_ProtocolType;  
    return true; //TO-DO: validity check
}

void Packet::setBGPPayload(BGPMessage& p_BGPPayload)
{
    m_BGPPayload = p_BGPPayload;
}

BGPMessage& Packet::getBGPPayload(void)
{

    return m_BGPPayload;
}

int Packet::getProtocolType(void)
{
    return m_ProtocolType;
}

void Packet::setPDU(const unsigned char *p_PDU)
{
    for (int i = 0; i < MTU; i++)
        {
            m_PDU[i] = p_PDU[i];
        }
}

/*! \sa Packet
 */
void Packet::getPDU(unsigned char *p_PDU)
{
    
    for (int i = 0; i < MTU; i++)
        {
            p_PDU[i] = m_PDU[i];
        }

}

void Packet::setTCPID(int p_TCPID)
{
	m_TCPID = p_TCPID;
}

int Packet::getTCPID()
{
	return m_TCPID;
}


bool Packet::operator == (const Packet& p_Packet) const {

    for (int i = 0; i < MTU; i++)
        {
            if(m_PDU[i] != p_Packet.m_PDU[i])
                return false;
        }
    return (p_Packet.m_BGPPayload == m_BGPPayload && p_Packet.m_ProtocolType == m_ProtocolType && p_Packet.m_TCPID == m_TCPID );
}

Packet& Packet::operator = (const Packet& p_Packet) {
    m_BGPPayload = p_Packet.m_BGPPayload;
    m_ProtocolType = p_Packet.m_ProtocolType;
    m_TCPID = p_Packet.m_TCPID;
    initPDU();
    setPDU(p_Packet.m_PDU);


    return *this;
}


/*! \sa Packet
 */
void Packet::initPDU(void)
{
for (int i = 0; i < MTU; i++)
    {
        m_PDU[i] = 0;
    }    
}


string Packet::outputPDU(void)
{
    string l_out, l_temp;
    std::stringstream l_ss;
    int i;
    for (i = 0; i < 20; i++)
        {
            if(i%4 == 0)
                l_out += "\n";

            l_out += u8ToS(m_PDU[i]) + ",";
        }

    for (; i < MTU; i++)
        {
            if(i%4 == 0)
                l_out += "\n";
            l_temp = string(1,(char)m_PDU[i]);
            l_out += l_temp + ",";
        }

    l_out += "\n";

    return l_out;
}


string Packet::u8ToS(unsigned char p_Value)
{
    string l_Result = "";
    unsigned char l_Value = p_Value, l_H, l_T;


    l_H = l_Value/100;

    if(l_H != 0)
        {

            l_Value -= (l_H*100);
            l_Result = StringTools().uToS(l_H);
        }
    l_T = l_Value/10;

    if(l_T != 0)
        {

            l_Value -= (l_T*10);

            l_Result += StringTools().uToS(l_T);
        }
    l_Result += StringTools().uToS(l_Value);

    return l_Result;
}

