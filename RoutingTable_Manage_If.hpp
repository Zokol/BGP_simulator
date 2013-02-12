/*! \file  RoutingTable_Manage_If.hpp
 *  \brief   
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 13:01:27 2013
 */

/*!
 * \class RoutingTable_Manage_If
 * \brief
 *  \details
 */


#include "systemc"


using namespace std;
using namespace sc_core;
using namespace sc_dt;


#ifndef _ROUTINGTABLE_MANAGE_IF_H_
#define _ROUTINGTABLE_MANAGE_IF_H_


class RoutingTable_Manage_If: virtual public sc_interface
{

public:
    //TODO Define the methods


  /*! \brief Set new route to the Routing Table
   * \details 
   * \public
   */
  virtual bool setRoute(void) = 0;
};




#endif /* _ROUTINGTABLE_MANAGE_IF_H_ */

