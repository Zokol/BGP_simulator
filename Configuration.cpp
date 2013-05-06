#include "Configuration.hpp"


/************* Implementation of BGPSessionParameters *****************/
BGPSessionParameters::BGPSessionParameters():m_KeepaliveTime(60), m_HoldDownTimeFactor(3)
{
    setHoldDownTime();
}

BGPSessionParameters::BGPSessionParameters(int p_KeepaliveTime, int p_HoldDownTimeFactor):m_KeepaliveTime(p_KeepaliveTime), m_HoldDownTimeFactor(p_HoldDownTimeFactor)
{
    setHoldDownTime();
}

BGPSessionParameters::~BGPSessionParameters(){}

    
void BGPSessionParameters::setKeepaliveTime(int p_KeepaliveTime)
{
	mtx_Keepalive.lock();
    m_KeepaliveTime = p_KeepaliveTime;
    setHoldDownTime();
	mtx_Keepalive.unlock();
}

void BGPSessionParameters::setHoldDownTimeFactor(int p_HoldDownTimeFactor)
{
	mtx_HoldDownFactor.lock();
    m_HoldDownTimeFactor = p_HoldDownTimeFactor;
    setHoldDownTime();
	mtx_HoldDownFactor.unlock();
}

int BGPSessionParameters::getKeepaliveTime(void){return m_KeepaliveTime;}

int BGPSessionParameters::getHoldDownTime(void){return m_HoldDownTime;}

int BGPSessionParameters::getHolDownTimeFactor(void){return m_HoldDownTimeFactor;}


BGPSessionParameters& BGPSessionParameters::operator = (const BGPSessionParameters& p_Original) {
    m_KeepaliveTime = p_Original.m_KeepaliveTime;
    m_HoldDownTimeFactor = p_Original.m_HoldDownTimeFactor;
    m_Prefix = p_Original.m_Prefix;
    m_PrefixMask = p_Original.m_PrefixMask;
    m_ASNumber = p_Original.m_ASNumber;
    m_MED = p_Original.m_MED;
    m_LocalPref = p_Original.m_LocalPref;
    m_KeepaliveTime = p_Original.m_KeepaliveTime;
    m_HoldDownTimeFactor = p_Original.m_HoldDownTimeFactor;
    return *this;
}


void BGPSessionParameters::setHoldDownTime(void)
{

    m_HoldDownTime = m_KeepaliveTime * m_HoldDownTimeFactor;
}

void BGPSessionParameters::setNICMode(int p_Interface, int p_Mode)
{
	m_NICMode[p_Interface] = p_Mode;
}

bool BGPSessionParameters::isClient(int p_Interface)
{
	return m_NICMode[p_Interface] == CLIENT?true:false;
}


void BGPSessionParameters::setPrefix(string p_Prefix)
{
    m_Prefix = m_IPConverter.convertIPToBinary(p_Prefix);
    m_PrefixMask = m_IPConverter.convertMaskToBinary(p_Prefix);
}

void BGPSessionParameters::setPrefix(sc_uint<32> p_Prefix)
{
    m_Prefix = p_Prefix;
}

void BGPSessionParameters::setPrefixMask(sc_uint<32> p_PrefixMask)
{
    m_PrefixMask = p_PrefixMask;
}

void BGPSessionParameters::setASNumber(int p_ASNumber)
{
    m_ASNumber = p_ASNumber;
}

void BGPSessionParameters::setMED(int p_MED)
{
    m_MED = p_MED;
}

void BGPSessionParameters::setLocalPref(int p_LocalPref)
{
    m_LocalPref = p_LocalPref;
}


string BGPSessionParameters::getIPAsString(void){return m_IPConverter.convertIPToString(m_Prefix);}

string BGPSessionParameters::getIPMaskAsString(void){return m_IPConverter.convertMaskToString(m_PrefixMask);}

sc_uint<32> BGPSessionParameters::getPrefix(void){return m_Prefix;}

sc_uint<32> BGPSessionParameters::getPrefixMask(void){return m_PrefixMask;}

int BGPSessionParameters::getASNumber(void){return m_ASNumber;}

int BGPSessionParameters::getMED(void){return m_MED;}

int BGPSessionParameters::getLocalPref(void){return m_LocalPref;}



/************* Implementation of ControlPlaneConfig *****************/

///Setters

void ControlPlaneConfig::setNumberOfInterfaces(int p_NumberOfInterfaces)
{
    m_NumberOfInterfaces = p_NumberOfInterfaces;
}
///Getters
int ControlPlaneConfig::getNumberOfInterfaces(void){return m_NumberOfInterfaces;}


ControlPlaneConfig& ControlPlaneConfig::operator = (const ControlPlaneConfig& p_Original) {
    m_NumberOfInterfaces = p_Original.m_NumberOfInterfaces;
    return *this;
}


/************* Implementation of Connection *****************/

Connection::Connection():m_NeighborInterfaceId(-1), m_NeighborRouterId(-1){}


Connection::Connection(int p_NeighborInterfaceId, int p_NeighborRouterId):m_NeighborInterfaceId(p_NeighborInterfaceId), m_NeighborRouterId(p_NeighborRouterId)
{



}

void Connection::setNeighborRouterId(int p_NeighborRouterId)
{
	m_NeighborRouterId = p_NeighborRouterId;
}

void Connection::setNeighborInterfaceId(int p_NeighborInterfaceId)
{
	m_NeighborInterfaceId = p_NeighborInterfaceId;
}

int Connection::getNeighborRouterId(void)
{
	return m_NeighborRouterId;
}

int Connection::getNeighborInterfaceId(void)
{
	return m_NeighborInterfaceId;
}
bool Connection::hasConnection(void)
{
	return (m_NeighborRouterId < 0 || m_NeighborInterfaceId < 0)?false:true;
}

string Connection::toString(void)
{
	StringTools cvr;
	return "Neighbor router ID: " + cvr.iToS(m_NeighborRouterId) + "\nNeighbor interface ID: " + cvr.iToS(m_NeighborInterfaceId) + "\n----------------------\n";
}

/************* Implementation of RouterConfig *****************/

///Constructors and Destructor

RouterConfig::RouterConfig(int p_NumberOfInterfaces)
{
	m_NeighborConnections = new Connection*[p_NumberOfInterfaces];
	m_NICMode = new int[p_NumberOfInterfaces];
	for(int i = 0; i < p_NumberOfInterfaces; i++)
	{
		m_NeighborConnections[i] = new Connection();
		m_NICMode[i] = SERVER;
	}
	setNumberOfInterfaces(p_NumberOfInterfaces);

}

RouterConfig::~RouterConfig()
{
    delete [] m_NeighborConnections;
    delete[] m_NICMode;
}

///Setters

    void RouterConfig::addConnectionConfig( int p_LocalInterfaceId, int p_NeighborInterfaceId, int p_NeighborRouterId)
    {
        // m_NeighborConnections[p_LocalInterfaceId] = new Connection(p_NeighborInterfaceId, p_NeighborRouterId);
        m_NeighborConnections[p_LocalInterfaceId]->setNeighborRouterId(p_NeighborRouterId);
        m_NeighborConnections[p_LocalInterfaceId]->setNeighborInterfaceId(p_NeighborInterfaceId);


    }    

///Getters


///Operators


RouterConfig& RouterConfig::operator = (const RouterConfig& p_Original) {
    m_NumberOfInterfaces = p_Original.m_NumberOfInterfaces;
    m_Prefix = p_Original.m_Prefix;
    m_PrefixMask = p_Original.m_PrefixMask;
    m_ASNumber = p_Original.m_ASNumber;
    m_MED = p_Original.m_MED;
    m_LocalPref = p_Original.m_LocalPref;
    m_KeepaliveTime = p_Original.m_KeepaliveTime;
    m_HoldDownTimeFactor = p_Original.m_HoldDownTimeFactor;
    m_NeighborConnections = new Connection*[p_Original.m_NumberOfInterfaces];
    for (int i = 0; i < p_Original.m_NumberOfInterfaces; ++i)
        {
            m_NeighborConnections[i] = p_Original.m_NeighborConnections[i];
        }
    return *this;
}



bool RouterConfig::isConnection(int p_InterfaceId)
{
    return m_NeighborConnections[p_InterfaceId]->hasConnection();

}


int RouterConfig::getNeighborRouterId(int p_LocalInterface)
{
    return m_NeighborConnections[p_LocalInterface]->getNeighborRouterId();
}

int RouterConfig::getNeighborInterfaceId(int p_LocalInterface)
{
    return m_NeighborConnections[p_LocalInterface]->getNeighborInterfaceId();
}


Connection *RouterConfig::getConnection(int p_ConnectionId)
{
    return m_NeighborConnections[p_ConnectionId];
}


string RouterConfig::toString(void)
{

    StringTools cvr;

    //build the connection string

    string l_ConnectionString = "";

    for(int i = 0; i < m_NumberOfInterfaces; i++)
        {

            l_ConnectionString += "\nInterface " + cvr.iToS(i) + " connection config\n-------------------------------\n" + m_NeighborConnections[i]->toString();
        }


    return "BGP Session Param\n-------------------------\nKeepalive time: " + cvr.iToS(m_KeepaliveTime) + "\nHold-Down time: " + cvr.iToS(m_HoldDownTime) + "\nHold-Down multiplier: " + cvr.iToS(m_HoldDownTimeFactor) + "\n-------------------------\nControl Plane Param\n-------------------------\nNumber of Interfaces: " + cvr.iToS(m_NumberOfInterfaces) + "\nPrefix: " + cvr.convertIPToString(m_Prefix, m_PrefixMask) + "\nAS number: " + cvr.iToS(m_ASNumber) + "\nMED: " + cvr.iToS(m_MED) + "\nLocal preference: " + cvr.iToS(m_LocalPref) + "\n-------------------------\nConnection params\n-------------------------" + l_ConnectionString;
}




/************* Implementation of SimulationConfig *****************/



SimulationConfig::SimulationConfig(int p_NumberOfRouters):m_NumberOfRouters(p_NumberOfRouters)
    {
        m_RouterConfiguration = new RouterConfig*[p_NumberOfRouters];

        cout << "size: "<< p_NumberOfRouters <<endl;
        m_HostConfig = new Connection*[p_NumberOfRouters];
    }


SimulationConfig::~SimulationConfig()
    {

        delete [] m_RouterConfiguration;
    }

void SimulationConfig::init(int p_NumberOfRouters)
{
    m_NumberOfRouters = p_NumberOfRouters;
    m_RouterConfiguration = new RouterConfig*[m_NumberOfRouters];
    m_HostConfig = new Connection*[p_NumberOfRouters];

}


void SimulationConfig::addRouterConfig(int p_RouterId, int p_NumberOfInterfaces)
    {
        m_RouterConfiguration[p_RouterId] = new RouterConfig(p_NumberOfInterfaces);

        m_RouterConfiguration[p_RouterId]->addConnectionConfig(p_NumberOfInterfaces-1, 0, 0x7FFFFFFF);
        cout << "index: "<< p_RouterId <<endl;
        m_HostConfig[p_RouterId] = new Connection(p_NumberOfInterfaces-1, p_RouterId);

    }
    
void SimulationConfig::addConnectionConfig(int p_LocalRouterId, int p_LocalInterfaceId, int p_NeighborInterfaceId, int p_NeighborRouterId)
    {

        m_RouterConfiguration[p_LocalRouterId]->addConnectionConfig(p_LocalInterfaceId, p_NeighborInterfaceId, p_NeighborRouterId);
    }

void SimulationConfig::addBGPSessionParameters(int p_LocalRouterId, int p_KeepaliveTime, int p_HoldDownTimeFactor)
    {
        m_RouterConfiguration[p_LocalRouterId]->setKeepaliveTime(p_KeepaliveTime);
        m_RouterConfiguration[p_LocalRouterId]->setHoldDownTimeFactor(p_HoldDownTimeFactor);
      
    }


void SimulationConfig::setNumberOfRouters(int p_NumberOfRouters)
{
    m_NumberOfRouters = p_NumberOfRouters;
}

int SimulationConfig::getNumberOfRouters(void){return m_NumberOfRouters;}

RouterConfig& SimulationConfig::getRouterConfiguration(int p_RouterId)
{
    return *(m_RouterConfiguration[p_RouterId]);
}

RouterConfig* SimulationConfig::getRouterConfigurationPtr(int p_RouterId)
{
    return m_RouterConfiguration[p_RouterId];
}
Connection* SimulationConfig::getHostConfigurationPtr(int p_RouterId)
{
    return m_HostConfig[p_RouterId];
}

///Operators

SimulationConfig& SimulationConfig::operator = (const SimulationConfig& p_Original) {

    m_NumberOfRouters = p_Original.m_NumberOfRouters;
    m_RouterConfiguration = new RouterConfig*[m_NumberOfRouters];
    for (int i = 0; i < m_NumberOfRouters; ++i)
        {
            m_RouterConfiguration[i] = p_Original.m_RouterConfiguration[i];
        }
    return *this;
}

string SimulationConfig::toString(void)
{

    StringTools cvr;

    //build the connection string

    string l_RouterString = "\n***********************************************";

    for(int i = 0; i < m_NumberOfRouters; i++)
        {
            if(m_RouterConfiguration[i] != NULL)
                l_RouterString += "\n***********************************************\nRouter " + cvr.iToS(i) + " config\n-------------------------------\n" + m_RouterConfiguration[i]->toString();
        }
    l_RouterString += "\n***********************************************\n";

        return "\n;;;;;\nSimulation Param\n-------------------------\nNumber of Routers: " + cvr.iToS(m_NumberOfRouters) + l_RouterString + "\n;;;;;\n";
}
