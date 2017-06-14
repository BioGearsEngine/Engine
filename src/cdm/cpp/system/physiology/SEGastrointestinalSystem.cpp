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
#include "system/physiology/SEGastrointestinalSystem.h"
#include "substance/SESubstanceManager.h"
#include "bind/NutritionData.hxx"

#include "properties/SEScalarMass.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEScalarMassPerTime.h"
#include "bind/ScalarVolumePerTimeData.hxx"

SEGastrointestinalSystem::SEGastrointestinalSystem(Logger* logger) : SESystem(logger)
{
	m_ChymeAbsorbtionRate = nullptr;
  m_StomachContents = nullptr;
}

SEGastrointestinalSystem::~SEGastrointestinalSystem()
{
	Clear();
}

void SEGastrointestinalSystem::Clear()
{
  SESystem::Clear();
	SAFE_DELETE(m_ChymeAbsorbtionRate);
  SAFE_DELETE(m_StomachContents);
}


const SEScalar* SEGastrointestinalSystem::GetScalar(const std::string& name)
{
	if(name.compare("ChymeAbsorbtionRate") == 0)
		return &GetChymeAbsorbtionRate();

  size_t split = name.find('-');
  if (split != name.npos)
  {
    std::string child = name.substr(0, split);
    std::string prop = name.substr(split + 1, name.npos);
    if (child == "StomachContents")
      return GetStomachContents().GetScalar(prop);
  }
	return nullptr;
}

bool SEGastrointestinalSystem::Load(const CDM::GastrointestinalSystemData& in)
{
	SESystem::Load(in);
	if (in.ChymeAbsorbtionRate().present())
		GetChymeAbsorbtionRate().Load(in.ChymeAbsorbtionRate().get());
  if (in.StomachContents().present())
    GetStomachContents().Load(in.StomachContents().get());
	return true;
}

CDM::GastrointestinalSystemData* SEGastrointestinalSystem::Unload() const
{
  CDM::GastrointestinalSystemData* data = new CDM::GastrointestinalSystemData();
  Unload(*data);
  return data;
}

void SEGastrointestinalSystem::Unload(CDM::GastrointestinalSystemData& data) const
{
	SESystem::Unload(data);
	if (m_ChymeAbsorbtionRate != nullptr)
		data.ChymeAbsorbtionRate(std::unique_ptr<CDM::ScalarVolumePerTimeData>(m_ChymeAbsorbtionRate->Unload())); 
  if(m_StomachContents!=nullptr)
    data.StomachContents(std::unique_ptr<CDM::NutritionData>(m_StomachContents->Unload()));
}

bool SEGastrointestinalSystem::HasChymeAbsorbtionRate() const
{
	return m_ChymeAbsorbtionRate == nullptr ? false : m_ChymeAbsorbtionRate->IsValid();
}
SEScalarVolumePerTime& SEGastrointestinalSystem::GetChymeAbsorbtionRate()
{
	if (m_ChymeAbsorbtionRate == nullptr)
		m_ChymeAbsorbtionRate = new SEScalarVolumePerTime();
	return *m_ChymeAbsorbtionRate;
}
double SEGastrointestinalSystem::GetChymeAbsorbtionRate(const VolumePerTimeUnit& unit) const
{
	if (m_ChymeAbsorbtionRate == nullptr)
		return SEScalar::dNaN();
	return m_ChymeAbsorbtionRate->GetValue(unit);
}

bool SEGastrointestinalSystem::HasStomachContents() const
{
  return m_StomachContents == nullptr ? false : true;
}
SENutrition& SEGastrointestinalSystem::GetStomachContents()
{
  if (m_StomachContents == nullptr)
    m_StomachContents = new SENutrition(GetLogger());
  return *m_StomachContents;
}
const SENutrition* SEGastrointestinalSystem::GetStomachContents() const
{
  return m_StomachContents;
}
void SEGastrointestinalSystem::RemoveStomachContents()
{
  SAFE_DELETE(m_StomachContents);
}
