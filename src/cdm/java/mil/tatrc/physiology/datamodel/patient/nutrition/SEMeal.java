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
package mil.tatrc.physiology.datamodel.patient.nutrition;

import mil.tatrc.physiology.datamodel.CDMSerializer;
import mil.tatrc.physiology.datamodel.bind.MealData;
import mil.tatrc.physiology.datamodel.properties.SEScalarTime;

public class SEMeal extends SENutrition
{
	protected SEScalarTime	elapsedTime;
	
	public SEMeal()
	{		
	  super();
		this.elapsedTime=null;
	}
	
	public void reset()
	{
	  super.reset();
		if (elapsedTime != null)
		  elapsedTime.invalidate();	
	}
	
	public void copy(SEMeal from)
	{
		super.copy(from);
		if(from.hasElapsedTime())
			this.getElapsedTime().set(from.getElapsedTime());
	}
	
	public boolean load(MealData in)
	{
	  super.load(in);
		if (in.getElapsedTime() != null)
			getElapsedTime().load(in.getElapsedTime());
		return true;
	}
	
	public MealData unload()
	{
	  MealData data = CDMSerializer.objFactory.createMealData();
		unload(data);
		return data;
	}
	
	protected void unload(MealData data)
	{
	  super.unload(data);
		if (elapsedTime != null)
			data.setElapsedTime(elapsedTime.unload());
	}		
	
	public SEScalarTime getElapsedTime()
	{
		if (elapsedTime == null)
		  elapsedTime = new SEScalarTime();
		return elapsedTime;
	}
	public boolean hasElapsedTime()
	{
		return elapsedTime == null ? false : elapsedTime.isValid();
	}
}
