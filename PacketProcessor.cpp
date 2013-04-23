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
    buildIPPacket();
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
    //initialize all fields to zero
    for (int i = 0; i < MTU; i++)
    {
        l_IPPacket[i] = 0;
    }

   
   
    //set version
    setSubField(l_IPPacket, VERSION, VERSION_POS);
    //set standard header length
    setSubField(l_IPPacket, IHL, 0);
    //set the second field to zero (DSCP[7-2] and ECN[1-0])



    //set identification

    //set flags
    clearBit(&l_IPPacket[6], 7); //reserved
    clearBit(&l_IPPacket[6], 6); //Don't fragment DF
    clearBit(&l_IPPacket[6], 5); //More fragments MF
    //set fragment offset

    //set TTL
    l_IPPacket[8] = TTL;
    //set Protocol
    l_IPPacket[9] = PROTOCOL;

    //set source IP
    m_Converter.ipToUChar(m_SourceIP, &l_IPPacket[12]);
    //set destination IP
    m_Converter.ipToUChar(m_DestinationIP, &l_IPPacket[16]);

    //set payload
    unsigned j = 0;
    for (unsigned i = 20; j < m_Payload.length(); i++)
        {
            //trunkate the message if it is too long
            if(i == MTU)
                break;
            l_IPPacket[i] = (unsigned char)m_Payload[j++];
        }
    unsigned l_Length = 0;
    if((m_Payload.length() + 20) > MTU)
        l_Length = MTU;
    else
        l_Length = m_Payload.length() + 20;

    cout << "Length: "<< l_Length << endl;
    //set packet length
    setMultipleFields(&l_IPPacket[3], MTU, 2);    

    //set the IP packet into the frame
    m_Packet.setPDU(l_IPPacket);
    // printf("m: %u\n", l_IPPacket[0]);
}


/*! \sa PacketProcessor
 */
void PacketProcessor::setSubField(unsigned char *ptr_Target, unsigned char p_Value, int p_Shift)
{
    
    unsigned char l_Value = p_Value;
    l_Value = l_Value << p_Shift;
    *ptr_Target = *ptr_Target^l_Value;
    
}


/*! \sa PacketProcessor
 */
void PacketProcessor::setBit(unsigned char *ptr_Target, int p_Position)
{
    *ptr_Target |= 1 << p_Position;     
}


/*! \sa PacketProcessor
 */
void PacketProcessor::clearBit(unsigned char *ptr_Target, int p_Position)
{

    *ptr_Target &= ~(1 << p_Position);    
    
}


/*! \sa PacketProcessor
 */
void PacketProcessor::setMultipleFields(unsigned char *ptr_Target, unsigned p_Value, unsigned p_NumberOfFields)
{
        *ptr_Target |= p_Value;

    for (unsigned i = 1; i < p_NumberOfFields; i++)
        *(ptr_Target - i) |= p_Value >> (8*i);

    
}









