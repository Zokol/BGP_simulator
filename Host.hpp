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



#include "systemc"
#include "Interface.hpp"
#include "StringTools.hpp"
#include "Configuration.hpp"


using namespace std;
using namespace sc_core;
using namespace sc_dt;



#ifndef HOST_H
#define HOST_H



class Host: public sc_module
{

public:

    sc_export<Interface_If> **export_ReceivingInterface;

    sc_port<Interface_If, 1, SC_ZERO_OR_MORE_BOUND> **port_ForwardingInterface;


	void hostMain(void);
	Host(sc_module_name p_ModuleName, Connection *p_ConnectionConfig);
	virtual ~Host();
	void interfaceUp(void);
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



string m_MsgBuffer;

Connection m_ConnectionConfig;
};

#endif
