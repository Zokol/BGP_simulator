/*! \file  Interface_If.hpp
 *  \brief   
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      24.1.2013
 */

/*!
 * \class Interface_If
 * \brief
 *  \details
 */


#include "systemc"
#include "Packet.hpp"


using namespace std;
using namespace sc_core;
using namespace sc_dt;



#ifndef INTERFACE_IF_H
#define INTERFACE_IF_H


class Interface_If: virtual public sc_interface
{

public:

    /*! \fn virtual bool forward(Packet p_Packet) = 0
     *  \brief Writes the p_Packet to the receiving buffer of NIC that
     *  implements this IF
     *  @param[in] Packet p_Packet The network frame carring BGP msg
     *  or IP packet
     * \return bool true: success false: NIC down or the buffer is full
     * \public
     */
    virtual bool forward(Packet p_Packet) = 0;
    
    /*! \fn virtual void interfaceDown(void) = 0
     *  \brief Sets the NIC down
     * \public
     */
    virtual void interfaceDown(void) = 0;

    /*! \fn virtual void interfaceUp(void) = 0
     *  \brief Sets the NIC up
     * \public
     */
    virtual bool interfaceUp(void) = 0;
    
    /*! \fn virtual bool isUp(void) = 0
     *  \brief Checks if the NIC is up or down
     *  \return bool true: NIC is up - false: NIC is down
     * \public
     */
    virtual bool isUp(void) = 0;

    /*! \fn virtual void killInterface(void) = 0
     *  \brief Sets the interface down and empties the receiving and forwarding buffers
     * \public
     */
    virtual void killInterface(void) = 0;

    /*! \fn virtual void resetInterface(void) = 0
     *  \brief Kills the interface and reconnects it
     * \public
     */
    virtual void resetInterface(void) = 0;


};


#endif
