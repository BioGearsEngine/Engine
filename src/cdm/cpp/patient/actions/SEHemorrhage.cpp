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
#include "bind/IntegerArray.hxx"
#include "bind/IntegerList.hxx"

SEHemorrhage::SEHemorrhage() : SEPatientAction()
{
  m_Compartment = ""; //This is the compartment we use to store information about hemorrhage
  m_MCIS;
  m_BleedName = "";	  //This is the name of the pathway in circuit that will have its resistance changed

  //Place organs in a map so that we don't get too messy with nested conditionals.  Each vector is digits 2-4 of the MCIS code
  organMap[{6, 4}] = std::make_pair("AortaBleed", "Major Artery");
  organMap[{6, 6}] = std::make_pair("VenaCavaBleed", "Vena Cava");
  organMap[{6, 5}] = std::make_pair("AortaBleed", "Major Artery");
  organMap[{7, 1}] = std::make_pair("LungBleed", "Lungs");
  organMap[{7, 2}] = std::make_pair("HeartBleed", "Heart");
  organMap[{8, 1}] = std::make_pair("LiverBleed", "Liver");
  organMap[{8, 2}] = std::make_pair("SpleenBleed", "Spleen");
  organMap[{8, 3}] = std::make_pair("SplanchnicBleed", "Splanchnic");
  organMap[{8, 4}] = std::make_pair("KidneyBleed", "Kidney");
  organMap[{8, 5}] = std::make_pair("SmallIntestineBleed", "Small Intestine");
  organMap[{8, 6}] = std::make_pair("LargeIntestineBleed", "Large Intestine");
}

SEHemorrhage::~SEHemorrhage()
{
	Clear();
}

void SEHemorrhage::Clear()
{
	SEPatientAction::Clear();
    m_Compartment = "";
	m_MCIS.clear();
	m_BleedName = "";
}

bool SEHemorrhage::IsValid() const
{
	return SEPatientAction::IsValid() && HasCompartment() && HasMCIS() && HasBleedName();
}

bool SEHemorrhage::IsActive() const
{
	return IsValid() ? !m_MCIS[0]==0 : false;
}

bool SEHemorrhage::Load(const CDM::HemorrhageData& in)
{
	SEPatientAction::Load(in);
	if (in.MCIS().present())
	{
		for (size_t i = 0; i < in.MCIS().get().IntegerList().size(); i++)
		{
			m_MCIS.push_back(in.MCIS().get().IntegerList()[i]);
		}
		if ((m_MCIS[0] < 0) || (m_MCIS[0] > 5)) //check to make sure no one puts in a severity of a million
		{
			SetComment("Invalid MCIS Code: Severity out of bounds (0-5).  Defaulting to 3");
			m_MCIS[0] = 3;
		}
		if (m_MCIS.size() != 5)	//make sure mcis code is proper length
		{
			SetComment("Invalid MCIS Code:  Code must be 5 digits.  Defaulting to aorta with bleeding severity = 3");
			m_MCIS = { 3,2,6,3,0 };
		}
	}
	ProcessMCIS();

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
	//Create Integer Array that stores Integer List and pass m_MCIS values to it (modeled after GetActiveIndices in electrocardiogram-
	//interpolatorWaveform.cpp)
	data.MCIS(std::unique_ptr<CDM::IntegerArray>(new CDM::IntegerArray()));
	data.MCIS().get().IntegerList(std::unique_ptr<CDM::IntegerList>(new CDM::IntegerList()));
	for (int i : m_MCIS)
		data.MCIS().get().IntegerList().push_back(i);
}

void SEHemorrhage::ProcessMCIS()
{
	switch (m_MCIS[1]) {
	case Head:
		//Note that this assumes that the third digit is 6 (for vessels).
		if (m_MCIS[3] == 1) //If bleeding is intracranial 
		{
			SetBleedName("BrainBleed");
			SetCompartment("Head");
		}
		else
		{	//If the bleeding is from the carotid artery/jugular vein			
			SetBleedName("AortaBleed");
			SetCompartment("Major Artery");
		}
		break;
	case Torso:
		if (organMap.find({ m_MCIS.begin() + 2, m_MCIS.end()-1}) != organMap.end()) //extract the two digits that map to an organ
		{
			SetBleedName(organMap[{m_MCIS.begin() + 2, m_MCIS.end()-1}].first);
			SetCompartment(organMap[{m_MCIS.begin() + 2, m_MCIS.end()-1}].second);
		}
		else
		{
			SetComment("Invalid MCIS Code: Does not map to BioGears compartment.  Defaulting to Aorta");
			SetBleedName("AortaBleed");
			SetCompartment("Major Artery");
		}
		break;
	case Arms:
		SetBleedName("ArmBleed");
		SetCompartment("Arm");
		break;
	case Legs:
		SetBleedName("LegBleed");
		SetCompartment("Leg");
		break;
	default:
		SetComment("Invalid MCIS Code: Does not map to BioGears compartment.  Defaulting to Aorta");
		SetBleedName("AortaBleed");
		SetCompartment("Major Artery");
		break;
	}
}



std::string SEHemorrhage::GetBleedName() const
{
	return m_BleedName;
}
void SEHemorrhage::SetBleedName(const std::string& name)
{
	m_BleedName = name;
}
bool SEHemorrhage::HasBleedName() const
{
	return !m_BleedName.empty();
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

bool SEHemorrhage::HasMCIS() const
{
	return !m_MCIS.empty();
}

void SEHemorrhage::SetMCIS(const std::vector<unsigned int>& mcisIn)
{
	if (mcisIn.size() != 5)
		Error("MCIS code must be five digits");
	else
		m_MCIS = mcisIn;
}

void SEHemorrhage::ToString(std::ostream &str) const
{
	str << "Patient Action : Hemorrhage";
	if (HasComment())
		str << "\n\tComment: " << m_Comment;
	str << "\n\tInjury Code: ";
	for (int i : m_MCIS)
		str << i;
	str << "\n\tCompartment: "; HasCompartment() ? str << GetCompartment() : str << "No Compartment Set";
	str << "\n\tSeverity:  ";  str << m_MCIS[0];
	str << std::flush;
	
}
