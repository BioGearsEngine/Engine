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
#include "system/physiology/SECardiovascularSystem.h"
#include "substance/SESubstanceManager.h"
#include "properties/SEScalarFraction.h"
#include "bind/ScalarFractionData.hxx"
#include "properties/SEScalarFrequency.h"
#include "bind/ScalarFrequencyData.hxx"
#include "properties/SEScalarPressure.h"
#include "bind/ScalarPressureData.hxx"
#include "properties/SEScalarVolume.h"
#include "bind/ScalarVolumeData.hxx"
#include "properties/SEScalarVolumePerTime.h"
#include "bind/ScalarVolumePerTimeData.hxx"
#include "properties/SEScalarFlowResistance.h"
#include "bind/ScalarFlowResistanceData.hxx"
#include "bind/enumHeartRhythm.hxx"
#include "properties/SEScalarPressurePerVolume.h"
#include "bind/ScalarPressurePerVolumeData.hxx"
#include "properties/SEScalarPressureTimePerArea.h"
#include "bind/ScalarPressureTimePerAreaData.hxx"
#include "properties/SEScalarPressureTimePerVolumeArea.h"
#include "bind/ScalarPressureTimePerVolumeAreaData.hxx"
#include "properties/SEScalarVolumePerTimeArea.h"
#include "bind/ScalarVolumePerTimeAreaData.hxx"


SECardiovascularSystem::SECardiovascularSystem(Logger* logger) : SESystem(logger)
{
	m_ArterialPressure = nullptr;
	m_BloodVolume = nullptr;
  m_CardiacIndex = nullptr;
	m_CardiacOutput = nullptr;
	m_CentralVenousPressure = nullptr;
  m_CerebralBloodFlow = nullptr;
  m_CerebralPerfusionPressure = nullptr;
	m_DiastolicArterialPressure = nullptr;
	m_HeartEjectionFraction = nullptr;
	m_HeartRate = nullptr;
	m_HeartRhythm = (CDM::enumHeartRhythm::value) - 1;
	m_HeartStrokeVolume = nullptr; 
  m_IntracranialPressure = nullptr;
	m_MeanArterialPressure = nullptr;
	m_MeanArterialCarbonDioxidePartialPressure = nullptr;
	m_MeanArterialCarbonDioxidePartialPressureDelta = nullptr;
	m_MeanCentralVenousPressure = nullptr;
	m_MeanSkinFlow = nullptr;
	m_PulmonaryArterialPressure = nullptr;
	m_PulmonaryCapillariesWedgePressure = nullptr;
	m_PulmonaryDiastolicArterialPressure = nullptr;
	m_PulmonaryMeanArterialPressure = nullptr;
	m_PulmonaryMeanCapillaryFlow = nullptr;
	m_PulmonaryMeanShuntFlow = nullptr;
	m_PulmonarySystolicArterialPressure = nullptr;
  m_PulmonaryVascularResistance = nullptr;
  m_PulmonaryVascularResistanceIndex = nullptr;
	m_PulsePressure = nullptr;
	m_SystemicVascularResistance = nullptr;
	m_SystolicArterialPressure = nullptr;
}

SECardiovascularSystem::~SECardiovascularSystem()
{
	Clear();
}

void SECardiovascularSystem::Clear()
{
  SESystem::Clear();

	SAFE_DELETE(m_ArterialPressure);
	SAFE_DELETE(m_BloodVolume);
	SAFE_DELETE(m_CardiacIndex);
  SAFE_DELETE(m_CardiacOutput);
	SAFE_DELETE(m_CentralVenousPressure);
  SAFE_DELETE(m_CerebralBloodFlow);
  SAFE_DELETE(m_CerebralPerfusionPressure);
	SAFE_DELETE(m_DiastolicArterialPressure);
	SAFE_DELETE(m_HeartEjectionFraction);
	SAFE_DELETE(m_HeartRate);
	m_HeartRhythm = (CDM::enumHeartRhythm::value) - 1;
	SAFE_DELETE(m_HeartStrokeVolume);
  SAFE_DELETE(m_IntracranialPressure);
	SAFE_DELETE(m_MeanArterialPressure);
	SAFE_DELETE(m_MeanArterialCarbonDioxidePartialPressure);
	SAFE_DELETE(m_MeanArterialCarbonDioxidePartialPressureDelta);
	SAFE_DELETE(m_MeanCentralVenousPressure);
	SAFE_DELETE(m_MeanSkinFlow);
	SAFE_DELETE(m_PulmonaryArterialPressure);
	SAFE_DELETE(m_PulmonaryCapillariesWedgePressure);
	SAFE_DELETE(m_PulmonaryDiastolicArterialPressure);
	SAFE_DELETE(m_PulmonaryMeanArterialPressure);
	SAFE_DELETE(m_PulmonaryMeanCapillaryFlow);
	SAFE_DELETE(m_PulmonaryMeanShuntFlow);
	SAFE_DELETE(m_PulmonarySystolicArterialPressure);
  SAFE_DELETE(m_PulmonaryVascularResistance);
  SAFE_DELETE(m_PulmonaryVascularResistanceIndex);
	SAFE_DELETE(m_PulsePressure);
	SAFE_DELETE(m_SystemicVascularResistance);
	SAFE_DELETE(m_SystolicArterialPressure);
}

const SEScalar* SECardiovascularSystem::GetScalar(const std::string& name)
{
	if (name.compare("ArterialPressure") == 0)
		return &GetArterialPressure();
	if (name.compare("BloodVolume") == 0)
		return &GetBloodVolume();
	if (name.compare("CardiacIndex") == 0)
		return &GetCardiacIndex();
  if (name.compare("CardiacOutput") == 0)
    return &GetCardiacOutput();
  if (name.compare("CentralVenousPressure") == 0)
		return &GetCentralVenousPressure();
  if (name.compare("CerebralBloodFlow") == 0)
    return &GetCerebralBloodFlow();
  if (name.compare("CerebralPerfusionPressure") == 0)
    return &GetCerebralPerfusionPressure();
  if (name.compare("DiastolicArterialPressure") == 0)
		return &GetDiastolicArterialPressure();
	if (name.compare("HeartEjectionFraction") == 0)
		return &GetHeartEjectionFraction();
	if (name.compare("HeartRate") == 0)
		return &GetHeartRate();
	if (name.compare("HeartStrokeVolume") == 0)
		return &GetHeartStrokeVolume();
  if (name.compare("IntracranialPressure") == 0)
    return &GetIntracranialPressure();
	if (name.compare("MeanArterialPressure") == 0)
		return &GetMeanArterialPressure();
	if (name.compare("MeanArterialCarbonDioxidePartialPressure") == 0)
		return &GetMeanArterialCarbonDioxidePartialPressure();
	if (name.compare("MeanArterialCarbonDioxidePartialPressureDelta") == 0)
		return &GetMeanArterialCarbonDioxidePartialPressureDelta();
	if (name.compare("MeanCentralVenousPressure") == 0)
		return &GetMeanCentralVenousPressure();
	if (name.compare("MeanSkinFlow") == 0)
		return &GetMeanSkinFlow();
	if (name.compare("PulmonaryArterialPressure") == 0)
		return &GetPulmonaryArterialPressure();
	if (name.compare("PulmonaryCapillariesWedgePressure") == 0)
		return &GetPulmonaryCapillariesWedgePressure();
	if (name.compare("PulmonaryDiastolicArterialPressure") == 0)
		return &GetPulmonaryDiastolicArterialPressure();
	if (name.compare("PulmonaryMeanArterialPressure") == 0)
		return &GetPulmonaryMeanArterialPressure();
	if (name.compare("PulmonaryMeanCapillaryFlow") == 0)
		return &GetPulmonaryMeanCapillaryFlow();
	if (name.compare("PulmonaryMeanShuntFlow") == 0)
		return &GetPulmonaryMeanShuntFlow();
	if (name.compare("PulmonarySystolicArterialPressure") == 0)
		return &GetPulmonarySystolicArterialPressure();
  if (name.compare("PulmonaryVascularResistance") == 0)
    return &GetPulmonaryVascularResistance();
  if (name.compare("PulmonaryVascularResistanceIndex") == 0)
    return &GetPulmonaryVascularResistanceIndex();
	if (name.compare("PulsePressure") == 0)
		return &GetPulsePressure();
	if (name.compare("SystemicVascularResistance") == 0)
		return &GetSystemicVascularResistance();
	if (name.compare("SystolicArterialPressure") == 0)
		return &GetSystolicArterialPressure();
	return nullptr;
}

bool SECardiovascularSystem::Load(const CDM::CardiovascularSystemData& in)
{
	SESystem::Load(in);

	if(in.ArterialPressure().present())
		GetArterialPressure().Load(in.ArterialPressure().get());
	if(in.BloodVolume().present())
		GetBloodVolume().Load(in.BloodVolume().get());
	if(in.CardiacIndex().present())
		GetCardiacIndex().Load(in.CardiacIndex().get());
  if (in.CardiacOutput().present())
    GetCardiacOutput().Load(in.CardiacOutput().get());
  if (in.CentralVenousPressure().present())
    GetCentralVenousPressure().Load(in.CentralVenousPressure().get());
  if (in.CerebralBloodFlow().present())
    GetCerebralBloodFlow().Load(in.CerebralBloodFlow().get());
  if (in.CerebralPerfusionPressure().present())
    GetCerebralPerfusionPressure().Load(in.CerebralPerfusionPressure().get());
  if(in.DiastolicArterialPressure().present())
		GetDiastolicArterialPressure().Load(in.DiastolicArterialPressure().get());	
	if(in.HeartEjectionFraction().present())
		GetHeartEjectionFraction().Load(in.HeartEjectionFraction().get());
	if(in.HeartRate().present())
		GetHeartRate().Load(in.HeartRate().get());
	if (in.HeartRhythm().present())
		SetHeartRhythm(in.HeartRhythm().get());
	if(in.HeartStrokeVolume().present())
		GetHeartStrokeVolume().Load(in.HeartStrokeVolume().get());
  if (in.IntracranialPressure().present())
    GetIntracranialPressure().Load(in.IntracranialPressure().get());
  if (in.MeanArterialPressure().present())
		GetMeanArterialPressure().Load(in.MeanArterialPressure().get());
	if (in.MeanArterialCarbonDioxidePartialPressure().present())
		GetMeanArterialCarbonDioxidePartialPressure().Load(in.MeanArterialCarbonDioxidePartialPressure().get());
	if (in.MeanArterialCarbonDioxidePartialPressureDelta().present())
		GetMeanArterialCarbonDioxidePartialPressureDelta().Load(in.MeanArterialCarbonDioxidePartialPressureDelta().get());
  if (in.MeanCentralVenousPressure().present())
    GetMeanCentralVenousPressure().Load(in.MeanCentralVenousPressure().get());
  if (in.MeanSkinFlow().present())
		GetMeanSkinFlow().Load(in.MeanSkinFlow().get());
	if (in.PulmonaryArterialPressure().present())
		GetPulmonaryArterialPressure().Load(in.PulmonaryArterialPressure().get());
	if (in.PulmonaryCapillariesWedgePressure().present())
		GetPulmonaryCapillariesWedgePressure().Load(in.PulmonaryCapillariesWedgePressure().get());
	if(in.PulmonaryDiastolicArterialPressure().present())
		GetPulmonaryDiastolicArterialPressure().Load(in.PulmonaryDiastolicArterialPressure().get());
	if(in.PulmonaryMeanArterialPressure().present())
		GetPulmonaryMeanArterialPressure().Load(in.PulmonaryMeanArterialPressure().get());
	if(in.PulmonaryMeanCapillaryFlow().present())
		GetPulmonaryMeanCapillaryFlow().Load(in.PulmonaryMeanCapillaryFlow().get());
	if(in.PulmonaryMeanShuntFlow().present())
		GetPulmonaryMeanShuntFlow().Load(in.PulmonaryMeanShuntFlow().get());
	if(in.PulmonarySystolicArterialPressure().present())
		GetPulmonarySystolicArterialPressure().Load(in.PulmonarySystolicArterialPressure().get());
	if(in.PulmonaryVascularResistance().present())
		GetPulmonaryVascularResistance().Load(in.PulmonaryVascularResistance().get());
  if (in.PulmonaryVascularResistanceIndex().present())
    GetPulmonaryVascularResistanceIndex().Load(in.PulmonaryVascularResistanceIndex().get());
  if (in.PulsePressure().present())
    GetPulsePressure().Load(in.PulsePressure().get());
  if (in.SystemicVascularResistance().present())
		GetSystemicVascularResistance().Load(in.SystemicVascularResistance().get());
	if(in.SystolicArterialPressure().present())
		GetSystolicArterialPressure().Load(in.SystolicArterialPressure().get());

	return true;
}

CDM::CardiovascularSystemData* SECardiovascularSystem::Unload() const
{
  CDM::CardiovascularSystemData* data = new CDM::CardiovascularSystemData();
  Unload(*data);
  return data;
}

void SECardiovascularSystem::Unload(CDM::CardiovascularSystemData& data) const
{
	SESystem::Unload(data);

	if(m_ArterialPressure!=nullptr)
		data.ArterialPressure(std::unique_ptr<CDM::ScalarPressureData>(m_ArterialPressure->Unload())); 
	if(m_BloodVolume!=nullptr)
		data.BloodVolume(std::unique_ptr<CDM::ScalarVolumeData>(m_BloodVolume->Unload())); 
	if(m_CardiacIndex !=nullptr)
		data.CardiacIndex(std::unique_ptr<CDM::ScalarVolumePerTimeAreaData>(m_CardiacIndex->Unload()));
  if (m_CardiacOutput != nullptr)
    data.CardiacOutput(std::unique_ptr<CDM::ScalarVolumePerTimeData>(m_CardiacOutput->Unload()));
  if(m_CentralVenousPressure!=nullptr)
		data.CentralVenousPressure(std::unique_ptr<CDM::ScalarPressureData>(m_CentralVenousPressure->Unload())); 
  if (m_CerebralBloodFlow != nullptr)
    data.CerebralBloodFlow(std::unique_ptr<CDM::ScalarVolumePerTimeData>(m_CerebralBloodFlow->Unload()));
  if (m_CerebralPerfusionPressure != nullptr)
    data.CerebralPerfusionPressure(std::unique_ptr<CDM::ScalarPressureData>(m_CerebralPerfusionPressure->Unload()));
  if(m_DiastolicArterialPressure!=nullptr)
		data.DiastolicArterialPressure(std::unique_ptr<CDM::ScalarPressureData>(m_DiastolicArterialPressure->Unload())); 
	if(m_HeartEjectionFraction!=nullptr)
		data.HeartEjectionFraction(std::unique_ptr<CDM::ScalarFractionData>(m_HeartEjectionFraction->Unload())); 
	if(m_HeartRate!=nullptr)
		data.HeartRate(std::unique_ptr<CDM::ScalarFrequencyData>(m_HeartRate->Unload()));
	if (HasHeartRhythm())
		data.HeartRhythm(m_HeartRhythm);
	if (m_HeartStrokeVolume != nullptr)
		data.HeartStrokeVolume(std::unique_ptr<CDM::ScalarVolumeData>(m_HeartStrokeVolume->Unload()));
  if (m_IntracranialPressure != nullptr)
    data.IntracranialPressure(std::unique_ptr<CDM::ScalarPressureData>(m_IntracranialPressure->Unload()));
  if (m_MeanArterialPressure != nullptr)
		data.MeanArterialPressure(std::unique_ptr<CDM::ScalarPressureData>(m_MeanArterialPressure->Unload()));
	if (m_MeanArterialCarbonDioxidePartialPressure != nullptr)
		data.MeanArterialCarbonDioxidePartialPressure(std::unique_ptr<CDM::ScalarPressureData>(m_MeanArterialCarbonDioxidePartialPressure->Unload()));
	if (m_MeanArterialCarbonDioxidePartialPressureDelta != nullptr)
		data.MeanArterialCarbonDioxidePartialPressureDelta(std::unique_ptr<CDM::ScalarPressureData>(m_MeanArterialCarbonDioxidePartialPressureDelta->Unload()));
	if (m_MeanCentralVenousPressure != nullptr)
		data.MeanCentralVenousPressure(std::unique_ptr<CDM::ScalarPressureData>(m_MeanCentralVenousPressure->Unload()));
	if (m_MeanSkinFlow != nullptr)
		data.MeanSkinFlow(std::unique_ptr<CDM::ScalarVolumePerTimeData>(m_MeanSkinFlow->Unload()));
	if (m_PulmonaryArterialPressure != nullptr)
		data.PulmonaryArterialPressure(std::unique_ptr<CDM::ScalarPressureData>(m_PulmonaryArterialPressure->Unload()));
	if (m_PulmonaryCapillariesWedgePressure != nullptr)
		data.PulmonaryCapillariesWedgePressure(std::unique_ptr<CDM::ScalarPressureData>(m_PulmonaryCapillariesWedgePressure->Unload())); 
	if(m_PulmonaryDiastolicArterialPressure!=nullptr)
		data.PulmonaryDiastolicArterialPressure(std::unique_ptr<CDM::ScalarPressureData>(m_PulmonaryDiastolicArterialPressure->Unload())); 
	if(m_PulmonaryMeanArterialPressure!=nullptr)
		data.PulmonaryMeanArterialPressure(std::unique_ptr<CDM::ScalarPressureData>(m_PulmonaryMeanArterialPressure->Unload())); 
	if(m_PulmonaryMeanCapillaryFlow!=nullptr)
		data.PulmonaryMeanCapillaryFlow(std::unique_ptr<CDM::ScalarVolumePerTimeData>(m_PulmonaryMeanCapillaryFlow->Unload())); 
	if(m_PulmonaryMeanShuntFlow!=nullptr)
		data.PulmonaryMeanShuntFlow(std::unique_ptr<CDM::ScalarVolumePerTimeData>(m_PulmonaryMeanShuntFlow->Unload())); 
	if(m_PulmonarySystolicArterialPressure!=nullptr)
		data.PulmonarySystolicArterialPressure(std::unique_ptr<CDM::ScalarPressureData>(m_PulmonarySystolicArterialPressure->Unload())); 
	if(m_PulmonaryVascularResistance !=nullptr)
		data.PulmonaryVascularResistance(std::unique_ptr<CDM::ScalarFlowResistanceData>(m_PulmonaryVascularResistance->Unload()));
  if (m_PulmonaryVascularResistanceIndex != nullptr)
    data.PulmonaryVascularResistanceIndex(std::unique_ptr<CDM::ScalarPressureTimePerVolumeAreaData>(m_PulmonaryVascularResistanceIndex->Unload()));
  if (m_PulsePressure != nullptr)
    data.PulsePressure(std::unique_ptr<CDM::ScalarPressureData>(m_PulsePressure->Unload()));
  if (m_SystemicVascularResistance != nullptr)
		data.SystemicVascularResistance(std::unique_ptr<CDM::ScalarFlowResistanceData>(m_SystemicVascularResistance->Unload()));
	if(m_SystolicArterialPressure!=nullptr)
		data.SystolicArterialPressure(std::unique_ptr<CDM::ScalarPressureData>(m_SystolicArterialPressure->Unload()));
}

bool SECardiovascularSystem::HasArterialPressure() const
{
	return m_ArterialPressure==nullptr?false:m_ArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetArterialPressure()
{
	if(m_ArterialPressure==nullptr)
		m_ArterialPressure=new SEScalarPressure();
	return *m_ArterialPressure;
}
double SECardiovascularSystem::GetArterialPressure(const PressureUnit& unit) const
{
	if (m_ArterialPressure == nullptr)
		return SEScalar::dNaN();
	return m_ArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasBloodVolume() const
{
	return m_BloodVolume==nullptr?false:m_BloodVolume->IsValid();
}
SEScalarVolume& SECardiovascularSystem::GetBloodVolume()
{
	if(m_BloodVolume==nullptr)
		m_BloodVolume=new SEScalarVolume();
	return *m_BloodVolume;
}
double SECardiovascularSystem::GetBloodVolume(const VolumeUnit& unit) const
{
	if (m_BloodVolume == nullptr)
		return SEScalar::dNaN();
	return m_BloodVolume->GetValue(unit);
}

bool SECardiovascularSystem::HasCardiacOutput() const
{
	return m_CardiacOutput==nullptr?false:m_CardiacOutput->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetCardiacOutput()
{
	if(m_CardiacOutput==nullptr)
		m_CardiacOutput=new SEScalarVolumePerTime();
	return *m_CardiacOutput;
}
double SECardiovascularSystem::GetCardiacOutput(const VolumePerTimeUnit& unit) const
{
	if (m_CardiacOutput == nullptr)
		return SEScalar::dNaN();
	return m_CardiacOutput->GetValue(unit);
}

bool SECardiovascularSystem::HasCardiacIndex() const
{
  return m_CardiacIndex == nullptr ? false : m_CardiacIndex->IsValid();
}
SEScalarVolumePerTimeArea& SECardiovascularSystem::GetCardiacIndex()
{
  if (m_CardiacIndex == nullptr)
    m_CardiacIndex = new SEScalarVolumePerTimeArea();
  return *m_CardiacIndex;
}
double SECardiovascularSystem::GetCardiacIndex(const VolumePerTimeAreaUnit& unit) const
{
  if (m_CardiacIndex == nullptr)
    return SEScalar::dNaN();
  return m_CardiacIndex->GetValue(unit);
}

bool SECardiovascularSystem::HasCentralVenousPressure() const
{
	return m_CentralVenousPressure==nullptr?false:m_CentralVenousPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetCentralVenousPressure()
{
	if(m_CentralVenousPressure==nullptr)
		m_CentralVenousPressure=new SEScalarPressure();
	return *m_CentralVenousPressure;
}
double SECardiovascularSystem::GetCentralVenousPressure(const PressureUnit& unit) const
{
	if (m_CentralVenousPressure == nullptr)
		return SEScalar::dNaN();
	return m_CentralVenousPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasCerebralBloodFlow() const
{
  return m_CerebralBloodFlow == nullptr ? false : m_CerebralBloodFlow->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetCerebralBloodFlow()
{
  if (m_CerebralBloodFlow == nullptr)
    m_CerebralBloodFlow = new SEScalarVolumePerTime();
  return *m_CerebralBloodFlow;
}
double SECardiovascularSystem::GetCerebralBloodFlow(const VolumePerTimeUnit& unit) const
{
  if (m_CerebralBloodFlow == nullptr)
    return SEScalar::dNaN();
  return m_CerebralBloodFlow->GetValue(unit);
}

bool SECardiovascularSystem::HasCerebralPerfusionPressure() const
{
  return m_CerebralPerfusionPressure == nullptr ? false : m_CerebralPerfusionPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetCerebralPerfusionPressure()
{
  if (m_CerebralPerfusionPressure == nullptr)
    m_CerebralPerfusionPressure = new SEScalarPressure();
  return *m_CerebralPerfusionPressure;
}
double SECardiovascularSystem::GetCerebralPerfusionPressure(const PressureUnit& unit) const
{
  if (m_CerebralPerfusionPressure == nullptr)
    return SEScalar::dNaN();
  return m_CerebralPerfusionPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasDiastolicArterialPressure() const
{
	return m_DiastolicArterialPressure==nullptr?false:m_DiastolicArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetDiastolicArterialPressure()
{
	if(m_DiastolicArterialPressure==nullptr)
		m_DiastolicArterialPressure=new SEScalarPressure();
	return *m_DiastolicArterialPressure;
}
double SECardiovascularSystem::GetDiastolicArterialPressure(const PressureUnit& unit) const
{
	if (m_DiastolicArterialPressure == nullptr)
		return SEScalar::dNaN();
	return m_DiastolicArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasHeartEjectionFraction() const
{
	return m_HeartEjectionFraction==nullptr?false:m_HeartEjectionFraction->IsValid();
}
SEScalarFraction& SECardiovascularSystem::GetHeartEjectionFraction()
{
	if(m_HeartEjectionFraction==nullptr)
		m_HeartEjectionFraction=new SEScalarFraction();
	return *m_HeartEjectionFraction;
}
double SECardiovascularSystem::GetHeartEjectionFraction() const
{
	if (m_HeartEjectionFraction == nullptr)
		return SEScalar::dNaN();
	return m_HeartEjectionFraction->GetValue();
}

bool SECardiovascularSystem::HasHeartRate() const
{
	return m_HeartRate==nullptr?false:m_HeartRate->IsValid();
}
SEScalarFrequency& SECardiovascularSystem::GetHeartRate()
{
	if(m_HeartRate==nullptr)
		m_HeartRate=new SEScalarFrequency();
	return *m_HeartRate;
}
double SECardiovascularSystem::GetHeartRate(const FrequencyUnit& unit) const
{
	if (m_HeartRate == nullptr)
		return SEScalar::dNaN();
	return m_HeartRate->GetValue(unit);
}

CDM::enumHeartRhythm::value SECardiovascularSystem::GetHeartRhythm() const
{
	return m_HeartRhythm;
}
void SECardiovascularSystem::SetHeartRhythm(CDM::enumHeartRhythm::value rhythm)
{
	m_HeartRhythm = rhythm;
}
bool SECardiovascularSystem::HasHeartRhythm()const
{
	return m_HeartRhythm == ((CDM::enumHeartRhythm::value) - 1) ? false : true;
}
void SECardiovascularSystem::InvalidateHeartRhythm()
{
	m_HeartRhythm = (CDM::enumHeartRhythm::value) - 1;
}

bool SECardiovascularSystem::HasHeartStrokeVolume() const
{
	return m_HeartStrokeVolume==nullptr?false:m_HeartStrokeVolume->IsValid();
}
SEScalarVolume& SECardiovascularSystem::GetHeartStrokeVolume()
{
	if(m_HeartStrokeVolume==nullptr)
		m_HeartStrokeVolume=new SEScalarVolume();
	return *m_HeartStrokeVolume;
}
double SECardiovascularSystem::GetHeartStrokeVolume(const VolumeUnit& unit) const
{
	if (m_HeartStrokeVolume == nullptr)
		return SEScalar::dNaN();
	return m_HeartStrokeVolume->GetValue(unit);
}

bool SECardiovascularSystem::HasIntracranialPressure() const
{
  return m_IntracranialPressure == nullptr ? false : m_IntracranialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetIntracranialPressure()
{
  if (m_IntracranialPressure == nullptr)
    m_IntracranialPressure = new SEScalarPressure();
  return *m_IntracranialPressure;
}
double SECardiovascularSystem::GetIntracranialPressure(const PressureUnit& unit) const
{
  if (m_IntracranialPressure == nullptr)
    return SEScalar::dNaN();
  return m_IntracranialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasMeanArterialPressure() const
{
	return m_MeanArterialPressure == nullptr ? false : m_MeanArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetMeanArterialPressure()
{
	if (m_MeanArterialPressure == nullptr)
		m_MeanArterialPressure = new SEScalarPressure();
	return *m_MeanArterialPressure;
}
double SECardiovascularSystem::GetMeanArterialPressure(const PressureUnit& unit) const
{
	if (m_MeanArterialPressure == nullptr)
		return SEScalar::dNaN();
	return m_MeanArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasMeanArterialCarbonDioxidePartialPressure() const
{
	return m_MeanArterialCarbonDioxidePartialPressure == nullptr ? false : m_MeanArterialCarbonDioxidePartialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetMeanArterialCarbonDioxidePartialPressure()
{
	if (m_MeanArterialCarbonDioxidePartialPressure == nullptr)
		m_MeanArterialCarbonDioxidePartialPressure = new SEScalarPressure();
	return *m_MeanArterialCarbonDioxidePartialPressure;
}
double SECardiovascularSystem::GetMeanArterialCarbonDioxidePartialPressure(const PressureUnit& unit) const
{
	if (m_MeanArterialCarbonDioxidePartialPressure == nullptr)
		return SEScalar::dNaN();
	return m_MeanArterialCarbonDioxidePartialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasMeanArterialCarbonDioxidePartialPressureDelta() const
{
	return m_MeanArterialCarbonDioxidePartialPressureDelta == nullptr ? false : m_MeanArterialCarbonDioxidePartialPressureDelta->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetMeanArterialCarbonDioxidePartialPressureDelta()
{
	if (m_MeanArterialCarbonDioxidePartialPressureDelta == nullptr)
		m_MeanArterialCarbonDioxidePartialPressureDelta = new SEScalarPressure();
	return *m_MeanArterialCarbonDioxidePartialPressureDelta;
}
double SECardiovascularSystem::GetMeanArterialCarbonDioxidePartialPressureDelta(const PressureUnit& unit) const
{
	if (m_MeanArterialCarbonDioxidePartialPressureDelta == nullptr)
		return SEScalar::dNaN();
	return m_MeanArterialCarbonDioxidePartialPressureDelta->GetValue(unit);
}

bool SECardiovascularSystem::HasMeanCentralVenousPressure() const
{
	return m_MeanCentralVenousPressure == nullptr ? false : m_MeanCentralVenousPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetMeanCentralVenousPressure()
{
	if (m_MeanCentralVenousPressure == nullptr)
		m_MeanCentralVenousPressure = new SEScalarPressure();
	return *m_MeanCentralVenousPressure;
}
double SECardiovascularSystem::GetMeanCentralVenousPressure(const PressureUnit& unit) const
{
	if (m_MeanCentralVenousPressure == nullptr)
		return SEScalar::dNaN();
	return m_MeanCentralVenousPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasMeanSkinFlow() const
{
	return m_MeanSkinFlow == nullptr ? false : m_MeanSkinFlow->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetMeanSkinFlow()
{
	if (m_MeanSkinFlow == nullptr)
		m_MeanSkinFlow = new SEScalarVolumePerTime();
	return *m_MeanSkinFlow;
}
double SECardiovascularSystem::GetMeanSkinFlow(const VolumePerTimeUnit& unit) const
{
	if (m_MeanSkinFlow == nullptr)
		return SEScalar::dNaN();
	return m_MeanSkinFlow->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryArterialPressure() const
{
	return m_PulmonaryArterialPressure == nullptr ? false : m_PulmonaryArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulmonaryArterialPressure()
{
	if (m_PulmonaryArterialPressure == nullptr)
		m_PulmonaryArterialPressure = new SEScalarPressure();
	return *m_PulmonaryArterialPressure;
}
double SECardiovascularSystem::GetPulmonaryArterialPressure(const PressureUnit& unit) const
{
	if (m_PulmonaryArterialPressure == nullptr)
		return SEScalar::dNaN();
	return m_PulmonaryArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryCapillariesWedgePressure() const
{
	return m_PulmonaryCapillariesWedgePressure==nullptr?false:m_PulmonaryCapillariesWedgePressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulmonaryCapillariesWedgePressure()
{
	if(m_PulmonaryCapillariesWedgePressure==nullptr)
		m_PulmonaryCapillariesWedgePressure=new SEScalarPressure();
	return *m_PulmonaryCapillariesWedgePressure;
}
double SECardiovascularSystem::GetPulmonaryCapillariesWedgePressure(const PressureUnit& unit) const
{
	if (m_PulmonaryCapillariesWedgePressure == nullptr)
		return SEScalar::dNaN();
	return m_PulmonaryCapillariesWedgePressure->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryDiastolicArterialPressure() const
{
	return m_PulmonaryDiastolicArterialPressure==nullptr?false:m_PulmonaryDiastolicArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulmonaryDiastolicArterialPressure()
{
	if(m_PulmonaryDiastolicArterialPressure==nullptr)
		m_PulmonaryDiastolicArterialPressure=new SEScalarPressure();
	return *m_PulmonaryDiastolicArterialPressure;
}
double SECardiovascularSystem::GetPulmonaryDiastolicArterialPressure(const PressureUnit& unit) const
{
	if (m_PulmonaryDiastolicArterialPressure == nullptr)
		return SEScalar::dNaN();
	return m_PulmonaryDiastolicArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryMeanArterialPressure() const
{
	return m_PulmonaryMeanArterialPressure==nullptr?false:m_PulmonaryMeanArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulmonaryMeanArterialPressure()
{
	if(m_PulmonaryMeanArterialPressure==nullptr)
		m_PulmonaryMeanArterialPressure=new SEScalarPressure();
	return *m_PulmonaryMeanArterialPressure;
}
double SECardiovascularSystem::GetPulmonaryMeanArterialPressure(const PressureUnit& unit) const
{
	if (m_PulmonaryMeanArterialPressure == nullptr)
		return SEScalar::dNaN();
	return m_PulmonaryMeanArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryMeanCapillaryFlow() const
{
	return m_PulmonaryMeanCapillaryFlow==nullptr?false:m_PulmonaryMeanCapillaryFlow->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetPulmonaryMeanCapillaryFlow()
{
	if(m_PulmonaryMeanCapillaryFlow==nullptr)
		m_PulmonaryMeanCapillaryFlow=new SEScalarVolumePerTime();
	return *m_PulmonaryMeanCapillaryFlow;
}
double SECardiovascularSystem::GetPulmonaryMeanCapillaryFlow(const VolumePerTimeUnit& unit) const
{
	if (m_PulmonaryMeanCapillaryFlow == nullptr)
		return SEScalar::dNaN();
	return m_PulmonaryMeanCapillaryFlow->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryMeanShuntFlow() const
{
	return m_PulmonaryMeanShuntFlow==nullptr?false:m_PulmonaryMeanShuntFlow->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetPulmonaryMeanShuntFlow()
{
	if(m_PulmonaryMeanShuntFlow==nullptr)
		m_PulmonaryMeanShuntFlow=new SEScalarVolumePerTime();
	return *m_PulmonaryMeanShuntFlow;
}
double SECardiovascularSystem::GetPulmonaryMeanShuntFlow(const VolumePerTimeUnit& unit) const
{
	if (m_PulmonaryMeanShuntFlow == nullptr)
		return SEScalar::dNaN();
	return m_PulmonaryMeanShuntFlow->GetValue(unit);
}

bool SECardiovascularSystem::HasSystemicVascularResistance() const
{
	return m_SystemicVascularResistance == nullptr ? false : m_SystemicVascularResistance->IsValid();
}
SEScalarFlowResistance& SECardiovascularSystem::GetSystemicVascularResistance()
{
	if (m_SystemicVascularResistance == nullptr)
		m_SystemicVascularResistance = new SEScalarFlowResistance();
	return *m_SystemicVascularResistance;
}
double SECardiovascularSystem::GetSystemicVascularResistance(const FlowResistanceUnit& unit) const
{
	if (m_SystemicVascularResistance == nullptr)
		return SEScalar::dNaN();
	return m_SystemicVascularResistance->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonarySystolicArterialPressure() const
{
	return m_PulmonarySystolicArterialPressure==nullptr?false:m_PulmonarySystolicArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulmonarySystolicArterialPressure()
{
	if(m_PulmonarySystolicArterialPressure==nullptr)
		m_PulmonarySystolicArterialPressure=new SEScalarPressure();
	return *m_PulmonarySystolicArterialPressure;
}
double SECardiovascularSystem::GetPulmonarySystolicArterialPressure(const PressureUnit& unit) const
{
	if (m_PulmonarySystolicArterialPressure == nullptr)
		return SEScalar::dNaN();
	return m_PulmonarySystolicArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryVascularResistance() const
{
  return m_PulmonaryVascularResistance == nullptr ? false : m_PulmonaryVascularResistance->IsValid();
}
SEScalarFlowResistance& SECardiovascularSystem::GetPulmonaryVascularResistance()
{
  if (m_PulmonaryVascularResistance == nullptr)
    m_PulmonaryVascularResistance = new SEScalarFlowResistance();
  return *m_PulmonaryVascularResistance;
}
double SECardiovascularSystem::GetPulmonaryVascularResistance(const FlowResistanceUnit& unit) const
{
  if (m_PulmonaryVascularResistance == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryVascularResistance->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryVascularResistanceIndex() const
{
  return m_PulmonaryVascularResistanceIndex == nullptr ? false : m_PulmonaryVascularResistanceIndex->IsValid();
}
SEScalarPressureTimePerVolumeArea& SECardiovascularSystem::GetPulmonaryVascularResistanceIndex()
{
  if (m_PulmonaryVascularResistanceIndex == nullptr)
    m_PulmonaryVascularResistanceIndex = new SEScalarPressureTimePerVolumeArea();
  return *m_PulmonaryVascularResistanceIndex;
}
double SECardiovascularSystem::GetPulmonaryVascularResistanceIndex(const PressureTimePerVolumeAreaUnit& unit) const
{
  if (m_PulmonaryVascularResistanceIndex == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryVascularResistanceIndex->GetValue(unit);
}

bool SECardiovascularSystem::HasPulsePressure() const
{
	return m_PulsePressure==nullptr?false:m_PulsePressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulsePressure()
{
	if(m_PulsePressure==nullptr)
		m_PulsePressure=new SEScalarPressure();
	return *m_PulsePressure;
}
double SECardiovascularSystem::GetPulsePressure(const PressureUnit& unit) const
{
	if (m_PulsePressure == nullptr)
		return SEScalar::dNaN();
	return m_PulsePressure->GetValue(unit);
}

bool SECardiovascularSystem::HasSystolicArterialPressure() const
{
	return m_SystolicArterialPressure==nullptr?false:m_SystolicArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetSystolicArterialPressure()
{
	if(m_SystolicArterialPressure==nullptr)
		m_SystolicArterialPressure=new SEScalarPressure();
	return *m_SystolicArterialPressure;
}
double SECardiovascularSystem::GetSystolicArterialPressure(const PressureUnit& unit) const
{
	if (m_SystolicArterialPressure == nullptr)
		return SEScalar::dNaN();
	return m_SystolicArterialPressure->GetValue(unit);
}
