#include "Params.h"

extern "C"
{
#include "LKH/LKHmain.h"
}

using namespace std;
int main(int argc, char *argv[])
{
	/* READ INSTANCE FILE AND INITIALIZE DATA STRUCTURES */
	Params params(argc, argv);
	

	/* FOR EACH DISTRICT SELECTED IN THE TRAINING SET */
	params.startTime = clock();

	cout << "NUMBER OF DISTRICT " << params.randomDistricts.size() << endl;

	for (int d = 0; d < params.randomDistricts.size(); d++)
	{
		District *myDistrict = &params.randomDistricts[d];
		myDistrict->evaluationCosts = vector<double>(params.nbScenarios);
		myDistrict->evaluationNbCustomers = vector<int>(params.nbScenarios);
		myDistrict->averageCost = 0.0;

		/* FOR EACH SCENARIO */
		for (int scenarioNumber= 0; scenarioNumber < params.nbScenarios; scenarioNumber++)
		{
			/* INCLUDE THE DEPOT, located in the middle of the considered region */
			vector<Point> evaluationPoints;
			evaluationPoints.push_back(params.depotPoint);

			/* SELECT RANDOM POINTS IN THE BLOCKS OF THIS DISTRICT */
			for (int b : myDistrict->blocks)
			{
				vector<Point> scenario = params.blocks[b].trainScenarios[scenarioNumber];
				
				for (Point randomPoint : scenario) 
				{
					evaluationPoints.push_back(randomPoint);
				}
			}

			/* EVALUATE TSP COSTS */
			if (evaluationPoints.size() <= 1)
				myDistrict->evaluationCosts[scenarioNumber] = 0.;

			else if (evaluationPoints.size() == 2)
				myDistrict->evaluationCosts[scenarioNumber] = 2.0 * Point::distance(evaluationPoints[0], evaluationPoints[1]);
			else
			{
				params.generateTSPinstance(evaluationPoints);
				myDistrict->evaluationCosts[scenarioNumber] = (double)runLKH(1, (params.outputName + ".par").c_str()) / 1000.; // run Lin-Kernighan Heuristic
			}

			myDistrict->averageCost += myDistrict->evaluationCosts[scenarioNumber];
			myDistrict->evaluationNbCustomers[scenarioNumber] = evaluationPoints.size();

			if (d % 50 == 0 && scenarioNumber % 250 == 0)
				cout << "District: " << d << " | NBlocks: " << myDistrict->blocks.size() << " | Scenario: " << scenarioNumber << " | NbPoints: " << myDistrict->evaluationNbCustomers[scenarioNumber] << " | Cost: " << myDistrict->evaluationCosts[scenarioNumber] << endl;
			
			params.totalCustomersScenarios[myDistrict->blocks.size()] += myDistrict->evaluationNbCustomers[scenarioNumber];
		}
		
		params.totalDistrictsNbBlocks[myDistrict->blocks.size()]++;
		myDistrict->averageCost /= (double)params.nbScenarios;
	}
	params.endTime = clock();

	/* COLLECT SOME LAST STATISTICS */
	for (int k = 1; k < params.averageCustomersScenarios.size(); k++)
		params.averageCustomersScenarios[k] = (double)params.totalCustomersScenarios[k] / ((double)params.totalDistrictsNbBlocks[k] * (double)params.nbScenarios);

	/* CLEAN AND EXPORT DISTRICTS TO FILE */
	remove((params.outputName + ".tsp").c_str());
	remove((params.outputName + ".par").c_str());
	params.exportDatasetJsonFile();

	return 0;
}
