#include "Params.h"

vector<string> split(const string &s, const char &c)
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

Params::Params(int argc, char *argv[])
{
	dataName = string(argv[1]).c_str();
	cout << "CITY NAME: " << dataName << endl;

	targetSizeDistricts = stoi(argv[2]);

	minSizeDistricts = floor(0.8 * targetSizeDistricts);
	maxSizeDistricts = ceil(1.2 * targetSizeDistricts);

	cout << "[MIN,TARGET,MAX] SIZE DISTRICTS: "
		 << "[" << minSizeDistricts << "," << targetSizeDistricts << "," << maxSizeDistricts << "]" << endl;


	depotPosition = string(argv[3]).c_str();
	cout << "DEPOT POSITION: " << depotPosition << endl;

	instanceSize = stoi(argv[4]);
	cout << "SIZE OF PROBLEM: " << instanceSize << endl;

	seed = stoi(argv[5]);
	generator.seed(seed);
	cout << "SEED: " << seed << endl;

	nbScenarios = 500;

	solutionFile = string(argv[6]);
	cout << "SOLUTON FILE: " << solutionFile << endl;

	// Define data paths and read datafile in JSON format
	instanceName = dataName;
	
	outputName = split(solutionFile, '/')[7];

	readBlocksJSON();
	readScenarionJSON();

	randomDistricts = getDistrictsFromSolution();
}

void Params::readScenarionJSON()
{
	string scenarioFileName = dataName + "_" + depotPosition + "_" 
	                        + to_string(instanceSize) + "_" 
							+ to_string(targetSizeDistricts) + ".train.json";
	ifstream myfile;
	string fileName = "../../Data/Dataset/scenarios/" + scenarioFileName;
	myfile.open(fileName);

	for (int iter=0; iter < 15000 && myfile.is_open(); iter++)
	{
		if ( iter%100 == 0 ) cout << "RETRY OPEN FILE " << fileName << endl;
		myfile.open(fileName);
	}

	if (myfile.is_open())
	{	
		json fileContent;
		
		myfile >> fileContent;
		myfile.close();
		
		for (auto &block : fileContent.at("blocks"))
		{
			int blockId = block.at("ID");
			
			blocks[blockId].distDepot = block.at("DEPOT_DIST");
			blocks[blockId].testScenarios = vector<vector<Point>>();

			for (auto &scenario : block.at("SCENARIOS"))
			{				
				vector<Point> scenarioPoints = vector<Point>();
				for (auto &point : scenario)
				{
					Point randomPoint = { point[0],  point[1]};
					scenarioPoints.push_back(randomPoint);
				}

				blocks[blockId].testScenarios.push_back(scenarioPoints);
			}
			
		}			
		
		vector<double> depot = fileContent.at("metadata").at("DEPOT_XY");
		depotPoint = {depot[0], depot[1]};
	}
	else
		throw std::invalid_argument("Impossible to open output file: " + scenarioFileName);
}


vector<District> Params::getDistrictsFromSolution()
{
	ifstream inputFile(solutionFile.c_str());
	string text;

	vector<District> districts = vector<District>();

	if (inputFile.is_open())
	{
		getline(inputFile, text);
		getline(inputFile, text);
		getline(inputFile, text);
		getline(inputFile, text);
		getline(inputFile, text);
		
		while (getline(inputFile, text))
		{
			vector<string> blocksInDistrict = split(text, '\t');
			set<int> blocks = set<int>();

			for (string block : blocksInDistrict)
				blocks.insert(stoi(block));

			District district = District(blocks);
			districts.push_back(district);
		}
	}
	else
	{
		cout << "CANNOT OPEN FILE " << solutionFile << endl;
		throw invalid_argument("CANNOT OPEN FILE " + solutionFile);
	}

	return districts;
}

void Params::readBlocksJSON()
{
	string fileName = "../../Data/Dataset/" + instanceName + ".geojson";

	ifstream myfile;
	myfile.open(fileName);

	for (int iter=0; iter < 15000 && myfile.is_open(); iter++)
	{
		if ( iter%1000 == 0 ) cout << "RETRY OPEN FILE " << fileName << endl;
		myfile.open(fileName);
	}


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
		myfile << "ASCENT_CANDIDATES = 5" << endl;
		myfile.close();
	}
	else
		throw std::invalid_argument("Impossible to create PAR file");
}
