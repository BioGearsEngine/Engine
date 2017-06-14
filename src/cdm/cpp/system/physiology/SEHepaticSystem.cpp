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
#include "system/physiology/SEHepaticSystem.h"
#include "substance/SESubstanceManager.h"

SEHepaticSystem::SEHepaticSystem(Logger* logger) : SESystem(logger)
{
	
}

SEHepaticSystem::~SEHepaticSystem()
{
  Clear();
}

void SEHepaticSystem::Clear()
{
  SESystem::Clear();

}

const SEScalar* SEHepaticSystem::GetScalar(const std::string& name)
{
  return nullptr;
}

bool SEHepaticSystem::Load(const CDM::HepaticSystemData& in)
{
	SESystem::Load(in);

	return true;
}
CDM::HepaticSystemData* SEHepaticSystem::Unload() const
{
  CDM::HepaticSystemData* data = new CDM::HepaticSystemData();
  Unload(*data);
  return data;
}
void SEHepaticSystem::Unload(CDM::HepaticSystemData& data) const
{
	SESystem::Unload(data);

}
