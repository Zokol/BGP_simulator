/*! \file  RoutingTable_If.hpp
 *  \brief
 *  \details
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 13:01:27 2013
 */

/*!
 * \class RoutingTable_If
 * \brief
 *  \details
 */


#include "systemc"


using namespace std;
using namespace sc_core;
using namespace sc_dt;


#ifndef _ROUTINGTABLE_IF_H_
#define _ROUTINGTABLE_IF_H_


class RoutingTable_If: virtual public sc_interface
{

public:


    //TODO
    /*! \brief Set new route to the Routing Table
     * \details
     * \public
     */
    virtual int resolveRoute(string p_IPAddress) = 0;





};




#endif /* _ROUTINGTABLE_IF_H_ */

