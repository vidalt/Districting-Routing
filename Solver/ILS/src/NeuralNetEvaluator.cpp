#include "CostEvaluator.h"
#include "../lib/http/HTTPRequest.hpp"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <string>


NeuralNetworkEvaluator::NeuralNetworkEvaluator()
{
    this->cache = new lru11::Cache<string, double>();
    this->hasModelBeenCreated = false;
}

double NeuralNetworkEvaluator::calculateCost(Solution *solution, vector<int> districtsToAvaliate)
{
    if (!this->hasModelBeenCreated)
        preloadModel(solution);

    string encodedSolution = getEncodedSolution(solution->districts, districtsToAvaliate);
    vector<string> splitedEncodedSolution = utils::splitString(encodedSolution, ';');

    vector<double> costs = getCosts(encodedSolution, solution);

    if (districtsToAvaliate.empty())
    {
        for (int idDistrict = 0; idDistrict < solution->districts.size(); idDistrict++)
        {
            solution->costOfDistrict[idDistrict] = costs[idDistrict];
            this->cache->insert(splitedEncodedSolution[idDistrict], costs[idDistrict]);
        }
    }
    else
    {
        for (int counter = 0; counter < districtsToAvaliate.size(); counter++)
        {
            solution->costOfDistrict[districtsToAvaliate[counter]] = costs[counter];
            this->cache->insert(splitedEncodedSolution[counter], costs[counter]);
        }
    }

    double totalCost = accumulate(solution->costOfDistrict.begin(), solution->costOfDistrict.end(), 0.);

    solution->cost = totalCost;

    return totalCost;
}

void NeuralNetworkEvaluator::preloadModel(Solution *solution)
{
    try
    {
        string instanceFileName = solution->instance->instanceName + ".geojson";
        int port = solution->instance->port;

        http::Request request("localhost:" + to_string(port) + "/");

        cout << "PRELOAD MODEL " << instanceFileName << endl;
        http::Response response = request.send("POST", instanceFileName, {"Content-Type: application/x-www-form-urlencoded"});

        this->hasModelBeenCreated = true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
}

string NeuralNetworkEvaluator::getEncodedSolution(vector<set<int>> districts, vector<int> districtsToEvaluate)
{
    string districtsAsString = "";
    vector<set<int>> orderedDistricts = vector<set<int>>(districts);

    // for (int i = 0; i < orderedDistricts.size(); i++)
    //     sort(orderedDistricts[i].begin(), orderedDistricts[i].end());

    if (districtsToEvaluate.empty())
    {
        for (int idDistrict = 0; idDistrict < districts.size(); idDistrict++)
        {
            for (int block : orderedDistricts[idDistrict])
                districtsAsString += to_string(block) + ",";

            districtsAsString += ";";
        }
    }
    else
    {
        for (int counter = 0; counter < districtsToEvaluate.size(); counter++)
        {
            for (int block : orderedDistricts[districtsToEvaluate[counter]])
                districtsAsString += to_string(block) + ",";

            districtsAsString += ";";
        }
    }

    return districtsAsString;
}

vector<double> NeuralNetworkEvaluator::getCosts(string encodedSolution, Solution *solution)
{
    vector<string> splitedSolutions = utils::splitString(encodedSolution, ';');
    vector<double> costs = vector<double>(splitedSolutions.size(), INFINITY);

    string encodedSolutionWithoutCached = "";
    vector<int> notCachedPositionInOriginalEncoding = vector<int>();

    for (int i = 0; i < splitedSolutions.size(); i++)
    {
        string district = splitedSolutions[i];

        if (this->cache->contains(district))
            costs[i] = this->cache->get(district);

        else
        {
            encodedSolutionWithoutCached += district + ";";
            notCachedPositionInOriginalEncoding.push_back(i);
        }
    }

    if (encodedSolutionWithoutCached.size() == 0)
        return costs;

    try
    {

        int port = solution->instance->port;
        http::Request request("localhost:" + to_string(port) + "/");
        
        http::Response response = request.send("PUT", encodedSolutionWithoutCached, {"Content-Type: application/x-www-form-urlencoded"});

        string rawResponse = std::string(response.body.begin(), response.body.end());

        vector<string> rawCosts = utils::splitString(rawResponse, ';');

        for (int i = 0; i < rawCosts.size(); i++)
            costs[notCachedPositionInOriginalEncoding[i]] = stod(rawCosts[i]);

        return costs;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
    return vector<double>();
}