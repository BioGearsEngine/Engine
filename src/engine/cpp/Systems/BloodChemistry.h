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

#include "../Controller/BioGearsSystem.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "bind/BioGearsBloodChemistrySystemData.hxx"
#include "utils/RunningAverage.h"
class SECompleteBloodCount;
class SEComprehensiveMetabolicPanel;

/**
 * @brief @copydoc Physiology_BloodChemistrySystemData
 * The Blood Chemistry System holds the system-level blood substance data that is computed on the compartment level by other systems.
 * @details
 * The goal of the blood chemistry system in the BioGears engine is complete analysis on the blood at any given point during the simulation.
 * The blood chemistry system houses all of the blood concentrations and compositions needed to assess a patient's health. This system is under development
 * and will be improved in future releases to include more substances that can provide clinician level details and assessments, such as a CBC and blood panel.
 */           
class BIOGEARS_API BloodChemistry : public SEBloodChemistrySystem, public BioGearsSystem
{
	friend BioGears;
  friend class BioGearsEngineTest;
protected:
	BloodChemistry(BioGears& bg);
	BioGears& m_data;

public:
  virtual ~BloodChemistry();

  void Clear();

  // Set members to a stable homeostatic state
  void Initialize();

  // Load a state
  bool Load(const CDM::BioGearsBloodChemistrySystemData& in);                                  
  CDM::BioGearsBloodChemistrySystemData* Unload() const;
protected:
  void Unload(CDM::BioGearsBloodChemistrySystemData& data) const;

  // Set pointers and other member varialbes common to both homeostatic initialization and loading a state
  void SetUp();

public:
	void AtSteadyState();
	void PreProcess();
	void Process();
	void PostProcess();

	bool CalculateCompleteBloodCount(SECompleteBloodCount& cbc);
	bool CalculateComprehensiveMetabolicPanel(SEComprehensiveMetabolicPanel& cmp);

protected:
  void CheckBloodGasLevels();
	
  // Serializable member variables (Set in Initialize and in schema)
  RunningAverage m_ArterialOxygen_mmHg;
  RunningAverage m_ArterialCarbonDioxide_mmHg;

  // Stateless member variable (Set in SetUp())
	double m_redBloodCellVolume_mL;
	double m_HbPerRedBloodCell_ug_Per_ct;
	SELiquidCompartment*       m_aorta;
	SELiquidSubstanceQuantity* m_aortaO2;
	SELiquidSubstanceQuantity* m_aortaCO2;
  SELiquidSubstanceQuantity* m_aortaCO;
	SELiquidSubstanceQuantity* m_aortaBicarbonate;
	SELiquidSubstanceQuantity* m_brainO2;
	SELiquidSubstanceQuantity* m_myocardiumO2;
  SELiquidSubstanceQuantity* m_pulmonaryArteriesO2;
  SELiquidSubstanceQuantity* m_pulmonaryArteriesCO2;
  SELiquidSubstanceQuantity* m_pulmonaryVeinsO2;
  SELiquidSubstanceQuantity* m_pulmonaryVeinsCO2;
	SELiquidCompartment*       m_venaCava;
	SELiquidSubstanceQuantity* m_venaCavaO2;
	SELiquidSubstanceQuantity* m_venaCavaCO2;
	SELiquidSubstanceQuantity* m_venaCavaAcetoacetate;
	SELiquidSubstanceQuantity* m_venaCavaAlbumin;
	SELiquidSubstanceQuantity* m_venaCavaBicarbonate;
	SELiquidSubstanceQuantity* m_venaCavaCalcium;
	SELiquidSubstanceQuantity* m_venaCavaChloride;
	SELiquidSubstanceQuantity* m_venaCavaCreatinine;
  SELiquidSubstanceQuantity* m_venaCavaEpinephrine;
	SELiquidSubstanceQuantity* m_venaCavaGlucose;
	SELiquidSubstanceQuantity* m_venaCavaInsulin;
	SELiquidSubstanceQuantity* m_venaCavaLactate;
	SELiquidSubstanceQuantity* m_venaCavaPotassium;
	SELiquidSubstanceQuantity* m_venaCavaSodium;
	SELiquidSubstanceQuantity* m_venaCavaTristearin;
	SELiquidSubstanceQuantity* m_venaCavaUrea;  
};
