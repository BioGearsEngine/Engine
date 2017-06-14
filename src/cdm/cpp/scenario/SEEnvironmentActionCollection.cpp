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
#include "scenario/SEEnvironmentActionCollection.h"
#include "system/environment/SEEnvironment.h"
#include "substance/SESubstanceFraction.h"
#include "properties/SEScalarLengthPerTime.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarHeatConductancePerArea.h"
#include "properties/SEScalarHeatResistanceArea.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTemperature.h"

SEEnvironmentActionCollection::SEEnvironmentActionCollection(SESubstanceManager& substances) : Loggable(substances.GetLogger()), m_Substances(substances)
{
	m_Change = nullptr;
	m_ThermalApplication = nullptr;
}

SEEnvironmentActionCollection::~SEEnvironmentActionCollection()
{
	Clear();
}

void SEEnvironmentActionCollection::Clear()
{
	RemoveChange();
	RemoveThermalApplication();
}

void SEEnvironmentActionCollection::Unload(std::vector<CDM::ActionData*>& to)
{
  if (HasChange())
    to.push_back(GetChange()->Unload());
  if (HasThermalApplication())
    to.push_back(GetThermalApplication()->Unload());
}

bool SEEnvironmentActionCollection::ProcessAction(const SEEnvironmentAction& action)
{
  if (!IsValid(action))
    return false;
  CDM::EnvironmentActionData* bind = action.Unload();
  bool b = ProcessAction(*bind);
  delete bind;
  return b;
}

bool SEEnvironmentActionCollection::ProcessAction(const CDM::EnvironmentActionData& action)
{
  const CDM::EnvironmentChangeData* change = dynamic_cast<const CDM::EnvironmentChangeData*>(&action);
	if (change != nullptr)
	{
		if (m_Change == nullptr)
			m_Change = new SEEnvironmentChange(m_Substances);
	  m_Change->Load(*change);   
		return IsValid(*m_Change);
	}

  const CDM::ThermalApplicationData *thermal = dynamic_cast<const CDM::ThermalApplicationData*>(&action);
	if (thermal != nullptr)
	{    
		if (m_ThermalApplication == nullptr)
			m_ThermalApplication = new SEThermalApplication();		
		m_ThermalApplication->Load(*thermal);
    if (!m_ThermalApplication->IsActive())
    {
      RemoveThermalApplication();
      return true;
    }
    return IsValid(*m_ThermalApplication);
	}

	/// \error Unsupported Action
	Error("Unsupported Action");
	return false;
}

bool SEEnvironmentActionCollection::IsValid(const SEEnvironmentAction& action)
{
  if (!action.IsValid())
  {
    Error("Invalid Inhaler Environment Action");
    return false;
  }
  return true;
}

bool SEEnvironmentActionCollection::HasChange() const
{
	return m_Change == nullptr ? false : true;
}
SEEnvironmentChange* SEEnvironmentActionCollection::GetChange() const
{
	return m_Change;
}
void SEEnvironmentActionCollection::RemoveChange()
{
	m_Change = nullptr;
}

bool SEEnvironmentActionCollection::HasThermalApplication() const
{
	return m_ThermalApplication != nullptr;
}
SEThermalApplication* SEEnvironmentActionCollection::GetThermalApplication() const
{
	return m_ThermalApplication;
}
void SEEnvironmentActionCollection::RemoveThermalApplication()
{
	SAFE_DELETE(m_ThermalApplication);
}