#include "Params.h"

extern "C"
{
#include "LKH/LKHmain.h"
}

vector<string> splits(const string &s, const char &c)
{
	string buff{""};
	vector<string> v;

	for (auto n : s)
	{
		if (n != c)
			buff += n;
		else if (n == c && buff != "")
		{
			v.push_back(buff);
			buff = "";
		}
	}
	if (buff != "")
		v.push_back(buff);

	return v;
}

int main(int argc, char *argv[])
{
	/* READ INSTANCE FILE AND INITIALIZE DATA STRUCTURES */
	Params params(argc, argv);

	/* FOR EACH DISTRICT SELECTED IN THE TRAINING SET */
	params.startTime = clock();

	for (int d = 0; d < params.randomDistricts.size(); d++)
	{
		District *myDistrict = &params.randomDistricts[d];
		
		myDistrict->evaluationCosts = vector<double>(params.nbScenarios);
		myDistrict->notDepotCosts = vector<double>(params.nbScenarios);
		myDistrict->onlyDepotCosts = vector<double>(params.nbScenarios);
		
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
				vector<Point> scenario = params.blocks[b].testScenarios[scenarioNumber];
				
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
				string resultStr = runLKH(1, (params.outputName + ".par").c_str()); // run Lin-Kernighan Heuristic

				vector<string> tspCosts = splits(resultStr, ';');

				double cost = stod(tspCosts[0]);
				double depotCost = stod(tspCosts[1]);
				double costWithoutDepot = stod(tspCosts[2]);

				myDistrict->evaluationCosts[scenarioNumber] = cost / 1000.;
				myDistrict->notDepotCosts[scenarioNumber] = costWithoutDepot/ 1000.;
				myDistrict->onlyDepotCosts[scenarioNumber] = depotCost/ 1000.;
			}

			myDistrict->averageCost += myDistrict->evaluationCosts[scenarioNumber];
			myDistrict->averageNoDepotCost += myDistrict->notDepotCosts[scenarioNumber];
			myDistrict->averageOnlyDepotCost += myDistrict->onlyDepotCosts[scenarioNumber];

			myDistrict->evaluationNbCustomers[scenarioNumber] = evaluationPoints.size();

			if (d % 1 == 0)
				cout << "District: " << d << " | NBlocks: " << myDistrict->blocks.size() << " | Scenario: " << scenarioNumber << " | NbPoints: " << myDistrict->evaluationNbCustomers[scenarioNumber] << " | Cost: " << myDistrict->evaluationCosts[scenarioNumber] << endl;
			
		}

		myDistrict->averageCost /= (double)params.nbScenarios;
		myDistrict->averageNoDepotCost /= (double)params.nbScenarios;
		myDistrict->averageOnlyDepotCost /= (double)params.nbScenarios;
	}

	params.endTime = clock();


	/* CLEAN AND EXPORT DISTRICTS TO FILE */
	remove((params.outputName + ".tsp").c_str());
	remove((params.outputName + ".par").c_str());
	
	double averageCostSum = 0.;
	double averageDepotSum = 0.;
	double averageIntraDistrictSum = 0.;

	for (District district : params.randomDistricts)
	{
		cout << district.averageCost << endl;
		averageCostSum += district.averageCost;
		averageDepotSum += district.averageOnlyDepotCost;
		averageIntraDistrictSum += district.averageNoDepotCost;
	}

	cout << "ESTIMATED COST FOR SOLUTION " << averageCostSum << endl;
	
	ofstream outputFile;
	
	outputFile.open(params.solutionFile + ".eval");

	if (outputFile.is_open())
	{
		outputFile << "ESTIMATED COST FOR SOLUTION " << averageCostSum << endl;
		outputFile << "DEPOT TRIP COST " << averageDepotSum << endl;
		outputFile << "INTRA DISTRICT COST " << averageIntraDistrictSum << endl;

		outputFile.close();
	}

	return 0;
}
