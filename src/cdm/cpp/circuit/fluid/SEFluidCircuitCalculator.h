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
//CDM_BIND_DECL(FluidCircuitData)
#include "circuit/SECircuitCalculator.h"
#include "circuit/fluid/SEFluidCircuit.h"

#include "properties/SEScalarFlowCompliance.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEScalarFlowInertance.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarFlowResistance.h"

class DLL_DECL SEFluidCircuitCalculator : public SECircuitCalculator<SEFluidCircuit, SEFluidCircuitNode, SEFluidCircuitPath, FlowComplianceUnit, VolumePerTimeUnit, FlowInertanceUnit, PressureUnit, VolumeUnit, FlowResistanceUnit>
{
public:

  SEFluidCircuitCalculator(Logger* logger);
  SEFluidCircuitCalculator(const FlowComplianceUnit&, const VolumePerTimeUnit&, const FlowInertanceUnit&, const PressureUnit&, const VolumeUnit&, const FlowResistanceUnit&, Logger* logger);
  virtual ~SEFluidCircuitCalculator();
};