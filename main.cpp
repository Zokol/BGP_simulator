/*! 
 *  \file main.cpp
 *  \brief    The sc_main
 *  \details Start point of the simulation.
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      28.1.2013
 */


#include "Simulation.hpp"



//!Defines the file name for the VCD output.
//#define VCD_FILE_NAME "anjosi_ex3_vcd"


using namespace std;
using namespace sc_core;
using namespace sc_dt;

#define SIMULATION_DURATION 200

/*!
 * \brief sc_main
 * \details Initiates the Simulation module, which builds up the Router modules and starts the simulation.
 */

const char* g_MessageId = "PP_G2_simulation/";
const char* g_SimulationVersion = "Test";
int sc_main(int argc, char * argv [])
{
    
    sc_report rp;
    sc_report_handler::set_log_file_name("test_simu.log");
    sc_report_handler::set_actions(g_MessageId, SC_INFO, SC_DO_NOTHING);
    SC_REPORT_INFO(g_MessageId, g_SimulationVersion);

  ///initiate the simulation
  Simulation test("Test");

  cout << "Simulation starts for " << SIMULATION_DURATION << " ns" << endl; 
  ///run the simulation	
  sc_start(SIMULATION_DURATION, SC_SEC);

return 0;
}//end of main
