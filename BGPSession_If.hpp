/*! \file  BGPSession_If.hpp
 *  \brief   
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 13:01:27 2013
 */

/*!
 * \class BGPSession_If
 * \brief
 *  \details
 */


#include "systemc"


using namespace std;
using namespace sc_core;
using namespace sc_dt;

#ifndef _BGPSESSION_IF_H_
#define _BGPSESSION_IF_H_




class BGPSession_If: virtual public sc_interface
{

public:
    //TODO Define the methods


    /*! \fn bool isSessionValid(void)
     *  \brief Checks whether this session is valid or not
     * \details Allows the control plane to check whether this session
     * is still valid or not. I.e. is the HoldDown timer expired.
     * \public
     */
    virtual bool isSessionValid(void) = 0;

    /*! \fn void sessionStop(void)
     *  \brief Stops this session
     * \details HoldDown and Keepalive timers are stopped and no
     * keepalive messages are sent after a call of this function
     * \public
     */
    virtual void sessionStop(void) = 0;

    /*! \fn int getPeeringInterface(void); 
     * \brief Returns the interface index of the session peer
     * \details 
     * \return int:  
     * \public
     */
    virtual int getPeeringInterface(void) = 0;

    /*! \fn int getPeerAS(void); 
     * \brief Returns the AS number of the session peer
     * \details 
     * \return string: 
     * \public
     */
    virtual string getPeerAS(void) = 0;    

};


#endif /* _BGPSESSION_IF_H_ */



