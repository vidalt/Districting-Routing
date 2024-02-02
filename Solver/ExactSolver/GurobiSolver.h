#ifndef GUROBISOLVER_H
#define GUROBISOLVER_H

#include "../ILS/src/Solution.h"
#include "../ILS/src/Instance.h"
#include "gurobi_c++.h"
#include <bits/stdc++.h> 


class GurobiSolver{

public:

    GurobiSolver();
    Solution* findBestSolution(Instance* instance, map<string,double> districtCosts);
};

#endif