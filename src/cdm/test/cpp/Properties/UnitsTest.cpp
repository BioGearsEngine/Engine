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

// Testing creating and converting units
#include "CommonDataModelTest.h"
#include "properties/SEScalarAmount.h"
#include "properties/SEScalarAmountPerVolume.h"
#include "properties/SEScalarLength.h"
#include "properties/SEScalarFlowCompliance.h"
#include "properties/SEScalarFlowInertance.h"
#include "properties/SEScalarFlowResistance.h"
#include "properties/SEScalarFraction.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarInversePressure.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarMassPerMass.h"
#include "properties/SEScalarMassPerTime.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerPressure.h"
#include "properties/SEScalarVolumePerTime.h"
#include <exception>

void CommonDataModelTest::ConvertList(std::vector<std::string> stringList)
{
	if (stringList.empty())
		return;

	int size = stringList.size();
	for (int i=0; i < size; i++)
	{
		for (int j=0; j < size; j++)
		{
			if (i == j)
				continue;

			std::cout << "converting from " << stringList[i] << " to " << stringList[j] << std::endl;
			//Convert(1.0, stringList[i], stringList[j]); TODO this whole test should use our predefined compound units
		}
	}
}

void CommonDataModelTest::UnitsTest(const std::string& sOutputDirectory)
{
	try
	{
		std::vector<std::string> stringList;

		SEScalarAmount amount;
    amount.SetValue(1.0, AmountUnit::GetCompoundUnit("mol"));

    ((SEUnitScalar&)amount).SetValue(5.0, AmountUnit::GetCompoundUnit("mol"));
    try
    {
      ((SEUnitScalar&)amount).SetValue(5.0, AmountPerVolumeUnit::GetCompoundUnit("mol/L"));
    }
    catch (CommonDataModelException ex)
    {
      // i am expexting this
    }


		SEScalarAmountPerVolume amountPerVolume;
    amountPerVolume.SetValue(1.0, AmountPerVolumeUnit::GetCompoundUnit("mol/L"));

		SEScalarLength length;
    length.SetValue(1.0, LengthUnit::GetCompoundUnit("cm"));
		stringList.clear();
		stringList.push_back("cm");
		stringList.push_back("in");
		ConvertList(stringList);

    ((SEScalar&)length).GetValue();

		SEScalarFlowCompliance flowCompliance;
    flowCompliance.SetValue(1.0, FlowComplianceUnit::GetCompoundUnit("L/cmH2O"));
		stringList.clear();
		stringList.push_back("L/cmH2O");
		stringList.push_back("mL/mmHg");
		stringList.push_back("m^3/Pa");
		ConvertList(stringList);

		SEScalarFlowInertance flowInertance;
    flowInertance.SetValue(1.0, FlowInertanceUnit::GetCompoundUnit("mmHg s^2/mL"));
		stringList.clear();
		stringList.push_back("mmHg s^2/mL");
		stringList.push_back("Pa s^2/m^3");
		ConvertList(stringList);

		SEScalarFlowResistance flowResistance;
    flowResistance.SetValue(1.0, FlowResistanceUnit::GetCompoundUnit("cmH2O s/L"));
		stringList.clear();
		stringList.push_back("cmH2O s/L");
		stringList.push_back("mmHg s/L");
		stringList.push_back("Pa s/m^3");
		ConvertList(stringList);

		SEScalarFraction fraction;
		fraction.SetValue(1.0);

		SEScalarFrequency frequency;
    frequency.SetValue(1.0, FrequencyUnit::GetCompoundUnit("1/min"));
		stringList.clear();
		stringList.push_back("1/min");
		stringList.push_back("1/s");
		stringList.push_back("Hz");
		ConvertList(stringList);

		SEScalarInversePressure inversePressure;
    inversePressure.SetValue(1.0, InversePressureUnit::GetCompoundUnit("1/cmH2O"));
		stringList.clear();
		stringList.push_back("1/cmH2O");
		stringList.push_back("1/mmHg");
		stringList.push_back("1/Pa");
		ConvertList(stringList);

		SEScalarMass mass;
    mass.SetValue(1.0, MassUnit::GetCompoundUnit("g"));
		stringList.clear();
		stringList.push_back("g");
		stringList.push_back("ug");
		stringList.push_back("mg");
		stringList.push_back("kg");
		stringList.push_back("lb");
		ConvertList(stringList);

		SEScalarMassPerMass massPerMass;
    massPerMass.SetValue(1.0, MassPerMassUnit::GetCompoundUnit("ug/kg"));

		SEScalarMassPerTime massPerTime;
    massPerTime.SetValue(1.0, MassPerTimeUnit::GetCompoundUnit("ug/s"));

		SEScalarMassPerVolume massPerVolume;
    massPerVolume.SetValue(1.0, MassPerVolumeUnit::GetCompoundUnit("g/dL"));
		stringList.clear();
		stringList.push_back("g/dL");
		stringList.push_back("ug/mL");
		stringList.push_back("mg/m^3");
		stringList.push_back("kg/m^3");
		stringList.push_back("ug/L");
		ConvertList(stringList);

		SEScalarPressure pressure;
    pressure.SetValue(1.0, PressureUnit::GetCompoundUnit("Pa"));
		stringList.clear();
		stringList.push_back("Pa");
		stringList.push_back("mmHg");
		stringList.push_back("cmH2O");
		ConvertList(stringList);

		SEScalarTemperature temperature;
    temperature.SetValue(1.0, TemperatureUnit::GetCompoundUnit("degF"));
		stringList.clear();
		stringList.push_back("degF");
		stringList.push_back("degC");
		stringList.push_back("K");
		ConvertList(stringList);

		SEScalarTime time;
    time.SetValue(1.0, TimeUnit::GetCompoundUnit("s"));
		stringList.clear();
		stringList.push_back("s");
		stringList.push_back("yr");
		ConvertList(stringList);

		SEScalarVolume volume;
    volume.SetValue(1.0, VolumeUnit::GetCompoundUnit("L"));
		stringList.clear();
		stringList.push_back("L");
		stringList.push_back("mL");
		stringList.push_back("m^3");
		ConvertList(stringList);

		SEScalarVolumePerPressure volumePerPressure;
    volumePerPressure.SetValue(1.0, VolumePerPressureUnit::GetCompoundUnit("L/Pa"));

		SEScalarVolumePerTime volumePerTime;
    volumePerTime.SetValue(1.0, VolumePerTimeUnit::GetCompoundUnit("L/s"));
		stringList.clear();
		stringList.push_back("L/s");
		stringList.push_back("mL/s");
		stringList.push_back("L/min");
		stringList.push_back("m^3/s");
		ConvertList(stringList);
	}
	catch (std::exception& e)
	{
		std::cout << "caught exception: " << e.what() << std::endl;
	}
}