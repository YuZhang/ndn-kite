/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

#ifndef SERVER_APP_H
#define SERVER_APP_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/ndnSIM/ndn.cxx/ndn-api-face.h"


namespace ns3 {
namespace ndn {

class ServerApp : public Application
{

public:
  ServerApp ();

  static TypeId
  GetTypeId ();
  
  virtual void
  StartApplication ();

  virtual void
  StopApplication ();

  void
  OnInterest (Ptr<const ndn::Name> origName, Ptr<const ndn::Interest> interest);

  void
  OnData (Ptr<const Interest> origInterest, Ptr<const ndn::Data> data);

  void
  OnTimeout (Ptr<const ndn::Interest> interest);

  void
  SendInterest (int seq);

private:
  uint32_t m_seq;
  uint32_t m_credit;
  std::string m_serverPrefix;
  Ptr<ndn::Name> m_uploadName;
  Ptr<ApiFace> m_face;
  double m_requestPeriod;
  std::string m_mobilePrefix;
  uint32_t m_mobilePrefixSize;
};

} // namespace nlsr
} // namespace ns3

#endif // SERVER_APP_H
