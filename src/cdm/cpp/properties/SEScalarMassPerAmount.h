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
#include "bind/ScalarMassPerAmountData.hxx"

class DLL_DECL MassPerAmountUnit : public CCompoundUnit
{
public:
  MassPerAmountUnit(const std::string& u) : CCompoundUnit(u) {}
  virtual ~MassPerAmountUnit() {}

  static bool IsValidUnit(const std::string& unit);
  static const MassPerAmountUnit& GetCompoundUnit(const std::string& unit);

  static const MassPerAmountUnit g_Per_ct;
  static const MassPerAmountUnit g_Per_mol;
  static const MassPerAmountUnit g_Per_mmol;
  static const MassPerAmountUnit g_Per_umol;
  static const MassPerAmountUnit mg_Per_mol;
  static const MassPerAmountUnit mg_Per_mmol;
  static const MassPerAmountUnit kg_Per_mol;
  static const MassPerAmountUnit ug_Per_mol;
  static const MassPerAmountUnit ug_Per_mmol;
  static const MassPerAmountUnit ug_Per_ct;
  static const MassPerAmountUnit pg_Per_ct;
};

class DLL_DECL SEScalarMassPerAmount : public SEScalarQuantity<MassPerAmountUnit>
{
public:
  SEScalarMassPerAmount() {}
	virtual ~SEScalarMassPerAmount() {}

  CDM::ScalarMassPerAmountData* Unload() const;
};

