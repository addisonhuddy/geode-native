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

#ifndef GEODE_DATAINPUTINTERNAL_H_
#define GEODE_DATAINPUTINTERNAL_H_

#include <geode/DataInput.hpp>

namespace apache {
namespace geode {
namespace client {

class DataInputInternal : public DataInput {
 public:
  DataInputInternal(const uint8_t* buffer, int32_t len)
      : DataInput(buffer, len, nullptr) {}

  DataInputInternal(const uint8_t* buffer, int32_t len, const CacheImpl* cache)
      : DataInput(buffer, len, cache) {}

  virtual const Cache* getCache() override {
    throw FatalInternalException("DataInputInternal does not have a Cache");
  }

  inline static const std::string& getPoolName(const DataInput& dataInput) {
    return dataInput.getPoolName();
  }

  inline static void setPoolName(DataInput& dataInput,
                                 const std::string& poolName) {
    return dataInput.setPoolName(poolName);
  }
};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_DATAINPUTINTERNAL_H_
