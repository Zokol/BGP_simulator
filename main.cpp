/*! 
 *  \file main.cpp
 *  \brief    The sc_main
 *  \details Start point of the simulation.
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      28.1.2013
 */


#include "Simulation.hpp"
#include "Configuration.hpp"


//!Defines the file name for the VCD output.
//#define VCD_FILE_NAME "anjosi_ex3_vcd"


using namespace std;
using namespace sc_core;
using namespace sc_dt;

#define SIMULATION_DURATION 20

/*!
 * TCP port of the GUI socket
 */
#define PORT 50000

/*!
 * Start tag of the configuration string
 */
#define START_TAG "<SIM_CONFIG>"

/*!
 * Length of the start tag
 */
#define START_TAG_LENGTH 12

/*!
 * End tag of the configuration string
 */
#define END_TAG "</SIM_CONFIG>"

/*!
 * Length of the end tag
 */
#define END_TAG_LENGTH START_TAG_LENGTH+1

/*!
 * Positive acknowledgement
 */
#define ACK "ack"

/*!
 * Negative acknowledgement
 */
#define NACK "nack"



#define _GUI

/*!
 * \brief SystemC main function
 * \details \li Establishes a TCP connection with the GUI program
 * \li Reads the simulation configuration from the GUI
 * \li Initiates the Simulation module
 * \li Starts the simulation
 */

const char* g_DebugID = "Level_debug:";
const char* g_ReportID = "Level_info:";
const char* g_SimulationVersion = "Test run";

int sc_main(int argc, char * argv [])
{
    ///Initiate a Server socket and bind it to port
    ServerSocket SimulationServer ( PORT ); 
    ///Declare a Server socket for the GUI connection
    ServerSocket GUISocket; 

#ifdef _GUI
   
    ///Accept the GUI connection
    cout << "Waiting the GUI to connect..." << endl;
    SimulationServer.accept ( GUISocket ); 
    ///Set the socket API to non-blocking mode
    GUISocket.set_non_blocking(true);
    ///String buffer for received data
    string DataWord, temp;
    bool setupLoop = true;
    cout << "Receiving from the GUI..." << endl;
    
    ///Start receiving from the GUI
    while(setupLoop)
        {
            try
                {
                    ///Try to receive
                    GUISocket >> DataWord;
                }
            catch(SocketException e)
                {
                    ///If the socket was empty continue receiving
                    continue;
                }

            ///Find the start tag
            
            if(DataWord.find(START_TAG, 0) == 0)
                {
                    ///start tag found. Continue to parse the
                    ///configuration string

                    ///Set the string index
                    int i_End, i_Start = START_TAG_LENGTH;
                    
                    ///find the end tag
                    i_End = DataWord.find(END_TAG, i_Start);

                    ///check that end tag was found
                    if(i_End == npos)
                        {
                            ///If end tag was not found, send NACK to
                            ///GUI and continue receiving
                            GUISocket << NACK;
                            continue;
                        }

                    ///Parse until the end tag is reached
                    while(i_Start < i_End)
                        {
                            ///declare and set the end and start
                            ///indeices for router parameters
                            int j_Start = i_Start, j_End;
                            
                            
                        }

                }
            else
                {
                    ///Report error to the GUI
                    GUISocket << NACK;
                    ///Continue receiving
                    continue;   
                }
                    
            if(DataWord.compare("SETUP") == 0)
                cout << "Set-up received" << endl;
            else if(DataWord.compare("STAR") == 0)
                {
                    cout << "Start received" << endl;
                    DataWord = "Simu";
                    setupLoop = false;
                }
            else
                cout << "Unknown GUI command: " << DataWord <<endl;
            try
                { 
                    GUISocket << DataWord;
                }
            catch(SocketException e)
                {
                    cout << e.description() << endl;
                }
        }
#endif


//testing the simulation configuration
SimulationConfig l_Config(3);
l_Config.addRouterConfig(0, 2);
l_Config.addRouterConfig(1, 2);
l_Config.addRouterConfig(2, 2);
l_Config.addBGPSessionParameters(0, 60, 3);
l_Config.addBGPSessionParameters(1, 60, 3);
l_Config.addBGPSessionParameters(2, 60, 3);

  /* Clock period intialization.
   * The clock period is 10 ns.
   */
  const sc_time clk_Period(1, SC_SEC);
  /* System clock.
   * The clock signal is specified in clk_Period.
   */
  sc_clock clk("clk", clk_Period);

    sc_report rp;
    sc_report_handler::set_log_file_name("test_simu.log");
    sc_report_handler::set_actions(g_ReportID, SC_INFO, SC_DISPLAY);
    sc_report_handler::set_actions(g_DebugID, SC_INFO, SC_DO_NOTHING);
    SC_REPORT_INFO(g_ReportID, g_SimulationVersion);

  ///initiate the simulation
Simulation test("Test", GUISocket, l_Config);

    ///connect the clock
    test.port_Clk(clk);
  SC_REPORT_INFO(g_ReportID, StringTools("Main").newReportString("Simulation starts"));


  ///run the simulation	
  sc_start(SIMULATION_DURATION, SC_SEC);
  SC_REPORT_INFO(g_ReportID, StringTools("Main").newReportString("Simulation ends"));
  GUISocket << "END";

return 0;
}//end of main
