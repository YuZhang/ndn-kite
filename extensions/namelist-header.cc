/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

// namelist-header.h

#include "namelist-header.h"
#include "ns3/assert.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("NameList");

namespace ns3 {
namespace ndn {

// ========== Class NameListHeader ============

NS_OBJECT_ENSURE_REGISTERED (NameListHeader);

NameListHeader::NameListHeader ()
{
}

NameListHeader::NameListHeader (const std::vector<std::string> & nameList)
{
   std::copy (nameList.begin(), nameList.end (), m_nameList.begin ());
}

NameListHeader::~NameListHeader ()
{
}

TypeId
NameListHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("NameListHeader")
    .SetParent<Header> ()
    .AddConstructor<NameListHeader> ()
  ;
  return tid;
}

TypeId
NameListHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NameListHeader::GetSerializedSize (void) const
{
  uint32_t size = 0;
  uint16_t smallSize = 0;

  size += sizeof (size);

  for ( std::vector<std::string>::const_iterator i = m_nameList.begin ();
        i != m_nameList.end ();
        i++ ) {
    size += sizeof (smallSize) + i->size (); 
  }
  NS_LOG_DEBUG ("GetSerializedSize NameListHeader: " << size); 
  return size;
}

void
NameListHeader::Print (std::ostream &os) const
{
  os << "=== NameListHeader ===" << std::endl;
  for ( std::vector<std::string>::const_iterator i = m_nameList.begin ();
        i != m_nameList.end ();
        i++ ) {
    os << "Name:  " << i->c_str() << std::endl;
  }
}

void
NameListHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU32 (GetSerializedSize () - sizeof (uint32_t));

  for ( std::vector<std::string>::const_iterator name = m_nameList.begin ();
        name != m_nameList.end();
        name++ ) {
    i.WriteHtonU16 (name->size ());
    i.Write ((const uint8_t *) name->c_str (), name->size ());
  }
}

uint32_t
NameListHeader::Deserialize (Buffer::Iterator start)
{

  uint32_t messageSize = start.ReadNtohU32 ();

  //NS_LOG_DEBUG ("Deserialize NameListHeader 1:" << messageSize); 

  uint32_t leftSize = messageSize;
  Buffer::Iterator i = start;

  while (leftSize > 0) {

    std::string name;
    uint16_t stringSize = 0;

    NS_ASSERT (leftSize >= sizeof (stringSize));
    stringSize = i.ReadNtohU16(); 
    leftSize -= sizeof (stringSize);
     
    //NS_LOG_DEBUG ("leftSize: " << leftSize << "  stringSize: " << stringSize);

    NS_ASSERT (leftSize >= stringSize);
    leftSize -= stringSize;
    name.clear ();
    for (; stringSize > 0; stringSize--) {
      name.push_back (i.ReadU8 ()); 
    }

    m_nameList.push_back (name);
  }
 
  NS_ASSERT (leftSize == 0);

  return messageSize + sizeof (messageSize);
}

const std::vector<std::string> &
NameListHeader::GetNameList () const
{
  return m_nameList;
}

std::vector<std::string> &
NameListHeader::GetRef ()
{
  return m_nameList;
}

void
NameListHeader::AddName (const std::string &name)
{
  m_nameList.push_back(name);
}
 
} // namespace ndn
} // namespace ns3

