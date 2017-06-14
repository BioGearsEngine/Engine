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
#include "system/environment/SEEnvironment.h"
#include "bind/BioGearsEnvironmentData.hxx"

/**
 * @brief The %Environment class characterizes the environment and manages interactions between the body its surroundings.
 */  
class BIOGEARS_API Environment : public SEEnvironment, public BioGearsSystem
{
	friend BioGears;
  friend class BioGearsEngineTest;
protected:
	Environment(BioGears& bg);
	BioGears& m_data;

public:
  virtual ~Environment();

  void Clear();

  // Set members to a stable homeostatic state
  void Initialize();

  // Load a state
  virtual bool Load(const CDM::BioGearsEnvironmentData& in);
  virtual CDM::BioGearsEnvironmentData* Unload() const;
protected:
  virtual void Unload(CDM::BioGearsEnvironmentData& data) const;

  // Set pointers and other member varialbes common to both homeostatic initialization and loading a state
  void SetUp();

public:
  void AtSteadyState();
	void PreProcess();
	void Process();
	void PostProcess();

  void StateChange();
protected:
	
	void ProcessActions();
	void CalculateSupplementalValues();
  /**/double AntoineEquation(double dTemperature_C);
	void CalculateRadiation();
	void CalculateConvection();
	void CalculateEvaporation();
	void CalculateRespiration();

  // Serializable member variables (Set in Initialize and in schema)
	

  // Stateless member variable (Calculated in Supplemental Method and used in other methods)
  double m_dLewisRelation;
  double m_dHeatOfVaporizationOfWater_J_Per_kg;
  double m_dWaterVaporPressureInAmbientAir_Pa;
  double m_dWaterVaporPressureAtSkin_Pa;
  //Prandtl number calculation
  double m_WaterSpecificHeat_J_Per_kg_K;
  double m_WaterViscosity_N_s_Per_m2;
  double m_WaterThermalConductivity_W_Per_m_K;
  //Grashof number calculation
  double m_ThermalExpansion_Per_K;
  // For Convection calculations
  double m_PatientEquivalentDiameter_m;

  // Stateless member variable (Set in SetUp())
  // Patient and Actions
  SEPatient*                     m_Patient;	
	SEPatientActionCollection*     m_PatientActions;
	SEEnvironmentActionCollection* m_EnvironmentActions;
  // Compartments
  SEGasCompartment*              m_AmbientGases;
  SELiquidCompartment*           m_AmbientAerosols;
	//Circuits                     
	SEThermalCircuit*              m_EnvironmentCircuit;
	//Nodes                        
	SEThermalCircuitNode*          m_ThermalEnvironment;
	SEThermalCircuitNode*          m_SkinNode;
	SEThermalCircuitNode*          m_ClothingNode;
	SEThermalCircuitNode*          m_EnclosureNode;
	//Paths                        
	SEThermalCircuitPath*          m_SkinToClothing;
	SEThermalCircuitPath*          m_ActiveHeatTransferRatePath;
	SEThermalCircuitPath*          m_ActiveTemperaturePath;
	SEThermalCircuitPath*          m_ActiveSwitchPath;
	SEThermalCircuitPath*          m_ClothingToEnclosurePath;
	SEThermalCircuitPath*          m_GroundToEnclosurePath;
	SEThermalCircuitPath*          m_ClothingToEnvironmentPath;
	SEThermalCircuitPath*          m_GroundToEnvironmentPath;
	SEThermalCircuitPath*          m_EnvironmentSkinToGroundPath;
	SEThermalCircuitPath*          m_EnvironmentCoreToGroundPath;
};
