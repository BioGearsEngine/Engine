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

package mil.tatrc.physiology.datamodel.patient.actions;


import java.util.*;
import mil.tatrc.physiology.datamodel.CDMSerializer;
import mil.tatrc.physiology.datamodel.bind.HemorrhageData;
import mil.tatrc.physiology.datamodel.bind.IntegerArray;

public class SEHemorrhage extends SEPatientAction
{
	protected String compartment;
	protected IntegerArray mcis;
	protected String bleedname;
	Map<List<Integer>,String> organMap = new HashMap<List<Integer>,String>();
	
	public SEHemorrhage()
	{
		compartment = null;
		mcis = null;
		bleedname=null;
		makeOrganMap(organMap);
	}
	
	public void copy(SEHemorrhage other)
	{
		if(this==other)
			return;
		super.copy(other);
		mcis = other.mcis;

	}
	
	public void reset()
	{
		super.reset();
		compartment = null;
		mcis = null;

	}
	
	public boolean isValid()
	{
		return hasMCIS();
	}
	
	public boolean load(HemorrhageData in)
	{
		super.load(in);
    mcis = in.getMCIS();
    setCompartment();
		return isValid();
	}
	
	public HemorrhageData unload()
	{
		HemorrhageData data = CDMSerializer.objFactory.createHemorrhageData();
		unload(data);
		return data;
	}
	
	protected void unload(HemorrhageData data)
	{
		super.unload(data);
		if(mcis!=null)
			data.setMCIS(getMCIS());
	}
	
	public IntegerArray getMCIS()
	{
		if(mcis==null)
			mcis = new IntegerArray();
		return mcis;
	}
	public boolean hasMCIS()
	{
		return mcis == null ? false : !mcis.getIntegerList().isEmpty();
	}
	
	public String getCompartment()
	{
		return compartment;
	}
	public void setCompartment()
	{
		int region = mcis.getIntegerList().get(1);
		
		switch(region){
			case 1:
				if(mcis.getIntegerList().get(3)==1)
					compartment = "Head";
				else
					compartment = "Major Artery";
				break;
			case 2:
				if(organMap.containsKey(mcis.getIntegerList().subList(2, 4)))
					compartment=organMap.get(mcis.getIntegerList().subList(2, 4));
				else
					compartment="Major Artery";
				break;
			case 3:
				compartment = "Arm";
				break;
			case 4:
				compartment = "Leg";
				break;
			default:
				compartment = "Major Artery";
				break;
		}		
	}
	
	private void makeOrganMap(Map<List<Integer>, String> organs)
	{
		organs.put(new ArrayList<Integer>(Arrays.asList(6,4)), "Major Artery");
		organs.put(new ArrayList<Integer>(Arrays.asList(6,6)), "Vena Cava");
		organs.put(new ArrayList<Integer>(Arrays.asList(6,5)), "Major Artery");
		organs.put(new ArrayList<Integer>(Arrays.asList(7,1)), "Lungs");
		organs.put(new ArrayList<Integer>(Arrays.asList(7,2)), "Heart");
		organs.put(new ArrayList<Integer>(Arrays.asList(8,1)), "Liver");
		organs.put(new ArrayList<Integer>(Arrays.asList(8,2)), "Spleen");
		organs.put(new ArrayList<Integer>(Arrays.asList(8,3)), "Splanchnic");
		organs.put(new ArrayList<Integer>(Arrays.asList(8,4)), "Kidney");
		organs.put(new ArrayList<Integer>(Arrays.asList(8,5)), "Small Intestine");
		organs.put(new ArrayList<Integer>(Arrays.asList(8,6)), "Large Intestine");
	}
	
	public String toString()
	{
		if (mcis != null)
			{
					if(mcis.getIntegerList().get(0)==0)
					{
							return "Stop Hemorrhage"
									+"\n\tCompartment: " + getCompartment();
					}
					else
					{
						return "Hemorrhage"
								+"\n\tSeverity: " + Integer.toString(mcis.getIntegerList().get(0))
								+"\n\tCompartment: " + getCompartment();
					}
			}
		else
			return "Action not specified properly";
	}
}



