#include "Params.h"
#include "Compactness.h"

int main(int argc, char *argv[])
{
	/* READ INSTANCE FILE AND INITIALIZE DATA STRUCTURES */
	Params params(argc, argv);

	for (int d = 0; d < params.districts.size(); d++)
	{
		District *myDistrict = &params.districts[d];
		myDistrict->compactness = getCompactnessMeasure(myDistrict, params);
	}

	ofstream outputFile;
	
	outputFile.open(params.solutionFile + ".compact");

	if (outputFile.is_open())
	{
		
		for (int d = 0; d < params.districts.size(); d++)
		{
			District *myDistrict = &params.districts[d];
			outputFile << myDistrict->compactness << endl;
		}

		outputFile.close();
	}

	return 0;
}
