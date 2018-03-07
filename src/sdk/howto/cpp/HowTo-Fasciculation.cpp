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
#include "patient/SEPatient.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SEEnergySystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "substance/SESubstanceManager.h"
#include "substance/SESubstanceCompound.h"
#include "properties/SEScalarAmountPerVolume.h"
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
/// Demonstrates how to set concentrations of ionic compounds in bloodstream and tissue and call events
/// based on changes in concentrations
///
/// \details
/// Refer to the SEEnvironmentChange class
/// Refer to the SEDrug Class
//--------------------------------------------------------------------------------------------------

void HowToFasciculation()
{
	// Create the engine and load the patient
	std::unique_ptr<PhysiologyEngine> bg = CreateBioGearsEngine("HowToFasciculation.log");
	bg->GetLogger()->Info("HowToFasciculation");

	if (!bg->LoadState("./states/StandardMale@0s.xml"))
	{
		bg->GetLogger()->Error("Could not load state, check the error");
		return;
	}

	//---Initialize all variables needed for scenario
	SESubstance* Na = bg->GetSubstanceManager().GetSubstance("Sodium");
	SESubstance* K = bg->GetSubstanceManager().GetSubstance("Potassium");
	SESubstance* Cl = bg->GetSubstanceManager().GetSubstance("Chloride");
	SESubstance* Ca = bg->GetSubstanceManager().GetSubstance("Calcium");

	double monitorTime = 200.0;			//how long we're going to be on the look out for ion imbalances in the blood
	CDM::enumOnOff::value lowKActive;
	lowKActive = CDM::enumOnOff::Off;

	// The tracker is responsible for advancing the engine time and outputting the data requests below at each time step
	HowToTracker tracker(*bg);

	bg->GetEngineTrack()->GetDataRequestManager().CreateLiquidCompartmentDataRequest().Set("VenaCava", *Na, "Molarity", AmountPerVolumeUnit::mmol_Per_L);
	bg->GetEngineTrack()->GetDataRequestManager().CreateLiquidCompartmentDataRequest().Set("VenaCava", *K, "Molarity", AmountPerVolumeUnit::mmol_Per_L);
	bg->GetEngineTrack()->GetDataRequestManager().CreateLiquidCompartmentDataRequest().Set("VenaCava", *Cl, "Molarity", AmountPerVolumeUnit::mmol_Per_L);
	bg->GetEngineTrack()->GetDataRequestManager().CreateLiquidCompartmentDataRequest().Set("VenaCava", *Ca, "Molarity", AmountPerVolumeUnit::mmol_Per_L);

	bg->GetEngineTrack()->GetDataRequestManager().SetResultsFilename("HowToFasciculation.txt");

	// Advance some time to get some resting data
	tracker.AdvanceModelTime(60);

	bg->GetLogger()->Info("The patient is nice and healthy");
	std::string message = "";
	message = "Increase membrane resistance to potassium";
	bg->GetLogger()->Info(message);
	K->GetMembraneResistance().SetValue(5.0, ElectricResistanceUnit::Ohm);


	while (bg->GetSimulationTime(TimeUnit::s) < monitorTime + 60.0)
	{
		if (bg->GetPatient().IsEventActive(CDM::enumPatientEvent::MildHypokalemia) && (lowKActive == CDM::enumOnOff::Off))
		{
			lowKActive = CDM::enumOnOff::On;
			message = "Patient has low serum potassium, muscle fasciculation may occur";
			bg->GetLogger()->Info(message);
		}
		tracker.AdvanceModelTime(10.0);
	}
	message = "Return membrane resistance to potassium to baseline";
	bg->GetLogger()->Info(message);
	K->GetMembraneResistance().SetValue(0.248, ElectricResistanceUnit::Ohm);

	tracker.AdvanceModelTime(300);
}
