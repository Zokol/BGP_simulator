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
#include "DataPlane.hpp"
#include "ControlPlane.hpp"
#include "RoutingTable.hpp"
#include "StringTools.hpp"
#include "Configuration.hpp"

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
    Router(sc_module_name p_ModuleName, RouterConfig& p_RouterConfiguration);

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

    /*!
     * \property ControlPlane m_Bgp 
     * \brief Conrol plane submodule of Router
     * \details Runs the BGP process
     * \private
     */
    ControlPlane m_Bgp;

    /*!
     * \property DataPlane m_IP 
     * \brief Data plane submodule of Router
     * \details Runs the IP process
     * \private
     */
    DataPlane m_IP;

    /*!
     * \property RoutingTable m_RoutingTable
     * \brief Routing table module of the router
     * \details Stores, manages, and resolves routes
     * \private
     */
    RoutingTable m_RoutingTable;

    /*!
     * \property Interface **m_NetworkInterface
     * \brief Pointer array to network interfaces of the router
     * \details 
     * \private
     */
    Interface **m_NetworkInterface;

    /*!
     * \property int m_InterfaceCount 
     * \brief Defines how many interface modules the router has
     * \details 
     * \private
     */
    int m_InterfaceCount;

    /*!
     * \property  StringTools m_Name
     * \brief 
     * \details  Used in dynamic module naming.
     * \private
     */
    StringTools m_Name;

    /*!
     * \property  RouterConfig m_RouterConfiguration
     * \brief Hold the configuration for this router
     * \details  
     * \private
     */
    RouterConfig m_RouterConfiguration;


};

#endif
