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
#include "bind/ScalarMassPerTimeData.hxx"

class DLL_DECL MassPerTimeUnit : public CCompoundUnit
{
public:
  MassPerTimeUnit(const std::string& u) : CCompoundUnit(u) {}
  virtual ~MassPerTimeUnit() {}

  static bool IsValidUnit(const std::string& unit);
  static const MassPerTimeUnit& GetCompoundUnit(const std::string& unit);

  static const MassPerTimeUnit g_Per_s;
  static const MassPerTimeUnit g_Per_min;
  static const MassPerTimeUnit g_Per_day;
  static const MassPerTimeUnit mg_Per_s;
  static const MassPerTimeUnit mg_Per_min;
  static const MassPerTimeUnit ug_Per_min;
  static const MassPerTimeUnit ug_Per_s;
  static const MassPerTimeUnit kg_Per_s;
};

class DLL_DECL SEScalarMassPerTime : public SEScalarQuantity<MassPerTimeUnit>
{
public:
  SEScalarMassPerTime() {}
	virtual ~SEScalarMassPerTime() {}

  CDM::ScalarMassPerTimeData* Unload() const;
};
