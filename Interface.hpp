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
  Interface(sc_module_name p_ModuleName);

  ~Interface();
  
  /*! \brief send a packet from the forwarding buffer
   * \public
   */
  virtual bool forward(Packet p_Packet);

  void interfaceMain(void);

  virtual void interfaceDown(void);

  virtual void interfaceUp(void);

  virtual bool isUp(void);

  /*! \brief Indicate the systemC producer that this module has a process.
   * \sa http://www.iro.umontreal.ca/~lablasso/docs/SystemC2.0.1/html/classproducer.html
   * \public
   */
  SC_HAS_PROCESS(Interface);




private:
  


    /*! \brief Receiving buffer
     * \details 
     * \private
     */
    sc_fifo<Packet> m_ReceivingBuffer;

    /*! \brief Forwardig buffer
     * \details 
     * \private
     */
    sc_fifo<Packet> m_ForwardingBuffer;

    bool m_InterfaceState;

    StringTools m_Report;
};


#endif
