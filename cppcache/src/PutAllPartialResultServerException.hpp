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

#ifndef GEODE_PUTALLPARTIALRESULTSERVEREXCEPTION_H_
#define GEODE_PUTALLPARTIALRESULTSERVEREXCEPTION_H_

#include <geode/Serializable.hpp>
#include <geode/CacheableString.hpp>
#include "VersionedCacheableObjectPartList.hpp"
#include "PutAllPartialResult.hpp"

namespace apache {
namespace geode {
namespace client {

class PutAllPartialResultServerException;

/**
 * @brief PutAllPartialResultServerException class is used to encapsulate
 *geode PutAllPartialResultServerException in case of PutAll execution.
 **/
class _GEODE_EXPORT PutAllPartialResultServerException : public Serializable {
  /**
   * @brief public methods
   */
 public:
  PutAllPartialResultServerException(std::shared_ptr<PutAllPartialResult> result);

  PutAllPartialResultServerException();

  // Consolidate exceptions
  void consolidate(std::shared_ptr<PutAllPartialResultServerException> pre);

  void consolidate(std::shared_ptr<PutAllPartialResult> otherResult);

  std::shared_ptr<PutAllPartialResult> getResult();

  /**
   * Returns the key set in exception
   */
  std::shared_ptr<VersionedCacheableObjectPartList> getSucceededKeysAndVersions();

  std::shared_ptr<Exception> getFailure();

  // Returns there's failedKeys
  bool hasFailure();

  std::shared_ptr<CacheableKey> getFirstFailedKey();

  std::shared_ptr<CacheableString> what();

  /**
   * @brief destructor
   */
  virtual ~PutAllPartialResultServerException() {}

  /**
   * @brief constructors
   */
  PutAllPartialResultServerException(std::shared_ptr<CacheableString> msg);

  /**
   *@brief serialize this object
   * @throws IllegalStateException If this api is called from User code.
   **/
  virtual void toData(DataOutput& output) const;

  /**
   *@brief deserialize this object, typical implementation should return
   * the 'this' pointer.
   * @throws IllegalStateException If this api is called from User code.
   **/
  virtual void fromData(DataInput& input);

  /**
   *@brief Return the classId of the instance being serialized.
   * This is used by deserialization to determine what instance
   * type to create and deserialize into.
   *
   * The classId must be unique within an application suite.
   * Using a negative value may result in undefined behavior.
   * @throws IllegalStateException If this api is called from User code.
   */
  virtual int32_t classId() const;

  /**
   *@brief return the size in bytes of the instance being serialized.
   * This is used to determine whether the cache is using up more
   * physical memory than it has been configured to use. The method can
   * return zero if the user does not require the ability to control
   * cache memory utilization.
   * Note that you must implement this only if you use the HeapLRU feature.
   * @throws IllegalStateException If this api is called from User code.
   */
  virtual size_t objectSize() const;

  /**
   *@brief return the typeId byte of the instance being serialized.
   * This is used by deserialization to determine what instance
   * type to create and deserialize into.
   *
   * Note that this should not be overridden by custom implementations
   * and is reserved only for builtin types.
   */
  virtual int8_t typeId() const;

  /**
   *@brief return as std::shared_ptr<CacheableString> the Exception name
   *returned from geode sendException api.
   **/
  std::shared_ptr<CacheableString> getName() {
    const char* msg = "PutAllPartialResultServerException";
    auto str = CacheableString::create(msg);
    return str;
  }

 private:
  // never implemented.
  PutAllPartialResultServerException(
      const PutAllPartialResultServerException& other);
  void operator=(const PutAllPartialResultServerException& other);

  std::shared_ptr<CacheableString> m_message;  // error message
  std::shared_ptr<PutAllPartialResult> m_result;
};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_PUTALLPARTIALRESULTSERVEREXCEPTION_H_
