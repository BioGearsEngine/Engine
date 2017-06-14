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
#include "bind/ScalarFlowResistanceData.hxx"

class DLL_DECL FlowResistanceUnit : public CCompoundUnit
{
public:
  FlowResistanceUnit(const std::string& u) : CCompoundUnit(u) {}
  virtual ~FlowResistanceUnit() {}

  static bool IsValidUnit(const std::string& unit);
  static const FlowResistanceUnit& GetCompoundUnit(const std::string& unit);

  static const FlowResistanceUnit cmH2O_s_Per_L;
  static const FlowResistanceUnit mmHg_s_Per_mL;
  static const FlowResistanceUnit mmHg_min_Per_mL;
  static const FlowResistanceUnit mmHg_min_Per_L;
  static const FlowResistanceUnit Pa_s_Per_m3;
};

class DLL_DECL SEScalarFlowResistance : public SEScalarQuantity<FlowResistanceUnit>
{
public:
  SEScalarFlowResistance() {}
	virtual ~SEScalarFlowResistance() {}

  CDM::ScalarFlowResistanceData* Unload() const;
};
