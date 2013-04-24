/*! \file PacketProcessor.cpp
 *  \brief Implementation of PacketProcessor methods
 *  \details
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date    Thu Apr 18 09:53:49 2013
 */

#include "PacketProcessor.hpp"
#include "ReportGlobals.hpp"


PacketProcessor::PacketProcessor():m_DestinationIP("127.0.0.2"), m_SourceIP("127.0.0.1"), m_Payload("--"), m_Valid(false)
{
    resetPacketBuffer();
}

PacketProcessor::~PacketProcessor()
{

}

/*! \sa PacketProcessor
 */
string PacketProcessor::readIPPacket(void)
{
    //Start tag
    string l_IPString = "";

    ///READ HEADER FIELDS
    //VERSION
    l_IPString += m_Converter.uToS(readSubField(m_PacketBuffer[0], 7, 4));
    l_IPString += ",";
    //IHL
    l_IPString += m_Converter.uToS(readSubField(m_PacketBuffer[0], 3, 0));
    l_IPString += ",";
    //DSCP
    l_IPString += m_Converter.uToS(readSubField(m_PacketBuffer[1], 7, 2));
    l_IPString += ",";
    //ECN
    l_IPString += m_Converter.uToS(readSubField(m_PacketBuffer[1], 1, 0));
    l_IPString += ",";
    //LENGTH
    unsigned short l_Length = readShort(&m_PacketBuffer[3]);
    l_IPString += m_Converter.uToS(l_Length);
    l_IPString += ",";
    //IDENTIFICATION
    l_IPString += m_Converter.uToS(readShort(&m_PacketBuffer[5]));
    l_IPString += ",";
    //FLAGS
    l_IPString += readBit(m_PacketBuffer[6], 7)?"1":"0"; //reserved
    l_IPString += ",";
    l_IPString += readBit(m_PacketBuffer[6], 6)?"1":"0"; //Don't fragment DF
    l_IPString += ",";
    l_IPString += readBit(m_PacketBuffer[6], 5)?"1":"0"; //More fragments MF
    l_IPString += ",";
    //FRAGMENT OFFSET
    l_IPString += m_Converter.uToS(readFragmentOffSet());
    l_IPString += ",";
    //TTL
    l_IPString += m_Converter.uToS(m_PacketBuffer[8]);
    l_IPString += ",";
    //PROTOCOL
    l_IPString += m_Converter.uToS(m_PacketBuffer[9]);
    l_IPString += ",";
    //CHECKSUM
    l_IPString += m_Converter.uToS(readShort(&m_PacketBuffer[11]));
    l_IPString += ",";
    //SOURCE IP
    l_IPString += m_Converter.ipToString(&m_PacketBuffer[12]);
    l_IPString += ",";
    //DESTINATION IP
    l_IPString += m_Converter.ipToString(&m_PacketBuffer[16]);
    l_IPString += ",";
    //return

    ///READ PAYLOAD

    for (int i = HEADER_LENGTH; i < l_Length; i++)
        {
            if(m_PacketBuffer[i] == 32)
                l_IPString += "_";
            else
                l_IPString += m_PacketBuffer[i];

        }

    l_IPString += ";";
    return l_IPString;
}

/*! \sa PacketProcessor
 */
bool PacketProcessor::processFrame(Packet& p_Frame)
{
    
    //store the argument
    m_Frame = p_Frame;
    m_Valid = confirmCheckSum();    

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
   
    //set version
    setSubField(m_PacketBuffer, VERSION, VERSION_POS);
    //set standard header length
    setSubField(m_PacketBuffer, IHL, 0);
    //set the second field to zero (DSCP[7-2] and ECN[1-0])

    //set identification

    //set flags
    clearBit(&m_PacketBuffer[6], 7); //reserved
    clearBit(&m_PacketBuffer[6], 6); //Don't fragment DF
    clearBit(&m_PacketBuffer[6], 5); //More fragments MF
    //set fragment offset

    //set TTL
    m_PacketBuffer[8] = TTL;
    //set Protocol
    m_PacketBuffer[9] = PROTOCOL;

    //set source IP
    m_Converter.ipToUChar(m_SourceIP, &m_PacketBuffer[12]);
    //set destination IP
    m_Converter.ipToUChar(m_DestinationIP, &m_PacketBuffer[16]);


    //set paylod and packet length
    setMultipleFields(&m_PacketBuffer[3], setPayload(&m_PacketBuffer[20]), 2);    

    addCheckSum(m_PacketBuffer);
    //set the IP packet into the frame
    m_Frame.setPDU(m_PacketBuffer);



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
bool PacketProcessor::readBit(unsigned p_Field, unsigned p_BitPosition)
{
    unsigned l_Result = p_Field & (1 << p_BitPosition);

    //return
    return l_Result?true:false;
}

/*! \sa PacketProcessor
 */
unsigned PacketProcessor::readSubField(unsigned p_Value, unsigned p_MSB, unsigned p_LSB)
{
    unsigned l_BuildMask = 0xffffffff, l_Mask = 0xffffffff, l_Value = 0;
    //shift the ones to the left. The MSB of the sub field is the
    //last 0 bit
    l_Mask <<= p_MSB+1;
    //shift the ones to the rigth. The LSB of the sub field is the
    //first 0 bit 
    l_BuildMask >>= (sizeof(unsigned)*8) - p_LSB;
    //combine the masks
    l_Mask ^= l_BuildMask;
    //inverse the mask. Now the subfield portion is set to ones and
    //the rest of the bits are set to zeros
    l_Mask = ~l_Mask;
    //mask the subfield value
    l_Value = p_Value & l_Mask;
    //shift the value to the correct position
    l_Value >>= p_LSB;
    //return the value
    return l_Value;
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
unsigned short PacketProcessor::calculateCheckSum(unsigned char *ptr_PacketBuffer)
{
    //checksum temp-store
    unsigned short l_16b=0;
    unsigned l_32b=0;
    //sum the header fields in chunks of short
    for (int i = 1; i < HEADER_LENGTH; i += 2)
        {
            //read next short from the header
            l_16b = readShort(&ptr_PacketBuffer[i]);

            //add it with the previous short
            l_32b += l_16b;            
        }


    //Store low order bits
    l_16b = 0;
    l_16b |= l_32b;

    //shift the carry bits
    l_32b >>= 16;

    //add the carry bits and low order bits
    l_16b += l_32b;
    return l_16b;    

}

/*! \sa PacketProcessor
 */
void PacketProcessor::addCheckSum(unsigned char *ptr_PacketBuffer)
{
    
    //sum the header fields
    unsigned short l_16b = calculateCheckSum(ptr_PacketBuffer);

    //inverse the sum

    l_16b = ~l_16b;

    //set the result to the checksum field of the header
    setMultipleFields(&ptr_PacketBuffer[CHECKSUM_FIELD], l_16b, 2);
}


/*! \sa PacketProcessor
 */
bool PacketProcessor::confirmCheckSum(void)
{
    //reset the buffer
    resetPacketBuffer();
    //get the IP packet from the frame
    m_Frame.getPDU(m_PacketBuffer);
    //perform the first portion of the checksum calculation
    unsigned short l_Result = calculateCheckSum(m_PacketBuffer);
    //check the result
    if(l_Result != 0xFFFF)
        return false;//The l_Result was not all ones: Checksum is not valid
    else
        return true;//The l_Result was all ones: Checksum is valid
}

/*! \sa PacketProcessor
 */
void PacketProcessor::resetPacketBuffer(void)
{
    //initialize all fields to zero
    for (int i = 0; i < MTU; i++)
        m_PacketBuffer[i] = 0;
}

/*! \sa PacketProcessor
 */
unsigned short PacketProcessor::readFragmentOffSet(void)
{
    //return
    return readShort(&m_PacketBuffer[7]) & 0x1fff;
}




//17664+42+3857+49320+258+49320+257 = 120718
