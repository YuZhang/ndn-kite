/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */


#include "upload-mobile.h"
#include <sstream>

NS_LOG_COMPONENT_DEFINE ("MobileApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (MobileApp);


MobileApp::MobileApp ()
{
  m_face = 0;
}

TypeId
MobileApp::GetTypeId ()
{
  static TypeId tid = TypeId ("MobileApp")
    .SetParent<Application> ()
    .AddConstructor<MobileApp> ()
    .AddAttribute ("ServerPrefix", "The name prefix of server.",
                  StringValue ("/server/upload/mobile/file"),
                  MakeStringAccessor (&MobileApp::m_serverPrefix),
                  MakeStringChecker ())
    .AddAttribute ("MobilePrefix", "The name prefix of mobile.",
                  StringValue ("/mobile/file"),
                  MakeStringAccessor (&MobileApp::m_mobilePrefix),
                  MakeStringChecker ())
    .AddAttribute ("RequestPeriod", "The time interval of sending request Interest.",
                   DoubleValue (2.0),
                   MakeDoubleAccessor (&MobileApp::m_requestPeriod),
                   MakeDoubleChecker<double> (0.0, 120.0))
    ;
  return tid;
}

void
MobileApp::StartApplication ()
{
  NS_LOG_FUNCTION (this);

  m_face = CreateObject<ApiFace> (GetNode ());
  Ptr<ndn::Name> prefix = Create<ndn::Name> (m_mobilePrefix);
  m_face->SetInterestFilter (prefix, MakeCallback (&MobileApp::OnInterest, this));
  SendInterest ();
}

void
MobileApp::StopApplication ()
{
  m_face->Shutdown ();
  m_face = 0;
}

void
MobileApp::OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest)
{
  NS_LOG_FUNCTION (interest);
  
  Ptr<ndn::Data> data = Create<ndn::Data> (Create<Packet> (1024));
  data->SetName (Create<ndn::NameComponents> (interest->GetName ())); 

  Simulator::ScheduleNow (&ApiFace::Put, m_face, data);
}

void
MobileApp::OnData (Ptr<const ndn::Interest> origInterest, Ptr<const ndn::Data> data)
{
  NS_LOG_FUNCTION (data->GetNamePtr ()); 
}

void
MobileApp::OnTimeout (Ptr<const ndn::Interest> interest)
{
  SendInterest ();
  return;
}

void
MobileApp::SendInterest ()
{
  Ptr<ndn::Interest> interest = Create<ndn::Interest> ();
  UniformVariable rand (0,std::numeric_limits<uint32_t>::max ());
  Ptr<ndn::Name> name = Create<ndn::Name> (m_serverPrefix); 
  interest->SetNonce            (rand.GetValue ());
  interest->SetName             (name);
  interest->SetInterestLifetime (Seconds (m_requestPeriod)); // 1 is just a random value 
  interest->SetPitForwardingFlag (1); // Tracable
  Simulator::ScheduleNow (&ApiFace::ExpressInterest, m_face, interest,
                     MakeCallback (&MobileApp::OnData, this),
                     MakeCallback (&MobileApp::OnTimeout, this));
}


} // namespace ndn
} // namespace ns3
