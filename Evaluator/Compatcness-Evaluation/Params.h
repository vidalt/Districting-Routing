#ifndef PARAMS_H
#define PARAMS_H

#include "District.h"
#include "Block.h"
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <iterator>
#include <random>
#include <climits>
#include <bits/stdc++.h>
#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

class Params
{
public:
	/* INPUT VARIABLES AND PARAMETERS OF THE GENERATOR */
	string dataName;			// Name of the dataset
	string instanceName;		// Name of the input file representing the original data (without the geojson extension)
	string outputName;			// Path to the output file representing a complete districting dataset (without the geojson extension -- will also be used for temporary LKH data)

	string solutionFile;		// Path to solution file - if true evaluate some solutions instead of generating dataset

	/* INPUT DATA */
	vector<Block> blocks;						 // Information on the blocks
	vector<District> districts;					 // Information on the districts
	
	void readBlocksJSON();
	vector<District> getDistrictsFromSolution();

	Params(int argc, char *argv[]);
};

#endif
