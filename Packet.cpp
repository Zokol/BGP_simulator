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

}

Packet::~Packet(void)
{
}

Packet::Packet(const Packet& p_Packet)
{
    *this = p_Packet;
}


Packet::Packet(BGPMessage& p_BGPPayload, int p_ProtocolType)
{
    m_BGPPayload = p_BGPPayload;
    m_ProtocolType = p_ProtocolType;


}

Packet::Packet(sc_bv<MTU> p_IPPayload, int p_ProtocolType)
{
    m_ProtocolType = p_ProtocolType;
    m_IPPayload = p_IPPayload;

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

void Packet::setIPPayload(sc_bv<MTU> p_IPPayload)
{
    m_IPPayload = p_IPPayload;
}




sc_bv<MTU> Packet::getIPPayload(void)
{

    return m_IPPayload;
}

BGPMessage& Packet::getBGPPayload(void)
{

    return m_BGPPayload;
}

int Packet::getProtocolType(void)
{
    return m_ProtocolType;
}


bool Packet::operator == (const Packet& p_Packet) const {
    return (p_Packet.m_IPPayload == m_IPPayload && p_Packet.m_BGPPayload == m_BGPPayload && p_Packet.m_ProtocolType == m_ProtocolType );
}

Packet& Packet::operator = (const Packet& p_Packet) {
    m_BGPPayload = p_Packet.m_BGPPayload;
    m_IPPayload = p_Packet.m_IPPayload;
    m_ProtocolType = p_Packet.m_ProtocolType;
    return *this;
}







