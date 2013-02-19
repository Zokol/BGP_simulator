/*! \file  RoutingTable.hpp
 *  \brief     Header file of RoutingTable module
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      12.2.2013
 */

/*!
 * \class RoutingTable
 * \brief RoutingTable module
 *  \details Routing Table contains BGP routes and offers methods
 * to manage, and access them
 */


#include "systemc"
#include "RoutingTable_If.hpp"


using namespace std;
using namespace sc_core;
using namespace sc_dt;



#ifndef _ROUTINGTABLE_H_
#define _ROUTINGTABLE_H_

class RoutingTable: public sc_module, RoutingTable_If
{

public:

    
    /*! \brief System clock signal
     * \details The router's internal clock
     * \public
     */
    sc_in_clk port_Clk;
        
    /*! \brief Input interface
     * \details Allows data plane to write received BGP messages into
     *  m_ReceivingBuffer-fifo
     * \public
     */
    sc_export<sc_fifo_in_if<BGPMessage> > export_ToRoutingTable;
   


void before_end_of_elaboration()
        {
        }



  /*! \brief Elaborates the RoutingTable module
   * \details 
   * \public
   */
    RoutingTable(sc_module_name p_ModuleName);



  /*! \brief Destructor of the RoutingTable module
   * \details Free's all the dynamically allocated memory 
   * \public
   */
  ~RoutingTable();
  


  /*! \brief The main process of Control Plane module
   * \details \li Reads BGP messages from the m_ReceivingBuffer. \li
   * performs the route resolution process accoriding to BGP protocol.
   * \li Generates the required update messages. \li Keeps track on
   * different BGP sessions.
   * \public
   */
  void routingTableMain(void);

  /*! \brief Indicate the systemC producer that this module has a process.
   * \sa http://www.iro.umontreal.ca/~lablasso/docs/SystemC2.0.1/html/classproducer.html
   * \public
   */
  SC_HAS_PROCESS(RoutingTable);




private:
  


    /*! \brief Receiving buffer
     * \details Data plain writes all the received BGP messages into
     * this fifo
     * \private
     */
    sc_fifo<BGPMessage> m_ReceivingBuffer;
    
    
  /*! \brief BGP message
   * \details 
   * \private
   */
    BGPMessage m_BGPMsg;


    //TODO Find out what parameters we need for this function.
    /*! \brief Set new route to the Routing Table
     * \details 
     * \public
     */
    virtual bool setRoute(void) = 0;

    //TODO 
    /*! \brief Set new route to the Routing Table
     * \details 
     * \public
     */
    virtual bool updateRoute(void) = 0;



};



#endif /* _ROUTINGTABLE_H_ */
