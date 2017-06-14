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
#include "compartment/SECompartment.h"
CDM_BIND_DECL(ThermalCompartmentData)
#include "bind/ThermalCompartmentData.hxx"
#include "compartment/SECompartmentNodes.h"
#include "circuit/thermal/SEThermalCircuitNode.h"
class SEThermalCompartmentLink;

class DLL_DECL SEThermalCompartment : public SECompartment
{
  friend class SECompartmentManager;
  template<typename CompartmentType, typename CompartmentLinkType> friend class SECompartmentGraph;
protected:
  SEThermalCompartment(const std::string& name, Logger* logger);
public:
  virtual ~SEThermalCompartment();

  virtual void Clear();

  virtual bool Load(const CDM::ThermalCompartmentData& in, SECircuitManager* circuits = nullptr);
  virtual CDM::ThermalCompartmentData* Unload();
protected:
  virtual void Unload(CDM::ThermalCompartmentData& data);

public:
  virtual const SEScalar* GetScalar(const std::string& name);

  virtual bool HasChildren() const { return !m_Children.empty(); }
  virtual const std::vector<SEThermalCompartment*>& GetChildren() { return m_Children; }
  virtual const std::vector<SEThermalCompartment*>& GetLeaves() { return m_Leaves; }

  virtual bool HasNodeMapping() const { return m_Nodes.HasMapping(); }
  virtual  SECompartmentNodes<THERMAL_COMPARTMENT_NODE>& GetNodeMapping() { return m_Nodes; }
  virtual void MapNode(SEThermalCircuitNode& node);

  virtual void StateChange();

	virtual bool HasHeatTransferRateIn() const;
	virtual const SEScalarPower& GetHeatTransferRateIn();
  virtual double GetHeatTransferRateIn(const PowerUnit& unit) const;

	virtual bool HasHeatTransferRateOut() const;
	virtual const SEScalarPower& GetHeatTransferRateOut();
  virtual double GetHeatTransferRateOut(const PowerUnit& unit) const;

  virtual bool HasHeat() const;
  virtual SEScalarEnergy& GetHeat();
  virtual double GetHeat(const EnergyUnit& unit) const;

	virtual bool HasTemperature() const;
	virtual SEScalarTemperature& GetTemperature();
  virtual double GetTemperature(const TemperatureUnit& unit) const;

  virtual void AddLink(SEThermalCompartmentLink& link);
  virtual void RemoveLink(SEThermalCompartmentLink& link);
  virtual void RemoveLinks();
  virtual const std::vector<SEThermalCompartmentLink*>& GetLinks();

  virtual bool HasChild(const std::string& name);
  virtual void AddChild(SEThermalCompartment& child);

protected:
  virtual double CalculateInFlow_W() const;
  virtual double CalculateOutFlow_W() const;

  SEScalarPower*       m_HeatTransferRateIn;
  SEScalarPower*       m_HeatTransferRateOut;
  SEScalarEnergy*      m_Heat;
  SEScalarTemperature* m_Temperature;

  std::vector<SEThermalCompartmentLink*> m_Links;
  std::vector<SEThermalCompartmentLink*> m_IncomingLinks;
  std::vector<SEThermalCompartmentLink*> m_OutgoingLinks;

  std::vector<SEThermalCompartment*> m_Children;
  std::vector<SEThermalCompartment*> m_Leaves;
  SECompartmentNodes<THERMAL_COMPARTMENT_NODE>  m_Nodes;
};