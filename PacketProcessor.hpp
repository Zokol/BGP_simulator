/*! \file  PacketProcessor.hpp
 *  \brief     Header file of PacketProcessor class
 *  \details
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Thu Apr 18 09:57:22 2013
 */

/*!
 * \class PacketProcessor
 * \brief PacketProcessor class
 *  \details Allows to send, reiceve, and forward IP packets
 */

#include "systemc"
#include "Packet.hpp"
#include "StringTools.hpp"

using namespace std;




#ifndef _PACKETPROCESSOR_H_
#define _PACKETPROCESSOR_H_

/*!
 *The length of short field
 */
#define SHORT 2

/*!
 *IP Protocol version
 */
#define VERSION 4
/*!
 *Standard header lenght
 */
#define IHL 5
/*!
 *Minimum packet length
 */
#define MIN_LENGTH 20
/*!
 *Minimum packet length
 */
#define MAX_LENGTH 65535
/*!
 *Time to leave TTL initial value
 */
#define TTL 15
/*!
 *Protocol type in IP datagram
 */
#define PROTOCOL 17
/*!
 *Defines the postion of the low order byte of checksum field
 */
#define CHECKSUM_FIELD 11

/*!
 *Header length
 */
#define HEADER_LENGTH 20







/*! \class PacketProcessor
 * \brief Performs all the tasks related to IP packet processing
 * \details 
 * \public
 */
class PacketProcessor
{
public:

    PacketProcessor(const char *p_Name);

    virtual ~PacketProcessor();

    /*! \fn void buildIPPacket(void) 
     * \brief Builds an IP packet using the passed fields as data
     * \details 
     * @param [in] string p_DestinationIP
     * @param [in] string p_SourceIP
     * @param [in] string p_Payload
     * \return Packet& reference to the frame that contains the built packet
     * \public
     */
    Packet& buildIPPacket(string p_DestinationIP, string p_SourceIP, string p_Payload);
    
    /*! \fn string readIPPacket(void); 
     * \brief Outputs the contents of an IP packet
     * \details The header and payload is written in the return string
     * \return string: the complete IP packet as a string 
     * \public
     */
    string readIPPacket(void);
    
    /*! \fn bool processFrame(Packet& p_Packet); 
     * \brief Validates an IP packet 
     * \details Check whether the passed packet is a valid IP packet. 
     * @param [in] Packet p_Packet IP packet to be processed
     * \return bool: true == Packet is valid || false == Packet is not valid
     * \public
     */
    bool processFrame(Packet& p_Packet);

    /*! \fn string getDestination(void); 
     * \brief Returns the destination of the processed packet
     * \details 
     * \return string: the destination IP address as string 
     * \public
     */
    string getDestination(void);

    /*! \fn bool forward(Packet& p_Frame); 
     * \brief copies the processed frame to the passed reference
     * \details decrements the TTL and recalculates the checksum using the incremental method
     * @param[out] Packet& p_Frame Reference to the object in which the processed frame should be copied 
     * \return bool true: packet is ready to be forwarded - false: packet was dropped
     * \public
     */
    bool forward(Packet& p_Frame);
    
private:

    /*! \property string m_DestinationIP 
     * \brief The destination IP of the packet
     * \details 
     * \private
     */
    string m_DestinationIP;
    
    /*! \property string m_SourceIP 
     * \brief The source IP of the packet
     * \details 
     * \private
     */
    string m_SourceIP;
    
    /*! \property string m_Payload 
     * \brief The payload of the packet
     * \details 
     * \private
     */
    string m_Payload;

    /*! \property Packet m_Frame 
     * \brief The frame in which the packet will be encapsulated
     * \details 
     * \private
     */
    Packet m_Frame;

    /*! \property bool m_Valid 
     * \brief Indicates whether the packet is valid or not
     * \details 
     * \private
     */
    bool m_Valid;

    /*! \property StringTools m_Converter 
     * \brief Tool to convert IP from string to binary
     * \details 
     * \private
     */
    StringTools m_Converter;  

    /*! \property unsigned char m_PacketBuffer[MTU] 
     * \brief Buffer for the IP packet being processed
     * \details 
     * \private
     */
    unsigned char m_PacketBuffer[MTU];

    /*! \property unsinged short m_Identification 
     * \brief The packet identification value
     * \details 
     * \private
     */
    unsigned short m_Identification;
    
    
    
    /*! \fn unsigned setSubField(unsigned p_Field, unsigned char p_Value, unsigned p_MSB, unsigned p_LSB); 
     * \brief Set the value into the target. p_Shift defines the LSBbit position
     * \details 
     * @param [in] unsigned p_Field The field in which the value will
     * be added
     * @param [in] unsigned p_Value  The value to be added into the subfield
     * @param [in] unsigned p_MSB The high order bit of the subfield
     * @param [in] unsigned p_LSB The low order bit of the subfield
     * \return unsigned: The field conataining the new value in the
     * defined subfield
     * \private
     */
    unsigned setSubField(unsigned p_Field, unsigned char p_Value, unsigned p_MSB, unsigned p_LSB);

    /*! \fn void setBit(unsigned char p_Target, int p_Position); 
     * \brief Set the bit in position p_Position of the field pointed by ptr_Target.
     * \details 
     * @param [out] unsigned char *ptr_PacketBuffer  
     * @param [in] int p_Position  
     * \private
     */
    void setBit(unsigned char *ptr_PacketBuffer, int p_Position);
    
    /*! \fn void clearBit(unsigned char *p_Target, int p_Position); 
     * \brief Clears the bit in position p_Position of the field pointed by ptr_Target
     * \details 
     * @param [out] unsigned char *ptr_PacketBuffer  
     * @param [in] int p_Position  
     * \private
     */
    void clearBit(unsigned char *ptr_PacketBuffer, int p_Position);

    /*! \fn void setMultipleFields(unsigned char *ptr_PacketBuffer, unsigned p_Value, unsigned p_NumberOfFields); 
     * \brief Sets the value in the target buffer
     * \details 
     * @param [out] unsigned char *ptr_PacketBuffer 
     * @param [in] unsigned p_Value  
     * @param [in] unsigned p_NumberOfFields 
     * \private
     */
    void setMultipleFields(unsigned char *ptr_PacketBuffer, unsigned p_Value, unsigned p_NumberOfFields);

    /*! \fn unsigned setPayload(unsigned char *ptr_PacketBuffer); 
     * \brief Set the payload to the IP packet and determinesthelengthof the packet
     * \details 
     * @param [out] unsigned char *ptr_PacketBuffer The target buffer in
     * which the IP packet is being built to
     * \return unsigned: The complete length of the built IP packet 
     * \private
     */
    unsigned setPayload(unsigned char *ptr_PacketBuffer);

    /*! \fn unsigned short readShort(unsigned char *ptr_PacketBuffer); 
     * \brief Reads a short value from the packetBuffer
     * \details The pointer to the packet buffer defines the loworderbyte ofthe short
     * @param [in] unsigned char *ptr_PacketBuffer 
     * \return unsigned short: the value read from the buffer 
     * \private
     */
    unsigned short readShort(unsigned char *ptr_PacketBuffer);

    /*! \fn bool readBit(unsigned p_Value, unsigned p_BitPosition); 
     * \brief Reads the bit from the p_Value field in position defined by p_BitPosition 
     * \details 
     * @param [in] unsigned p_Value The field from where the bit is to
     * be read 
     * @param [in] unsigned p_BitPosition The bit position in the
     * p_Value field 
     * \return bool: true - bit is set | false - bit is cleared
     * \private
     */
    bool readBit(unsigned p_Value, unsigned p_BitPosition);

    /*! \fn unsigned readSubField(unsigned p_Value, unsigned p_LSB, unsigned p_MSB); 
     * \brief Reads a sub field from the p_Value field starting fromthe po
     * \details 
     * @param [in] unsigned p_Value The field including the sub field
     * @param [in] unsigned p_LSB The LSB bit of the sub field
     * @param [in] unsigned p_MSB The MSB bit of the sub field
     * \return unsigned: The value of the sub field
     * \private
     */
    unsigned readSubField(unsigned p_Value, unsigned p_MSB, unsigned p_LSB);
    
    /*! \fn unsigned buildSubFieldMask(unsigned p_MSB, unsigned p_LSB); 
     * \brief Build the mask used to insert a sub field to a field
     * \details 
     * @param [in] unsigned p_MSB The high order bit of the subfield
     * @param [in] unsigned p_LSB The low order bit of the subfield
     * \return unsigned: The subfield mask
     * \private
     */
    unsigned buildSubFieldMask(unsigned p_MSB, unsigned p_LSB);
    

    /*! \fn void calculateCheckSum(unsigned_char p_PacketBuffer); 
     * \brief The common portion of the checksum calculation
     * \details Divides the IP header into 16 bit fields, sums up
     * those fields to a 32 bit field, and finally sums the 16 bit high order
     * portion of the 32 bit field to the remaining low order portion and
     * returns the sum
     * @param [in] unsigned_char p_PacketBuffer  
     * \return unsigned short: the first part of the header sum
     * calculation: adding header fields in 16bit blocks and the sum
     * of the carries and the loworder portion.
     * \private
     */
    unsigned short calculateCheckSum(unsigned char *ptr_PacketBuffer);

    /*! \fn void addCheckSum(unsigned char *ptr_PacketBuffer); 
     * \brief Calculates and adds the check sum to the given IP packet
     * \details 
     * @param [in/out] unsigned p_PacketBuffer Pointer to the IP packet 
     * \private
     */
    void  addCheckSum(unsigned char *ptr_PacketBuffer);

    /*! \fn bool confirmCheckSum(void); 
     * \brief Confirms that the checksum of the IP packet in m_Frame is
     * valid
     * \details 
     * \return bool: true - checksum is valid | false - check sum is not valid 
     * \private
     */
    bool confirmCheckSum(void);

    /*! \fn bool incremetalCheckSumUpdate(void); 
     * \brief Recalculates the checksum value using the incremental method
     * \details 
     * \return bool true: packet is ready to be forwarded - false: packet was dropped
     * \private
     */
    bool incrementalCheckSumUpdate(void);

    /*! \fn void resetPacketBuffer(void) 
     * \brief Resets all the fields in the m_PacketBuffer array
     * \details 
     * \private
     */
    void resetPacketBuffer(void);

    /*! \fn unsigned short readFragmentOffSet(void); 
     * \brief Reads the fragment offset field from IP header
     * \details 
     * \return unsigned_short: the fragment offset value 
     * \private
     */
    unsigned short readFragmentOffSet(void);
    
    
    
    
    
};


#endif /*_PACKETPROCESSOR_H_ */

