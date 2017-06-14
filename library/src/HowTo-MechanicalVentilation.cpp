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
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "patient/actions/SEMechanicalVentilation.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEScalar0To1.h"
#include "engine/PhysiologyEngineTrack.h"
#include "substance/SESubstance.h"
#include "substance/SESubstanceFraction.h"
#include "substance/SESubstanceManager.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for the Mechanical Ventilation Patient Action
/// Drive respiration with your own driver
///
/// \details
/// Refer to the SEMechanicalVentilation class
//--------------------------------------------------------------------------------------------------
void HowToMechanicalVentilation()
{
  std::stringstream ss;
  // Create a BioGears Engine and load the standard patient
  std::unique_ptr<PhysiologyEngine> bg = CreateBioGearsEngine("HowToMechanicalVentilation.log");
  
  bg->GetLogger()->Info("HowToMechanicalVentilation");
	if (!bg->LoadState("./states/StandardMale@0s.xml"))
  {
    bg->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // The tracker is responsible for advancing the engine time and outputting the data requests below at each time step
	HowToTracker tracker(*bg);

	// Create data requests for each value that should be written to the output log as the engine is executing
	// Physiology System Names are defined on the System Objects 
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("HeartRate", FrequencyUnit::Per_min);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("SystolicArterialPressure", PressureUnit::mmHg);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("DiastolicArterialPressure", PressureUnit::mmHg);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("RespirationRate", FrequencyUnit::Per_min);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("TidalVolume", VolumeUnit::mL);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("TotalLungVolume", VolumeUnit::mL);
  bg->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("OxygenSaturation");

	bg->GetEngineTrack()->GetDataRequestManager().SetResultsFilename("HowToMechanicalVentilation.txt");

  bg->GetLogger()->Info("The patient is nice and healthy");
  bg->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << bg->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  bg->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  bg->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  bg->GetLogger()->Info(std::stringstream() << "Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  bg->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << bg->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  bg->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << bg->GetBloodChemistrySystem()->GetOxygenSaturation());

	tracker.AdvanceModelTime(5);

  // Create an SEMechanicalVentilation object
  SEMechanicalVentilation mechVent;
  // Grab the substance fractions so we can quickly modify them
  SESubstanceFraction& O2frac = mechVent.GetGasFraction(*bg->GetSubstanceManager().GetSubstance("Oxygen"));
  SESubstanceFraction& CO2frac = mechVent.GetGasFraction(*bg->GetSubstanceManager().GetSubstance("CarbonDioxide"));
  SESubstanceFraction& N2frac = mechVent.GetGasFraction(*bg->GetSubstanceManager().GetSubstance("Nitrogen"));

  // Drive the system for 2 mins
  for (int i = 0; i < 120; i++)
  {    
    // Going to update values every second

    mechVent.GetPressure().SetValue(0, PressureUnit::cmH2O);
    mechVent.GetFlow().SetValue(0, VolumePerTimeUnit::mL_Per_s);
    O2frac.GetFractionAmount().SetValue(0.21);
    CO2frac.GetFractionAmount().SetValue(4.0E-4);
    N2frac.GetFractionAmount().SetValue(0.7896);    
    bg->ProcessAction(mechVent);

    tracker.AdvanceModelTime(1);

    bg->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << bg->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
    bg->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
    bg->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
    bg->GetLogger()->Info(std::stringstream() << "Heart Rate : " << bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
    bg->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << bg->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
    bg->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << bg->GetBloodChemistrySystem()->GetOxygenSaturation());
  }

  bg->GetLogger()->Info("Finished");
}