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

using namespace std;




#ifndef _PACKETPROCESSOR_H_
#define _PACKETPROCESSOR_H_

#define VERSION 4

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

    /*! \fn void buildIPPacket(void) 
     * \brief Builds an IP packet using the member fields as data
     * \details 
     * \private
     */
    void buildIPPacket(void);

    /*! \fn void setBits(unsigned char *ptr_Target, unsigned char
     * p_Value, int p_Shift); 
     * \brief Set the value into the target. p_Shift defines the LSBbit position
     * \details 
     * @param [out] unsigned char *ptr_Target Pointer to the target value to be modified
     * @param [in] unsigned char p_Value  Value to be added into p_Target
     * @param [in] int p_Shift The LSB position from where the
     * value will be added
     * \p
     */
    void setBits(unsigned char *ptr_Target, unsigned char p_Value, int p_Shift);
    
    
    
};


#endif /*_PACKETPROCESSOR_H_ */

