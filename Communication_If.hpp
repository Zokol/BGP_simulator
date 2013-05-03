/*! \file  Communication_If.hpp
 *  \brief   
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      03.05.2013
 */

/*!
 * \class Communication_If
 * \brief
 *  \details
 */


#include "systemc"
#include "Packet.hpp"


using namespace std;
using namespace sc_core;
using namespace sc_dt;



#ifndef COMMUNICATION_IF_H
#define COMMUNICATION_IF_H


class Communication_If
{

public:

    /*! \fn virtual bool sendMessage(string p_DestinationIP, string p_SourceIP, string p_Payload) = 0
     * \brief Builds an IP packet using the passed fields as data
     * \details
     * @param [in] string p_DestinationIP
     * @param [in] string p_SourceIP
     * @param [in] string p_Payload
     * \return bool true: message send succeeded - false: error
     * \public
     */
    virtual bool sendMessage(string p_DestinationIP, string p_SourceIP, string p_Payload) = 0;
    
    /*! \fn virtual string reaMessageBuffer(void) = 0
     *  \brief Return the content of the message buffer string
     *  \return Message buffer string
     * \public
     */
    virtual string reaMessageBuffer(void) = 0;

    /*! \fn virtual void clearMessageBuffer(void) = 0
     *  \brief Clears the message buffer string
     * \public
     */
    virtual void clearMessageBuffer(void) = 0;


};


#endif
