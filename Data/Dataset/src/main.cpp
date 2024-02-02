#include "Params.h"

int main(int argc, char *argv[])
{
	/* READ INSTANCE FILE AND INITIALIZE DATA STRUCTURES */
	Params params(argc, argv);

	/* FOR EACH DISTRICT SELECTED IN THE TRAINING SET */
	params.startTime = clock();

	for(pair<int,float> probByTargetSizeDistrict : params.probaCustomerDemandPerTargetSizeDistrict)
	{
		float probaCustomerDemand = probByTargetSizeDistrict.second;
		int targetSizeOfDistrict = probByTargetSizeDistrict.first;

		for (int b = 0; b < params.blocks.size(); b++)
		{			
			vector<vector<Point>> testScenarios  = vector<vector<Point>>();
			vector<vector<Point>> trainScenarios = vector<vector<Point>>();
		
			for (int s = 0; s < 2*params.sizeTrainingSet; s++)
			{
				// Pick the number of points in the block following a binomial distribution based on the number of inhabitants
				vector<Point> evaluationPoints;
				binomial_distribution<> distB(params.blocks[b].nbInhabitants, probaCustomerDemand);
				int nbCustomersSampledBlock = distB(params.generator);

				for (int i = 0; i < nbCustomersSampledBlock; i++)
				{
					uniform_real_distribution<> distX(params.blocks[b].minX, params.blocks[b].maxX);
					uniform_real_distribution<> distY(params.blocks[b].minY, params.blocks[b].maxY);

					Point randomPoint = {distX(params.generator), distY(params.generator)};

					while (!isInside(params.blocks[b].verticesPoints, randomPoint))
						randomPoint = {distX(params.generator), distY(params.generator)};
					
					evaluationPoints.push_back(randomPoint);
				}

				if(s%2 ==0)  testScenarios.push_back(evaluationPoints);
				else 		 trainScenarios.push_back(evaluationPoints);
			}

			params.blocks[b].testScenarios[targetSizeOfDistrict]  = testScenarios;
			params.blocks[b].trainScenarios[targetSizeOfDistrict] = trainScenarios;
		}
	}

	params.endTime = clock();

	/* EXPORT DISTRICTS TO FILE */
	params.exportJSONFile();
	params.exportBlockScenarios();
	
	return 0;
}
