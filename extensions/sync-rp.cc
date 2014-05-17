/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

// sync-rp.cc

#include "sync-rp.h"
#include "namelist-header.h"
#include <sstream>

NS_LOG_COMPONENT_DEFINE ("SyncRP");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (SyncRP);


SyncRP::SyncRP ()
{
  m_face = 0;
}

TypeId
SyncRP::GetTypeId ()
{
  static TypeId tid = TypeId ("SyncRP")
    .SetParent<Application> ()
    .AddConstructor<SyncRP> ()
    .AddAttribute ("SyncPrefix", "The name prefix of Sync Interest.",
                  StringValue ("/ndn/sync"),
                  MakeStringAccessor (&SyncRP::m_syncInterestPrefix),
                  MakeStringChecker ());
  return tid;
}

void
SyncRP::StartApplication ()
{
  NS_LOG_FUNCTION (this);

  m_face = CreateObject<ApiFace> (GetNode ());
  Ptr<ndn::Name> prefix = Create<ndn::Name> (m_syncInterestPrefix);
  m_face->SetInterestFilter (prefix, MakeCallback (&SyncRP::OnInterest, this));
}

void
SyncRP::StopApplication ()
{
  m_face->Shutdown ();
  m_face = 0;
}

void
SyncRP::OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest)
{
  NS_LOG_FUNCTION (interest->GetName () << interest->GetNonce ());
}

} // namespace ndn
} // namespace ns3
