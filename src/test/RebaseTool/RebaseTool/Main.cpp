#include "RebaseTool.h"
#include "Arguments.h"

#include <iostream>

int main(int argc, char* argv[])
{
    RebaseTool rebase(Arguments(argc, argv));
    rebase.Rebase();

    return 0;
}