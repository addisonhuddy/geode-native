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

#include <geode/CacheListener.hpp>
#include <geode/Region.hpp>
#include <geode/EntryEvent.hpp>
#include <geode/RegionEvent.hpp>

namespace apache {
namespace geode {
namespace client {

CacheListener::CacheListener() {}

CacheListener::~CacheListener() {}

void CacheListener::close(Region& region) {}

void CacheListener::afterCreate(const EntryEvent& event) {}

void CacheListener::afterUpdate(const EntryEvent& event) {}

void CacheListener::afterInvalidate(const EntryEvent& event) {}

void CacheListener::afterDestroy(const EntryEvent& event) {}

void CacheListener::afterRegionInvalidate(const RegionEvent& event) {}

void CacheListener::afterRegionDestroy(const RegionEvent& event) {}

void CacheListener::afterRegionClear(const RegionEvent& event) {}

void CacheListener::afterRegionLive(const RegionEvent& event) {}

void CacheListener::afterRegionDisconnected(Region& region) {}
}  // namespace client
}  // namespace geode
}  // namespace apache
