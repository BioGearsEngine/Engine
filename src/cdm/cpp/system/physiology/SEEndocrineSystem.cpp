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
#include "system/physiology/SEEndocrineSystem.h"
#include "substance/SESubstanceManager.h"
#include "properties/SEScalarAmountPerTime.h"
#include "bind/ScalarAmountPerTimeData.hxx"

SEEndocrineSystem::SEEndocrineSystem(Logger* logger) : SESystem(logger)
{
	m_InsulinSynthesisRate = nullptr;
}

SEEndocrineSystem::~SEEndocrineSystem()
{
	Clear();
}

void SEEndocrineSystem::Clear()
{
  SESystem::Clear();
	SAFE_DELETE(m_InsulinSynthesisRate);
}

const SEScalar* SEEndocrineSystem::GetScalar(const std::string& name)
{
	if (name.compare("InsulinSynthesisRate") == 0)
		return &GetInsulinSynthesisRate();
	return nullptr;
}

bool SEEndocrineSystem::Load(const CDM::EndocrineSystemData& in)
{
	SESystem::Load(in);
	if (in.InsulinSynthesisRate().present())
		GetInsulinSynthesisRate().Load(in.InsulinSynthesisRate().get());
	return true;
}

CDM::EndocrineSystemData* SEEndocrineSystem::Unload() const
{
  CDM::EndocrineSystemData* data = new CDM::EndocrineSystemData();
  Unload(*data);
  return data;
}

void SEEndocrineSystem::Unload(CDM::EndocrineSystemData& data) const
{
	if (m_InsulinSynthesisRate != nullptr)
		data.InsulinSynthesisRate(std::unique_ptr<CDM::ScalarAmountPerTimeData>(m_InsulinSynthesisRate->Unload()));
	SESystem::Unload(data);
}

bool SEEndocrineSystem::HasInsulinSynthesisRate() const
{
	return m_InsulinSynthesisRate == nullptr ? false : m_InsulinSynthesisRate->IsValid();
}
SEScalarAmountPerTime& SEEndocrineSystem::GetInsulinSynthesisRate()
{
	if (m_InsulinSynthesisRate == nullptr)
		m_InsulinSynthesisRate = new SEScalarAmountPerTime();
	return *m_InsulinSynthesisRate;
}
double SEEndocrineSystem::GetInsulinSynthesisRate(const AmountPerTimeUnit& unit) const
{
	if (m_InsulinSynthesisRate == nullptr)
		return SEScalar::dNaN();
	return m_InsulinSynthesisRate->GetValue(unit);
}


