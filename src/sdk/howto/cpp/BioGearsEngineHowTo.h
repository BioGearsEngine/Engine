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

// Note that this project is set with the following Additional Include Paths: ../include;../include/cdm;../include/cdm/bind
// This will build an executable that is intended to execute a how-to method

#include "bind/biogears-cdm.hxx"
#include "CommonDataModel.h"            // Needs to be included before BioGearsPhysiologyEngine.h
#include "BioGearsPhysiologyEngine.h"

// The following how-to functions are defined in their own file
void HowToEngineUse();
void HowToCreateAPatient();

void HowToAirwayObstruction();
void HowToAnesthesiaMachine();
void HowToAsthmaAttack();
void HowToBrainInjury();
void HowToBolusDrug();
void HowToConsumeNutrients();
void HowToCOPD();
void HowToCPR();
void HowToEnvironmentChange();
void HowToExercise();
void HowToFasciculation();
void HowToHemorrhage();
void HowToInfusionDrug();
void HowToLobarPneumonia();
void HowToMechanicalVentilation();
void HowToPulmonaryFunctionTest();
void HowToSarinExposure();
void HowToSmoke();
void HowToTensionPneumothorax();
void HowToVasopressinShockTherapy();

void HowToConcurrentEngines();
void HowToRunScenario();
void HowToDynamicHemorrhage();

class SEDataRequest;

/// This class is here to demonstrate executing the engine
/// and populating a txt file with data from the engine 
/// This class will handle advancing time on the engine
class HowToTracker
{
private:
	double m_dT_s;  // Cached Engine Time Step
	PhysiologyEngine& m_Engine;
public:
	HowToTracker(PhysiologyEngine& engine);
  ~HowToTracker();

	// This class will operate on seconds
	void AdvanceModelTime(double time_s);
};