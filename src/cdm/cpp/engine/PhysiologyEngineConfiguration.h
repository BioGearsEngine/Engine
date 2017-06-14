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

#pragma once
class SEElectroCardioGramInterpolator;
class PhysiologyEngineStabilization;
class PhysiologyEngineTimedStabilization;
class PhysiologyEngineDynamicStabilization;
#include "utils/GeneralMath.h"
#include "bind/enumOnOff.hxx"

CDM_BIND_DECL(PhysiologyEngineConfigurationData)

class DLL_DECL PhysiologyEngineConfiguration : public Loggable
{
public:

	PhysiologyEngineConfiguration(Logger* logger);
	virtual ~PhysiologyEngineConfiguration();

	virtual void Clear();

	virtual void Merge(const PhysiologyEngineConfiguration& from);

	virtual bool Load(const CDM::PhysiologyEngineConfigurationData& in);
	virtual CDM::PhysiologyEngineConfigurationData* Unload() const;
protected:
	void Unload(CDM::PhysiologyEngineConfigurationData& data) const;

public:
	virtual bool LoadFile(const std::string& file);

  virtual bool HasECGInterpolator() const;
  virtual SEElectroCardioGramInterpolator& GetECGInterpolator();
  virtual const SEElectroCardioGramInterpolator* GetECGInterpolator() const;
  virtual void RemoveECGInterpolator();

  // You can have either timed or dynamic stabilization criteria
  virtual bool HasStabilizationCriteria() const;
  virtual PhysiologyEngineStabilization* GetStabilizationCriteria();
	virtual void RemoveStabilizationCriteria();
  // Timed Methods, If you have dynamic, calling GetTimedStabilizationCriteria will remove the dynamic object
  virtual bool HasTimedStabilizationCriteria() const;
  virtual PhysiologyEngineTimedStabilization& GetTimedStabilizationCriteria();
  virtual const PhysiologyEngineTimedStabilization* GetTimedStabilizationCriteria() const;
  virtual void RemoveTimedStabilizationCriteria();
  // Dynamic Methods, If you have timed, calling GetDynamicStabilizationCriteria will remove the timed object
  virtual bool HasDynamicStabilizationCriteria() const;
  virtual PhysiologyEngineDynamicStabilization& GetDynamicStabilizationCriteria();
  virtual const PhysiologyEngineDynamicStabilization* GetDynamicStabilizationCriteria() const;
  virtual void RemoveDynamicStabilizationCriteria();

  
  virtual bool HasTimeStep() const;
  virtual SEScalarTime& GetTimeStep();
  virtual double GetTimeStep(const TimeUnit& unit) const;

  virtual bool HasWritePatientBaselineFile() const { return m_WritePatientBaselineFile != (CDM::enumOnOff::value) - 1; }
  virtual bool WritePatientBaselineFile() const { return m_WritePatientBaselineFile == CDM::enumOnOff::On; }
  virtual void SetWritePatientBaselineFile(CDM::enumOnOff::value v) { m_WritePatientBaselineFile = v; }

protected:
  bool                                     m_Merge;
  SEElectroCardioGramInterpolator*         m_ECGInterpolator;

  PhysiologyEngineStabilization*           m_StabilizationCriteria;
  PhysiologyEngineTimedStabilization*      m_TimedStabilizationCriteria;
  PhysiologyEngineDynamicStabilization*    m_DynamicStabilizationCriteria;
  SEScalarTime*				                     m_TimeStep;  
  CDM::enumOnOff::value                    m_WritePatientBaselineFile;
};