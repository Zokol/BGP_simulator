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


  /*! \brief Sets the BGP hold down time for this session
   * \details
   * @param[in] int p_HoldDown  The hold down time in seconds
   * \return bool True: if p_HoldDown is valid, False: if p_HoldDown
   * is not valid
   * \public
   */
  virtual bool setHoldDown(int p_HoldDown) = 0;

  /*! \brief Sets the BGP keepalive interval for this session
   * \details This value defines the keepalive interval that this BGP
   * session uses for sending keepalive messages. The keepalive
   * interval is a fraction of Hold Down time that the p_KeepaliveFraction
   * defines. I.e. If Hold Down is set to 180s and the Keepalive
   * fraction is 3 then the keepalive interval would be 60s.
   * @param[in] int p_KeepaliveFraction The fraction of Hold Down time
   * that determines the keepalive interval
   * \return bool True: if p_KeepaliveFraction is valid, False: if p_KeepaliveFraction
   * is not valid
   * \public
   */
  virtual bool setKeepaliveFraction(int p_KeepaliveFraction) = 0;


  /*! \brief Reset the hold down timer for this session
   * \details 
   * \public
   */
  virtual void resetHoldDown(void) = 0;




};


#endif /* _BGPSESSION_IF_H_ */



