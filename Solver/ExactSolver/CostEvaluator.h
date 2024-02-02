#ifndef COSTEVALUATOR_H
#define COSTEVALUATOR_H

#include <bits/stdc++.h>
#include <string>
#include "../ILS/src/Instance.h"

using namespace std;

class CostEvaluator
{

public:
    CostEvaluator();
    void run(Instance *instance, map<string, double> *districtCosts);

private:
    int port;
    void preloadModel(Instance *instance);
    void runEvaluation(map<string, double> districtCosts);
    string encodeDistricts(vector<string> districts);
    void getCostsFromNeuralNet(map<string, double> *districtCosts);
};

#endif