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
#include "bind/DataRequestData.hxx"
#include "properties/SEDecimalFormat.h"
class SESubstanceManager;
class CCompoundUnit;
class SEDecimalFormat;
class SEDataRequestManager;

class DLL_DECL SEDataRequest : public SEDecimalFormat
{
  friend class SEDataRequestManager;
protected:
  SEDataRequest(const SEDecimalFormat* dfault = nullptr);
public:

	virtual ~SEDataRequest();

	virtual void Clear(); //clear memory

  virtual bool Load(const CDM::DataRequestData& in);
	virtual CDM::DataRequestData* Unload() const;
protected:
  virtual void Unload(CDM::DataRequestData& data) const;
public:

	virtual size_t HashCode() const;

	// The System Property Name
	virtual std::string GetName() const;
	virtual void SetName(const std::string& name);
	virtual bool HasName() const;
	virtual void InvalidateName();

	// The Requested Unit String
	virtual std::string GetRequestedUnit() const;
	virtual void SetRequestedUnit(const std::string& unit);
	virtual bool HasRequestedUnit() const;
	virtual void InvalidateRequestedUnit();

	// The Unit that data will be retrieved in
	// If the Requested unit is present, this should be set to that unit
	// I cannot do this automatically, since I don't know what the quantity of the property is, so some other logic needs to complete this logic
	// If the Requested unit is NOT present, this should be set to whatever the unit of the System Property is set as
	virtual const CCompoundUnit* GetUnit() const;
	virtual void SetUnit(const CCompoundUnit& unit);
	virtual bool HasUnit() const;
	virtual void InvalidateUnit();

	virtual void Set(const std::string& name, const std::string&unit = "");
	virtual void Set(const std::string& name, const CCompoundUnit& unit);

protected:

	std::string m_Name;
	std::string m_RequestedUnit;
	const CCompoundUnit* m_Unit;
};                  