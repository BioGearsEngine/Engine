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

#include "stdafx.h"
#include "Energy.h"
#include "Drugs.h"
#include "Environment.h"
#include "Cardiovascular.h"
#include "BloodChemistry.h"
#include "bind/RunningAverageData.hxx"

#include "patient/SEPatient.h"
#include "patient/SEMeal.h"
#include "patient/SENutrition.h"
#include "patient/conditions/SEConsumeMeal.h"
#include "circuit/thermal/SEThermalCircuit.h"
#include "circuit/fluid/SEFluidCircuit.h"
#include "compartment/fluid/SELiquidCompartment.h"
#include "compartment/substances/SELiquidSubstanceQuantity.h"
#include "properties/SEScalar0To1.h"
#include "properties/SEScalarLength.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarMassPerTime.h"
#include "properties/SEScalarPower.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarArea.h"
#include "properties/SEScalarHeatCapacitancePerMass.h"
#include "properties/SEScalarHeatCapacitance.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarHeatResistance.h"
#include "properties/SEScalarHeatConductance.h"
#include "properties/SEScalarEnergyPerMass.h"
#include "properties/SEScalarEnergy.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarEnergyPerAmount.h"
#include "properties/SEScalarAmountPerVolume.h"
#include "properties/SEScalarAmountPerTime.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarFlowResistance.h"
#include "properties/SEScalarFlowCompliance.h"
#include "properties/SEScalarAmountPerTime.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolumePerTimeMass.h"

Energy::Energy(BioGears& bg) : SEEnergySystem(bg.GetLogger()), m_data(bg), m_circuitCalculator(GetLogger())
{
  Clear();
}

Energy::~Energy()
{
  Clear();
}

void Energy::Clear()
{
  SEEnergySystem::Clear();
  m_Patient = nullptr;
  m_PatientActions = nullptr;
  m_AortaHCO3 = nullptr;
  m_coreNode = nullptr;
  m_skinNode = nullptr;
  m_temperatureGroundToCorePath = nullptr;
  m_coreToSkinPath = nullptr;
  m_skinExtravascularToSweatingGroundPath = nullptr;
  m_InternalTemperatureCircuit = nullptr;
  m_TemperatureCircuit = nullptr;

  m_BloodpH.Reset(); 
  m_BicarbonateMolarity_mmol_Per_L.Reset();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Initializes system properties to valid homeostatic values.
//--------------------------------------------------------------------------------------------------
void Energy::Initialize()
{
  BioGearsSystem::Initialize();

  GetTotalMetabolicRate().Set(m_Patient->GetBasalMetabolicRate());
  //Initialization of other system variables
  /// \cite herman2008physics
  GetCoreTemperature().SetValue(37.0, TemperatureUnit::C);
  GetSkinTemperature().SetValue(33.0, TemperatureUnit::C);  
  /// \cite phypers2006lactate
  GetLactateProductionRate().SetValue(1.3, AmountPerTimeUnit::mol_Per_day);
  /// \cite garber1974ketone
  GetKetoneProductionRate().SetValue(300.0, AmountPerTimeUnit::umol_Per_min);
  /// \cite guyton2006medical
  GetExerciseMeanArterialPressureDelta().SetValue(0.0, PressureUnit::mmHg);
  GetTotalWorkRateLevel().SetValue(0.0);
  GetFatigueLevel().SetValue(0.0);

  //Running average quantities used to trigger events
  m_BloodpH.Sample(7.4); //Initialize
  m_BicarbonateMolarity_mmol_Per_L.Sample(24.0); //Initialize

  // Energy buckets for fatigue
  m_UsableEnergyStore_J = 2600.0;
  m_PeakPowerEnergyStore_J = 4200.0;
  m_MediumPowerEnergyStore_J = 35000.0;
  m_EnduranceEnergyStore_J = 400000.0;
}

bool Energy::Load(const CDM::BioGearsEnergySystemData& in)
{
  if (!SEEnergySystem::Load(in))
    return false;
  m_UsableEnergyStore_J = in.UsableEnergyStore_J();
  m_PeakPowerEnergyStore_J = in.PeakPowerEnergyStore_J();
  m_MediumPowerEnergyStore_J = in.MediumPowerEnergyStore_J();
  m_EnduranceEnergyStore_J = in.EnduranceEnergyStore_J();

  m_BloodpH.Load(in.BloodpH());
  m_BicarbonateMolarity_mmol_Per_L.Load(in.BicarbonateMolarity_mmol_Per_L());
  BioGearsSystem::LoadState();
  return true;
}
CDM::BioGearsEnergySystemData* Energy::Unload() const
{
  CDM::BioGearsEnergySystemData* data = new CDM::BioGearsEnergySystemData();
  Unload(*data);
  return data;
}
void Energy::Unload(CDM::BioGearsEnergySystemData& data) const
{
  SEEnergySystem::Unload(data);
  data.UsableEnergyStore_J(m_UsableEnergyStore_J);
  data.PeakPowerEnergyStore_J(m_PeakPowerEnergyStore_J);
  data.MediumPowerEnergyStore_J(m_MediumPowerEnergyStore_J);
  data.EnduranceEnergyStore_J(m_EnduranceEnergyStore_J);

  data.BloodpH(std::unique_ptr<CDM::RunningAverageData>(m_BloodpH.Unload()));
  data.BicarbonateMolarity_mmol_Per_L(std::unique_ptr<CDM::RunningAverageData>(m_BicarbonateMolarity_mmol_Per_L.Unload()));
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Initializes the energy specific quantities
///
/// \details
/// The energy local member variables, system level quantities, and metabolic-specific substance
/// quantities are initialized here. These variables are used in the numerous metabolic and thermal regulation
/// functions contained in the energy system. Encompassing them in this function allows for easy initialization
/// either at the beginning of a simulation or after a system crash
//--------------------------------------------------------------------------------------------------
void Energy::SetUp()
{
  m_dT_s = m_data.GetTimeStep().GetValue(TimeUnit::s);
	m_PatientActions = &m_data.GetActions().GetPatientActions();
	m_Patient = &m_data.GetPatient();

  m_AortaHCO3 = m_data.GetCompartments().GetLiquidCompartment(BGE::VascularCompartment::Aorta)->GetSubstanceQuantity(m_data.GetSubstances().GetHCO3());

	//Circuit elements
	//Circuits
  m_TemperatureCircuit = &m_data.GetCircuits().GetTemperatureCircuit();
  m_InternalTemperatureCircuit = &m_data.GetCircuits().GetInternalTemperatureCircuit();
	//Nodes
	m_coreNode = m_InternalTemperatureCircuit->GetNode(BGE::InternalTemperatureNode::InternalCore);
	m_skinNode = m_InternalTemperatureCircuit->GetNode(BGE::InternalTemperatureNode::InternalSkin);
	//Paths
	m_temperatureGroundToCorePath = m_InternalTemperatureCircuit->GetPath(BGE::InternalTemperaturePath::GroundToInternalCore);
  m_coreToSkinPath = m_InternalTemperatureCircuit->GetPath(BGE::InternalTemperaturePath::InternalCoreToInternalSkin);
  m_skinExtravascularToSweatingGroundPath = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetPath(BGE::TissuePath::SkinSweating);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Applies an imposed dehydration time to the current fluid stores
///
/// \details
/// Starvation uses a coarse time-step approach with an assumed time-step of one day. This coarse time-step
/// is used to to manually decrement all of the fluid stores which represents the loss due to urination, sweating, and other mechanisms.
/// For time-steps longer than one day there is a reduced fluid loss.
//--------------------------------------------------------------------------------------------------
//void Energy::Dehydration(double time)
//{
//  Warning("Consume Dehydration is currently disabled.");
//  return;
//	//Fluid loss is approximately 2600 mL/day. Shirreffs SM. Markers of hydration status. J Sports Med Phys Fitness 2000;40:80-4. 
//	//After the first day without fluid intake, the renal loss decreases by 1000 mL leading to a total fluid loss of 1600 mL/day after day one.
//	double fluidLossRate_mL_Per_day = 2600;
//	double fluidLossRate_mL_Per_day2 = 1600;
//	double coarseTimeStep_days = time;
//	double fluidLoss_mL = 0.0;
//
//	if (coarseTimeStep_days<0.0)
//		/// \error Cannot specify elapsed time less than zero in the consume meal action.
//		Error("Cannot specify a negative time since last meal. Time since last meal is now set to zero.");
//
//	if (coarseTimeStep_days > 1.0)
//		fluidLoss_mL = fluidLossRate_mL_Per_day + fluidLossRate_mL_Per_day2*(coarseTimeStep_days - 1.0);
//	else
//		fluidLoss_mL = fluidLossRate_mL_Per_day*coarseTimeStep_days;
//
//	SEScalarVolume fluidDehydrated;
//	fluidDehydrated.SetValue(-fluidLoss_mL, VolumeUnit::mL);
//
//	// How do we take fluid from the body m_data.GetCircuits().DistributeVolume(fluidDehydrated);
//
//	//Patient weight decrease due to fluid mass lost
//	double patientMassLost_kg = m_data.GetConfiguration().GetWaterDensity(MassPerVolumeUnit::kg_Per_mL)*fluidLoss_mL;
//	m_Patient->GetWeight().IncrementValue(-patientMassLost_kg, MassUnit::kg);
//}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Applies an imposed starvation time to the current nutrient stores
///
/// \details
/// Starvation uses a coarse time-step approach with an assumed time-step of one day. This coarse time-step
/// is used to to manually decrement all of the nutrient stores which represents the loss due to metabolic consumption.
/// For time durations greater than one day, there is a decrease in the basal metabolic rate due to mass loss from increased
/// starvation times.
//--------------------------------------------------------------------------------------------------
// Starvation currently disabled
//void Energy::Starvation(double time)
//{
//	if (time < 1.0) //Less than one day without food. Decrement according to consumption/production methodology
//	{
//		//CalculateNutrientConsumptionAndProduction(Convert(time, TimeUnit::day, TimeUnit::s)); //TODO:: Add in clearances if they are needed
//		//BalanceSubstances();
//	}
//	else
//	{
//		unsigned int numTimeSteps = (int)time;
//		double deltaT_days = 1.0;
//		for (unsigned int iStep = 0; iStep < numTimeSteps; iStep++)
//		{
//			CalculateNutrientConsumptionAndProduction(Convert(deltaT_days, TimeUnit::day, TimeUnit::s));
//			CalculateBasalMetabolicRate();
//			GetTotalMetabolicRate().SetValue(m_PatientBMR_kcal_Per_day, PowerUnit::kcal_Per_day); //Metabolic rate decrease due to reduction in body mass
//			//BalanceSubstances();
//		}
//	}
//}


void Energy::AtSteadyState()
{
	if (m_data.GetState() == EngineState::AtInitialStableState)
	{
		//TODO: Get consume meal files
    // Consume meal condition currently disabled
		if (m_data.GetConditions().HasConsumeMeal())
		{
      Warning("Consume Meal Condition is currently disabled.");
		//	double elapsedTime = m_data.GetConditions().GetConsumeMeal()->GetMeal().GetElapsedTime(TimeUnit::day);
		//	Dehydration(elapsedTime);
		//	m_AdjustCO2 = false;// Don't mess with CO2 (in the blood) during this Starvation call
		//	Starvation(elapsedTime);
		//	m_AdjustCO2 = true;
		//	//Systemic clearance of calcium occurs over the elapsed time period
		//	double patientWeight_kg = m_Patient->GetWeight(MassUnit::kg);
		//	double renalVolumeCleared = m_calcium->GetClearance().GetRenalClearance(VolumePerTimeMassUnit::mL_Per_s_kg)*patientWeight_kg*elapsedTime;
		//	double systemicVolumeCleared = m_calcium->GetClearance().GetSystemicClearance(VolumePerTimeMassUnit::mL_Per_s_kg)*patientWeight_kg*elapsedTime - renalVolumeCleared;
		//	SEScalarVolume* integratedVolume = new SEScalarVolume();
		//	integratedVolume->SetValue(systemicVolumeCleared, VolumeUnit::mL);
		//	m_data.GetCircuits().BalanceBloodMassByClearedVolume(*m_calcium, *integratedVolume);
		}
	}
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Prepares the energy system for the circuit solver
///
/// \details
/// The PreProcess function performs function in preparation to update the thermal circuit.
/// It also processes energy-related actions (exercise) and computes a sweat rate.
//--------------------------------------------------------------------------------------------------
void Energy::PreProcess()
{
	CalculateMetabolicHeatGeneration();
	CalculateSweatRate();
	UpdateHeatResistance();
  Exercise();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// The exercise function updates the patient's metabolic rate if the exercise action is present and
/// computes energy-store recovery post exercise.
///
/// \details
/// The exercise function adds to the body's basal metabolic rate a value that is specified by 
/// the exercise action. The metabolic rate set-point is specified by the action but limited by 
/// the amount of energy available. The energy limit is computed, and the actual metabolic rate is
/// ramped to the limited set-point. The body's actual work rate as a fraction of the total possible
/// work rate and the body's actual work rate as a fraction of the action-requested work rate are set
/// in this method.
//--------------------------------------------------------------------------------------------------
void Energy::Exercise()
{	
	// If there is no exercise action and no fatigue, then return.
	// Any exercise action will reduce energy stores and induce some fatigue, setting the fatigue event.
	// While the fatigue event is active, the exercise method will execute even if there is no exercise action.
	// This allows the energy stores to refill post-exercise.
	if (!m_PatientActions->HasExercise() && !m_Patient->IsEventActive(CDM::enumPatientEvent::Fatigue))
		return;

	double exerciseIntensity = 0.0;
	double currentMetabolicRate_kcal_Per_day = GetTotalMetabolicRate().GetValue(PowerUnit::kcal_Per_day);
  double basalMetabolicRate_kcal_Per_day = m_Patient->GetBasalMetabolicRate().GetValue(PowerUnit::kcal_Per_day);

	// Only try to get intensity if the exercise action is active. Otherwise, we are just refilling energy buckets post-exercise.
	if (m_PatientActions->HasExercise())
  {
		if (m_PatientActions->GetExercise()->HasIntensity())
		{
			exerciseIntensity = m_PatientActions->GetExercise()->GetIntensity().GetValue();
		}		
		else
		{
			Warning("Exercise call with no severity. Action ignored.");
		}
	}
	
	if (exerciseIntensity > 0.5)
	{
		// Need to limit the exercise intensity due to current (as of 2/03/2016) engine limitations (CO2 solution error).
		Warning("Exercise intensity currently limited to 0.5. Setting intensity to 0.5.");
	}
	
	
	// Fatigue calculations
	double energyIncrement_J = 0.0;

	// There are four individual energy stores that comprise the fatigue system.
	// The usable and peak power stores together can be thought of as similar to the phosphagen energy system.
	// The medium power store can be thought of as similar to the glycogen-lactic acid energy system.
	// The endurance energy store can be thought of as similar to the aerobic energy system.
	// The energy store full levels and the maximum work rate are currently hard-coded, but will be a function of physical conditioning,
	// body composition, and starvation levels. See Future Work section in BioGears Energy documentation.
	double usableEnergyStoreFull_J = 2600.0;
	double peakPowerEnergyStoreFull_J = 4200.0;
	double mediumPowerEnergyStoreFull_J = 35000.0;
	double enduranceEnergyStoreFull_J = 400000.0;
	double maxWorkRate_W = 1200.0;
	// The maximum depletion rates will also scale with the body composition, physical conditioning, and
	// fed/starvation state of the BioGears body.
	double maxEnduranceOutRate_W = 240.0;
	double maxPeakOutRate_W = 621.0;
	double maxMediumOutRate_W = 420.0;

	// The following rates dictate the fill rate of the endurance energy store.
	// These rates are currently hard-coded, but will be tied to the concentration
	// of nutrient substances in the blood in the future. See Future Work section in BioGears Energy documentation.
	double glucoseConstant_W = 10.0;
	double lactateConstant_W = 10.0;
	double tristearinConstant_W = 10.0;

	double workRateDesired_W = exerciseIntensity*maxWorkRate_W;
	double workRate_W = 0.0;
	double normalizedUsableEnergyDeficit = (usableEnergyStoreFull_J - m_UsableEnergyStore_J) / usableEnergyStoreFull_J;
	double normalizedPeakEnergyDeficit = (peakPowerEnergyStoreFull_J - m_PeakPowerEnergyStore_J) / peakPowerEnergyStoreFull_J;
	double normalizedMediumEnergyDeficit = (mediumPowerEnergyStoreFull_J - m_MediumPowerEnergyStore_J) / mediumPowerEnergyStoreFull_J;
	double normalizedEnduranceEnergyDeficit = (enduranceEnergyStoreFull_J - m_EnduranceEnergyStore_J) / enduranceEnergyStoreFull_J;

	// Modify the rates based on the usable energy deficit.
	// This effectively creates piecewise rate functions so that all of the storage
	// outflow rates are zero until the immediate energy storage tank is almost depleted.
	double energyRateModifier = 6.0*normalizedUsableEnergyDeficit - 5.0;
	LLIM(energyRateModifier, 0.0);

	double glucoseRate_W = glucoseConstant_W*(normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit + normalizedEnduranceEnergyDeficit) / 4.0;
	double lactateRate_W = lactateConstant_W*(normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit + normalizedEnduranceEnergyDeficit) / 4.0;
	double tristearinRate_W = tristearinConstant_W*(normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit + normalizedEnduranceEnergyDeficit) / 4.0;

	energyIncrement_J = (glucoseRate_W + lactateRate_W + tristearinRate_W)*m_dT_s;

	if (m_EnduranceEnergyStore_J + energyIncrement_J > enduranceEnergyStoreFull_J){
		glucoseRate_W = lactateRate_W = tristearinRate_W = 0.0;
		energyIncrement_J = 0.0;
	}

	double enduranceOutRate_W = (normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit)*maxEnduranceOutRate_W;
	BLIM(enduranceOutRate_W, 0.0, maxEnduranceOutRate_W);
	if (m_EnduranceEnergyStore_J + energyIncrement_J < enduranceOutRate_W*m_dT_s){
		enduranceOutRate_W = (m_EnduranceEnergyStore_J + energyIncrement_J) / m_dT_s;
	}
	m_EnduranceEnergyStore_J += energyIncrement_J - enduranceOutRate_W*m_dT_s;

	// The aerobic store has multiple outflow paths. One path represents purely aerobic energy
	// usage, and the other paths represent the aerobic replenishment of the anaerobic stores. The following
	// algorithm computes the percent energy flow at the path junctions. There are two junctions.
	// First junction:
	double splitFraction = 1.0;
	if (normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit > 0.00000001){
		splitFraction = normalizedUsableEnergyDeficit / (normalizedUsableEnergyDeficit + (normalizedMediumEnergyDeficit + normalizedPeakEnergyDeficit) / 2.0);
	}
	double enduranceToUsableRate_W = splitFraction*enduranceOutRate_W;
	double enduranceToFillRate_W = (1.0 - splitFraction)*enduranceOutRate_W;
	// Second junction:
	splitFraction = 1.0;
	if (normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit > 0.00000001){
		if (normalizedPeakEnergyDeficit > 0.00000001)
		splitFraction = normalizedPeakEnergyDeficit / (normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit);
	}
	double peakFillRate_W = splitFraction*enduranceToFillRate_W;
	double mediumFillRate_W = (1.0 - splitFraction)*enduranceToFillRate_W;

	// Advance peak store state
	energyIncrement_J = peakFillRate_W*m_dT_s;
	double peakOutRate_W = normalizedUsableEnergyDeficit*maxPeakOutRate_W*energyRateModifier;
	BLIM(peakOutRate_W, 0.0, maxPeakOutRate_W);
	if (m_PeakPowerEnergyStore_J + energyIncrement_J < peakOutRate_W*m_dT_s){
		peakOutRate_W = (m_PeakPowerEnergyStore_J + energyIncrement_J) / m_dT_s;
	}
	m_PeakPowerEnergyStore_J += energyIncrement_J - peakOutRate_W*m_dT_s;
	// Advance medium store state
	energyIncrement_J = mediumFillRate_W*m_dT_s;
	double mediumOutRate_W = normalizedUsableEnergyDeficit*maxMediumOutRate_W*energyRateModifier;
	BLIM(mediumOutRate_W, 0.0, maxMediumOutRate_W);
	if (m_MediumPowerEnergyStore_J + energyIncrement_J < mediumOutRate_W*m_dT_s){
		mediumOutRate_W = (m_MediumPowerEnergyStore_J + energyIncrement_J) / m_dT_s;
	}
	m_MediumPowerEnergyStore_J += energyIncrement_J - mediumOutRate_W*m_dT_s;

	// Advance usable bucket state
	energyIncrement_J = (enduranceToUsableRate_W + mediumOutRate_W + peakOutRate_W)*m_dT_s;
	if (m_UsableEnergyStore_J + energyIncrement_J < workRateDesired_W*m_dT_s)
	{
		workRate_W = (m_UsableEnergyStore_J + energyIncrement_J) / m_dT_s;
	}
	else 
	{
		workRate_W = workRateDesired_W;
	}

	m_UsableEnergyStore_J += energyIncrement_J - workRate_W*m_dT_s;

  GetTotalWorkRateLevel().SetValue(workRate_W / maxWorkRate_W);
	double fatigue = (normalizedEnduranceEnergyDeficit + normalizedMediumEnergyDeficit + normalizedPeakEnergyDeficit + normalizedUsableEnergyDeficit) / 4.0;
  /// \event Patient: Fatigue - Energy stores are sub-maximal.
	if (fatigue > 0.0){
		m_Patient->SetEvent(CDM::enumPatientEvent::Fatigue, true, m_data.GetSimulationTime());
	}
	else {
    m_Patient->SetEvent(CDM::enumPatientEvent::Fatigue, false, m_data.GetSimulationTime());
	}
	GetFatigueLevel().SetValue(fatigue);

	double kcal_Per_day_Per_Watt = 20.6362855;

	if (exerciseIntensity > 0.0)
  {
    double  achievedExerciseFraction = GetTotalWorkRateLevel().GetValue() / exerciseIntensity;
    /// \todo Log an event or info message that available energy is insufficient to meet demand.
    //if (achievedExerciseFraction < 1.0)
    //  Info("Available energy insufficient to meet requested work rate.");
    GetAchievedExerciseLevel().SetValue(achievedExerciseFraction);
  }		
	else
		GetAchievedExerciseLevel().Clear();
	// End fatigue calculations
	
	double TotalMetabolicRateSetPoint_kcal_Per_day = basalMetabolicRate_kcal_Per_day + workRate_W*kcal_Per_day_Per_Watt;

	// The MetabolicRateGain is used to ramp the metabolic rate to the value specified by the user's exercise intensity.
	double MetabolicRateGain = 1.0;
	// We only let exercise control the metabolic rate if it is active, otherwise the heat generation method is in charge of the metabolic rate.
	if (m_PatientActions->HasExercise())
  {
		double TotalMetabolicRateProduced_kcal_Per_day = currentMetabolicRate_kcal_Per_day + MetabolicRateGain*(TotalMetabolicRateSetPoint_kcal_Per_day - currentMetabolicRate_kcal_Per_day)*m_dT_s;
		GetTotalMetabolicRate().SetValue(TotalMetabolicRateProduced_kcal_Per_day, PowerUnit::kcal_Per_day);
	}
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Processes the temperature circuit and calculates extravascular transport
///
/// \details
/// The Process function solves the temperature circuit for all path heat transfer rates and nodal temperatures.
/// Additionally, the process function also initiates the flow transport of the metabolic substance quantities
/// from the vascular to extravascular space. After this is complete, the vital signs of the patient are updated
/// with regards to the new state.
//--------------------------------------------------------------------------------------------------
void Energy::Process()
{
	m_circuitCalculator.Process(*m_TemperatureCircuit, m_dT_s);
	CalculateVitalSigns();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Updates the parameters for the temperature circuit
///
/// \details
/// Updates the resulting heat transfer rates, temperatures and nodal heat values; effectively moving
/// the next values to the current ones before time can be advanced.
//--------------------------------------------------------------------------------------------------
void Energy::PostProcess()
{
  m_circuitCalculator.PostProcess(*m_TemperatureCircuit);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Checks the patient's thermal and metabolic state
///
/// \details
/// The core and skin temperatures are recorded in this function. In addition, the current metabolic
/// state of the patient may trigger the following events: hypothermia, hyperthermia, dehydration, fasciculation
/// or fatigue. These events are only triggered if the current state falls within the criteria of the specific event
//--------------------------------------------------------------------------------------------------
void Energy::CalculateVitalSigns()
{
	double coreTemperature_degC = m_coreNode->GetTemperature(TemperatureUnit::C);
	double skinTemperature_degC = m_skinNode->GetTemperature(TemperatureUnit::C);
	GetCoreTemperature().SetValue(coreTemperature_degC, TemperatureUnit::C);
	GetSkinTemperature().SetValue(skinTemperature_degC, TemperatureUnit::C);
  std::stringstream ss; 

	//Hypothermia check
  double coreTempIrreversible_degC = 20.0;   /// \cite Stocks2004HumanPhysiologicalResponseCold
	if (coreTemperature_degC < 35.0) /// \cite mallet2001hypothermia
	{
		/// \event Patient: Core temperature has fallen below 35 degrees Celsius. Patient is hypothermic.
    m_Patient->SetEvent(CDM::enumPatientEvent::Hypothermia, true, m_data.GetSimulationTime());

    /// \irreversible State: Core temperature has fallen below 20 degrees Celsius.
    if (coreTemperature_degC < coreTempIrreversible_degC)
    {
      ss << "Core temperature is " << coreTemperature_degC << ". This is below 20 degrees C, patient is experiencing extreme hypothermia and is in an irreversible state.";
      Warning(ss);
      m_Patient->SetEvent(CDM::enumPatientEvent::IrreversibleState, true, m_data.GetSimulationTime());
    }

	}
	else if (m_Patient->IsEventActive(CDM::enumPatientEvent::Hypothermia) && coreTemperature_degC>35.2)
	{
    m_Patient->SetEvent(CDM::enumPatientEvent::Hypothermia, false, m_data.GetSimulationTime());
	}
	//Hyperthermia check
	if (coreTemperature_degC > 38.8) /// \cite mallet2001hypothermia
	{
		/// \event Patient: Core temperature has exceeded 38.3 degrees Celsius. Patient is hyperthermic.
    m_Patient->SetEvent(CDM::enumPatientEvent::Hyperthermia, true, m_data.GetSimulationTime());
	}
	else if (m_Patient->IsEventActive(CDM::enumPatientEvent::Hyperthermia) && coreTemperature_degC < 38.0)
	{
    m_Patient->SetEvent(CDM::enumPatientEvent::Hyperthermia, false, m_data.GetSimulationTime());
	}

  /// \todo Move to blood chemistry
  // Although it is called metabolic acidosis/alkalosis, the detection and event setting is actually a part of the @ref BloodChemistrySystem
  // The terms "metabolic" and "respiratory" refer to the origin of the acid-base disturbance
  // The hydrogen ion concentration is a property of the blood
  // The events related to blood concentrations should be detected and set in blood chemistry.
  double highPh = 8.5;
  double lowPh = 6.5;   // \cite Edge2006AcidosisConscious
	m_BloodpH.Sample(m_data.GetBloodChemistry().GetBloodPH().GetValue());
	m_BicarbonateMolarity_mmol_Per_L.Sample(m_AortaHCO3->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
	//Only check these at the end of a cardiac cycle and reset at start of cardiac cycle 
	if (m_Patient->IsEventActive(CDM::enumPatientEvent::StartOfCardiacCycle))
	{
		double bloodPH = m_BloodpH.Value();
		double bloodBicarbonate_mmol_Per_L = m_BicarbonateMolarity_mmol_Per_L.Value();

    if (m_data.GetState() > EngineState::InitialStabilization)
    {// Don't throw events if we are initializing
      if (bloodPH < 7.35 && bloodBicarbonate_mmol_Per_L < 22.0)
        /// \event The patient is in a state of metabolic acidosis
        m_Patient->SetEvent(CDM::enumPatientEvent::MetabolicAcidosis, true, m_data.GetSimulationTime());

      /// \irreversible State: arterial blood pH has dropped below 6.5.
      if (bloodPH < lowPh)
      {
        ss << " Arterial blood PH is " << bloodPH << ". This is below 6.5, patient is experiencing extreme metabolic acidosis and is in an irreversible state.";
        Warning(ss);
        m_Patient->SetEvent(CDM::enumPatientEvent::IrreversibleState, true, m_data.GetSimulationTime());
      }
      else if (bloodPH > 7.38 && bloodBicarbonate_mmol_Per_L > 23.0)
        /// \event The patient has exited the state state of metabolic acidosis
        m_Patient->SetEvent(CDM::enumPatientEvent::MetabolicAcidosis, false, m_data.GetSimulationTime());

      if (bloodPH > 7.45 && bloodBicarbonate_mmol_Per_L > 26.0)
        /// \event The patient is in a state of metabolic alkalosis
        m_Patient->SetEvent(CDM::enumPatientEvent::MetabolicAlkalosis, true, m_data.GetSimulationTime());

      /// \irreversible State: arterial blood pH has increased above 8.5.
      if (bloodPH > highPh)
      {
        ss << " Arterial blood PH is " << bloodPH << ". This is above 8.5, patient is experiencing extreme metabolic Alkalosis and is in an irreversible state.";
        Warning(ss);
        m_Patient->SetEvent(CDM::enumPatientEvent::IrreversibleState, true, m_data.GetSimulationTime());
      }

      else if (bloodPH < 7.42 && bloodBicarbonate_mmol_Per_L < 25.0)
        /// \event The patient has exited the state of metabolic alkalosis
        m_Patient->SetEvent(CDM::enumPatientEvent::MetabolicAlkalosis, false, m_data.GetSimulationTime());
    }
    // Reset the running averages. Why do we need running averages here? Does the aorta pH fluctuate that much? 
		m_BloodpH.Reset();
		m_BicarbonateMolarity_mmol_Per_L.Reset();
	}
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the metabolic rate, depending on the current state of the patient
///
/// \details
/// The metabolic rate is dictated by the current state of the core temperature. This function includes
/// states of increased metabolic rate due to shivering or severe hyperthermia. Additionally, an extreme
/// drop in core temperature leads to decreasing metabolic rate. If the core temperature does not meet any
/// of the criteria for increases/decreases, the metabolic rate will be calculated as the basal metabolic rate.
//--------------------------------------------------------------------------------------------------
void Energy::CalculateMetabolicHeatGeneration()
{
	const BioGearsConfiguration& config = m_data.GetConfiguration();
	double coreTemperature_degC = m_coreNode->GetTemperature(TemperatureUnit::C);
	double coreTemperatureLow_degC = config.GetCoreTemperatureLow(TemperatureUnit::C);
	double coreTemperatureLowDelta_degC = config.GetDeltaCoreTemperatureLow(TemperatureUnit::C);
	double coreTemperatureHigh_degC = 40.0; //TODO: This should use the config property why doesn't it?
	double totalMetabolicRateNew_Kcal_Per_day = 0.0;
	double totalMetabolicRateNew_W = 0.0;
	//The summit metabolism is the maximum amount of power the human body can generate due to shivering/response to the cold.
	double summitMetabolism_W = 21.0*pow(m_Patient->GetWeight(MassUnit::kg), 0.75); /// \cite herman2008physics
	double currentMetabolicRate_kcal_Per_day = GetTotalMetabolicRate().GetValue(PowerUnit::kcal_Per_day);
  double basalMetabolicRate_kcal_Per_day = m_Patient->GetBasalMetabolicRate().GetValue(PowerUnit::kcal_Per_day);

	if (coreTemperature_degC < 34.0) //Hypothermic state inducing metabolic depression (decline of metabolic heat generation)
	{
		totalMetabolicRateNew_W = summitMetabolism_W*pow(0.94, 34.0 - coreTemperature_degC); //The metabolic heat generated will drop by 6% for every degree below 34 C
		GetTotalMetabolicRate().SetValue(totalMetabolicRateNew_W, PowerUnit::W);		 /// \cite mallet2002hypothermia
	}
	else if (coreTemperature_degC >= 34.0 && coreTemperature_degC < 36.8) //Patient is increasing heat generation via shivering. This caps out at the summit metabolism
	{
		//Add an event for shivering
		double basalMetabolicRate_W = m_Patient->GetBasalMetabolicRate(PowerUnit::W);
		totalMetabolicRateNew_W = basalMetabolicRate_W + (summitMetabolism_W - basalMetabolicRate_W)*(coreTemperatureLow_degC - coreTemperature_degC) / coreTemperatureLowDelta_degC;
		totalMetabolicRateNew_W = MIN(totalMetabolicRateNew_W, summitMetabolism_W); //Bounded at the summit metabolism so further heat generation doesn't continue for continue drops below 34 C.
		GetTotalMetabolicRate().SetValue(totalMetabolicRateNew_W, PowerUnit::W);
	}
	else if (coreTemperature_degC >= 36.8 && coreTemperature_degC < 42.5 && !m_PatientActions->HasExercise()) //Basic Metabolic rate
	{
		double TotalMetabolicRateSetPoint_kcal_Per_day = basalMetabolicRate_kcal_Per_day;
		double MetabolicRateGain = 0.0001;	//Used to ramp the metabolic rate from its current value to the basal value if the patient meet's the basal criteria
		double TotalMetabolicRateProduced_kcal_Per_day = currentMetabolicRate_kcal_Per_day + MetabolicRateGain*(TotalMetabolicRateSetPoint_kcal_Per_day - currentMetabolicRate_kcal_Per_day);
		GetTotalMetabolicRate().SetValue(TotalMetabolicRateProduced_kcal_Per_day, PowerUnit::kcal_Per_day);
	}
	else if (coreTemperature_degC>40.0  && !m_PatientActions->HasExercise()) //Core temperature greater than 40.0. If not exercising, then the hyperthermia leads to increased metabolism
	{
		totalMetabolicRateNew_Kcal_Per_day = basalMetabolicRate_kcal_Per_day*pow(1.11, coreTemperature_degC - coreTemperatureHigh_degC);  //The metabolic heat generated will increase by 11% for every degree above 40.0 C
		GetTotalMetabolicRate().SetValue(totalMetabolicRateNew_Kcal_Per_day, PowerUnit::kcal_Per_day);								  /// \cite pate2001thermal								  
	}

	m_temperatureGroundToCorePath->GetNextHeatSource().SetValue(GetTotalMetabolicRate(PowerUnit::W), PowerUnit::W);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the sweat rate if the core temperature is too high
///
/// \details
/// The sweat rate is calculated from a core temperature control function. The mass lost due to sweating is accounted for
/// and a flow source from the skin extravascular to ground path is updated to ensure fluid loss
//--------------------------------------------------------------------------------------------------
void Energy::CalculateSweatRate()
{
	const BioGearsConfiguration& config = m_data.GetConfiguration();
	double coreTemperature_degC = m_coreNode->GetTemperature(TemperatureUnit::C);
	double coreTemperatureHigh_degC = config.GetCoreTemperatureHigh(TemperatureUnit::C);
	double sweatHeatTranferCoefficient_W_Per_K = config.GetSweatHeatTransfer(HeatConductanceUnit::W_Per_K);
	double vaporizationEnergy_J_Per_kg = config.GetVaporizationEnergy(EnergyPerMassUnit::J_Per_kg);

	//Calculate sweat rate (in kg/s) from core temperature feedback.
	//The sweat rate heat transfer is determined from a control equation that attempts to keep the core temperature in line
	/// \cite herman2008physics
	double sweatRate_kg_Per_s = (0.25*sweatHeatTranferCoefficient_W_Per_K / vaporizationEnergy_J_Per_kg)*(coreTemperature_degC - coreTemperatureHigh_degC);
	sweatRate_kg_Per_s = MAX(sweatRate_kg_Per_s, 0.0);


	//Account for mass lost by substracting from the current patient mass
	double massLost_kg = sweatRate_kg_Per_s*m_dT_s;
	m_Patient->GetWeight().IncrementValue(-massLost_kg, MassUnit::kg);

	GetSweatRate().SetValue(sweatRate_kg_Per_s, MassPerTimeUnit::kg_Per_s);

	double sweatDensity_kg_Per_m3 = config.GetWaterDensity(MassPerVolumeUnit::kg_Per_m3); /// \todo Convert to sweat density once specific gravity calculation is in

	//Set the flow source on the extravascular circuit to begin removing the fluid that is excreted
  double sweatRate_mL_Per_s = sweatRate_kg_Per_s / sweatDensity_kg_Per_m3 * 1.e6;
  //m_data.GetDataTrack().Probe("sweatRate_mL_Per_s", sweatRate_mL_Per_s);
  //m_data.GetDataTrack().Probe("sweatRate_mg_Per_min", sweatRate_kg_Per_s*60.*1000.*1000.);
  m_skinExtravascularToSweatingGroundPath->GetNextFlowSource().SetValue(sweatRate_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Updates the variable core to skin heat transfer resistance
///
/// \details
/// The variable core to skin heat transfer resistance is updated here according to the  inverse 
/// of the skin blood flow.
//--------------------------------------------------------------------------------------------------
void Energy::UpdateHeatResistance()
{
	double skinBloodFlow_m3_Per_s = m_data.GetCardiovascular().GetMeanSkinFlow().GetValue(VolumePerTimeUnit::m3_Per_s);
	double bloodDensity_kg_Per_m3 = m_data.GetBloodChemistry().GetBloodDensity().GetValue(MassPerVolumeUnit::kg_Per_m3);
	double bloodSpecificHeat_J_Per_K_kg = m_data.GetBloodChemistry().GetBloodSpecificHeat().GetValue(HeatCapacitancePerMassUnit::J_Per_K_kg);

	double alphaScale = 0.5; //Scaling factor for convective heat transfer from core to skin

	//The heat transfer resistance from the core to the skin is inversely proportional to the skin blood flow.
	//When skin blood flow increases, then heat transfer resistance decreases leading to more heat transfer from core to skin. 
	//The opposite occurs for skin blood flow decrease.
	double coreToSkinResistance_K_Per_W = 1.0 / (alphaScale*bloodDensity_kg_Per_m3*bloodSpecificHeat_J_Per_K_kg*skinBloodFlow_m3_Per_s);
	
	coreToSkinResistance_K_Per_W = BLIM(coreToSkinResistance_K_Per_W, 0.0001, 20.0);
	m_coreToSkinPath->GetNextResistance().SetValue(coreToSkinResistance_K_Per_W, HeatResistanceUnit::K_Per_W);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the basal metabolic rate from the Harris-Benedict formula
///
///
/// \details
/// The Harris-Benedict formula uses the patient height, weight age and sex to determine
/// the basal metabolic requirements.
//--------------------------------------------------------------------------------------------------
void Energy::CalculateBasalMetabolicRate()
{
	SEPatient& patient = m_data.GetPatient();
	double PatientMass_kg = patient.GetWeight(MassUnit::kg);
	double PatientAge_yr = patient.GetAge(TimeUnit::yr);
	double PatientHeight_cm = patient.GetHeight(LengthUnit::cm);

	//The basal metabolic rate is determined from the Harris-Benedict formula, with differences dependent on sex, age, height and mass
	/// \cite roza1984metabolic
	double patientBMR_kcal_Per_day = 0.0;
	if (patient.GetSex() == CDM::enumSex::Male)
		patientBMR_kcal_Per_day = 88.632 + 13.397*PatientMass_kg + 4.799*PatientHeight_cm - 5.677*PatientAge_yr;
	else
		patientBMR_kcal_Per_day = 447.593 + 9.247*PatientMass_kg + 3.098*PatientHeight_cm - 4.330*PatientAge_yr;
  // Systems do their math with MetabolicRate in Watts, so let's make these consistent
	patient.GetBasalMetabolicRate().SetValue(patientBMR_kcal_Per_day, PowerUnit::kcal_Per_day);

	std::stringstream ss;
	ss << "Conditions applied homeostasis: " << "Patient basal metabolic rate = " << patientBMR_kcal_Per_day << " kcal/day";
	Info(ss);
}
