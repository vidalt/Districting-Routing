#include "Instance.h"
#include <fstream>
#include <iostream>
#include <bits/stdc++.h>
#include "../lib/nlohmann/json.hpp"
#include "utils.h"
using json = nlohmann::json;

using namespace std;

Instance::Instance(string instanceName)
{
	vector<string> instanceData = utils::splitString(instanceName, '_');

	this->cityName = instanceData[0];
	int instanceSize = stoi(instanceData[2]);

	this->instanceName = instanceName;

	string geojsonFileName = "../../Data/Dataset/" + cityName + ".geojson";
	ifstream geojsonFile = utils::openCommonFile(geojsonFileName.c_str());

	if (geojsonFile.is_open())
	{
		json j;
		geojsonFile >> j;
		geojsonFile.close();

		for (auto &blockIN : j.at("features"))
		{
			auto featuresOfBlock = blockIN.at("properties");

			if (featuresOfBlock.contains("ID"))
			{
				if (featuresOfBlock.at("ID") >= instanceSize) continue;

				Block block = Block();
				block.id = featuresOfBlock.at("ID");
				block.area = featuresOfBlock.at("AREA");
				block.density = featuresOfBlock.at("DENSITY");
				block.nbInhabitants = featuresOfBlock.at("POPULATION");
				block.zoneName = featuresOfBlock.at("NAME");
				
				block.adjacentBlocks = vector<int>();

				for (int blockRef : featuresOfBlock.at("LIST_ADJACENT"))
				{
					if (blockRef >= instanceSize) break;
					block.adjacentBlocks.push_back(blockRef);
				}

				this->blocks.push_back(block);
			}
		}
	}
	else
	{
		string message = "Cannot open instance file: " + geojsonFileName;
		cout << message << endl;
		throw invalid_argument(message);
	}

	string scenarioFileName = "../../Data/Dataset/scenarios/" + instanceName + ".train.json";
	ifstream scenarioFile = utils::openCommonFile(scenarioFileName.c_str());

	if (scenarioFile.is_open())
	{
		json j;
		scenarioFile >> j;
		scenarioFile.close();

		probaCustomerDemand = j.at("metadata").at("PROBA_CUSTOMER_DEMAND");
		minSizeDistricts    = j.at("metadata").at("MIN_SIZE_DISTRICT");
		targetSizeDistricts = j.at("metadata").at("TARGET_SIZE_DISTRICT");
		maxSizeDistricts    = j.at("metadata").at("MAX_SIZE_DISTRICT");
		numDistricts        = j.at("metadata").at("NUMBER_OF_DISTRICTS");


		for (auto &blockIN : j.at("blocks"))
		{
			if (blockIN.contains("ID"))
			{
				int blockId = blockIN.at("ID");
				Block* block = & blocks[blockId];

				block->depotDistance = blockIN.at("DEPOT_DIST");
				block->estimatedCostumers = block->nbInhabitants * probaCustomerDemand;
			}
		}
	}
	else
	{
		string message = "Cannot open instance file: " + scenarioFileName;
		cout << message << endl;
		throw invalid_argument(message);
	}
}

void Instance::setParameters(double probabilitySkipMovePertubation,
							 int seed, string solutionFileName,
							 double timeLimitIteration,
							 int port, string predictorMethod)
{
	if (!solutionFileName.empty())
		this->solutionFileName = solutionFileName;
	else
		this->solutionFileName = this->instanceName;

	this->seed = seed;
	this->generator.seed(seed);
	this->port = port;

	std::uniform_real_distribution<double> distribution(0, 1);
	this->randomNumber = std::bind(distribution, generator);

	this->probabilitySkipMovePertubation = probabilitySkipMovePertubation;
	this->timeLimitIteration = timeLimitIteration;

	this->predictorMethod = predictorMethod;
}

bool Instance::isDistrictContigous(set<int> *basicUnitOfADistrict)
{
	int initialUnit = (int)*basicUnitOfADistrict->begin();
	unordered_set<int> unitsToVisit = unordered_set<int>(basicUnitOfADistrict->begin(), basicUnitOfADistrict->end());
	unordered_set<int> unitsOfDistrict = unordered_set<int>(basicUnitOfADistrict->begin(), basicUnitOfADistrict->end());

	queue<int> nextUnitToVisit = queue<int>();
	unitsToVisit.erase(initialUnit);
	nextUnitToVisit.push(initialUnit);

	while (!unitsToVisit.empty())
	{
		if (nextUnitToVisit.empty())
			return false;

		int currentUnit = nextUnitToVisit.front();
		nextUnitToVisit.pop();

		for (int neighbor : this->blocks[currentUnit].adjacentBlocks)
		{
			bool isNeighborAtSameDistrict = unitsOfDistrict.find(neighbor) != unitsOfDistrict.end();
			if (!isNeighborAtSameDistrict)
				continue;

			bool neighborAlreadyVisited = unitsToVisit.find(neighbor) == unitsToVisit.end();

			if (!neighborAlreadyVisited)
			{
				unitsToVisit.erase(neighbor);
				nextUnitToVisit.push(neighbor);
			}
		}
	}
	return true;
}
