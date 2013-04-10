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

Simulation::Simulation(sc_module_name p_ModuleName, ServerSocket& p_GUISocket, SimulationConfig& p_SimuConfiguration):sc_module(p_ModuleName), m_GUISocket(p_GUISocket), m_SimuConfiguration(p_SimuConfiguration)
{

//  //Throws segmentation fault

//  DEBUGGING
m_Name.appendReportString("Router count: ");
SC_REPORT_INFO(g_DebugID, m_Name.appendReportString(m_SimuConfiguration.getNumberOfRouters()));

m_Name.appendReportString("Interface count: ");
 SC_REPORT_INFO(g_DebugID, m_Name.appendReportString(m_SimuConfiguration.getRouterConfiguration(0).getNumberOfInterfaces()));


  /// \li Allocate Router pointer array
  m_Router = new Router*[m_SimuConfiguration.getNumberOfRouters()];

  /// \li Set the base name for the StringTools object
  m_Name.setBaseName("Router");




  /// \li Initiate the Router modules as m_Router
  for(int i = 0; i < m_SimuConfiguration.getNumberOfRouters(); i++)
    {
      /// \li Generate the routers
        m_Router[i] = new Router(m_Name.getNextName(), m_SimuConfiguration.getRouterConfiguration(i));

    }
  

  ///Build the network 
  RouterConfig *l_Handle;
  ///connect all the routers
  for (int i = 0; i < m_SimuConfiguration.getNumberOfRouters(); ++i)
      {
          ///get handle to the routers configuration object
          l_Handle = m_SimuConfiguration.getRouterConfigurationPtr(i);

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
  for(int i = 0; i < m_SimuConfiguration.getNumberOfRouters(); i++)
    delete m_Router[i];

  delete m_Router;
}


void Simulation::simulationMain(void)
{

#ifdef _GUI_TEST

    bool state = true, run = true;
#elif defined (_GUI)
    enum_State = RECEIVE;


#else

#endif
    // m_GUISocket.set_non_blocking(true);
    while(run)
        {
            wait();

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

            ///START:FSM of the simulation server
            switch(enum_State)
                {

                case RECEIVE:

                    
                    enum_State = receiveRoutine()? PROCESS:TERMINATE;

                    break;
                case PROCESS:
                    socketRoutine();
                    break;
                case SEND:
                    enum_State = sendRoutine()?RECEIVE:TERMINATE;
                    break;
                case TERMINATE:
                    sendRoutine();
                    run = false;
                    break;

                default:
                    SC_REPORT_INFO(g_ReportID, m_Name.newReportString("Server state failure.\nSimulation ends."));

                    run = false;
                    break;
                }
            ///END:FSM of the simulation server

#else

#endif
        }

#ifdef _GUI

    sc_stop();

#endif
}


void Simulation::socketRoutine(void)
{
    //TODO: the whole shit

    ///default next state is SEND
    enum_State = SEND;

    if(m_Word.compare(STOP)== 0)
        {
            m_Word = ACK;
            sendRoutine();
            enum_State = TERMINATE;
        }
    else if (m_Word.compare(READ_TABLE) == 0)
        {
            
        }
    else if (m_Word.compare(READ_TABLE) == 0)
        {
            
        }
    else
        {
            m_Word = NACK;
            enum_State = RECEIVE;            
        }
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
                            
            //TODO: implement proper retransmission
            //mechanism here

            SC_REPORT_INFO(g_ReportID, m_Name.newReportString("Server sending failure.\nSimulation ends."));
            return false;


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

            SC_REPORT_INFO(g_ReportID, m_Name.newReportString("Server receiving failure.\nSimulation ends."));
            
            return  false;
        }

}
