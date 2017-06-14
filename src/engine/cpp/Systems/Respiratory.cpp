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
#include "Respiratory.h"
#include "bind/RunningAverageData.hxx"

#include "circuit/fluid/SEFluidCircuit.h"
// Properties
#include "properties/SEScalar0To1.h"
#include "properties/SEScalarNeg1To1.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarFlowResistance.h"
#include "properties/SEScalarInversePressure.h"
#include "properties/SEScalarFlowCompliance.h"
#include "properties/SEScalarLength.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarInverseVolume.h"
#include "properties/SEScalarPower.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarArea.h"
#include "properties/SEFunctionVolumeVsTime.h"
//Circuits
#include "circuit/fluid/SEFluidCircuit.h"
#include "circuit/fluid/SEFluidCircuitNode.h"
#include "circuit/fluid/SEFluidCircuitPath.h"
#include "compartment/fluid/SEGasCompartmentGraph.h"
#include "compartment/fluid/SELiquidCompartmentGraph.h"
#include "substance/SESubstanceFraction.h"
// Actions
#include "patient/actions/SEConsciousRespiration.h"
#include "patient/actions/SEBreathHold.h"
#include "patient/actions/SEForcedExhale.h"
#include "patient/actions/SEForcedInhale.h"
#include "patient/actions/SEUseInhaler.h"
// Conditions 
#include "patient/conditions/SEChronicObstructivePulmonaryDisease.h"
#include "patient/conditions/SELobarPneumonia.h"
#include "patient/conditions/SEImpairedAlveolarExchange.h"
// Assessments
#include "patient/assessments/SEPulmonaryFunctionTest.h"


#ifdef _MSC_VER 
#pragma warning( disable : 4305 4244 )  // Disable some warning messages
#endif

//Flag for setting things constant to test
//Should be commented out, unless debugging/tuning
// #define TUNING

Respiratory::Respiratory(BioGears& bg) : SERespiratorySystem(bg.GetLogger()), m_data(bg),
m_Calculator(FlowComplianceUnit::L_Per_cmH2O, VolumePerTimeUnit::L_Per_s, FlowInertanceUnit::cmH2O_s2_Per_L, PressureUnit::cmH2O, VolumeUnit::L, FlowResistanceUnit::cmH2O_s_Per_L, GetLogger()),
m_GasTransporter(VolumePerTimeUnit::L_Per_s, VolumeUnit::L, VolumeUnit::L, NoUnit::unitless, GetLogger()),
m_AerosolTransporter(VolumePerTimeUnit::mL_Per_s, VolumeUnit::mL, MassUnit::ug, MassPerVolumeUnit::ug_Per_mL, GetLogger())
{
  Clear();
}

Respiratory::~Respiratory()
{
  Clear();
}

void Respiratory::Clear()
{
  SERespiratorySystem::Clear();
  m_Patient = nullptr;
  m_PatientActions = nullptr;

  m_Environment = nullptr;
  m_AerosolMouth = nullptr;
  m_AerosolCarina = nullptr;
  m_AerosolLeftDeadSpace = nullptr;
  m_AerosolLeftAlveoli = nullptr;
  m_AerosolRightDeadSpace = nullptr;
  m_AerosolRightAlveoli = nullptr;
  m_Lungs = nullptr;
  m_LeftLungExtravascular = nullptr;
  m_RightLungExtravascular = nullptr;
  m_Carina = nullptr;
  m_AortaO2 = nullptr;
  m_AortaCO2 = nullptr;
  m_MechanicalVentilatorConnection = nullptr;

  m_RespiratoryCircuit = nullptr;

  m_LeftAlveoli = nullptr;
  m_LeftDeadSpace = nullptr;
  m_LeftPleural = nullptr;
  m_RespiratoryMuscle = nullptr;
  m_RightAlveoli = nullptr;
  m_RightDeadSpace = nullptr;
  m_RightPleural = nullptr;

  m_CarinaToLeftAnatomicDeadSpace = nullptr;
  m_CarinaToRightAnatomicDeadSpace = nullptr;
  m_LeftAnatomicDeadSpaceToLeftAlveoli = nullptr;
  m_RightAnatomicDeadSpaceToRightAlveoli = nullptr;
  m_RightPleuralToRespiratoryMuscle = nullptr;
  m_LeftPleuralToRespiratoryMuscle = nullptr;
  m_DriverPressurePath = nullptr;
  m_LeftDriverPressurePath = nullptr;
  m_MouthToCarina = nullptr;
  m_MouthToStomach = nullptr;
  m_EnvironmentToLeftChestLeak = nullptr;
  m_EnvironmentToRightChestLeak = nullptr;
  m_LeftAlveoliLeakToLeftPleural = nullptr;
  m_RightAlveoliLeakToRightPleural = nullptr;
  m_LeftPleuralToEnvironment = nullptr;
  m_RightPleuralToEnvironment = nullptr;
  m_RightAlveoliToRightPleuralConnection = nullptr;
  m_LeftAlveoliToLeftPleuralConnection = nullptr;
  m_RightPulmonaryCapillary = nullptr;
  m_LeftPulmonaryCapillary = nullptr;
  m_ConnectionToMouth = nullptr;
  m_GroundToConnection = nullptr;

  m_BloodPHRunningAverage.Reset();
  m_ArterialO2RunningAverage_mmHg.Reset();
  m_ArterialCO2RunningAverage_mmHg.Reset();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Initializes system properties to valid homeostatic values.
//--------------------------------------------------------------------------------------------------
void Respiratory::Initialize()
{
  BioGearsSystem::Initialize();

  //Vital signs
  m_bNotBreathing = false;
  m_TopBreathTotalVolume_L = 0.0;
  m_TopBreathAlveoliVolume_L = 0.0;
  m_TopBreathDeadSpaceVolume_L = 0.0;
  m_TopBreathPleuralPressure_cmH2O = 0.0;
  m_LastCardiacCycleBloodPH = 7.4;


  //Driver
  //Basically a Y-shift for the driver
  m_DefaultDrivePressure_cmH2O = VolumeToDriverPressure(m_Patient->GetFunctionalResidualCapacity(VolumeUnit::L));
  m_MaxDriverPressure_cmH2O = VolumeToDriverPressure(m_Patient->GetTotalLungCapacity(VolumeUnit::L));
  m_ElapsedBreathingCycleTime_min = 0.0;
  m_BreathTimeExhale_min = 0.0;
  m_BreathingCycle = false;
  m_BreathingCycleTime_s = 0.0;
  m_VentilationFrequency_Per_min = m_Patient->GetRespirationRateBaseline(FrequencyUnit::Per_min);
  m_DriverPressure_cmH2O = m_DefaultDrivePressure_cmH2O;
  m_DriverPressureMin_cmH2O = m_DefaultDrivePressure_cmH2O;
  m_VentilationToTidalVolumeSlope = 30.0;
  //The peak driver pressure is the pressure above the default pressure
  m_PeakRespiratoryDrivePressure_cmH2O = VolumeToDriverPressure(m_Patient->GetTotalLungCapacity(VolumeUnit::L) -
    m_Patient->GetInspiratoryReserveVolume(VolumeUnit::L)) -
    m_DefaultDrivePressure_cmH2O;
  m_ArterialO2PartialPressure_mmHg = m_AortaO2->GetPartialPressure(PressureUnit::mmHg);
  m_ArterialCO2PartialPressure_mmHg = m_AortaCO2->GetPartialPressure(PressureUnit::mmHg);
  m_PreviousTargetAlveolarVentilation_L_Per_min = m_Patient->GetTidalVolumeBaseline(VolumeUnit::L) * m_VentilationFrequency_Per_min;
  m_AverageLocalTissueBronchodilationEffects = 0.0;

  //Asthma
  m_IEscaleFactor = 1.0;

  // Conscious Respiration
  m_ConsciousRespirationPeriod_s = 0.0;
  m_ConsciousRespirationRemainingPeriod_s = 0.0;
  m_ExpiratoryReserveVolumeFraction = -1.0;
  m_InspiratoryCapacityFraction = -1.0;
  m_ConsciousStartPressure_cmH2O = 0.0;
  m_ConsciousEndPressure_cmH2O = 0.0;
  m_ConsciousBreathing = false;

  //Patient data
  m_InitialExpiratoryReserveVolume_L = m_Patient->GetExpiratoryReserveVolume(VolumeUnit::L);
  m_InitialInspiratoryCapacity_L = m_Patient->GetInspiratoryCapacity(VolumeUnit::L);
  m_InitialFunctionalResidualCapacity_L = m_Patient->GetFunctionalResidualCapacity(VolumeUnit::L);
  m_InitialResidualVolume_L = m_Patient->GetResidualVolume(VolumeUnit::L);
  m_InstantaneousFunctionalResidualCapacity_L = m_Patient->GetFunctionalResidualCapacity(VolumeUnit::L);
  m_PreviousTotalLungVolume_L = m_Patient->GetFunctionalResidualCapacity(VolumeUnit::L);

  //System data
  double TidalVolume_L = m_Patient->GetTidalVolumeBaseline(VolumeUnit::L);
  double RespirationRate_Per_min = m_Patient->GetRespirationRateBaseline(FrequencyUnit::Per_min);
  GetTidalVolume().SetValue(TidalVolume_L, VolumeUnit::L);
  GetRespirationRate().SetValue(RespirationRate_Per_min, FrequencyUnit::Per_min);
  GetCarricoIndex().SetValue(452.0, PressureUnit::mmHg);

  double DeadSpace_L = m_LeftDeadSpace->GetVolumeBaseline(VolumeUnit::L) + m_RightDeadSpace->GetVolumeBaseline(VolumeUnit::L);
  GetTotalAlveolarVentilation().SetValue(RespirationRate_Per_min * (TidalVolume_L - DeadSpace_L), VolumePerTimeUnit::L_Per_min);
  GetTotalPulmonaryVentilation().SetValue(RespirationRate_Per_min * TidalVolume_L, VolumePerTimeUnit::L_Per_min);
  GetTotalDeadSpaceVentilation().SetValue(0.15 * RespirationRate_Per_min, VolumePerTimeUnit::L_Per_min);
  GetPulmonaryCompliance().SetValue(1.6, FlowComplianceUnit::L_Per_cmH2O);
  GetSpecificVentilation().SetValue(0.21);
  GetEndTidalCarbonDioxideFraction().SetValue(0.0827);
  GetEndTidalCarbonDioxidePressure().SetValue(0.0, PressureUnit::mmHg);

  //Get the fluid mechanics to a good starting point
  TuneCircuit();
}

bool Respiratory::Load(const CDM::BioGearsRespiratorySystemData& in)
{
  if (!SERespiratorySystem::Load(in))
    return false;

  m_InitialExpiratoryReserveVolume_L = in.InitialExpiratoryReserveVolume_L();
  m_InitialFunctionalResidualCapacity_L = in.InitialFunctionalResidualCapacity_L();
  m_InitialInspiratoryCapacity_L = in.InitialInspiratoryCapacity_L();
  m_InitialResidualVolume_L = in.InitialResidualVolume_L();

  m_bNotBreathing = in.NotBreathing();
  m_TopBreathTotalVolume_L = in.TopBreathTotalVolume_L();
  m_TopBreathAlveoliVolume_L = in.TopBreathAlveoliVolume_L();
  m_TopBreathDeadSpaceVolume_L = in.TopBreathDeadSpaceVolume_L();
  m_TopBreathPleuralPressure_cmH2O = in.TopBreathPleuralPressure_cmH2O();
  m_LastCardiacCycleBloodPH = in.LastCardiacCycleBloodPH();
  m_PreviousTotalLungVolume_L = in.PreviousTotalLungVolume_L();
  m_BloodPHRunningAverage.Load(in.BloodPHRunningAverage());

  m_BreathingCycle = in.BreathingCycle();
  m_ArterialO2PartialPressure_mmHg = in.ArterialO2PartialPressure_mmHg();
  m_ArterialCO2PartialPressure_mmHg = in.ArterialCO2PartialPressure_mmHg();
  m_BreathingCycleTime_s = in.BreathingCycleTime_s();
  m_BreathTimeExhale_min = in.BreathTimeExhale_min();
  m_DefaultDrivePressure_cmH2O = in.DefaultDrivePressure_cmH2O();
  m_DriverPressure_cmH2O = in.DriverPressure_cmH2O();
  m_DriverPressureMin_cmH2O = in.DriverPressureMin_cmH2O();
  m_ElapsedBreathingCycleTime_min = in.ElapsedBreathingCycleTime_min();
  m_IEscaleFactor = in.IEscaleFactor();
  m_InstantaneousFunctionalResidualCapacity_L = in.InstantaneousFunctionalResidualCapacity_L();
  m_MaxDriverPressure_cmH2O = in.MaxDriverPressure_cmH2O();
  m_PeakRespiratoryDrivePressure_cmH2O = in.PeakRespiratoryDrivePressure_cmH2O();
  m_PreviousTargetAlveolarVentilation_L_Per_min = in.PreviousTargetAlveolarVentilation_L_Per_min();
  m_VentilationFrequency_Per_min = in.VentilationFrequency_Per_min();
  m_VentilationToTidalVolumeSlope = in.VentilationToTidalVolumeSlope();
  m_ArterialO2RunningAverage_mmHg.Load(in.ArterialO2RunningAverage_mmHg());
  m_ArterialCO2RunningAverage_mmHg.Load(in.ArterialCO2RunningAverage_mmHg());

  m_ConsciousBreathing = in.ConsciousBreathing();
  m_ConsciousRespirationPeriod_s = in.ConsciousRespirationPeriod_s();
  m_ConsciousRespirationRemainingPeriod_s = in.ConsciousRespirationRemainingPeriod_s();
  m_ExpiratoryReserveVolumeFraction = in.ExpiratoryReserveVolumeFraction();
  m_InspiratoryCapacityFraction = in.InspiratoryCapacityFraction();
  m_ConsciousStartPressure_cmH2O = in.ConsciousStartPressure_cmH2O();
  m_ConsciousEndPressure_cmH2O = in.ConsciousEndPressure_cmH2O();

  BioGearsSystem::LoadState();
  return true;
}
CDM::BioGearsRespiratorySystemData* Respiratory::Unload() const
{
  CDM::BioGearsRespiratorySystemData* data = new CDM::BioGearsRespiratorySystemData();
  Unload(*data);
  return data;
}
void Respiratory::Unload(CDM::BioGearsRespiratorySystemData& data) const
{
  SERespiratorySystem::Unload(data);

  data.InitialExpiratoryReserveVolume_L(m_InitialExpiratoryReserveVolume_L);
  data.InitialFunctionalResidualCapacity_L(m_InitialFunctionalResidualCapacity_L);
  data.InitialInspiratoryCapacity_L(m_InitialInspiratoryCapacity_L);
  data.InitialResidualVolume_L(m_InitialResidualVolume_L);

  data.NotBreathing(m_bNotBreathing);
  data.TopBreathTotalVolume_L(m_TopBreathTotalVolume_L);
  data.TopBreathAlveoliVolume_L(m_TopBreathAlveoliVolume_L);
  data.TopBreathDeadSpaceVolume_L(m_TopBreathDeadSpaceVolume_L);
  data.TopBreathPleuralPressure_cmH2O(m_TopBreathPleuralPressure_cmH2O);
  data.LastCardiacCycleBloodPH(m_LastCardiacCycleBloodPH);
  data.PreviousTotalLungVolume_L(m_PreviousTotalLungVolume_L);
  data.BloodPHRunningAverage(std::unique_ptr<CDM::RunningAverageData>(m_BloodPHRunningAverage.Unload()));

  data.BreathingCycle(m_BreathingCycle);
  data.ArterialO2PartialPressure_mmHg(m_ArterialO2PartialPressure_mmHg);
  data.ArterialCO2PartialPressure_mmHg(m_ArterialCO2PartialPressure_mmHg);
  data.BreathingCycleTime_s(m_BreathingCycleTime_s);
  data.BreathTimeExhale_min(m_BreathTimeExhale_min);
  data.DefaultDrivePressure_cmH2O(m_DefaultDrivePressure_cmH2O);
  data.DriverPressure_cmH2O(m_DriverPressure_cmH2O);
  data.DriverPressureMin_cmH2O(m_DriverPressureMin_cmH2O);
  data.ElapsedBreathingCycleTime_min(m_ElapsedBreathingCycleTime_min);
  data.IEscaleFactor(m_IEscaleFactor);
  data.InstantaneousFunctionalResidualCapacity_L(m_InstantaneousFunctionalResidualCapacity_L);
  data.MaxDriverPressure_cmH2O(m_MaxDriverPressure_cmH2O);
  data.PeakRespiratoryDrivePressure_cmH2O(m_PeakRespiratoryDrivePressure_cmH2O);
  data.PreviousTargetAlveolarVentilation_L_Per_min(m_PreviousTargetAlveolarVentilation_L_Per_min);
  data.VentilationFrequency_Per_min(m_VentilationFrequency_Per_min);
  data.VentilationToTidalVolumeSlope(m_VentilationToTidalVolumeSlope);
  data.ArterialO2RunningAverage_mmHg(std::unique_ptr<CDM::RunningAverageData>(m_ArterialO2RunningAverage_mmHg.Unload()));
  data.ArterialCO2RunningAverage_mmHg(std::unique_ptr<CDM::RunningAverageData>(m_ArterialCO2RunningAverage_mmHg.Unload()));

  data.ConsciousBreathing(m_ConsciousBreathing);
  data.ConsciousRespirationPeriod_s(m_ConsciousRespirationPeriod_s);
  data.ConsciousRespirationRemainingPeriod_s(m_ConsciousRespirationRemainingPeriod_s);
  data.ExpiratoryReserveVolumeFraction(m_ExpiratoryReserveVolumeFraction);
  data.InspiratoryCapacityFraction(m_InspiratoryCapacityFraction);
  data.ConsciousStartPressure_cmH2O(m_ConsciousStartPressure_cmH2O);
  data.ConsciousEndPressure_cmH2O(m_ConsciousEndPressure_cmH2O);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Initializes parameters for Respiratory Class
///
///  \details
///	 Initializes member variables and system level values on the common data model.
//--------------------------------------------------------------------------------------------------
void Respiratory::SetUp()
{
  //Time Step
  m_dt_s = m_data.GetTimeStep().GetValue(TimeUnit::s);
  m_dt_min = m_data.GetTimeStep().GetValue(TimeUnit::min);
  //Patient
  m_Patient = &m_data.GetPatient();
  m_PatientActions = &m_data.GetActions().GetPatientActions();
  //Configuration parameters
  m_dDefaultOpenResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetDefaultOpenFlowResistance(FlowResistanceUnit::cmH2O_s_Per_L);
  m_dDefaultClosedResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetDefaultClosedFlowResistance(FlowResistanceUnit::cmH2O_s_Per_L);
  m_dRespOpenResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetRespiratoryOpenResistance(FlowResistanceUnit::cmH2O_s_Per_L);
  m_dRespClosedResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetRespiratoryClosedResistance(FlowResistanceUnit::cmH2O_s_Per_L);
  m_PeripheralControlGainConstant = m_data.GetConfiguration().GetPeripheralVentilatoryControllerGain();
  m_CentralControlGainConstant = m_data.GetConfiguration().GetCentralVentilatoryControllerGain();
  m_VentilationTidalVolumeIntercept = m_data.GetConfiguration().GetVentilationTidalVolumeIntercept(VolumeUnit::L);
  m_VentilatoryOcclusionPressure_cmH2O = m_data.GetConfiguration().GetVentilatoryOcclusionPressure(PressureUnit::cmH2O); //This increases the absolute max driver pressure
  m_PleuralComplianceSensitivity_Per_L = m_data.GetConfiguration().GetPleuralComplianceSensitivity(InverseVolumeUnit::Inverse_L);
  //Compartments
  m_Environment = m_data.GetCompartments().GetGasCompartment(BGE::EnvironmentCompartment::Ambient);
  m_AerosolMouth = m_data.GetCompartments().GetLiquidCompartment(BGE::PulmonaryCompartment::Mouth);
  m_AerosolCarina = m_data.GetCompartments().GetLiquidCompartment(BGE::PulmonaryCompartment::Carina);
  m_AerosolLeftDeadSpace = m_data.GetCompartments().GetLiquidCompartment(BGE::PulmonaryCompartment::LeftDeadSpace);
  m_AerosolLeftAlveoli = m_data.GetCompartments().GetLiquidCompartment(BGE::PulmonaryCompartment::LeftAlveoli);
  m_AerosolRightDeadSpace = m_data.GetCompartments().GetLiquidCompartment(BGE::PulmonaryCompartment::RightDeadSpace);
  m_AerosolRightAlveoli = m_data.GetCompartments().GetLiquidCompartment(BGE::PulmonaryCompartment::RightAlveoli);
  m_Lungs = m_data.GetCompartments().GetGasCompartment(BGE::PulmonaryCompartment::Lungs);
  m_LeftLungExtravascular = m_data.GetCompartments().GetLiquidCompartment(BGE::ExtravascularCompartment::LeftLungIntracellular);
  m_RightLungExtravascular = m_data.GetCompartments().GetLiquidCompartment(BGE::ExtravascularCompartment::RightLungIntracellular);
  m_Carina = m_data.GetCompartments().GetGasCompartment(BGE::PulmonaryCompartment::Carina);
  m_CarinaO2 = m_Carina->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
  SELiquidCompartment* Aorta = m_data.GetCompartments().GetLiquidCompartment(BGE::VascularCompartment::Aorta);
  m_AortaO2 = Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
  m_AortaCO2 = Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());
  m_LeftAlveoliO2 = m_data.GetCompartments().GetGasCompartment(BGE::PulmonaryCompartment::LeftAlveoli)->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
  m_RightAlveoliO2 = m_data.GetCompartments().GetGasCompartment(BGE::PulmonaryCompartment::RightAlveoli)->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
  m_MechanicalVentilatorConnection = m_data.GetCompartments().GetGasCompartment(BGE::MechanicalVentilatorCompartment::Connection);
  // Compartments we will process aerosol effects on
  m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(BGE::PulmonaryCompartment::Carina));
  m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(BGE::PulmonaryCompartment::LeftAlveoli));
  m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(BGE::PulmonaryCompartment::LeftDeadSpace));
  m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(BGE::PulmonaryCompartment::RightAlveoli));
  m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(BGE::PulmonaryCompartment::RightDeadSpace));
  //Circuits
  m_RespiratoryCircuit = &m_data.GetCircuits().GetRespiratoryCircuit();
  //Nodes
  m_LeftAlveoli = m_RespiratoryCircuit->GetNode(BGE::RespiratoryNode::LeftAlveoli);
  m_LeftDeadSpace = m_RespiratoryCircuit->GetNode(BGE::RespiratoryNode::LeftAnatomicDeadSpace);
  m_LeftPleural = m_RespiratoryCircuit->GetNode(BGE::RespiratoryNode::LeftPleural);
  m_RespiratoryMuscle = m_RespiratoryCircuit->GetNode(BGE::RespiratoryNode::RespiratoryMuscle);
  m_RightAlveoli = m_RespiratoryCircuit->GetNode(BGE::RespiratoryNode::RightAlveoli);
  m_RightDeadSpace = m_RespiratoryCircuit->GetNode(BGE::RespiratoryNode::RightAnatomicDeadSpace);
  m_RightPleural = m_RespiratoryCircuit->GetNode(BGE::RespiratoryNode::RightPleural);
  m_Ambient = m_RespiratoryCircuit->GetNode(BGE::EnvironmentNode::Ambient);
  m_Stomach = m_RespiratoryCircuit->GetNode(BGE::RespiratoryNode::Stomach);
  //Paths
  m_CarinaToLeftAnatomicDeadSpace = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::CarinaToLeftAnatomicDeadSpace);
  m_CarinaToRightAnatomicDeadSpace = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::CarinaToRightAnatomicDeadSpace);
  m_LeftAnatomicDeadSpaceToLeftAlveoli = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::LeftAnatomicDeadSpaceToLeftAlveoli);
  m_RightAnatomicDeadSpaceToRightAlveoli = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::RightAnatomicDeadSpaceToRightAlveoli);
  m_RightPleuralToRespiratoryMuscle = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::RightPleuralToRespiratoryMuscle);
  m_LeftPleuralToRespiratoryMuscle = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::LeftPleuralToRespiratoryMuscle);
  m_DriverPressurePath = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::EnvironmentToRespiratoryMuscle);
  m_MouthToCarina = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::MouthToCarina);
  m_MouthToStomach = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::MouthToStomach);
  m_EnvironmentToLeftChestLeak = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::EnvironmentToLeftChestLeak);
  m_EnvironmentToRightChestLeak = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::EnvironmentToRightChestLeak);
  m_LeftAlveoliLeakToLeftPleural = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::LeftAlveoliLeakToLeftPleural);
  m_RightAlveoliLeakToRightPleural = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::RightAlveoliLeakToRightPleural);
  m_LeftPleuralToEnvironment = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::LeftPleuralToEnvironment);
  m_RightPleuralToEnvironment = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::RightPleuralToEnvironment);
  m_RightAlveoliToRightPleuralConnection = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::RightAlveoliToRightPleuralConnection);
  m_LeftAlveoliToLeftPleuralConnection = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::LeftAlveoliToLeftPleuralConnection);
  m_RightAnatomicDeadSpaceToRightPleuralConnection = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::RightAnatomicDeadSpaceToRightPleuralConnection);
  m_LeftAnatomicDeadSpaceToLeftPleuralConnection = m_RespiratoryCircuit->GetPath(BGE::RespiratoryPath::LeftAnatomicDeadSpaceToLeftPleuralConnection);
  m_ConnectionToMouth = m_data.GetCircuits().GetRespiratoryAndMechanicalVentilatorCircuit().GetPath(BGE::MechanicalVentilatorPath::ConnectionToMouth);
  m_GroundToConnection = m_data.GetCircuits().GetRespiratoryAndMechanicalVentilatorCircuit().GetPath(BGE::MechanicalVentilatorPath::GroundToConnection);

  /// \todo figure out how to modify these resistances without getting the cv circuit - maybe add a parameter, like baroreceptors does
  m_RightPulmonaryCapillary = m_data.GetCircuits().GetCardiovascularCircuit().GetPath(BGE::CardiovascularPath::RightPulmonaryCapillariesToRightPulmonaryVeins);
  m_LeftPulmonaryCapillary = m_data.GetCircuits().GetCardiovascularCircuit().GetPath(BGE::CardiovascularPath::LeftPulmonaryCapillariesToLeftPulmonaryVeins);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Respiratory system at steady state
///
/// \details
/// Initializes respiratory conditions if any are present.
///  <UL>
///   <LI>COPD</LI>
///   <LI>Lobar Pneumonia</LI>
///   <LI>ImpairedAlveolarExchange</LI>
///  </UL>
///
//--------------------------------------------------------------------------------------------------
void Respiratory::AtSteadyState()
{
  double respirationRate_Per_min = GetRespirationRate(FrequencyUnit::Per_min);
  double tidalVolume_L = GetTidalVolume(VolumeUnit::L);
  double totalLungCapacity_L = m_Patient->GetTotalLungCapacity(VolumeUnit::L);
  double residualVolume_L = m_Patient->GetResidualVolume(VolumeUnit::L);
  double vitalCapacity_L = totalLungCapacity_L - residualVolume_L;
  double expiratoryReserveVolume_L = m_InstantaneousFunctionalResidualCapacity_L - residualVolume_L;
  double inspiratoryReserveVolume_L = totalLungCapacity_L - m_InstantaneousFunctionalResidualCapacity_L - tidalVolume_L;
  double inspiratoryCapacity_L = totalLungCapacity_L - m_InstantaneousFunctionalResidualCapacity_L;
  m_Patient->GetRespirationRateBaseline().SetValue(respirationRate_Per_min, FrequencyUnit::Per_min);
  m_Patient->GetTidalVolumeBaseline().SetValue(tidalVolume_L, VolumeUnit::L);
  m_Patient->GetFunctionalResidualCapacity().SetValue(m_InstantaneousFunctionalResidualCapacity_L, VolumeUnit::L);
  m_Patient->GetVitalCapacity().SetValue(vitalCapacity_L, VolumeUnit::L);
  m_Patient->GetExpiratoryReserveVolume().SetValue(expiratoryReserveVolume_L, VolumeUnit::L);
  m_Patient->GetInspiratoryReserveVolume().SetValue(inspiratoryReserveVolume_L, VolumeUnit::L);
  m_Patient->GetInspiratoryCapacity().SetValue(inspiratoryCapacity_L, VolumeUnit::L);

  std::string typeString = "Initial Stabilization Homeostasis: ";
  if (m_data.GetState() == EngineState::AtSecondaryStableState)
    typeString = "Secondary Stabilization Homeostasis: ";

  std::stringstream ss;
  ss << typeString << "Patient respiration rate = " << respirationRate_Per_min << " bpm.";
  Info(ss);
  ss << typeString << "Patient tidal volume = " << tidalVolume_L << " L.";
  Info(ss);
  ss << typeString << "Patient functional residual capacity = " << m_InstantaneousFunctionalResidualCapacity_L << " L.";
  Info(ss);
  ss << typeString << "Patient vital capacity = " << vitalCapacity_L << " L.";
  Info(ss);
  ss << typeString << "Patient expiratory reserve volume = " << expiratoryReserveVolume_L << " L.";
  Info(ss);
  ss << typeString << "Patient inspiratory reserve volume = " << inspiratoryReserveVolume_L << " L.";
  Info(ss);
  ss << typeString << "Patient inspiratory capacity = " << inspiratoryCapacity_L << " L.";
  Info(ss);

  if (m_data.GetState() == EngineState::AtInitialStableState)
  {// At Resting State, apply conditions if we have them
    COPD();    
    LobarPneumonia();
	//These conditions stack effects
	//If combined, it will be a fraction of the already effected alveolar surface area
	ImpairedAlveolarExchange();
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Respiratory system preprocess function
///
/// \details
/// Calculates the muscle pressure source that drives the respiratory system.
/// Updates airway resistance to account for changes arising from factors 
/// like drugs and respiratory insults and interventions. 
//// Updates the chest wall variable compliance. Handles all respiratory
/// insults and actions.
//--------------------------------------------------------------------------------------------------
void Respiratory::PreProcess()
{
  UpdatePleuralCompliance();
  ProcessAerosolSubstances();
  AirwayObstruction();
  UpdateObstructiveResistance();
  BronchoConstriction();
  BronchoDilation();
  Intubation();
  Pneumothorax();  
  ConsciousRespiration();

  MechanicalVentilation();

  RespiratoryDriver();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Respiratory system process function
///
/// \details
/// Ensures the selection of the respiratory system with or without the anesthesia machine. 
///	Handles the integration of the anesthesia machine to the respiratory system when the anesthesia machine is turned on.
/// The integration of the anesthesia machine to the respiratory system is handled at run time by constructing a combined circuit of 
/// the respiratory and anesthesia machine.  
/// Handles lung volume changes during alveolar gas transfer. 
/// Calculates physiological parameters such as respiration rate, tidal volume and others that belonging to the respiratory system.
//--------------------------------------------------------------------------------------------------
void Respiratory::Process()
{
  // Respiration circuit changes based on if Anesthesia Machine is on or off
  // When dynamic intercircuit connections work, we can stash off the respiration circuit in a member variable
  SEFluidCircuit& RespirationCircuit = m_data.GetCircuits().GetActiveRespiratoryCircuit();
  // Calc the circuits
  m_Calculator.Process(RespirationCircuit, m_dt_s);
  //ModifyPleuralVolume();
  SEGasCompartmentGraph& RespirationGraph = m_data.GetCompartments().GetActiveRespiratoryGraph();
  SELiquidCompartmentGraph& AerosolGraph = m_data.GetCompartments().GetActiveAerosolGraph();
  // Transport substances
  m_GasTransporter.Transport(RespirationGraph, m_dt_s);
  if(m_AerosolMouth->HasSubstanceQuantities())
    m_AerosolTransporter.Transport(AerosolGraph, m_dt_s);
  //Update system data
  CalculateVitalSigns();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Respiratory system postprocess function
///
/// \details
/// Updates the current values of the gas volume fraction and gas volumes for the nodes in the respiratory circuit 
/// or the nodes in the combined (respiratory + anesthesia machine) circuit when the anesthesia machine is turned on.
//--------------------------------------------------------------------------------------------------
void Respiratory::PostProcess()
{
  // Respiration circuit changes based on if Anesthesia Machine is on or off
  // When dynamic intercircuit connections work, we can stash off the respiration circuit in a member variable
  SEFluidCircuit& RespirationCircuit = m_data.GetCircuits().GetActiveRespiratoryCircuit();  
  m_Calculator.PostProcess(RespirationCircuit);
  
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Chest wall compliance modifier
///
/// \details
/// Adjusts the chest wall compliance based on the pleural volume. The variable compliance allows some level of 
/// tolerance to mechanical stresses that may arise from events like airway obstruction. Currently, a  
/// linear relation with an upper bound has been used. 
//--------------------------------------------------------------------------------------------------
void Respiratory::UpdatePleuralCompliance()
{
	double dRightPleuralCompliance = m_RightPleuralToRespiratoryMuscle->GetNextCompliance().GetValue(FlowComplianceUnit::L_Per_cmH2O);
	double dLeftPleuralCompliance = m_LeftPleuralToRespiratoryMuscle->GetNextCompliance().GetValue(FlowComplianceUnit::L_Per_cmH2O);
	double dRightPleuralVolumeBaseline = m_RightPleural->GetVolumeBaseline().GetValue(VolumeUnit::L);
	double dLeftPleuralVolumeBaseline = m_LeftPleural->GetVolumeBaseline().GetValue(VolumeUnit::L);
	double dRightPleuralVolume = m_RightPleural->GetNextVolume().GetValue(VolumeUnit::L);
	double dLeftPleuralVolume = m_LeftPleural->GetNextVolume().GetValue(VolumeUnit::L);

	dRightPleuralCompliance = (dRightPleuralVolume - dRightPleuralVolumeBaseline) * m_PleuralComplianceSensitivity_Per_L * dRightPleuralCompliance + dRightPleuralCompliance;
	dLeftPleuralCompliance = (dLeftPleuralVolume - dLeftPleuralVolumeBaseline) * m_PleuralComplianceSensitivity_Per_L * dLeftPleuralCompliance + dLeftPleuralCompliance;

	dRightPleuralCompliance = LIMIT(dRightPleuralCompliance, 1e-6, 0.05);
	dLeftPleuralCompliance = LIMIT(dLeftPleuralCompliance, 1e-6, 0.05);

  m_RightPleuralToRespiratoryMuscle->GetNextCompliance().SetValue(dRightPleuralCompliance, FlowComplianceUnit::L_Per_cmH2O);
  m_LeftPleuralToRespiratoryMuscle->GetNextCompliance().SetValue(dLeftPleuralCompliance, FlowComplianceUnit::L_Per_cmH2O);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Compute deposited mass, update localized PD effects 
///
/// \details
/// For each aerosol get the SIDE coefficient to determine deposited mass in each respiratory compartment. 
/// Adjust the resistances between compartments as a function of deposited mass to reach validated data.  
/// Liquid and solid aerosols are handeled here. 
//--------------------------------------------------------------------------------------------------
void Respiratory::ProcessAerosolSubstances()
{
	m_AverageLocalTissueBronchodilationEffects = 0.0; //No effect

	size_t numAerosols = m_AerosolMouth->GetSubstanceQuantities().size();
  if (numAerosols == 0)
    return;

  double inflammationCoefficient;

  // For this time step
  double mouthDepositied_ug = 0;
  double carinaDepositied_ug = 0;
  double leftDeadSpaceDepositied_ug = 0;
  double leftAlveoliDepositied_ug = 0;
  double rightDeadSpaceDepositied_ug = 0;
  double rightAlveoliDepositied_ug = 0;

  // Total amount deposited (including this time step)
  double mouthTotalDepositied_ug = 0;
  double carinaTotalDepositied_ug = 0;
  double leftDeadSpaceTotalDepositied_ug = 0;
  double leftAlveoliTotalDepositied_ug = 0;
  double rightDeadSpaceTotalDepositied_ug = 0;
  double rightAlveoliTotalDepositied_ug = 0;

  // Resistance Modifier Sum
  double mouthResistanceModifier=1;
  double carinaResistanceModifier=1;
  double leftDeadSpaceResistanceModifier=1;
  double leftAlveoliResistanceModifier=1;
  double rightDeadSpaceResistanceModifier=1;
  double rightAlveoliResistanceModifier=1;

  // Currently, There is no way to clear out depositied particulate out of the respiratory systems.
  // Maybe we could have it to cough or some other excretion related method... 
  
  SELiquidSubstanceQuantity* subQ;
  SELiquidSubstanceQuantity* tSubQ;
  const SizeIndependentDepositionEfficencyCoefficient* SIDECoeff;
  double combinedRightBronchodilationEffects = 0.0;
  double combinedLeftBronchodilationEffects = 0.0;
  for (size_t i=0; i<numAerosols; i++)
  {
	  //initialize substance
	  subQ = m_AerosolMouth->GetSubstanceQuantities()[i];
	  //Adjust inflammation coefficient (scaled down):
	  inflammationCoefficient = subQ->GetSubstance().GetAerosolization().GetInflammationCoefficient().GetValue();// Once for each subQ
	  inflammationCoefficient *= 0.01;
    //Mouth
    SIDECoeff = &m_data.GetSubstances().GetSizeIndependentDepositionEfficencyCoefficient(subQ->GetSubstance());// Once for each subQ
    mouthDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolMouth->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_dt_s*SIDECoeff->GetMouth();
    if (mouthDepositied_ug > subQ->GetMass(MassUnit::ug))
    {
      mouthDepositied_ug = subQ->GetMass(MassUnit::ug);
      subQ->GetMass().SetValue(0, MassUnit::ug);
    }
    else
      subQ->GetMass().IncrementValue(-mouthDepositied_ug, MassUnit::ug); 
    subQ->Balance(BalanceLiquidBy::Mass);
    mouthTotalDepositied_ug = subQ->GetMassDeposited().IncrementValue(mouthDepositied_ug, MassUnit::ug);
    mouthResistanceModifier += mouthTotalDepositied_ug*inflammationCoefficient;
    //Carina
    subQ = m_AerosolCarina->GetSubstanceQuantities()[i];
    carinaDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolCarina->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_dt_s*SIDECoeff->GetCarina();
    if (carinaDepositied_ug > subQ->GetMass(MassUnit::ug))
    {
      carinaDepositied_ug = subQ->GetMass(MassUnit::ug);
      subQ->GetMass().SetValue(0, MassUnit::ug);
    }
    else
      subQ->GetMass().IncrementValue(-carinaDepositied_ug, MassUnit::ug);
    subQ->Balance(BalanceLiquidBy::Mass);
    carinaTotalDepositied_ug = subQ->GetMassDeposited().IncrementValue(carinaDepositied_ug, MassUnit::ug);
    carinaResistanceModifier += carinaTotalDepositied_ug*inflammationCoefficient;
    //Left DeadSpace
    subQ = m_AerosolLeftDeadSpace->GetSubstanceQuantities()[i];
    leftDeadSpaceDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolLeftDeadSpace->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_dt_s*SIDECoeff->GetDeadSpace();
    if (leftDeadSpaceDepositied_ug > subQ->GetMass(MassUnit::ug))
    {
      leftDeadSpaceDepositied_ug = subQ->GetMass(MassUnit::ug);
      subQ->GetMass().SetValue(0, MassUnit::ug);
    }
    else
      subQ->GetMass().IncrementValue(-leftDeadSpaceDepositied_ug, MassUnit::ug);
    subQ->Balance(BalanceLiquidBy::Mass);
    leftDeadSpaceTotalDepositied_ug = subQ->GetMassDeposited().IncrementValue(leftDeadSpaceDepositied_ug, MassUnit::ug);
    leftDeadSpaceResistanceModifier += leftDeadSpaceTotalDepositied_ug*inflammationCoefficient;
    //Left Alveoli
    subQ = m_AerosolLeftAlveoli->GetSubstanceQuantities()[i];
    leftAlveoliDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolLeftAlveoli->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_dt_s*SIDECoeff->GetAlveoli();
    if (leftAlveoliDepositied_ug > subQ->GetMass(MassUnit::ug))
    {
      leftAlveoliDepositied_ug = subQ->GetMass(MassUnit::ug);
      subQ->GetMass().SetValue(0, MassUnit::ug);
    }
    else
      subQ->GetMass().IncrementValue(-leftAlveoliDepositied_ug, MassUnit::ug);
    subQ->Balance(BalanceLiquidBy::Mass);
    leftAlveoliTotalDepositied_ug = subQ->GetMassDeposited().IncrementValue(leftAlveoliDepositied_ug, MassUnit::ug);
    leftAlveoliResistanceModifier += leftAlveoliTotalDepositied_ug*inflammationCoefficient;
    //Right DeadSpace
    subQ = m_AerosolRightDeadSpace->GetSubstanceQuantities()[i];
    rightDeadSpaceDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolRightDeadSpace->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_dt_s*SIDECoeff->GetDeadSpace();
    if (rightDeadSpaceDepositied_ug > subQ->GetMass(MassUnit::ug))
    {
      rightDeadSpaceDepositied_ug = subQ->GetMass(MassUnit::ug);
      subQ->GetMass().SetValue(0, MassUnit::ug);
    }
    else
      subQ->GetMass().IncrementValue(-rightDeadSpaceDepositied_ug, MassUnit::ug);
    subQ->Balance(BalanceLiquidBy::Mass);
    rightDeadSpaceTotalDepositied_ug = subQ->GetMassDeposited().IncrementValue(rightDeadSpaceDepositied_ug, MassUnit::ug);
    rightDeadSpaceResistanceModifier += rightDeadSpaceTotalDepositied_ug*inflammationCoefficient;
    //Right Alveoli
    subQ = m_AerosolRightAlveoli->GetSubstanceQuantities()[i];
    rightAlveoliDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolRightAlveoli->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_dt_s*SIDECoeff->GetAlveoli();
    if (rightAlveoliDepositied_ug > subQ->GetMass(MassUnit::ug))
    {
      rightAlveoliDepositied_ug = subQ->GetMass(MassUnit::ug);
      subQ->GetMass().SetValue(0, MassUnit::ug);
    }
    else
      subQ->GetMass().IncrementValue(-rightAlveoliDepositied_ug, MassUnit::ug);
    subQ->Balance(BalanceLiquidBy::Mass);
    rightAlveoliTotalDepositied_ug = subQ->GetMassDeposited().IncrementValue(rightAlveoliDepositied_ug, MassUnit::ug);
    rightAlveoliResistanceModifier += rightAlveoliTotalDepositied_ug*inflammationCoefficient;
    
	  // Apply the BronchileModifier dilation effect
	  // This is all just tuned to Albuterol - it'll work for other substances, and can be tuned using the other parameters (especially BronchioleModifier)
    if (subQ->GetSubstance().GetState() == CDM::enumSubstanceState::Liquid)
    {
      // Sum the Bronchiole Effects
		  // Must be positive
      double bronchioleModifier = subQ->GetSubstance().GetAerosolization().GetBronchioleModifier().GetValue();

      // Diffuse into Tissues
      // We only process mass deposited on the lungs (dead space and alveoli)
      // We do not currently do anything with the mass in the mouth and carina
      // Could possibly let it go into the stomach somehow... 
      tSubQ = m_LeftLungExtravascular->GetSubstanceQuantity(subQ->GetSubstance());
      tSubQ->GetMass().IncrementValue(leftDeadSpaceDepositied_ug + leftAlveoliDepositied_ug, MassUnit::ug); tSubQ->Balance(BalanceLiquidBy::Mass);	  
	    combinedLeftBronchodilationEffects += bronchioleModifier * tSubQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL);

      tSubQ = m_RightLungExtravascular->GetSubstanceQuantity(subQ->GetSubstance());
      tSubQ->GetMass().IncrementValue(rightDeadSpaceDepositied_ug + rightAlveoliDepositied_ug, MassUnit::ug); tSubQ->Balance(BalanceLiquidBy::Mass);
	    combinedRightBronchodilationEffects += bronchioleModifier * tSubQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL);
    }
  }

  //We're going to make the bronchodilation effects be based off of Albuterol.
  //Experimentally, 1mg of Albuterol given via a spacer device on an endotracheal tube caused a pulmonary resistance change of ~20% @cite mancebo1991effects.
  //The bronchi are ~30% of the total pulmonary resistance, so we'll make a dilation effect really strong with a normal dose.
  //This was tuned using a standard inhaler dose of 90ug
  m_AverageLocalTissueBronchodilationEffects = (combinedLeftBronchodilationEffects + combinedRightBronchodilationEffects) / 2.0;
  combinedLeftBronchodilationEffects = exp(-32894.0 * combinedLeftBronchodilationEffects);
  combinedRightBronchodilationEffects = exp(-32894.0 * combinedRightBronchodilationEffects);  

    // Change resistances due to deposition
  double dTracheaResistance = m_MouthToCarina->GetNextResistance(FlowResistanceUnit::cmH2O_s_Per_L)*carinaResistanceModifier;
  double dLeftAlveoliResistance = m_LeftAnatomicDeadSpaceToLeftAlveoli->GetNextResistance(FlowResistanceUnit::cmH2O_s_Per_L)*leftAlveoliResistanceModifier;
  double dRightAlveoliResistance = m_RightAnatomicDeadSpaceToRightAlveoli->GetNextResistance(FlowResistanceUnit::cmH2O_s_Per_L)*rightAlveoliResistanceModifier;

  double dLeftBronchiResistance = m_CarinaToLeftAnatomicDeadSpace->GetNextResistance(FlowResistanceUnit::cmH2O_s_Per_L);
  double dRightBronchiResistance = m_CarinaToRightAnatomicDeadSpace->GetNextResistance(FlowResistanceUnit::cmH2O_s_Per_L);
  dLeftBronchiResistance *= leftDeadSpaceResistanceModifier * combinedLeftBronchodilationEffects;
  dRightBronchiResistance *= rightDeadSpaceResistanceModifier * combinedRightBronchodilationEffects;
  dLeftBronchiResistance = LIMIT(dLeftBronchiResistance, m_dRespClosedResistance_cmH2O_s_Per_L, m_dRespOpenResistance_cmH2O_s_Per_L);
  dRightBronchiResistance = LIMIT(dRightBronchiResistance, m_dRespClosedResistance_cmH2O_s_Per_L, m_dRespOpenResistance_cmH2O_s_Per_L);

  m_MouthToCarina->GetNextResistance().SetValue(dTracheaResistance, FlowResistanceUnit::cmH2O_s_Per_L);
  m_CarinaToLeftAnatomicDeadSpace->GetNextResistance().SetValue(dLeftBronchiResistance, FlowResistanceUnit::cmH2O_s_Per_L);
  m_CarinaToRightAnatomicDeadSpace->GetNextResistance().SetValue(dRightBronchiResistance, FlowResistanceUnit::cmH2O_s_Per_L);
  m_LeftAnatomicDeadSpaceToLeftAlveoli->GetNextResistance().SetValue(dLeftAlveoliResistance, FlowResistanceUnit::cmH2O_s_Per_L);
  m_RightAnatomicDeadSpaceToRightAlveoli->GetNextResistance().SetValue(dRightAlveoliResistance, FlowResistanceUnit::cmH2O_s_Per_L);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Modifies the pressure and/or flow at the mouth
///
/// \details
/// Handles the mechanical ventilation action that adds a flow and pressure source to instantaneously
/// set the respiratory connection (mouth) to user specified values.  
//--------------------------------------------------------------------------------------------------
void Respiratory::MechanicalVentilation()
{
  if (m_data.GetActions().GetPatientActions().HasMechanicalVentilation())
  {
    SEMechanicalVentilation* mv = m_data.GetActions().GetPatientActions().GetMechanicalVentilation();
    // You only get here if action is On
    m_data.SetAirwayMode(CDM::enumBioGearsAirwayMode::MechanicalVentilator);
    
	//Set the substance volume fractions ********************************************
	std::vector<SESubstanceFraction*> gasFractions = mv->GetGasFractions();
	
	//Reset the substance quantities at the connection
	for (SEGasSubstanceQuantity* subQ : m_MechanicalVentilatorConnection->GetSubstanceQuantities())
		subQ->SetToZero();	
	
	//If no gas fractions specified, assume ambient
	if (gasFractions.empty())
	{
		for (auto s : m_Environment->GetSubstanceQuantities())
		{
			m_MechanicalVentilatorConnection->GetSubstanceQuantity(s->GetSubstance())->GetVolumeFraction().Set(s->GetVolumeFraction());
		}
	}
	else
	{
		//Has fractions defined
		for (auto f : gasFractions)
		{
			SESubstance& sub = f->GetSubstance();
			double fraction = f->GetFractionAmount().GetValue();

			//Do this, just in case it's something new
			m_data.GetSubstances().AddActiveSubstance(sub);

			//Now set it on the connection compartment
			//It has a NaN volume, so this will keep the same volume fraction no matter what's going on around it
			m_MechanicalVentilatorConnection->GetSubstanceQuantity(sub)->GetVolumeFraction().SetValue(fraction);
		}
	}

	//Apply the instantaneous flow ********************************************
	if (mv->HasFlow())
	{
		m_ConnectionToMouth->GetNextFlowSource().Set(mv->GetFlow());
		//It may or may not be there
		if (!m_ConnectionToMouth->HasFlowSource())
		{
			m_data.GetCircuits().GetRespiratoryAndMechanicalVentilatorCircuit().StateChange();
		}
	}
	else
	{
		//If there's no flow specified, we need to remove the flow source		
		if (m_ConnectionToMouth->HasNextFlowSource())
		{
			m_ConnectionToMouth->GetNextFlowSource().Invalidate();
			m_data.GetCircuits().GetRespiratoryAndMechanicalVentilatorCircuit().StateChange();
		}
	}
	
	//Apply the instantaneous pressure ********************************************	
	if (mv->HasPressure())
	{
		//This is the pressure above ambient
		m_GroundToConnection->GetNextPressureSource().Set(mv->GetPressure());
	}
	else
	{
		//Pressure is same as ambient
		m_GroundToConnection->GetNextPressureSource().SetValue(0.0, PressureUnit::cmH2O);
	}	
  }
  else if (m_data.GetAirwayMode() == CDM::enumBioGearsAirwayMode::MechanicalVentilator)
  {
    // Was just turned off
    m_data.SetAirwayMode(CDM::enumBioGearsAirwayMode::Free);
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Respiratory driver pressure source
///
/// \details
/// Calculates the muscle pressure source pressure by using the chemical stimuli as feedback control mechanism.
/// The method reads the arterial O2 and CO2 partial pressures. Using these partial pressures, the method calculates the
/// alveolar ventilation from which the method calculates the target breathing frequency. The target breathing frequency is
/// used in the equation for muscle pressure source. The muscle pressure source is used as a driver for ventilation.
/// This method also calculates the drug modifiers that adjusts the depth and frequency of respiration.
//--------------------------------------------------------------------------------------------------
void Respiratory::RespiratoryDriver()
{
  m_BreathingCycleTime_s += m_dt_s;

  //Keep a running average of the Arterial Partial Pressures	
  m_ArterialO2RunningAverage_mmHg.Sample(m_AortaO2->GetPartialPressure(PressureUnit::mmHg));
  m_ArterialCO2RunningAverage_mmHg.Sample(m_AortaCO2->GetPartialPressure(PressureUnit::mmHg));
  //Reset at start of cardiac cycle 
  if (m_Patient->IsEventActive(CDM::enumPatientEvent::StartOfCardiacCycle))
  {
    m_ArterialO2PartialPressure_mmHg = m_ArterialO2RunningAverage_mmHg.Value();
    m_ArterialCO2PartialPressure_mmHg = m_ArterialCO2RunningAverage_mmHg.Value();

    m_ArterialO2RunningAverage_mmHg.Reset();
    m_ArterialCO2RunningAverage_mmHg.Reset();
  }


#ifdef TUNING
  m_ArterialO2PartialPressure_mmHg = 95.0;
  m_ArterialCO2PartialPressure_mmHg = 40.0;
#endif	

  if (m_ConsciousBreathing) //Conscious breathing 
  {
	  SEConsciousRespiration* cr = m_data.GetActions().GetPatientActions().GetConsciousRespiration();
	  SEConsciousRespirationCommand* cmd = cr->GetActiveCommand();
	  SEUseInhaler* ui = dynamic_cast<SEUseInhaler*>(cmd);
	  if (ui != nullptr)
	  {
		  //We're using the inhaler, so just wait at this driver pressure
		  m_DriverPressure_cmH2O = m_DriverPressurePath->GetPressureSource(PressureUnit::cmH2O);
		  m_ConsciousBreathing = false;
	  }
	  else
	  {
		  //Just increase/decrease the driver pressure linearly to achieve the desired 
		//pressure (attempting to reach a specific volume) at the end of the user specified period
		  double Time_s = m_ConsciousRespirationPeriod_s - m_ConsciousRespirationRemainingPeriod_s;
		  double Slope = (m_ConsciousEndPressure_cmH2O - m_ConsciousStartPressure_cmH2O) / m_ConsciousRespirationPeriod_s;
		  //Form of y=mx+b
		  m_DriverPressure_cmH2O = Slope * Time_s + m_ConsciousStartPressure_cmH2O;

		  //Make it so we start a fresh cycle when we go back to spontaneous breathing
		  //Adding 2.0 * timestamp just makes it greater than the TotalBreathingCycleTime_s
		  m_VentilationFrequency_Per_min = m_Patient->GetRespirationRateBaseline(FrequencyUnit::Per_min);
		  m_BreathingCycleTime_s = 60.0 / m_VentilationFrequency_Per_min + 2.0;
	  }
  }
  else //Spontaneous (i.e. not conscious) breathing
  {
    double TotalBreathingCycleTime_s = 0.0;
    if (m_VentilationFrequency_Per_min < 1.0)
    {
      TotalBreathingCycleTime_s = 0.0;
    }
    else
    {
      TotalBreathingCycleTime_s = 60.0 / m_VentilationFrequency_Per_min; //Total time of one breathing cycle	
    }

    //Prepare for the next cycle -------------------------------------------------------------------------------
    if (m_BreathingCycleTime_s > TotalBreathingCycleTime_s) //End of the cycle or currently not breathing
    {
 
		UpdateIERatio();

      // Make a cardicArrestEffect that is 1.0 unless cardiac arrest is true
      double cardiacArrestEffect = 1.0;
      // If the cv system parameter is true, then make the cardicArrestEffect = 0
      if (m_Patient->IsEventActive(CDM::enumPatientEvent::CardiacArrest))
      {
        cardiacArrestEffect = 0.0;
      }

      //Ventilatory Negative Feedback Control *************************************************************************
      double PeripheralCO2PartialPressureSetPoint = m_data.GetConfiguration().GetPeripheralControllerCO2PressureSetPoint(PressureUnit::mmHg);
      double CentralCO2PartialPressureSetPoint = m_data.GetConfiguration().GetCentralControllerCO2PressureSetPoint(PressureUnit::mmHg);

      double metabolicModifier = 1.0;
      double TMR_W = m_data.GetEnergy().GetTotalMetabolicRate(PowerUnit::W);
      double BMR_W = m_Patient->GetBasalMetabolicRate(PowerUnit::W);
      double metabolicFraction = TMR_W / BMR_W;

      //Get Drug Effects
      SEDrugSystem& Drugs = m_data.GetDrugs();
      double DrugRRChange_Per_min = Drugs.GetRespirationRateChange(FrequencyUnit::Per_min);
      double DrugsTVChange_L = Drugs.GetTidalVolumeChange(VolumeUnit::L);
      double NMBModifier = 1.0;
      double SedationModifier = 1.0;
      //Make changes to Respiration Rate change based on the neuromuscular block level
      if (Drugs.GetNeuromuscularBlockLevel().GetValue() > 0.135)
      {
        NMBModifier = 0.0;
        DrugRRChange_Per_min = 0.0;
        DrugsTVChange_L = 0.0;
      }
      else if (Drugs.GetNeuromuscularBlockLevel().GetValue() > 0.11)
      {
        NMBModifier = 0.5;
        DrugRRChange_Per_min = 0.0;
        DrugsTVChange_L = 0.0;
      }
      else if (Drugs.GetSedationLevel().GetValue() > 0.15 && std::abs(m_Patient->GetRespirationRateBaseline(FrequencyUnit::Per_min) + DrugRRChange_Per_min) < 1.0)
      {
        SedationModifier = 0.0;
        DrugRRChange_Per_min = 0.0;
        DrugsTVChange_L = 0.0;
      }

      //Calculate the target Alveolar Ventilation based on the Arterial O2 and CO2 concentrations
      double dTargetAlveolarVentilation_L_Per_min = m_PeripheralControlGainConstant*exp(-0.05*m_ArterialO2PartialPressure_mmHg)*MAX(0., m_ArterialCO2PartialPressure_mmHg - PeripheralCO2PartialPressureSetPoint); //Peripheral portion
      dTargetAlveolarVentilation_L_Per_min += m_CentralControlGainConstant*MAX(0., m_ArterialCO2PartialPressure_mmHg - CentralCO2PartialPressureSetPoint); //Central portion

    //Metabolic modifier is used to drive the system to reasonable levels achievable during increased metabolic exertion
    //The modifier is tuned to achieve the correct respiratory response for near maximal exercise. A linear relationship is assumed
    // for the respiratory effects due to increased metabolic exertion	  
      double tunedVolumeMetabolicSlope = 0.2; //Tuned fractional increase of the tidal volume due to increased metabolic rate
      metabolicModifier = 1.0 + tunedVolumeMetabolicSlope*(metabolicFraction - 1.0);
      dTargetAlveolarVentilation_L_Per_min *= metabolicModifier;

      //Only move it part way to where it wants to be.
      //If you stays there, it will get there, just more controlled/slowly.
      //This is needed so we don't overshoot and introduce low frequency oscillations into the system (i.e. overdamped).
      double targetCO2PP_mmHg = 40.0;
      double changeFraction = 1.0;
      if (m_data.GetState() <= EngineState::InitialStabilization)
      {
        //This gets it nice and stable
        changeFraction = 0.1;
      }
      else
      {
        //This keeps it from going crazy with modifiers applied
        changeFraction = std::abs(m_ArterialCO2PartialPressure_mmHg - targetCO2PP_mmHg) / targetCO2PP_mmHg * 0.5;
      }
      changeFraction = MIN(changeFraction, 1.0);
      dTargetAlveolarVentilation_L_Per_min = m_PreviousTargetAlveolarVentilation_L_Per_min + (dTargetAlveolarVentilation_L_Per_min - m_PreviousTargetAlveolarVentilation_L_Per_min) * changeFraction;
      m_PreviousTargetAlveolarVentilation_L_Per_min = dTargetAlveolarVentilation_L_Per_min;

      //Target Tidal Volume (i.e. Driver amplitude) *************************************************************************
      //Calculate the target Tidal Volume based on the Alveolar Ventilation
      double dTargetPulmonaryVentilation_L_Per_min = dTargetAlveolarVentilation_L_Per_min + GetTotalDeadSpaceVentilation(VolumePerTimeUnit::L_Per_min);

      double dMaximumPulmonaryVentilationRate = m_data.GetConfiguration().GetPulmonaryVentilationRateMaximum(VolumePerTimeUnit::L_Per_min);
      /// \event Patient: Maximum Pulmonary Ventilation Rate : Pulmonary ventilation exceeds maximum value
      if (dTargetPulmonaryVentilation_L_Per_min > dMaximumPulmonaryVentilationRate)
      {
        dTargetPulmonaryVentilation_L_Per_min = dMaximumPulmonaryVentilationRate;
        m_Patient->SetEvent(CDM::enumPatientEvent::MaximumPulmonaryVentilationRate, true, m_data.GetSimulationTime());
      }

      if (dTargetPulmonaryVentilation_L_Per_min < dMaximumPulmonaryVentilationRate && m_Patient->IsEventActive(CDM::enumPatientEvent::MaximumPulmonaryVentilationRate))
      {
        m_Patient->SetEvent(CDM::enumPatientEvent::MaximumPulmonaryVentilationRate, false, m_data.GetSimulationTime());
      }

      //Calculate the target Tidal Volume based on the calculated target pulmonary ventilation, plot slope (determined during initialization), and x-intercept
      double dTargetTidalVolume_L = dTargetPulmonaryVentilation_L_Per_min / m_VentilationToTidalVolumeSlope + m_VentilationTidalVolumeIntercept;

      //Modify the target tidal volume due to other external effects - probably eventually replaced by the Nervous system
      dTargetTidalVolume_L *= cardiacArrestEffect*NMBModifier;

      //Apply Drug Effects to the target tidal volume
      dTargetTidalVolume_L += DrugsTVChange_L;

      //This is a piecewise function that plateaus at the Tidal Volume equal to 1/2 * Vital Capacity
      //The Respiration Rate will make up for the Alveoli Ventilation difference
      double dHalfVitalCapacity_L = m_Patient->GetVitalCapacity(VolumeUnit::L) / 2;
      dTargetTidalVolume_L = MIN(dTargetTidalVolume_L, dHalfVitalCapacity_L);

      //Map the Target Tidal Volume to the Driver
      double TargetVolume_L = m_InitialFunctionalResidualCapacity_L + dTargetTidalVolume_L;
      m_PeakRespiratoryDrivePressure_cmH2O = VolumeToDriverPressure(TargetVolume_L);
      //There's a maximum force the driver can try to achieve
      m_PeakRespiratoryDrivePressure_cmH2O = MAX(m_PeakRespiratoryDrivePressure_cmH2O, m_MaxDriverPressure_cmH2O);
      //The peak driver pressure is the pressure above the default pressure.  Therefore, we subtract it from the base pressure.
      m_PeakRespiratoryDrivePressure_cmH2O -= m_DefaultDrivePressure_cmH2O;

      //Respiration Rate (i.e. Driver frequency) *************************************************************************
      //Calculate the Respiration Rate given the Alveolar Ventilation and the Target Tidal Volume
      if (dTargetTidalVolume_L == 0.0) //Can't divide by zero
      {
        m_VentilationFrequency_Per_min = 0.0;
        m_bNotBreathing = true;
      }
      else
      {
        m_VentilationFrequency_Per_min = dTargetPulmonaryVentilation_L_Per_min / dTargetTidalVolume_L; //breaths/min
        m_VentilationFrequency_Per_min *= NMBModifier * SedationModifier;
        m_VentilationFrequency_Per_min += DrugRRChange_Per_min;
        m_bNotBreathing = false;
      }

      if (m_VentilationFrequency_Per_min > dMaximumPulmonaryVentilationRate / dHalfVitalCapacity_L)
        m_VentilationFrequency_Per_min = dMaximumPulmonaryVentilationRate / dHalfVitalCapacity_L;

      if (m_VentilationFrequency_Per_min < 0.0)
        m_VentilationFrequency_Per_min = 0.0;

      //Patient Definition *************************************************************************
      //We need to hit the patient's defined Respiration Rate Baseline, no matter what,
      //so we'll keep adjusting the slope of the function to achieve this
      if (m_data.GetState() <= EngineState::InitialStabilization)
      {
        double dRespirationRateBaseline_Per_min = m_Patient->GetRespirationRateBaseline(FrequencyUnit::Per_min);
        double dPercentError = (m_VentilationFrequency_Per_min - dRespirationRateBaseline_Per_min) / dRespirationRateBaseline_Per_min; //negative if too low

        //Amplitude set-point - this will set the Tidal Volume baseline when O2 and CO2 are at the correct/balanced level
        m_VentilationToTidalVolumeSlope = m_VentilationToTidalVolumeSlope - m_VentilationToTidalVolumeSlope * dPercentError;

        //Put bounds on this
        m_VentilationToTidalVolumeSlope = LIMIT(m_VentilationToTidalVolumeSlope, 1.0, 100.0);
      }

#ifdef TUNING
      m_VentilationToTidalVolumeSlope = 22.0;
      m_VentilationFrequency_Per_min = 16.0;
#endif

      m_BreathingCycleTime_s = 0.0;

      //KEEP COMMENTED OUT - this is for keeping the driver constant while debugging
      //m_VentilationFrequency_Per_min = 16.0;
      //m_PeakRespiratoryDrivePressure = -11.3;
    }


    //Run the driver based on the waveform -------------------------------------------------------------------------------
    double driverInspirationTime_s = m_IEscaleFactor*(0.0125*m_VentilationFrequency_Per_min + 0.125)*TotalBreathingCycleTime_s; //Inspiration time

    if (m_BreathingCycleTime_s < driverInspirationTime_s) //Inspiration
    {
      double driverPressure_cmH2O = m_DefaultDrivePressure_cmH2O + m_PeakRespiratoryDrivePressure_cmH2O*(1.0 - exp(-((m_VentilationFrequency_Per_min + 4.0 * m_VentilatoryOcclusionPressure_cmH2O) / 10.0)*m_BreathingCycleTime_s));
      m_DriverPressure_cmH2O = driverPressure_cmH2O;
    }
    else if (m_BreathingCycleTime_s >= driverInspirationTime_s &&  m_BreathingCycleTime_s < driverInspirationTime_s + m_dt_s) //Transition
    {
      m_DriverPressureMin_cmH2O = m_PeakRespiratoryDrivePressure_cmH2O*(1 - exp(-((m_VentilationFrequency_Per_min + 4.0 * m_VentilatoryOcclusionPressure_cmH2O) / 10.0)*driverInspirationTime_s));
    }
    else //Expiration
    {
      double driverPressure_cmH2O = m_DefaultDrivePressure_cmH2O + m_DriverPressureMin_cmH2O*(exp(-((m_VentilationFrequency_Per_min + m_VentilatoryOcclusionPressure_cmH2O / 2.0) / 10.0)*(m_BreathingCycleTime_s - driverInspirationTime_s)));
      m_DriverPressure_cmH2O = driverPressure_cmH2O;
    }

	Apnea();

    if (m_bNotBreathing)
    {
      m_DriverPressure_cmH2O = m_DefaultDrivePressure_cmH2O;
    }
  }

  //Push Driving Data to the Circuit -------------------------------------------------------------------------------
  m_DriverPressurePath->GetNextPressureSource().SetValue(m_DriverPressure_cmH2O, PressureUnit::cmH2O);
}
//--------------------------------------------------------------------------------------------------
/// \brief
/// Airway obstruction 
///
/// \details
/// Various factors can trigger upper airway obstruction. The Biogears engine handles airway obstruction that arise from 
/// the partial or complete obstruction of the upper airways by foreign objects. The model accounts for such obstruction by setting the 
/// flow resistance of the trachea compartment in proportion to the severity of the obstruction.
/// The function updates the tracheal flow resistance by modifying the resistance across the airway to carina node path.
//--------------------------------------------------------------------------------------------------
void Respiratory::AirwayObstruction()
{
  if (!m_PatientActions->HasAirwayObstruction())
    return;

  double Severity = m_PatientActions->GetAirwayObstruction()->GetSeverity().GetValue();
  double AirwayResistance = m_MouthToCarina->GetNextResistance().GetValue(FlowResistanceUnit::cmH2O_s_Per_L);
  double dClosedResistance = AirwayResistance;
  AirwayResistance = GeneralMath::ResistanceFunction(20.0, m_dRespOpenResistance_cmH2O_s_Per_L, dClosedResistance, Severity);
  m_MouthToCarina->GetNextResistance().SetValue(AirwayResistance, FlowResistanceUnit::cmH2O_s_Per_L);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Bronchoconstriction
///
/// \details
/// Bronchoconstriction involves the tightening of smooth muscles surrounding bronchi. The effect of such airway constriction is the 
/// reduction of air flow or increase of flow resistance of the lower airways. The Biogears model handles bronchoconstriction by increasing
/// the flow resistances of the bronchi compartments. The function updates the bronchial resistances in proportion to the severity of the 
/// bronchoconstriction. 
//--------------------------------------------------------------------------------------------------
void Respiratory::BronchoConstriction()
{
  if (!m_PatientActions->HasBronchoconstriction())
    return;

  double LeftBronchiResistance = m_CarinaToLeftAnatomicDeadSpace->GetNextResistance().GetValue(FlowResistanceUnit::cmH2O_s_Per_L);
  double RightBronchiResistance = m_CarinaToRightAnatomicDeadSpace->GetNextResistance().GetValue(FlowResistanceUnit::cmH2O_s_Per_L);
  double dSeverity = m_PatientActions->GetBronchoconstriction()->GetSeverity().GetValue();
  double dClosedResistance = LeftBronchiResistance;
  LeftBronchiResistance = GeneralMath::ResistanceFunction(70.0, m_dRespOpenResistance_cmH2O_s_Per_L, dClosedResistance, dSeverity);
  dClosedResistance = RightBronchiResistance;
  RightBronchiResistance = GeneralMath::ResistanceFunction(70.0, m_dRespOpenResistance_cmH2O_s_Per_L, dClosedResistance, dSeverity);

  m_CarinaToLeftAnatomicDeadSpace->GetNextResistance().SetValue(LeftBronchiResistance, FlowResistanceUnit::cmH2O_s_Per_L);
  m_CarinaToRightAnatomicDeadSpace->GetNextResistance().SetValue(RightBronchiResistance, FlowResistanceUnit::cmH2O_s_Per_L);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Bronchodilation PD effects.
///
/// \details
/// This reduces the bronchi resitances based on drug PD effects (i.e., plasma concentrations).
//--------------------------------------------------------------------------------------------------
void Respiratory::BronchoDilation()
{
	//Note: this could constrict with a negative number from PD
	
	//We're going to make the bronchodilation effects be based off of Albuterol.
	//A value of 1.0 will be the effective Albuterol EMax (with the current, non-overdose implementation), and -1.0 will be a bronchconstriction the same percentage in the other direction.
	//Experimentally, 1mg of Albuterol given via a spacer device on an endotracheal tube caused a pulmonary resistance change of ~20% @cite mancebo1991effects.
	//The bronchi are ~30% of the total pulmonary resistance, so we'll make a dilation effect of 1.0 be at the respiratory open resistance.
	//Dilation effect values have max effect at 1 and below -1, so anything outside of that will maintain that effect.
	double bronchoDilationEffect = m_data.GetDrugs().GetBronchodilationLevel().GetValue();
	if (bronchoDilationEffect != 0.0)
	{
		//Note: It'll pretty much always get in here because there's epinephrine present
		// Get the path resistances 
		double dLeftBronchiResistance = m_CarinaToLeftAnatomicDeadSpace->GetNextResistance().GetValue(FlowResistanceUnit::cmH2O_s_Per_L);
		double dRightBronchiResistance = m_CarinaToRightAnatomicDeadSpace->GetNextResistance().GetValue(FlowResistanceUnit::cmH2O_s_Per_L);
		if (bronchoDilationEffect >= 0.0)// positive, therefore dilation
		{
			bronchoDilationEffect = MIN(bronchoDilationEffect, 1.0);
			dLeftBronchiResistance = GeneralMath::ResistanceFunction(10.0, m_dRespClosedResistance_cmH2O_s_Per_L, dLeftBronchiResistance, bronchoDilationEffect);
			dRightBronchiResistance = GeneralMath::ResistanceFunction(10.0, m_dRespClosedResistance_cmH2O_s_Per_L, dRightBronchiResistance, bronchoDilationEffect);
		}
		else //negative, therefore constriction
		{
			bronchoDilationEffect = MIN(-bronchoDilationEffect, 1.0);
			dLeftBronchiResistance = GeneralMath::ResistanceFunction(10.0, dLeftBronchiResistance, m_dRespOpenResistance_cmH2O_s_Per_L, bronchoDilationEffect);
			dRightBronchiResistance = GeneralMath::ResistanceFunction(10.0, dRightBronchiResistance, m_dRespOpenResistance_cmH2O_s_Per_L, bronchoDilationEffect);
		}
		m_CarinaToLeftAnatomicDeadSpace->GetNextResistance().SetValue(dLeftBronchiResistance, FlowResistanceUnit::cmH2O_s_Per_L);
		m_CarinaToRightAnatomicDeadSpace->GetNextResistance().SetValue(dRightBronchiResistance, FlowResistanceUnit::cmH2O_s_Per_L);
	}
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Esophageal Intubation
///
/// \details
/// During mechanical ventilation, one of the clinical complications of endotracheal intubation is esophageal intubation. This involves the 
/// misplacement of the tube down the esophagus. Such event prohibits air flow into or out of the lungs. The Biogers circuit handles 
/// this respiratory distress by manipulating the tracheal resistance. When esophageal intubation incidence is triggered, significantly large 
/// resistance is assigned to the trachea compartment. Otherwise, the esophageal compartment resistance is set to be significantly
/// large value under normal condition. 
//--------------------------------------------------------------------------------------------------
void Respiratory::Intubation()
{
  //Don't modify the stomach on environment changes
  if (m_Ambient->GetNextPressure(PressureUnit::cmH2O) != m_Ambient->GetPressure(PressureUnit::cmH2O))
  {
    //The environment just changed
    //Keep volume the same by changing the pressure equally on both sides
    double pressureChange_cmH2O = m_Ambient->GetNextPressure(PressureUnit::cmH2O) - m_Ambient->GetPressure(PressureUnit::cmH2O);
    m_Stomach->GetNextPressure().IncrementValue(pressureChange_cmH2O, PressureUnit::cmH2O);
  }

  if (m_PatientActions->HasIntubation())
  {
    m_data.SetIntubation(CDM::enumOnOff::On);
    SEIntubation* intubation = m_PatientActions->GetIntubation();
    switch (intubation->GetType())
    {
      case CDM::enumIntubationType::Tracheal:
      {// The proper way to intubate
        // Airway mode handles this case by default
        break;
      }
      case CDM::enumIntubationType::Esophageal:
      {        
        // Allow air flow between Airway and Stomach
        ///\todo Make this a modifier (i.e. multiplier), instead of setting it directly
        m_MouthToStomach->GetNextResistance().SetValue(1.2, FlowResistanceUnit::cmH2O_s_Per_L);
        // Stop air flow between the Airway and Carina
        //This is basically an open switch.  We don't need to worry about anyone else modifying it if this action is on.
        m_MouthToCarina->GetNextResistance().SetValue(m_dDefaultOpenResistance_cmH2O_s_Per_L, FlowResistanceUnit::cmH2O_s_Per_L);
        break;
      }      
      case CDM::enumIntubationType::RightMainstem:
      {
        m_CarinaToLeftAnatomicDeadSpace->GetNextResistance().SetValue(m_dRespOpenResistance_cmH2O_s_Per_L, FlowResistanceUnit::cmH2O_s_Per_L);
        break;
      }
      case CDM::enumIntubationType::LeftMainstem:
      {
        m_CarinaToRightAnatomicDeadSpace->GetNextResistance().SetValue(m_dRespOpenResistance_cmH2O_s_Per_L, FlowResistanceUnit::cmH2O_s_Per_L);
        break;
      }
    }
  }
  else
  {
    m_data.SetIntubation(CDM::enumOnOff::Off);
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tension pneumothorax
///
/// \details
/// Pneumothorax is an abnormal accumulation of air in the pleural cavity. Tension pneumothorax occurs when 
/// air escapes into the pleural space on inspiration, but cannot return on expiration. Different types of pneumothorces are known.
/// The Biogears engine models  open (sucking chest wound) and closed (intact thoracic cage) pnumothoraces whereas occlusive dressing
/// and needle decompression are modeled as interventions. 
/// The function models pneumothorax and needle decompression by opening respiratory circuits that allow air flows into 
/// and out of the pleural cavity. Under normal condition, the path between the lung node or the outside environment and the intrapleural cavity 
/// is blocked via significantly high resistances that serve as open switches. When open or closed pneumothorax is triggered, these resistances 
/// are reduced in proportion to the severity of the pneumothorax. This reduction permits air flow into the pleural cavity a
/// and cause respiratory and cardiovascular distress.
/// The function allows the selection of tension pneumothorax and/or needle decompression events on the left or right lung side. The effects on 
/// the cardiovascular system parameters is accounted through modifiers that scale in proportion to the accumulated gas. 
//--------------------------------------------------------------------------------------------------
void Respiratory::Pneumothorax()
{
  if (m_PatientActions->HasTensionPneumothorax())
  {
    // Minimum flow resistance for the chest cavity or alveoli leak 
    double dPneumoMinFlowResistance_cmH2O_s_Per_L = 30.0;
    // Maximum flow resistance for the chest cavity or alveoli leak
    double dPneumoMaxFlowResistance_cmH2O_s_Per_L = m_dDefaultOpenResistance_cmH2O_s_Per_L;
    // Flow resistance for the decompression needle, if used
    double dNeedleFlowResistance_cmH2O_s_Per_L = 15.0;

    if (m_PatientActions->HasLeftOpenTensionPneumothorax())
    {
      // Scale the flow resistance through the chest opening based on severity
      double severity = m_PatientActions->GetLeftOpenTensionPneumothorax()->GetSeverity().GetValue();
	  double resistance_cmH2O_s_Per_L = dPneumoMaxFlowResistance_cmH2O_s_Per_L;
	  if (severity > 0.0 && !m_PatientActions->HasLeftChestOcclusiveDressing())
	  {
		resistance_cmH2O_s_Per_L = dPneumoMinFlowResistance_cmH2O_s_Per_L / pow(severity, 2.0);
	  }	  
	  resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, dPneumoMaxFlowResistance_cmH2O_s_Per_L);
      m_EnvironmentToLeftChestLeak->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, FlowResistanceUnit::cmH2O_s_Per_L);
      
	  if (m_PatientActions->HasLeftNeedleDecompression())
      {
        DoLeftNeedleDecompression(dNeedleFlowResistance_cmH2O_s_Per_L);
      }
    }

    if (m_PatientActions->HasRightOpenTensionPneumothorax())
    {
		// Scale the flow resistance through the chest opening based on severity
		double severity = m_PatientActions->GetRightOpenTensionPneumothorax()->GetSeverity().GetValue();
		double resistance_cmH2O_s_Per_L = dPneumoMaxFlowResistance_cmH2O_s_Per_L;
		if (severity > 0.0 && !m_PatientActions->HasRightChestOcclusiveDressing())
		{
			resistance_cmH2O_s_Per_L = dPneumoMinFlowResistance_cmH2O_s_Per_L / pow(severity, 2.0);
		}
		resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, dPneumoMaxFlowResistance_cmH2O_s_Per_L);
		m_EnvironmentToRightChestLeak->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, FlowResistanceUnit::cmH2O_s_Per_L);

      if (m_PatientActions->HasRightNeedleDecompression())
      {
        DoRightNeedleDecompression(dNeedleFlowResistance_cmH2O_s_Per_L);
      }
      m_EnvironmentToRightChestLeak->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, FlowResistanceUnit::cmH2O_s_Per_L);
    }

    if (m_PatientActions->HasLeftClosedTensionPneumothorax())
    {
		// Scale the flow resistance through the chest opening based on severity
		double severity = m_PatientActions->GetLeftClosedTensionPneumothorax()->GetSeverity().GetValue();
		double resistance_cmH2O_s_Per_L = dPneumoMaxFlowResistance_cmH2O_s_Per_L;
		if (severity > 0.0)
		{
			resistance_cmH2O_s_Per_L = dPneumoMinFlowResistance_cmH2O_s_Per_L / pow(severity, 2.0);
		}
		resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, dPneumoMaxFlowResistance_cmH2O_s_Per_L);
		m_LeftAlveoliLeakToLeftPleural->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, FlowResistanceUnit::cmH2O_s_Per_L);

      if (m_PatientActions->HasLeftNeedleDecompression())
      {
        DoLeftNeedleDecompression(dNeedleFlowResistance_cmH2O_s_Per_L);
      }
    }

    if (m_PatientActions->HasRightClosedTensionPneumothorax())
    {
		// Scale the flow resistance through the chest opening based on severity
		double severity = m_PatientActions->GetRightClosedTensionPneumothorax()->GetSeverity().GetValue();
		double resistance_cmH2O_s_Per_L = dPneumoMaxFlowResistance_cmH2O_s_Per_L;
		if (severity > 0.0)
		{
			resistance_cmH2O_s_Per_L = dPneumoMinFlowResistance_cmH2O_s_Per_L / pow(severity, 2.0);
		}
		resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, dPneumoMaxFlowResistance_cmH2O_s_Per_L);
		m_RightAlveoliLeakToRightPleural->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, FlowResistanceUnit::cmH2O_s_Per_L);

      if (m_PatientActions->HasRightNeedleDecompression())
      {
        DoRightNeedleDecompression(dNeedleFlowResistance_cmH2O_s_Per_L);
      }
    }

    //Check for interventions without insult
    if (!m_PatientActions->HasLeftClosedTensionPneumothorax() && !m_PatientActions->HasLeftOpenTensionPneumothorax())
    {
      if (m_PatientActions->HasLeftChestOcclusiveDressing() || m_PatientActions->HasLeftNeedleDecompression())
      {
        /// \error Patient: Cannot perform an intervention if Tension Pneumothorax is not present on that side.
        Error("Cannot perform an intervention if Tension Pneumothorax is not present on that side.");
        m_PatientActions->RemoveLeftChestOcclusiveDressing();
        m_PatientActions->RemoveLeftNeedleDecompression();
        return;
      }
    }
    if (!m_PatientActions->HasRightClosedTensionPneumothorax() && !m_PatientActions->HasRightOpenTensionPneumothorax())
    {
      if (m_PatientActions->HasRightChestOcclusiveDressing() || m_PatientActions->HasRightNeedleDecompression())
      {
        /// \error Patient: Cannot perform an intervention if Tension Pneumothorax is not present on that side.
        Error("Cannot perform an intervention if Tension Pneumothorax is not present on that side.");
        m_PatientActions->RemoveRightChestOcclusiveDressing();
        m_PatientActions->RemoveRightNeedleDecompression();
        return;
      }
    }
  }
  else
  {
    // Check for cases where the needle decompression or occlusive dressing actions are called without having
    // initiated a pneumothorax action
    if (m_PatientActions->HasNeedleDecompression())
    {
      /// \error Patient: can't process needle decompression if no pneumothorax is present
      Error("Cannot perform a Needle Decompression intervention if Tension Pneumothorax is not present");
      m_PatientActions->RemoveLeftNeedleDecompression();
      m_PatientActions->RemoveRightNeedleDecompression();
      return;
    }
    if (m_PatientActions->HasChestOcclusiveDressing())
    {
      /// \error Patient: can't process a chest occlusive dressing if no pneumothorax is present			
      Error("Cannot perform a Chest Occlusive Dressing intervention if Tension Pneumothorax is not present");
      m_PatientActions->RemoveLeftChestOcclusiveDressing();
      m_PatientActions->RemoveRightChestOcclusiveDressing();
      return;
    }
  }
}


//--------------------------------------------------------------------------------------------------
/// \brief
/// Conscious respiration
///
/// \param  None
///
/// \return void
///
/// \details
/// This method determines when to process a conscious respiration command and removes the command
/// from the stack.
//--------------------------------------------------------------------------------------------------
void Respiratory::ConsciousRespiration()
{
  if (m_ConsciousBreathing && m_ConsciousRespirationRemainingPeriod_s >= 0.0)
  {
    m_ConsciousRespirationRemainingPeriod_s -= m_dt_s;

    if (m_ConsciousRespirationRemainingPeriod_s <= 0.0)
    {// We are done with this command
      m_ConsciousRespirationRemainingPeriod_s = 0.0;
      m_ConsciousRespirationPeriod_s = 0.0;
      m_ConsciousStartPressure_cmH2O = 0.0;
      m_ConsciousEndPressure_cmH2O = 0.0;
      m_ExpiratoryReserveVolumeFraction = -1.0;
      m_InspiratoryCapacityFraction = -1.0;
      m_ConsciousBreathing = false;
      SEConsciousRespiration* cr = m_PatientActions->GetConsciousRespiration();
      m_ss << "Completed Conscious Respiration Command : " << *cr->GetActiveCommand();
      Info(m_ss);
      cr->RemoveActiveCommand();
      
    }
  }

  //We'll wait for any current commands to complete first - only one at a time
  if (!m_ConsciousBreathing && m_PatientActions->HasConsciousRespiration())
  {
    SEConsciousRespiration* cr = m_PatientActions->GetConsciousRespiration();
    SEConsciousRespirationCommand* cmd = cr->GetActiveCommand();
	ProcessConsciousRespiration(*cmd);
  }
}


//--------------------------------------------------------------------------------------------------
/// \brief
/// Process conscious respiration
///
/// \param  cmd - Conscious respiration command
///
/// \details
/// Processes conscious respiration commands.
//--------------------------------------------------------------------------------------------------
void Respiratory::ProcessConsciousRespiration(SEConsciousRespirationCommand& cmd)
{
  if (m_ConsciousRespirationRemainingPeriod_s > 0)
  {
    Error("Already processing a Conscious Respiration Command, ignoring this command");
    return;
  }

  m_ConsciousBreathing = true;
  m_ConsciousStartPressure_cmH2O = m_DriverPressurePath->GetPressureSource(PressureUnit::cmH2O);

  SEBreathHold* bh = dynamic_cast<SEBreathHold*>(&cmd);
  if (bh != nullptr)
  {
    m_ConsciousRespirationRemainingPeriod_s = bh->GetPeriod().GetValue(TimeUnit::s);
    m_ConsciousRespirationPeriod_s = m_ConsciousRespirationRemainingPeriod_s;
    m_ConsciousEndPressure_cmH2O = m_ConsciousStartPressure_cmH2O;

    return;
  }
  SEForcedExhale* fe = dynamic_cast<SEForcedExhale*>(&cmd);
  if (fe != nullptr)
  {
    m_ExpiratoryReserveVolumeFraction = fe->GetExpiratoryReserveVolumeFraction().GetValue();
    m_ConsciousRespirationRemainingPeriod_s = fe->GetPeriod().GetValue(TimeUnit::s);
    m_ConsciousRespirationPeriod_s = m_ConsciousRespirationRemainingPeriod_s;

    //Pressure effects
    double TargetVolume_L = m_InitialResidualVolume_L + m_InitialExpiratoryReserveVolume_L * (1.0 - m_ExpiratoryReserveVolumeFraction);
    m_ConsciousEndPressure_cmH2O = VolumeToDriverPressure(TargetVolume_L);

    return;
  }
  SEForcedInhale* fi = dynamic_cast<SEForcedInhale*>(&cmd);
  if (fi != nullptr)
  {
    m_InspiratoryCapacityFraction = fi->GetInspiratoryCapacityFraction().GetValue();
    m_ConsciousRespirationRemainingPeriod_s = fi->GetPeriod().GetValue(TimeUnit::s);
    m_ConsciousRespirationPeriod_s = m_ConsciousRespirationRemainingPeriod_s;

    //Pressure effects
    double TargetVolume_L = m_InitialFunctionalResidualCapacity_L + m_InitialInspiratoryCapacity_L * m_InspiratoryCapacityFraction;
    m_ConsciousEndPressure_cmH2O = VolumeToDriverPressure(TargetVolume_L);

    return;
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Left Side Needle Decompression
///
/// \param  dFlowResistance - Resistance value for air flow through the needle
///
/// \details
/// Used for left side needle decompression. this is an intervention (action) used to treat left 
/// side tension pneumothorax
//--------------------------------------------------------------------------------------------------
void Respiratory::DoLeftNeedleDecompression(double dFlowResistance)
{
  //Leak flow resistance that is scaled in proportion to Lung resistance, depending on severity
  double dScalingFactor = 0.5; //Tuning parameter to allow gas flow due to needle decompression using lung resistance as reference
  double dFlowResistanceLeftNeedle = dScalingFactor * dFlowResistance;
  m_LeftPleuralToEnvironment->GetNextResistance().SetValue(dFlowResistanceLeftNeedle, FlowResistanceUnit::cmH2O_s_Per_L);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Right Side Needle Decompression
///
/// \param  dFlowResistance - Resistance value for air flow through the needle
///
/// \details
/// Used for right side needle decompression. this is an intervention (action) used to treat right
/// side tension pneumothorax
//--------------------------------------------------------------------------------------------------
void Respiratory::DoRightNeedleDecompression(double dFlowResistance)
{
  //Leak flow resistance that is scaled in proportion to Lung resistance, depending on severity
  double dScalingFactor = 0.5; //Tuning parameter to allow gas flow due to needle decompression using lung resistance as reference
  double dFlowResistanceRightNeedle = dScalingFactor * dFlowResistance;
  m_RightPleuralToEnvironment->GetNextResistance().SetValue(dFlowResistanceRightNeedle, FlowResistanceUnit::cmH2O_s_Per_L);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Chronic Obstructive Pulmonary Disease (COPD)
///
/// \param  None
///
/// \return void
///
/// \details
/// This method handles the COPD condition. It determines if the patient has COPD, and if so,  
/// calculates a set of multipliers to model the various symptoms. This method should only
/// run once per simulation. Note that the bronchitis symptom (airway obstruction) is handled by
/// another method that is called every time-step in pre-process.
//--------------------------------------------------------------------------------------------------
void Respiratory::COPD()
{
  if (m_data.GetConditions().HasChronicObstructivePulmonaryDisease())
  {
    double dBronchitisSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease()->GetBronchitisSeverity().GetValue();
    double dEmphysemaSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease()->GetEmphysemaSeverity().GetValue();
    double dLeftLungFraction = 1.0;
    double dRightLungFraction = 1.0;

    // Calculate Pulmonary Capillary Resistance Multiplier based on severities
    double dMaxSeverity = MAX(dBronchitisSeverity, dEmphysemaSeverity);
    // Resistance is based on a a simple line fit where severity = 0 is resistance multiplier = 1.0
    // and severity = 0.6 is resistance multiplier = 2.0.
    double dSlopePulResist = 1.66666; // hard-coded slope for line
    double dPulmonaryResistanceMultiplier = 1.0 + (dMaxSeverity*dSlopePulResist);
    // Call UpdatePulmonaryCapillaryResistance
    UpdatePulmonaryCapillaryResistance(dPulmonaryResistanceMultiplier, dLeftLungFraction, dRightLungFraction);

    // Calculate Alveoli Compliance Multiplier based on severities
    // Compliance is based on a a simple line fit where severity = 0 is compliance multiplier = 1.0
    // and severity = 0.6 is compliance multiplier = 1.25
    double dSlopeAlveoCompl = 0.41; // hard-coded slope for line
    double dCompilanceScalingFactor = 1.0 + (dEmphysemaSeverity*dSlopeAlveoCompl);
    // Call UpdateAlveoliCompliance
    UpdateAlveoliCompliance(dCompilanceScalingFactor, dLeftLungFraction, dRightLungFraction);

    // Calculate Gas Diffusion Surface Area Multiplier based on severities
    // Resistance function: Base = 10, Min = 0.15, Max = 1.0 (decreasing with severity)
    double dGasDiffScalingFactor = GeneralMath::ResistanceFunction(10, 0.15, 1.0, dEmphysemaSeverity);
    // Call UpdateGasDiffusionSurfaceArea
    UpdateGasDiffusionSurfaceArea(dGasDiffScalingFactor, dLeftLungFraction, dRightLungFraction);
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Impaired Alveolar Exchange
///
/// \param  None
///
/// \return void
///
/// \details
/// This method handles the Impaired Alveolar Exchange condition. 
//--------------------------------------------------------------------------------------------------
void Respiratory::ImpairedAlveolarExchange()
{
	if (!m_data.GetConditions().HasImpairedAlveolarExchange())
	{
		return;
	}

	if (!m_data.GetConditions().GetImpairedAlveolarExchange()->HasImpairedSurfaceArea() && !m_data.GetConditions().GetImpairedAlveolarExchange()->HasImpairedFraction())
	{
		/// \error Fatal: The Impaired Alveolar Exchange action must include either a surface area of fraction.
		Fatal("The Impaired Alveolar Exchange action must include either a surface area of fraction.");
	}

	double alveoliDiffusionArea_cm2 = m_Patient->GetAlveoliSurfaceArea(AreaUnit::cm2);
	if (m_data.GetConditions().GetImpairedAlveolarExchange()->HasImpairedSurfaceArea() && m_data.GetConditions().GetImpairedAlveolarExchange()->HasImpairedFraction())
	{
		/// \error Error: The Impaired Alveolar Exchange action cannot have both an impaired surface area and impaired fraction defined. Defaulting to the surface area value.
		Warning("The Impaired Alveolar Exchange action cannot have both an impaired surface area and impaired fraction defined. Defaulting to the surface area value.");
		alveoliDiffusionArea_cm2 = alveoliDiffusionArea_cm2 - m_data.GetConditions().GetImpairedAlveolarExchange()->GetImpairedSurfaceArea(AreaUnit::cm2);
	}
	else if (m_data.GetConditions().GetImpairedAlveolarExchange()->HasImpairedSurfaceArea())
	{
		alveoliDiffusionArea_cm2 = alveoliDiffusionArea_cm2 - m_data.GetConditions().GetImpairedAlveolarExchange()->GetImpairedSurfaceArea(AreaUnit::cm2);
	}
	else if (m_data.GetConditions().GetImpairedAlveolarExchange()->HasImpairedFraction())
	{
		alveoliDiffusionArea_cm2 = (1.0 - m_data.GetConditions().GetImpairedAlveolarExchange()->GetImpairedFraction().GetValue()) * alveoliDiffusionArea_cm2;
	}

	//This is a conditions, so we change it pertinently
	m_Patient->GetAlveoliSurfaceArea().SetValue(alveoliDiffusionArea_cm2, AreaUnit::cm2);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Lobar Pneumonia
///
/// \param  None
///
/// \return void
///
/// \details
/// This method handles the lobar pneumonia condition. It determines if the patient has lobar pneumonia and  
/// calculates a set of multipliers to model the various symptoms. This method should only run once per simulation. 
//--------------------------------------------------------------------------------------------------
void Respiratory::LobarPneumonia()
{
  // Check to see if we have Lobar Pneumonia
  if (m_data.GetConditions().HasLobarPneumonia())
  {
    // Get Lobar Pneumonia Severity
    double dLobarPneumoniaSeverity = m_data.GetConditions().GetLobarPneumonia()->GetSeverity().GetValue();

    // Get lung fractions
    double dLeftLungFraction = m_data.GetConditions().GetLobarPneumonia()->GetLeftLungAffected().GetValue();
    double dRightLungFraction = m_data.GetConditions().GetLobarPneumonia()->GetRightLungAffected().GetValue();

    // Calculate Alveoli Compliance Multiplier based on severities
    // Compliance is based on a exponential fit where severity = 0 is compliance multiplier = 1.0
    // and severity = 1.0 is compliance multiplier = 0.01.  
    // Compliance function: Base = 10, Min = 0.01, Max = 1.0 (decreasing with severity)
    double dCompilanceScalingFactor = GeneralMath::ResistanceFunction(10, 1.000, 0.010, (1 - dLobarPneumoniaSeverity));
    // Call UpdateAlveoliCompliance
    UpdateAlveoliCompliance(dCompilanceScalingFactor, dLeftLungFraction, dRightLungFraction);

    // Calculate Gas Diffusion Surface Area Multiplier based on severities
    // Diffusion area function: Base = 10, Min = 0.15, Max = 1.0 (decreasing with severity)
    double dGasDiffScalingFactor = GeneralMath::ResistanceFunction(10, 0.15, 1.0, dLobarPneumoniaSeverity);
    // Call UpdateGasDiffusionSurfaceArea
    UpdateGasDiffusionSurfaceArea(dGasDiffScalingFactor, dLeftLungFraction, dRightLungFraction);
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Apply apnea action.
///
/// \param  None
///
/// \return void
///
/// \details
/// This reduces the respiratory driver pressure source amplitude by the percentage defined by the
/// action severity.
//--------------------------------------------------------------------------------------------------
void Respiratory::Apnea()
{
	if (m_PatientActions->HasApnea())
	{
		double apneaSeverity = m_PatientActions->GetApnea()->GetSeverity().GetValue();

		if (apneaSeverity == 1.0)
		{
			m_bNotBreathing = true;
		}

		//Just reduce the tidal volume by the percentage given
		m_DriverPressure_cmH2O = m_DefaultDrivePressure_cmH2O + (m_DriverPressure_cmH2O - m_DefaultDrivePressure_cmH2O) * (1 - apneaSeverity);
	}
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates key respiratory physiological parameters
///
/// \details
/// Calculates the respiration rate and tidal volume. For each breathing cycle, the tidal volume is calculated by identifying 
/// the peaks in the total lung volume.
/// The tidal volume of a particular breathing cycle is then calculated by taking the difference between the 
/// maximum and minimum values of the total lung volume. 
/// The respiration rate is calculated by measuring the duration for one complete breathing cycle and then 
/// converting the duration to the number of breaths per minute.
// The method also checks for bradypnea and tachypnea events and reports to the use.
//--------------------------------------------------------------------------------------------------
void Respiratory::CalculateVitalSigns()
{
  std::stringstream ss;
  //Total Respiratory Volume - this should not include the Pleural Space
  GetTotalLungVolume().Set(m_Lungs->GetVolume());

  //Record values each time-step
  double tracheaFlow_L_Per_s = m_MouthToCarina->GetNextFlow().GetValue(VolumePerTimeUnit::L_Per_s);
  GetInspiratoryFlow().SetValue(tracheaFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);
  GetExpiratoryFlow().SetValue(-tracheaFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);  

  double dEnvironmentPressure = m_Environment->GetPressure(PressureUnit::cmH2O);
  double dAlveolarPressure = (m_LeftAlveoli->GetNextPressure().GetValue(PressureUnit::cmH2O) + m_RightAlveoli->GetNextPressure().GetValue(PressureUnit::cmH2O)) / 2.0; //Average of L and R
  GetPulmonaryResistance().SetValue((dEnvironmentPressure - dAlveolarPressure) / tracheaFlow_L_Per_s, FlowResistanceUnit::cmH2O_s_Per_L);

  double dPleuralPressure_cmH2O = (m_LeftPleural->GetNextPressure().GetValue(PressureUnit::cmH2O) + m_RightPleural->GetNextPressure().GetValue(PressureUnit::cmH2O)) / 2.0; //Average of L and R
  GetTranspulmonaryPressure().SetValue(dAlveolarPressure - dPleuralPressure_cmH2O, PressureUnit::cmH2O);

  GetRespirationDriverPressure().Set(m_RespiratoryMuscle->GetNextPressure());
  GetRespirationMusclePressure().Set(m_RespiratoryMuscle->GetNextPressure());

  double avgAlveoliO2PP_mmHg = (m_LeftAlveoliO2->GetPartialPressure(PressureUnit::mmHg) + m_RightAlveoliO2->GetPartialPressure(PressureUnit::mmHg)) / 2.0;
  GetAlveolarArterialGradient().SetValue(avgAlveoliO2PP_mmHg - m_AortaO2->GetPartialPressure(PressureUnit::mmHg), PressureUnit::mmHg);

  /// \event Patient: Start of exhale/inhale
  if (m_Patient->IsEventActive(CDM::enumPatientEvent::StartOfExhale))
    m_Patient->SetEvent(CDM::enumPatientEvent::StartOfExhale, false, m_data.GetSimulationTime());
  if (m_Patient->IsEventActive(CDM::enumPatientEvent::StartOfInhale))
    m_Patient->SetEvent(CDM::enumPatientEvent::StartOfInhale, false, m_data.GetSimulationTime());

  //Record values at the breathing inflection points (i.e. switch between inhale and exhale)	
  // Temporal tolerance to avoid accidental entry in the the inhalation and exhalation code blocks 
  // dTimeTol = One fourth of the smallest possible period between breaths for a human. 60 breaths per
  // minute is a typical upper limit, so dTimeTol = 1 / (60 *4) = 0.004167 seconds.
  double dTimeTol = 0.004167;
  m_ElapsedBreathingCycleTime_min += m_dt_min;
  if (m_BreathingCycle && ((GetTotalLungVolume(VolumeUnit::L) - m_PreviousTotalLungVolume_L) > ZERO_APPROX)
    && (m_ElapsedBreathingCycleTime_min > dTimeTol))
  {
    m_Patient->SetEvent(CDM::enumPatientEvent::StartOfInhale, true, m_data.GetSimulationTime());
    // Calculate Respiration Rate and track time and update cycle flag
    double RespirationRate_Per_min = 1.0 / m_ElapsedBreathingCycleTime_min;
    GetRespirationRate().SetValue(RespirationRate_Per_min, FrequencyUnit::Per_min);

    double dExpirationTime = m_ElapsedBreathingCycleTime_min - m_BreathTimeExhale_min;
    double dInspirationTime = m_BreathTimeExhale_min;
    double dieratio = dInspirationTime / dExpirationTime;
    GetInspiratoryExpiratoryRatio().SetValue(dieratio);

    m_ElapsedBreathingCycleTime_min = 0.0;
    m_BreathingCycle = false;

    // Calculate the Tidal Volume from the last peak
    double dBottomBreathTotalVolume_L = GetTotalLungVolume(VolumeUnit::L);
    m_InstantaneousFunctionalResidualCapacity_L = dBottomBreathTotalVolume_L;
    double dBottomBreathAlveoliVolume_L = m_RightAlveoli->GetNextVolume().GetValue(VolumeUnit::L) + m_LeftAlveoli->GetNextVolume().GetValue(VolumeUnit::L);
    double dBottomBreathDeadSpaceVolume_L = m_RightDeadSpace->GetNextVolume().GetValue(VolumeUnit::L) + m_LeftDeadSpace->GetNextVolume().GetValue(VolumeUnit::L);
    double dBottomBreathPleuralPressure_cmH2O = dPleuralPressure_cmH2O;

    double TidalVolume_L = std::abs(m_TopBreathTotalVolume_L - dBottomBreathTotalVolume_L);
    double AlveoliDeltaVolume_L = std::abs(m_TopBreathAlveoliVolume_L - dBottomBreathAlveoliVolume_L);
    double DeadSpaceDeltaVolume_L = std::abs(m_TopBreathDeadSpaceVolume_L - dBottomBreathDeadSpaceVolume_L);
    double PleuralDeltaPressure_cmH2O = dBottomBreathPleuralPressure_cmH2O - m_TopBreathPleuralPressure_cmH2O;
   
    GetTidalVolume().SetValue(TidalVolume_L, VolumeUnit::L);

    // Set the End Tidal Concentration for our gases at the end of a respiration cycle
    for (SEGasSubstanceQuantity* subQ : m_Carina->GetSubstanceQuantities())
    {
      subQ->GetSubstance().GetEndTidalFraction().Set(subQ->GetVolumeFraction());
      subQ->GetSubstance().GetEndTidalPressure().Set(subQ->GetPartialPressure());
    }
    GetEndTidalCarbonDioxideFraction().Set(m_data.GetSubstances().GetCO2().GetEndTidalFraction());
    GetEndTidalCarbonDioxidePressure().Set(m_data.GetSubstances().GetCO2().GetEndTidalPressure());

    // Calculate Ventilations
    GetTotalAlveolarVentilation().SetValue(AlveoliDeltaVolume_L * RespirationRate_Per_min, VolumePerTimeUnit::L_Per_min);
    GetTotalPulmonaryVentilation().SetValue(TidalVolume_L * RespirationRate_Per_min, VolumePerTimeUnit::L_Per_min);
    GetTotalDeadSpaceVentilation().SetValue(DeadSpaceDeltaVolume_L * RespirationRate_Per_min, VolumePerTimeUnit::L_Per_min);
    GetSpecificVentilation().SetValue(TidalVolume_L / m_InstantaneousFunctionalResidualCapacity_L);

    // Calculate Total Circuit Values
    GetPulmonaryCompliance().SetValue(TidalVolume_L / PleuralDeltaPressure_cmH2O, FlowComplianceUnit::L_Per_cmH2O);
  }
  else if (!m_BreathingCycle
    && (m_PreviousTotalLungVolume_L - GetTotalLungVolume(VolumeUnit::L) > ZERO_APPROX)
    && (m_ElapsedBreathingCycleTime_min > dTimeTol))
  {
    m_Patient->SetEvent(CDM::enumPatientEvent::StartOfExhale, true, m_data.GetSimulationTime());
    m_BreathTimeExhale_min = m_ElapsedBreathingCycleTime_min;
    m_BreathingCycle = true;
    m_TopBreathTotalVolume_L = GetTotalLungVolume(VolumeUnit::L);
    m_TopBreathAlveoliVolume_L = m_RightAlveoli->GetNextVolume().GetValue(VolumeUnit::L) + m_LeftAlveoli->GetNextVolume().GetValue(VolumeUnit::L);
    m_TopBreathDeadSpaceVolume_L = m_RightDeadSpace->GetNextVolume().GetValue(VolumeUnit::L) + m_LeftDeadSpace->GetNextVolume().GetValue(VolumeUnit::L);
    m_TopBreathPleuralPressure_cmH2O = dPleuralPressure_cmH2O;

    //We want the peak Carina O2 value - this should be the inspired value
    GetCarricoIndex().SetValue(m_ArterialO2PartialPressure_mmHg / m_CarinaO2->GetVolumeFraction().GetValue(), PressureUnit::mmHg);

    if (m_data.GetState() > EngineState::InitialStabilization)
    {// Don't throw events if we are initializing
    //Check for ARDS - 3 different levels
      if (GetCarricoIndex().GetValue(PressureUnit::mmHg) < 100.0)
      {
        /// \event Patient: Severe ARDS: Carrico Index is below 100 mmHg
        m_Patient->SetEvent(CDM::enumPatientEvent::SevereAcuteRespiratoryDistress, true, m_data.GetSimulationTime());	/// \cite ranieriacute
        m_Patient->SetEvent(CDM::enumPatientEvent::ModerateAcuteRespiratoryDistress, false, m_data.GetSimulationTime());
        m_Patient->SetEvent(CDM::enumPatientEvent::MildAcuteRespiratoryDistress, false, m_data.GetSimulationTime());
      }
      else if (GetCarricoIndex().GetValue(PressureUnit::mmHg) < 200.0)
      {
        /// \event Patient: Moderate ARDS: Carrico Index is below 200 mmHg
        m_Patient->SetEvent(CDM::enumPatientEvent::ModerateAcuteRespiratoryDistress, true, m_data.GetSimulationTime());	/// \cite ranieriacute
        m_Patient->SetEvent(CDM::enumPatientEvent::SevereAcuteRespiratoryDistress, false, m_data.GetSimulationTime());
        m_Patient->SetEvent(CDM::enumPatientEvent::MildAcuteRespiratoryDistress, false, m_data.GetSimulationTime());
      }
      else if (GetCarricoIndex().GetValue(PressureUnit::mmHg) < 300.0)
      {
        /// \event Patient: Mild ARDS: Carrico Index is below 300 mmHg
        m_Patient->SetEvent(CDM::enumPatientEvent::MildAcuteRespiratoryDistress, true, m_data.GetSimulationTime());	/// \cite ranieriacute
        m_Patient->SetEvent(CDM::enumPatientEvent::SevereAcuteRespiratoryDistress, false, m_data.GetSimulationTime());
        m_Patient->SetEvent(CDM::enumPatientEvent::ModerateAcuteRespiratoryDistress, false, m_data.GetSimulationTime());
      }
      else
      {
        /// \event Patient: End ARDS: Carrico Index is above 305 mmHg
        m_Patient->SetEvent(CDM::enumPatientEvent::SevereAcuteRespiratoryDistress, false, m_data.GetSimulationTime());
        m_Patient->SetEvent(CDM::enumPatientEvent::ModerateAcuteRespiratoryDistress, false, m_data.GetSimulationTime());
        m_Patient->SetEvent(CDM::enumPatientEvent::MildAcuteRespiratoryDistress, false, m_data.GetSimulationTime());
      }
    }
  }

  //Zero out if waiting longer than 15 sec
  if (m_ElapsedBreathingCycleTime_min > 0.25)
  {
    GetRespirationRate().SetValue(0.0, FrequencyUnit::Per_min);
    GetTidalVolume().SetValue(0.0, VolumeUnit::L);
    GetTotalAlveolarVentilation().SetValue(0.0, VolumePerTimeUnit::L_Per_min);
    GetTotalPulmonaryVentilation().SetValue(0.0, VolumePerTimeUnit::L_Per_min);
  }

 /// \todo Move to blood chemistry
  // Although it is called respiratory acidosis/alkalosis, the detection and event setting is actually a part of the @ref BloodChemistrySystem
  // The terms "metabolic" and "respiratory" refer to the origin of the acid-base disturbance
  // The hydrogen ion concentration is a property of the blood
  // The events related to blood concentrations should be detected and set in blood chemistry.
  //Keep a running average of the pH
  m_BloodPHRunningAverage.Sample(m_data.GetBloodChemistry().GetBloodPH().GetValue());
  //Reset at start of cardiac cycle 
  if (m_Patient->IsEventActive(CDM::enumPatientEvent::StartOfCardiacCycle))
  {
    m_LastCardiacCycleBloodPH = m_BloodPHRunningAverage.Value();
    m_BloodPHRunningAverage.Reset();
  }

  if (m_data.GetState() > EngineState::InitialStabilization)
  {// Don't throw events if we are initializing

    //Bradypnea
    if (GetRespirationRate().GetValue(FrequencyUnit::Per_min) < 10)
    {
      /// \event Patient: Bradypnea: Respiration rate is below 10 breaths per minute
      /// The patient has bradypnea.
      m_Patient->SetEvent(CDM::enumPatientEvent::Bradypnea, true, m_data.GetSimulationTime());	/// \cite overdyk2007continuous
    }
    else if (GetRespirationRate().GetValue(FrequencyUnit::Per_min) >= 10.5)	// offset by .5 
    {
      /// \event Patient: End Bradypnea Event. The respiration rate has risen above 10. 
      /// The patient is no longer considered to have bradypnea.
      m_Patient->SetEvent(CDM::enumPatientEvent::Bradypnea, false, m_data.GetSimulationTime());
    }

    //Tachypnea
    if (GetRespirationRate().GetValue(FrequencyUnit::Per_min) > 20)
    {
      /// \event Patient: Tachypnea: Respiration rate is above 20 breaths per minute.
      /// The patient has tachypnea.
      m_Patient->SetEvent(CDM::enumPatientEvent::Tachypnea, true, m_data.GetSimulationTime());	/// \cite 
    }
    else if (GetRespirationRate().GetValue(FrequencyUnit::Per_min) <= 19.5) // offset by .5 
    {
      /// \event Patient: End Tachypnea Event. The respiration rate has fallen below 19.5. 
      /// The patient is no longer considered to have tachypnea.
      m_Patient->SetEvent(CDM::enumPatientEvent::Tachypnea, false, m_data.GetSimulationTime());
    }

    double highPh = 8.5;
    double lowPh = 6.5;   // \cite Edge2006AcidosisConscious
    //// Respiratory Acidosis
    if (m_LastCardiacCycleBloodPH < 7.35 && m_ArterialCO2PartialPressure_mmHg > 47.0)
    {
      /// \event Patient: Respiratory Acidosis: event is triggered when blood pH is below 7.36
      /// The patient has respiratory acidosis.
      m_Patient->SetEvent(CDM::enumPatientEvent::RespiratoryAcidosis, true, m_data.GetSimulationTime());

      /// \event Patient: arterial blood ph has dropped below 6.5.
      if (m_LastCardiacCycleBloodPH < lowPh)
      {
        ss << "Arterial blood pH is  " << m_LastCardiacCycleBloodPH << ". This is below 6.5, Patient is experiencing extreme respiratory Acidosis and is in an irreversible state.";
        Warning(ss);
        /// \irreversible Extreme respiratory Acidosis: blood pH below 6.5.
        m_Patient->SetEvent(CDM::enumPatientEvent::IrreversibleState, true, m_data.GetSimulationTime());
      }
    }
    else if (m_LastCardiacCycleBloodPH >= 7.38 && m_ArterialCO2PartialPressure_mmHg < 44.0)
    {
      /// \event Patient: End Respiratory Acidosis Event. The pH value has risen above 7.38. 
      /// The patient is no longer considered to have respiratory acidosis.
      m_Patient->SetEvent(CDM::enumPatientEvent::RespiratoryAcidosis, false, m_data.GetSimulationTime());
    }

    ////Respiratory Alkalosis
    if (m_LastCardiacCycleBloodPH > 7.45 && m_ArterialCO2PartialPressure_mmHg < 37.0)
    {
      /// \event Patient: Respiratory Alkalosis: event is triggered when blood pH is above 7.45
      /// The patient has respiratory alkalosis.
      m_Patient->SetEvent(CDM::enumPatientEvent::RespiratoryAlkalosis, true, m_data.GetSimulationTime());

      /// \event Patient: arterial blood ph has gotten above 8.5.
      if (m_LastCardiacCycleBloodPH > highPh)
      {
        ss << "Arterial blood pH is  " << m_LastCardiacCycleBloodPH << ". This is above 8.5, Patient is experiencing extreme respiratory Alkalosis and is in an irreversible state.";
        Warning(ss);
        /// \irreversible Extreme respiratory Alkalosis: blood pH above 8.5.
        m_Patient->SetEvent(CDM::enumPatientEvent::IrreversibleState, true, m_data.GetSimulationTime());
      }
    }
    else if (m_LastCardiacCycleBloodPH <= 7.43 && m_ArterialCO2PartialPressure_mmHg > 39.0)
    {
      /// \event Patient: End Respiratory Alkalosis Event. The pH value has has fallen below 7.45. 
      /// The patient is no longer considered to have respiratory alkalosis.
      m_Patient->SetEvent(CDM::enumPatientEvent::RespiratoryAlkalosis, false, m_data.GetSimulationTime());
    }
  }

  m_PreviousTotalLungVolume_L = GetTotalLungVolume(VolumeUnit::L);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Update obstructive (airway) resistance 
///
/// \return void
///
/// \details
/// This method takes a resistance scaling factor and lung percentages (left and right) as input
/// variables.  It updates the Carina to Dead Space path resistances in order to model airflow 
/// blockage through the bronchi and bronchioles.
//--------------------------------------------------------------------------------------------------
void Respiratory::UpdateObstructiveResistance()
{
	if ((!m_PatientActions->HasAsthmaAttack() && !m_data.GetConditions().HasChronicObstructivePulmonaryDisease())
		|| GetExpiratoryFlow(VolumePerTimeUnit::L_Per_s) < 0.0) // Only on exhalation
	{
		return;
	}
	
	double combinedResistanceScalingFactor = 1.0;	
	//Asthma attack on
	if (m_PatientActions->HasAsthmaAttack())
	{
		double dSeverity = m_PatientActions->GetAsthmaAttack()->GetSeverity().GetValue();
		// Resistance function: Base = 10, Min = 1.0, Max = 90.0 (increasing with severity)
		double dResistanceScalingFactor = GeneralMath::ResistanceFunction(10, 90, 1, dSeverity);
		combinedResistanceScalingFactor = dResistanceScalingFactor;
	}
	//COPD on
	if (m_data.GetConditions().HasChronicObstructivePulmonaryDisease())
	{
		double dSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease()->GetBronchitisSeverity().GetValue();
		// Resistance function: Base = 10, Min = 1.0, Max = 100.0 (increasing with severity)
		double dResistanceScalingFactor = GeneralMath::ResistanceFunction(10, 90, 1, dSeverity);
		combinedResistanceScalingFactor = MAX(combinedResistanceScalingFactor, dResistanceScalingFactor);
	}
	
	// Get the path resistances 
  double dLeftBronchiResistance = m_CarinaToLeftAnatomicDeadSpace->GetNextResistance().GetValue(FlowResistanceUnit::cmH2O_s_Per_L);
  double dRightBronchiResistance = m_CarinaToRightAnatomicDeadSpace->GetNextResistance().GetValue(FlowResistanceUnit::cmH2O_s_Per_L);

  dLeftBronchiResistance *= combinedResistanceScalingFactor;
  m_CarinaToLeftAnatomicDeadSpace->GetNextResistance().SetValue(dLeftBronchiResistance, FlowResistanceUnit::cmH2O_s_Per_L);
  dRightBronchiResistance *= combinedResistanceScalingFactor;
  m_CarinaToRightAnatomicDeadSpace->GetNextResistance().SetValue(dRightBronchiResistance, FlowResistanceUnit::cmH2O_s_Per_L);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Update the inspiratory-expiratory ratio
///
/// \return void
///
/// \details
/// The inspiratory-expiratory ratio is modified based on certain actions and conditions, as well as
/// local bronchodilation effects from aerosols.
//--------------------------------------------------------------------------------------------------
void Respiratory::UpdateIERatio()
{
	// Asthma, Chronic Bronchitis, and Lobar Pneumonia
	//  Adjust the inspiration/expiration ratio based on severity
	double combinedSeverity = 0.0;
	m_IEscaleFactor = 1.0;
	if (m_PatientActions->HasAsthmaAttack())
	{
		combinedSeverity = m_PatientActions->GetAsthmaAttack()->GetSeverity().GetValue();
	}
	if (m_data.GetConditions().HasChronicObstructivePulmonaryDisease())
	{
		double dBronchitisSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease()->GetBronchitisSeverity().GetValue();
		double dEmphysemaSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease()->GetEmphysemaSeverity().GetValue();
		combinedSeverity = MAX(combinedSeverity, dEmphysemaSeverity);
		combinedSeverity = MAX(combinedSeverity, dBronchitisSeverity);
	}
	if (m_data.GetConditions().HasLobarPneumonia())
	{
		double severity = m_data.GetConditions().GetLobarPneumonia()->GetSeverity().GetValue();
		// Get lung fractions
		double dRightLungFraction = m_data.GetConditions().GetLobarPneumonia()->GetRightLungAffected().GetValue();
		double dLeftLungFraction = m_data.GetConditions().GetLobarPneumonia()->GetLeftLungAffected().GetValue();

		// Get the right and left lung ratios
		double dRightLungRatio = m_Patient->GetRightLungRatio().GetValue();
		double dLeftLungRatio = 1.0 - dRightLungRatio;

		double dLP_ScaledSeverity = (0.75*severity) + (severity*dLeftLungFraction*dLeftLungRatio) + (severity*dRightLungFraction*dRightLungRatio);
		combinedSeverity = MAX(combinedSeverity, dLP_ScaledSeverity);
	}

	if (combinedSeverity > 0.0)
	{
		//When albuterol is administered, the bronchodilation also causes the IE ratio to correct itself
		m_IEscaleFactor = 1.0 - combinedSeverity;
		m_IEscaleFactor *= exp(7728.4 * m_AverageLocalTissueBronchodilationEffects);

		// IE scale factor is constrained to a minimum of 0.1 and a maximum 1.0. Lower than 0.1 causes simulation instability.
		// Greater than 1.0 is not possible for patients with these conditions
		m_IEscaleFactor = LIMIT(m_IEscaleFactor, 0.1, 1.0);
	}
}


//--------------------------------------------------------------------------------------------------
/// \brief
/// Update Alveoli Compliance 
///
/// \param  dComplianceScalingFactor	Alveoli compliance multiplier
/// \param  dLeftLungFraction			Fraction of left lung affected by change in surface area (0 to 1)
/// \param  dRightLungFraction			Fraction of right lung affected by change in surface area (0 to 1)
///
/// \return void
///
/// \details
/// This method takes a compliance scaling factor and lung percentages (left and right) as input
/// variables.  It updates the Alveoli to Pleural compliances in order to model changes in alveoli
/// compliance resulting from alveolus membrane damage or alveolus fluid content
//--------------------------------------------------------------------------------------------------
void Respiratory::UpdateAlveoliCompliance(double dCompilanceScalingFactor, double dLeftLungFraction, double dRightLungFraction)
{
  // Get path compliances
  double dRightAlveoliBaselineCompliance = m_RightAlveoliToRightPleuralConnection->GetComplianceBaseline().GetValue(FlowComplianceUnit::L_Per_cmH2O);
  double dLeftAlveoliBaselineCompliance = m_LeftAlveoliToLeftPleuralConnection->GetComplianceBaseline().GetValue(FlowComplianceUnit::L_Per_cmH2O);

  // Left lung alveoli
  dLeftAlveoliBaselineCompliance = (dLeftAlveoliBaselineCompliance*(1.0 - dLeftLungFraction)) + (dLeftAlveoliBaselineCompliance*dCompilanceScalingFactor*dLeftLungFraction);
  m_LeftAlveoliToLeftPleuralConnection->GetComplianceBaseline().SetValue(dLeftAlveoliBaselineCompliance, FlowComplianceUnit::L_Per_cmH2O);

  // Right lung alveoli
  dRightAlveoliBaselineCompliance = (dRightAlveoliBaselineCompliance*(1.0 - dRightLungFraction)) + (dRightAlveoliBaselineCompliance*dCompilanceScalingFactor*dRightLungFraction);
  m_RightAlveoliToRightPleuralConnection->GetComplianceBaseline().SetValue(dRightAlveoliBaselineCompliance, FlowComplianceUnit::L_Per_cmH2O);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Update Pulmonary Capillary Resistance 
///
/// \param  dResistanceScalingFactor	Pulmonary capillary resistance multiplier
/// \param  dLeftLungFraction			Fraction of left lung affected by change in surface area (0 to 1)
/// \param  dRightLungFraction			Fraction of right lung affected by change in surface area (0 to 1)
///
/// \return void
///
/// \details
/// This method takes a resistance scaling factor and lung fractions (left and right) as input
/// variables.  It updates the pulmonary capillary to pulmonary vein resistance in order to model the 
/// destruction of capillaries in the alveolus membrane.
//--------------------------------------------------------------------------------------------------
void Respiratory::UpdatePulmonaryCapillaryResistance(double dResistanceScalingFactor, double dLeftLungPercent, double dRightLungPercent)
{
  // Get path resistances
  double dRightPulmonaryCapillaryResistance = m_RightPulmonaryCapillary->GetResistanceBaseline().GetValue(FlowResistanceUnit::cmH2O_s_Per_L);
  double dLeftPulmonaryCapillaryResistance = m_LeftPulmonaryCapillary->GetResistanceBaseline().GetValue(FlowResistanceUnit::cmH2O_s_Per_L);

  dLeftPulmonaryCapillaryResistance = (dLeftPulmonaryCapillaryResistance*(1.0 - dLeftLungPercent)) + (dLeftPulmonaryCapillaryResistance*dResistanceScalingFactor*dLeftLungPercent);
  m_LeftPulmonaryCapillary->GetResistanceBaseline().SetValue(dLeftPulmonaryCapillaryResistance, FlowResistanceUnit::cmH2O_s_Per_L);
  dRightPulmonaryCapillaryResistance = (dRightPulmonaryCapillaryResistance*(1.0 - dRightLungPercent)) + (dRightPulmonaryCapillaryResistance*dResistanceScalingFactor*dRightLungPercent);
  m_RightPulmonaryCapillary->GetResistanceBaseline().SetValue(dRightPulmonaryCapillaryResistance, FlowResistanceUnit::cmH2O_s_Per_L);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Update Gas Diffusion Surface Area
///
/// \param  dFractionArea		Fractional change in gas diffusion surface area (0 to 1)
/// \param  dLeftLungFraction	Fraction of left lung affected by change in surface area (0 to 1)
/// \param  dRightLungFraction  Fraction of right lung affected by change in surface area (0 to 1)
///
/// \return void
///
/// \details
/// This method takes a percent valve and lung percentages (left and right) as input
/// variables.  It updates the gas diffusion surface area in the lungs in order to model the 
/// destruction of alveoli membranes and/or lung consolidation.
//--------------------------------------------------------------------------------------------------
void Respiratory::UpdateGasDiffusionSurfaceArea(double dFractionArea, double dLeftLungFraction, double dRightLungFraction)
{
  double AlveoliDiffusionArea_cm2 = m_Patient->GetAlveoliSurfaceArea(AreaUnit::cm2);

  // Get the right and left lung ratios
  double RightLungRatio = m_Patient->GetRightLungRatio().GetValue();
  double LeftLungRatio = 1.0 - RightLungRatio;

  // Calculate the surface area contributions for each lung
  double dRightContribution = RightLungRatio*((AlveoliDiffusionArea_cm2*(1.0 - dRightLungFraction)) + (AlveoliDiffusionArea_cm2*dFractionArea*dRightLungFraction));
  double dLeftContribution = LeftLungRatio*((AlveoliDiffusionArea_cm2*(1.0 - dLeftLungFraction)) + (AlveoliDiffusionArea_cm2*dFractionArea*dLeftLungFraction));

  // Calculate the total surface area
  AlveoliDiffusionArea_cm2 = dLeftContribution + dRightContribution;

  m_Patient->GetAlveoliSurfaceArea().SetValue(AlveoliDiffusionArea_cm2, AreaUnit::cm2);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Peak Driver Pressure Based on Target Volume 
///
/// \param  TargetVolume_L				Target tidal volume, including residual capacity (L)
///
/// \return Peak Driver Pressure		The driver pressure required to achieve target tidal volume	 (cm H2O)
///
/// \details
/// This method returns the peak driver pressure as a function of target tidal volume. The equation
/// used is constructed from respiratory unit test data (see RespiratoryDriverTest). 
//--------------------------------------------------------------------------------------------------
double Respiratory::VolumeToDriverPressure(double TargetVolume_L)
{
  return -0.3743 * pow(TargetVolume_L, 5.0) + 7.4105 * pow(TargetVolume_L, 4.0) - 57.076 * pow(TargetVolume_L, 3.0) + 214.11 * pow(TargetVolume_L, 2.0) - 404.97 * TargetVolume_L + 262.22;
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Populate the Pulmonary Function Test Assessment
///
/// \param  pft				pft data
///
/// \return success		    true, if everything worked out
//--------------------------------------------------------------------------------------------------
bool Respiratory::CalculatePulmonaryFunctionTest(SEPulmonaryFunctionTest& pft)
{
  pft.Reset();
  pft.GetExpiratoryReserveVolume().Set(m_Patient->GetExpiratoryReserveVolume());
  pft.GetFunctionalResidualCapacity().Set(m_Patient->GetFunctionalResidualCapacity());
  pft.GetInspiratoryCapacity().Set(m_Patient->GetInspiratoryCapacity());
  pft.GetInspiratoryReserveVolume().Set(m_Patient->GetInspiratoryReserveVolume());
  pft.GetResidualVolume().Set(m_Patient->GetResidualVolume());
  pft.GetTotalLungCapacity().Set(m_Patient->GetTotalLungCapacity());
  pft.GetVitalCapacity().Set(m_Patient->GetVitalCapacity());

  double rr_Hz = GetRespirationRate(FrequencyUnit::Hz);
  double tv_L = GetTidalVolume(VolumeUnit::L);
  double waveRespirationRate = rr_Hz;
  double pi = 3.14159265359;
  double magnitude = 0.5*tv_L;
  double superPosition = m_Patient->GetFunctionalResidualCapacity(VolumeUnit::L) + magnitude;
  double waveTime = 0.0;
  double currentTime = 0.0;

  double numPlotPoint = pft.GetNumberOfPlotPoints();
  SEFunctionVolumeVsTime& plot = pft.GetLungVolumePlot();
  plot.Invalidate();
  std::vector<double>& time = plot.GetTime();
  std::vector<double>& volume = plot.GetVolume();
  plot.SetTimeUnit(TimeUnit::s);
  plot.SetVolumeUnit(VolumeUnit::L);
  if (numPlotPoint > 0)
  {
    double dt = 60.0 / numPlotPoint;
    for (unsigned int i = 0; i < numPlotPoint; i++)
    {
      time.push_back(currentTime);
      if (waveTime >= 12.0 && waveTime < 13.0)
      {//Period dilation due to forced inspiration
        magnitude = 0.5*tv_L + pft.GetInspiratoryReserveVolume().GetValue(VolumeUnit::L);
        waveRespirationRate = rr_Hz
          / (1 + pft.GetExpiratoryReserveVolume().GetValue(VolumeUnit::L) / tv_L);
      }
      else if (waveTime >= 13.0 && waveTime < 14.0)
      {//Period dilation due to forced expiration
        magnitude = 0.5*tv_L + pft.GetExpiratoryReserveVolume().GetValue(VolumeUnit::L);
        waveRespirationRate = rr_Hz
          / (1 + pft.GetExpiratoryReserveVolume().GetValue(VolumeUnit::L) / tv_L);
      }
      else
      {
        magnitude = 0.5*tv_L;
        waveRespirationRate = rr_Hz;
      }

      volume.push_back(magnitude*sin(pi*waveTime) + superPosition);
      waveTime += 2.0*waveRespirationRate*dt;
      currentTime += dt;
    }
  }

  /// \todo Insert more PFT calculations here
  /*
  All Known:
  VitalCapacity=TotaLungCapacity-ResidualVolume
  InspiratoryCapacity=TotaLungCapacity-FunctionalResidualCapacity
  InspiratoryReserveVolume=InspiratoryCapacity-TidalVolume
  ExpiratoryReserveVolume=FunctionalResidualCapacity-ResidualVolume

  Need to define:
  ForcedVitalCapacity				|	Increase the pressure driver in the lungs
  ForcedExpiratoryVolume			|	for forced PFT constituents?
  ForcedExpiratoryFlow			|
  PeakExpiratoryFlow				|
  MaximumVoluntaryVentilation		|	The same pressure driver increase should be used here. Calculate as a time integration of lung volume over 1 min.

  SlowVitalCapacity				|

  */
  return true;
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Run the circuit standalone for a little to get it into a good starting state 
/// - keep volume fractions constant
//--------------------------------------------------------------------------------------------------
void Respiratory::TuneCircuit()
{
	SEFluidCircuit& RespiratoryCircuit = m_data.GetCircuits().GetRespiratoryCircuit();
	//This is precharge everything to help stabilization
	//Hold it steady for 5s to make sure the compliances are fully charged
	for (unsigned int loops = 0; loops < static_cast<unsigned int>(5.0 / m_dt_s); loops++)
	{
		//Set the starting/default driver pressure
		m_DriverPressurePath->GetNextPressureSource().SetValue(m_DefaultDrivePressure_cmH2O, PressureUnit::cmH2O);
		m_Calculator.Process(RespiratoryCircuit, m_dt_s);
		m_Calculator.PostProcess(RespiratoryCircuit);
	}

	//Make sure the new volumes are accounted for with all the substance stuff
	//Keep the same volume fraction originally initialized
	SEGasCompartmentGraph* RespiratoryGraph = &m_data.GetCompartments().GetRespiratoryGraph();
	for (SEGasCompartment* compartment : RespiratoryGraph->GetCompartments())
	{
		if (compartment->HasVolume())
			compartment->Balance(BalanceGasBy::VolumeFraction);
	}
}

