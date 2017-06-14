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
#include "compartment/fluid/SELiquidCompartmentLink.h"
#include "compartment/fluid/SEFluidCompartment.h"
#include "compartment/substances/SELiquidSubstanceQuantity.h"
#include "bind/LiquidCompartmentData.hxx"
#include "substance/SESubstanceTransport.h"

class DLL_DECL SELiquidCompartment : public SEFluidCompartment<SELiquidCompartmentLink, SELiquidTransportVertex, SELiquidTransportSubstance, SELiquidSubstanceQuantity>
{
  friend class SETissueCompartment;
  friend class SECompartmentManager;
  friend class SELiquidSubstanceQuantity;
protected:
  SELiquidCompartment(const std::string& name, Logger* logger);
public:
  virtual ~SELiquidCompartment();

  virtual void Clear();

  virtual bool Load(const CDM::LiquidCompartmentData& in, SESubstanceManager& subMgr, SECircuitManager* circuits = nullptr);
  virtual CDM::LiquidCompartmentData* Unload();
protected:
  virtual void Unload(CDM::LiquidCompartmentData& data);

public:
  virtual const SEScalar* GetScalar(const std::string& name);

  virtual void StateChange();

  virtual void Balance(BalanceLiquidBy e);// Balance all substances based on a specific property

	virtual bool HasPH() const;
	virtual SEScalar& GetPH();
	virtual double GetPH() const;

  virtual bool HasWaterVolumeFraction() const;
  virtual SEScalarFraction& GetWaterVolumeFraction();
  virtual double GetWaterVolumeFraction() const;

  virtual void AddChild(SELiquidCompartment& child);
  virtual const std::vector<SELiquidCompartment*>& GetChildren() { return m_Children; }
  virtual const std::vector<SELiquidCompartment*>& GetLeaves() { return m_Leaves; }

protected:
  virtual SELiquidSubstanceQuantity& CreateSubstanceQuantity(SESubstance& substance);

  SEScalar* m_pH;
  SEScalarFraction* m_WaterVolumeFraction;

  std::vector<SELiquidCompartment*> m_Children;
  std::vector<SELiquidCompartment*> m_Leaves;
};