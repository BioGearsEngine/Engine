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
#include "patient/conditions/SEPatientCondition.h"
#include "bind/DiabetesType1Data.hxx"

class DLL_DECL SEDiabetesType1 : public SEPatientCondition
{
public:

  SEDiabetesType1();
	virtual ~SEDiabetesType1();

	virtual void Clear(); //clear memory

	virtual bool IsValid() const;

  virtual bool Load(const CDM::DiabetesType1Data& in);
  virtual CDM::DiabetesType1Data* Unload() const;
protected:
  virtual void Unload(CDM::DiabetesType1Data& data) const;

public:
	virtual std::string GetName() const{ return "DiabetesType1"; }

	virtual bool HasInsulinProductionSeverity() const;
	virtual SEScalar0To1& GetInsulinProductionSeverity();

	virtual void ToString(std::ostream &str) const;

protected:
	SEScalar0To1*           m_InsulinProductionSeverity;
};      