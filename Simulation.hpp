/*! \file Simulation.hpp
 *  \brief     Builds the simulation environment
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      28.1.2013
 */


/*!
 * \mainpage Example: Connecting two routers
 * \author Antti Siirilä, 501449, anjosi@utu.fi
 * \date 29.1.2013
 * \section s_intro
 */


/*!
 * \class Simulation
 * \brief Simulation top module
 *  \details
 */


#define _GUI

#include "systemc"
#include "Router.hpp"
#include "StringTools.hpp"
#include "ServerSocket.h"
#include "SocketException.h"


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
Simulation(sc_module_name p_Name, ServerSocket& p_Socket, SimulationConfig& p_SimuConfiguration);

    ~Simulation();


    void simulationMain(void);
    /*
      void before_end_of_elaboration()
      {
      cout << "Pata" << endl;
      }

      void end_of_elaboration()
      {
      cout << "Pata Pata" << endl;
      }
    */
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
     * \property string word
     * \brief holds the commands received from the UI
     * \details 
     * \private
     */
    string m_Word;

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
     * \property  int m_NumberOfRouters
     * \brief Holds the value that defines the number of routers to be
     * allocated in this simulations
     * \private
     */
    SimulationConfig m_SimuConfiguration;



};

