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

#include "BioGearsEngineHowTo.h"

// Include the various types you will be using in your code
#include "patient/actions/SEExercise.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SEEnergySystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "substance/SESubstanceManager.h"
#include "substance/SESubstanceCompound.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEScalarPower.h"
#include "properties/SEScalar0To1.h"
#include "engine/PhysiologyEngineTrack.h"
#include "compartment/SECompartmentManager.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for applying an Exercise insult to the patient
///
/// \details
/// Refer to the SEExercise class
//--------------------------------------------------------------------------------------------------
void HowToExercise() 
{
  // Create the engine and load the patient
	std::unique_ptr<PhysiologyEngine> bg = CreateBioGearsEngine("HowToExercise.log");
  bg->GetLogger()->Info("HowToExercise");
	if (!bg->LoadState("./states/StandardMale@0s.xml"))
  {
    bg->GetLogger()->Error("Could not load state, check the error");
    return;
  }

    // The tracker is responsible for advancing the engine time and outputting the data requests below at each time step
	HowToTracker tracker(*bg);

	// Create data requests for each value that should be written to the output log as the engine is executing
	// Physiology System Names are defined on the System Objects 
	// defined in the Physiology.xsd file
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("HeartRate", FrequencyUnit::Per_min);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("MeanArterialPressure", PressureUnit::mmHg);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("SystolicArterialPressure", PressureUnit::mmHg);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("DiastolicArterialPressure", PressureUnit::mmHg);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("TotalMetabolicRate", PowerUnit::kcal_Per_day);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("CoreTemperature", TemperatureUnit::C);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("RespirationRate", FrequencyUnit::Per_min);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("AchievedExerciseLevel");
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("FatigueLevel");
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("TotalMetabolicRate", PowerUnit::W);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("TotalWorkRateLevel");
	
  bg->GetEngineTrack()->GetDataRequestManager().SetResultsFilename("HowToExercise.txt");

	// Advance some time to get some resting data
	tracker.AdvanceModelTime(20);
	
	bg->GetLogger()->Info("The patient is nice and healthy");
	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
	bg->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << bg->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::kcal_Per_day) << PowerUnit::kcal_Per_day);
	bg->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << bg->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
	bg->GetLogger()->Info(std::stringstream() <<"RespirationRate : " << bg->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  //bg->GetLogger()->Info(std::stringstream() <<"AchievedExerciseLevel : " << bg->GetEnergySystem()->GetAchievedExerciseLevel()); // This will be NaN as the patient is not doing any exercise
  //bg->GetLogger()->Info(std::stringstream() <<"FatigueLevel : " << bg->GetEnergySystem()->GetFatigueLevel()); // No fatigue either
  bg->GetLogger()->Info(std::stringstream() <<"TotalMetabolicRate : " << bg->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);//This will be at Basal Level
  bg->GetLogger()->Info(std::stringstream() <<"TotalWorkRateLevel : " << bg->GetEnergySystem()->GetTotalWorkRateLevel());; // Nothing here either as the patient is resting

	// Exercise Starts - instantiate an Exercise action and have the engine process it.
	// After initiating exercise the patient�s metabolic rate begins to increased. 
	// An intensity of 1.0 asks the patient to exercise at the maximum work rate capable by the patient (an output of ~1200W for a patient in relativly good shape).
  // The patient will not be able to sustain this pace for long... and will get fatigued
	// This leads to an increase in core temperature, cardiac output, respiration rate and tidal volume.
  // The increase in core temperature leads to an elevated sweat rate, which causes the patient�s skin temperature to drop due to evaporation. 
	SEExercise ex;
	ex.GetIntensity().SetValue(0.5);
	bg->ProcessAction(ex);
	tracker.AdvanceModelTime(30);

	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
	bg->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << bg->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::kcal_Per_day) << PowerUnit::kcal_Per_day);
	bg->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << bg->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
	bg->GetLogger()->Info(std::stringstream() <<"RespirationRate : " << bg->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  bg->GetLogger()->Info(std::stringstream() <<"AchievedExerciseLevel : " << bg->GetEnergySystem()->GetAchievedExerciseLevel()); // this is the fraction of what we asked for, 1.0 means we are doing what you asked for.
  bg->GetLogger()->Info(std::stringstream() <<"FatigueLevel : " << bg->GetEnergySystem()->GetFatigueLevel()); // Patient is very tired
  bg->GetLogger()->Info(std::stringstream() <<"TotalMetabolicRate : " << bg->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);// We are burning 
  bg->GetLogger()->Info(std::stringstream() <<"TotalWorkRateLevel : " << bg->GetEnergySystem()->GetTotalWorkRateLevel());; // How much work we are getting / the max work rate possible

	// Once exercise has ended, the patient is in a recovery period where the metabolic rate begins to return to its basal value. 
	// The cardiac output, respiration rate and tidal volume follow this recovery trend towards their normal values.
	ex.GetIntensity().SetValue(0);
	bg->ProcessAction(ex);
	
	// Advance some time while the medic gets the drugs ready
	tracker.AdvanceModelTime(30);

	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
	bg->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << bg->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::kcal_Per_day) << PowerUnit::kcal_Per_day);
	bg->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << bg->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
	bg->GetLogger()->Info(std::stringstream() <<"RespirationRate : " << bg->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  //bg->GetLogger()->Info(std::stringstream() <<"AchievedExerciseLevel : " << bg->GetEnergySystem()->GetAchievedExerciseLevel()); // This will be NaN as the patient is not doing any exercise
  //bg->GetLogger()->Info(std::stringstream() <<"FatigueLevel : " << bg->GetEnergySystem()->GetFatigueLevel()); // We are not working out but we are still fatigued
  bg->GetLogger()->Info(std::stringstream() <<"TotalMetabolicRate : " << bg->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);// We are still burning 
  bg->GetLogger()->Info(std::stringstream() <<"TotalWorkRateLevel : " << bg->GetEnergySystem()->GetTotalWorkRateLevel());; // How much work we are getting over the max work rate possible
  bg->GetLogger()->Info("Finished");
}
