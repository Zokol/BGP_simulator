
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
#include <algorithm>





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
    m_headOfRawTable = new Route();
    m_endOfRawTable = new Route();
    m_iterator = new Route();

    m_headOfRoutingTable = new Route();
    m_endOfRoutingTable = new Route();

    StringTools *l_Report = new StringTools(name());

    //debugging
    SC_REPORT_INFO(g_ReportID, l_Report->newReportString("starting") );


    //The main thread of routing table module starts
    while(true)
        {
            fillRoutingTable(); // IIRO testing

            wait();
            m_ReceivingBuffer.read(m_BGPMsg);
            l_Report->newReportString("Received BGP message from CP with outbound interface set to ");
            SC_REPORT_INFO(g_DebugID, l_Report->appendReportString(m_BGPMsg.m_OutboundInterface) );

            if((m_BGPMsg.m_Type = UPDATE))
            {

                // IIRO testailuu kui hommat skulaa
                addNewRoute(m_BGPMsg.m_Message,m_BGPMsg.m_OutboundInterface);

                // updateRoutingTable();
                // cout << "Table length: " << tableLength() << endl;
                // cout << "Raw table: " << endl;
                // printRawRoutingTable();

                // cout << "Main table: " << endl;
                // printRoutingTable();
            }
            else if(m_BGPMsg.m_Type == NOTIFICATION)
            {
                cout << "In notification handling" << endl;
                handleNotification(m_BGPMsg);
            }
        }
}

/*
    Iterate through RawRoutingTable and check that every route in it is also found in MainRoutingTable.
    When same route is found from both tables. Check which one is preferred and update that to MainRoutingTable.
*/
void RoutingTable::updateRoutingTable()
{
    Route * l_iteratorRaw = m_headOfRawTable;

    bool l_routeInBothTables;   // Used to check that every route in RawTable exists in MainRoutingTable

/*
    if(m_headOfRoutingTable->next == 0)
        setRoute(m_headOfRawTable->next);
*/
    // Iterate RawTable
    while(l_iteratorRaw->next != 0)
    {
        l_iteratorRaw = l_iteratorRaw->next;

        m_iterator = m_headOfRoutingTable;
        l_routeInBothTables = false;

        // Iterate MainRoutingTable
        while(m_iterator->next != 0)
        {
            m_iterator= m_iterator->next;

            // Check if RawTable route has the same Prefix&Mask than current MainRoutingTable route
            if(m_iterator->prefix == l_iteratorRaw->prefix && m_iterator->mask == l_iteratorRaw->mask)
            {
                // Same Prefix&Mask combination was found from both tables. Check which route is preferred
                // Only one match in mainRT, so no need to continue iterating mainRT --> break inner while-loop
                addPreferredRoute(*m_iterator, *l_iteratorRaw);
                l_routeInBothTables = true;
                break;
            }
        }
        // Check that RawRoute was found from MainRoutingTable
        if(!l_routeInBothTables)
            setRoute(*l_iteratorRaw);
    }
}

/*
    Add new route to MainRoutingTable
*/
void RoutingTable::setRoute(Route p_rawRoute)
{
    Route * newRoute = new Route();
    newRoute->prefix = p_rawRoute.prefix;
    newRoute->mask = p_rawRoute.mask;
    newRoute->OutputPort = p_rawRoute.OutputPort;
    newRoute->ASes = p_rawRoute.ASes;
    if(m_headOfRoutingTable->next == 0)
    {
        // RoutingTable is empty. So add the first Route in it
        m_headOfRoutingTable->next = newRoute;
        newRoute->prev = m_headOfRoutingTable;
        m_endOfRoutingTable = newRoute;
        newRoute->next = 0;
    }
    else
    {
        // RoutingTable wasn't empty
        m_endOfRoutingTable->next = newRoute;
        newRoute->prev = m_endOfRoutingTable;
        m_endOfRoutingTable = newRoute;
        newRoute->next = 0;
    }


}

/*
    Take two routes as a parameter, check which one preferred and
    if the one from RawTable is preferred, replace the other one with that.
    Which route is preferred is decided by policies.

*/
void RoutingTable::addPreferredRoute(Route p_route1, Route p_route2)
{

}

// Print the RoutingTable
void RoutingTable::printRoutingTable()
{
    // Set the iterator in the beginning of table
    m_iterator = m_headOfRoutingTable;

    while(m_iterator->next != 0)
    {
        m_iterator = m_iterator->next;
        printOneRoute(*m_iterator);
    }

}

// Print the RawRoutingTable
void RoutingTable::printRawRoutingTable()
{
    // Set the iterator in the beginning of table
    m_iterator = m_headOfRawTable;

    while(m_iterator->next != 0)
    {
        m_iterator = m_iterator->next;
        printOneRoute(*m_iterator);
    }

}

void RoutingTable::printOneRoute(Route p_route)
{
    stringstream ss;
    ss << "Prefix: " <<  p_route.prefix << " Mask: " <<  p_route.mask << " Output port: " << p_route.OutputPort << " ASes: ";
    string message = ss.str();
    ss.str("");

    for(int i =0;i<p_route.ASes.size();i++)
    {
        ss << p_route.ASes.at(i);
    }

    message.append(ss.str());
    cout << message << endl;
}

/*
    Add new route to RawRoutingTable
*/
void RoutingTable::addNewRoute(string p_msg,int OutputPort)
{
    Route * newRoute = new Route();
    // Set data to newRoute in CreateRoute(...)
    createRoute(p_msg,OutputPort,newRoute);

    // Add new Route object to the RoutingTable

    // Check if the RoutingTable is empty
    if(m_headOfRawTable->next == 0)
    {
        // RoutinTable was empty. Add the first item in it. It's also the last item of it.
        m_headOfRawTable->next = newRoute;
        newRoute->prev = m_headOfRawTable;
        m_endOfRawTable = newRoute;
        newRoute->next = 0;
    }
    else
    {
        // RoutinTable wasn't empty. Add new Route object in it. Add it to the end.
        m_endOfRawTable->next = newRoute;
        newRoute->prev = m_endOfRawTable;
        m_endOfRawTable = newRoute;
        newRoute->next = 0;
    }
}

/*
    Create a Route object from p_msg. p_msg must be constructed as follows:
    IP;Mask;ASes (e.g. 10.255.0.100;8;550-7564-4)
    Parse message and collect IP,Mask and ASes which are separated by ";"-mark
*/
void RoutingTable::createRoute(string p_msg,int p_outputPort ,Route * p_route)
{
    // Use these to collect data separetad by semicolon
    int position = 0;
    int IP_end,Mask_end,ASes_end;

    for(int i=0;i<3;i++)
    {
        position = p_msg.find(";",position+1);

        if(i==0)
            IP_end = position;
        else if(i==1)
            Mask_end = position;
        else if(i==2)
            ASes_end = position;
    }


    string IPAddress = p_msg.substr(0,IP_end);
    string Mask = p_msg.substr((IP_end+1),(Mask_end-IP_end-1));  // -1 to remove ";"-sign
    string ASes = p_msg.substr((Mask_end+1),(ASes_end-Mask_end-1));

    // Set the values to Route pointer
    p_route->prefix = IPAddress;
    p_route->mask = atoi(Mask.c_str());
    p_route->ASes = ASes;
    p_route->OutputPort = p_outputPort;
}

/*
    Remove given route from raw routing table.
*/
void RoutingTable::removeFromRawTable(Route p_route)
{

}

void RoutingTable::handleNotification(BGPMessage p_msg)
{
}

/*
    Find Route object from RoutingTable by given IPAddress.
    Iterate through the RoutingTable and find the longest match with the given IPAddress.
    Then return pointer to the Route object that had the longest match
*/
Route * RoutingTable::findRoute(string p_IPAddress)
{
    Route * l_route = new Route();
    int l_longestMatch = 0;
    int l_matchLength;
    m_iterator = m_headOfRoutingTable;

    while(m_iterator->next != 0)
    {
        m_iterator = m_iterator->next;
        l_matchLength = matchLength(m_iterator, p_IPAddress);
        // if new match is longer than current longest match, set it as new longest match
        if(l_matchLength >= l_longestMatch)
        {
            l_longestMatch = l_matchLength;
            l_route = m_iterator;
        }
    }
    return l_route;
}

// Return how many "bits" from prefix match with IP address.
int RoutingTable::matchLength(Route * p_route, string  p_IP)
{
    return 0;
}

/*
    Take ip address as a parameter and return the outputport.
    DataPlane uses this function to find out where to forward its packets.
*/
int RoutingTable::resolveRoute(string p_IPAddress)
{
    SC_REPORT_INFO(g_DebugID, StringTools(name()).appendReportString("resolveRoute-method was called.") );
    Route * foundRoute = findRoute(p_IPAddress);
    return foundRoute->OutputPort;
}

/*
    only for testing?
*/
int RoutingTable::tableLength()
{
    int length = 0;
    m_iterator = m_headOfRoutingTable;
    while(m_iterator->next!=0)
    {
        length++;
        m_iterator= m_iterator->next;
    }
    return length;
}

// IIRO for testing
void RoutingTable::fillRoutingTable()
{
    // Create random numbers for BGP-message
    string prefix;

    int prefix1;
    int prefix2;
    int prefix3;
    int prefix4;

    int mask;
    int OutputPort;

    int AS1;
    int AS2;
    int AS3;
    string ASes;
    stringstream ss;
    string l_message;
    srand(time(NULL));

    for(int i=0;i<5;i++)
    {

        ss.str("");
        prefix1 = 1+(rand()%255);
        prefix2 = 1+(rand()%255);
        prefix3 = 1+(rand()%255);
        prefix4 = 1+(rand()%255);
        ss << prefix1 << "." << prefix2 << "." << prefix3 << "." << prefix4;
        prefix = ss.str();
        ss.str("");

        // to test routeFind()
        if(i==3)
            prefix = "50.40.200.0";

        AS1 = 1+(rand()%5000);
        AS2 = 1+(rand()%5000);
        AS3 = 1+(rand()%5000);
        ss << AS1 << "-" << AS2 << "-" << AS3;
        ASes = ss.str();
        ss.str("");

        mask = 1+(rand()%8);
        OutputPort = 1+(rand()%20);
        ss << prefix << ";" << mask << ";" << ASes;
        l_message = ss.str();

        //cout << "filled data before added to routing table: " << l_message << endl;

        //cout << l_message << endl;




        addNewRoute(l_message,OutputPort);
        addNewRoute(l_message,OutputPort);

    }

}












