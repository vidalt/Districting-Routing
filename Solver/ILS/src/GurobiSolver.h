#ifndef GUROBISOLVER_H
#define GUROBISOLVER_H

#include "Solution.h"
#include "gurobi_c++.h"
#include "utils.h"

class GurobiSolver{

public:

    GurobiSolver();
    Solution* createInitialSolution(Instance* instance);

private:
    Solution* getSolutionFromFlow(vector<vector<int>> flows, Instance* instance);
};

#endif