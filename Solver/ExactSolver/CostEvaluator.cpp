#include "CostEvaluator.h"
#include <iostream>
#include <string>
#include <vector>
#include "../ILS/lib/http/HTTPRequest.hpp"
#include "../ILS/lib/nlohmann/json.hpp"
using json = nlohmann::json;

using namespace std;

CostEvaluator::CostEvaluator()
{
}

vector<string> split(const string &s, const char &c)
{
    string buff{""};
    vector<string> v;

    for (auto n : s)
    {
        if (n != c)
            buff += n;
        else if (n == c && buff != "")
        {
            v.push_back(buff);
            buff = "";
        }
    }
    if (buff != "")
        v.push_back(buff);

    return v;
}

void CostEvaluator::run(Instance *instance, map<string, double> *districtCosts)
{
    if (instance->predictorMethod != "full")
    {
        this->port = instance->port;
        this->preloadModel(instance);
        this->getCostsFromNeuralNet(districtCosts);
    }
    else
    {
        string dataFileName = "../../Data/TSP-Scenarios/districts-with-costs/" + instance->instanceName + "_tsp.all_test.json";
        ifstream instanceFile = ifstream();
        instanceFile.open(dataFileName);

        if (instanceFile.is_open())
        {
            json content;
            instanceFile >> content;
            instanceFile.close();
            
            for (auto& district : content.at("districts"))
            {
                string districtAsString = "";
                
                vector<int> blockIds = district.at("list-blocks");

                if (blockIds.size() > 1)
                {
                    for (int blockId: blockIds)
                        districtAsString += std::to_string(blockId) + ",";
                
                    districtCosts->at(districtAsString) = district.at("average-cost");
                }

            }

            for (pair<string, double> districtCost : *districtCosts)
            {
                if (districtCost.second == -1)
                {
                    cout << "MISSING VALUE " << districtCost.first << endl;
                    throw invalid_argument("MISSING SOME DISTRICT: " + districtCost.first);
                } 

            }
        }
        else
        {
            string message = "Cannot open instance file: " + dataFileName;
            cout << message << endl;
            throw invalid_argument(message);
        }
    }

}

void CostEvaluator::preloadModel(Instance *instance)
{
    try
    {
        string instanceFileName = instance->instanceName + ".geojson";
        http::Request request("localhost:" + to_string(this->port) + "/");

        http::Response response = request.send("POST", instanceFileName, {"Content-Type: application/x-www-form-urlencoded"});
    }
    catch (const std::exception &e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
}

string CostEvaluator::encodeDistricts(vector<string> districts)
{
    string encodedSolution = "";

    for (string district : districts)
        encodedSolution += district + ";";

    return encodedSolution;
}

void CostEvaluator::getCostsFromNeuralNet(map<string, double> *districtCosts)
{
    int BATCH_SIZE = 1500;
    int numberOfBatches = ceil(districtCosts->size() / BATCH_SIZE);

    vector<vector<string>> batchedDistricts = vector<vector<string>>();

    int iter = 0;
    vector<string> currentBatch = vector<string>();

    for (pair<string, double> districtCost : *districtCosts)
    {
        if (iter % BATCH_SIZE == 0)
        {
            if (!currentBatch.empty())
                batchedDistricts.push_back(currentBatch);

            currentBatch = vector<string>();
        }
        iter++;
        currentBatch.push_back(districtCost.first);
    }

    if (iter - 1 % BATCH_SIZE != 0)
        batchedDistricts.push_back(currentBatch);

    for (vector<string> batch : batchedDistricts)
    {
        string encodedDistricts = this->encodeDistricts(batch);

        try
        {
            http::Request request("localhost:" + to_string(this->port) + "/");
            http::Response response = request.send("PUT", encodedDistricts, {"Content-Type: application/x-www-form-urlencoded"});

            string rawResponse = std::string(response.body.begin(), response.body.end());

            vector<string> rawCosts = split(rawResponse, ';');

            vector<string> encodedDistrictsSep = split(encodedDistricts, ';');

            for (int i = 0; i < encodedDistrictsSep.size(); i++)
                districtCosts->at(encodedDistrictsSep[i]) = stod(rawCosts[i]);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Request failed, error: " << e.what() << '\n';
        }
    }
}
