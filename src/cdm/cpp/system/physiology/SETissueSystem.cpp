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

#include "stdafx.h"
#include "system/physiology/SETissueSystem.h"
#include "substance/SESubstanceManager.h"
#include "properties/SEScalarVolume.h"
#include "bind/ScalarVolumeData.hxx"
#include "properties/SEScalarVolumePerTime.h"
#include "bind/ScalarVolumePerTimeData.hxx"

SETissueSystem::SETissueSystem(Logger* logger) : SESystem(logger)
{
  m_CarbonDioxideProductionRate = nullptr;
  m_ExtracellularFluidVolume = nullptr;
  m_ExtravascularFluidVolume = nullptr;
  m_IntracellularFluidVolume = nullptr;
  m_IntracellularFluidPH = nullptr;
  m_OxygenConsumptionRate = nullptr;
  m_RespiratoryExchangeRatio = nullptr;
}

SETissueSystem::~SETissueSystem()
{
	Clear();
}

void SETissueSystem::Clear()
{
  SAFE_DELETE(m_CarbonDioxideProductionRate);
  SAFE_DELETE(m_ExtracellularFluidVolume);
  SAFE_DELETE(m_ExtravascularFluidVolume);
  SAFE_DELETE(m_IntracellularFluidVolume);
  SAFE_DELETE(m_IntracellularFluidPH);
  SAFE_DELETE(m_OxygenConsumptionRate);
  SAFE_DELETE(m_RespiratoryExchangeRatio);
}

const SEScalar* SETissueSystem::GetScalar(const std::string& name)
{
  if (name.compare("CarbonDioxideProductionRate") == 0)
    return &GetCarbonDioxideProductionRate();
  if (name.compare("ExtracellularFluidVolume") == 0)
    return &GetExtracellularFluidVolume();
  if (name.compare("ExtravascularFluidVolume") == 0)
    return &GetExtravascularFluidVolume();
  if (name.compare("IntracellularFluidPH") == 0)
    return &GetIntracellularFluidPH();
  if (name.compare("IntracellularFluidVolume") == 0)
    return &GetIntracellularFluidVolume();
  if (name.compare("OxygenConsumptionRate") == 0)
    return &GetOxygenConsumptionRate();
  if (name.compare("RespiratoryExchangeRatio") == 0)
    return &GetRespiratoryExchangeRatio();
	return nullptr;
}

bool SETissueSystem::Load(const CDM::TissueSystemData& in)
{
	SESystem::Load(in);
  if (in.CarbonDioxideProductionRate().present())
    GetCarbonDioxideProductionRate().Load(in.CarbonDioxideProductionRate().get());
  if (in.ExtracellularFluidVolume().present())
    GetExtracellularFluidVolume().Load(in.ExtracellularFluidVolume().get());
  if (in.ExtravascularFluidVolume().present())
    GetExtravascularFluidVolume().Load(in.ExtravascularFluidVolume().get());
  if (in.IntracellularFluidPH().present())
    GetIntracellularFluidPH().Load(in.IntracellularFluidPH().get());
  if (in.IntracellularFluidVolume().present())
    GetIntracellularFluidVolume().Load(in.IntracellularFluidVolume().get());
  if (in.OxygenConsumptionRate().present())
    GetOxygenConsumptionRate().Load(in.OxygenConsumptionRate().get());
  if (in.RespiratoryExchangeRatio().present())
    GetRespiratoryExchangeRatio().Load(in.RespiratoryExchangeRatio().get());

	return true;
}

CDM::TissueSystemData* SETissueSystem::Unload() const
{
  CDM::TissueSystemData* data = new CDM::TissueSystemData();
  Unload(*data);
  return data;
}

void SETissueSystem::Unload(CDM::TissueSystemData& data) const
{	
  if (m_CarbonDioxideProductionRate != nullptr)
    data.CarbonDioxideProductionRate(std::unique_ptr<CDM::ScalarVolumePerTimeData>(m_CarbonDioxideProductionRate->Unload()));
  if (m_ExtracellularFluidVolume != nullptr)
    data.ExtracellularFluidVolume(std::unique_ptr<CDM::ScalarVolumeData>(m_ExtracellularFluidVolume->Unload()));
  if (m_ExtravascularFluidVolume != nullptr)
    data.ExtravascularFluidVolume(std::unique_ptr<CDM::ScalarVolumeData>(m_ExtravascularFluidVolume->Unload()));
  if (m_IntracellularFluidPH != nullptr)
    data.IntracellularFluidPH(std::unique_ptr<CDM::ScalarData>(m_IntracellularFluidPH->Unload()));
  if (m_IntracellularFluidVolume != nullptr)
    data.IntracellularFluidVolume(std::unique_ptr<CDM::ScalarVolumeData>(m_IntracellularFluidVolume->Unload()));
  if (m_OxygenConsumptionRate != nullptr)
    data.OxygenConsumptionRate(std::unique_ptr<CDM::ScalarVolumePerTimeData>(m_OxygenConsumptionRate->Unload()));
  if (m_RespiratoryExchangeRatio != nullptr)
    data.RespiratoryExchangeRatio(std::unique_ptr<CDM::ScalarData>(m_RespiratoryExchangeRatio->Unload()));

	SESystem::Unload(data);
}

bool SETissueSystem::HasCarbonDioxideProductionRate() const
{
  return m_CarbonDioxideProductionRate == nullptr ? false : m_CarbonDioxideProductionRate->IsValid();
}
SEScalarVolumePerTime& SETissueSystem::GetCarbonDioxideProductionRate()
{
  if (m_CarbonDioxideProductionRate == nullptr)
    m_CarbonDioxideProductionRate = new SEScalarVolumePerTime();
  return *m_CarbonDioxideProductionRate;
}
double SETissueSystem::GetCarbonDioxideProductionRate(const VolumePerTimeUnit& unit) const
{
  if (m_CarbonDioxideProductionRate == nullptr)
    return SEScalar::dNaN();
  return m_CarbonDioxideProductionRate->GetValue(unit);
}

bool SETissueSystem::HasExtracellularFluidVolume() const
{
  return m_ExtracellularFluidVolume == nullptr ? false : m_ExtracellularFluidVolume->IsValid();
}
SEScalarVolume& SETissueSystem::GetExtracellularFluidVolume()
{
  if (m_ExtracellularFluidVolume == nullptr)
    m_ExtracellularFluidVolume = new SEScalarVolume();
  return *m_ExtracellularFluidVolume;
}
double SETissueSystem::GetExtracellularFluidVolume(const VolumeUnit& unit) const
{
  if (m_ExtracellularFluidVolume == nullptr)
    return SEScalar::dNaN();
  return m_ExtracellularFluidVolume->GetValue(unit);
}

bool SETissueSystem::HasExtravascularFluidVolume() const
{
  return m_ExtravascularFluidVolume == nullptr ? false : m_ExtravascularFluidVolume->IsValid();
}
SEScalarVolume& SETissueSystem::GetExtravascularFluidVolume()
{
  if (m_ExtravascularFluidVolume == nullptr)
    m_ExtravascularFluidVolume = new SEScalarVolume();
  return *m_ExtravascularFluidVolume;
}
double SETissueSystem::GetExtravascularFluidVolume(const VolumeUnit& unit) const
{
  if (m_ExtravascularFluidVolume == nullptr)
    return SEScalar::dNaN();
  return m_ExtravascularFluidVolume->GetValue(unit);
}

bool SETissueSystem::HasIntracellularFluidVolume() const
{
  return m_IntracellularFluidVolume == nullptr ? false : m_IntracellularFluidVolume->IsValid();
}
SEScalarVolume& SETissueSystem::GetIntracellularFluidVolume()
{
  if (m_IntracellularFluidVolume == nullptr)
    m_IntracellularFluidVolume = new SEScalarVolume();
  return *m_IntracellularFluidVolume;
}
double SETissueSystem::GetIntracellularFluidVolume(const VolumeUnit& unit) const
{
  if (m_IntracellularFluidVolume == nullptr)
    return SEScalar::dNaN();
  return m_IntracellularFluidVolume->GetValue(unit);
}

bool SETissueSystem::HasIntracellularFluidPH() const
{
  return m_IntracellularFluidPH == nullptr ? false : m_IntracellularFluidPH->IsValid();
}
SEScalar& SETissueSystem::GetIntracellularFluidPH()
{
  if (m_IntracellularFluidPH == nullptr)
    m_IntracellularFluidPH = new SEScalar();
  return *m_IntracellularFluidPH;
}
double SETissueSystem::GetIntracellularFluidPH() const
{
  if (m_IntracellularFluidPH == nullptr)
    return SEScalar::dNaN();
  return m_IntracellularFluidPH->GetValue();
}

bool SETissueSystem::HasOxygenConsumptionRate() const
{
  return m_OxygenConsumptionRate == nullptr ? false : m_OxygenConsumptionRate->IsValid();
}
SEScalarVolumePerTime& SETissueSystem::GetOxygenConsumptionRate()
{
  if (m_OxygenConsumptionRate == nullptr)
    m_OxygenConsumptionRate = new SEScalarVolumePerTime();
  return *m_OxygenConsumptionRate;
}
double SETissueSystem::GetOxygenConsumptionRate(const VolumePerTimeUnit& unit) const
{
  if (m_OxygenConsumptionRate == nullptr)
    return SEScalar::dNaN();
  return m_OxygenConsumptionRate->GetValue(unit);
}

bool SETissueSystem::HasRespiratoryExchangeRatio() const
{
  return m_RespiratoryExchangeRatio == nullptr ? false : m_RespiratoryExchangeRatio->IsValid();
}
SEScalar& SETissueSystem::GetRespiratoryExchangeRatio()
{
  if (m_RespiratoryExchangeRatio == nullptr)
    m_RespiratoryExchangeRatio = new SEScalar();
  return *m_RespiratoryExchangeRatio;
}
double SETissueSystem::GetRespiratoryExchangeRatio() const
{
  if (m_RespiratoryExchangeRatio == nullptr)
    return SEScalar::dNaN();
  return m_RespiratoryExchangeRatio->GetValue();
}

