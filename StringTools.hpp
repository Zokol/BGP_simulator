/*! \file  StringTools.hpp
 *  \brief   Tool to construct unique module names
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      Tue Feb 12 13:01:27 2013
 */

/*!
 * \class StringTool
 * \brief
 *  \details
 */

#include "systemc"

using namespace std;
using namespace sc_dt;

#ifndef _STRINGTOOLS_H_
#define _STRINGTOOLS_H_



class StringTools
{
public:


    StringTools();

    StringTools(string p_BaseName);

    StringTools(string p_BaseName, string p_Separator);

    StringTools(const char *p_BaseName);

    StringTools(const char *p_BaseName, bool p_StampTime);

    virtual ~StringTools();

    void setBaseName(string p_BaseName);
    
    const char* getNextName(void);

    const char* getCurrentName(void);

    void resetIdentifier(void);
    
    void setIdentifier(int p_Identifier);

    void setSeparator(string p_Separator);

    void setStampTime(bool p_StampTime);

    void setReset(bool p_Reset);

    void appendString(const char *p_ReportString);

    const char* getReportString(void);

    const char* newReportString(const char* p_ReportString);

    const char* appendReportString(const char* p_ReportString);

    const char* newReportString(int p_ReportInt);

    const char* appendReportString(int p_ReportInt);

    void resetReportString(void);

    string iToS(int p_Value);

    sc_uint<32> convertIPToBinary(string p_Prefix);

    sc_uint<32> convertMaskToBinary(string p_Prefix);

    string convertIPToString(sc_uint<32> p_IP, sc_uint<32> p_Mask);

    string convertIPToString(sc_uint<32> p_IP);

    string convertMaskToString(sc_uint<32> p_Mask);

    /*! \fn bool ipToUChar(string p_IPAddress, unsigned char *p_IPBinAddress); 
     * \brief Converts string type IPAddress to binary form in four element unsigned char array
     * \details 
     * @param [in] string p_IPAddress 
     * @param [out] unsigned char *p_IPBinAddress 
     * \return bool: true - conversion success | false - conversion failed
     * \public
     */
    bool ipToUChar(string p_IPAddress, unsigned char *p_IPBinAddress);
    


private:

    string m_BaseName;
    
    string m_CurrentName;

    string m_Separator;

    int m_Identifier;

    ostringstream m_ResportString;

    bool m_StampTime;    

    bool m_Reset;


};


#endif /* _STRINGTOOLS_H_ */
