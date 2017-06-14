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
#include "compartment/SECompartmentLink.h"
#include "bind/ThermalCompartmentLinkData.hxx"
#include "substance/SESubstanceTransport.h"
#include "compartment/thermal/SEThermalCompartment.h"
#include "circuit/thermal/SEThermalCircuitPath.h"

class DLL_DECL SEThermalCompartmentLink : public SECompartmentLink
{
  friend class SECompartmentManager;
protected:
  SEThermalCompartmentLink(SEThermalCompartment& src, SEThermalCompartment & tgt, const std::string& name);
public:
  virtual ~SEThermalCompartmentLink();

  virtual void Clear();
  
  virtual bool Load(const CDM::ThermalCompartmentLinkData& in, SECircuitManager* circuits = nullptr);
  virtual CDM::ThermalCompartmentLinkData* Unload();
protected:
  virtual void Unload(CDM::ThermalCompartmentLinkData& data);
  
public:
  virtual const SEScalar* GetScalar(const std::string& name);

  virtual bool HasHeatTransferRate() const;
  virtual SEScalarPower& GetHeatTransferRate();
  virtual double GetHeatTransferRate(const PowerUnit& unit) const;

  virtual SEThermalCompartment& GetSourceCompartment() { return m_SourceCmpt; }
  virtual SEThermalCompartment& GetTargetCompartment() { return m_TargetCmpt; }

  virtual bool HasPath() { return m_Path != nullptr; }
  virtual SEThermalCircuitPath* GetPath() { return m_Path; }
  virtual void RemovePath() { m_Path = nullptr; }
  virtual void MapPath(SEThermalCircuitPath& path) { Clear();  m_Path = &path; }

protected:
  SEScalarPower*         m_HeatTransferRate;
  SEThermalCompartment&  m_SourceCmpt;
  SEThermalCompartment&  m_TargetCmpt;
  SEThermalCircuitPath*  m_Path;
};

#include "compartment/SECompartmentGraph.h"
class SEThermalCompartmentGraph : public SECompartmentGraph<SEThermalCompartment, SEThermalCompartmentLink>
{
public:
  SEThermalCompartmentGraph(const std::string& name, Logger* logger) : SECompartmentGraph(name, logger) {};
  virtual ~SEThermalCompartmentGraph() {}
};
