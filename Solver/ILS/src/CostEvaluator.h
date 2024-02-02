#ifndef COST_EVALUATOR_H
#define COST_EVALUATOR_H

#include "Solution.h"
#include "../lib/Cache/LRUCache11.hpp"

class CostEvaluator
{
public:
    virtual double calculateCost(Solution *solution, vector<int> districtsToAvaliate = vector<int>()) = 0; // Generic interface to calculate cost
};

class NeuralNetworkEvaluator : public CostEvaluator
{
public:
    double calculateCost(Solution *solution, vector<int> districtsToAvaliate); // Implementation of cost evaluator interface, it only evaluates districts with id in districtsToAvaliate or evaluate all if list is empty
    NeuralNetworkEvaluator();

private:
    lru11::Cache<string, double> *cache;            // LRU Cache to avoid calling server for every call
    bool hasModelBeenCreated;                       // True only if model has been pre-load into server
    void preloadModel(Solution *solution);          // It is useful to have some data in memory on server, so this should setup for the next calls
    vector<double> getCosts(string encodedSolution, Solution *solution);  // Function that actually calls server
    string getEncodedSolution(vector<set<int>> districts, vector<int> districtsToEvaluate); // Format solution to format expected by server
};

#endif