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

#define MTU 576
#define TYPE_IP 0
#define TYPE_BGP 1

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
     * \brief Set BGP message as payload
     * @param[in] BGPMessage& p_BGPPayload Reference to the BGP
     * payload object 
     * \public
     */
    void setBGPPayload(BGPMessage& p_BGPPayload);


    /*!
     * \brief Set the upper layer protocol type
     * @param[in] int p_ProtocolType Value to indicate the upper layer
     * protocol type
     * \public
     */
    bool setProtocolType(int p_ProtocolType);

    /*! \fn void setPDU(const unsigned char *p_PDU); 
     * \brief Sets the PDU
     * \details 
     * @param [in] const unsigned char *p_PDU  
     * \public
     */
    void setPDU(const unsigned char *p_PDU);
    

    /*! \fn void getPDU(unsigned char *p_PDU); 
     * \brief Copy PDU to the array pointed by p_PDU
     * \details 
     * \return unsigned char *p_PDU
     * \public
     */
    void getPDU(unsigned char *p_PDU);


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

        os  << "**********************" << endl << "BGP_Payload: " << p_Packet.m_BGPPayload << ", Protocol type: " << p_Packet.m_ProtocolType<< endl << "**********************" << endl << "PDU" << endl <<"----------------------";

        for (int i = 0; i < MTU; i++)
            {
                if(i%4 == 0)
                    os << endl;
                os << p_Packet.m_PDU[i] << ";";
            }

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
        sc_trace(p_TraceFilePointer, p_Packet.m_BGPPayload, p_TraceObjectName + ".BGP_Payload");

    }
  

private:



    /*! \brief Holds the BGP message object
     * \details 
     * \private
     */
    BGPMessage m_BGPPayload;
    
    /*! \property unsigned char m_PDU[MTU] 
     * \brief The PDU that this packet carries 
     * \details IP packet is stored in this one
     * \private
     */
    unsigned char m_PDU[MTU];

    /*! \brief Holds the protocol type of the packet 
     * \details 
     * \private
     */
    int m_ProtocolType;

    /*! \fn void initPDU(void) 
     * \brief Sets all the fields in m_PDU to zero
     * \details 
     * \private
     */
    void initPDU(void);

};

#endif
