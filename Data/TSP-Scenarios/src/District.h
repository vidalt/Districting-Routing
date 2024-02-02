#ifndef DISTRICT_H
#define DISTRICT_H

#include <vector>
#include <set>
#include "Point.h"
using namespace std;

class District
{

public:
	
	set < int > blocks;											// Blocks composing the district
	vector < double > evaluationCosts;							// TSP cost evaluations
	vector < int > evaluationNbCustomers;						// Number of customers sampled in each TSP cost evaluation
	double averageCost;											// Average TSP cost over all scenarios

	int districtId;
	bool isTestSet;

	// Comparison operator
	bool operator <(const District& d)
	{
		if (blocks.size() != d.blocks.size()) return (blocks.size() < d.blocks.size());
		else return (blocks < d.blocks);
	}
	
	// Constructor
	District(set < int > & blocks, bool isTestSet=false, int districtId=-1) : blocks(blocks), isTestSet(isTestSet), districtId(districtId)
	{};			
};

// JSON output
void to_json(json& j, const District& p);

#endif
