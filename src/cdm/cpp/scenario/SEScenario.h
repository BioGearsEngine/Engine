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
#include "bind/DataRequestsData.hxx"
#include "bind/ScenarioData.hxx"
class SESubstanceManager;
class SEAction;
class SEDataRequest;
class SEScenarioInitialParameters;
class SEScenarioAutoSerialization;
class SEDecimalFormat;
#include "scenario/requests/SEDataRequestManager.h"

class DLL_DECL SEScenario : public Loggable
{
public:

  SEScenario(SESubstanceManager& subMgr);
	virtual ~SEScenario();
	
	virtual void Clear(); //clear memory

  bool Load(const CDM::ScenarioData& in);
	CDM::ScenarioData* Unload() const;
protected:
	void Unload(CDM::ScenarioData& data)const;

public:
  bool LoadFile(const std::string& scenarioFile);
	bool IsValid() const;

	virtual std::string GetName() const;
	virtual void SetName(const std::string& name);
	virtual bool HasName() const;
	virtual void InvalidateName();

	virtual std::string GetDescription() const;
	virtual void SetDescription(const std::string& desc);
	virtual bool HasDescription() const;
	virtual void InvalidateDescription();

  virtual std::string GetEngineStateFile() const;
  virtual void SetEngineStateFile(const std::string& file);
  virtual bool HasEngineStateFile() const;
  virtual void InvalidateEngineStateFile();
	
  virtual SEScenarioInitialParameters& GetInitialParameters();
  virtual const SEScenarioInitialParameters* GetInitialParameters() const;
  virtual bool HasInitialParameters() const;
  virtual void InvalidateInitialParameters();

  virtual bool HasAutoSerialization() const;
  virtual SEScenarioAutoSerialization& GetAutoSerialization();
  virtual const SEScenarioAutoSerialization* GetAutoSerialization() const;
  virtual void RemoveAutoSerialization();

  virtual void AddAction(const SEAction& action);
  virtual const std::vector<SEAction*>& GetActions() const;

  virtual SEDataRequestManager& GetDataRequestManager() { return m_DataRequestMgr; }
  virtual const SEDataRequestManager& GetDataRequestManager() const { return m_DataRequestMgr; }


protected:
  SESubstanceManager&                         m_SubMgr;
	std::string                                 m_Name;
	std::string                                 m_Description;
  std::string                                 m_EngineStateFile;
  SEScenarioInitialParameters*                m_InitialParameters;
  SEScenarioAutoSerialization*                m_AutoSerialization;
  SEDataRequestManager                        m_DataRequestMgr;
  std::vector<SEAction*>                      m_Actions;

};