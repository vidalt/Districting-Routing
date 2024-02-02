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
	int nbInhabitants;									// Number of inhabitants
	
	vector <Point> verticesPoints;						// Copy of the points that delimit the block
	vector <std::array<double, 2>> verticesLongLat;		// Lat/Long of the points that delimit the block
	
	set <int> adjacentBlocks;							// Adjacent blocks
	vector <int> distanceGraph;							// Distance to the other blocks: Number of edges in the adjacency graph
	
	double area;										// Area of the block
	double recArea;										// Area of the enclosing rectangle
	double perimeter = 0.0;								// Perimeter of the block
	double density;										// Density of the block
	double distDepot;									// Distance from the depot
	double distReferencePoint;							// Distance from the reference point

	vector<vector<Point>> trainScenarios;				// Scenarios for training
	
	// overloaded comparison operator (to order by increasing distance from the reference point)
	bool operator <(const Block& b) {return (distReferencePoint < b.distReferencePoint);}

	// Distance of a point to the closest point in the block
	double distance(const Point & p1);
};

// JSON output
void to_json(json& j, const Block& b);

#endif
