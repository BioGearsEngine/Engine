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

// Testing some basic circuit functionality testing
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

#include "utils/DataTrack.h"
#include "properties/SEDecimalFormat.h"

void CommonDataModelTest::ScalarTest(const std::string& sOutputDirectory)
{
  std::ofstream file;
  SEDecimalFormat dfault;
  dfault.SetNotation(DecimalNotation::Default);
  dfault.SetPrecision(6);
  SEDecimalFormat fixed;
  dfault.SetNotation(DecimalNotation::Fixed);
  dfault.SetPrecision(8);
  SEDecimalFormat sci;
  dfault.SetNotation(DecimalNotation::Scientific);
  dfault.SetPrecision(4);

  DataTrack track;
  track.SetFormatting("Default", dfault);
  track.SetFormatting("Fixed", fixed);
  track.SetFormatting("Scientific", sci);
  track.CreateFile("TrackText.txt", file);
  for (int i = 0; i < 10; i++)
  {
    double d = std::sqrt(i)+100;
    track.Probe("Default", d);
    track.Probe("Fixed", d);
    track.Probe("Scientific", d);
    track.StreamProbesToFile(i, file);
    std::cout << d << std::endl;
  }


  CCompoundUnit g_Per_m3("g/m^3");
  CCompoundUnit ug_Per_mL("ug/mL"); 
  Convert(1, g_Per_m3, ug_Per_mL);
  
  SEScalarLength d;
  d.SetValue(1, LengthUnit::m);
  std::cout << d << std::endl;
  std::cout << d.GetValue(LengthUnit::m) << std::endl;
  std::cout << d.GetValue(LengthUnit::cm) << std::endl;

	SEScalarTemperature t;
	t.SetValue(5,TemperatureUnit::C);
	std::cout<<t.GetValue(TemperatureUnit::K)<<std::endl;
	t.SetValue(5,TemperatureUnit::K);
	std::cout<<t.GetValue(TemperatureUnit::C)<<std::endl;


	SEScalarFlowResistance r;
  r.SetValue(0, FlowResistanceUnit::GetCompoundUnit("mmHg s/mL"));

	SEScalarVolume v;
	v.SetValue(1,VolumeUnit::L);
	double m3 = v.GetValue(VolumeUnit::m3);//0.0010000000000000000
	v.IncrementValue(-2*6,VolumeUnit::m3);
	m3 = v.GetValue(VolumeUnit::m3);//m3 = -1.9990000000000001

	SEScalarPressure p;
	p.SetValue(1,PressureUnit::Pa);

	//
	//if(!p.IsValidUnit("Pa"))
	//	std::cerr<<"Pa should be a valid Pressure unit"<<std::endl;
	//if(p.IsValidUnit("Pap"))
	//	std::cerr<<"Pap should NOT be a valid Pressure unit"<<std::endl;
	//p.SetValue(5,PressureUnit::Pa);
	//std::cout<<"Testing Scalar "<<p.GetValue(PressureUnit::Pa)<<PressureUnit::Pa<<std::endl;
	//std::cout<<"Testing Scalar "<<p.GetValue(PressureUnit::mmHg)<<PressureUnit::mmHg<<std::endl;
	//std::cout<<"Testing Scalar "<<p.GetValue(PressureUnit::cmH2O)<<PressureUnit::cmH2O<<std::endl;
}