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
#include "scenario/SEAction.h"
#include "bind/ActionData.hxx"
#include "substance/SESubstanceManager.h"

#include "scenario/SEAdvanceTime.h"
#include "bind/AdvanceTimeData.hxx"

#include "scenario/SESerializeState.h"
#include "bind/SerializeStateData.hxx"
// Environment Actions
#include "system/environment/actions/SEEnvironmentChange.h"
#include "bind/EnvironmentChangeData.hxx"
#include "system/environment/actions/SEThermalApplication.h"
#include "bind/ThermalApplicationData.hxx"
// Anesthesia Actions
#include "bind/AnesthesiaMachineActionData.hxx"
#include "bind/AnesthesiaMachineConfigurationData.hxx"
#include "system/equipment/Anesthesia/actions/SEAnesthesiaMachineConfiguration.h"
#include "bind/OxygenWallPortPressureLossData.hxx"
#include "system/equipment/Anesthesia/actions/SEOxygenWallPortPressureLoss.h"
#include "bind/OxygenTankPressureLossData.hxx"
#include "system/equipment/Anesthesia/actions/SEOxygenTankPressureLoss.h"
#include "bind/ExpiratoryValveLeakData.hxx"
#include "system/equipment/Anesthesia/actions/SEExpiratoryValveLeak.h"
#include "bind/ExpiratoryValveObstructionData.hxx"
#include "system/equipment/Anesthesia/actions/SEExpiratoryValveObstruction.h"
#include "bind/InspiratoryValveLeakData.hxx"
#include "system/equipment/Anesthesia/actions/SEInspiratoryValveLeak.h"
#include "bind/InspiratoryValveObstructionData.hxx"
#include "system/equipment/Anesthesia/actions/SEInspiratoryValveObstruction.h"
#include "bind/MaskLeakData.hxx"
#include "system/equipment/Anesthesia/actions/SEMaskLeak.h"
#include "bind/SodaLimeFailureData.hxx"
#include "system/equipment/Anesthesia/actions/SESodaLimeFailure.h"
#include "bind/TubeCuffLeakData.hxx"
#include "system/equipment/Anesthesia/actions/SETubeCuffLeak.h"
#include "bind/VaporizerFailureData.hxx"
#include "system/equipment/Anesthesia/actions/SEVaporizerFailure.h"
#include "bind/VentilatorPressureLossData.hxx"
#include "system/equipment/Anesthesia/actions/SEVentilatorPressureLoss.h"
#include "bind/YPieceDisconnectData.hxx"
#include "system/equipment/Anesthesia/actions/SEYPieceDisconnect.h"
// Inhaler Actions
#include "system/equipment/Inhaler/actions/SEInhalerConfiguration.h"
#include "bind/InhalerConfigurationData.hxx"
// Patient Actions
#include "bind/PatientActionData.hxx"
#include "bind/PatientAssessmentRequestData.hxx"
#include "patient/actions/SEPatientAssessmentRequest.h"
#include "bind/AcuteStressData.hxx"
#include "patient/actions/SEAcuteStress.h"
#include "bind/AirwayObstructionData.hxx"
#include "patient/actions/SEAirwayObstruction.h"
#include "bind/ApneaData.hxx"
#include "patient/actions/SEApnea.h"
#include "bind/AsthmaAttackData.hxx"
#include "patient/actions/SEAsthmaAttack.h"
#include "bind/BrainInjuryData.hxx"
#include "patient/actions/SEBrainInjury.h"
#include "bind/BronchoconstrictionData.hxx"
#include "patient/actions/SEBronchoconstriction.h"
#include "bind/CardiacArrestData.hxx"
#include "patient/actions/SECardiacArrest.h"
#include "bind/ChestCompressionData.hxx"
#include "patient/actions/SEChestCompression.h"
#include "bind/ChestCompressionForceData.hxx"
#include "patient/actions/SEChestCompressionForce.h"
#include "bind/ChestCompressionForceScaleData.hxx"
#include "patient/actions/SEChestCompressionForceScale.h"
#include "bind/ChestOcclusiveDressingData.hxx"
#include "patient/actions/SEChestOcclusiveDressing.h"
#include "bind/ConsciousRespirationData.hxx"
#include "patient/actions/SEConsciousRespiration.h"
#include "bind/ConsumeNutrientsData.hxx"
#include "patient/actions/SEConsumeNutrients.h"
#include "bind/IntubationData.hxx"
#include "patient/actions/SEIntubation.h"
#include "bind/ExerciseData.hxx"
#include "patient/actions/SEExercise.h"
#include "bind/HemorrhageData.hxx"
#include "patient/actions/SEHemorrhage.h"
#include "bind/IntubationData.hxx"
#include "patient/actions/SEIntubation.h"
#include "bind/MechanicalVentilationData.hxx"
#include "patient/actions/SEMechanicalVentilation.h"
#include "bind/NeedleDecompressionData.hxx"
#include "patient/actions/SENeedleDecompression.h"
#include "bind/PericardialEffusionData.hxx"
#include "patient/actions/SEPericardialEffusion.h"
#include "bind/SubstanceBolusData.hxx"
#include "patient/actions/SESubstanceBolus.h"
#include "bind/SubstanceInfusionData.hxx"
#include "patient/actions/SESubstanceInfusion.h"
#include "bind/SubstanceCompoundInfusionData.hxx"
#include "patient/actions/SESubstanceCompoundInfusion.h"
#include "bind/TensionPneumothoraxData.hxx"
#include "patient/actions/SETensionPneumothorax.h"
#include "bind/UrinateData.hxx"
#include "patient/actions/SEUrinate.h"

SEAction::SEAction() : Loggable()
{
	m_Comment="";
}

SEAction::~SEAction()
{
	Clear();
}

void SEAction::Clear()
{
	m_Comment="";
}

SEAction* SEAction::newFromBind(const CDM::ActionData& data, SESubstanceManager& substances)
{
	std::stringstream ss;
	SESubstance* substance;
	SESubstanceCompound* compound;

	// Anesthesia Machine Actions
	CDM::AnesthesiaMachineConfigurationData* anConfig;
	// Anesthesia Machine Failure Action
	CDM::ExpiratoryValveLeakData* anExLeak;
	CDM::ExpiratoryValveObstructionData* anExObs;
	CDM::InspiratoryValveLeakData* anInLeak;
	CDM::InspiratoryValveObstructionData* anInObs;
	CDM::MaskLeakData* anMskLeak;
	CDM::SodaLimeFailureData* anSodaFail;
	CDM::TubeCuffLeakData* anTubLeak;
	CDM::VaporizerFailureData* anVapFail;
	CDM::VentilatorPressureLossData* anVentLoss;
	CDM::YPieceDisconnectData* anYDisc;
	// Anesthesia Machine Incidents
	CDM::OxygenWallPortPressureLossData* anO2WallLoss;
	CDM::OxygenTankPressureLossData* anO2TankLoss;


	CDM::ActionData* action = (CDM::ActionData*)&data;
	CDM::AdvanceTimeData* advData = dynamic_cast<CDM::AdvanceTimeData*>(action);
	if (advData != nullptr)
	{
		SEAdvanceTime* a = new SEAdvanceTime();
		a->Load(*advData);
		return a;
	}

  CDM::SerializeStateData* stData = dynamic_cast<CDM::SerializeStateData*>(action);
  if (stData != nullptr)
  {
    SESerializeState* a = new SESerializeState();
    a->Load(*stData);
    return a;
  }

	if (dynamic_cast<CDM::EnvironmentActionData*>(action) != nullptr)
	{
		if (dynamic_cast<CDM::EnvironmentChangeData*>(action) != nullptr)
		{
			SEEnvironmentChange* a = new SEEnvironmentChange(substances);
			a->Load(*(CDM::EnvironmentChangeData*)action);
			return a;
		}
		if (dynamic_cast<CDM::ThermalApplicationData*>(action) != nullptr)
		{
			SEThermalApplication* a = new SEThermalApplication();
			a->Load(*(CDM::ThermalApplicationData*)action);
			return a;
		}
		substances.GetLogger()->Error("Unsupported Environment Action Received", "SEScenario::Load");
	}

	if (dynamic_cast<CDM::PatientActionData*>(action) != nullptr)
	{
		if (dynamic_cast<CDM::PatientAssessmentRequestData*>(action) != nullptr)
		{
			SEPatientAssessmentRequest* a = new SEPatientAssessmentRequest();
			a->Load(*(CDM::PatientAssessmentRequestData*)action);
			return a;
		}

    CDM::AcuteStressData* aStressData = dynamic_cast<CDM::AcuteStressData*>(action);
    if (aStressData != nullptr)
    {
      SEAcuteStress* a = new SEAcuteStress();
      a->Load(*aStressData);
      return a;
    }

		CDM::AirwayObstructionData* airObData = dynamic_cast<CDM::AirwayObstructionData*>(action);
		if (airObData != nullptr)
		{
			SEAirwayObstruction* a = new SEAirwayObstruction();
			a->Load(*airObData);
			return a;
		}

    CDM::ApneaData* apneaData = dynamic_cast<CDM::ApneaData*>(action);
    if (apneaData != nullptr)
    {
      SEApnea* a = new SEApnea();
      a->Load(*apneaData);
      return a;
    }

    CDM::BrainInjuryData* brainInjData = dynamic_cast<CDM::BrainInjuryData*>(action);
    if (brainInjData != nullptr)
    {
      SEBrainInjury* a = new SEBrainInjury();
      a->Load(*brainInjData);
      return a;
    }

		CDM::BronchoconstrictionData* bconData = dynamic_cast<CDM::BronchoconstrictionData*>(action);
		if (bconData != nullptr)
		{
			SEBronchoconstriction* a = new SEBronchoconstriction();
			a->Load(*bconData);
			return a;
		}

		CDM::CardiacArrestData* carrestData = dynamic_cast<CDM::CardiacArrestData*>(action);
		if (carrestData != nullptr)
		{
			SECardiacArrest* a = new SECardiacArrest();
			a->Load(*carrestData);
			return a;
		}

		CDM::AsthmaAttackData* asthmaData = dynamic_cast<CDM::AsthmaAttackData*>(action);
		if (asthmaData != nullptr)
		{
			SEAsthmaAttack* a = new SEAsthmaAttack();
			a->Load(*asthmaData);
			return a;
		}

		CDM::ChestCompressionData* cprData = dynamic_cast<CDM::ChestCompressionData*>(action);
		if (cprData != nullptr)
		{
			CDM::ChestCompressionForceData* cprForce = dynamic_cast<CDM::ChestCompressionForceData*>(cprData);
			if (cprForce != nullptr)
			{
				SEChestCompressionForce* a = new SEChestCompressionForce();
				a->Load(*cprForce);
				return a;
			}
			CDM::ChestCompressionForceScaleData* cprScale = dynamic_cast<CDM::ChestCompressionForceScaleData*>(cprData);
			if (cprScale != nullptr)
			{
				SEChestCompressionForceScale* a = new SEChestCompressionForceScale();
				a->Load(*cprScale);
				return a;
			}
		}

		CDM::ChestOcclusiveDressingData* codData = dynamic_cast<CDM::ChestOcclusiveDressingData*>(action);
		if (codData != nullptr)
		{
			SEChestOcclusiveDressing* a = new SEChestOcclusiveDressing();
			a->Load(*codData);
			return a;
		}

		CDM::ConsciousRespirationData* conRespData = dynamic_cast<CDM::ConsciousRespirationData*>(action);
		if (conRespData != nullptr)
		{
			SEConsciousRespiration* a = new SEConsciousRespiration();
			a->Load(*conRespData, substances);
			return a;
		}

		CDM::ConsumeNutrientsData* consumeData = dynamic_cast<CDM::ConsumeNutrientsData*>(action);
		if (consumeData != nullptr)
		{
			SEConsumeNutrients* a = new SEConsumeNutrients();
			a->Load(*consumeData);
			return a;
		}

		CDM::ExerciseData* exerciseData = dynamic_cast<CDM::ExerciseData*>(action);
		if (exerciseData != nullptr)
		{
			SEExercise* a = new SEExercise();
			a->Load(*exerciseData);
			return a;
		}

    CDM::IntubationData* intub8Data = dynamic_cast<CDM::IntubationData*>(action);
    if (intub8Data != nullptr)
    {
      SEIntubation* a = new SEIntubation();
      a->Load(*intub8Data);
      return a;
    }

    CDM::MechanicalVentilationData* mechVentData = dynamic_cast<CDM::MechanicalVentilationData*>(action);
    if (mechVentData != nullptr)
    {
      SEMechanicalVentilation* a = new SEMechanicalVentilation();
      a->Load(*mechVentData, substances);
      return a;
    }

		CDM::NeedleDecompressionData* needlData = dynamic_cast<CDM::NeedleDecompressionData*>(action);
		if (needlData != nullptr)
		{
			SENeedleDecompression* a = new SENeedleDecompression();
			a->Load(*needlData);
			return a;
		}

		CDM::HemorrhageData* hemData = dynamic_cast<CDM::HemorrhageData*>(action);
		if (hemData != nullptr)
		{
			SEHemorrhage* a = new SEHemorrhage();
			a->Load(*hemData);
			return a;
		}

		CDM::PericardialEffusionData* pericData = dynamic_cast<CDM::PericardialEffusionData*>(action);
		if (pericData != nullptr)
		{
			SEPericardialEffusion* a = new SEPericardialEffusion();
			a->Load(*pericData);
			return a;
		}

		CDM::TensionPneumothoraxData* pneumoData = dynamic_cast<CDM::TensionPneumothoraxData*>(action);
		if (pneumoData != nullptr)
		{
			SETensionPneumothorax* a = new SETensionPneumothorax();
			a->Load(*pneumoData);
			return a;
		}

		CDM::SubstanceBolusData* bolusData = dynamic_cast<CDM::SubstanceBolusData*>(action);
		if (bolusData != nullptr)
		{
			substance = substances.GetSubstance(bolusData->Substance());
			if (substance == nullptr)
			{
				ss << "Unknown substance : " << bolusData->Substance();
				substances.GetLogger()->Fatal(ss, "SEScenario::Load");
			}
			SESubstanceBolus* a = new SESubstanceBolus(*substance);
			a->Load(*bolusData);
			return a;
		}

		CDM::SubstanceInfusionData* subInfuzData = dynamic_cast<CDM::SubstanceInfusionData*>(action);
		if (subInfuzData != nullptr)
		{
			substance = substances.GetSubstance(subInfuzData->Substance());
			if (substance == nullptr)
			{
				ss << "Unknown substance : " << subInfuzData->Substance();
				substances.GetLogger()->Fatal(ss, "SEScenario::Load");
			}
			SESubstanceInfusion* a = new SESubstanceInfusion(*substance);
			a->Load(*subInfuzData);
			return a;
		}

		CDM::SubstanceCompoundInfusionData* subCInfuzData = dynamic_cast<CDM::SubstanceCompoundInfusionData*>(action);
		if (subCInfuzData != nullptr)
		{
			compound = substances.GetCompound(subCInfuzData->SubstanceCompound());
			if (compound == nullptr)
			{
				ss << "Unknown substance : " << subCInfuzData->SubstanceCompound();
				substances.GetLogger()->Fatal(ss, "SEScenario::Load");
			}
			SESubstanceCompoundInfusion* a = new SESubstanceCompoundInfusion(*compound);
			a->Load(*subCInfuzData);
			return a;
		}

		CDM::UrinateData* urinateData = dynamic_cast<CDM::UrinateData*>(action);
		if (urinateData != nullptr)
		{
			SEUrinate* a = new SEUrinate();
			a->Load(*urinateData);
			return a;
		}
		substances.GetLogger()->Error("Unsupported Patient Action Received", "SEScenario::Load");
	}
	else if (dynamic_cast<CDM::AnesthesiaMachineActionData*>(action) != nullptr)
	{
		anConfig = dynamic_cast<CDM::AnesthesiaMachineConfigurationData*>(action);
		if (anConfig != nullptr)
		{
			SEAnesthesiaMachineConfiguration* a = new SEAnesthesiaMachineConfiguration(substances);
			a->Load(*anConfig);
			return a;
		}

		anO2WallLoss = dynamic_cast<CDM::OxygenWallPortPressureLossData*>(action);
		if (anO2WallLoss != nullptr)
		{
			SEOxygenWallPortPressureLoss* a = new SEOxygenWallPortPressureLoss();
			a->Load(*anO2WallLoss);
			return a;
		}

		anO2TankLoss = dynamic_cast<CDM::OxygenTankPressureLossData*>(action);
		if (anO2TankLoss != nullptr)
		{
			SEOxygenTankPressureLoss* a = new SEOxygenTankPressureLoss();
			a->Load(*anO2TankLoss);
			return a;
		}

		anExLeak = dynamic_cast<CDM::ExpiratoryValveLeakData*>(action);
		if (anExLeak != nullptr)
		{
			SEExpiratoryValveLeak* a = new SEExpiratoryValveLeak();
			a->Load(*anExLeak);
			return a;
		}

		anExObs = dynamic_cast<CDM::ExpiratoryValveObstructionData*>(action);
		if (anExObs != nullptr)
		{
			SEExpiratoryValveObstruction* a = new SEExpiratoryValveObstruction();
			a->Load(*anExObs);
			return a;
		}

		anInLeak = dynamic_cast<CDM::InspiratoryValveLeakData*>(action);
		if (anInLeak != nullptr)
		{
			SEInspiratoryValveLeak* a = new SEInspiratoryValveLeak();
			a->Load(*anInLeak);
			return a;
		}

		anInObs = dynamic_cast<CDM::InspiratoryValveObstructionData*>(action);
		if (anInObs != nullptr)
		{
			SEInspiratoryValveObstruction* a = new SEInspiratoryValveObstruction();
			a->Load(*anInObs);
			return a;
		}

		anMskLeak = dynamic_cast<CDM::MaskLeakData*>(action);
		if (anMskLeak != nullptr)
		{
			SEMaskLeak* a = new SEMaskLeak();
			a->Load(*anMskLeak);
			return a;
		}

		anSodaFail = dynamic_cast<CDM::SodaLimeFailureData*>(action);
		if (anSodaFail != nullptr)
		{
			SESodaLimeFailure* a = new SESodaLimeFailure();
			a->Load(*anSodaFail);
			return a;
		}

		anTubLeak = dynamic_cast<CDM::TubeCuffLeakData*>(action);
		if (anTubLeak != nullptr)
		{
			SETubeCuffLeak* a = new SETubeCuffLeak();
			a->Load(*anTubLeak);
			return a;
		}

		anVapFail = dynamic_cast<CDM::VaporizerFailureData*>(action);
		if (anVapFail != nullptr)
		{
			SEVaporizerFailure* a = new SEVaporizerFailure();
			a->Load(*anVapFail);
			return a;
		}

		anVentLoss = dynamic_cast<CDM::VentilatorPressureLossData*>(action);
		if (anVentLoss != nullptr)
		{
			SEVentilatorPressureLoss* a = new SEVentilatorPressureLoss();
			a->Load(*anVentLoss);
			return a;
		}

		anYDisc = dynamic_cast<CDM::YPieceDisconnectData*>(action);
		if (anYDisc != nullptr)
		{
			SEYPieceDisconnect* a = new SEYPieceDisconnect();
			a->Load(*anYDisc);
			return a;
		}
		substances.GetLogger()->Error("Unsupported Anesthesia Machine Action Received", "SEScenario::Load");
	}
	else if (dynamic_cast<CDM::InhalerActionData*>(action) != nullptr)
	{
		CDM::InhalerConfigurationData* inhaleConfig = dynamic_cast<CDM::InhalerConfigurationData*>(action);
		if (inhaleConfig != nullptr)
		{
			SEInhalerConfiguration* a = new SEInhalerConfiguration(substances);
			a->Load(*inhaleConfig); 
			return a;
		}
	}
	
	if (substances.GetLogger() != nullptr)
		substances.GetLogger()->Error("Unsupported Action Received", "SEAction::newFromBind");
	return nullptr;
}

bool SEAction::Load(const CDM::ActionData& in)
{
	Clear();
	if(in.Comment().present())
		m_Comment=in.Comment().get();
	return true;
}

CDM::ActionData* SEAction::Unload() const
{
	CDM::ActionData* data = new CDM::ActionData();
	Unload(*data);
	return data;
}

void SEAction::Unload(CDM::ActionData& data) const
{
	if(HasComment())
		data.Comment(m_Comment);
}


std::string SEAction::GetComment() const
{
	return m_Comment;
}
void SEAction::SetComment(const std::string& comment)
{
	m_Comment = comment;
}
bool SEAction::HasComment() const
{
	return m_Comment.empty()?false:true;
}
void SEAction::InvalidateComment()
{
	m_Comment = "";
}