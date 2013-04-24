
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




RoutingTable::RoutingTable(sc_module_name p_ModName, ControlPlaneConfig * const p_RTConfig):sc_module(p_ModName), m_RTConfig(p_RTConfig)
{

    //make the inner bindings
    export_ToRoutingTable(m_ReceivingBuffer); //export the receiving
    //buffer's input

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

    fillRoutingTable(); // IIRO testing


    int count = 0;
    //The main thread of routing table module starts
    while(true)
        {

            wait();

            ///Check the Interface states
            for (int i = 0; i < m_RTConfig->getNumberOfInterfaces()-1; i++)
                {
                    if(!(port_Session[i]->isSessionValid()))
                    {
                        //deleteRoutes(i);
                        // cout << "Session " << i << " has AS number: " << port_Session[i]->getPeerAS()  << endl;
                    }
                }





            m_ReceivingBuffer.read(m_BGPMsg);
            if(!(count%20))
                {
                    l_Report->newReportString("Received BGP message from CP with outbound interface set to ");
                    SC_REPORT_INFO(g_DebugID, l_Report->appendReportString(m_BGPMsg.m_OutboundInterface) );
                }
            count++;

            ///BGP notification and update output port
            // port_Output->write(m_BGPMsg);

            if((m_BGPMsg.m_Type = UPDATE))
            {
                updateRoutingTable();

                // IIRO testailuu
                //addNewRoute(m_BGPMsg.m_Message,m_BGPMsg.m_OutboundInterface);


 /*               cout << "Raw table: " << endl;
                printRawRoutingTable();

                preferredASes.push_back(5432);
                preferredASes.push_back(100);


                cout << "Main table: " << endl;
                printRoutingTable();

                handleWithdraw("0,50.40.200.0,2,50-70-100-5555");
*/            }
            else if(m_BGPMsg.m_Type == NOTIFICATION)
            {
                cout << "In notification handling" << endl;
                handleNotification(m_BGPMsg);
            }
        }
}

/*
    1. Iterate through RawRoutingTable and check that every route in it is also found in MainRoutingTable.
    When same route is found from both tables check which one is preferred and update that to MainRoutingTable.

    2. Iterate through MainRT and verify that there isn't any route that doesn't exist in RawRT.
*/
void RoutingTable::updateRoutingTable()
{
    // 1. Verify that every route in RawRT is found from MainRT
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
            if(sameRoutes(*m_iterator, *l_iteratorRaw))
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
        {
            setRoute(*l_iteratorRaw);
        }

    }

    // 2. Now verify that every route in MainRT is also found from RawRT
    l_iteratorRaw = m_headOfRawTable;
    m_iterator = m_headOfRoutingTable;

    // Iterate MainRT
    bool foundFromRawTable;
    while(m_iterator->next != 0)
    {
        l_iteratorRaw = m_headOfRawTable;
        foundFromRawTable = false;
        m_iterator = m_iterator->next;

        // Iterate RawRT
        while(l_iteratorRaw->next != 0)
        {

            l_iteratorRaw = l_iteratorRaw->next;
            if(sameRoutes(*m_iterator,*l_iteratorRaw))
            {
                foundFromRawTable = true;
                break;
            }
        }
        if(!foundFromRawTable)
        {
            // Route didin't exist in RawRT so remove it from MainTable
            removeFromRoutingTable(m_iterator->id);
        }
    }



}

/*
    Compare two routes and return true if they are the same
*/
bool RoutingTable::sameRoutes(Route p_route1, Route p_route2)
{
    if(p_route1.prefix == p_route2.prefix && p_route1.mask == p_route2.mask)
        return true;
    else
        return false;
}

// Return the AS Path length
int RoutingTable::ASpathLength(Route p_route)
{
    int ASCount = count(p_route.ASes.begin(), p_route.ASes.end(), '-') + 1;
    return ASCount;
}

/*
    Add new route to MainRoutingTable
*/
void RoutingTable::setRoute(Route p_route)
{
    Route * newRoute = new Route();

    newRoute->id = (m_endOfRoutingTable->id)+1;
    newRoute->prefix = p_route.prefix;
    newRoute->mask = p_route.mask;
    newRoute->OutputPort = p_route.OutputPort;
    newRoute->ASes = p_route.ASes;


    if(m_headOfRoutingTable->next == 0)
    {
        // RoutingTable is empty. So add the first Route in it
        m_headOfRoutingTable->next = newRoute;
        m_endOfRoutingTable = newRoute;
        newRoute->next = 0;
    }
    else
    {
        // RoutingTable wasn't empty
        m_endOfRoutingTable->next = newRoute;
        m_endOfRoutingTable = newRoute;
        newRoute->next = 0;
    }
}

/*
    Take two routes as a parameter, check which one is preferred and
    if the one from RawTable is preferred, replace the other one with that.
    Which route is preferred is decided by policies.
    p_route1 is already in MainRoutingTable, so do nothing if that route is preferred over p_route2.

*/
void RoutingTable::addPreferredRoute(Route p_route1, Route p_route2)
{
    /*
        Add preferred route to routing table according to policies. Policies:
        1. Check if which route has higher preferredAS
        2. Check AS-path length
        3. Origin type, not used?

    */

    /*  1. TODO: only picks up the highes value. What if p_route1 and p_route2 have same highest value but different 2. highest value?
        Solution: calculate sum instead of the highest value?
    */
    int route1_highest_pref = 0;
    int route2_highest_pref = 0;
    int AS;
    unsigned newPosition = 0;
    unsigned oldPosition = 0;

    // 1. Iterate through preferredASes and check if other Route has preferred AS on its ASpath
    for(unsigned i = 0;i<preferredASes.size();i = i+2)
    {
        while(newPosition < p_route1.ASes.size())
        {
            newPosition = p_route1.ASes.find("-",oldPosition+1);
            AS =  atoi((p_route1.ASes.substr(oldPosition,newPosition-oldPosition)).c_str());
            oldPosition = newPosition+1;

            if(AS == preferredASes.at(i))
            {
                if(route1_highest_pref < preferredASes.at(i+1))
                    route1_highest_pref = preferredASes.at(i+1);
            }
        }
        newPosition = 0;
        oldPosition = 0;


        while(newPosition < p_route2.ASes.size())
        {

            newPosition = p_route2.ASes.find("-",oldPosition+1);
            AS =  atoi((p_route2.ASes.substr(oldPosition,newPosition-oldPosition)).c_str());
            oldPosition = newPosition+1;

            if(AS == preferredASes.at(i))
            {
                if(route2_highest_pref < preferredASes.at(i+1))
                    route2_highest_pref = preferredASes.at(i+1);
            }
        }
    }



    if(route2_highest_pref > route1_highest_pref)
    {
        // p_route2 was preferred over p_route1. So remove p_route1 from MainRoutingTable and replace it by p_route2
        removeFromRoutingTable(p_route1.id);
        setRoute(p_route2);
        return;
    }

    // 2. AS-path length
    if(ASpathLength(p_route1) < ASpathLength(p_route2))
    {
        // p_route2 had shorter AS-path, so replace p_route1
        removeFromRoutingTable(p_route1.id);
        setRoute(p_route2);
        return;
    }


}

/*
    Return MainRoutingTable
*/
string RoutingTable::getRoutingTable()
{
    return routingTableToString(m_headOfRoutingTable);
}

/*
    Return RawRoutingTable
*/
string RoutingTable::getRawRoutingTable()
{
    return routingTableToString(m_headOfRawTable);
}


/*
    Iterate through the routing table, beginning from p_route.
    Create a string from the routes and return that string.
*/
string RoutingTable::routingTableToString(Route * p_route)
{
    m_iterator = p_route;
    string table;
    table.append("<TABLE>");

    while(m_iterator->next != 0)
    {
        m_iterator = m_iterator->next;
        table.append(routeToString(*m_iterator));
        table.append(";");
    }
    table.append("</TABLE>");

    return table;
}

/*
    Create string from given route. Prefix, Mask, Routers and ASes are included in the string.
    Syntax: ID,Prefix,Mask,ASes (e.g. 5,100100200050,8,100-4212-231-22)
*/
string RoutingTable::routeToString(Route p_route)
{
    stringstream ss;
    ss.str("");
    ss << p_route.id  << "," << p_route.prefix << "," << p_route.mask << "," << p_route.ASes;
    return ss.str();
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
    ss << "Id:" << p_route.id <<" Prefix: " <<  p_route.prefix << " Mask: " <<  p_route.mask << " Output port: " << p_route.OutputPort << " ASes: " << p_route.ASes;
    string message = ss.str();
    cout << message << endl;
}

/*
    Add new route to RawRoutingTable
*/
void RoutingTable::addRouteToRawTable(string p_msg,int OutputPort)
{
    Route * newRoute = new Route();

    // Get ID for this new route
    newRoute->id = (m_endOfRawTable->id)+1;

    // Set data to newRoute in CreateRoute(...)
    createRoute(p_msg,OutputPort,newRoute);


    // Add new Route object to the RoutingTable

    // Check if the RoutingTable is empty
    if(m_headOfRawTable->next == 0)
    {
        // RoutinTable was empty. Add the first item in it. It's also the last item of it.
        m_headOfRawTable->next = newRoute;
        m_endOfRawTable = newRoute;
        newRoute->next = 0;
    }
    else
    {
        // RoutinTable wasn't empty. Add new Route object in it. Add it to the end.
        m_endOfRawTable->next = newRoute;
        m_endOfRawTable = newRoute;
        newRoute->next = 0;
    }
}

/*
    Create a Route object from p_msg. p_msg must be constructed as follows:
    IP;Mask;ASes;ownRouterId;ownAS (e.g. 10.255.0.100,8,550-7564-4,9,555)
    Parse message and collect IP,Mask,ASes, own router id and own AS number which are separated by ","-mark
*/
void RoutingTable::createRoute(string p_msg,int p_outputPort ,Route * p_route)
{
    // Use these to collect data separetad by semicolon
    int position = 0;
    int IP_end,Mask_end, ASes_end,ownRouterId_end,ownAsId_end;

    for(int i=0;i<5;i++)
    {
        position = p_msg.find(",",position+1);

        if(i==0)
            IP_end = position;
        else if(i==1)
            Mask_end = position;
        else if(i==2)
            ASes_end = position;
        else if(i==3)
            ownRouterId_end = position;
        else if(i==4)
            ownAsId_end = position;
    }

    string IPAddress = p_msg.substr(0,IP_end);
    string Mask = p_msg.substr((IP_end+1),(Mask_end-IP_end-1));  // -1 to remove ";"-sign
    string ASes = p_msg.substr((Mask_end+1),(ASes_end-Mask_end-1));
    string ownRouter = p_msg.substr((ASes_end+1),(ownRouterId_end-ASes_end-1));
    string ownAS = p_msg.substr((ownRouterId_end+1),(ownAsId_end-ownRouterId_end-1));

    ASes.append("-");
    ASes.append(ownAS);


    // Set the values to Route pointer
    p_route->prefix = IPAddress;
    p_route->mask = atoi(Mask.c_str());
    p_route->ASes = ASes;
    p_route->OutputPort = p_outputPort;
}

/*
    Remove given route from RawRoutingTable
*/
void RoutingTable::removeFromRawTable(int p_routeId)
{
    cout << "REMOVE FROM RAWTABLE" << endl;
    Route * deleteRoute = new Route();
    Route * tempRoute = new Route();
    deleteRoute = m_headOfRawTable;
    while(deleteRoute->next != 0)
    {
        tempRoute = deleteRoute;
        deleteRoute = deleteRoute->next;
        if(deleteRoute->id == p_routeId)
        {
            if(deleteRoute == m_endOfRoutingTable)
            {
                m_endOfRoutingTable = tempRoute;
            }
            tempRoute->next = deleteRoute->next;
            return;
        }
    }
}

/*
    Remove given route from MainRoutingTable
*/
void RoutingTable::removeFromRoutingTable(int p_routeId)
{
    Route * deleteRoute = new Route();
    Route * tempRoute = new Route();
    deleteRoute = m_headOfRoutingTable;
    while(deleteRoute->next != 0)
    {
        tempRoute = deleteRoute;
        deleteRoute = deleteRoute->next;
        if(deleteRoute->id == p_routeId)
        {
            tempRoute->next = deleteRoute->next;
            if(deleteRoute == m_endOfRoutingTable)
            {
                m_endOfRoutingTable = tempRoute;
            }
            return;
        }
    }
}

/*
    Remove all Routes from RawRoutingTable and then update MainRoutingTable too(i.e. clear it)
*/
void RoutingTable::clearRoutingTables()
{
    if(m_headOfRawTable->next == 0)
        return;
    m_iterator = m_headOfRawTable->next;
    Route * l_deleteRoute = new Route;

    while(m_iterator->next != 0)
    {
        l_deleteRoute = m_iterator;
        m_iterator = m_iterator->next;
        removeFromRawTable(l_deleteRoute->id);
    }
    l_deleteRoute = m_iterator;
    removeFromRawTable(l_deleteRoute->id);
    updateRoutingTable();
}


/*
    Delete all the routes from RawRoutingTable that have p_outputPort as output port.
    Then update MainRoutingTable if changes were made
*/
void RoutingTable::deleteRoutes(int p_outputPort)
{
    m_iterator = m_headOfRawTable;
    bool routesDeleted = false;
    while(m_iterator->next != 0)
    {
        m_iterator = m_iterator->next;
        if(m_iterator->OutputPort == p_outputPort)
        {
            // Same output port so delete the route from RawTable and send UPDATE-withdraw message to all peers
            sendWithdraw(*m_iterator);

            removeFromRawTable(m_iterator->id);
            routesDeleted = true;
        }
    }
    // If RawTable was modified update MainRoutingTable
    if(routesDeleted)
        updateRoutingTable();
}

// Handle withdraw-message. Remove given route from RawRoutingTable, update MainRoutingTable and forward this message
// Withdraw-message syntax: withdraw/ad,prefix,mask,AS-path (i.e. 0,100.200.50.0,16,50-123-321-999)
void RoutingTable::handleWithdraw(string p_message)
{
    // Parse message

    int pos_prefix, pos_mask, pos_ASes;
    int position = 1;

    for(int i = 0; i < 3;i++)
    {
        position = p_message.find(",",position+1);

        if(i==0)
            pos_prefix = position;
        else if(i==1)
            pos_mask = position;
        else if(i==2)
            pos_ASes = position;
    }
    string l_prefix = p_message.substr(2,pos_prefix-2);
    string l_mask = p_message.substr((pos_prefix+1),(pos_mask-pos_prefix-1));  // -1 to remove ","-sign
    string l_ASpath = p_message.substr((pos_mask+1),(pos_ASes-pos_mask-1));

//    cout << "|Prefix|" << l_prefix << "|Mask|" << l_mask << "|ASes|" << l_ASpath << "|end|" << endl;


    stringstream ss;
    Route * l_iterator = new Route;
    l_iterator = m_headOfRawTable;
    while(l_iterator->next != 0)
    {
        l_iterator = l_iterator->next;
        ss << l_iterator->mask;

        // Comapre if p_message and current ruote have same prefix,mask and AS-path
        if(l_iterator->prefix == l_prefix && ss.str() == l_mask && l_iterator->ASes == l_ASpath)
        {
            // Same, so remove this route from own RawRoutingTable and send advertise this to peers
            Route * removedRoute = new Route;
            removedRoute->prefix = l_iterator->prefix;
            removedRoute->mask = l_iterator->mask;
            removedRoute->ASes = "0";   // TODO add own AS to AS-path
            sendWithdraw(*removedRoute);

            // Now remove this route from RawRoutingTable
            removeFromRawTable(l_iterator->id);
        }
        ss.str("");
    }


}
// Send withdraw-message to all peers
void RoutingTable::sendWithdraw(Route p_route)
{
    stringstream ss;
    p_route.ASes = "";

    // Construct the message here

    // This is withdraw-message so it begins with "0"
    string l_message = "0,";
    l_message.append(p_route.prefix);
    l_message.append(",");
    ss << p_route.mask;
    l_message.append(ss.str());
    l_message.append(",");
    // TODO add peer router and own router

    ss.str("");
    ss << m_RTConfig->getASNumber();
    l_message.append(ss.str());


    // Create new BGPMessage
    BGPMessage * l_msg = new BGPMessage;
    l_msg->m_Type = UPDATE;
    l_msg->m_Message = l_message;
}

void RoutingTable::handleNotification(BGPMessage p_msg)
{
}

/*
    Find Route object from RoutingTable by given IPAddress.
    Iterate through the RoutingTable and find the longest match with the given IPAddress.
    Then return pointer to the Route object that had the longest match
*/
Route * RoutingTable::findRoute(string p_prefix)
{
    Route * l_route = new Route();
    int l_longestMatch = 0;
    int l_matchLength;
    m_iterator = m_headOfRoutingTable;

    while(m_iterator->next != 0)
    {
        m_iterator = m_iterator->next;
        l_matchLength = matchLength(m_iterator, p_prefix);
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
int RoutingTable::matchLength(Route * p_route, string p_IP)
{
    int matchLength = 0;
    string routePrefix = p_route->prefix;
    for(unsigned i = 0; i < p_IP.size();i++)
    {

        if(p_IP.at(i) == routePrefix.at(i))
            matchLength++;
        else
            break;
    }
    return matchLength;

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

// Add new AS to the preferred ASes vector
void RoutingTable::setLocalPreference(int p_AS, int p_preferenceValue)
{
    preferredASes.push_back(p_AS);
    preferredASes.push_back(p_preferenceValue);
}

void RoutingTable::removeLocalPref(int p_AS)
{
    // Find given AS
    for(unsigned i = 0; i < preferredASes.size();i++)
    {
        if(p_AS == preferredASes.at(i))
        {
            // AS found, remove that and its preference value
            preferredASes.erase(preferredASes.begin() + i);
            preferredASes.erase(preferredASes.begin()+ i+1);
        }

    }
}


/*
    only for testing?
*/
int RoutingTable::tableLength()
{
    int length = 0;
    Route * l_iterator = new Route;
    l_iterator = m_headOfRoutingTable;
    while(l_iterator->next!=0)
    {
        length++;
        l_iterator = l_iterator->next;
    }
    return length;
}

// IIRO for testing. FIll routing table and add some preferred ASes
void RoutingTable::fillRoutingTable()
{
    // ADD some preferred ASes to preferredASes



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

    for(int i=0;i<4;i++)
    {
        ss.str("");
        prefix1 = 1+(rand()%255);
        prefix2 = 1+(rand()%255);
        prefix3 = 1+(rand()%255);
        prefix4 = 0;//1+(rand()%255);
        ss << prefix1 << "." << prefix2 << "." << prefix3 << "." << prefix4;
        prefix = ss.str();
        ss.str("");

        AS1 = 1+(rand()%5000);
        AS2 = 1+(rand()%5000);
        AS3 = 1+(rand()%5000);
        ss << AS1 << "-" << AS2 << "-" << AS3;
        ASes = ss.str();
        ss.str("");

        mask = 1+(rand()%8);
        OutputPort = 1+(rand()%20);


        if(i==1)
        {
            prefix = "50.40.200.0";
            ASes = "50-70-100";
            mask = 2;
            OutputPort = 10;
        }

        ss << prefix << "," << mask << "," << ASes << "," << "9" << "," << "5555";
        l_message = ss.str();

        addRouteToRawTable(l_message,OutputPort);
        addRouteToRawTable(l_message,OutputPort);

    }

}












