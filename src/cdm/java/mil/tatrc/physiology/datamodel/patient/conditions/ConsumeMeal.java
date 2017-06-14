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

package mil.tatrc.physiology.datamodel.patient.conditions;

import mil.tatrc.physiology.datamodel.CDMSerializer;
import mil.tatrc.physiology.datamodel.bind.ConsumeMealData;
import mil.tatrc.physiology.datamodel.patient.nutrition.SEMeal;

public class ConsumeMeal extends SEPatientCondition
{
	protected SEMeal       meal;
	protected String       mealFile;
  
	public ConsumeMeal()
	{
		this.meal=new SEMeal();
	}
	
	public ConsumeMeal(ConsumeMeal other)
	{
		this();
		copy(other);		
	}
	
	public void copy(ConsumeMeal other)
	{
	  if(this==other)
      return;
    super.copy(other);   
		this.meal.copy(other.meal);
		this.mealFile=other.mealFile;
	}
	
	public void reset()
	{
		super.reset();  
		this.meal.reset();
		this.mealFile="";
	}
	
	public boolean load(ConsumeMealData in)
	{
		super.load(in);  
		if(in.getMeal()!=null)
			this.meal.load(in.getMeal());
		else if(in.getMealFile()!=null)
		  this.mealFile=in.getMealFile();
		return true;
	}
	
	public ConsumeMealData unload()
	{
		ConsumeMealData data = CDMSerializer.objFactory.createConsumeMealData();
		unload(data);
		return data;
	}
	
	protected void unload(ConsumeMealData data)
	{
		super.unload(data);  
		if(this.hasMeal())
		  data.setMeal(this.meal.unload());
		else if(this.hasMealFile())
		  data.setMealFile(this.mealFile);
	}
	
	public boolean hasMeal()
	{
	  return this.meal!=null;
	}
	public SEMeal getMeal()
	{
		return this.meal;
	}
	
	public boolean hasMealFile()
	{
	  return this.mealFile!=null&&!this.mealFile.isEmpty();
	}
	public String getMealFile()
	{
	  return this.mealFile;
	}
	public void setMealFile(String s)
	{
	  this.mealFile = s;
	}
	
	public String toString()
	{
	  String str="Consume Meal:";
		if (meal != null)
		  str +="\n\tElapsedTime: " + (this.meal.hasElapsedTime()?this.meal.getElapsedTime():"None")
					+ "\n\tCarbohydrate: " + (this.meal.hasCarbohydrate()?this.meal.getCarbohydrate():"None")
					+ "\n\tFat: " + (this.meal.hasFat()?this.meal.getFat():"None")
					+ "\n\tProtein: " + (this.meal.hasProtein()?this.meal.getProtein():"None")
					+ "\n\tSodium: " + (this.meal.hasSodium()?this.meal.getSodium():"None")
					+ "\n\tWater: " + (this.meal.hasWater()?this.meal.getWater():"None");
		if(this.hasMealFile())
		  str += "\n\tFile: "+this.mealFile;

		return str;
	}
}