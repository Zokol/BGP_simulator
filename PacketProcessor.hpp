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
 *IP Protocol version
 */
#define VERSION 4
/*!
 *Version length in bits
 */
#define VERSION_LENGTH 4
/*!
 * The first bit of version field starting from LSB bit of the field
 */
#define VERSION_POS 4
/*!
 *Version field index
 */
#define VERSION_INDEX 0
/*!
 *Standard header lenght
 */
#define IHL 5
/*!
 *Minimum packet length
 */
#define MIN_LENGTH 20
/*!
 *Time to leave TTL initial value
 */
#define TTL 15
/*!
 *Protocol type in IP datagram
 */
#define PROTOCOL 17







/*! \class PacketProcessor
 * \brief Performs all the tasks related to IP packet processing
 * \details 
 * \public
 */
class PacketProcessor
{
public:

    PacketProcessor();

    virtual ~PacketProcessor();

    /*! \fn void sendMessage(string p_DestinationIP, string
     * p_SourceIP, string p_Payload) 
     * \brief Sends an IP message to the given destination, from the
     * given source
     * @param [in] string p_DestinationIP
     * @param [in] string p_SourceIP
     * @param [in] string p_Payload
     * \details 
     * \public
     */
    void sendMessage(string p_DestinationIP, string p_SourceIP, string p_Payload);
    
    /*! \fn string readMessage(void); 
     * \brief Outputs the contents of an IP packet
     * \details The header and payload is written in the return string
     * \return string: the complete IP packet as a string 
     * \public
     */
    string readMessage(void);
    
    /*! \fn bool processPacket(Packet& p_Packet); 
     * \brief Validates an IP packet 
     * \details Check whether the passed packet is a valid IP packet. 
     * @param [in] Packet p_Packet IP packet to be processed
     * \return bool: true == Packet is valid || false == Packet is not valid
     * \public
     */
    bool processPacket(Packet& p_Packet);

    /*! \fn string getDestination(void); 
     * \brief Returns the destination of the processed packet
     * \details 
     * \return string: the destination IP address as string 
     * \public
     */
    string getDestination(void);

    /*! \fn Packet& forward(void); 
     * \brief Decrement the TTL, re-calculates the checksum, and returns the packet
     * \details 
     * \return Packet&: Reference to this packet object 
     * \public
     */
    Packet& forward(void);
    
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

    /*! \property Packet m_Packet 
     * \brief The packet under processing
     * \details 
     * \private
     */
    Packet m_Packet;

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
    

    /*! \fn void buildIPPacket(void) 
     * \brief Builds an IP packet using the member fields as data
     * \details 
     * \private
     */
    void buildIPPacket(void);

    /*! \fn void setSubField(unsigned char *ptr_Target, unsigned char
     * p_Value, int p_Shift); 
     * \brief Set the value into the target. p_Shift defines the LSBbit position
     * \details 
     * @param [out] unsigned char *ptr_Target Pointer to the target value to be modified
     * @param [in] unsigned char p_Value  Value to be added into p_Target
     * @param [in] int p_Shift The LSB position from where the
     * value will be added
     * \private
     */
    void setSubField(unsigned char *ptr_Target, unsigned char p_Value, int p_Shift);

    /*! \fn void setBit(unsigned char p_Target, int p_Position); 
     * \brief Set the bit in position p_Position of the field pointed by ptr_Target.
     * \details 
     * @param [out] unsigned char *ptr_Target  
     * @param [in] int p_Position  
     * \private
     */
    void setBit(unsigned char *ptr_Target, int p_Position);
    
    /*! \fn void clearBit(unsigned char *p_Target, int p_Position); 
     * \brief Clears the bit in position p_Position of the field pointed by ptr_Target
     * \details 
     * @param [out] unsigned char *ptr_Target  
     * @param [in] int p_Position  
     * \private
     */
    void clearBit(unsigned char *ptr_Target, int p_Position);

    /*! \fn void setMultipleFields(unsigned char *ptr_Target, unsigned p_Value, unsigned p_NumberOfFields); 
     * \brief Sets the value in the target buffer
     * \details 
     * @param [out] unsigned char *ptr_Target 
     * @param [in] unsigned p_Value  
     * @param [in] unsigned p_NumberOfFields 
     * \private
     */
    void setMultipleFields(unsigned char *ptr_Target, unsigned p_Value, unsigned p_NumberOfFields);
    
    

    
    
    
};


#endif /*_PACKETPROCESSOR_H_ */

