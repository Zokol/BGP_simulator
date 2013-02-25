
/*! \file RoutingTable.cpp
 *  \brief     Implementation of RoutingTable.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 19 08:44:06 2013
 */


#include "RoutingTable.hpp"
#include "StringTools.hpp"
#include "ReportGlobals.hpp"



RoutingTable::RoutingTable(sc_module_name p_ModName):sc_module(p_ModName)
{

    //make the inner bindings
    export_ToRoutingTable(m_ReceivingBuffer); //export the receiving
    //buffer's input
    //interface for the data plane

    
    SC_THREAD(routingTableMain);
    sensitive << port_Clk.pos();
}

RoutingTable::~RoutingTable()
{
}



void RoutingTable::routingTableMain(void)
{
    StringTools *l_Report = new StringTools(name());

    //debugging 
    SC_REPORT_INFO(g_ReportID, l_Report->newReportString("starting") );




    //The main thread of routing table module starts
    while(true)
        {
            wait();
            
            m_ReceivingBuffer.read(m_BGPMsg);
            l_Report->newReportString("Received BGP message from CP with outbound interface set to ");
            SC_REPORT_INFO(g_DebugID, l_Report->appendReportString(m_BGPMsg.m_OutboundInterface) );



        }
}


int RoutingTable::resolveRoute(sc_int<32> p_IPAddress)
{
    SC_REPORT_INFO(g_DebugID, StringTools(name()).appendReportString("resolveRoute-method was called.") );

    //TODO: implement the logic
    return 1;
}
