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
    Router(sc_module_name p_ModuleName, RouterConfig * const p_RouterConfiguration);

    ~Router();


    /*! \fn void interfaceUp(int p_InterfaceId)
     *  \brief Sets the given interface up
     *  @param[in] int p_InterfaceId The id of the interface to be
     *  set up
     * \public
     */
    void interfaceUp(int p_InterfaceId);

    /*! \fn void interfaceDown(int p_InterfaceId)
     *  \brief Sets the given interface down
     *  @param[in] int p_NumberOfRouters The id of the interface to be
     *  set down
     * \public
     */
    void interfaceDown(int p_InterfaceId);


    /*! \fn bool interfaceIsUp(int p_InterfaceId)
     *  \brief Checks whether the given interface is up or not
     *  @param[in] int p_InterfaceId The id of the interface to be
     *  set checked
     *  \return bool true: if up - false: if down
     * \public
     */
    bool interfaceIsUp(int p_InterfaceId);

    /*! \fn bool connectInterface(Router *p_TargetRouter, int p_LocalInterface,  int p_TargetInterface)
     *  \brief Connects the given interface of this router to the
     *  given interface of the given router
     *  @param[in] Router *p_TargetRouter
     *  @param[in] int p_LocalInterface
     *  @param[in] int p_TargetInterface
     *  \return bool true: if success or alredy connected - false: if
     *  one of the interfaces is conected and the other is not
     * \public
     */
    bool connectInterface(Router *p_TargetRouter, int p_LocalInterface,  int p_TargetInterface);

    /*! \fn void killRouter(void)
     *  \brief Sets all the interfaces of this router down, clears all
     *  the buffers, clears all BGP sessions, resets the BGP
     * \public
     */
    void killRouter(void);

    /*! \fn void killRouter(void)
     *  \brief Sets all the interfaces of this router down, clears all
     *  the buffers, clears all BGP sessions, resets the BGP
     * \public
     */
    void resetRouter(void);

    /*! \fn void killRouter(void)
     *  \brief Sets all the interfaces of this router down, clears all
     *  the buffers, clears all BGP sessions, resets the BGP
     * \public
     */
    void reviveRouter(void);

    /*! \fn void killInterface(p_InterfaceId)
     *  \brief kills the given interface
     *  \sa Interface_If
     *  @param[in] int p_InterfaceId The id of the interface to be disconnected
     * \public
     */
    void killInterface(int p_InterfaceId);

    /*! \fn void resetInterface(p_InterfaceId)
     *  \brief reset the given interface
     *  \sa Interface_If
     *  @param[in] int p_InterfaceId The id of the interface to be connected
     * \public
     */
    void resetInterface(int p_InterfaceId);

    /*! \fn void disconnectInterface(p_InterfaceId)
     *  \brief disconnects the given interface
     *  @param[in] int p_InterfaceId The id of the interface to be disconnected
     * \public
     */
    void disconnectInterface(int p_InterfaceId);

    /*! \fn void disconnectInterface(p_InterfaceId)
     *  \brief connects the given interface
     *  @param[in] int p_InterfaceId The id of the interface to be connected
     * \public
     */
    void connectInterface(int p_InterfaceId);

    /*! \fn void disconnectInterfaces(void)
     *  \brief disconnects all the interfaces
     * \public
     */
    void disconnectInterfaces(void);

    /*! \fn void disconnectInterface(void)
     *  \brief connects all the interfaces
     * \public
     */
    void connectInterfaces(void);

    /*! \fn string getRoutingTable(void)
     *  \brief get the routing table as a string
     * \public
     */
    string getRoutingTable(void);

    /*! \fn string getRawRoutingTable(void)
     *  \brief get the raw routing table as a string
     * \public
     */
    string getRawRoutingTable(void);

    /*! \fn void setPreferredAS(int p_AS, int p_pref_value)
     *  \brief give preference value for AS
     * \public
     */
    void setPreferredAS(int p_AS, int p_pref_value);

    /*! \fn void removeLocalPref(int p_AS)
     *  \brief remove given AS from the list of preferred ASes
     * \public
     */
    void removeLocalPref(int p_AS);

private:

    /************* Private members **************************/

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
    RouterConfig *m_RouterConfiguration;

    /************* Private functions **************************/

    /*! \fn void killInterfaces(void)
     *  \brief Sets all the interfaces of this router down
     * \public
     */
    void killInterfaces(void);

    /*! \fn void emptyInterfaces(void)
     *  \brief Clears the buffers in all the interfaces
     * \public
     */
    void emptyInterfaces(void);


};

#endif
