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

#pragma once
class SESubstanceManager;
CDM_BIND_DECL(SystemData)

class DLL_DECL SESystem : public Loggable
{
public:

	SESystem(Logger* logger);
	virtual ~SESystem();

  virtual void Clear();// Deletes all members
  
	/** @name GetScalar
	*   @brief - A reflextion type call that will return the Scalar associated
	*            with the string. ex. GetScalar("Hematocrit") will return the
	*            SEScalarPercent object associated with Hematocrit
	*   @details - Note this is an expensive call as it will string compare many times
	*              This is best used, and intended for, you to dynamically prepopulate
	*              a mapping data structure that will help access what you need
	*/
  virtual const SEScalar* GetScalar(const std::string& name) = 0; 

	/** @name GetScalar
	*   @brief - Look for the Scalar property in the systems contained in the provided vector
	*/
	static const SEScalar* GetScalar(const std::string& name, std::vector<SESystem*>* systems);
	
	virtual bool Load(const CDM::SystemData& in);
  virtual CDM::SystemData* Unload() const = 0;
protected:
	virtual void Unload(CDM::SystemData& data) const;

	std::stringstream m_ss;
};
