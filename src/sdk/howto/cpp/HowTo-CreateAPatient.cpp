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
#include "patient/SEPatient.h"
#include "patient/assessments/SEPulmonaryFunctionTest.h"
#include "compartment/SECompartmentManager.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SEEnergySystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "substance/SESubstanceManager.h"
#include "substance/SESubstance.h"
#include "engine/PhysiologyEngineTrack.h"
#include "utils/SEEventHandler.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEFunctionVolumeVsTime.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarLength.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Creating a patient
///
/// \details
/// Creating a customized patient in BioGears
//--------------------------------------------------------------------------------------------------
void HowToCreateAPatient()
{
  std::unique_ptr<PhysiologyEngine> bg = CreateBioGearsEngine("HowToEngineUse.log");
  bg->GetLogger()->Info("HowToCreateAPatient");

  SEPatient patient(bg->GetLogger());
  patient.SetName("HowToCreateAPatient");
  //Patient sex is the only thing that is absolutely required to be set.
  //All value not explicitly set based or standard values or calculations.
  //If you do something out of bounds or set something you're not allowed to, it will alert you with a warning/error.
  patient.SetSex(CDM::enumSex::Male);
  patient.GetAge().SetValue(44, TimeUnit::yr);
  patient.GetWeight().SetValue(170, MassUnit::lb);
  patient.GetHeight().SetValue(71, LengthUnit::in);
  patient.GetBodyFatFraction().SetValue(0.21);
  patient.GetDiastolicArterialPressureBaseline().SetValue(74, PressureUnit::mmHg);
  patient.GetHeartRateBaseline().SetValue(72, FrequencyUnit::Per_min);
  patient.GetRespirationRateBaseline().SetValue(16, FrequencyUnit::Per_min);
  patient.GetSystolicArterialPressureBaseline().SetValue(114, PressureUnit::mmHg);

  // You can save off the patient if you want to use it later
  CDM::PatientData* pData = patient.Unload();
  // Write out the stable patient state
  std::ofstream stream("./patients/HowToCreateAPatient.xml");
  // Write out the xml file
  xml_schema::namespace_infomap map;
  map[""].name = "uri:/mil/tatrc/physiology/datamodel";
  Patient(stream, *pData, map);
  stream.close();
  SAFE_DELETE(pData);

	if (!bg->InitializeEngine(patient))
	{
		bg->GetLogger()->Error("Could not load state, check the error");
		return;
	}

  // You can save off the initial patient state if you want to use it later
  bg->SaveState("./states/HowToCreateAPatient@0s.xml");  
}