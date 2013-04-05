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

#define IF_COUNT 4

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
 * Router field separator
 */
#define ROUTER_SEPARATOR ";"

/*!
 * Field separator
 */
#define FIELD_SEPARATOR ","

/*!
 * Positive acknowledgement
 */
#define ACK "ack"

/*!
 * Negative acknowledgement
 */
#define NACK "nack"



//#define _GUI

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


    ///field parsing states
    enum fieldStates{S_AS_ID, S_PREFIX, S_MED, S_LOCAL_PREF, S_KEEPALIVE, S_HOLDDOWNMUL, S_PORT_ID};

    ///Initiate a Server socket and bind it to port
    ServerSocket SimulationServer ( PORT ); 
    ///Declare a Server socket for the GUI connection
    ServerSocket GUISocket; 
    ///Instantiate a SimulationConfig object to store simulation
    ///configuration received from the GUI
    SimulationConfig l_Config;

    StringTools converter;
//    cout << converter.convertIPToBinary("192.168.1.0/24") << endl;
//    cout << converter.convertMaskToBinary("192.168.1.0/24") << endl;
//    cout << converter.convertIPToString(converter.convertIPToBinary("192.168.1.0/24"),converter.convertMaskToBinary("192.168.1.0/24")) << endl;

#ifdef _GUI
   
    ///Accept the GUI connection
    cout << "Waiting the GUI to connect..." << endl;
    SimulationServer.accept ( GUISocket ); 
    ///Set the socket API to non-blocking mode
    GUISocket.set_non_blocking(true);
    ///String buffer for received data
    string DataWord, temp;
    bool setupLoop = true, retrans = false;
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
            
            if(DataWord.compare(0, START_TAG_LENGTH, START_TAG) == 0)
                {
                    ///start tag found. Continue to parse the
                    ///configuration string

                    ///Set the string index
                    unsigned i_End, i_Start = START_TAG_LENGTH;
                    
                    ///find the end tag
                    i_End = DataWord.find(END_TAG, i_Start);

                    ///check that end tag was found
                    if(i_End == string::npos)
                        {
                            ///If end tag was not found, send NACK to
                            ///GUI and continue receiving
                            GUISocket << NACK;
                            continue;
                        }

                    ///Determine the number of routers
                    unsigned count = 1;
                    while(i_Start < i_End)
                        {
                            i_Start = DataWord.find(ROUTER_SEPARATOR, i_Start+1);
                            if (i_Start != string::npos)
                                {
                                    count++;
                                }
                            else
                                break;
                        }

                    ///set the number of routers
                    l_Config.init(count);

                    ///reset the i_Start
                    i_Start = START_TAG_LENGTH;

                    ///define an index and initiate it to zero
                    unsigned l_Idx = 0;

                    ///Parse the fields of all the routers
                    while(l_Idx < count)
                        {
                            ///declare and set the end and start
                            ///indeices for router parameters
                            unsigned j_Start = i_Start, j_End;

                    
                            ///find the end of the router field
                            if(l_Idx == count-1) //the last router
                                j_End = DataWord.find(END_TAG, j_Start);
                            else //other than the last router
                                j_End = DataWord.find(ROUTER_SEPARATOR, j_Start);

                            // ///check that the router field end was found
                            // if(j_End == string::npos)
                            //     {
                            //         ///If the router field separator was not found, send NACK to
                            //         ///GUI and continue receiving
                            //         GUISocket << NACK;
                            //         retrans = true;
                            //         break;
                            //     }

                            ///set the state to AS_ID
                            fieldStates state = S_AS_ID;
                            unsigned k_End = j_End;

                            ///Init router configuration object for
                            ///this 
                            l_Config.addRouterConfig(l_Idx, IF_COUNT);

                            ///refer to the previous
                            RouterConfig l_Router = l_Config.getRouterConfiguration(l_Idx);

                            bool parse = true;

                            ///parse until all the fields are extracted
                            while(parse)
                                {

                                    string field;

                                    ///parse the fields one by one
                                    k_End = DataWord.find(FIELD_SEPARATOR, j_Start);

                                    ///check the exception cases
                                    if (k_End > j_End || k_End == string::npos)
                                        {
                                            ///this is the end of the router field
                                            k_End = j_End; //set the end index accordingly
                                            parse = false;  //this is the final iteration in the while loop
                                        }

                                    ///get the field
                                    field = DataWord.substr(j_Start, k_End-j_Start);
                                    int l_IntField = 0;
                                    ///store the field
                                    switch (state)
                                        {
                                        case S_AS_ID:
                                        	istringstream(field) >> l_IntField;
                                            l_Router.setASNumber(l_IntField);
                                            state = S_PREFIX;
                                            break;
                                        case S_PREFIX:
                                        	l_Router.setPrefix(converter.convertIPToBinary(field));
                                        	l_Router.setPrefixMask(converter.convertMaskToBinary(field));
                                            state = S_MED;
                                            break;
                                        case S_MED:
                                           	istringstream(field) >> l_IntField;
                                            l_Router.setMED(l_IntField);
                                            state = S_LOCAL_PREF;
                                            break;
                                        case S_LOCAL_PREF:
                                           	istringstream(field) >> l_IntField;
                                            l_Router.setLocalPref(l_IntField);
                                            state = S_KEEPALIVE;
                                            break;
                                        case S_KEEPALIVE:
                                           	istringstream(field) >> l_IntField;
                                            l_Router.setKeepaliveTime(l_IntField);
                                            state = S_HOLDDOWNMUL;
                                            break;
                                        case S_HOLDDOWNMUL:
                                           	istringstream(field) >> l_IntField;
                                            l_Router.setHoldDownTimeFactor(l_IntField);
                                            state = S_PORT_ID;
                                            break;
                                        case S_PORT_ID:
                                        	//TODO: Parsing the connection configurations
                                            break;
                                        default:
                                            retrans = true;                  
                                        }
                                    if(retrans)
                                        break;
                                }//end of while
                            if(retrans)
                                break;
                          
                                
                            
                            
                        } //end of while

                    if(retrans)
                        {
                            retrans = false;
                            continue;
                        }
                }
            else
                {
                    ///Report error to the GUI
                    GUISocket << NACK;
                    ///Continue receiving
                    continue;   
                }

        }
#else


//testing the simulation configuration
    ///set the number of routers
    l_Config.init(3);

    l_Config.addRouterConfig(0, 2);
    l_Config.addRouterConfig(1, 2);
    l_Config.addRouterConfig(2, 2);
    l_Config.addBGPSessionParameters(0, 60, 3);
    l_Config.addBGPSessionParameters(1, 60, 3);
    l_Config.addBGPSessionParameters(2, 60, 3);
#endif
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

#ifdef _GUI  
  GUISocket << "END";
#endif

return 0;
}//end of main
