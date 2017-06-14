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

// The following tests will be exported and runnable
#include <math.h>
#include "bind/biogears-cdm.hxx"
#include "BioGearsPhysiologyEngine.h"
#include "Controller/BioGearsEngine.h"

#if defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    #ifdef TEST_EXPORTS
	    #define TEST_DECL __declspec(dllexport)
    #else
	    #define TEST_DECL __declspec(dllimport)
    #endif
#elif defined(__gnu_linux__)
    #define TEST_DECL __attribute__ ((visibility ("default")))
#else
    #define TEST_DECL
#endif

class SETestSuite;
class HowToTracker;

class TEST_DECL BioGearsEngineTest : Loggable
{
public:
  BioGearsEngineTest();
  BioGearsEngineTest(Logger* logger);
  virtual ~BioGearsEngineTest();

  bool RunTest(const std::string& testName, const std::string& sOutputDirectory);
  void FillFunctionMap();

protected:
  bool myLogger;
	typedef void(BioGearsEngineTest::*testFunction)(const std::string&);
	typedef std::map<std::string, testFunction> testMap;
	testMap bgeMap;
	std::stringstream m_ss;

  //////////////////////////////////////////////////////////
  // Circuit Hemodynamics and Compartment Transport Tests //
  // System Specific Functions Tests as well              //
  //////////////////////////////////////////////////////////
public:
  ////////////////////
  // Cardiovascular //
  void CardiovascularCircuitAndTransportTest(const std::string& sTestDirectory);
  void CardiovascularAndRenalCircuitAndTransportTest(const std::string& sTestDirectory);
  void CardiovascularAndTissueCircuitAndTransportTest(const std::string& sTestDirectory);
  void CardiovascularTissueAndRenalCircuitAndTransportTest(const std::string& sTestDirectory);
  void CardiovascularBloodGasesTest(const std::string& sTestDirectory);// Characterize the cv circuit
  void TuneCardiovascularCircuitTest(const std::string& sTestDirectory);
  // Cardiovascular Utilities //
  void CardiovascularCircuitScaleTests(const std::string& sTestDirectory);
protected:
  enum CardiovascularDriver { Sinusoid = 0, Heart };
  void SinusoidHeartDriver(double time_s, double heartRate_s, double& lHeartElastance, double& rHeartElastance);
  void CardiovascularCircuitAndTransportTest(CardiovascularDriver driverType, double complianceScale, double resistanceScale, double volumeScale, double heartRate_bpm,
    double sysRes, double sysComp, double aortaRes, double aortaComp, double rightHeartRes, double venaCavaComp, bool connectTissue, bool connectRenal, 
	bool balanceBloodgases, const std::string& sTestDirectory, const std::string& sTestName, bool breakOutResults);
  void TuneCardiovascularCircuitTest(SETestSuite& testSuite, const std::string& sTestDirectory, const std::string& sTestName, SEPatient& patient);

  double m_MeanAortaPressure;
  double m_MeanCVPressure;
  double m_Systolic;
  double m_Diastolic;
  double m_Pulse;
  double m_CardiacOutput;

public:
  ///////////
  // Renal //
  void RenalCircuitAndTransportTest(const std::string& sTestDirectory);
  void RenalTGFFeedbackTest(const std::string& sTestDirectory);
  void RenalTGFandUPRFeedbackTest(const std::string& sTestDirectory); 
  void RenalSecretionTest(const std::string& sTestDirectory);
  void RenalUrinateTest(const std::string& sTestDirectory);
protected:
  enum RenalFeedback { TGF, TGFandUPR };
  void RenalFeedbackTest(RenalFeedback feedback, const std::string& sTestDirectory, const std::string& sTestName);
  enum RenalSystems { Secretion, Urinating};
  void RenalSystemTest(RenalSystems systemtest, const std::string& sTestDirectory, const std::string& sTestName);

public:
  /////////////////
  // Respiratory //
  void RespiratoryCircuitAndTransportTest(const std::string& sTestDirectory);
  void RespiratoryDriverTest(const std::string& sTestDirectory);
protected:
  enum RespiratoryConfiguration { RespiratorySolo, AnesthesiaMachineSolo, RespiratoryWithAnesthesiaMachine, RespiratoryWithInhaler, RespiratoryWithMechanicalVentilator};
  void RespiratoryCircuitAndTransportTest(RespiratoryConfiguration config, const std::string& sTestDirectory);

public:
  ////////////////////////
  // Anesthesia Machine //
	void AnesthesiaMachineCircuitAndTransportTest(const std::string& sTestDirectory);
  void RespiratoryWithAnesthesiaMachineCircuitAndTransportTest(const std::string& sTestDirectory);
protected:
	void AnesthesiaMachineCircuitAndTransportTest(RespiratoryConfiguration config, const std::string& sTestDirectory);

public:
  /////////////
  // Inhaler //
  void RespiratoryWithInhalerCircuitAndTransportTest(const std::string& sTestDirectory);
protected:

public:
  /////////////
  // Inhaler //
  void RespiratoryWithMechanicalVentilatorCircuitAndTransportTest(const std::string& sTestDirectory);
protected:

public:
  ////////////////////////////
  // Environment and Energy //
  void InternalTemperatureVariableBMRCircuitTest(const std::string& sTestDirectory);
  void InternalTemperatureVariableSkinCircuitTest(const std::string& sTestDirectory);
  void InternalTemperatureVariableCoreCircuitTest(const std::string& sTestDirectory);
  void EnvironmentVariableTemperatureCircuitTest(const std::string& sTestDirectory);
  void CombinedInternalAndEnvironmentVariableBMRandTemperatureCircuitTest(const std::string& sTestDirectory);
  void CombinedInternalAndEnvironmentSkinTempDropCircuitTest(const std::string& sTestDirectory);
  void EnvironmentISO7730ComparisonTest(const std::string& sTestDirectory);
protected:

public:
  //////////////////
  // Tissue Tests // 
  void DiffusionClearanceExcretionTests(const std::string& rptDirectory);
  void AlveolarCarbonDioxideDiffusionTest(const std::string& rptDirectory);
  void AlveolarOxygenDiffusionTest(const std::string& rptDirectory);
  void InstantPlusSimpleDiffusionTest(const std::string& rptDirectory);
  void SimpleDiffusionTwoCompartmentTest(const std::string& rptDirectory);
  void SimpleDiffusionFourCompartmentTest(const std::string& rptDirectory);
  void SimpleDiffusionHierarchyTest(const std::string& rptDirectory);
  void FacilitatedDiffusionTest(const std::string& rptDirectory);
protected:
  void DistributeMass(SETestSuite& testSuite);
  void PerfusionLimitedDiffusionTest(SETestSuite& testSuite);
  void InstantDiffusionTest(SETestSuite& testSuite);
  void ActiveTransportTest(SETestSuite& testSuite);
  void GenericClearanceTest(SETestSuite& testSuite);
  void GenericExcretionTest(SETestSuite& testSuite);

public:
    ///////////////////
    // Nervous Tests //
    void BrainInjuryTest(const std::string& rptDirectory);
protected:
  

public:
  ////////////////////////////
  // Acid Base Engine Tests //
  void FourCompartmentTestSimple(const std::string& sOutputDirectory);
  void AcidBaseFourCompartmentTest(const std::string& sOutputDirectory);
  void FiveCompartmentTestWithDiffusion(const std::string& sOutputDirectory);
  void AcidBaseFourCompartmentTestWithProductionConsumption(const std::string& sOutputDirectory);
  void AcidBaseFiveCompartmentTestWithDiffusion(const std::string& sOutputDirectory);
  void AcidBaseFiveCompartmentTestWithProductionConsumptionAndDiffusion(const std::string& sOutputDirectory);
 protected:
   void FourCompartmentTest(bool usingAcidBase, bool usingProductionConsumption, bool usingDiffusion, const std::string& sOutputDirectory);
   bool usingAcidBase;
   bool usingProductionConsumption;
   bool usingDiffusion;

   /////////////////////
   // Acid Base Tests //
   /////////////////////
public:
  void AcidBaseMathTest(const std::string& sOutputDirectory);
  void AcidBaseFeedbackTest(const std::string& sOutputDirectory);
  void AcidBaseLimitsTest(const std::string& sOutputDirectory);
  void AcidBaseExtremeTest(const std::string & rptDirectory);
  void AcidBaseBloodGasTests(const std::string& sOutputDirectory);
protected:
  enum bloodType { ARTERIAL, VENOUS, CAPILLARY, RESPIRATORY_ACIDOSIS, METABOLIC_ALKALOSIS, METABOLIC_ACIDOSIS, RESPIRATORY_ALKALOSIS, CUSTOM };
  void AcidBaseBloodGasTest(BioGears& bg, bloodType compartment, SETestSuite& testSuite);

  /////////////////////
  // Aerosol Tests //
  /////////////////////
public:
  void AerosolTest(const std::string& sOutputDirectory);
protected:
  void SizeIndependentDepositionEfficencyCoefficientsTest(SETestSuite& suite, SESubstance& substance, double expectedMouthCoeff, double expectedCarinaCoeff, double expectedDeadSpaceCoeff, double expectedAlveoliCoeff);
  void DepositionFractionTest(SETestSuite& suite, SESubstance& substance, double expectedMouthDepFrac, double expectedCarinaDepFrac, double expectedDeadSpaceDepFrac, double expectedAlveoliDepFrac);
 

public:
  //////////////////////////
  // General Engine Tests //
  void ConditionCombinations(const std::string& rptDirectory);
  void MultiEngineTest(const std::string& sTestDirectory);
  void SerializationTest(const std::string& sTestDirectory);
  void SolverSpeedTest(const std::string& sTestDirectory);
protected:
  void InhalerState(PhysiologyEngine* bg, HowToTracker& tracker);
  void InjectSuccsState(PhysiologyEngine* bg, HowToTracker& tracker, const SESubstance& succs);
  

};

