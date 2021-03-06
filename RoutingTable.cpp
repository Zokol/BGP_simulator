

/*! \file RoutingTable.cpp
 *  \brief     Implementation of RoutingTable.
 *  \details
 *  \author    Antti Siiril�, 501449
 *  \version   1.0
 *  \date      Tue Feb 19 08:44:06 2013
 */


#include "RoutingTable.hpp"
#include "ReportGlobals.hpp"
#include <algorithm>




RoutingTable::RoutingTable(sc_module_name p_ModName, ControlPlaneConfig * const p_RTConfig):sc_module(p_ModName), m_RTConfig(p_RTConfig), limit(300, SC_SEC), m_Reporter(""), m_AS("")
{

	setUp(true);

	m_UpdateOut.m_AS = m_RTConfig->getASNumber();
	m_UpdateOut.m_BGPIdentifier = m_RTConfig->getBGPIdentifier();
	m_UpdateOut.m_HoldDownTime = m_RTConfig->getHoldDownTime();

	m_Reporter.setBaseName(name());
    //buffer's input

	m_AS = m_RTConfig->getASNumberAsString();
    SC_THREAD(routingTableMain);
    sensitive << port_Clk.pos();
    SC_REPORT_INFO(g_DebugRTID, m_Reporter.newReportString("Elaborated"));
    cout << name() << ": my AS: " << m_AS << endl;
}

RoutingTable::~RoutingTable()
{

}


void RoutingTable::routingTableMain(void)
{
    m_headOfRawTable = new struct_Route();
    m_endOfRawTable = new struct_Route();
    m_iterator = new struct_Route();

 //   l_LocalRoute->next = 0;
    m_headOfRoutingTable = new struct_Route();
    m_endOfRoutingTable = new struct_Route();

    addLocalRoute();


    StringTools *l_Report = new StringTools(name());

    //debugging
    SC_REPORT_INFO(g_ReportID, l_Report->newReportString("starting") );

    //fillRoutingTable(); // IIRO testing

    // Initialize m_sessions
    for(int i = 0; i < m_RTConfig->getNumberOfInterfaces()-1;i++)
        m_sessions.push_back(0);



    //The main thread of routing table module starts
    while(true)
        {
            wait();

    		if(!isRunning())
    			continue;

            /*
            Check if some of the sessions has gone up or down
            session up - was it up before?
                Case 1. yes -> do nothing
                Case 2. no -> send own RT to this new lovely peer
            session down - was it down before?
                Case 3. yes -> do nothing
                Case 4. no -> remove from RTs and send withdraws
            */

            // First check is there new sessions and add them if so
            if((unsigned)m_RTConfig->getNumberOfInterfaces()-1 > m_sessions.size())
            {
                int numberOfNewSessions = m_RTConfig->getNumberOfInterfaces() - 1 - m_sessions.size();
                for(int i = 0; i < numberOfNewSessions; i++)
                    m_sessions.push_back(0);
            }
            for (int i = 0; i < m_RTConfig->getNumberOfInterfaces()-1; i++)
                {
                   if(port_Session[i]->isSessionValid())
                    {
                        if(m_sessions.at(i) == 1)   // Case 1
                            continue;
                        else if(m_sessions.at(i) == 0) // Case 2
                            {
                                // Change this session's state in m_sessions
                                m_sessions.at(i) = 1;
                                // Send own RT to peer
                                advertiseRawRoutingTable(i);



                            }
                    }
                    else if(!(port_Session[i]->isSessionValid()))
                    {
                        if(m_sessions.at(i) == 0)   // Case 3
                            continue;
                        else if(m_sessions.at(i) == 1) // Case 4
                        {
                            m_sessions.at(i) = 0;
                            // Iterate through the RawTable and send withdraw message to peers if route's outputport is i
                            m_iterator = m_headOfRawTable;
                            while(m_iterator->next != 0)
                            {
                                m_iterator= m_iterator->next;
                              if(m_iterator->OutputPort == i)
                                {
                                    // Clear m_iterator->ASpath for sendWithdraw.
                                    m_iterator->ASes = "";
                                    for (int j = 0; j < m_RTConfig->getNumberOfInterfaces()-1; j++) {
                                    	if(j != i)
                                    		sendWithdraw(*m_iterator,j);

									}
                                }
                            }
                            // Remove all the router from RawRoutingTable where outputport is i.
                            deleteRoutes(i);

                        }

                    }

                }

            if(m_ReceivingBuffer.num_available() > 0)
            {
//            	cout << name() << " receiving buffer has: " << m_ReceivingBuffer.num_available() << endl;
            	m_ReceivingBuffer.read(m_BGPMsg);
            	if(m_PreviousInput == m_BGPMsg)
            		m_NewInputMsg = false;
            	else
            		m_NewInputMsg = true;
            }
            else
            	m_NewInputMsg = false;

            ///BGP notification and update output port
            // port_Output->write(m_BGPMsg);


            if((m_BGPMsg.m_Type = UPDATE) && m_NewInputMsg)
            {

               // Read the first integer of the m_BGPMsg.m_Message. That indicates if this UPDATE-message is an advertise or a withdraw

                if(m_BGPMsg.m_Message.substr(0,1) == "0")
                {
                    // First char was 0, so this is a withdraw-message
                    handleWithdraw(m_BGPMsg.m_Message);
                }
                else if(m_BGPMsg.m_Message.substr(0,1) == "1")
                {

                    // This is an advertise-message. Add it to own RawTable, add own AS in AS-path and then forward the message to peers
                    //      antti oti pois kun buffaa muistia.

                	if(!addRouteToRawTable(m_BGPMsg.m_Message,m_BGPMsg.m_OutboundInterface))
                	{
cout << "Oma as: " << m_AS << "  ei lisätty raw tableen: " << m_BGPMsg.m_Message << endl;
printRawRoutingTable();
                		continue;
                	}
                    //      antti oti pois kun buffaa muistia.
                    for (int k = 0; k < m_RTConfig->getNumberOfInterfaces()-1; k++) {
                    	if(m_BGPMsg.m_OutboundInterface != k && port_Session[k]->isSessionValid())
                    	{
//                    		cout << name() << " advertises to the peer in interface: " << k << endl;
                    		advertiseRoute(m_endOfRoutingTable, k);

                    	}
					}
               }
                else
                {

                }
            }
            else if(m_BGPMsg.m_Type == NOTIFICATION && m_NewInputMsg)
            {
                cout << "In notification handling" << endl;
                handleNotification(m_BGPMsg);
            }


                updateRoutingTable();
                /*
                cout << "Raw table: " << endl;
                printRawRoutingTable();

                cout << "Main table: " << endl;
                printRoutingTable();


                */


//            if(sc_time_stamp() == limit)
//            {
//            	printRoutingTable();
//
//            }
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
    struct_Route * l_iteratorRaw = m_headOfRawTable;

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
            // struct_Route didin't exist in RawRT so remove it from MainTable
            removeFromRoutingTable(m_iterator->id);
        }
    }



}

/*
    Compare two routes and return true if they are the same
*/
bool RoutingTable::sameRoutes(struct_Route p_route1, struct_Route p_route2)
{
    if(p_route1.prefix == p_route2.prefix && p_route1.mask == p_route2.mask)
        return true;
    else
        return false;
}

// Return the AS Path length
int RoutingTable::ASpathLength(struct_Route p_route)
{
    int ASCount = count(p_route.ASes.begin(), p_route.ASes.end(), '-') + 1;
    return ASCount;
}

/*
    Add new route to MainRoutingTable
*/
void RoutingTable::setRoute(struct_Route p_route)
{
    struct_Route * newRoute = new struct_Route();

    newRoute->id = (m_endOfRoutingTable->id)+1;
    newRoute->prefix = p_route.prefix;
    newRoute->mask = p_route.mask;
    newRoute->OutputPort = p_route.OutputPort;
    newRoute->ASes = p_route.ASes;


    if(m_headOfRoutingTable->next == 0)
    {
        // RoutingTable is empty. So add the first struct_Route in it
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
void RoutingTable::addPreferredRoute(struct_Route p_route1, struct_Route p_route2)
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

    // 1. Iterate through preferredASes and check if other struct_Route has preferred AS on its ASpath
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
string RoutingTable::routingTableToString(struct_Route * p_route)
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
string RoutingTable::routeToString(struct_Route p_route)
{
    stringstream ss;
    ss.str("");
    ss << p_route.id  << "," << p_route.prefix << "," << p_route.mask << "," << p_route.ASes << "," << p_route.OutputPort;
    return ss.str();
}


// Print the RoutingTable
void RoutingTable::printRoutingTable()
{
	cout << name() << endl;

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


void RoutingTable::printOneRoute(struct_Route p_route)
{
    stringstream ss;
    ss << "Id:" << p_route.id <<" Prefix: " <<  p_route.prefix << " Mask: " <<  p_route.mask << " Output port: " << p_route.OutputPort << " ASes: " << p_route.ASes;
    string message = ss.str();
    cout << message << endl;
}

/*
    Add new route to RawRoutingTable
    TODO: check why this is being called continuesly
*/
bool RoutingTable::addRouteToRawTable(string p_msg,int OutputPort)
{

    struct_Route * newRoute = new struct_Route();

    // Get ID for this new route
    newRoute->id = (m_endOfRawTable->id)+1;

    // Set data to newRoute in CreateRoute(...)
   if(!createRoute(p_msg,OutputPort,newRoute))
   {
	   m_Reporter.newReportString(" Route was not add to raw table: ");
		SC_REPORT_INFO(g_DebugRTID, m_Reporter.appendReportString(p_msg));
cout << "ei lisätty reittiä" << endl;
	   return false;
   }
	   // Add new struct_Route object to the RoutingTable

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
       // RoutinTable wasn't empty. Add new struct_Route object in it. Add it to the end.
        m_endOfRawTable->next = newRoute;
        m_endOfRawTable = newRoute;
        newRoute->next = 0;
    }

   // SC_REPORT_INFO(g_DebugRTID, m_Reporter.newReportString("Routing table update after a route is added into the raw table"));
    updateRoutingTable();
    return true;
}

/*
    Create a struct_Route object from p_msg. p_msg must be constructed as follows:
    0 or 1,IP,Mask,ASes(e.g. 1,10.255.0.100,8,550-7564-4)
    Parse message and collect IP,Mask,ASes which are separated by ","-mark
*/
bool RoutingTable::createRoute(string p_msg,int p_outputPort ,struct_Route * p_route)
{
    // Use these to collect data separetad by semicolon
    int position = 2;
    unsigned IP_end,Mask_end, ASes_end;

    for(int i=0;i<3;i++)
    {
        position = p_msg.find(",",position+1);
        if(i==0)
            IP_end = position;
        else if(i==1)
            Mask_end = position;
        else if(i==2)
            ASes_end = position;
    }

    string IPAddress = p_msg.substr(2,IP_end-2);
    string Mask = p_msg.substr((IP_end+1),(Mask_end-IP_end-1));  // -1 to remove ";"-sign
    string ASes = p_msg.substr((Mask_end+1),(ASes_end-Mask_end-1));

//cout << "IPAddress: " << IPAddress << "mask: " << Mask << " ASes: " << ASes << endl;
	string l_AS;
    if(ASes.compare(AS_EMPTY) == 0)//local route is to be created
    {
    	if(p_outputPort == m_RTConfig->getNumberOfInterfaces()-1)
    	{
    		ASes = m_RTConfig->getASNumberAsString();
    		SC_REPORT_INFO(g_DebugRTID, m_Reporter.newReportString(" Local route was created"));
   	}
    	else
    	{
    	    SC_REPORT_WARNING(g_DebugRTID, m_Reporter.newReportString(" Wrong output port for local route"));
    	    return false;
    	}
    }
    else if(ASes.find("-") == string::npos) // route that is advertised an adjacent router
    {

    	if(ASes.compare(m_AS) == 0) //create route only if it is not from this router
    	{
    		cout << "AS found from the path" << endl;
    		m_Reporter.newReportString(" Local route advertisement received, advertised AS: ");
    		m_Reporter.appendReportString(ASes);
    		m_Reporter.appendReportString(" Local AS: ");
    		SC_REPORT_WARNING(g_DebugRTID, m_Reporter.appendReportString(m_AS));
    	    return false;
    	}
    	else
    	{
        	l_AS = m_AS + "-";
    		// Add own AS in AS-path
    		l_AS += ASes;
    		ASes = l_AS;
    		m_Reporter.newReportString(" Adjacent AS route created, AS: ");
    	    SC_REPORT_INFO(g_DebugRTID, m_Reporter.appendReportString(ASes));

    	}
    }
    else //other
    {
    	position = 0;
       	ASes_end = ASes.find("-",position);
   	//Check if own AS exist in the path
    	while(ASes_end != string::npos)
    	{
    		l_AS = ASes.substr(position, ASes_end-position);
        	//if own as was found, just return false
    		if(l_AS.compare(m_AS) == 0)
    		{
    			cout << "AS PATH: " << ASes << " My path: " << m_AS << endl;
//            	cout << " as from the message: " << ASes<< endl;
            	return false;
    		}
    		position = ASes_end + 1;
        	ASes_end = ASes.find("-",position);

    	}
    	l_AS = m_AS + "-";
		// Add own AS in AS-path
		l_AS += ASes;
		ASes = l_AS;
		m_Reporter.newReportString(" More than one hop route created, AS: ");
	    SC_REPORT_INFO(g_DebugRTID, m_Reporter.appendReportString(ASes));

    }
 //=======
//
//    stringstream ss;
//    // Add own AS in AS-path
//    if(ASes.size() == 0)
//    {
//        // Add own as without "-" in the beginning since this is the first AS-number
//        ss << m_RTConfig->getASNumber();
//        ASes.append(ss.str());
//
//    }
//    else
//    {
//        ASes.append("-");
//        ss << m_RTConfig->getASNumber();
//        ASes.append(ss.str());
//    }
//
//>>>>>>> 4ee3ed4b07096339af037a9f7c39dbd08e896861
//cout << name() << ": struct_Route added: " << ASes << " @ " << sc_time_stamp() << endl;
    // Set the values to struct_Route pointer
    p_route->prefix = IPAddress;
    p_route->mask = atoi(Mask.c_str());
    p_route->ASes = ASes;
    p_route->OutputPort = p_outputPort;
    return true;
}

/*
    Remove given route from RawRoutingTable
*/
void RoutingTable::removeFromRawTable(int p_routeId)
{
    struct_Route * deleteRoute;
    struct_Route * tempRoute;
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
            delete deleteRoute;
            return;
        }
    }
}

/*
    Remove given route from MainRoutingTable
*/
void RoutingTable::removeFromRoutingTable(int p_routeId)
{
    struct_Route * deleteRoute = new struct_Route();
    struct_Route * tempRoute = new struct_Route();
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
    struct_Route * l_deleteRoute;

    while(m_iterator->next != 0)
    {
        l_deleteRoute = m_iterator;
        m_iterator = m_iterator->next;
        removeFromRawTable(l_deleteRoute->id);
    }
    l_deleteRoute = m_iterator;
    removeFromRawTable(l_deleteRoute->id);
	SC_REPORT_INFO(g_DebugRTID,m_Reporter.newReportString("Routing table is being updated after clearRoutingTables was called"));
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

            for (int i = 0; i < m_RTConfig->getNumberOfInterfaces()-1; i++) {

            	if(p_outputPort != i)
            		sendWithdraw(*m_iterator, i);
            }


            removeFromRawTable(m_iterator->id);
            routesDeleted = true;
        }
    }
    // If RawTable was modified update MainRoutingTable
    if(routesDeleted)
    {
    	SC_REPORT_INFO(g_DebugRTID,m_Reporter.newReportString("Routing table is being updated after delete routes was called"));
        updateRoutingTable();
    }
}

// Handle withdraw-message. Remove given route from RawRoutingTable, update MainRoutingTable and forward this message
// Withdraw-message syntax: withdraw/ad,prefix,mask,AS-path (i.e. 0,100.200.50.0,16,50-123-321-999)
void RoutingTable::handleWithdraw(string p_message)
{
    /*  First check that own AS is not in AS-path.
        If own AS is found from AS-path --> do nothing since the package has made a circle and this router has already handled this packege
    */
    stringstream ss;


    if(p_message.find(m_AS) == string::npos)
    {
        // Own AS number was found from AS-path so exit from this method
        return;
    }

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

//   cout << "|Prefix|" << l_prefix << "|Mask|" << l_mask << "|ASes|" << l_ASpath << "|end|" << endl;


    ss.str("");
    struct_Route * l_iterator;
    struct_Route * removedRoute = new struct_Route;
    l_iterator = m_headOfRawTable;
    while(l_iterator->next != 0)
    {
        l_iterator = l_iterator->next;
        ss << l_iterator->mask;

        // Comapre if p_message and current ruote have same prefix,mask and AS-path
        if(l_iterator->prefix == l_prefix && ss.str() == l_mask && l_iterator->ASes == l_ASpath)
        {
            // Same, so remove this route from own RawRoutingTable and send advertise to peers

            removedRoute->prefix = l_iterator->prefix;
            removedRoute->mask = l_iterator->mask;

            l_ASpath.append("-");
            ss.str("");
            // Add own AS-number to AS-path
            ss << m_RTConfig->getASNumber();
            l_ASpath.append(ss.str());
            removedRoute->ASes = l_ASpath;

            for (int i = 0; i < m_RTConfig->getNumberOfInterfaces()-1; i++) {

            	if(removedRoute->OutputPort != i)
            		sendWithdraw(*removedRoute, i);
            }
            // Now remove this route from RawRoutingTable
            removeFromRawTable(l_iterator->id);
        }
        ss.str("");
    }

    delete removedRoute;
}
/*
    Send withdraw-message to all peers. If this message is created in this router write peer's and own AS-number to p_route.ASes.
    Otherwise p_route.ASes should already include this router's AS, since it's added in handleWithdraw()
*/
void RoutingTable::sendWithdraw(struct_Route p_route, int p_OutputPort)
{
	//return if this the session that the withdraw is to be sent is down
    if(!port_Session[p_OutputPort]->isSessionValid())
    	return;

    stringstream ss;
//    if(p_route.ASes.size() == 0)
//    {
//        // This withdraw message is created in this router, so get peer AS-number and own AS-number
//        string peerAS = port_Session[p_route.OutputPort]->getPeerAS();
//        int ownAS = m_RTConfig->getASNumber();
//        ss << peerAS << "-" << ownAS;
//        p_route.ASes = ss.str();
//        ss.str("");
//
//    }
    // Construct the message here

    // This is withdraw-message so it begins with "0"
    string l_message = "0,";
    l_message.append(p_route.prefix);
    l_message.append(",");
    ss << p_route.mask;
    l_message.append(ss.str());
    l_message.append(",");


    m_UpdateOut.m_Type = UPDATE;
    m_UpdateOut.m_Message = l_message;
    m_UpdateOut.m_OutboundInterface = p_OutputPort;

    // Send the message

    port_Output->write(m_UpdateOut);
}

void RoutingTable::handleNotification(BGPMessage p_msg)
{
}

/*
    Find struct_Route object from RoutingTable by given IPAddress.
    Iterate through the RoutingTable and find the longest match with the given IPAddress.
    Then return pointer to the struct_Route object that had the longest match
*/
struct_Route * RoutingTable::findRoute(string p_prefix)
{
    struct_Route * l_route = new struct_Route();
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
int RoutingTable::matchLength(struct_Route * p_route, string p_IP)
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
	if(!isRunning())
		return -1;

    SC_REPORT_INFO(g_DebugID, StringTools(name()).appendReportString("resolveRoute-method was called.") );
    struct_Route * foundRoute = findRoute(p_IPAddress);



    if(foundRoute->OutputPort < m_RTConfig->getNumberOfInterfaces() && foundRoute->OutputPort >= 0)
    	return foundRoute->OutputPort;
    else
    	return -1;
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
            return;
        }

    }
}
void RoutingTable::addLocalRoute()
{
    //add local AS route in the routing table
    struct_Route *l_LocalRoute = new struct_Route();
    l_LocalRoute->id = 1;
    l_LocalRoute->prefix = m_RTConfig->getIPAsString();
    l_LocalRoute->mask = StringTools().sToI(m_RTConfig->getIPMaskAsString());
    l_LocalRoute->ASes = AS_EMPTY;
    l_LocalRoute->OutputPort = m_RTConfig->getNumberOfInterfaces()-1;

    stringstream ss;
    string routeAsString = "1,";  // It's advertisement, not withdraw, so string begins by "1"
    routeAsString.append(l_LocalRoute->prefix);
    routeAsString.append(",");
    ss << l_LocalRoute->mask;
    routeAsString.append(ss.str());
    routeAsString.append(",");
    routeAsString.append(l_LocalRoute->ASes);
    addRouteToRawTable(routeAsString, l_LocalRoute->OutputPort);

}

// Advertise this route to peers

void RoutingTable::advertiseRoute(struct_Route * p_route, int p_Outputport)
{
    stringstream ss;
    string routeAsString = "1,";  // It's advertisement, not withdraw, so string begins by "1"
    routeAsString.append(p_route->prefix);
    routeAsString.append(",");
    ss << p_route->mask;
    routeAsString.append(ss.str());
    routeAsString.append(",");
    routeAsString.append(p_route->ASes);
    cout << name() << " advertising route: " << routeAsString << " @ " << sc_time_stamp() << endl;
//    routeAsString.append("-");
//    routeAsString.append(m_RTConfig->getASNumberAsString());

    m_UpdateOut.m_Message = routeAsString;
    m_UpdateOut.m_Type = UPDATE;

    m_UpdateOut.m_OutboundInterface = p_Outputport;
    //do not advertise to the local as port
    if(m_UpdateOut.m_OutboundInterface != m_RTConfig->getNumberOfInterfaces()-1)
    {
    	if(m_Previous == m_UpdateOut)
    		cout << "Same message was send" << endl;
    	port_Output->write(m_UpdateOut);
    }
m_Previous = m_UpdateOut;

}

// Advertise the whole table to given peer
void RoutingTable::advertiseRawRoutingTable(int p_outputPort)
{
    struct_Route * l_route;
    l_route = m_headOfRoutingTable;
    while(l_route->next != 0)
    {
        l_route = l_route->next;

    	if(l_route->OutputPort != p_outputPort)
    	{
    		advertiseRoute(l_route, p_outputPort);
    	}

    }
}


/*
    only for testing?
*/
int RoutingTable::tableLength()
{
    int length = 0;
    struct_Route * l_iterator = new struct_Route;
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
        int withdrawOrAd = 0;

        ss << withdrawOrAd << "," << prefix << "," << mask << "," << ASes;
        l_message = ss.str();

        addRouteToRawTable(l_message,OutputPort);
        addRouteToRawTable(l_message,OutputPort);

    }

}

bool RoutingTable::write(BGPMessage& p_BGPMsg)
{
	m_ReceivingBufferMutex.lock();
	if(isRunning())
		m_ReceivingBuffer.write(p_BGPMsg);
	m_ReceivingBufferMutex.unlock();
	return true;
}


void RoutingTable::killRoutingTable(void)
{
	setUp(false);
	clearRoutingTables();
	while(m_ReceivingBuffer.num_available() > 0)
		m_ReceivingBuffer.read(m_BGPMsg);

	m_BGPMsg.clearMessage();


}

void RoutingTable::reviveRoutingTable(void)
{
	addLocalRoute();
	setUp(true);
}

void RoutingTable::setUp(bool p_Value)
{
	m_UpMutex.lock();
	m_Up = p_Value;
	m_UpMutex.unlock();
}

bool RoutingTable::isRunning(void)
{
	bool currentV;
	m_UpMutex.lock();
	currentV = m_Up;
	m_UpMutex.unlock();
	return currentV;

}


