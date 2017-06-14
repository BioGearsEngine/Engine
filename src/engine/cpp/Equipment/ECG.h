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

#include "system/equipment/ElectroCardioGram/SEElectroCardioGram.h"
#include "system/equipment/ElectroCardioGram/SEElectroCardioGramInterpolator.h"
#include "bind/BioGearsElectroCardioGramData.hxx"

/**
* @brief 
* Generic ECG machine to assess the heart rhythm.
*/
class BIOGEARS_API ECG : public SEElectroCardioGram, public BioGearsSystem
{
	friend BioGears;
  friend class BioGearsEngineTest;
protected:
	ECG(BioGears& bg);
	BioGears& m_data;

public:
  virtual ~ECG();

  void Clear();

  // Set members to a stable homeostatic state
  void Initialize();

  // Load a state
  virtual bool Load(const CDM::BioGearsElectroCardioGramData& in);
  virtual CDM::BioGearsElectroCardioGramData* Unload() const;
protected:
  virtual void Unload(CDM::BioGearsElectroCardioGramData& data) const;

  // Set pointers and other member varialbes common to both homeostatic initialization and loading a state
  void SetUp();

public:

	// main driver function responsible for calling the various ECG functions:
	void PreProcess();
	void Process();
	void PostProcess();

protected:
  // Serializable member variables (Set in Initialize and in schema)
  SEScalarTime m_heartRhythmTime;
  SEScalarTime m_heartRhythmPeriod;
  SEElectroCardioGramInterpolator m_interpolator;

  // Stateless member variable (Set in SetUp())
	double m_dt_s;
};
