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
#include "GUIProtocolTags.hpp"
#include "PacketProcessor.hpp"

//!Defines the file name for the VCD output.
//#define VCD_FILE_NAME "anjosi_ex3_vcd"


using namespace std;
using namespace sc_core;
using namespace sc_dt;

/*!
 * \brief SystemC main function
 * \details \li Establishes a TCP connection with the GUI program
 * \li Reads the simulation configuration from the GUI
 * \li Initiates the Simulation module
 * \li Starts the simulation
 */

const char* g_DebugMainID = "Level_debug_main:";
const char* g_DebugID = "Level_debug:";
const char* g_ReportID = "Level_info:";
const char *g_DebugCPID = "Level_debug_CP";
const char *g_DebugBSID = "Level_debug_BS";
const char *g_DebugRTID = "Level_debug_RT";
const char *g_ErrorID = "Level_error";
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


    sc_report rp;
    //set log file
    sc_report_handler::set_log_file_name("test_simu.log");


    //SC_DO_NOTHING turns off the reportting for the specified flag
    //SC_DISPLAY turns on the reportting for the specified flag

    //general reporting flag
    sc_report_handler::set_actions(g_ReportID, SC_INFO, SC_DISPLAY);
    //general debugging flag
    sc_report_handler::set_actions(g_DebugID, SC_INFO, SC_DO_NOTHING);
    //debugging flag for ControlPlane
    sc_report_handler::set_actions(g_DebugCPID, SC_INFO, SC_DO_NOTHING);
    //debugging flag for BGPSession
    sc_report_handler::set_actions(g_DebugBSID, SC_INFO, SC_DO_NOTHING);
    //debugging flag for RoutringTable
    sc_report_handler::set_actions(g_DebugRTID, SC_INFO, SC_DISPLAY);
    //debuggin flag for this file
    sc_report_handler::set_actions(g_DebugMainID, SC_INFO, SC_DO_NOTHING);
    //debuggin flag for this file
    sc_report_handler::set_actions(g_ErrorID, SC_WARNING, SC_DISPLAY);
    SC_REPORT_INFO(g_ReportID, g_SimulationVersion);

    StringTools l_DebugReport("Main");
#if defined (_GUI) || defined(_GUI_TEST)
    RouterConfig *ptr_Router;

    ///Accept the GUI connection
    cout << "Waiting the GUI to connect..." << endl;
    SimulationServer.accept ( GUISocket );
    ///Set the socket API to non-blocking mode
    GUISocket.set_non_blocking(true);
    ///String buffer for received data
    string DataWord, temp;
    bool setupLoop = true, retrans = false;
    cout << "Receiving from the GUI..." << endl;

    //Simulation loop
//    while(true)
//    {
    ///Start receiving from the GUI
    while(setupLoop)
        {
            try
                {
                    ///Try to receive
                    GUISocket >> DataWord;
                    cout << DataWord << endl;

                }
            catch(SocketException e)
                {
                    ///If the socket was empty continue receiving
                    continue;
                }

            ///Find the start tag

            if(DataWord.compare(0, START_TAG_LENGTH, START_TAG) == 0)
                {
                    SC_REPORT_INFO(g_DebugMainID, l_DebugReport.newReportString("Start tag found"));

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

                    l_DebugReport.newReportString("Count: ");
                    SC_REPORT_INFO(g_DebugMainID, l_DebugReport.appendReportString(count));

                    ///set the number of routers
                    l_Config.init(count);

                    ///reset the i_Start
                    i_Start = START_TAG_LENGTH;

                    ///define an index and initiate it to zero
                    unsigned l_Idx = 0;

                    ///Parse the fields of all the routers
                    while(l_Idx < count) //Start of router field loop
                        {

                            l_DebugReport.newReportString("Router field #");
                            SC_REPORT_INFO(g_DebugMainID, l_DebugReport.appendReportString(l_Idx));

                            ///declare and set the end and start
                            ///indeices for router parameters
                            unsigned j_Start = i_Start, j_End;

                            ///find the end of the router field
                            if(l_Idx == count-1) //the last router
                                {
                                    j_End = DataWord.find(END_TAG, i_Start);
                                    setupLoop = false;
                                }
                            else //other than the last router
                                j_End = DataWord.find(ROUTER_SEPARATOR, i_Start);

                            ///set the state to AS_ID
                            fieldStates state = S_AS_ID;
                            unsigned k_End = j_End;

                            ///Init router configuration object for
                            ///this
                            l_Config.addRouterConfig(l_Idx, IF_COUNT);

                            ///refer to the previous
                            ptr_Router = l_Config.getRouterConfigurationPtr(l_Idx);

                            bool parse = true;

                            ///parse until all the fields are extracted
                            while(parse) //Start of field loop
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
                                            SC_REPORT_INFO(g_DebugMainID, l_DebugReport.newReportString("End of router field."));

                                        }
                                    l_DebugReport.newReportString("j_Start: ");
                                    SC_REPORT_INFO(g_DebugMainID, l_DebugReport.appendReportString(j_Start));
                                    l_DebugReport.newReportString("k_End: ");
                                    SC_REPORT_INFO(g_DebugMainID, l_DebugReport.appendReportString(k_End));


                                    ///get the field
                                    field = DataWord.substr(j_Start, k_End-j_Start);

                                    l_DebugReport.newReportString("Field: ");
                                    SC_REPORT_INFO(g_DebugMainID, l_DebugReport.appendReportString(field.c_str()));



                                    int l_IntField = 0;

                                    string subField = "";
                                    int m_Start = 0, m_End, tempFields[3];

                                    ///store the field
                                    switch (state)
                                        {
                                        case S_AS_ID:
                                        	istringstream(field) >> l_IntField;
                                            ptr_Router->setASNumber(l_IntField);
                                            state = S_PREFIX;
                                            break;
                                        case S_PREFIX:
                                        	ptr_Router->setPrefix(field);
                                            state = S_MED;
                                            break;
                                        case S_MED:
                                           	istringstream(field) >> l_IntField;
                                            ptr_Router->setMED(l_IntField);
                                            state = S_LOCAL_PREF;
                                            break;
                                        case S_LOCAL_PREF:
                                           	istringstream(field) >> l_IntField;
                                            ptr_Router->setLocalPref(l_IntField);
                                            state = S_KEEPALIVE;
                                            break;
                                        case S_KEEPALIVE:
                                           	istringstream(field) >> l_IntField;
                                            ptr_Router->setKeepaliveTime(l_IntField);
                                            state = S_HOLDDOWNMUL;
                                            break;
                                        case S_HOLDDOWNMUL:
                                           	istringstream(field) >> l_IntField;
                                            ptr_Router->setHoldDownTimeFactor(l_IntField);
                                            state = S_PORT_ID;
                                            break;
                                        case S_PORT_ID:

                                            for(int i = 0; i < 3; i++)
                                                {
                                                    //parse each
                                                    //connecion field:
                                                    //local port id,
                                                    //client port id,
                                                    //client id
                                                    m_End = field.find(CONNECTION_SEPARATOR, m_Start);

                                                    ///store the sub field
                                                    subField = field.substr(m_Start, m_End-m_Start);


                                                    l_DebugReport.newReportString("subField: ");
                                                    SC_REPORT_INFO(g_DebugMainID, l_DebugReport.appendReportString(subField.c_str()));


                                                    ///Store each
                                                    ///sub field into a
                                                    ///temporary array
                                                    istringstream(subField) >> tempFields[i];

                                                    ///update the
                                                    ///index for the
                                                    ///next iteration
                                                    m_Start = m_End + 1;
                                                }

                                            ///Store the connection
                                            ///parameters to the
                                            ///connection confi object
                                            ptr_Router->addConnectionConfig(tempFields[0], tempFields[1], tempFields[2]);

                                            break;
                                        default:
                                            retrans = true;
                                        }

                                    j_Start = k_End + 1;

                                    if(retrans)
                                        break;

                                    i_Start = k_End + 1;
                                }//end of field loop

                            if(retrans)
                                break;


                            l_Idx++;

                        } //End of router field loop

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
                    cout << "Unknown messge" << endl;
                    ///Continue receiving
                    continue;
                }

        }//End of receiving loop

    //cout << l_Config.toString().c_str() << endl;

    l_Config.ifModes();
    //Sync with the test client
    GUISocket << ACK;
    setupLoop = true;

    while(setupLoop)
        {

            try
                {
                    ///Try to receive
                    GUISocket >> DataWord;
            		if(DataWord.find("<CMD>", 0, 5) == string::npos)
            		{
            			GUISocket << NACK;
            			continue;
            		}
            		else if(DataWord.find("</CMD>",0) == string::npos)
            		{
            			GUISocket << NACK;
            			continue;
            		}
            		else
            		{
            			DataWord = DataWord.substr(5);
            			DataWord = DataWord.substr(0,DataWord.find("</CMD>",0));
            		}
                }
            catch(SocketException e)
                {
                    ///If the socket was empty continue receiving
                    continue;
                }
            if (DataWord.compare("START") == 0)
                {
                    GUISocket << ACK;
                    setupLoop = false;
                }
            else
            	GUISocket << NACK;
        }


    SC_REPORT_INFO(g_ReportID, StringTools("Main").newReportString("Out of receiving loop"));

    ///Output received configuration

#else

    cout << "CONF"<< endl;
//testing the simulation configuration
    ///set the number of routers
    l_Config.init(3);

    l_Config.addRouterConfig(0, 4);
    l_Config.addRouterConfig(1, 4);
    l_Config.addRouterConfig(2, 4);
    cout << "CONF"<< endl;
    l_Config.addBGPSessionParameters(0, 60, 3);
    l_Config.addBGPSessionParameters(1, 60, 3);
    l_Config.addBGPSessionParameters(2, 60, 3);
	
	//set AS number here
    l_Config.getRouterConfiguration(0).setASNumber(101);
    l_Config.getRouterConfiguration(1).setASNumber(201);
    l_Config.getRouterConfiguration(2).setASNumber(301);

    //set Prefix
    string l_Prefix = "11.0.0.0/8";
    l_Config.getRouterConfiguration(0).setPrefix(l_Prefix);
    l_Prefix = "12.0.0.0/8";
    l_Config.getRouterConfiguration(1).setPrefix(l_Prefix);
    l_Prefix = "13.0.0.0/8";
    l_Config.getRouterConfiguration(2).setPrefix(l_Prefix);

    //    int p_LocalRouterId, int p_LocalInterfaceId, int p_NeighborInterfaceId, int p_NeighborRouterId
    l_Config.addConnectionConfig(0, 0, 0, 1 );
    l_Config.addConnectionConfig(1, 0, 0, 0 );
    l_Config.addConnectionConfig(1, 1, 0, 2 );
    l_Config.addConnectionConfig(2, 0, 1, 1 );
    l_Config.addConnectionConfig(2, 1, 1, 0 );
    l_Config.addConnectionConfig(0, 1, 1, 2 );


#endif


  /* Clock period intialization.
   * The clock period is 10 ns.
   */
    const sc_time clk_Period(1, SC_SEC);
  /* System clock.
   * The clock signal is specified in clk_Period.
   */
  sc_clock clk("clk", clk_Period);

  ///initiate the simulation
  Simulation test("Test", GUISocket, &l_Config);

  ///connect the clock
  test.port_Clk(clk);
  SC_REPORT_INFO(g_ReportID, StringTools("Main").newReportString("Simulation starts"));


  ///run the simulation

cout << "START"<< endl;
#if defined (_GUI) || defined(_GUI_TEST)
  sc_start();
  GUISocket << "ACK";

  //  ptr_Router = NULL;
#else
  sc_start(SIMULATION_DURATION, SC_SEC);
#endif



//    PacketProcessor tes;
//    Packet l_Frame = tes.buildIPPacket("192.168.1.1", "192.168.1.2", "_ This is a test message");
//
//    PacketProcessor check;
//    check.processFrame(l_Frame);
//    cout << "First packet built: " << check.readIPPacket() << endl;
//    PacketProcessor rcv;
//    if(rcv.processFrame(l_Frame))
//        cout << "Packet is valid" << endl;// << l_Frame << endl;
//    else
//        cout << "Packet is not valid" << endl;// << l_Frame << endl;
//
//    cout << endl << endl << "Destination IP is " << rcv.getDestination() << endl;
//
//    if(rcv.forward(l_Frame))
//        {
//
//            check.processFrame(l_Frame);
//            cout<<"First packet forwarded: " << check.readIPPacket() << endl;
//        }
//
//    l_Frame = tes.buildIPPacket("192.168.2.1", "192.168.1.3", "_ This is another test message with ä and ö and å");
//    check.processFrame(l_Frame);
//    cout <<"Second packet built: " << check.readIPPacket() << endl;
//
//
//    if(rcv.processFrame(l_Frame))
//        cout << "Packet is valid" << endl;// << l_Frame << endl;
//    else
//        cout << "Packet is not valid" << endl;// << l_Frame << endl;
//    if(rcv.forward(l_Frame))
//        {
//
//
//                        check.processFrame(l_Frame);
//                        cout <<"Second packet forwarded: " << check.readIPPacket() << endl;
//        }
//
//
//  cout << endl << endl << "Destination IP is " << rcv.getDestination() << endl;
//
//
//  cout << l_Frame;
//
//  cout << l_Frame.outputPDU();
//
  SC_REPORT_INFO(g_ReportID, StringTools("Main").newReportString("Simulation ends"));
//#if defined (_GUI) || defined(_GUI_TEST)
//    }
//#endif
return 0;
}//end of main
