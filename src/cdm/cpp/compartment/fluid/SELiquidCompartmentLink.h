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
#include "bind/LiquidCompartmentData.hxx"
#include "compartment/fluid/SEFluidCompartmentLink.h"
#include "bind/LiquidCompartmentLinkData.hxx"

class SELiquidCompartment;

class DLL_DECL SELiquidCompartmentLink : public SEFluidCompartmentLink<SELiquidTransportEdge, SELiquidTransportVertex, SELiquidCompartment>
{
  friend class SECompartmentManager;
protected:
  SELiquidCompartmentLink(SELiquidCompartment& src, SELiquidCompartment & tgt, const std::string& name) : SEFluidCompartmentLink(src, tgt, name) {}
public:
  ~SELiquidCompartmentLink() {}

  virtual CDM::LiquidCompartmentLinkData* Unload();
};
