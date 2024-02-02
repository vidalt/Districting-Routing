#include "Params.h"

using namespace std;

Params::Params(int argc, char *argv[])
{

	dataName = string(argv[1]).c_str();
	cout << "CITY NAME: " << dataName << endl;

	referenceLongLat = {stod(argv[2]), stod(argv[3])};
	cout << "REFERENCE POINT {LONG,LAT} : {" << referenceLongLat[0] << "," << referenceLongLat[1] << "}" << endl;

	sizeTrainingSet = stoi(argv[4]);
	cout << "NB SCENARIOS PER BLOCK: " << sizeTrainingSet << endl;

	seed = stoi(argv[5]);
	generator.seed(seed);
	cout << "SEED: " << seed << endl;

	// Define data paths and read datafile in JSON format
	instanceName = "./CityData/" + dataName;
	readBlocksJSON();

	outputName = dataName;

	probaCustomerDemandPerTargetSizeDistrict = map<int, double> {{3, 0.004}, {6, 0.002}, {12, 0.001}, {20, 0.0006}, {30, 0.0004}};

	cout << "FINISHED READING DATA" << endl;

	// Complete the data structures
	computeBlockDistances();
	cout << "FINISHED COMPUTING DISTANCES" << endl;
}

bool Params::isBlockToRemove(string zone_id)
{
	if (zone_id == "E02001100" || zone_id == "E02003859")
		return true; // Manchester data
	if (zone_id == "E02005372" || zone_id == "E02002929")
		return true; // West Midlands data
	if (zone_id == "E02006649" || zone_id == "E02006657")
		return true; // Bristol data
	if (zone_id == "E02006678" || zone_id == "E02006658")
		return true; // Bristol data
	if (zone_id == "E02006680" || zone_id == "E02006049")
		return true; // Bristol data
	if (zone_id == "E02006050" || zone_id == "E02006053")
		return true; // Bristol data
	return false;
}

void Params::readBlocksJSON()
{
	// Parsing the population data
	map<string, int> zoneIDPopulation;
	ifstream inputFilePop("./CityData/Census-Data.json");
	if (inputFilePop.is_open())
	{
		json j;
		inputFilePop >> j;
		inputFilePop.close();
		for (std::pair<string, int> myPair : j)
			zoneIDPopulation.insert(myPair);
	}
	else
		throw std::invalid_argument("Impossible to open population file: CityData/Census-Data.json");

	// Parsing all blocks in the JSON file
	ifstream inputFile(instanceName + ".geojson");
	if (inputFile.is_open())
	{
		json j;
		inputFile >> j;
		inputFile.close();
		int blockID = 0;
		for (auto &blockIN : j.at("features"))
		{
			if (!isBlockToRemove(blockIN.at("properties").at("zone_id"))) // Some blocks have been flagged for removal after data inspection to avoid connectivity issues
			{
				blocks.push_back(Block());
				blocks[blockID].zone_id = blockIN.at("properties").at("zone_id");
				blocks[blockID].zone_name = blockIN.at("properties").at("DISPLAY_NAME");

				auto it = zoneIDPopulation.find(blocks[blockID].zone_id);
				if (it != zoneIDPopulation.end())
					blocks[blockID].nbInhabitants = it->second;
				else
				{
					cout << "Impossible to find population data for this block, setting 8000 inhabitants as default" << endl;
					blocks[blockID].nbInhabitants = 8000;
				}

				if (blockIN.at("geometry").at("coordinates").size() > 1)
				{
					cout << "Warning: " << blocks[blockID].zone_name << " " << blocks[blockID].zone_id << " contains " << blockIN.at("geometry").at("coordinates").size() << " polygons " << endl;
					cout << "The first polygon only will be retained" << endl;
				}

				auto &polyIN = blockIN.at("geometry").at("coordinates")[0];
				if (blockIN.at("geometry").at("type") == "MultiPolygon") // Some special cases in the data format
				{
					if (blockIN.at("geometry").at("coordinates")[0].size() > 1)
					{
						cout << "Warning: " << blocks[blockID].zone_name << " " << blocks[blockID].zone_id << " contains " << blockIN.at("geometry").at("coordinates")[0].size() << " polygons " << endl;
						cout << "The first polygon only will be retained" << endl;
					}
					polyIN = blockIN.at("geometry").at("coordinates")[0][0];
				}

				for (array<double, 2> longLatInfo : polyIN)
				{
					//longLatInfo[0] Represents the longitude -- should be close to 0 for UK data
					//longLatInfo[1] Represents the latitude  -- should be around 52-54 for UK data
					std::array<double, 2> cartPosition = wgs84::toCartesian({referenceLongLat[1], referenceLongLat[0]}, {longLatInfo[1], longLatInfo[0]}); // receives input as {latitude,longitude}
					blocks[blockID].verticesLongLat.push_back(longLatInfo);
					Point myPoint = {(double)(int)cartPosition[0] / 1000., (double)(int)cartPosition[1] / 1000.}; // conversion into km with a precision of 1m at most
					blocks[blockID].verticesPoints.push_back(myPoint);
					blocks[blockID].minX = min<double>(myPoint.x, blocks[blockID].minX);
					blocks[blockID].maxX = max<double>(myPoint.x, blocks[blockID].maxX);
					blocks[blockID].minY = min<double>(myPoint.y, blocks[blockID].minY);
					blocks[blockID].maxY = max<double>(myPoint.y, blocks[blockID].maxY);
				}

				// Distance from the reference point (0,0)
				blocks[blockID].distReferencePoint = blocks[blockID].distance({0, 0});
				blockID++;
			}
		}

		// Order the blocks by increasing distance from the reference point
		std::sort(blocks.begin(), blocks.end());

		for (int blockID = 0; blockID < blocks.size(); blockID++)
		{
			// Recording block ID's
			blocks[blockID].id = blockID;

			// Perimeter calculation
			for (int i = 0; i < blocks[blockID].verticesPoints.size(); i++)
			{
				blocks[blockID].perimeter += Point::distance(blocks[blockID].verticesPoints[i], blocks[blockID].verticesPoints[(i + 1) % blocks[blockID].verticesPoints.size()]);
				allPointsLongLat.insert(blocks[blockID].verticesLongLat[i]);
			}

			// Area calculation, we are going to use a sampling approach in which we calculate the area of the enclosing rectangle
			// And then randomly generate points within this rectangle to know the fraction which falls inside the polygon
			blocks[blockID].recArea = (blocks[blockID].maxY - blocks[blockID].minY) * (blocks[blockID].maxX - blocks[blockID].minX);
			uniform_real_distribution<> distX(blocks[blockID].minX, blocks[blockID].maxX);
			uniform_real_distribution<> distY(blocks[blockID].minY, blocks[blockID].maxY);

			int success = 0;
			int nbSampling = 50000;

			for (int k = 0; k < nbSampling; k++)
			{
				Point randomPoint = {distX(generator), distY(generator)};
				if (isInside(blocks[blockID].verticesPoints, randomPoint))
					success++;
			}

			blocks[blockID].area = blocks[blockID].recArea * (double)success / (double)nbSampling;
			blocks[blockID].density = (double)blocks[blockID].nbInhabitants / blocks[blockID].area;
			cout << "BlockID: " << blockID << " | Inhabitants: " << blocks[blockID].nbInhabitants << " | Perimeter: " << blocks[blockID].perimeter << " km | Area: " << blocks[blockID].area << " km2" << endl;

			// Calculating additional meta information
			minX = min<double>(blocks[blockID].minX, minX);
			maxX = max<double>(blocks[blockID].maxX, maxX);
			
			minY = min<double>(blocks[blockID].minY, minY);
			maxY = max<double>(blocks[blockID].maxY, maxY);

			totalInhabitants += blocks[blockID].nbInhabitants;
			
			minInhabitants = min<int>(minInhabitants, blocks[blockID].nbInhabitants);
			maxInhabitants = max<int>(maxInhabitants, blocks[blockID].nbInhabitants);
			
			totalArea += blocks[blockID].area;
			
			minArea = min<double>(minArea, blocks[blockID].area);
			maxArea = max<double>(maxArea, blocks[blockID].area);
		}
	}
	else
		throw std::invalid_argument("Impossible to open instance JSON file: " + instanceName + ".geojson");
}

void Params::computeBlockDistances()
{
	// Calculating distances
	for (int b1 = 0; b1 < blocks.size(); b1++)
	{
		cout << "Distance Calculations: Block " << b1 << endl;
		for (int b2 = 0; b2 < blocks.size(); b2++)
		{
			int nbPointsTouching = 0;
			for (Point &p1 : blocks[b1].verticesPoints)
			{
				double dist = blocks[b2].distance(p1);
				
				if (dist < 0.05)
					nbPointsTouching++; // Allowing an approximation of 50 meters when checking connectivity
			}
			if (nbPointsTouching >= 2 && b1 != b2) // b1 and b2 touch in at least two points, and therefore are considered to be adjacent.
			{
				blocks[b1].adjacentBlocks.insert(b2);
				blocks[b2].adjacentBlocks.insert(b1);
			}
		}
	}
}


void Params::exportJSONFile()
{
	ofstream myfile;
	myfile.open(outputName + ".geojson");
	if (myfile.is_open())
	{
		json jmeta = json{
			{"CITY", dataName},
			{"REFERENCE_LONGLAT", referenceLongLat},
			{"REFERENCE_XY", referencePoint},
			{"SEED", seed},
			{"NB_BLOCKS", blocks.size()},
			{"NB_POINTS", allPointsLongLat.size()},
			{"NB_INHABITANTS", totalInhabitants},
			{"AVG_INHABITANTS", (double)totalInhabitants / (double)blocks.size()},
			{"MIN_INHABITANTS", minInhabitants},
			{"MAX_INHABITANTS", maxInhabitants},
			{"TOT_AREA", totalArea},
			{"AVG_AREA", totalArea / (double)blocks.size()},
			{"MIN_AREA", minArea},
			{"MAX_AREA", maxArea},
			{"MIN_X", minX},
			{"MAX_X", maxX},
			{"MIN_Y", minY},
			{"MAX_Y", maxY},
			{"GENERATION_TIME_SECONDS", (double)(endTime - startTime) / (double)CLOCKS_PER_SEC}
			};

		json jblocks = json(blocks);

		myfile << json{{"type", "FeatureCollection"}, {"features", jblocks},{"metadata", jmeta}};
		myfile.close();
	}
	else
		throw std::invalid_argument("Impossible to open output file: " + outputName + ".geojson");
}

void Params::exportBlockScenarios()
{
	vector<int> sizesOfInstance = vector<int> { 60, 90, 120, 150, 180, 240 };

	for (int instanceSize : sizesOfInstance)
	{
		vector<Block> currentBlocks = vector<Block> (blocks);

		if (instanceSize > currentBlocks.size()) return;

		currentBlocks.resize(instanceSize);

		maxX = -INFINITY;
		maxY = -INFINITY;

		minX = INFINITY;
		minY = INFINITY;

		for (Block block : currentBlocks)
		{
			maxX = max(maxX, block.maxX);
			maxY = max(maxY, block.maxY);

			minX = min(minX, block.minX);
			minY = min(minY, block.minY);
		}

		vector<string> depotPositions = vector<string> ({"C", "NE", "NW", "SE", "SW"});

		for(string depotPosition : depotPositions)
		{
			Point depotPoint;

			if (depotPosition == "C")  depotPoint = {(minX + maxX) / 2.0, (minY + maxY) / 2.0 };
			if (depotPosition == "NW") depotPoint = { minX, maxY };
			if (depotPosition == "NE") depotPoint = { maxX, maxY };
			if (depotPosition == "SW") depotPoint = { minX, minY };
			if (depotPosition == "SE") depotPoint = { maxX, minY };

			for (int nbBlock=0 ; nbBlock < currentBlocks.size(); nbBlock++)
			{
				Block* block = &currentBlocks[nbBlock];
				block->distDepot = block->distance(depotPoint);
			}

			auto temp = wgs84::fromCartesian({referenceLongLat[1], referenceLongLat[0]}, {1000. * depotPoint.x, 1000. * depotPoint.y});
			array<double,2> depotLongLat = array<double,2> ({temp[1], temp[0]});

			for (pair<int, double> probaCustomer : probaCustomerDemandPerTargetSizeDistrict)
			{
				int targetSizeOfDistrict = probaCustomer.first;
				
				int maxSizeOfDistrict = ceil(1.2 * targetSizeOfDistrict);
				int minSizeOfDistrict = floor(0.8 * targetSizeOfDistrict);

				int numberOfDistricts = floor(instanceSize / targetSizeOfDistrict);

				string instanceName = dataName + "_" + depotPosition + "_" + to_string(instanceSize) + "_" + to_string(targetSizeOfDistrict);

				vector<string> fileTypes = vector<string>({"train", "test"});

				for(string fileType : fileTypes)
				{
					string outputName = "./output/" + instanceName + "." + fileType + ".json";

					ofstream myfile;
					myfile.open(outputName);

					bool isTrainFile = fileType == "train";

					if (myfile.is_open())
					{
						json jblocks;
						for (Block block : currentBlocks)
						{
							json jBlock = {
								{"ID", block.id},
								{"SCENARIOS", isTrainFile? block.trainScenarios[targetSizeOfDistrict] : 
														   block.testScenarios[targetSizeOfDistrict]},
								{"DEPOT_DIST", block.distDepot},
							};
							jblocks += jBlock;
						}	

						myfile << json{{"blocks", jblocks},
									   {"metadata", {
										   	{"TARGET_SIZE_DISTRICT", targetSizeOfDistrict}, 
									   		{"MAX_SIZE_DISTRICT", maxSizeOfDistrict},
											{"MIN_SIZE_DISTRICT", minSizeOfDistrict},
											{"NUMBER_OF_DISTRICTS", numberOfDistricts},
											{"DEPOT_XY", depotPoint},
											{"DEPOT_LONGLAT", depotLongLat},
											{"PROBA_CUSTOMER_DEMAND", probaCustomer.second},
									   }}};
						myfile.close();
					}
					else
						throw std::invalid_argument("Impossible to open output file: " + outputName);
				}

			}
		}
	}

}