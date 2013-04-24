/*! \file  ControlPlane.hpp
 *  \brief     Header file of ControlPlane module
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      12.2.2013
 */

/*!
 * \class ControlPlane
 * \brief ControlPlane module runs the BGP process
 *  \details   
 */


#include "systemc"
#include "BGPMessage.hpp"
#include "Configuration.hpp"
#include "BGPSession.hpp"
#include "BGPSession_If.hpp"
#include "RoutingTable_If.hpp"
#include "StringTools.hpp"
#include "Output_If.hpp"

using namespace std;
using namespace sc_core;
using namespace sc_dt;



#ifndef CONTROLPLANE_H
#define CONTROLPLANE_H




class ControlPlane: public sc_module, public Output_If
{

public:

    
    /*! \brief System clock signal
     * \details The router's internal clock
     * \public
     */
    sc_in_clk port_Clk;
   
    /*! \brief Forwarding port
     * \details Used to write BGP messages to the data plane
     * \public
     */
    sc_port<Output_If,0, SC_ZERO_OR_MORE_BOUND> port_ToDataPlane;
   
    /*! \brief Routing Table's management port
     * \details Used to manage the routing table. Add, remove, update routes
     * \public
     */
    sc_port<sc_fifo_out_if<BGPMessage> ,1, SC_ZERO_OR_MORE_BOUND> port_ToRoutingTable;
   
    /*! \brief Input interface
     * \details Allows data plane to write received BGP messages into
     *  m_ReceivingBuffer-fifo
     * \public
     */
    sc_export<sc_fifo_out_if<BGPMessage> > export_ToControlPlane;

    sc_export<BGPSession_If> **export_Session;
   
   
    /*! \brief This provides the DataPlane_In_If for BGP sessions
     * \details 
     * \public
     */
    sc_export<Output_If > export_ToDataPlane;

    void before_end_of_elaboration()
    {

        //inititate the session events
        for (int i = 0; i < m_BGPConfig->getNumberOfInterfaces()-1; i++)
            {
                //connect the session to the data plane
                m_BGPSessions[i]->port_ToDataPlane.bind(export_ToDataPlane);
            }


    }

    /*! \brief Elaborates the ControlPlane module
     * \details 
     * \public
     */
    ControlPlane(sc_module_name p_ModName, ControlPlaneConfig * const p_BGPConfig);

    /*! \brief Destructor of the ControlPlane module
     * \details Free's all the dynamically allocated memory 
     * \public
     */
    ~ControlPlane();

    /*! \brief The main process of Control Plane module
     * \details \li Reads BGP messages from the m_ReceivingBuffer. \li
     * performs the route resolution process accoriding to BGP protocol.
     * \li Generates the required update messages. \li Keeps track on
     * different BGP sessions.
     * \public
     */
    void controlPlaneMain(void);

    /*! \sa Output_If
     * \public
     */
    virtual bool write(BGPMessage p_BGPMsg);

    /*! \brief Indicate the systemC producer that this module has a process.
     * \sa http://www.iro.umontreal.ca/~lablasso/docs/SystemC2.0.1/html/classproducer.html
     * \public
     */
    SC_HAS_PROCESS(ControlPlane);




private:
  

    /*! \property sc_mutex mutex_Write 
     * \brief Handles the arbitration inside the write method
     * \private
     */
    sc_mutex mutex_Write;

    /*! \brief Receiving buffer
     * \details Data plain writes all the received BGP messages into
     * this fifo
     * \private
     */
    sc_fifo<BGPMessage> m_ReceivingBuffer;

    // /*! \brief Number of BGP sessions
    //  * \details This defines how many BGP sessions there are in this router
    //  * \private
    //  */
    //   int  m_SessionCount;
    
    /*! \brief The BGP session modules
     * \details 
     * \private
     */
    BGPSession **m_BGPSessions;
    
    /*! \brief BGP message
     * \details 
     * \private
     */
    BGPMessage m_BGPMsg;

    ControlPlaneConfig *m_BGPConfig;


    /*! \property StringTools m_Name  
     * \brief Dynamic module naming
     * \details 
     * \private
     */
    StringTools m_Name;


};


#endif
