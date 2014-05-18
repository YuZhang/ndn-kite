/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn> 
 */

// sync-logic.cc

#include "sync-logic.h"
#include "namelist-header.h"
#include <sstream>

NS_LOG_COMPONENT_DEFINE ("SyncLogic");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (SyncLogic);


SyncLogic::SyncLogic ()
{
  m_outstandingDigest = 0;
  m_face = 0;
}

SyncLogic::SyncLogic (Ptr<ApiFace> face, Ptr<const Name> prefix, UpdateCallback onUpdate)
{
  m_outstandingDigest = 0;
  m_face = 0;
  m_face = face;
  m_syncInterestPrefix = prefix->toUri ();
  NS_ASSERT (! onUpdate.IsNull ());
  m_onUpdate = onUpdate;
}

TypeId
SyncLogic::GetTypeId ()
{
  static TypeId tid = TypeId ("SyncLogic")
    .SetParent<Application> ()
    .AddConstructor<SyncLogic> ()
    .AddAttribute ("SyncPrefix", "The name prefix of Sync Interest.",
                   StringValue ("/ndn/sync"),
                   MakeStringAccessor (&SyncLogic::m_syncInterestPrefix),
                   MakeStringChecker ())
    .AddAttribute ("LossRate", "The packet loss rate for Sync Application.",
                   DoubleValue (0),
                   MakeDoubleAccessor (&SyncLogic::m_lossRate),
                   MakeDoubleChecker<double> (0.0, 1.0))
    .AddAttribute ("MaxLogLength", "The maximum length of digest log.",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&SyncLogic::m_logLength),
                   MakeUintegerChecker<uint32_t> (10))
    .AddAttribute ("SyncPeriod", "The time interval of sending Sync Interests.",
                   DoubleValue (2.0),
                   MakeDoubleAccessor (&SyncLogic::m_syncPeriod),
                   MakeDoubleChecker<double> (0.5, 120.0))
    .AddAttribute ("JoinPeriod", "The time interval of sending Join Interests. 0 means NO Join Interest, and NO PIT Forwarding.",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&SyncLogic::m_joinPeriod),
                   MakeDoubleChecker<double> (0.0, 120.0))
    ;
  return tid;
}

void
SyncLogic::Start ()
{
  NS_LOG_FUNCTION (this);

  m_state.SetMaxLogLength (m_logLength);

  Ptr<ndn::Name> prefix = Create<ndn::Name> (m_syncInterestPrefix);
  m_syncInterestPrefixSize = prefix->size ();

  m_face->SetInterestFilter (prefix, MakeCallback (&SyncLogic::OnInterest, this));

  Simulator::Schedule (Seconds (0.0), &SyncLogic::PeriodicalSyncInterest, this);

  if (m_joinPeriod) {
    Simulator::Schedule (Seconds (0.0), &SyncLogic::PeriodicalJoinInterest, this);    
  }
}

void
SyncLogic::Stop ()
{
  NS_LOG_FUNCTION (CurrentDigest ());
}

void
SyncLogic::OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest)
{
  Ptr<const ndn::Name> name = interest->GetNamePtr ();

  NS_LOG_FUNCTION (interest);

  if (name->toUri ().compare (m_syncInterestPrefix + "/join") == 0)
  {
    OnJoinInterest (origName, interest);
    return;
  }

  uint64_t digest1 = 0;
  uint64_t digest2 = 0;
  GetDigestFromName (name, digest1, digest2);

  if ( IsPacketDropped () ) { NS_LOG_INFO ("Interest Packet Lost !"); return; }

  NS_LOG_FUNCTION (digest1 << digest2 << CurrentDigest ());

  if (digest2 == 0) {
    if (CurrentDigest () == digest1) {
      NS_LOG_LOGIC ("Synced!");
        m_outstandingDigest = digest1;
        m_state.SetSyncDigest (digest1);
    } else {
      if (m_state.IsDigestInLog (digest1)) {
        NS_LOG_LOGIC ("Known!");
        SendUpdateInbetween (digest1, CurrentDigest ());
      } else {
        NS_LOG_LOGIC ("Unknown!");
        //To-Do: Maybe an elder digest is more likely to be synced.
        SendSyncInterest (m_state.GetSyncDigest (), digest1, Seconds (0));
      }
    }
  }
  else {
    if (m_state.IsDigestInLog (digest2) && m_state.IsDigestInLog (digest1)) {
      NS_LOG_LOGIC ("Resynced!");
      SendUpdateInbetween (digest1, digest2);
    }
    else {
      if (m_state.IsDigestInLog (digest2)) {
        NS_LOG_LOGIC ("Cannot Resync with Unknown 1st Digest! ");
        //To-Do: there should be some ways to recalculate digest log to find out digest1
      }
      else {
        NS_LOG_LOGIC ("Cannot Resync with Two Unknown Digests! ");
        // ignore unknown and un-sync-able digest      
      }
    } 
  }
}

void
SyncLogic::OnData (Ptr<const ndn::Interest> origInterest, Ptr<const ndn::Data> data)
{
  if ( IsPacketDropped () ) { NS_LOG_INFO ("Data Packet Lost!"); return;}

  uint64_t digest1, digest2;
  GetDigestFromName (data->GetNamePtr (), digest1, digest2);

  NS_LOG_FUNCTION (digest1 << digest2);

  Ptr<Packet> payload = data->GetPayload ()->Copy ();    
  NameListHeader nameList;
  payload->RemoveHeader (nameList);
  //nameList.Print(std::cout);
  NS_LOG_DEBUG (nameList);
  std::vector<std::string> newNameList;
  bool hasNew = false;
  
  // if (digest1 == digest2)
  // {
  //   NS_LOG_LOGIC ("Keepalive Message");

  //   for (std::vector<std::string>::const_iterator i = nameList.GetNameList ().begin ();
  //        i != nameList.GetNameList ().end ();
  //        i++) 
  //   {
  //     std::string oldName;
  //     NS_ASSERT (m_state.Update (*i, oldName) == false); // there should not be any new update
  //     Ptr<const ndn::Name> update = Create<ndn::Name> (*i);
  //     (m_onUpdate) (update);
  //   }
  //   SendSyncInterest (CurrentDigest(), 0, Seconds (1.5));
  // }

  for (std::vector<std::string>::const_iterator i = nameList.GetNameList ().begin ();
       i != nameList.GetNameList ().end ();
       i++) 
  {
    std::string oldName;
    if (m_state.Update (*i, oldName) == true) {
      hasNew = true;
      newNameList.push_back (*i);
      Ptr<const ndn::Name> update = Create<ndn::Name> (*i);
      (m_onUpdate) (update);
    }
  }

  GetDigestFromName (origInterest->GetNamePtr (), digest1, digest2);
  if (hasNew && digest2 == 0) // 
  {
    SendSyncInterest (CurrentDigest(), 0, Seconds (0.2));
  }
}

void
SyncLogic::OnTimeout (Ptr<const ndn::Interest> interest)
{
  uint64_t digest1 = 0;
  uint64_t digest2 = 0;
  GetDigestFromName (interest->GetNamePtr (), digest1, digest2);
  NS_LOG_FUNCTION (digest1 << digest2 << CurrentDigest ());

  if (digest2 != 0) // Resync Failed!
  {
    if (digest1 != m_state.IniDigest ())  // if Not recovery, then do recovery
    {
      NS_LOG_LOGIC ("Recovery " << digest2);
      SendSyncInterest (m_state.IniDigest (), digest2, Seconds (0));
    }
  }

  return;
}

void
SyncLogic::SendSyncInterest (uint64_t digest1, uint64_t digest2, Time const & time)
{
  const Ptr<ndn::Interest> interest = BuildSyncInterest (digest1, digest2);

  NS_LOG_FUNCTION (digest1 << digest2);
  
  Simulator::Schedule (time, &ApiFace::ExpressInterest, m_face, interest,
                       MakeCallback (&SyncLogic::OnData, this),
                       MakeCallback (&SyncLogic::OnTimeout, this));
  //m_transmittedInterests (interest, this, m_face);
}

void
SyncLogic::PeriodicalSyncInterest ()
{
  SendSyncInterest (CurrentDigest (), 0, Seconds (0));
  UniformVariable jitter (0.001, 0.5);
  Simulator::Schedule (Seconds (m_syncPeriod - jitter.GetValue ()), &SyncLogic::PeriodicalSyncInterest, this);
}

void
SyncLogic::PeriodicalJoinInterest ()
{
  Ptr<ndn::Interest> interest = Create<ndn::Interest> ();
  UniformVariable rand (0,std::numeric_limits<uint32_t>::max ());
  Ptr<ndn::Name> name = Create<ndn::Name> (m_syncInterestPrefix + "/join"); 

  interest->SetNonce            (rand.GetValue ());
  NS_LOG_DEBUG ("Nonce: " << interest->GetNonce ());
  interest->SetName             (name);
  interest->SetInterestLifetime (Seconds (m_joinPeriod+1)); // 1 is just a random value 
  interest->SetPitForwardingFlag (1); // Tracable
  interest->SetPitForwardingName (name);
  
  Simulator::ScheduleNow (&ApiFace::ExpressInterest, m_face, interest,
                          MakeNullCallback< void, Ptr<const Interest>, Ptr<const Data> > (),
                          MakeNullCallback< void, Ptr<const Interest> > ());
  UniformVariable jitter (0.001, 0.5);
  Simulator::Schedule (Seconds (m_joinPeriod - jitter.GetValue ()), &SyncLogic::PeriodicalJoinInterest, this);
}

void
SyncLogic::OnJoinInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest)
{
  NS_LOG_FUNCTION (interest->GetName () << interest->GetNonce ());
}

void
SyncLogic::SendUpdateInbetween (uint64_t digest1, uint64_t digest2)
{
  NS_LOG_FUNCTION (digest1 << digest2);

  Ptr<NameListHeader> nameList = Create<NameListHeader> ();
  if (m_state.GetUpdateInbetween (digest1, digest2, nameList->GetRef ()) == false)
    return;
  //nameList->Print (std::cout);
  NS_LOG_DEBUG (nameList);
  Ptr<Packet> packet = Create<Packet> ();
  packet->AddHeader (*nameList);
  Ptr<ndn::Data> data = Create<ndn::Data> (packet);

  Ptr<Name> prefix = Create<Name> (m_syncInterestPrefix);
  prefix->appendNumber (digest1);
  prefix->appendNumber (digest2);
  data->SetName (prefix);

  data->SetFreshness (Seconds (10));

  Simulator::ScheduleNow (&ApiFace::Put, m_face, data);
}

// void
// SyncLogic::SendKeepAlive (Ptr<const ndn::Name> name)
// {
//   NS_LOG_FUNCTION (name->toUri ());

//   Ptr<NameListHeader> nameList = Create<NameListHeader> ();
//   nameList->AddName (name->toUri ());
//   Ptr<Packet> packet = Create<Packet> ();
//   packet->AddHeader (*nameList);
//   Ptr<ndn::Data> data = Create<ndn::Data> (packet);

//   Ptr<Name> prefix = Create<Name> (m_syncInterestPrefix);
//   prefix->appendNumber (CurrentDigest ());
//   prefix->appendNumber (CurrentDigest ());
//   data->SetName (prefix);

//   data->SetFreshness (Seconds (1));

//   Simulator::ScheduleNow (&ApiFace::Put, m_face, data);
// }

bool
SyncLogic::Publish (Ptr<const ndn::Name> name)
{
  std::string old;

  if (m_state.Update (name->toUri (), old) == false) 
  {
    // Publish an existing name will be considered as a keepalive message
    // if (m_outstandingDigest != 0)
    // {
    //   SendKeepAlive (name);
    //   m_outstandingDigest = 0; 
    // }
    return false;
  } 
  else 
  {
    NS_LOG_FUNCTION ("Publish " << name->toUri () << " New Digest: " << CurrentDigest ());
    OnNewUpdate ();
    return true;
  }
}

void
SyncLogic::OnNewUpdate ()
{
  if (m_outstandingDigest == 0) {
    NS_LOG_LOGIC ("No Outstanding Interest");
  } else {
    SendUpdateInbetween (m_outstandingDigest, CurrentDigest ());
    m_outstandingDigest = 0;
  }
}

const Ptr<ndn::Interest>
SyncLogic::BuildSyncInterest (uint64_t digest1, uint64_t digest2)
{
  Ptr<ndn::Interest> interest = Create<ndn::Interest> ();

  UniformVariable rand (0,std::numeric_limits<uint32_t>::max ());
  
  interest->SetNonce            (rand.GetValue ());
  interest->SetInterestLifetime (Seconds (m_syncPeriod));
  interest->SetScope            (2);  

  Ptr<ndn::Name> name = Create<ndn::Name> (m_syncInterestPrefix);
  name->appendNumber (digest1);
  if (digest2 != 0) {
    name->appendNumber (digest2);
  }
  interest->SetName (name);

  if (! m_joinPeriod) // No Pit Forwarding
  {
    return interest;
  }

  // Set Pit forwarding flag and name
  if (digest2 == 0) { // This means a normal Sync Interest
    Ptr<ndn::Name> joinName = Create<ndn::Name> (m_syncInterestPrefix + "/join");
    interest->SetPitForwardingFlag (1|2); // Tracable and TraceOnly
    interest->SetPitForwardingName (joinName);
  } else { // This is a Request Sync Interest
    Ptr<ndn::Name> pfName = Create<ndn::Name> (m_syncInterestPrefix);
    pfName->appendNumber (digest2);
    interest->SetPitForwardingFlag (2); // TraceOnly
    interest->SetPitForwardingName (pfName);
  }
  
  return interest;
}

bool
SyncLogic::IsPacketDropped () const
{
  UniformVariable rand (0, 1);
  return rand.GetValue () >= m_lossRate ? false : true;
}

uint64_t
SyncLogic::GetDigestFromName (Ptr<const ndn::Name> name, uint64_t & digest1, uint64_t & digest2) const
{ 
  NS_ASSERT (name->getPrefix (m_syncInterestPrefixSize).toUri ().compare (m_syncInterestPrefix) == 0);

  digest1 = name->get (m_syncInterestPrefixSize).toNumber ();  
  if (name->size () == m_syncInterestPrefixSize + 2) {
    digest2 = name->get (m_syncInterestPrefixSize + 1).toNumber ();
  } else {
    digest2 = 0;
  }
  return digest1;
}

inline uint64_t
SyncLogic::CurrentDigest () const
{
  return m_state.GetCurrentDigest ();
}

} // namespace ndn
} // namespace ns3
