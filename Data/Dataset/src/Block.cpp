#include "Block.h"
using namespace std;

// Distance of a point to the closest point in the block
double Block::distance(const Point & p1)
{
	double minDistance = 1.e30;
	for (int i2 = 0; i2 < verticesPoints.size(); i2++)
	{
		Point & p2 = verticesPoints[i2];
		Point & p3 = verticesPoints[(i2 + 1) % verticesPoints.size()];
		double dist = Point::distance(p1, p2, p3);
		if (dist < minDistance) minDistance = dist;
	}
	return minDistance;
};

// Distance of a point to the farther point in the block
double Block::maxDistance(const Point & p1)
{
	double maxDistance = -1;
	for (int i2 = 0; i2 < verticesPoints.size(); i2++)
	{
		Point & p2 = verticesPoints[i2];
		Point & p3 = verticesPoints[(i2 + 1) % verticesPoints.size()];
		
		double dist = Point::distance(p1, p2, p3);

		if (dist > maxDistance) maxDistance = dist;
	}
	return maxDistance;
};

void Block::calculateTrainTestAverageDepotDistance(const Point& depotPosition)
{
	vector<int> sizesOfDistricts = vector<int> { 3, 6, 12, 20 ,30 };
	
	double counterTrain = 0.;
	double counterTest = 0.;

	double sumOfTrainDistances = 0;
	double sumOfTestDistances = 0;

	for (int sizeOfDistrict : sizesOfDistricts)
	{
		vector<vector<Point>> trainScenarios = this->trainScenarios[sizeOfDistrict];
		
		for (vector<Point> scenario : trainScenarios)
		{
			for (Point customer : scenario)
			{
				sumOfTrainDistances += Point::distance(customer, depotPosition);
				counterTrain++;
			}
		}

		vector<vector<Point>> testScenarios = this->testScenarios[sizeOfDistrict];

		for (vector<Point> scenario : testScenarios)
		{
			for (Point customer : scenario)
			{
				sumOfTestDistances += Point::distance(customer, depotPosition);
				counterTest++;
			}
		}
		
		this->averageCustomerDistanceTrain[sizeOfDistrict] = sumOfTrainDistances/counterTrain;
		this->averageCustomerDistanceTest[sizeOfDistrict]  = sumOfTestDistances/counterTest;

		this->totalCustomersTrain[sizeOfDistrict] = counterTrain;
		this->totalCustomersTest[sizeOfDistrict]  = counterTest;

		this->averageCustomersTrain[sizeOfDistrict] = counterTrain / 500;
		this->averageCustomersTest[sizeOfDistrict]  = counterTest / 500;

	}

	return;
}


void to_json(json& j, const Block& b) 
{
	json jprop = json{
	{"ID", b.id},
	{"CENSUS_ID", b.zone_id},
	{"NAME", b.zone_name},
	{"POPULATION", b.nbInhabitants},
	{"AREA", b.area},
	{"PERIMETER", b.perimeter},
	{"DENSITY", b.density},
	{"LIST_ADJACENT", b.adjacentBlocks},
	{"DIST_EUCL", b.distanceEucl},
	{"POINTS", b.verticesPoints}
	};
	
	json jgeom = json{
	{"type", "Polygon"},
	{"coordinates", {b.verticesLongLat}}
	};

	j = json{
	{"type", "Feature"},
	{"properties", jprop},
	{"geometry", jgeom}
	};
}
