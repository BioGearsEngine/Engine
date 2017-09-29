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
#include "patient/actions/SEPatientAction.h"
#include "bind/HemorrhageData.hxx"







class DLL_DECL SEHemorrhage : public SEPatientAction
{
public:

	SEHemorrhage();
	virtual ~SEHemorrhage();

	virtual void Clear(); //clear memory

	virtual bool IsValid() const;
	virtual bool IsActive() const;

  virtual bool Load(const CDM::HemorrhageData& in);
  virtual CDM::HemorrhageData* Unload()const;
protected:
  virtual void Unload(CDM::HemorrhageData& data) const;

public:

	virtual std::string GetCompartment() const;
	virtual void SetCompartment(const std::string& name);
	virtual bool HasCompartment() const;
	virtual void InvalidateCompartment();
	virtual void ProcessMCIS();
	
	virtual bool HasMCIS() const;
	virtual std::vector<unsigned int>& GetMCIS() { return m_MCIS; }
	virtual void SetMCIS(const std::vector<unsigned int>& mcisIn);

	virtual bool HasBleedName() const;
	virtual std::string GetBleedName() const;
	virtual void SetBleedName(const std::string& name);

	
	virtual void ToString(std::ostream &str) const;

protected:
	std::string             m_Compartment;
	std::vector<unsigned int>		m_MCIS;
	std::string				m_BleedName;
	std::map<std::vector<int>, std::pair<std::string, std::string>> organMap;

	enum region { Head = 1, Torso = 2, Arms = 3, Legs = 4 }; //mcis digit 2
};    