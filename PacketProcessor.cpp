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
    buildIPPacket();
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

/*! \sa PacketProcessor
 */
void PacketProcessor::buildIPPacket(void)
{
    unsigned char l_IPPacket[MTU];
    l_IPPacket[0] = 0;
    printf("o: %u\n", l_IPPacket[0]);
    //set version
    setBits(l_IPPacket, VERSION, 4);
    printf("m: %u\n", l_IPPacket[0]);
    setBits(l_IPPacket, 2, 0);
    printf("m: %u\n", l_IPPacket[0]);
}


/*! \sa PacketProcessor
 */
void PacketProcessor::setBits(unsigned char *ptr_Target, unsigned char p_Value, int p_Shift)
{
    
    unsigned char l_Value = p_Value;
    l_Value = l_Value << p_Shift;
    *ptr_Target = *ptr_Target^l_Value;
    
}










