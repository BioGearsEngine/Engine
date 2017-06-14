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

#include "SEInhalerAction.h"
#include "bind/InhalerData.hxx"
#include "bind/InhalerConfigurationData.hxx"

class SEInhaler;
class SESubstanceManager;

class DLL_DECL SEInhalerConfiguration : public SEInhalerAction
{
public:

  SEInhalerConfiguration(SESubstanceManager& substances);
	virtual ~SEInhalerConfiguration();

  virtual void Clear();
  
	virtual bool IsValid() const;

	virtual bool Load(const CDM::InhalerConfigurationData& in);
  virtual CDM::InhalerConfigurationData* Unload() const;
protected:
  virtual void Unload(CDM::InhalerConfigurationData& data) const;
public:

	bool HasConfiguration() const;
	SEInhaler& GetConfiguration();
	const SEInhaler* GetConfiguration() const;

	virtual std::string GetConfigurationFile() const;
	virtual void SetConfigurationFile(const std::string& fileName);
	virtual bool HasConfigurationFile() const;
	virtual void InvalidateConfigurationFile();

	virtual void ToString(std::ostream &str) const;

protected:
	SESubstanceManager&  m_Substances;

	std::string m_ConfigurationFile;
	SEInhaler*  m_Configuration;
};
