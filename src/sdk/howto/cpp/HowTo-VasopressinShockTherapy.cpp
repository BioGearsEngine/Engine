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
#include "patient/actions/SESubstanceCompoundInfusion.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SEEnergySystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "system/physiology/SEDrugSystem.h"
#include "system/physiology/SENervousSystem.h"
#include "substance/SESubstanceManager.h"
#include "patient/SEPatient.h"
#include "patient/actions/SEHemorrhage.h"
#include "properties/SEScalar.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalar0To1.h"
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
void HowToVasopressinShockTherapy()
{
  // Create the engine and load the patient
	std::unique_ptr<PhysiologyEngine> bg = CreateBioGearsEngine("ShockTherapy_Control.log");
  bg->GetLogger()->Info("ShockTherapy_Control");

  if (!bg->InitializeEngine("StandardMale.xml"))
  {
	  bg->GetLogger()->Error("Could not load initialize engine, check the error");
	  return;
  }
  else
  {
	  bg->GetLogger()->Info("Engine stabilization complete");
  }

	//The tracker is responsible for advancing the engine time and outputting the data requests below at each time step
	HowToTracker tracker(*bg);

	//Set up substances.  Initialized vasopressin plasma concentration to 0.  Note that saline is technically a compound--this is
	//so the engine knows to look for multiple components within the same substance file (i.e. Na, Cl, etc)
	SESubstance* vas = bg->GetSubstanceManager().GetSubstance("Vasopressin");
	vas->GetPlasmaConcentration().SetValue(0.0, MassPerVolumeUnit::ug_Per_L);
	SESubstanceCompound* sal = bg->GetSubstanceManager().GetCompound("Saline");
	
	//Each infusion is managed by a separate object
	//This object is the vasopressin infusion.  It requires a concentration and an admin rate.  The infusion will continue unabated until the 
	//rate of infusion is reset to 0.0
	SESubstanceInfusion vasInfuse(*vas);
	vasInfuse.GetConcentration().SetValue(10.0, MassPerVolumeUnit::ug_Per_mL);
	vasInfuse.GetRate().SetValue(0.33, VolumePerTimeUnit::mL_Per_min);
	
	//This object is the saline infusion that accompanies the vasopressin in the treatment group.  Compounds have a separate infusion object
	//that mimics an IV drip.  Thus, the compound infusion object assumes intravenous injection.  The user must specify the volume available to
	//administer and the rate.  We set the rate at 3.0 so that the combined vasopressin/saline mixture is given at 3.33 mL/min (or 200 mL/hr)
	SESubstanceCompoundInfusion treatmentInfuse(*sal);
	treatmentInfuse.GetBagVolume().SetValue(500.0, VolumeUnit::mL);
	treatmentInfuse.GetRate().SetValue(3.0, VolumePerTimeUnit::mL_Per_min);

	//This is the saline infusion given to the control group.  It is given a large bag volume to ensure that it will not run out during trial.
	SESubstanceCompoundInfusion controlInfuse(*sal);
	controlInfuse.GetBagVolume().SetValue(2000.0, VolumeUnit::mL);
	controlInfuse.GetRate().SetValue(3.33, VolumePerTimeUnit::mL_Per_min);

	//The two hemorrhage objects (one for each wound location) are created below.  Hemorrhages are stopped by setting the severity to 0.0
	SEHemorrhage venaCavaBleed;
	venaCavaBleed.SetCompartment("Vena Cava");
	venaCavaBleed.GetSeverity().SetValue(1.0);

	// Create data requests for each value that should be written to the output log as the engine is executing
	// Physiology System Names are defined on the System Objects in the Physiology.xsd file
	bg->GetEngineTrack()->GetDataRequestManager().CreateSubstanceDataRequest().Set(*vas, "PlasmaConcentration", MassPerVolumeUnit::ug_Per_L);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("HeartRate", FrequencyUnit::Per_min);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("HeartStrokeVolume", VolumeUnit::mL);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("SystolicArterialPressure", PressureUnit::mmHg);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("DiastolicArterialPressure", PressureUnit::mmHg);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("MeanArterialPressure", PressureUnit::mmHg);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("UrineProductionRate", VolumePerTimeUnit::mL_Per_min);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("UrineOsmolality", OsmolalityUnit::mOsm_Per_kg);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("CerbralPerfusionPressure", PressureUnit::mmHg);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("IntracranialPressure", PressureUnit::mmHg);
	bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("SystemicVascularResistance", FlowResistanceUnit::mmHg_s_Per_mL);
	bg->GetEngineTrack()->GetDataRequestManager().CreateLiquidCompartmentDataRequest().Set("SkinVasculature", "InFlow", VolumePerTimeUnit::mL_Per_min);
	bg->GetEngineTrack()->GetDataRequestManager().CreateLiquidCompartmentDataRequest().Set("BrainVasculature", "InFlow", VolumePerTimeUnit::mL_Per_min);
	bg->GetEngineTrack()->GetDataRequestManager().CreateLiquidCompartmentDataRequest().Set("MyocardiumVasculature", "InFlow", VolumePerTimeUnit::mL_Per_min);
	bg->GetEngineTrack()->GetDataRequestManager().CreateLiquidCompartmentDataRequest().Set("MuscleVasculature", "InFlow", VolumePerTimeUnit::mL_Per_min);
	bg->GetEngineTrack()->GetDataRequestManager().CreateLiquidCompartmentDataRequest().Set("SmallIntestineVasculature", "InFlow", VolumePerTimeUnit::mL_Per_min);
	bg->GetEngineTrack()->GetDataRequestManager().CreateLiquidCompartmentDataRequest().Set("Ground", "InFlow", VolumePerTimeUnit::mL_Per_min);

	bg->GetEngineTrack()->GetDataRequestManager().SetResultsFilename("ShockTherapy_Control.txt");

	bool control = true;	//Use to flag which variation you want to run

	bg->GetLogger()->Info("Beginning Scenario");

	//Initiate both hemorrhage actions and advance engine eight minutes
	bg->ProcessAction(venaCavaBleed);
	tracker.AdvanceModelTime(480);

	//Remove artery bleeding, begin initial fluid rescue, advance engine ten minutes
	venaCavaBleed.GetSeverity().SetValue(0.0);
	bg->ProcessAction(venaCavaBleed);

	//Begin either the control treatment (control = true) or experimental treatment (control = false). Toggle between using boolean above
	if (control)
	{
		bg->ProcessAction(controlInfuse);
	}
	else
	{
		bg->ProcessAction(vasInfuse);
		bg->ProcessAction(treatmentInfuse);
	}

	//Allow fluid administration for 30 minutes 
	tracker.AdvanceModelTime(1800);

	bg->GetLogger()->Info("Finished");
}