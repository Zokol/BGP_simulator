/*! \file Interface.cpp
 *  \brief     Implementation of Network Interface.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      25.1.2013
 */


#include "Interface.hpp"
#include "ReportGlobals.hpp"


Interface::Interface(sc_module_name p_ModName, Connection *p_IfConfig):sc_module(p_ModName), m_IfConfig(p_IfConfig)
{
  //make the inner bindings
    export_FromDataPlane(m_ForwardingBuffer); //export the forwarding buffer's input interface for the protocol engine
    export_ToDataPlane(m_ReceivingBuffer); // //export the forwarding buffer's input interface for the protocol engine

    m_InterfaceState = DOWN;
    m_Report.setBaseName(name());

    SC_THREAD(interfaceMain);
    sensitive << port_Clk.pos();
}

Interface::~Interface()
{
}


void Interface::interfaceMain(void)
{
    SC_REPORT_INFO(g_ReportID, m_Report.newReportString("starting") );

    while(true)
    {
        wait();
	
	if(m_InterfaceState) //only if interface is up
	  //Forward the next packet from the local forwarding queue
	  port_Output->forward(m_ForwardingBuffer.read());
    }
}

bool Interface::forward(Packet p_Packet)
{
    //allow writing only if the interface is up
    if(m_InterfaceState)
        {
            //check that the buffer is not full
            if(m_ReceivingBuffer.num_free() > 0)
                {
                    m_ReceivingBuffer.write(p_Packet);
                    return true;
                }
            else
                return false;
        }
    else
        return false;
}

void Interface::interfaceDown(void)
{
    //set interface down
    m_InterfaceState = DOWN;
    SC_REPORT_INFO(g_ReportID, m_Report.newReportString("DOWN"));
}

bool Interface::interfaceUp(void)
{
    if(m_IfConfig->hasConnection())
        {
            m_InterfaceState = UP;
            SC_REPORT_INFO(g_ReportID, m_Report.newReportString("UP"));

        }
    else
        m_InterfaceState = DOWN;
    return m_InterfaceState;
}

bool Interface::isUp(void)
{
    return m_InterfaceState;
}

void Interface::emptyBuffers(void)
{
    //empty the receiving buffer
    while(m_ReceivingBuffer.num_available() > 0)
        m_ReceivingBuffer.read();
    //empty the forwarding buffer
    while(m_ForwardingBuffer.num_available() > 0)
        m_ForwardingBuffer.read();

}



void Interface::killInterface(void)
{
    interfaceDown();
    emptyBuffers();
}

void Interface::resetInterface(void)
{
    killInterface();
    interfaceUp();
}
