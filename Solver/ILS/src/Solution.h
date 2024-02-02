#ifndef SOLUTION_H
#define SOLUTION_H

#include "Instance.h"

using namespace std;

class Solution{
public: 
    vector<int> blocksDistrict;         // Given a Block returns which district it is in -> returns -1 if block in none district                     
    vector<set<int>> districts;         // Matrix containing the districts - the value in (i,j) represents the id of a basic unit in district i
    vector<double> costOfDistrict;      // Vector containing current cost of a district
    Instance* instance;

    bool isFeasible();                   // Return true if solution attends feasibility criteria
    void exportToGeoJson(string outputName);
    void exportToTxt(string outputName, int nbIterations=0, double executionTime=0);
    
    double cost;
    vector<int> getDistrictsInBorder(int sourceDistrictId, int targetDistrictId); // Returns the districts of sourceDistrictId that have some neighboor in targetDistrictId

    // Constructors
    Solution(Instance* instance);    // Create clean solution
    Solution(Solution* solution);    // Create an object copy of an existing solution    
};

#endif