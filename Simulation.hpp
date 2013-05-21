/*! \file Simulation.hpp
 *  \brief     Builds the simulation environment
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      28.1.2013
 */


/*!
 * \mainpage BGP Simulator
 * \authors \li Antti Siirilä, 501449, anjosi@utu.fi \li Heikki Juva \li Iiro Räsänen
 * \date 13.5.2013
 * \section s_intro Introduction
 * BGP is a vital part of the Internet providing routing services between autonomous system. The goal of this projecs was to get familiar with the BGP protocol through a programming project. The programming projects contained the design and implementation of a BGP simulator. The design contains two main components the Simulation and the User Interface (UI). The implementation of the Simulation bases on SystemC where as the UI was implemeted with Python. 
 * \section s_design Design
 * The architecture of the BGP simulator has devided in two modules: User Interface (UI) implemented with Python, and Simulation implemented with the SystemC. The figure 1 illustrates the modules and their interconnection.
 * @image html Top.png "Figure 1. The top level of the BGP simulator"
 * The UI allows the user to setup the simulation environment including the number of routers, the network topology, and the router configurations. Once the simulation configuration is setup, it is sent to the sc_main through the TCP socket. The sc_main is responsiple on building the simulation environment according to the configuration data received from the UI.
Once the sc_main has elaborated the Simulation, it passes the TCP socket handle to the Simulation and start the SystemC kernel. From there on the BGP simulation stars and the UI is able to receive simulation status over the TCP link by using a defined set of commands (GUIProtocolTags.hpp). 
 * \subsection sub_Simulation Simulation
 The simulation module (figure 2) comprises of network nodes and their interconnection. Each network node contains the Router and Host modules. The host module represents the local AS and can be used to send and receive IP packets to/from other ASes.
 * @image html Simulation_diagram.png "Figure 2. The router modules inside the Simulation module"
 * The figure 3 defines the submodules and their relations inside the router module. The main modules are Data and Control planes, Routing table, and network Interfaces.
 * @image html Module_diagram.png "Figure 3. The submodules of the Router module"
 * The number of network interfaces of the Router is defined to four. Three of the interfaces are dedicated for BGP peer connections and the remaining one connects to the host module. Each Interface has receiving and forwarding buffers. 
 * The DataPlane (DP) module handles the IP packet and BGP message forwarding. It connects to the receiving and forwarding buffers of all interfaces. In addition the DP connect to the RoutingTable (RT), which allows it to resolve routes for IP packets. Furhtermore, the DP passes all the BGP message to the ControlPlane (CP) module and provides an interface for BGP message forwarding. IP packets are processed on bitlevel by PacketProcessor submodule. The forwarding function includes the IP packet validation, destination resolving, TTL decrementation, and checksum recalculation processes.
 * The CP has three BGP sessions for each BGP adjacent interface as submodules. ControlPlane simply distributes the received BGP messages to the correct bGP session.
 * The BGPSession module controls the BGP session states and timers. Once the peer interface of BGP session comes up, the session tries to establish a connection with the adjacent BGP router. The connection states follows the BGP specification (https://en.wikipedia.org/wiki/Border_Gateway_Protocol).
 * Once the connection is successfully established including the BGP OPEN message exchange, the BGP session transitions to the ESTABLISHED state. In ESTABLISHED state the BGP session simply sends keepalives, if required, and forwards BGP UPDATE messages to the RoutingTable (RT). Whenever the hold-down timer expires or a NOTIFICATION message is received, the BGPSession invalidates and transition to the IDLE state. The RoutingTable module builds the BGP routing table based on the local AS information and the received BGP UPDATE messages. RT listens the BGPSession states and updates the routing table and generates required UPDATE messages in case of change in state. In addition, RT reads the UPDATE messages from the input buffer and does the required table updates  according to BGP specification and local policies.
 * \subsection sub_UI User Interface
 * \section s_conclusion Conclusion
 * In this programming exercise, a BGP network simulator was designed, and implemented using Python and SystemC. The BGP cold start and table update processes were successfully simulated for start topologies. The IP packet processesing was implemented on bitlevel as specified in RFC 1812. The IP packet routing was also simulated by sending test packets between ASes.
 * \subsection sub_futureImprovements Future Improvements
 * Some of the basic functionality such as withdrawing a route and avoiding routing loops was not successfully simulated due to some bugs in the RT module. The simulation of the previously mentioned functions of BGP would require a thorough debugging of RT and perhaps a redesign of some of its components. In addition, implementing some experimental BGP functionalites such as security improvments would be interesting to simulate in the future.
 * \subsection sub_whatwelearned What We Learned
 */


/*!
 * \class Simulation
 * \brief Simulation top module
 *  \details
 */




#include "systemc"
#include "Router.hpp"
#include "Host.hpp"
#include "StringTools.hpp"
#include "ServerSocket.h"
#include "SocketException.h"
#include "PacketProcessor.hpp"


using namespace std;
using namespace sc_core;
using namespace sc_dt;



class Simulation: public sc_module
{

public:

    sc_in_clk port_Clk;

    /*!
     * \brief Constructor
     * \details Builds the simulation
     * @param[in] p_Name The name of the module
     * \public
     */
    Simulation(sc_module_name p_Name, ServerSocket& p_Socket, SimulationConfig * const p_SimuConfiguration);

    ~Simulation();

    void simulationMain(void);

    SC_HAS_PROCESS(Simulation);

private:

    /*!
     * \property ServerSocket m_GUISocket
     * \brief Socket to allow communication with GUI software
     * \details 
     * \private
     */
    ServerSocket m_GUISocket;

    /*!
     * \property string m_Word
     * \brief holds the commands received from the UI
     * \details 
     * \private
     */
    string m_Word;

    string m_Fields;

    string m_Cmd;

    /*!
     * \property Packet m_Packet
     * \brief 
     * \details 
     * \private
     */
    Packet m_Packet;

    /*!
     * \property sc_trace_file *m_TraceFilePointer
     * \brief Pointer to the VCD trace file
     * \details The packet trace data is stored into the file pointed by this pointer.
     * \private
     */
    sc_trace_file *m_TraceFilePointer;

    /*!
     * \property  StringTools m_Name
     * \brief Name string
     * \details  Used in dynamic module naming.
     * \private
     */
    StringTools m_Name;

    /*!
     * \property  Router **m_router
     * \brief Pointer to Router pointer
     * \details  Used in dynamic allocation of Router Modules
     * \private
     */
    Router **m_Router;

    /*!
     * \property  SimulationConfig m_SimuConfiguration
     * \brief Holds the configuration of the whole simulation
     * \private
     */
    SimulationConfig *m_SimuConfiguration;

    /*!
     * \property  int m_FieldBuffer
     * \brief Holds temporarly the values of command arguments
     * \private
     */
    string m_FieldBuffer[3];

    /*!
     * \property  int m_IntBuffer
     * \brief Holds temporarly the values of command arguments
     * \private
     */
    int m_IntBuffer[3];

    Host **m_Host;
    /*!
     * \property  enum ServerStates{RECEIVE, PROCESS, SEND, TERMINATE} enum_State
     * \brief Defines the socket server states
     * \private
     */
    enum ServerStates{ACTIVE, PROCESS, SEND, TERMINATE} enum_State, prev_State;

    /*!
     * \fn void socketRoutine(void) 
     * \brief Determines the type of the received command
     * \private
     */
    void socketRoutine(void);

    /*!
     * \fn bool sendRoutine(void)
     * \brief Writes the content of m_Word to the socket
     * return true: word sent - false: word not sent
     * \private
     */
    bool sendRoutine(void);

    /*!
     * \fn bool receiveRoutine(void)
     * \brief Reads a word from the socket if available
     * return true: word received - false: word not received
     * \private
     */
    bool receiveRoutine(void);

    /*!
     * \fn bool fieldRoutine(int p_NumOfFields)
     * \brief Reads the argument fields of a command
     * param[in] int p_NumOfFields Defines how many fields there is to be received
     * return true: word received - false: word not received
     * \private
     */
    bool fieldRoutine(int p_NumOfFields);

    /*!
     * \fn void bufferToInt(int p_NumberOfElements)
     * \brief Converts the string values of m_FieldBUffer to integer values in m_IntBuffer
     * param[in] int p_NumberOfElements Defines how many elements from the m_FieldBuffer will be converted
     * \private
     */
    void bufferToInt(int p_NumberOfElements);


};

