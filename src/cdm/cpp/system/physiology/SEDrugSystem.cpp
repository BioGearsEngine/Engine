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
#include "system/physiology/SEDrugSystem.h"
#include "substance/SESubstanceManager.h"
#include "properties/SEScalar.h"
#include "bind/ScalarData.hxx"
#include "properties/SEScalarFraction.h"
#include "bind/ScalarFractionData.hxx"
#include "properties/SEScalarFrequency.h"
#include "bind/ScalarFrequencyData.hxx"
#include "properties/SEScalarPressure.h"
#include "bind/ScalarPressureData.hxx"
#include "properties/SEScalarVolume.h"
#include "bind/ScalarVolumeData.hxx"
#include "properties/SEScalarLength.h"
#include "bind/ScalarLengthData.hxx"
#include "properties/SEScalarMassPerVolume.h"
#include "bind/ScalarMassPerVolumeData.hxx"



SEDrugSystem::SEDrugSystem(Logger* logger) : SESystem(logger)
{
	m_BronchodilationLevel = nullptr;
	m_HeartRateChange = nullptr;
	m_MeanBloodPressureChange = nullptr;
	m_NeuromuscularBlockLevel = nullptr;
	m_PulsePressureChange = nullptr;
  m_PupillaryResponse = nullptr;
	m_RespirationRateChange = nullptr;
	m_SedationLevel = nullptr;
	m_TidalVolumeChange = nullptr;
  m_TubularPermeabilityChange = nullptr;
  m_CentralNervousResponse = nullptr;
}

SEDrugSystem::~SEDrugSystem()
{
  Clear();
}

void SEDrugSystem::Clear()
{
  SESystem::Clear();

	SAFE_DELETE(m_BronchodilationLevel);
	SAFE_DELETE(m_HeartRateChange);
	SAFE_DELETE(m_MeanBloodPressureChange);
	SAFE_DELETE(m_NeuromuscularBlockLevel);
	SAFE_DELETE(m_PulsePressureChange);
  SAFE_DELETE(m_PupillaryResponse);
	SAFE_DELETE(m_RespirationRateChange);
	SAFE_DELETE(m_SedationLevel);
	SAFE_DELETE(m_TidalVolumeChange);
  SAFE_DELETE(m_TubularPermeabilityChange);
  SAFE_DELETE(m_CentralNervousResponse);
  
}

bool SEDrugSystem::Load(const CDM::DrugSystemData& in)
{
	SESystem::Load(in);

	if (in.BronchodilationLevel().present())
		GetBronchodilationLevel().Load(in.BronchodilationLevel().get());
	if (in.HeartRateChange().present())
		GetHeartRateChange().Load(in.HeartRateChange().get());
	if (in.MeanBloodPressureChange().present())
		GetMeanBloodPressureChange().Load(in.MeanBloodPressureChange().get());
	if (in.NeuromuscularBlockLevel().present())
		GetNeuromuscularBlockLevel().Load(in.NeuromuscularBlockLevel().get());
	if (in.PulsePressureChange().present())
		GetPulsePressureChange().Load(in.PulsePressureChange().get());
	if (in.PupillaryResponse().present())
		GetPupillaryResponse().Load(in.PupillaryResponse().get());
	if (in.RespirationRateChange().present())
		GetRespirationRateChange().Load(in.RespirationRateChange().get());
	if (in.SedationLevel().present())
		GetSedationLevel().Load(in.SedationLevel().get());
	if (in.TidalVolumeChange().present())
		GetTidalVolumeChange().Load(in.TidalVolumeChange().get());
	if (in.TubularPermeabilityChange().present())
		GetTubularPermeabilityChange().Load(in.TubularPermeabilityChange().get());
	if (in.CentralNervousResponse().present())
		GetCentralNervousResponse().Load(in.CentralNervousResponse().get());


	return true;
}

const SEScalar* SEDrugSystem::GetScalar(const std::string& name)
{
	if (name.compare("BronchodilationLevel") == 0)
		return &GetBronchodilationLevel();
	if (name.compare("HeartRateChange") == 0)
		return &GetHeartRateChange();
	if (name.compare("MeanBloodPressureChange") == 0)
		return &GetMeanBloodPressureChange();
	if (name.compare("NeuromuscularBlockLevel") == 0)
		return &GetNeuromuscularBlockLevel();
	if (name.compare("PulsePressureChange") == 0)
		return &GetPulsePressureChange();
	if (name.compare("RespirationRateChange") == 0)
		return &GetRespirationRateChange();
	if (name.compare("SedationLevel") == 0)
		return &GetSedationLevel();
	if (name.compare("TidalVolumeChange") == 0)
		return &GetTidalVolumeChange();
	if (name.compare("TubularPermeabilityChange") == 0)
		return &GetTubularPermeabilityChange();
	if (name.compare("CentralNervousResponse") == 0)
		return &GetCentralNervousResponse();


  size_t split = name.find('-');
  if (split != name.npos)
  {
    std::string child = name.substr(0, split);
    std::string prop = name.substr(split + 1, name.npos);
    if (child == "PupillaryResponse")
      return GetPupillaryResponse().GetScalar(prop);    
  }

	return nullptr;
}

CDM::DrugSystemData* SEDrugSystem::Unload() const
{
  CDM::DrugSystemData* data = new CDM::DrugSystemData();
  Unload(*data);
  return data;
}

void SEDrugSystem::Unload(CDM::DrugSystemData& data) const
{
	SESystem::Unload(data);

	if (m_BronchodilationLevel != nullptr)
		data.BronchodilationLevel(std::unique_ptr<CDM::ScalarFractionData>(m_BronchodilationLevel->Unload()));
	if (m_HeartRateChange != nullptr)
		data.HeartRateChange(std::unique_ptr<CDM::ScalarFrequencyData>(m_HeartRateChange->Unload()));
	if (m_MeanBloodPressureChange != nullptr)
		data.MeanBloodPressureChange(std::unique_ptr<CDM::ScalarPressureData>(m_MeanBloodPressureChange->Unload()));
	if (m_NeuromuscularBlockLevel != nullptr)
		data.NeuromuscularBlockLevel(std::unique_ptr<CDM::ScalarFractionData>(m_NeuromuscularBlockLevel->Unload()));
	if (m_PulsePressureChange != nullptr)
		data.PulsePressureChange(std::unique_ptr<CDM::ScalarPressureData>(m_PulsePressureChange->Unload()));
	if (m_PupillaryResponse != nullptr)
		data.PupillaryResponse(std::unique_ptr<CDM::PupillaryResponseData>(m_PupillaryResponse->Unload()));
	if (m_RespirationRateChange != nullptr)
		data.RespirationRateChange(std::unique_ptr<CDM::ScalarFrequencyData>(m_RespirationRateChange->Unload()));
	if (m_SedationLevel != nullptr)
		data.SedationLevel(std::unique_ptr<CDM::ScalarFractionData>(m_SedationLevel->Unload()));
	if (m_TidalVolumeChange != nullptr)
		data.TidalVolumeChange(std::unique_ptr<CDM::ScalarVolumeData>(m_TidalVolumeChange->Unload()));
	if (m_TubularPermeabilityChange != nullptr)
		data.TubularPermeabilityChange(std::unique_ptr<CDM::ScalarFractionData>(m_TubularPermeabilityChange->Unload()));
	if (m_CentralNervousResponse != nullptr)
		data.CentralNervousResponse(std::unique_ptr<CDM::ScalarFractionData>(m_CentralNervousResponse->Unload()));
}

bool SEDrugSystem::HasBronchodilationLevel() const
{
	return m_BronchodilationLevel == nullptr ? false : m_BronchodilationLevel->IsValid();
}
SEScalarFraction& SEDrugSystem::GetBronchodilationLevel()
{
	if (m_BronchodilationLevel == nullptr)
		m_BronchodilationLevel = new SEScalarFraction();
	return *m_BronchodilationLevel;
}
double SEDrugSystem::GetBronchodilationLevel() const
{
	if (m_BronchodilationLevel == nullptr)
		return SEScalar::dNaN();
	return m_BronchodilationLevel->GetValue();
}

bool SEDrugSystem::HasHeartRateChange() const
{
	return m_HeartRateChange == nullptr ? false : m_HeartRateChange->IsValid();
}
SEScalarFrequency& SEDrugSystem::GetHeartRateChange()
{
	if (m_HeartRateChange == nullptr)
		m_HeartRateChange = new SEScalarFrequency();
	return *m_HeartRateChange;
}
double SEDrugSystem::GetHeartRateChange(const FrequencyUnit& unit) const
{
	if (m_HeartRateChange == nullptr)
		return SEScalar::dNaN();
	return m_HeartRateChange->GetValue(unit);
}

bool SEDrugSystem::HasMeanBloodPressureChange() const
{
	return m_MeanBloodPressureChange == nullptr ? false : m_MeanBloodPressureChange->IsValid();
}
SEScalarPressure& SEDrugSystem::GetMeanBloodPressureChange()
{
	if (m_MeanBloodPressureChange == nullptr)
		m_MeanBloodPressureChange = new SEScalarPressure();
	return *m_MeanBloodPressureChange;
}
double SEDrugSystem::GetMeanBloodPressureChange(const PressureUnit& unit) const
{
	if (m_MeanBloodPressureChange == nullptr)
		return SEScalar::dNaN();
	return m_MeanBloodPressureChange->GetValue(unit);
}

bool SEDrugSystem::HasNeuromuscularBlockLevel() const
{
	return m_NeuromuscularBlockLevel == nullptr ? false : m_NeuromuscularBlockLevel->IsValid();
}
SEScalarFraction& SEDrugSystem::GetNeuromuscularBlockLevel()
{
	if (m_NeuromuscularBlockLevel == nullptr)
		m_NeuromuscularBlockLevel = new SEScalarFraction();
	return *m_NeuromuscularBlockLevel;
}
double SEDrugSystem::GetNeuromuscularBlockLevel() const
{
	if (m_NeuromuscularBlockLevel == nullptr)
		return SEScalar::dNaN();
	return m_NeuromuscularBlockLevel->GetValue();
}

bool SEDrugSystem::HasPulsePressureChange() const
{
	return m_PulsePressureChange == nullptr ? false : m_PulsePressureChange->IsValid();
}
SEScalarPressure& SEDrugSystem::GetPulsePressureChange()
{
	if (m_PulsePressureChange == nullptr)
		m_PulsePressureChange = new SEScalarPressure();
	return *m_PulsePressureChange;
}
double SEDrugSystem::GetPulsePressureChange(const PressureUnit& unit) const
{
	if (m_PulsePressureChange == nullptr)
		return SEScalar::dNaN();
	return m_PulsePressureChange->GetValue(unit);
}

bool SEDrugSystem::HasPupillaryResponse() const
{
  return (m_PupillaryResponse != nullptr);
}
SEPupillaryResponse& SEDrugSystem::GetPupillaryResponse()
{
  if (m_PupillaryResponse == nullptr)
    m_PupillaryResponse = new SEPupillaryResponse(GetLogger());
  return *m_PupillaryResponse;
}
const SEPupillaryResponse* SEDrugSystem::GetPupillaryResponse() const
{
  return m_PupillaryResponse;
}
void SEDrugSystem::RemovePupillaryResponse()
{
  SAFE_DELETE(m_PupillaryResponse);
}

bool SEDrugSystem::HasRespirationRateChange() const
{
	return m_RespirationRateChange == nullptr ? false : m_RespirationRateChange->IsValid();
}
SEScalarFrequency& SEDrugSystem::GetRespirationRateChange()
{
	if (m_RespirationRateChange == nullptr)
		m_RespirationRateChange = new SEScalarFrequency();
	return *m_RespirationRateChange;
}
double SEDrugSystem::GetRespirationRateChange(const FrequencyUnit& unit) const
{
	if (m_RespirationRateChange == nullptr)
		return SEScalar::dNaN();
	return m_RespirationRateChange->GetValue(unit);
}

bool SEDrugSystem::HasSedationLevel() const
{
	return m_SedationLevel == nullptr ? false : m_SedationLevel->IsValid();
}
SEScalarFraction& SEDrugSystem::GetSedationLevel()
{
	if (m_SedationLevel == nullptr)
		m_SedationLevel = new SEScalarFraction();
	return *m_SedationLevel;
}
double SEDrugSystem::GetSedationLevel() const
{
	if (m_SedationLevel == nullptr)
		return SEScalar::dNaN();
	return m_SedationLevel->GetValue();
}

bool SEDrugSystem::HasTidalVolumeChange() const
{
	return m_TidalVolumeChange == nullptr ? false : m_TidalVolumeChange->IsValid();
}
SEScalarVolume& SEDrugSystem::GetTidalVolumeChange()
{
	if (m_TidalVolumeChange == nullptr)
		m_TidalVolumeChange = new SEScalarVolume();
	return *m_TidalVolumeChange;
}

double SEDrugSystem::GetTidalVolumeChange(const VolumeUnit& unit) const
{
	if (m_TidalVolumeChange == nullptr)
		return SEScalar::dNaN();
	return m_TidalVolumeChange->GetValue(unit);
}

bool SEDrugSystem::HasTubularPermeabilityChange() const
{
  return m_TubularPermeabilityChange == nullptr ? false : m_TubularPermeabilityChange->IsValid();
}
SEScalarFraction& SEDrugSystem::GetTubularPermeabilityChange()
{
  if (m_TubularPermeabilityChange == nullptr)
    m_TubularPermeabilityChange = new SEScalarFraction();
  return *m_TubularPermeabilityChange;
}
double SEDrugSystem::GetTubularPermeabilityChange() const
{
  if (m_TubularPermeabilityChange == nullptr)
    return SEScalar::dNaN();
  return m_TubularPermeabilityChange->GetValue();
}

bool SEDrugSystem::HasCentralNervousResponse() const
{
	return m_CentralNervousResponse == nullptr ? false : m_CentralNervousResponse->IsValid();
}

SEScalarFraction& SEDrugSystem::GetCentralNervousResponse()
{
	if (m_CentralNervousResponse == nullptr)
		m_CentralNervousResponse = new SEScalarFraction();
	return *m_CentralNervousResponse;
}
double SEDrugSystem::GetCentralNervousResponse() const
{
	if (m_CentralNervousResponse == nullptr)
		return SEScalar::dNaN();
	return m_CentralNervousResponse->GetValue();
}

