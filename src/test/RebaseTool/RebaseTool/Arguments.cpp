#include "Arguments.h"

#include <iostream>
#include <algorithm>

Arguments::Arguments(int argc, char* argv[])
{
    Parse(argc, argv);
}

void Arguments::Parse(int argc, char* argv[])
{
    if (argc < 2)
    {
        m_valid = false;

        return;
    }

    int currentArg = 1;
    while (currentArg < argc)
    {
        std::string arg = argv[currentArg];
        if (currentArg == argc - 1)
        {
            if (arg[0] != '-')
            {
                m_operand = arg;
            }
            else
            {
                m_valid = false;

                return;
            }
        }
        else if (arg.size() > 1 && arg[0] == '-')
        {
            if (arg[1] != '-')
            {
                std::string options = arg.substr(1);
                if (options.empty())
                {
                    m_valid = false;

                    return;
                }

                for (unsigned int a = 0; a < options.size(); ++a)
                {
                    m_options.push_back(
                        std::make_tuple(std::string(1, options[a]), ""));
                }

                if (options.size() == 1 && ((currentArg + 1) != (argc - 1)))
                {
                    arg = argv[currentArg + 1];
                    if (arg[0] != '-')
                    {
                        ++currentArg;

                        std::get<1>(m_options.back()) = arg;
                    }
                }
            }
            else
            {
                std::string option = arg.substr(2);
                if (option.empty())
                {
                    m_valid = false;

                    return;
                }

                m_options.push_back(std::make_tuple(option, ""));

                arg = argv[currentArg + 1];
                if (arg[0] != '-')
                {
                    ++currentArg;

                    std::get<1>(m_options.back()) = arg;
                }
            }
        }
        else
        {
            m_valid = false;

            return;
        }

        ++currentArg;
    }
}

bool Arguments::HasOption(const std::string& option) const
{
    auto find = std::find_if(begin(m_options), end(m_options),
        [&option](const std::tuple<std::string, std::string>& optionPair)
        {
            return std::get<0>(optionPair) == option;
        }
    );

    return find != end(m_options);
}

bool Arguments::HasAnyOption(const std::vector<std::string>& options) const
{
    auto find = std::find_if(begin(m_options), end(m_options),
        [&options](const std::tuple<std::string, std::string>& optionPair)
        {
            return std::find(begin(options), end(options), std::get<0>(optionPair)) != end(options);
        }
    );

    return find != end(m_options);
}

std::string Arguments::GetOptionValue(const std::string& option) const
{
    auto find = std::find_if(begin(m_options), end(m_options),
        [&option](const std::tuple<std::string, std::string>& optionPair)
        {
            return std::get<0>(optionPair) == option;
        }
    );

    if (find == end(m_options))
    {
        return "";
    }
    else
    {
        return std::get<1>(*find);
    }
}

std::string Arguments::GetAnyOptionValue(const std::vector<std::string>& options) const
{
    auto find = std::find_if(begin(m_options), end(m_options),
        [&options](const std::tuple<std::string, std::string>& optionPair)
        {
            return std::find(begin(options), end(options), std::get<0>(optionPair)) != end(options);
        }
    );

    if (find == end(m_options))
    {
        return "";
    }
    else
    {
        return std::get<1>(*find);
    }
}