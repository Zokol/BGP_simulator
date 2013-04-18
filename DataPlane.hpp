/*! \file DataPlane.hpp
 *  \brief     
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 12:14:20 2013
 */



/*!
 * \class DataPlane
 * \brief Protocol Engine module
 *  \details
 */



#include "systemc"
#include "Packet.hpp"
#include "BGPMessage.hpp"
#include "Output_If.hpp"
#include "RoutingTable_If.hpp"
#include "Configuration.hpp"
#include "StringTools.hpp"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

#ifndef _DATAPLANE_H_
#define _DATAPLANE_H_




class DataPlane: public sc_module, public Output_If
{

public:



    /*! \brief Export the BGP forwarding buffer's interface for
     * Control Plane
     * \details
     * \public
     */
    sc_port<RoutingTable_If > port_ToRoutingTable;

    /*! \brief Output port for BGP messages
     * \details Data Plane writes all the received BGP messages into
     * this port. The port shall be bind to the Control Plane's
     * receiving FIFO
     * \public
     */
    sc_port<sc_fifo_out_if<BGPMessage>,1, SC_ZERO_OR_MORE_BOUND> port_ToControlPlane;

    /*! \brief Neighbor writes to the receiving buffer
     * \details
     * \public
     */
    sc_port<sc_fifo_in_if<Packet>,0, SC_ZERO_OR_MORE_BOUND> port_FromInterface;

    /*! \brief Forward to the neighbor
     * \details
     * \public
     */
    sc_port<sc_fifo_out_if<Packet>,0, SC_ZERO_OR_MORE_BOUND> port_ToInterface;

    /*! \brief Clock signal
     * \details 
     * \public
     */
    sc_in_clk port_Clk;

    /*!
     * \brief Constructor
     * \details Builds the router
     * @param[in] p_Name The name of the module
     * \public
     */
    DataPlane(sc_module_name p_ModuleName, ControlPlaneConfig * const p_Config );

    ~DataPlane();

    void main(void);


    virtual bool write(BGPMessage p_BGPMsg);

    /*! \brief Indicate the systemC producer that this module has a process.
     * \sa http://www.iro.umontreal.ca/~lablasso/docs/SystemC2.0.1/html/classproducer.html
     * \public
     */
    SC_HAS_PROCESS(DataPlane);

private:

    sc_mutex m_BGPForwardingBufferMutex;

    sc_fifo<BGPMessage> m_BGPForwardingBuffer;

    int m_InterfaceCount;
  
    Packet m_Packet;

    BGPMessage m_BGPMsg;
	
	ControlPlaneConfig *m_Config;

    StringTools m_Rpt;

};


#endif /* _DATAPLANE_H_ */

