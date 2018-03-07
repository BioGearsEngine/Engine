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

#include "BioGearsEngineTest.h"
#include "Systems/Tissue.h"

#include "Serializer.h"
#include "substance/SESubstanceManager.h"
#include "substance/SESubstance.h"
#include "utils/TimingProfile.h"
#include "utils/testing/SETestReport.h"
#include "utils/testing/SETestCase.h"
#include "utils/testing/SETestSuite.h"
#include "compartment/SECompartmentManager.h"
#include "compartment/fluid/SELiquidCompartment.h"
#include "compartment/tissue/SETissueCompartment.h"
#include "compartment/fluid/SEGasCompartment.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarMassPerAmount.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarAmountPerVolume.h"
#include "properties/SEScalarMassPerAreaTime.h"

#include "utils/DataTrack.h"

void BioGearsEngineTest::DistributeMass(SETestSuite& testSuite)
{
  TimingProfile timer;
  BioGears bg(testSuite.GetLogger());
  Tissue& tsu = (Tissue&)bg.GetTissue();
  // First test case  
  SETestCase& testCase = testSuite.CreateTestCase();
  testCase.SetName("DistributeMassToHierarchy");
  timer.Start("Test");

  //                L0C0
  //               /    \
  //           L1C0      L1C1
  //           /  \      /  \
  //        L2C0  L2C1 L2C2 L2C3 <-- Only these cmpts have data

  
  SELiquidCompartment& L0C0 = bg.GetCompartments().CreateLiquidCompartment("L0C0");
  SELiquidCompartment& L1C0 = bg.GetCompartments().CreateLiquidCompartment("L1C0");
  SELiquidCompartment& L1C1 = bg.GetCompartments().CreateLiquidCompartment("L1C1");
  SELiquidCompartment& L2C0 = bg.GetCompartments().CreateLiquidCompartment("L2C0");
  SELiquidCompartment& L2C1 = bg.GetCompartments().CreateLiquidCompartment("L2C1");
  SELiquidCompartment& L2C2 = bg.GetCompartments().CreateLiquidCompartment("L2C2");
  SELiquidCompartment& L2C3 = bg.GetCompartments().CreateLiquidCompartment("L2C3");

  // Build up the hierarchy
  L0C0.AddChild(L1C0);
  L0C0.AddChild(L1C1);
  L1C0.AddChild(L2C0);
  L1C0.AddChild(L2C1);
  L1C1.AddChild(L2C2);
  L1C1.AddChild(L2C3);
  bg.GetCompartments().StateChange();// Call this, AFTER YOU SET UP YOUR HIERARCHY, to ensure all parent compartments have their link data

  SESubstance* sub = bg.GetSubstances().GetSubstance("Oxygen");
  bg.GetSubstances().AddActiveSubstance(*sub);

  double L2C0_mL = 10;
  double L2C0_g = 10;
  double L2C1_mL = 20;
  double L2C1_g = 20;
  double L2C2_mL = 40;
  double L2C2_g = 40;
  double L2C3_mL = 80;
  double L2C3_g = 80;
  double total_mL = L2C0_mL + L2C1_mL + L2C2_mL + L2C3_mL;

  L2C0.GetVolume().SetValue(L2C0_mL, VolumeUnit::mL);
  L2C0.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C0_g, MassUnit::g);
  L2C1.GetVolume().SetValue(L2C1_mL, VolumeUnit::mL);
  L2C1.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C1_g, MassUnit::g);
  L2C2.GetVolume().SetValue(L2C2_mL, VolumeUnit::mL);
  L2C2.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C2_g, MassUnit::g);
  L2C3.GetVolume().SetValue(L2C3_mL, VolumeUnit::mL);
  L2C3.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C3_g, MassUnit::g);

  double delta_g = -20;

  double expected_L2C0_g = L2C0_g + (delta_g * L2C0_mL / total_mL);
  double expected_L2C1_g = L2C1_g + (delta_g * L2C1_mL / total_mL);
  double expected_L2C2_g = L2C2_g + (delta_g * L2C2_mL / total_mL);
  double expected_L2C3_g = L2C3_g + (delta_g * L2C3_mL / total_mL);

  tsu.DistributeMassbyVolumeWeighted(L0C0, *sub, delta_g, MassUnit::g);

  double L2C0_calc = L2C0.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C0_calc - expected_L2C0_g) > ZERO_APPROX)
    testCase.AddFailure("DistributeMassbyVolumeWeighted L2C0 mass is not expected mass");

  double L2C1_calc = L2C1.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C1_calc - expected_L2C1_g) > ZERO_APPROX)
	  testCase.AddFailure("DistributeMassbyVolumeWeighted L2C1 mass is not expected mass");

  double L2C2_calc = L2C2.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C2_calc - expected_L2C2_g) > ZERO_APPROX)
	  testCase.AddFailure("DistributeMassbyVolumeWeighted L2C2 mass is not expected mass");

  double L2C3_calc = L2C3.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C3_calc - expected_L2C3_g) > ZERO_APPROX)
	  testCase.AddFailure("DistributeMassbyVolumeWeighted L2C3 mass is not expected mass");

  // Log how long our test took
  testCase.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

  // Second test case  
  SETestCase& testCase2 = testSuite.CreateTestCase();
  testCase2.SetName("DistributeMassbyMassToHierarchy");
  timer.Start("Test");

  L2C0_mL = 10;
  L2C0_g = 10;
  L2C1_mL = 20;
  L2C1_g = 20;
  L2C2_mL = 40;
  L2C2_g = 40;
  L2C3_mL = 80;
  L2C3_g = 80;
  double total_g = L2C0_g + L2C1_g + L2C2_g + L2C3_g;

  L2C0.GetVolume().SetValue(L2C0_mL, VolumeUnit::mL);
  L2C0.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C0_g, MassUnit::g);
  L2C1.GetVolume().SetValue(L2C1_mL, VolumeUnit::mL);
  L2C1.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C1_g, MassUnit::g);
  L2C2.GetVolume().SetValue(L2C2_mL, VolumeUnit::mL);
  L2C2.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C2_g, MassUnit::g);
  L2C3.GetVolume().SetValue(L2C3_mL, VolumeUnit::mL);
  L2C3.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C3_g, MassUnit::g);

  delta_g = -20;

  expected_L2C0_g = L2C0_g + (delta_g * L2C0_g / total_g);
  expected_L2C1_g = L2C1_g + (delta_g * L2C1_g / total_g);
  expected_L2C2_g = L2C2_g + (delta_g * L2C2_g / total_g);
  expected_L2C3_g = L2C3_g + (delta_g * L2C3_g / total_g);

  tsu.DistributeMassbyMassWeighted(L0C0, *sub, delta_g, MassUnit::g);

  L2C0_calc = L2C0.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C0_calc - expected_L2C0_g) > ZERO_APPROX)
    testCase2.AddFailure("DistributeMassbyMassWeighted L2C0 mass is not expected mass");

  L2C1_calc = L2C1.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C1_calc - expected_L2C1_g) > ZERO_APPROX)
    testCase2.AddFailure("DistributeMassbyMassWeighted L2C1 mass is not expected mass");

  L2C2_calc = L2C2.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C2_calc - expected_L2C2_g) > ZERO_APPROX)
    testCase2.AddFailure("DistributeMassbyMassWeighted L2C2 mass is not expected mass");

  L2C3_calc = L2C3.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C3_calc - expected_L2C3_g) > ZERO_APPROX)
    testCase2.AddFailure("DistributeMassbyMassWeighted L2C3 mass is not expected mass");

  // Third test case
  // Second test case  
  SETestCase& testCase3 = testSuite.CreateTestCase();
  testCase3.SetName("DistributeMassbyMassToHierarchy_tooMuch");
  timer.Start("Test");

  L2C0_mL = 10;
  L2C0_g = 10;
  L2C1_mL = 20;
  L2C1_g = 20;
  L2C2_mL = 40;
  L2C2_g = 40;
  L2C3_mL = 80;
  L2C3_g = 80;
  total_g = L2C0_g + L2C1_g + L2C2_g + L2C3_g;

  L2C0.GetVolume().SetValue(L2C0_mL, VolumeUnit::mL);
  L2C0.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C0_g, MassUnit::g);
  L2C1.GetVolume().SetValue(L2C1_mL, VolumeUnit::mL);
  L2C1.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C1_g, MassUnit::g);
  L2C2.GetVolume().SetValue(L2C2_mL, VolumeUnit::mL);
  L2C2.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C2_g, MassUnit::g);
  L2C3.GetVolume().SetValue(L2C3_mL, VolumeUnit::mL);
  L2C3.GetSubstanceQuantity(*sub)->GetMass().SetValue(L2C3_g, MassUnit::g);

  delta_g = -2000;

  expected_L2C0_g = 0.;
  expected_L2C1_g = 0.;
  expected_L2C2_g = 0.;
  expected_L2C3_g = 0.;

  tsu.DistributeMassbyMassWeighted(L0C0, *sub, delta_g, MassUnit::g);

  L2C0_calc = L2C0.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C0_calc - expected_L2C0_g) > ZERO_APPROX)
    testCase3.AddFailure("L2C0 mass is not expected mass - Expecting zero");

  L2C1_calc = L2C1.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C1_calc - expected_L2C1_g) > ZERO_APPROX)
    testCase3.AddFailure("L2C1 mass is not expected mass - Expecting zero");

  L2C2_calc = L2C2.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C2_calc - expected_L2C2_g) > ZERO_APPROX)
    testCase3.AddFailure("L2C2 mass is not expected mass - Expecting zero");

  L2C3_calc = L2C3.GetSubstanceQuantity(*sub)->GetMass(MassUnit::g);
  if (std::abs(L2C3_calc - expected_L2C3_g) > ZERO_APPROX)
    testCase3.AddFailure("L2C3 mass is not expected mass - Expecting zero");


}

void BioGearsEngineTest::PerfusionLimitedDiffusionTest(SETestSuite& testSuite)
{
  BioGears bg(testSuite.GetLogger());
  Tissue& tsu = (Tissue&)bg.GetTissue();
  TimingProfile timer;
  double timestep_s = 1. / 90.;
  SESubstance* sub = bg.GetSubstances().GetSubstance("Ketamine");
  double bFlow_mL_Per_s = 2.0;
  double PartitionCoeff = 1.52201;
  double matrixVolume_mL = 2.0;
  double tissueMass_ug = 3.;
  double bVol_mL = 10.;
  double bConc_ug_Per_mL = 10.;

  bg.GetSubstances().AddActiveSubstance(*sub);

  // First test case  
  SETestCase& testCase1 = testSuite.CreateTestCase();
  testCase1.SetName("PerfusionLimitedDiffusionTest");
  timer.Start("Test");

  SETissueCompartment& tissue = bg.GetCompartments().CreateTissueCompartment(BGE::TissueCompartment::Bone);
  SELiquidCompartment& extracellular = bg.GetCompartments().CreateLiquidCompartment(BGE::ExtravascularCompartment::BoneExtracellular);
  SELiquidCompartment& intracellular = bg.GetCompartments().CreateLiquidCompartment(BGE::ExtravascularCompartment::BoneIntracellular);
  tissue.GetMatrixVolume().SetValue(matrixVolume_mL,VolumeUnit::mL);
  intracellular.GetSubstanceQuantity(*sub)->GetMass().SetValue(tissueMass_ug, MassUnit::ug);
  intracellular.GetVolume().SetValue(1.0, VolumeUnit::mL); //Need fluid volume to balance. 
  extracellular.GetVolume().SetValue(1.0, VolumeUnit::mL); //Should be fine, right? We don't validate and tissue concentrations.
  
  SELiquidCompartment& vascular = bg.GetCompartments().CreateLiquidCompartment(BGE::VascularCompartment::Bone);
  vascular.GetVolume().SetValue(bVol_mL, VolumeUnit::mL);
  vascular.GetSubstanceQuantity(*sub)->GetConcentration().SetValue(bConc_ug_Per_mL, MassPerVolumeUnit::ug_Per_mL);
  vascular.GetSubstanceQuantity(*sub)->Balance(BalanceLiquidBy::Concentration);
  intracellular.GetSubstanceQuantity(*sub)->Balance(BalanceLiquidBy::Mass);

  SELiquidCompartmentLink& flow = bg.GetCompartments().CreateLiquidLink(intracellular, vascular, "ExtravascularExchange");
  flow.GetFlow().SetValue(bFlow_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
  extracellular.AddLink(flow);
  vascular.AddLink(flow);

  bg.GetCompartments().StateChange();

  double rtnMassInc_ug = tsu.PerfusionLimitedDiffusion(tissue,vascular,*sub, PartitionCoeff,timestep_s);

  // Check
  double MassIncrement_ug = bFlow_mL_Per_s * timestep_s * ((bConc_ug_Per_mL) - ((tissueMass_ug/ matrixVolume_mL) / PartitionCoeff));
  if (std::abs(rtnMassInc_ug - MassIncrement_ug) > 1.e-10)
  {
    testCase1.AddFailure("PerfusionLimitedDiffusion returned unexpected mass increment.");
  }

  // Log how long our test took
  testCase1.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

  // Subsequent test cases
  //timer.Start("Test");
}

void BioGearsEngineTest::AlveolarOxygenDiffusionTest(const std::string& rptDirectory)
{
  //This test examines diffusion of O2 from a gas compartment (like lungs) to a liquid compartment
  //The gas compartment is set to resemble atmospheric conditions, and the liquid compartment is oxygen-poor
  //We should expect to see oxygen diffuse into the liquid compartment
  BioGears bg(m_Logger);
  Tissue& tsu = (Tissue&)bg.GetTissue();

  double timestep_s = 1.0 / 90;
  SESubstance* O2 = bg.GetSubstances().GetSubstance("Oxygen");
  SESubstance* N2 = bg.GetSubstances().GetSubstance("Nitrogen");
  bg.GetSubstances().AddActiveSubstance(*O2);
  bg.GetSubstances().AddActiveSubstance(*N2);

  SELiquidCompartment& lcmpt = bg.GetCompartments().CreateLiquidCompartment("lcmpt");
  SEGasCompartment& gcmpt = bg.GetCompartments().CreateGasCompartment("gcmpt");

  DataTrack trk1;
  std::string rptFile = rptDirectory + "\\AlveolarOxygenDiffusionTest.txt";
  double time = 0.0;

  double liquidVol_mL = 1000.0;
  double lMass_g = .001;  //oxygen-poor

  gcmpt.GetVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
  lcmpt.GetVolume().SetValue(liquidVol_mL, VolumeUnit::mL);

  gcmpt.GetSubstanceQuantity(*O2)->GetVolumeFraction().SetValue(.2);
  gcmpt.GetSubstanceQuantity(*N2)->GetVolumeFraction().SetValue(.8);
  gcmpt.GetPressure().SetValue(760, PressureUnit::mmHg);
  lcmpt.GetSubstanceQuantity(*O2)->GetMass().SetValue(lMass_g, MassUnit::g);
  lcmpt.GetSubstanceQuantity(*N2)->GetMass().SetValue(lMass_g, MassUnit::g);
  gcmpt.Balance(BalanceGasBy::VolumeFraction);
  lcmpt.Balance(BalanceLiquidBy::Mass);

  double diffusingCapacityO2_mL_Per_s_mmHg = .33;

  double l_O2_ug = lcmpt.GetSubstanceQuantity(*O2)->GetMass().GetValue(MassUnit::ug);
  double lconc = lcmpt.GetSubstanceQuantity(*O2)->GetConcentration().GetValue(MassPerVolumeUnit::ug_Per_L);
  double lppO2_mmHg = lcmpt.GetSubstanceQuantity(*O2)->GetPartialPressure(PressureUnit::mmHg);
  double gppO2_mmHg = gcmpt.GetSubstanceQuantity(*O2)->GetPartialPressure(PressureUnit::mmHg);

  trk1.Track("LiquidCompartmentOxygenMass_ug", time, l_O2_ug);
  trk1.Track("LiquidCompartmentOxygenConc_ug_Per_L", time, lconc);
  trk1.Track("LiquidCompartmentPO2_mmHg", time, lcmpt.GetSubstanceQuantity(*O2)->GetPartialPressure(PressureUnit::mmHg));
  trk1.Track("GasCompartmentPO2_mmHg", time, gcmpt.GetSubstanceQuantity(*O2)->GetPartialPressure(PressureUnit::mmHg));

  for (int i = 0; i < 180; i++)
  {
    tsu.AlveolarPartialPressureGradientDiffusion(gcmpt, lcmpt, *O2, diffusingCapacityO2_mL_Per_s_mmHg, timestep_s);
    gcmpt.Balance(BalanceGasBy::VolumeFraction);
    time += timestep_s;
    l_O2_ug = lcmpt.GetSubstanceQuantity(*O2)->GetMass().GetValue(MassUnit::ug);
    lconc = lcmpt.GetSubstanceQuantity(*O2)->GetConcentration().GetValue(MassPerVolumeUnit::ug_Per_L);
    lppO2_mmHg = lcmpt.GetSubstanceQuantity(*O2)->GetPartialPressure(PressureUnit::mmHg);
    gppO2_mmHg = gcmpt.GetSubstanceQuantity(*O2)->GetPartialPressure(PressureUnit::mmHg);
    trk1.Track("LiquidCompartmentOxygenMass_ug", time, l_O2_ug);
    trk1.Track("LiquidCompartmentOxygenConc_ug_Per_L", time, lconc);
    trk1.Track("LiquidCompartmentPO2_mmHg", time, lcmpt.GetSubstanceQuantity(*O2)->GetPartialPressure(PressureUnit::mmHg));
    trk1.Track("GasCompartmentPO2_mmHg", time, gcmpt.GetSubstanceQuantity(*O2)->GetPartialPressure(PressureUnit::mmHg));
  }

  // Write data to file
  trk1.WriteTrackToFile(rptFile.c_str());
}

void BioGearsEngineTest::AlveolarCarbonDioxideDiffusionTest(const std::string& rptDirectory)
{
  //Second test case: High liquid compartment CO2 concentration with a lower gas compartment CO2 concentration
  //We should expect CO2 to diffuse out of the liquid compartment into the gas compartment
  BioGears bg(m_Logger);
  Tissue& tsu = (Tissue&)bg.GetTissue();

  DataTrack trk2;
  std::string rptFile = rptDirectory + "\\AlveolarCarbonDioxideDiffusionTest.txt";
  
  double timestep_s = 1.0 / 90;

  SESubstance* CO2 = bg.GetSubstances().GetSubstance("CarbonDioxide");
  SESubstance* N2 = bg.GetSubstances().GetSubstance("Nitrogen");
  bg.GetSubstances().AddActiveSubstance(*CO2);
  bg.GetSubstances().AddActiveSubstance(*N2);

  SELiquidCompartment& lcmpt2 = bg.GetCompartments().CreateLiquidCompartment("lcmpt2");
  SEGasCompartment& gcmpt2 = bg.GetCompartments().CreateGasCompartment("gcmpt2");

  double time = 0.0;

  double liquidVol_mL = 1000.0;
  double lMass_g = .04;  //high amount of CO2

  gcmpt2.GetVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
  lcmpt2.GetVolume().SetValue(liquidVol_mL, VolumeUnit::mL);

  gcmpt2.GetSubstanceQuantity(*CO2)->GetVolumeFraction().SetValue(.0004); //atmospheric CO2 is .04%
  gcmpt2.GetSubstanceQuantity(*N2)->GetVolumeFraction().SetValue(1 - .0004);
  gcmpt2.GetPressure().SetValue(760, PressureUnit::mmHg);
  lcmpt2.GetSubstanceQuantity(*CO2)->GetMass().SetValue(lMass_g, MassUnit::g);
  lcmpt2.GetSubstanceQuantity(*N2)->GetMass().SetValue(lMass_g, MassUnit::g);
  gcmpt2.Balance(BalanceGasBy::VolumeFraction);
  lcmpt2.Balance(BalanceLiquidBy::Mass);

  double l_CO2_ug = lcmpt2.GetSubstanceQuantity(*CO2)->GetMass().GetValue(MassUnit::ug);
  double lconc = lcmpt2.GetSubstanceQuantity(*CO2)->GetConcentration().GetValue(MassPerVolumeUnit::ug_Per_L);
  double lppO2_mmHg = lcmpt2.GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg);
  double gppO2_mmHg = gcmpt2.GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg);
  double diffusingCapacityO2_mL_Per_s_mmHg = .33;

  trk2.Track("LiquidCompartmentCO2Mass_ug", time, l_CO2_ug);
  trk2.Track("LiquidCompartmentCO2Conc_ug_Per_L", time, lconc);
  trk2.Track("LiquidCompartmentPCO2_mmHg", time, lcmpt2.GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg));
  trk2.Track("GasCompartmentPCO2_mmHg", time, gcmpt2.GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg));

  for (int i = 0; i < 180; i++)
  {
    tsu.AlveolarPartialPressureGradientDiffusion(gcmpt2, lcmpt2, *CO2, diffusingCapacityO2_mL_Per_s_mmHg, timestep_s);
    gcmpt2.Balance(BalanceGasBy::VolumeFraction);
    time += timestep_s;
    l_CO2_ug = lcmpt2.GetSubstanceQuantity(*CO2)->GetMass().GetValue(MassUnit::ug);
    lconc = lcmpt2.GetSubstanceQuantity(*CO2)->GetConcentration().GetValue(MassPerVolumeUnit::ug_Per_L);
    lppO2_mmHg = lcmpt2.GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg);
    gppO2_mmHg = gcmpt2.GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg);
    trk2.Track("LiquidCompartmentCO2Mass_ug", time, l_CO2_ug);
    trk2.Track("LiquidCompartmentCO2Conc_ug_Per_L", time, lconc);
    trk2.Track("LiquidCompartmentPCO2_mmHg", time, lcmpt2.GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg));
    trk2.Track("GasCompartmentPCO2_mmHg", time, gcmpt2.GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg));
  }

  // Write data to file
  trk2.WriteTrackToFile(rptFile.c_str());

}

void BioGearsEngineTest::InstantPlusSimpleDiffusionTest(const std::string& rptDirectory)
{
  // Second test - cmpt2 and cmpt4 are connected by instant diffusion, cmpt2 and cmpt1 by simple and cmpt2 and cmpt3 by simple
  //        cmpt1 <-> cmpt2 <-> cmpt3
  //                    |
  //                  cmpt4
  // Expect cmpt2 and cmpt4 to quickly equilibrate, while the others take more time
  BioGears bg(m_Logger);
  Tissue& tsu = (Tissue&)bg.GetTissue();
  double timestep_s = 1.0 / 90;
  SESubstance* o2 = bg.GetSubstances().GetSubstance("Oxygen");
  bg.GetSubstances().AddActiveSubstance(*o2);
  SELiquidCompartment& cmpt1 = bg.GetCompartments().CreateLiquidCompartment("cmpt1");
  SELiquidCompartment& cmpt2 = bg.GetCompartments().CreateLiquidCompartment("cmpt2");
  SELiquidCompartment& cmpt3 = bg.GetCompartments().CreateLiquidCompartment("cmpt3");
  SELiquidCompartment& cmpt4 = bg.GetCompartments().CreateLiquidCompartment("cmpt4");

  DataTrack trk;
  std::string rptFile = rptDirectory + "\\InstantPlusSimpleDiffusionTest.txt";
  double time = 0.0;

  double permeabilityCoefficient21_mL_Per_s = 5.0;
  double permeabilityCoefficient23_mL_Per_s = 500.0;

  cmpt1.GetSubstanceQuantity(*o2)->GetConcentration().SetValue(100.0, MassPerVolumeUnit::g_Per_L);
  cmpt2.GetSubstanceQuantity(*o2)->GetConcentration().SetValue(100.0, MassPerVolumeUnit::g_Per_L);
  cmpt3.GetSubstanceQuantity(*o2)->GetConcentration().SetValue(100.0, MassPerVolumeUnit::g_Per_L);
  cmpt4.GetSubstanceQuantity(*o2)->GetConcentration().SetValue(0.0, MassPerVolumeUnit::g_Per_L);
  cmpt1.GetVolume().SetValue(1.0, VolumeUnit::L);
  cmpt2.GetVolume().SetValue(5.0, VolumeUnit::L);
  cmpt3.GetVolume().SetValue(20.0, VolumeUnit::L);
  cmpt4.GetVolume().SetValue(10.0, VolumeUnit::L);
  cmpt1.Balance(BalanceLiquidBy::Concentration);
  cmpt2.Balance(BalanceLiquidBy::Concentration);
  cmpt3.Balance(BalanceLiquidBy::Concentration);
  cmpt4.Balance(BalanceLiquidBy::Concentration);

  trk.Track("cmpt1Mass_g", time, cmpt1.GetSubstanceQuantity(*o2)->GetMass(MassUnit::g));
  trk.Track("cmpt2Mass_g", time, cmpt2.GetSubstanceQuantity(*o2)->GetMass(MassUnit::g));
  trk.Track("cmpt3Mass_g", time, cmpt3.GetSubstanceQuantity(*o2)->GetMass(MassUnit::g));
  trk.Track("cmpt4Mass_g", time, cmpt4.GetSubstanceQuantity(*o2)->GetMass(MassUnit::g));
  trk.Track("cmpt1Conc_g_Per_L", time, cmpt1.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L));
  trk.Track("cmpt2Conc_g_Per_L", time, cmpt2.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L));
  trk.Track("cmpt3Conc_g_Per_L", time, cmpt3.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L));
  trk.Track("cmpt4Conc_g_Per_L", time, cmpt4.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L));

  for (int i = 0; i < 3600; i++)
  {
    tsu.MoveMassByInstantDiffusion(cmpt2, cmpt4, *o2, timestep_s);
    tsu.MoveMassBySimpleDiffusion(cmpt2, cmpt1, *o2, permeabilityCoefficient21_mL_Per_s, timestep_s);
    tsu.MoveMassBySimpleDiffusion(cmpt2, cmpt3, *o2, permeabilityCoefficient23_mL_Per_s, timestep_s);
    cmpt1.Balance(BalanceLiquidBy::Mass);
    cmpt2.Balance(BalanceLiquidBy::Mass);
    cmpt3.Balance(BalanceLiquidBy::Mass);
    cmpt4.Balance(BalanceLiquidBy::Mass);
    time += timestep_s;
    trk.Track("cmpt1Mass_g", time, cmpt1.GetSubstanceQuantity(*o2)->GetMass(MassUnit::g));
    trk.Track("cmpt2Mass_g", time, cmpt2.GetSubstanceQuantity(*o2)->GetMass(MassUnit::g));
    trk.Track("cmpt3Mass_g", time, cmpt3.GetSubstanceQuantity(*o2)->GetMass(MassUnit::g));
    trk.Track("cmpt4Mass_g", time, cmpt4.GetSubstanceQuantity(*o2)->GetMass(MassUnit::g));
    trk.Track("cmpt1Conc_g_Per_L", time, cmpt1.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L));
    trk.Track("cmpt2Conc_g_Per_L", time, cmpt2.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L));
    trk.Track("cmpt3Conc_g_Per_L", time, cmpt3.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L));
    trk.Track("cmpt4Conc_g_Per_L", time, cmpt4.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L));
  }

  trk.WriteTrackToFile(rptFile.c_str());
}

void BioGearsEngineTest::InstantDiffusionTest(SETestSuite& testSuite)
{
  TimingProfile timer;
  BioGears bg(testSuite.GetLogger());
  Tissue& tsu = (Tissue&)bg.GetTissue();
  double timestep_s = 1.0 / 90;
  SESubstance* o2 = bg.GetSubstances().GetSubstance("Oxygen");
  bg.GetSubstances().AddActiveSubstance(*o2);
  SELiquidCompartment& cmpt1 = bg.GetCompartments().CreateLiquidCompartment("cmpt1");
  SELiquidCompartment& cmpt2 = bg.GetCompartments().CreateLiquidCompartment("cmpt2");
  SELiquidCompartment& cmpt3 = bg.GetCompartments().CreateLiquidCompartment("cmpt3");
  SELiquidCompartment& cmpt4 = bg.GetCompartments().CreateLiquidCompartment("cmpt4");

  // First test - simple two compartment instant diffusion test
  timer.Start("Test");
  SETestCase& testCase1 = testSuite.CreateTestCase();
  testCase1.SetName("InstantDiffusionTest");

  cmpt1.GetSubstanceQuantity(*o2)->GetConcentration().SetValue(100.0, MassPerVolumeUnit::g_Per_L);
  cmpt2.GetSubstanceQuantity(*o2)->GetConcentration().SetValue(0.0, MassPerVolumeUnit::g_Per_L);

  cmpt1.GetVolume().SetValue(1.0, VolumeUnit::L);
  cmpt2.GetVolume().SetValue(10.0, VolumeUnit::L);
  cmpt1.Balance(BalanceLiquidBy::Concentration);
  cmpt2.Balance(BalanceLiquidBy::Concentration);

  tsu.MoveMassByInstantDiffusion(cmpt1, cmpt2, *o2, timestep_s);
  cmpt1.Balance(BalanceLiquidBy::Mass);
  cmpt2.Balance(BalanceLiquidBy::Mass);

  if (std::abs(cmpt1.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L) - cmpt2.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L)) > ZERO_APPROX)
    testCase1.AddFailure("Unequal concentrations after instant diffusion.");
  if (std::abs(cmpt1.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L) - 9.0909090909090909) > ZERO_APPROX)
    testCase1.AddFailure("Compartment 1 concentration after instant diffusion not as expected.");
  if (std::abs(cmpt2.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_L) - 9.0909090909090909) > ZERO_APPROX)
    testCase1.AddFailure("Compartment 2 concentration after instant diffusion not as expected.");

  testCase1.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

  /// \todo This methodology assumes binary compartments. It works in prototype for multiple compartment instant diffusion, but needs to be tested beyond two compartments.
}

void BioGearsEngineTest::SimpleDiffusionTwoCompartmentTest(const std::string& rptDirectory)
{
  // Second test case -- 
  // Tests the diffusion between two tissue liquid compartments
  // Calculated permeability coefficient
  BioGears bg(m_Logger);
  Tissue& tsu = (Tissue&)bg.GetTissue();
  double timestep_s = 1.0 / 90;
  SESubstance* o2 = bg.GetSubstances().GetSubstance("Oxygen");
  bg.GetSubstances().AddActiveSubstance(*o2);
  SETissueCompartment& tissue = bg.GetCompartments().CreateTissueCompartment("Tissue");
  SELiquidCompartment& cmpt1_IC = bg.GetCompartments().CreateLiquidCompartment("cmpt1_IC");
  SELiquidCompartment& cmpt2_EC = bg.GetCompartments().CreateLiquidCompartment("cmpt2_EC");

  DataTrack trk2;
  std::string rptFile = rptDirectory + "\\SimpleDiffusionTwoCompartmentTest.txt";
  double time = 0.0;

  double ecVol_mL = 40.0;
  double icVol_mL = 30.0;
  double ecMass_g = 1.0;
  double icMass_g = 20.0;

  cmpt2_EC.GetVolume().SetValue(ecVol_mL, VolumeUnit::mL);
  cmpt1_IC.GetVolume().SetValue(icVol_mL, VolumeUnit::mL);
  cmpt2_EC.GetSubstanceQuantity(*o2)->GetMass().SetValue(ecMass_g, MassUnit::g);
  cmpt1_IC.GetSubstanceQuantity(*o2)->GetMass().SetValue(icMass_g, MassUnit::g);
  cmpt2_EC.Balance(BalanceLiquidBy::Mass);
  cmpt1_IC.Balance(BalanceLiquidBy::Mass);

  tissue.GetTotalMass().SetValue(500.0, MassUnit::g);

  double molarMass = o2->GetMolarMass().GetValue(MassPerAmountUnit::g_Per_mol);
  double molecularRadius = 0.0348*pow(molarMass, 0.4175);
  double permeabilityCoefficient_mL_Per_s_hg;
  if (molecularRadius > 1.0)
  {
    permeabilityCoefficient_mL_Per_s_hg = 0.0287*pow(molecularRadius, -2.920);
  }
  else
  {
    permeabilityCoefficient_mL_Per_s_hg = 0.0184*pow(molecularRadius, -1.223);
  }

  // The tissue mass baseline is a constant property of the tissue - values can be found in the ICRP and other sources
  // We use the tissue mass as a stand-in for surface area, follow the lead of Renkin and Curry
  // Here are the rules for the different types of compartments
  // Vascular to tissue (in BioGears it is always extracellular, but it doesn't matter)
  // The mass is the tissue compartment mass
  // Tissue to tissue
  // The mass is always the smaller of the two (the smaller area will be the max area for diffusion)
  double permeabilityCoefficient_mL_Per_s = permeabilityCoefficient_mL_Per_s_hg / 100.0 * tissue.GetTotalMass(MassUnit::g);

  trk2.Track("ExtracellularMass_g", time, cmpt2_EC.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g));
  trk2.Track("IntracellularMass_g", time, cmpt1_IC.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g));
  trk2.Track("ExtracellularConc_g_Per_mL", time, cmpt2_EC.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk2.Track("IntracellularConc_g_Per_mL", time, cmpt1_IC.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));

  for (int i = 0; i < 3600; i++)
  {
    tsu.MoveMassBySimpleDiffusion(cmpt1_IC, cmpt2_EC, *o2, permeabilityCoefficient_mL_Per_s, timestep_s);
    cmpt2_EC.Balance(BalanceLiquidBy::Mass);
    cmpt1_IC.Balance(BalanceLiquidBy::Mass);
    time += timestep_s;
    ecMass_g = cmpt2_EC.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g);
    icMass_g = cmpt1_IC.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g);
    double ecConc_g_Per_mL = cmpt2_EC.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL);
    double icConc_g_Per_mL = cmpt1_IC.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL);
    trk2.Track("ExtracellularMass_g", time, ecMass_g);
    trk2.Track("IntracellularMass_g", time, icMass_g);
    trk2.Track("ExtracellularConc_g_Per_mL", time, ecConc_g_Per_mL);
    trk2.Track("IntracellularConc_g_Per_mL", time, icConc_g_Per_mL);
  }

  trk2.WriteTrackToFile(rptFile.c_str());
}

void BioGearsEngineTest::SimpleDiffusionFourCompartmentTest(const std::string& rptDirectory)
{
  // Now we will add two more compartments and test two things
  // First, that the concentration does not change within a time slice
  // Second, that the concentrations equilibrate appropriately
  //              / cmpt1
  //     cmpt4 <->  cmpt2
  //              \ cmpt3                 
  // Artificial permeability coefficient
  BioGears bg(m_Logger);
  Tissue& tsu = (Tissue&)bg.GetTissue();
  double timestep_s = 1.0 / 90;
  SESubstance* o2 = bg.GetSubstances().GetSubstance("Oxygen");
  bg.GetSubstances().AddActiveSubstance(*o2);
  SETissueCompartment& tissue = bg.GetCompartments().CreateTissueCompartment("Tissue");
  SELiquidCompartment& cmpt1_IC = bg.GetCompartments().CreateLiquidCompartment("cmpt1_IC");
  SELiquidCompartment& cmpt2_EC = bg.GetCompartments().CreateLiquidCompartment("cmpt2_EC");
  SELiquidCompartment& cmpt3_LQ = bg.GetCompartments().CreateLiquidCompartment("cmpt3_LQ");
  SELiquidCompartment& cmpt4_LQ = bg.GetCompartments().CreateLiquidCompartment("cmpt4_LQ");

  DataTrack trk3;
  std::string rptFile = rptDirectory + "\\SimpleDiffusionFourCompartmentTest.txt";
  double time = 0.0;

  // Initialize
  double v3Mass_g = 10.0;
  double v4Mass_g = 0.0;
  double v3Vol_mL = 20.0;
  double v4Vol_mL = 50.0;
  double ecVol_mL = 10.0;
  double icVol_mL = 50.0;
  double ecMass_g = 2.5;
  double icMass_g = 2.0;
  cmpt2_EC.GetVolume().SetValue(ecVol_mL, VolumeUnit::mL);
  cmpt1_IC.GetVolume().SetValue(icVol_mL, VolumeUnit::mL);
  cmpt3_LQ.GetVolume().SetValue(v3Vol_mL, VolumeUnit::mL);
  cmpt4_LQ.GetVolume().SetValue(v4Vol_mL, VolumeUnit::mL);
  cmpt2_EC.GetSubstanceQuantity(*o2)->GetMass().SetValue(ecMass_g, MassUnit::g);
  cmpt1_IC.GetSubstanceQuantity(*o2)->GetMass().SetValue(icMass_g, MassUnit::g);
  cmpt3_LQ.GetSubstanceQuantity(*o2)->GetMass().SetValue(v3Mass_g, MassUnit::g);
  cmpt4_LQ.GetSubstanceQuantity(*o2)->GetMass().SetValue(v4Mass_g, MassUnit::g);
  cmpt4_LQ.Balance(BalanceLiquidBy::Mass);
  cmpt3_LQ.Balance(BalanceLiquidBy::Mass);
  cmpt2_EC.Balance(BalanceLiquidBy::Mass);
  cmpt1_IC.Balance(BalanceLiquidBy::Mass);

  // Make it a little faster for this test
  double permeabilityCoefficient_mL_Per_s = 20.0;

  tsu.MoveMassBySimpleDiffusion(cmpt1_IC, cmpt4_LQ, *o2, permeabilityCoefficient_mL_Per_s, timestep_s);
  tsu.MoveMassBySimpleDiffusion(cmpt2_EC, cmpt4_LQ, *o2, permeabilityCoefficient_mL_Per_s, timestep_s);
  tsu.MoveMassBySimpleDiffusion(cmpt3_LQ, cmpt4_LQ, *o2, permeabilityCoefficient_mL_Per_s, timestep_s);

  double v3CalculatedConcentration_g_Per_mL = cmpt3_LQ.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_mL);
  double v4CalculatedConcentration_g_Per_mL = cmpt4_LQ.GetSubstanceQuantity(*o2)->GetConcentration(MassPerVolumeUnit::g_Per_mL);
  double v3ExpectedConcentration_g_Per_mL = (v3Mass_g / v3Vol_mL);
  double v4ExpectedConcentration_g_Per_mL = (v4Mass_g / v4Vol_mL);

  // Ok, now balance
  cmpt4_LQ.Balance(BalanceLiquidBy::Mass);
  cmpt3_LQ.Balance(BalanceLiquidBy::Mass);
  cmpt2_EC.Balance(BalanceLiquidBy::Mass);
  cmpt1_IC.Balance(BalanceLiquidBy::Mass);
  trk3.Track("cmpt1mass_g", time, cmpt1_IC.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g));
  trk3.Track("cmpt2mass_g", time, cmpt2_EC.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g));
  trk3.Track("cmpt3mass_g", time, cmpt3_LQ.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g));
  trk3.Track("cmpt4mass_g", time, cmpt4_LQ.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g));
  trk3.Track("cmpt1conc_g_Per_mL", time, cmpt1_IC.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk3.Track("cmpt2conc_g_Per_mL", time, cmpt2_EC.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk3.Track("cmpt3conc_g_Per_mL", time, cmpt3_LQ.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk3.Track("cmpt4conc_g_Per_mL", time, cmpt4_LQ.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));

  for (int i = 0; i < 3600; i++)
  {
    tsu.MoveMassBySimpleDiffusion(cmpt1_IC, cmpt4_LQ, *o2, permeabilityCoefficient_mL_Per_s, timestep_s);
    tsu.MoveMassBySimpleDiffusion(cmpt2_EC, cmpt4_LQ, *o2, permeabilityCoefficient_mL_Per_s, timestep_s);
    tsu.MoveMassBySimpleDiffusion(cmpt3_LQ, cmpt4_LQ, *o2, permeabilityCoefficient_mL_Per_s, timestep_s);
    cmpt4_LQ.Balance(BalanceLiquidBy::Mass);
    cmpt3_LQ.Balance(BalanceLiquidBy::Mass);
    cmpt2_EC.Balance(BalanceLiquidBy::Mass);
    cmpt1_IC.Balance(BalanceLiquidBy::Mass);
    time += timestep_s;
    trk3.Track("cmpt1mass_g", time, cmpt1_IC.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g));
    trk3.Track("cmpt2mass_g", time, cmpt2_EC.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g));
    trk3.Track("cmpt3mass_g", time, cmpt3_LQ.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g));
    trk3.Track("cmpt4mass_g", time, cmpt4_LQ.GetSubstanceQuantity(*o2)->GetMass().GetValue(MassUnit::g));
    trk3.Track("cmpt1conc_g_Per_mL", time, cmpt1_IC.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk3.Track("cmpt2conc_g_Per_mL", time, cmpt2_EC.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk3.Track("cmpt3conc_g_Per_mL", time, cmpt3_LQ.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk3.Track("cmpt4conc_g_Per_mL", time, cmpt4_LQ.GetSubstanceQuantity(*o2)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  }

  // Write data to file
  trk3.WriteTrackToFile(rptFile.c_str());
}

void BioGearsEngineTest::SimpleDiffusionHierarchyTest(const std::string& rptDirectory)
{
  // Tests diffusion with distribution for hierarchical compartments
  //                                       L0C0        <---->        M0C0
  //                                      /    \                    /  |  \
  //                                  L1C0      L1C1            M1C0  M1C1 M1C2  <--Only these cmpts have data
  //                                  /  \      /  \
  // Only these cmpts have data--> L2C0  L2C1 L2C2 L2C3
  // Artificial permeability coefficient

  BioGears bg(m_Logger);
  Tissue& tsu = (Tissue&)bg.GetTissue();
  double timestep_s = 1.0 / 90;
  SESubstance* sub = bg.GetSubstances().GetSubstance("Desflurane");
  bg.GetSubstances().AddActiveSubstance(*sub);

  DataTrack trk4;
  std::string rptFile = rptDirectory + "\\SimpleDiffusionHierarchyTest.txt";
  double time = 0.0;

  SELiquidCompartment& L0C0 = bg.GetCompartments().CreateLiquidCompartment("L0C0");
  SELiquidCompartment& L1C0 = bg.GetCompartments().CreateLiquidCompartment("L1C0");
  SELiquidCompartment& L1C1 = bg.GetCompartments().CreateLiquidCompartment("L1C1");
  SELiquidCompartment& L2C0 = bg.GetCompartments().CreateLiquidCompartment("L2C0");
  SELiquidCompartment& L2C1 = bg.GetCompartments().CreateLiquidCompartment("L2C1");
  SELiquidCompartment& L2C2 = bg.GetCompartments().CreateLiquidCompartment("L2C2");
  SELiquidCompartment& L2C3 = bg.GetCompartments().CreateLiquidCompartment("L2C3");
  SELiquidCompartment& M0C0 = bg.GetCompartments().CreateLiquidCompartment("M0C0");
  SELiquidCompartment& M1C0 = bg.GetCompartments().CreateLiquidCompartment("M1C0");
  SELiquidCompartment& M1C1 = bg.GetCompartments().CreateLiquidCompartment("M1C1");
  SELiquidCompartment& M1C2 = bg.GetCompartments().CreateLiquidCompartment("M1C2");

  // Build up the hierarchy
  L0C0.AddChild(L1C0);
  L0C0.AddChild(L1C1);
  L1C0.AddChild(L2C0);
  L1C0.AddChild(L2C1);
  L1C1.AddChild(L2C2);
  L1C1.AddChild(L2C3);
  M0C0.AddChild(M1C0);
  M0C0.AddChild(M1C1);
  M0C0.AddChild(M1C2);
  bg.GetCompartments().StateChange();// Call this, AFTER YOU SET UP YOUR HIERARCHY, to ensure all parent compartments have their link data

  double Lvol_mL = 10.0;
  double Mvol_mL = 8.0;
  double M1C2mol_fraction = 0.01;
  L2C0.GetVolume().SetValue(Lvol_mL, VolumeUnit::mL);
  L2C1.GetVolume().SetValue(Lvol_mL, VolumeUnit::mL);
  L2C2.GetVolume().SetValue(Lvol_mL, VolumeUnit::mL);
  L2C3.GetVolume().SetValue(Lvol_mL, VolumeUnit::mL);
  M1C0.GetVolume().SetValue(Mvol_mL, VolumeUnit::mL);
  M1C1.GetVolume().SetValue(Mvol_mL, VolumeUnit::mL);
  M1C2.GetVolume().SetValue(Mvol_mL, VolumeUnit::mL);

  double Lmolarity_mmol_Per_mL = 10.0;
  double Mmolarity_mmol_Per_mL = 0.0;
  L2C0.GetSubstanceQuantity(*sub)->GetMolarity().SetValue(Lmolarity_mmol_Per_mL, AmountPerVolumeUnit::mmol_Per_mL);
  L2C1.GetSubstanceQuantity(*sub)->GetMolarity().SetValue(0.5*Lmolarity_mmol_Per_mL, AmountPerVolumeUnit::mmol_Per_mL);
  L2C2.GetSubstanceQuantity(*sub)->GetMolarity().SetValue(2.0*Lmolarity_mmol_Per_mL, AmountPerVolumeUnit::mmol_Per_mL);
  L2C3.GetSubstanceQuantity(*sub)->GetMolarity().SetValue(1.5*Lmolarity_mmol_Per_mL, AmountPerVolumeUnit::mmol_Per_mL);
  M1C0.GetSubstanceQuantity(*sub)->GetMolarity().SetValue(Mmolarity_mmol_Per_mL, AmountPerVolumeUnit::mmol_Per_mL);
  M1C1.GetSubstanceQuantity(*sub)->GetMolarity().SetValue(Mmolarity_mmol_Per_mL, AmountPerVolumeUnit::mmol_Per_mL);
  M1C2.GetSubstanceQuantity(*sub)->GetMolarity().SetValue(Mmolarity_mmol_Per_mL*M1C2mol_fraction, AmountPerVolumeUnit::mmol_Per_mL); // He has less moles

  L2C0.Balance(BalanceLiquidBy::Molarity);
  L2C1.Balance(BalanceLiquidBy::Molarity);
  L2C2.Balance(BalanceLiquidBy::Molarity);
  L2C3.Balance(BalanceLiquidBy::Molarity);
  M1C0.Balance(BalanceLiquidBy::Molarity);
  M1C1.Balance(BalanceLiquidBy::Molarity);
  M1C2.Balance(BalanceLiquidBy::Molarity);

  double permeabilityCoefficient_mL_Per_s = 2.0;

  trk4.Track("L0C0_mass_g", time, L0C0.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
  trk4.Track("L1C0_mass_g", time, L1C0.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
  trk4.Track("L1C1_mass_g", time, L1C1.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
  trk4.Track("L2C0_mass_g", time, L2C0.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
  trk4.Track("L2C1_mass_g", time, L2C1.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
  trk4.Track("L2C2_mass_g", time, L2C2.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
  trk4.Track("L2C3_mass_g", time, L2C3.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
  trk4.Track("M0C0_mass_g", time, M0C0.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
  trk4.Track("M1C0_mass_g", time, M1C0.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
  trk4.Track("M1C1_mass_g", time, M1C1.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
  trk4.Track("M1C2_mass_g", time, M1C2.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
  trk4.Track("L0C0_conc_g_Per_mL", time, L0C0.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk4.Track("L1C0_conc_g_Per_mL", time, L1C0.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk4.Track("L1C1_conc_g_Per_mL", time, L1C1.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk4.Track("L2C0_conc_g_Per_mL", time, L2C0.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk4.Track("L2C1_conc_g_Per_mL", time, L2C1.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk4.Track("L2C2_conc_g_Per_mL", time, L2C2.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk4.Track("L2C3_conc_g_Per_mL", time, L2C3.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk4.Track("M0C0_conc_g_Per_mL", time, M0C0.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk4.Track("M1C0_conc_g_Per_mL", time, M1C0.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk4.Track("M1C1_conc_g_Per_mL", time, M1C1.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  trk4.Track("M1C2_conc_g_Per_mL", time, M1C2.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));

  for (int i = 0; i < 3600; i++)
  {
    tsu.MoveMassBySimpleDiffusion(L0C0, M0C0, *sub, permeabilityCoefficient_mL_Per_s, timestep_s);
    L2C0.Balance(BalanceLiquidBy::Mass);
    L2C1.Balance(BalanceLiquidBy::Mass);
    L2C2.Balance(BalanceLiquidBy::Mass);
    L2C3.Balance(BalanceLiquidBy::Mass);
    M1C0.Balance(BalanceLiquidBy::Mass);
    M1C1.Balance(BalanceLiquidBy::Mass);
    M1C2.Balance(BalanceLiquidBy::Mass);
    time += timestep_s;
    trk4.Track("L0C0_mass_g", time, L0C0.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
    trk4.Track("L1C0_mass_g", time, L1C0.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
    trk4.Track("L1C1_mass_g", time, L1C1.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
    trk4.Track("L2C0_mass_g", time, L2C0.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
    trk4.Track("L2C1_mass_g", time, L2C1.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
    trk4.Track("L2C2_mass_g", time, L2C2.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
    trk4.Track("L2C3_mass_g", time, L2C3.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
    trk4.Track("M0C0_mass_g", time, M0C0.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
    trk4.Track("M1C0_mass_g", time, M1C0.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
    trk4.Track("M1C1_mass_g", time, M1C1.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
    trk4.Track("M1C2_mass_g", time, M1C2.GetSubstanceQuantity(*sub)->GetMass().GetValue(MassUnit::g));
    trk4.Track("L0C0_conc_g_Per_mL", time, L0C0.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk4.Track("L1C0_conc_g_Per_mL", time, L1C0.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk4.Track("L1C1_conc_g_Per_mL", time, L1C1.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk4.Track("L2C0_conc_g_Per_mL", time, L2C0.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk4.Track("L2C1_conc_g_Per_mL", time, L2C1.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk4.Track("L2C2_conc_g_Per_mL", time, L2C2.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk4.Track("L2C3_conc_g_Per_mL", time, L2C3.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk4.Track("M0C0_conc_g_Per_mL", time, M0C0.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk4.Track("M1C0_conc_g_Per_mL", time, M1C0.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk4.Track("M1C1_conc_g_Per_mL", time, M1C1.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
    trk4.Track("M1C2_conc_g_Per_mL", time, M1C2.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL));
  }

  trk4.WriteTrackToFile(rptFile.c_str());
}

void BioGearsEngineTest::FacilitatedDiffusionTest(const std::string& rptDirectory)
{
  BioGears bg(m_Logger);
  Tissue& tsu = (Tissue&)bg.GetTissue();
  double timestep_s = 1.0 / 90;
  SESubstance* sub = bg.GetSubstances().GetSubstance("Glucose");
  bg.GetSubstances().AddActiveSubstance(*sub);
  SETissueCompartment& tissue = bg.GetCompartments().CreateTissueCompartment("Tissue");  
  SELiquidCompartment& tissueExtracellular = bg.GetCompartments().CreateLiquidCompartment("Extracellular");
  SELiquidCompartment& vascular = bg.GetCompartments().CreateLiquidCompartment("Vascular");

  DataTrack trk1;
  std::string   rptFile = rptDirectory + "\\FacilitatedDiffusionTest.txt";
  double time = 0.0;

  tissueExtracellular.GetVolume().SetValue(45.0, VolumeUnit::mL);
  vascular.GetVolume().SetValue(100.0, VolumeUnit::mL);
  tissueExtracellular.GetSubstanceQuantity(*sub)->GetMass().SetValue(2.9, MassUnit::g);
  vascular.GetSubstanceQuantity(*sub)->GetMass().SetValue(5.0, MassUnit::g);
  tissueExtracellular.Balance(BalanceLiquidBy::Mass);
  vascular.Balance(BalanceLiquidBy::Mass);
  
  double capCoverage_cm2 = 90.0; /// \todo Relate capillary coverage to tissue mass.
  double maximumMassFlux = sub->GetMaximumDiffusionFlux(MassPerAreaTimeUnit::g_Per_cm2_s);
  double combinedCoefficient_g_Per_s = maximumMassFlux*capCoverage_cm2;

  double vascularConc_g_Per_mL = vascular.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL);
  double ECconc_g_Per_mL = tissueExtracellular.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL);
  trk1.Track("VascularConc_g_Per_mL", time, vascularConc_g_Per_mL);
  trk1.Track("ExtracellularConc_g_Per_mL", time, ECconc_g_Per_mL);

  for (int i = 0; i < 180; i++)
  {
    tsu.MoveMassByFacilitatedDiffusion(vascular, tissueExtracellular, *sub, combinedCoefficient_g_Per_s, timestep_s);
    vascular.Balance(BalanceLiquidBy::Mass);
    tissueExtracellular.Balance(BalanceLiquidBy::Mass);
    time += timestep_s;
    vascularConc_g_Per_mL = vascular.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL);
    ECconc_g_Per_mL = tissueExtracellular.GetSubstanceQuantity(*sub)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_mL);
    trk1.Track("VascularConc_g_Per_mL", time, vascularConc_g_Per_mL);
    trk1.Track("ExtracellularConc_g_Per_mL", time, ECconc_g_Per_mL);
  }
  
  trk1.WriteTrackToFile(rptFile.c_str());
}

void BioGearsEngineTest::ActiveIonTransportTest(const std::string& rptDirectory)
{  
//----Constants for each test-------------------------------------------	
  BioGears bg(m_Logger);
  Tissue& tsu = (Tissue&)bg.GetTissue();
  double timestep_s = 0.02;

  //Note: Do not call "Load Substance Directory".  This is already called in the BioGears constructor.
  bg.GetCompartments().StateChange();


  SESubstance* Na = &bg.GetSubstances().GetSodium();
  SESubstance* K = &bg.GetSubstances().GetPotassium();
  SESubstance* Cl = &bg.GetSubstances().GetChloride();
  SESubstance* Ca = &bg.GetSubstances().GetCalcium();

  bg.GetSubstances().AddActiveSubstance(*Na);
  bg.GetSubstances().AddActiveSubstance(*K);
  bg.GetSubstances().AddActiveSubstance(*Cl);
  bg.GetSubstances().AddActiveSubstance(*Ca);

  //Make a compartment to mimic adipose that has high extra:intra volume ratio
  SETissueCompartment& adiposeTis = bg.GetCompartments().CreateTissueCompartment("Adipose");
  SELiquidCompartment& adiposeEC = bg.GetCompartments().CreateLiquidCompartment("AdiposeExtra");
  SELiquidCompartment& adiposeIC = bg.GetCompartments().CreateLiquidCompartment("AdiposeIntra");
  SELiquidCompartment& adiposeVas = bg.GetCompartments().CreateLiquidCompartment("AdiposeVascular");
  //Make a compartment to mimic liver that has low extra:intra volume ratio
  SETissueCompartment& liverTis = bg.GetCompartments().CreateTissueCompartment("Liver");
  SELiquidCompartment& liverEC = bg.GetCompartments().CreateLiquidCompartment("LiverExtra");
  SELiquidCompartment& liverIC = bg.GetCompartments().CreateLiquidCompartment("LiverIntra");
  SELiquidCompartment& liverVas = bg.GetCompartments().CreateLiquidCompartment("LiverVascular");


  //Substance Molar Masses
  double NaMM = Na->GetMolarMass(MassPerAmountUnit::g_Per_mol);
  double KMM = K->GetMolarMass(MassPerAmountUnit::g_Per_mol);
  double ClMM = Cl->GetMolarMass(MassPerAmountUnit::g_Per_mol);
  double CaMM = Ca->GetMolarMass(MassPerAmountUnit::g_Per_mol);



  DataTrack tracker;

  double adiposeECVol_L = 2.13;
  double adiposeICVol_L = 0.27;
  double adiposeMass_kg = 0.25;
  double liverECVol_L = 0.3;
  double liverICVol_L = 1.03;
  double liverMass_kg = 1.0;
  
  //Set tissue volumes and masses
  adiposeTis.GetTotalMass().SetValue(adiposeMass_kg, MassUnit::kg);
  adiposeEC.GetVolume().SetValue(adiposeECVol_L, VolumeUnit::L);
  adiposeIC.GetVolume().SetValue(adiposeICVol_L, VolumeUnit::L);
  adiposeVas.GetVolume().SetValue(5.0*0.106, VolumeUnit::L);			//based on 5L blood volume and vascular fraction from BioGears.cpp
  liverTis.GetTotalMass().SetValue(liverMass_kg, MassUnit::kg);
  liverEC.GetVolume().SetValue(liverECVol_L, VolumeUnit::L);
  liverIC.GetVolume().SetValue(liverICVol_L, VolumeUnit::L);			
  liverVas.GetVolume().SetValue(5.0*0.05, VolumeUnit::L);				//based on 5L blood volume and vascular fraction from BioGears.cpp

  //Set substance concentrations
  adiposeEC.GetSubstanceQuantity(*Na)->GetMass().SetValue(0.145*adiposeECVol_L*NaMM, MassUnit::g);
  adiposeEC.GetSubstanceQuantity(*K)->GetMass().SetValue(0.0045*adiposeECVol_L*KMM, MassUnit::g);
  adiposeEC.GetSubstanceQuantity(*Cl)->GetMass().SetValue(0.116*adiposeECVol_L*ClMM, MassUnit::g);
  adiposeEC.GetSubstanceQuantity(*Ca)->GetMass().SetValue(0.0012*adiposeECVol_L*CaMM, MassUnit::g);
  adiposeIC.GetSubstanceQuantity(*Na)->GetMass().SetValue(0.015*adiposeICVol_L*NaMM, MassUnit::g);
  adiposeIC.GetSubstanceQuantity(*K)->GetMass().SetValue(0.120*adiposeICVol_L*KMM, MassUnit::g);
  adiposeIC.GetSubstanceQuantity(*Cl)->GetMass().SetValue(0.02*adiposeICVol_L*ClMM, MassUnit::g);
  adiposeIC.GetSubstanceQuantity(*Ca)->GetMass().SetValue(1e-7*adiposeICVol_L*CaMM, MassUnit::g);
  adiposeVas.GetSubstanceQuantity(*Na)->GetMass().SetValue(0.142*5.0*.106*NaMM, MassUnit::g);
  adiposeVas.GetSubstanceQuantity(*K)->GetMass().SetValue(0.0044*5.0*.106*KMM, MassUnit::g);
  adiposeVas.GetSubstanceQuantity(*Cl)->GetMass().SetValue(0.110*5.0*.106*ClMM, MassUnit::g);
  adiposeVas.GetSubstanceQuantity(*Ca)->GetMass().SetValue(0.0012*5.0*.106*CaMM, MassUnit::g);
  

  liverEC.GetSubstanceQuantity(*Na)->GetMass().SetValue(0.145*liverECVol_L*NaMM, MassUnit::g);
  liverEC.GetSubstanceQuantity(*K)->GetMass().SetValue(0.0045*liverECVol_L*KMM, MassUnit::g);
  liverEC.GetSubstanceQuantity(*Cl)->GetMass().SetValue(0.116*liverECVol_L*ClMM, MassUnit::g);
  liverEC.GetSubstanceQuantity(*Ca)->GetMass().SetValue(0.0012*liverECVol_L*CaMM, MassUnit::g);
  liverIC.GetSubstanceQuantity(*Na)->GetMass().SetValue(0.015*liverICVol_L*NaMM, MassUnit::g);
  liverIC.GetSubstanceQuantity(*K)->GetMass().SetValue(0.120*liverICVol_L*KMM, MassUnit::g);
  liverIC.GetSubstanceQuantity(*Cl)->GetMass().SetValue(0.02*liverICVol_L*ClMM, MassUnit::g);
  liverIC.GetSubstanceQuantity(*Ca)->GetMass().SetValue(1e-7*liverICVol_L*CaMM, MassUnit::g);
  liverVas.GetSubstanceQuantity(*Na)->GetMass().SetValue(0.142*5.0*0.05*NaMM, MassUnit::g);
  liverVas.GetSubstanceQuantity(*K)->GetMass().SetValue(0.0044*5.0*0.05*KMM, MassUnit::g);
  liverVas.GetSubstanceQuantity(*Cl)->GetMass().SetValue(0.102*5.0*0.05*ClMM, MassUnit::g);
  liverVas.GetSubstanceQuantity(*Ca)->GetMass().SetValue(0.0012*5.0*.05*CaMM, MassUnit::g);

  //Balance everything out
  adiposeEC.Balance(BalanceLiquidBy::Mass);
  adiposeIC.Balance(BalanceLiquidBy::Mass);
  adiposeVas.Balance(BalanceLiquidBy::Mass);
  liverEC.Balance(BalanceLiquidBy::Mass);
  liverIC.Balance(BalanceLiquidBy::Mass);
  liverVas.Balance(BalanceLiquidBy::Mass);

  //Initialize membrane potential
 
  adiposeTis.GetMembranePotential().SetValue(-85.0, ElectricPotentialUnit::mV);
  liverTis.GetMembranePotential().SetValue(-85.0, ElectricPotentialUnit::mV);

  //Set up variables to track
  double adiposePotential_mV = adiposeTis.GetMembranePotential(ElectricPotentialUnit::mV);
  double sodiumAdiposeIC_M = adiposeIC.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double sodiumAdiposeEC_M = adiposeEC.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double sodiumAdiposeVas_M = adiposeVas.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double potassiumAdiposeIC_M = adiposeIC.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double potassiumAdiposeEC_M = adiposeEC.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double potassiumAdiposeVas_M = adiposeVas.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double chlorideAdiposeIC_M = adiposeIC.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double chlorideAdiposeEC_M = adiposeEC.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double chlorideAdiposeVas_M = adiposeVas.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double calciumAdiposeIC_M = adiposeIC.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double calciumAdiposeEC_M = adiposeEC.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double calciumAdiposeVas_M = adiposeVas.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);

  double liverPotential_mV = liverTis.GetMembranePotential(ElectricPotentialUnit::mV);
  double sodiumLiverIC_M = liverIC.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double sodiumLiverEC_M = liverEC.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double sodiumLiverVas_M = liverVas.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double potassiumLiverIC_M = liverIC.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double potassiumLiverEC_M = liverEC.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double potassiumLiverVas_M = liverVas.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double chlorideLiverIC_M = liverIC.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double chlorideLiverEC_M = liverEC.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double chlorideLiverVas_M = liverVas.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double calciumLiverIC_M = liverIC.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double calciumLiverEC_M = liverEC.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
  double calciumLiverVas_M = liverVas.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);

  //Initialize tracking
  std::string rptFile = rptDirectory + "\\ActiveIonTransportTest.txt";
  double time = 0.0;
  //tracker.Track("Adipose_Na_EC", time, sodiumAdiposeEC_M);
  //tracker.Track("Adipose_Na_IC", time, sodiumAdiposeIC_M);
  //tracker.Track("Adipose_Na_Vas", time, sodiumAdiposeVas_M);
  //tracker.Track("Adipose_K_EC", time, potassiumAdiposeEC_M);
  //tracker.Track("Adipose_K_IC", time, potassiumAdiposeIC_M);
  //tracker.Track("Adipose_K_Vas", time, potassiumAdiposeVas_M);
  //tracker.Track("Adipose_Cl_EC", time, chlorideAdiposeEC_M);
  //tracker.Track("Adipose_Cl_IC", time, chlorideAdiposeIC_M);
  //tracker.Track("Adipose_Cl_Vas", time, chlorideAdiposeVas_M);
  //tracker.Track("Adipose_Ca_EC", time, calciumAdiposeEC_M);
  //tracker.Track("Adipose_Ca_IC", time, calciumAdiposeIC_M);
  //tracker.Track("Adipose_Ca_Vas", time, calciumAdiposeVas_M);
  //tracker.Track("Adipose_Potential", time, adiposePotential_mV);


  tracker.Track("Liver_Na_EC", time, sodiumLiverEC_M);
  tracker.Track("Liver_Na_IC", time, sodiumLiverIC_M);
  tracker.Track("Liver_Na_Vas", time, sodiumLiverVas_M);
  tracker.Track("Liver_K_EC", time, potassiumLiverEC_M);
  tracker.Track("Liver_K_IC", time, potassiumLiverIC_M);
  tracker.Track("Liver_K_Vas", time, potassiumLiverVas_M);
  tracker.Track("Liver_Cl_EC", time, chlorideLiverEC_M);
  tracker.Track("Liver_Cl_IC", time, chlorideLiverIC_M);
  tracker.Track("Liver_Cl_Vas", time, chlorideLiverVas_M);
  tracker.Track("Liver_Ca_EC", time, calciumLiverEC_M);
  tracker.Track("Liver_Ca_IC", time, calciumLiverIC_M);
  tracker.Track("Liver_Ca_Vas", time, calciumLiverVas_M);
  tracker.Track("Liver_Potential", time, liverPotential_mV);


  //------------Test 1:  Maintain Steady State------------------------
   

  //------------Test 2:  Sodium-----------------------
  //Change vascular to 80 and 200
  
  //-----------Test 3:  Potassium------------------------------------
  //Change vascular to 1.5 and 10

  //-----------Test 4:  Chloride------------------------------------
  //Change vascular to 130 and 80


  for (int i = 0; i < 90000; i++)
  {
	  time += timestep_s;
	  //if (i <=30000)
	  //{
		 //// adiposeVas.GetSubstanceQuantity(*Na)->GetMolarity().SetValue(95, AmountPerVolumeUnit::mmol_Per_L);
		 //// adiposeVas.Balance(BalanceLiquidBy::Molarity);
		 //// adiposeEC.GetSubstanceQuantity(*Na)->GetMolarity().SetValue(95, AmountPerVolumeUnit::mmol_Per_L);
		 //// adiposeEC.Balance(BalanceLiquidBy::Molarity);
		 // liverVas.GetSubstanceQuantity(*Na)->GetMolarity().SetValue(200, AmountPerVolumeUnit::mmol_Per_L);
		 // liverVas.Balance(BalanceLiquidBy::Molarity);
		 // //liverEC.GetSubstanceQuantity(*Na)->GetMolarity().SetValue(200, AmountPerVolumeUnit::mmol_Per_L);
		 // //liverEC.Balance(BalanceLiquidBy::Molarity);
	  //}

	  //tsu.MoveIonsByActiveTransport(adiposeTis, adiposeVas, adiposeEC, adiposeIC, timestep_s);
	  tsu.MoveIonsByActiveTransport(liverTis, liverVas, liverEC, liverIC, timestep_s);

	  //adiposePotential_mV = adiposeTis.GetMembranePotential(ElectricPotentialUnit::mV);
	  //sodiumAdiposeIC_M = adiposeIC.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  //sodiumAdiposeEC_M = adiposeEC.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  //sodiumAdiposeVas_M = adiposeVas.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  //potassiumAdiposeIC_M = adiposeIC.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  //potassiumAdiposeEC_M = adiposeEC.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  //potassiumAdiposeVas_M = adiposeVas.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  //chlorideAdiposeIC_M = adiposeIC.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  //chlorideAdiposeEC_M = adiposeEC.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  //chlorideAdiposeVas_M = adiposeVas.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  //calciumAdiposeIC_M = adiposeIC.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  //calciumAdiposeEC_M = adiposeEC.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  //calciumAdiposeVas_M = adiposeVas.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);


	  liverPotential_mV = liverTis.GetMembranePotential(ElectricPotentialUnit::mV);
	  sodiumLiverIC_M = liverIC.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  sodiumLiverEC_M = liverEC.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  sodiumLiverVas_M = liverVas.GetSubstanceQuantity(*Na)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  potassiumLiverIC_M = liverIC.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  potassiumLiverEC_M = liverEC.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  potassiumLiverVas_M = liverVas.GetSubstanceQuantity(*K)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  chlorideLiverIC_M = liverIC.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  chlorideLiverEC_M = liverEC.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  chlorideLiverVas_M = liverVas.GetSubstanceQuantity(*Cl)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  calciumLiverIC_M = liverIC.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  calciumLiverEC_M = liverEC.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
	  calciumLiverVas_M = liverVas.GetSubstanceQuantity(*Ca)->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);


	  //tracker.Track("Adipose_Na_EC", time, sodiumAdiposeEC_M);
	  //tracker.Track("Adipose_Na_IC", time, sodiumAdiposeIC_M);
	  //tracker.Track("Adipose_Na_Vas", time, sodiumAdiposeVas_M);
	  //tracker.Track("Adipose_K_EC", time, potassiumAdiposeEC_M);
	  //tracker.Track("Adipose_K_IC", time, potassiumAdiposeIC_M);
	  //tracker.Track("Adipose_K_Vas", time, potassiumAdiposeVas_M);
	  //tracker.Track("Adipose_Cl_EC", time, chlorideAdiposeEC_M);
	  //tracker.Track("Adipose_Cl_IC", time, chlorideAdiposeIC_M);
	  //tracker.Track("Adipose_Cl_Vas", time, chlorideAdiposeVas_M);
	  //tracker.Track("Adipose_Ca_EC", time, calciumAdiposeEC_M);
	  //tracker.Track("Adipose_Ca_IC", time, calciumAdiposeIC_M);
	  //tracker.Track("Adipose_Ca_Vas", time, calciumAdiposeVas_M);
	  //tracker.Track("Adipose_Potential", time, adiposePotential_mV);

	  tracker.Track("Liver_Na_EC", time, sodiumLiverEC_M);
	  tracker.Track("Liver_Na_IC", time, sodiumLiverIC_M);
	  tracker.Track("Liver_Na_Vas", time, sodiumLiverVas_M);
	  tracker.Track("Liver_K_EC", time, potassiumLiverEC_M);
	  tracker.Track("Liver_K_IC", time, potassiumLiverIC_M);
	  tracker.Track("Liver_K_Vas", time, potassiumLiverVas_M);
	  tracker.Track("Liver_Cl_EC", time, chlorideLiverEC_M);
	  tracker.Track("Liver_Cl_IC", time, chlorideLiverIC_M);
	  tracker.Track("Liver_Cl_Vas", time, chlorideLiverVas_M);
	  tracker.Track("Liver_Ca_EC", time, calciumLiverEC_M);
	  tracker.Track("Liver_Ca_IC", time, calciumLiverIC_M);
	  tracker.Track("Liver_Ca_Vas", time, calciumLiverVas_M);
	  tracker.Track("Liver_Potential", time, liverPotential_mV);

  }

  tracker.WriteTrackToFile(rptFile.c_str());

}

void BioGearsEngineTest::GenericClearanceTest(SETestSuite& testSuite)
{
  TimingProfile timer;
  BioGears bg(testSuite.GetLogger());
  Tissue& tsu = (Tissue&)bg.GetTissue();

  // First test case
  SETestCase& testCase1 = testSuite.CreateTestCase();
  testCase1.SetName("GenericClearanceTestLiquid");  
  // You at least need 2 test cases here (liquid and tissue)
  SETestCase& testCase = testSuite.CreateTestCase();
  timer.Start("Test");
  // Test Logic
  //testCase.GetFailures().push_back("Report any errors like this");
  // Log how long our test took
  testCase1.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

  // Second test case
  SETestCase& testCase2 = testSuite.CreateTestCase();
  testCase2.SetName("GenericClearanceTestTissue");
  timer.Start("Test");

  testCase2.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);
  // Subsequent test cases
}

void BioGearsEngineTest::GenericExcretionTest(SETestSuite& testSuite)
{
  TimingProfile timer;
  BioGears bg(testSuite.GetLogger());
  Tissue& tsu = (Tissue&)bg.GetTissue();

  // First test case
  SETestCase& testCase1 = testSuite.CreateTestCase();
  testCase1.SetName("GenericExcretionTest");
  timer.Start("Test");
  // Test Logic
  //testCase.GetFailures().push_back("Report any errors like this");
  // Log how long our test took
  testCase1.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);
}

//Set-up the test suite
void BioGearsEngineTest::DiffusionClearanceExcretionTests(const std::string& rptDirectory)
{

  m_Logger->ResetLogFile(rptDirectory + "\\DiffusionClearanceExcretionTests.log");
  // Set up our test report
  
  SETestReport testReport = SETestReport(m_Logger);

  SETestSuite& tsDistMass = testReport.CreateTestSuite();
  tsDistMass.SetName("DistributeMass");
  DistributeMass(tsDistMass);

  SETestSuite& ts1 = testReport.CreateTestSuite();
  ts1.SetName("PerfusionLimitedDiffusionTest");
  PerfusionLimitedDiffusionTest(ts1);

  //SETestSuite& ts5 = testReport.CreateTestSuite();
  //ts5.SetName("ActiveTransportTest");
  //ActiveTransportTest(ts5, rptDirectory);

  SETestSuite& ts6 = testReport.CreateTestSuite();
  ts6.SetName("InstantDiffusionTest");
  InstantDiffusionTest(ts6);

  //SETestSuite& ts7 = testReport.CreateTestSuite();
  //ts6.SetName("GenericClearanceTest");
  //GenericClearanceTest(ts7);

  //SETestSuite& ts8 = testReport.CreateTestSuite();
  //ts7.SetName("GenericExcretionTest");
  //GenericExcretionTest(ts8);

  //testReport.WriteFile(rptDirectory + "\\GasCompartmentTest.xml");
}