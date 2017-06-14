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
#include "patient/SEPatient.h"
#include "patient/actions/SEAcuteStress.h"
#include "patient/actions/SEAirwayObstruction.h"
#include "patient/actions/SEApnea.h"
#include "patient/actions/SEBrainInjury.h"
#include "patient/actions/SEBronchoconstriction.h"
#include "patient/actions/SECardiacArrest.h"
#include "patient/actions/SEAsthmaAttack.h"
#include "patient/actions/SEChestCompressionForce.h"
#include "patient/actions/SEChestCompressionForceScale.h"
#include "patient/actions/SEChestOcclusiveDressing.h"
#include "patient/actions/SEConsciousRespiration.h"
#include "patient/actions/SEConsumeNutrients.h"
#include "patient/actions/SEExercise.h"
#include "patient/actions/SEHemorrhage.h"
#include "patient/actions/SEIntubation.h"
#include "patient/actions/SEMechanicalVentilation.h"
#include "patient/actions/SENeedleDecompression.h"
#include "patient/actions/SEPericardialEffusion.h"
#include "patient/actions/SETensionPneumothorax.h"
#include "patient/actions/SESubstanceBolus.h"
#include "patient/actions/SESubstanceCompoundInfusion.h"
#include "patient/actions/SESubstanceInfusion.h"
#include "patient/actions/SEUrinate.h"
#include "patient/actions/SEPatientAssessmentRequest.h"

#include "substance/SESubstanceManager.h"

#include "system/physiology/SEGastrointestinalSystem.h"

class DLL_DECL SEPatientActionCollection : public Loggable
{
public:

	SEPatientActionCollection(SESubstanceManager&);
	~SEPatientActionCollection();

	void Clear();

  void Unload(std::vector<CDM::ActionData*>& to);

  bool ProcessAction(const SEPatientAction& action);
  bool ProcessAction(const CDM::PatientActionData& action);

  bool HasAcuteStress() const;
  SEAcuteStress* GetAcuteStress() const;
  void RemoveAcuteStress();

	bool HasAirwayObstruction() const;
	SEAirwayObstruction* GetAirwayObstruction() const;
	void RemoveAirwayObstruction();

  bool HasApnea() const;
  SEApnea* GetApnea() const;
  void RemoveApnea();

	bool HasAsthmaAttack() const;
	SEAsthmaAttack* GetAsthmaAttack() const;
	void RemoveAsthmaAttack();

  bool HasBrainInjury() const;
  SEBrainInjury* GetBrainInjury() const;
  void RemoveBrainInjury();

	bool HasBronchoconstriction() const;
	SEBronchoconstriction* GetBronchoconstriction() const;
	void RemoveBronchoconstriction();

	bool HasCardiacArrest() const;
	SECardiacArrest* GetCardiacArrest() const;
	void RemoveCardiacArrest();

	bool HasChestCompression() const;
	void RemoveChestCompression();
	bool HasChestCompressionForce() const;
	SEChestCompressionForce* GetChestCompressionForce() const;
	bool HasChestCompressionForceScale() const;
	SEChestCompressionForceScale* GetChestCompressionForceScale() const;

	bool HasChestOcclusiveDressing() const;
	bool HasLeftChestOcclusiveDressing() const;
	SEChestOcclusiveDressing* GetLeftChestOcclusiveDressing() const;
	void RemoveLeftChestOcclusiveDressing();
	bool HasRightChestOcclusiveDressing() const;
	SEChestOcclusiveDressing* GetRightChestOcclusiveDressing() const;
	void RemoveRightChestOcclusiveDressing();

	bool HasConsciousRespiration() const;
	SEConsciousRespiration* GetConsciousRespiration() const;
	void RemoveConsciousRespiration();

	bool HasConsumeNutrients() const;
	SEConsumeNutrients* GetConsumeNutrients() const;
	void RemoveConsumeNutrients();

	bool HasExercise() const;
	SEExercise* GetExercise() const;
	void RemoveExercise();

	bool HasHemorrhage() const;
	const std::map<std::string,SEHemorrhage*>& GetHemorrhages() const;
	void RemoveHemorrhage(const std::string& cmpt);

	bool HasIntubation() const;
  SEIntubation* GetIntubation() const;
	void RemoveIntubation();

  bool HasMechanicalVentilation() const;
  SEMechanicalVentilation* GetMechanicalVentilation() const;
  void RemoveMechanicalVentilation();

	bool HasNeedleDecompression() const;

	bool HasLeftNeedleDecompression() const;
	SENeedleDecompression* GetLeftNeedleDecompression() const;
	void RemoveLeftNeedleDecompression();

	bool HasRightNeedleDecompression() const;
	SENeedleDecompression* GetRightNeedleDecompression() const;
	void RemoveRightNeedleDecompression();

	bool HasPericardialEffusion() const;
	SEPericardialEffusion* GetPericardialEffusion() const;
	void RemovePericardialEffusion();

	bool HasTensionPneumothorax() const;

	bool HasLeftClosedTensionPneumothorax() const;
	SETensionPneumothorax* GetLeftClosedTensionPneumothorax() const;
	void RemoveLeftClosedTensionPneumothorax();

	bool HasLeftOpenTensionPneumothorax() const;
	SETensionPneumothorax* GetLeftOpenTensionPneumothorax() const;
	void RemoveLeftOpenTensionPneumothorax();

	bool HasRightClosedTensionPneumothorax() const;
	SETensionPneumothorax* GetRightClosedTensionPneumothorax() const;
	void RemoveRightClosedTensionPneumothorax();

	bool HasRightOpenTensionPneumothorax() const;
	SETensionPneumothorax* GetRightOpenTensionPneumothorax() const;
	void RemoveRightOpenTensionPneumothorax();

	const std::map<const SESubstance*, SESubstanceBolus*>&  GetSubstanceBoluses() const;
	void RemoveSubstanceBolus(const SESubstance& sub);

	const std::map<const SESubstance*, SESubstanceInfusion*>& GetSubstanceInfusions() const;
	void RemoveSubstanceInfusion(const SESubstance& sub);

	const std::map<const SESubstanceCompound*, SESubstanceCompoundInfusion*>&  GetSubstanceCompoundInfusions() const;
	void RemoveSubstanceCompoundInfusion(const SESubstanceCompound& sub);

	bool HasUrinate() const;
	SEUrinate* GetUrinate() const;
	void RemoveUrinate();

protected:
  bool IsValid(const SEPatientAction& action);

  SEAcuteStress*                m_AcuteStress;
	SEAirwayObstruction*				  m_AirwayObstruction;
  SEApnea*				              m_Apnea;
	SEAsthmaAttack*							  m_AsthmaAttack;
  SEBrainInjury*							  m_BrainInjury;
	SEBronchoconstriction*				m_Bronchoconstriction;
	SECardiacArrest*              m_CardiacArrest;
	SEChestCompression*					  m_ChestCompression;
	SEChestOcclusiveDressing*			m_LeftChestOcclusiveDressing;
	SEChestOcclusiveDressing*			m_RightChestOcclusiveDressing;
	SEConsciousRespiration*       m_ConsciousRespiration;
	SEConsumeNutrients*           m_ConsumeNutrients;
	SEExercise*					          m_Exercise;
	SEIntubation*					        m_Intubation;
  SEMechanicalVentilation*	    m_MechanicalVentilation;
	SENeedleDecompression*				m_LeftNeedleDecompression;
	SENeedleDecompression*				m_RightNeedleDecompression;
	SEPericardialEffusion*				m_PericardialEffusion;
	SETensionPneumothorax*				m_LeftClosedTensionPneumothorax;
	SETensionPneumothorax*				m_LeftOpenTensionPneumothorax;
	SETensionPneumothorax*				m_RightClosedTensionPneumothorax;
	SETensionPneumothorax*				m_RightOpenTensionPneumothorax;
	SEUrinate*                    m_Urinate;

	std::map<std::string, SEHemorrhage*>                              m_Hemorrhages;
	std::map<const SESubstance*,SESubstanceBolus*>                    m_SubstanceBolus;
	std::map<const SESubstance*,SESubstanceInfusion*>                 m_SubstanceInfusions;
	std::map<const SESubstanceCompound*,SESubstanceCompoundInfusion*> m_SubstanceCompoundInfusions;

	bool AdministerSubstance(const CDM::SubstanceAdministrationData& subAdmin);

	SESubstanceManager& m_Substances;
	std::stringstream m_ss;
};
