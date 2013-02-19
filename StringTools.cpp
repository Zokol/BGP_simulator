#include "StringTools.hpp"



StringTools::StringTools():m_Identifier(0), m_BaseName("--"), m_Separator("_")
{
}


StringTools::StringTools(string p_BaseName):m_Identifier(0), m_BaseName(p_BaseName), m_Separator("_")
{
}

StringTools::StringTools(string p_BaseName, string p_Separator):m_Identifier(0), m_BaseName(p_BaseName), m_Separator(p_Separator)
{
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
