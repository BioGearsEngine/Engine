#include "RebaseTool.h"

#include <fstream>
#include <iostream>

RebaseTool::RebaseTool(const Arguments& arguments)
    : m_arguments(arguments)
{
}

void RebaseTool::Rebase()
{
    if (!m_arguments.Valid())
    {
        std::cout << "Invalid arguments, expected usaged: RebaseTool.exe -options <config file>\n";

        return;
    }

    ConfigParser parser(m_arguments.GetOperand());
    for (const ConfigSet& configSet : parser.GetConfigSets())
    {
        for (const std::pair<std::string, std::string>& keyValue : configSet.GetKeyValues())
        {
            if (!keyValue.first.empty())
            {
                if (keyValue.first[0] == '-' || keyValue.first[0] == '*')
                {
                    continue;
                }
            }

            const std::string& scenarioFilePath = keyValue.second;

            std::string::size_type lastSlashPosition = scenarioFilePath.rfind('/');
            if (lastSlashPosition == std::string::npos)
            {
                continue;
            }

            std::string::size_type extensionPosition = scenarioFilePath.rfind(".xml");
            if (extensionPosition == std::string::npos)
            {
                continue;
            }

            std::string scenarioSubFolder = scenarioFilePath.substr(0, lastSlashPosition);
            std::string scenarioName = scenarioFilePath.substr(lastSlashPosition + 1, extensionPosition - (lastSlashPosition + 1));
            
            {
                std::string from = "Scenarios\\" + scenarioSubFolder + "\\" + scenarioName + "Results.txt";
                std::string to = "..\\verification\\Scenarios\\" + scenarioSubFolder + "\\Current Baseline\\" + scenarioName + "Results.txt";

                if (m_arguments.HasAnyOption({ "h", "history" }))
                {
                    std::string folder = m_arguments.GetAnyOptionValue({ "h", "history" });
                    from = "\\\\sed-biogears\\BGBuildHistory\\msvc\\" + folder + "\\" + from;
                }

                std::ifstream source(from, std::ios::binary);
                if (source.is_open())
                {
                    std::cout << "Rebasing " << scenarioSubFolder << "/" << scenarioName << " results\n";

                    std::ofstream destination(to, std::ios::binary);
                    destination << source.rdbuf();
                }
            }

            {
                std::string from = "Scenarios\\" + scenarioSubFolder + "\\" + scenarioName + ".log";
                std::string to = "..\\verification\\Scenarios\\" + scenarioSubFolder + "\\Current Baseline\\" + scenarioName + ".log";

                if (m_arguments.HasAnyOption({ "h", "history" }))
                {
                    std::string folder = m_arguments.GetAnyOptionValue({ "h", "history" });
                    from = "\\\\sed-biogears\\BGBuildHistory\\msvc\\" + folder + "\\" + from;
                }

                std::ifstream source(from, std::ios::binary);
                if (source.is_open())
                {
                    std::cout << "Rebasing " << scenarioSubFolder << "/" << scenarioName << " log\n";

                    std::ofstream destination(to, std::ios::binary);
                    destination << source.rdbuf();
                }
            }
        }
    }
}