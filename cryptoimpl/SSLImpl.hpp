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

#ifndef GEODE_CRYPTOIMPL_SSLIMPL_H_
#define GEODE_CRYPTOIMPL_SSLIMPL_H_

#include <ace/ACE.h>
#include <ace/OS.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_IO.h>
#include <ace/SSL/SSL_SOCK_Connector.h>
#include <ace/SSL/SSL_SOCK_Acceptor.h>
#include <ace/OS.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Time_Value.h>

#include <geode/internal/geode_base.hpp>

#include "Ssl.hpp"

namespace apache {
namespace geode {
namespace client {

class SSLImpl : public apache::geode::client::Ssl {
 private:
  ACE_SSL_SOCK_Stream* m_io;
  static ACE_Recursive_Thread_Mutex s_mutex;
  volatile static bool s_initialized;

 public:
  SSLImpl(ACE_SOCKET sock, const char* pubkeyfile, const char* privkeyfile,
          const char* password);
  virtual ~SSLImpl();

  int setOption(int, int, void*, int) override;
  int listen(ACE_INET_Addr, std::chrono::microseconds) override;
  int connect(ACE_INET_Addr, std::chrono::microseconds) override;
  ssize_t recv(void*, size_t, const ACE_Time_Value*, size_t*) override;
  ssize_t send(const void*, size_t, const ACE_Time_Value*, size_t*) override;
  int getLocalAddr(ACE_Addr&) override;
  void close() override;
};

extern "C" {
_GEODE_EXPORT void* gf_create_SslImpl(ACE_SOCKET sock, const char* pubkeyfile,
                                      const char* privkeyfile,
                                      const char* pemPassword);
_GEODE_EXPORT void gf_destroy_SslImpl(void* impl);
}

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_CRYPTOIMPL_SSLIMPL_H_
