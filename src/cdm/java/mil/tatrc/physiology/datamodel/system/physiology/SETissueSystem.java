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
import mil.tatrc.physiology.datamodel.bind.TissueSystemData;
import mil.tatrc.physiology.datamodel.properties.*;
import mil.tatrc.physiology.datamodel.system.SESystem;

public class SETissueSystem extends SEPhysiologySystem implements SESystem
{
  protected SEScalarVolumePerTime carbonDioxideProductionRate;
  protected SEScalarVolume        extracellularFluidVolume;
  protected SEScalarVolume        extravascularFluidVolume;
  protected SEScalarVolume        intracellularFluidVolume;
  protected SEScalar              intracellularFluidPH;  
  protected SEScalarVolumePerTime oxygenConsumptionRate;
  protected SEScalar				      respiratoryExchangeRatio;

  public SETissueSystem()
  {
    carbonDioxideProductionRate = null;
    extracellularFluidVolume = null;
    extravascularFluidVolume = null;
    intracellularFluidVolume = null;
    intracellularFluidPH = null;
    oxygenConsumptionRate = null;
    respiratoryExchangeRatio = null;
  }

  public void reset()
  {		
    if (carbonDioxideProductionRate != null)
      carbonDioxideProductionRate.invalidate();
    if (extracellularFluidVolume != null)
      extracellularFluidVolume.invalidate();
    if (extravascularFluidVolume != null)
      extravascularFluidVolume.invalidate();
    if (intracellularFluidVolume != null)
      intracellularFluidVolume.invalidate(); 
    if (intracellularFluidPH != null)
      	intracellularFluidPH.invalidate();
    if (oxygenConsumptionRate != null)
      oxygenConsumptionRate.invalidate();
    if (respiratoryExchangeRatio != null)
      respiratoryExchangeRatio.invalidate();
  }

  public boolean load(TissueSystemData in)
  {		
    if (in.getCarbonDioxideProductionRate() != null)
      getCarbonDioxideProductionRate().load(in.getCarbonDioxideProductionRate());   
    if (in.getExtracellularFluidVolume() != null)
      getExtracellularFluidVolume().load(in.getExtracellularFluidVolume());
    if (in.getExtravascularFluidVolume() != null)
      getExtravascularFluidVolume().load(in.getExtravascularFluidVolume());
    if (in.getIntracellularFluidVolume() != null)
      getIntracellularFluidVolume().load(in.getIntracellularFluidVolume());
    if (in.getIntracellularFluidPH() != null)
      getIntracellularFluidPH().load(in.getIntracellularFluidPH());
    if (in.getOxygenConsumptionRate() != null)
      getOxygenConsumptionRate().load(in.getOxygenConsumptionRate());
    if (in.getRespiratoryExchangeRatio() != null)
      getRespiratoryExchangeRatio().load(in.getRespiratoryExchangeRatio());
   
    return true;
  }

  public TissueSystemData unload()
  {
    TissueSystemData data = CDMSerializer.objFactory.createTissueSystemData();
    unload(data);
    return data;
  }

  protected void unload(TissueSystemData data)
  {		
    if (hasCarbonDioxideProductionRate())
      data.setCarbonDioxideProductionRate(carbonDioxideProductionRate.unload());    
    if (extracellularFluidVolume != null)
      data.setExtracellularFluidVolume(extracellularFluidVolume.unload());
    if (extravascularFluidVolume != null)
      data.setExtravascularFluidVolume(extravascularFluidVolume.unload());
    if (intracellularFluidVolume != null)
      data.setIntracellularFluidVolume(intracellularFluidVolume.unload());
    if (intracellularFluidPH != null)
      data.setIntracellularFluidPH(intracellularFluidPH.unload());
    if (hasOxygenConsumptionRate())
      data.setOxygenConsumptionRate(oxygenConsumptionRate.unload());
    if (hasRespiratoryExchangeRatio())
      data.setRespiratoryExchangeRatio(respiratoryExchangeRatio.unload());    
  }

  public boolean hasCarbonDioxideProductionRate()
  {
    return carbonDioxideProductionRate == null ? false : carbonDioxideProductionRate.isValid();
  }
  public SEScalarVolumePerTime getCarbonDioxideProductionRate()
  {
    if (carbonDioxideProductionRate == null)
      carbonDioxideProductionRate = new SEScalarVolumePerTime();
    return carbonDioxideProductionRate;
  }
  
  public boolean hasExtracellularFluidVolume()
  {
    return extracellularFluidVolume == null ? false : extracellularFluidVolume.isValid();
  }
  public SEScalarVolume getExtracellularFluidVolume()
  {
    if (extracellularFluidVolume == null)
      extracellularFluidVolume = new SEScalarVolume();
    return extracellularFluidVolume;
  }

  public boolean hasExtravascularFluidVolume()
  {
    return extravascularFluidVolume == null ? false : extravascularFluidVolume.isValid();
  }
  public SEScalarVolume getExtravascularFluidVolume()
  {
    if (extravascularFluidVolume == null)
      extravascularFluidVolume = new SEScalarVolume();
    return extravascularFluidVolume;
  }

  public boolean hasIntracellularFluidVolume()
  {
    return intracellularFluidVolume == null ? false : intracellularFluidVolume.isValid();
  }
  public SEScalarVolume getIntracellularFluidVolume()
  {
    if (intracellularFluidVolume == null)
      intracellularFluidVolume = new SEScalarVolume();
    return intracellularFluidVolume;
  }

  public boolean hasIntracellularFluidPH()
  {
    return intracellularFluidPH == null ? false : intracellularFluidPH.isValid();
  }
  public SEScalar getIntracellularFluidPH()
  {
    if (intracellularFluidPH == null)
      intracellularFluidPH = new SEScalar();
    return intracellularFluidPH;
  }

  public boolean hasOxygenConsumptionRate()
  {
    return oxygenConsumptionRate == null ? false : oxygenConsumptionRate.isValid();
  }
  public SEScalarVolumePerTime getOxygenConsumptionRate()
  {
    if (oxygenConsumptionRate == null)
      oxygenConsumptionRate = new SEScalarVolumePerTime();
    return oxygenConsumptionRate;
  }
  
  public boolean hasRespiratoryExchangeRatio()
  {
    return respiratoryExchangeRatio == null ? false : respiratoryExchangeRatio.isValid();
  }
  public SEScalar getRespiratoryExchangeRatio()
  {
    if (respiratoryExchangeRatio == null)
      respiratoryExchangeRatio = new SEScalar();
    return respiratoryExchangeRatio;
  }
}
