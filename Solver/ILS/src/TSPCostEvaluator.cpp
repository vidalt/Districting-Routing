#include "CostEvaluator.h"
#include <fstream> 

#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

extern "C"
{
#include "../lib/LKH/LKHmain.h"
}

#endif


TSPCostEvaluator::TSPCostEvaluator()
{}

double TSPCostEvaluator::calculateCost(Solution *solution, vector<int> districtsToAvaliate)
{
    if (districtsToAvaliate.empty()){
        for (int idDistrict = 0; idDistrict < solution->districts.size(); idDistrict++)
        {
            solution->costOfDistrict[idDistrict] = -1;
            districtsToAvaliate.push_back(idDistrict);
        }
    }

    for (int districtId : districtsToAvaliate)
    {
        set<int> district = solution->districts[districtId];

        vector<Point> evaluationPoints;
        evaluationPoints.push_back(solution->instance->depotPoint);

        for (int blockId : district)
        {
            vector<Point> scenario = solution->instance->blocks[blockId].trainScenarios[0]; // Only evaluate the first train scenario

            for (Point point : scenario)
            {
                evaluationPoints.push_back(point);
            }
        }

        generateTSPinstance(evaluationPoints, solution->instance->depotPoint, solution->instance->instanceName);
        string runFileName = solution->instance->instanceName + ".par";
        solution->costOfDistrict[districtId] = (double)runLKH(1, runFileName.c_str()) / 1000.;
    }

    double totalCost = accumulate(solution->costOfDistrict.begin(), solution->costOfDistrict.end(), 0.0);
    solution->cost = totalCost;

    return totalCost;
}

void TSPCostEvaluator::generateTSPinstance(vector<Point> &points, Point depotPoint, string fileName)
{
	ofstream myfile;
	myfile.open(fileName + ".tsp");
	if (myfile.is_open())
	{
		myfile << "NAME : TSP" << endl;
		myfile << "COMMENT : Districting Scenario Evaluation" << endl;
		myfile << "TYPE : TSP" << endl;
		myfile << "DIMENSION : " << points.size() << endl;
		myfile << "EDGE_WEIGHT_TYPE: EUC_2D" << endl;
		myfile << "NODE_COORD_SECTION" << endl;

		for (int i = 0; i < points.size(); i++)
			myfile << i + 1 << " " << (int)(1000. * (points[i].x - depotPoint.x)) << " " << (int)(1000. * (points[i].y - depotPoint.y)) << endl;
		
        myfile.close();
	}
	else
		throw std::invalid_argument("Impossible to create TSP file");

	myfile.open(fileName + ".par");
	if (myfile.is_open())
	{
		myfile << "PROBLEM_FILE = " << fileName << ".tsp" << endl;
		myfile << "MOVE_TYPE = 5" << endl;
		myfile << "PATCHING_C = 3" << endl;
		myfile << "PATCHING_A = 2" << endl;
		myfile << "RUNS = 1" << endl;
		myfile << "TRACE_LEVEL = 0" << endl;
		myfile << "GAIN23 = NO" << endl;
		myfile << "MAX_TRIALS = 5" << endl;		
		myfile << "MAX_SWAPS = 0" << endl;
		myfile << "ASCENT_CANDIDATES = 5" << endl;
		myfile.close();
	}
	else
		throw std::invalid_argument("Impossible to create PAR file");
}