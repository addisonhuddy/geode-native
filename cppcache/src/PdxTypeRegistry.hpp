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

#pragma once

#ifndef GEODE_PDXTYPEREGISTRY_H_
#define GEODE_PDXTYPEREGISTRY_H_

#include <unordered_map>
#include <map>

#include <ace/ACE.h>
#include <ace/Recursive_Thread_Mutex.h>

#include <geode/PdxSerializable.hpp>
#include <geode/Cache.hpp>
#include <geode/internal/functional.hpp>

#include "PdxRemotePreservedData.hpp"
#include "ReadWriteLock.hpp"
#include "PdxType.hpp"
#include "EnumInfo.hpp"
#include "PreservedDataExpiryHandler.hpp"
#include "ExpiryTaskManager.hpp"

namespace apache {
namespace geode {
namespace client {

struct PdxTypeLessThan {
  bool operator()(std::shared_ptr<PdxType> const& n1,
                  std::shared_ptr<PdxType> const& n2) const {
    // call to PdxType::operator <()
    return *n1 < *n2;
  }
};

typedef std::map<int32_t, std::shared_ptr<PdxType>> TypeIdVsPdxType;
typedef std::map<std::string, std::shared_ptr<PdxType>> TypeNameVsPdxType;
typedef std::unordered_map<std::shared_ptr<PdxSerializable>,
                           std::shared_ptr<PdxRemotePreservedData>,
                           dereference_hash<std::shared_ptr<CacheableKey>>,
                           dereference_equal_to<std::shared_ptr<CacheableKey>>>
    PreservedHashMap;
typedef std::map<std::shared_ptr<PdxType>, int32_t, PdxTypeLessThan>
    PdxTypeToTypeIdMap;

class _GEODE_EXPORT PdxTypeRegistry
    : public std::enable_shared_from_this<PdxTypeRegistry> {
 private:
  CacheImpl* cache;

  TypeIdVsPdxType typeIdToPdxType;

  TypeIdVsPdxType remoteTypeIdToMergedPdxType;

  TypeNameVsPdxType localTypeToPdxType;

  PdxTypeToTypeIdMap pdxTypeToTypeIdMap;

  // TODO:: preserveData need to be of type WeakHashMap
  PreservedHashMap preserveData;

  mutable ACE_RW_Thread_Mutex g_readerWriterLock;

  mutable ACE_RW_Thread_Mutex g_preservedDataLock;

  bool pdxIgnoreUnreadFields;

  bool pdxReadSerialized;

  std::shared_ptr<CacheableHashMap> enumToInt;

  std::shared_ptr<CacheableHashMap> intToEnum;

 public:
  PdxTypeRegistry(CacheImpl* cache);
  PdxTypeRegistry(const PdxTypeRegistry& other) = delete;

  virtual ~PdxTypeRegistry();

  // test hook
  size_t testNumberOfPreservedData() const;

  void addPdxType(int32_t typeId, std::shared_ptr<PdxType> pdxType);

  std::shared_ptr<PdxType> getPdxType(int32_t typeId) const;

  void addLocalPdxType(const std::string& localType,
                       std::shared_ptr<PdxType> pdxType);

  // newly added
  std::shared_ptr<PdxType> getLocalPdxType(const std::string& localType) const;

  void setMergedType(int32_t remoteTypeId, std::shared_ptr<PdxType> mergedType);

  std::shared_ptr<PdxType> getMergedType(int32_t remoteTypeId) const;

  void setPreserveData(std::shared_ptr<PdxSerializable> obj,
                       std::shared_ptr<PdxRemotePreservedData> preserveDataPtr,
                       ExpiryTaskManager& expiryTaskManager);

  std::shared_ptr<PdxRemotePreservedData> getPreserveData(
      std::shared_ptr<PdxSerializable> obj) const;

  void clear();

  int32_t getPDXIdForType(const std::string& type, const std::string& poolname,
                          std::shared_ptr<PdxType> nType, bool checkIfThere);

  bool getPdxIgnoreUnreadFields() const { return pdxIgnoreUnreadFields; }

  void setPdxIgnoreUnreadFields(bool value) { pdxIgnoreUnreadFields = value; }

  void setPdxReadSerialized(bool value) { pdxReadSerialized = value; }

  bool getPdxReadSerialized() const { return pdxReadSerialized; }

  inline const PreservedHashMap& getPreserveDataMap() const {
    return preserveData;
  };

  int32_t getEnumValue(std::shared_ptr<EnumInfo> ei);

  std::shared_ptr<EnumInfo> getEnum(int32_t enumVal);

  int32_t getPDXIdForType(std::shared_ptr<PdxType> nType,
                          const std::string& poolname);

  ACE_RW_Thread_Mutex& getPreservedDataLock() const {
    return g_preservedDataLock;
  }
};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_PDXTYPEREGISTRY_H_
