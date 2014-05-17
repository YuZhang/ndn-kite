/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

#include "interest-pit-forwarding.h"

#include "ns3/ndn-interest.h"
#include "ns3/ndn-pit.h"
#include "ns3/ndn-pit-entry.h"

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/boolean.h"

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
namespace ll = boost::lambda;

namespace ns3 {
namespace ndn {
namespace fw {

NS_OBJECT_ENSURE_REGISTERED (PitForwarding);

LogComponent PitForwarding::g_log = LogComponent (PitForwarding::GetLogName ().c_str ());

std::string
PitForwarding::GetLogName ()
{
  return super::GetLogName ()+".PitForwarding";
}

TypeId PitForwarding::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::ndn::fw::PitForwarding")
    .SetGroupName ("Ndn")
    .SetParent <ForwardingStrategy> ()
    .AddConstructor <PitForwarding> ()
    ;
  return tid;
}

PitForwarding::PitForwarding ()
{
  // m_pft = Create<Pft> ();
}


// ToDo: the current implementation of Pull is the most `simple and stupid'
int
PitForwarding::Pull (Ptr<Face> inFace,
                     Ptr<Face> outFace,  // useful for sending
                     Ptr<const Interest> interest,
                     Ptr<pit::Entry> pitEntry)
{
  int propagatedCount = 0;
  if (((interest->GetPitForwardingFlag () & 1) != 1))   // if NOT Tracable
    {
      return 0;
    }

  for (Ptr<pit::Entry> pfEntry = m_pit->Begin(); pfEntry != m_pit->End(); pfEntry = m_pit->Next(pfEntry))
    {
      Ptr<const Interest> pfInterest = pfEntry->GetInterest (); 
      if ( (pfInterest->GetPitForwardingNamePtr () != 0) &&
           (interest->GetName () == pfInterest->GetPitForwardingName ()) )
        {
          pit::Entry::out_iterator face = pfEntry->GetOutgoing ().find (inFace);
          if (face == pfEntry-> GetOutgoing ().end ()) // Not yet being sent to inFace
            {
              //pit::Entry::in_iterator face = pfEntry->GetIncoming ().begin (); // First incoming face
              
              NS_LOG_INFO ("Interest Pulled by " << interest->GetName ());
              NS_LOG_DEBUG ("Inface: " << *outFace << " Outface: " << *inFace << " pfInterest" << *pfInterest);
              if (TrySendOutInterest (outFace, inFace, pfInterest, pfEntry))
                {
                  NS_LOG_DEBUG ("Succeed: Interest Pulled to " << *inFace);
                  propagatedCount++;
                }
              else
                {
                  NS_LOG_DEBUG ("Failed: Interest Pulled to " << *inFace);
                }
            }
        }
    }
  return propagatedCount;
}

bool
PitForwarding::DoPitForwarding (Ptr<Face> inFace,
                                Ptr<const Interest> interest,
                                Ptr<pit::Entry> pitEntry)
{
  int propagatedCount = 0;

  if ((interest->GetPitForwardingFlag () & 1) == 1) // If Traceable 
    {
      // To-do: Pit Forwarding Table
    }

  if (interest->GetPitForwardingNamePtr () == 0)
    {
      return false;
    } 

  NS_LOG_FUNCTION (this << interest->GetName ());
  NS_LOG_INFO ("PF Name " << interest->GetPitForwardingName () << " Face: " << *inFace);
  Ptr<pit::Entry> pfEntry = m_pit->Find (interest->GetPitForwardingName ());
  if (pfEntry == 0)
    {
      return false;
    }

  Ptr<const Interest> pfInterest = pfEntry->GetInterest (); 
  // Issue: for Interests with the same name, only the first one is here
  if ((pfInterest->GetPitForwardingFlag () & 1) != 1) // If not Tracable
    {
      return false;
    }
  
  // this single Interest will fan out to all incomming faces except the one through which the Interest came in 
  Ptr<Face> outFace = 0;
  for (pit::Entry::in_iterator face = pfEntry->GetIncoming ().begin ();
       face != pfEntry->GetIncoming ().end ();
       face++)
    {
      if (inFace == face->m_face)
        {
          NS_LOG_DEBUG ("Same Face " << *face->m_face);
          continue;
        }
      if (TrySendOutInterest (inFace, face->m_face, interest, pitEntry))
        {
          NS_LOG_DEBUG ("Propagated to " << *face->m_face);
          outFace = face->m_face;
          propagatedCount++;
        }
      else
        {
          NS_LOG_DEBUG ("Failed: Propagated to " << *face->m_face);
        }
    }

  // mutual Interest forwarding: only support sync app
  // To-Do: A fully functional mutual forwarding needs Pit Forwarding Table
  // if ((propagatedCount >0) &&
  //     ((interest->GetPitForwardingFlag () & 1) == 1) &&  // if Tracable
  //     (pfInterest->GetPitForwardingNamePtr () != 0) &&
  //     (interest->GetName () == pfInterest->GetPitForwardingName ())) 
  //   {
  //     pit::Entry::out_iterator face = pfEntry->GetOutgoing ().find (inFace);
  //     if (face == pfEntry-> GetOutgoing ().end ())
  //       {
  //         NS_LOG_INFO ("Mutual Interest forwarding on " << interest->GetName ());
  //         NS_LOG_DEBUG ("Inface: " << *inFace << " Outface: " << *outFace);
  //         if (TrySendOutInterest (outFace, inFace, pfInterest, pfEntry))
  //           {
  //             NS_LOG_DEBUG ("Mutual Interest Propagated to " << *inFace);
  //             propagatedCount++;
  //           }
  //         else
  //           {
  //             NS_LOG_DEBUG ("Failed: Mutual Interest Propagated to " << *inFace);
  //           }
  //       }
  //   }
  if (propagatedCount >0)
    {
      propagatedCount += Pull (inFace, outFace, interest, pitEntry);
    }

  NS_LOG_INFO ("Propagated to " << propagatedCount << " faces");
  return propagatedCount > 0;
}

// This is copied from Flooding.cc
bool
PitForwarding::DoFlooding (Ptr<Face> inFace,
                           Ptr<const Interest> interest,
                           Ptr<pit::Entry> pitEntry)
{
  NS_LOG_FUNCTION (this << interest->GetName ());

  int propagatedCount = 0;

  BOOST_FOREACH (const fib::FaceMetric &metricFace, pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ())
    {
      //NS_LOG_DEBUG ("Trying " << boost::cref(metricFace));
      if (metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_RED) // all non-read faces are in the front of the list
        break;

      if (!TrySendOutInterest (inFace, metricFace.GetFace (), interest, pitEntry))
        {
          NS_LOG_DEBUG ("Failed: Propagated to " << *metricFace.GetFace ());
          continue;
        }
      NS_LOG_DEBUG ("Propagated to " << *metricFace.GetFace ());
      propagatedCount++;
    }

  NS_LOG_INFO ("Propagated to " << propagatedCount << " faces");
  return propagatedCount > 0;
}


bool
PitForwarding::DoPropagateInterest (Ptr<Face> inFace,
                                    Ptr<const Interest> interest,
                                    Ptr<pit::Entry> pitEntry)
{
  // std::cerr << "Print PIT:" << std::endl;
  // std::cerr << *m_pit;

  // Current implementation is a hack in forwarding strategy 
  // To-Do: implement prefix-specific strategy inside daemon

  // Join Interest forwarded by PIT and FIB
  // An Interest will not be sent back to its incoming face
  // On a router, a PIT-forwarding-only Interest will be forwarded to faces with default '/' route
  NS_LOG_FUNCTION (this << interest->GetName ());
  NS_LOG_INFO ("PF Name: " << interest->GetPitForwardingName () << " Face: " << *inFace << " Flag: " << (interest->GetPitForwardingFlag ()+60) );

  bool didPitForwarding = DoPitForwarding (inFace, interest, pitEntry);
  //if (didPitForwarding && (interest->GetPitForwardingFlag () & 2) == 2) // TraceOnly

  // !!! This is NOT the default behavior of TraceOnly

  if ((interest->GetPitForwardingFlag () & 2) == 2) // TraceOnly
    {
      return didPitForwarding;
    }
  bool didFlooding = DoFlooding (inFace, interest, pitEntry);      

  return didPitForwarding || didFlooding;
}

// to bypass Suppression
bool
PitForwarding::ShouldSuppressIncomingInterest (Ptr<Face> inFace,
                                               Ptr<const Interest> interest,
                                               Ptr<pit::Entry> pitEntry)
{
  return false;
}

// to bypass GetMaxRetxCount
bool
PitForwarding::CanSendOutInterest (Ptr<Face> inFace,
                                   Ptr<Face> outFace,
                                   Ptr<const Interest> interest,
                                   Ptr<pit::Entry> pitEntry)
{
  if (outFace == inFace)
    {
     return false; // same face as incoming, don't forward
    }
  return true;
}

} // namespace fw
} // namespace ndn
} // namespace ns3
