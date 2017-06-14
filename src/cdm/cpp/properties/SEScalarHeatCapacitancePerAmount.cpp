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

#include "stdafx.h"
#include "properties/SEScalarHeatCapacitancePerAmount.h"

const HeatCapacitancePerAmountUnit HeatCapacitancePerAmountUnit::J_Per_K_mol("J/K mol");

CDM::ScalarHeatCapacitancePerAmountData* SEScalarHeatCapacitancePerAmount::Unload() const
{
  if (!IsValid())
    return nullptr;
  CDM::ScalarHeatCapacitancePerAmountData* data(new CDM::ScalarHeatCapacitancePerAmountData());
  SEScalarQuantity::Unload(*data);
  return data;
}

bool HeatCapacitancePerAmountUnit::IsValidUnit(const std::string& unit)
{
  if (J_Per_K_mol.GetString().compare(unit) == 0)
    return true;
  return false;
}

const HeatCapacitancePerAmountUnit& HeatCapacitancePerAmountUnit::GetCompoundUnit(const std::string& unit)
{
  if (J_Per_K_mol.GetString().compare(unit) == 0)
    return J_Per_K_mol;
  std::stringstream err;
  err << unit << " is not a valid HeatCapacitancePerAmount unit";
  throw CommonDataModelException(err.str());
}