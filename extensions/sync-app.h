/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

// sync-app.h

#ifndef SYNC_APP_H
#define SYNC_APP_H

#include "sync-state.h"
#include "sync-logic.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/ndnSIM/ndn.cxx/ndn-api-face.h"

namespace ns3 {
namespace ndn {

class SyncApp : public Application
{

public:
  SyncApp ();

  static TypeId
  GetTypeId ();
  
  virtual void
  StartApplication ();

  virtual void
  StopApplication ();

  void
  OnUpdate (Ptr<const ndn::Name> update);

  void
  GenerateUpdate ();

private:
  std::string m_routerName;
  uint32_t m_seq;
  std::string m_syncInterestPrefix;
  Ptr<SyncLogic> m_sync;
  Ptr<ApiFace> m_face;
  double m_updatePeriod;
};

} // namespace nlsr
} // namespace ns3

#endif // SYNC_APP_H
