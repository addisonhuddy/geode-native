#pragma once

#ifndef GEODE_TRACKEDMAPENTRY_H_
#define GEODE_TRACKEDMAPENTRY_H_

/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "MapEntry.hpp"

namespace apache {
namespace geode {
namespace client {

class TrackedMapEntry : public MapEntry {
 public:
  // Constructor should be invoked only when starting the tracking
  // of a MapEntry, so m_trackingNumber is initialized with 1.
  inline TrackedMapEntry(const std::shared_ptr<MapEntryImpl>& entry,
                         int trackingNumber, int updateCount)
      : m_entry(const_cast<std::shared_ptr<MapEntryImpl>&>(entry)),
        m_trackingNumber(trackingNumber),
        m_updateCount(updateCount) {}

  virtual ~TrackedMapEntry() {}

  virtual std::shared_ptr<MapEntryImpl> getImplPtr() { return m_entry; }

  virtual int addTracker(std::shared_ptr<MapEntry>& newEntry) {
    ++m_trackingNumber;
    return m_updateCount;
  }

  virtual std::pair<bool, int> removeTracker() {
    if (m_trackingNumber > 0) {
      --m_trackingNumber;
    }
    if (m_trackingNumber == 0) {
      m_updateCount = 0;
      return std::make_pair(true, 0);
    }
    return std::make_pair(false, m_trackingNumber);
  }

  virtual int incrementUpdateCount(std::shared_ptr<MapEntry>& newEntry) {
    return ++m_updateCount;
  }

  virtual int getTrackingNumber() const { return m_trackingNumber; }

  virtual int getUpdateCount() const { return m_updateCount; }

  virtual void getKey(std::shared_ptr<CacheableKey>& result) const;
  virtual void getValue(std::shared_ptr<Cacheable>& result) const;
  virtual void setValue(const std::shared_ptr<Cacheable>& value);
  virtual LRUEntryProperties& getLRUProperties();
  virtual ExpEntryProperties& getExpProperties();
  virtual VersionStamp& getVersionStamp();
  virtual void cleanup(const CacheEventFlags eventFlags);

 private:
  std::shared_ptr<MapEntryImpl> m_entry;
  int m_trackingNumber;
  int m_updateCount;
};
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_TRACKEDMAPENTRY_H_
