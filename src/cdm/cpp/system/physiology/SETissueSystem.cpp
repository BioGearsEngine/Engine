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
#include "properties/SEScalarAmountPerVolume.h"
#include "bind/ScalarAmountPerVolumeData.hxx"
#include "properties/SEScalarMassPerVolume.h"
#include "bind/ScalarMassPerVolumeData.hxx"
#include "properties/SEScalarMass.h"
#include "bind/ScalarMassData.hxx"


SETissueSystem::SETissueSystem(Logger* logger) : SESystem(logger)
{
  m_CarbonDioxideProductionRate = nullptr;
  m_ExtracellularFluidVolume = nullptr;
  m_ExtravascularFluidVolume = nullptr;
  m_IntracellularFluidVolume = nullptr;
  m_IntracellularFluidPH = nullptr;
  m_OxygenConsumptionRate = nullptr;
  m_RespiratoryExchangeRatio = nullptr;
  m_LiverInsulinSetPoint = nullptr;
  m_LiverGlucagonSetPoint = nullptr;
  m_MuscleInsulinSetPoint = nullptr;
  m_MuscleGlucagonSetPoint = nullptr;
  m_FatInsulinSetPoint = nullptr;
  m_FatGlucagonSetPoint = nullptr;
  m_LiverGlycogen = nullptr;
  m_MuscleGlycogen = nullptr;
  m_StoredProtein = nullptr;
  m_StoredFat = nullptr;
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
  SAFE_DELETE(m_LiverInsulinSetPoint);
  SAFE_DELETE(m_LiverGlucagonSetPoint);
  SAFE_DELETE(m_MuscleInsulinSetPoint);
  SAFE_DELETE(m_MuscleGlucagonSetPoint);
  SAFE_DELETE(m_FatInsulinSetPoint);
  SAFE_DELETE(m_FatGlucagonSetPoint);
  SAFE_DELETE(m_LiverGlycogen);
  SAFE_DELETE(m_MuscleGlycogen);
  SAFE_DELETE(m_StoredProtein);
  SAFE_DELETE(m_StoredFat);
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
  if (name.compare("LiverInsulinSetPoint") == 0)
    return &GetLiverInsulinSetPoint();
  if (name.compare("LiverGlucagonSetPoint") == 0)
    return &GetLiverGlucagonSetPoint();
  if (name.compare("MuscleInsulinSetPoint") == 0)
    return &GetMuscleInsulinSetPoint();
  if (name.compare("MuscleGlucagonSetPoint") == 0)
    return &GetMuscleGlucagonSetPoint();
  if (name.compare("FatInsulinSetPoint") == 0)
    return &GetFatInsulinSetPoint();
  if (name.compare("FatGlucagonSetPoint") == 0)
    return &GetFatGlucagonSetPoint();
  if (name.compare("LiverGlycogen") == 0)
    return &GetLiverGlycogen();
  if (name.compare("MuscleGlycogen") == 0)
    return &GetMuscleGlycogen();
  if (name.compare("StoredProtein") == 0)
    return &GetStoredProtein();
  if (name.compare("StoredFat") == 0)
    return &GetStoredFat();

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
  if (in.LiverInsulinSetPoint().present())
    GetLiverInsulinSetPoint().Load(in.LiverInsulinSetPoint().get());
  if (in.LiverGlucagonSetPoint().present())
    GetLiverGlucagonSetPoint().Load(in.LiverGlucagonSetPoint().get());
  if (in.MuscleInsulinSetPoint().present())
    GetMuscleInsulinSetPoint().Load(in.MuscleInsulinSetPoint().get());
  if (in.MuscleGlucagonSetPoint().present())
    GetMuscleGlucagonSetPoint().Load(in.MuscleGlucagonSetPoint().get());
  if (in.FatInsulinSetPoint().present())
    GetFatInsulinSetPoint().Load(in.FatInsulinSetPoint().get());
  if (in.FatGlucagonSetPoint().present())
    GetFatGlucagonSetPoint().Load(in.FatGlucagonSetPoint().get());
  if (in.LiverGlycogen().present())
    GetLiverGlycogen().Load(in.LiverGlycogen().get());
  if (in.MuscleGlycogen().present())
    GetMuscleGlycogen().Load(in.MuscleGlycogen().get());
  if (in.StoredProtein().present())
    GetStoredProtein().Load(in.StoredProtein().get());
  if (in.StoredFat().present())
    GetStoredFat().Load(in.StoredFat().get());

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
  if (m_LiverInsulinSetPoint != nullptr)
    data.LiverInsulinSetPoint(std::unique_ptr<CDM::ScalarAmountPerVolumeData>(m_LiverInsulinSetPoint->Unload()));
  if (m_LiverGlucagonSetPoint != nullptr)
    data.LiverGlucagonSetPoint(std::unique_ptr<CDM::ScalarMassPerVolumeData>(m_LiverGlucagonSetPoint->Unload()));
  if (m_MuscleInsulinSetPoint != nullptr)
    data.MuscleInsulinSetPoint(std::unique_ptr<CDM::ScalarAmountPerVolumeData>(m_MuscleInsulinSetPoint->Unload()));
  if (m_MuscleGlucagonSetPoint != nullptr)
    data.MuscleGlucagonSetPoint(std::unique_ptr<CDM::ScalarMassPerVolumeData>(m_MuscleGlucagonSetPoint->Unload()));
  if (m_FatInsulinSetPoint != nullptr)
    data.FatInsulinSetPoint(std::unique_ptr<CDM::ScalarAmountPerVolumeData>(m_FatInsulinSetPoint->Unload()));
  if (m_FatGlucagonSetPoint != nullptr)
    data.FatGlucagonSetPoint(std::unique_ptr<CDM::ScalarMassPerVolumeData>(m_FatGlucagonSetPoint->Unload()));
  if (m_LiverGlycogen != nullptr)
    data.LiverGlycogen(std::unique_ptr<CDM::ScalarMassData>(m_LiverGlycogen->Unload()));
  if (m_MuscleGlycogen != nullptr)
    data.MuscleGlycogen(std::unique_ptr<CDM::ScalarMassData>(m_MuscleGlycogen->Unload()));
  if (m_StoredProtein != nullptr)
    data.StoredProtein(std::unique_ptr<CDM::ScalarMassData>(m_StoredProtein->Unload()));
  if (m_StoredFat != nullptr)
    data.StoredFat(std::unique_ptr<CDM::ScalarMassData>(m_StoredFat->Unload()));

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

bool SETissueSystem::HasLiverInsulinSetPoint() const
{
  return m_LiverInsulinSetPoint == nullptr ? false : m_LiverInsulinSetPoint->IsValid();
}
SEScalarAmountPerVolume& SETissueSystem::GetLiverInsulinSetPoint()
{
  if (m_LiverInsulinSetPoint == nullptr)
    m_LiverInsulinSetPoint = new SEScalarAmountPerVolume();
  return *m_LiverInsulinSetPoint;
}
double SETissueSystem::GetLiverInsulinSetPoint(const AmountPerVolumeUnit& unit) const
{
  if (m_LiverInsulinSetPoint == nullptr)
    return SEScalar::dNaN();
  return m_LiverInsulinSetPoint->GetValue(unit);
}

bool SETissueSystem::HasLiverGlucagonSetPoint() const
{
  return m_LiverGlucagonSetPoint == nullptr ? false : m_LiverGlucagonSetPoint->IsValid();
}
SEScalarMassPerVolume& SETissueSystem::GetLiverGlucagonSetPoint()
{
  if (m_LiverGlucagonSetPoint == nullptr)
    m_LiverGlucagonSetPoint = new SEScalarMassPerVolume();
  return *m_LiverGlucagonSetPoint;
}
double SETissueSystem::GetLiverGlucagonSetPoint(const MassPerVolumeUnit& unit) const
{
  if (m_LiverGlucagonSetPoint == nullptr)
    return SEScalar::dNaN();
  return m_LiverGlucagonSetPoint->GetValue(unit);
}

bool SETissueSystem::HasMuscleInsulinSetPoint() const
{
  return m_MuscleInsulinSetPoint == nullptr ? false : m_MuscleInsulinSetPoint->IsValid();
}
SEScalarAmountPerVolume& SETissueSystem::GetMuscleInsulinSetPoint()
{
  if (m_MuscleInsulinSetPoint == nullptr)
    m_MuscleInsulinSetPoint = new SEScalarAmountPerVolume();
  return *m_MuscleInsulinSetPoint;
}
double SETissueSystem::GetMuscleInsulinSetPoint(const AmountPerVolumeUnit& unit) const
{
  if (m_MuscleInsulinSetPoint == nullptr)
    return SEScalar::dNaN();
  return m_MuscleInsulinSetPoint->GetValue(unit);
}

bool SETissueSystem::HasMuscleGlucagonSetPoint() const
{
  return m_MuscleGlucagonSetPoint == nullptr ? false : m_MuscleGlucagonSetPoint->IsValid();
}
SEScalarMassPerVolume& SETissueSystem::GetMuscleGlucagonSetPoint()
{
  if (m_MuscleGlucagonSetPoint == nullptr)
    m_MuscleGlucagonSetPoint = new SEScalarMassPerVolume();
  return *m_MuscleGlucagonSetPoint;
}
double SETissueSystem::GetMuscleGlucagonSetPoint(const MassPerVolumeUnit& unit) const
{
  if (m_MuscleGlucagonSetPoint == nullptr)
    return SEScalar::dNaN();
  return m_MuscleGlucagonSetPoint->GetValue(unit);
}

bool SETissueSystem::HasFatInsulinSetPoint() const
{
  return m_FatInsulinSetPoint == nullptr ? false : m_FatInsulinSetPoint->IsValid();
}
SEScalarAmountPerVolume& SETissueSystem::GetFatInsulinSetPoint()
{
  if (m_FatInsulinSetPoint == nullptr)
    m_FatInsulinSetPoint = new SEScalarAmountPerVolume();
  return *m_FatInsulinSetPoint;
}
double SETissueSystem::GetFatInsulinSetPoint(const AmountPerVolumeUnit& unit) const
{
  if (m_FatInsulinSetPoint == nullptr)
    return SEScalar::dNaN();
  return m_FatInsulinSetPoint->GetValue(unit);
}

bool SETissueSystem::HasFatGlucagonSetPoint() const
{
  return m_FatGlucagonSetPoint == nullptr ? false : m_FatGlucagonSetPoint->IsValid();
}
SEScalarMassPerVolume& SETissueSystem::GetFatGlucagonSetPoint()
{
  if (m_FatGlucagonSetPoint == nullptr)
    m_FatGlucagonSetPoint = new SEScalarMassPerVolume();
  return *m_FatGlucagonSetPoint;
}
double SETissueSystem::GetFatGlucagonSetPoint(const MassPerVolumeUnit& unit) const
{
  if (m_FatGlucagonSetPoint == nullptr)
    return SEScalar::dNaN();
  return m_FatGlucagonSetPoint->GetValue(unit);
}

bool SETissueSystem::HasLiverGlycogen() const
{
  return m_LiverGlycogen == nullptr ? false : m_LiverGlycogen->IsValid();
}
SEScalarMass& SETissueSystem::GetLiverGlycogen()
{
  if (m_LiverGlycogen == nullptr)
    m_LiverGlycogen = new SEScalarMass();
  return *m_LiverGlycogen;
}
double SETissueSystem::GetLiverGlycogen(const MassUnit& unit) const
{
  if (m_LiverGlycogen == nullptr)
    return SEScalar::dNaN();
  return m_LiverGlycogen->GetValue(unit);
}

bool SETissueSystem::HasMuscleGlycogen() const
{
  return m_MuscleGlycogen == nullptr ? false : m_MuscleGlycogen->IsValid();
}
SEScalarMass& SETissueSystem::GetMuscleGlycogen()
{
  if (m_MuscleGlycogen == nullptr)
    m_MuscleGlycogen = new SEScalarMass();
  return *m_MuscleGlycogen;
}
double SETissueSystem::GetMuscleGlycogen(const MassUnit& unit) const
{
  if (m_MuscleGlycogen == nullptr)
    return SEScalar::dNaN();
  return m_MuscleGlycogen->GetValue(unit);
}

bool SETissueSystem::HasStoredProtein() const
{
  return m_StoredProtein == nullptr ? false : m_StoredProtein->IsValid();
}
SEScalarMass& SETissueSystem::GetStoredProtein()
{
  if (m_StoredProtein == nullptr)
    m_StoredProtein = new SEScalarMass();
  return *m_StoredProtein;
}
double SETissueSystem::GetStoredProtein(const MassUnit& unit) const
{
  if (m_StoredProtein == nullptr)
    return SEScalar::dNaN();
  return m_StoredProtein->GetValue(unit);
}

bool SETissueSystem::HasStoredFat() const
{
  return m_StoredFat == nullptr ? false : m_StoredFat->IsValid();
}
SEScalarMass& SETissueSystem::GetStoredFat()
{
  if (m_StoredFat == nullptr)
    m_StoredFat = new SEScalarMass();
  return *m_StoredFat;
}
double SETissueSystem::GetStoredFat(const MassUnit& unit) const
{
  if (m_StoredFat == nullptr)
    return SEScalar::dNaN();
  return m_StoredFat->GetValue(unit);
}

