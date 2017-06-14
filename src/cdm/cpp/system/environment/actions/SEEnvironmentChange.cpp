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
#include "system/environment/actions/SEEnvironmentChange.h"
#include "bind/EnvironmentalConditionsData.hxx"
#include "substance/SESubstanceFraction.h"

#include "properties/SEScalarFraction.h"
#include "properties/SEScalarHeatConductancePerArea.h"
#include "properties/SEScalarLengthPerTime.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarPower.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarHeatResistanceArea.h"

#include "substance/SESubstance.h"
#include "substance/SESubstanceFraction.h"
#include "substance/SESubstanceConcentration.h"
#include "substance/SESubstanceManager.h"

SEEnvironmentChange::SEEnvironmentChange(SESubstanceManager& substances) : SEEnvironmentAction(), m_Substances(substances)
{
	m_Conditions = nullptr;
	InvalidateConditionsFile();
}

SEEnvironmentChange::~SEEnvironmentChange()
{
	Clear();
}

void SEEnvironmentChange::Clear()
{
	SEEnvironmentAction::Clear();
	InvalidateConditionsFile();
	SAFE_DELETE(m_Conditions);
}

bool SEEnvironmentChange::IsValid() const
{
	return SEEnvironmentAction::IsValid() && (HasConditions() || HasConditionsFile());
}

bool SEEnvironmentChange::Load(const CDM::EnvironmentChangeData& in)
{
	SEEnvironmentAction::Load(in);
	if (in.ConditionsFile().present())
		SetConditionsFile(in.ConditionsFile().get());
	else if (in.Conditions().present())
		GetConditions().Load(in.Conditions().get());
	return true;
}

CDM::EnvironmentChangeData* SEEnvironmentChange::Unload() const
{
	CDM::EnvironmentChangeData* data = new CDM::EnvironmentChangeData();
	Unload(*data);
	return data;
}

void SEEnvironmentChange::Unload(CDM::EnvironmentChangeData& data) const
{
	SEEnvironmentAction::Unload(data);
	if (HasConditions())
		data.Conditions(std::unique_ptr<CDM::EnvironmentalConditionsData>(m_Conditions->Unload()));
	else if (HasConditionsFile())
		data.ConditionsFile(m_ConditionsFile);
}

bool SEEnvironmentChange::HasConditions() const
{
	return m_Conditions != nullptr;
}
SEEnvironmentalConditions& SEEnvironmentChange::GetConditions()
{
	m_ConditionsFile = "";
	if (m_Conditions == nullptr)
		m_Conditions = new SEEnvironmentalConditions(m_Substances);
	return *m_Conditions;
}
const SEEnvironmentalConditions* SEEnvironmentChange::GetConditions() const
{
	return m_Conditions;
}

std::string SEEnvironmentChange::GetConditionsFile() const
{
	return m_ConditionsFile;
}
void SEEnvironmentChange::SetConditionsFile(const std::string& fileName)
{
	if (m_Conditions != nullptr)
		SAFE_DELETE(m_Conditions);
	m_ConditionsFile = fileName;
}
bool SEEnvironmentChange::HasConditionsFile() const
{
	return m_ConditionsFile.empty() ? false : true;
}
void SEEnvironmentChange::InvalidateConditionsFile()
{
	m_ConditionsFile = "";
}


void SEEnvironmentChange::ToString(std::ostream &str) const
{
	str << "Environment Action : Environment Change"; 
	if(HasComment())
		str<<"\n\tComment: "<<m_Comment;
	if (HasConditionsFile())
		str << "\n\tConditions File: "; str << m_ConditionsFile;
	if (HasConditions())
	{
		str << "\n\tSurroundingType: "; m_Conditions->HasSurroundingType() ? str << m_Conditions->GetSurroundingType() : str << "Not Set";
		str << "\n\tAir Velocity: ";  m_Conditions->HasAirVelocity() ? str << m_Conditions->GetAirVelocity() : str << "Not Set";
		str << "\n\tAmbient Temperature: ";  m_Conditions->HasAmbientTemperature() ? str << m_Conditions->GetAmbientTemperature() : str << "Not Set";
		str << "\n\tAtmospheric Pressure: ";  m_Conditions->HasAtmosphericPressure() ? str << m_Conditions->GetAtmosphericPressure() : str << "Not Set";
		str << "\n\tClothing Resistance: ";  m_Conditions->HasClothingResistance() ? str << m_Conditions->GetClothingResistance() : str << "Not Set";
		str << "\n\tEmissivity: ";  m_Conditions->HasEmissivity() ? str << m_Conditions->GetEmissivity() : str << "Not Set";
		str << "\n\tMean Radiant Temperature: ";  m_Conditions->HasMeanRadiantTemperature() ? str << m_Conditions->GetMeanRadiantTemperature() : str << "Not Set";
		str << "\n\tRelative Humidity: ";  m_Conditions->HasRelativeHumidity() ? str << m_Conditions->GetRelativeHumidity() : str << "Not Set";
		str << "\n\tRespiration Ambient Temperature: ";  m_Conditions->HasRespirationAmbientTemperature() ? str << m_Conditions->GetRespirationAmbientTemperature() : str << "Not Set";
		if (m_Conditions->HasAmbientGas())
		{
			for (SESubstanceFraction* sf : m_Conditions->GetAmbientGases())
			{
				str << "\n\tSubstance : " << sf->GetSubstance().GetName() << " Fraction Amount " << sf->GetFractionAmount();
			}
		}
    if (m_Conditions->HasAmbientAerosol())
    {
      for (SESubstanceConcentration* sc : m_Conditions->GetAmbientAerosols())
      {
        str << "\n\tSubstance : " << sc->GetSubstance().GetName() << " Concentration " << sc->GetConcentration();
      }
    }
	}
	str << std::flush;
}

