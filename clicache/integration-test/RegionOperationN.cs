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
using System;
using System.Diagnostics;
using System.Threading;

namespace Apache.Geode.Client.UnitTests
{
  using NUnit.Framework;
  using Apache.Geode.DUnitFramework;
  using Apache.Geode.Client;

  using Region = Apache.Geode.Client.IRegion<Object, Object>;
  public class RegionOperation
  {
    protected IRegion<object, object> m_region;

    public Region Region
    {
      get
      {
        return m_region;
      }
    }

    public RegionOperation(string RegionName)
    {
      Serializable.RegisterPdxType(PdxTests.PdxTypes1.CreateDeserializable);
      Serializable.RegisterPdxType(PdxTests.PdxTypes8.CreateDeserializable);
      m_region = CacheHelper.GetRegion<object, object>(RegionName);
    }

    public void PutOp(int key, Object CallbackArg)
    { 

      Object value = "value";
      for (int i = 1; i <= key; i++)
      {
        Util.Log("PutOp:key={0},value={1}",i,value);
        m_region.Put(i,value,CallbackArg);
        //m_region[10000 + i] = new PdxTests.PdxTypes1();
        m_region[10000 + i] = new PdxTests.PdxTypes8();
      }
    }

    public void InvalidateOp(int key, Object CallbackArg)
    {
      for (int i = 1; i <= key; i++)
      {
        Util.Log("InvalidateOp:key={0}", i);
        m_region.GetLocalView().Invalidate(i, CallbackArg);
      }
    }

    public void DestroyOp(int key, Object CallbackArg)
    {
      for (int i = 1; i <= key; i++)
      {
        Util.Log("DestroyOp:key={0}", i);
        m_region.Remove(i, CallbackArg);
      }
    }
    public void DestroyOpWithPdxValue(int key, Object CallbackArg)
    {
      for (int i = 1; i <= key; i++)
      {
        Util.Log("DestroyOpWithPdxValue:key={0}", i);
        m_region.Remove(i, CallbackArg);
        m_region.Remove(10000 + i, null);
      }
    }

    public void RemoveOp(int key, Object CallbackArg)
    {

      string value = "value";
      for (int i = 1; i <= key; i++) {
        Util.Log("PutOp:key={0},value={1}", i, value);
        m_region.Remove(i, value, CallbackArg);
      }
    }
  }
}
