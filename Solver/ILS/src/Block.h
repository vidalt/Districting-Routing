#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <set>
#include <string>


using namespace std;

class Block
{
public:

	int id;												// ID of the block
	
	string zoneName;									// Name of the zone
	int nbInhabitants;									// Number of inhabitants
	int estimatedCostumers;								// Estimated value of costumers = nbInhabitants * probCustomerDemand
	vector <int> adjacentBlocks;						// Adjacent blocks
	
	double area;										// Area of the block
	double density;										// Density of the block
	double depotDistance;								// Distance from Block to Depot
};

#endif