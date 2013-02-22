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

#ifndef _STRINGTOOLS_H_
#define _STRINGTOOLS_H_



class StringTools
{
public:


    StringTools();

    StringTools(string p_BaseName);

    StringTools(string p_BaseName, string p_Separator);

    virtual ~StringTools();

    void setBaseName(string p_BaseName);
    
    const char* getNextName(void);

    const char* getCurrentName(void);

    void resetIdentifier(void);
    
    void setIdentifier(int p_Identifier);

    void setSeparator(string p_Separator);

    void appendReportString(string p_ReportString);

    const char* getReportString(void);

    void resetReportString(void);

private:

    string m_BaseName;
    
    string m_CurrentName;

    string m_Separator;

    int m_Identifier;

    ostringstream m_ResportString;
    
};


#endif /* _STRINGTOOLS_H_ */
