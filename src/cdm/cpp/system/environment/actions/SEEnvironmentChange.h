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
#include "system/environment/SEEnvironmentalConditions.h"
#include "system/environment/actions/SEEnvironmentAction.h"
#include "bind/EnvironmentChangeData.hxx"

class DLL_DECL SEEnvironmentChange : public SEEnvironmentAction
{
public:

	SEEnvironmentChange(SESubstanceManager& substances);
	virtual ~SEEnvironmentChange();

  virtual void Clear();

	virtual bool IsValid() const;

	virtual bool Load(const CDM::EnvironmentChangeData& in);
  virtual CDM::EnvironmentChangeData* Unload() const;
protected:
  virtual void Unload(CDM::EnvironmentChangeData& data) const;

public:
	
	virtual bool HasConditions() const;
	virtual SEEnvironmentalConditions& GetConditions();
	virtual const SEEnvironmentalConditions* GetConditions() const;

	virtual std::string GetConditionsFile() const;
	virtual void SetConditionsFile(const std::string& fileName);
	virtual bool HasConditionsFile() const;
	virtual void InvalidateConditionsFile();
	
	virtual void ToString(std::ostream &str) const;

protected:
	SESubstanceManager&  m_Substances;

	std::string                m_ConditionsFile;
	SEEnvironmentalConditions* m_Conditions;
};  
