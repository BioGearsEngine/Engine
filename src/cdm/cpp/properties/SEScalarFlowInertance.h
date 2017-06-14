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
#include "properties/SEScalar.h"
#include "bind/ScalarFlowInertanceData.hxx"

class DLL_DECL FlowInertanceUnit : public CCompoundUnit
{
public:
  FlowInertanceUnit(const std::string& u) : CCompoundUnit(u) {}
  virtual ~FlowInertanceUnit() {}

  static bool IsValidUnit(const std::string& unit);
  static const FlowInertanceUnit& GetCompoundUnit(const std::string& unit);

  static const FlowInertanceUnit mmHg_s2_Per_mL;
  static const FlowInertanceUnit mmHg_s2_Per_L;
  static const FlowInertanceUnit cmH2O_s2_Per_mL;
  static const FlowInertanceUnit cmH2O_s2_Per_L;
  static const FlowInertanceUnit Pa_s2_Per_m3;
};

class DLL_DECL SEScalarFlowInertance : public SEScalarQuantity<FlowInertanceUnit>
{
public:
  SEScalarFlowInertance() {}
	virtual ~SEScalarFlowInertance() {}

  CDM::ScalarFlowInertanceData* Unload() const;
};
