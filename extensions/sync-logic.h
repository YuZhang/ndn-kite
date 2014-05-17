/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

// sync-logic.h

#ifndef SYNC_LOGIC_H
#define SYNC_LOGIC_H

#include "sync-state.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/ndnSIM/ndn.cxx/ndn-api-face.h"

namespace ns3 {
namespace ndn {

class SyncLogic : public Object
{
public:
  typedef Callback<void, Ptr<const ndn::Name> > UpdateCallback;

  SyncLogic ();

  SyncLogic (Ptr<ApiFace> face, Ptr<const Name> prefix, UpdateCallback onUpdate);

  static TypeId
  GetTypeId ();
  
  virtual void
  Start ();

  virtual void
  Stop ();

  // Self-published Updates won't trigger UpdateCallback onUpdate()
  bool
  Publish (Ptr<const ndn::Name> name); 

private:
  void
  OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest);

  void
  OnData (Ptr<const Interest> origInterest, Ptr<const ndn::Data> data);

  void
  OnTimeout (Ptr<const ndn::Interest> interest);

  void
  SendSyncInterest (uint64_t oldDigest, uint64_t newDigest, Time const & time);

  void
  SendUpdateInbetween (uint64_t digest1, uint64_t digest2);

  // void
  // SendKeepAlive (Ptr<const ndn::Name> name);

  void
  PeriodicalSyncInterest ();

  void
  PeriodicalJoinInterest ();

  void
  OnJoinInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest);

  const Ptr<ndn::Interest>
  BuildSyncInterest (uint64_t digest1, uint64_t digest2);

  uint64_t
  GetDigestFromName (Ptr<const ndn::Name> name, uint64_t & digest1, uint64_t & digest2) const;

  void
  OnNewUpdate ();
  
  bool
  IsPacketDropped () const;

  inline uint64_t
  CurrentDigest () const;

private:
  std::string m_syncInterestPrefix;
  uint16_t m_syncInterestPrefixSize;

  double m_lossRate; // just for experimence
  uint32_t m_logLength;

  uint64_t m_outstandingDigest;

  SyncState m_state;

  Ptr<ApiFace> m_face;
  UpdateCallback m_onUpdate;

  double m_syncPeriod;
  double m_joinPeriod;

  bool m_useJoin;
};

} // namespace nlsr
} // namespace ns3

#endif // SYNC_LOGIC_H
