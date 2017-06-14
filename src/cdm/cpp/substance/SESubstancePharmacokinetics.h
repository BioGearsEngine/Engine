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
CDM_BIND_DECL(SubstancePharmacokineticsData)
#include "bind/enumSubstanceIonicState.hxx"
#include "bind/enumSubstanceBindingProtein.hxx"
#include "substance/SESubstancePhysicochemicals.h"
#include "substance/SESubstanceTissuePharmacokinetics.h"

class DLL_DECL SESubstancePharmacokinetics : public Loggable
{
public:

	SESubstancePharmacokinetics(Logger* logger);
	virtual ~SESubstancePharmacokinetics();

  virtual void Clear();
  virtual bool IsValid() const;

	virtual const SEScalar* GetScalar(const std::string& name);

	virtual bool Load(const CDM::SubstancePharmacokineticsData& in);
	virtual CDM::SubstancePharmacokineticsData* Unload() const;
protected:
	virtual void Unload(CDM::SubstancePharmacokineticsData& data) const;

public:
  virtual bool HasPhysicochemicals() const;
  virtual SESubstancePhysicochemicals& GetPhysicochemicals();
  virtual const SESubstancePhysicochemicals* GetPhysicochemicals() const;

  virtual bool HasTissueKinetics() const;
  virtual bool HasTissueKinetics(const std::string& name) const;
  virtual SESubstanceTissuePharmacokinetics& GetTissueKinetics(const std::string& name);
  virtual const SESubstanceTissuePharmacokinetics* GetTissueKinetics(const std::string& name) const;
  virtual void RemoveTissueKinetics(const std::string& name);

protected: 

  SESubstancePhysicochemicals*                              m_Physicochemicals;
  std::map<std::string, SESubstanceTissuePharmacokinetics*> m_TissueKinetics;

};