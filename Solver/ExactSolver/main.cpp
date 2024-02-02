#ifndef MAIN
#define MAIN

#include "../ILS/src/CommandLineHelper.h"
#include "../ILS/src/Instance.h"
#include "Solver.h"

using namespace std;

int main(int argc, char* argv[] )
{
    Instance* instance = CommandLineHelper::getInstanceFromCommandLine(argc, argv);
    Solver solver = Solver(instance);

    solver.run();
}

#endif