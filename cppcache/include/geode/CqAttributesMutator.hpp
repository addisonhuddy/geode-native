#pragma once

#ifndef GEODE_CQATTRIBUTESMUTATOR_H_
#define GEODE_CQATTRIBUTESMUTATOR_H_

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

#include "internal/geode_globals.hpp"
#include <vector>
#include <memory>
/**
 * @file
 */

namespace apache {
namespace geode {
namespace client {
class CqListener;
/**
 * @class CqAttributesMutator CqAttributesMutator.hpp
 *
 * This interface is used to modify the listeners that are associated with a CQ.
 * Each CqQuery has an CqAttributesMutator interface which supports modification
 * of certain CQ attributes after the CQ has been created.
 *
 */
class _GEODE_EXPORT CqAttributesMutator {
 public:
  /**
   * Adds a CQ listener to the end of the list of CQ listeners on this CqQuery.
   * @param aListener the user defined CQ listener to add to the CqQuery.
   * @throws IllegalArgumentException if <code>aListener</code> is nullptr
   */
  virtual void addCqListener(const std::shared_ptr<CqListener>& aListener) = 0;

  /**
   * Removes given CQ listener from the list of CQ listeners on this CqQuery.
   * Does nothing if the specified listener has not been added.
   * If the specified listener has been added then will
   * be called on it; otherwise does nothing.
   * @param aListener the CQ listener to remove from the CqQuery.
   * @throws IllegalArgumentException if <code>aListener</code> is nullptr
   */
  virtual void removeCqListener(
      const std::shared_ptr<CqListener>& aListener) = 0;

  /**
   * Adds the given set CqListner on this CQ. If the CQ already has CqListeners,
   * this
   * removes those old CQs and initializes with the newListeners.
   * @param newListeners a possibly empty array of listeners to add
   * to this CqQuery.
   * @throws IllegalArgumentException if the <code>newListeners</code> array
   * has a nullptr element
   */
  virtual void setCqListeners(
      const std::vector<std::shared_ptr<CqListener>>& newListeners) = 0;
};
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_CQATTRIBUTESMUTATOR_H_
