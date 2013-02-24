/*! \file DataPlane.cpp
 *  \brief     
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 12:14:35 2013
 */


#include "DataPlane.hpp"
#include "StringTools.hpp"

extern char* g_MessageId;

DataPlane::DataPlane(sc_module_name p_ModuleName, int p_InterfaceCount):sc_module(p_ModuleName), m_InterfaceCount(p_InterfaceCount)
{
    // Export the BGP message buffer interface
    //    export_ToDataPlane(m_BGPForwardingBuffer);

    SC_THREAD(main);
    sensitive << port_Clk.pos();
}

DataPlane::~DataPlane()
{
}


///This is where the BGP will sit on. Now there is only a simple example, which shows that the connections work.
void DataPlane::main(void)
{

    StringTools *l_Report = new StringTools(name());

    SC_REPORT_INFO(g_MessageId, l_Report->newReportString("Main of IP plane starts "));
    SC_REPORT_INFO(g_MessageId, l_Report->appendReportString("--- "));
    l_Report->setStampTime(false);
    SC_REPORT_INFO(g_MessageId, l_Report->getReportString());
    l_Report->resetReportString();
    SC_REPORT_INFO(g_MessageId, l_Report->getReportString());
    
  int i = 0;

  m_Packet.setProtocolType(0);
  m_Packet.setIPPayload("1");

  port_ToInterface[0]->write(m_Packet);
    while(true)
    {
      wait();
      
      if(port_FromInterface[i]->num_available() > 0)
          {
              port_FromInterface[i]->read(m_Packet);
              //cout << name() << " received: " << m_Packet << ". At time: " << sc_time_stamp() << endl;
              //      cout << name() << " resolved route in interface " << port_ToRoutingTable->resolveRoute(9) << endl;
              //              SC_REPORT_INFO(g_MessageId, "TESTING");

              m_Packet.setProtocolType(m_Packet.getProtocolType()+1);
              m_Packet.setIPPayload(m_Packet.getIPPayload() << 1);
              //              cout << name() << " forwarding out from interface 0" << endl;
              port_ToInterface[0]->write(m_Packet);
              //              cout << name() << " forwarding out from interface 1" << endl;
              port_ToInterface[1]->write(m_Packet);
              
          }
      i++;
      if(i == m_InterfaceCount-1)
	i = 0;


    }
    delete l_Report;
}

bool DataPlane::write(BGPMessage p_BGPMsg)
{
    m_BGPForwardingBufferMutex.lock();
    m_BGPForwardingBuffer.write(p_BGPMsg);
    m_BGPForwardingBufferMutex.unlock();
    return true;
}
