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
#include "patient/actions/SEHemorrhage.h"
#include "bind/HemorrhageData.hxx"
#include "properties/SEScalarVolumePerTime.h"
#include "bind/ScalarVolumePerTimeData.hxx"

SEHemorrhage::SEHemorrhage() : SEPatientAction()
{
  m_Compartment = "";
	m_Rate=nullptr;
}

SEHemorrhage::~SEHemorrhage()
{
	Clear();
}

void SEHemorrhage::Clear()
{
	SEPatientAction::Clear();
  m_Compartment = "";
	SAFE_DELETE(m_Rate);
}

bool SEHemorrhage::IsValid() const
{
	return SEPatientAction::IsValid() && HasCompartment() && HasRate();
}

bool SEHemorrhage::IsActive() const
{
	return IsValid() ? !m_Rate->IsZero() : false;
}

bool SEHemorrhage::Load(const CDM::HemorrhageData& in)
{
	SEPatientAction::Load(in);
	GetRate().Load(in.Rate());
	m_Compartment=in.Compartment();
	return true;
}

CDM::HemorrhageData* SEHemorrhage::Unload() const
{
	CDM::HemorrhageData*data(new CDM::HemorrhageData());
	Unload(*data);
	return data;
}

void SEHemorrhage::Unload(CDM::HemorrhageData& data) const
{
	SEPatientAction::Unload(data);
	if(m_Rate!=nullptr)
		data.Rate(std::unique_ptr<CDM::ScalarVolumePerTimeData>(m_Rate->Unload()));
	if(HasCompartment())
		data.Compartment(m_Compartment);
}

std::string SEHemorrhage::GetCompartment() const
{
	return m_Compartment;
}

void SEHemorrhage::SetCompartment(const std::string& name)
{
  m_Compartment = name;
}

bool SEHemorrhage::HasCompartment() const
{
	return !m_Compartment.empty();
}

void SEHemorrhage::InvalidateCompartment()
{
  m_Compartment = "";
}

bool SEHemorrhage::HasRate() const
{
	return m_Rate==nullptr?false:m_Rate->IsValid();
}

SEScalarVolumePerTime& SEHemorrhage::GetRate()
{
	if(m_Rate==nullptr)
		m_Rate=new SEScalarVolumePerTime();
	return *m_Rate;
}

void SEHemorrhage::ToString(std::ostream &str) const
{
	str << "Patient Action : Hemorrhage"; 
	if(HasComment())
		str<<"\n\tComment: "<<m_Comment;
	str << "\n\tRate: "; HasRate() ? str << *m_Rate : str << "NaN";
	str << "\n\tFor Compartment: "; HasCompartment()? str << GetCompartment() : str << "No Compartment Set"; 
	str << std::flush;
}
