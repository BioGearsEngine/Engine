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
#include "properties/SEScalarVolumePerTimeMass.h"

const VolumePerTimeMassUnit VolumePerTimeMassUnit::L_Per_s_g("L/s g");
const VolumePerTimeMassUnit VolumePerTimeMassUnit::mL_Per_s_g("mL / s g");
const VolumePerTimeMassUnit VolumePerTimeMassUnit::mL_Per_min_kg("mL/min kg");
const VolumePerTimeMassUnit VolumePerTimeMassUnit::mL_Per_s_kg("mL/s kg");
const VolumePerTimeMassUnit VolumePerTimeMassUnit::uL_Per_min_kg("uL/min kg");

CDM::ScalarVolumePerTimeMassData* SEScalarVolumePerTimeMass::Unload() const
{
  if (!IsValid())
    return nullptr;
  CDM::ScalarVolumePerTimeMassData* data(new CDM::ScalarVolumePerTimeMassData());
  SEScalarQuantity::Unload(*data);
  return data;
}

bool VolumePerTimeMassUnit::IsValidUnit(const std::string& unit)
{
  if (L_Per_s_g.GetString().compare(unit) == 0)
    return true;
  if (mL_Per_s_g.GetString().compare(unit) == 0)
    return true;
  if (mL_Per_min_kg.GetString().compare(unit) == 0)
    return true;
  if (mL_Per_s_kg.GetString().compare(unit) == 0)
    return true;
  if (uL_Per_min_kg.GetString().compare(unit) == 0)
	  return true;
  return false;
}

const VolumePerTimeMassUnit& VolumePerTimeMassUnit::GetCompoundUnit(const std::string& unit)
{
  if (L_Per_s_g.GetString().compare(unit) == 0)
    return L_Per_s_g;
  if (mL_Per_s_g.GetString().compare(unit) == 0)
    return mL_Per_s_g;
  if (mL_Per_min_kg.GetString().compare(unit) == 0)
    return mL_Per_min_kg;
  if (mL_Per_s_kg.GetString().compare(unit) == 0)
    return mL_Per_s_kg;
  if (uL_Per_min_kg.GetString().compare(unit) == 0)
	  return uL_Per_min_kg;
  std::stringstream err;
  err << unit << " is not a valid VolumePerTimeMass unit";
  throw CommonDataModelException(err.str());
}