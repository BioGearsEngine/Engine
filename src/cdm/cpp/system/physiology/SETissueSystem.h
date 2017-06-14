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
#include "system/SESystem.h"
#include "bind/TissueSystemData.hxx"

class DLL_DECL SETissueSystem : public SESystem
{
public:

	SETissueSystem(Logger* logger);
	virtual ~SETissueSystem();

  virtual void Clear();// Deletes all members
  
	virtual const SEScalar* GetScalar(const std::string& name);
	
  virtual bool Load(const CDM::TissueSystemData& in);
  virtual CDM::TissueSystemData* Unload() const;
protected:
  virtual void Unload(CDM::TissueSystemData& data) const;

public:

  virtual bool HasCarbonDioxideProductionRate() const;
  virtual SEScalarVolumePerTime& GetCarbonDioxideProductionRate();
  virtual double GetCarbonDioxideProductionRate(const VolumePerTimeUnit& unit) const;

  virtual bool HasExtracellularFluidVolume() const;
  virtual SEScalarVolume& GetExtracellularFluidVolume();
  virtual double GetExtracellularFluidVolume(const VolumeUnit& unit) const;
 
  virtual bool HasExtravascularFluidVolume() const;
  virtual SEScalarVolume& GetExtravascularFluidVolume();
  virtual double GetExtravascularFluidVolume(const VolumeUnit& unit) const;

  virtual bool HasIntracellularFluidVolume() const;
  virtual SEScalarVolume& GetIntracellularFluidVolume();
  virtual double GetIntracellularFluidVolume(const VolumeUnit& unit) const;

  virtual bool HasIntracellularFluidPH() const;
  virtual SEScalar& GetIntracellularFluidPH();
  virtual double GetIntracellularFluidPH() const;

  virtual bool HasOxygenConsumptionRate() const;
  virtual SEScalarVolumePerTime& GetOxygenConsumptionRate();
  virtual double GetOxygenConsumptionRate(const VolumePerTimeUnit& unit) const;

  virtual bool HasRespiratoryExchangeRatio() const;
  virtual SEScalar& GetRespiratoryExchangeRatio();
  virtual double GetRespiratoryExchangeRatio() const;


protected:
  SEScalarVolumePerTime*  m_CarbonDioxideProductionRate;
  SEScalarVolume*         m_ExtracellularFluidVolume;//planned
  SEScalarVolume*         m_ExtravascularFluidVolume;//planned
  SEScalarVolume*         m_IntracellularFluidVolume;//planned
  SEScalar*               m_IntracellularFluidPH;
  SEScalarVolumePerTime*	m_OxygenConsumptionRate;
  SEScalar*				        m_RespiratoryExchangeRatio;


};
