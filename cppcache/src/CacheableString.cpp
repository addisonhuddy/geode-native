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

#include <codecvt>
#include <locale>
#include <cwchar>
#include <cstdlib>

#include <ace/ACE.h>
#include <ace/OS.h>

#include <geode/CacheableString.hpp>
#include <geode/DataOutput.hpp>
#include <geode/DataInput.hpp>
#include <geode/ExceptionTypes.hpp>
#include <geode/GeodeTypeIds.hpp>

#include "DataOutputInternal.hpp"
#include "SerializationRegistry.hpp"
#include "Utils.hpp"
#include "util/string.hpp"

namespace apache {
namespace geode {
namespace client {

void CacheableString::toData(DataOutput& output) const {
  if (m_type == GeodeTypeIds::CacheableASCIIString) {
    output.writeAscii(m_str);
  } else if (m_type == GeodeTypeIds::CacheableString) {
    output.writeJavaModifiedUtf8(m_str);
  } else if (m_type == GeodeTypeIds::CacheableASCIIStringHuge) {
    output.writeAsciiHuge(m_str);
  } else if (m_type == GeodeTypeIds::CacheableStringHuge) {
    output.writeUtf16Huge(m_str);
  }
}

void CacheableString::fromData(DataInput& input) {
  if (m_type == GeodeTypeIds::CacheableASCIIString) {
    input.readAscii(m_str);
  } else if (m_type == GeodeTypeIds::CacheableString) {
    input.readJavaModifiedUtf8(m_str);
  } else if (m_type == GeodeTypeIds::CacheableASCIIStringHuge) {
    input.readAsciiHuge(m_str);
  } else if (m_type == GeodeTypeIds::CacheableStringHuge) {
    input.readUtf16Huge(m_str);
  }
}

std::shared_ptr<Serializable> CacheableString::createDeserializable() {
  return std::make_shared<CacheableString>(GeodeTypeIds::CacheableASCIIString);
}

std::shared_ptr<Serializable> CacheableString::createDeserializableHuge() {
  return std::make_shared<CacheableString>(
      GeodeTypeIds::CacheableASCIIStringHuge);
}

std::shared_ptr<Serializable> CacheableString::createUTFDeserializable() {
  return std::make_shared<CacheableString>(GeodeTypeIds::CacheableString);
}

std::shared_ptr<Serializable> CacheableString::createUTFDeserializableHuge() {
  return std::make_shared<CacheableString>(GeodeTypeIds::CacheableStringHuge);
}

std::shared_ptr<CacheableString> CacheableString::create(
    const std::u16string& value) {
  return std::make_shared<CacheableString>(to_utf8(value));
}

std::shared_ptr<CacheableString> CacheableString::create(
    std::u16string&& value) {
  return std::make_shared<CacheableString>(to_utf8(value));
}

std::shared_ptr<CacheableString> CacheableString::create(
    const std::u32string& value) {
  return std::make_shared<CacheableString>(to_utf8(value));
}

std::shared_ptr<CacheableString> CacheableString::create(
    std::u32string&& value) {
  return std::make_shared<CacheableString>(to_utf8(value));
}

int32_t CacheableString::classId() const { return 0; }

int8_t CacheableString::typeId() const { return m_type; }

bool CacheableString::operator==(const CacheableKey& other) const {
  // use typeId() call instead of m_type to work correctly with derived
  // classes like CacheableFileName
  int8_t thisType = typeId();
  int8_t otherType = other.typeId();
  if (thisType != otherType) {
    if (!(thisType == GeodeTypeIds::CacheableASCIIString ||
          thisType == GeodeTypeIds::CacheableString ||
          thisType == GeodeTypeIds::CacheableASCIIStringHuge ||
          thisType == GeodeTypeIds::CacheableStringHuge) ||
        !(otherType == GeodeTypeIds::CacheableASCIIString ||
          otherType == GeodeTypeIds::CacheableString ||
          otherType == GeodeTypeIds::CacheableASCIIStringHuge ||
          otherType == GeodeTypeIds::CacheableStringHuge)) {
      return false;
    }
  }

  auto&& otherStr = static_cast<const CacheableString&>(other);
  return m_str == otherStr.m_str;
}

int32_t CacheableString::hashcode() const {
  if (m_hashcode == 0) {
    m_hashcode = geode_hash<std::string>{}(m_str);
  }
  return m_hashcode;
}

bool CacheableString::isAscii(const std::string& str) {
  for (auto&& c : str) {
    if (c & 0x80) {
      return false;
    }
  }
  return true;
}


CacheableString::~CacheableString() {}

size_t CacheableString::objectSize() const {
  auto size = sizeof(CacheableString) +
              sizeof(std::string::value_type) * m_str.capacity();
  return size;
}

std::string CacheableString::toString() const { return m_str; }

}  // namespace client
}  // namespace geode
}  // namespace apache
