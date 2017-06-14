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
#include "compartment/fluid/SEFluidCompartment.h"
#include "compartment/fluid/SEGasCompartmentLink.h"
#include "compartment/substances/SEGasSubstanceQuantity.h"
#include "bind/GasCompartmentData.hxx"
#include "substance/SESubstanceTransport.h"

enum class BalanceGasBy { Volume, VolumeFraction };

class DLL_DECL SEGasCompartment : public SEFluidCompartment<SEGasCompartmentLink, SEGasTransportVertex, SEGasTransportSubstance, SEGasSubstanceQuantity>
{
  friend class SECompartmentManager;
protected:
  SEGasCompartment(const std::string& name, Logger* logger);
public:
	virtual ~SEGasCompartment();

  virtual bool Load(const CDM::GasCompartmentData& in, SESubstanceManager& subMgr, SECircuitManager* circuits = nullptr);
  virtual CDM::GasCompartmentData* Unload();
protected:
  virtual void Unload(CDM::GasCompartmentData& data);

public:
  virtual void StateChange();

  virtual void Balance(BalanceGasBy e);

  virtual void AddChild(SEGasCompartment& child);
  virtual const std::vector<SEGasCompartment*>& GetChildren() { return m_Children; }
  virtual const std::vector<SEGasCompartment*>& GetLeaves() { return m_Leaves; }

protected:
  virtual SEGasSubstanceQuantity& CreateSubstanceQuantity(SESubstance& substance);
 
  std::vector<SEGasCompartment*> m_Children;
  std::vector<SEGasCompartment*> m_Leaves;
};
