#ifndef MAIN
#define MAIN

#include <iostream>
#include <vector>
#include <string>
#include "CostEvaluator.h"
#include "CommandLineHelper.h"
#include "Instance.h"
#include "ILS.h"
#include "LocalSearch.h"

int main(int argc, char *argv[])
{
    Instance *instance = CommandLineHelper::getInstanceFromCommandLine(argc, argv);
    CostEvaluator *costEvaluator;

    costEvaluator = new NeuralNetworkEvaluator();
    LocalSearch localSearch = LocalSearch(instance);

    ILS solver = ILS(instance, costEvaluator, localSearch);
    solver.execute();
}

#endif