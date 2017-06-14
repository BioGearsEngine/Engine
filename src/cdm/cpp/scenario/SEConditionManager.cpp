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
#include "scenario/SEConditionManager.h"
#include "scenario/SECondition.h"
#include "substance/SESubstanceManager.h"
//Patient Conditions
#include "patient/conditions/SEChronicAnemia.h"
#include "bind/ChronicAnemiaData.hxx"
#include "patient/conditions/SEChronicObstructivePulmonaryDisease.h"
#include "bind/ChronicObstructivePulmonaryDiseaseData.hxx"
#include "patient/conditions/SEChronicHeartFailure.h"
#include "bind/ChronicHeartFailureData.hxx"
#include "patient/conditions/SEChronicRenalStenosis.h"
#include "bind/ChronicRenalStenosisData.hxx"
#include "patient/conditions/SEChronicVentricularSystolicDysfunction.h"
#include "bind/ChronicVentricularSystolicDysfunctionData.hxx"
#include "patient/conditions/SEConsumeMeal.h"
#include "bind/ConsumeMealData.hxx"
#include "patient/conditions/SELobarPneumonia.h"
#include "bind/LobarPneumoniaData.hxx"
#include "patient/conditions/SEChronicPericardialEffusion.h"
#include "bind/ChronicPericardialEffusionData.hxx"

#include "patient/conditions/SEImpairedAlveolarExchange.h"
#include "bind/ImpairedAlveolarExchangeData.hxx"
// Environment Conditions
#include "system/environment/conditions/SEInitialEnvironment.h"
#include "bind/InitialEnvironmentData.hxx"



SEConditionManager::SEConditionManager(SESubstanceManager& substances) : Loggable(substances.GetLogger()), m_Substances(substances)
{
	m_Anemia = nullptr;
	m_COPD = nullptr;
	m_HeartFailure = nullptr;
	m_RenalStenosis = nullptr;
	m_Dehydration = nullptr;
	m_ConsumeMeal = nullptr;
	m_LobarPneumonia = nullptr;
	m_PericardialEffusion = nullptr;
  m_ImpairedAlveolarExchange = nullptr;
	m_InitialEnvironment = nullptr;
}

SEConditionManager::~SEConditionManager()
{
	Clear();
}

void SEConditionManager::Clear()
{
	m_Anemia = nullptr;
	m_COPD = nullptr;
	m_HeartFailure = nullptr;
	m_RenalStenosis = nullptr;
	m_Dehydration = nullptr;
	m_ConsumeMeal = nullptr;
	m_LobarPneumonia = nullptr;
	m_PericardialEffusion = nullptr;
  m_ImpairedAlveolarExchange = nullptr;
	m_InitialEnvironment = nullptr;
	DELETE_VECTOR(m_Conditions);
}

void SEConditionManager::Unload(std::vector<CDM::ConditionData*>& to)
{
  for (SECondition* c : m_Conditions)
    to.push_back(c->Unload());
}

bool SEConditionManager::ProcessCondition(const SECondition& condition)
{
  //if (!IsValid(action))
  //  return false;
  CDM::ConditionData* bind = condition.Unload();
  bool b = ProcessCondition(*bind);
  delete bind;
  return b;
}

bool SEConditionManager::ProcessCondition(const CDM::ConditionData& condition)
{
  const CDM::ChronicAnemiaData* a = dynamic_cast<const CDM::ChronicAnemiaData*>(&condition);
	if (a != nullptr)
	{
		if (HasChronicAnemia())
		{
			Error("Cannot have multiple Anemia conditions");
			return false;
		}
    m_Anemia = new SEChronicAnemia();
    m_Anemia->Load(*a);
		m_Conditions.push_back(m_Anemia);
		return true;
	}	
 
  const CDM::ChronicObstructivePulmonaryDiseaseData* copd = dynamic_cast<const CDM::ChronicObstructivePulmonaryDiseaseData*>(&condition);
	if (copd != nullptr)
	{
		if (HasChronicObstructivePulmonaryDisease())
		{
			Error("Cannot have multiple COPD conditions");
			return false;
		}
		m_COPD = new SEChronicObstructivePulmonaryDisease();
    m_COPD->Load(*copd);
		m_Conditions.push_back(m_COPD);
		return true;
	}
	
  const CDM::ChronicHeartFailureData* h = dynamic_cast<const CDM::ChronicHeartFailureData*>(&condition);
	if (h != nullptr)
	{
		if (HasChronicHeartFailure())
		{
			Error("Cannot have multiple Heart Failure conditions");
			return false;
		}
    const CDM::ChronicVentricularSystolicDysfunctionData* vsd = dynamic_cast<const CDM::ChronicVentricularSystolicDysfunctionData*>(&condition);
		if (vsd != nullptr)
		{
			m_HeartFailure = new SEChronicVentricularSystolicDysfunction();
      m_HeartFailure->Load(*vsd);
			m_Conditions.push_back(m_HeartFailure);
			return true;
		}
		Error("Unknown Heart Failure condition");		
		return false;
	}	

  const CDM::ChronicPericardialEffusionData* pe = dynamic_cast<const CDM::ChronicPericardialEffusionData*>(&condition);
  if (pe != nullptr)
  {
    if (HasChronicPericardialEffusion())
    {
      Error("Cannot have multiple Pericardial Effusion conditions");
      return false;
    }
    m_PericardialEffusion = new SEChronicPericardialEffusion();
    m_PericardialEffusion->Load(*pe);
    m_Conditions.push_back(m_PericardialEffusion);
    return true;
  }

  const CDM::ChronicRenalStenosisData* r = dynamic_cast<const CDM::ChronicRenalStenosisData*>(&condition);
	if (r != nullptr)
	{
		if (HasChronicRenalStenosis())
		{
			Error("Cannot have multiple Renal Stenosis conditions");
			return false;
		}
		m_RenalStenosis = new SEChronicRenalStenosis();
    m_RenalStenosis->Load(*r);
		m_Conditions.push_back(m_RenalStenosis);
		return true;
	}

  const CDM::ConsumeMealData* g = dynamic_cast<const CDM::ConsumeMealData*>(&condition);
	if (g != nullptr)
	{
		if (HasConsumeMeal())
		{
			Error("Cannot have multiple Gut Nutrient conditions");
			return false;
		}		
		m_ConsumeMeal = new SEConsumeMeal();
    m_ConsumeMeal->Load(*g);
		m_Conditions.push_back(m_ConsumeMeal);
		return true;
	}

  const CDM::ImpairedAlveolarExchangeData* iae = dynamic_cast<const CDM::ImpairedAlveolarExchangeData*>(&condition);
  if (iae != nullptr)
  {
    if (HasImpairedAlveolarExchange())
    {
      Error("Cannot have multiple Impaired Alveolar Exchange conditions");
      return false;
    }
    m_ImpairedAlveolarExchange = new SEImpairedAlveolarExchange();
    m_ImpairedAlveolarExchange->Load(*iae);
    m_Conditions.push_back(m_ImpairedAlveolarExchange);
    return true;
  }

  const CDM::LobarPneumoniaData* lp = dynamic_cast<const CDM::LobarPneumoniaData*>(&condition);
	if (lp != nullptr)
	{
		if (HasLobarPneumonia())
		{
			Error("Cannot have multiple Lobar Pneumonia conditions");
			return false;
		}
		m_LobarPneumonia = new SELobarPneumonia();
    m_LobarPneumonia->Load(*lp);
		m_Conditions.push_back(m_LobarPneumonia);
		return true;
	}
  

  const CDM::InitialEnvironmentData* ie = dynamic_cast<const CDM::InitialEnvironmentData*>(&condition);
	if (ie != nullptr)
	{
		if (HasInitialEnvironment())
		{
			Error("Cannot have multiple Initial Environment conditions");
			return false;
		}
		m_InitialEnvironment = new SEInitialEnvironment(m_Substances);
    m_InitialEnvironment->Load(*ie);
		m_Conditions.push_back(m_InitialEnvironment);
		return true;
	}
	
	/// \error Unsupported Condition
	Error("Unsupported Condition");
	return false;
}

bool SEConditionManager::HasChronicAnemia() const
{
	return m_Anemia != nullptr;
}
SEChronicAnemia* SEConditionManager::GetChronicAnemia() const
{
	return m_Anemia;
}

bool SEConditionManager::HasChronicObstructivePulmonaryDisease() const
{
	return m_COPD != nullptr;
}
SEChronicObstructivePulmonaryDisease* SEConditionManager::GetChronicObstructivePulmonaryDisease() const
{
	return m_COPD;
}

bool SEConditionManager::HasChronicHeartFailure() const
{
	return m_HeartFailure != nullptr;
}
bool SEConditionManager::HasChronicVentricularSystolicDysfunction() const
{
	return m_HeartFailure == nullptr ? false : dynamic_cast<SEChronicVentricularSystolicDysfunction*>(m_HeartFailure) != nullptr;
}
SEChronicVentricularSystolicDysfunction* SEConditionManager::GetChronicVentricularSystolicDysfunction() const
{
	return dynamic_cast<SEChronicVentricularSystolicDysfunction*>(m_HeartFailure);
}

bool SEConditionManager::HasChronicPericardialEffusion() const
{
  return m_PericardialEffusion != nullptr;
}
SEChronicPericardialEffusion* SEConditionManager::GetChronicPericardialEffusion() const
{
  return m_PericardialEffusion;
}

bool SEConditionManager::HasChronicRenalStenosis() const
{
	return m_RenalStenosis != nullptr;
}
SEChronicRenalStenosis* SEConditionManager::GetChronicRenalStenosis() const
{
	return m_RenalStenosis;
}

bool SEConditionManager::HasDehydration() const
{
	return m_Dehydration != nullptr;
}
SEDehydration* SEConditionManager::GetDehydration() const
{
	return m_Dehydration;
}

bool SEConditionManager::HasConsumeMeal() const
{
	return m_ConsumeMeal != nullptr;
}
SEConsumeMeal* SEConditionManager::GetConsumeMeal() const
{
	return m_ConsumeMeal;
}

bool SEConditionManager::HasImpairedAlveolarExchange() const
{
  return m_ImpairedAlveolarExchange == nullptr ? false : true;
}
SEImpairedAlveolarExchange* SEConditionManager::GetImpairedAlveolarExchange() const
{
  return m_ImpairedAlveolarExchange;
}

bool SEConditionManager::HasLobarPneumonia() const
{
	return m_LobarPneumonia != nullptr;
}
SELobarPneumonia* SEConditionManager::GetLobarPneumonia() const
{
	return m_LobarPneumonia;
}

bool SEConditionManager::HasInitialEnvironment() const
{
	return m_InitialEnvironment != nullptr;
}
SEInitialEnvironment* SEConditionManager::GetInitialEnvironment() const
{
	return m_InitialEnvironment;
}