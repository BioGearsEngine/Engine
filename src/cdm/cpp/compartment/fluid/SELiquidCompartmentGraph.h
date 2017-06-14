/**************************************************************************************
Copyright 2015 Applied Research Associates, Inc.
Licensed under the Apache License, Version 2.0 (the "License"); you may not use
this file except in compliance with the License. You may obtain a copy of the License
at:
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
**************************************************************************************/

#pragma once
CDM_BIND_DECL(LiquidCompartmentGraphData)
#include "compartment/fluid/SELiquidCompartment.h"
#include "compartment/fluid/SELiquidCompartmentLink.h"
#include "bind/GasCompartmentData.hxx"
#include "compartment/SECompartmentTransportGraph.h"
class SECompartmentManager;

class DLL_DECL SELiquidCompartmentGraph : public SECompartmentTransportGraph<SELiquidTransportGraph, SELiquidTransportVertex, SELiquidTransportEdge, SELiquidCompartment, SELiquidCompartmentLink>
{
  friend class SECompartmentManager;
protected:
  SELiquidCompartmentGraph(const std::string& name, Logger* logger) : SECompartmentTransportGraph(name, logger) {};
public:  
  virtual ~SELiquidCompartmentGraph() {}

  void AddGraph(SELiquidCompartmentGraph& graph);

  virtual bool Load(const CDM::LiquidCompartmentGraphData& in, SECompartmentManager& cmptMgr);
  virtual CDM::LiquidCompartmentGraphData* Unload();
protected:
  virtual void Unload(CDM::LiquidCompartmentGraphData& data);

protected:
  void BalanceByIntensive();
};