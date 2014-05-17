/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

#ifndef INTEREST_PIT_FORWARDING_H
#define INTEREST_PIT_FORWARDING_H

#include "ns3/ndn-forwarding-strategy.h"
#include "ns3/log.h"
#include "ns3/ndnSIM-module.h"

#include "ns3/ndnSIM/model/pit/ndn-pit-impl.h"
#include "ns3/ndnSIM/utils/trie/fifo-policy.h"

namespace ns3 {
namespace ndn {
namespace fw {

/**
 * @ingroup ndn-fw
 * @brief Interest Forwarding via PIT
 *
 * An Interest I(A) will be forwarded according to another Interest I(B) in PIT, if
 * I(A)'s PitForwardingName is the same with I(B)'s Name, and I(B)'s PitForwardingFlag
 * is 1.
 *
 * Otherwise, Interests will be forwarded in Flooding strategy:
 * Interests will be forwarded to all available faces available for a route (FIB entry).
 * If there are no available GREEN or YELLOW faces, interests is dropped.
 *
 * Usage example:
 * @code
 *     ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::PitForwarding");
 *     ...
 *     ndnHelper.Install (nodes);
 * @endcode
 */
class PitForwarding :
    public ForwardingStrategy
{
private:
  typedef ForwardingStrategy super;

public:
  static TypeId
  GetTypeId ();

  /**
   * @brief Helper function to retrieve logging name for the forwarding strategy
   */
  static std::string
  GetLogName ();
  
  /**
   * @brief Default constructor
   */
  PitForwarding ();

protected:
  // inherited from  ForwardingStrategy
  virtual bool
  DoPropagateInterest (Ptr<Face> inFace,
                       Ptr<const Interest> interest,
                       Ptr<pit::Entry> pitEntry);

  virtual bool
  ShouldSuppressIncomingInterest (Ptr<Face> inFace,
                       Ptr<const Interest> interest,
                       Ptr<pit::Entry> pitEntry);

  virtual bool
  CanSendOutInterest (Ptr<Face> inFace,
                      Ptr<Face> outFace,
                      Ptr<const Interest> interest,
                      Ptr<pit::Entry> pitEntry);

  virtual bool
  DoPitForwarding (Ptr<Face> inFace,
                   Ptr<const Interest> interest,
                   Ptr<pit::Entry> pitEntry);

  virtual bool
  DoFlooding (Ptr<Face> inFace,
              Ptr<const Interest> interest,
              Ptr<pit::Entry> pitEntry);

  virtual int
  Pull (Ptr<Face> inFace,
        Ptr<Face> outFace,
        Ptr<const Interest> interest,
        Ptr<pit::Entry> pitEntry);

protected:
  static LogComponent g_log;

// To-Do: Pit Forwarding Table for 
// typedef ndnSIM::fifo_policy_traits policy;
// typedef pit::PitImpl<policy> Pft;
//   Ptr< Pft >  m_pft;         // Pit Forwarding Table
};

} // namespace fw
} // namespace ndn
} // namespace ns3

#endif // INTEREST_PIT_FORWARDING_H