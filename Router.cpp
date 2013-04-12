/*! \file Router.cpp
 *  \brief     
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      27.1.2013
 */


#include "Router.hpp"
#include "ReportGlobals.hpp"

Router::Router(sc_module_name p_ModuleName, RouterConfig& p_RouterConfiguration):sc_module(p_ModuleName), m_Bgp("BGP", p_RouterConfiguration), m_IP("IP", p_RouterConfiguration.getNumberOfInterfaces()), m_RoutingTable("RoutingTable", p_RouterConfiguration), m_Name("Interface"),m_RouterConfiguration(p_RouterConfiguration)
{

    ///StringTools instance for reporting
    StringTools *l_Report = new StringTools(name());  
    l_Report->resetReportString();



    //DEBUGGING
    SC_REPORT_INFO(g_DebugID, l_Report->newReportString("Router starts"));
    l_Report->newReportString("Interface count: ");
    SC_REPORT_INFO(g_DebugID, l_Report->appendReportString(m_RouterConfiguration.getNumberOfInterfaces()));

    /// \li define clock period for Router
    m_ClkPeriod = new const sc_time(1, SC_SEC);

    /// \li Allocate clock for the Routers using the previously allocated period
    m_ClkRouter = new sc_clock("CLK", *m_ClkPeriod);

 
    SC_REPORT_INFO(g_DebugID, l_Report->newReportString("Binding the clock signal to submodules."));
 
    //set the clock for the planes
    m_Bgp.port_Clk(*m_ClkRouter);
    m_IP.port_Clk(*m_ClkRouter);
    //pass clock to the routing table module
    m_RoutingTable.port_Clk(*m_ClkRouter);

    SC_REPORT_INFO(g_DebugID, l_Report->newReportString("Binding the planes."));

    //bind
    //Control plane output to data plane
    m_IP.port_ToControlPlane(m_Bgp.export_ToControlPlane);
    //control plane output to data plane
    m_Bgp.port_ToDataPlane(m_IP); 
    //sessions output to data plane
    m_Bgp.export_ToDataPlane(m_IP);

    SC_REPORT_INFO(g_DebugID, l_Report->newReportString("Binding the routing table to the planes."));

    //bind the control plane to routing table
    m_Bgp.port_ToRoutingTable(m_RoutingTable.export_ToRoutingTable);

    //bind data plane to the routing table
    m_IP.port_ToRoutingTable(m_RoutingTable);

    //bind routing table to the data plane
    m_RoutingTable.port_Output(m_IP);

    SC_REPORT_INFO(g_DebugID, l_Report->newReportString("Building the network interfaces"));

    //allocate reference array for network interface modules
    m_NetworkInterface = new Interface*[m_RouterConfiguration.getNumberOfInterfaces()];

    //allocate reference array for receiving exports
    export_ReceivingInterface = new sc_export<Interface_If>*[m_RouterConfiguration.getNumberOfInterfaces()];

    //allocate reference array for fowarding ports
    port_ForwardingInterface = new sc_port<Interface_If, 1, SC_ZERO_OR_MORE_BOUND>*[m_RouterConfiguration.getNumberOfInterfaces()];

    //instantiate the network interface modules
    for(int i = 0; i < m_RouterConfiguration.getNumberOfInterfaces(); i++)
        {


      
            //instantiate an interface
            m_NetworkInterface[i] = new Interface(m_Name.getNextName());

            //instantiate hierarchial forwarding port
            port_ForwardingInterface[i] = new sc_port<Interface_If, 1, SC_ZERO_OR_MORE_BOUND>;
      
            //bind network interface port to router's hierarchial port
            m_NetworkInterface[i]->port_Output.bind(*port_ForwardingInterface[i]);
      
            //make the hierarchial binding for receiving exports
            export_ReceivingInterface[i] = new sc_export<Interface_If>;
            export_ReceivingInterface[i]->bind(*m_NetworkInterface[i]);

            //bind the clock to the network interface
            m_NetworkInterface[i]->port_Clk(*m_ClkRouter);      

            //bind the interfaces to the data plane
            m_IP.port_FromInterface(m_NetworkInterface[i]->export_ToDataPlane);
            m_IP.port_ToInterface(m_NetworkInterface[i]->export_FromDataPlane);

            //bind the interface to the Routing Table
            m_RoutingTable.port_Control(*m_NetworkInterface[i]);

        }
    //delete the StringTools object
    delete l_Report;
}

Router::~Router()
{

    for(int i = 0; i < m_RouterConfiguration.getNumberOfInterfaces(); i++)
        {
            delete export_ReceivingInterface[i];
            delete port_ForwardingInterface[i];
            delete m_NetworkInterface[i];
        }

  
    delete export_ReceivingInterface;
    delete port_ForwardingInterface;
    delete m_NetworkInterface;


    delete m_ClkPeriod;
    delete m_ClkRouter;
}

void Router::interfaceUp(int p_InterfaceId)
{
    m_NetworkInterface[p_InterfaceId]->interfaceUp();

    m_Name.setBaseName(m_NetworkInterface[p_InterfaceId]->name());
    SC_REPORT_INFO(g_DebugID, m_Name.newReportString("Network interface up."));
}

void Router::interfaceDown(int p_InterfaceId)
{
    m_NetworkInterface[p_InterfaceId]->interfaceDown();

    m_Name.setBaseName(m_NetworkInterface[p_InterfaceId]->name());
    SC_REPORT_INFO(g_DebugID, m_Name.newReportString("Network interface Down."));
}

void Router::emptyInterface(int p_InterfaceId)
{
    m_NetworkInterface[p_InterfaceId]->emptyBuffers();
}

bool Router::interfaceIsUp(int p_InterfaceId)
{
    return m_NetworkInterface[p_InterfaceId]->isUp();
}


bool Router::connectInterface(Router *p_TargetRouter,int p_LocalInterface, int p_TargetInterface)
{

    if(interfaceIsUp(p_LocalInterface) && p_TargetRouter->interfaceIsUp(p_TargetInterface))
        return true;
    else if (interfaceIsUp(p_LocalInterface) && !(p_TargetRouter->interfaceIsUp(p_TargetInterface)))
        {
            return false;
        }
    else if (!(interfaceIsUp(p_LocalInterface)) && p_TargetRouter->interfaceIsUp(p_TargetInterface))
        {
            return false;
        }
    else
        {
            port_ForwardingInterface[p_LocalInterface]->bind(*(p_TargetRouter->export_ReceivingInterface[p_TargetInterface]));

            p_TargetRouter->port_ForwardingInterface[p_TargetInterface]->bind(*(export_ReceivingInterface[p_LocalInterface]));

            interfaceUp(p_LocalInterface);
            p_TargetRouter->interfaceUp(p_TargetInterface);
            return true;
        }
}

void Router::killInterfaces(void)
{
    for (int i = 0; i < m_RouterConfiguration.getNumberOfInterfaces(); ++i)
    {
        interfaceDown(i);
    }
}

void Router::emptyInterfaces(void)
{
    for (int i = 0; i < m_RouterConfiguration.getNumberOfInterfaces(); ++i)
    {
        emptyInterface(i);
    }
}

