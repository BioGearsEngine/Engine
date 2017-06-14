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
#include "patient/actions/SEPatientAction.h"
#include "bind/MechanicalVentilationData.hxx"
#include "bind/enumOnOff.hxx"
class Serializer;
class SESubstance;
class SESubstanceFraction;
class SEMechanicalVentilationConfiguration;

class DLL_DECL SEMechanicalVentilation : public SEPatientAction
{
protected:

public:

	SEMechanicalVentilation();
  virtual ~SEMechanicalVentilation();

  virtual void Clear();

  virtual bool IsValid() const;
  virtual bool IsActive() const;

	virtual bool Load(const CDM::MechanicalVentilationData& in, const SESubstanceManager& subMgr);
  virtual CDM::MechanicalVentilationData* Unload() const;
protected:
  virtual void Unload(CDM::MechanicalVentilationData& data) const;

public:

	virtual CDM::enumOnOff::value GetState() const;
	virtual void SetState(CDM::enumOnOff::value name);
	virtual bool HasState() const;
	virtual void InvalidateState();

	virtual bool HasFlow() const;
	virtual SEScalarVolumePerTime& GetFlow();
  virtual double GetFlow(const VolumePerTimeUnit& unit) const;

	virtual bool HasPressure() const;
	virtual SEScalarPressure& GetPressure();
	virtual double GetPressure(const PressureUnit& unit) const;

  bool HasGasFraction() const;
  bool HasGasFraction(const SESubstance& substance) const;
  const std::vector<SESubstanceFraction*>& GetGasFractions();
  const std::vector<const SESubstanceFraction*>& GetGasFractions() const;
  SESubstanceFraction& GetGasFraction(SESubstance& substance);
  const SESubstanceFraction* GetGasFraction(const SESubstance& substance) const;
  void RemoveGasFraction(const SESubstance& substance);
  void RemoveGasFractions();

  virtual void ToString(std::ostream &str) const;
protected:

	std::stringstream	  m_ss;

	CDM::enumOnOff::value   m_State;
  SEScalarVolumePerTime*  m_Flow;
  SEScalarPressure*       m_Pressure;

  std::vector<SESubstanceFraction*>       m_GasFractions;
  std::vector<const SESubstanceFraction*> m_cGasFractions;
};
