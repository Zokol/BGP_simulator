/*! \file Packet.hpp
 *  \brief     Header file of Packet class
 *  \details   defines the addressed_pkt class.
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      29.1.2013
 */

/*! \class Packet
 *  \brief     
 *  \details   
 */

#include "systemc"



using std::cout;
using std::endl;
using std::ostream;
using std::string;
using sc_core::sc_trace_file;
using sc_core::sc_trace;

#ifndef PACKET_H
#define PACKET_H

class Packet
{
public: 

  /*! \brief Default constructor.
   * \details Initiates the packet fields to zero 
   * \public
   */
  Packet(); 
  /*! \brief Default constructor.
   * \details Initiates the packet fields to zero 
   * \public
   */
 /* 
 inline Packet():m_DottedSourceIP("0.0.0.0"), m_DottedDestinationIP("0.0.0.0"), m_ProtocolType(0)
  {
    m_SourceIP = 0;
    m_DestinationIP = 0;
    m_Payload = NULL;
  };
*/

  /*!
   * \brief Constructor with member data.
   * \details Initiates the packet data and all the id fields to given values.
   * @param[in] p_Data The data that this packet holds
   * @param[in] p_Id The unique indentification of this packet
   * @param[in] p_SourceId The unique indentification of the packet sender
   * @param[in] p_TargetId The unique indentification of the packet destination
   * \public
   */
  Packet(void *p_Payload, int p_ProtocolType, string p_SourceIP, string p_DestinationIP);

  /*!
   * \brief Constructor with member data.
   * \details Initiates the packet data and all the id fields to given values.
   * @param[in] p_Data The data that this packet holds
   * @param[in] p_Id The unique indentification of this packet
   * @param[in] p_SourceId The unique indentification of the packet sender
   * @param[in] p_TargetId The unique indentification of the packet destination
   * \public
   */
  Packet(void *p_Payload, int p_ProtocolType, int p_SourceIP, int p_DestinationIP);


  /*!
   * \brief Set packet source IP
   * @param[in] string p_SourceId source ID to be assigned for this packet
   * @return TRUE: success FALSE: fail
  * \public
   */
  bool setSourceIP(string p_SourceIP);

  /*!
   * \brief Set packet source IP
   * @param[in] int p_SourceIP source ID to be assigned for this packet
   * @return TRUE: success FALSE: fail
    * \public
   */
  bool setSourceIP(int p_SourceIP);

  /*!
   * \brief Set packet destination IP
   * @param[in] string p_DestinationIP destination IP to be assigned for this packet
   * @return TRUE: success FALSE: fail
   * \public
   */
  bool setDestinationIP(string p_DestinationIP);

  /*!
   * \brief Set packet destination IP
   * @param[in] int p_DestinationIP destination IP to be assigned for this packet
   * @return TRUE: success FALSE: fail
   * \public
   */
  bool setDestinationIP(int p_DestinationIP);

  void setPayload(void *p_Payload);

  bool setProtocolType(int p_ProtocolType);

  /*!
   * \brief  source IP to string
   * \public
   */
  void sourceIPToString(void);

  /*!
   * \brief Get packet source IP
   * \return \b <int> source IP assigned for this packet
   * \public
   */
  int getSourceIP(void);

  /*!
   * \brief destination IP to string
   * \public
   */
  void destinationIPToString(void);

  /*!
   * \brief Get packet  destination IP
   * \return \b <int> destination IP assigned for this packet
   * \public
   */
  int getDestinationIP(void);
  
  void getPayload(void *p_Payload);

  int getProtocolType(void);

  /*!
   * \brief Overload of compare operator
   * \details Compare the data fields of this pkt-object to the onces in the given pkt-object.
   * @param[in] p_Pkt Reference to a pkt-object to be compared with this object
   * \return \b <bool> true if the member fields of both objects match otherwise false.
   * \public
   */
  bool operator == (const Packet& p_Packet) const;

  /*!
   * \brief Overload of assign operator
   * \details Copy the data fields of given pkt-object to the this. Return a reference to this.
   * @param[in] p_Pkt Reference to a pkt-object that member values is to be assigned to the member fields of this object accordingly
   * \return \b <pkt> Reference to this object
   * \public
   */
  Packet& operator = (const Packet& p_Packet);

  /*! \relates sc_signal
   * \brief Overload stream operator
   * \details Write data members into given output stream.
   * @param[in] os Reference to outputstream in which the member field values is to be written.
   * @param[in] p_Pkt Reference to pkt-object, which member fields is to be written into outputstream.
   * \return \b <ostream> Reference to the outputstream containing the written values
   */

  inline friend ostream& operator << (ostream& os,  Packet const & p_Packet )
  {   

    os  << "Payload: " << p_Packet.m_Payload << ", Protocol type: " << p_Packet.m_ProtocolType << ", Source IP:" << p_Packet.m_SourceIP << ", Destination IP: " << p_Packet.m_DestinationIP;
    return os;
  }

  /*! \relates sc_trace_file
   * \brief Set trace file for this packet
   * \details All the member fields shall be traced. Allow systemC library to access the private members of this class by declaring the function as friend
   * @param[out] traceFile Pointer to sc_trace_file-object
   * @param[in] p_Pkt Reference to pkt-object to be traced
   * @param[in] pktName Name of the pkt-object
   * 
   */
  inline friend void sc_trace(sc_trace_file *p_TraceFilePointer, const Packet& p_Packet, const string & p_TraceObjectName )
  {
  
    sc_trace(p_TraceFilePointer, p_Packet.m_Payload, p_TraceObjectName + ".Payload_pointer");
    sc_trace(p_TraceFilePointer, p_Packet.m_ProtocolType, p_TraceObjectName + ".Protocol_Type");
    sc_trace(p_TraceFilePointer, p_Packet.m_SourceIP, p_TraceObjectName + ".Source_IP");
    sc_trace(p_TraceFilePointer, p_Packet.m_DestinationIP, p_TraceObjectName + ".Destination_IP");
    
  }
  

private:

  /*! \brief Holds a pointer to the payload of the packet
   * \details 
   * \private
   */
  void *m_Payload;

  /*! \brief Holds the protocol type of the packet 
   * \details 
   * \private
   */
  int m_ProtocolType;

  /*! \brief Holds the source IP of the packet
   * \details 
   * \private
   */
  int m_SourceIP;

  /*! \brief Holds the destinatin IP of the packet
   * \details 
   * \private
   */
  int m_DestinationIP;

  /*! \brief Holds the source IP of the packet in dotted decimal form
   * \details 
   * \private
   */
  string m_DottedSourceIP;

  /*! \brief Holds the destinatin IP of the packet in dotted decimal form
   * \details 
   * \private
   */
  string m_DottedDestinationIP;

  int convertIPToInt(string p_IP);
  string convertIPToString(int p_IP);
};

#endif
