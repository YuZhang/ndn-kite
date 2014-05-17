/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

// namelist-header.h

#ifndef NAMELIST_HEADER_H
#define NAMELIST_HEADER_H

#include "ns3/header.h"

namespace ns3 {
namespace ndn {


// ========== Class NameListHeader ============
  
class NameListHeader : public Header, public SimpleRefCount<NameListHeader> {

public:

  NameListHeader ();
  NameListHeader (const std::vector<std::string> & nameList);
  virtual ~NameListHeader ();  

  static TypeId
  GetTypeId (void);
  
  virtual TypeId
  GetInstanceTypeId (void) const;
  
  void
  Print (std::ostream &os) const;
  
  uint32_t
  GetSerializedSize (void) const;
  
  void
  Serialize (Buffer::Iterator start) const;
  
  uint32_t
  Deserialize (Buffer::Iterator start);

  const std::vector<std::string> &
  GetNameList () const;

  std::vector<std::string> &
  GetRef ();
  
  void
  AddName (const std::string & name);

private:
  std::vector<std::string> m_nameList;

}; // class NameListHeader

} // namespace ndn
} // namespace ns3

#endif /* NAMELIST_HEADER_H */
