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
#include "patient/actions/SEAsthmaAttack.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEScalar0To1.h"
#include "engine/PhysiologyEngineTrack.h"
#include "compartment/SECompartmentManager.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for applying an Asthma Attack insult to the patient
///
/// \details
/// Refer to the SEAsthmaAttack class
//--------------------------------------------------------------------------------------------------
void HowToAsthmaAttack() 
{
  // Create the engine and load the patient
	std::unique_ptr<PhysiologyEngine> bg = CreateBioGearsEngine("HowToAsthma.log");
  bg->GetLogger()->Info("HowToAsthmaAttack");
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
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("HemoglobinContent",MassUnit::g);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("InspiratoryExpiratoryRatio");
  bg->GetEngineTrack()->GetDataRequestManager().CreateGasCompartmentDataRequest().Set(BGE::PulmonaryCompartment::Carina, "InFlow");

  bg->GetEngineTrack()->GetDataRequestManager().SetResultsFilename("HowToAsthma.txt");

	// Advance some time to get some healthy data
	tracker.AdvanceModelTime(50);
  // Cache off compartments of interest!
  const SEGasCompartment* carina = bg->GetCompartments().GetGasCompartment(BGE::PulmonaryCompartment::Carina);
	
	bg->GetLogger()->Info("The patient is nice and healthy");
	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << bg->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
	bg->GetLogger()->Info(std::stringstream() <<"InspiratoryExpiratoryRatio : " << bg->GetRespiratorySystem()->GetInspiratoryExpiratoryRatio());
	bg->GetLogger()->Info(std::stringstream() <<"Carina InFlow : " << carina->GetInFlow(VolumePerTimeUnit::L_Per_s) << VolumePerTimeUnit::L_Per_s);;

	// Asthma Attack Starts - instantiate an asthma attack action and have the engine process it
	// Asthma is a common inflammatory disease of the airways where air flow into the lungs is partially obstructed. 
	// Acute asthma is an exacerbation of asthma that does not respond to standard treatments. 
	// BioGears models asthma by increasing the airway flow resistance in the circuit model. 
	// The higher the severity, the more severe the asthma attack.
	SEAsthmaAttack asthmaAttack;
	asthmaAttack.GetSeverity().SetValue(0.3);
	bg->ProcessAction(asthmaAttack);

	tracker.AdvanceModelTime(550);

	bg->GetLogger()->Info("The patient has been having an asthma attack for 550s");
	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << bg->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
	bg->GetLogger()->Info(std::stringstream() <<"InspiratoryExpiratoryRatio : " << bg->GetRespiratorySystem()->GetInspiratoryExpiratoryRatio());
	bg->GetLogger()->Info(std::stringstream() <<"Carina InFlow : " << carina->GetInFlow(VolumePerTimeUnit::L_Per_s) << VolumePerTimeUnit::L_Per_s);;

	// Asthma Attack Stops
	asthmaAttack.GetSeverity().SetValue(0.0);
	bg->ProcessAction(asthmaAttack);
	
	// Advance some time while the patient catches their breath
	tracker.AdvanceModelTime(200);

	bg->GetLogger()->Info("The patient has NOT had an asthma attack for 200s");
	bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << bg->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
	bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
	bg->GetLogger()->Info(std::stringstream() <<"InspiratoryExpiratoryRatio : " << bg->GetRespiratorySystem()->GetInspiratoryExpiratoryRatio());
	bg->GetLogger()->Info(std::stringstream() <<"Carina InFlow : " << carina->GetInFlow(VolumePerTimeUnit::L_Per_s) << VolumePerTimeUnit::L_Per_s);;
  bg->GetLogger()->Info("Finished");
}
