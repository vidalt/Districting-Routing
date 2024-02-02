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
	string zone_id;										// ID of the zone (from JSON input)
	string zone_name;									// Name of the zone
	int nbInhabitants ;									// Number of inhabitants
	vector <Point> verticesPoints;						// Copy of the points that delimit the block
	vector <std::array<double, 2>> verticesLongLat;		// Lat/Long of the points that delimit the block
	double minX = 1.e30;								// Minimum X coordinate of the vertices of the block
	double maxX = -1.e30;								// Maximum X coordinate of the vertices of the block
	double minY = 1.e30;								// Minimum Y coordinate of the vertices of the block
	double maxY = -1.e30;								// Maximum Y coordinate of the vertices of the block
	set <int> adjacentBlocks;							// Adjacent blocks
	vector <double> distanceEucl;						// Distance to the other blocks: Euclidian
	vector <int> distanceGraph;							// Distance to the other blocks: Number of edges in the adjacency graph
	double area;										// Area of the block
	double recArea;										// Area of the enclosing rectangle
	double perimeter = 0.0;								// Perimeter of the block
	double density;										// Density of the block
	double distDepot;									// Distance from the depot
	double distReferencePoint;							// Distance from the reference point

	// overloaded comparison operator (to order by increasing distance from the reference point)
	bool operator <(const Block& b) {return (distReferencePoint < b.distReferencePoint);}

	// Distance of a point to the closest point in the block
	double distance(const Point & p1);
	vector<vector<Point>> testScenarios;				// Scenarios for training

};

// JSON output
void to_json(json& j, const Block& b);

#endif
