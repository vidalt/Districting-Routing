#ifndef BLOCK_H
#define BLOCK_H

#include "Point.h"
#include <vector>
#include <set>
#include <string>
using namespace std;

class Block
{
public:
	int id;												// ID of the block
	double area;										// Area of the block
	double recArea;										// Area of the enclosing rectangle
	vector <Point> verticesPoints;						// Copy of the points that delimit the block
};

// JSON output
void to_json(json& j, const Block& b);

#endif
