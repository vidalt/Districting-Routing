#include "LocalSearch.h"
#include <algorithm>
#include <iostream>

LocalSearch::LocalSearch(Instance *instance) : instance(instance){
    this->hasMovesToTry = vector<vector<bool>>(instance->numDistricts);

    for(int i=0 ; i < instance->numDistricts; i ++)
    {
        this->hasMovesToTry[i] = vector<bool>(i, true);
    }
};

Solution *LocalSearch::pertubate(Solution *solution)
{
    this->currentSolution = solution;

    vector<int> shuffledDistricts(instance->numDistricts);
    iota(begin(shuffledDistricts), end(shuffledDistricts), 0);
    shuffle(begin(shuffledDistricts), end(shuffledDistricts), instance->generator);
    
    for (int districtA : shuffledDistricts)
    {
        for (int districtB : shuffledDistricts)
        {
            if (districtA <= districtB)
                continue;

            vector<int> borderA = this->currentSolution->getDistrictsInBorder(districtA, districtB);
            vector<int> borderB = this->currentSolution->getDistrictsInBorder(districtB, districtA);

            for (int u : borderA)
            {
                if(instance->randomNumber() >= instance->probabilitySkipMovePertubation)
                {
                    pair<Solution*, bool> result = relocate(districtB, u, true);

                    if(result.second){
                        
                        for (int districtId=districtB+1; districtId < instance->numDistricts; districtId++)
                        {                            
                            this->hasMovesToTry[districtId][districtB] = true;

                            if(districtId > districtA)
                                this->hasMovesToTry[districtId][districtA] = true;
                        }
                    } 
                }

                for (int v : borderB)
                {
                    if(instance->randomNumber() >= instance->probabilitySkipMovePertubation)
                    {
                        pair<Solution*, bool> result = swap(u, v, true);
                        
                        if(result.second){

                            for (int districtId=districtB+1; districtId < instance->numDistricts; districtId++)
                            {
                                this->hasMovesToTry[districtId][districtB] = true;

                                if(districtId > districtA)
                                    this->hasMovesToTry[districtId][districtA] = true;
                            }
                        } 
                    }
                }
            }

            for (int v : borderB)
            {
                if(instance->randomNumber() >= instance->probabilitySkipMovePertubation)
                {
                    pair<Solution*, bool> result = relocate(districtA, v, true);

                    if(result.second){
                    
                        for (int districtId=districtB+1; districtId < instance->numDistricts; districtId++)
                        {
                            this->hasMovesToTry[districtId][districtB] = true;

                            if(districtId > districtA)
                                this->hasMovesToTry[districtId][districtA] = true;
                        }
                    } 
                }
            }
        }
    }

    costEvaluator->calculateCost(this->currentSolution);
    return this->currentSolution;
}

Solution *LocalSearch::run(Solution *solution, CostEvaluator *costEvaluator)
{
    this->currentSolution = solution;
    this->costEvaluator = costEvaluator;

    bool foundNewBetterSolution = true;

    while(foundNewBetterSolution)
    {
        foundNewBetterSolution = false;

        vector<int> shuffledDistricts(instance->numDistricts);
        iota(begin(shuffledDistricts), end(shuffledDistricts), 0);
        shuffle(begin(shuffledDistricts), end(shuffledDistricts), instance->generator);

        for (int districtA : shuffledDistricts)
        {
            for (int districtB : shuffledDistricts)
            {
                if (districtA <= districtB || !hasMovesToTry[districtA][districtB])
                    continue;

                Solution* currentBestSolution = this->currentSolution;

                vector<int> borderA = this->currentSolution->getDistrictsInBorder(districtA, districtB);
                vector<int> borderB = this->currentSolution->getDistrictsInBorder(districtB, districtA);

                for (int u : borderA)
                {
                    pair<Solution*, bool> result = relocate(districtB, u);

                    if (result.second && currentBestSolution->cost > result.first->cost)
                        currentBestSolution = result.first;

                    for (int v : borderB)
                    {
                        pair<Solution*, bool> result = swap(u, v);

                        if (result.second && currentBestSolution->cost > result.first->cost)
                            currentBestSolution = result.first;
                    }
                }

                for (int v : borderB)
                {
                    pair<Solution*, bool> result = relocate(districtA, v);

                    if (result.second && currentBestSolution->cost > result.first->cost)
                        currentBestSolution = result.first;
                }

                if (this->currentSolution->cost > currentBestSolution->cost)
                {
                    this->currentSolution = currentBestSolution;
                    foundNewBetterSolution = true;

                    for (int districtId=districtB+1; districtId < instance->numDistricts; districtId++)
                    {
                        this->hasMovesToTry[districtId][districtB] = true;

                        if(districtId > districtA)
                            this->hasMovesToTry[districtId][districtA] = true;
                    }

                }
                else
                {
                    this->hasMovesToTry[districtA][districtB] = false;
                }
            }
        }
    }

    return this->currentSolution;
}

pair<Solution*, bool> LocalSearch::relocate(int idTargetDistrict, int idBlock, bool ignoreMovementeCost)
{
    int idCurrentDistrict = this->currentSolution->blocksDistrict[idBlock];

    if (idCurrentDistrict == idTargetDistrict) 
        return pair<Solution*, bool>(this->currentSolution, false);


    set<int> *currentDistrict = &this->currentSolution->districts[idCurrentDistrict];
    set<int> *targetDistrict  = &this->currentSolution->districts[idTargetDistrict];
    
    // Check bounds
    if (currentDistrict->size() - 1 < instance->minSizeDistricts ||
        targetDistrict->size()  + 1 > instance->maxSizeDistricts)
        return pair<Solution*, bool> (this->currentSolution, false);

    // Copy data from original solution in order to rollback
    double originalCost = this->currentSolution->cost;
    double originalCostOfCurrentDistrict = this->currentSolution->costOfDistrict[idCurrentDistrict];
    double originalCostOfTargetDistrict  = this->currentSolution->costOfDistrict[idTargetDistrict];

    // Remove block from current district
    currentDistrict->erase(idBlock);
     // Insert it into target district
    targetDistrict->insert(idBlock);
    this->currentSolution->blocksDistrict[idBlock] = idTargetDistrict;

    if (!instance->isDistrictContigous(currentDistrict) ||
        !instance->isDistrictContigous(targetDistrict))
    {
        targetDistrict->erase(idBlock);
        currentDistrict->insert(idBlock);

        this->currentSolution->blocksDistrict[idBlock] = idCurrentDistrict;

        return pair<Solution*, bool> (this->currentSolution, false);
    }

    Solution* solutionToReturn = this->currentSolution;
    
    bool hasImprovedCost = false;

    if (!ignoreMovementeCost)
    {
        vector<int> districsToEvaluate = vector<int>({idCurrentDistrict, idTargetDistrict});
        double newCost = costEvaluator->calculateCost(this->currentSolution, districsToEvaluate);
        hasImprovedCost = newCost < originalCost;

        if (hasImprovedCost) {
            solutionToReturn = new Solution(this->currentSolution);
        }
    }
    else
    {
        return pair<Solution*, bool> (solutionToReturn, true);
    }
  
    currentDistrict->insert(idBlock);
    targetDistrict->erase(idBlock);
    
    this->currentSolution->blocksDistrict[idBlock] = idCurrentDistrict;
    this->currentSolution->costOfDistrict[idCurrentDistrict] = originalCostOfCurrentDistrict;
    this->currentSolution->costOfDistrict[idTargetDistrict]  = originalCostOfTargetDistrict;
    this->currentSolution->cost = originalCost;

    return pair<Solution*, bool> (solutionToReturn, hasImprovedCost);
}

pair<Solution*, bool> LocalSearch::swap(int idBlockA, int idBlockB, bool ignoreMovementeCost)
{   
    // Create copy of initial solution
    int idDistrictA = this->currentSolution->blocksDistrict[idBlockA];
    int idDistrictB = this->currentSolution->blocksDistrict[idBlockB];

    if (idDistrictA == idDistrictB) 
        return pair<Solution*, bool>(this->currentSolution, false);

    set<int> *districtA = &this->currentSolution->districts[idDistrictA];
    set<int> *districtB = &this->currentSolution->districts[idDistrictB];

    // Copy data from original solution in order to rollback
    double originalCost = this->currentSolution->cost;
    double originalCostOfDistrictA = this->currentSolution->costOfDistrict[idDistrictA];
    double originalCostOfDistrictB  = this->currentSolution->costOfDistrict[idDistrictB];

    // Erase block A and B from its respective districts
    districtA->erase(idBlockA);
    districtB->erase(idBlockB);

    // Insert block A into old's block B district and vice-versa
    districtA->insert(idBlockB);
    districtB->insert(idBlockA);

    this->currentSolution->blocksDistrict[idBlockA] = idDistrictB;
    this->currentSolution->blocksDistrict[idBlockB] = idDistrictA;

    if (!instance->isDistrictContigous(districtA) ||
        !instance->isDistrictContigous(districtB))
    {
        // Rollback movement
        districtA->erase(idBlockB);
        districtB->erase(idBlockA);

        districtA->insert(idBlockA);
        districtB->insert(idBlockB);

        this->currentSolution->blocksDistrict[idBlockA] = idDistrictA;
        this->currentSolution->blocksDistrict[idBlockB] = idDistrictB;

        return pair<Solution*, bool> (this->currentSolution, false);
    }

    Solution* solutionToReturn = this->currentSolution;
    
    bool hasImprovedCost = false;
    
    if (!ignoreMovementeCost)
    {
        vector<int> districsToEvaluate = vector<int>({idDistrictA, idDistrictB});
        
        double newCost = costEvaluator->calculateCost(this->currentSolution, districsToEvaluate);
        hasImprovedCost = newCost < originalCost;

        if (hasImprovedCost)
            solutionToReturn = new Solution(this->currentSolution);
    }
    else 
    {
        return pair<Solution*, bool> (solutionToReturn, true);
    }

    // Rollback movement
    districtA->erase(idBlockB);
    districtB->erase(idBlockA);

    districtA->insert(idBlockA);
    districtB->insert(idBlockB);

    this->currentSolution->blocksDistrict[idBlockA] = idDistrictA;
    this->currentSolution->blocksDistrict[idBlockB] = idDistrictB;

    this->currentSolution->costOfDistrict[idDistrictA]  = originalCostOfDistrictA;
    this->currentSolution->costOfDistrict[idDistrictB]  = originalCostOfDistrictB;

    this->currentSolution->cost = originalCost;

    return pair<Solution*, bool> (solutionToReturn, hasImprovedCost);
}
