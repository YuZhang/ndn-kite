/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

// sync-rp.h

#ifndef SYNC_RP_H
#define SYNC_RP_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/ndnSIM/ndn.cxx/ndn-api-face.h"

namespace ns3 {
namespace ndn {

class SyncRP : public Application
{

public:
  SyncRP ();

  static TypeId
  GetTypeId ();
  
  virtual void
  StartApplication ();

  virtual void
  StopApplication ();

  void
  OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest);

private:
  std::string m_syncInterestPrefix;
  Ptr<ApiFace> m_face;
};

} // namespace ndn
} // namespace ns3

#endif // SYNC_RP_H
