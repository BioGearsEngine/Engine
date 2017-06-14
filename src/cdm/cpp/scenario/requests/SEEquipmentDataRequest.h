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
#include "scenario/requests/SEDataRequest.h"
#include "bind/EquipmentDataRequestData.hxx"
class SEDataRequestManager;

class DLL_DECL SEEquipmentDataRequest : public SEDataRequest
{
  friend class SEDataRequestManager;
protected:
  SEEquipmentDataRequest(const SEDecimalFormat* dfault = nullptr);
public:
  virtual ~SEEquipmentDataRequest();

	virtual void Clear(); //clear memory

  virtual bool Load(const CDM::EquipmentDataRequestData& in);
  virtual CDM::EquipmentDataRequestData* Unload() const;
protected:
  virtual void Unload(CDM::EquipmentDataRequestData& data) const;
public:

  virtual std::string GetType() const;
  virtual void SetType(const std::string& t);
  virtual bool HasType() const;
  virtual void InvalidateType();

protected:

  std::string m_Type;
};                  