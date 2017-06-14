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
#include "properties/SEScalar.h"
#include "bind/ScalarAreaData.hxx"

class DLL_DECL AreaUnit : public CCompoundUnit
{
public:
  AreaUnit(const std::string& u) : CCompoundUnit(u) {}
  virtual ~AreaUnit() {}

  static bool IsValidUnit(const std::string& unit);
  static const AreaUnit& GetCompoundUnit(const std::string& unit);

  static const AreaUnit cm2;
  static const AreaUnit m2;
};

class DLL_DECL SEScalarArea : public SEScalarQuantity<AreaUnit>
{
public:
  SEScalarArea() {}
	virtual ~SEScalarArea() {}

  CDM::ScalarAreaData* Unload() const;
};