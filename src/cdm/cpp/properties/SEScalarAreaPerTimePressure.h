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
#include "bind/ScalarAreaPerTimePressureData.hxx"

class DLL_DECL AreaPerTimePressureUnit : public CCompoundUnit
{
public:
  AreaPerTimePressureUnit(const std::string& u) : CCompoundUnit(u) {}
  virtual ~AreaPerTimePressureUnit() {}

  static bool IsValidUnit(const std::string& unit);
  static const AreaPerTimePressureUnit& GetCompoundUnit(const std::string& unit);

  static const AreaPerTimePressureUnit m2_Per_s_mmHg;
  static const AreaPerTimePressureUnit cm2_Per_s_mmHg;
  static const AreaPerTimePressureUnit m2_Per_min_mmHg;
  static const AreaPerTimePressureUnit cm2_Per_min_mmHg;
};

class DLL_DECL SEScalarAreaPerTimePressure : public SEScalarQuantity<AreaPerTimePressureUnit>
{
public:
  SEScalarAreaPerTimePressure() {}
	virtual ~SEScalarAreaPerTimePressure() {}

  CDM::ScalarAreaPerTimePressureData* Unload() const;
};
