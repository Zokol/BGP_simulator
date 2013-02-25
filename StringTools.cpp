#include "StringTools.hpp"
#include <stdio.h>
#include <stdlib.h>


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


void StringTools::appendString(const char *p_ReportString)
{
    ostringstream l_Temp(p_ReportString);
    m_CurrentName += l_Temp.str();

}


const char* StringTools::getReportString(void)
{
    m_ResportString << m_BaseName << ": " << m_CurrentName;
    if(m_StampTime)
        m_ResportString << ": @ " << sc_core::sc_time_stamp();
    const char *temp = m_ResportString.str().c_str();
    m_ResportString.str("");
    return temp;
}

const char* StringTools::newReportString(const char* p_ReportString)
{
    ostringstream l_Temp(p_ReportString);
    m_CurrentName = l_Temp.str();
    return getReportString();
}

const char* StringTools::appendReportString(const char* p_ReportString)
{


    appendString(p_ReportString);
    return getReportString();
}

const char* StringTools::newReportString(int p_ReportInt)
{

    ostringstream l_Temp;
    l_Temp << p_ReportInt;
    return newReportString(l_Temp.str().c_str());
}

const char* StringTools::appendReportString(int p_ReportInt)
{
    ostringstream l_Temp;
    l_Temp << p_ReportInt;
    return appendReportString(l_Temp.str().c_str());
}

void StringTools::resetReportString(void)
{
    m_CurrentName = "";

}
