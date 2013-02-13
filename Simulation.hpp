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



#include "systemc"
#include "Router.hpp"
#include "BGPSessionParameters.hpp"

using namespace std;
using namespace sc_core;
using namespace sc_dt;



//Simulation parameters


/*! \def ROUTER_COUNT
 *  Defines the number of rotuers in the simulation
 */
#define ROUTER_COUNT 3


/*! \def IF_COUNT
 *  Defines the number of interfaces in each router
 */
#define INTERFACE_COUNT 2


class Simulation: public sc_module
{

public:

    /*!
     * \brief Constructor
     * \details Builds the simulation
     * @param[in] p_Name The name of the module
     * \public
     */
    Simulation(sc_module_name p_Name);

    ~Simulation();
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
private:

    Packet m_Packet;


    /*!
     * \property sc_trace_file *m_TraceFilePointer
     * \brief Pointer to the VCD trace file
     * \details The packet trace data is stored into the file pointed by this pointer.
     * \private
     */
    sc_trace_file *m_TraceFilePointer;



    /*!
     * \property  string m_Name
     * \brief Name string
     * \details  Used in dynamic module naming.
     * \private
     */
    string m_Name;


    /*!
     * \property  Router **m_router
     * \brief Pointer to Router pointer
     * \details  Used in dynamic allocation of Router Modules
     * \private
     */

    Router **m_Router;

    BGPSessionParameters m_BGPSessionParam;


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

