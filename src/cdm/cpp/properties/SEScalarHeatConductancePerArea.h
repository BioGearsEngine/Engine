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
#include "bind/ScalarHeatConductancePerAreaData.hxx"

class DLL_DECL HeatConductancePerAreaUnit : public CCompoundUnit
{
public:
  HeatConductancePerAreaUnit(const std::string& u) : CCompoundUnit(u) {}
  virtual ~HeatConductancePerAreaUnit() {}

  static bool IsValidUnit(const std::string& unit);
  static const HeatConductancePerAreaUnit& GetCompoundUnit(const std::string& unit);

  static const HeatConductancePerAreaUnit W_Per_m2_K;
  static const HeatConductancePerAreaUnit W_Per_m2_C;
  static const HeatConductancePerAreaUnit BTU_Per_hr_ft2_C;
};

class DLL_DECL SEScalarHeatConductancePerArea : public SEScalarQuantity<HeatConductancePerAreaUnit>
{
public:
  SEScalarHeatConductancePerArea() {}
	virtual ~SEScalarHeatConductancePerArea() {}

  CDM::ScalarHeatConductancePerAreaData* Unload() const;
};