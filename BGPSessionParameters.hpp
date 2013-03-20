/*! \file  BGPSessionParameters.hpp
 *  \brief    Holds the parameters for a BGP session
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Wed Feb 13 14:38:03 2013
 */

/*!
 * \class BGPSessionParameters
 * \brief Holds the parameters for a BGP session
 *  \details   
 */







using namespace std;

#ifndef _BGPSESSIONPARAMETERS_H_
#define _BGPSESSIONPARAMETERS_H_




class BGPSessionParameters
{

public:
  

    

    /*! \brief HoldDown time for this session
     * \details Defines the keepalive time for this session. The default
     * value needs to be set in the elaboration phase. After that the
     * BGP session may negotiated a new value between the session peers
     * \private
     */
    int m_KeepaliveTime;

    /*! \brief HoldDown time factor
     * \details Defines the multiplier that determines the holdDown
     * time by m_KeepaliveTime X m_HoldDownTimeFactor
     * \private
     */
    int m_HoldDownTimeFactor;
    
};




#endif /* _BGPSESSIONPARAMETERS_H_ */
