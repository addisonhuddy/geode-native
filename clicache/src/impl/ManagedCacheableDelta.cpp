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


#include "../begin_native.hpp"
#include <GeodeTypeIdsImpl.hpp>
#include "../end_native.hpp"

#include "ManagedCacheableDelta.hpp"
#include "../DataInput.hpp"
#include "../DataOutput.hpp"
#include "../CacheableString.hpp"
#include "../ExceptionTypes.hpp"
#include "SafeConvert.hpp"
#include "CacheResolver.hpp"

using namespace System;

namespace apache
{
  namespace geode
  {
    namespace client
    {

      void ManagedCacheableDeltaGeneric::toData(DataOutput& output) const
      {
        try {
          System::UInt32 pos = (int)output.getBufferLength();
          auto cache = CacheResolver::Lookup(output.getCache());
          Apache::Geode::Client::DataOutput mg_output(&output, true, cache);
          m_managedSerializableptr->ToData(%mg_output);
          //this will move the cursor in c++ layer
          mg_output.WriteBytesToUMDataOutput();
          auto tmp = const_cast<ManagedCacheableDeltaGeneric*>(this);
          tmp->m_objectSize = (int)(output.getBufferLength() - pos);
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
      }

     void ManagedCacheableDeltaGeneric::fromData(DataInput& input)
      {
        try {
          int pos = input.getBytesRead();
          auto cache = CacheResolver::Lookup(input.getCache());
          Apache::Geode::Client::DataInput mg_input(&input, true, cache);
          m_managedSerializableptr->FromData(%mg_input);

          //this will move the cursor in c++ layer
          input.advanceCursor(mg_input.BytesReadInternally);

          m_objectSize = input.getBytesRead() - pos;

          if (m_hashcode == 0)
            m_hashcode = m_managedptr->GetHashCode();

        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
      }

     size_t ManagedCacheableDeltaGeneric::objectSize() const
      {
        try {
          auto ret = m_managedSerializableptr->ObjectSize;
          if (ret > m_objectSize)
            return ret;
          else
            return m_objectSize;
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return 0;
      }

      System::Int32 ManagedCacheableDeltaGeneric::classId() const
      {
        System::UInt32 classId;
        try {
          classId = m_managedSerializableptr->ClassId;
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return (classId >= 0x80000000 ? 0 : classId);
      }

      int8_t ManagedCacheableDeltaGeneric::typeId() const
      {
        try {
          System::UInt32 classId = m_classId;
          if (classId >= 0x80000000) {
            return (int8_t)((classId - 0x80000000) % 0x20000000);
          }
          else if (classId <= 0x7F) {
            return (int8_t)GeodeTypeIdsImpl::CacheableUserData;
          }
          else if (classId <= 0x7FFF) {
            return (int8_t)GeodeTypeIdsImpl::CacheableUserData2;
          }
          else {
            return (int8_t)GeodeTypeIdsImpl::CacheableUserData4;
          }
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return 0;
      }

      int8_t ManagedCacheableDeltaGeneric::DSFID() const
      {
        // convention that [0x8000000, 0xa0000000) is for FixedIDDefault,
        // [0xa000000, 0xc0000000) is for FixedIDByte,
        // [0xc0000000, 0xe0000000) is for FixedIDShort
        // and [0xe0000000, 0xffffffff] is for FixedIDInt
        // Note: depends on fact that FixedIDByte is 1, FixedIDShort is 2
        // and FixedIDInt is 3; if this changes then correct this accordingly
        System::UInt32 classId = m_managedSerializableptr->ClassId;
        if (classId >= 0x80000000) {
          return (int8_t)((classId - 0x80000000) / 0x20000000);
        }
        return 0;
      }

      bool ManagedCacheableDeltaGeneric::hasDelta() const
      {
        return m_managedptr->HasDelta();
      }

      void ManagedCacheableDeltaGeneric::toDelta(DataOutput& output) const
      {
        try {
          auto cache = CacheResolver::Lookup(output.getCache());
          Apache::Geode::Client::DataOutput mg_output(&output, true, cache);
          m_managedptr->ToDelta(%mg_output);
          //this will move the cursor in c++ layer
          mg_output.WriteBytesToUMDataOutput();
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
      }

      void ManagedCacheableDeltaGeneric::fromDelta(DataInput& input)
      {
        try {
          auto cache = CacheResolver::Lookup(input.getCache());
          Apache::Geode::Client::DataInput mg_input(&input, true, cache);
          m_managedptr->FromDelta(%mg_input);

          //this will move the cursor in c++ layer
          input.advanceCursor(mg_input.BytesReadInternally);

          m_hashcode = m_managedptr->GetHashCode();
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
      }

      std::shared_ptr<Delta> ManagedCacheableDeltaGeneric::clone() const
      {
        try {
          if (auto cloneable = dynamic_cast<ICloneable^>((
            Apache::Geode::Client::IGeodeDelta^) m_managedptr)) {
            auto Mclone = 
              dynamic_cast<Apache::Geode::Client::IGeodeSerializable^>(cloneable->Clone());
            return std::shared_ptr<Delta>(static_cast<ManagedCacheableDeltaGeneric*>(
              SafeMSerializableConvertGeneric(Mclone)));
          }
          else {
            return Delta::clone();
          }
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return nullptr;
      }

      bool ManagedCacheableDeltaGeneric::operator ==(const apache::geode::client::CacheableKey& other) const
      {
        try {
          // now checking classId(), typeId(), DSFID() etc. will be much more
          // expensive than just a dynamic_cast
          const ManagedCacheableDeltaGeneric* p_other =
            dynamic_cast<const ManagedCacheableDeltaGeneric*>(&other);
          if (p_other != NULL) {
            return m_managedptr->Equals(p_other->ptr());
          }
          return false;
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return false;
      }

      bool ManagedCacheableDeltaGeneric::operator == (const ManagedCacheableDeltaGeneric& other) const
      {
        try {
          return m_managedptr->Equals(other.ptr());
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return false;

      }

      System::Int32 ManagedCacheableDeltaGeneric::hashcode() const
      {
        throw gcnew System::NotSupportedException;
      }

    }  // namespace client
  }  // namespace geode
}  // namespace apache
