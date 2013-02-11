/*! \file BGPMessage.cpp
 *  \brief     Implementation of BGPMessage class.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      11.2.2013
 */


#include "BGPMessage.hpp"


BGPMessage::BGPMessage(BGPMessage& p_Msg)
{
    *this = p_Msg;
}


BGPMessage& BGPMessage::operator = (const BGPMessage& p_Msg)
{
    
    return *this;
}



bool BGPMessage::operator == (const BGPMessage& p_Msg) const {
    return true;
}
