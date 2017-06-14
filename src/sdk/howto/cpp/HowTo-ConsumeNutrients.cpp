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
#include "patient/actions/SEConsumeNutrients.h"
#include "system/physiology/SEGastrointestinalSystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SEEnergySystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "patient/SENutrition.h"
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
#include "engine/PhysiologyEngineTrack.h"
#include "compartment/SECompartmentManager.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for applying a ConsumeNutrients insult to the patient
///
/// \details
/// Refer to the SEConsumeNutrients class
//--------------------------------------------------------------------------------------------------
void HowToConsumeNutrients() 
{
  // Create the engine and load the patient
	std::unique_ptr<PhysiologyEngine> bg = CreateBioGearsEngine("HowToConsumeNutrients.log");
  bg->GetLogger()->Info("HowToConsumeNutrients");
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
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("BloodVolume", VolumeUnit::mL);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("MeanArterialPressure", PressureUnit::mmHg);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("SystolicArterialPressure", PressureUnit::mmHg);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("DiastolicArterialPressure", PressureUnit::mmHg);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("StomachContents-Water",VolumeUnit::mL);
	
  bg->GetEngineTrack()->GetDataRequestManager().SetResultsFilename("HowToConsumeNutrients.txt");

	// Advance some time to get some resting data
	tracker.AdvanceModelTime(50);
	
	bg->GetLogger()->Info("The patient is nice and healthy");
	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << bg->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
	bg->GetLogger()->Info(std::stringstream() <<"Stomach Water : " << bg->GetGastrointestinalSystem()->GetStomachContents()->GetWater(VolumeUnit::mL) << VolumeUnit::mL);;

	// ConsumeNutrients Starts - instantiate a ConsumeNutrients action and have the engine process it
	// Have some water
	SEConsumeNutrients eat;
	eat.GetNutrition().GetWater().SetValue(50, VolumeUnit::mL);
	bg->ProcessAction(eat);

	tracker.AdvanceModelTime(30);

	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << bg->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
	bg->GetLogger()->Info(std::stringstream() <<"Stomach Water : " << bg->GetGastrointestinalSystem()->GetStomachContents()->GetWater(VolumeUnit::mL) << VolumeUnit::mL);;
  bg->GetLogger()->Info("Finished");
}
