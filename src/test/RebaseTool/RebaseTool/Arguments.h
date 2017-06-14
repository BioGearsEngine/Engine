#pragma once

#include <vector>
#include <tuple>
#include <string>

class Arguments
{
public:
    Arguments(int argc, char* argv[]);
    ~Arguments() = default;

    bool Valid() const { return m_valid; }

    const std::string& GetOperand() const { return m_operand; }

    bool HasOption(const std::string& option) const;
    bool HasAnyOption(const std::vector<std::string>& options) const;

    std::string GetOptionValue(const std::string& option) const;
    std::string GetAnyOptionValue(const std::vector<std::string>& options) const;

private:
    void Parse(int argc, char* argv[]);

    std::vector<std::tuple<std::string, std::string>> m_options;
    std::string m_operand;

    bool m_valid = true;
};