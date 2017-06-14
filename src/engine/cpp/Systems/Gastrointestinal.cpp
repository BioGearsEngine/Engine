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
#include "Gastrointestinal.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "substance/SESubstance.h"
#include "patient/SEMeal.h"
#include "patient/SENutrition.h"
#include "patient/conditions/SEConsumeMeal.h"
#include "patient/actions/SEConsumeNutrients.h"
#include "circuit/fluid/SEFluidCircuit.h"
#include "properties/SEScalar.h"
#include "properties/SEScalarAmountPerVolume.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarMassPerTime.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarMassPerAmount.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEScalarVolumePerTime.h"

#pragma warning(disable:4786)
#pragma warning(disable:4275)

//#define logMeal

Gastrointestinal::Gastrointestinal(BioGears& bg) : SEGastrointestinalSystem(bg.GetLogger()), m_data(bg)
{
  Clear();
	/* Move to a unit test
	SENutrition one(m_Logger);
	SENutrition two(m_Logger);
	one.GetCarbohydrate()->SetValue(1, MassUnit::g);
	one.GetCarbohydrateDigestionRate()->SetValue(1, MassPerTimeUnit::g_Per_s);
	one.GetFat()->SetValue(2, MassUnit::g);
	one.GetFatDigestionRate()->SetValue(2, MassPerTimeUnit::g_Per_s);
	one.GetProtein()->SetValue(3, MassUnit::g);
	one.GetProteinDigestionRate()->SetValue(3, MassPerTimeUnit::g_Per_s);

	two.GetCarbohydrate()->SetValue(1, MassUnit::g);
	two.GetCarbohydrateDigestionRate()->SetValue(2, MassPerTimeUnit::g_Per_s);
	two.GetFat()->SetValue(2, MassUnit::g);
	two.GetFatDigestionRate()->SetValue(4, MassPerTimeUnit::g_Per_s);
	two.GetProtein()->SetValue(3, MassUnit::g);
	two.GetProteinDigestionRate()->SetValue(6, MassPerTimeUnit::g_Per_s);
	one.Increment(two);
	std::cout << "Carbo Rate: " << one.GetCarbohydrateDigestionRate() << std::endl;
	std::cout << "Fat Rate: " << one.GetFatDigestionRate() << std::endl;
	std::cout << "Protein Rate: " << one.GetProteinDigestionRate() << std::endl;
	*/
}

Gastrointestinal::~Gastrointestinal()
{
  Clear();
}

void Gastrointestinal::Clear()
{
  SEGastrointestinalSystem::Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Initializes system properties to valid homeostatic values.
//--------------------------------------------------------------------------------------------------
void Gastrointestinal::Initialize()
{
  BioGearsSystem::Initialize();

  if (m_data.GetConfiguration().HasDefaultStomachContents())
  {
    // We are going to initialize the body with 2 meals so we process the default meal twice
    // 1 meal about 5hrs ago, and one meal at the start of the scenario	
    CDM_COPY((m_data.GetConfiguration().GetDefaultStomachContents()), (&GetStomachContents()));
    m_data.GetPatient().GetWeight().IncrementValue(2 * m_StomachContents->GetWeight(MassUnit::g), MassUnit::g);
    // Now digest the contents
    DigestStomachNutrients(5 * 60 * 60);//hrs to seconds (note decrement is off, so the stomach will stay full)		
                                        // TODO Should I be getting the weight here? After we digest?
  }
  // Cache off the initial Gut masses so we can reset back to them after stabilization
  m_InitialSubstanceMasses_ug[m_SmallIntestineChymeGlucose]    = m_SmallIntestineChymeGlucose->GetMass(MassUnit::ug);
  m_InitialSubstanceMasses_ug[m_SmallIntestineChymeTristearin] = m_SmallIntestineChymeTristearin->GetMass(MassUnit::ug);
  m_InitialSubstanceMasses_ug[m_SmallIntestineChymeCalcium]    = m_SmallIntestineChymeCalcium->GetMass(MassUnit::ug);
  m_InitialSubstanceMasses_ug[m_SmallIntestineChymeSodium]     = m_SmallIntestineChymeSodium->GetMass(MassUnit::ug);
  m_InitialSubstanceMasses_ug[m_SmallIntestineChymeUrea]       = m_SmallIntestineChymeUrea->GetMass(MassUnit::ug);
}

bool Gastrointestinal::Load(const CDM::BioGearsGastrointestinalSystemData& in)
{
  if (!SEGastrointestinalSystem::Load(in))
    return false;
  BioGearsSystem::LoadState();
  m_DecrementNutrients = true;
  return true;
}
CDM::BioGearsGastrointestinalSystemData* Gastrointestinal::Unload() const
{
  CDM::BioGearsGastrointestinalSystemData* data = new CDM::BioGearsGastrointestinalSystemData();
  Unload(*data);
  return data;
}
void Gastrointestinal::Unload(CDM::BioGearsGastrointestinalSystemData& data) const
{
  SEGastrointestinalSystem::Unload(data);
}

void Gastrointestinal::SetUp()
{
	m_ConsumeRate = false;
	m_DecrementNutrients = false;

  m_WaterDigestionRate.SetValue(m_data.GetConfiguration().GetWaterDigestionRate(VolumePerTimeUnit::mL_Per_s), VolumePerTimeUnit::mL_Per_s);
  m_CalciumDigestionRate.SetValue(m_data.GetConfiguration().GetCalciumDigestionRate(MassPerTimeUnit::g_Per_s), MassPerTimeUnit::g_Per_s);

	m_GItoCVPath = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetPath(BGE::ChymePath::SmallIntestineC1ToSmallIntestine1);
	m_GutT1ToGroundPath = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetPath(BGE::ChymePath::GutT1ToGround);

	m_SmallIntestineChyme = m_data.GetCompartments().GetLiquidCompartment(BGE::ChymeCompartment::SmallIntestine);
	m_SmallIntestineChymeGlucose    = m_SmallIntestineChyme->GetSubstanceQuantity(m_data.GetSubstances().GetGlucose());
	m_SmallIntestineChymeTristearin = m_SmallIntestineChyme->GetSubstanceQuantity(m_data.GetSubstances().GetTristearin());
	m_SmallIntestineChymeCalcium    = m_SmallIntestineChyme->GetSubstanceQuantity(m_data.GetSubstances().GetCalcium());
	m_SmallIntestineChymeSodium     = m_SmallIntestineChyme->GetSubstanceQuantity(m_data.GetSubstances().GetSodium());
	m_SmallIntestineChymeUrea       = m_SmallIntestineChyme->GetSubstanceQuantity(m_data.GetSubstances().GetUrea());

	m_secretionRate_mL_Per_s = 0.041;// Average from daily secretion rates : Gastric (1500mL) ,Small Intestine(1800mL), Large Intestine (200mL) form Guyton p775
	m_dT_s = m_data.GetTimeStep().GetValue(TimeUnit::s);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Do anything needed between stabilization
///
/// \details
/// At the end of resting stabilization we apply a ConsumeMeal condition to modify the body state
/// if no condition is present we reset the body state substance levels to their original level
//--------------------------------------------------------------------------------------------------

void Gastrointestinal::AtSteadyState()
{
	if (m_data.GetState() == EngineState::AtInitialStableState)
	{// Apply our conditions		
		if (m_data.GetConditions().HasConsumeMeal())
		{
			SEMeal& meal = m_data.GetConditions().GetConsumeMeal()->GetMeal();
			DefaultNutritionRates(meal);// If no rates are provided, use the defaults
#ifdef logMeal
			m_ss << "Original Patient weight : " << m_data.GetPatient()->GetWeight();
			Info(m_ss);
			m_ss << "Original Stomach weight : " << m_StomachContents->GetWeight(MassUnit::g) << "(g)";
			Info(m_ss);
			m_ss << m_NutrientsInStomach;
			Info(m_ss);
#endif
      // Remove the default meal weight from the patient
			m_data.GetPatient().GetWeight().IncrementValue(-m_StomachContents->GetWeight(MassUnit::g), MassUnit::g);
			// Overwrite meal contents into our stomach
      CDM_COPY((&meal), (m_StomachContents));
			if (!m_StomachContents->HasWater() || m_StomachContents->GetWater().IsZero())
        m_StomachContents->GetWater().SetValue(m_secretionRate_mL_Per_s*m_dT_s, VolumeUnit::mL);//Add a time steps worth of water if empty
			// Increase our weight by the meal
			m_data.GetPatient().GetWeight().IncrementValue(m_StomachContents->GetWeight(MassUnit::g), MassUnit::g);
			// Empty out the gut
			m_SmallIntestineChyme->GetVolume().SetValue(0, VolumeUnit::mL);
			m_SmallIntestineChymeGlucose->GetMass().SetValue(0,MassUnit::ug);
			m_SmallIntestineChymeGlucose->Balance(BalanceLiquidBy::Mass);
			m_SmallIntestineChymeTristearin->GetMass().SetValue(0, MassUnit::ug);
			m_SmallIntestineChymeTristearin->Balance(BalanceLiquidBy::Mass);
			m_SmallIntestineChymeCalcium->GetMass().SetValue(0, MassUnit::ug);
			m_SmallIntestineChymeCalcium->Balance(BalanceLiquidBy::Mass);
			m_SmallIntestineChymeSodium->GetMass().SetValue(0, MassUnit::ug);
			m_SmallIntestineChymeSodium->Balance(BalanceLiquidBy::Mass);
			m_SmallIntestineChymeUrea->GetMass().SetValue(0, MassUnit::ug);
			m_SmallIntestineChymeUrea->Balance(BalanceLiquidBy::Mass);
#ifdef logMeal
			m_ss << "Stomach weight with meal : " << m_StomachContents->GetWeight(MassUnit::g) << "(g)";
			Info(m_ss);
			m_ss << m_NutrientsInStomach;
			Info(m_ss);
			m_ss << "New Patient weight : " << m_data.GetPatient()->GetWeight();
			Info(m_ss);
#endif
			// Flip flag in order for digestion to actually remove the proper amount from the stomach contents
			m_DecrementNutrients = true;
			// Digest for the elapsed time
#ifdef logMeal
			for (auto isub : m_InitialGutMasses_ug)
			{
				m_ss << "Predigestion Gut mass for " << isub.first->GetSubstance().GetName() << " : " << isub.first->GetMass();
				Info(m_ss);
			}
			m_ss << "Meal Elapsed time " << meal->GetElapsedTime();
			Info(m_ss);
#endif
			DigestStomachNutrients(meal.GetElapsedTime(TimeUnit::s));
#ifdef logMeal
			m_ss << "Stomach weight with digested meal : " << m_StomachContents->GetWeight(MassUnit::g) << "(g)";
			Info(m_ss);
			m_ss << m_NutrientsInStomach;
			Info(m_ss);
			for (auto isub : m_InitialGutMasses_ug)
			{
				m_ss << "Postdigestion Gut mass for " << isub.first->GetSubstance().GetName() << " : " << isub.first->GetMass();
				Info(m_ss);
			}
#endif
			// Flip it back to not decrement since we are still in stabilization 
			m_DecrementNutrients = false;
			// Based on flow, take off mass from GutChyme and put that mass throughout the body (CV/Tissues)
			AbsorbMeal(meal.GetElapsedTime(TimeUnit::min));
#ifdef logMeal
			for (auto isub : m_InitialGutMasses_ug)
			{
				m_ss << "Post-absorbed Gut mass for " << isub.first->GetSubstance().GetName() << " : " << isub.first->GetMass();
				Info(m_ss);
			}
#endif
      // Update our initial masses
      m_InitialSubstanceMasses_ug[m_SmallIntestineChymeGlucose]    = m_SmallIntestineChymeGlucose->GetMass(MassUnit::ug);
      m_InitialSubstanceMasses_ug[m_SmallIntestineChymeTristearin] = m_SmallIntestineChymeTristearin->GetMass(MassUnit::ug);
      m_InitialSubstanceMasses_ug[m_SmallIntestineChymeCalcium]    = m_SmallIntestineChymeCalcium->GetMass(MassUnit::ug);
      m_InitialSubstanceMasses_ug[m_SmallIntestineChymeSodium]     = m_SmallIntestineChymeSodium->GetMass(MassUnit::ug);
      m_InitialSubstanceMasses_ug[m_SmallIntestineChymeUrea]       = m_SmallIntestineChymeUrea->GetMass(MassUnit::ug);
		}
	}
	if (m_data.GetState() == EngineState::AtSecondaryStableState)
	{
		m_DecrementNutrients = true;
		// Reset the Gut Chyme substance to their original values
		for (auto i : m_InitialSubstanceMasses_ug)
		{
			i.first->GetMass().SetValue(i.second, MassUnit::ug);
			i.first->Balance(BalanceLiquidBy::Mass);
		}
    // We never decremented the volume so we are good there
	}
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Gastrointestinal Preprocess function
///
/// \details
/// Apply any meal consumtion actions
/// Absorb fluids from the tissues back into the stomach so it always has fluid in it
/// Digest substances based on our time step and transfer them to the Gut
/// Absorb substances from the gut into the vascular system via the small intestine 
//--------------------------------------------------------------------------------------------------

void Gastrointestinal::PreProcess()
{
	if (m_data.GetState() == EngineState::Active)
	{
		if (m_data.GetActions().GetPatientActions().HasConsumeNutrients())
		{
			// Use Default Rates if none provided
			SEConsumeNutrients* c = m_data.GetActions().GetPatientActions().GetConsumeNutrients();
			if (c->HasNutritionFile())
			{// Grab file, then load it (note GetNutrition will remove the file name, so get it first)
				std::string file = c->GetNutritionFile();
				if (!c->GetNutrition().LoadFile(file))
				{
					/// \error Unable to read consume meal action file
					Error("Could not read provided nutrition file", "Gastrointestinal::PreProcess");
				}
			}
			DefaultNutritionRates(c->GetNutrition());
      m_StomachContents->Increment(c->GetNutrition());
			m_data.GetPatient().GetWeight().IncrementValue(c->GetNutrition().GetWeight(MassUnit::kg), MassUnit::kg);
			m_data.GetActions().GetPatientActions().RemoveConsumeNutrients();
		}
		GastricSecretion(m_dT_s); // Move some water from the Gut EV fluids to the Stomach
		DigestStomachNutrients(m_dT_s);
	}
	else
	{
		// Reset the Gut Chyme substance to their original values
		for (auto i : m_InitialSubstanceMasses_ug)
		{
			i.first->GetMass().SetValue(i.second, MassUnit::ug);
			i.first->Balance(BalanceLiquidBy::Mass);
		}
	}
	AbsorbNutrients(m_dT_s); // Absorb nutrients into the Blood from the small intestine chyme	
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Absorb fluids from the tissues back into the stomach so it always has fluid in it
///
/// \details
/// Fluid absorption is based on time step at a constant rate
//--------------------------------------------------------------------------------------------------
void Gastrointestinal::GastricSecretion(double duration_s)
{
	// Increase the water in the stomach by the flow rate
	// Take it out of the tissue at the same rate it's being added to the stomach
	//There is a compliance, so the volume will be modified accordingly
	m_GutT1ToGroundPath->GetNextFlowSource().SetValue(m_secretionRate_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
  m_StomachContents->GetWater().IncrementValue(m_secretionRate_mL_Per_s * duration_s, VolumeUnit::mL);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Digest substances based on our time step and transfer them to the Gut
///
/// \details
/// Each substance can provide a digestion rate, if none is provided a default will be used
/// A portion of each substance is removed from the stomach based on time step and digestion rate
/// The substance is then incremented into the Gut. Each substance can have it's own movement logic
/// Water is also transfered to the Gut based on a constant rate
//--------------------------------------------------------------------------------------------------
void Gastrointestinal::DigestStomachNutrients(double duration_s)
{
	if (duration_s <= 0)
		return; // Nothing to do then
	if (!m_StomachContents->HasWater())
		Fatal("There is no water in the Stomach??");// This is not good... what is going on?	

	double digestedAmount;// either in g or mL

	if (m_StomachContents->HasSodium())
	{// Sodium rate is a function of the concentration of sodium in the stomach, so do this before we do water
		double totalNa_g = m_StomachContents->GetSodium(MassUnit::g);
		double digestedNa_g = (totalNa_g / m_StomachContents->GetWater(VolumeUnit::mL))
			* m_WaterDigestionRate.GetValue(VolumePerTimeUnit::mL_Per_s) * duration_s;
		if (totalNa_g <= digestedNa_g)
		{
			digestedNa_g = totalNa_g;
			if (m_DecrementNutrients)
			{
				m_StomachContents->GetSodium().Invalidate();
				Info("Stomach is out of Sodium");
			}
		}
		else
		{
			if (m_DecrementNutrients)
				m_StomachContents->GetSodium().IncrementValue(-digestedNa_g, MassUnit::g);
		}
#ifdef logDigest
		m_ss << "Digested " << digestedNa_g << "(g) of Sodium";
		Info(m_ss);
#endif
		m_SmallIntestineChymeSodium->GetMass().IncrementValue(digestedNa_g, MassUnit::g);
		// Wait till the water volume is corret on the chyme before we balance
	}

	digestedAmount = DigestNutrient(m_StomachContents->GetWater(), m_WaterDigestionRate, false, duration_s);
	if (digestedAmount > 0)
	{
#ifdef logDigest
		m_ss << "Digested " << digestedAmount << "(mL) of Water";
		Info(m_ss);
#endif
		m_SmallIntestineChyme->GetVolume().IncrementValue(digestedAmount, VolumeUnit::mL);
	}
	// Balance Sodium, now that we have proper volume on the gut
	m_SmallIntestineChymeSodium->Balance(BalanceLiquidBy::Mass);

	m_ConsumeRate = true;// We are digesting nutrient rates provided by our consume action, if we run out of a substance, we invalidate this rate
	if (m_StomachContents->HasCarbohydrate())
	{
		digestedAmount = DigestNutrient(m_StomachContents->GetCarbohydrate(), m_StomachContents->GetCarbohydrateDigestionRate(), true, duration_s);
		digestedAmount *= m_data.GetConfiguration().GetCarbohydrateAbsorbtionFraction(); // Take off percent that usually passes through the body
		if (digestedAmount != 0)
		{
#ifdef logDigest
			m_ss << "Digested " << digestedAmount << "(g) of Carbs";
			Info(m_ss);
#endif
			m_SmallIntestineChymeGlucose->GetMass().IncrementValue(digestedAmount, MassUnit::g);
			m_SmallIntestineChymeGlucose->Balance(BalanceLiquidBy::Mass);
		}
	}
	if (m_StomachContents->HasFat())
	{
		digestedAmount = DigestNutrient(m_StomachContents->GetFat(), m_StomachContents->GetFatDigestionRate(), true, duration_s);
		digestedAmount *= m_data.GetConfiguration().GetFatAbsorbtionFraction(); // Take off percent that usually passes through the body
		if (digestedAmount != 0)
		{
#ifdef logDigest
			m_ss << "Digested " << digestedAmount << "(g) of Fat";
			Info(m_ss);
#endif
			m_SmallIntestineChymeTristearin->GetMass().IncrementValue(digestedAmount, MassUnit::g);
			m_SmallIntestineChymeTristearin->Balance(BalanceLiquidBy::Mass);
		}
	}
	if (m_StomachContents->HasProtein())
	{
		digestedAmount = DigestNutrient(m_StomachContents->GetProtein(), m_StomachContents->GetProteinDigestionRate(), true, duration_s);		
		digestedAmount *= m_data.GetConfiguration().GetProteinToUreaFraction(); // How much gets turned into Urea
		if (digestedAmount != 0)
		{			
#ifdef logDigest
			m_ss << "Digested " << digestedAmount << "(g) of Urea";
			Info(m_ss);
#endif
      double tuningFactor = 1.0; /// \todo Remove tuning factor and adjust protein to urea fraction following investigation
			m_SmallIntestineChymeUrea->GetMass().IncrementValue(digestedAmount * tuningFactor, MassUnit::g);
			m_SmallIntestineChymeUrea->Balance(BalanceLiquidBy::Mass);
		}
	}
	m_ConsumeRate = false; // Other rates are specified by configuration and should never be invalidated

	if (m_StomachContents->HasCalcium())
	{
		digestedAmount = DigestNutrient(m_StomachContents->GetCalcium(), m_CalciumDigestionRate, true, duration_s);
		digestedAmount *= m_data.GetConfiguration().GetCalciumAbsorbtionFraction(); // Take off percent that usually passes through the body
		if (digestedAmount != 0)
		{
#ifdef logDigest
			m_ss << "Digested " << digestedAmount << "(g) of Calcium";
			Info(m_ss);
#endif
			m_SmallIntestineChymeCalcium->GetMass().IncrementValue(digestedAmount, MassUnit::g);
			m_SmallIntestineChymeCalcium->Balance(BalanceLiquidBy::Mass);
		}
	}
}

// --------------------------------------------------------------------------------------------------
/// \brief
/// Generic code for removal of an amout of a substance from the stomach based on rate and duration
///
/// \details
//--------------------------------------------------------------------------------------------------
double Gastrointestinal::DigestNutrient(SEUnitScalar& totalAmt, SEUnitScalar& rate, bool mass, double duration_s)
{	
	double digestedAmt = 0;
	if (totalAmt.IsValid())
	{
    double t = totalAmt.GetValue(mass ? (const CCompoundUnit&)MassUnit::g : (const CCompoundUnit&)VolumeUnit::mL);
    digestedAmt = rate.GetValue(mass ? (const CCompoundUnit&)MassPerTimeUnit::g_Per_s : (const CCompoundUnit&)VolumePerTimeUnit::mL_Per_s)*duration_s;
		if (t <= digestedAmt)
		{
			digestedAmt = t;
			if (m_DecrementNutrients)
			{// Decrement stomach contents only if we are running (not stabilizing)
				totalAmt.Invalidate();
				if (m_ConsumeRate)// We keep this rate, it's a system parameter not a per nutrition rate as the masses are
					rate.Invalidate();
			}
		}
		else
		{
			if (m_DecrementNutrients)// Decrement stomach content only if we are running (not stabilizing)
        totalAmt.IncrementValue(-digestedAmt, mass ? (const CCompoundUnit&)MassUnit::g : (const CCompoundUnit&)VolumeUnit::mL);
		}
	}
	return digestedAmt;
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// If no substance rates are provided on meals (Action or Condition) use a default from configuration
///
/// \details
//--------------------------------------------------------------------------------------------------
void Gastrointestinal::DefaultNutritionRates(SENutrition& n)
{
	if (n.HasCarbohydrate() && !n.HasCarbohydrateDigestionRate())
    n.GetCarbohydrateDigestionRate().SetValue(m_data.GetConfiguration().GetDefaultCarbohydrateDigestionRate(MassPerTimeUnit::g_Per_min),MassPerTimeUnit::g_Per_min);
	if (n.HasFat() && !n.HasFatDigestionRate())
    n.GetFatDigestionRate().SetValue(m_data.GetConfiguration().GetDefaultFatDigestionRate(MassPerTimeUnit::g_Per_min),MassPerTimeUnit::g_Per_min);
	if (n.HasProtein() && !n.HasProteinDigestionRate())
    n.GetProteinDigestionRate().SetValue(m_data.GetConfiguration().GetDefaultProteinDigestionRate(MassPerTimeUnit::g_Per_min), MassPerTimeUnit::g_Per_min);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Absorb substances from the gut into the vascular system via the small intestine
///
/// \details
/// Move substances from the chyme system into the vascular system based on a rate that is a 
/// function of the sodium concentration in the body
//--------------------------------------------------------------------------------------------------
void Gastrointestinal::AbsorbNutrients(double duration_s)
{
	double absorptionRate_mL_Per_min = 3.3; // Average water absorption rate Peronnet

	double absorbedVolume_mL = absorptionRate_mL_Per_min * duration_s / 60;
	double siVolume_mL = m_SmallIntestineChyme->GetVolume().GetValue(VolumeUnit::mL);
	if (siVolume_mL <= 0)
	{
		absorptionRate_mL_Per_min = 0;// No volume, no flow
	}
	else if (absorbedVolume_mL > siVolume_mL)
	{//Only take what we have
		absorbedVolume_mL = siVolume_mL;
		absorptionRate_mL_Per_min = 60 * (absorbedVolume_mL / duration_s);
	}

	if (m_data.GetState() == EngineState::AtSecondaryStableState)
	{// Don't Remove volume while stabilizing
    m_SmallIntestineChyme->GetVolume().IncrementValue(-absorbedVolume_mL, VolumeUnit::mL);
    // Calculate new concentrations for everything based on new volume
		for (SELiquidSubstanceQuantity* subQ : m_SmallIntestineChyme->GetSubstanceQuantities())
		{
			if (subQ->HasMass())
				subQ->Balance(BalanceLiquidBy::Mass);
		}
	}
 
	GetChymeAbsorbtionRate().SetValue(absorptionRate_mL_Per_min, VolumePerTimeUnit::mL_Per_min);
	//This will move substances
	m_GItoCVPath->GetNextFlowSource().SetValue(absorptionRate_mL_Per_min, VolumePerTimeUnit::mL_Per_min);
	//Need to manually remove volume, since there isn't a compliance
	m_GItoCVPath->GetSourceNode().GetNextVolume().IncrementValue(-absorptionRate_mL_Per_min / 60.0 * m_dT_s, VolumeUnit::mL);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Absorb a meal over a specified lenth of time
///
/// \details
/// This method supports the ConsumeMeal Condition, it will move the provied meal into the stomach
/// Digest food for the specifed length of time and move it to the Gut, then calculate the amount
/// of substance that would then have been absorbed from the Gut and distributes that mass throughout
/// the body tissues
//--------------------------------------------------------------------------------------------------
void Gastrointestinal::AbsorbMeal(double duration_min)
{
	if (duration_min <= 0)
		return;

	// We use the default healthy flow rate throughout this absorbtion
	// We could step wise this and see if we can figure out how pH is changing
	// in order to change the flow rate, but that is a bit much for this model implementation
	double flowRate_mL_Per_min = 3.3;

	// Calculate the amount of fluid flow based on the elaspsed time 
	double absorbedVolume_mL = flowRate_mL_Per_min*duration_min;
	double siVolume_mL = m_SmallIntestineChyme->GetVolume().GetValue(VolumeUnit::mL);
	if (absorbedVolume_mL > siVolume_mL)
		absorbedVolume_mL = siVolume_mL;// Don't take off what we don't have
#ifdef logMeal
	double totVol_mL = m_data.GetBloodChemistry()->GetExtravascularFluidVolume()->GetValue(VolumeUnit::mL);
	totVol_mL += m_data.GetCardiovascular()->GetBloodVolume()->GetValue(VolumeUnit::mL);
	m_ss << "Blood Volume before absorbtion " << totVol_mL << "(mL)";
	Info(m_ss);
	m_ss << "Gut Volume before absorbtion: " << m_GutChyme->GetVolume();
	Info(m_ss);
	m_ss << "Absorbed Volume : " << absorbedVolume_mL << "(mL)";
	Info(m_ss);
#endif
  m_SmallIntestineChyme->GetVolume().IncrementValue(-absorbedVolume_mL, VolumeUnit::mL);

	SEScalarVolume absorbedVolume;
	absorbedVolume.SetValue(absorbedVolume_mL, VolumeUnit::mL);
//m_data.GetCircuits().DistributeVolume(absorbedVolume);
#ifdef logMeal
	double newtotVol_mL = m_data.GetBloodChemistry()->GetExtravascularFluidVolume()->GetValue(VolumeUnit::mL);
	newtotVol_mL += m_data.GetCardiovascular()->GetBloodVolume()->GetValue(VolumeUnit::mL);
	m_ss << "Blood Volume after absorbtion " << newtotVol_mL << "(mL), body absorbed : " << newtotVol_mL - totVol_mL;
	Info(m_ss);
	m_ss << "Gut Volume after absorbtion: " << m_GutChyme->GetVolume();
	Info(m_ss);
	if (newtotVol_mL - totVol_mL != absorbedVolume_mL)
		Error("Water volume is NOT conserved");
	else
		Info("Water volume is conserved");
#endif

	// Doing mass then volume, I don't think it matters which order this happens in since 
	// the amount of mass that goes on a compartment is the ratio of it volume over the total
	// and incrementing total volume via compartment should keep that same ratio

	double bloodVol_mL = m_data.GetCardiovascular().GetBloodVolume(VolumeUnit::mL);
	double tissueVol_mL = m_data.GetTissue().GetExtravascularFluidVolume(VolumeUnit::mL);

	SEScalarMass mass;
	double gutMass_g;
	double absorbedMass_g = 0;
	for (auto i : m_InitialSubstanceMasses_ug)
	{
		gutMass_g = i.first->GetMass().GetValue(MassUnit::g);
		if (gutMass_g == 0)
			continue;// Nothing to move!
		// Calculate the amout that absorbed over the elapsed duration time
		absorbedMass_g = flowRate_mL_Per_min * duration_min * i.first->GetConcentration(MassPerVolumeUnit::g_Per_mL);
		
		// Make sure we do not pull out more than we have in the Gut Chyme
		if (gutMass_g < absorbedMass_g)
			absorbedMass_g = gutMass_g;
		if (absorbedMass_g <= 0)
			continue;
		// Pull mass off the chyme
#ifdef logMeal
		m_ss << "Preabsortion Gut mass for " << i.first->GetSubstance().GetName() << " " << i.first->GetMass();
		Info(m_ss);
		m_ss << "Absorbing " << absorbedMass_g << " (g)";
		Info(m_ss);
#endif
		i.first->GetMass().IncrementValue(-absorbedMass_g, MassUnit::g);
		i.first->Balance(BalanceLiquidBy::Mass);
    m_InitialSubstanceMasses_ug[i.first] = i.first->GetMass(MassUnit::ug);
#ifdef logMeal
		m_ss << "Postabsorbtion Gut mass for " << i.first->GetSubstance().GetName() << " " << i.first->GetMass();
		Info(m_ss);

		// Add mass to the tissue
		double bPreAbsorbed_g = m_data.GetCircuits()->TotalSubstanceInBlood(i.first->GetSubstance(), MassUnit::g);
		double tPreAbsorbed_g = m_data.GetCircuits()->TotalSubstanceInTissue(i.first->GetSubstance(), MassUnit::g);
		m_ss << "Preabsorbed Total " << i.first->GetSubstance().GetName() << " in body : " << bPreAbsorbed_g + tPreAbsorbed_g << "(g) b(" << bPreAbsorbed_g << ") t(" << tPreAbsorbed_g << ")";
		Info(m_ss);
#endif
		// Distribute the mass over the body 
		mass.SetValue(absorbedMass_g, MassUnit::g);
//	m_data.GetCircuits().DistributeBloodAndTissueMass(i.first->GetSubstance(), mass);
#ifdef logMeal
		double bPostAbsorbed_g = m_data.GetCircuits()->TotalSubstanceInBlood(i.first->GetSubstance(), MassUnit::g);
		double tPostAbsorbed_g = m_data.GetCircuits()->TotalSubstanceInTissue(i.first->GetSubstance(), MassUnit::g);
		m_ss << "PostAbsorbed Total " << i.first->GetSubstance().GetName() << " in body : " << bPostAbsorbed_g + tPostAbsorbed_g << "(g), body absorbed : " << (bPostAbsorbed_g + tPostAbsorbed_g) - (bPreAbsorbed_g + tPreAbsorbed_g) << "(g) b(" << bPostAbsorbed_g << ") t(" << tPostAbsorbed_g << ")";
		Info(m_ss);
		double massDifference_g = (bPostAbsorbed_g + tPostAbsorbed_g) - (bPreAbsorbed_g + tPreAbsorbed_g);
		if (!SEScalar::IsZero(massDifference_g - absorbedMass_g))
			Error("Mass is NOT Conserved");
		else
			Info("Mass is conserved");
#endif
	}
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Gastrointestinal Preprocess function
///
/// \details
/// The current BioGears implementation has no functionality in the process function for Gastrointestinal.
//--------------------------------------------------------------------------------------------------
void Gastrointestinal::Process()
{
	
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Gastrointestinal postprocess function
///
/// \details
/// The current BioGears implementation has no specific postprocess functionality.
//--------------------------------------------------------------------------------------------------
void Gastrointestinal::PostProcess()
{
	
}