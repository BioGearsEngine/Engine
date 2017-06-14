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
#include "bind/ScalarPressurePerVolumeData.hxx"

class DLL_DECL PressurePerVolumeUnit : public CCompoundUnit
{
public:
  PressurePerVolumeUnit(const std::string& u) : CCompoundUnit(u) {}
  virtual ~PressurePerVolumeUnit() {}

  static bool IsValidUnit(const std::string& unit);
  static const PressurePerVolumeUnit& GetCompoundUnit(const std::string& unit);

  static const PressurePerVolumeUnit mmHg_Per_mL;
  static const PressurePerVolumeUnit cmH2O_Per_mL;
};

class DLL_DECL SEScalarPressurePerVolume : public SEScalarQuantity<PressurePerVolumeUnit>
{
public:
  SEScalarPressurePerVolume() {}
	virtual ~SEScalarPressurePerVolume() {}

  CDM::ScalarPressurePerVolumeData* Unload() const;
};
