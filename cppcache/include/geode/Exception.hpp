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

#ifndef GEODE_EXCEPTION_H_
#define GEODE_EXCEPTION_H_

#include <string>
#include <stdexcept>
#include <unordered_map>

#include "internal/geode_globals.hpp"
#include "internal/functional.hpp"

/**
 * @file
 */

namespace apache {
namespace geode {
namespace client {

class StackTrace;

// Ignore C4275 - This class extends std C++ class
#pragma warning(push)
#pragma warning(disable : 4275)

/**
 * @class Exception Exception.hpp
 * A description of an exception that occurred during a cache operation.
 */
class _GEODE_EXPORT Exception : public std::exception {
  /**
   * @brief public methods
   */
 public:
  explicit Exception(const std::string& message);
  Exception(std::string&& message);
  Exception(const char* message);

  /** Creates an exception as a copy of the given other exception.
   * @param  other the original exception.
   *
   **/
  Exception(const Exception& other) = default;
  Exception(Exception&& other) noexcept = default;

  /**
   * @brief destructor
   */
  virtual ~Exception() noexcept;

  /** Get a stacktrace string from the location the exception was created.
   */
  virtual std::string getStackTrace() const;

  /** Return the name of this exception type. */
  virtual std::string getName() const;

  virtual const std::string& getMessage() const noexcept;

  virtual const char* what() const noexcept override;

 private:
  std::string message;
  std::shared_ptr<StackTrace> m_stack;
};

#pragma warning(pop)

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_EXCEPTION_H_
