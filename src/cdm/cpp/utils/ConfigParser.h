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

#ifdef REBASE_TOOL
#define DLL_DECL
#else
#include "CommonDataModel.h"
#endif

#include <string>
#include <vector>
#include <map>

class DLL_DECL ConfigSet
{
public:
    void SetKeyValues(const std::map<std::string, std::string>& keyValues);
    void AddKeyValue(const std::string& key, const std::string& value);
    const std::map<std::string, std::string>& GetKeyValues() const { return m_keyValues; }

    bool HasKey(const std::string& key) const;
    std::string GetValue(const std::string& key) const;

private:
    std::map<std::string, std::string> m_keyValues;
};

class DLL_DECL ConfigParser
{
public:
    ConfigParser(const std::string& configFilePath);

    const std::vector<ConfigSet>& GetConfigSets() const { return m_configSets; }

private:
    void ParseConfigFile(const std::string& configFile);
    std::tuple<std::string, std::string> ParseKeyValue(const std::string& line);

    std::map<std::string, std::string> m_globalKeyValues;
    std::vector<ConfigSet> m_configSets;

    bool m_parsingGlobalKeyValues = true;
};