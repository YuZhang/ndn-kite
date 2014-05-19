/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */


#include "upload-server.h"
#include <sstream>

NS_LOG_COMPONENT_DEFINE ("ServerApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (ServerApp);


ServerApp::ServerApp ()
{
  m_face = 0;
}

TypeId
ServerApp::GetTypeId ()
{
  static TypeId tid = TypeId ("ServerApp")
    .SetParent<Application> ()
    .AddConstructor<ServerApp> ()
    .AddAttribute ("ServerPrefix", "The name prefix of server.",
                  StringValue ("/server/upload"),
                  MakeStringAccessor (&ServerApp::m_serverPrefix),
                  MakeStringChecker ())
    .AddAttribute ("RequestPeriod", "The time interval of sending request Interest.",
                   DoubleValue (2.0),
                   MakeDoubleAccessor (&ServerApp::m_requestPeriod),
                   MakeDoubleChecker<double> (0.0, 120.0))
    ;
  return tid;
}

void
ServerApp::StartApplication ()
{
  NS_LOG_FUNCTION (this);

  m_face = CreateObject<ApiFace> (GetNode ());
  Ptr<ndn::Name> prefix = Create<ndn::Name> (m_serverPrefix);
  m_face->SetInterestFilter (prefix, MakeCallback (&ServerApp::OnInterest, this));
  m_mobilePrefixSize = 1;
  m_credit = 20;
  m_seq = 0;
}

void
ServerApp::StopApplication ()
{
  m_face->Shutdown ();
  m_face = 0;
}

void
ServerApp::OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest)
{
  NS_LOG_FUNCTION (interest);
  if (m_seq > 0)
  {
    return;    
  }
  Ptr<const ndn::Name> name = interest->GetNamePtr ();
  m_uploadName = Create<ndn::Name> (name->toUri ());
  //m_uploadName = Create<ndn::Name> ("/server/upload/mobile/file");
  m_mobilePrefix = name->getPostfix (m_mobilePrefixSize+1, 0).toUri ();
  NS_LOG_DEBUG ("Mobile prefix " << m_mobilePrefix);
  m_seq = 1;
  int i = 0;
  for (; i < m_credit; i++)
    {
      SendInterest (m_seq);
      m_seq++;
    }
}

void
ServerApp::OnData (Ptr<const ndn::Interest> origInterest, Ptr<const ndn::Data> data)
{
  NS_LOG_FUNCTION (data->GetNamePtr ()); 
  SendInterest (m_seq);
  m_seq ++;
}

void
ServerApp::OnTimeout (Ptr<const ndn::Interest> interest)
{
  Ptr<const ndn::Name> name = interest->GetNamePtr ();
  SendInterest (name->get (-1).toNumber ());
  return;
}

void
ServerApp::SendInterest (int seq)
{
  NS_LOG_FUNCTION (seq);
  Ptr<ndn::Interest> interest = Create<ndn::Interest> ();
  UniformVariable rand (0,std::numeric_limits<uint32_t>::max ());
  Ptr<ndn::Name> name = Create<ndn::Name> (m_mobilePrefix);
  name->appendNumber (seq);

  interest->SetNonce            (rand.GetValue ());
  interest->SetName             (name);
  interest->SetInterestLifetime (Seconds (m_requestPeriod)); // 1 is just a random value 
  interest->SetPitForwardingFlag (2); // TraceOnly
  interest->SetPitForwardingName (m_uploadName);
  
  Simulator::Schedule (Seconds (0), &ApiFace::ExpressInterest, m_face, interest,
                     MakeCallback (&ServerApp::OnData, this),
                     MakeCallback (&ServerApp::OnTimeout, this));
  //m_face->ExpressInterest( interest,
  //                   MakeCallback (&ServerApp::OnData, this),
  //                   MakeCallback (&ServerApp::OnTimeout, this));
}


} // namespace ndn
} // namespace ns3
