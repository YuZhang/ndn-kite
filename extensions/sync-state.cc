/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Harbin Institute of Technology, China
 *
 * Author: Yu Zhang <yuzhang@hit.edu.cn>
 */

// nlsr-lsdb.cc

#include "sync-state.h"
#include "ns3/assert.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("SyncState");

namespace ns3 {
namespace ndn {

// ========== Class SyncState ============

SyncState::SyncState ()
{ 
  m_iniDigest = 7036231242510567892; //  ns3::Hash64 ("YUZHANG")
}

SyncState::~SyncState ()
{
}

TypeId
SyncState::GetTypeId (void)
{
  static TypeId tid = TypeId ("SyncState");
  return tid;
}

TypeId
SyncState::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

std::string &
SyncState::IdSeqToName (const std::string & id, uint64_t seq, std::string & name)
{
  Ptr<ndn::Name> s = Create<ndn::Name> (id);

  s->appendNumber (seq);
  name = s->toUri ();

  return name;
}

void
SyncState::NameToIdSeq (const std::string & name, std::string & id, uint64_t & seq)
{
  NS_LOG_DEBUG ("Name: " << name);
  Ptr<ndn::Name> n = Create<ndn::Name> (name);
  id = n->getPrefix (n->size () - 1).toUri ();
  seq = n->get (-1).toNumber ();
}

uint64_t
SyncState::GetCurrentDigest () const
{
  return m_digestLog.empty ()?  m_iniDigest : m_digestLog.front ().digest;
}

bool
SyncState::IsCurrentDigest (uint64_t digest) const
{
  return (digest == GetCurrentDigest () ? true : false);
}

bool
SyncState::IsDigestInLog (uint64_t digest) const
{
  if (digest == m_iniDigest) {
    return true;
  }
  return (FindDigestInLog (digest) != m_digestLog.end ());
}

uint64_t
SyncState::GetSyncDigest () const
{
 for (DigestLog::const_iterator i = m_digestLog.begin ();
       i != m_digestLog.end ();
       i++)
  {
    if (i->counter > 0) {
      return i->digest;
    }
  }
  return m_iniDigest;
}

bool
SyncState::SetSyncDigest (uint64_t digest)
{
  DigestLog::iterator i = FindDigestInLog (digest);
  if (i != m_digestLog.end ())
  {
      i->counter ++;
      return true;
  }
  return false;
}

uint64_t
SyncState::IncrementalHash (const std::string & newName, const std::string & oldName) const
{
  if (oldName.empty ()) {
    return GetCurrentDigest() ^ ns3::Hash64(newName);
  } else {
    return GetCurrentDigest() ^ ns3::Hash64(newName) ^ ns3::Hash64 (oldName);
  }
}

void
SyncState::AddToLog (uint64_t digest, const std::string & newName, const std::string & oldName)
{
  LogTuple logTuple (digest, newName, oldName);
  m_digestLog.push_front (logTuple);
  NS_LOG_DEBUG ("digest: " << digest << "lsuName: " << newName << "oldName: " << oldName);

  if (m_digestLog.size () > m_maxLogLength) {
    m_digestLog.pop_back ();
  }
}

void
SyncState::GetAllName (NameList & nameList) const
{
  NS_LOG_FUNCTION_NOARGS ();
  std::string str;
  for (IdSeqMap::const_iterator i = m_idSeqMap.begin ();
       i != m_idSeqMap.end ();
       i++)
  {
    nameList.push_back (IdSeqToName (i->first, i->second, str));
  }
}

bool
SyncState::GetUpdateByThen (uint64_t digest, NameList & nameList) const
{
  if (digest == GetCurrentDigest ()) {
    GetAllName (nameList);
    return true;
  } 
  if (IsDigestInLog (digest) == false) {
    return false;
  }
  // To-Do: there must be more efficient way
  std::map<std::string, bool > nameMap;  // the 2nd 'bool' is only a placeholder
  for (IdSeqMap::const_iterator i = m_idSeqMap.begin ();
       i != m_idSeqMap.end ();
       i++)
  {
    std::string str;
    nameMap[IdSeqToName (i->first, i->second, str)] = true;
  }

  for (DigestLog::const_iterator i = m_digestLog.begin ();
       i != m_digestLog.end ();
       i++)
  {
    if (digest != i->digest) {
      nameMap.erase (i->newName);
      if (! i->oldName.empty ())
        nameMap[i->oldName] = true;
    } else {
      break;
    }
  }
  for (std::map<std::string, bool >::const_iterator i = nameMap.begin ();
       i != nameMap.end ();
       i++)
  {
    nameList.push_back (i->first);
  }
  return true;
}

bool
SyncState::GetUpdateInbetween (uint64_t oldDigest, uint64_t newDigest, NameList & nameList) const
{
  if (oldDigest == m_iniDigest) {
    return GetUpdateByThen (newDigest, nameList);
  }

  // To-Do: there must be more efficient way
  std::map<std::string, bool > nameMap;  // the 2nd 'bool' is only a placeholder

  DigestLog::const_iterator i = m_digestLog.begin ();
  for (; i != m_digestLog.end (); i++)
  {
    if (newDigest == i->digest) break;
  }
  DigestLog::const_iterator j = i;
  for (; j != m_digestLog.end (); j++)
  {
    if (oldDigest == j->digest) break;
    nameMap[j->newName] = true;
  }
  if (j == m_digestLog.end ()) {
    return false;
  }

  for (; j != i; j--) {
    nameMap.erase (j->oldName);
  }
  nameMap.erase (j->oldName);
  for (std::map<std::string, bool >::const_iterator i = nameMap.begin ();
       i != nameMap.end ();
       i++)
  {
    nameList.push_back (i->first);
  }
  return true;
}

// std::vector<uint64_t> _updates;
// std::vector<uint64_t> _combination;
// std::vector<uint64_t> _result;
// uint64_t _digest;

// void calculateDigest(const std::vector<uint64_t> & v) {
//   uint64_t digest = 0;
//   for (int i = 0; i < v.size(); ++i) { digest ^= v[i]; }
//   if (digest == _digest) {copy (v.begin (), v.end (), _result.begin());}
// }

// void go(int offset, int k) {
//   if (k == 0) {
//     calculateDigest (_combination);
//     return;
//   }
//   for (int i = offset; i <= _updates.size() - k; ++i) {
//     _combination.push_back(_updates[i]);
//     go(i+1, k-1);
//     _combination.pop_back();
//   }
// }

// bool
// SyncState::GetUpdateByCombination (uint64_t digest1, uint64_t digest2, NameList & nameList) const
// {
      // std::map<uint64_t, string> digestMap;
      // for (i = 1; i < _updates.size ()-1; i++)
      // {
      //   _result.clear ();
      //   go (0, i);
      //   if (!_result.empty ())
      //   {
      //     break;
      //   }
      // }
// }

bool
SyncState::Update (const std::string & newName, std::string & oldName)
{
  std::string id;
  uint64_t seq;
  NameToIdSeq (newName, id, seq);

  oldName.clear ();

  IdSeqMap::iterator i = m_idSeqMap.find (id);
  if (i != m_idSeqMap.end ()) {
    if (seq > i->second) {
      IdSeqToName (i->first, i->second, oldName);
      i->second = seq;
    } else {
      return false;
    }
  } else {
    m_idSeqMap[id] = seq;
  }
  AddToLog (IncrementalHash (newName, oldName), newName, oldName);
  return true;
}

DigestLog::const_iterator
SyncState::FindDigestInLog (uint64_t digest) const
{
  DigestLog::const_iterator i = m_digestLog.begin ();
  for (;
       i != m_digestLog.end ();
       i++)
  {
    if (digest == i->digest) {
      break;
    }
  }
  return i;
}

DigestLog::iterator
SyncState::FindDigestInLog (uint64_t digest)
{
  DigestLog::iterator i = m_digestLog.begin ();
  for (;
       i != m_digestLog.end ();
       i++)
  {
    if (digest == i->digest) {
      break;
    }
  }
  return i;
}

void
SyncState::SetMaxLogLength (uint32_t len)
{
  m_maxLogLength = len;
}

uint64_t
SyncState::IniDigest () const
{
  return m_iniDigest;
}

} // namespace ndn
} // namespace ns3