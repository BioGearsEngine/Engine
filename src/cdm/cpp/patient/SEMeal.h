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
#include "bind/MealData.hxx"
#include "patient/SENutrition.h"

class DLL_DECL SEMeal : public SENutrition
{
public:

	SEMeal(Logger* logger);
	virtual ~SEMeal();

  virtual void Clear();

	virtual bool Load(const CDM::MealData& in);
	virtual CDM::MealData* Unload() const;
protected:
	virtual void Unload(CDM::MealData& data) const;

public:

	bool LoadFile(const std::string& MealFile);
	
	virtual bool HasElapsedTime() const;
	virtual SEScalarTime& GetElapsedTime();
  virtual double GetElapsedTime(const TimeUnit& unit) const;
	
protected:

	SEScalarTime*    m_ElapsedTime;
};