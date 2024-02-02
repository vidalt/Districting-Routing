#include "Params.h"

Params::Params(int argc, char *argv[])
{
	dataName = string(argv[1]).c_str();
	cout << "CITY NAME: " << dataName << endl;

	targetSizeDistricts = stoi(argv[2]);

	minSizeDistricts = floor(0.8 * targetSizeDistricts);
	maxSizeDistricts = ceil(1.2 * targetSizeDistricts);

	cout << "[MIN,TARGET,MAX] SIZE DISTRICTS: "
		 << "[" << minSizeDistricts << "," << targetSizeDistricts << "," << maxSizeDistricts << "]" << endl;

	sizeTrainingSet = stoi(argv[3]);
	cout << "SIZE TRAINING SET: " << sizeTrainingSet << endl;

	depotPosition = string(argv[4]).c_str();
	cout << "DEPOT POSITION: " << depotPosition << endl;

	instanceSize = stoi(argv[5]);
	cout << "SIZE OF PROBLEM: " << instanceSize << endl;

	seed = stoi(argv[6]);
	generator.seed(seed);
	cout << "SEED: " << seed << endl;

	int solve_test_scenarios = stoi(argv[7]);
	cout << "SOLVE ONLY TEST: " << solve_test_scenarios << endl;

	nbScenarios = 500;
	this->solveOnlyTest = solve_test_scenarios == 1;


	this->solveEveryDistrict = argc > 7? stoi(argv[8]) != 0: false;
	cout << "SOLVE EVERY SCENARIO: " << solveEveryDistrict << endl;

	// Define data paths and read datafile in JSON format
	instanceName = dataName;
	
	outputName = dataName + "_" + depotPosition + "_"
				+ to_string(instanceSize) + "_" + to_string(targetSizeDistricts);

	readBlocksJSON();
	readScenarionJSON();

	totalCustomersScenarios = vector<long int>(1 + maxSizeDistricts, 0);
	averageCustomersScenarios = vector<double>(1 + maxSizeDistricts, 0.);
	totalDistrictsNbBlocks = vector<int>(1 + maxSizeDistricts, 0);

	if (!this->solveEveryDistrict)
	{
		string districtsFileName = "./districts-with-costs/" + dataName + "_" + depotPosition + "_"
				                   + to_string(instanceSize) + "_" + to_string(targetSizeDistricts)
				                   + "_tsp.train_and_test.json";
		ifstream myfile;
		myfile.open(districtsFileName);

		if (myfile.is_open())
		{	
			json fileContent;
			myfile >> fileContent;
			myfile.close();

			int districtId = 0;

			for (auto& currentDistrict : fileContent.at("districts"))
			{
				
				set<int> blocks = currentDistrict.at("list-blocks");
				bool isTestset = currentDistrict.contains("TEST_SET");
				
				if (isTestset == this->solveOnlyTest) randomDistricts.push_back(District(blocks, isTestset, districtId));
				districtId++;
			}

		}
	}
	else
	{
		cout << "SOLVE EVERY FEASIBLE DISTRICT" << endl;	
		
		uniform_int_distribution<> distI(0, blocks.size() - 1);
		uniform_int_distribution<> distB(1, maxSizeDistricts);

		// Sample sizeTrainingSet distinct random districts
		set<set<int>> sampledDistricts;
		int reallyLargeNumber = 1000000000;

		// for (int nbIt = 0; sampledDistricts.size() < sizeTrainingSet; nbIt++)
		for (int nbIt = 0; sampledDistricts.size() < reallyLargeNumber; nbIt++)
		{
			// Picking a starting block to construct a random district
			int myblock = distI(generator);
			// Picking the desired district size (uniformly between 1 and 1.2 * target district size)
			int nbRemaining = distB(generator) - 1;
			// Building a random district
			completeRandomDistrict(sampledDistricts, {myblock}, blocks[myblock].adjacentBlocks, nbRemaining);
			if (nbIt == sizeTrainingSet * 100)
			{
				cout << "COULD NOT SUCCESSFULLY SAMPLE THE REQUIRED NUMBER OF DISTRICTS" << endl;
				cout << "SAMPLED: " << sampledDistricts.size() << endl;
				break;
			}
		}
	}
}

void Params::readBlocksJSON()
{	
	string fileName = "../Dataset/" + instanceName + ".geojson";
	ifstream myfile;
	myfile.open(fileName);

	int numberTries = 0;

	if (myfile.is_open())
	{	
		json fileContent;
		
		myfile >> fileContent;
		myfile.close();

		for (auto& currentBlock : fileContent.at("features"))
		{
			Block block;

			block.id = currentBlock.at("properties").at("ID");
			
			if(block.id > instanceSize-1) continue;

			block.verticesPoints = vector<Point>();
			
			for (auto& verticePoint : currentBlock.at("geometry").at("coordinates")[0])
				block.verticesPoints.push_back({verticePoint[0], verticePoint[1]});

			vector<int> adjacentBlocks = currentBlock.at("properties").at("LIST_ADJACENT");

			block.adjacentBlocks = set<int>();

			for(int adjBlock: adjacentBlocks)
			{
				if(adjBlock > instanceSize-1) continue;
				block.adjacentBlocks.insert(adjBlock);
			}
			
			this->blocks.push_back(block);
		}
	}

	else
		throw std::invalid_argument("Impossible to open output file: " + fileName);

}

void Params::readScenarionJSON()
{
	string typeOfScenario = this->solveOnlyTest? "test" : "train"; 

	string scenarioFileName = dataName + "_" + depotPosition + "_" 
	                        + to_string(instanceSize) + "_" 
							+ to_string(targetSizeDistricts) + "." + typeOfScenario + ".json";
	ifstream myfile;
	myfile.open("../Dataset/scenarios/" + scenarioFileName);

	if (myfile.is_open())
	{	
		json fileContent;
		
		myfile >> fileContent;
		myfile.close();
		
		for (auto &block : fileContent.at("blocks"))
		{
			int blockId = block.at("ID");
			
			blocks[blockId].distDepot = block.at("DEPOT_DIST");
			blocks[blockId].trainScenarios = vector<vector<Point>>();

			for (auto &scenario : block.at("SCENARIOS"))
			{
				vector<Point> scenarioPoints = vector<Point>();
				for (auto &point : scenario)
				{
					Point randomPoint = { point[0],  point[1]};
					scenarioPoints.push_back(randomPoint);
				}

				blocks[blockId].trainScenarios.push_back(scenarioPoints);
			}
			
		}			
		
		vector<double> depot = fileContent.at("metadata").at("DEPOT_XY");
		depotPoint = {depot[0], depot[1]};
	}
	else
		throw std::invalid_argument("Impossible to open output file: " + scenarioFileName);
}

void Params::completeRandomDistrict(set<set<int>> &sampledDistricts, set<int> included, set<int> reachable, int nbRemaining)
{
	if (nbRemaining == 0)
	{
		auto res = sampledDistricts.insert(included);
		if (res.second == true)
			randomDistricts.push_back(District(included));
	}
	else
	{
		if (reachable.empty())
			throw string("Issue: impossible to crease a district of the specified size");

		// Convert reachable set into a vector and pick a random element
		vector<int> reachableVect(reachable.begin(), reachable.end());
		uniform_int_distribution<> distI(0, reachableVect.size() - 1);
		int myRandomDistrict = reachableVect[distI(generator)];

		set<int> included2 = included;
		set<int> reachable2 = reachable;
		included2.insert(myRandomDistrict);
		reachable2.erase(myRandomDistrict);
		for (int b : blocks[myRandomDistrict].adjacentBlocks)
		{
			if (included2.find(b) == included2.end())
				reachable2.insert(b);
		}
		completeRandomDistrict(sampledDistricts, included2, reachable2, nbRemaining - 1);
	}
}

void Params::generateTSPinstance(vector<Point> &points)
{
	ofstream myfile;
	myfile.open(outputName + ".tsp");
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

	myfile.open(outputName + ".par");
	if (myfile.is_open())
	{
		myfile << "PROBLEM_FILE = " << outputName << ".tsp" << endl;
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

void Params::exportDatasetJsonFile()
{
	string typeOfScenario = this->solveOnlyTest? "test" : "train"; 

	if (this->solveEveryDistrict) typeOfScenario = "all_" + typeOfScenario;

	string outputName = "../" + dataName + "_" + depotPosition + "_"
						+ to_string(instanceSize) + "_" + to_string(targetSizeDistricts)
						+ "_tsp." + typeOfScenario + ".json";

	ofstream myfile;
	myfile.open(outputName);

	if (myfile.is_open())
	{
		json jDistricts;

		for(District district : randomDistricts)
		{
			jDistricts += district;
		}

		float executionTime = ((float) endTime - startTime)/CLOCKS_PER_SEC;

		myfile << json{{"districts", jDistricts}, 
		               {"metadata", {{"TIME_TO_GENERATE", executionTime}}}};

		myfile.close();
	}
	else
		throw std::invalid_argument("Impossible to open output file: " + outputName);
}