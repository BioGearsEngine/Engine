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
#include "patient/actions/SEConsciousRespirationCommand.h"
#include "bind/UseInhalerData.hxx"
class SESubstance;

class DLL_DECL SEUseInhaler : public SEConsciousRespirationCommand
{
  friend class SEConsciousRespiration;
  SEUseInhaler();
public:
	virtual ~SEUseInhaler();

	virtual void Clear(); //clear memory

	virtual bool IsValid() const;
	virtual bool IsActive() const;

  virtual bool Load(const CDM::UseInhalerData& in);
  virtual CDM::UseInhalerData* Unload() const;
protected:
  virtual void Unload(CDM::UseInhalerData& data) const;

public:

	virtual void ToString(std::ostream &str) const;
protected:

};  
