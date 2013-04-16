/*! \file  Interface.hpp
 *  \brief     Header file of link module
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      24.1.2013
 */

/*!
 * \class Interface
 * \brief Interface module inside a Router module
 *  \details   
 */


#include "systemc"
#include "Packet.hpp"
#include "Interface_If.hpp"
#include "StringTools.hpp"
#include "Configuration.hpp"

using namespace std;
using namespace sc_core;
using namespace sc_dt;



#ifndef INTERFACE_H
#define INTERFACE_H


#define DOWN false

#define UP true



class Interface: public sc_module, public Interface_If
{

public:


    /*! \brief Clock signal
     * \details 
     * \public
     */
    sc_in_clk port_Clk;

    sc_port<Interface_If,1, SC_ZERO_OR_MORE_BOUND> port_Output;

    sc_export<sc_fifo_out_if<Packet> > export_FromDataPlane;

    sc_export<sc_fifo_in_if<Packet> > export_ToDataPlane;

    /*! \brief Network interface constructor
     * \details 
     * \public
     */
    Interface(sc_module_name p_ModuleName, Connection *p_IfConfig);

    ~Interface();

    /*!
     * \sa Interface_If
     */  
    virtual bool forward(Packet p_Packet);

    /*! \fn void interfaceMain(void)
     *  \brief The SystemC process of the Interface module
     * \public
     */
    void interfaceMain(void);

    /*!
     * \sa Interface_If
     */  
    virtual void interfaceDown(void);

    /*!
     * \sa Interface_If
     */  
    virtual bool interfaceUp(void);

    /*!
     * \sa Interface_If
     */  
    virtual bool isUp(void);

    /*!
     * \sa Interface_If
     */  
    virtual void killInterface(void);

    /*!
     * \sa Interface_If
     */  
    virtual void resetInterface(void);

    /*! \brief Indicate the systemC producer that this module has a process.
     * \sa http://www.iro.umontreal.ca/~lablasso/docs/SystemC2.0.1/html/classproducer.html
     * \public
     */
    SC_HAS_PROCESS(Interface);




private:
  


    /*! \property sc_fifo<Packet> m_ReceivingBuffer
     *  \brief Receiving buffer
     * \details 
     * \private
     */
    sc_fifo<Packet> m_ReceivingBuffer;

    /*! \property sc_fifo<Packet> m_ForwardingBuffer
     * \brief Forwardig buffer
     * \details 
     * \private
     */
    sc_fifo<Packet> m_ForwardingBuffer;

    /*! \property bool m_InterfaceState 
     *  \brief The current state of the interface
     *  \details Possible values UP or DOWN
     *  \private
     */
    bool m_InterfaceState;

    /*! \property StringTools m_Report
     *  \brief Internal reporting tool of the simulation
     *  \details
     *  \private
     */
    StringTools m_Report;

    /*! \property Connection *m_IfConfig
     *  \brief Holds the connection parameters of this interface
     *  \details
     *  \private
     */
    Connection *m_IfConfig;

    /*! \fn void emptyBuffers(void)
     *  \brief empties both the receiving and forwarding buffers
     * \public
     */
    void emptyBuffers(void);


};


#endif
