#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "Solution.h"
#include "CostEvaluator.h"
#include "Instance.h"

class LocalSearch
{
public:
    LocalSearch(Instance *instance);
    Solution *run(Solution *solution, CostEvaluator *costEvaluator);
    Solution *pertubate(Solution *solution);
    double swapTime;
    double relocateTime;
    double calculateBorderTime;
    double checkContiguityTime;
    double copyingSolutionTime;

private:
    Instance *instance;
    Solution *currentSolution;
    CostEvaluator *costEvaluator;
    pair<Solution*, bool> relocate(int idDistrict, int idBlock, bool ignoreMovementeCost = false);  // Return true if current solution was improved, can ignore movement cost - it helps at pertubation
    pair<Solution*, bool> swap(int idDistrictA, int idDistrictB, bool ignoreMovementeCost = false); // Return true if current solution was improved, can ignore movement cost - it helps at pertubation
    vector<vector<bool>> hasMovesToTry;
};

#endif