/*! \file Simulation.cpp
 *  \brief     Implementation of Simulation module.
 *  \details
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      28.1.2013
 */


#include "Simulation.hpp"
#include "ReportGlobals.hpp"
#include "GUIProtocolTags.hpp"

Simulation::Simulation(sc_module_name p_ModuleName, ServerSocket& p_GUISocket, SimulationConfig * const p_SimuConfiguration):sc_module(p_ModuleName), m_GUISocket(p_GUISocket), m_SimuConfiguration(p_SimuConfiguration)
{

//  //Throws segmentation fault

//  DEBUGGING
m_Name.appendReportString("Router count: ");
SC_REPORT_INFO(g_DebugID, m_Name.appendReportString(m_SimuConfiguration->getNumberOfRouters()));

m_Name.appendReportString("Interface count: ");
 SC_REPORT_INFO(g_DebugID, m_Name.appendReportString(m_SimuConfiguration->getRouterConfiguration(0).getNumberOfInterfaces()));


  /// \li Allocate Router pointer array
  m_Router = new Router*[m_SimuConfiguration->getNumberOfRouters()];

  /// \li Set the base name for the StringTools object
  m_Name.setBaseName("Router");




  /// \li Initiate the Router modules as m_Router
  for(int i = 0; i < m_SimuConfiguration->getNumberOfRouters(); i++)
    {
      /// \li Generate the routers
        m_Router[i] = new Router(m_Name.getNextName(), m_SimuConfiguration->getRouterConfigurationPtr(i));

    }


  ///Build the network
  RouterConfig *l_Handle;
  ///connect all the routers
  for (int i = 0; i < m_SimuConfiguration->getNumberOfRouters(); ++i)
      {
          ///get handle to the routers configuration object
          l_Handle = m_SimuConfiguration->getRouterConfigurationPtr(i);

          ///connect all the interfaces
          for (int j = 0; j < l_Handle->getNumberOfInterfaces(); ++j)
              {
                  ///Connect only those interfaces that have been configured
                  if (l_Handle->isConnection(j))
                      {
                          ///Connect the interfaces
                          if(m_Router[i]->connectInterface(m_Router[l_Handle->getNeighborRouterId(j)], j, l_Handle->getNeighborInterfaceId(j)))
                              {
                                  cout << "Router_" << i << "'s interface_" << j << " connected with the interface_" << l_Handle->getNeighborInterfaceId(j) <<" of router_" << l_Handle->getNeighborRouterId(j) << endl;
                              }
                      }
              }


      }


    SC_THREAD(simulationMain);
    sensitive << port_Clk.pos();


}

Simulation::~Simulation()
{

  /// \li Free all the memory
  for(int i = 0; i < m_SimuConfiguration->getNumberOfRouters(); i++)
    delete m_Router[i];

  delete m_Router;
}


void Simulation::simulationMain(void)
{

#ifdef _GUI_TEST

    bool state = true;
#elif defined (_GUI)
    enum_State = ACTIVE;


#else

#endif
    bool run = true;
    // m_GUISocket.set_non_blocking(true);
       RouterConfig *testC = m_SimuConfiguration->getRouterConfigurationPtr(0);
		int t = 20;
    while(run)
        {
            wait();
                testC->setMED(t++);
#ifdef _GUI_TEST

            m_Word = "";

            if(state)
                {
                    try
                        {
                            m_GUISocket >> m_Word;
                        }
                    catch(SocketException e)
                        {
                            //cout << e.description() << endl;
                            continue;
                        }

                    if(m_Word != "")
                        {
                            cout << "Received: " << m_Word << endl;
                            m_GUISocket << m_Word;
                            if(m_Word.compare("STOP") == 0)
                                sc_stop();
                        }
                }

            if(!(m_GUISocket.is_valid()))
                cout << "socket not valid" << endl;
#elif defined (_GUI)

            ///START:FSM of the socket server
            switch(enum_State)
                {

                case ACTIVE:

                    if(m_GUISocket.is_valid())
                        enum_State = receiveRoutine()?PROCESS:ACTIVE;
                    else
                        enum_State = TERMINATE;
                    break;
                case PROCESS:
                    socketRoutine();
                    break;
                case SEND:
                    if(m_GUISocket.is_valid())
                        enum_State = sendRoutine()?ACTIVE:SEND;
                    else
                        enum_State = TERMINATE;
                    break;
                case TERMINATE:
                    m_Word = STOP;
                    sendRoutine();
                    run = false;
                    break;

                default:
                    SC_REPORT_INFO(g_ReportID, m_Name.newReportString("Server state failure.\nSimulation ends."));

                    run = false;
                    break;
                }
            ///END:FSM of the socket server

#else

#endif
        }

#if defined (_GUI) || defined (_GUI_TEST)

    sc_stop();

#endif
}


void Simulation::socketRoutine(void)
{
    //TODO: Implement the methods

    ///default next state is SEND
    enum_State = SEND;

    ///The command cases
    if(m_Word.compare(STOP) == 0) ///STOP
        {
            //acknowledge
            m_Word = ACK;
            //set the next server state to TERMINATE
            enum_State = TERMINATE;
        }
    else if (m_Word.compare(RESET_ROUTER) == 0) ///RESET_ROUTER
        {

            //get the router ID
            fieldRoutine(1);
            //reset router
            m_Router[m_FieldBuffer[0]]->resetRouter();
            //send acknowledgement
            m_Word = ACK;
            //set next server state to ACTIVE
            enum_State = ACTIVE;
        }
    else if (m_Word.compare(KILL_ROUTER) == 0) ///KILL_ROUTER
        {

            //get the router ID
            fieldRoutine(1);
            //kill the router
            m_Router[m_FieldBuffer[0]]->killRouter();
            //send acknowledgement
            m_Word = ACK;
            //set next server state to ACTIVE
            enum_State = ACTIVE;

        }
    else if (m_Word.compare(REVIVE_ROUTER) == 0) ///REVIVE_ROUTER
        {

            //get the router ID
            fieldRoutine(1);
            //kill the router
            m_Router[m_FieldBuffer[0]]->reviveRouter();
            //send acknowledgement
            m_Word = ACK;
            //set next server state to ACTIVE
            enum_State = ACTIVE;
        }
    else if (m_Word.compare(READ_PACKET) == 0) ///READ_PACKET
        {

        }
    else if (m_Word.compare(CLEAR_PACKET) == 0) ///CLEAR_PACKET
        {

        }
    else if (m_Word.compare(SEND_PACKET) == 0) ///SEND_PACKET
        {

        }
    else if (m_Word.compare(CONNECT) == 0) ///CONNECT
        {
            //get the router ID
            fieldRoutine(2);
            //kill the router
            m_Router[m_FieldBuffer[0]]->connectInterface(m_FieldBuffer[1]);
            //send acknowledgement
            m_Word = ACK;
            //set next server state to ACTIVE
            enum_State = ACTIVE;

        }
    else if (m_Word.compare(DISCONNECT) == 0) ///DISCONNECT
        {
            //get the router ID
            fieldRoutine(2);
            //kill the router
            m_Router[m_FieldBuffer[0]]->disconnectInterface(m_FieldBuffer[1]);
            //send acknowledgement
            m_Word = ACK;
            //set next server state to ACTIVE
            enum_State = ACTIVE;

        }
    else if (m_Word.compare(READ_TABLE) == 0) ///READ_TABLE
        {

        }
    else if (m_Word.compare(READ_RAW_TABLE) == 0) ///READ_RAW_TABLE
        {

        }
    else if (m_Word.compare(SET_LOCAL_PREF) == 0) ///SET_LOCAL_PREF
        {

        }
    else if (m_Word.compare(SET_KEEPALIVE) == 0) ///SET_KEEPALIVE
        {

        }
    else if (m_Word.compare(SET_HOLDDOWN_MULT) == 0) ///SET_HOLDDOWN_MULT
        {

        }
    else if (m_Word.compare(SHOW_IF) == 0) /// SHOW_IF
        {

        }
    else
        {
            m_Word = NACK;
            enum_State = ACTIVE;
        }

    //send the response
    sendRoutine();


}

bool Simulation::sendRoutine(void)
{

    try
        {
            m_GUISocket << m_Word;
            return true;
        }
    catch(SocketException e)
        {
            return  false;
        }

}

bool Simulation::receiveRoutine()
{

    try
        {
            m_GUISocket >> m_Word;
            return true;
        }
    catch(SocketException e)
        {
            return  false;
        }

}

bool Simulation::fieldRoutine(int p_NumOfFields)
{
    bool read = true;
    int i = 0;
    while ((i < p_NumOfFields) && read)
        {
            if(receiveRoutine())
                {
                    istringstream(m_Word) >> m_FieldBuffer[i++];
                }
            else
                read = false;

        }
    return read;
}
