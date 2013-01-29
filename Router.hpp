/*! \file Router.hpp
 *  \brief     
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      27.1.2013
 */



/*!
 * \class Router
 * \brief Router module
 *  \details
 */



#include "systemc"
#include "Interface.hpp"
#include "ProtocolEngine.hpp"

using namespace std;
using namespace sc_core;
using namespace sc_dt;



#ifndef ROUTER_H
#define ROUTER_H

 

class Router: public sc_module
{

public:

  sc_export<Interface_If> **export_ReceivingInterface;

  sc_port<Interface_If, 1, SC_ZERO_OR_MORE_BOUND> **port_ForwardingInterface;


  /*!
   * \brief Constructor
   * \details Builds the router
   * @param[in] p_Name The name of the module
   * \public
   */
  Router(sc_module_name p_ModuleName, int p_InterfaceCount);

  ~Router();

  void interfaceUp(int p_InterfaceId);  

private:



  /*!
   * \property   const sc_time *clk_Periods
   * \brief 
   * \details 
   * \private
   */
  const sc_time *m_ClkPeriod;


  /*!
   * \property sc_clock *clk_Router
   * \brief Pointer to sc_clock
   * \details  
   * \private
   */
  sc_clock *m_ClkRouter;


  ProtocolEngine *m_Bgp;

  Interface **m_NetworkInterface;

  int m_InterfaceCount;

  /*!
   * \property  string m_Name
   * \brief Name string
   * \details  Used in dynamic module naming.
   * \private
   */
  string m_Name;


  /*!
   * \fn   const char *appendName(string p_Name, int p)
   * \brief Append integer to a string and return const pointer to char string
   * \details  Used to append module id into the module base name
   * @param[in] p_Name string  Name string to be appended
   * @param[in] p int Interger value to be appended into the p_Name
   * \return const char pointer to the appended string
   * \private
   */
  const char *appendName(string p_Name, int p);
};

#endif
