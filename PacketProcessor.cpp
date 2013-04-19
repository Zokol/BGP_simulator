/*! \file PacketProcessor.cpp
 *  \brief Implementation of PacketProcessor methods
 *  \details
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date    Thu Apr 18 09:53:49 2013
 */

#include "PacketProcessor.hpp"
#include "ReportGlobals.hpp"


PacketProcessor::PacketProcessor():m_DestinationIP("127.0.0.2"), m_SourceIP("127.0.0.1"), m_Payload("--")
{

}

PacketProcessor::~PacketProcessor()
{

}

/*! \sa PacketProcessor
 */
void PacketProcessor::sendMessage(string p_DestinationIP, string p_SourceIP, string p_Payload)
{
    
    //store the arguments
    m_DestinationIP = p_DestinationIP;
    m_SourceIP = p_SourceIP;
    m_Payload = p_Payload;
    
}

/*! \sa PacketProcessor
 */
string PacketProcessor::readMessage(void)
{
    
    //return
    return "";
}

/*! \sa PacketProcessor
 */
bool PacketProcessor::processPacket(Packet& p_Packet)
{
    
    //store the argument
    m_Packet = p_Packet;
    
    //return
    return m_Valid;
}

/*! \sa PacketProcessor
 */
string PacketProcessor::getDestination(void)
{
    
    //return
    return m_DestinationIP;
}

/*! \sa PacketProcessor
 */
Packet& PacketProcessor::forward(void)
{
    
    
    //return
    return m_Packet;
}






