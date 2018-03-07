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
#include "utils/SEEventHandler.h"
#include "patient/actions/SESubstanceBolus.h"
#include "patient/actions/SESubstanceInfusion.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SEEnergySystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "system/physiology/SEDrugSystem.h"
#include "substance/SESubstanceManager.h"
#include "patient/SEPatient.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEScalarOsmolality.h"
#include "engine/PhysiologyEngineTrack.h"
#include "compartment/SECompartmentManager.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for adminstering a substance to the patient via a continuous infusion
///
/// \details
/// Refer to the SESubstanceInfusion class
/// Refer to the SESubstanceManager class
//--------------------------------------------------------------------------------------------------
void HowToInfusionDrug()
{
  // Create the engine and load the patient
	std::unique_ptr<PhysiologyEngine> bg = CreateBioGearsEngine("VasopressinPD.log");
	bg->GetLogger()->Info("VasopressinPD");
	if (!bg->LoadState("StandardMale.xml"))
  {
    bg->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // The tracker is responsible for advancing the engine time and outputting the data requests below at each time step
	HowToTracker tracker(*bg);

	enum PDCase { LowDose,HighDose,RampDose };

	SESubstance* vas = bg->GetSubstanceManager().GetSubstance("Vasopressin");
	vas->GetPlasmaConcentration().SetValue(0.0, MassPerVolumeUnit::ug_Per_L);
	SESubstanceInfusion infuse(*vas);

	// Create data requests for each value that should be written to the output log as the engine is executing
	// Physiology System Names are defined on the System Objects 
	// defined in the Physiology.xsd file
	bg->GetEngineTrack()->GetDataRequestManager().CreateSubstanceDataRequest().Set(*vas, "PlasmaConcentration", MassPerVolumeUnit::ug_Per_L);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("HeartRate", FrequencyUnit::Per_min);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("SystolicArterialPressure", PressureUnit::mmHg);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("DiastolicArterialPressure", PressureUnit::mmHg);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("MeanArterialPressure", PressureUnit::mmHg);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("CardiacOutput", VolumePerTimeUnit::L_Per_min);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("UrineProductionRate", VolumePerTimeUnit::mL_Per_min);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("UrineOsmolarity", OsmolalityUnit::mOsm_Per_kg);

	bg->GetEngineTrack()->GetDataRequestManager().SetResultsFilename("VasopressinPD.txt");

	bg->GetLogger()->Info("Beginning PD Scenario");

	//Choose which test to run
	PDCase test = LowDose;

	switch (test) {
		case LowDose:
			infuse.GetConcentration().SetValue(0.200, MassPerVolumeUnit::ug_Per_mL);
			infuse.GetRate().SetValue(1.0, VolumePerTimeUnit::mL_Per_min);
			bg->ProcessAction(infuse);
			tracker.AdvanceModelTime(1200.0);
			break;
		case HighDose:
			infuse.GetConcentration().SetValue(0.484, MassPerVolumeUnit::ug_Per_mL);
			infuse.GetRate().SetValue(1.0, VolumePerTimeUnit::mL_Per_min);
			bg->ProcessAction(infuse);
			tracker.AdvanceModelTime(1200.0);
			break;
		case RampDose:
			infuse.GetConcentration().SetValue(0.0417, MassPerVolumeUnit::ug_Per_mL);
			infuse.GetRate().SetValue(1.0, VolumePerTimeUnit::mL_Per_min);
			bg->ProcessAction(infuse);
			tracker.AdvanceModelTime(1200.0);
			infuse.GetConcentration().SetValue(0.1754, MassPerVolumeUnit::ug_Per_mL);
			bg->ProcessAction(infuse);
			tracker.AdvanceModelTime(1200.0);
			infuse.GetConcentration().SetValue(0.518, MassPerVolumeUnit::ug_Per_mL);
			bg->ProcessAction(infuse);
			tracker.AdvanceModelTime(1200.0);
			break;
	}

  bg->GetLogger()->Info("Finished");
}