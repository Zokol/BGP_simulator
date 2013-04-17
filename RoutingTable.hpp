/*! \file  RoutingTable.hpp
 *  \brief     Header file of RoutingTable module
 *  \details
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 19 09:27:44 2013
 */

/*!
 * \class RoutingTable
 * \brief RoutingTable module
 *  \details Routing Table contains BGP routes and offers methods
 * to manage, and access them
 */


#include "systemc"
#include "RoutingTable_If.hpp"
#include "BGPMessage.hpp"
#include "Configuration.hpp"
#include "Interface_If.hpp"
#include "DataPlane_In_If.hpp"

using namespace std;
using namespace sc_core;
using namespace sc_dt;


#ifndef _ROUTINGTABLE_H_
#define _ROUTINGTABLE_H_

struct Route
{
    int id;
    string prefix;
    int mask;
    string ASes;
    vector<int> routers;    // this is the sequence of routers in one path
    int OutputPort;
    Route * next;
    Route * prev;
};

class RoutingTable: public sc_module, public RoutingTable_If
{

public:


    /*! \brief System clock signal
     * \details The router's internal clock
     * \public
     */
    sc_in_clk port_Clk;

    /*! \brief Input interface
     * \details Allows data plane to write received BGP messages into
     *  m_ReceivingBuffer-fifo
     * \public
     */
    sc_export<sc_fifo_out_if<BGPMessage> > export_ToRoutingTable;

    /*! \brief Control port
     * \details Routing table can check through this port whether the
     * network interfaces are up or not.
     * \public
     */
    sc_port<Interface_If, 0, SC_ZERO_OR_MORE_BOUND> port_Control;

    /*! \brief Output port for BGP messages
     * \details The RoutingTable writes all the BGP messages to be send
     * to its neighbors into
     * this port. The port should be bind to the Data Plane's.
     * receiving FIFO
     * \public
     */
    sc_port<DataPlane_In_If> port_Output;


    //    void before_end_of_elaboration()
    //{
    //}



    /*! \brief Elaborates the RoutingTable module
     * \details
     * \public
     */
    RoutingTable(sc_module_name p_ModuleName, ControlPlaneConfig * const p_RTConfig);



    /*! \brief Destructor of the RoutingTable module
     * \details Free's all the dynamically allocated memory
     * \public
     */
    ~RoutingTable();



    /*! \brief The main process of Control Plane module
     * \details \li Reads BGP messages from the m_ReceivingBuffer. \li
     * performs the route resolution process accoriding to BGP protocol.
     * \li Generates the required update messages. \li Keeps track on
     * different BGP sessions.
     * \public
     */
    void routingTableMain(void);

    /*! \brief
     * \details
     * \public
     */
    virtual int resolveRoute(string  p_IPAddress);



    /*! \brief Indicate the systemC producer that this module has a process.
     * \sa http://www.iro.umontreal.ca/~lablasso/docs/SystemC2.0.1/html/classproducer.html
     * \public
     */
    SC_HAS_PROCESS(RoutingTable);

    void addNewRoute(string p_msg, int p_outputPort);
    //void setMED(int p_routeId,)

    // Give preferred AS and some preference value to it.
    void setLocalPreference(int p_AS, int p_preferenceValue);

    // Remove some as from the list of preferred ASes
    void removeLocalPref(int p_AS);

    // Delete route from the RawRoutingTable. Parameters are router IDs.
    void deleteRoute(int p_router1, int p_router2);





    string getRoutingTable();
    string getRawRoutingTable();






private:



    /*! \brief Receiving buffer
     * \details Data plain writes all the received BGP messages into
     * this fifo
     * \private
     */
    sc_fifo<BGPMessage> m_ReceivingBuffer;

    void createRoute(string p_msg,int p_outputPort, Route * p_route);
    void handleNotification (BGPMessage NOTIFICATION_message);
    Route * findRoute(string p_IPAddress);
    int matchLength(Route * p_route, string p_IP);
    void updateRoutingTable();
    void addPreferredRoute(Route p_route1, Route p_route2);
    void setRoute(Route p_route);
    void removeFromRawTable(int p_route);
    void removeFromRoutingTable(int p_routerId);
    bool sameRoutes(Route p_route1, Route p_route2);
    int ASpathLength(Route p_route);

    string routeToString(Route p_route);

    string routingTableToString(Route * p_route);


    void printRoutingTable();
    void printRawRoutingTable();
    void printOneRoute(Route p_route);

    int tableLength();
    void fillRoutingTable();

    ControlPlaneConfig *m_RTConfig;

    /*! \brief BGP message
     * \details
     * \private
     */
    BGPMessage m_BGPMsg;

    Route * m_headOfRawTable;
    Route * m_endOfRawTable;
    Route * m_iterator;

    Route * m_headOfRoutingTable;
    Route * m_endOfRoutingTable;



    // Preferred ASes and their preference values are stored in here
    // Syntax: [AS][VALUE][AS][VALUE][AS][VALUE]... so even number contain the as and odd numbers their values.
    vector<int> preferredASes;


    // //TODO Find out what parameters we need for this function.
    // /*! \brief Set new route to the Routing Table
    //  * \details
    //  * \public
    //  */
    // bool setRoute(void);

    // //TODO
    // /*! \brief Set new route to the Routing Table
    //  * \details
    //  * \public
    //  */
    // bool updateRoute(void);



};



#endif /* _ROUTINGTABLE_H_ */
