/*! \file Packet.cpp
 *  \brief     Implementation of Packet class.
 *  \details   
 *  \author    Antti Siirilä, 501449
 *  \version   1.0
 *  \date      29.1.2013
 */

#include "Packet.hpp"


Packet::Packet()
  {
    m_DottedSourceIP = "0.0.0.0";
    m_DottedDestinationIP = "0.0.0.0";
    m_SourceIP = 0;
    m_DestinationIP = 0;
    m_ProtocolType = 0;
    m_Payload = NULL;
  };



Packet::Packet(void *p_Payload, int p_ProtocolType, int p_SourceIP, int p_DestinationIP)
{
  m_SourceIP = p_SourceIP;
  m_DestinationIP = p_DestinationIP;
  m_ProtocolType = p_ProtocolType;
  m_DottedSourceIP = convertIPToString(p_SourceIP);
  m_DottedDestinationIP = convertIPToString(p_DestinationIP);
  m_Payload = NULL;
}

Packet::Packet(void *p_Payload, int p_ProtocolType, string p_SourceIP, string p_DestinationIP)
{
  m_DottedSourceIP = p_SourceIP;
  m_DottedDestinationIP = p_DestinationIP;
  m_ProtocolType = p_ProtocolType;
  m_SourceIP = convertIPToInt(p_SourceIP);
  m_DestinationIP = convertIPToInt(p_DestinationIP);
  m_Payload = NULL;

}

bool Packet::setSourceIP(int p_SourceIP)
{
  m_SourceIP = p_SourceIP;
  m_DottedSourceIP = convertIPToString(p_SourceIP);
  return true;  //TO-DO: address check
}

bool Packet::setSourceIP(string p_SourceIP)
{
  m_DottedSourceIP = p_SourceIP;
  m_SourceIP = convertIPToInt(p_SourceIP);
  return true;  //TO-DO: address check
}

bool Packet::setDestinationIP(int p_DestinationIP)
{
  m_DestinationIP = p_DestinationIP;
  m_DottedDestinationIP = convertIPToString(p_DestinationIP);
  return true;  //TO-DO: address check
}

bool Packet::setDestinationIP(string p_DestinationIP)
{
  m_DottedDestinationIP = p_DestinationIP;
  m_DestinationIP = convertIPToInt(p_DestinationIP);
  return true;  //TO-DO: address check
}

bool Packet::setProtocolType(int p_ProtocolType)
{
  m_ProtocolType = p_ProtocolType;  
  return true; //TO-DO: validity check
}

void Packet::setPayload(void *p_Payload)
{
  m_Payload = p_Payload;
}



int Packet::getSourceIP(void)
{
  return m_SourceIP;
}


int Packet::getDestinationIP(void)
{
  return m_DestinationIP;
}


void Packet::sourceIPToString(void)
{
  cout << m_DottedSourceIP << endl;
}


void Packet::destinationIPToString(void)
{
  cout << m_DottedDestinationIP << endl;
}


void Packet::getPayload(void *p_Payload)
{
  //TO-DO: This needs to be figured out. What is the best way to do it.
}

int Packet::getProtocolType(void)
{
  return m_ProtocolType;
}


bool Packet::operator == (const Packet& p_Packet) const {
  return (p_Packet.m_Payload == m_Payload &&  p_Packet.m_ProtocolType == m_ProtocolType && p_Packet.m_SourceIP == m_SourceIP &&  p_Packet.m_DestinationIP == m_DestinationIP );
}

Packet& Packet::operator = (const Packet& p_Packet) {
  m_Payload = p_Packet.m_Payload;
  m_ProtocolType = p_Packet.m_ProtocolType;
  m_SourceIP = p_Packet.m_SourceIP;
  m_DestinationIP = p_Packet.m_DestinationIP;
  return *this;
}



string Packet::convertIPToString(int p_IP)
{

  //TO-DO: the int-to-dotted converter

  return "";
}

int Packet::convertIPToInt(string p_IP)
{

  //TO-DO: the dotted-to-int converter

  return 0;
}





