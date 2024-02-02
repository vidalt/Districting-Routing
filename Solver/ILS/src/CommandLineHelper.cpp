#include "CommandLineHelper.h"
#include <iostream>
#include <string>
using namespace std;

Instance *CommandLineHelper::getInstanceFromCommandLine(int argc, char *argv[])
{
	if (argc < 3 || string(argv[1]) != "-f" )
	{
		CommandLineHelper::printHelper();
		throw new invalid_argument("NO INSTANCE FILE PROVIDED");
	}
	string filePath = string(argv[2]);
	cout << "Instance " << filePath << endl;

	string outputName;
	int seed = 0;
	float probabilitySkip = 0.99;
	double executionTime = 60;
	int port = 8000;
	string predictorMethod= "nn";

	for (int i = 3; i < argc; i += 2)
	{
		if (string(argv[i]) == "-output")
			outputName = std::string(argv[i + 1]);
		else if (string(argv[i]) == "-seed")
			seed = atoi(argv[i + 1]);
		else if (string(argv[i]) == "-p")
			probabilitySkip = atof(argv[i + 1]);
		else if (string(argv[i]) == "-t")
			executionTime = atof(argv[i + 1]);
		else if (string(argv[i]) == "-port")
			port = atoi(argv[i + 1]);
		else if (string(argv[i]) == "--prediction-method")
			predictorMethod = std::string(argv[i + 1]);
		else
		{
			CommandLineHelper::printHelper();
			std::cout << "INVALID ARGUMENT: " << std::string(argv[i]) << std::endl;
		}
	}

	Instance *instance = new Instance(filePath);

	instance->setParameters(probabilitySkip, seed, outputName,
							executionTime,
							port, predictorMethod);

	return instance;
}

void CommandLineHelper::printHelper()
{
	std::cout << std::endl;
	std::cout << "-------------------------------------------- COMMANDLINE -------------------------------------------------------------" << std::endl;
	std::cout << "Usage: ./districting_solver -f instancePath [-output solPath] [-p probabilitySkipMove] [-t timeLImitInSeconds]";
	std::cout << "[-seed randomSeed] [-port portToCallServer]" 											                	  << std::endl;
	
	std::cout << "[-out solPath] sets the output solution path. Defaults to the instance file name prepended with sol-                  " << std::endl;
	std::cout << "[-p probabilitySkipMove] parameter of pertubation, default is 0.99                                                    " << std::endl;
	std::cout << "[-t timeLImitInSeconds] time limit to run ILS in seconds, default is 60                                               " << std::endl;
	
	std::cout << "[-seed mySeed] sets a fixed random seed. Defaults to 0                                                                " << std::endl;
	std::cout << "[-port portToCallServer] if not beardwood expects a cost evaluator in port (default 8000)                             " << std::endl;

	std::cout << "----------------------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;
}