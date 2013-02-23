#include "StringTools.hpp"



StringTools::StringTools():m_BaseName("--"), m_Separator("_"), m_Identifier(0), m_StampTime(true), m_Reset(true)
{
}

StringTools::StringTools(string p_BaseName):m_BaseName(p_BaseName), m_Separator("_"), m_Identifier(0)
{
}

StringTools::StringTools(string p_BaseName, string p_Separator):m_BaseName(p_BaseName), m_Separator(p_Separator), m_Identifier(0)
{
}

StringTools::StringTools(const char *p_BaseName):m_Separator("_"), m_Identifier(0), m_StampTime(true), m_Reset(true)
{
    m_BaseName = p_BaseName;
}

StringTools::StringTools(const char *p_BaseName, bool p_StampTime):m_Separator("_"), m_Identifier(0), m_StampTime(p_StampTime), m_Reset(true)
{
    m_ResportString << p_BaseName << ": ";
    

}

StringTools::~StringTools()
{
}


void StringTools::setBaseName(string p_BaseName)
{
    m_BaseName = p_BaseName;
}

void StringTools::resetIdentifier(void)
{
    m_Identifier = 0;
}

void StringTools::setIdentifier(int p_Identifier)
{
    m_Identifier = p_Identifier;
}

void StringTools::setSeparator(string p_Separator)
{
    m_Separator = p_Separator;
}

void StringTools::setStampTime(bool p_StampTime)
{
    m_StampTime  = p_StampTime;
}

void StringTools::setReset(bool p_Reset)
{
    m_Reset  = p_Reset;
}

const char* StringTools::getCurrentName(void)
{
    return m_CurrentName.c_str();
}

const char* StringTools::getNextName(void)
{
  stringstream ss;
  ss << m_BaseName << m_Separator << m_Identifier++;
  m_CurrentName = ss.str();
  return m_CurrentName.c_str();
}


void StringTools::appendReportString(const char *p_ReportString)
{
    ostringstream l_Temp(p_ReportString);
    m_CurrentName += l_Temp.str();

}


void StringTools::appendReportString(const sc_core::sc_time p_ReportString)
{
    m_ResportString << p_ReportString;
}

const char* StringTools::getReportString(void)
{
    m_ResportString << m_BaseName << ": " << m_CurrentName;
    if(m_StampTime)
        m_ResportString << ": @ " << sc_core::sc_time_stamp();
    const char *temp = m_ResportString.str().c_str();
    return temp;
}

const char* StringTools::getReportString(bool p_Reset)
{
    if(p_Reset)
        {
            resetReportString();
            return getReportString();
        }
    else
        return m_ResportString.str().c_str();
}

// const char* StringTools::getReportString(const char* p_DirectFeed)
// {
//     resetReportString();
//     appendReportString(p_DirectFeed);
//     return getReportString();
// }

const char* StringTools::getReportString(const char* p_DirectFeed, bool p_Append)
{
    resetReportString();

if (p_Append)
    {

        appendReportString(p_DirectFeed);
    }
 else
     {
            ostringstream l_Temp(p_DirectFeed);
            m_CurrentName = l_Temp.str();

     }
    return getReportString();
}

void StringTools::resetReportString(void)
{
    m_ResportString.str("");

}
