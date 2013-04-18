/*! \file  Output_If.hpp
 *  \brief   
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 13:01:27 2013
 */

/*!
 * \class Output_If
 * \brief
 *  \details
 */


#include "systemc"
#include "BGPMessage.hpp"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

#ifndef _OUTPUT_IF_H_
#define  _OUTPUT_IF_H_




class Output_If: virtual public sc_interface
{

public:



  /*! \brief Allows the session to pass BGP messages to the DataPlane
   * \details The method shall implement a mutex that takes care of
   * the arbitraton between sessions
   * @param[in] BGPMessage p_BGPMsg  The BGP message to be send
   * \return bool True: if success is valid, False: if not success
   * \public
   */
    virtual bool write(BGPMessage p_BGPMsg) = 0;




};


#endif /* _OUTPUT_IF_H_ */



