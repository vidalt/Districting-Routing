#ifndef INSTANCE_H
#define INSTANCE_H

#include "Block.h"
#include "functional"
#include "random"
#include <map>

using namespace std;

class Instance
{
public:
    Instance(string filePath); // Constructor that reads geojson file

    void setParameters(double probabilitySkipMovePertubation,
					   int seed, string solutionFileName,
					   double timeLimitIteration,
					   int port, string predictorMethod);

    string instancePath; // Path to instance file
    string instanceName; // Name of instance
    string solutionFileName;
    string cityName;      // Name of the city
    vector<Block> blocks; // Basic Units
    string predictorMethod;   //Identifier of prediction method

    int minSizeDistricts;    // Lower bound for size of District
    int targetSizeDistricts; // Target size of a District
    int maxSizeDistricts;    // Upper bound for size of District

    int numDistricts;          // Number of districts
    float probaCustomerDemand; // Probability of a inhabitant to have a demand
    int seed;
    double timeLimitIteration; // Time limit for whole execution

    double probabilitySkipMovePertubation; // Probability of not trying some move while pertubating
    default_random_engine generator;       // Random number generator
    function<double()> randomNumber;       // Return random double in [0,1)

    int port;                               // expects a cost evaluator in port (default 8000)

    bool isDistrictContigous(set<int> *basicUnitOfADistrict); // Return true if configuration represents a contigous area
};

#endif