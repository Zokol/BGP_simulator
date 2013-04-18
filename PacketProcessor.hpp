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


#ifndef _PACKETPROCESSOR_H_
#define _PACKETPROCESSOR_H_

class PacketProcessor
{
public:

    PacketProcessor();

    virtual ~PacketProcessor();

    /*! \fn void sendMessage(string p_DestinationIP, string
     * p_SourceIP, string p_Payload) 
     * \brief Sends an IP message to the given destination, from the
     * given source
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
     * @param [in|out] Packet p_Packet IP packet to be processed
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
    
    

    



};




