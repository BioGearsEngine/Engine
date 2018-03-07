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

#include "stdafx.h"
#include "substance/SESubstanceManager.h"
#include "substance/SESubstance.h"
#include "bind/SubstanceData.hxx"
#include "substance/SESubstanceCompound.h"
#include "bind/SubstanceCompoundData.hxx"
#include "utils/FileUtils.h"
#include "Serializer.h"
#include "../utils/unitconversion/UnitConversionEngine.h"
#include "dirent.h"

SESubstanceManager::SESubstanceManager(Logger* logger) : Loggable(logger)
{
	Clear();
}

SESubstanceManager::~SESubstanceManager()
{
	Clear();
}

void SESubstanceManager::Clear()
{
	DELETE_VECTOR(m_Substances);
	DELETE_VECTOR(m_Compounds);
	m_ActiveSubstances.clear();
	m_ActiveCompounds.clear();
	m_ActiveGases.clear();
	m_ActiveLiquids.clear();
  DELETE_MAP_SECOND(m_OriginalCompoundData);
  DELETE_MAP_SECOND(m_OriginalSubstanceData);
}

void SESubstanceManager::Reset()
{
  m_ActiveCompounds.clear();
  m_ActiveSubstances.clear();
  m_ActiveGases.clear();
  m_ActiveLiquids.clear();
  for (auto itr : m_OriginalSubstanceData)
    itr.first->Load(*itr.second);
  for (auto itr : m_OriginalCompoundData)
    itr.first->Load(*itr.second, *this);
}

/**
 * TRANSFERS OWNERSHIP TO THE MANAGER
 */
void SESubstanceManager::AddSubstance(SESubstance& substance) 
{
	SESubstance* s;
	for(unsigned int i=0; i<m_Substances.size(); i++)
	{
		s=m_Substances.at(i);
		if(s==&substance)
			return;
	}
	m_Substances.push_back(&substance);
}

SESubstance* SESubstanceManager::GetSubstance(const std::string& name) const
{
	SESubstance* s;
	for(unsigned int i=0; i<m_Substances.size(); i++)
	{
		s=m_Substances.at(i);
		if(name.compare(s->GetName())==0)
			return s;
	}
	return nullptr;
}

const std::vector<SESubstance*>& SESubstanceManager::GetSubstances() const
{
	return m_Substances;
}

bool SESubstanceManager::IsActive(const SESubstance& substance) const
{
	SESubstance* sub;
	for(unsigned int iSubstance=0; iSubstance < m_ActiveSubstances.size(); iSubstance++)
	{
		sub=m_ActiveSubstances.at(iSubstance); 
		if(sub==&substance)
			return true;
	}
	return false;
}

const std::vector<SESubstance*>& SESubstanceManager::GetActiveSubstances() const
{
	return m_ActiveSubstances;
}

void SESubstanceManager::AddActiveSubstance(SESubstance& substance) 
{
	if (IsActive(substance))
		return;
	if(substance.GetState()==CDM::enumSubstanceState::Gas)
		m_ActiveGases.push_back(&substance);
	if(substance.GetState()==CDM::enumSubstanceState::Liquid)
		m_ActiveLiquids.push_back(&substance);
    m_ActiveSubstances.push_back(&substance);
}

void SESubstanceManager::RemoveActiveSubstance(const SESubstance& substance)
{
	SESubstance* sub;
	for(unsigned int iSubstance=0; iSubstance < m_ActiveSubstances.size(); iSubstance++)
	{
		sub=m_ActiveSubstances.at(iSubstance); 
		if(sub==&substance)
		{
			m_ActiveSubstances.erase(m_ActiveSubstances.begin()+iSubstance);
			break;
		}
	}
	for(unsigned int iSubstance=0; iSubstance < m_ActiveGases.size(); iSubstance++)
	{
		sub=m_ActiveGases.at(iSubstance); 
		if(sub==&substance)
		{
			m_ActiveGases.erase(m_ActiveGases.begin()+iSubstance);
			break;
		}
	}
	for(unsigned int iSubstance=0; iSubstance < m_ActiveLiquids.size(); iSubstance++)
	{
		sub=m_ActiveLiquids.at(iSubstance); 
		if(sub==&substance)
		{
			m_ActiveLiquids.erase(m_ActiveLiquids.begin()+iSubstance);
			break;
		}
	}
}

void SESubstanceManager::RemoveActiveSubstances(const std::vector<SESubstance*>& substances)
{
	for(SESubstance* sub : substances)
		RemoveActiveSubstance(*sub);
}

void SESubstanceManager::RemoveActiveSubstances()
{
  std::vector<SESubstance*> copy(m_ActiveSubstances);
  for (SESubstance* sub : copy)
    RemoveActiveSubstance(*sub);
}


const std::vector<SESubstance*>& SESubstanceManager::GetActiveGases() const
{
	return m_ActiveGases;
}

const std::vector<SESubstance*>& SESubstanceManager::GetActiveLiquids() const
{
	return m_ActiveLiquids;
}

void SESubstanceManager::AddCompound(SESubstanceCompound& compound) 
{
	SESubstanceCompound* c;
	for(unsigned int i=0; i<m_Compounds.size(); i++)
	{
		c=m_Compounds.at(i);
		if(c==&compound)
			return;
	}
	m_Compounds.push_back(&compound);
}

SESubstanceCompound* SESubstanceManager::GetCompound(const std::string& name) const
{
	SESubstanceCompound* c;
	for(unsigned int i=0; i<m_Compounds.size(); i++)
	{
		c=m_Compounds.at(i);
		if(name.compare(c->GetName())==0)
			return c;
	}
	return nullptr;
}

const std::vector<SESubstanceCompound*>& SESubstanceManager::GetCompounds() const
{
	return m_Compounds;
}

bool SESubstanceManager::IsActive(const SESubstanceCompound& compound) const
{
	SESubstanceCompound* c;
	for(unsigned int i=0; i < m_ActiveCompounds.size(); i++)
	{
		c=m_ActiveCompounds.at(i); 
		if(c==&compound)
			return true;
	}
	return false;
}

const std::vector<SESubstanceCompound*>& SESubstanceManager::GetActiveCompounds() const
{
	return m_ActiveCompounds;
}

void SESubstanceManager::AddActiveCompound(SESubstanceCompound& compound) 
{
	SESubstanceCompound* c;
	for(unsigned int i=0; i < m_ActiveCompounds.size(); i++)
	{
		c=m_ActiveCompounds.at(i); 
		if(c==&compound)
			return;
	}
    m_ActiveCompounds.push_back(&compound);
}

void SESubstanceManager::RemoveActiveCompound(SESubstanceCompound& compound)
{
	unsigned int i = 0;
	for (SESubstanceCompound* c : m_ActiveCompounds)
	{
		if(c==&compound)
		{
			m_ActiveCompounds.erase(m_ActiveCompounds.begin()+i);
			break;
		}
		i++;
	}
}

void SESubstanceManager::RemoveActiveCompounds(const std::vector<SESubstanceCompound*>& compounds)
{
	for (SESubstanceCompound* c : compounds)
		RemoveActiveCompound(*c);
}

SESubstance* SESubstanceManager::ReadSubstanceFile(const std::string &xmlFile)
{
	std::stringstream ss;
	CDM::ObjectData *obj;

	SESubstance* sub;
	CDM::SubstanceData *subData;

	std::unique_ptr<CDM::ObjectData> data;

	obj=nullptr;
	sub=nullptr;
	subData=nullptr;
			
	data=Serializer::ReadFile(xmlFile,GetLogger());
	//ss.str("");				
	//ss<<"Reading substance file : ./substances/"<<xmlFile;				
	//Info(ss);
	obj = data.get();
	
	subData = dynamic_cast<CDM::SubstanceData*>(obj);
	if(subData!=nullptr)
	{
		sub = new SESubstance(GetLogger());
		sub->Load(*subData);
		return sub;
	}
	ss.str("");
	/// \error Substance file could not be read in
	ss<<"Substance file could not be read in : ./substances/"<<xmlFile;		
	Error(ss,"SESubstanceManager::ReadSubstanceFile");
	return nullptr;
}

bool SESubstanceManager::LoadSubstanceDirectory()
{
	bool succeed = true;
	Clear();
	std::stringstream ss;	
	DIR *dir;
	struct dirent *ent;

  ScopedFileSystemLock lock;

  std::string workingDirectory = GetCurrentWorkingDirectory();

#if defined(_WIN32)
    dir = opendir("./substances/");
#else
    dir = opendir(std::string(workingDirectory + std::string("/substances/")).c_str());
#endif

  if(dir != nullptr)
	{
		CDM::ObjectData *obj;

		SESubstance* sub;
		CDM::SubstanceData *subData;

		SESubstanceCompound* compound;
		CDM::SubstanceCompoundData *compoundData;

		std::unique_ptr<CDM::ObjectData> data;
		
		while ((ent = readdir(dir)) != nullptr)
		{
			obj = nullptr;
			sub = nullptr;
			subData = nullptr;
			ss.str("");
			ss << workingDirectory << "/substances/" << ent->d_name;
			if (!IsDirectory(ent) && strlen(ent->d_name) > 2)
			{
				data = Serializer::ReadFile(ss.str(),GetLogger());
				ss.str("");
				ss << "Reading substance file : ./substances/" << ent->d_name;
				Debug(ss);
				obj = data.release();
				subData = dynamic_cast<CDM::SubstanceData*>(obj);
				if (subData != nullptr)
				{
					sub = new SESubstance(GetLogger());
					sub->Load(*subData);
					AddSubstance(*sub);
					m_OriginalSubstanceData[sub] = subData;
					continue;
				}
				compoundData = dynamic_cast<CDM::SubstanceCompoundData*>(obj);
				if (compoundData != nullptr)
				{// Save this off and process it till later, once all substances are read
					compound = new SESubstanceCompound(GetLogger());
					m_OriginalCompoundData[compound] = compoundData;
					AddCompound(*compound);
					continue;
				}
				Error("Unknown Type");
				succeed = false;
			}			
		}// Done with directory search
		// Ok, now let's load up our compounds
		for (auto itr : m_OriginalCompoundData)
      itr.first->Load((const CDM::SubstanceCompoundData&)*itr.second, *this);			
		
		return succeed;

		closedir(dir);
		return succeed;
	}
	else
	{
		ss << "Unable to read files" << std::ends;
		Error(ss);
		succeed = false;
		return succeed;
	}
}

