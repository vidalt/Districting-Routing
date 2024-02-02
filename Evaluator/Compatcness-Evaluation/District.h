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
	double  compactness;							            // Value for compactness
	
	// Constructor
	District(set < int > & blocks) : blocks(blocks)
	{};			
};

// JSON output
void to_json(json& j, const District& p);

#endif
