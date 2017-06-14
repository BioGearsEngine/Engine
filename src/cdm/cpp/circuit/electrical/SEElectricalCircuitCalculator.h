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
//CDM_BIND_DECL(ElectricalCircuitData)
#include "circuit/SECircuitCalculator.h"
#include "circuit/electrical/SEElectricalCircuit.h"

#include "properties/SEScalarElectricCapacitance.h"
#include "properties/SEScalarElectricCurrent.h"
#include "properties/SEScalarElectricCharge.h"
#include "properties/SEScalarElectricInductance.h"
#include "properties/SEScalarElectricPotential.h"
#include "properties/SEScalarElectricResistance.h"

class DLL_DECL SEElectricalCircuitCalculator : public SECircuitCalculator<SEElectricalCircuit, SEElectricalCircuitNode, SEElectricalCircuitPath, ElectricCapacitanceUnit, ElectricCurrentUnit, ElectricInductanceUnit, ElectricPotentialUnit, ElectricChargeUnit, ElectricResistanceUnit>
{
public:

  SEElectricalCircuitCalculator(Logger* logger);
  SEElectricalCircuitCalculator(const ElectricCapacitanceUnit&, const ElectricCurrentUnit&, const ElectricInductanceUnit&, const ElectricPotentialUnit&, const ElectricChargeUnit&, const ElectricResistanceUnit&, Logger* logger);
  virtual ~SEElectricalCircuitCalculator();
};