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
#include "bind/ScalarOsmolalityData.hxx"

class DLL_DECL OsmolalityUnit : public CCompoundUnit
{
public:
  OsmolalityUnit(const std::string& u) : CCompoundUnit(u) {}
  virtual ~OsmolalityUnit() {}

  static bool IsValidUnit(const std::string& unit);
  static const OsmolalityUnit& GetCompoundUnit(const std::string& unit);

  static const OsmolalityUnit Osm_Per_kg;
  static const OsmolalityUnit mOsm_Per_kg;
};

class DLL_DECL SEScalarOsmolality : public SEScalarQuantity<OsmolalityUnit>
{
public:
  SEScalarOsmolality() {}
	virtual ~SEScalarOsmolality() {}

  CDM::ScalarOsmolalityData* Unload() const;
};
