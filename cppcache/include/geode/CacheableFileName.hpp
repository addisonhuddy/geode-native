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

#ifndef GEODE_CACHEABLEFILENAME_H_
#define GEODE_CACHEABLEFILENAME_H_



#include <memory>

#include "CacheableKey.hpp"
#include "CacheableString.hpp"
#include "internal/geode_globals.hpp"

/** @file
 */

namespace apache {
namespace geode {
namespace client {

/**
 * Implement an immutable wrapper for filenames that can serve as a
 * distributable filename object for caching as both key and value.
 */
class DataInput;
class DataOutput;
class Serializable;

class _GEODE_EXPORT CacheableFileName : public CacheableString {
 public:
  /**
   *@brief serialize this object
   **/
  virtual void toData(DataOutput& output) const override;

  /**
   *@brief deserialize this object
   * Throw IllegalArgumentException if the packed CacheableString is not less
   * than 64K bytes.
   **/
  virtual void fromData(DataInput& input) override;

  /**
   *@brief Return the classId of the instance being serialized.
   * This is used by deserialization to determine what instance
   * type to create and deserialize into.
   */
  virtual int32_t classId() const override;

  /**
   *@brief return the typeId byte of the instance being serialized.
   * This is used by deserialization to determine what instance
   * type to create and deserialize into.
   */
  virtual int8_t typeId() const override;

  /**
   * @brief creation function for filenames.
   */
  static std::shared_ptr<Serializable> createDeserializable() {
    return std::make_shared<CacheableFileName>();
  }

  /**
   * Factory method for creating an instance of CacheableFileName from a
   * C string optionally given the length.
   */
  static std::shared_ptr<CacheableFileName> create(const std::string& value) {
    return std::make_shared<CacheableFileName>(value);
  }

  /** return the hashcode for this key. */
  virtual int32_t hashcode() const override;

 protected:
  _GEODE_FRIEND_STD_SHARED_PTR(CacheableFileName)

  /** Default constructor. */
  inline CacheableFileName() : CacheableString(), m_hashcode(0) {}
  inline CacheableFileName(const std::string& value) : CacheableString(value) {}
  inline CacheableFileName(std::string&& value)
      : CacheableString(std::move(value)) {}

 private:
  // never implemented.
  void operator=(const CacheableFileName& other) = delete;
  CacheableFileName(const CacheableFileName& other) = delete;

 private:
  mutable int m_hashcode;
};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_CACHEABLEFILENAME_H_
