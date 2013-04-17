/*! \file DataPlane.cpp
 *  \brief
 *  \details
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 12:14:35 2013
 */


#include "DataPlane.hpp"

#include "ReportGlobals.hpp"



DataPlane::DataPlane(sc_module_name p_ModuleName, ControlPlaneConfig * const p_Config):sc_module(p_ModuleName), m_Config(p_Config)
{

    m_Rpt.setBaseName(name());

	m_InterfaceCount = m_Config->getNumberOfInterfaces();
    SC_THREAD(main);
    sensitive << port_Clk.pos();
}

DataPlane::~DataPlane()
{

}


///This is where the BGP will sit on. Now there is only a simple example, which shows that the connections work.
void DataPlane::main(void)
{



    SC_REPORT_INFO(g_ReportID, m_Rpt.newReportString("starting "));

    //EXAMPLE OF HOW TO USE STRINGTOOLS FOR REPORTING.

    // SC_REPORT_INFO(g_MessageId, m_Report.appendReportString("--- "));
    //m_Report->setStampTime(false);
    // SC_REPORT_INFO(g_MessageId, m_Report.getReportString());
    //m_Report->resetReportString();
    // SC_REPORT_INFO(g_MessageId, m_Report.getReportString());

    int i = 0;

    m_Packet.setProtocolType(0);
    m_Packet.setIPPayload("1");
  
    port_ToInterface[0]->write(m_Packet);
    while(true)
        {
            wait();
            
            //To check that data is written in this buffer. Seems to work
            // cout << "DataPlane: BGP forwarding buffer has " << m_BGPForwardingBuffer.num_free() << " elements free." << endl;
 

            if(port_FromInterface[i]->num_available() > 0)
                {

                    port_FromInterface[i]->read(m_Packet);

                    if(m_Packet.getProtocolType() == TYPE_IP)
                        {
                            // cout << name() << " received: " << m_Packet << ". At time: " << sc_time_stamp() << endl;
                            // cout << name() << " resolved route in interface " << port_ToRoutingTable->resolveRoute(9) << endl;


                            m_Packet.setProtocolType(m_Packet.getProtocolType()+1);
                            m_Packet.setIPPayload(m_Packet.getIPPayload() << 1);

                            // cout << name() << " forwarding out from interface 0" << endl;
                            port_ToInterface[0]->write(m_Packet);

                            // cout << name() << " forwarding out from interface 1" << endl;
                            port_ToInterface[1]->write(m_Packet);
                        }
                    else if (m_Packet.getProtocolType() == TYPE_BGP)
                        {
                            SC_REPORT_INFO(g_ReportID, m_Rpt.newReportString("Sending BGP message to CP"));
                            m_BGPMsg = m_Packet.getBGPPayload();
                            m_BGPMsg.m_OutboundInterface = i;
                            port_ToControlPlane->write(m_BGPMsg);                            
                        }
                }
     
            if (m_BGPForwardingBuffer.num_available() > 0)
                {

                    SC_REPORT_INFO(g_ReportID, m_Rpt.newReportString("Sending BGP message"));
                    m_BGPForwardingBuffer.read(m_BGPMsg);
                    m_Packet.setBGPPayload(m_BGPMsg);
                    m_Packet.setProtocolType(TYPE_BGP);
                    cout << "Outboud: " << m_BGPMsg.m_OutboundInterface << endl;
                    port_ToInterface[m_BGPMsg.m_OutboundInterface]->write(m_Packet);
                }

            i++;
            if(i == m_InterfaceCount-1)
                i = 0;

            //Example how to resolve a route
            //port_ToRoutingTable->resolveRoute(1);

        }
}

bool DataPlane::write(BGPMessage p_BGPMsg)
{
    m_BGPForwardingBufferMutex.lock();
    m_BGPForwardingBuffer.write(p_BGPMsg);
    m_BGPForwardingBufferMutex.unlock();
    return true;
}
