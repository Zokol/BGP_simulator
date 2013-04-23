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
string PacketProcessor::readIPPacket(void)
{
    
    //return
    return "";
}

/*! \sa PacketProcessor
 */
bool PacketProcessor::processPacket(Packet& p_Frame)
{
    
    //store the argument
    m_Frame = p_Frame;
    
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
    return m_Frame;
}

/*! \sa PacketProcessor
 */
void PacketProcessor::buildIPPacket(string p_DestinationIP, string p_SourceIP, string p_Payload)
{

    //store the arguments
    m_DestinationIP = p_DestinationIP;
    m_SourceIP = p_SourceIP;
    m_Payload = p_Payload;    

    //local buffer for the packet
    unsigned char l_IPPacket[MTU];
    //initialize all fields to zero
    for (int i = 0; i < MTU; i++)
        l_IPPacket[i] = 0;
   
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


    //set paylod and packet length
    setMultipleFields(&l_IPPacket[3], setPayload(&l_IPPacket[20]), 2);    

    calculateCheckSum(l_IPPacket);

    //set the IP packet into the frame
    m_Frame.setPDU(l_IPPacket);

}


/*! \sa PacketProcessor
 */
void PacketProcessor::setSubField(unsigned char *ptr_PacketBuffer, unsigned char p_Value, int p_Shift)
{
    
    unsigned char l_Value = p_Value;
    l_Value = l_Value << p_Shift;
    *ptr_PacketBuffer = *ptr_PacketBuffer^l_Value;
    
}


/*! \sa PacketProcessor
 */
void PacketProcessor::setBit(unsigned char *ptr_PacketBuffer, int p_Position)
{
    *ptr_PacketBuffer |= 1 << p_Position;     
}


/*! \sa PacketProcessor
 */
void PacketProcessor::clearBit(unsigned char *ptr_PacketBuffer, int p_Position)
{

    *ptr_PacketBuffer &= ~(1 << p_Position);    
    
}


/*! \sa PacketProcessor
 */
void PacketProcessor::setMultipleFields(unsigned char *ptr_PacketBuffer, unsigned p_Value, unsigned p_NumberOfFields)
{
        *ptr_PacketBuffer |= p_Value;

    for (unsigned i = 1; i < p_NumberOfFields; i++)
        *(ptr_PacketBuffer - i) |= p_Value >> (8*i);

    
}
 
/*! \sa PacketProcessor
 */
unsigned short PacketProcessor::readShort(unsigned char *ptr_PacketBuffer)
{
    
    unsigned short l_16b = 0;
    l_16b |= *(ptr_PacketBuffer-1);
    l_16b <<= 8;
    l_16b ^= *ptr_PacketBuffer;

    
    //return
    return l_16b;
}



/*! \sa PacketProcessor
 */
unsigned PacketProcessor::setPayload(unsigned char *ptr_PacketBuffer)
{


    //set payload
    unsigned j = 0;
    for (unsigned i = 0; j < m_Payload.length(); i++)
        {
            //trunkate the message if it is too long
            if(i == MTU)
                break;
            ptr_PacketBuffer[i] = (unsigned char)m_Payload[j++];
        }

    if((m_Payload.length() + HEADER_LENGTH) > MTU)
        return MTU;
    else
        return m_Payload.length() + HEADER_LENGTH;
    
    
}

/*! \sa PacketProcessor
 */
void PacketProcessor::calculateCheckSum(unsigned char *ptr_PacketBuffer)
{
    //checksum temp-store
    unsigned short l_16b=0;
    unsigned l_32b=0;
    //sum the header fields in chunks of short
    for (int i = 1; i < HEADER_LENGTH; i += 2)
        {
            //read next short from the header
            l_16b = readShort(&ptr_PacketBuffer[i]);
            cout << i <<". short: " << l_16b << endl;
            //add it with the previous short
            l_32b += l_16b;            
        }
    cout << "Header sum: " << l_32b << endl;

    //Store low order bits
    l_16b = 0;
    l_16b |= l_32b;
    cout << "Low order byte: " << l_16b << endl;
    //shift the carry bits
    l_32b >>= 16;
    cout << "Carry bits: " << l_32b << endl;
    //add the carry bits and low order bits
    l_16b += l_32b;
    cout << "Carry bitss + Low order byte = " << l_16b << endl;
    //inverse the sum
    cout << "before inversion: " << l_16b << endl;
    l_16b = ~l_16b;
    cout << "inversed: " << l_16b << endl;
    //set the result to the checksum field of the header
    setMultipleFields(&ptr_PacketBuffer[CHECKSUM_FIELD], l_16b, 2);

    
}








//17664+42+3857+49320+258+49320+257 = 120718
