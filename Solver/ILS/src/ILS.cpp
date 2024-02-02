#include "ILS.h"
#include "GurobiSolver.h"

#include <iostream>
#include <vector>
#include <chrono>
using namespace std;

ILS::ILS(Instance *instance, CostEvaluator *costEvaluator, LocalSearch localSearch)
    : instance(instance), localSearch(localSearch), costEvaluator(costEvaluator){};

void ILS::execute()
{
    auto wcts = std::chrono::system_clock::now();

    GurobiSolver grbSolver = GurobiSolver();
    Solution *initialSolution = grbSolver.createInitialSolution(instance);

    this->costEvaluator->calculateCost(initialSolution);

    Solution *bestSolution = localSearch.run(initialSolution, costEvaluator);
    Solution *currentSolution = new Solution(bestSolution);
    
    bool timeLimitReached = false;
    int iter = 0;

    double totalLsTime = 0.;
    double totalPertubateTime = 0.;

    while (!timeLimitReached)
    {
        auto pertubateTime = std::chrono::system_clock::now();

        currentSolution = localSearch.pertubate(currentSolution);

        std::chrono::duration<double> wctdurationPert = (std::chrono::system_clock::now() - pertubateTime);
        totalPertubateTime += wctdurationPert.count();

        auto lsTime = std::chrono::system_clock::now();

        currentSolution = localSearch.run(currentSolution, costEvaluator);

        if (currentSolution->cost < bestSolution->cost)
        {
            bestSolution = new Solution(currentSolution);
        }

        if (iter % 5000 == 0)
            cout << "IT " << iter << " Cost " << bestSolution->cost << endl;

        iter++;
        std::chrono::duration<double> wctdurationLs = (std::chrono::system_clock::now() - lsTime);
        totalLsTime += wctdurationLs.count();

        std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
        timeLimitReached = wctduration.count() >= instance->timeLimitIteration;
    }

    std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
    std::cout << wctduration.count() << " seconds [Wall Clock]" << std::endl;
    
    cout << "Final Cost: " << bestSolution->cost << endl;
    cout << "IT " << iter << endl;

    cout << "Final Cost: " << bestSolution->cost << endl;

    bestSolution->exportToGeoJson("./data/solutions/" + instance->solutionFileName);
    bestSolution->exportToTxt("./data/solutions/" + instance->solutionFileName, iter, wctduration.count());
}
