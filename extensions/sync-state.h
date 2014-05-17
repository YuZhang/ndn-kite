/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

// sync-state.h

#ifndef SYNC_STATE_H
#define SYNC_STATE_H

#include "ns3/header.h"
#include "ns3/ndn-data.h"

namespace ns3 {
namespace ndn {

/// ========== Class NlsrSync ============

struct LogTuple
{
  uint64_t digest;
  std::string newName;
  std::string oldName;
  uint32_t counter;

  LogTuple (uint64_t d, std::string n, std::string o)
  : digest (d), newName (n), oldName (o), counter (0)
  {}

  LogTuple ()
  : counter (0)
  {}
};

typedef std::vector<std::string> NameList;
typedef std::map<std::string, uint64_t> IdSeqMap;
typedef std::list<LogTuple> DigestLog;

class SyncState {

public:

  SyncState ();

  virtual ~SyncState ();

  static TypeId
  GetTypeId (void);

  virtual TypeId
  GetInstanceTypeId (void) const;

  static std::string &
  IdSeqToName (const std::string & id, uint64_t seq, std::string & name);

  static void
  NameToIdSeq (const std::string & name, std::string & id, uint64_t & seq);

  void
  SetMaxLogLength (uint32_t len);

  uint64_t
  IniDigest (void) const;

  uint64_t
  GetCurrentDigest () const;

  bool
  IsDigestInLog (uint64_t digest) const;

  uint64_t
  GetSyncDigest () const;

  bool
  SetSyncDigest (uint64_t digest);

  bool
  GetUpdateInbetween (uint64_t oldDigest, uint64_t newDigest, NameList & nameList) const;

  bool
  Update (const std::string & newName, std::string & oldName);

private:

  bool
  IsCurrentDigest (uint64_t digest) const;

  void
  AddToLog (uint64_t digest, const std::string & newName, const std::string & oldName);

  uint64_t
  IncrementalHash (const std::string & newName, const std::string & oldName) const;

  DigestLog::const_iterator
  FindDigestInLog (uint64_t digest) const;

  DigestLog::iterator
  FindDigestInLog (uint64_t digest);

  void
  GetAllName (NameList & nameList) const;

  bool
  GetUpdateByThen (uint64_t digest, NameList & nameList) const;

  bool
  GetUpdateByCombination (uint64_t digest1, uint64_t digest2, NameList & nameList) const;

private:
  uint64_t m_iniDigest;       /// Initial digest for empty log
  uint32_t m_maxLogLength;    /// Maxmimum length of digest log
  DigestLog m_digestLog;
  IdSeqMap m_idSeqMap;
}; // Class SyncState


} // namespace ndn
} // namespace ns3

#endif /* SYNC_STATE_H */
