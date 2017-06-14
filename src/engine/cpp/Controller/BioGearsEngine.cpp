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
#include "BioGearsPhysiologyEngine.h"
#include "bind/BioGears.hxx"
#include "bind/BioGearsStateData.hxx"
#include "patient/SEPatient.h"
#include "circuit/SECircuit.h"
#include "compartment//SECompartmentManager.h"
#include "engine/PhysiologyEngineStabilization.h"
#include "scenario/SEScenario.h"
#include "scenario/SECondition.h"
#include "bind/ConditionData.hxx"

#include "utils/FileUtils.h"

#include "scenario/SESerializeState.h"
#include "patient/actions/SEPatientAssessmentRequest.h"
#include "bind/PatientAssessments.hxx"
#include "patient/assessments/SEPulmonaryFunctionTest.h"
#include "patient/assessments/SEUrinalysis.h"
#include "patient/assessments/SECompleteBloodCount.h"
#include "patient/assessments/SEComprehensiveMetabolicPanel.h"
#include "bind/PatientData.hxx"
#include "bind/Patient.hxx"
#include "bind/BioGearsConfigurationData.hxx"
#include "substance/SESubstanceCompound.h"

#include "bind/BioGearsBloodChemistrySystemData.hxx"
#include "bind/BioGearsCardiovascularSystemData.hxx"
#include "bind/BioGearsDrugSystemData.hxx"
#include "bind/BioGearsEndocrineSystemData.hxx"
#include "bind/BioGearsEnergySystemData.hxx"
#include "bind/BioGearsGastrointestinalSystemData.hxx"
#include "bind/BioGearsHepaticSystemData.hxx"
#include "bind/BioGearsNervousSystemData.hxx"
#include "bind/BioGearsRenalSystemData.hxx"
#include "bind/BioGearsRespiratorySystemData.hxx"
#include "bind/BioGearsTissueSystemData.hxx"
#include "bind/BioGearsEnvironmentData.hxx"
#include "bind/BioGearsAnesthesiaMachineData.hxx"
#include "bind/BioGearsElectroCardioGramData.hxx"
#include "bind/BioGearsInhalerData.hxx"

#include "Serializer.h"

#include <memory>

BIOGEARS_API std::unique_ptr<PhysiologyEngine> CreateBioGearsEngine(const std::string& logfile)
{
	return std::unique_ptr<BioGearsEngine>(new BioGearsEngine(logfile));
}

BIOGEARS_API std::unique_ptr<PhysiologyEngine> CreateBioGearsEngine(Logger* logger)
{
	return std::unique_ptr<BioGearsEngine>(new BioGearsEngine(logger));
}

BioGearsEngine::BioGearsEngine(Logger* logger) : BioGears(logger), m_EngineTrack(*this)
{
  m_State = EngineState::NotReady;
  m_EventHandler = nullptr;
	m_DataTrack = &m_EngineTrack.GetDataTrack();
}

BioGearsEngine::BioGearsEngine(const std::string& logFileName) : BioGears(logFileName), m_EngineTrack(*this)
{
  m_State = EngineState::NotReady;
  m_EventHandler = nullptr;
	m_DataTrack = &m_EngineTrack.GetDataTrack();
}

BioGearsEngine::~BioGearsEngine()
{	
	
}

Logger* BioGearsEngine::GetLogger()
{
	return Loggable::GetLogger();
}

PhysiologyEngineTrack* BioGearsEngine::GetEngineTrack()
{
	return &m_EngineTrack;
}

bool BioGearsEngine::LoadState(const std::string& file, const SEScalarTime* simTime)
{
  std::unique_ptr<CDM::ObjectData> bind = Serializer::ReadFile(file, GetLogger());
  CDM::BioGearsStateData* state = dynamic_cast<CDM::BioGearsStateData*>(bind.get());
  if (state != nullptr)
    return LoadState(*state,simTime);
  Error("File does not contain a valid PhysiologyEngineState");
  return false;
}

bool BioGearsEngine::LoadState(const CDM::PhysiologyEngineStateData& state, const SEScalarTime* simTime)
{
  m_ss.str("");
  
  // We could preserve the tracker, but I think I want to force the user to set it up
  // again, they should have the data tracks (or easily get them), and they should
  // Set it back up, and set or reset the results file they are using
  m_State = EngineState::NotReady;

  const CDM::BioGearsStateData* bgState = dynamic_cast<const CDM::BioGearsStateData*>(&state);
  if (bgState == nullptr)
  {
    Error("State data is not a BioGearsStateData object");
    return false;
  }

 
  if (state.DataRequests().present())
  {
    m_EngineTrack.GetDataRequestManager().Clear();
    m_EngineTrack.GetDataRequestManager().Load(state.DataRequests().get(), *m_Substances);
    m_EngineTrack.ForceConnection();// I don't want to rest the file because I would loose all my data      
  }

  if (simTime != nullptr)
  {
    m_CurrentTime->Set(*simTime);
    m_SimulationTime->Set(*simTime);
  }
  else
  {
    if (state.SimulationTime().present())
    {
      m_CurrentTime->Load(state.SimulationTime().get());
      m_SimulationTime->Load(state.SimulationTime().get());
    }
    else
    {
      m_CurrentTime->SetValue(0, TimeUnit::s);
      m_SimulationTime->SetValue(0, TimeUnit::s);
    }
  }
  m_AirwayMode = bgState->AirwayMode();
  m_Intubation = bgState->Intubation();
   
  /// Patient //  
  if (!bgState->Patient().present())
    m_ss << "BioGearsState must have a patient" << std::endl;
  else if (!m_Patient->Load(bgState->Patient().get()))
    m_ss << "Error loading patient data" << std::endl;
  // Conditions //
  m_Conditions->Clear();
  for (const CDM::ConditionData& cData : bgState->Condition())
  {
    if (!m_Conditions->ProcessCondition(cData))
      m_ss << "Unable to load condition" << std::endl;
  }
  // Actions //
  m_Actions->Clear();
  for (const CDM::ActionData& cData : bgState->ActiveAction())
  {
    if (!m_Actions->ProcessAction(cData))
      m_ss << "Unable to load action" << std::endl;
  }
  // Substances //
  for (const CDM::SubstanceData& subData : bgState->ActiveSubstance())
  {
    SESubstance* sub = m_Substances->GetSubstance(subData.Name());
    if (sub == nullptr)
    {
      sub = new SESubstance(GetLogger());
      m_Substances->AddSubstance(*sub);
    }
    if (!sub->Load(subData))    
      m_ss << "Unable to load substance" << subData.Name() << std::endl;
    m_Substances->AddActiveSubstance(*sub);
  }
  // Compounds //
  for (const CDM::SubstanceCompoundData& cmpdData : bgState->ActiveSubstanceCompound())
  {
    SESubstanceCompound* cmpd = m_Substances->GetCompound(cmpdData.Name());
    if (cmpd == nullptr)
      cmpd = new SESubstanceCompound(GetLogger());
    if (!cmpd->Load(cmpdData, *m_Substances))
      m_ss << "Unable to load compound" << cmpdData.Name() << std::endl;      
  }
  // Circuit Manager //
  if (!bgState->CircuitManager().present())
    m_ss << "BioGearsState must have a circuit manager" << std::endl;
  else
  {
    const CDM::CircuitManagerData* cmptMgrData = dynamic_cast<const CDM::CircuitManagerData*>(&bgState->CircuitManager().get());
    if (cmptMgrData == nullptr)
      m_ss << "BioGearsState must have a BioGears circuit manager" << std::endl;
    else if (!m_Circuits->Load(*cmptMgrData))
      m_ss << "Error loading circuit manager data" << std::endl;
  }
  // Compartment Manager //
  if (!bgState->CompartmentManager().present())
    m_ss << "BioGearsState must have a compartment manager" << std::endl;
  else
  {
    const CDM::CompartmentManagerData* cmptMgrData = dynamic_cast<const CDM::CompartmentManagerData*>(&bgState->CompartmentManager().get());
    if (cmptMgrData == nullptr)
      m_ss << "BioGearsState must have a BioGears compartment manager" << std::endl;
    else if (!m_Compartments->Load(*cmptMgrData, m_Circuits.get()))
      m_ss << "Error loading compartment manager data" << std::endl;
  }
  // Configuration //
  if (!bgState->Configuration().present())
    m_ss << "BioGearsState must have a configuration" << std::endl;
  else
  {
    const CDM::BioGearsConfigurationData* confData = dynamic_cast<const CDM::BioGearsConfigurationData*>(&bgState->Configuration().get());
    if (confData == nullptr)
      m_ss << "BioGearsState must have a BioGears configuration" << std::endl;
    else if (!m_Config->Load(*confData))
      m_ss << "Error loading configuration data" << std::endl;
  }
  // Now, Let's see if there is anything to merge into our base configuration
  // At this point I don't think we should be doing this... but maybe you want to...
  //Info("Merging OnDisk Configuration");
  //BioGearsConfiguration cFile(*m_Substances);
  //cFile.LoadFile("BioGearsConfiguration.xml");
  //m_Config->Merge(cFile);
  

  /////////////
  // Systems //
  /////////////
  // Physiology
  const CDM::BioGearsBloodChemistrySystemData*   bcData  = nullptr;
  const CDM::BioGearsCardiovascularSystemData*   cvData  = nullptr;
  const CDM::BioGearsDrugSystemData*             dData   = nullptr;
  const CDM::BioGearsEndocrineSystemData*        ndoData = nullptr;
  const CDM::BioGearsEnergySystemData*           nrgData = nullptr;
  const CDM::BioGearsGastrointestinalSystemData* gasData = nullptr;
  const CDM::BioGearsHepaticSystemData*          hepData = nullptr;
  const CDM::BioGearsNervousSystemData*          nrvData = nullptr;
  const CDM::BioGearsRenalSystemData*            rnlData = nullptr;
  const CDM::BioGearsRespiratorySystemData*      rspData = nullptr;
  const CDM::BioGearsTissueSystemData*           tsuData = nullptr;
  // Environment                                         
  const CDM::BioGearsEnvironmentData*            envData = nullptr;
  // Equipment                                           
  const CDM::BioGearsAnesthesiaMachineData*      amData  = nullptr;
  const CDM::BioGearsElectroCardioGramData*      ecgData = nullptr;
  const CDM::BioGearsInhalerData*                nhlData = nullptr;
  for (const CDM::SystemData& sysData : bgState->System())
  {
    if (bcData == nullptr)
    {
      bcData = dynamic_cast<const CDM::BioGearsBloodChemistrySystemData*>(&sysData);
      if (bcData != nullptr && !m_BloodChemistrySystem->Load(*bcData))
        m_ss << "Error loading Blood Chemistry data" << std::endl;
    }
    if (cvData == nullptr)
    {
      cvData = dynamic_cast<const CDM::BioGearsCardiovascularSystemData*>(&sysData);
      if (cvData != nullptr && !m_CardiovascularSystem->Load(*cvData))
        m_ss << "Error loading Cardiovascular data" << std::endl;
    }
    if (dData == nullptr)
    {
      dData = dynamic_cast<const CDM::BioGearsDrugSystemData*>(&sysData);
      if (dData != nullptr && !m_DrugSystem->Load(*dData))
        m_ss << "Error loading Drug data" << std::endl;
    }
    if (ndoData == nullptr)
    {
      ndoData = dynamic_cast<const CDM::BioGearsEndocrineSystemData*>(&sysData);
      if (ndoData != nullptr && !m_EndocrineSystem->Load(*ndoData))
        m_ss << "Error loading Endocrine data" << std::endl;
    }
    if (nrgData == nullptr)
    {
      nrgData = dynamic_cast<const CDM::BioGearsEnergySystemData*>(&sysData);
      if (nrgData != nullptr && !m_EnergySystem->Load(*nrgData))
        m_ss << "Error loading Energy data" << std::endl;
    }
    if (gasData == nullptr)
    {
      gasData = dynamic_cast<const CDM::BioGearsGastrointestinalSystemData*>(&sysData);
      if (gasData != nullptr && !m_GastrointestinalSystem->Load(*gasData))
        m_ss << "Error loading Gastrointestinal data" << std::endl;
    }
    if (hepData == nullptr)
    {
      hepData = dynamic_cast<const CDM::BioGearsHepaticSystemData*>(&sysData);
      if (hepData != nullptr && !m_HepaticSystem->Load(*hepData))
        m_ss << "Error loading Hepatic data" << std::endl;
    }
    if (nrvData == nullptr)
    {
      nrvData = dynamic_cast<const CDM::BioGearsNervousSystemData*>(&sysData);
      if (nrvData != nullptr && !m_NervousSystem->Load(*nrvData))
        m_ss << "Error loading Nervous data" << std::endl;
    }
    if (rnlData == nullptr)
    {
      rnlData = dynamic_cast<const CDM::BioGearsRenalSystemData*>(&sysData);
      if (rnlData != nullptr && !m_RenalSystem->Load(*rnlData))
        m_ss << "Error loading Renal data" << std::endl;
    }
    if (rspData == nullptr)
    {
      rspData = dynamic_cast<const CDM::BioGearsRespiratorySystemData*>(&sysData);
      if (rspData != nullptr && !m_RespiratorySystem->Load(*rspData))
        m_ss << "Error loading Respiratory data" << std::endl;
    }
    if (tsuData == nullptr)
    {
      tsuData = dynamic_cast<const CDM::BioGearsTissueSystemData*>(&sysData);
      if (tsuData != nullptr && !m_TissueSystem->Load(*tsuData))
        m_ss << "Error loading Tissue data" << std::endl;
    }
    if (envData == nullptr)
    {
      envData = dynamic_cast<const CDM::BioGearsEnvironmentData*>(&sysData);
      if (envData != nullptr && !m_Environment->Load(*envData))
        m_ss << "Error loading Environment data" << std::endl;
    }
    if (amData == nullptr)
    {
      amData = dynamic_cast<const CDM::BioGearsAnesthesiaMachineData*>(&sysData);
      if (amData != nullptr && !m_AnesthesiaMachine->Load(*amData))
        m_ss << "Error loading Anesthesia Machine data" << std::endl;
    }
    if (ecgData == nullptr)
    {
      ecgData = dynamic_cast<const CDM::BioGearsElectroCardioGramData*>(&sysData);
      if (ecgData != nullptr && !m_ECG->Load(*ecgData))
        m_ss << "Error loading ECG data" << std::endl;
    }
    if (nhlData == nullptr)
    {
      nhlData = dynamic_cast<const CDM::BioGearsInhalerData*>(&sysData);
      if (nhlData != nullptr && !m_Inhaler->Load(*nhlData))
        m_ss << "Error loading Inhaler data" << std::endl;
    }
  }
  // Make sure we had all systems in the state
  if (bcData == nullptr)
    m_ss<<"Missing Blood Chemistry State"<<std::endl;
  if (cvData == nullptr)
    m_ss << "Missing Cardiovascular State" << std::endl;
  if (dData == nullptr)
    m_ss << "Missing Drug State" << std::endl;
  if (ndoData == nullptr)
    m_ss << "Missing Endocrine State" << std::endl;
  if (nrgData == nullptr)
    m_ss << "Missing Energy State" << std::endl;
  if (gasData == nullptr)
    m_ss << "Missing Gastrointestinal State" << std::endl;
  if (hepData == nullptr)
    m_ss << "Missing Hepatic State" << std::endl;
  if (nrvData == nullptr)
    m_ss << "Missing Nervous State" << std::endl;
  if (rnlData == nullptr)
    m_ss << "Missing Renal State" << std::endl;
  if (rspData == nullptr)
    m_ss << "Missing Respiratory State" << std::endl;
  if (tsuData == nullptr)
    m_ss << "Missing Tissue State" << std::endl;
  if (envData == nullptr)
    m_ss << "Missing Environment State" << std::endl;
  if (amData == nullptr)
    m_ss << "Missing Anesthesia Machine State" << std::endl;
  if (ecgData == nullptr)
    m_ss << "Missing ECG State" << std::endl;
  if (nhlData == nullptr)
    m_ss << "Missing Inhaler State" << std::endl;

  if (!m_ss.str().empty())
  {
    Error(m_ss);
    return false;
  }
  // Make sure links are correct with our modes
  m_Compartments->UpdateAirwayGraph();
  m_Compartments->GetActiveRespiratoryGraph();
  m_Compartments->GetActiveAerosolGraph();

  // It helps to unload what you just loaded and to a compare if you have issues
  //SaveState("WhatIJustLoaded.xml");

  // Good to go, save it off and carry on!
  m_State = EngineState::Active;
  return true;// return CheckDataRequirements/IsValid() or something
}

std::unique_ptr<CDM::PhysiologyEngineStateData> BioGearsEngine::SaveState(const std::string& file)
{
  std::unique_ptr<CDM::PhysiologyEngineStateData> state(new CDM::BioGearsStateData());

  state->contentVersion(BGE::Version);
  
  state->SimulationTime(std::unique_ptr<CDM::ScalarTimeData>(m_SimulationTime->Unload()));
  if(m_EngineTrack.GetDataRequestManager().HasDataRequests())
    state->DataRequests(std::unique_ptr<CDM::DataRequestsData>(m_EngineTrack.GetDataRequestManager().Unload()));

  ((CDM::BioGearsStateData*)state.get())->AirwayMode(m_AirwayMode);
  ((CDM::BioGearsStateData*)state.get())->Intubation(m_Intubation);
  // Patient
  state->Patient(std::unique_ptr<CDM::PatientData>(m_Patient->Unload()));  
  // Conditions
  std::vector<CDM::ConditionData*> conditions;
  m_Conditions->Unload(conditions);
  for (CDM::ConditionData* cData : conditions)
    state->Condition().push_back(std::unique_ptr<CDM::ConditionData>(cData));
  // Actions
  std::vector<CDM::ActionData*> activeActions;
  m_Actions->Unload(activeActions);
  for (CDM::ActionData* aData : activeActions)
    state->ActiveAction().push_back(std::unique_ptr<CDM::ActionData>(aData));
 // Active Substances/Compounds
  for (SESubstance* s : m_Substances->GetActiveSubstances())
    state->ActiveSubstance().push_back(std::unique_ptr<CDM::SubstanceData>(s->Unload()));
  for (SESubstanceCompound* c : m_Substances->GetActiveCompounds())
    state->ActiveSubstanceCompound().push_back(std::unique_ptr<CDM::SubstanceCompoundData>(c->Unload()));  
  // Systems
  state->System().push_back(std::unique_ptr<CDM::BioGearsBloodChemistrySystemData>(m_BloodChemistrySystem->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsCardiovascularSystemData>(m_CardiovascularSystem->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsDrugSystemData>(m_DrugSystem->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsEndocrineSystemData>(m_EndocrineSystem->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsEnergySystemData>(m_EnergySystem->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsGastrointestinalSystemData>(m_GastrointestinalSystem->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsHepaticSystemData>(m_HepaticSystem->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsNervousSystemData>(m_NervousSystem->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsRenalSystemData>(m_RenalSystem->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsRespiratorySystemData>(m_RespiratorySystem->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsTissueSystemData>(m_TissueSystem->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsEnvironmentData>(m_Environment->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsAnesthesiaMachineData>(m_AnesthesiaMachine->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsElectroCardioGramData>(m_ECG->Unload()));
  state->System().push_back(std::unique_ptr<CDM::BioGearsInhalerData>(m_Inhaler->Unload()));
  // Compartments
  state->CompartmentManager(std::unique_ptr<CDM::CompartmentManagerData>(m_Compartments->Unload()));
  // Configuration
  state->Configuration(std::unique_ptr<CDM::PhysiologyEngineConfigurationData>(m_Config->Unload()));
  // Circuitsk
  state->CircuitManager(std::unique_ptr<CDM::CircuitManagerData>(m_Circuits->Unload()));

  if (!file.empty())
  {
    CreateFilePath(file);
    // Write out the engine state
    std::ofstream stream(file);
    // Write out the xml file
    xml_schema::namespace_infomap map;
    map[""].name = "uri:/mil/tatrc/physiology/datamodel";
    try
    {
      BioGearsState(stream, dynamic_cast<CDM::BioGearsStateData&>(*state), map);
    }
    catch (std::exception& ex)
    {
      Error(ex.what());
    }
    stream.close();
  }

  return state;
}

bool BioGearsEngine::InitializeEngine(const std::string& patientFile, const std::vector<const SECondition*>* conditions, const PhysiologyEngineConfiguration* config)
{
	std::string pFile = patientFile;
	if (pFile.find("/patients") == std::string::npos)
	{// Prepend the patient directory if it's not there
		pFile = "./patients/";
		pFile += patientFile;
	}
	if (!m_Patient->LoadFile(pFile))
		return false;
	return InitializeEngine(conditions,config);
}

bool BioGearsEngine::InitializeEngine(const SEPatient& patient, const std::vector<const SECondition*>* conditions, const PhysiologyEngineConfiguration* config)
{ 
  CDM_COPY((&patient), m_Patient);
  // We need logic here that makes sure we have what we need
  // and notify we are ignoring anything provided we won't use
	return InitializeEngine(conditions,config);
}

bool BioGearsEngine::InitializeEngine(const std::vector<const SECondition*>* conditions, const PhysiologyEngineConfiguration* config)
{
  m_EngineTrack.ResetFile();
  m_State = EngineState::Initialization;
  if (!BioGears::Initialize(config))
    return false;

	// We don't capture events during initialization
	m_Patient->ForwardEvents(nullptr);
	m_AnesthesiaMachine->ForwardEvents(nullptr);

	// Stabilize the engine to a resting state (with a standard meal and environment)
  if (!m_Config->HasStabilizationCriteria())
  {
    Error("BioGears needs stabilization criteria, none provided in configuration file");
    return false;
  }

  m_State = EngineState::InitialStabilization;
  if (!m_Config->GetStabilizationCriteria()->StabilizeRestingState(*this))
		return false;	
  // We need to process conditions here, so systems can prepare for them in their AtSteadyState method
  if (conditions != nullptr && !conditions->empty())
  {
    for (const SECondition* c : *conditions)
    {
      m_ss << "[Condition] " << *c;
      Info(m_ss);
      if (!m_Conditions->ProcessCondition(*c))
        return false;
    }
  }
  AtSteadyState(EngineState::AtInitialStableState);

  m_State = EngineState::SecondaryStabilization;
  // Apply conditions and anything else to the physiology
  // now that it's steady with provided patient, environment, and feedback
  if (conditions != nullptr && !conditions->empty())
  {// Now restabilize the patient with any conditions that were applied
   // Push conditions into condition manager
	  if (!m_Config->GetStabilizationCriteria()->StabilizeConditions(*this, *conditions))
      return false;
  }
  else
  {
    if (!m_Config->GetStabilizationCriteria()->StabilizeFeedbackState(*this))
      return false;
  }
  AtSteadyState(EngineState::AtSecondaryStableState);

  m_State = EngineState::Active;
  // Hook up the handlers (Note events will still be in the log)
  m_Patient->ForwardEvents(m_EventHandler);
  m_AnesthesiaMachine->ForwardEvents(m_EventHandler);
  Info("Finalizing homeostasis");

  // Run this again to clear out any bumps from systems resetting baselines in the last AtSteadyState call
  AdvanceModelTime(30, TimeUnit::s); // I would rather run Feedback stablization again, but...
  // This does not work for a few patients, they will not stay stable (???)  
  //if (!m_Config->GetStabilizationCriteria()->StabilizeFeedbackState(*this))
  //  return false;

  if (!m_Config->GetStabilizationCriteria()->IsTrackingStabilization())
    m_SimulationTime->SetValue(0, TimeUnit::s);  
  // Don't allow any changes to Quantity/Potential/Flux values directly
  // Use Quantity/Potential/Flux Sources
  m_Circuits->SetReadOnly(true);

	return true;
}

double BioGearsEngine::GetTimeStep(const TimeUnit& unit)
{
  return m_Config->GetTimeStep(unit);
}

double BioGearsEngine::GetSimulationTime(const TimeUnit& unit)
{
  return m_SimulationTime->GetValue(unit);
}

void BioGearsEngine::AdvanceModelTime()
{
  if (!IsReady())
    return;  
	if(m_Patient->IsEventActive(CDM::enumPatientEvent::IrreversibleState))
		return;	

	PreProcess();
	Process();
	PostProcess();

  m_Patient->UpdateEvents(m_Config->GetTimeStep());
  m_CurrentTime->Increment(m_Config->GetTimeStep());
  m_SimulationTime->Increment(m_Config->GetTimeStep());
}

void BioGearsEngine::AdvanceModelTime(double time, const TimeUnit& unit)
{
	double time_s = Convert(time,unit,TimeUnit::s);
  int count = (int)(time_s / m_Config->GetTimeStep(TimeUnit::s));
	for(int i=0;i<count;i++)
		AdvanceModelTime();
}

bool BioGearsEngine::ProcessAction(const SEAction& action)
{	
  if (!IsReady())
    return false;
  m_ss << "[Action] " << *m_SimulationTime << ", " << action;
	Info(m_ss);

  const SESerializeState* serialize = dynamic_cast<const SESerializeState*>(&action);
  if (serialize != nullptr)
  {
    if (serialize->GetType() == CDM::enumSerializationType::Save)
    {
      if (serialize->HasFilename())
        SaveState(serialize->GetFilename());
      else
      {
        std::stringstream ss;
        MKDIR("./states");
        ss << "./states/" << m_Patient->GetName() << "@" << GetSimulationTime(TimeUnit::s) << "s.xml";
        SaveState(ss.str());
      }     
    }
    else
      return LoadState(serialize->GetFilename());
    return true;
  }

	const SEPatientAssessmentRequest* patientAss = dynamic_cast<const SEPatientAssessmentRequest*>(&action);
	if (patientAss != nullptr)
	{
		switch (patientAss->GetType())
		{
			case CDM::enumPatientAssessment::PulmonaryFunctionTest:
			{
				SEPulmonaryFunctionTest pft(m_Logger);
				GetPatientAssessment(pft);

				// Write out the Assessement
				std::string pftFile = GetEngineTrack()->GetDataRequestManager().GetResultFilename();
				if (pftFile.empty())
					pftFile = "PulmonaryFunctionTest";
				m_ss << "PFT@" << GetSimulationTime(TimeUnit::s) << "s";
				pftFile = Replace(pftFile, "Results", m_ss.str());
				pftFile = Replace(pftFile, ".txt", ".xml");
				m_ss << "PulmonaryFunctionTest@" << GetSimulationTime(TimeUnit::s) << "s.xml";
				std::ofstream stream(pftFile);
				m_ss.str("");
				m_ss.clear();
				// Write out the xml file
				xml_schema::namespace_infomap map;
				map[""].name = "uri:/mil/tatrc/phsyiology/datamodel";
				map[""].schema = "BioGears.xsd";
        std::unique_ptr<CDM::PulmonaryFunctionTestData> unloaded(pft.Unload());
        unloaded->contentVersion(BGE::Version);
        PulmonaryFunctionTest(stream, *unloaded, map);
        stream.close();
				break;
			}
			case CDM::enumPatientAssessment::Urinalysis:
			{
				SEUrinalysis upan(m_Logger);
				GetPatientAssessment(upan);

				std::string upanFile = GetEngineTrack()->GetDataRequestManager().GetResultFilename();
				if (upanFile.empty())
					upanFile = "Urinalysis";
        m_ss << "Urinalysis@" << GetSimulationTime(TimeUnit::s) << "s";
				upanFile = Replace(upanFile, "Results", m_ss.str());
				upanFile = Replace(upanFile, ".txt", ".xml");
        m_ss << "Urinalysis@" << GetSimulationTime(TimeUnit::s) << "s.xml";
				std::ofstream stream(upanFile);
				m_ss.str("");
				m_ss.clear();
				// Write out the xml file
				xml_schema::namespace_infomap map;
				map[""].name = "uri:/mil/tatrc/phsyiology/datamodel";
        std::unique_ptr<CDM::UrinalysisData> unloaded(upan.Unload());
        unloaded->contentVersion(BGE::Version);
				Urinalysis(stream, *unloaded, map);
        stream.close();
				break;
			}

			case CDM::enumPatientAssessment::CompleteBloodCount:
			{
				SECompleteBloodCount cbc(m_Logger);
				GetPatientAssessment(cbc);
				std::string cbcFile = GetEngineTrack()->GetDataRequestManager().GetResultFilename();
				if (cbcFile.empty())
					cbcFile = "CompleteBloodCount";
        m_ss << "CBC@" << GetSimulationTime(TimeUnit::s) << "s";
				cbcFile = Replace(cbcFile, "Results", m_ss.str());
				cbcFile = Replace(cbcFile, ".txt", ".xml");
        m_ss << "CompleteBloodCount@" << GetSimulationTime(TimeUnit::s) << "s.xml";
				std::ofstream stream(cbcFile);
				m_ss.str("");
				m_ss.clear();
				// Write out the xml file
				xml_schema::namespace_infomap map;
				map[""].name = "uri:/mil/tatrc/phsyiology/datamodel";
        std::unique_ptr<CDM::CompleteBloodCountData> unloaded(cbc.Unload());
        unloaded->contentVersion(BGE::Version);
        CompleteBloodCount(stream, *unloaded, map);
        stream.close();
				break;
			}

			case CDM::enumPatientAssessment::ComprehensiveMetabolicPanel:
			{
				SEComprehensiveMetabolicPanel mp(m_Logger);
				GetPatientAssessment(mp);
				std::string mpFile = GetEngineTrack()->GetDataRequestManager().GetResultFilename();
				if (mpFile.empty())
					mpFile = "ComprehensiveMetabolicPanel";
        m_ss << "CMP@" << GetSimulationTime(TimeUnit::s) << "s";
				mpFile = Replace(mpFile, "Results", m_ss.str());
				mpFile = Replace(mpFile, ".txt", ".xml");
        m_ss << "ComprehensiveMetabolicPanel@" << GetSimulationTime(TimeUnit::s) << "s.xml";
				std::ofstream stream(mpFile);
				m_ss.str("");
				m_ss.clear();
				// Write out the xml file
				xml_schema::namespace_infomap map;
				map[""].name = "uri:/mil/tatrc/phsyiology/datamodel";
        std::unique_ptr<CDM::ComprehensiveMetabolicPanelData> unloaded(mp.Unload());
        unloaded->contentVersion(BGE::Version);
        ComprehensiveMetabolicPanel(stream, *mp.Unload(), map);
        stream.close();
				break;
			}
			default:
			{
				m_ss << "Unsupported assessment request " << patientAss->GetType();
				Error(m_ss);
				return false;
			}
		}
		return true;
	}

	return GetActions().ProcessAction(action);  
}



bool BioGearsEngine::IsReady()
{
  if (m_State == EngineState::NotReady)
  {
    Error("Engine is not ready to process, Initialize the engine or Load a state.");
    return false;
  }  
  return true;
}

void BioGearsEngine::SetEventHandler(SEEventHandler* handler)
{
  m_EventHandler = handler;
  if (m_Patient != nullptr)
    m_Patient->ForwardEvents(m_EventHandler);
  m_AnesthesiaMachine->ForwardEvents(m_EventHandler);
}

const PhysiologyEngineConfiguration* BioGearsEngine::GetConfiguration()
{
  return &BioGears::GetConfiguration();
}

const SEPatient&  BioGearsEngine::GetPatient()
{
	return BioGears::GetPatient();
}

bool BioGearsEngine::GetPatientAssessment(SEPatientAssessment& assessment)
{
  if (!IsReady())
    return false;
	return BioGears::GetPatientAssessment(assessment);
}

const SEEnvironment* BioGearsEngine::GetEnvironment()
{
	return &BioGears::GetEnvironment();
}

SESubstanceManager& BioGearsEngine::GetSubstanceManager()
{
	return *m_Substances;
}

const SEBloodChemistrySystem* BioGearsEngine::GetBloodChemistrySystem()
{
	return &BioGears::GetBloodChemistry();
}

const SECardiovascularSystem* BioGearsEngine::GetCardiovascularSystem()
{
	return &BioGears::GetCardiovascular();
}

const SEDrugSystem* BioGearsEngine::GetDrugSystem()
{
  return &BioGears::GetDrugs();
}

const SEEndocrineSystem* BioGearsEngine::GetEndocrineSystem()
{
	return &BioGears::GetEndocrine();
}

const SEEnergySystem* BioGearsEngine::GetEnergySystem()
{
	return &BioGears::GetEnergy();
}

const SEGastrointestinalSystem* BioGearsEngine::GetGastrointestinalSystem()
{
	return &BioGears::GetGastrointestinal();
}

const SEHepaticSystem* BioGearsEngine::GetHepaticSystem()
{
  return &BioGears::GetHepatic();
}

const SENervousSystem* BioGearsEngine::GetNervousSystem()
{
  return &BioGears::GetNervous();
}

const SERenalSystem* BioGearsEngine::GetRenalSystem()
{
	return &BioGears::GetRenal();
}

const SERespiratorySystem* BioGearsEngine::GetRespiratorySystem()
{
	return &BioGears::GetRespiratory();
}

const SETissueSystem* BioGearsEngine::GetTissueSystem()
{
  return &BioGears::GetTissue();
}


const SEAnesthesiaMachine* BioGearsEngine::GetAnesthesiaMachine()
{
	return &BioGears::GetAnesthesiaMachine();
}

const SEElectroCardioGram* BioGearsEngine::GetElectroCardioGram()
{
	return &BioGears::GetECG();
}

const SEInhaler* BioGearsEngine::GetInhaler()
{
	return &BioGears::GetInhaler();
}

const SECompartmentManager& BioGearsEngine::GetCompartments()
{
	return BioGears::GetCompartments();
}
