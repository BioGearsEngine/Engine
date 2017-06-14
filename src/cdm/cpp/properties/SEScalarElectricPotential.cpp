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
#include "properties/SEScalarElectricPotential.h"

const ElectricPotentialUnit ElectricPotentialUnit::V("V");
const ElectricPotentialUnit ElectricPotentialUnit::mV("mV");

CDM::ScalarElectricPotentialData* SEScalarElectricPotential::Unload() const
{
	if(!IsValid())
		return nullptr;
	CDM::ScalarElectricPotentialData* data(new CDM::ScalarElectricPotentialData());
  SEScalarQuantity::Unload(*data);
	return data;
}

bool ElectricPotentialUnit::IsValidUnit(const std::string& unit)
{
  if (V.GetString().compare(unit) == 0)
    return true;
  if (mV.GetString().compare(unit) == 0)
    return true;
  return false;
}

const ElectricPotentialUnit& ElectricPotentialUnit::GetCompoundUnit(const std::string& unit)
{
	if(V.GetString().compare(unit)==0)
		return V;
	if(mV.GetString().compare(unit)==0)
		return mV;
  std::stringstream err;
  err << unit << " is not a valid Electric Potential unit";
  throw CommonDataModelException(err.str());
}