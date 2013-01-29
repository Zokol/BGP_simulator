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


/*!
 * \brief sc_main
 * \details Initiates the Simulation module, which builds up the Router modules and starts the simulation.
 */
int sc_main(int argc, char * argv [])
{

  ///initiate and run the simulation	
  Simulation("Test"); 

return 0;
}//end of main
