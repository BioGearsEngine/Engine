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
#include "patient/actions/SEHemorrhage.h"
#include "patient/actions/SESubstanceCompoundInfusion.h"
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
#include "engine/PhysiologyEngineTrack.h"
#include "compartment/SECompartmentManager.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for applying a Hemorrhage insult to the patient
///
/// \details
/// Refer to the SEHemorrhage class
/// Refer to the SESubstanceManager class
/// Refer to the SESubstanceIVFluids class for applying an IV to the patient
//--------------------------------------------------------------------------------------------------
void HowToHemorrhage() 
{
  // Create the engine and load the patient
	std::unique_ptr<PhysiologyEngine> bg = CreateBioGearsEngine("HowToHemorrhage.log");
  bg->GetLogger()->Info("HowToHemorrhage");
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
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("HemoglobinContent",MassUnit::g);
	
  bg->GetEngineTrack()->GetDataRequestManager().SetResultsFilename("HowToHemorrhage.txt");
	
	bg->GetLogger()->Info("The patient is nice and healthy");
	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << bg->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
	bg->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << bg->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");;

	// Hemorrhage Starts - instantiate a hemorrhage action and have the engine process it
	SEHemorrhage hemorrhageLeg;
	hemorrhageLeg.SetCompartment(BGE::VascularCompartment::RightLeg);//the location of the hemorrhage
	hemorrhageLeg.GetRate().SetValue(250,VolumePerTimeUnit::mL_Per_min);//the rate of hemorrhage
	bg->ProcessAction(hemorrhageLeg);

	// Advance some time to let the body bleed out a bit
	tracker.AdvanceModelTime(300);

	bg->GetLogger()->Info("The patient has been hemorrhaging for 300s");
	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << bg->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
	bg->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << bg->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");;

	// Hemorrhage is sealed
	hemorrhageLeg.SetCompartment(BGE::VascularCompartment::RightLeg);//location of hemorrhage
	hemorrhageLeg.GetRate().SetValue(0,VolumePerTimeUnit::mL_Per_min);//rate is set to 0 to close the bleed
	bg->ProcessAction(hemorrhageLeg);
	
	
	// Advance some time while the medic gets the drugs ready
	tracker.AdvanceModelTime(100);

	bg->GetLogger()->Info("The patient has NOT been hemorrhaging for 100s");
	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << bg->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
	bg->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << bg->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");;

	// Patient is stabilizing, but not great

	// Let's administer a saline drip, we need to get saline from the substance maganer
	SESubstanceCompound* saline = bg->GetSubstanceManager().GetCompound("Saline");
	SESubstanceCompoundInfusion iVSaline(*saline);
	iVSaline.GetBagVolume().SetValue(500,VolumeUnit::mL);//the total volume in the bag of Saline
	iVSaline.GetRate().SetValue(100,VolumePerTimeUnit::mL_Per_min);//The rate to admnister the compound in the bag in this case saline
	bg->ProcessAction(iVSaline);

	tracker.AdvanceModelTime(400);

	bg->GetLogger()->Info("The patient has been getting fluids for the past 400s");
	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << bg->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
	bg->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << bg->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");;
  bg->GetLogger()->Info("Finished");
}
