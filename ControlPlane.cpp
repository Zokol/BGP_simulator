/*! \file ControlPlane.cpp
 *  \brief     Implementation of ControlPlane.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 12:32:09 2013
 */


#include "ControlPlane.hpp"


ControlPlane::ControlPlane(sc_module_name p_ModName):sc_module(p_ModName)
{
  //make the inner bindings
    export_ToControlPlane(m_ReceivingBuffer); //export the receiving
                                             //buffer's input
                                             //interface for the data plane



    SC_THREAD(controlPlaneMain);
    sensitive << port_Clk.pos();
}

ControlPlane::~ControlPlane()
{
}


void ControlPlane::controlPlaneMain(void)
{
  cout << name() << " starting at time" << sc_time_stamp()  << endl;
    while(true)
    {
        wait();

        
    }

}
