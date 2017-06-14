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
#include "patient/SEMeal.h"
#include "Serializer.h"
#include "properties/SEScalarTime.h"
#include "bind/ScalarTimeData.hxx"

SEMeal::SEMeal(Logger* logger) : SENutrition(logger)
{
	m_ElapsedTime = nullptr;
}

SEMeal::~SEMeal()
{
	Clear();	
}

void SEMeal::Clear()
{
	SENutrition::Clear();
	SAFE_DELETE(m_ElapsedTime);
}


bool SEMeal::Load(const CDM::MealData& in)
{
	SENutrition::Load(in);
	GetElapsedTime().Load(in.ElapsedTime());	
	return true;
}

CDM::MealData*  SEMeal::Unload() const
{
	CDM::MealData* data = new CDM::MealData();
	Unload(*data);
	return data;
}

void SEMeal::Unload(CDM::MealData& data) const
{
	SENutrition::Unload(data);
	if (m_ElapsedTime != nullptr)
		data.ElapsedTime(std::unique_ptr<CDM::ScalarTimeData>(m_ElapsedTime->Unload()));
}

bool SEMeal::LoadFile(const std::string& MealFile)
{
	CDM::MealData* pData;
	std::unique_ptr<CDM::ObjectData> data;

	std::string nFile = MealFile;
	if (nFile.find("/Meals") == std::string::npos)
	{
		nFile = "./Meals/";
		nFile += MealFile;
	}

	data = Serializer::ReadFile(nFile,GetLogger());
	pData = dynamic_cast<CDM::MealData*>(data.get());
	if (pData == nullptr)
	{
		std::stringstream ss;
		ss << "Meal file could not be read : " << MealFile << std::endl;
		Error(ss);
		return false;
	}
	return Load(*pData);
}

bool SEMeal::HasElapsedTime() const
{
	return m_ElapsedTime == nullptr ? false : m_ElapsedTime->IsValid();
}
SEScalarTime& SEMeal::GetElapsedTime()
{
	if (m_ElapsedTime == nullptr)
		m_ElapsedTime = new SEScalarTime();
	return *m_ElapsedTime;
}
double SEMeal::GetElapsedTime(const TimeUnit& unit) const
{
	if (m_ElapsedTime == nullptr)
		return SEScalar::dNaN();
	return m_ElapsedTime->GetValue(unit);
}