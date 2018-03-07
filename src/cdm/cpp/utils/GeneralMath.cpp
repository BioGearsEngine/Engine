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
#include <sstream>
#include "utils/GeneralMath.h"
#include "substance/SESubstance.h"
#include "substance/SESubstanceManager.h"
#include "compartment/fluid/SELiquidCompartment.h"
#include "compartment/fluid/SELiquidCompartmentLink.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarInversePressure.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarMassPerAmount.h"
#include "properties/SEScalarAmountPerVolume.h"
#include "properties/SEScalarOsmolarity.h"
#include "properties/SEScalarOsmolality.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the concentration of a substance given mass and volume. concentration = mass / volume
//--------------------------------------------------------------------------------------------------
void GeneralMath::CalculateConcentration(const SEScalarMass& mass, const SEScalarVolume& volume, SEScalarMassPerVolume& concentration, Logger* logger)
{
  //concentration = mass/volume
  double mass_ug = mass.GetValue(MassUnit::ug);
  double  volume_mL = volume.GetValue(VolumeUnit::mL);
  //Can't have a negative mass or volume
  if (mass_ug < 0.0)
  {
	  std::stringstream ss;
	  ss << "GeneralMath::CalculateConcentration: Mass is negative:" << mass_ug << " ug. Setting concentration to 0.";
	  logger->Error(ss);
	  mass_ug = 0.0;
  }
  if (volume_mL < 0.0)
  {
	  std::stringstream ss;
	  ss << "GeneralMath::CalculateConcentration: Volume is negative:" << volume_mL << " mL. Setting it to 0.";
	  logger->Error(ss);
	  volume_mL = 0.0;
  }
  if (volume_mL == 0.0)
    concentration.SetValue(0.0, MassPerVolumeUnit::ug_Per_mL);
  else
    concentration.SetValue(mass_ug / volume_mL, MassPerVolumeUnit::ug_Per_mL);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the mass of a substance given concentration and volume. mass = concentration * volume
//--------------------------------------------------------------------------------------------------
void GeneralMath::CalculateMass(const SEScalarVolume& volume, const SEScalarMassPerVolume& concentration, SEScalarMass& mass, Logger* logger)
{
  // mass = concentration*volume
  double volume_mL = volume.GetValue(VolumeUnit::mL);
  double concentration_ug_Per_mL = concentration.GetValue(MassPerVolumeUnit::ug_Per_mL);
  if (concentration_ug_Per_mL < 0.0)
  {
	  std::stringstream ss;
	  ss << "GeneralMath::CalculateMass: Mass is negative:" << concentration_ug_Per_mL << " ug/mL. Setting it to 0.";
	  logger->Error(ss);
	  concentration_ug_Per_mL = 0.0;
  }
  if (volume_mL < 0.0)
  {
	  std::stringstream ss;
	  ss << "GeneralMath::CalculateMass: Volume is negative:" << volume_mL << " mL. Setting it to 0.";
	  logger->Error(ss);
	  volume_mL = 0.0;
  }
  mass.SetValue(volume_mL*concentration_ug_Per_mL, MassUnit::ug);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the mass of a substance given concentration and volume. mass = concentration * volume
//--------------------------------------------------------------------------------------------------
void GeneralMath::CalculateHenrysLawConcentration(const SESubstance& substance, const SEScalarPressure& partialPressure, SEScalarMassPerVolume& concentration, Logger* logger)
{
  double pp_mmHg = partialPressure.GetValue(PressureUnit::mmHg);
  if (substance.GetState() != CDM::enumSubstanceState::Gas)
    throw CommonDataModelException("Cannot calculate a molarity by Henry's law from partial pressure of a non gaseous substance in a liquid");
  if (pp_mmHg < 0.0)
  {
	  std::stringstream ss;
	  ss << "GeneralMath::CalculateHenrysLawConcentration: Pressure is negative:" << pp_mmHg << " mmHg. Setting it to 0.";
	  logger->Error(ss);
	  pp_mmHg = 0.0;
  }
  double density_ug_Per_mL = substance.GetDensity(MassPerVolumeUnit::ug_Per_mL);
  double solubilityCoefficient_Per_mmHg = substance.GetSolubilityCoefficient(InversePressureUnit::Inverse_mmHg);
  double concentration_ug_Per_mL = pp_mmHg * density_ug_Per_mL * solubilityCoefficient_Per_mmHg;
  concentration.SetValue(concentration_ug_Per_mL, MassPerVolumeUnit::ug_Per_mL);
}


///\cite Bhagat1984CalculatedOsmolality
void GeneralMath::CalculateOsmolarity(const SEScalarAmountPerVolume& sodiumMolarity, const SEScalarAmountPerVolume& potassiumMolarity, const SEScalarAmountPerVolume& glucoseMolarity, const SEScalarAmountPerVolume& ureaMolarity, SEScalarOsmolarity& fluidOsmolarity)
{
  double osmolarity_mOsm_Per_L = 1.89 * sodiumMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.38 * potassiumMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.08 * glucoseMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.03 * ureaMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 7.45;
  fluidOsmolarity.SetValue(osmolarity_mOsm_Per_L, OsmolarityUnit::mOsm_Per_L);
}

///\cite Bhagat1984CalculatedOsmolality
void GeneralMath::CalculateOsmolality(const SEScalarAmountPerVolume& sodiumMolarity, const SEScalarAmountPerVolume& potassiumMolarity, const SEScalarAmountPerVolume& glucoseMolarity, const SEScalarAmountPerVolume& ureaMolarity, const SEScalar& specificGravity, SEScalarOsmolality& fluidOsmolality)
{
  double waterDensity_g_mL = 0.993;
  double fluidDensity_kg_L = specificGravity.GetValue() * waterDensity_g_mL ;
  double osmolarity_mOsm_Per_L = (1.89 * sodiumMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.38 * potassiumMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.08 * glucoseMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.03 * ureaMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 7.45);

  // Convert to kilograms by dividing by density of the fluid:
  double osmolality_mOsm_Per_kg = osmolarity_mOsm_Per_L / fluidDensity_kg_L;
  fluidOsmolality.SetValue(osmolality_mOsm_Per_kg, OsmolalityUnit::mOsm_Per_kg);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the partial pressure of a substance in gas based on the volumeFraction * pressure
//--------------------------------------------------------------------------------------------------
void GeneralMath::CalculatePartialPressureInGas(const SEScalarFraction& volumeFraction, const SEScalarPressure& pressure, SEScalarPressure& partialPressure, Logger* logger)
{
  double VolumeFraction = volumeFraction.GetValue();
  double pressure_cmH2O = pressure.GetValue(PressureUnit::cmH2O);
  if (VolumeFraction < 0.0)
  {
	  std::stringstream ss;
	  ss << "GeneralMath::CalculateConcentration: Volume Fraction is negative:" << VolumeFraction << ". Setting it to 0.";
	  logger->Error(ss);
	  VolumeFraction = 0.0;
  }
  partialPressure.SetValue(VolumeFraction*pressure_cmH2O, PressureUnit::cmH2O);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the partial pressure of a substance in liquid using (density and solubility coefficient) and the concentration
//--------------------------------------------------------------------------------------------------
void GeneralMath::CalculatePartialPressureInLiquid(const SESubstance& substance, const SEScalarMassPerVolume& concentration, SEScalarPressure& partialPressure, Logger* logger)
{
  if (substance.GetState() != CDM::enumSubstanceState::Gas)
    throw CommonDataModelException("Cannot calculate a partial pressure of a non gaseous substance in a liquid");
  double concentration_ug_Per_mL = concentration.GetValue(MassPerVolumeUnit::ug_Per_mL);
  if (concentration_ug_Per_mL < 0.0)
  {
	  std::stringstream ss;
	  ss << "GeneralMath::CalculateConcentration: Concentration is negative:" << concentration_ug_Per_mL << " ug/mL. Setting it to 0.";
	  logger->Error(ss);
	  concentration_ug_Per_mL = 0.0;
  }
  double density_ug_Per_mL = substance.GetDensity(MassPerVolumeUnit::ug_Per_mL);
  double solubilityCoefficient_Per_mmHg = substance.GetSolubilityCoefficient(InversePressureUnit::Inverse_mmHg);
  double partialPressure_mmHg = (concentration_ug_Per_mL / density_ug_Per_mL) / solubilityCoefficient_Per_mmHg;
  partialPressure.SetValue(partialPressure_mmHg, PressureUnit::mmHg);
}

//--------------------------------------------------------------------------------------------------
/// \brief
// This function makes the assumption that the fluid medium is water to extrapolate that the fluid volume in mL is equivalent to the fluid weight in g  
//--------------------------------------------------------------------------------------------------
void GeneralMath::CalculateSpecificGravity(const SEScalarMass& mass, const SEScalarVolume& volume, SEScalar& specificGravity, Logger* logger)
{
  // compute density of given substance then divide by water density in g/mL (assuming 37 degrees C, body internal temp):
  double mass_g = mass.GetValue(MassUnit::g);
  double  volume_mL = volume.GetValue(VolumeUnit::mL);
  double totalmass_g = mass_g + volume_mL;
  double waterDensity_g_mL = 0.993;
  //Can't have a negative mass or volume
  if (totalmass_g < 0.0)
  {
	  std::stringstream ss;
	  ss << "GeneralMath::CalculateSpecificGravity: Mass is negative:" << totalmass_g << " ug. Setting it to 0.";
	  logger->Error(ss);
	  totalmass_g = 0.0;
  }
  if (volume_mL < 0.0)
  {
	  std::stringstream ss;
	  ss << "GeneralMath::CalculateSpecificGravity: Volume is negative:" << volume_mL << " mL. Setting it to 0.";
	  logger->Error(ss);
	  volume_mL = 0.0;
  }
  if (volume_mL == 0.0)
    specificGravity.SetValue(0.0);
  else
    specificGravity.SetValue((totalmass_g / volume_mL) / waterDensity_g_mL);
}

//--------------------------------------------------------------------------------------------------
/// \brief
// Calculates the density of water at a given temperature using DIPPR105 equation, valid from 32-707F  
//--------------------------------------------------------------------------------------------------
void GeneralMath::CalculateWaterDensity(const SEScalarTemperature& temp, SEScalarMassPerVolume& density)
{
  //DIPPR105 equation, see http://ddbonline.ddbst.de/DIPPR105DensityCalculation/DIPPR105CalculationCGI.exe
  double A = .14395;
  double B = .0112;
  double C = 649.727;
  double D = .05107;
  double temp_K = temp.GetValue(TemperatureUnit::K);
  
  double density_kg_Per_m3 = A / (pow(B, 1 + pow(1 - (temp_K / C), D)));

  density.SetValue(density_kg_Per_m3, MassPerVolumeUnit::kg_Per_m3);

}

double GeneralMath::PercentDifference(double expected, double calculated)
{
  if (calculated == 0.0&&expected == 0.0)
    return 0.0;

  double difference = (calculated - expected);
  double average = (calculated + expected) / 2.0;

  if (average == 0.0)
  {
    return SEScalar::dNaN();
  }
  return std::abs(difference / average) * 100.0;
}

double GeneralMath::PercentTolerance(double expected, double calculated, double epsilon)
{
  if (expected == 0.0)
  {
    if (calculated == 0.0)
      return 0.0;
    else
    {
      if (std::abs(calculated) > epsilon)
        throw CommonDataModelException("Percent Tolerance is NaN");
      else
        return 0.0;
    }
  }
  double err = std::abs(calculated - expected) / expected * 100.0;
  if (std::isnan(err))
  {
    if (std::isnan(expected))
      throw CommonDataModelException("Provided Expected value is NaN");
    if (std::isnan(calculated))
      throw CommonDataModelException("Provided Calculated value is NaN");
    throw CommonDataModelException("Percent Tolerance is NaN");
  }
  return err;
}

void GeneralMath::Combinations(std::vector<int> maxValues, std::vector<std::vector<int>>& permutations)
{
  int Oidx = 0;
  int numVals = 1;
  int Olength = maxValues.size();
  std::vector<int> *current, *next;

  for (int i = 0; i < Olength; i++)
    numVals *= maxValues[Oidx] + 1;
  permutations.clear();
  permutations.resize(numVals);
  // First is all zeros
  int perm = 0;
  current = &permutations[perm++];
  current->resize(Olength, 0);

  for (int iter = 0; iter < numVals - 1; iter++)
  {
    next = &permutations[perm++];
    Copy(*current, *next);
    current = next;
    for (int i = 0; i <= Olength; i++)
    {
      if (++(*current)[i] > maxValues[i])
      {
        (*current)[i] = 0;
      }
      else
      {
        break;
      }
    }
    //for (int c = 0; c < Olength;c++)
    //	std::cout << (*current)[c] << " ";
    //std::cout << std::endl;
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Performs linear interpolation between two points
///
/// \param  x1      value 1 on the x axis
/// \param  x2      value 2 on the x axis
/// \param  y1      the y value corresponding to x1
/// \param  y2      the y value corresponding to x2
/// \param  xPrime  the interpolation point
///
/// \return the y value corresponding to xPrime
///
/// \details
/// Linear Interpolator finds the y value at xPrime. The slope
/// and Y intercept of the line connecting (x1,y1) and (x2,y2) are then found so y = mx + b can be used 
/// to find yPrime by inputting xPrime.
//--------------------------------------------------------------------------------------------------
double GeneralMath::LinearInterpolator(double x1, double x2, double y1, double y2, double xPrime)
{
  double yPrime; // Initialize the output variable
  double slope;  // Initialize the variable used to determine the slope between x1 and x2
  double yInt;   // Initialize the variable used to determine the y intercept

  slope = (y2 - y1) / (x2 - x1); // linear slope equals dy/dx

  yInt = y1 - slope*x1;		   // Y = mx + b -> b = Y - mx

  yPrime = slope*xPrime + yInt;  // Interpolate yPrime from the created line

  return yPrime;
}

// --------------------------------------------------------------------------------------------------
/// \brief
/// Returns y value for the specified exponential function given a normalized x value (0.0 to 1.0).
///
///   y = dbase ^ [(Log10(dmin/dmax) * x) + Log10(dmax)]
/// 
/// \param  dbase    base value
/// \param  dmin     minimum 
/// \param  dmax     maximum
/// \param  x		normalized x, 0.0 to 1.0
///
/// \return y
///
/// \details
/// Exponential function used to model airflow resistance. Requires a base value for the function, a minimum y value 
/// (x = 0), and maximum y value (x = 1).  Return the y value that maps to x.  
///
/// Limitations:
///  - dmin > 0
///  - dmax > 0 
///  - 0.0 <= dx <= 1.0
///  
/// If any of the input variables fall outside the above bounds, the function returns NaN
///
//--------------------------------------------------------------------------------------------------
double GeneralMath::ResistanceFunction(double dbase, double dmin, double dmax, double dx)
{
  double dy = SEScalar::dNaN();     // Resistance value
  if (dmin > 0.0 && dmax > 0.0 && dx >= 0.0 && dx <= 1.0)
  {
    dy = pow(dbase, ((log10(dmin / dmax) * dx) + log10(dmax)));
  }
  return dy;
}

// --------------------------------------------------------------------------------------------------
/// \brief
/// A logistic function.
/// 
/// \param  a    asymptote
/// \param  x50  x-value at sigmoid midpoint 
/// \param  k    curve steepness and direction parameter
/// \param  x		 input value
///
/// \return y
///
/// \details
/// A logistic function.
//--------------------------------------------------------------------------------------------------
double GeneralMath::LogisticFunction(double a, double x50, double k, double x)
{
  double y = a / (1 + exp(-k*(x - x50)));

  return y;
}

