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

#include <geode/AttributesFactory.hpp>
#include <geode/PoolFactory.hpp>
#include <geode/internal/chrono/duration.hpp>

#include "fwklib/FrameworkTest.hpp"
#include "fwklib/TestClient.hpp"
#include "fwklib/FwkLog.hpp"

#include "PoolAttributes.hpp"

#include <util/concurrent/spinlock_mutex.hpp>

namespace apache {
namespace geode {
namespace client {
namespace testframework {

using util::concurrent::spinlock_mutex;

// ========================================================================

spinlock_mutex FrameworkTest::m_lck;

// ----------------------------------------------------------------------------

FrameworkTest::FrameworkTest(const char* initArgs) {
  txManager = nullptr;
  // parse args into variables,
  char xml[4096];   // xml file name
  char addr[1024];  // ip address, host:port
  int32_t port;     // int TS port number
  int32_t cnt = sscanf(initArgs, "%s %d %s %d", xml, &m_id, addr, &port);
  if (cnt != 4) {
    FWKEXCEPTION("Failed to parse init args: " << initArgs);
  }
  m_bbc = new FwkBBClient(addr);
  m_deltaMicros = 0;
  m_timeSync = new TimeSync(
      port, const_cast<int32_t*>(
                reinterpret_cast<volatile int32_t*>(&m_deltaMicros)));
  m_coll = new TestDriver(xml);
  TestClient::createTestClient(50, m_id);
  incClientCount();
}

FrameworkTest::~FrameworkTest() {
  if (m_coll != NULL) {
    delete m_coll;
    m_coll = NULL;
  }

  if (m_bbc != NULL) {
    delete m_bbc;
    m_bbc = NULL;
  }

  if (m_timeSync != NULL) {
    m_timeSync->stop();
    delete m_timeSync;
    m_timeSync = NULL;
  }

  if (m_cache != nullptr) {
    cacheFinalize();
  }
}

// ----------------------------------------------------------------------------

const FwkRegion* FrameworkTest::getSnippet(const std::string& name) const {
  FwkRegion* value = NULL;
  const FwkData* data = getData(name.c_str());
  if (data) {
    value = const_cast<FwkRegion*>(data->getSnippet());
  }
  std::string bbs("GFE_BB");
  std::string key("testScheme");
  std::string mode = bbGetString(bbs, key);
  std::string tag = getStringValue("TAG");
  std::string poolName = "_Test_Pool";
  bool isDC = getBoolValue("isDurable");
  int32_t redundancyLevel = getIntValue("redundancyLevel");
  std::string TAG("tags");
  std::string Count("count");

  if (mode == "poolwithendpoints" || mode == "poolwithlocator") {
    FWKINFO("Current Scheme::" << mode);
    Attributes* atts = value->getAttributes();
    if (!tag.empty()) {
      poolName.append(tag);
      atts->setPoolName(poolName);
    } else {
      atts->setPoolName(poolName);
    }
  }
  std::string label = "EndPoints";

  if (!tag.empty()) {
    label += "_";
    label += tag;
  }

  std::string endPts;
  std::string bb("GFE_BB");
  std::string cnt("EP_COUNT");
  int32_t epCount = static_cast<int32_t>(bbGet(bb, cnt));
  for (int32_t i = 1; i <= epCount; i++) {
    std::string key = label + "_";
    key.append(FwkStrCvt(i).toString());
    std::string ep = bbGetString(bb, key);
    if (!endPts.empty()) endPts.append(",");
    endPts.append(ep);
  }
  if (!endPts.empty()) {
    Attributes* atts = value->getAttributes();
    if ((atts != NULL) && (!atts->isLocal()) && (!atts->isWithPool()) &&
        (!isDC) && (redundancyLevel <= 0)) {
      // atts->setEndpoints( endPts );
      FWKINFO("Setting EndPoints to: " << endPts);
    }
  }
  return value;
}

std::vector<std::string> FrameworkTest::getRoundRobinEP() const {
  int32_t contactnum = getIntValue("contactNum");
  std::string label = "EndPoints";
  std::vector<std::string> rbEP;
  int32_t epCount = static_cast<int32_t>(bbGet("GFE_BB", "EP_COUNT"));
  if (contactnum < 0) contactnum = epCount;
  ACE_OS::sleep(1);
  std::string currEPKey("CURRENTEP_COUNT");
  int32_t currentEP =
      static_cast<int32_t>(m_bbc->increment("GFERR_BB", currEPKey));
  for (int i = 0; i < contactnum; i++) {
    if (currentEP > epCount) {
      m_bbc->clear("GFERR_BB");
      currentEP = static_cast<int32_t>(m_bbc->increment("GFERR_BB", currEPKey));
    }
    std::string key = label + "_";
    key.append(FwkStrCvt(currentEP).toString());
    std::string ep = bbGetString("GFE_BB", key);
    rbEP.push_back(ep);
    FWKINFO("getRoundRobinEP = " << ep << " key = " << key
                                 << " currentEP = " << currentEP);
  }

  return rbEP;
}
// ----------------------------------------------------------------------------

const FwkPool* FrameworkTest::getPoolSnippet(const std::string& name) const {
  FwkPool* value = NULL;
  const FwkData* data = getData(name.c_str());
  if (data) {
    value = const_cast<FwkPool*>(data->getPoolSnippet());
  }
  // Make sure , locator + servers both are not set
  if (value->isPoolWithServers() && value->isPoolWithLocators()) {
    FWKEXCEPTION("Both Servers and Locators Can't be specified for Pool.");
  }
  if (value->isPoolWithServers()) {
    std::string tag = getStringValue("TAG");
    std::string label = "EndPoints";

    if (!tag.empty()) {
      label += "_";
      label += tag;
    }
    std::string bb("GFE_BB");
    std::string cnt("EP_COUNT");

    int32_t epCount = static_cast<int32_t>(bbGet(bb, cnt));
    bool disableShufflingEP = getBoolValue("disableShufflingEP");
    if (disableShufflingEP) {
      FWKINFO("disable Shuffling EndPoint is true");
      std::vector<std::string> rbep = getRoundRobinEP();
      for (uint32_t cnt = 0; cnt < rbep.size(); cnt++) {
        value->addServer(rbep[cnt]);
      }
    } else {
      for (int32_t i = 1; i <= epCount; i++) {
        std::string key = label + "_";
        key.append(FwkStrCvt(i).toString());
        std::string ep = bbGetString(bb, key);
        value->addServer(ep);
      }
    }
  } else {
    // add locators
    std::string tag = getStringValue("TAG");
    std::string label = "LOCPORTS";

    if (!tag.empty()) {
      label += "_";
      label += tag;
    }
    std::string bb("GFE_BB");
    std::string cnt("LOC_CNT");

    int32_t locCount = static_cast<int32_t>(bbGet(bb, cnt));
    FWKINFO("LOC_CNT: " << locCount);
    for (int32_t i = 1; i <= locCount; i++) {
      std::string key = label + "_";
      key.append(FwkStrCvt(i).toString());
      std::string loc = bbGetString(bb, key);
      FWKINFO("Locator Key: " << key << "Value:" << loc);
      value->addLocator(loc);
    }
  }

  return value;
}
// ----------------------------------------------------------------------------

void FrameworkTest::cacheInitialize(
    std::shared_ptr<Properties>& props,
    const std::shared_ptr<CacheAttributes>& cAttrs) {
  // failures:
  // 2. cache exists exception
  // 3. create encountered exception
  // 4. overall failure at the end - ownership cruft/?

  try {
    std::string name = getStringValue("systemName");
    bool isPdxSerialized = getBoolValue("PdxReadSerialized");
    if (name.empty()) {
      name = "TestDS";
    }
    bool isSslEnable = getBoolValue("sslEnable");
    if (isSslEnable) {
      props->insert("ssl-enabled", "true");
      std::string keystore =
          std::string(ACE_OS::getenv("BUILDDIR")) + "/framework/data/keystore";
      std::string pubkey = keystore + "/client_truststore.pem";
      std::string privkey = keystore + "/client_keystore.pem";
      props->insert("ssl-keystore", privkey.c_str());
      props->insert("ssl-truststore", pubkey.c_str());
    }
    auto cacheFactory = CacheFactory(props);

    if (isPdxSerialized) {
      cacheFactory.setPdxReadSerialized(isPdxSerialized);
    }

    m_cache = std::make_shared<Cache>(cacheFactory.create());
    bool m_istransaction = getBoolValue("useTransactions");
    if (m_istransaction) {
      txManager = m_cache->getCacheTransactionManager();
    }
  } catch (CacheExistsException& ignore) {
    m_cache = nullptr;
  } catch (Exception& e) {
    FWKEXCEPTION(
        "CacheFactory::create encountered Exception: " << e.what());
  }

  if (m_cache == nullptr) {
    FWKEXCEPTION("FrameworkTest: Failed to initialize cache.");
  }
}

// ----------------------------------------------------------------------------

void FrameworkTest::cacheFinalize() {
  if (m_cache != nullptr) {
    try {
      destroyAllRegions();
      m_cache->close();
    } catch (CacheClosedException& ignore) {
    } catch (Exception& e) {
      FWKSEVERE("Caught an unexpected Exception during cache close: "
                << e.what());
    } catch (...) {
      FWKSEVERE("Caught an unexpected unknown exception during cache close.");
    }
  }
  m_cache = nullptr;
  FWKINFO("Cache closed.");
}

// ----------------------------------------------------------------------------

void FrameworkTest::incClientCount() {
  char buf[16];
  sprintf(buf, "%d", m_id);
  std::string key(buf);
  int64_t cnt = m_bbc->increment(CLIENTBB, buf);
  int32_t scnt = static_cast<int32_t>(cnt);
  FWKINFO("Start count for client: " << m_id << " is currently: " << scnt);
  if (scnt == 1) {
    cnt = m_bbc->increment(CLIENTBB, CLIENTCOUNT);
    scnt = static_cast<int32_t>(cnt);
    FWKINFO("Client count is currently: " << scnt);
  }
}

// ----------------------------------------------------------------------------

void FrameworkTest::destroyAllRegions() {
  // destroy all root regions
  for (auto& region : m_cache->rootRegions()) {
    localDestroyRegion(region);
  }
}

// ----------------------------------------------------------------------------

void FrameworkTest::localDestroyRegion(std::shared_ptr<Region>& region) {
  try {
    region->localDestroyRegion();
  } catch (RegionDestroyedException& ignore) {
    ignore.what();
    // the region could be already destroyed.
  } catch (Exception& ex) {
    FWKEXCEPTION("Caught unexpected exception during region local destroy: "
                 << ex.what());
  }
}

void FrameworkTest::parseEndPoints(int32_t ep, std::string label,
                                   bool isServer) {
  std::string poolName = "_Test_Pool";
  auto pf = m_cache->getPoolManager().createFactory();
  std::string tag = getStringValue("TAG");
  std::string bb("GFE_BB");

  std::string TAG("tags");
  std::string Count("count");
  std::string epList = " ";
  std::string stickykey("teststicky");
  std::string checksticky = bbGetString(bb, stickykey);

  bool multiUserMode = getBoolValue("multiUserMode");
  bool isExcpHandling = getBoolValue("isExcpHnd");
  FWKINFO("FrameworkTest::parseEndPoints ep count = : " << ep);
  for (int32_t i = 1; i <= ep; i++) {
    std::string key = label + "_";
    key.append(FwkStrCvt(i).toString());
    std::string ep = bbGetString(bb, key);
    epList += ep;
    size_t position = ep.find_first_of(":");
    if (position != std::string::npos) {
      std::string hostname = ep.substr(0, position);
      int portnumber = atoi((ep.substr(position + 1)).c_str());
      if (isServer) {
        pf.addServer(hostname.c_str(), portnumber);
      } else {
        pf.addLocator(hostname.c_str(), portnumber);
      }
    }
  }
  FWKINFO("Setting server or locator endpoints for pool:" << epList);
  FWKINFO("TESTSTICKY value is:" << checksticky);
  if ((checksticky.compare("ON")) == 0) {
    FWKINFO("setThreadLocalConnections to true & setMaxConnections to 13");
    pf.setThreadLocalConnections(true);
    pf.setPRSingleHopEnabled(false);
  } else {
    FWKINFO("ThreadLocalConnections set to false:Default");
  }

  if (isExcpHandling) {
    FWKINFO("The test is Exception Handling Test");
    pf.setRetryAttempts(10);
  }

  if (multiUserMode) {
    FWKINFO("Setting multiuser mode");
    pf.setMultiuserAuthentication(true);
    pf.setSubscriptionEnabled(true);
  } else {
    pf.setSubscriptionEnabled(true);
  }

  pf.setMinConnections(20);
  pf.setMaxConnections(30);
  pf.setSubscriptionEnabled(true);
  pf.setReadTimeout(std::chrono::seconds(180));
  pf.setFreeConnectionTimeout(std::chrono::seconds(180));
  int32_t redundancyLevel = getIntValue("redundancyLevel");
  if (redundancyLevel > 0) pf.setSubscriptionRedundancy(redundancyLevel);
  // create tag specific pools
  std::shared_ptr<Pool> pptr = nullptr;
  if (!tag.empty()) {
    poolName.append(tag);
    // check if pool already exists
    pptr = m_cache->getPoolManager().find(poolName.c_str());
    if (pptr == nullptr) {
      pptr = pf.create(poolName.c_str());
    }
  }
  // create default pool
  else {
    pptr = m_cache->getPoolManager().find(poolName.c_str());
    if (pptr == nullptr) {
      pptr = pf.create(poolName.c_str());
    }
  }
  if (pptr != nullptr)
    FWKINFO(" Region Created with following Pool attributes :"
            << poolAttributesToString(pptr));
}

void FrameworkTest::createPool() {
  std::string bb("GFE_BB");
  std::string keys("testScheme");
  std::string mode = bbGetString(bb, keys);
  int32_t count = 0;
  std::string cnt;
  if (mode == "poolwithendpoints") {
    std::string label = "EndPoints";
    std::string tag = getStringValue("TAG");
    if (!tag.empty()) {
      label += "_";
      label += tag;
    }
    cnt = "EP_COUNT";
    count = static_cast<int32_t>(bbGet(bb, cnt));
    parseEndPoints(count, label, true);
  } else if (mode == "poolwithlocator") {
    std::string label = "LOCPORTS";
    cnt = "LOC_CNT";
    count = static_cast<int32_t>(bbGet(bb, cnt));
    parseEndPoints(count, label, false);
  }
}
std::shared_ptr<QueryService> FrameworkTest::checkQueryService() {
  auto pfPtr = m_cache->getPoolManager().createFactory();
  std::string bb("GFE_BB");
  std::string keys("testScheme");
  std::string mode = bbGetString(bb, keys);
  if (mode == "poolwithendpoints" || mode == "poolwithlocator") {
    auto pool = m_cache->getPoolManager().find("_Test_Pool");
    return pool->getQueryService();
  } else {
    return m_cache->getQueryService();
  }
}

void FrameworkTest::setTestScheme() {
  FWKINFO("FrameworkTest::setTestScheme called");
  std::string bb("GFE_BB");
  std::string key("testScheme");
  std::string lastpsc = bbGetString(bb, key);
  std::string psc = "";
  resetValue(key.c_str());
  while (psc != lastpsc) {
    psc = getStringValue(key.c_str());
  }
  while (psc == lastpsc || psc.empty()) {
    psc = getStringValue(key.c_str());
    if (psc.empty()) {
      break;
    }
  }
  if (!psc.empty()) {
    bbSet(bb, key, psc);
    FWKINFO("last test scheme = " << lastpsc << " current scheme = " << psc);
    FWKINFO("Test scheme : " << psc);
  }
}

std::string FrameworkTest::poolAttributesToString(std::shared_ptr<Pool>& pool) {
  using namespace apache::geode::internal::chrono::duration;

  std::string sString;
  sString += "\npoolName: ";
  sString += FwkStrCvt(pool->getName()).toString();
  sString += "\nFreeConnectionTimeout: ";
  sString += to_string(pool->getFreeConnectionTimeout());
  sString += "\nLoadConditioningInterval: ";
  sString += to_string(pool->getLoadConditioningInterval());
  sString += "\nSocketBufferSize: ";
  sString += FwkStrCvt(pool->getSocketBufferSize()).toString();
  sString += "\nReadTimeout: ";
  sString += to_string(pool->getReadTimeout());
  sString += "\nMinConnections: ";
  sString += FwkStrCvt(pool->getMinConnections()).toString();
  sString += "\nMaxConnections: ";
  sString += FwkStrCvt(pool->getMaxConnections()).toString();
  sString += "\nStatisticInterval: ";
  sString += to_string(pool->getStatisticInterval());
  sString += "\nRetryAttempts: ";
  sString += FwkStrCvt(pool->getRetryAttempts()).toString();
  sString += "\nSubscriptionEnabled: ";
  sString += pool->getSubscriptionEnabled() ? "true" : "false";
  sString += "\nSubscriptionRedundancy: ";
  sString += FwkStrCvt(pool->getSubscriptionRedundancy()).toString();
  sString += "\nSubscriptionMessageTrackingTimeout: ";
  sString += to_string(pool->getSubscriptionMessageTrackingTimeout());
  sString += "\nSubscriptionAckInterval: ";
  sString += to_string(pool->getSubscriptionAckInterval());
  sString += "\nServerGroup: ";
  sString += pool->getServerGroup();
  sString += "\nIdleTimeout: ";
  sString += apache::geode::internal::chrono::duration::to_string(
      pool->getIdleTimeout());
  sString += "\nPingInterval: ";
  sString += to_string(pool->getPingInterval());
  sString += "\nThreadLocalConnections: ";
  sString += pool->getThreadLocalConnections() ? "true" : "false";
  sString += "\nMultiuserAuthentication: ";
  sString += pool->getMultiuserAuthentication() ? "true" : "false";
  sString += "\nPRSingleHopEnabled: ";
  sString += pool->getPRSingleHopEnabled() ? "true" : "false";
  sString += "\nLocator: ";
  auto str =
      std::dynamic_pointer_cast<CacheableStringArray>(pool->getLocators());
  if (str != nullptr) {
    for (int32_t stri = 0; stri < str->length(); stri++) {
      sString += str->operator[](stri)->value().c_str();
      sString += ",";
    }
  }
  sString += "\nServers: ";
  str = std::dynamic_pointer_cast<CacheableStringArray>(pool->getServers());
  if (str != nullptr) {
    for (int32_t stri = 0; stri < str->length(); stri++) {
      sString += str->operator[](stri)->value().c_str();
      sString += ",";
    }
  }
  sString += "\n";
  return sString;
}
}  // namespace testframework
}  // namespace client
}  // namespace geode
}  // namespace apache
