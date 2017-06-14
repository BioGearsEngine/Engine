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
#include "scenario/requests/SECompartmentDataRequest.h"
#include "bind/CompartmentSubstanceDataRequestData.hxx"
class SESubstance;
class SESubstanceManager;
class SEDataRequestManager;

class DLL_DECL SECompartmentSubstanceDataRequest : public SECompartmentDataRequest
{
  friend class SEDataRequestManager;
protected:
  SECompartmentSubstanceDataRequest(const SEDecimalFormat* dfault = nullptr);
public:
  virtual ~SECompartmentSubstanceDataRequest();

	virtual void Clear(); //clear memory

  virtual bool Load(const CDM::CompartmentSubstanceDataRequestData& in, const SESubstanceManager& substances);
  virtual CDM::CompartmentSubstanceDataRequestData* Unload() const = 0;
protected:
  virtual void Unload(CDM::CompartmentSubstanceDataRequestData& data) const;

public:

	virtual size_t HashCode();

	virtual SESubstance* GetSubstance() const;
	virtual void SetSubstance(SESubstance* sub);
	virtual bool HasSubstance() const;
	virtual void InvalidateSubstance();

  using SECompartmentDataRequest::Set;
	virtual void Set(const std::string& cmpt, SESubstance& substance, const std::string& name, const std::string&unit = "");
	virtual void Set(const std::string& cmpt, SESubstance& substance, const std::string& name, const CCompoundUnit& unit);


protected:
	SESubstance*                    m_Substance;

};                  