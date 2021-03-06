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

#ifndef GEODE_EXECUTION_H_
#define GEODE_EXECUTION_H_

#include <memory>
#include <chrono>
#include <string>

#include "internal/geode_globals.hpp"
#include "CacheableBuiltins.hpp"
#include "ResultCollector.hpp"

/**
 * @file
 */

namespace apache {
namespace geode {
namespace client {
/**
 * @class Execution Execution.hpp
 * gathers results from function execution
 * @see FunctionService
 */

class _GEODE_EXPORT Execution {
 public:
  /**
   * Specifies a data filter of routing objects for selecting the Geode
   * members
   * to execute the function.
   * <p>
   * If the filter set is empty the function is executed on all members
   * that have the  FunctionService::onRegion(Region).</p>
   * @param routingObj Set defining the data filter to be used for executing the
   * function
   * @return an Execution with the filter
   * @throws IllegalArgumentException if filter passed is nullptr.
   * @throws UnsupportedOperationException if not called after
   *    FunctionService::onRegion(Region).
   */
  virtual std::shared_ptr<Execution> withFilter(std::shared_ptr<CacheableVector> routingObj) = 0;
  /**
   * Specifies the user data passed to the function when it is executed.
   * @param args user data passed to the function execution
   * @return an Execution with args
   * @throws IllegalArgumentException if the input parameter is nullptr
   *
   */
  virtual std::shared_ptr<Execution> withArgs(std::shared_ptr<Cacheable> args) = 0;
  /**
   * Specifies the {@link ResultCollector} that will receive the results after
   * the function has been executed.
   * @return an Execution with a collector
   * @throws IllegalArgumentException if {@link ResultCollector} is nullptr
   * @see ResultCollector
   */
  virtual std::shared_ptr<Execution> withCollector(std::shared_ptr<ResultCollector> rs) = 0;
  /**
   * Executes the function using its name
   * <p>
   * @param func the name of the function to be executed
   * @param timeout value to wait for the operation to finish before timing out.
   * @throws Exception if there is an error during function execution
   * @return either a default result collector or one specified by {@link
   * #withCollector(ResultCollector)}
   */
  virtual std::shared_ptr<ResultCollector> execute(
      const std::string& func,
      std::chrono::milliseconds timeout = DEFAULT_QUERY_RESPONSE_TIMEOUT) = 0;

  /**
   * Executes the function using its name
   * <p>
   * @param routingObj Set defining the data filter to be used for executing the
   * function
   * @param args user data passed to the function execution
   * @param rs * Specifies the {@link ResultCollector} that will receive the
   * results after
   * the function has been executed.
   * @param func the name of the function to be executed
   * @param timeout value to wait for the operation to finish before timing out.
   * @throws Exception if there is an error during function execution
   * @return either a default result collector or one specified by {@link
   * #withCollector(ResultCollector)}
   */
  virtual std::shared_ptr<ResultCollector> execute(
      const std::shared_ptr<CacheableVector>& routingObj,
      const std::shared_ptr<Cacheable>& args,
      const std::shared_ptr<ResultCollector>& rs, const std::string& func,
      std::chrono::milliseconds timeout) = 0;
};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_EXECUTION_H_
