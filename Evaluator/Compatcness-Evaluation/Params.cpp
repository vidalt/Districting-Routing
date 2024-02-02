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
	solutionFile = string(argv[1]);
	cout << "SOLUTON FILE: " << solutionFile << endl;

	outputName = split(solutionFile, '/')[7];
	instanceName = split(outputName, '_')[0];

	readBlocksJSON();
	districts = getDistrictsFromSolution();
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

	for (int iter=0; iter < 150000 && myfile.is_open(); iter++)
	{
		// if ( iter%1000 == 0 ) cout << "RETRY OPEN FILE " << fileName << endl;
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
			block.area = currentBlock.at("properties").at("AREA");

			vector<Point> blockVertices = vector<Point>();
			
			for (vector<double> vertices : currentBlock.at("properties").at("POINTS"))
			{
				Point vertice = {vertices[0], vertices[1]};
				blockVertices.push_back(vertice);
			}
			
			block.verticesPoints = blockVertices;

			this->blocks.push_back(block);
		}
	}

	else
		throw std::invalid_argument("Impossible to open output file: " + fileName);
}