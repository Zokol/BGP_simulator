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

string StringTools::uToS(unsigned p_Value)
{
    ostringstream l_Temp;
    l_Temp << p_Value;
    return l_Temp.str();
}

string StringTools::iToS(int p_Value)
{
    ostringstream l_Temp;
    l_Temp << p_Value;
    return l_Temp.str();
}


sc_uint<32> StringTools::convertIPToBinary(string p_Prefix)
{
	string l_Prefix = p_Prefix, l_Octet;
	int l_IntOctet = 0;
	sc_uint<8> l_BinOctet[4];

	unsigned l_Start = 0, l_End;

	for(int i = 0; i < 4; i++ )
	{
		if(i < 3)
			l_End = l_Prefix.find(".", l_Start);
		else
			l_End = l_Prefix.find("/", l_Start);

		l_Octet = l_Prefix.substr(l_Start,l_End-l_Start);
		l_Start = l_End+1;
		istringstream(l_Octet) >> l_IntOctet;

		if(l_IntOctet > 255)
		{
			return -1;
		}

		l_BinOctet[i] = l_IntOctet;
	}

	return (l_BinOctet[0], l_BinOctet[1], l_BinOctet[2], l_BinOctet[3]);


}

sc_uint<32> StringTools::convertMaskToBinary(string p_Prefix)
{
	sc_uint<32> l_Mask = 0;
	unsigned i, j;
	i = p_Prefix.find("/",0);
	string l_MaskLength = p_Prefix.substr(++i);
	istringstream(l_MaskLength) >> i;
	j = (32 - i);
	while(j < 32)
		l_Mask[j++] = 1;
	return l_Mask;
}

string StringTools::convertIPToString(sc_uint<32> p_IP, sc_uint<32> p_Mask)
{

	string l_Prefix = convertIPToString(p_IP);
    l_Prefix += "/";
    l_Prefix += convertMaskToString(p_Mask);
	return l_Prefix;


}


string StringTools::convertIPToString(sc_uint<32> p_IP)
{

	string l_Prefix = "";
	ostringstream convert;   // stream used for the conversion

	for(int j = 0; j < 32; j += 8)
	{
		convert.str("");
		convert << p_IP.range((31-j), (24-j));
		l_Prefix += convert.str();
		if(j != 24)
			l_Prefix += ".";

	}
    return l_Prefix;
}

string StringTools::convertMaskToString(sc_uint<32> p_Mask)
{
	int i = 0;
	ostringstream convert;   // stream used for the conversion

	while(i < 32)
	{
		if(p_Mask[i])
			break;
		i++;
	}
	convert << 32-i;      // insert the textual representation of 'Number' in the characters in the stream
    return convert.str();
}


/*! \sa StringTools
 */
bool StringTools::ipToUChar(string p_IPAddress, unsigned char *p_IPBinAddress)
{
    
	string l_Octet;
	unsigned l_BinOctet = 0;

	unsigned l_Start = 0, l_End;

    //Loop through all the octets
	for(int i = 0; i < 4; i++ )
	{
        //The three first octets
		if(i < 3)
            {
                l_End = p_IPAddress.find(".", l_Start);
                l_Octet = p_IPAddress.substr(l_Start,l_End-l_Start);
                l_Start = l_End+1;
            }
        //the last octet
		else
            l_Octet = p_IPAddress.substr(l_Start);

        //convet the string to binary
		istringstream(l_Octet) >> l_BinOctet;

        //check that the value is inside the range
		if(l_BinOctet > 255)
		{
			return false;
		}

        //store the value in the buffer
		*(p_IPBinAddress+i) = (unsigned char)l_BinOctet;
    
    }

    //return
    return true;
}

/*! \sa StringTools
 */
string StringTools::ipToString(unsigned char *p_IPBinAddress)
{
	string l_IP = "";
    //Loop through all the octets
	for(int i = 0; i < 3; i++ )
	{
        l_IP += uToS(p_IPBinAddress[i]) + "."; 
    }
    l_IP += uToS(p_IPBinAddress[3]); 

    //return
    return l_IP;
}

