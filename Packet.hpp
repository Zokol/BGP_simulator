/*! \file Packet.hpp
 *  \brief     Header file of Packet class
 *  \details   defines the Packet class.
 *  \author    Antti Siirilä, 501449
 *  \version   2.0
 *  \date      11.2.2013
 */

/*! \class Packet
 *  \brief     
 *  \details   
 */

#include <systemc>
#include "BGPMessage.hpp"


using std::cout;
using std::endl;
using std::ostream;
using std::string;
using sc_core::sc_trace_file;
using sc_core::sc_trace;
using sc_dt::sc_bv;


#ifndef PACKET_H
#define PACKET_H

#define MTU 192

class Packet
{
 
public: 

  
    /*! \brief Default constructor.
     * \details Initiates the packet fields to zero 
     * \public
     */
    Packet(void); 

    /*! \brief Destructor.
     * \details Frees the member memory allocations
     * \public
     */
    ~Packet(void); 


    /*!
     * \brief Constructor with member data.
     * \details Initiates the packet data and all the id fields to given values.
     * @param[in] const Packet& Reference to the object to be copied
     * into this
     * \public
     */
    Packet(const Packet& p_Packet);

    /*!
     * \brief Constructor with member data.
     * \details Initiates the packet data and all the id fields to given values.
     * @param[in] BGPMessage& p_BGPPayload Reference to the BGP
     * payload object
     * @param[in] int p_ProtocolType The upper layer protocol type carried in the payload
     * \public
     */
    Packet(BGPMessage& p_BGPPayload, int p_ProtocolType);

    /*!
     * \brief Constructor with member data.
     * \details Initiates the packet data and all the id fields to given values.
     * @param[in] sc_bv<MTU> p_IPPayload The IP packet as bit string
     * @param[in] int p_ProtocolType The upper layer protocol type carried in the payload
     * \public
     */
    Packet(sc_bv<MTU> p_IPPayload, int p_ProtocolType);


    /*!
     * \brief Set BGP message as payload
     * @param[in] BGPMessage& p_BGPPayload Reference to the BGP
     * payload object 
     * \public
     */
    void setBGPPayload(BGPMessage& p_BGPPayload);


    /*!
     * \brief Set IP packet as payload
     * @param[in] sc_bv<MTU> p_IPPayload IP packet as bit string
     * \public
     */
    void setIPPayload(sc_bv<MTU> p_IPPayload);


    /*!
     * \brief Set the upper layer protocol type
     * @param[in] int p_ProtocolType Value to indicate the upper layer
     * protocol type
     * \public
     */
    bool setProtocolType(int p_ProtocolType);

    /*!
     * \brief Get IP packet
     * \return \b <sc_bv<int>> The IP packet as bit string
     * \public
     */
    sc_bv<MTU> getIPPayload(void);


    /*!
     * \brief Get BGP Message
     * \return \b BGPMessage& Reference to BGP message object
     * \public
     */
    BGPMessage& getBGPPayload(void);


    /*!
     * \brief Get upper layer protocol type
     * \return \b int Indicates the upper layer protocol
     * \public
     */
    int getProtocolType(void);

    /*!
     * \brief Overload of compare operator
     * \details Compare the data fields of this Packet-object to the onces in the given Packet-object.
     * @param[in] Packet& p_Packet Reference to a Packet-object to be compared with this object
     * \return \b <bool> true if the member fields of both objects match otherwise false.
     * \public
     */
    bool operator == (const Packet& p_Packet) const;

    /*!
     * \brief Overload of assign operator
     * \details Copy the data fields of given Packet-object to the this. Return a reference to this.
     * @param[in] Packet p_Packet Reference to a Packet-object, which member values is to be assigned to the member fields of this object accordingly
     * \return \b <Packet> Reference to this object
     * \public
     */
    Packet& operator = (const Packet& p_Packet);

    /*! \relates sc_signal
     * \brief Overload stream operator
     * \details Write data members into given output stream.
     * @param[in] os Reference to outputstream in which the member field values is to be written.
     * @param[in] p_Packet Reference to Packet-object, which member fields is to be written into outputstream.
     * \return \b <ostream> Reference to the outputstream containing the written values
     */

    inline friend ostream& operator << (ostream& os,  Packet const & p_Packet )
    {   

        os  << "BGP_Payload: " << p_Packet.m_BGPPayload << ", IP_Payload: " << p_Packet.m_IPPayload << ", Protocol type: " << p_Packet.m_ProtocolType;
        return os;
    }

    /*! \relates sc_trace_file
     * \brief Set trace file for this packet
     * \details All the member fields shall be traced. Allow systemC library to access the private members of this class by declaring the function as friend
     * @param[out] p_TraceFilePointer Pointer to sc_trace_file-object
     * @param[in] p_Packet Reference to Packet-object to be traced
     * @param[in] p_TraceObjectName Name of the Packet-object
     * 
     */
    inline friend void sc_trace(sc_trace_file *p_TraceFilePointer, const Packet& p_Packet, const string & p_TraceObjectName )
    {
  
        sc_trace(p_TraceFilePointer, p_Packet.m_ProtocolType, p_TraceObjectName + ".Protocol_Type");
        sc_trace(p_TraceFilePointer, p_Packet.m_IPPayload, p_TraceObjectName + ".IP_Payload");
        sc_trace(p_TraceFilePointer, p_Packet.m_BGPPayload, p_TraceObjectName + ".BGP_Payload");
    }
  

private:



    /*! \brief Holds the BGP message object
     * \details 
     * \private
     */
    BGPMessage m_BGPPayload;

    /*! \brief Holds the IP packet as bit string
     * \details The MTU defines the length of the packet fragment
     * \private
     */
    sc_bv<MTU> m_IPPayload;

    /*! \brief Holds the protocol type of the packet 
     * \details 
     * \private
     */
    int m_ProtocolType;

};

#endif
