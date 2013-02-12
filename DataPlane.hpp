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


using namespace std;
using namespace sc_core;
using namespace sc_dt;

#ifndef _DATAPLANE_H_
#define _DATAPLANE_H_




class DataPlane: public sc_module
{

public:



    /*! \brief Export the BGP forwarding buffer's interface for
     * Control Plane
     * \details
     * \public
     */
sc_export<sc_fifo_in_if<BGPMessage> > export_ToDataPlane;

    /*! \brief Output port for BGP messages
     * \details Data Plane writes all the received BGP messages into
     * this port. The port shall be bind to the Control Plane's
     * receiving FIFO
     * \public
     */
    sc_port<sc_fifo_in_if<BGPMessage>,1, SC_ZERO_OR_MORE_BOUND> port_ToControlPlane;

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
    DataPlane(sc_module_name p_ModuleName, int p_InterfaceCount);

    ~DataPlane();

    void main(void);

    /*! \brief Indicate the systemC producer that this module has a process.
     * \sa http://www.iro.umontreal.ca/~lablasso/docs/SystemC2.0.1/html/classproducer.html
     * \public
     */
    SC_HAS_PROCESS(DataPlane);

private:

    sc_fifo<BGPMessage> m_BGPForwardingBuffer;

    int m_InterfaceCount;
  
    Packet m_Packet;

};


#endif /* _DATAPLANE_H_ */

