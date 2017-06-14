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

class SEAction;
class SEScenario;
class PhysiologyEngine;
CDM_BIND_DECL(ScenarioData)

class DLL_DECL SEScenarioCustomExec
{
public:
	virtual void CustomExec(double time_s, PhysiologyEngine* engine)=0;
};

class DLL_DECL SEScenarioExec : public Loggable
{
public:
	SEScenarioExec(PhysiologyEngine& engine);
	virtual ~SEScenarioExec();

	virtual PhysiologyEngine& GetEngine(){ return m_Engine; }

	virtual void Cancel();

	virtual bool Execute(const std::string& scenarioFile, const std::string& resultsFile, SEScenarioCustomExec* cExec = nullptr);
	virtual bool Execute(const SEScenario& scenario,      const std::string& resultsFile, SEScenarioCustomExec* cExec = nullptr);
	
protected:

	virtual bool ProcessActions(const SEScenario& scenario);
	/// This does not include advance time actions
	/// To override default functionality with those 
	/// actions override the ProcessActions method
	virtual bool ProcessAction(const SEAction& action);

	bool                  m_Cancel;
	SEScenarioCustomExec* m_CustomExec;
	PhysiologyEngine&     m_Engine;

	std::stringstream     m_ss;
};