/*! \file DataPlane.hpp
 *  \brief     
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      27.1.2013
 */



/*!
 * \class DataPlane
 * \brief Protocol Engine module
 *  \details
 */



#include "systemc"
#include "Packet.hpp"

using namespace std;
using namespace sc_core;
using namespace sc_dt;


#ifndef PROEGN_H
#define PROEGN_H

 

class DataPlane: public sc_module
{

public:

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

  int m_InterfaceCount;
  
  Packet m_Packet;

};

#endif
