#include "Solver.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

Solver::Solver(Instance *instance)
{
    this->instance = instance;
    this->districtCosts = map<string, double>();
    this->grbSolver = GurobiSolver();
    this->costEvaluator = CostEvaluator();
}

void Solver::run()
{
    auto wcts = std::chrono::system_clock::now();
    for (Block block : instance->blocks)
    {
        set<int> initialSet = set<int>();
        initialSet.insert(block.id);

        this->getPossibleDistricst(block.id, block.id, initialSet);
    }

    std::chrono::duration<double> wctEnumerating = (std::chrono::system_clock::now() - wcts);

    std::cout << this->districtCosts.size() << std::endl;
    costEvaluator.run(instance, &this->districtCosts);
    std::chrono::duration<double> wctEvaluatingCosts = (std::chrono::system_clock::now() - wcts);

    Solution bestFeasibleSolution = this->grbSolver.findBestSolution(this->instance, this->districtCosts);

    std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
    std::cout << wctduration.count() << " seconds [Wall Clock]" << std::endl;

    bestFeasibleSolution.exportToTxt(instance->instanceName, this->districtCosts.size(), wctduration.count());

    ofstream outputFile;
    string sufix    = this->instance->predictorMethod;    
    string fileName = this->instance->instanceName + "." + sufix + ".time.eval";

    outputFile.open(fileName);
    
    if (outputFile.is_open())
    {
        outputFile << "ENUM TIME " << wctEnumerating.count() << endl;
        outputFile << "COST EVAL TIME " << (wctEvaluatingCosts.count() - wctEnumerating.count()) << endl;
        outputFile << "MIP TIME " << (wctduration.count() - wctEvaluatingCosts.count()) << endl;
        outputFile << "TOTAL TIME " << wctduration.count() << endl;
    }

    outputFile.close();

}

void Solver::getPossibleDistricst(int currentBlockId,
                                  int initialBlockId,
                                  set<int> currentBlocks,
                                  set<int> currentAdjacencyList,
                                  set<int> saturatedNodes,
                                  int currentDepth)
{
    if (currentDepth > instance->maxSizeDistricts)
        return;

    if (currentDepth >= instance->minSizeDistricts)
    {
        string encodedDistrict = "";

        for (int blockId : currentBlocks)
            encodedDistrict += to_string(blockId) + ",";

        this->districtCosts.insert(pair<string, double>(encodedDistrict, -1));
    }

    Block currentBlock = instance->blocks[currentBlockId];

    for (int idNeighbor : currentBlock.adjacentBlocks)
    {
        if (idNeighbor < initialBlockId) 
            currentAdjacencyList.insert(idNeighbor);
    }

    for (int neighbor : currentAdjacencyList)
    {
        bool isBlockSaturated = saturatedNodes.count(neighbor) != 0;
        bool isBlockAlreadyInSolution = currentBlocks.count(neighbor) != 0;

        if (!isBlockAlreadyInSolution && !isBlockSaturated)
        {
            set<int> copyOfBlocks = set<int>(currentBlocks);
            copyOfBlocks.insert(neighbor);

            this->getPossibleDistricst(neighbor, initialBlockId, copyOfBlocks, 
                                       set<int>(currentAdjacencyList), set<int>(saturatedNodes),
                                       currentDepth + 1);
        }

        saturatedNodes.insert(neighbor);
    }
}
