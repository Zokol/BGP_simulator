/*! \file Host.hpp
 *  \brief
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      02.5.2013
 */



/*!
 * \class Host
 * \brief Host module
 *  \details
 */




#ifndef HOST_H
#define HOST_H


#include "systemc"
#include "Interface.hpp"
#include "StringTools.hpp"
#include "Configuration.hpp"
#include "PacketProcessor.hpp"
#include "Communication_If.hpp"

#define EMPTY "<EMPTY />"

using namespace std;
using namespace sc_core;
using namespace sc_dt;



class Host: public sc_module, public Communication_If
{

public:

    sc_export<Interface_If> **export_ReceivingInterface;

    sc_port<Interface_If, 1, SC_ZERO_OR_MORE_BOUND> **port_ForwardingInterface;


	void hostMain(void);
	Host(sc_module_name p_ModuleName, Connection *p_ConnectionConfig);
	virtual ~Host();

	void interfaceUp(void);

	/*! \sa Communication_If
     */
    virtual bool sendMessage(string p_DestinationIP, string p_SourceIP, string p_Payload);

    /*! \sa Communication_If
     */
    virtual string reaMessageBuffer(void);

    /*! \sa Communication_If
     */
    virtual void clearMessageBuffer(void);

   SC_HAS_PROCESS(Host);

private:


    /*!
     * \property   const sc_time *clk_Periods
     * \brief
     * \details
     * \private
     */
    const sc_time *m_ClkPeriod;

    /*!
     * \property sc_clock *clk_Router
     * \brief Pointer to sc_clock
     * \details
     * \private
     */
    sc_clock *m_ClkRouter;

    /*!
     * \property Interface **m_NetworkInterface
     * \brief Pointer array to network interfaces of the router
     * \details
     * \private
     */
    Interface **m_NetworkInterface;

    /*!
     * \property  StringTools m_Name
     * \brief
     * \details  Used in dynamic module naming.
     * \private
     */
    StringTools m_Name;

    Packet m_Frame;

    PacketProcessor m_Encoder;

    PacketProcessor m_Decoder;



string m_MsgBuffer;

Connection m_ConnectionConfig;
};

#endif
