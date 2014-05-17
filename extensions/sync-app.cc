/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn> 
 */

// sync-app.cc

#include "sync-app.h"
#include "namelist-header.h"

#include "ns3/random-variable.h"
#include <sstream>

NS_LOG_COMPONENT_DEFINE ("SyncApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (SyncApp);


SyncApp::SyncApp ()
{
  m_seq = 1;
  m_face = 0;
}

TypeId
SyncApp::GetTypeId ()
{
  static TypeId tid = TypeId ("SyncApp")
    .SetParent<Application> ()
    .AddConstructor<SyncApp> ()
    .AddAttribute ("SyncPrefix", "The name prefix of Sync Interest.",
                   StringValue ("/ndn/sync"),
                   MakeStringAccessor (&SyncApp::m_syncInterestPrefix),
                   MakeStringChecker ())
    .AddAttribute ("UpdatePeriod", "The time interval of generating updates.",
                   DoubleValue (3.0),
                   MakeDoubleAccessor (&SyncApp::m_updatePeriod),
                   MakeDoubleChecker<double> (0.5, 120.0))
    ;
  return tid;
}

void
SyncApp::StartApplication ()
{
  NS_LOG_FUNCTION (this);

  m_face = CreateObject<ApiFace> (GetNode ());

  Ptr<ndn::Name> prefix = Create<ndn::Name> (m_syncInterestPrefix);

  m_sync = CreateObject<SyncLogic> (m_face, prefix, MakeCallback (&SyncApp::OnUpdate, this));
  m_sync->Start ();

  std::stringstream ss;
  ss << GetNode ()-> GetId ();
  m_routerName = "APP-" +  ss.str();

  m_seq = 0;
  Simulator::Schedule (Seconds (1), &SyncApp::GenerateUpdate, this);
}

void
SyncApp::StopApplication ()
{
  m_face->Shutdown ();
  m_sync->Stop ();
  m_face = 0;
}

void
SyncApp::GenerateUpdate ()
{
  std::string s;

  m_seq++;
  std::stringstream out;
  out << m_seq;
  // SyncState::IdSeqToName ("/" + m_routerName + "/unit-" + out.str(), 1, s);
  SyncState::IdSeqToName ("/" + m_routerName + "/unit1", m_seq, s);
  Ptr<ndn::Name> name = Create<ndn::Name> (s);
  m_sync->Publish (name);

  UniformVariable jitter (0.001, m_updatePeriod);
  Simulator::Schedule (Seconds (jitter.GetValue ()), &SyncApp::GenerateUpdate, this);
}

void
SyncApp::OnUpdate (Ptr<const ndn::Name> update)
{
  NS_LOG_FUNCTION (update->toUri ());
}

} // namespace ndn
} // namespace ns3
