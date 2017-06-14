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

#include "engine/PhysiologyEngineStabilization.h"
#include "properties/SEScalarTime.h"
#include "bind/PhysiologyEngineTimedStabilizationData.hxx"

CDM_BIND_DECL(PhysiologyEngineTimedConditionStabilizationData)
class DLL_DECL PhysiologyEngineTimedStabilizationCriteria : public Loggable
{
public:

	PhysiologyEngineTimedStabilizationCriteria(Logger* logger);
	virtual ~PhysiologyEngineTimedStabilizationCriteria();

  virtual void Clear();

	virtual bool Load(const CDM::PhysiologyEngineTimedConditionStabilizationData& in);
  virtual CDM::PhysiologyEngineTimedConditionStabilizationData* Unload() const;
protected:
	virtual void Unload(CDM::PhysiologyEngineTimedConditionStabilizationData& data) const;
public:

  virtual std::string GetName() const;
  virtual void SetName(const std::string& name);
  virtual bool HasName() const;
  virtual void InvalidateName();

  virtual bool HasTime();
  virtual SEScalarTime& GetTime();
  virtual const SEScalarTime& GetTime() const;


protected:
	std::string    m_Name;
	SEScalarTime   m_Time;
	
};

CDM_BIND_DECL(PhysiologyEngineTimedStabilizationData)
class DLL_DECL PhysiologyEngineTimedStabilization : public PhysiologyEngineStabilization
{
public:
	PhysiologyEngineTimedStabilization(Logger* logger);
	virtual ~PhysiologyEngineTimedStabilization();

  virtual void Clear();

  virtual bool Load(const CDM::PhysiologyEngineTimedStabilizationData& in);
  virtual CDM::PhysiologyEngineTimedStabilizationData* Unload() const;
protected:
  virtual void Unload(CDM::PhysiologyEngineTimedStabilizationData& data) const;
public:

  virtual bool LoadFile(const std::string& file);

  virtual bool StabilizeRestingState(PhysiologyEngine& engine);
  virtual bool StabilizeFeedbackState(PhysiologyEngine& engine);
  virtual bool StabilizeConditions(PhysiologyEngine& engine, const std::vector<const SECondition*>& conditions);

  virtual SEScalarTime& GetRestingStabilizationTime();
  virtual double GetRestingStabilizationTime(const TimeUnit& unit) const;

  virtual bool HasFeedbackStabilizationTime() const;
  virtual SEScalarTime& GetFeedbackStabilizationTime();
  virtual double GetFeedbackStabilizationTime(const TimeUnit& unit) const;

  virtual bool HasConditionCriteria(const std::string& name) const;
  virtual void RemoveConditionCriteria(const std::string& name);
  virtual const std::vector<PhysiologyEngineTimedStabilizationCriteria*>& GetConditionCriteria() const;
  virtual PhysiologyEngineTimedStabilizationCriteria& GetConditionCriteria(const std::string& name);
  virtual PhysiologyEngineTimedStabilizationCriteria* GetConditionCriteria(const std::string& name) const;

protected:

  virtual bool Stabilize(PhysiologyEngine& engine, const SEScalarTime& time);

	SEScalarTime m_RestingStabilizationTime;
	SEScalarTime* m_FeedbackStabilizationTime;
	std::vector<PhysiologyEngineTimedStabilizationCriteria*> m_ConditionCriteria;
};
