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
#include "Hepatic.h"

Hepatic::Hepatic(BioGears& bg) : SEHepaticSystem(bg.GetLogger()), m_data(bg)
{
  Clear();
}

Hepatic::~Hepatic()
{
  Clear();
}

void Hepatic::Clear()
{
  SEHepaticSystem::Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Initializes system properties to valid homeostatic values.
//--------------------------------------------------------------------------------------------------
void Hepatic::Initialize()
{
  BioGearsSystem::Initialize();

}

bool Hepatic::Load(const CDM::BioGearsHepaticSystemData& in)
{
  if (!SEHepaticSystem::Load(in))
    return false;
  BioGearsSystem::LoadState();

  return true;
}
CDM::BioGearsHepaticSystemData* Hepatic::Unload() const
{
  CDM::BioGearsHepaticSystemData* data = new CDM::BioGearsHepaticSystemData();
  Unload(*data);
  return data;
}
void Hepatic::Unload(CDM::BioGearsHepaticSystemData& data) const
{
  SEHepaticSystem::Unload(data);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Initializes parameters for the Hepatic Class
///
///  \details
///	 Initializes member variables and system level values on the common data model.
//--------------------------------------------------------------------------------------------------
void Hepatic::SetUp()
{
 
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Determines if the engine is stable.
///
/// \details
/// When the engine is stable, the CDM makes this call to update the member variable.
//--------------------------------------------------------------------------------------------------
void Hepatic::AtSteadyState()
{

}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Preprocess function
///
/// \details
//--------------------------------------------------------------------------------------------------
void Hepatic::PreProcess()
{

}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Process function
///
/// \details
//--------------------------------------------------------------------------------------------------
void Hepatic::Process()
{	
	
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// PostProcess function
///
/// \details
//--------------------------------------------------------------------------------------------------
void Hepatic::PostProcess()
{

}