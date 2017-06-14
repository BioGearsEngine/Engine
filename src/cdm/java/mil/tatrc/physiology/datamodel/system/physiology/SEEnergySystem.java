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

package mil.tatrc.physiology.datamodel.system.physiology;

import mil.tatrc.physiology.datamodel.CDMSerializer;
import mil.tatrc.physiology.datamodel.bind.EnergySystemData;
import mil.tatrc.physiology.datamodel.properties.*;
import mil.tatrc.physiology.datamodel.system.SESystem;

public class SEEnergySystem extends SEPhysiologySystem implements SESystem
{
  protected SEScalarFraction      achievedExerciseLevel;
  protected SEScalarTemperature 	coreTemperature;
  protected SEScalarAmountPerTime creatinineProductionRate;
  protected SEScalarPressure      exerciseMeanArterialPressureDelta;
  protected SEScalarFraction      fatigueLevel;
  protected SEScalarAmountPerTime ketoneProductionRate;
  protected SEScalarAmountPerTime lactateProductionRate;
  protected SEScalarTemperature 	skinTemperature;
  protected SEScalarMassPerTime 	sweatRate;
  protected SEScalarPower 		    totalMetabolicRate;
  protected SEScalarFraction      totalWorkRateLevel;

  public SEEnergySystem()
  {
    coreTemperature = null;
    creatinineProductionRate = null;
    exerciseMeanArterialPressureDelta = null;
    achievedExerciseLevel = null;
    fatigueLevel = null;
    ketoneProductionRate = null;
    lactateProductionRate = null;
    skinTemperature = null;
    sweatRate = null;
    totalMetabolicRate = null;
    totalWorkRateLevel = null;
  }

  public void reset()
  {		
    if (coreTemperature != null)
      coreTemperature.invalidate();
    if (creatinineProductionRate != null)
      creatinineProductionRate.invalidate();
    if (exerciseMeanArterialPressureDelta != null)
      exerciseMeanArterialPressureDelta.invalidate();
    if (achievedExerciseLevel != null)
      achievedExerciseLevel.invalidate();
    if (fatigueLevel != null)
      fatigueLevel.invalidate();
    if (ketoneProductionRate != null)
      ketoneProductionRate.invalidate();
    if (lactateProductionRate != null)
      lactateProductionRate.invalidate();
    if (skinTemperature != null)
      skinTemperature.invalidate();
    if (sweatRate != null)
      sweatRate.invalidate();
    if (totalMetabolicRate != null)
      totalMetabolicRate.invalidate();
    if (totalWorkRateLevel != null)
      totalWorkRateLevel.invalidate();    
  }

  public boolean load(EnergySystemData in)
  {		
    if (in.getCoreTemperature() != null)
      getCoreTemperature().load(in.getCoreTemperature());
    if (in.getCreatinineProductionRate() != null)
      getCreatinineProductionRate().load(in.getCreatinineProductionRate());
    if (in.getExerciseMeanArterialPressureDelta() != null)
      getExerciseMeanArterialPressureDelta().load(in.getExerciseMeanArterialPressureDelta());
    if (in.getAchievedExerciseLevel() != null)
      getAchievedExerciseLevel().load(in.getAchievedExerciseLevel());
    if (in.getFatigueLevel() != null)
      getFatigueLevel().load(in.getFatigueLevel());
    if (in.getKetoneProductionRate() != null)
      getKetoneProductionRate().load(in.getKetoneProductionRate());
    if (in.getLactateProductionRate() != null)
      getLactateProductionRate().load(in.getLactateProductionRate());
    if (in.getSkinTemperature() != null)
      getSkinTemperature().load(in.getSkinTemperature());
    if (in.getSweatRate() != null)
      getSweatRate().load(in.getSweatRate());
    if (in.getTotalMetabolicRate() != null)
      getTotalMetabolicRate().load(in.getTotalMetabolicRate());
    if (in.getTotalWorkRateLevel() != null)
      getTotalWorkRateLevel().load(in.getTotalWorkRateLevel());
    
    return true;
  }

  public EnergySystemData unload()
  {
    EnergySystemData data = CDMSerializer.objFactory.createEnergySystemData();
    unload(data);
    return data;
  }

  protected void unload(EnergySystemData data)
  {		
    if (hasCoreTemperature())
      data.setCoreTemperature(coreTemperature.unload());
    if (hasCreatinineProductionRate())
      data.setCreatinineProductionRate(creatinineProductionRate.unload());
    if (hasExerciseMeanArterialPressureDelta())
      data.setExerciseMeanArterialPressureDelta(exerciseMeanArterialPressureDelta.unload());
    if (hasAchievedExerciseLevel())
      data.setAchievedExerciseLevel(achievedExerciseLevel.unload());
    if (hasFatigueLevel())
      data.setFatigueLevel(fatigueLevel.unload());
    if (hasKetoneProductionRate())
      data.setKetoneProductionRate(ketoneProductionRate.unload());
    if (hasLactateProductionRate())
      data.setLactateProductionRate(lactateProductionRate.unload());
    if (hasSkinTemperature())
      data.setSkinTemperature(skinTemperature.unload());
    if (hasSweatRate())
      data.setSweatRate(sweatRate.unload());
    if (hasTotalMetabolicRate())
      data.setTotalMetabolicRate(totalMetabolicRate.unload());
    if (hasTotalWorkRateLevel())
      data.setTotalWorkRateLevel(totalWorkRateLevel.unload());    
  }

  public boolean hasCoreTemperature()
  {
    return coreTemperature == null ? false : coreTemperature.isValid();
  }
  public SEScalarTemperature getCoreTemperature()
  {
    if (coreTemperature == null)
      coreTemperature = new SEScalarTemperature();
    return coreTemperature;
  }

  public boolean hasCreatinineProductionRate()
  {
    return creatinineProductionRate == null ? false : creatinineProductionRate.isValid();
  }
  public SEScalarAmountPerTime getCreatinineProductionRate()
  {
    if (creatinineProductionRate == null)
      creatinineProductionRate = new SEScalarAmountPerTime();
    return creatinineProductionRate;
  }

  public boolean hasExerciseMeanArterialPressureDelta()
  {
    return exerciseMeanArterialPressureDelta == null ? false : exerciseMeanArterialPressureDelta.isValid();
  }
  public SEScalarPressure getExerciseMeanArterialPressureDelta()
  {
    if (exerciseMeanArterialPressureDelta == null)
      exerciseMeanArterialPressureDelta = new SEScalarPressure();
    return exerciseMeanArterialPressureDelta;
  }
  
  public boolean hasAchievedExerciseLevel()
  {
    return achievedExerciseLevel == null ? false : achievedExerciseLevel.isValid();
  }
  public SEScalarFraction getAchievedExerciseLevel()
  {
    if (achievedExerciseLevel == null)
      achievedExerciseLevel = new SEScalarFraction();
    return achievedExerciseLevel;
  }
  
  public boolean hasFatigueLevel()
  {
    return fatigueLevel == null ? false : fatigueLevel.isValid();
  }
  public SEScalarFraction getFatigueLevel()
  {
    if (fatigueLevel == null)
      fatigueLevel = new SEScalarFraction();
    return fatigueLevel;
  }

  public boolean hasKetoneProductionRate()
  {
    return ketoneProductionRate == null ? false : ketoneProductionRate.isValid();
  }
  public SEScalarAmountPerTime getKetoneProductionRate()
  {
    if (ketoneProductionRate == null)
      ketoneProductionRate = new SEScalarAmountPerTime();
    return ketoneProductionRate;
  }

  public boolean hasLactateProductionRate()
  {
    return lactateProductionRate == null ? false : lactateProductionRate.isValid();
  }
  public SEScalarAmountPerTime getLactateProductionRate()
  {
    if (lactateProductionRate == null)
      lactateProductionRate = new SEScalarAmountPerTime();
    return lactateProductionRate;
  }

  public boolean hasSkinTemperature()
  {
    return skinTemperature == null ? false : skinTemperature.isValid();
  }
  public SEScalarTemperature getSkinTemperature()
  {
    if (skinTemperature == null)
      skinTemperature = new SEScalarTemperature();
    return skinTemperature;
  }

  public boolean hasSweatRate()
  {
    return sweatRate == null ? false : sweatRate.isValid();
  }
  public SEScalarMassPerTime getSweatRate()
  {
    if (sweatRate == null)
      sweatRate = new SEScalarMassPerTime();
    return sweatRate;
  }

  public boolean hasTotalMetabolicRate()
  {
    return totalMetabolicRate == null ? false : totalMetabolicRate.isValid();
  }
  public SEScalarPower getTotalMetabolicRate()
  {
    if (totalMetabolicRate == null)
      totalMetabolicRate = new SEScalarPower();
    return totalMetabolicRate;
  }
  
  public boolean hasTotalWorkRateLevel()
  {
    return totalWorkRateLevel == null ? false : totalWorkRateLevel.isValid();
  }
  public SEScalarFraction getTotalWorkRateLevel()
  {
    if (totalWorkRateLevel == null)
      totalWorkRateLevel = new SEScalarFraction();
    return totalWorkRateLevel;
  }
  
  
}
