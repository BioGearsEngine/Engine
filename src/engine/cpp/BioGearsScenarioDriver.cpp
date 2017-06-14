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

#include "BioGearsScenarioDriver.h"

#include "Verification.h"
#include "BioGearsPhysiologyEngine.h"

bool BioGearsScenarioDriver::Configure(int argc, char* argv[])
{
    if (argc <= 1)
    {
        std::cerr << "Need scenario file or config file to execute" << std::endl;
        return false;
    }

    std::string file(argv[1]);
    if (file.find(".xml") != std::string::npos)
    {
        m_file = file;
        m_mode = RunMode::Scenario;
    }
    else if (file.find(".config") != std::string::npos)
    {
        m_file = file;
        m_mode = RunMode::Verification;
    }
    else
    {
        std::cerr << "Invalid file type, provide either a scenario file or verification config file" << std::endl;
        return false;
    }

    for (int i = 2; i < argc; ++i)
    {
        std::string argument(argv[i]);
        if (argument.at(0) == '-')
        {
            m_arguments.insert(argument.substr(1));
        }
    }
	return true;
}

void BioGearsScenarioDriver::Run()
{
    if (m_mode == RunMode::Scenario)
    {
        RunScenario();
    }
    else if (m_mode == RunMode::Verification)
    {
        RunVerification();
    }
    else
    {
        std::cerr << "Cannot run scenario driver in invalid state, please reconfigure with valid options" << std::endl;
        return;
    }
}

void BioGearsScenarioDriver::RunScenario()
{
	// Set up the log file
	std::string logFile = m_file;
	logFile = Replace(logFile, "verification", "bin");
	logFile = Replace(logFile, ".xml", ".log");
	// Set up the verification output file	
	std::string dataFile = m_file;
	dataFile = Replace(dataFile, "verification", "bin");
	dataFile = Replace(dataFile, ".xml", "Results.txt");
	// Delete any results file that may be there
	remove(dataFile.c_str());
	std::unique_ptr<PhysiologyEngine> bioGears = CreateBioGearsEngine(logFile.c_str());
	if (!bioGears)
	{
		std::cerr << "Unable to create BioGearsEngine" << std::endl;
		return;
	}
	try
	{
		BioGearsScenarioExec exec(*bioGears);
		exec.Execute(m_file.c_str(), dataFile.c_str(), nullptr);
	}
	catch (std::exception ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unable to run scenario " << m_file << std::endl;
	}
}

void BioGearsScenarioDriver::RunVerification()
{
    Verification::RunMode mode = Verification::RunMode::Default;
    if (HasArgument("runall"))
    {
        mode = Verification::RunMode::All;
    }
    else if (HasArgument("runred"))
    {
        mode = Verification::RunMode::KnownFailing;
    }

    Verification verifier(m_file, mode);
    verifier.Verify();
}

bool BioGearsScenarioDriver::HasArgument(const std::string& argument)
{
    return m_arguments.find(argument) != end(m_arguments);
}

int main(int argc, char* argv[])
{
	try
	{
		BioGearsScenarioDriver scenarioDriver;
		if (!scenarioDriver.Configure(argc, argv))
		{
			return 1;
		}
		scenarioDriver.Run();
	}
	catch (std::exception ex)
	{
		std::cerr << ex.what() << std::endl;
	}

    return 0;
}
