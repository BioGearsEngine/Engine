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
class SECondition;
class SEChronicAnemia;
class SEChronicHeartFailure;
class SEChronicObstructivePulmonaryDisease;
class SEChronicPericardialEffusion;
class SEChronicVentricularSystolicDysfunction;
class SEChronicRenalStenosis;
class SEConsumeMeal;
class SEDehydration;
class SEFasting; 
class SEImpairedAlveolarExchange;
class SELobarPneumonia;
class SEInitialEnvironment;
class SESubstanceManager;
CDM_BIND_DECL(ConditionData);

class DLL_DECL SEConditionManager : public Loggable
{
public:

  SEConditionManager(SESubstanceManager&);
	~SEConditionManager();

	void Clear();

  void Unload(std::vector<CDM::ConditionData*>& to);

	bool ProcessCondition(const SECondition& condition);
  bool ProcessCondition(const CDM::ConditionData& condition);

	// Not too many conditions, so just have one manager
	// If we start getting alot, I will make patient/environment/equipment condition managers, like the action managers

	// Patient Conditions

	bool HasChronicAnemia() const;
	SEChronicAnemia* GetChronicAnemia() const;

	bool HasChronicObstructivePulmonaryDisease() const;
	SEChronicObstructivePulmonaryDisease* GetChronicObstructivePulmonaryDisease() const;

	bool HasChronicHeartFailure() const;
	bool HasChronicVentricularSystolicDysfunction() const;
	SEChronicVentricularSystolicDysfunction* GetChronicVentricularSystolicDysfunction() const;

	bool HasChronicPericardialEffusion() const;
	SEChronicPericardialEffusion* GetChronicPericardialEffusion() const;

	bool HasChronicRenalStenosis() const;
	SEChronicRenalStenosis* GetChronicRenalStenosis() const;

	bool HasConsumeMeal() const;
	SEConsumeMeal* GetConsumeMeal() const;

	bool HasDehydration() const;
	SEDehydration* GetDehydration() const;

  bool HasImpairedAlveolarExchange() const;
  SEImpairedAlveolarExchange* GetImpairedAlveolarExchange() const;

	bool HasLobarPneumonia() const;
	SELobarPneumonia* GetLobarPneumonia() const;

	// Environment Conditions

	bool HasInitialEnvironment() const;
	SEInitialEnvironment* GetInitialEnvironment() const;
	

protected:

  SESubstanceManager&                   m_Substances;
	SEChronicAnemia*                      m_Anemia;
	SEConsumeMeal*                        m_ConsumeMeal;
	SEChronicObstructivePulmonaryDisease* m_COPD;
	SEDehydration*                        m_Dehydration;
	SEChronicHeartFailure*                m_HeartFailure;
  SEImpairedAlveolarExchange*	          m_ImpairedAlveolarExchange;
	SEChronicPericardialEffusion*         m_PericardialEffusion;
	SELobarPneumonia*                     m_LobarPneumonia;
	SEChronicRenalStenosis*               m_RenalStenosis;

	SEInitialEnvironment*                 m_InitialEnvironment;

	std::vector<SECondition*>             m_Conditions;
	std::stringstream m_ss;
};