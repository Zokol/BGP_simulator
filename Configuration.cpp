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
    m_KeepaliveTime = p_KeepaliveTime;
    setHoldDownTime();
}

void BGPSessionParameters::setHoldDownTimeFactor(int p_HoldDownTimeFactor)
{
    m_HoldDownTimeFactor = p_HoldDownTimeFactor;
    setHoldDownTime();
}

int BGPSessionParameters::getKeepaliveTime(void){return m_KeepaliveTime;}

int BGPSessionParameters::getHoldDownTime(void){return m_HoldDownTime;}

int BGPSessionParameters::getHolDownTimeFactor(void){return m_HoldDownTimeFactor;}


BGPSessionParameters& BGPSessionParameters::operator = (const BGPSessionParameters& p_Original) {
    m_KeepaliveTime = p_Original.m_KeepaliveTime;
    m_HoldDownTimeFactor = p_Original.m_HoldDownTimeFactor;
    return *this;
}


void BGPSessionParameters::setHoldDownTime(void)
{

    m_HoldDownTime = m_KeepaliveTime * m_HoldDownTimeFactor;
}

    
/************* Implementation of ControlPlaneConfig *****************/

///Setters

void ControlPlaneConfig::setNumberOfInterfaces(int p_NumberOfInterfaces)
{
    m_NumberOfInterfaces = p_NumberOfInterfaces;
}

void ControlPlaneConfig::setPrefix(sc_int<32> p_Prefix)
{
    m_Prefix = p_Prefix;
}

void ControlPlaneConfig::setPrefixMask(sc_int<32> p_PrefixMask)
{
    m_PrefixMask = p_PrefixMask;
}

void ControlPlaneConfig::setASNumber(int p_ASNumber)
{
    m_ASNumber = p_ASNumber;
}

void ControlPlaneConfig::setMED(int p_MED)
{
    m_MED = p_MED;
}

void ControlPlaneConfig::setLocalPref(int p_LocalPref)
{
    m_LocalPref = p_LocalPref;
}

///Getters
int ControlPlaneConfig::getNumberOfInterfaces(void){return m_NumberOfInterfaces;}

sc_int<32> ControlPlaneConfig::getPrefix(void){return m_Prefix;}

sc_int<32> ControlPlaneConfig::getPrefixMask(void){return m_PrefixMask;}

int ControlPlaneConfig::getASNumber(void){return m_ASNumber;}

int ControlPlaneConfig::getMED(void){return m_MED;}

int ControlPlaneConfig::getLocalPref(void){return m_LocalPref;}


ControlPlaneConfig& ControlPlaneConfig::operator = (const ControlPlaneConfig& p_Original) {
    m_NumberOfInterfaces = p_Original.m_NumberOfInterfaces;
    m_Prefix = p_Original.m_Prefix;
    m_PrefixMask = p_Original.m_PrefixMask;
    m_ASNumber = p_Original.m_ASNumber;
    m_MED = p_Original.m_MED;
    m_LocalPref = p_Original.m_LocalPref;
    m_KeepaliveTime = p_Original.m_KeepaliveTime;
    m_HoldDownTimeFactor = p_Original.m_HoldDownTimeFactor;
    return *this;
}


/************* Implementation of Connection *****************/




/************* Implementation of RouterConfig *****************/

///Constructors and Destructor

RouterConfig::RouterConfig(int p_NumberOfInterfaces)
    {
        m_NeighborConnections = new Connection*[p_NumberOfInterfaces];
        
        setNumberOfInterfaces(p_NumberOfInterfaces);

    }

RouterConfig::~RouterConfig()
{
    delete [] m_NeighborConnections;
}

///Setters

void RouterConfig::addConnectionConfig( int p_LocalInterfaceId, int p_NeighborRouterId, int p_NeighborInterfaceId)
    {
        m_NeighborConnections[p_LocalInterfaceId] = new Connection;
        m_NeighborConnections[p_LocalInterfaceId]->m_NeighborRouterId = p_NeighborRouterId;
        m_NeighborConnections[p_LocalInterfaceId]->m_NeighborInterfaceId = p_NeighborInterfaceId;


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


/************* Implementation of SimulationConfig *****************/



SimulationConfig::SimulationConfig(int p_NumberOfRouters):m_NumberOfRouters(p_NumberOfRouters)
    {
        m_RouterConfiguration = new RouterConfig*[p_NumberOfRouters];

    }

SimulationConfig::~SimulationConfig()
    {

        delete [] m_RouterConfiguration;
    }

void SimulationConfig::addRouterConfig(int p_RouterId, int p_NumberOfInterfaces)
    {
        m_RouterConfiguration[p_RouterId] = new RouterConfig(p_NumberOfInterfaces);

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
