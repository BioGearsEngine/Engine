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
#include "bind/ScalarEnergyData.hxx"

class DLL_DECL EnergyUnit : public CCompoundUnit
{
public:
  EnergyUnit(const std::string& u) : CCompoundUnit(u) {}
  virtual ~EnergyUnit() {}

  static bool IsValidUnit(const std::string& unit);
  static const EnergyUnit& GetCompoundUnit(const std::string& unit);

  static const EnergyUnit J;
  static const EnergyUnit mJ;
  static const EnergyUnit kJ;
  static const EnergyUnit kcal;
};

class DLL_DECL SEScalarEnergy : public SEScalarQuantity<EnergyUnit>
{
public:
  SEScalarEnergy() {}
	virtual ~SEScalarEnergy() {}

  CDM::ScalarEnergyData* Unload() const;
};
