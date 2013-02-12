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
#include "Packet.hpp"
#include "Interface_If.hpp"



using namespace std;
using namespace sc_core;
using namespace sc_dt;



#ifndef CONTROLPLANE_H
#define CONTROLPLANE_H




class ControlPlane: public sc_module
{

public:


  /*! \brief Clock signal
   * \details 
   * \public
   */
  sc_in_clk port_Clk;

    sc_port<sc_fifo_out_if,1, SC_ZERO_OR_MORE_BOUND> port_Output;
    sc_export<sc_fifo_in_if, 1, SC_ZERO_OR_MORE_BOUND> export_Input;
  




  /*! \brief Elaborates the ControlPlane module
   * \details 
   * \public
   */
  ControlPlane(sc_module_name p_ModuleName);

  ~ControlPlane();
  

  void controlPlaneMain(void);

  /*! \brief Indicate the systemC producer that this module has a process.
   * \sa http://www.iro.umontreal.ca/~lablasso/docs/SystemC2.0.1/html/classproducer.html
   * \public
   */
  SC_HAS_PROCESS(ControlPlane);




private:
  


  /*! \brief Receiving buffer
   * \details Data plain writes all the received BGP messages into
   * this fifo
   * \private
   */
  sc_fifo<BGPMessage> m_ReceivingBuffer;


};


#endif
