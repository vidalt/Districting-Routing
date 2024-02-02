#ifndef ILS_H
#define ILS_H

#include "LocalSearch.h"
#include <stdlib.h>
#include <stdio.h>

class ILS{

public:

    ILS(Instance* instance, CostEvaluator* costEvaluator, LocalSearch localSearch);

    void execute();

private:
    CostEvaluator* costEvaluator;
    Instance* instance;
    LocalSearch localSearch;

    Solution* createInitialSolution();
};

#endif