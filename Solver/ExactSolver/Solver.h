#ifndef SOLVER_H
#define SOLVER_H

#include "../ILS/src/CommandLineHelper.h"
#include "CostEvaluator.h"
#include "GurobiSolver.h"
#include <bits/stdc++.h>

class Solver
{

public:
    Solver(Instance *instance);
    void run();

private:
    Instance *instance;

    void getPossibleDistricst(int currentBlockId,
                              int initialBlockId,
                              set<int> currentBlocks,
                              set<int> currentAdjacencyList = set<int>(),
                              set<int> saturatedNodes = set<int>(),
                              int currentDepth = 1);

    map<string, double> districtCosts;
    GurobiSolver grbSolver;
    CostEvaluator costEvaluator;
    map<int, int> tmp;
};

#endif