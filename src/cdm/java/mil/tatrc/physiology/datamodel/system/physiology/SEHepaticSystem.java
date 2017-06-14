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
import mil.tatrc.physiology.datamodel.bind.HepaticSystemData;
import mil.tatrc.physiology.datamodel.properties.*;
import mil.tatrc.physiology.datamodel.system.SESystem;

public class SEHepaticSystem extends SEPhysiologySystem implements SESystem
{
	public SEHepaticSystem()
	{
		
	}

	public void reset()
	{		
		
	}

	public boolean load(HepaticSystemData in)
	{				
		return true;
	}

	public HepaticSystemData unload()
	{
		HepaticSystemData data = CDMSerializer.objFactory.createHepaticSystemData();
		unload(data);
		return data;
	}

	protected void unload(HepaticSystemData data)
	{		
		
	}

	
}
