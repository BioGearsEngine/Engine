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

#include "HowTo-ThreadedBioGears.h"

#include "patient/actions/SEHemorrhage.h"
#include "patient/actions/SESubstanceCompoundInfusion.h"
#include "substance/SESubstanceManager.h"
#include "substance/SESubstanceCompound.h"

#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SERespiratorySystem.h"

#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"

void HowToDynamicHemorrhage()
{
  // Create the engine and have it run in it's own thread
  // This call will block while the engine stabilizes
  BioGearsThread bgThread("./ThreadedDriver.txt");
  // When it comes back, the engine will be running, waiting for your input

  int action;
  double rate;
  bool active = true;
  do
  {
    bgThread.GetLogger()->Info("Enter Interger for Action to Perform : [1]Status, [2]Hemorrhage, [3]IVFluids, [4]Quit");
    std::cin >> action;
    switch (action)
    {
    case 1:
      bgThread.Status();
      break;
    case 2:
      bgThread.GetLogger()->Info("Enter Hemorrhage Rate in mL/min : ");
      std::cin >> rate;
      bgThread.GetLogger()->Info(std::stringstream() << rate);
      bgThread.SetHemorrhageFlow_mL_Per_min(rate);
      break;
    case 3:
      bgThread.GetLogger()->Info("Enter IV Fluids Rate in mL/min : ");
      std::cin >> rate;
      bgThread.GetLogger()->Info(std::stringstream() << rate);
      bgThread.SetIVFluidsFlow_mL_Per_min(rate);
      break;
    case 4:
      active = false;
    }
  } while (active);
}

BioGearsThread::BioGearsThread(const std::string& logfile) : m_thread()
{
  // Create our engine with the standard patient
  m_bg = CreateBioGearsEngine(logfile);
  SESubstanceCompound* saline = m_bg->GetSubstanceManager().GetCompound("Saline");

  if (!m_bg->LoadState("./states/StandardMale@0s.xml"))
  {
    m_bg->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // Create and initialize our actions
  m_infusion = new SESubstanceCompoundInfusion(*saline);
  m_hemorrhage = new SEHemorrhage();
  m_hemorrhage->SetCompartment(BGE::VascularCompartment::RightLeg);//the location of the hemorrhage  

  // Start advancing time in a seperate thread
  m_runThread = true;
  m_thread = std::thread(&BioGearsThread::AdvanceTime, this);
}

BioGearsThread::~BioGearsThread()
{
  m_runThread = false;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  SAFE_DELETE(m_infusion);
  SAFE_DELETE(m_hemorrhage);
}

void BioGearsThread::SetHemorrhageFlow_mL_Per_min(double rate)
{
  m_hemorrhage->GetRate().SetValue(rate, VolumePerTimeUnit::mL_Per_min);//the rate of hemorrhage
  m_mutex.lock();
  m_bg->ProcessAction(*m_hemorrhage);
  m_mutex.unlock();
}

void BioGearsThread::SetIVFluidsFlow_mL_Per_min(double rate)
{
  // For this example, I am always resetting the bag volume on every change, but you may want to allow the bag to run out..
  m_infusion->GetBagVolume().SetValue(500, VolumeUnit::mL);//the total volume in the bag of Saline
  m_infusion->GetRate().SetValue(100, VolumePerTimeUnit::mL_Per_min);//The rate to admnister the compound in the bag in this case saline
  m_mutex.lock();
  m_bg->ProcessAction(*m_infusion);
  m_mutex.unlock();
}

void BioGearsThread::AdvanceTime()
{
  while(m_runThread)
  {
    m_mutex.lock();
    m_bg->AdvanceModelTime(1, TimeUnit::s);
    m_mutex.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(25));// let other things happen
  }
}

void BioGearsThread::Status()
{// On demand call to print vitals to the screen
  m_mutex.lock();
  m_bg->GetLogger()->Info("");
  m_bg->GetLogger()->Info(std::stringstream() <<"Simulation Time : " << m_bg->GetSimulationTime(TimeUnit::s) << "s");
  m_bg->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << m_bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  m_bg->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << m_bg->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
  m_bg->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << m_bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  m_bg->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << m_bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  m_bg->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << m_bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  m_bg->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << m_bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  m_bg->GetLogger()->Info(std::stringstream() <<"Respiration Rate : " << m_bg->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  m_bg->GetLogger()->Info("");
  m_mutex.unlock();
}