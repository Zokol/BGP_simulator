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

    /*! \brief forward a packet to the connected router from the forwarding buffer
     * \public
     */
    virtual bool forward(Packet p_Packet) = 0;
    
    virtual void interfaceDown(void) = 0;
    
    virtual void interfaceUp(void) = 0;
    
    virtual bool isUp(void) = 0;

};


#endif
