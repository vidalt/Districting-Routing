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
#include "WGS84toCartesian.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

class Params
{
public:
	/* INPUT VARIABLES AND PARAMETERS OF THE GENERATOR */
	string dataName;			// Name of the dataset
	string instanceName;		// Name of the input file representing the original data (without the geojson extension)
	string scenariosFilePath;	// Path to scenarios file   
	string outputName;			// Path to the output file representing a complete districting dataset (without the geojson extension -- will also be used for temporary LKH data)

	int minSizeDistricts;		// Minimum number of blocks in each district allowed in the strategic districting problem
	int targetSizeDistricts;	// Target number of blocks in each district
	int maxSizeDistricts;		// Maximum number of blocks in each district allowed in the strategic districting problem
	int targetNbDistricts;		// Target number of districts
	double probaCustomerDemand; // Fraction of the inhabitants requiring deliveries
	int nbScenarios;			// Number of TSP scenarios to evaluate routing costs
	int sizeTrainingSet;		// Number of districts we wist to sample to construct the training data (the actual number of samples may be smaller if we don't find that many districts)
	string depotPosition;		// Depot position (C, NW, NE, SW, SE)
	int instanceSize;			// Parameter for problem refuction. 0 means no reduction is applied, otherwise data set is restricted to the reducedProblem closest blocks to the reference point
	int seed;					// Random seed
	string solutionFile;		// Path to solution file - if true evaluate some solutions instead of generating dataset
	bool solveOnlyTest=false;
	bool solveEveryDistrict=false;

	/* INPUT DATA */
	mt19937 generator;							 // Random number generator
	Point depotPoint;							 // Point representing the depot in XY coordinates
	Point referencePoint = {0, 0};				 // Point representing the reference position in XY coordinates (always 0,0 per definition)
	array<double, 2> depotLongLat;				 // Depot longitude and latitude
	set<array<double, 2>> allPointsLongLat;		 // All points in the considered instance
	vector<Block> blocks;						 // Information on the blocks
	vector<District> randomDistricts;			 // Vector representing the districts which have been selected for the training set
	double minX = 1.e30;						 // Minimum X coordinate of the vertices of the entire dataset
	double maxX = -1.e30;						 // Maximum X coordinate of the vertices of the entire dataset
	double minY = 1.e30;						 // Minimum Y coordinate of the vertices of the entire dataset
	double maxY = -1.e30;						 // Maximum Y coordinate of the vertices of the entire dataset
	int totalInhabitants = 0;					 // Total number of inhabitants in the blocks
	int minInhabitants = INT_MAX;				 // Minimum number of inhabitants in the blocks
	int maxInhabitants = 0;						 // Maximum number of inhabitants in the blocks
	double totalArea = 0;						 // Total area of the blocks
	double minArea = 1.e30;						 // Minimal area of a block
	double maxArea = 0.;						 // Maximum area of a block
	vector<long int> totalCustomersScenarios;	 // totalCustomersScenarios[nb] will count the total number of customers involved in scenarios for districts with nb blocks
	vector<double> averageCustomersScenarios;	 // totalCustomersScenarios[nb] will count the average number of customers involved in scenarios for districts with nb blocks
	vector<int> totalDistrictsNbBlocks;			 // totalDistrictsNbBlocks[nb] will count the total number of districts with nbBlocks in the training set

	/* TIME MEASUREMENT */
	clock_t startTime; // Time when the generation of the training examples started
	clock_t endTime;   // Time when the generation of the training examples started

	/* FUNCTIONS TO READ FILE AND SET UP THE DATA STRUCTURES */
	bool isBlockToRemove(string zone_id); // Blocks which have been flagged to be ignored in the datasets (to avoid connectivity issues)
	
	void readBlocksJSON();
	void readScenarionJSON();
	void exportDatasetJsonFile();

	void completeRandomDistrict(set<set<int>> &sampledDistricts, set<int> included, set<int> reachable, int nbRemaining);
	void generateTSPinstance(vector<Point> &points);
	
	Params(int argc, char *argv[]);
};

#endif
