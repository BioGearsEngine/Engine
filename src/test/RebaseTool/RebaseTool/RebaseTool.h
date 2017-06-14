#pragma once

#include "Arguments.h"
#include "utils/ConfigParser.h"

class RebaseTool
{
public:
    RebaseTool(const Arguments& arguments);
    ~RebaseTool() = default;

    void Rebase();

private:
    Arguments m_arguments;
};