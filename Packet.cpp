/*! \file Packet.cpp
 *  \brief     Implementation of Packet class.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   2.0
 *  \date      11.2.2013
 */


#include "Packet.hpp"





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



bool Packet::operator == (const Packet& p_Packet) const {

    for (int i = 0; i < MTU; i++)
        {
            if(m_PDU[i] != p_Packet.m_PDU[i])
                return false;
        }
    return (p_Packet.m_BGPPayload == m_BGPPayload && p_Packet.m_ProtocolType == m_ProtocolType );
}

Packet& Packet::operator = (const Packet& p_Packet) {
    m_BGPPayload = p_Packet.m_BGPPayload;
    m_ProtocolType = p_Packet.m_ProtocolType;
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








